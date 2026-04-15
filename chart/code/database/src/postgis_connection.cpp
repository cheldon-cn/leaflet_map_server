#include "ogc/db/postgis_connection.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/geometrycollection.h"
#include "ogc/geom/envelope.h"
#include <sstream>
#include <cstring>

namespace ogc {
namespace db {

#ifndef INT2OID
#define INT2OID 21
#endif
#ifndef INT4OID
#define INT4OID 23
#endif
#ifndef INT8OID
#define INT8OID 20
#endif
#ifndef FLOAT4OID
#define FLOAT4OID 700
#endif
#ifndef FLOAT8OID
#define FLOAT8OID 701
#endif
#ifndef TEXTOID
#define TEXTOID 25
#endif
#ifndef BOOLOID
#define BOOLOID 16
#endif
#ifndef BYTEAOID
#define BYTEAOID 17
#endif
#ifndef DATEOID
#define DATEOID 1082
#endif
#ifndef TIMEOID
#define TIMEOID 1083
#endif
#ifndef TIMESTAMPOID
#define TIMESTAMPOID 1114
#endif
#ifndef TIMESTAMPTZOID
#define TIMESTAMPTZOID 1184
#endif
#ifndef VARCHAROID
#define VARCHAROID 1043
#endif
#ifndef BPCHAROID
#define BPCHAROID 1042
#endif

std::string PostGISOptions::ToConnectionString() const {
    std::ostringstream oss;
    oss << "host=" << host << " ";
    oss << "port=" << port << " ";
    oss << "dbname=" << database << " ";
    oss << "user=" << user << " ";
    oss << "password=" << password << " ";
    oss << "sslmode=" << sslmode << " ";
    oss << "connect_timeout=" << connectionTimeout;
    return oss.str();
}

struct PostGISConnection::Impl {
    PGconn* conn;
    bool isTransaction;
    std::string lastError;
    ConnectionInfo connectionInfo;
    TransactionIsolation isolationLevel;
    int64_t rowsAffected;
    
