#include "ogc/db/postgis_connection.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include <sstream>
#include <cstring>

namespace ogc {
namespace db {

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

PostGISConnectionPtr PostGISConnection::Create() {
    return std::make_unique<PostGISConnection>();
}

PostGISConnection::PostGISConnection()
    : m_conn(nullptr), m_isTransaction(false) {
}

PostGISConnection::~PostGISConnection() {
    Disconnect();
}

Result PostGISConnection::Connect(const std::string& connectionString) {
    Disconnect();
    
    m_conn = PQconnectdb(connectionString.c_str());
    
    if (PQstatus(m_conn) != CONNECTION_OK) {
        m_lastError = PQerrorMessage(m_conn);
        PQfinish(m_conn);
        m_conn = nullptr;
        return Result::Error(DbResult::kConnectionFailed, m_lastError);
    }
    
    return Result::Success();
}

Result PostGISConnection::Connect(const PostGISOptions& options) {
    return Connect(options.ToConnectionString());
}

void PostGISConnection::Disconnect() {
    if (m_conn) {
        if (m_isTransaction) {
            PQexec(m_conn, "ROLLBACK");
        }
        PQfinish(m_conn);
        m_conn = nullptr;
    }
}

bool PostGISConnection::IsConnected() const {
    return m_conn && PQstatus(m_conn) == CONNECTION_OK;
}

bool PostGISConnection::Ping() const {
    if (!m_conn) return false;
    
    PGresult* res = PQexec(m_conn, "SELECT 1");
    bool ok = PQresultStatus(res) == PGRES_TUPLES_OK;
    PQclear(res);
    
    return ok;
}

Result PostGISConnection::BeginTransaction() {
    if (m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "Transaction already in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("BEGIN", res);
    
    if (result.IsSuccess()) {
        m_isTransaction = true;
    }
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::Commit() {
    if (!m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("COMMIT", res);
    
    if (result.IsSuccess()) {
        m_isTransaction = false;
    }
    
    if (res) PQclear(res);
    return result;
}

Result PostGISConnection::Rollback() {
    if (!m_isTransaction) {
        return Result::Error(DbResult::kTransactionError, "No transaction in progress");
    }
    
    PGresult* res = nullptr;
    Result result = ExecuteInternal("ROLLBACK", res);
    
    if (result.IsSuccess()) {
        m_isTransaction = false;
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
    PGresult* res = PQprepare(m_conn, name.c_str(), sql.c_str(), 0, nullptr);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        m_lastError = PQerrorMessage(m_conn);
        PQclear(res);
        return Result::Error(DbResult::kPrepareError, m_lastError);
    }
    
    PQclear(res);
    
    stmt = std::make_unique<PostGISStatement>(m_conn, name);
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
    return m_lastError;
}

Result PostGISConnection::CheckConnection() {
    if (!m_conn || PQstatus(m_conn) != CONNECTION_OK) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    return Result::Success();
}

Result PostGISConnection::ExecuteInternal(const std::string& sql, PGresult*& result) {
    Result connResult = CheckConnection();
    if (!connResult.IsSuccess()) {
        return connResult;
    }
    
    result = PQexec(m_conn, sql.c_str());
    
    ExecStatusType status = PQresultStatus(result);
    
    if (status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK) {
        return Result::Success();
    }
    
    m_lastError = PQerrorMessage(m_conn);
    
    if (status == PGRES_FATAL_ERROR) {
        return Result::Error(DbResult::kExecutionError, m_lastError);
    }
    
    return Result::Error(DbResult::kUnknownError, m_lastError);
}

PostGISStatement::PostGISStatement(PGconn* conn, const std::string& name)
    : m_conn(conn), m_name(name) {
}

PostGISStatement::~PostGISStatement() {
}

Result PostGISStatement::BindInt(int paramIndex, int32_t value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].clear();
    m_paramValues[paramIndex] = std::vector<char>(sizeof(int32_t));
    std::memcpy(m_paramValues[paramIndex].data(), &value, sizeof(int32_t));
    m_paramLengths[paramIndex] = sizeof(int32_t);
    m_paramFormats[paramIndex] = 1;
    m_paramTypes[paramIndex] = INT4OID;
    
    return Result::Success();
}

Result PostGISStatement::BindInt64(int paramIndex, int64_t value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].clear();
    m_paramValues[paramIndex] = std::vector<char>(sizeof(int64_t));
    std::memcpy(m_paramValues[paramIndex].data(), &value, sizeof(int64_t));
    m_paramLengths[paramIndex] = sizeof(int64_t);
    m_paramFormats[paramIndex] = 1;
    m_paramTypes[paramIndex] = INT8OID;
    
    return Result::Success();
}

Result PostGISStatement::BindDouble(int paramIndex, double value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].clear();
    m_paramValues[paramIndex] = std::vector<char>(sizeof(double));
    std::memcpy(m_paramValues[paramIndex].data(), &value, sizeof(double));
    m_paramLengths[paramIndex] = sizeof(double);
    m_paramFormats[paramIndex] = 1;
    m_paramTypes[paramIndex] = FLOAT8OID;
    
    return Result::Success();
}

Result PostGISStatement::BindString(int paramIndex, const std::string& value) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].assign(value.begin(), value.end());
    m_paramLengths[paramIndex] = static_cast<int>(value.length());
    m_paramFormats[paramIndex] = 0;
    m_paramTypes[paramIndex] = TEXTOID;
    
    return Result::Success();
}

