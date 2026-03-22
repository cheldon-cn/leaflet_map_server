#include "ogc/db/postgis_connection.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/envelope.h"
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
    DbResultSetPtr dummy;
    return ExecuteQuery(dummy);
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
    return static_cast<int>(m_paramValues.size());
}

int PostGISStatement::GetParameterIndex(const std::string& paramName) const {
    return 0;
}

Result PostGISStatement::BindBool(int paramIndex, bool value) {
    return BindInt(paramIndex, value ? 1 : 0);
}

Result PostGISStatement::BindNull(int paramIndex) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].clear();
    m_paramLengths[paramIndex] = 0;
    m_paramFormats[paramIndex] = 0;
    m_paramTypes[paramIndex] = 0;
    
    return Result::Success();
}

Result PostGISStatement::BindBlob(int paramIndex, const std::vector<uint8_t>& data) {
    return BindBlob(paramIndex, data.data(), data.size());
}

Result PostGISStatement::BindBlob(int paramIndex, const uint8_t* data, size_t size) {
    if (paramIndex < 0 || paramIndex >= static_cast<int>(m_paramValues.size())) {
        return Result::Error(DbResult::kInvalidParameter, "Invalid parameter index");
    }
    
    m_paramValues[paramIndex].assign(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
    m_paramLengths[paramIndex] = static_cast<int>(size);
    m_paramFormats[paramIndex] = 1;
    m_paramTypes[paramIndex] = BYTEAOID;
    
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

PostGISResultSet::PostGISResultSet(PGresult* result)
    : m_result(result), m_currentRow(-1), m_numRows(0), m_numColumns(0), m_lastError() {
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
        return std::move(geometry);
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
    
    unsigned int typeOid = m_columnTypes[columnIndex];
    return typeOid == BYTEAOID || typeOid == 1784;
}

void PostGISResultSet::Reset() {
    m_currentRow = -1;
}

int PostGISResultSet::GetColumnIndex(const std::string& columnName) const {
    for (int i = 0; i < m_numColumns; ++i) {
        if (m_columnNames[i] == columnName) {
            return i;
        }
    }
    return -1;
}

bool PostGISResultSet::IsNull(int columnIndex) const {
    if (columnIndex < 0 || columnIndex >= m_numColumns || m_currentRow < 0) {
        return true;
    }
    return PQgetisnull(m_result, m_currentRow, columnIndex);
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
    if (columnIndex < 0 || columnIndex >= m_numColumns || m_currentRow < 0) {
        return {};
    }
    
    if (PQgetisnull(m_result, m_currentRow, columnIndex)) {
        return {};
    }
    
    const char* data = PQgetvalue(m_result, m_currentRow, columnIndex);
    int len = PQgetlength(m_result, m_currentRow, columnIndex);
    
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
    return m_numRows;
}

int64_t PostGISResultSet::GetCurrentRow() const {
    return m_currentRow;
}

Result PostGISResultSet::GetLastError() const {
    return m_lastError;
}

void PostGISResultSet::InitializeColumns() {
    m_columnNames.resize(m_numColumns);
    m_columnTypes.resize(m_numColumns);
    
    for (int i = 0; i < m_numColumns; ++i) {
        m_columnNames[i] = PQfname(m_result, i);
        m_columnTypes[i] = PQftype(m_result, i);
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
    return m_isTransaction;
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
    
    PGresult* res = PQexec(m_conn, sql.str().c_str());
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
    
    PGresult* res = PQexec(m_conn, sql.str().c_str());
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
    
    PGresult* res = PQexec(m_conn, sql.str().c_str());
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
    if (!m_conn) return "";
    int version = PQserverVersion(m_conn);
    return std::to_string(version);
}

int64_t PostGISConnection::GetLastInsertId() const {
    PGresult* res = PQexec(m_conn, "SELECT lastval()");
    int64_t id = 0;
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        id = std::stoll(PQgetvalue(res, 0, 0));
    }
    if (res) PQclear(res);
    return id;
}

int64_t PostGISConnection::GetRowsAffected() const {
    return m_rowsAffected;
}

Result PostGISConnection::SetIsolationLevel(TransactionIsolation level) {
    m_isolationLevel = level;
    return Result::Success();
}

TransactionIsolation PostGISConnection::GetIsolationLevel() const {
    return m_isolationLevel;
}

const ConnectionInfo& PostGISConnection::GetConnectionInfo() const {
    return m_connectionInfo;
}

std::string PostGISConnection::EscapeString(const std::string& value) const {
    if (!m_conn) return value;
    char* escaped = PQescapeLiteral(m_conn, value.c_str(), value.length());
    std::string result = escaped ? escaped : value;
    if (escaped) PQfreemem(escaped);
    return result;
}

}
}
