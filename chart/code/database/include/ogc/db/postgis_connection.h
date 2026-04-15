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
#include <libpq-fe.h>

namespace ogc {
    class Envelope;
namespace db {

class PostGISConnection;
class PostGISStatement;
class PostGISResultSet;

using PostGISConnectionPtr = std::unique_ptr<PostGISConnection>;
using PostGISStatementPtr = std::unique_ptr<PostGISStatement>;
using PostGISResultSetPtr = std::unique_ptr<PostGISResultSet>;

constexpr int kDefaultPostGISPort = 5432;

struct PostGISOptions {
    std::string host = "localhost";
    int port = kDefaultPostGISPort;
    std::string database;
    std::string user;
    std::string password;
    std::string sslmode = "disable";
    int connectionTimeout = 10;
    
    std::string ToConnectionString() const;
};

class OGC_DB_API PostGISConnection : public DbConnection {
public:
    static PostGISConnectionPtr Create();
    
    explicit PostGISConnection();
    ~PostGISConnection() override;
    
    Result Connect(const std::string& connectionString) override;
    Result Connect(const PostGISOptions& options);
    void Disconnect() override;
    bool IsConnected() const override;
    bool Ping() const override;
    
    Result BeginTransaction() override;
    Result Commit() override;
    Result Rollback() override;
    bool InTransaction() const override;
    
    Result Execute(const std::string& sql) override;
    Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) override;
    Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) override;
    
    Result InsertGeometry(const std::string& table,
                         const std::string& geomColumn,
                         const Geometry* geometry,
                         const std::map<std::string, std::string>& attributes,
                         int64_t& outId) override;
    
    Result InsertGeometries(const std::string& table,
                            const std::string& geomColumn,
                            const std::vector<const Geometry*>& geometries,
                            const std::vector<std::map<std::string, std::string>>& attributes,
                            std::vector<int64_t>& outIds) override;
    
    Result SelectGeometries(const std::string& table,
                            const std::string& geomColumn,
                            const std::string& whereClause,
                            std::vector<GeometryPtr>& geometries) override;
    
    Result SpatialQuery(const std::string& table,
                        const std::string& geomColumn,
                        SpatialOperator op,
                        const Geometry* queryGeom,
                        std::vector<GeometryPtr>& results) override;
    
    Result SpatialQueryWithEnvelope(const std::string& table,
                                    const std::string& geomColumn,
                                    SpatialOperator op,
                                    const Geometry* queryGeom,
                                    const ::ogc::Envelope& filter,
                                    std::vector<GeometryPtr>& results) override;
    
    Result UpdateGeometry(const std::string& table,
                          const std::string& geomColumn,
                          int64_t id,
                          const Geometry* geometry) override;
    
    Result DeleteGeometry(const std::string& table, int64_t id) override;
    
    Result CreateSpatialTable(const std::string& tableName,
                              const std::string& geomColumn,
                              int geomType,
                              int srid,
                              int coordDimension) override;
    
    Result CreateSpatialIndex(const std::string& tableName,
                              const std::string& geomColumn) override;
    
    Result DropSpatialIndex(const std::string& tableName,
                            const std::string& geomColumn) override;
    
    bool SpatialTableExists(const std::string& tableName) const override;
    
    Result GetTableInfo(const std::string& tableName, TableInfo& info) const override;
    
    Result GetColumns(const std::string& tableName, std::vector<ColumnInfo>& columns) const override;
    
    DatabaseType GetType() const override;
    std::string GetVersion() const override;
    int64_t GetLastInsertId() const override;
    int64_t GetRowsAffected() const override;
    
    Result SetIsolationLevel(TransactionIsolation level) override;
    TransactionIsolation GetIsolationLevel() const override;
    
    const ConnectionInfo& GetConnectionInfo() const override;
    
    std::string EscapeString(const std::string& value) const override;
    
    std::string GetLastError() const;
    PGconn* GetRawConnection();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    Result CheckConnection();
    Result ExecuteInternal(const std::string& sql, PGresult*& result);
};

class OGC_DB_API PostGISStatement : public DbStatement {
public:
    explicit PostGISStatement(PGconn* conn, const std::string& name);
    ~PostGISStatement() override;
    
    void SetConnection(PGconn* conn);
    
    const std::string& GetName() const override;
    const std::string& GetSql() const override;
    
    Result BindInt(int paramIndex, int32_t value) override;
    Result BindInt64(int paramIndex, int64_t value) override;
    Result BindDouble(int paramIndex, double value) override;
    Result BindString(int paramIndex, const std::string& value) override;
    Result BindBool(int paramIndex, bool value) override;
    Result BindNull(int paramIndex) override;
    Result BindBlob(int paramIndex, const std::vector<uint8_t>& data) override;
    Result BindBlob(int paramIndex, const uint8_t* data, size_t size) override;
    Result BindGeometry(int paramIndex, const Geometry* geometry) override;
    Result BindEnvelope(int paramIndex, const ::ogc::Envelope& envelope) override;
    
    Result BindInt(const std::string& paramName, int32_t value) override;
    Result BindInt64(const std::string& paramName, int64_t value) override;
    Result BindDouble(const std::string& paramName, double value) override;
    Result BindString(const std::string& paramName, const std::string& value) override;
    Result BindBool(const std::string& paramName, bool value) override;
    Result BindNull(const std::string& paramName) override;
    Result BindBlob(const std::string& paramName, const std::vector<uint8_t>& data) override;
    Result BindGeometry(const std::string& paramName, const Geometry* geometry) override;
    
    Result Execute() override;
    Result ExecuteQuery(DbResultSetPtr& result) override;
    
    Result Reset() override;
    Result ClearBindings() override;
    
    int64_t GetLastInsertId() const override;
    int64_t GetRowsAffected() const override;
    
    int GetParameterCount() const override;
    int GetParameterIndex(const std::string& paramName) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    Result Prepare();
};

class OGC_DB_API PostGISResultSet : public DbResultSet {
public:
    explicit PostGISResultSet(PGresult* result);
    ~PostGISResultSet() override;
    
    bool Next() override;
    bool IsEOF() const override;
    void Reset() override;
    
    int GetColumnCount() const override;
    std::string GetColumnName(int columnIndex) const override;
    ColumnType GetColumnType(int columnIndex) const override;
    int GetColumnIndex(const std::string& columnName) const override;
    
    bool IsNull(int columnIndex) const override;
    bool IsNull(const std::string& columnName) const override;
    
    int32_t GetInt(int columnIndex) const override;
    int32_t GetInt(const std::string& columnName) const override;
    int64_t GetInt64(int columnIndex) const override;
    int64_t GetInt64(const std::string& columnName) const override;
    double GetDouble(int columnIndex) const override;
    double GetDouble(const std::string& columnName) const override;
    std::string GetString(int columnIndex) const override;
    std::string GetString(const std::string& columnName) const override;
    bool GetBool(int columnIndex) const override;
    bool GetBool(const std::string& columnName) const override;
    std::vector<uint8_t> GetBlob(int columnIndex) const override;
    std::vector<uint8_t> GetBlob(const std::string& columnName) const override;
    
    GeometryPtr GetGeometry(int columnIndex) const override;
    GeometryPtr GetGeometry(const std::string& columnName) const override;
    
    int64_t GetRowCount() const override;
    int64_t GetCurrentRow() const override;
    
    Result GetLastError() const override;
    
    bool HasGeometry(int columnIndex) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    void InitializeColumns();
    ColumnType ConvertPgType(unsigned int pgType) const;
};

}
}
