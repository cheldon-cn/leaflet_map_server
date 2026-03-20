#pragma once

#include "common.h"
#include "result.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace ogc {
    class Geometry;
    class Envelope;
}

namespace ogc {
namespace db {

class DbStatement;
class DbResultSet;

using DbStatementPtr = std::unique_ptr<DbStatement>;
using DbResultSetPtr = std::unique_ptr<DbResultSet>;
using GeometryPtr = std::unique_ptr<Geometry>;

struct ConnectionInfo {
    std::string host;
    int port = 0;
    std::string database;
    std::string username;
    std::string password;
    std::string filePath;
    int srid = 4326;
    int connectionTimeout = DEFAULT_CONNECTION_TIMEOUT_MS;
    int queryTimeout = DEFAULT_QUERY_TIMEOUT_MS;
    bool isInMemory = false;
    std::map<std::string, std::string> options;
};

struct TableInfo {
    std::string name;
    std::string geomColumn;
    std::string geomType;
    int srid = 0;
    int coordDimension = 2;
    bool hasSpatialIndex = false;
};

struct ColumnInfo {
    std::string name;
    ColumnType type;
    bool isNullable = true;
    bool isPrimaryKey = false;
    std::string defaultValue;
};

class OGC_DB_API DbConnection {
public:
    virtual ~DbConnection() = default;
    
    DbConnection(const DbConnection&) = delete;
    DbConnection& operator=(const DbConnection&) = delete;
    
    DbConnection(DbConnection&&) noexcept = default;
    DbConnection& operator=(DbConnection&&) noexcept = default;

    virtual Result Connect(const std::string& connectionString) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual bool Ping() const = 0;
    
    virtual Result BeginTransaction() = 0;
    virtual Result Commit() = 0;
    virtual Result Rollback() = 0;
    virtual bool InTransaction() const = 0;
    
    virtual Result Execute(const std::string& sql) = 0;
    virtual Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) = 0;
    virtual Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) = 0;
    
    virtual Result InsertGeometry(const std::string& table,
                                   const std::string& geomColumn,
                                   const Geometry* geometry,
                                   const std::map<std::string, std::string>& attributes,
                                   int64_t& outId) = 0;
    
    virtual Result InsertGeometries(const std::string& table,
                                     const std::string& geomColumn,
                                     const std::vector<const Geometry*>& geometries,
                                     const std::vector<std::map<std::string, std::string>>& attributes,
                                     std::vector<int64_t>& outIds) = 0;
    
    virtual Result SelectGeometries(const std::string& table,
                                     const std::string& geomColumn,
                                     const std::string& whereClause,
                                     std::vector<GeometryPtr>& geometries) = 0;
    
    virtual Result SpatialQuery(const std::string& table,
                                 const std::string& geomColumn,
                                 SpatialOperator op,
                                 const Geometry* queryGeom,
                                 std::vector<GeometryPtr>& results) = 0;
    
    virtual Result SpatialQueryWithEnvelope(const std::string& table,
                                             const std::string& geomColumn,
                                             SpatialOperator op,
                                             const Geometry* queryGeom,
                                             const Envelope& filter,
                                             std::vector<GeometryPtr>& results) = 0;
    
    virtual Result UpdateGeometry(const std::string& table,
                                   const std::string& geomColumn,
                                   int64_t id,
                                   const Geometry* geometry) = 0;
    
    virtual Result DeleteGeometry(const std::string& table, int64_t id) = 0;
    
    virtual Result CreateSpatialTable(const std::string& tableName,
                                       const std::string& geomColumn,
                                       int geomType,
                                       int srid,
                                       int coordDimension) = 0;
    
    virtual Result CreateSpatialIndex(const std::string& tableName,
                                       const std::string& geomColumn) = 0;
    
    virtual Result DropSpatialIndex(const std::string& tableName,
                                     const std::string& geomColumn) = 0;
    
    virtual bool SpatialTableExists(const std::string& tableName) const = 0;
    
    virtual Result GetTableInfo(const std::string& tableName, TableInfo& info) const = 0;
    
    virtual Result GetColumns(const std::string& tableName, std::vector<ColumnInfo>& columns) const = 0;
    
    virtual DatabaseType GetType() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual int64_t GetLastInsertId() const = 0;
    virtual int64_t GetRowsAffected() const = 0;
    
    virtual Result SetIsolationLevel(TransactionIsolation level) = 0;
    virtual TransactionIsolation GetIsolationLevel() const = 0;
    
    virtual const ConnectionInfo& GetConnectionInfo() const = 0;
    
    virtual std::string EscapeString(const std::string& value) const = 0;
    
    static ConnectionInfo ParseConnectionString(const std::string& connectionString);

protected:
    DbConnection() = default;
};

using DbConnectionPtr = std::unique_ptr<DbConnection>;
using DbConnectionSharedPtr = std::shared_ptr<DbConnection>;

}
}
