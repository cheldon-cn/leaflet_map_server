#include "ogc/db/sqlite_connection.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/envelope.h"
#include <sstream>
#include <cstring>
#include <cstdint>

namespace ogc {
namespace db {

std::string SpatiaLiteOptions::ToConnectionString() const {
    std::ostringstream oss;
    oss << databasePath;
    
    if (readOnly) {
        oss << "?mode=ro";
    }
    if (createIfNotExist) {
        oss << "?mode=rwc";
    }
    
    return oss.str();
}

struct SpatiaLiteConnection::Impl {
    sqlite3* db;
    bool isTransaction;
    std::string lastError;
    bool spatialInitialized;
    ConnectionInfo connectionInfo;
    TransactionIsolation isolationLevel;
    int64_t rowsAffected;
    
    Impl() : db(nullptr), isTransaction(false), spatialInitialized(false), isolationLevel(TransactionIsolation::kReadCommitted), rowsAffected(0) {}
};

SpatiaLiteConnectionPtr SpatiaLiteConnection::Create() {
    return std::make_unique<SpatiaLiteConnection>();
}

SpatiaLiteConnection::SpatiaLiteConnection()
    : impl_(new Impl()) {
}

SpatiaLiteConnection::~SpatiaLiteConnection() {
    Disconnect();
}

Result SpatiaLiteConnection::Connect(const std::string& connectionString) {
    Disconnect();
    
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    int rc = sqlite3_open_v2(connectionString.c_str(), &impl_->db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        impl_->lastError = sqlite3_errmsg(impl_->db);
        sqlite3_close(impl_->db);
        impl_->db = nullptr;
        return Result::Error(DbResult::kConnectionFailed, impl_->lastError);
    }
    
    sqlite3_busy_timeout(impl_->db, 5000);
    
    return Result::Success();
}

Result SpatiaLiteConnection::Connect(const SpatiaLiteOptions& options) {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    
    if (options.readOnly) {
        flags = SQLITE_OPEN_READONLY;
    }
    
    int rc = sqlite3_open_v2(options.databasePath.c_str(), &impl_->db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        impl_->lastError = sqlite3_errmsg(impl_->db);
        sqlite3_close(impl_->db);
        impl_->db = nullptr;
        return Result::Error(DbResult::kConnectionFailed, impl_->lastError);
    }
    
    std::string cachePragma = "PRAGMA cache_size = " + options.cacheSize;
    sqlite3_exec(impl_->db, cachePragma.c_str(), nullptr, nullptr, nullptr);
    
    std::string timeoutPragma = "PRAGMA busy_timeout = " + std::to_string(options.connectionTimeout);
    sqlite3_exec(impl_->db, timeoutPragma.c_str(), nullptr, nullptr, nullptr);
    
    return Result::Success();
}

void SpatiaLiteConnection::Disconnect() {
    if (impl_->db) {
        if (impl_->isTransaction) {
            sqlite3_exec(impl_->db, "ROLLBACK", nullptr, nullptr, nullptr);
        }
        sqlite3_close(impl_->db);
        impl_->db = nullptr;
    }
}

bool SpatiaLiteConnection::IsConnected() const {
    return impl_->db != nullptr;
}

bool SpatiaLiteConnection::Ping() const {
    if (!impl_->db) return false;
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(impl_->db, "SELECT 1", -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_ROW;
}

Result SpatiaLiteConnection::BeginTransaction() {
    if (impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "Transaction already in progress");
    }
    
    Result result = Execute("BEGIN TRANSACTION");
    
    if (result.IsSuccess()) {
        impl_->isTransaction = true;
    }
    
    return result;
}

Result SpatiaLiteConnection::Commit() {
    if (!impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    Result result = Execute("COMMIT");
    
    if (result.IsSuccess()) {
        impl_->isTransaction = false;
    }
    
    return result;
}

Result SpatiaLiteConnection::Rollback() {
    if (!impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    Result result = Execute("ROLLBACK");
    
    if (result.IsSuccess()) {
        impl_->isTransaction = false;
    }
    
    return result;
}

Result SpatiaLiteConnection::Execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(impl_->db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        impl_->lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return Result::Error(DbResult::kExecutionError, impl_->lastError);
    }
    
    return Result::Success();
}

Result SpatiaLiteConnection::ExecuteQuery(const std::string& sql, DbResultSetPtr& result) {
    sqlite3_stmt* stmt = nullptr;
    Result dbResult = ExecuteInternal(sql, stmt, false);
    
    if (!dbResult.IsSuccess()) {
        if (stmt) sqlite3_finalize(stmt);
        return dbResult;
    }
    
    result = std::make_unique<SpatiaLiteResultSet>(stmt);
    return Result::Success();
}

Result SpatiaLiteConnection::PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) {
    stmt = std::make_unique<SpatiaLiteStatement>(impl_->db, sql);
    stmt->Reset();
    
    return Result::Success();
}

Result SpatiaLiteConnection::InsertGeometry(const std::string& table,
                                             const std::string& geomColumn,
                                             const Geometry* geometry,
                                             const std::map<std::string, std::string>& attributes,
                                             int64_t& outId) {
    if (!IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = false;
    
    Result wkbResult = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    std::string hexWkb;
    wkbResult = WkbConverter::GeometryToHexWkb(geometry, hexWkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    int srid = geometry ? geometry->GetSRID() : 0;
    
    std::ostringstream sql;
    sql << "INSERT INTO " << table << " (";
    
    bool first = true;
    sql << geomColumn;
    for (const auto& attr : attributes) {
        sql << ", " << attr.first;
    }
    sql << ") VALUES (";
    
    first = true;
    if (srid > 0) {
        sql << "ST_GeomFromWKB(X'" << hexWkb << "', " << srid << ")";
    } else {
        sql << "ST_GeomFromWKB(X'" << hexWkb << "')";
    }
    for (const auto& attr : attributes) {
        sql << ", '" << attr.second << "'";
    }
    sql << ")";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(impl_->db, sql.str().c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        impl_->lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return Result::Error(DbResult::kExecutionError, impl_->lastError);
    }
    
    outId = GetLastInsertRowId();
    return Result::Success();
}

Result SpatiaLiteConnection::InitializeSpatialMetaData() {
    if (!IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    Result result = Execute("SELECT InitSpatialMetadata(1)");
    
    if (result.IsSuccess()) {
        impl_->spatialInitialized = true;
    }
    
    return result;
}

Result SpatiaLiteConnection::CreateSpatialIndex(const std::string& table, const std::string& geomColumn) {
    if (!IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::ostringstream sql;
    sql << "SELECT CreateSpatialIndex('" << table << "', '" << geomColumn << "')";
    
    return Execute(sql.str());
}

std::string SpatiaLiteConnection::GetLastError() const {
    return impl_->lastError;
}

Result SpatiaLiteConnection::CheckConnection() {
    if (!impl_->db) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    return Result::Success();
}

Result SpatiaLiteConnection::ExecuteInternal(const std::string& sql, sqlite3_stmt*& stmt, bool finalize) {
    Result connResult = CheckConnection();
    if (!connResult.IsSuccess()) {
        return connResult;
    }
    
    int rc = sqlite3_prepare_v2(impl_->db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        impl_->lastError = sqlite3_errmsg(impl_->db);
        return Result::Error(DbResult::kPrepareError, impl_->lastError);
    }
    
    rc = sqlite3_step(stmt);
    
    if (finalize && stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) {
        return Result::Success();
    }
    
    impl_->lastError = sqlite3_errmsg(impl_->db);
    return Result::Error(DbResult::kExecutionError, impl_->lastError);
}

int SpatiaLiteConnection::GetLastInsertRowId() const {
    return static_cast<int>(sqlite3_last_insert_rowid(impl_->db));
}

struct SpatiaLiteStatement::Impl {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql;
    std::vector<std::string> bindStrings;
    
    Impl(sqlite3* d, const std::string& s) : db(d), stmt(nullptr), sql(s) {}
};

SpatiaLiteStatement::SpatiaLiteStatement(sqlite3* db, const std::string& sql)
    : impl_(new Impl(db, sql)) {
}

SpatiaLiteStatement::~SpatiaLiteStatement() {
    Finalize();
}

void SpatiaLiteStatement::SetConnection(sqlite3* db) { impl_->db = db; }
const std::string& SpatiaLiteStatement::GetName() const { return impl_->sql; }
const std::string& SpatiaLiteStatement::GetSql() const { return impl_->sql; }

Result SpatiaLiteStatement::BindInt(int paramIndex, int32_t value) {
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_int(impl_->stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind int");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindInt64(int paramIndex, int64_t value) {
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_int64(impl_->stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind int64");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindDouble(int paramIndex, double value) {
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_double(impl_->stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind double");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindString(int paramIndex, const std::string& value) {
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    impl_->bindStrings.push_back(value);
    int rc = sqlite3_bind_text(impl_->stmt, paramIndex + 1, value.c_str(), -1, SQLITE_TRANSIENT);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind string");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindGeometry(int paramIndex, const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result wkbResult = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    int rc = sqlite3_bind_blob(impl_->stmt, paramIndex + 1, wkb.data(), static_cast<int>(wkb.size()), SQLITE_TRANSIENT);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind geometry");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::Execute() {
    if (!impl_->stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_step(impl_->stmt);
    
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) {
        sqlite3_reset(impl_->stmt);
        return Result::Success();
    }
    
    std::string error = sqlite3_errmsg(impl_->db);
    return Result::Error(DbResult::kExecutionError, error);
}

Result SpatiaLiteStatement::ExecuteQuery(DbResultSetPtr& result) {
    if (!impl_->stmt) {
        Result prepResult = Prepare();
        if (!prepResult.IsSuccess()) {
            return prepResult;
        }
    }
    
    result = std::make_unique<SpatiaLiteResultSet>(impl_->stmt);
    impl_->stmt = nullptr;
    
    return Result::Success();
}

Result SpatiaLiteStatement::Reset() {
    if (impl_->stmt) {
        sqlite3_reset(impl_->stmt);
        sqlite3_clear_bindings(impl_->stmt);
    }
    impl_->bindStrings.clear();
    return Result::Success();
}

Result SpatiaLiteStatement::ClearBindings() {
    if (impl_->stmt) {
        sqlite3_clear_bindings(impl_->stmt);
    }
    impl_->bindStrings.clear();
    return Result::Success();
}

int64_t SpatiaLiteStatement::GetLastInsertId() const {
    if (impl_->db) {
        return sqlite3_last_insert_rowid(impl_->db);
    }
    return 0;
}

int64_t SpatiaLiteStatement::GetRowsAffected() const {
    if (impl_->db) {
        return sqlite3_changes(impl_->db);
    }
    return 0;
}

int SpatiaLiteStatement::GetParameterCount() const {
    if (impl_->stmt) {
        return sqlite3_bind_parameter_count(impl_->stmt);
    }
    return 0;
}

int SpatiaLiteStatement::GetParameterIndex(const std::string& paramName) const {
    if (impl_->stmt) {
        return sqlite3_bind_parameter_index(impl_->stmt, paramName.c_str());
    }
    return 0;
}

Result SpatiaLiteStatement::BindBool(int paramIndex, bool value) {
    return BindInt(paramIndex, value ? 1 : 0);
}

Result SpatiaLiteStatement::BindNull(int paramIndex) {
    if (!impl_->stmt) {
        return Result::Error(DbResult::kNotConnected, "Statement not prepared");
    }
    
    int rc = sqlite3_bind_null(impl_->stmt, paramIndex + 1);
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, sqlite3_errmsg(impl_->db));
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindBlob(int paramIndex, const std::vector<uint8_t>& data) {
    return BindBlob(paramIndex, data.data(), data.size());
}

Result SpatiaLiteStatement::BindBlob(int paramIndex, const uint8_t* data, size_t size) {
    if (!impl_->stmt) {
        return Result::Error(DbResult::kNotConnected, "Statement not prepared");
    }
    
    int rc = sqlite3_bind_blob(impl_->stmt, paramIndex + 1, data, static_cast<int>(size), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, sqlite3_errmsg(impl_->db));
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindEnvelope(int paramIndex, const ::ogc::Envelope& envelope) {
    std::ostringstream oss;
    oss << "POLYGON(("
        << envelope.GetMinX() << " " << envelope.GetMinY() << ", "
        << envelope.GetMaxX() << " " << envelope.GetMinY() << ", "
        << envelope.GetMaxX() << " " << envelope.GetMaxY() << ", "
        << envelope.GetMinX() << " " << envelope.GetMaxY() << ", "
        << envelope.GetMinX() << " " << envelope.GetMinY() << "))";
    return BindString(paramIndex, oss.str());
}

Result SpatiaLiteStatement::BindInt(const std::string& paramName, int32_t value) {
    return BindInt(GetParameterIndex(paramName), value);
}

Result SpatiaLiteStatement::BindInt64(const std::string& paramName, int64_t value) {
    return BindInt64(GetParameterIndex(paramName), value);
}

Result SpatiaLiteStatement::BindDouble(const std::string& paramName, double value) {
    return BindDouble(GetParameterIndex(paramName), value);
}

Result SpatiaLiteStatement::BindString(const std::string& paramName, const std::string& value) {
    return BindString(GetParameterIndex(paramName), value);
}

Result SpatiaLiteStatement::BindBool(const std::string& paramName, bool value) {
    return BindBool(GetParameterIndex(paramName), value);
}

Result SpatiaLiteStatement::BindNull(const std::string& paramName) {
    return BindNull(GetParameterIndex(paramName));
}

Result SpatiaLiteStatement::BindBlob(const std::string& paramName, const std::vector<uint8_t>& data) {
    return BindBlob(GetParameterIndex(paramName), data);
}

Result SpatiaLiteStatement::BindGeometry(const std::string& paramName, const Geometry* geometry) {
    return BindGeometry(GetParameterIndex(paramName), geometry);
}

Result SpatiaLiteStatement::Prepare() {
    if (!impl_->db || impl_->sql.empty()) {
        return Result::Error(DbResult::kPrepareError, "Invalid database or SQL");
    }
    
    if (impl_->stmt) {
        sqlite3_finalize(impl_->stmt);
    }
    
    int rc = sqlite3_prepare_v2(impl_->db, impl_->sql.c_str(), -1, &impl_->stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(impl_->db);
        return Result::Error(DbResult::kPrepareError, error);
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::Finalize() {
    if (impl_->stmt) {
        sqlite3_finalize(impl_->stmt);
        impl_->stmt = nullptr;
    }
    return Result::Success();
}

struct SpatiaLiteResultSet::Impl {
    sqlite3_stmt* stmt;
    int columnCount;
    int64_t currentRow;
    Result lastError;
    std::vector<std::string> columnNames;
    std::vector<int> columnTypes;
    
    Impl(sqlite3_stmt* s) : stmt(s), columnCount(0), currentRow(0) {
        if (stmt) {
            columnCount = sqlite3_column_count(stmt);
        }
    }
};

SpatiaLiteResultSet::SpatiaLiteResultSet(sqlite3_stmt* stmt)
    : impl_(new Impl(stmt)) {
    if (impl_->stmt) {
        InitializeColumns();
    }
}

SpatiaLiteResultSet::~SpatiaLiteResultSet() {
    if (impl_->stmt) {
        sqlite3_finalize(impl_->stmt);
        impl_->stmt = nullptr;
    }
}

bool SpatiaLiteResultSet::Next() {
    if (!impl_->stmt) {
        return false;
    }
    
    int rc = sqlite3_step(impl_->stmt);
    return rc == SQLITE_ROW;
}

bool SpatiaLiteResultSet::IsEOF() const {
    return impl_->stmt == nullptr;
}

int32_t SpatiaLiteResultSet::GetInt(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return 0;
    }
    
    return static_cast<int32_t>(sqlite3_column_int(impl_->stmt, columnIndex));
}

int64_t SpatiaLiteResultSet::GetInt64(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return 0;
    }
    
    return sqlite3_column_int64(impl_->stmt, columnIndex);
}

double SpatiaLiteResultSet::GetDouble(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return 0.0;
    }
    
    return sqlite3_column_double(impl_->stmt, columnIndex);
}

std::string SpatiaLiteResultSet::GetString(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return "";
    }
    
    const unsigned char* text = sqlite3_column_text(impl_->stmt, columnIndex);
    if (!text) {
        return "";
    }
    
    return std::string(reinterpret_cast<const char*>(text));
}

GeometryPtr SpatiaLiteResultSet::GetGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return nullptr;
    }
    
    const void* blob = sqlite3_column_blob(impl_->stmt, columnIndex);
    int len = sqlite3_column_bytes(impl_->stmt, columnIndex);
    
    if (!blob || len <= 0) {
        return nullptr;
    }
    
    std::vector<uint8_t> wkb(reinterpret_cast<const uint8_t*>(blob), 
                             reinterpret_cast<const uint8_t*>(blob) + len);
    
    std::unique_ptr<Geometry> geometry;
    Result result = WkbConverter::WkbToGeometry(wkb, geometry);
    
    if (result.IsSuccess() && geometry) {
        return std::move(geometry);
    }
    
    return nullptr;
}

int SpatiaLiteResultSet::GetColumnCount() const {
    return impl_->columnCount;
}

std::string SpatiaLiteResultSet::GetColumnName(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return "";
    }
    
    const char* name = sqlite3_column_name(impl_->stmt, columnIndex);
    return name ? std::string(name) : "";
}

ColumnType SpatiaLiteResultSet::GetColumnType(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount) {
        return ColumnType::kUnknown;
    }
    
    return ConvertSqliteType(sqlite3_column_type(impl_->stmt, columnIndex));
}

bool SpatiaLiteResultSet::HasGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return false;
    }
    
    return sqlite3_column_type(impl_->stmt, columnIndex) == SQLITE_BLOB;
}

void SpatiaLiteResultSet::Reset() {
    if (impl_->stmt) {
        sqlite3_reset(impl_->stmt);
        impl_->currentRow = 0;
    }
}

int SpatiaLiteResultSet::GetColumnIndex(const std::string& columnName) const {
    for (int i = 0; i < impl_->columnCount; ++i) {
        if (impl_->columnNames[i] == columnName) {
            return i;
        }
    }
    return -1;
}

bool SpatiaLiteResultSet::IsNull(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return true;
    }
    return sqlite3_column_type(impl_->stmt, columnIndex) == SQLITE_NULL;
}

bool SpatiaLiteResultSet::IsNull(const std::string& columnName) const {
    return IsNull(GetColumnIndex(columnName));
}

int32_t SpatiaLiteResultSet::GetInt(const std::string& columnName) const {
    return GetInt(GetColumnIndex(columnName));
}

int64_t SpatiaLiteResultSet::GetInt64(const std::string& columnName) const {
    return GetInt64(GetColumnIndex(columnName));
}

double SpatiaLiteResultSet::GetDouble(const std::string& columnName) const {
    return GetDouble(GetColumnIndex(columnName));
}

std::string SpatiaLiteResultSet::GetString(const std::string& columnName) const {
    return GetString(GetColumnIndex(columnName));
}

bool SpatiaLiteResultSet::GetBool(int columnIndex) const {
    return GetInt(columnIndex) != 0;
}

bool SpatiaLiteResultSet::GetBool(const std::string& columnName) const {
    return GetBool(GetColumnIndex(columnName));
}

std::vector<uint8_t> SpatiaLiteResultSet::GetBlob(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->columnCount || !impl_->stmt) {
        return {};
    }
    
