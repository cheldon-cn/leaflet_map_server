#include "sqlite_database.h"
#include "../utils/logger.h"
#include <sstream>

namespace cycle {
namespace database {

SqliteDatabase::SqliteDatabase(const std::string& dbPath)
    : db_(nullptr)
    , dbPath_(dbPath)
    , hasSpatialite_(false)
    , lastErrorCode_(0) {
}

SqliteDatabase::~SqliteDatabase() {
    Close();
}

bool SqliteDatabase::Open() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (db_) {
            return true;
        }

        int result = sqlite3_open(dbPath_.c_str(), &db_);
        if (result != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            lastErrorCode_ = result;
            LOG_ERROR("Failed to open SQLite database: " + lastError_);
            sqlite3_close(db_);
            db_ = nullptr;
            return false;
        }
    }
    
    if (!InitSpatialite()) {
        LOG_WARN("Failed to initialize SpatiaLite extension");
    }
    
    LOG_INFO("SQLite database opened successfully: " + dbPath_);
    return true;
}

void SqliteDatabase::Close() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
        LOG_INFO("SQLite database closed");
    }
}

bool SqliteDatabase::IsOpen() const {
    return db_ != nullptr;
}

bool SqliteDatabase::Execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!db_) {
        lastError_ = "Database not open";
        return false;
    }
    
    char* errMsg = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (result != SQLITE_OK) {
        lastError_ = errMsg ? errMsg : "Unknown error";
        lastErrorCode_ = result;
        LOG_ERROR("SQL execution failed: " + lastError_);
        if (errMsg) {
            sqlite3_free(errMsg);
        }
        return false;
    }
    
    return true;
}

bool SqliteDatabase::Execute(const std::string& sql, const std::vector<SqlParameter>& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!db_) {
        lastError_ = "Database not open";
        return false;
    }
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        lastErrorCode_ = result;
        LOG_ERROR("Failed to prepare SQL: " + lastError_);
        return false;
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        int paramIndex = static_cast<int>(i + 1);
        const SqlParameter& param = params[i];
        
        switch (param.type) {
            case SqlParameter::INTEGER:
                sqlite3_bind_int(stmt, paramIndex, param.int_value);
                break;
            case SqlParameter::REAL:
                sqlite3_bind_double(stmt, paramIndex, param.real_value);
                break;
            case SqlParameter::TEXT:
                sqlite3_bind_text(stmt, paramIndex, param.text_value, -1, SQLITE_TRANSIENT);
                break;
            case SqlParameter::BLOB:
                sqlite3_bind_blob(stmt, paramIndex, param.blob_data, 
                                 static_cast<int>(param.blob_size), SQLITE_TRANSIENT);
                break;
            case SqlParameter::NULL_VALUE:
                sqlite3_bind_null(stmt, paramIndex);
                break;
        }
    }
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE && result != SQLITE_ROW) {
        lastError_ = sqlite3_errmsg(db_);
        lastErrorCode_ = result;
        LOG_ERROR("SQL execution failed: " + lastError_);
        return false;
    }
    
    return true;
}

std::unique_ptr<ResultSet> SqliteDatabase::Query(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!db_) {
        lastError_ = "Database not open";
        return nullptr;
    }
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        lastErrorCode_ = result;
        LOG_ERROR("Failed to prepare query: " + lastError_);
        return nullptr;
    }
    
    return std::unique_ptr<ResultSet>(new SqliteResultSet(stmt));
}

std::unique_ptr<ResultSet> SqliteDatabase::Query(const std::string& sql, 
                                                  const std::vector<SqlParameter>& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!db_) {
        lastError_ = "Database not open";
        return nullptr;
    }
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        lastErrorCode_ = result;
        LOG_ERROR("Failed to prepare query: " + lastError_);
        return nullptr;
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        int paramIndex = static_cast<int>(i + 1);
        const SqlParameter& param = params[i];
        
        switch (param.type) {
            case SqlParameter::INTEGER:
                sqlite3_bind_int(stmt, paramIndex, param.int_value);
                break;
            case SqlParameter::REAL:
                sqlite3_bind_double(stmt, paramIndex, param.real_value);
                break;
            case SqlParameter::TEXT:
                sqlite3_bind_text(stmt, paramIndex, param.text_value, -1, SQLITE_TRANSIENT);
                break;
            case SqlParameter::BLOB:
                sqlite3_bind_blob(stmt, paramIndex, param.blob_data, 
                                 static_cast<int>(param.blob_size), SQLITE_TRANSIENT);
                break;
            case SqlParameter::NULL_VALUE:
                sqlite3_bind_null(stmt, paramIndex);
                break;
        }
    }
    
    return std::unique_ptr<ResultSet>(new SqliteResultSet(stmt));
}

