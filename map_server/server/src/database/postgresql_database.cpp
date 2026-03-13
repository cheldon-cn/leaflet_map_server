#include "postgresql_database.h"
#include "../utils/logger.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace cycle {
namespace database {

PostgresqlDatabase::PostgresqlDatabase(const DatabaseConfig& config)
    : config_(config)
    , postgisInitialized_(false)
    , lastErrorCode_(0) {
    
    LOG_INFO("PostgreSQL database adapter created for: " + config_.pg_host + 
             ":" + std::to_string(config_.pg_port) + "/" + config_.pg_database);
}

PostgresqlDatabase::~PostgresqlDatabase() {
    Close();
    LOG_INFO("PostgreSQL database adapter destroyed");
}

bool PostgresqlDatabase::Open() {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    if (!connectionPool_.empty()) {
        LOG_WARN("PostgreSQL database is already open");
        return true;
    }
    
    LOG_INFO("Opening PostgreSQL database connection pool");
    
    for (int i = 0; i < config_.max_connections; ++i) {
        PGconn* conn = CreateConnection();
        if (!conn || PQstatus(conn) != CONNECTION_OK) {
            lastError_ = PQerrorMessage(conn);
            lastErrorCode_ = 1;
            LOG_ERROR("Failed to create PostgreSQL connection: " + lastError_);
            
            if (conn) {
                PQfinish(conn);
            }
            
            Close();
            return false;
        }
        
        connectionPool_.push_back(conn);
    }
    
    if (!InitPostGIS()) {
        LOG_ERROR("Failed to initialize PostGIS extension");
        Close();
        return false;
    }
    
    LOG_INFO("PostgreSQL database opened successfully with " + 
             std::to_string(connectionPool_.size()) + " connections");
    
    return true;
}

void PostgresqlDatabase::Close() {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    for (PGconn* conn : connectionPool_) {
        if (conn) {
            PQfinish(conn);
        }
    }
    
    connectionPool_.clear();
    postgisInitialized_ = false;
    
    LOG_INFO("PostgreSQL database closed");
}

bool PostgresqlDatabase::IsOpen() const {
    std::lock_guard<std::mutex> lock(poolMutex_);
    return !connectionPool_.empty();
}

bool PostgresqlDatabase::Execute(const std::string& sql) {
    PGconn* conn = GetConnection();
    if (!conn) {
        lastError_ = "No database connection available";
        lastErrorCode_ = 2;
        return false;
    }
    
    PGresult* result = PQexec(conn, sql.c_str());
    
    bool success = (PQresultStatus(result) == PGRES_COMMAND_OK);
    if (!success) {
        lastError_ = PQresultErrorMessage(result);
        lastErrorCode_ = PQresultStatus(result);
        LOG_ERROR("PostgreSQL execute failed: " + lastError_);
    }
    
    PQclear(result);
    ReturnConnection(conn);
    
    return success;
}

bool PostgresqlDatabase::Execute(const std::string& sql, const std::vector<SqlParameter>& params) {
    PGconn* conn = GetConnection();
    if (!conn) {
        lastError_ = "No database connection available";
        lastErrorCode_ = 2;
        return false;
    }
    
    std::vector<const char*> paramValues;
    std::vector<int> paramLengths;
    std::vector<int> paramFormats;
    
    for (const auto& param : params) {
        switch (param.type) {
            case SqlParameter::INTEGER: {
                std::string value = std::to_string(param.int_value);
                paramValues.push_back(value.c_str());
                paramLengths.push_back(value.length());
                paramFormats.push_back(0); // text format
                break;
            }
            case SqlParameter::REAL: {
                std::string value = std::to_string(param.real_value);
                paramValues.push_back(value.c_str());
                paramLengths.push_back(value.length());
                paramFormats.push_back(0);
                break;
            }
            case SqlParameter::TEXT: {
                paramValues.push_back(param.text_value);
                paramLengths.push_back(strlen(param.text_value));
                paramFormats.push_back(0);
                break;
            }
            case SqlParameter::BLOB: {
                paramValues.push_back(static_cast<const char*>(param.blob_data));
                paramLengths.push_back(param.blob_size);
                paramFormats.push_back(1); // binary format
                break;
            }
            case SqlParameter::NULL_VALUE: {
                paramValues.push_back(nullptr);
                paramLengths.push_back(0);
                paramFormats.push_back(0);
                break;
            }
        }
    }
    
    PGresult* result = PQexecParams(conn, sql.c_str(), 
                                    params.size(), 
                                    nullptr, // param types
                                    paramValues.data(),
                                    paramLengths.data(),
                                    paramFormats.data(),
                                    0); // result format
    
    bool success = (PQresultStatus(result) == PGRES_COMMAND_OK);
    if (!success) {
        lastError_ = PQresultErrorMessage(result);
        lastErrorCode_ = PQresultStatus(result);
        LOG_ERROR("PostgreSQL execute with params failed: " + lastError_);
    }
    
    PQclear(result);
    ReturnConnection(conn);
    
    return success;
}

std::unique_ptr<ResultSet> PostgresqlDatabase::Query(const std::string& sql) {
    PGconn* conn = GetConnection();
    if (!conn) {
        lastError_ = "No database connection available";
        lastErrorCode_ = 2;
        return nullptr;
    }
    
    PGresult* result = PQexec(conn, sql.c_str());
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError_ = PQresultErrorMessage(result);
        lastErrorCode_ = PQresultStatus(result);
        LOG_ERROR("PostgreSQL query failed: " + lastError_);
        
        PQclear(result);
        ReturnConnection(conn);
        return nullptr;
    }
    