    Impl() : conn(nullptr), isTransaction(false), isolationLevel(TransactionIsolation::kReadCommitted), rowsAffected(0) {}
};

PostGISConnectionPtr PostGISConnection::Create() {
    return std::make_unique<PostGISConnection>();
}

PostGISConnection::PostGISConnection()
    : impl_(new Impl()) {
}

PostGISConnection::~PostGISConnection() {
    Disconnect();
}

Result PostGISConnection::Connect(const std::string& connectionString) {
    Disconnect();
    
    impl_->conn = PQconnectdb(connectionString.c_str());
    
    if (PQstatus(impl_->conn) != CONNECTION_OK) {
        impl_->lastError = PQerrorMessage(impl_->conn);
        PQfinish(impl_->conn);
        impl_->conn = nullptr;
        return Result::Error(DbResult::kConnectionFailed, impl_->lastError);
    }
    
    return Result::Success();
}

Result PostGISConnection::Connect(const PostGISOptions& options) {
    return Connect(options.ToConnectionString());
}

void PostGISConnection::Disconnect() {
    if (impl_->conn) {
        if (impl_->isTransaction) {
            PQexec(impl_->conn, "ROLLBACK");
        }
        PQfinish(impl_->conn);
        impl_->conn = nullptr;
    }
}

bool PostGISConnection::IsConnected() const {
    return impl_->conn && PQstatus(impl_->conn) == CONNECTION_OK;
}

bool PostGISConnection::Ping() const {
    if (!impl_->conn) return false;
    
    PGresult* res = PQexec(impl_->conn, "SELECT 1");
    bool ok = PQresultStatus(res) == PGRES_TUPLES_OK;
    PQclear(res);
    
    return ok;
}

Result PostGISConnection::BeginTransaction() {
    if (impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "Transaction already in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("BEGIN", res);
    
    if (result.IsSuccess()) {
        impl_->isTransaction = true;
    }
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::Commit() {
    if (!impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("COMMIT", res);
    
    if (result.IsSuccess()) {
        impl_->isTransaction = false;
    }
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::Rollback() {
    if (!impl_->isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("ROLLBACK", res);
    
    if (result.IsSuccess()) {
        impl_->isTransaction = false;
    }
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::Execute(const std::string& sql) {
    PGresult* res = nullptr;
    Result result = ExecuteInternal(sql, res);
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::ExecuteQuery(const std::string& sql, DbResultSetPtr& result) {
    PGresult* res = nullptr;
    Result dbResult = ExecuteInternal(sql, res);
    
    if (!dbResult.IsSuccess()) {
        if (res) PQclear(res);
        return dbResult;
    }
    
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (res) PQclear(res);
        return Result::Error(DbResult::kQueryError, "Query did not return tuples");
    }
    
    result = std::make_unique<PostGISResultSet>(res);
    return Result::Success();
}

Result PostGISConnection::PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) {
    PGresult* res = PQprepare(impl_->conn, name.c_str(), sql.c_str(), 0, nullptr);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        impl_->lastError = PQerrorMessage(impl_->conn);
        PQclear(res);
        return Result::Error(DbResult::kPrepareError, impl_->lastError);
    }
    
    PQclear(res);
    
    stmt = std::make_unique<PostGISStatement>(impl_->conn, name);
    stmt->Reset();
    
    return Result::Success();
}

Result PostGISConnection::InsertGeometry(const std::string& table,
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
    sql << "ST_GeomFromEWKB('\\x" << hexWkb << "'::bytea)";
    for (const auto& attr : attributes) {
        sql << ", '" << attr.second << "'";
    }
    sql << ") RETURNING id";
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal(sql.str(), res);
    
    if (!result.IsSuccess()) {
        if (res) PQclear(res);
        return result;
    }
    
    if (res && PQntuples(res) > 0) {
        const char* idStr = PQgetvalue(res, 0, 0);
        outId = std::stoll(idStr);
    }
    
    if (res) PQclear(res);
    return Result::Success();
}

std::string PostGISConnection::GetLastError() const {
    return impl_->lastError;
}

Result PostGISConnection::CheckConnection() {
    if (!impl_->conn || PQstatus(impl_->conn) != CONNECTION_OK) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    return Result::Success();
}

Result PostGISConnection::ExecuteInternal(const std::string& sql, PGresult*& result) {
    Result connResult = CheckConnection();
    if (!connResult.IsSuccess()) {
        return connResult;
    }
    
    result = PQexec(impl_->conn, sql.c_str());
    
    ExecStatusType status = PQresultStatus(result);
    
    if (status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK) {
        return Result::Success();
    }
    
    impl_->lastError = PQerrorMessage(impl_->conn);
    
    if (status == PGRES_FATAL_ERROR) {
        return Result::Error(DbResult::kExecutionError, impl_->lastError);
    }
    
    return Result::Error(DbResult::kUnknownError, impl_->lastError);
}

struct PostGISStatement::Impl {
    PGconn* conn;
    std::string name;
    std::string sql;
    std::vector<std::vector<char>> paramValues;
    std::vector<int> paramLengths;
    std::vector<unsigned int> paramTypes;
    std::vector<int> paramFormats;
    
    Impl(PGconn* c, const std::string& n) : conn(c), name(n) {}
};

PostGISStatement::PostGISStatement(PGconn* conn, const std::string& name)
    : impl_(new Impl(conn, name)) {
}

PostGISStatement::~PostGISStatement() = default;

void PostGISStatement::SetConnection(PGconn* conn) { impl_->conn = conn; }
const std::string& PostGISStatement::GetName() const { return impl_->name; }
const std::string& PostGISStatement::GetSql() const { return impl_->sql; }

Result PostGISStatement::BindInt(int paramIndex, int32_t value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].clear();
    impl_->paramValues[paramIndex] = std::vector<char>(sizeof(int32_t));
    std::memcpy(impl_->paramValues[paramIndex].data(), &value, sizeof(int32_t));
    impl_->paramLengths[paramIndex] = sizeof(int32_t);
    impl_->paramFormats[paramIndex] = 1;
    impl_->paramTypes[paramIndex] = INT4OID;
    
    return Result::Success();
}

Result PostGISStatement::BindInt64(int paramIndex, int64_t value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].clear();
    impl_->paramValues[paramIndex] = std::vector<char>(sizeof(int64_t));
    std::memcpy(impl_->paramValues[paramIndex].data(), &value, sizeof(int64_t));
    impl_->paramLengths[paramIndex] = sizeof(int64_t);
    impl_->paramFormats[paramIndex] = 1;
    impl_->paramTypes[paramIndex] = INT8OID;
    
    return Result::Success();
}

Result PostGISStatement::BindDouble(int paramIndex, double value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].clear();
    impl_->paramValues[paramIndex] = std::vector<char>(sizeof(double));
    std::memcpy(impl_->paramValues[paramIndex].data(), &value, sizeof(double));
    impl_->paramLengths[paramIndex] = sizeof(double);
    impl_->paramFormats[paramIndex] = 1;
    impl_->paramTypes[paramIndex] = FLOAT8OID;
    
    return Result::Success();
}

Result PostGISStatement::BindString(int paramIndex, const std::string& value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].assign(value.begin(), value.end());
    impl_->paramLengths[paramIndex] = static_cast<int>(value.length());
    impl_->paramFormats[paramIndex] = 0;
    impl_->paramTypes[paramIndex] = TEXTOID;
    
    return Result::Success();
}

Result PostGISStatement::BindGeometry(int paramIndex, const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result result = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!result.IsSuccess()) {
        return result;
    }
    
