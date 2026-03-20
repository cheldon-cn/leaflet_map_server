#include "ogc/db/sqlite_connection.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <sstream>
#include <cstring>

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

SpatiaLiteConnectionPtr SpatiaLiteConnection::Create() {
    return std::make_unique<SpatiaLiteConnection>();
}

SpatiaLiteConnection::SpatiaLiteConnection()
    : m_db(nullptr), m_isTransaction(false), m_spatialInitialized(false) {
}

SpatiaLiteConnection::~SpatiaLiteConnection() {
    Disconnect();
}

Result SpatiaLiteConnection::Connect(const std::string& connectionString) {
    Disconnect();
    
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    int rc = sqlite3_open_v2(connectionString.c_str(), &m_db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        m_lastError = sqlite3_errmsg(m_db);
        sqlite3_close(m_db);
        m_db = nullptr;
        return Result::Error(DbResult::kConnectionFailed, m_lastError);
    }
    
    sqlite3_busy_timeout(m_db, 5000);
    
    return Result::Success();
}

Result SpatiaLiteConnection::Connect(const SpatiaLiteOptions& options) {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    
    if (options.readOnly) {
        flags = SQLITE_OPEN_READONLY;
    }
    
    int rc = sqlite3_open_v2(options.databasePath.c_str(), &m_db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        m_lastError = sqlite3_errmsg(m_db);
        sqlite3_close(m_db);
        m_db = nullptr;
        return Result::Error(DbResult::kConnectionFailed, m_lastError);
    }
    
    std::string cachePragma = "PRAGMA cache_size = " + options.cacheSize;
    sqlite3_exec(m_db, cachePragma.c_str(), nullptr, nullptr, nullptr);
    
    std::string timeoutPragma = "PRAGMA busy_timeout = " + std::to_string(options.connectionTimeout);
    sqlite3_exec(m_db, timeoutPragma.c_str(), nullptr, nullptr, nullptr);
    
    return Result::Success();
}