    ReturnConnection(conn);
    
    return std::make_unique<PostgresqlResultSet>(result);
}

std::unique_ptr<ResultSet> PostgresqlDatabase::Query(const std::string& sql, 
                                                     const std::vector<SqlParameter>& params) {
    PGconn* conn = GetConnection();
    if (!conn) {
        lastError_ = "No database connection available";
        lastErrorCode_ = 2;
        return nullptr;
    }
    
    std::vector<const char*> paramValues;
    std::vector<int> paramLengths;
    std::vector<int> paramFormats;
    
    for (const auto& param : params) {
        switch (param.type) {
            case SqlParameter::INTEGER: {
                std::string value = std::to_string(param.int_value);
                paramValues.push_back(value.c_str());
                paramLengths.push_back(value.length());
                paramFormats.push_back(0);
                break;
            }
            case SqlParameter::REAL: {
                std::string value = std::to_string(param.real_value);
                paramValues.push_back(value.c_str());
                paramLengths.push_back(value.length());
                paramFormats.push_back(0);
                break;
            }
            case SqlParameter::TEXT: {
                paramValues.push_back(param.text_value);
                paramLengths.push_back(strlen(param.text_value));
                paramFormats.push_back(0);
                break;
            }
            case SqlParameter::BLOB: {
                paramValues.push_back(static_cast<const char*>(param.blob_data));
                paramLengths.push_back(param.blob_size);
                paramFormats.push_back(1);
                break;
            }
            case SqlParameter::NULL_VALUE: {
                paramValues.push_back(nullptr);
                paramLengths.push_back(0);
                paramFormats.push_back(0);
                break;
            }
        }
    }
    
    PGresult* result = PQexecParams(conn, sql.c_str(), 
                                    params.size(), 
                                    nullptr,
                                    paramValues.data(),
                                    paramLengths.data(),
                                    paramFormats.data(),
                                    0);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError_ = PQresultErrorMessage(result);
        lastErrorCode_ = PQresultStatus(result);
        LOG_ERROR("PostgreSQL query with params failed: " + lastError_);
        
        PQclear(result);
        ReturnConnection(conn);
        return nullptr;
    }
    
    ReturnConnection(conn);
    
    return std::make_unique<PostgresqlResultSet>(result);
}

bool PostgresqlDatabase::BeginTransaction() {
    return Execute("BEGIN");
}

bool PostgresqlDatabase::CommitTransaction() {
    return Execute("COMMIT");
}

bool PostgresqlDatabase::RollbackTransaction() {
    return Execute("ROLLBACK");
}

std::string PostgresqlDatabase::GetLastError() const {
    return lastError_;
}

int PostgresqlDatabase::GetLastErrorCode() const {
    return lastErrorCode_;
}

std::string PostgresqlDatabase::GetDatabaseType() const {
    return "PostgreSQL";
}

std::unique_ptr<ResultSet> PostgresqlDatabase::QuerySpatial(const std::string& table,
                                                           const BoundingBox& envelope,
                                                           const std::string& geometryColumn) {
    if (!postgisInitialized_) {
        lastError_ = "PostGIS not initialized";
        lastErrorCode_ = 3;
        return nullptr;
    }
    
    std::string sql = BuildSpatialQuery(table, envelope, geometryColumn);
    return Query(sql);
}

PGconn* PostgresqlDatabase::GetConnection() {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    if (connectionPool_.empty()) {
        return nullptr;
    }
    
    PGconn* conn = connectionPool_.back();
    connectionPool_.pop_back();
    
    if (PQstatus(conn) != CONNECTION_OK) {
        PQreset(conn);
        
        if (PQstatus(conn) != CONNECTION_OK) {
            PQfinish(conn);
            return nullptr;
        }
    }
    
    return conn;
}

void PostgresqlDatabase::ReturnConnection(PGconn* conn) {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    if (conn && PQstatus(conn) == CONNECTION_OK) {
        connectionPool_.push_back(conn);
    } else {
        if (conn) {
            PQfinish(conn);
        }
    }
}

