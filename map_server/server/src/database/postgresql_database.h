#ifndef CYCLE_DATABASE_POSTGRESQL_DATABASE_H
#define CYCLE_DATABASE_POSTGRESQL_DATABASE_H

#include "idatabase.h"

#ifdef HAVE_POSTGRESQL
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <vector>

namespace cycle {
namespace database {
class PostgresqlDatabase : public IDatabase {
public:
    explicit PostgresqlDatabase(const DatabaseConfig& config);
    ~PostgresqlDatabase() override;
    
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
    
    std::unique_ptr<ResultSet> QuerySpatial(const std::string& table,
                                           const BoundingBox& envelope,
                                           const std::string& geometryColumn = "geometry") override;
    
    std::string GetDatabaseType() const override;
    
private:
    PGconn* GetConnection();
    void ReturnConnection(PGconn* conn);
    bool InitPostGIS();
    std::string BuildConnectionString() const;
    
    PGconn* CreateConnection() const;
    void DestroyConnection(PGconn* conn) const;
    
    std::string BuildSpatialQuery(const std::string& table,
                                 const BoundingBox& envelope,
                                 const std::string& geometryColumn) const;
    
    DatabaseConfig config_;
    std::vector<PGconn*> connectionPool_;
    mutable std::mutex poolMutex_;
    bool postgisInitialized_;
    
    std::string lastError_;
    int lastErrorCode_;
};

class PostgresqlDatabaseRow : public DatabaseRow {
public:
    explicit PostgresqlDatabaseRow(PGresult* result, int rowIndex);
    ~PostgresqlDatabaseRow() override = default;
    
    int GetColumnCount() const override;
    std::string GetColumnName(int index) const override;
    
    int GetInt(int index) const override;
    double GetDouble(int index) const override;
    std::string GetString(int index) const override;
    std::vector<uint8_t> GetBlob(int index) const override;
    bool IsNull(int index) const override;
    
private:
    PGresult* result_;
    int rowIndex_;
    int columnCount_;
};

class PostgresqlResultSet : public ResultSet {
public:
    explicit PostgresqlResultSet(PGresult* result);
    ~PostgresqlResultSet() override;
    
    bool Next() override;
    std::unique_ptr<DatabaseRow> GetCurrentRow() const override;
    int GetRowCount() const override;
    void Close() override;
    
private:
    PGresult* result_;
    int currentRow_;
    int totalRows_;
    bool closed_;
};

} // namespace database
} // namespace cycle

#endif // HAVE_POSTGRESQL

#endif // CYCLE_DATABASE_POSTGRESQL_DATABASE_H
