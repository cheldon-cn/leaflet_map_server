#ifndef CYCLE_DATABASE_SQLITE_DATABASE_H
#define CYCLE_DATABASE_SQLITE_DATABASE_H

#include "idatabase.h"
#include <sqlite3.h>
#include <mutex>

namespace cycle {
namespace database {

class SqliteDatabase : public IDatabase {
public:
    explicit SqliteDatabase(const std::string& dbPath);
    ~SqliteDatabase() override;
    
    bool Open() override;
    void Close() override;
    bool IsOpen() const override;
    
    bool Execute(const std::string& sql) override;
    bool Execute(const std::string& sql, const std::vector<SqlParameter>& params) override;
    
    std::unique_ptr<ResultSet> Query(const std::string& sql) override;
    std::unique_ptr<ResultSet> Query(const std::string& sql, 
                                     const std::vector<SqlParameter>& params) override;
    
    bool BeginTransaction() override;
    bool CommitTransaction() override;
    bool RollbackTransaction() override;
    
    std::string GetLastError() const override;
    int GetLastErrorCode() const override;
    
    std::unique_ptr<ResultSet> QuerySpatial(
        const std::string& table,
        const BoundingBox& envelope,
        const std::string& geometryColumn = "geometry") override;
    
    std::string GetDatabaseType() const override { return "sqlite3"; }
    
private:
    bool InitSpatialite();
    bool LoadSpatialiteExtension();
    
    sqlite3* db_;
    std::string dbPath_;
    bool hasSpatialite_;
    std::string lastError_;
    int lastErrorCode_;
    mutable std::mutex mutex_;
};

class SqliteResultSet : public ResultSet {
public:
    explicit SqliteResultSet(sqlite3_stmt* stmt);
    ~SqliteResultSet() override;
    
    bool Next() override;
    std::unique_ptr<DatabaseRow> GetCurrentRow() const override;
    int GetRowCount() const override;
    void Close() override;
    
private:
    sqlite3_stmt* stmt_;
    bool hasData_;
    int rowCount_;
};

class SqliteDatabaseRow : public DatabaseRow {
public:
    explicit SqliteDatabaseRow(sqlite3_stmt* stmt);
    
    int GetColumnCount() const override;
    std::string GetColumnName(int index) const override;
    
    int GetInt(int index) const override;
    double GetDouble(int index) const override;
    std::string GetString(int index) const override;
    std::vector<uint8_t> GetBlob(int index) const override;
    bool IsNull(int index) const override;
    
private:
    sqlite3_stmt* stmt_;
};

} // namespace database
} // namespace cycle

#endif // CYCLE_DATABASE_SQLITE_DATABASE_H
