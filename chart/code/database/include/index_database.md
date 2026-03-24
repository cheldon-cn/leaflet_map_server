# Database Module - Header Index

## Overview

Database模块提供空间数据库访问的抽象接口和具体实现。支持PostGIS和SQLite/SpatiaLite两种空间数据库，提供统一的连接管理、查询执行、空间操作接口。

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [connection.h](ogc/db/connection.h) | 数据库连接基类 | `DbConnection` |
| [postgis_connection.h](ogc/db/postgis_connection.h) | PostGIS连接 | `PostGISConnection` |
| [sqlite_connection.h](ogc/db/sqlite_connection.h) | SQLite连接 | `SQLiteConnection` |
| [async_connection.h](ogc/db/async_connection.h) | 异步连接 | `AsyncConnection` |
| [connection_pool.h](ogc/db/connection_pool.h) | 连接池 | `DbConnectionPool` |
| [statement.h](ogc/db/statement.h) | 预处理语句 | `DbStatement` |
| [result.h](ogc/db/result.h) | 查询结果 | `Result` |
| [resultset.h](ogc/db/resultset.h) | 结果集 | `DbResultSet` |
| [postgis_batch.h](ogc/db/postgis_batch.h) | PostGIS批量操作 | `PostGISBatch` |
| [sqlite_transaction.h](ogc/db/sqlite_transaction.h) | SQLite事务 | `SQLiteTransaction` |
| [sqlite_spatial.h](ogc/db/sqlite_spatial.h) | SQLite空间扩展 | `SQLiteSpatial` |
| [srid_manager.h](ogc/db/srid_manager.h) | SRID管理器 | `SridManager` |
| [wkb_converter.h](ogc/db/wkb_converter.h) | WKB转换器 | `WkbConverter` |
| [geojson_converter.h](ogc/db/geojson_converter.h) | GeoJSON转换器 | `DbGeoJSONConverter` |
| [common.h](ogc/db/common.h) | 公共定义 | `Result`, `DatabaseType` |
| [export.h](ogc/db/export.h) | 导出宏定义 | `OGC_DB_API` |

## Class Inheritance Diagram

```
DbConnection (abstract base)
    ├── PostGISConnection
    ├── SQLiteConnection
    └── AsyncConnection (decorator)

DbStatement (abstract base)
    ├── PostGISStatement
    └── SQLiteStatement

DbResultSet (abstract base)
    ├── PostGISResultSet
    └── SQLiteResultSet

DbConnectionPool
    └── (manages DbConnection instances)
```

## Core Classes Detail

### DbConnection

**File**: [connection.h](ogc/db/connection.h)

**Base Classes**: None (abstract interface)

**Purpose**: 数据库连接抽象基类，定义了所有数据库类型的通用接口。

**Key Methods**:
- `Connect()` / `Disconnect()` / `IsConnected()` - 连接管理
- `Ping()` - 连接检测
- `BeginTransaction()` / `Commit()` / `Rollback()` - 事务操作
- `Execute()` / `ExecuteQuery()` - 执行SQL
- `PrepareStatement()` - 创建预处理语句
- `InsertGeometry()` / `InsertGeometries()` - 插入几何
- `SelectGeometries()` - 查询几何
- `SpatialQuery()` - 空间查询
- `UpdateGeometry()` / `DeleteGeometry()` - 更新/删除几何
- `CreateSpatialTable()` / `CreateSpatialIndex()` - 创建空间表和索引
- `GetTableInfo()` / `GetColumns()` - 获取表信息

### PostGISConnection

**File**: [postgis_connection.h](ogc/db/postgis_connection.h)

**Base Classes**: `DbConnection`

**Purpose**: PostGIS数据库连接实现。

**Key Methods**:
- `Connect(const PostGISOptions&)` - 使用选项连接
- `GetVersion()` - 获取PostGIS版本
- `GetRawConnection()` - 获取原生PGconn指针
- `EscapeString()` - 字符串转义
- `SetIsolationLevel()` - 设置隔离级别

