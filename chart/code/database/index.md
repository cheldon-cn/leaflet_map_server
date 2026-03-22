# Database 模块 - 头文件索引

## 概述

Database 模块提供数据库访问功能，支持 PostGIS (PostgreSQL) 和 SpatiaLite (SQLite) 两种空间数据库。该模块包含连接管理、语句执行、结果集处理、连接池、事务管理等功能。

## 头文件列表

| 文件 | 描述 | 核心类/结构 |
|------|-------------|-------------|
| [export.h](include/ogc/db/export.h) | 导出宏定义 | `OGC_DB_API` |
| [common.h](include/ogc/db/common.h) | 公共枚举和常量 | `DatabaseType`, `DbResult`, `SpatialOperator` |
| [result.h](include/ogc/db/result.h) | 结果封装类 | `Result` |
| [connection.h](include/ogc/db/connection.h) | 数据库连接基类 | `DbConnection`, `ConnectionInfo`, `TableInfo` |
| [statement.h](include/ogc/db/statement.h) | 预编译语句接口 | `DbStatement` |
| [resultset.h](include/ogc/db/resultset.h) | 结果集接口 | `DbResultSet` |
| [postgis_connection.h](include/ogc/db/postgis_connection.h) | PostGIS连接实现 | `PostGISConnection` |
| [sqlite_connection.h](include/ogc/db/sqlite_connection.h) | SpatiaLite连接实现 | `SpatiaLiteConnection` |
| [connection_pool.h](include/ogc/db/connection_pool.h) | 连接池 | `DbConnectionPool`, `PoolConfig` |
| [async_connection.h](include/ogc/db/async_connection.h) | 异步连接 | - |
| [srid_manager.h](include/ogc/db/srid_manager.h) | SRID管理 | - |
| [wkb_converter.h](include/ogc/db/wkb_converter.h) | WKB转换 | - |
| [geojson_converter.h](include/ogc/db/geojson_converter.h) | GeoJSON转换 | - |
| [sqlite_transaction.h](include/ogc/db/sqlite_transaction.h) | SQLite事务 | - |
| [sqlite_spatial.h](include/ogc/db/sqlite_spatial.h) | SQLite空间扩展 | - |
| [postgis_batch.h](include/ogc/db/postgis_batch.h) | PostGIS批量操作 | - |

## 核心枚举和结构

### DatabaseType
数据库类型枚举。

```cpp
enum class DatabaseType : uint8_t {
    kUnknown = 0,
    kPostGIS = 1,
    kSpatiaLite = 2,
    kOracleSpatial = 3,
    kSQLServerSpatial = 4
};
```

### DbResult
数据库操作结果枚举。

```cpp
enum class DbResult : int32_t {
    kSuccess = 0,
    
    kNotConnected = 1,
    kConnectionFailed = 2,
    kConnectionLost = 3,
    kConnectionTimeout = 4,
    kConnectionPoolExhausted = 5,
    kInvalidConnectionString = 6,
    kTimeout = 7,
    kInvalidConnection = 8,
    
    kExecutionError = 100,
    kSqlError = 101,
    kSyntaxError = 102,
    kTableNotFound = 103,
    kColumnNotFound = 104,
    // ...
};
```

### SpatialOperator
空间操作符枚举。

```cpp
enum class SpatialOperator : uint8_t {
    kIntersects = 0,
    kContains = 1,
    kWithin = 2,
    kTouches = 3,
    kCrosses = 4,
    kOverlaps = 5,
    kDisjoint = 6,
    kEquals = 7,
    kDWithin = 8
};
```

### ConnectionInfo
数据库连接信息。

```cpp
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
```

## 核心类详细说明

### Result
数据库操作结果封装类。

```cpp
class Result {
public:
    Result() : m_code(DbResult::kSuccess), m_message(), m_context() {}
    
    explicit Result(DbResult code);
    Result(DbResult code, const std::string& message);
    Result(DbResult code, const std::string& message, const std::string& context);
    
    bool IsSuccess() const noexcept;
    bool IsError() const noexcept;
    DbResult GetCode() const noexcept;
    const std::string& GetMessage() const noexcept;
    const std::string& GetContext() const noexcept;
    
    std::string ToString() const;
    
    static DbResult ToDbResult(int code);
};
```

### DbConnection (抽象基类)
数据库连接基类。

```cpp
class DbConnection {
public:
    virtual ~DbConnection() = default;
    
    virtual Result Connect(const std::string& connectionString) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual bool Ping() const = 0;
    
    virtual Result BeginTransaction() = 0;
    virtual Result CommitTransaction() = 0;
    virtual Result RollbackTransaction() = 0;
    
    virtual DbStatementPtr PrepareStatement(const std::string& sql) = 0;
    virtual DbResultSetPtr ExecuteQuery(const std::string& sql) = 0;
    virtual Result ExecuteNonQuery(const std::string& sql) = 0;
    
    virtual std::vector<TableInfo> GetTables() = 0;
    virtual std::vector<ColumnInfo> GetColumns(const std::string& tableName) = 0;
};
```

### PostGISConnection
PostGIS 数据库连接实现。

```cpp
class PostGISConnection : public DbConnection {
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
    Result CommitTransaction() override;
    Result RollbackTransaction() override;
    
    DbStatementPtr PrepareStatement(const std::string& sql) override;
    DbResultSetPtr ExecuteQuery(const std::string& sql) override;
    Result ExecuteNonQuery(const std::string& sql) override;
    
    std::vector<TableInfo> GetTables() override;
    std::vector<ColumnInfo> GetColumns(const std::string& tableName) override;
};
```