Result PostGISStatement::BindGeometry(int paramIndex, const Geometry* geometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result result = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!result.IsSuccess()) {
        return result;
    }
    
    m_paramValues[paramIndex].assign(wkb.begin(), wkb.end());
    m_paramLengths[paramIndex] = static_cast<int>(wkb.size());
    m_paramFormats[paramIndex] = 1;
    m_paramTypes[paramIndex] = BYTEAOID;
    
    return Result::Success();
}

Result PostGISStatement::Execute() {
    return ExecuteQuery(nullptr);
}

Result PostGISStatement::ExecuteQuery(DbResultSetPtr& result) {
    if (!m_conn) {
        return Result::Error(DbResult::kNotConnected, "Not connected");
    }
    
    std::vector<const char*> paramValues(m_paramValues.size());
    for (size_t i = 0; i < m_paramValues.size(); ++i) {
        if (!m_paramValues[i].empty()) {
            paramValues[i] = m_paramValues[i].data();
        } else {
            paramValues[i] = "";
        }
    }
    
    PGresult* res = PQexecPrepared(m_conn, m_name.c_str(),
        static_cast<int>(paramValues.size()),
        paramValues.data(),
        m_paramLengths.data(),
        m_paramFormats.data(),
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
    
    std::string error = PQerrorMessage(m_conn);
    PQclear(res);
    return Result::Error(DbResult::kExecutionError, error);
}

Result PostGISStatement::Reset() {
    for (auto& param : m_paramValues) {
        param.clear();
    }
    std::fill(m_paramLengths.begin(), m_paramLengths.end(), 0);
    std::fill(m_paramFormats.begin(), m_paramFormats.end(), 0);
    return Result::Success();
}

PostGISResultSet::PostGISResultSet(PGresult* result)
    : m_result(result), m_currentRow(-1), m_numRows(0), m_numColumns(0) {
    if (m_result) {
        m_numRows = PQntuples(m_result);
        m_numColumns = PQnfields(m_result);
        InitializeColumns();
    }
}

PostGISResultSet::~PostGISResultSet() {
    if (m_result) {
        PQclear(m_result);
        m_result = nullptr;
    }
}

bool PostGISResultSet::Next() {
    if (!m_result || m_currentRow >= m_numRows - 1) {
        return false;
    }
    ++m_currentRow;
    return true;
}

bool PostGISResultSet::IsEOF() const {
    return !m_result || m_currentRow >= m_numRows || m_currentRow < 0;
}

int32_t PostGISResultSet::GetInt(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return 0;
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return 0;
    }
    
    return static_cast<int32_t>(std::atoi(PQgetvalue(m_result, m_currentRow, columnIndex)));
}

int64_t PostGISResultSet::GetInt64(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return 0;
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return 0;
    }
    
    return std::stoll(PQgetvalue(m_result, m_currentRow, columnIndex));
}

double PostGISResultSet::GetDouble(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return 0.0;
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return 0.0;
    }
    
    return std::stod(PQgetvalue(m_result, m_currentRow, columnIndex));
}

std::string PostGISResultSet::GetString(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return "";
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return "";
    }
    
    return std::string(PQgetvalue(m_result, m_currentRow, columnIndex));
}

GeometryPtr PostGISResultSet::GetGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return nullptr;
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return nullptr;
    }
    
    int len = PQgetlength(m_result, m_currentRow, columnIndex);
    const char* value = PQgetvalue(m_result, m_currentRow, columnIndex);
    
    if (!value || len <= 0) {
        return nullptr;
    }
    
    std::vector<uint8_t> wkb(reinterpret_cast<const uint8_t*>(value), 
                             reinterpret_cast<const uint8_t*>(value) + len);
    
    std::unique_ptr<Geometry> geometry;
    Result result = WkbConverter::WkbToGeometry(wkb, geometry);
    
    if (result.IsSuccess() && geometry) {
        return geometry.release();
    }
    
    return nullptr;
}

int PostGISResultSet::GetColumnCount() const {
    return m_numColumns;
}

std::string PostGISResultSet::GetColumnName(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns || !m_result) {
        return "";
    }
    
    return std::string(PQfname(m_result, columnIndex));
}

ColumnType PostGISResultSet::GetColumnType(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return ColumnType::kUnknown;
    }
    
    return ConvertPgType(m_columnTypes[columnIndex]);
}

bool PostGISResultSet::HasGeometry(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns) {
        return false;
    }
    
    Oid typeOid = m_columnTypes[columnIndex];
    return typeOid == BYTEAOID || typeOid == 1784;
}

void PostGISResultSet::InitializeColumns() {
    m_columnNames.resize(m_numColumns);
    m_columnTypes.resize(m_numColumns);
    
    for (int i = 0; i < m_numColumns; ++i) {
        m_columnNames[i] = PQfname(m_result, i);
        m_columnTypes[i] = PQftype(m_result, i);
    }
}

ColumnType PostGISResultSet::ConvertPgType(Oid pgType) const {
    switch (pgType) {
        case BOOLOID: return ColumnType::kBool;
        case INT2OID: return ColumnType::kInt16;
        case INT4OID: return ColumnType::kInt32;
        case INT8OID: return ColumnType::kInt64;
        case FLOAT4OID: return ColumnType::kFloat32;
        case FLOAT8OID: return ColumnType::kFloat64;
        case TEXTOID:
        case VARCHAROID:
        case BPCHAROID: return ColumnType::kString;
        case BYTEAOID: return ColumnType::kBinary;
        case DATEOID: return ColumnType::kDate;
        case TIMEOID: return ColumnType::kTime;
        case TIMESTAMPOID:
        case TIMESTAMPTZOID: return ColumnType::kTimestamp;
        default: return ColumnType::kUnknown;
    }
}

}
}