    const void* blob = sqlite3_column_blob(impl_->stmt, columnIndex);
    int size = sqlite3_column_bytes(impl_->stmt, columnIndex);
    
    if (!blob || size <= 0) {
        return {};
    }
    
    const uint8_t* data = static_cast<const uint8_t*>(blob);
    return std::vector<uint8_t>(data, data + size);
}

std::vector<uint8_t> SpatiaLiteResultSet::GetBlob(const std::string& columnName) const {
    return GetBlob(GetColumnIndex(columnName));
}

GeometryPtr SpatiaLiteResultSet::GetGeometry(const std::string& columnName) const {
    return GetGeometry(GetColumnIndex(columnName));
}

int64_t SpatiaLiteResultSet::GetRowCount() const {
    return -1;
}

int64_t SpatiaLiteResultSet::GetCurrentRow() const {
    return impl_->currentRow;
}

Result SpatiaLiteResultSet::GetLastError() const {
    return impl_->lastError;
}

void SpatiaLiteResultSet::InitializeColumns() {
    impl_->columnNames.resize(impl_->columnCount);
    impl_->columnTypes.resize(impl_->columnCount);
    
    for (int i = 0; i < impl_->columnCount; ++i) {
        const char* name = sqlite3_column_name(impl_->stmt, i);
        impl_->columnNames[i] = name ? name : "";
        impl_->columnTypes[i] = sqlite3_column_type(impl_->stmt, i);
    }
}