    impl_->paramValues[paramIndex].assign(wkb.begin(), wkb.end());
    impl_->paramLengths[paramIndex] = static_cast<int>(wkb.size());
    impl_->paramFormats[paramIndex] = 1;
    impl_->paramTypes[paramIndex] = BYTEAOID;
    
    return Result::Success();
}

Result PostGISStatement::Execute() {
    DbResultSetPtr dummy;
    return ExecuteQuery(dummy);
}

Result PostGISStatement::ExecuteQuery(DbResultSetPtr& result) {
    if (!impl_->conn) {
        return Result::Error(DbResult::kNotConnected, "Not connected");
    }
    
    std::vector<const char*> paramValues(impl_->paramValues.size());
    for (size_t i = 0; i < impl_->paramValues.size(); ++i) {
        if (!impl_->paramValues[i].empty()) {
            paramValues[i] = impl_->paramValues[i].data();
        } else {
            paramValues[i] = "";
        }
    }
    
    PGresult* res = PQexecPrepared(impl_->conn, impl_->name.c_str(),
        static_cast<int>(paramValues.size()),
        paramValues.data(),
        impl_->paramLengths.data(),
        impl_->paramFormats.data(),
        0);
    
    ExecStatusType status = PQresultStatus(res);
    
    if (status == PGRES_TUPLES_OK && result != nullptr) {
        result = std::make_unique<PostGISResultSet>(res);
        return Result::Success();
    }
    
    if (status == PGRES_COMMAND_OK) {
        PQclear(res);
        return Result::Success();
    }
    
    std::string error = PQerrorMessage(impl_->conn);
    PQclear(res);
    return Result::Error(DbResult::kExecutionError, error);
}

Result PostGISStatement::Reset() {
    for (auto& param : impl_->paramValues) {
        param.clear();
    }
    std::fill(impl_->paramLengths.begin(), impl_->paramLengths.end(), 0);
    std::fill(impl_->paramFormats.begin(), impl_->paramFormats.end(), 0);
    return Result::Success();
}

Result PostGISStatement::ClearBindings() {
    return Reset();
}

int64_t PostGISStatement::GetLastInsertId() const {
    return 0;
}

int64_t PostGISStatement::GetRowsAffected() const {
    return 0;
}

int PostGISStatement::GetParameterCount() const {
    return static_cast<int>(impl_->paramValues.size());
}

int PostGISStatement::GetParameterIndex(const std::string& paramName) const {
    return 0;
}

Result PostGISStatement::BindBool(int paramIndex, bool value) {
    return BindInt(paramIndex, value ? 1 : 0);
}

Result PostGISStatement::BindNull(int paramIndex) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].clear();
    impl_->paramLengths[paramIndex] = 0;
    impl_->paramFormats[paramIndex] = 0;
    impl_->paramTypes[paramIndex] = 0;
    
    return Result::Success();
}

Result PostGISStatement::BindBlob(int paramIndex, const std::vector<uint8_t>& data) {
    return BindBlob(paramIndex, data.data(), data.size());
}

