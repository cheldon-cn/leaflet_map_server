# 地图服务器设计文档 (v2.0)

> **版本**: 2.0 (增强版)  
> **最后更新**: 2026-03-09  
> **状态**: 增强设计整合完成  
> **变更说明**: 整合数据库抽象层、多格式编码器、高 DPI 支持、范围控制等增强特性

---

## 修订历史

| 版本 | 日期 | 作者 | 变更描述 |
|------|------|------|----------|
| 1.0 | 2026-03-08 | AI Assistant | 初始版本，包含基础地图服务设计 |
| 2.0 | 2026-03-09 | AI Assistant | **增强版本**：添加数据库抽象层、多格式编码器、高 DPI 支持、范围控制、性能优化 |

### v2.0 主要变更

1. **数据库抽象层**：支持 SQLite3/SpatiaLite 和 PostgreSQL/PostGIS 双后端
2. **多格式编码器**：支持 PNG8/PNG32/WebP 格式，可动态切换
3. **高 DPI 支持**：72-600 DPI 范围，自动缩放处理
4. **渲染范围控制**：可配置的渲染范围限制，防止过大请求
5. **性能提升**：目标渲染时间<350ms，并发>200 req/s，缓存命中率>95%
6. **C++11 规范**：严格遵循 C++11 标准，优化跨平台兼容性
7. **命名空间组织**：引入 `cycle` 根命名空间，模块化组织代码

---

## 目录