ColumnType SpatiaLiteResultSet::ConvertSqliteType(int sqliteType) const {
    switch (sqliteType) {
        case SQLITE_INTEGER: return ColumnType::kBigInt;
        case SQLITE_FLOAT: return ColumnType::kDouble;
        case SQLITE_TEXT: return ColumnType::kText;
        case SQLITE_BLOB: return ColumnType::kBlob;
        case SQLITE_NULL: return ColumnType::kUnknown;
        default: return ColumnType::kUnknown;
    }
}

bool SpatiaLiteConnection::InTransaction() const {
    return impl_->isTransaction;
}

Result SpatiaLiteConnection::InsertGeometries(const std::string& table,
                                              const std::string& geomColumn,
                                              const std::vector<const Geometry*>& geometries,
                                              const std::vector<std::map<std::string, std::string>>& attributes,
                                              std::vector<int64_t>& outIds) {
    outIds.clear();
    for (size_t i = 0; i < geometries.size(); ++i) {
        int64_t id = 0;
        const auto& attrs = i < attributes.size() ? attributes[i] : std::map<std::string, std::string>();
        Result result = InsertGeometry(table, geomColumn, geometries[i], attrs, id);
        if (!result.IsSuccess()) {
            return result;
        }
        outIds.push_back(id);
    }
    return Result::Success();
}