Result PostGISStatement::BindBlob(int paramIndex, const uint8_t* data, size_t size) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(impl_->paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    impl_->paramValues[paramIndex].assign(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
    impl_->paramLengths[paramIndex] = static_cast<int>(size);
    impl_->paramFormats[paramIndex] = 1;
    impl_->paramTypes[paramIndex] = BYTEAOID;
    
    return Result::Success();
}

Result PostGISStatement::BindEnvelope(int paramIndex, const ::ogc::Envelope& envelope) {
    std::ostringstream oss;
    oss << "POLYGON(("
        << envelope.GetMinX() << " " << envelope.GetMinY() << ", "
        << envelope.GetMaxX() << " " << envelope.GetMinY() << ", "
        << envelope.GetMaxX() << " " << envelope.GetMaxY() << ", "
        << envelope.GetMinX() << " " << envelope.GetMaxY() << ", "
        << envelope.GetMinX() << " " << envelope.GetMinY() << "))";
    return BindString(paramIndex, oss.str());
}

Result PostGISStatement::BindInt(const std::string& paramName, int32_t value) {
    return BindInt(GetParameterIndex(paramName), value);
}

Result PostGISStatement::BindInt64(const std::string& paramName, int64_t value) {
    return BindInt64(GetParameterIndex(paramName), value);
}

Result PostGISStatement::BindDouble(const std::string& paramName, double value) {
    return BindDouble(GetParameterIndex(paramName), value);
}

Result PostGISStatement::BindString(const std::string& paramName, const std::string& value) {
    return BindString(GetParameterIndex(paramName), value);
}

Result PostGISStatement::BindBool(const std::string& paramName, bool value) {
    return BindBool(GetParameterIndex(paramName), value);
}

Result PostGISStatement::BindNull(const std::string& paramName) {
    return BindNull(GetParameterIndex(paramName));
}

Result PostGISStatement::BindBlob(const std::string& paramName, const std::vector<uint8_t>& data) {
    return BindBlob(GetParameterIndex(paramName), data);
}

Result PostGISStatement::BindGeometry(const std::string& paramName, const Geometry* geometry) {
    return BindGeometry(GetParameterIndex(paramName), geometry);
}

struct PostGISResultSet::Impl {
    PGresult* result;
    int currentRow;
    int numRows;
    int numColumns;
    Result lastError;
    std::vector<std::string> columnNames;
    std::vector<unsigned int> columnTypes;
    
    Impl(PGresult* r) : result(r), currentRow(-1), numRows(0), numColumns(0) {
        if (result) {
            numRows = PQntuples(result);
            numColumns = PQnfields(result);
        }
    }
};

PostGISResultSet::PostGISResultSet(PGresult* result)
    : impl_(new Impl(result)) {
    if (impl_->result) {
        InitializeColumns();
    }
}

PostGISResultSet::~PostGISResultSet() {
    if (impl_->result) {
        PQclear(impl_->result);
        impl_->result = nullptr;
    }
}

bool PostGISResultSet::Next() {
    if (!impl_->result || impl_->currentRow >= impl_->numRows - 1) {
        return false;
    }
    ++impl_->currentRow;
    return true;
}

bool PostGISResultSet::IsEOF() const {
    return !impl_->result || impl_->currentRow >= impl_->numRows || impl_->currentRow < 0;
}

int32_t PostGISResultSet::GetInt(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return 0;
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return 0;
    }
    
    return static_cast<int32_t>(std::atoi(PQgetvalue(impl_->result, impl_->currentRow, columnIndex)));
}

int64_t PostGISResultSet::GetInt64(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return 0;
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return 0;
    }
    
    return std::stoll(PQgetvalue(impl_->result, impl_->currentRow, columnIndex));
}

double PostGISResultSet::GetDouble(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return 0.0;
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return 0.0;
    }
    
    return std::stod(PQgetvalue(impl_->result, impl_->currentRow, columnIndex));
}

std::string PostGISResultSet::GetString(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return "";
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return "";
    }
    
    return std::string(PQgetvalue(impl_->result, impl_->currentRow, columnIndex));
}

