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
    
    Result Execute(const std::string& sql) override;
    Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) override;
    Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) override;
    
    Result InsertGeometry(const std::string& table,
                         const std::string& geomColumn,
                         const Geometry* geometry,
                         const std::map<std::string, std::string>& attributes,
                         int64_t& outId) override;
    
    std::string GetLastError() const;
    PGconn* GetRawConnection() { return m_conn; }

private:
    PGconn* m_conn;
    bool m_isTransaction;
    std::string m_lastError;
    
    Result CheckConnection();
    Result ExecuteInternal(const std::string& sql, PGresult*& result);
};

class OGC_DB_API PostGISStatement : public DbStatement {
public:
    explicit PostGISStatement(PGconn* conn, const std::string& name);
    ~PostGISStatement() override;
    
    void SetConnection(PGconn* conn) { m_conn = conn; }
    
    Result BindInt(int paramIndex, int32_t value) override;
    Result BindInt64(int paramIndex, int64_t value) override;
    Result BindDouble(int paramIndex, double value) override;
    Result BindString(int paramIndex, const std::string& value) override;
    Result BindGeometry(int paramIndex, const Geometry* geometry) override;
    
    Result Execute() override;
    Result ExecuteQuery(DbResultSetPtr& result) override;
    
    Result Reset() override;
    const std::string& GetName() const { return m_name; }

private:
    PGconn* m_conn;
    std::string m_name;
    std::string m_sql;
    std::vector<std::vector<char>> m_paramValues;
    std::vector<int> m_paramLengths;
    std::vector<Oid> m_paramTypes;
    std::vector<Oid> m_paramFormats;
    
    Result Prepare();
};

class OGC_DB_API PostGISResultSet : public DbResultSet {
public:
    explicit PostGISResultSet(PGresult* result);
    ~PostGISResultSet() override;
    
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
    PGresult* m_result;
    int m_currentRow;
    int m_numRows;
    int m_numColumns;
    
    std::vector<std::string> m_columnNames;
    std::vector<Oid> m_columnTypes;
    
    void InitializeColumns();
    ColumnType ConvertPgType(Oid pgType) const;
};

}
}
