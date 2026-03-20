#pragma once

#include "common.h"
#include "connection.h"
#include "result.h"
#include "statement.h"
#include "resultset.h"
#include "wkb_converter.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sqlite3.h>

namespace ogc {
namespace db {

class SpatiaLiteConnection;
class SpatiaLiteStatement;
class SpatiaLiteResultSet;

using SpatiaLiteConnectionPtr = std::unique_ptr<SpatiaLiteConnection>;
using SpatiaLiteStatementPtr = std::unique_ptr<SpatiaLiteStatement>;
using SpatiaLiteResultSetPtr = std::unique_ptr<SpatiaLiteResultSet>;

struct SpatiaLiteOptions {
    std::string databasePath;
    bool readOnly = false;
    bool createIfNotExist = false;
    std::string cacheSize = "2000";
    int connectionTimeout = 5000;
    
    std::string ToConnectionString() const;
};

class OGC_DB_API SpatiaLiteConnection : public DbConnection {
public:
    static SpatiaLiteConnectionPtr Create();
    
    explicit SpatiaLiteConnection();
    ~SpatiaLiteConnection() override;
    
    Result Connect(const std::string& connectionString) override;
    Result Connect(const SpatiaLiteOptions& options);
    void Disconnect() override;
    bool IsConnected() const override;
    bool Ping() const override;
    
    Result BeginTransaction() override;
    Result Commit() override;
    Result Rollback() override;
    
    Result Execute(const std::string& sql) override;
    Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) override;
    Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) override;
    
    Result InsertGeometry(const std::string& table,
                         const std::string& geomColumn,
                         const Geometry* geometry,
                         const std::map<std::string, std::string>& attributes,
                         int64_t& outId) override;
    
    Result InitializeSpatialMetaData();
    Result CreateSpatialIndex(const std::string& table, const std::string& geomColumn);
    
    std::string GetLastError() const;
    sqlite3* GetRawConnection() { return m_db; }

private:
    sqlite3* m_db;
    bool m_isTransaction;
    std::string m_lastError;
    bool m_spatialInitialized;
    
    Result CheckConnection();
    Result ExecuteInternal(const std::string& sql, sqlite3_stmt*& stmt, bool finalize = true);
    int GetLastInsertRowId() const;
};

class OGC_DB_API SpatiaLiteStatement : public DbStatement {
public:
    explicit SpatiaLiteStatement(sqlite3* db, const std::string& sql);
    ~SpatiaLiteStatement() override;
    
    void SetConnection(sqlite3* db) { m_db = db; }
    
    Result BindInt(int paramIndex, int32_t value) override;
    Result BindInt64(int paramIndex, int64_t value) override;
    Result BindDouble(int paramIndex, double value) override;
    Result BindString(int paramIndex, const std::string& value) override;
    Result BindGeometry(int paramIndex, const Geometry* geometry) override;
    
    Result Execute() override;
    Result ExecuteQuery(DbResultSetPtr& result) override;
    
    Result Reset() override;
    const std::string& GetSQL() const { return m_sql; }

private:
    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    std::string m_sql;
    std::vector<std::string> m_bindStrings;
    
    Result Prepare();
    Result Finalize();
};

class OGC_DB_API SpatiaLiteResultSet : public DbResultSet {
public:
    explicit SpatiaLiteResultSet(sqlite3_stmt* stmt);
    ~SpatiaLiteResultSet() override;
    
    bool Next() override;
    bool IsEOF() const override;
    
    int32_t GetInt(int columnIndex) const override;
    int64_t GetInt64(int columnIndex) const override;
    double GetDouble(int columnIndex) const override;
    std::string GetString(int columnIndex) const override;
    GeometryPtr GetGeometry(int columnIndex) const override;
    
    int GetColumnCount() const override;
    std::string GetColumnName(int columnIndex) const override;
    ColumnType GetColumnType(int columnIndex) const override;
    
    bool HasGeometry(int columnIndex) const;

private:
    sqlite3_stmt* m_stmt;
    int m_columnCount;
    
    std::vector<std::string> m_columnNames;
    std::vector<int> m_columnTypes;
    
    void InitializeColumns();
    ColumnType ConvertSqliteType(int sqliteType) const;
};

}
}