Result SpatiaLiteConnection::SelectGeometries(const std::string& table,
                                              const std::string& geomColumn,
                                              const std::string& whereClause,
                                              std::vector<GeometryPtr>& geometries) {
    std::ostringstream sql;
    sql << "SELECT " << geomColumn << " FROM " << table;
    if (!whereClause.empty()) {
        sql << " WHERE " << whereClause;
    }
    
    DbResultSetPtr resultSet;
    Result result = ExecuteQuery(sql.str(), resultSet);
    if (!result.IsSuccess()) {
        return result;
    }
    
    while (resultSet->Next()) {
        GeometryPtr geom = resultSet->GetGeometry(0);
        if (geom) {
            geometries.push_back(std::move(geom));
        }
    }
    
    return Result::Success();
}

Result SpatiaLiteConnection::SpatialQuery(const std::string& table,
                                          const std::string& geomColumn,
                                          SpatialOperator op,
                                          const Geometry* queryGeom,
                                          std::vector<GeometryPtr>& results) {
    if (!queryGeom) {
        return Result::Error(DbResult::kInvalidParameter, "Query geometry is null");
    }
    
    std::string opName = GetSpatialOperatorName(op);
    std::ostringstream sql;
    sql << "SELECT ST_AsBinary(" << geomColumn << ") FROM " << table 
        << " WHERE " << opName << "(" << geomColumn << ", GeomFromText('" 
        << queryGeom->AsText() << "')) = 1";
    
    DbResultSetPtr resultSet;
    Result result = ExecuteQuery(sql.str(), resultSet);
    if (!result.IsSuccess()) {
        return result;
    }
    
    while (resultSet->Next()) {
        GeometryPtr geom = resultSet->GetGeometry(0);
        if (geom) {
            results.push_back(std::move(geom));
        }
    }
    
    return Result::Success();
}