### SpatiaLiteConnection
SpatiaLite 数据库连接实现。

```cpp
class SpatiaLiteConnection : public DbConnection {
public:
    static SpatiaLiteConnectionPtr Create();
    
    explicit SpatiaLiteConnection();
    ~SpatiaLiteConnection() override;
    
    Result Connect(const std::string& connectionString) override;
    Result Connect(const SpatiaLiteOptions& options);
    void Disconnect() override;
    
    Result InitSpatialMetaData();
    Result CreateSpatialIndex(const std::string& tableName, const std::string& geomColumn);
};
```

### DbStatement
预编译语句接口。

```cpp
class DbStatement {
public:
    virtual ~DbStatement() = default;
    
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetSql() const = 0;
    
    virtual Result BindInt(int paramIndex, int32_t value) = 0;
    virtual Result BindInt64(int paramIndex, int64_t value) = 0;
    virtual Result BindDouble(int paramIndex, double value) = 0;
    virtual Result BindString(int paramIndex, const std::string& value) = 0;
    virtual Result BindBool(int paramIndex, bool value) = 0;
    virtual Result BindNull(int paramIndex) = 0;
    virtual Result BindBlob(int paramIndex, const std::vector<uint8_t>& data) = 0;
    virtual Result BindGeometry(int paramIndex, const Geometry* geometry) = 0;
    virtual Result BindEnvelope(int paramIndex, const Envelope& envelope) = 0;
    
    virtual DbResultSetPtr ExecuteQuery() = 0;
    virtual Result ExecuteNonQuery() = 0;
    
    virtual void Close() = 0;
};
```

### DbResultSet
结果集接口。

```cpp
class DbResultSet {
public:
    virtual ~DbResultSet() = default;
    
    virtual bool Next() = 0;
    virtual bool IsEOF() const = 0;
    virtual void Reset() = 0;
    
    virtual int GetColumnCount() const = 0;
    virtual std::string GetColumnName(int columnIndex) const = 0;
    virtual ColumnType GetColumnType(int columnIndex) const = 0;
    
    virtual int32_t GetInt(int columnIndex) const = 0;
    virtual int64_t GetInt64(int columnIndex) const = 0;
    virtual double GetDouble(int columnIndex) const = 0;
    virtual std::string GetString(int columnIndex) const = 0;
    virtual std::vector<uint8_t> GetBlob(int columnIndex) const = 0;
    
    virtual GeometryPtr GetGeometry(int columnIndex) const = 0;
    virtual GeometryPtr GetGeometry(const std::string& columnName) const = 0;
};
```

### DbConnectionPool
数据库连接池。

```cpp
class DbConnectionPool {
public:
    DbConnectionPool();
    ~DbConnectionPool();
    
    Result Initialize(const std::string& connectionString, const PoolConfig& config);
    Result Initialize(DatabaseType type, const std::string& connectionString, const PoolConfig& config);
    
    DbConnectionPtr AcquireConnection();
    DbConnectionPtr AcquireConnection(int timeoutMs);
    void ReleaseConnection(DbConnection* connection);
    
    void Shutdown();
    PoolStatus GetStatus() const;
};
```

## 连接池配置

### PoolConfig
连接池配置结构。

```cpp
struct PoolConfig {
    int minConnections = 1;
    int maxConnections = 10;
    int maxIdleTimeMs = 30000;
    int maxLifetimeMs = 3600000;
    int acquireTimeoutMs = 5000;
    int validationIntervalMs = 60000;
    bool autoShrink = true;
    int shrinkIntervalMs = 300000;
    float shrinkThreshold = 0.3f;
};
```

## 使用示例

### 连接 PostGIS

```cpp
PostGISConnectionPtr conn = PostGISConnection::Create();
PostGISOptions options;
options.host = "localhost";
options.port = 5432;
options.database = "testdb";
options.user = "postgres";
options.password = "password";

Result result = conn->Connect(options);
if (result.IsSuccess()) {
    auto rs = conn->ExecuteQuery("SELECT * FROM cities WHERE population > 100000");
    while (rs->Next()) {
        std::string name = rs->GetString("name");
        int pop = rs->GetInt("population");
        // ...
    }
}
```

### 使用连接池

```cpp
PoolConfig config;
config.minConnections = 2;
config.maxConnections = 10;

DbConnectionPool pool;
pool.Initialize(DatabaseType::kPostGIS, connectionString, config);

auto conn = pool.AcquireConnection();
auto rs = conn->ExecuteQuery("SELECT * FROM cities");

pool.ReleaseConnection(conn.get());
```

### 执行空间查询

```cpp
auto rs = conn->ExecuteQuery(
    "SELECT * FROM cities WHERE ST_Intersects(geom, ST_MakeEnvelope(0, 0, 100, 100, 4326))"
);
```

## 依赖关系

```
connection.h
    ├── common.h
    ├── result.h
    └── statement.h

postgis_connection.h
    ├── common.h
    ├── connection.h
    ├── result.h
    ├── statement.h
    └── resultset.h

sqlite_connection.h
    ├── common.h
    ├── connection.h
    ├── result.h
    ├── statement.h
    └── resultset.h

connection_pool.h
    ├── connection.h
    ├── postgis_connection.h
    └── sqlite_connection.h

resultset.h
    └── result.h
```