GeometryPtr PostGISResultSet::GetGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return nullptr;
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return nullptr;
    }
    
    int len = PQgetlength(impl_->result, impl_->currentRow, columnIndex);
    const char* value = PQgetvalue(impl_->result, impl_->currentRow, columnIndex);
    
    if (!value || len <= 0) {
        return nullptr;
    }
    
    std::vector<uint8_t> wkb(reinterpret_cast<const uint8_t*>(value), 
                             reinterpret_cast<const uint8_t*>(value) + len);
    
    std::unique_ptr<Geometry> geometry;
    Result result = WkbConverter::WkbToGeometry(wkb, geometry);
    
    if (result.IsSuccess() && geometry) {
        return std::move(geometry);
    }
    
    return nullptr;
}

int PostGISResultSet::GetColumnCount() const {
    return impl_->numColumns;
}

std::string PostGISResultSet::GetColumnName(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns || !impl_->result) {
        return "";
    }
    
    return std::string(PQfname(impl_->result, columnIndex));
}

ColumnType PostGISResultSet::GetColumnType(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return ColumnType::kUnknown;
    }
    
    return ConvertPgType(impl_->columnTypes[columnIndex]);
}

bool PostGISResultSet::HasGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns) {
        return false;
    }
    
    unsigned int typeOid = impl_->columnTypes[columnIndex];
    return typeOid == BYTEAOID || typeOid == 1784;
}

void PostGISResultSet::Reset() {
    impl_->currentRow = -1;
}

int PostGISResultSet::GetColumnIndex(const std::string& columnName) const {
    for (int i = 0; i < impl_->numColumns; ++i) {
        if (impl_->columnNames[i] == columnName) {
            return i;
        }
    }
    return -1;
}

bool PostGISResultSet::IsNull(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns || impl_->currentRow < 0) {
        return true;
    }
    return PQgetisnull(impl_->result, impl_->currentRow, columnIndex);
}

bool PostGISResultSet::IsNull(const std::string& columnName) const {
    return IsNull(GetColumnIndex(columnName));
}

int32_t PostGISResultSet::GetInt(const std::string& columnName) const {
    return GetInt(GetColumnIndex(columnName));
}

int64_t PostGISResultSet::GetInt64(const std::string& columnName) const {
    return GetInt64(GetColumnIndex(columnName));
}

double PostGISResultSet::GetDouble(const std::string& columnName) const {
    return GetDouble(GetColumnIndex(columnName));
}

std::string PostGISResultSet::GetString(const std::string& columnName) const {
    return GetString(GetColumnIndex(columnName));
}

bool PostGISResultSet::GetBool(int columnIndex) const {
    return GetInt(columnIndex) != 0;
}

bool PostGISResultSet::GetBool(const std::string& columnName) const {
    return GetBool(GetColumnIndex(columnName));
}

std::vector<uint8_t> PostGISResultSet::GetBlob(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= impl_->numColumns || impl_->currentRow < 0) {
        return {};
    }
    
    if (PQgetisnull(impl_->result, impl_->currentRow, columnIndex)) {
        return {};
    }
    
    const char* data = PQgetvalue(impl_->result, impl_->currentRow, columnIndex);
    int len = PQgetlength(impl_->result, impl_->currentRow, columnIndex);
    
    if (!data || len <= 0) {
        return {};
    }
    
    return std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(data), 
                                reinterpret_cast<const uint8_t*>(data) + len);
}

std::vector<uint8_t> PostGISResultSet::GetBlob(const std::string& columnName) const {
    return GetBlob(GetColumnIndex(columnName));
}

GeometryPtr PostGISResultSet::GetGeometry(const std::string& columnName) const {
    return GetGeometry(GetColumnIndex(columnName));
}

int64_t PostGISResultSet::GetRowCount() const {
    return impl_->numRows;
}

int64_t PostGISResultSet::GetCurrentRow() const {
    return impl_->currentRow;
}

Result PostGISResultSet::GetLastError() const {
    return impl_->lastError;
}

void PostGISResultSet::InitializeColumns() {
    impl_->columnNames.resize(impl_->numColumns);
    impl_->columnTypes.resize(impl_->numColumns);
    
    for (int i = 0; i < impl_->numColumns; ++i) {
        impl_->columnNames[i] = PQfname(impl_->result, i);
        impl_->columnTypes[i] = PQftype(impl_->result, i);
    }
}