Result SpatiaLiteConnection::SpatialQueryWithEnvelope(const std::string& table,
                                                      const std::string& geomColumn,
                                                      SpatialOperator op,
                                                      const Geometry* queryGeom,
                                                      const ::ogc::Envelope& filter,
                                                      std::vector<GeometryPtr>& results) {
    std::ostringstream sql;
    sql << "SELECT ST_AsBinary(" << geomColumn << ") FROM " << table 
        << " WHERE MbrIntersects(" << geomColumn << ", BuildMbr(" 
        << filter.GetMinX() << ", " << filter.GetMinY() << ", "
        << filter.GetMaxX() << ", " << filter.GetMaxY() << "))";
    
    if (queryGeom) {
        std::string opName = GetSpatialOperatorName(op);
        sql << " AND " << opName << "(" << geomColumn << ", GeomFromText('" 
            << queryGeom->AsText() << "')) = 1";
    }
    
    DbResultSetPtr resultSet;
    Result result = ExecuteQuery(sql.str(), resultSet);
    if (!result.IsSuccess()) {
        return result;
    }
    
    while (resultSet->Next()) {
        GeometryPtr geom = resultSet->GetGeometry(0);
        if (geom) {
            results.push_back(std::move(geom));
        }
    }
    
    return Result::Success();
}