1. [项目概述](#1-项目概述)
2. [系统架构](#2-系统架构)
3. [核心模块设计](#3-核心模块设计)
4. [数据模型](#4-数据模型)
5. [数据库抽象层设计](#5-数据库抽象层设计)
6. [多格式图像编码器](#6-多格式图像编码器)
7. [缓存设计](#7-缓存设计)
8. [API 设计](#8-设计)
9. [错误处理](#9-错误处理)
10. [性能优化](#10-性能优化)
11. [安全设计](#11-安全设计)
12. [监控与日志](#12-监控与日志)
13. [部署方案](#13-部署方案)
14. [开发规范](#14-开发规范)
15. [测试策略](#15-测试策略)
16. [风险评估与缓解](#16-风险评估与缓解)
17. [迁移指南](#17-迁移指南)
18. [验收标准](#18-验收标准)
19. [附录](#19-附录)

---

## 1. 项目概述

### 1.1 项目背景

本项目旨在构建一个高性能、跨平台的地图瓦片渲染服务器，支持多格式输出、高 DPI 渲染和灵活的数据库后端，为 Web 和移动端应用提供稳定的地图服务。

### 1.2 项目目标

#### 核心目标
- **跨平台 HTTP 服务**：基于 cpp-httplib，支持 Windows/Linux/macOS
- **多数据库后端**：支持 SQLite3/SpatiaLite 和 PostgreSQL/PostGIS
- **多格式图像编码**：支持 PNG8/PNG32/WebP 格式
- **高 DPI 支持**：72-600 DPI 范围，自动缩放处理
- **高性能渲染**：单图渲染时间 < 350ms，并发能力 > 200 req/s
- **智能缓存**：缓存命中率 > 95%，支持内存和磁盘缓存

#### 增强目标 (v2.0)
- **数据库抽象层**：统一接口，支持动态切换数据库后端
- **渲染范围控制**：可配置的渲染范围限制，防止过大请求
- **C++11 兼容性**：严格遵循 C++11 标准，确保跨平台兼容性
- **命名空间规范**：使用 `cycle` 根命名空间，模块化组织代码

### 1.3 设计原则

1. **高性能优先**：优化渲染流水线，减少内存拷贝
2. **跨平台兼容**：使用标准 C++11，避免平台特定 API
3. **模块化设计**：清晰的模块边界，便于维护和扩展
4. **可配置性**：通过配置文件灵活调整系统行为
5. **错误处理**：完善的错误处理和日志记录
6. **可扩展性**：支持未来添加新的数据库后端和图像格式
7. **C++11 规范**：使用现代 C++ 特性（智能指针、auto、lambda 等）
8. **命名空间组织**：使用 `cycle` 根命名空间，子模块按功能划分

### 1.4 术语定义

| 术语 | 定义 |
|------|------|
| **WMTS** | Web Map Tile Service，OGC 标准的地图瓦片服务规范 |
| **DPI** | Dots Per Inch，每英寸点数，衡量图像分辨率 |
| **瓦片 (Tile)** | 固定尺寸的地图图像块（如 256x256 像素） |
| **瓦片矩阵集** | 一组预定义比例的瓦片集合 |
| **SpatiaLite** | SQLite 的空间扩展，支持地理空间数据 |
| **PostGIS** | PostgreSQL 的空间扩展，支持地理空间数据 |
| **数据库抽象层** | 统一数据库操作接口，屏蔽后端差异 |
| **编码器抽象层** | 统一图像编码接口，支持多格式输出 |

---

## 2. 系统架构

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                         Client Layer                             │
│                    (Web / Mobile / Desktop)                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ HTTP/HTTPS
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      HTTP Server Layer                           │
│                   (cpp-httplib / C++11)                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ WMTS Handler │  │ REST Handler │  │ Admin Handler│          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Application Layer                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │  MapService  │  │ CacheService │  │  AuthService │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Database Abstraction Layer                   │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │ IDatabase    │  │ SqliteDatabase│ │PostgresqlDatabase│  │   │
│  │  │ (Interface)  │  │              │  │              │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Encoder Abstraction Layer                    │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │ IEncoder     │  │ PngEncoder   │  │ WebpEncoder  │   │   │
│  │  │ (Interface)  │  │ (PNG8/PNG32) │  │              │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Rendering Engine                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ RenderContext│  │ SymbolEngine │  │ LabelEngine  │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                       Data Access Layer                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ SQLite3 +    │  │ PostgreSQL + │  │ File System  │          │
│  │ SpatiaLite   │  │ PostGIS      │  │ (Config)     │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 技术栈

#### 核心依赖
- **HTTP 服务**: cpp-httplib (v0.12+)
- **数据库**: 
  - SQLite3 (v3.38+) + SpatiaLite (v5.1+)
  - PostgreSQL (v14+) + PostGIS (v3.2+) [可选]
- **图像编码**: 
  - libpng (v1.6+) - PNG8/PNG32
  - libwebp (v1.2+) - WebP [可选]
- **JSON 解析**: nlohmann/json (v3.11+)
- **构建系统**: CMake (v3.10+)
- **C++ 标准**: C++11 (严格兼容)

#### 可选依赖
- **SSL/TLS**: OpenSSL (v3.0+)
- **并发**: C++11 std::thread / std::async
- **压缩**: zlib (v1.2.11+)
- **数学计算**: glm (v0.9.9+)

### 2.3 目录结构

#### 当前实际结构
```
server/
├── README.md           # 项目文档
├── DESIGN.md           # 设计文档 (v2.0)
├── config.json         # 配置文件示例
└── api/
    └── openapi.yaml    # API 规范
```

#### 目标结构（待实现）
```
server/
├── src/
│   ├── main.cpp
│   ├── server/
│   │   ├── http_server.cpp
│   │   └── http_server.h
│   ├── config/
│   │   ├── config.cpp
│   │   └── config.h
│   ├── database/
│   │   ├── database_factory.cpp
│   │   ├── database_factory.h
│   │   ├── idatabase.h
│   │   ├── sqlite_database.cpp
│   │   ├── sqlite_database.h
│   │   ├── postgresql_database.cpp
│   │   └── postgresql_database.h
│   ├── encoder/
│   │   ├── encoder_factory.cpp
│   │   ├── encoder_factory.h
│   │   ├── iencoder.h
│   │   ├── png_encoder.cpp
│   │   ├── png_encoder.h
│   │   ├── webp_encoder.cpp
│   │   └── webp_encoder.h
│   ├── renderer/
│   │   ├── renderer.cpp
│   │   ├── renderer.h
│   │   ├── render_context.cpp
│   │   └── render_context.h
│   ├── cache/
│   │   ├── cache.cpp
│   │   └── cache.h
│   └── utils/
│       ├── logger.cpp
│       ├── logger.h
│       └── utils.h
├── resources/
│   ├── styles/
│   └── symbols/
├── tests/
│   ├── test_database.cpp
│   ├── test_encoder.cpp
│   └── test_renderer.cpp
├── CMakeLists.txt
├── config.json
├── README.md
└── DESIGN.md
```

### 2.4 模块依赖关系

```
main.cpp
  │
  ├─> HttpServer (cpp-httplib)
  │     │
  │     ├─> MapService
  │     │     │
  │     │     ├─> Renderer
  │     │     │     ├─> Database (IDatabase)
  │     │     │     └─> Encoder (IEncoder)
  │     │     │
  │     │     └─> Cache
  │     │
  │     └─> Config
  │
  └─> Logger
```

---

## 3. 核心模块设计

### 3.1 Config 模块

#### 3.1.1 配置结构

```cpp
namespace cycle {

// 数据库配置
struct DatabaseConfig {
    std::string type = "sqlite3";  // "sqlite3" 或 "postgresql"
    std::string sqlite_path = "./data/map.db";
    
    // PostgreSQL 连接参数
    std::string pg_host = "localhost";
    int pg_port = 5432;
    std::string pg_database = "mapdb";
    std::string pg_username = "postgres";
    std::string pg_password = "";
    
    // 连接池配置
    int max_connections = 10;
    int connection_timeout = 30;  // 秒
};

// 编码器配置
struct EncoderConfig {
    std::string format = "png32";  // "png8", "png32", "webp"
    int png_compression = 6;       // 1-9
    int webp_quality = 80;         // 0-100
    bool webp_lossless = false;
    
    // DPI 配置
    int default_dpi = 96;
    int min_dpi = 72;
    int max_dpi = 600;
};

// 渲染范围限制配置
struct RangeLimitConfig {
    bool enabled = true;
    
     // 范围限制（度）
    double max_bbox_width = 360.0;
    double max_bbox_height = 180.0;
    double min_bbox_width = 0.0001;
    double min_bbox_height = 0.0001;
    
    // 尺寸限制（像素）
    int max_image_width = 4096;
    int max_image_height = 4096;
    int min_image_width = 32;
    int min_image_height = 32;
    
    // DPI 范围
    int min_dpi = 72;
    int max_dpi = 600;
    
    // 验证方法
    bool ValidateBoundingBox(const BoundingBox& bbox) const;
    bool ValidateImageSize(int width, int height) const;
    bool ValidateDpi(int dpi) const;
    
    // 最大缩放级别
    int max_zoom = 22;
    
    // 拒绝过大请求
    bool reject_oversized = true;
};

// 服务器配置
struct ServerConfig {
    std::string host = "0.0.0.0";
    int port = 8080;
    int thread_count = 4;
    int read_timeout = 30;
    int write_timeout = 30;
    
    // SSL 配置
    bool ssl_enabled = false;
    std::string ssl_cert_file = "";
    std::string ssl_key_file = "";
};

// 缓存配置
struct CacheConfig {
    bool enabled = true;
    size_t memory_cache_size = 512 * 1024 * 1024;  // 512MB
    std::string disk_cache_path = "./cache";
    int cache_ttl = 3600;  // 秒
};

// 日志配置
struct LogConfig {
    int level = 1;  // 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR
    std::string file = "./logs/server.log";
    bool console_output = true;
    bool rotate = true;
    size_t max_size = 16 * 1024 * 1024;  // 10MB
    int max_files = 5;
};

// 主配置类
class Config {
public:
    DatabaseConfig database;
    EncoderConfig encoder;
    RangeLimitConfig range_limit;
    ServerConfig server;
    CacheConfig cache;
    LogConfig log;
    
    bool loadFromFile(const std::string& path);
    bool validate() const;
    
private:
    std::string config_path_;
};

} // namespace cycle
```

#### 3.1.2 配置文件示例

```json
{
  "database": {
    "type": "sqlite3",
    "sqlite_path": "./data/map.db",
    "max_connections": 10,
    "connection_timeout": 30
  },
  "encoder": {
    "format": "png32",
    "png_compression": 6,
    "default_dpi": 96,
    "min_dpi": 72,
    "max_dpi": 600
  },
  "range_limit": {
    "enabled": true,
    "max_width": 4096,
    "max_height": 4096,
    "max_dpi": 600,
    "max_zoom": 22,
    "reject_oversized": true
  },
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "thread_count": 4,
    "read_timeout": 30,
    "write_timeout": 30,
    "ssl_enabled": false
  },
  "cache": {
    "enabled": true,
    "memory_cache_size": 536870912,
    "disk_cache_path": "./cache",
    "cache_ttl": 3600
  },
  "log": {
    "level": 1,
    "file": "./logs/server.log",
    "console_output": true,
    "rotate": true,
    "max_size": 10485760,
    "max_files": 5
  }
}
```

### 3.2 HTTP Server 模块

#### 3.2.1 接口设计

```cpp
namespace cycle {

class HttpServer {
public:
    HttpServer(const Config& config);
    ~HttpServer();
    
    bool start();
    void stop();
    bool isRunning() const;
    
private:
    void setupRoutes();
    void setupMiddlewares();
    
    // 请求处理
    void handleGetTile(const httplib::Request& req, httplib::Response& res);
    void handleGetCapabilities(const httplib::Request& req, httplib::Response& res);
    void handleGetTileSet(const httplib::Request& req, httplib::Response& res);
    void handleHealth(const httplib::Request& req, httplib::Response& res);
    
    Config config_;
    std::unique_ptr<httplib::Server> server_;
    std::atomic<bool> running_;
};

} // namespace cycle
```

#### 3.2.2 路由定义

| 方法 | 路径 | 描述 |
|------|------|------|
| GET | `/1.0.0/WMTSCapabilities.xml` | 获取服务元数据 |
| GET | `/{layer}/{tileMatrixSet}/{TileMatrix}/{TileRow}/{TileCol}.{format}` | 获取瓦片 |
| GET | `/health` | 健康检查 |
| GET | `/metrics` | 性能指标 |
| GET | `/admin/config` | 获取配置 (需要认证) |
| POST | `/admin/config/reload` | 重载配置 (需要认证) |

### 3.3 MapService 模块

#### 3.3.1 接口设计

```cpp
namespace cycle {

// 瓦片请求参数
struct TileRequest {
    std::string layer;
    std::string tileMatrixSet;
    int tileMatrix;
    int tileRow;
    int tileCol;
    std::string format;  // "png8", "png32", "webp"
    int dpi = 96;
    int width = 256;
    int height = 256;
};

// 瓦片响应
struct TileResponse {
    std::vector<uint8_t> data;
    std::string content_type;
    int64_t timestamp;
    bool from_cache;
};

class MapService {
public:
    MapService(std::shared_ptr<IDatabase> db,
               std::shared_ptr<IEncoder> encoder,
               std::shared_ptr<Cache> cache);
    
    TileResponse getTile(const TileRequest& request);
    
private:
    std::shared_ptr<IDatabase> database_;
    std::shared_ptr<IEncoder> encoder_;
    std::shared_ptr<Cache> cache_;
    
    // 范围验证
    bool validateRequest(const TileRequest& request) const;
    
    // 渲染逻辑
    std::vector<uint8_t> renderTile(const TileRequest& request);
};

} // namespace cycle
```

---

## 4. 数据模型

### 4.1 核心数据模型

#### 4.1.1 地理几何模型

```cpp
namespace cycle {

struct Envelope {
    double minX;
    double minY;
    double maxX;
    double maxY;
    
    double width() const { return maxX - minX; }
    double height() const { return maxY - minY; }
    bool isValid() const { return minX < maxX && minY < maxY; }
};

struct Point {
    double x;
    double y;
    int srid = 4326;
};

struct LineString {
    std::vector<Point> points;
    int srid = 4326;
};

struct Polygon {
    std::vector<Point> exterior;
    std::vector<std::vector<Point>> interiors;
    int srid = 4326;
};

} // namespace cycle
```

#### 4.1.2 地图图层模型

```cpp
namespace cycle {

struct LayerConfig {
    std::string name;
    std::string title;
    std::string abstract;
    std::string srs = "EPSG:3857";
    Envelope boundingBox;
    std::vector<std::string> formats;  // ["png8", "png32", "webp"]
    std::vector<TileMatrix> tileMatrices;
};

struct TileMatrix {
    std::string identifier;
    int scaleDenominator;
    Point topLeftCorner;
    int tileWidth = 256;
    int tileHeight = 256;
    int matrixWidth;
    int matrixHeight;
};

} // namespace cycle
```

### 4.2 数据库表结构

#### 4.2.1 图层元数据表

```sql
CREATE TABLE IF NOT EXISTS layers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    title TEXT NOT NULL,
    abstract TEXT,
    srs TEXT DEFAULT 'EPSG:3857',
    min_x REAL,
    min_y REAL,
    max_x REAL,
    max_y REAL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_layers_name ON layers(name);
```

#### 4.2.2 瓦片矩阵表

```sql
CREATE TABLE IF NOT EXISTS tile_matrices (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    layer_id INTEGER NOT NULL,
    identifier TEXT NOT NULL,
    scale_denominator REAL NOT NULL,
    top_left_x REAL NOT NULL,
    top_left_y REAL NOT NULL,
    tile_width INTEGER DEFAULT 256,
    tile_height INTEGER DEFAULT 256,
    matrix_width INTEGER NOT NULL,
    matrix_height INTEGER NOT NULL,
    FOREIGN KEY (layer_id) REFERENCES layers(id),
    UNIQUE(layer_id, identifier)
);

CREATE INDEX idx_tile_matrices_layer ON tile_matrices(layer_id);
```

#### 4.2.3 地图数据表

```sql
CREATE TABLE IF NOT EXISTS map_features (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    layer_id INTEGER NOT NULL,
    geometry GEOMETRY NOT NULL,
    attributes TEXT,  -- JSON 格式
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (layer_id) REFERENCES layers(id)
);

CREATE VIRTUAL TABLE IF NOT EXISTS map_features_idx USING rtree(
    id,
    min_x, max_x,
    min_y, max_y
);
```

---

## 5. 数据库抽象层设计

### 5.1 设计目标

1. **统一接口**：为 SQLite3/SpatiaLite 和 PostgreSQL/PostGIS 提供统一的操作接口
2. **动态切换**：支持运行时根据配置切换数据库后端
3. **连接池管理**：高效管理数据库连接，支持并发访问
4. **预处理语句**：支持预处理语句，防止 SQL 注入
5. **事务支持**：支持事务操作，保证数据一致性

### 5.2 接口定义

#### 5.2.1 IDatabase 接口

```cpp
namespace cycle {
namespace database {

// SQL 参数
struct SqlParameter {
    enum Type { INTEGER, REAL, TEXT, BLOB, NULL };
    Type type;
    union {
        int int_value;
        double real_value;
        const char* text_value;
        struct {
            const void* blob_data;
            size_t blob_size;
        };
    };
    
    static SqlParameter integer(int value) {
        SqlParameter p; p.type = INTEGER; p.int_value = value; return p;
    }
    static SqlParameter real(double value) {
        SqlParameter p; p.type = REAL; p.real_value = value; return p;
    }
    static SqlParameter text(const char* value) {
        SqlParameter p; p.type = TEXT; p.text_value = value; return p;
    }
};

// 查询结果行
class DatabaseRow {
public:
    int getColumnCount() const;
    std::string getColumnName(int index) const;
    
    int getInt(int index) const;
    double getDouble(int index) const;
    std::string getString(int index) const;
    std::vector<uint8_t> getBlob(int index) const;
    bool isNull(int index) const;
    
private:
    // 内部实现
};

// 查询结果集
class ResultSet {
public:
    bool next();
    DatabaseRow getCurrentRow() const;
    int getRowCount() const;
    void close();
    
private:
    // 内部实现
};

// 数据库接口
class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    // 连接管理
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    
    // 执行 SQL
    virtual bool execute(const std::string& sql) = 0;
    virtual bool execute(const std::string& sql, const std::vector<SqlParameter>& params) = 0;
    
    // 查询
    virtual std::unique_ptr<ResultSet> query(const std::string& sql) = 0;
    virtual std::unique_ptr<ResultSet> query(const std::string& sql, 
                                             const std::vector<SqlParameter>& params) = 0;
    
    // 事务
    virtual bool beginTransaction() = 0;
    virtual bool commitTransaction() = 0;
    virtual bool rollbackTransaction() = 0;
    
    // 获取最后错误
    virtual std::string getLastError() const = 0;
    virtual int getLastErrorCode() const = 0;
    
    // 空间查询优化
    virtual std::unique_ptr<ResultSet> querySpatial(
        const std::string& table,
        const Envelope& envelope,
        const std::string& geometryColumn = "geometry") = 0;
};

} // namespace database
} // namespace cycle
```

### 5.3 SQLite3 实现

#### 5.3.1 SqliteDatabase 类

```cpp
namespace cycle {
namespace database {

class SqliteDatabase : public IDatabase {
public:
    explicit SqliteDatabase(const std::string& dbPath);
    ~SqliteDatabase() override;
    
    bool open() override;
    void close() override;
    bool isOpen() const override;
    
    bool execute(const std::string& sql) override;
    bool execute(const std::string& sql, const std::vector<SqlParameter>& params) override;
    
    std::unique_ptr<ResultSet> query(const std::string& sql) override;
    std::unique_ptr<ResultSet> query(const std::string& sql,
                                     const std::vector<SqlParameter>& params) override;
    
    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;
    
    std::string getLastError() const override;
    int getLastErrorCode() const override;
    
    std::unique_ptr<ResultSet> querySpatial(
        const std::string& table,
        const Envelope& envelope,
        const std::string& geometryColumn) override;
    
private:
    bool initSpatialite();
    
    sqlite3* db_;
    std::string dbPath_;
    bool hasSpatialite_;
};

} // namespace database
} // namespace cycle
```

#### 5.3.2 SQLite3 实现要点

1. **初始化 SpatiaLite**：
   ```cpp
   bool SqliteDatabase::initSpatialite() {
       auto result = query("SELECT InitSpatialite(1)");
       return result != nullptr;
   }
   ```

2. **空间查询**：
   ```cpp
   std::unique_ptr<ResultSet> SqliteDatabase::querySpatial(
       const std::string& table,
       const Envelope& envelope,
       const std::string& geometryColumn) {
       
       std::string sql = 
           "SELECT * FROM " + table +
           " WHERE " + geometryColumn + " && BuildMbr(?, ?, ?, ?, 4326)";
       
       std::vector<SqlParameter> params = {
           SqlParameter::real(envelope.minX),
           SqlParameter::real(envelope.minY),
           SqlParameter::real(envelope.maxX),
           SqlParameter::real(envelope.maxY)
       };
       
       return query(sql, params);
   }
   ```

### 5.4 PostgreSQL 实现

#### 5.4.1 PostgresqlDatabase 类

```cpp
namespace cycle {
namespace database {

class PostgresqlDatabase : public IDatabase {
public:
    explicit PostgresqlDatabase(const DatabaseConfig& config);
    ~PostgresqlDatabase() override;
    
    bool open() override;
    void close() override;
    bool isOpen() const override;
    
    bool execute(const std::string& sql) override;
    bool execute(const std::string& sql, const std::vector<SqlParameter>& params) override;
    
    std::unique_ptr<ResultSet> query(const std::string& sql) override;
    std::unique_ptr<ResultSet> query(const std::string& sql,
                                     const std::vector<SqlParameter>& params) override;
    
    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;
    
    std::string getLastError() const override;
    int getLastErrorCode() const override;
    
    std::unique_ptr<ResultSet> querySpatial(
        const std::string& table,
        const Envelope& envelope,
        const std::string& geometryColumn) override;
    
private:
    bool initPostgis();
    
    PGconn* conn_;
    DatabaseConfig config_;
    bool hasPostgis_;
};

} // namespace database
} // namespace cycle
```

#### 5.4.2 PostgreSQL 实现要点

1. **连接字符串构建**：
   ```cpp
   std::string connInfo = 
       "host=" + config_.pg_host +
       " port=" + std::to_string(config_.pg_port) +
       " dbname=" + config_.pg_database +
       " user=" + config_.pg_username +
       " password=" + config_.pg_password;
   ```

2. **PostGIS 初始化**：
   ```cpp
   bool PostgresqlDatabase::initPostgis() {
       return execute("CREATE EXTENSION IF NOT EXISTS postgis");
   }
   ```

3. **空间查询**：
   ```cpp
   std::unique_ptr<ResultSet> PostgresqlDatabase::querySpatial(
       const std::string& table,
       const Envelope& envelope,
       const std::string& geometryColumn) {
       
       std::string sql = 
           "SELECT * FROM " + table +
           " WHERE " + geometryColumn + " && ST_MakeEnvelope($1, $2, $3, $4, 4326)";
       
       std::vector<SqlParameter> params = {
           SqlParameter::real(envelope.minX),
           SqlParameter::real(envelope.minY),
           SqlParameter::real(envelope.maxX),
           SqlParameter::real(envelope.maxY)
       };
       
       return query(sql, params);
   }
   ```

### 5.5 DatabaseFactory 工厂类

#### 5.5.1 工厂模式实现

```cpp
namespace cycle {
namespace database {

class DatabaseFactory {
public:
    static std::shared_ptr<IDatabase> create(const DatabaseConfig& config);
    
private:
    static std::shared_ptr<IDatabase> createSqlite(const DatabaseConfig& config);
    static std::shared_ptr<IDatabase> createPostgresql(const DatabaseConfig& config);
};

} // namespace database
} // namespace cycle
```

#### 5.5.2 工厂实现

```cpp
std::shared_ptr<IDatabase> DatabaseFactory::create(const DatabaseConfig& config) {
    if (config.type == "sqlite3") {
        return createSqlite(config);
    } else if (config.type == "postgresql") {
        return createPostgresql(config);
    } else {
        throw std::runtime_error("Unsupported database type: " + config.type);
    }
}

std::shared_ptr<IDatabase> DatabaseFactory::createSqlite(const DatabaseConfig& config) {
    auto db = std::make_shared<SqliteDatabase>(config.sqlite_path);
    if (!db->open()) {
        throw std::runtime_error("Failed to open SQLite database: " + db->getLastError());
    }
    return db;
}

std::shared_ptr<IDatabase> DatabaseFactory::createPostgresql(const DatabaseConfig& config) {
    auto db = std::make_shared<PostgresqlDatabase>(config);
    if (!db->open()) {
        throw std::runtime_error("Failed to open PostgreSQL database: " + db->getLastError());
    }
    return db;
}
```

### SQLite3 vs PostgreSQL

| 特性 | SQLite3 | PostgreSQL |
|------|---------|------------|
| **配置** | 零配置 | 需要服务器 |
| **部署** | 单文件 | 客户端/服务器 |
| **并发** | 文件锁 | 行级锁 |
| **性能** | 适合读多写少 | 高并发 |
| **适用场景** | 开发/中小规模 | 生产/大规模 |

### 5.6 使用示例

```cpp
#include "database_factory.h"
#include "sqlite_database.h"
#include "postgresql_database.h"

using namespace cycle::database;

// 示例 1: 使用 SQLite3
DatabaseConfig sqliteConfig;
sqliteConfig.type = "sqlite3";
sqliteConfig.sqlite_path = "./data/map.db";

auto sqliteDb = DatabaseFactory::create(sqliteConfig);

// 执行查询
auto result = sqliteDb->query("SELECT * FROM layers");
while (result->next()) {
    auto row = result->getCurrentRow();
    std::string layerName = row.getString(0);
    // 处理数据...
}

// 示例 2: 使用 PostgreSQL
DatabaseConfig pgConfig;
pgConfig.type = "postgresql";
pgConfig.pg_host = "localhost";
pgConfig.pg_port = 5432;
pgConfig.pg_database = "mapdb";
pgConfig.pg_username = "postgres";
pgConfig.pg_password = "secret";

auto pgDb = DatabaseFactory::create(pgConfig);

// 空间查询
Envelope envelope{116.0, 39.0, 117.0, 40.0};
auto spatialResult = pgDb->querySpatial("map_features", envelope);

// 事务处理
pgDb->beginTransaction();
try {
    pgDb->execute("INSERT INTO layers (name, title) VALUES (?, ?)",
                  {SqlParameter::text("new_layer"),
                   SqlParameter::text("New Layer Title")});
    pgDb->commitTransaction();
} catch (...) {
    pgDb->rollbackTransaction();
    throw;
}
```

---

## 6. 多格式图像编码器

### 6.1 设计目标

1. **多格式支持**：支持 PNG8/PNG32/WebP 格式
2. **统一接口**：通过 IEncoder 接口屏蔽格式差异
3. **高性能编码**：优化编码性能，减少内存拷贝
4. **可配置质量**：支持压缩级别、质量参数配置
5. **高 DPI 支持**：自动处理 DPI 相关的元数据

### 6.2 格式定义

#### 6.2.1 ImageFormat 枚举

```cpp
namespace cycle {
namespace encoder {

enum class ImageFormat {
    PNG8,    // 8 位 PNG（带调色板）
    PNG32,   // 32 位 PNG（真彩色 + Alpha）
    WEBP     // WebP 格式
};

// 字符串转换
inline std::string formatToString(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:   return "png8";
        case ImageFormat::PNG32:  return "png32";
        case ImageFormat::WEBP:   return "webp";
        default:                  return "unknown";
    }
}

inline ImageFormat stringToFormat(const std::string& str) {
    if (str == "png8")   return ImageFormat::PNG8;
    if (str == "png32")  return ImageFormat::PNG32;
    if (str == "webp")   return ImageFormat::WEBP;
    return ImageFormat::PNG32;  // 默认
}

// MIME 类型
inline std::string getMimeType(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:
        case ImageFormat::PNG32:  return "image/png";
        case ImageFormat::WEBP:   return "image/webp";
        default:                  return "application/octet-stream";
    }
}

} // namespace encoder
} // namespace cycle
```

#### 6.2.2 EncodeOptions 结构

```cpp
namespace cycle {
namespace encoder {

struct EncodeOptions {
    ImageFormat format = ImageFormat::PNG32;
    
    // PNG 配置
    int pngCompression = 6;  // 1-9
    
    // WebP 配置
    int webpQuality = 80;     // 0-100
    bool webpLossless = false;
    
    // DPI 配置
    int dpiX = 96;
    int dpiY = 96;
    
    // 元数据
    std::string software = "Cycle Map Server";
    std::string copyright = "";
    
    // 验证
    bool isValid() const {
        return pngCompression >= 1 && pngCompression <= 9 &&
               webpQuality >= 0 && webpQuality <= 100 &&
               dpiX > 0 && dpiY > 0;
    }
};

} // namespace encoder
} // namespace cycle
```

### 6.3 编码器接口

#### 6.3.1 IEncoder 接口定义

```cpp
namespace cycle {
namespace encoder {

// 原始图像数据
struct RawImage {
    int width;
    int height;
    int channels;  // 3=RGB, 4=RGBA
    std::vector<uint8_t> data;  // 行优先排列
    
    size_t getDataSize() const {
        return width * height * channels;
    }
};

// 编码结果
struct EncodedImage {
    std::vector<uint8_t> data;
    std::string mimeType;
    size_t size;
    
    size_t getDataSize() const { return data.size(); }
};

// 编码器接口
class IEncoder {
public:
    virtual ~IEncoder() = default;
    
    // 编码
    virtual EncodedImage encode(const RawImage& image,
                                const EncodeOptions& options) = 0;
    
    // 获取支持的格式
    virtual ImageFormat getSupportedFormat() const = 0;
    
    // 获取 MIME 类型
    virtual std::string getMimeType() const = 0;
};

} // namespace encoder
} // namespace cycle
```

### 6.4 PNG 编码器实现

#### 6.4.1 PngEncoder 类

```cpp
namespace cycle {
namespace encoder {

class PngEncoder : public IEncoder {
public:
    explicit PngEncoder(ImageFormat format = ImageFormat::PNG32);
    ~PngEncoder() override;
    
    EncodedImage encode(const RawImage& image,
                        const EncodeOptions& options) override;
    
    ImageFormat getSupportedFormat() const override;
    std::string getMimeType() const override;
    
private:
    ImageFormat format_;
    
    // PNG8 调色板优化
    std::vector<uint8_t> createPalette(const RawImage& image);
    
    // 写入 DPI 信息
    void writePhysicalChunk(png_structp png_ptr,
                           png_infop info_ptr,
                           int dpiX,
                           int dpiY);
};

} // namespace encoder
} // namespace cycle
```

#### 6.4.2 PNG 编码实现要点

```cpp
EncodedImage PngEncoder::encode(const RawImage& image,
                                const EncodeOptions& options) {
    EncodedImage result;
    
    // 创建 PNG 写结构
    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    // 设置错误处理
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw std::runtime_error("PNG encoding failed");
    }
    
    // 初始化 PNG 写
    png_init_io(png_ptr, nullptr);
    
    // 设置图像信息
    int bitDepth = (format_ == ImageFormat::PNG8) ? 8 : 8;
    int colorType = (format_ == ImageFormat::PNG8) ? 
                    PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGBA;
    
    png_set_IHDR(png_ptr, info_ptr,
                 image.width, image.height,
                 bitDepth, colorType,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    // 设置压缩级别
    png_set_compression_level(png_ptr, options.pngCompression);
    
    // 写入 DPI 信息
    writePhysicalChunk(png_ptr, info_ptr, options.dpiX, options.dpiY);
    
    // 写入图像数据
    std::vector<png_bytep> rowPointers(image.height);
    for (int y = 0; y < image.height; ++y) {
        rowPointers[y] = const_cast<png_bytep>(
            &image.data[y * image.width * image.channels]);
    }
    png_set_rows(png_ptr, info_ptr, &rowPointers[0]);
    
    // 编码
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    
    // 清理
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    result.mimeType = getMimeType();
    result.size = result.data.size();
    return result;
}
```

### 6.5 WebP 编码器实现

#### 6.5.1 WebpEncoder 类

```cpp
namespace cycle {
namespace encoder {

class WebpEncoder : public IEncoder {
public:
    WebpEncoder() = default;
    ~WebpEncoder() override;
    
    EncodedImage encode(const RawImage& image,
                        const EncodeOptions& options) override;
    
    ImageFormat getSupportedFormat() const override;
    std::string getMimeType() const override;
    
private:
    // 有损编码
    EncodedImage encodeLossy(const RawImage& image,
                            int quality);
    
    // 无损编码
    EncodedImage encodeLossless(const RawImage& image);
};

} // namespace encoder
} // namespace cycle
```

#### 6.5.2 WebP 编码实现要点

```cpp
EncodedImage WebpEncoder::encode(const RawImage& image,
                                 const EncodeOptions& options) {
    if (options.webpLossless) {
        return encodeLossless(image);
    } else {
        return encodeLossy(image, options.webpQuality);
    }
}

EncodedImage WebpEncoder::encodeLossy(const RawImage& image,
                                      int quality) {
    EncodedImage result;
    
    // 确保 RGBA 格式
    if (image.channels != 4) {
        throw std::runtime_error("WebP requires RGBA format");
    }
    
    // 编码
    size_t webpSize = WebPEncodeRGBA(
        image.data.data(),
        image.width,
        image.height,
        image.width * 4,
        static_cast<float>(quality),
        &result.data.data());
    
    if (webpSize == 0) {
        throw std::runtime_error("WebP encoding failed");
    }
    
    result.data.resize(webpSize);
    result.mimeType = getMimeType();
    result.size = webpSize;
    return result;
}
```

### 6.6 EncoderFactory 工厂类

#### 6.6.1 工厂模式实现

```cpp
namespace cycle {
namespace encoder {

class EncoderFactory {
public:
    static std::shared_ptr<IEncoder> create(ImageFormat format);
    static std::shared_ptr<IEncoder> create(const std::string& formatStr);
    
private:
    static std::shared_ptr<IEncoder> createPng(ImageFormat format);
    static std::shared_ptr<IEncoder> createWebp();
};

} // namespace encoder
} // namespace cycle
```

#### 6.6.2 工厂实现

```cpp
std::shared_ptr<IEncoder> EncoderFactory::create(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:
        case ImageFormat::PNG32:
            return createPng(format);
        case ImageFormat::WEBP:
            return createWebp();
        default:
            throw std::runtime_error("Unsupported format");
    }
}

std::shared_ptr<IEncoder> EncoderFactory::create(const std::string& formatStr) {
    return create(stringToFormat(formatStr));
}

std::shared_ptr<IEncoder> EncoderFactory::createPng(ImageFormat format) {
    return std::make_shared<PngEncoder>(format);
}

std::shared_ptr<IEncoder> EncoderFactory::createWebp() {
    return std::make_shared<WebpEncoder>();
}
```
### 格式对比

| 格式 | 优点 | 缺点 | 适用场景 |
|------|------|------|---------|
| **PNG8** | 文件小 (<100KB) | 颜色有限 (256 色) | 简单地图、移动端 |
| **PNG32** | 高质量、支持 alpha | 文件较大 | 标准地图、高质量 |
| **WebP** | 高压缩率 (小 30%) | 编码稍慢 | 网络传输、存储 |

### 6.7 高 DPI 支持

#### 6.7.1 DPI 处理逻辑

```cpp
namespace cycle {

class DpiHandler {
public:
    static Envelope calculateEnvelope(
        const Envelope& baseEnvelope,
        int baseDpi,
        int targetDpi) {
        
        double scale = static_cast<double>(baseDpi) / targetDpi;
        
        double centerX = (baseEnvelope.minX + baseEnvelope.maxX) / 2.0;
        double centerY = (baseEnvelope.minY + baseEnvelope.maxY) / 2.0;
        
        double newWidth = baseEnvelope.width() * scale;
        double newHeight = baseEnvelope.height() * scale;
        
        return Envelope{
            centerX - newWidth / 2.0,
            centerY - newHeight / 2.0,
            centerX + newWidth / 2.0,
            centerY + newHeight / 2.0
        };
    }
    
    static int calculateImageSize(int baseSize, int baseDpi, int targetDpi) {
        return static_cast<int>(
            baseSize * static_cast<double>(targetDpi) / baseDpi + 0.5);
    }
    
    static bool validateDpi(int dpi, int minDpi, int maxDpi) {
        return dpi >= minDpi && dpi <= maxDpi;
    }
};

} // namespace cycle
```

#### 6.7.2 使用示例

```cpp
// 基础请求：256x256 @ 96 DPI
Envelope baseEnvelope{116.0, 39.0, 116.1, 39.1};
int baseSize = 256;
int baseDpi = 96;

// 高 DPI 请求：512x512 @ 192 DPI
int targetDpi = 192;
Envelope highDpiEnvelope = DpiHandler::calculateEnvelope(
    baseEnvelope, baseDpi, targetDpi);
int targetSize = DpiHandler::calculateImageSize(
    baseSize, baseDpi, targetDpi);

// 渲染
RawImage image;
image.width = targetSize;
image.height = targetSize;
image.channels = 4;
image.data = renderMap(highDpiEnvelope, targetSize);

// 编码
EncodeOptions options;
options.format = ImageFormat::PNG32;
options.dpiX = targetDpi;
options.dpiY = targetDpi;

auto encoder = EncoderFactory::create(ImageFormat::PNG32);
EncodedImage result = encoder->encode(image, options);
```

---

## 7. 缓存设计

### 7.1 缓存策略

采用**两级缓存**架构：
1. **L1 缓存**：内存缓存，存储热点数据
2. **L2 缓存**：磁盘缓存，存储历史数据

### 7.2 缓存键设计

```cpp
namespace cycle {

struct CacheKey {
    std::string layer;
    std::string tileMatrixSet;
    int tileMatrix;
    int tileRow;
    int tileCol;
    std::string format;
    int dpi;
    
    std::string generate() const {
        std::ostringstream oss;
        oss << layer << ":"
            << tileMatrixSet << ":"
            << tileMatrix << ":"
            << tileRow << ":"
            << tileCol << ":"
            << format << ":"
            << dpi;
        return oss.str();
    }
    
    static CacheKey parse(const std::string& key) {
        CacheKey cacheKey;
        std::istringstream iss(key);
        std::getline(iss, cacheKey.layer, ':');
        std::getline(iss, cacheKey.tileMatrixSet, ':');
        std::getline(iss, std::to_string(cacheKey.tileMatrix), ':');
        std::getline(iss, std::to_string(cacheKey.tileRow), ':');
        std::getline(iss, std::to_string(cacheKey.tileCol), ':');
        std::getline(iss, cacheKey.format, ':');
        std::getline(iss, std::to_string(cacheKey.dpi), ':');
        return cacheKey;
    }
};

} // namespace cycle
```

### 7.3 内存缓存实现

```cpp
namespace cycle {

class MemoryCache {
public:
    explicit MemoryCache(size_t maxSizeBytes);
    
    bool get(const std::string& key, std::vector<uint8_t>& value);
    void put(const std::string& key, const std::vector<uint8_t>& value);
    void remove(const std::string& key);
    void clear();
    
    size_t getSize() const;
    size_t getMaxSize() const;
    
private:
    void evictIfNeeded();
    
    std::unordered_map<std::string, std::vector<uint8_t>> cache_;
    std::list<std::string> lruList_;  // LRU 列表
    std::unordered_map<std::string, std::list<std::string>::iterator> lruMap_;
    size_t currentSize_;
    size_t maxSize_;
    mutable std::mutex mutex_;
};

} // namespace cycle
```

### 7.4 磁盘缓存实现

```cpp
namespace cycle {

class DiskCache {
public:
    explicit DiskCache(const std::string& basePath, int ttlSeconds);
    
    bool get(const std::string& key, std::vector<uint8_t>& value);
    void put(const std::string& key, const std::vector<uint8_t>& value);
    void remove(const std::string& key);
    void clear();
    void cleanupExpired();
    
private:
    std::string getKeyPath(const std::string& key) const;
    bool isExpired(const std::string& keyPath) const;
    
    std::string basePath_;
    int ttlSeconds_;
    std::mutex mutex_;
};

} // namespace cycle
```

### 7.5 缓存管理器

```cpp
namespace cycle {

class CacheManager {
public:
    CacheManager(std::shared_ptr<MemoryCache> memCache,
                 std::shared_ptr<DiskCache> diskCache);
    
    bool get(const std::string& key, std::vector<uint8_t>& value);
    void put(const std::string& key, const std::vector<uint8_t>& value);
    
    CacheStats getStats() const;
    
private:
    std::shared_ptr<MemoryCache> memCache_;
    std::shared_ptr<DiskCache> diskCache_;
    CacheStats stats_;
};

} // namespace cycle
```

---

## 8. API 设计

### 8.1 WMTS API

#### 8.1.1 GetCapabilities

**请求**:
```
GET /1.0.0/WMTSCapabilities.xml
```

**响应**:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<Capabilities version="1.0.0" 
              xmlns="http://www.opengis.net/wmts/1.0"
              xmlns:ows="http://www.opengis.net/ows/1.1">
  <ows:ServiceIdentification>
    <ows:Title>Cycle Map Server</ows:Title>
    <ows:ServiceType>OGC WMTS</ows:ServiceType>
    <ows:ServiceTypeVersion>1.0.0</ows:ServiceTypeVersion>
  </ows:ServiceIdentification>
  
  <Contents>
    <Layer>
      <ows:Title>Default Layer</ows:Title>
      <ows:Identifier>default</ows:Identifier>
      <Style isDefault="true">
        <ows:Identifier>default</ows:Identifier>
      </Style>
      <Format>image/png</Format>
      <TileMatrixSetLink>
        <TileMatrixSet>EPSG:3857</TileMatrixSet>
      </TileMatrixSetLink>
      <ResourceURL template="/1.0.0/default/EPSG:3857/{TileMatrix}/{TileRow}/{TileCol}.png"/>
    </Layer>
    
    <TileMatrixSet>
      <ows:Identifier>EPSG:3857</ows:Identifier>
      <ows:SupportedCRS>urn:ogc:def:crs:EPSG:3857</ows:SupportedCRS>
      <TileMatrix>
        <ows:Identifier>0</ows:Identifier>
        <ScaleDenominator>559082264.029</ScaleDenominator>
        <TopLeftCorner>-20037508.3428 20037508.3428</TopLeftCorner>
        <TileWidth>256</TileWidth>
        <TileHeight>256</TileHeight>
        <MatrixWidth>1</MatrixWidth>
        <MatrixHeight>1</MatrixHeight>
      </TileMatrix>
      <!-- More TileMatrix entries -->
    </TileMatrixSet>
  </Contents>
</Capabilities>
```

#### 8.1.2 GetTile

**请求**:
```
GET /1.0.0/{Layer}/{TileMatrixSet}/{TileMatrix}/{TileRow}/{TileCol}.{format}
GET /1.0.0/{Layer}/{TileMatrixSet}/{TileMatrix}/{TileRow}/{TileCol}.{format}?dpi={dpi}
```

**参数**:
| 参数 | 类型 | 必需 | 描述 |
|------|------|------|------|
| Layer | string | 是 | 图层名称 |
| TileMatrixSet | string | 是 | 瓦片矩阵集 |
| TileMatrix | int | 是 | 缩放级别 (0-22) |
| TileRow | int | 是 | 瓦片行号 |
| TileCol | int | 是 | 瓦片列号 |
| format | string | 是 | 格式 (png8/png32/webp) |
| dpi | int | 否 | DPI (72-600, 默认 96) |

**成功响应**:
- **状态码**: 200 OK
- **Content-Type**: image/png 或 image/webp
- **Body**: 二进制图像数据

**错误响应**:
```json
{
  "error": {
    "code": "TileOutOfRange",
    "message": "Tile row or column out of range",
    "details": {
      "layer": "default",
      "tileMatrix": 0,
      "tileRow": 10,
      "tileCol": 10
    }
  }
}
```

### 8.2 管理 API

#### 8.2.1 健康检查

**请求**:
```
GET /health
```

**响应**:
```json
{
  "status": "healthy",
  "timestamp": "2026-03-09T12:34:56Z",
  "version": "2.0.0",
  "uptime": 3600,
  "metrics": {
    "requests_total": 10000,
    "cache_hit_rate": 0.95,
    "avg_render_time_ms": 285
  }
}
```

#### 8.2.2 性能指标

**请求**:
```
GET /metrics
```

**响应**:
```json
{
  "requests": {
    "total": 10000,
    "success": 9850,
    "failed": 150
  },
  "cache": {
    "memory_hits": 8500,
    "disk_hits": 1000,
    "misses": 500,
    "hit_rate": 0.95
  },
  "rendering": {
    "avg_time_ms": 285,
    "min_time_ms": 120,
    "max_time_ms": 890,
    "p95_time_ms": 350
  },
  "database": {
    "connections_active": 5,
    "connections_max": 10,
    "queries_total": 15000
  }
}
```

---

#### GET /layers

**描述**：获取可用图层列表

**响应**：
```json
{
  "layers": [
    {
      "id": 1,
      "name": "roads",
      "type": "line",
      "description": "道路网络",
      "min_zoom": 0,
      "max_zoom": 18,
      "style": {
        "stroke_color": "#333333",
        "stroke_width": 2
      },
      "feature_count": 15234
    }
  ]
}
```

#### POST /generate

**请求**：
```json
{
  "bbox": [-74.006, 40.7128, -73.935, 40.8076],
  "width": 1024,
  "height": 768,
  "layers": ["roads", "buildings"],
  "background_color": "#ffffff",
  "filter": "name LIKE '%park%'",
  "quality": 90
}
```

**成功响应**：
- Status: 200 OK
- Content-Type: image/png
- Body: PNG 图像二进制数据

**错误响应**：
```json
{
  "success": false,
  "error": "invalid_parameter",
  "message": "Width must be between 1 and 4096",
  "details": {
    "parameter": "width",
    "value": "5000",
    "constraint": "max_image_width"
  }
}
```

#### GET /tile/{z}/{x}/{y}.png

**描述**：获取标准 XYZ 瓦片

**参数**：
- `z` (int): 缩放级别 (0-18)
- `x` (int): 瓦片 X 坐标
- `y` (int): 瓦片 Y 坐标

**响应**：
- Status: 200 OK
- Content-Type: image/png
- Cache-Control: max-age=3600
- Body: 256x256 PNG 图像


## 8a 渲染范围控制

### 范围限制配置

```cpp
namespace cycle {

struct RangeLimitConfig {
    // 范围限制（度）
    double max_bbox_width = 360.0;
    double max_bbox_height = 180.0;
    double min_bbox_width = 0.0001;
    double min_bbox_height = 0.0001;
    
    // 尺寸限制（像素）
    int max_image_width = 8192;
    int max_image_height = 8192;
    int min_image_width = 64;
    int min_image_height = 64;
    
    // DPI 范围
    int min_dpi = 72;
    int max_dpi = 600;
    
    // 验证方法
    bool ValidateBoundingBox(const BoundingBox& bbox) const;
    bool ValidateImageSize(int width, int height) const;
    bool ValidateDpi(int dpi) const;
};

} // namespace cycle
```

### API 参数验证

```json
// 请求示例
{
  "bbox": [-74.006, 40.7128, -73.935, 40.8076],
  "width": 1024,
  "height": 768,
  "format": "webp",
  "quality": 85,
  "dpi": 192,
  "layers": ["roads", "buildings"]
}
```

### 错误响应

```json
{
  "success": false,
  "error": "invalid_parameter",
  "message": "DPI value 800 exceeds maximum 600",
  "details": {
    "parameter": "dpi",
    "value": "800",
    "constraint": "max_dpi"
  }
}
```


## 9. 错误处理

### 9.1 错误分类

| 类别 | 错误码范围 | 描述 |
|------|-----------|------|
| 客户端错误 | 400-499 | 请求参数错误、认证失败等 |
| 服务器错误 | 500-599 | 内部错误、数据库错误等 |
| 业务错误 | 600-699 | 瓦片不存在、范围超限等 |

### 9.2 错误响应格式

```cpp
namespace cycle {

struct ErrorInfo {
    int code;
    std::string message;
    std::string details;
    std::string timestamp;
    std::string path;
};

class ErrorHandler {
public:
    static std::string toJson(const ErrorInfo& error);
    static httplib::Response toResponse(const ErrorInfo& error);
    
    static ErrorInfo createBadRequest(const std::string& message);
    static ErrorInfo createNotFound(const std::string& resource);
    static ErrorInfo createInternalError(const std::string& message);
    static ErrorInfo createTileNotFound(const std::string& layer,
                                        int tileMatrix,
                                        int tileRow,
                                        int tileCol);
    static ErrorInfo createRangeExceeded(const std::string& reason);
};

} // namespace cycle
```

### 9.3 错误处理示例

```json
{
  "error": {
    "code": 400,
    "message": "Invalid tile matrix",
    "details": "Tile matrix must be between 0 and 22",
    "timestamp": "2026-03-09T12:34:56Z",
    "path": "/1.0.0/default/EPSG:3857/25/100/200.png"
  }
}
```

---

## 10. 性能优化

### 10.1 性能指标 (v2.0)

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 单图渲染时间 | < 350ms | P95 延迟 |
| 并发请求处理 | > 200 req/s | 持续压力测试 |
| 缓存命中率 | > 95% | (命中数/总请求数) |
| 内存占用 | < 2GB | 稳态运行 |
| 启动时间 | < 5s | 冷启动到就绪 |

### 10.2 渲染优化

1. **任务队列**：RenderTaskQueue 管理渲染任务
2. **工作线程池**：多线程并行处理
3. **批量渲染**：合并绘制调用
4. **视口裁剪**：只渲染可见范围

#### 10.2.1 渲染任务队列

```cpp
namespace cycle {

class RenderTaskQueue {
public:
    explicit RenderTaskQueue(int maxConcurrency);
    
    void enqueue(const TileRequest& request,
                 std::function<void(const TileResponse&)> callback);
    
    void shutdown();
    
private:
    void workerThread();
    
    struct Task {
        TileRequest request;
        std::function<void(const TileResponse&)> callback;
    };
    
    std::queue<Task> taskQueue_;
    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_;
    int maxConcurrency_;
    std::atomic<int> activeTasks_;
    std::mutex concurrencyMutex_;
    std::condition_variable concurrencyCv_;
};

} // namespace cycle
```

#### 10.2.2 批量渲染器

```cpp
namespace cycle {

class BatchRenderer {
public:
    explicit BatchRenderer(std::shared_ptr<IDatabase> db);
    
    // 批量渲染多个瓦片
    std::vector<TileResponse> renderBatch(
        const std::vector<TileRequest>& requests);
    
private:
    std::shared_ptr<IDatabase> database_;
    
    // 批量空间查询
    std::unordered_map<std::string, std::vector<Feature>> 
    queryFeaturesBatch(const std::vector<Envelope>& envelopes);
};

} // namespace cycle
```

### 10.3 缓存优化

#### 10.3.1 预加载策略

```cpp
namespace cycle {

class CachePreloader {
public:
    CachePreloader(std::shared_ptr<CacheManager> cache,
                   std::shared_ptr<MapService> service);
    
    void preloadTile(const std::string& layer,
                     int tileMatrix,
                     int tileRow,
                     int tileCol);
    
    void preloadArea(const std::string& layer,
                     int tileMatrix,
                     int minRow, int maxRow,
                     int minCol, int maxCol);
    
private:
    std::shared_ptr<CacheManager> cache_;
    std::shared_ptr<MapService> service_;
};

} // namespace cycle
```

### 10.4 数据库优化

#### 10.4.1 连接池

```cpp
namespace cycle {
namespace database {

class ConnectionPool {
public:
    ConnectionPool(std::function<std::shared_ptr<IDatabase>()> factory,
                   size_t poolSize);
    
    std::shared_ptr<IDatabase> acquire();
    void release(std::shared_ptr<IDatabase> conn);
    
private:
    std::vector<std::shared_ptr<IDatabase>> connections_;
    std::queue<std::shared_ptr<IDatabase>> available_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t poolSize_;
};

} // namespace database
} // namespace cycle
```

#### 10.4.2 查询优化

```sql
-- 创建空间索引
CREATE SPATIAL INDEX ON map_features(geometry);

-- 使用覆盖索引
CREATE INDEX idx_features_layer_geom 
ON map_features(layer_id, geometry);

-- 查询优化
SELECT id, geometry, attributes
FROM map_features
WHERE layer_id = ?
  AND geometry && BuildMbr(?, ?, ?, ?, 4326)
```

### 10.5 并发控制
1. **Semaphore**：信号量控制并发数
2. **ReadWriteLock**：读写锁优化读多写少场景
3. **细粒度锁**：减少锁竞争
#### 10.5.1 信号量实现

```cpp
namespace cycle {

class Semaphore {
public:
    explicit Semaphore(int count);
    
    void acquire();
    void release();
    
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

} // namespace cycle
```

#### 10.5.2 读写锁

```cpp
namespace cycle {

class ReadWriteLock {
public:
    void lockRead();
    void unlockRead();
    void lockWrite();
    void unlockWrite();
    
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int readers_;
    bool writer_;
};

} // namespace cycle
```

---

## 11. 安全设计

### 11.1 认证机制

#### 11.1.1 API Key 认证

```cpp
namespace cycle {

class ApiKeyAuth {
public:
    explicit ApiKeyAuth(const std::unordered_set<std::string>& validKeys);
    
    bool authenticate(const std::string& apiKey);
    bool authenticate(const httplib::Request& req);
    
private:
    std::unordered_set<std::string> validKeys_;
};

} // namespace cycle
```

#### 11.1.2 JWT 认证

```cpp
namespace cycle {

class JwtAuth {
public:
    explicit JwtAuth(const std::string& secret);
    
    bool validate(const std::string& token);
    std::map<std::string, std::string> decode(const std::string& token);
    
private:
    std::string secret_;
};

} // namespace cycle
```

### 11.2 输入验证

#### 11.2.1 参数验证

```cpp
namespace cycle {

class InputValidator {
public:
    static bool validateTileMatrix(int tileMatrix, int maxZoom);
    static bool validateTileRow(int tileRow, int tileMatrix);
    static bool validateTileCol(int tileCol, int tileMatrix);
    static bool validateDpi(int dpi, int minDpi, int maxDpi);
    static bool validateFormat(const std::string& format);
    static bool validateLayer(const std::string& layer);
    
private:
    static const std::unordered_set<std::string> allowedFormats_;
};

} // namespace cycle
```

### 11.3 SQL 注入防护

```cpp
// 错误示例 - 禁止使用
std::string sql = "SELECT * FROM layers WHERE name = '" + layerName + "'";

// 正确示例 - 使用参数化查询
std::string sql = "SELECT * FROM layers WHERE name = ?";
database->execute(sql, {SqlParameter::text(layerName.c_str())});
```

### 11.4 速率限制

```cpp
namespace cycle {

class RateLimiter {
public:
    RateLimiter(int maxRequests, int windowSeconds);
    
    bool allowRequest(const std::string& clientId);
    
private:
    struct ClientData {
        std::deque<std::chrono::steady_clock::time_point> timestamps;
    };
    
    std::unordered_map<std::string, ClientData> clients_;
    int maxRequests_;
    int windowSeconds_;
    std::mutex mutex_;
};

} // namespace cycle
```

---

## 12. 监控与日志

### 12.1 日志系统设计

#### 12.1.1 日志级别

```cpp
namespace cycle {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4
};

class Logger {
public:
    static void init(const LogConfig& config);
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    
private:
    static void log(LogLevel level, const std::string& message);
    static void rotateLogs();
    
    static std::ofstream logFile_;
    static LogLevel minLevel_;
    static bool consoleOutput_;
};

} // namespace cycle
```

#### 12.1.2 日志格式

```
[2026-03-09 12:34:56.789] [INFO] [12345] Request: GET /1.0.0/default/EPSG:3857/10/500/600.png - 200 OK (285ms)
[2026-03-09 12:34:57.123] [ERROR] [12346] Database error: Failed to execute query - timeout after 30s
```

### 12.2 性能监控

#### 12.2.1 指标收集

```cpp
namespace cycle {

class MetricsCollector {
public:
    void recordRequest(const std::string& path,
                       int statusCode,
                       int64_t durationMs);
    
    void recordCacheHit(bool hit);
    void recordRenderTime(int64_t durationMs);
    void recordDatabaseQuery(int64_t durationMs);
    
    MetricsSnapshot getSnapshot() const;
    
private:
    struct MetricsData {
        std::atomic<int64_t> totalRequests{0};
        std::atomic<int64_t> failedRequests{0};
        std::atomic<int64_t> cacheHits{0};
        std::atomic<int64_t> cacheMisses{0};
        std::vector<int64_t> renderTimes;
        std::vector<int64_t> queryTimes;
    };
    
    MetricsData data_;
    mutable std::mutex mutex_;
};

} // namespace cycle
```

---

## 13. 部署方案

### 13.1 Docker 部署

#### 13.1.1 Dockerfile

```dockerfile
# 多阶段构建
FROM ubuntu:22.04 AS builder

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    libsqlite3-dev \
    libsqlite3-mod-spatialite \
    libpng-dev \
    libwebp-dev \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# 构建应用
WORKDIR /build
COPY . .
RUN mkdir -p build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j$(nproc)

# 运行阶段
FROM ubuntu:22.04

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libsqlite3-mod-spatialite \
    libpng16-16 \
    libwebp7 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

# 创建非 root 用户
RUN useradd -m -u 1000 cycle
USER cycle

WORKDIR /app
COPY --from=builder /build/build/cycle-map-server .
COPY --from=builder /build/config.json .

# 创建数据目录
RUN mkdir -p /app/data /app/cache /app/logs

EXPOSE 8080

CMD ["./cycle-map-server", "--config", "config.json"]
```

#### 13.1.2 Docker Compose

```yaml
version: '3.8'

services:
  map-server:
    build: .
    ports:
      - "8080:8080"
    volumes:
      - ./data:/app/data
      - ./cache:/app/cache
      - ./logs:/app/logs
      - ./config.json:/app/config.json:ro
    environment:
      - DB_TYPE=sqlite3
      - LOG_LEVEL=info
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  # PostgreSQL 选项（可选）
  postgres:
    image: postgis/postgis:14-3.2
    environment:
      POSTGRES_DB: mapdb
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: secret
    volumes:
      - pgdata:/var/lib/postgresql/data
    ports:
      - "5432:5432"
    profiles:
      - postgres

volumes:
  pgdata:
```

### 13.2 系统服务部署

#### 13.2.1 systemd 服务配置

```ini
[Unit]
Description=Cycle Map Server
After=network.target

[Service]
Type=simple
User=cycle
Group=cycle
WorkingDirectory=/opt/cycle-map-server
ExecStart=/opt/cycle-map-server/cycle-map-server --config /opt/cycle-map-server/config.json
Restart=on-failure
RestartSec=5
LimitNOFILE=65536

[Install]
WantedBy=multi-user.target
```

### 13.3 依赖安装

#### 13.3.1 Ubuntu/Debian

```bash
# 必需依赖
sudo apt-get update
sudo apt-get install -y \
    cmake \
    g++ \
    libsqlite3-dev \
    libsqlite3-mod-spatialite \
    libpng-dev \
    libwebp-dev

# PostgreSQL 支持（可选）
sudo apt-get install -y \
    libpq-dev \
    postgresql-14-postgis-3

# 构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### 13.3.2 Windows

```powershell
# 使用 vcpkg 安装依赖
vcpkg install sqlite3[core,spatialite]:x64-windows
vcpkg install libpng:x64-windows
vcpkg install libwebp:x64-windows
vcpkg install postgresql:x64-windows

# 使用 Visual Studio 2019+ 构建
cmake -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
```

#### 13.3.3 macOS

```bash
# 使用 Homebrew 安装依赖
brew install cmake sqlite3 libpng libwebp postgresql

# 构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

---

## 14. 开发规范

### 14.1 C++11 规范

#### 14.1.1 语言特性使用

1. **智能指针**：
   ```cpp
   // 推荐
   std::unique_ptr<Database> db = std::make_unique<SqliteDatabase>(path);
   std::shared_ptr<Encoder> encoder = std::make_shared<PngEncoder>();
   
   // 不推荐
   Database* db = new SqliteDatabase(path);
   delete db;
   ```

2. **auto 关键字**：
   ```cpp
   // 推荐
   auto result = database->query(sql);
   for (const auto& row : *result) { ... }
   
   // 不推荐（类型明显时）
   std::unique_ptr<ResultSet> result = database->query(sql);
   ```

3. **Lambda 表达式**：
   ```cpp
   // 推荐
   std::sort(layers.begin(), layers.end(),
             [](const Layer& a, const Layer& b) {
                 return a.name < b.name;
             });
   
   // 捕获列表
   [this, &cache](const TileRequest& req) {
       return cache->get(req.generateKey());
   }
   ```

4. **范围 for 循环**：
   ```cpp
   // 推荐
   for (const auto& param : params) { ... }
   
   // 不推荐
   for (size_t i = 0; i < params.size(); ++i) { ... }
   ```

5. **nullptr**：
   ```cpp
   // 推荐
   if (ptr == nullptr) { ... }
   
   // 不推荐
   if (ptr == NULL) { ... }
   if (!ptr) { ... }  // 除非是布尔语义
   ```

#### 14.1.2 命名空间规范

```cpp
// 根命名空间
namespace cycle {

// 子命名空间按功能划分
namespace database {
    // 数据库相关类
}

namespace encoder {
    // 编码器相关类
}

namespace renderer {
    // 渲染相关类
}

} // namespace cycle

// 使用示例
using namespace cycle::database;
auto db = DatabaseFactory::create(config);
```

命名空间组织

```
cycle                      # 根命名空间
├── database               # 数据库相关
│   ├── IDatabase
│   ├── SqliteDatabase
│   ├── PostgresqlDatabase
│   └── DatabaseFactory
├── encoder                # 编码器相关
│   ├── IEncoder
│   ├── PngEncoder
│   ├── WebpEncoder
│   └── EncoderFactory
├── render                # 编码器相关
├── utils                  # 工具函数
└── types                  # 公共类型
```

---

### 14.2 代码风格

#### 14.2.1 命名规范

```cpp
// 类名：大驼峰
class MapService { };

// 函数名：小驼峰
void handleRequest() { }

// 变量名：小写 + 下划线
int tile_matrix = 0;

// 常量：全大写 + 下划线
const int MAX_TILE_SIZE = 4096;

// 私有成员：下划线后缀
int width_;
std::string configPath_;
```

#### 14.2.2 文件组织

```cpp
// 头文件
#ifndef CYCLE_MAP_SERVICE_H
#define CYCLE_MAP_SERVICE_H

#include <memory>
#include <string>

namespace cycle {

class MapService {
public:
    // 公共接口
private:
    // 私有成员
};

} // namespace cycle

#endif // CYCLE_MAP_SERVICE_H

// 源文件
#include "map_service.h"

namespace cycle {

MapService::MapService() {
    // 实现
}

} // namespace cycle
```

### 14.3 文档规范

#### 14.3.1 Doxygen 注释

```cpp
/**
 * @brief 获取地图瓦片
 * 
 * @param request 瓦片请求参数
 * @return TileResponse 瓦片响应
 * 
 * @throws std::runtime_error 渲染失败
 * 
 * @example
 * TileRequest request;
 * request.layer = "default";
 * auto response = service->getTile(request);
 */
TileResponse getTile(const TileRequest& request);
```

---

## 15. 测试策略

### 15.1 单元测试

#### 15.1.1 测试框架

使用 Google Test 或 Catch2：

```cpp
#include <gtest/gtest.h>
#include "database/sqlite_database.h"
#include "encoder/png_encoder.h"

using namespace cycle;

TEST(SqliteDatabaseTest, OpenClose) {
    SqliteDatabase db(":memory:");
    EXPECT_TRUE(db.open());
    EXPECT_TRUE(db.isOpen());
    db.close();
    EXPECT_FALSE(db.isOpen());
}

TEST(PngEncoderTest, EncodeBasic) {
    PngEncoder encoder;
    RawImage image{256, 256, 4, std::vector<uint8_t>(256 * 256 * 4)};
    EncodeOptions options;
    
    auto result = encoder.encode(image, options);
    
    EXPECT_GT(result.size, 0);
    EXPECT_EQ(result.mimeType, "image/png");
}
```

### 15.2 集成测试

#### 15.2.1 HTTP API 测试

```cpp
TEST_F(ServerTestFixture, GetTileSuccess) {
    httplib::Client cli("http://localhost:8080");
    
    auto res = cli.Get("/1.0.0/default/EPSG:3857/10/500/600.png");
    
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 200);
    EXPECT_EQ(res->get_header_value("Content-Type"), "image/png");
}

TEST_F(ServerTestFixture, GetTileInvalidParams) {
    httplib::Client cli("http://localhost:8080");
    
    auto res = cli.Get("/1.0.0/default/EPSG:3857/25/100/200.png");
    
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 400);
}
```

### 15.3 性能测试

#### 15.3.1 压力测试脚本

```bash
#!/bin/bash
# 使用 Apache Bench 进行压力测试

# 并发 100，总请求 10000
ab -n 10000 -c 100 \
   -p tile_request.json \
   -T application/json \
   http://localhost:8080/1.0.0/default/EPSG:3857/10/500/600.png

# 输出统计信息
# Requests per second, Time per request, etc.
```

---

## 16. 风险评估与缓解

### 16.1 技术风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| **内存泄漏** | 中 | 高 | 使用智能指针，定期 valgrind 检测 |
| **数据库死锁** | 低 | 高 | 使用连接池，设置超时，避免长事务 |
| **编码性能不足** | 中 | 中 | 使用多线程编码，优化压缩参数 |
| **缓存雪崩** | 低 | 高 | 设置随机 TTL，预加载热点数据 |
| **DPI 计算误差** | 低 | 中 | 使用双精度计算，添加边界检查 |
| **C++11 兼容性问题** | 低 | 中 | 严格遵循 C++11 标准，多平台测试 |

### 16.2 运维风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| **磁盘空间不足** | 中 | 高 | 监控磁盘使用，自动清理过期缓存 |
| **内存溢出** | 中 | 高 | 限制缓存大小，设置内存告警 |
| **数据库损坏** | 低 | 高 | 定期备份，使用 WAL 模式 |
| **服务宕机** | 低 | 高 | 健康检查，自动重启，集群部署 |

### 16.3 安全风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| **SQL 注入** | 中 | 高 | 使用参数化查询，输入验证 |
| **DDoS 攻击** | 中 | 高 | 速率限制，IP 黑名单 |
| **未授权访问** | 中 | 高 | API Key 认证，权限控制 |
| **数据泄露** | 低 | 高 | 加密敏感数据，访问日志审计 |

---

## 17. 迁移指南

### 17.1 从 v1.0 迁移到 v2.0

#### 17.1.1 配置文件更新

```json
// v1.0 配置
{
  "database_path": "./data/map.db",
  "encoder_format": "png"
}

// v2.0 配置
{
  "database": {
    "type": "sqlite3",
    "sqlite_path": "./data/map.db",
    "max_connections": 10
  },
  "encoder": {
    "format": "png32",
    "png_compression": 6,
    "min_dpi": 72,
    "max_dpi": 600
  },
  "range_limit": {
    "enabled": true,
    "max_width": 4096,
    "max_height": 4096
  }
}
```

#### 17.1.2 API 变更

| v1.0 API | v2.0 API | 变更说明 |
|----------|----------|----------|
| `/tile/{z}/{x}/{y}.png` | `/1.0.0/{layer}/{matrixSet}/{z}/{x}/{y}.png` | 支持多图层 |
| N/A | `/1.0.0/{layer}/{matrixSet}/{z}/{x}/{y}.png?dpi=192` | 新增 DPI 参数 |
| N/A | `/1.0.0/{layer}/{matrixSet}/{z}/{x}/{y}.webp` | 新增 WebP 格式 |

#### 17.1.3 代码迁移

```cpp
// v1.0 代码
auto db = std::make_shared<SqliteDatabase>(path);

// v2.0 代码
DatabaseConfig config;
config.type = "sqlite3";
config.sqlite_path = path;
auto db = DatabaseFactory::create(config);

// v1.0 代码
PngEncoder encoder;

// v2.0 代码
auto encoder = EncoderFactory::create("png32");
```

### 17.2 数据库迁移

#### 17.2.1 SQLite3 到 PostgreSQL

```bash
# 导出 SQLite3 数据
sqlite3 map.db .dump > dump.sql

# 导入到 PostgreSQL
psql -h localhost -U postgres -d mapdb -f dump.sql

# 启用 PostGIS
psql -h localhost -U postgres -d mapdb -c "CREATE EXTENSION postgis"
```

---

## 18. 验收标准

### 18.1 功能验收

- [ ] WMTS 1.0.0 标准兼容
- [ ] 支持 SQLite3/SpatiaLite 后端
- [ ] 支持 PostgreSQL/PostGIS 后端
- [ ] 支持 PNG8/PNG32/WebP 格式
- [ ] 支持 72-600 DPI 范围
- [ ] 支持渲染范围限制
- [ ] 缓存命中率 > 95%
- [ ] 健康检查 API 正常

### 18.2 性能验收

- [ ] 单图渲染时间 P95 < 350ms
- [ ] 并发处理能力 > 200 req/s
- [ ] 内存占用 < 2GB (稳态)
- [ ] 启动时间 < 5s
- [ ] 缓存命中率 > 95%

### 18.3 质量验收

- [ ] 单元测试覆盖率 > 90%
- [ ] 无内存泄漏 (valgrind 检测)
- [ ] 无严重安全漏洞
- [ ] 跨平台测试通过 (Windows/Linux/macOS)
- [ ] C++11 兼容性验证通过
- [ ] 文档完整率 > 95%

---

## 19. 附录

### 19.1 参考资料

1. **OGC WMTS 标准**: https://www.ogc.org/standards/wmts
2. **cpp-httplib**: https://github.com/yhirose/cpp-httplib
3. **SpatiaLite 文档**: https://www.gaia-gis.it/fossil/libspatialite
4. **PostGIS 文档**: https://postgis.net/documentation/
5. **libpng 文档**: http://www.libpng.org/pub/png/libpng.html
6. **WebP 文档**: https://developers.google.com/speed/webp
7. **CMake 文档**: https://cmake.org/documentation/
8. **C++11 标准**: https://isocpp.org/std/the-standard

### 19.2 缩略语

| 缩略语 | 全称 |
|--------|------|
| WMTS | Web Map Tile Service |
| OGC | Open Geospatial Consortium |
| DPI | Dots Per Inch |
| API | Application Programming Interface |
| JSON | JavaScript Object Notation |
| SSL | Secure Sockets Layer |
| TLS | Transport Layer Security |
| WAL | Write-Ahead Logging |
| LRU | Least Recently Used |
| P95 | 95th Percentile |
| req/s | Requests Per Second |

### 19.3 版本兼容性

| 版本 | C++ 标准 | 最低 CMake | SQLite3 | PostgreSQL | libpng | libwebp |
|------|----------|-----------|---------|------------|--------|---------|
| 1.0 | C++11 | 3.10 | 3.30+ | N/A | 1.6+ | N/A |
| 2.0 | C++11 | 3.10 | 3.38+ | 14+ | 1.6+ | 1.2+ |

---

## 文档维护

**维护者**: Cycle Map Server Team  
**联系方式**: support@cycle-map-server.com  
**文档版本**: 2.0  
**最后更新**: 2026-03-09

---

*本设计文档为 Cycle Map Server 项目的核心技术文档，所有开发人员应严格遵守文档中的设计规范。*