ColumnType PostGISResultSet::ConvertPgType(unsigned int pgType) const {
    switch (pgType) {
        case BOOLOID: return ColumnType::kBoolean;
        case INT2OID: return ColumnType::kInteger;
        case INT4OID: return ColumnType::kInteger;
        case INT8OID: return ColumnType::kBigInt;
        case FLOAT4OID: return ColumnType::kReal;
        case FLOAT8OID: return ColumnType::kDouble;
        case TEXTOID:
        case VARCHAROID:
        case BPCHAROID: return ColumnType::kText;
        case BYTEAOID: return ColumnType::kBlob;
        case DATEOID: return ColumnType::kDate;
        case TIMEOID: return ColumnType::kDateTime;
        case TIMESTAMPOID:
        case TIMESTAMPTZOID: return ColumnType::kTimeStamp;
        default: return ColumnType::kUnknown;
    }
}

bool PostGISConnection::InTransaction() const {
    return impl_->isTransaction;
}

Result PostGISConnection::InsertGeometries(const std::string& table,
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

Result PostGISConnection::SelectGeometries(const std::string& table,
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

Result PostGISConnection::SpatialQuery(const std::string& table,
                                       const std::string& geomColumn,
                                       SpatialOperator op,
                                       const Geometry* queryGeom,
                                       std::vector<GeometryPtr>& results) {
    if (!queryGeom) {
        return Result::Error(DbResult::kInvalidParameter, "Query geometry is null");
    }
    
    std::string opName = GetSpatialOperatorName(op);
    std::ostringstream sql;
    sql << "SELECT " << geomColumn << " FROM " << table 
        << " WHERE " << opName << "(" << geomColumn << ", ST_GeomFromText('" 
        << queryGeom->AsText() << "'))";
    
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

Result PostGISConnection::SpatialQueryWithEnvelope(const std::string& table,
                                                   const std::string& geomColumn,
                                                   SpatialOperator op,
                                                   const Geometry* queryGeom,
                                                   const ::ogc::Envelope& filter,
                                                   std::vector<GeometryPtr>& results) {
    if (!queryGeom) {
        return Result::Error(DbResult::kInvalidParameter, "Query geometry is null");
    }
    
    std::string opName = GetSpatialOperatorName(op);
    std::ostringstream sql;
    sql << "SELECT " << geomColumn << " FROM " << table 
        << " WHERE " << geomColumn << " && ST_MakeEnvelope(" 
        << filter.GetMinX() << ", " << filter.GetMinY() << ", "
        << filter.GetMaxX() << ", " << filter.GetMaxY() << ")"
        << " AND " << opName << "(" << geomColumn << ", ST_GeomFromText('" 
        << queryGeom->AsText() << "'))";
    
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

Result PostGISConnection::UpdateGeometry(const std::string& table,
                                         const std::string& geomColumn,
                                         int64_t id,
                                         const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidParameter, "Geometry is null");
    }
    
    std::ostringstream sql;
    sql << "UPDATE " << table << " SET " << geomColumn << " = ST_GeomFromText('" 
        << geometry->AsText() << "') WHERE id = " << id;
    
    return Execute(sql.str());
}

Result PostGISConnection::DeleteGeometry(const std::string& table, int64_t id) {
    std::ostringstream sql;
    sql << "DELETE FROM " << table << " WHERE id = " << id;
    return Execute(sql.str());
}

Result PostGISConnection::CreateSpatialTable(const std::string& tableName,
                                             const std::string& geomColumn,
                                             int geomType,
                                             int srid,
                                             int coordDimension) {
    std::ostringstream sql;
    sql << "CREATE TABLE " << tableName << " (id SERIAL PRIMARY KEY, " 
        << geomColumn << " geometry(";
    
    switch (geomType) {
        case 1: sql << "POINT"; break;
        case 2: sql << "LINESTRING"; break;
        case 3: sql << "POLYGON"; break;
        case 4: sql << "MULTIPOINT"; break;
        case 5: sql << "MULTILINESTRING"; break;
        case 6: sql << "MULTIPOLYGON"; break;
        default: sql << "GEOMETRY"; break;
    }
    
    sql << ", " << srid << "))";
    
    return Execute(sql.str());
}

Result PostGISConnection::CreateSpatialIndex(const std::string& tableName,
                                             const std::string& geomColumn) {
    std::ostringstream sql;
    sql << "CREATE INDEX idx_" << tableName << "_" << geomColumn 
        << " ON " << tableName << " USING GIST (" << geomColumn << ")";
    return Execute(sql.str());
}

Result PostGISConnection::DropSpatialIndex(const std::string& tableName,
                                           const std::string& geomColumn) {
    std::ostringstream sql;
    sql << "DROP INDEX IF EXISTS idx_" << tableName << "_" << geomColumn;
    return Execute(sql.str());
}

bool PostGISConnection::SpatialTableExists(const std::string& tableName) const {
    std::ostringstream sql;
    sql << "SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = '" 
        << tableName << "')";
    
    PGresult* res = PQexec(impl_->conn, sql.str().c_str());
    bool exists = false;
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        exists = (strcmp(PQgetvalue(res, 0, 0), "t") == 0);
    }
    if (res) PQclear(res);
    return exists;
}

Result PostGISConnection::GetTableInfo(const std::string& tableName, TableInfo& info) const {
    info.name = tableName;
    
    std::ostringstream sql;
    sql << "SELECT f_geometry_column, srid, type, coord_dimension FROM geometry_columns "
        << "WHERE f_table_name = '" << tableName << "'";
    
    PGresult* res = PQexec(impl_->conn, sql.str().c_str());
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        info.geomColumn = PQgetvalue(res, 0, 0);
        info.srid = std::atoi(PQgetvalue(res, 0, 1));
        info.geomType = PQgetvalue(res, 0, 2);
        info.coordDimension = std::atoi(PQgetvalue(res, 0, 3));
        PQclear(res);
        return Result::Success();
    }
    if (res) PQclear(res);
    return Result::Error(DbResult::kTableNotFound, "Table not found in geometry_columns");
}