Result SpatiaLiteConnection::UpdateGeometry(const std::string& table,
                                            const std::string& geomColumn,
                                            int64_t id,
                                            const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidParameter, "Geometry is null");
    }
    
    std::ostringstream sql;
    sql << "UPDATE " << table << " SET " << geomColumn << " = GeomFromText('" 
        << geometry->AsText() << "') WHERE rowid = " << id;
    
    return Execute(sql.str());
}

Result SpatiaLiteConnection::DeleteGeometry(const std::string& table, int64_t id) {
    std::ostringstream sql;
    sql << "DELETE FROM " << table << " WHERE rowid = " << id;
    return Execute(sql.str());
}

Result SpatiaLiteConnection::CreateSpatialTable(const std::string& tableName,
                                                const std::string& geomColumn,
                                                int geomType,
                                                int srid,
                                                int coordDimension) {
    std::ostringstream sql;
    sql << "CREATE TABLE " << tableName << " (id INTEGER PRIMARY KEY AUTOINCREMENT)";
    
    Result result = Execute(sql.str());
    if (!result.IsSuccess()) {
        return result;
    }
    
    std::string geomTypeStr;
    switch (geomType) {
        case 1: geomTypeStr = "POINT"; break;
        case 2: geomTypeStr = "LINESTRING"; break;
        case 3: geomTypeStr = "POLYGON"; break;
        case 4: geomTypeStr = "MULTIPOINT"; break;
        case 5: geomTypeStr = "MULTILINESTRING"; break;
        case 6: geomTypeStr = "MULTIPOLYGON"; break;
        default: geomTypeStr = "GEOMETRY"; break;
    }
    
    std::ostringstream addGeomSql;
    addGeomSql << "SELECT AddGeometryColumn('" << tableName << "', '" << geomColumn 
               << "', " << srid << ", '" << geomTypeStr << "', " << coordDimension << ")";
    
    return Execute(addGeomSql.str());
}