bool PostgresqlDatabase::InitPostGIS() {
    PGconn* conn = GetConnection();
    if (!conn) {
        return false;
    }
    
    PGresult* result = PQexec(conn, "CREATE EXTENSION IF NOT EXISTS postgis");
    bool success = (PQresultStatus(result) == PGRES_COMMAND_OK);
    
    if (success) {
        postgisInitialized_ = true;
        LOG_INFO("PostGIS extension initialized successfully");
    } else {
        lastError_ = PQresultErrorMessage(result);
        lastErrorCode_ = PQresultStatus(result);
        LOG_ERROR("Failed to initialize PostGIS: " + lastError_);
    }
    
    PQclear(result);
    ReturnConnection(conn);
    
    return success;
}

std::string PostgresqlDatabase::BuildConnectionString() const {
    std::ostringstream oss;
    
    oss << "host=" << config_.pg_host
        << " port=" << config_.pg_port
        << " dbname=" << config_.pg_database
        << " user=" << config_.pg_username
        << " password=" << config_.pg_password
        << " connect_timeout=" << config_.connection_timeout;
    
    return oss.str();
}

PGconn* PostgresqlDatabase::CreateConnection() const {
    std::string connInfo = BuildConnectionString();
    PGconn* conn = PQconnectdb(connInfo.c_str());
    
    if (PQstatus(conn) != CONNECTION_OK) {
        LOG_ERROR("Failed to create PostgreSQL connection: " + 
                  std::string(PQerrorMessage(conn)));
        PQfinish(conn);
        return nullptr;
    }
    
    return conn;
}

void PostgresqlDatabase::DestroyConnection(PGconn* conn) const {
    if (conn) {
        PQfinish(conn);
    }
}

std::string PostgresqlDatabase::BuildSpatialQuery(const std::string& table,
                                                  const BoundingBox& envelope,
                                                  const std::string& geometryColumn) const {
    std::ostringstream oss;
    
    oss << "SELECT * FROM " << table
        << " WHERE ST_Intersects(" << geometryColumn 
        << ", ST_MakeEnvelope(" 
        << envelope.minX << ", " << envelope.minY << ", " 
        << envelope.maxX << ", " << envelope.maxY 
        << ", 4326))"
        << " LIMIT 1000";
    
    return oss.str();
}

// PostgresqlDatabaseRow implementation
PostgresqlDatabaseRow::PostgresqlDatabaseRow(PGresult* result, int rowIndex)
    : result_(result)
    , rowIndex_(rowIndex)
    , columnCount_(PQnfields(result)) {
}

int PostgresqlDatabaseRow::GetColumnCount() const {
    return columnCount_;
}

std::string PostgresqlDatabaseRow::GetColumnName(int index) const {
    if (index < 0 || index >= columnCount_) {
        return "";
    }
    return PQfname(result_, index);
}

int PostgresqlDatabaseRow::GetInt(int index) const {
    if (IsNull(index)) {
        return 0;
    }
    
    const char* value = PQgetvalue(result_, rowIndex_, index);
    return std::stoi(value);
}

double PostgresqlDatabaseRow::GetDouble(int index) const {
    if (IsNull(index)) {
        return 0.0;
    }
    
    const char* value = PQgetvalue(result_, rowIndex_, index);
    return std::stod(value);
}

std::string PostgresqlDatabaseRow::GetString(int index) const {
    if (IsNull(index)) {
        return "";
    }
    
    const char* value = PQgetvalue(result_, rowIndex_, index);
    return value ? std::string(value) : "";
}

std::vector<uint8_t> PostgresqlDatabaseRow::GetBlob(int index) const {
    if (IsNull(index)) {
        return {};
    }
    
    const char* value = PQgetvalue(result_, rowIndex_, index);
    int length = PQgetlength(result_, rowIndex_, index);
    
    std::vector<uint8_t> blob(length);
    std::memcpy(blob.data(), value, length);
    
    return blob;
}

bool PostgresqlDatabaseRow::IsNull(int index) const {
    if (index < 0 || index >= columnCount_) {
        return true;
    }
    
    return PQgetisnull(result_, rowIndex_, index);
}

// PostgresqlResultSet implementation
PostgresqlResultSet::PostgresqlResultSet(PGresult* result)
    : result_(result)
    , currentRow_(-1)
    , totalRows_(PQntuples(result))
    , closed_(false) {
}

PostgresqlResultSet::~PostgresqlResultSet() {
    Close();
}

bool PostgresqlResultSet::Next() {
    if (closed_) {
        return false;
    }
    
    currentRow_++;
    return currentRow_ < totalRows_;
}

std::unique_ptr<DatabaseRow> PostgresqlResultSet::GetCurrentRow() const {
    if (closed_ || currentRow_ < 0 || currentRow_ >= totalRows_) {
        return nullptr;
    }
    
    return std::make_unique<PostgresqlDatabaseRow>(result_, currentRow_);
}

int PostgresqlResultSet::GetRowCount() const {
    return totalRows_;
}

void PostgresqlResultSet::Close() {
    if (!closed_ && result_) {
        PQclear(result_);
        result_ = nullptr;
        closed_ = true;
    }
}

} // namespace database
} // namespace cycle