Result PostGISConnection::GetColumns(const std::string& tableName, std::vector<ColumnInfo>& columns) const {
    columns.clear();
    
    std::ostringstream sql;
    sql << "SELECT column_name, data_type, is_nullable FROM information_schema.columns "
        << "WHERE table_name = '" << tableName << "'";
    
    PGresult* res = PQexec(impl_->conn, sql.str().c_str());
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
        int numRows = PQntuples(res);
        for (int i = 0; i < numRows; ++i) {
            ColumnInfo col;
            col.name = PQgetvalue(res, i, 0);
            col.type = ColumnType::kUnknown;
            col.isNullable = (strcmp(PQgetvalue(res, i, 2), "YES") == 0);
            columns.push_back(col);
        }
    }
    if (res) PQclear(res);
    return Result::Success();
}

DatabaseType PostGISConnection::GetType() const {
    return DatabaseType::kPostGIS;
}

std::string PostGISConnection::GetVersion() const {
    if (!impl_->conn) return "";
    int version = PQserverVersion(impl_->conn);
    return std::to_string(version);
}

int64_t PostGISConnection::GetLastInsertId() const {
    PGresult* res = PQexec(impl_->conn, "SELECT lastval()");
    int64_t id = 0;
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        id = std::stoll(PQgetvalue(res, 0, 0));
    }
    if (res) PQclear(res);
    return id;
}

int64_t PostGISConnection::GetRowsAffected() const {
    return impl_->rowsAffected;
}

Result PostGISConnection::SetIsolationLevel(TransactionIsolation level) {
    impl_->isolationLevel = level;
    return Result::Success();
}

TransactionIsolation PostGISConnection::GetIsolationLevel() const {
    return impl_->isolationLevel;
}

const ConnectionInfo& PostGISConnection::GetConnectionInfo() const {
    return impl_->connectionInfo;
}

std::string PostGISConnection::EscapeString(const std::string& value) const {
    if (!impl_->conn) return value;
    char* escaped = PQescapeLiteral(impl_->conn, value.c_str(), value.length());
    std::string result = escaped ? escaped : value;
    if (escaped) PQfreemem(escaped);
    return result;
}

PGconn* PostGISConnection::GetRawConnection() {
    return impl_->conn;
}

}
}