bool SqliteDatabase::BeginTransaction() {
    return Execute("BEGIN TRANSACTION");
}

bool SqliteDatabase::CommitTransaction() {
    return Execute("COMMIT");
}

bool SqliteDatabase::RollbackTransaction() {
    return Execute("ROLLBACK");
}

std::string SqliteDatabase::GetLastError() const {
    return lastError_;
}

int SqliteDatabase::GetLastErrorCode() const {
    return lastErrorCode_;
}

std::unique_ptr<ResultSet> SqliteDatabase::QuerySpatial(
    const std::string& table,
    const BoundingBox& envelope,
    const std::string& geometryColumn) {
    
    std::ostringstream sql;
    sql << "SELECT * FROM " << table
        << " WHERE " << geometryColumn 
        << " && BuildMbr(?, ?, ?, ?, 4326)";
    
    std::vector<SqlParameter> params = {
        SqlParameter::Real(envelope.minX),
        SqlParameter::Real(envelope.minY),
        SqlParameter::Real(envelope.maxX),
        SqlParameter::Real(envelope.maxY)
    };
    
    return Query(sql.str(), params);
}

bool SqliteDatabase::InitSpatialite() {
    if (!LoadSpatialiteExtension()) {
        return false;
    }
    
    if (!hasSpatialite_) {
        return true;
    }
    
    return Execute("SELECT InitSpatialMetaData(1)");
}

bool SqliteDatabase::LoadSpatialiteExtension() {
    if (!db_) {
        return false;
    }
    
    sqlite3_enable_load_extension(db_, 1);
    
    const char* extensionPaths[] = {
        "mod_spatialite",
        "mod_spatialite.so",
        "mod_spatialite.dylib",
        "spatialite",
        nullptr
    };
    
    for (int i = 0; extensionPaths[i] != nullptr; ++i) {
        char* errMsg = nullptr;
        int result = sqlite3_load_extension(db_, extensionPaths[i], nullptr, &errMsg);
        
        if (result == SQLITE_OK) {
            hasSpatialite_ = true;
            LOG_INFO("SpatiaLite extension loaded successfully");
            sqlite3_enable_load_extension(db_, 0);
            return true;
        }
        
        if (errMsg) {
            sqlite3_free(errMsg);
        }
    }
    
    sqlite3_enable_load_extension(db_, 0);
    LOG_WARN("Failed to load SpatiaLite extension");
    return false;
}

SqliteResultSet::SqliteResultSet(sqlite3_stmt* stmt)
    : stmt_(stmt)
    , hasData_(false)
    , rowCount_(0) {
}

SqliteResultSet::~SqliteResultSet() {
    Close();
}

bool SqliteResultSet::Next() {
    if (!stmt_) {
        return false;
    }
    
    int result = sqlite3_step(stmt_);
    
    if (result == SQLITE_ROW) {
        hasData_ = true;
        rowCount_++;
        return true;
    }
    
    hasData_ = false;
    return false;
}

std::unique_ptr<DatabaseRow> SqliteResultSet::GetCurrentRow() const {
    if (!hasData_ || !stmt_) {
        return nullptr;
    }
    
    return std::unique_ptr<DatabaseRow>(new SqliteDatabaseRow(stmt_));
}

int SqliteResultSet::GetRowCount() const {
    return rowCount_;
}

void SqliteResultSet::Close() {
    if (stmt_) {
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
}

SqliteDatabaseRow::SqliteDatabaseRow(sqlite3_stmt* stmt)
    : stmt_(stmt) {
}

int SqliteDatabaseRow::GetColumnCount() const {
    return sqlite3_column_count(stmt_);
}

std::string SqliteDatabaseRow::GetColumnName(int index) const {
    const char* name = sqlite3_column_name(stmt_, index);
    return name ? name : "";
}

int SqliteDatabaseRow::GetInt(int index) const {
    return sqlite3_column_int(stmt_, index);
}

double SqliteDatabaseRow::GetDouble(int index) const {
    return sqlite3_column_double(stmt_, index);
}

std::string SqliteDatabaseRow::GetString(int index) const {
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, index));
    return text ? text : "";
}

std::vector<uint8_t> SqliteDatabaseRow::GetBlob(int index) const {
    const void* blob = sqlite3_column_blob(stmt_, index);
    int size = sqlite3_column_bytes(stmt_, index);
    
    if (blob && size > 0) {
        const uint8_t* data = static_cast<const uint8_t*>(blob);
        return std::vector<uint8_t>(data, data + size);
    }
    
    return std::vector<uint8_t>();
}

bool SqliteDatabaseRow::IsNull(int index) const {
    return sqlite3_column_type(stmt_, index) == SQLITE_NULL;
}

} // namespace database
} // namespace cycle