void SpatiaLiteConnection::Disconnect() {
    if (m_db) {
        if (m_isTransaction) {
            sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
        }
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool SpatiaLiteConnection::IsConnected() const {
    return m_db != nullptr;
}

bool SpatiaLiteConnection::Ping() const {
    if (!m_db) return false;
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, "SELECT 1", -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_ROW;
}

Result SpatiaLiteConnection::BeginTransaction() {
    if (m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "Transaction already in progress");
    }
    
    Result result = Execute("BEGIN TRANSACTION");
    
    if (result.IsSuccess()) {
        m_isTransaction = true;
    }
    
    return result;
}

Result SpatiaLiteConnection::Commit() {
    if (!m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    Result result = Execute("COMMIT");
    
    if (result.IsSuccess()) {
        m_isTransaction = false;
    }
    
    return result;
}

Result SpatiaLiteConnection::Rollback() {
    if (!m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    Result result = Execute("ROLLBACK");
    
    if (result.IsSuccess()) {
        m_isTransaction = false;
    }
    
    return result;
}

Result SpatiaLiteConnection::Execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        m_lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return Result::Error(DbResult::kExecutionError, m_lastError);
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
    stmt = std::make_unique<SpatiaLiteStatement>(m_db, sql);
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
    options.includeSRID = true;
    
    Result wkbResult = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    std::string hexWkb;
    wkbResult = WkbConverter::GeometryToHexWkb(geometry, hexWkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    std::ostringstream sql;
    sql << "INSERT INTO " << table << " (";
    
    bool first = true;
    sql << geomColumn;
    for (const auto& attr : attributes) {
        sql << ", " << attr.first;
    }
    sql << ") VALUES (";
    
    first = true;
    sql << "GeomFromEWKB(X'" << hexWkb << "')";
    for (const auto& attr : attributes) {
        sql << ", '" << attr.second << "'";
    }
    sql << ")";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.str().c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        m_lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return Result::Error(DbResult::kExecutionError, m_lastError);
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
        m_spatialInitialized = true;
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
    return m_lastError;
}

Result SpatiaLiteConnection::CheckConnection() {
    if (!m_db) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    return Result::Success();
}

Result SpatiaLiteConnection::ExecuteInternal(const std::string& sql, sqlite3_stmt*& stmt, bool finalize) {
    Result connResult = CheckConnection();
    if (!connResult.IsSuccess()) {
        return connResult;
    }
    
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        m_lastError = sqlite3_errmsg(m_db);
        return Result::Error(DbResult::kPrepareError, m_lastError);
    }
    
    rc = sqlite3_step(stmt);
    
    if (finalize && stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) {
        return Result::Success();
    }
    
    m_lastError = sqlite3_errmsg(m_db);
    return Result::Error(DbResult::kExecutionError, m_lastError);
}

int SpatiaLiteConnection::GetLastInsertRowId() const {
    return static_cast<int>(sqlite3_last_insert_rowid(m_db));
}

SpatiaLiteStatement::SpatiaLiteStatement(sqlite3* db, const std::string& sql)
    : m_db(db), m_stmt(nullptr), m_sql(sql) {
}

SpatiaLiteStatement::~SpatiaLiteStatement() {
    Finalize();
}

Result SpatiaLiteStatement::BindInt(int paramIndex, int32_t value) {
    if (!m_stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_int(m_stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind int");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindInt64(int paramIndex, int64_t value) {
    if (!m_stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_int64(m_stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind int64");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindDouble(int paramIndex, double value) {
    if (!m_stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_bind_double(m_stmt, paramIndex + 1, value);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind double");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindString(int paramIndex, const std::string& value) {
    if (!m_stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    m_bindStrings.push_back(value);
    int rc = sqlite3_bind_text(m_stmt, paramIndex + 1, value.c_str(), -1, SQLITE_TRANSIENT);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind string");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::BindGeometry(int paramIndex, const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (!m_stmt) {
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
    
    int rc = sqlite3_bind_blob(m_stmt, paramIndex + 1, wkb.data(), static_cast<int>(wkb.size()), SQLITE_TRANSIENT);
    
    if (rc != SQLITE_OK) {
        return Result::Error(DbResult::kBindError, "Failed to bind geometry");
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::Execute() {
    if (!m_stmt) {
        Result result = Prepare();
        if (!result.IsSuccess()) {
            return result;
        }
    }
    
    int rc = sqlite3_step(m_stmt);
    
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) {
        sqlite3_reset(m_stmt);
        return Result::Success();
    }
    
    std::string error = sqlite3_errmsg(m_db);
    return Result::Error(DbResult::kExecutionError, error);
}

Result SpatiaLiteStatement::ExecuteQuery(DbResultSetPtr& result) {
    if (!m_stmt) {
        Result prepResult = Prepare();
        if (!prepResult.IsSuccess()) {
            return prepResult;
        }
    }
    
    result = std::make_unique<SpatiaLiteResultSet>(m_stmt);
    m_stmt = nullptr;
    
    return Result::Success();
}

Result SpatiaLiteStatement::Reset() {
    if (m_stmt) {
        sqlite3_reset(m_stmt);
        sqlite3_clear_bindings(m_stmt);
    }
    m_bindStrings.clear();
    return Result::Success();
}

Result SpatiaLiteStatement::Prepare() {
    if (!m_db || m_sql.empty()) {
        return Result::Error(DbResult::kPrepareError, "Invalid database or SQL");
    }
    
    if (m_stmt) {
        sqlite3_finalize(m_stmt);
    }
    
    int rc = sqlite3_prepare_v2(m_db, m_sql.c_str(), -1, &m_stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(m_db);
        return Result::Error(DbResult::kPrepareError, error);
    }
    
    return Result::Success();
}

Result SpatiaLiteStatement::Finalize() {
    if (m_stmt) {
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
    }
    return Result::Success();
}

SpatiaLiteResultSet::SpatiaLiteResultSet(sqlite3_stmt* stmt)
    : m_stmt(stmt), m_columnCount(0) {
    if (m_stmt) {
        m_columnCount = sqlite3_column_count(m_stmt);
        InitializeColumns();
    }
}

SpatiaLiteResultSet::~SpatiaLiteResultSet() {
    if (m_stmt) {
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
    }
}

bool SpatiaLiteResultSet::Next() {
    if (!m_stmt) {
        return false;
    }
    
    int rc = sqlite3_step(m_stmt);
    return rc == SQLITE_ROW;
}

bool SpatiaLiteResultSet::IsEOF() const {
    return m_stmt == nullptr;
}

int32_t SpatiaLiteResultSet::GetInt(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return 0;
    }
    
    return static_cast<int32_t>(sqlite3_column_int(m_stmt, columnIndex));
}

int64_t SpatiaLiteResultSet::GetInt64(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return 0;
    }
    
    return sqlite3_column_int64(m_stmt, columnIndex);
}

double SpatiaLiteResultSet::GetDouble(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return 0.0;
    }
    
    return sqlite3_column_double(m_stmt, columnIndex);
}

std::string SpatiaLiteResultSet::GetString(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return "";
    }
    
    const unsigned char* text = sqlite3_column_text(m_stmt, columnIndex);
    if (!text) {
        return "";
    }
    
    return std::string(reinterpret_cast<const char*>(text));
}

GeometryPtr SpatiaLiteResultSet::GetGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return nullptr;
    }
    
    const void* blob = sqlite3_column_blob(m_stmt, columnIndex);
    int len = sqlite3_column_bytes(m_stmt, columnIndex);
    
    if (!blob || len <= 0) {
        return nullptr;
    }
    
    std::vector<uint8_t> wkb(reinterpret_cast<const uint8_t*>(blob), 
                             reinterpret_cast<const uint8_t*>(blob) + len);
    
    std::unique_ptr<Geometry> geometry;
    Result result = WkbConverter::WkbToGeometry(wkb, geometry);
    
    if (result.IsSuccess() && geometry) {
        return geometry.release();
    }
    
    return nullptr;
}

int SpatiaLiteResultSet::GetColumnCount() const {
    return m_columnCount;
}

std::string SpatiaLiteResultSet::GetColumnName(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return "";
    }
    
    const char* name = sqlite3_column_name(m_stmt, columnIndex);
    return name ? std::string(name) : "";
}

ColumnType SpatiaLiteResultSet::GetColumnType(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount) {
        return ColumnType::kUnknown;
    }
    
    return ConvertSqliteType(sqlite3_column_type(m_stmt, columnIndex));
}

bool SpatiaLiteResultSet::HasGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_columnCount || !m_stmt) {
        return false;
    }
    
    return sqlite3_column_type(m_stmt, columnIndex) == SQLITE_BLOB;
}

void SpatiaLiteResultSet::InitializeColumns() {
    m_columnNames.resize(m_columnCount);
    m_columnTypes.resize(m_columnCount);
    
    for (int i = 0; i < m_columnCount; ++i) {
        const char* name = sqlite3_column_name(m_stmt, i);
        m_columnNames[i] = name ? name : "";
        m_columnTypes[i] = sqlite3_column_type(m_stmt, i);
    }
}

ColumnType SpatiaLiteResultSet::ConvertSqliteType(int sqliteType) const {
    switch (sqliteType) {
        case SQLITE_INTEGER: return ColumnType::kInt64;
        case SQLITE_FLOAT: return ColumnType::kFloat64;
        case SQLITE_TEXT: return ColumnType::kString;
        case SQLITE_BLOB: return ColumnType::kBinary;
        case SQLITE_NULL: return ColumnType::kUnknown;
        default: return ColumnType::kUnknown;
    }
}

}
}