Result SpatiaLiteConnection::DropSpatialIndex(const std::string& tableName,
                                              const std::string& geomColumn) {
    std::ostringstream sql;
    sql << "SELECT DisableSpatialIndex('" << tableName << "', '" << geomColumn << "')";
    return Execute(sql.str());
}

bool SpatiaLiteConnection::SpatialTableExists(const std::string& tableName) const {
    std::ostringstream sql;
    sql << "SELECT name FROM sqlite_master WHERE type='table' AND name='" << tableName << "'";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(impl_->db, sql.str().c_str(), -1, &stmt, nullptr);
    bool exists = false;
    if (rc == SQLITE_OK) {
        exists = (sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
    }
    return exists;
}

Result SpatiaLiteConnection::GetTableInfo(const std::string& tableName, TableInfo& info) const {
    info.name = tableName;
    
    std::ostringstream sql;
    sql << "SELECT f_geometry_column, srid, type, coord_dimension FROM geometry_columns "
        << "WHERE f_table_name = '" << tableName << "'";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(impl_->db, sql.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        info.geomColumn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        info.srid = sqlite3_column_int(stmt, 1);
        info.geomType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        info.coordDimension = sqlite3_column_int(stmt, 3);
        sqlite3_finalize(stmt);
        return Result::Success();
    }
    if (stmt) sqlite3_finalize(stmt);
    return Result::Error(DbResult::kTableNotFound, "Table not found in geometry_columns");
}

Result SpatiaLiteConnection::GetColumns(const std::string& tableName, std::vector<ColumnInfo>& columns) const {
    columns.clear();
    
    std::ostringstream sql;
    sql << "PRAGMA table_info(" << tableName << ")";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(impl_->db, sql.str().c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ColumnInfo col;
            col.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            col.type = ColumnType::kUnknown;
            col.isNullable = (sqlite3_column_int(stmt, 3) == 0);
            col.isPrimaryKey = (sqlite3_column_int(stmt, 5) > 0);
            columns.push_back(col);
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return Result::Success();
}

DatabaseType SpatiaLiteConnection::GetType() const {
    return DatabaseType::kSpatiaLite;
}

std::string SpatiaLiteConnection::GetVersion() const {
    return sqlite3_libversion();
}

int64_t SpatiaLiteConnection::GetLastInsertId() const {
    return sqlite3_last_insert_rowid(impl_->db);
}

int64_t SpatiaLiteConnection::GetRowsAffected() const {
    return impl_->rowsAffected;
}

Result SpatiaLiteConnection::SetIsolationLevel(TransactionIsolation level) {
    impl_->isolationLevel = level;
    return Result::Success();
}

TransactionIsolation SpatiaLiteConnection::GetIsolationLevel() const {
    return impl_->isolationLevel;
}

const ConnectionInfo& SpatiaLiteConnection::GetConnectionInfo() const {
    return impl_->connectionInfo;
}

std::string SpatiaLiteConnection::EscapeString(const std::string& value) const {
    std::string result;
    result.reserve(value.length() * 2);
    for (char c : value) {
        if (c == '\'') {
            result += "''";
        } else {
            result += c;
        }
    }
    return result;
}

sqlite3* SpatiaLiteConnection::GetRawConnection() {
    return impl_->db;
}

}
}
