# Database Module - Header Index

## 模块描述

Database模块提供空间数据库访问抽象层，支持PostGIS和SpatiaLite两种空间数据库，提供连接池管理、事务处理、空间查询等功能。

## 核心特性

- 支持PostGIS和SpatiaLite空间数据库
- 连接池管理，支持自动扩缩容
- 完整的事务支持（BEGIN/COMMIT/ROLLBACK）
- WKB/WKT/GeoJSON几何格式转换
- 异步数据库操作
- 空间查询和空间索引支持

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [common.h](ogc/db/common.h) | 公共定义 | `DatabaseType`, `SpatialOperator`, `DbResult`, `ColumnType`, `TransactionIsolation` |
| [export.h](ogc/db/export.h) | DLL导出宏 | `OGC_DB_API` |
| [result.h](ogc/db/result.h) | 操作结果 | `Result`, `DbException` |
| [connection.h](ogc/db/connection.h) | 数据库连接接口 | `DbConnection`, `ConnectionInfo`, `TableInfo`, `ColumnInfo` |
| [connection_pool.h](ogc/db/connection_pool.h) | 连接池 | `DbConnectionPool`, `PoolConfig`, `PooledConnection`, `PooledConnectionGuard` |
| [statement.h](ogc/db/statement.h) | 预处理语句 | `DbStatement` |
| [resultset.h](ogc/db/resultset.h) | 结果集 | `DbResultSet` |
| [sqlite_connection.h](ogc/db/sqlite_connection.h) | SQLite连接 | `SpatiaLiteConnection`, `SpatiaLiteStatement`, `SpatiaLiteResultSet`, `SpatiaLiteOptions` |
| [postgis_connection.h](ogc/db/postgis_connection.h) | PostGIS连接 | `PostGISConnection`, `PostGISStatement`, `PostGISResultSet`, `PostGISOptions` |
| [async_connection.h](ogc/db/async_connection.h) | 异步连接 | `DbAsyncExecutor`, `DbAsyncConnection`, `AsyncQueryBuilder`, `AsyncBatchInserter` |
| [srid_manager.h](ogc/db/srid_manager.h) | SRID管理 | `SridManager` |
| [sqlite_transaction.h](ogc/db/sqlite_transaction.h) | SQLite事务 | `SQLiteTransaction` |
| [sqlite_spatial.h](ogc/db/sqlite_spatial.h) | SQLite空间扩展 | `SQLiteSpatial` |
| [postgis_batch.h](ogc/db/postgis_batch.h) | PostGIS批量操作 | `PostGISBatch` |
| [wkb_converter.h](ogc/db/wkb_converter.h) | WKB转换器 | `WkbConverter` |
| [geojson_converter.h](ogc/db/geojson_converter.h) | GeoJSON转换器 | `GeoJsonConverter` |

---

## 类继承关系图

```
DbConnection (abstract)
    ├── SpatiaLiteConnection
    └── PostGISConnection

DbStatement (abstract)
    ├── SpatiaLiteStatement
    └── PostGISStatement

DbResultSet (abstract)
    ├── SpatiaLiteResultSet
    └── PostGISResultSet

DbAsyncExecutor (独立类，包装线程池)
DbAsyncConnection (独立类，包装DbConnection指针)
AsyncQueryBuilder (独立类，构建异步查询)
AsyncBatchInserter (独立类，批量异步插入)
```

---

## 依赖关系图

```
common.h
    │
    ├──► result.h
    │
    ├──► connection.h ──► connection_pool.h
    │         │
    │         ├──► statement.h
    │         ├──► resultset.h
    │         └──► wkb_converter.h
    │
    ├──► sqlite_connection.h ──► sqlite_transaction.h
    │         │
    │         └──► sqlite_spatial.h
    │
    ├──► postgis_connection.h ──► postgis_batch.h
    │
    ├──► async_connection.h
    │
    └──► geojson_converter.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | common.h, export.h, result.h, connection.h |
| **Connections** | sqlite_connection.h, postgis_connection.h |
| **Async** | async_connection.h |
| **Pool** | connection_pool.h |
| **Query** | statement.h, resultset.h |
| **Spatial** | wkb_converter.h, geojson_converter.h, sqlite_spatial.h |
| **Transaction** | sqlite_transaction.h |
| **Batch** | postgis_batch.h |
| **Utility** | srid_manager.h |

---

## 关键类

### DbConnection (抽象基类)
**File**: [connection.h](ogc/db/connection.h)  
**Description**: 数据库连接抽象接口

```cpp
class DbConnection {
public:
    virtual ~DbConnection() = default;
    
    // 连接管理
    virtual Result Connect(const std::string& connectionString) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual bool Ping() const = 0;
    
    // 事务管理
    virtual Result BeginTransaction() = 0;
    virtual Result Commit() = 0;
    virtual Result Rollback() = 0;
    virtual bool InTransaction() const = 0;
    