**Configuration**:
```cpp
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

### SQLiteConnection

**File**: [sqlite_connection.h](ogc/db/sqlite_connection.h)

**Base Classes**: `DbConnection`

**Purpose**: SQLite/SpatiaLite数据库连接实现。

**Key Methods**:
- `Connect(const std::string& path)` - 连接数据库文件
- `LoadExtension()` - 加载扩展
- `EnableSpatialite()` - 启用SpatiaLite
- `GetDatabasePath()` - 获取数据库路径
- `GetPageSize()` / `GetPageCount()` - 数据库信息

### DbStatement

**File**: [statement.h](ogc/db/statement.h)

**Base Classes**: None (abstract interface)

**Purpose**: 预处理语句接口，支持参数绑定。

**Key Methods**:
- `BindInt()` / `BindInt64()` / `BindDouble()` - 绑定数值
- `BindString()` / `BindBool()` - 绑定基本类型
- `BindBlob()` - 绑定二进制
- `BindGeometry()` / `BindEnvelope()` - 绑定几何
- `Execute()` / `ExecuteQuery()` - 执行语句
- `Reset()` / `ClearBindings()` - 重置语句

### DbResultSet

**File**: [resultset.h](ogc/db/resultset.h)

**Base Classes**: None (abstract interface)

**Purpose**: 查询结果集接口。

**Key Methods**:
- `Next()` / `IsEOF()` / `Reset()` - 遍历控制
- `GetColumnCount()` / `GetColumnName()` / `GetColumnType()` - 列信息
- `GetInt()` / `GetInt64()` / `GetDouble()` / `GetString()` - 获取值
- `GetBlob()` - 获取二进制
- `GetGeometry()` - 获取几何
- `GetRowCount()` / `GetCurrentRow()` - 行信息

## Type Aliases

```cpp
using DbConnectionPtr = std::unique_ptr<DbConnection>;
using DbConnectionRef = std::shared_ptr<DbConnection>;
using DbStatementPtr = std::unique_ptr<DbStatement>;
using DbResultSetPtr = std::unique_ptr<DbResultSet>;
using PostGISConnectionPtr = std::unique_ptr<PostGISConnection>;
using SQLiteConnectionPtr = std::unique_ptr<SQLiteConnection>;
```

## Enumerations

### DatabaseType

```cpp
enum class DatabaseType {
    kUnknown,
    kPostgreSQL,
    kSQLite,
    kMySQL,
    kOracle
};
```

### SpatialOperator

```cpp
enum class SpatialOperator {
    kIntersects,
    kWithin,
    kContains,
    kTouches,
    kCrosses,
    kOverlaps,
    kDWithin,
    kEquals
};
```

### TransactionIsolation

```cpp
enum class TransactionIsolation {
    kDefault,
    kReadUncommitted,
    kReadCommitted,
    kRepeatableRead,
    kSerializable
};
```

## Dependencies

```
database
  ├── geom (Geometry, Envelope)
  └── external (libpq, sqlite3)
```

## Quick Usage Examples

```cpp
#include "ogc/db/postgis_connection.h"
#include "ogc/db/sqlite_connection.h"

// PostGIS连接
PostGISOptions options;
options.host = "localhost";
options.database = "mydb";
options.user = "postgres";
options.password = "password";

auto conn = PostGISConnection::Create();
if (conn->Connect(options) == Result::kSuccess) {
    // 空间查询
    std::vector<GeometryPtr> geometries;
    conn->SpatialQuery("mytable", "geom", 
        SpatialOperator::kIntersects,
        queryGeom.get(), geometries);
}

// SQLite连接
auto sqlite = SQLiteConnection::Create();
if (sqlite->Connect("/path/to/db.sqlite") == Result::kSuccess) {
    sqlite->EnableSpatialite();
    
    // 执行查询
    DbResultSetPtr result;
    sqlite->ExecuteQuery("SELECT * FROM features", result);
    while (result->Next()) {
        auto geom = result->GetGeometry("geom");
    }
}
```