    // SQL执行
    virtual Result Execute(const std::string& sql) = 0;
    virtual Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) = 0;
    virtual Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) = 0;
    
    // 几何操作
    virtual Result InsertGeometry(...) = 0;
    virtual Result SelectGeometries(...) = 0;
    virtual Result SpatialQuery(...) = 0;
    virtual Result UpdateGeometry(...) = 0;
    virtual Result DeleteGeometry(...) = 0;
    
    // 表管理
    virtual Result CreateSpatialTable(...) = 0;
    virtual Result CreateSpatialIndex(...) = 0;
    virtual bool SpatialTableExists(const std::string& tableName) const = 0;
    virtual Result GetTableInfo(const std::string& tableName, TableInfo& info) const = 0;
};
```

### DbConnectionPool
**File**: [connection_pool.h](ogc/db/connection_pool.h)  
**Description**: 数据库连接池

```cpp
class DbConnectionPool {
public:
    static std::unique_ptr<DbConnectionPool> Create();
    
    Result Initialize(const PoolConfig& config, const std::string& connectionString);
    Result InitializePostGIS(const PoolConfig& config, const PostGISOptions& options);
    Result InitializeSpatiaLite(const PoolConfig& config, const SpatiaLiteOptions& options);
    
    Result Acquire(DbConnectionPtr& connection);
    Result Release(DbConnection* connection);
    void Shutdown();
    
    int GetTotalConnections() const;
    int GetAvailableConnections() const;
    int GetInUseConnections() const;
    
    void StartHealthCheck();
    void StopHealthCheck();
};
```

### SpatiaLiteConnection
**File**: [sqlite_connection.h](ogc/db/sqlite_connection.h)  
**Base**: `DbConnection`  
**Description**: SpatiaLite数据库连接

```cpp
class SpatiaLiteConnection : public DbConnection {
public:
    static SpatiaLiteConnectionPtr Create();
    Result Connect(const SpatiaLiteOptions& options);
    Result InitializeSpatialMetaData();
    sqlite3* GetRawConnection();
};
```

### PostGISConnection
**File**: [postgis_connection.h](ogc/db/postgis_connection.h)  
**Base**: `DbConnection`  
**Description**: PostGIS数据库连接

```cpp
class PostGISConnection : public DbConnection {
public:
    static PostGISConnectionPtr Create();
    Result Connect(const PostGISOptions& options);
    PGconn* GetRawConnection();
};
```

### DbAsyncConnection
**File**: [async_connection.h](ogc/db/async_connection.h)  
**Description**: 异步数据库连接包装器

```cpp
class DbAsyncConnection {
public:
    static std::unique_ptr<DbAsyncConnection> Create(DbConnection* syncConnection);
    
    std::future<Result> ConnectAsync(const std::string& connectionString);
    std::future<void> DisconnectAsync();
    std::future<Result> ExecuteAsync(const std::string& sql);
    std::future<Result> ExecuteQueryAsync(const std::string& sql, DbResultSetPtr& result);
    
    void ExecuteAsync(const std::string& sql, AsyncCallback callback);
    void ExecuteQueryAsync(const std::string& sql, AsyncResultSetCallback callback);
};
```

### Result
**File**: [result.h](ogc/db/result.h)  
**Description**: 操作结果封装类

```cpp
class Result {
public:
    Result();
    explicit Result(DbResult code);
    Result(DbResult code, const std::string& message);
    
    bool IsSuccess() const noexcept;
    bool IsError() const noexcept;
    DbResult GetCode() const noexcept;
    const std::string& GetMessage() const noexcept;
    
    static Result Success();
    static Result Error(DbResult code, const std::string& message);
};
```

---

## 接口

### DbConnection 连接接口方法
```cpp
virtual Result Connect(const std::string& connectionString) = 0;
virtual void Disconnect() = 0;
virtual bool IsConnected() const = 0;
virtual bool Ping() const = 0;
```

### DbConnection 事务接口方法
```cpp
virtual Result BeginTransaction() = 0;
virtual Result Commit() = 0;
virtual Result Rollback() = 0;
virtual bool InTransaction() const = 0;
```

### DbConnection 查询接口方法
```cpp
virtual Result Execute(const std::string& sql) = 0;
virtual Result ExecuteQuery(const std::string& sql, DbResultSetPtr& result) = 0;
virtual Result PrepareStatement(const std::string& name, const std::string& sql, DbStatementPtr& stmt) = 0;
```

### DbStatement 参数绑定接口
```cpp
virtual Result BindInt(int paramIndex, int32_t value) = 0;
virtual Result BindInt64(int paramIndex, int64_t value) = 0;
virtual Result BindDouble(int paramIndex, double value) = 0;
virtual Result BindString(int paramIndex, const std::string& value) = 0;
virtual Result BindGeometry(int paramIndex, const Geometry* geometry) = 0;
virtual Result BindNull(int paramIndex) = 0;
```

### DbResultSet 数据读取接口
```cpp
virtual bool Next() = 0;
virtual bool IsEOF() const = 0;
virtual int32_t GetInt(int columnIndex) const = 0;
virtual int64_t GetInt64(int columnIndex) const = 0;
virtual double GetDouble(int columnIndex) const = 0;
virtual std::string GetString(int columnIndex) const = 0;
virtual GeometryPtr GetGeometry(int columnIndex) const = 0;
```

---

## 类型定义

### 枚举类型

```cpp
enum class DatabaseType : uint8_t {
    kUnknown = 0,
    kPostGIS = 1,
    kSpatiaLite = 2,
    kOracleSpatial = 3,
    kSQLServerSpatial = 4
};

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

enum class DbResult : int32_t {
    kSuccess = 0,
    kNotConnected = 1,
    kConnectionFailed = 2,
    kConnectionLost = 3,
    kConnectionTimeout = 4,
    kConnectionPoolExhausted = 5,
    kExecutionError = 100,
    kSqlError = 101,
    kInvalidGeometry = 200,
    kTransactionError = 300,
    kOutOfMemory = 400,
    kNotSupported = 999,
    kNotImplemented = 1000,
    kUnknownError = 1001
};

enum class ColumnType : uint8_t {
    kUnknown = 0,
    kInteger = 1,
    kBigInt = 2,
    kReal = 3,
    kDouble = 4,
    kText = 5,
    kBlob = 6,
    kGeometry = 7,
    kBoolean = 8,
    kDate = 9,
    kDateTime = 10,
    kTimeStamp = 11
};

enum class TransactionIsolation : uint8_t {
    kReadUncommitted = 0,
    kReadCommitted = 1,
    kRepeatableRead = 2,
    kSerializable = 3,
    kDeferred = 4,
    kImmediate = 5,
    kExclusive = 6
};

enum class PooledConnectionState {
    kAvailable,
    kInUse,
    kExpired,
    kInvalid
};
```

### 常量定义

```cpp
constexpr int DEFAULT_CONNECTION_TIMEOUT_MS = 30000;
constexpr int DEFAULT_QUERY_TIMEOUT_MS = 60000;
constexpr int DEFAULT_POOL_MIN_SIZE = 2;
constexpr int DEFAULT_POOL_MAX_SIZE = 16;
constexpr int DEFAULT_POOL_TIMEOUT_MS = 5000;
```

### 结构体

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

struct SpatiaLiteOptions {
    std::string databasePath;
    bool readOnly = false;
    bool createIfNotExist = false;
    std::string cacheSize = "2000";
    int connectionTimeout = 5000;
};

struct PostGISOptions {
    std::string host = "localhost";
    int port = 5432;
    std::string database;
    std::string user;
    std::string password;
    std::string sslmode = "disable";
    int connectionTimeout = 10;
};
```

---

## 使用示例

### 连接数据库

```cpp
#include "ogc/db/sqlite_connection.h"
#include "ogc/db/postgis_connection.h"

using namespace ogc::db;

// SQLite/SpatiaLite
SpatiaLiteOptions options;
options.databasePath = "/path/to/database.sqlite";
options.createIfNotExist = true;

auto conn = SpatiaLiteConnection::Create();
auto result = conn->Connect(options);

// PostGIS
PostGISOptions pgOptions;
pgOptions.host = "localhost";
pgOptions.port = 5432;
pgOptions.database = "mydb";
pgOptions.user = "postgres";
pgOptions.password = "password";

auto pgConn = PostGISConnection::Create();
auto pgResult = pgConn->Connect(pgOptions);
```

### 使用连接池

```cpp
#include "ogc/db/connection_pool.h"

PoolConfig config;
config.minConnections = 2;
config.maxConnections = 10;

auto pool = DbConnectionPool::Create(DatabaseType::kPostGIS);
pool->Initialize(config, connectionString);

// 使用RAII守卫获取连接
{
    DbConnectionPtr conn;
    if (pool->Acquire(conn) == Result::Success()) {
        PooledConnectionGuard guard(pool, conn);
        guard->Execute("SELECT * FROM features");
    }
}

pool->Shutdown();
```

### 空间查询

```cpp
#include "ogc/db/connection.h"

Envelope bounds(0, 0, 100, 100);
std::vector<GeometryPtr> results;

conn->SpatialQuery("features", "geom", SpatialOperator::kIntersects, 
                   queryGeom.get(), results);

for (const auto& geom : results) {
    // 处理几何对象
}
```

### 异步操作

```cpp
#include "ogc/db/async_connection.h"

auto asyncConn = DbAsyncConnection::Create(conn.get());

// 使用future
auto future = asyncConn->ExecuteAsync("SELECT * FROM features");
auto result = future.get();

// 使用回调
asyncConn->ExecuteAsync("SELECT * FROM features", [](Result result) {
    if (result.IsSuccess()) {
        // 操作成功
    }
});
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加连接池支持 |
| v1.2 | 2026-03-20 | Team | 添加异步连接支持 |
| v1.3 | 2026-04-06 | index-validator | 修复继承关系、补充类型定义、修正章节顺序 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
