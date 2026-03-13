# Map Server Cycle - 详细设计文档

> **版本**: v1.0  
> **创建日期**: 2026-03-09  
> **文档状态**: 设计阶段  
> **目标**: 提供稳定高效的地图服务，指导后期功能实现与维护

---

## 目录

1. [系统架构设计](#1-系统架构设计)
2. [核心模块设计](#2-核心模块设计)
3. [接口设计](#3-接口设计)
4. [数据模型设计](#4-数据模型设计)
5. [性能设计](#5-性能设计)
6. [并发设计](#6-并发设计)
7. [安全设计](#7-安全设计)
8. [可靠性设计](#8-可靠性设计)
9. [可维护性设计](#9-可维护性设计)
10. [部署方案](#10-部署方案)
11. [运维指南](#11-运维指南)
12. [附录](#12-附录)

---

## 1. 系统架构设计

### 1.1 整体架构

Map Server Cycle 采用**分层架构**设计，分为表现层、业务层、数据层三个层次，确保系统的高内聚、低耦合。

```
┌─────────────────────────────────────────────────────────────┐
│                      表现层 (Presentation Layer)              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ HTTP Server  │  │ REST API     │  │ Static Files │      │
│  │ (cpp-httplib)│  │ (JSON)       │  │ (Images)     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                      业务层 (Business Layer)                  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ MapService   │  │ LayerManager │  │ CacheManager │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ RenderEngine │  │ ImageEncoder │  │ ConfigManager│      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                      数据层 (Data Layer)                      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ Database     │  │ Cache        │  │ File System  │      │
│  │ (SQLite/PG)  │  │ (Memory/Disk)│  │ (Config)     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 技术栈选型

#### 1.2.1 核心技术栈

| 技术领域 | 技术选型 | 版本要求 | 选型理由 |
|---------|---------|---------|---------|
| **编程语言** | C++ | C++11 | 跨平台、高性能、成熟稳定 |
| **HTTP框架** | cpp-httplib | v0.12+ | 轻量级、Header-only、易集成 |
| **图像编码** | libpng | v1.6+ | 标准PNG库、跨平台、性能好 |
| **图像编码** | libwebp | v1.2+ | WebP格式支持、压缩率高 |
| **数据库** | SQLite3 | v3.38+ | 嵌入式、零配置、支持空间扩展 |
| **数据库** | PostgreSQL | v14+ | 企业级、支持PostGIS、高并发 |
| **JSON解析** | nlohmann/json | v3.11+ | Header-only、易用、性能好 |
| **构建系统** | CMake | v3.10+ | 跨平台、标准化、社区支持好 |

#### 1.2.2 可选技术栈

| 技术领域 | 技术选型 | 版本要求 | 适用场景 |
|---------|---------|---------|---------|
| **图像渲染** | Qt | v5.12+ | 高质量渲染、抗锯齿、跨平台 |
| **空间扩展** | SpatiaLite | v5.1+ | SQLite空间查询增强 |
| **空间扩展** | PostGIS | v3.2+ | PostgreSQL空间查询增强 |
| **SSL/TLS** | OpenSSL | v3.0+ | HTTPS支持 |

### 1.3 模块划分

#### 1.3.1 核心模块

```cpp
namespace cycle {

// HTTP服务模块
class HttpServer;

// 地图服务模块
class MapService;

// 渲染引擎模块
class RenderEngine;

// 图像编码模块
class ImageEncoder;

// 图层管理模块
class LayerManager;

// 缓存管理模块
class CacheManager;

// 配置管理模块
class ConfigManager;

// 数据库访问模块
class DatabaseManager;

} // namespace cycle
```

#### 1.3.2 模块职责

| 模块名称 | 职责描述 | 依赖模块 |
|---------|---------|---------|
| **HttpServer** | HTTP请求处理、路由分发、线程管理 | ConfigManager, MapService |
| **MapService** | 地图业务逻辑、请求处理、响应生成 | RenderEngine, CacheManager, DatabaseManager |
| **RenderEngine** | 地图渲染、图像生成、样式应用 | ImageEncoder, LayerManager |
| **ImageEncoder** | 图像编码、格式转换、质量优化 | - |
| **LayerManager** | 图层加载、卸载、查询、样式管理 | DatabaseManager |
| **CacheManager** | 缓存管理、键值存储、淘汰策略 | - |
| **ConfigManager** | 配置加载、验证、热更新 | - |
| **DatabaseManager** | 数据库连接、查询、事务管理 | - |

### 1.4 依赖关系

```
HttpServer
    ├── ConfigManager
    └── MapService
        ├── RenderEngine
        │   ├── ImageEncoder
        │   └── LayerManager
        │       └── DatabaseManager
        ├── CacheManager
        └── DatabaseManager
```

### 1.5 设计原则

1. **单一职责原则**: 每个模块只负责一个功能领域
2. **开闭原则**: 对扩展开放，对修改关闭
3. **依赖倒置原则**: 依赖抽象而非具体实现
4. **接口隔离原则**: 接口最小化，避免冗余
5. **迪米特法则**: 最少知识原则，减少耦合

---

## 2. 核心模块设计

### 2.1 HTTP服务模块

#### 2.1.1 架构设计

```cpp
namespace cycle {

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    
    bool Initialize(const Config& config);
    void Start();
    void Stop();
    bool IsRunning() const;
    
private:
    void SetupRoutes();
    void SetupMiddleware();
    void HandleRequest(const httplib::Request& req, httplib::Response& res);
    
    std::unique_ptr<httplib::Server> m_server;
    std::shared_ptr<MapService> m_mapService;
    std::shared_ptr<ConfigManager> m_configManager;
    std::atomic<bool> m_running;
    int m_port;
    int m_workerThreads;
};

} // namespace cycle
```

#### 2.1.2 线程模型

采用**线程池模型**，主线程负责监听和分发请求，工作线程负责处理业务逻辑。

```
┌─────────────┐
│ 主线程      │ ← 监听端口，接收连接
└──────┬──────┘
       │
       ↓
┌─────────────────────────────────┐
│      线程池 (Thread Pool)        │
│  ┌──────┐  ┌──────┐  ┌──────┐  │
│  │线程1 │  │线程2 │  │线程N │  │
│  └──────┘  └──────┘  └──────┘  │
│      ↓          ↓          ↓    │
│  ┌──────────────────────────┐  │
│  │    任务队列 (Task Queue)  │  │
│  └──────────────────────────┘  │
└─────────────────────────────────┘
```

**配置示例**:
```json
{
  "server": {
    "worker_threads": 4,
    "max_connections": 1000,
    "timeout_seconds": 30
  }
}
```

#### 2.1.3 路由设计

```cpp
void HttpServer::SetupRoutes() {
    // 健康检查
    m_server->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        HandleHealth(req, res);
    });
    
    // 图层列表
    m_server->Get("/layers", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetLayers(req, res);
    });
    
    // 地图生成
    m_server->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
    // 瓦片服务
    m_server->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.png)", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
}
```

#### 2.1.4 错误处理

```cpp
namespace cycle {

enum class ErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = 400,
    NOT_FOUND = 404,
    INTERNAL_ERROR = 500,
    TIMEOUT = 504
};

struct ErrorResponse {
    ErrorCode code;
    std::string message;
    std::string details;
    
    std::string ToJson() const {
        nlohmann::json j;
        j["error"]["code"] = static_cast<int>(code);
        j["error"]["message"] = message;
        j["error"]["details"] = details;
        return j.dump();
    }
};

} // namespace cycle
```

### 2.2 渲染引擎模块

#### 2.2.1 架构设计

```cpp
namespace cycle {

class RenderEngine {
public:
    RenderEngine();
    ~RenderEngine();
    
    bool Initialize();
    std::vector<uint8_t> RenderMap(const RenderRequest& request);
    std::vector<uint8_t> RenderTile(int z, int x, int y);
    
private:
    bool RenderLayer(const Layer& layer, ImageBuffer& buffer);
    void ApplyStyle(const StyleConfig& style, ImageBuffer& buffer);
    
    std::shared_ptr<ImageEncoder> m_encoder;
    std::shared_ptr<LayerManager> m_layerManager;
};

struct RenderRequest {
    std::vector<double> bbox;      // [minx, miny, maxx, maxy]
    int width;
    int height;
    std::vector<std::string> layers;
    std::string backgroundColor;
    int quality;
    std::string format;
};

} // namespace cycle
```

#### 2.2.2 渲染流程

```
┌─────────────┐
│ 接收请求    │
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 解析参数    │ ← bbox, width, height, layers
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 创建画布    │ ← 分配内存，设置背景色
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 加载图层    │ ← 查询数据库，获取要素
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 渲染要素    │ ← 绘制点、线、面
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 应用样式    │ ← 颜色、线宽、填充
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 图像编码    │ ← PNG/WebP编码
└──────┬──────┘
       │
       ↓
┌─────────────┐
│ 返回结果    │
└─────────────┘
```

#### 2.2.3 图像编码器设计

```cpp
namespace cycle {

enum class ImageFormat {
    PNG8,
    PNG32,
    WEBP
};

class ImageEncoder {
public:
    virtual ~ImageEncoder() = default;
    
    virtual std::vector<uint8_t> Encode(
        const ImageBuffer& buffer,
        const EncodeOptions& options) = 0;
    
    virtual ImageFormat GetFormat() const = 0;
    virtual std::string GetMimeType() const = 0;
};

class PngEncoder : public ImageEncoder {
public:
    std::vector<uint8_t> Encode(
        const ImageBuffer& buffer,
        const EncodeOptions& options) override;
    
    ImageFormat GetFormat() const override { return ImageFormat::PNG32; }
    std::string GetMimeType() const override { return "image/png"; }
};

class WebpEncoder : public ImageEncoder {
public:
    std::vector<uint8_t> Encode(
        const ImageBuffer& buffer,
        const EncodeOptions& options) override;
    
    ImageFormat GetFormat() const override { return ImageFormat::WEBP; }
    std::string GetMimeType() const override { return "image/webp"; }
};

struct EncodeOptions {
    ImageFormat format = ImageFormat::PNG32;
    int quality = 90;
    int compression = 6;
    int dpi = 96;
};

} // namespace cycle
```

#### 2.2.4 高DPI支持

```cpp
namespace cycle {

class DpiHandler {
public:
    static int CalculateImageSize(int baseSize, int baseDpi, int targetDpi) {
        return static_cast<int>(baseSize * static_cast<double>(targetDpi) / baseDpi + 0.5);
    }
    
    static bool ValidateDpi(int dpi) {
        return dpi >= 72 && dpi <= 600;
    }
    
    static BoundingBox AdjustBboxForDpi(
        const BoundingBox& bbox,
        int baseDpi,
        int targetDpi) {
        
        double scale = static_cast<double>(baseDpi) / targetDpi;
        double centerX = (bbox.minX + bbox.maxX) / 2.0;
        double centerY = (bbox.minY + bbox.maxY) / 2.0;
        double width = (bbox.maxX - bbox.minX) * scale;
        double height = (bbox.maxY - bbox.minY) * scale;
        
        return BoundingBox{
            centerX - width / 2.0,
            centerY - height / 2.0,
            centerX + width / 2.0,
            centerY + height / 2.0
        };
    }
};

} // namespace cycle
```

### 2.3 数据库模块

#### 2.3.1 数据库抽象层

```cpp
namespace cycle {

class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    virtual bool Connect(const std::string& connectionString) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    
    virtual std::vector<Feature> QueryFeatures(
        const std::string& layerName,
        const BoundingBox& bbox) = 0;
    
    virtual bool ExecuteSQL(const std::string& sql) = 0;
    virtual std::string GetLastError() const = 0;
};

class SqliteDatabase : public IDatabase {
public:
    bool Connect(const std::string& connectionString) override;
    void Disconnect() override;
    bool IsConnected() const override;
    
    std::vector<Feature> QueryFeatures(
        const std::string& layerName,
        const BoundingBox& bbox) override;
    
    bool ExecuteSQL(const std::string& sql) override;
    std::string GetLastError() const override;
    
private:
    sqlite3* m_db;
    std::string m_connectionString;
};

class PostgresqlDatabase : public IDatabase {
public:
    bool Connect(const std::string& connectionString) override;
    void Disconnect() override;
    bool IsConnected() const override;
    
    std::vector<Feature> QueryFeatures(
        const std::string& layerName,
        const BoundingBox& bbox) override;
    
    bool ExecuteSQL(const std::string& sql) override;
    std::string GetLastError() const override;
    
private:
    PGconn* m_conn;
    std::string m_connectionString;
};

} // namespace cycle
```

#### 2.3.2 连接池设计

```cpp
namespace cycle {

class DatabaseConnectionPool {
public:
    DatabaseConnectionPool(
        std::function<std::shared_ptr<IDatabase>()> factory,
        size_t poolSize);
    
    std::shared_ptr<IDatabase> Acquire();
    void Release(std::shared_ptr<IDatabase> conn);
    
    size_t GetActiveConnections() const;
    size_t GetAvailableConnections() const;
    
private:
    std::vector<std::shared_ptr<IDatabase>> m_connections;
    std::queue<std::shared_ptr<IDatabase>> m_available;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    size_t m_poolSize;
};

} // namespace cycle
```

#### 2.3.3 空间查询优化

**SQLite/SpatiaLite**:
```sql
-- 创建空间索引
SELECT CreateSpatialIndex('map_features', 'geometry');

-- 空间查询
SELECT id, name, geometry
FROM map_features
WHERE layer_id = ?
  AND ST_Intersects(geometry, BuildMbr(?, ?, ?, ?, 4326));
```

**PostgreSQL/PostGIS**:
```sql
-- 创建空间索引
CREATE INDEX idx_features_geom ON map_features USING GIST(geometry);

-- 空间查询
SELECT id, name, ST_AsBinary(geometry) as geometry
FROM map_features
WHERE layer_id = $1
  AND geometry && ST_MakeEnvelope($2, $3, $4, $5, 4326);
```

### 2.4 缓存模块

#### 2.4.1 多级缓存架构

```
┌─────────────────────────────────────┐
│      L1 Cache (Memory Cache)         │
│  - 热点数据                           │
│  - 快速访问 (< 1ms)                   │
│  - 容量: 100-500 items               │
└──────────────┬──────────────────────┘
               │ Miss
               ↓
┌─────────────────────────────────────┐
│      L2 Cache (Disk Cache)           │
│  - 持久化数据                         │
│  - 中速访问 (1-10ms)                  │
│  - 容量: 10GB+                       │
└──────────────┬──────────────────────┘
               │ Miss
               ↓
┌─────────────────────────────────────┐
│      Database (SQLite/PostgreSQL)    │
│  - 原始数据                           │
│  - 慢速访问 (10-100ms)                │
│  - 容量: TB级                        │
└─────────────────────────────────────┘
```

#### 2.4.2 缓存键设计

```cpp
namespace cycle {

class CacheKeyGenerator {
public:
    static std::string GenerateTileKey(
        const std::string& layer,
        int z, int x, int y,
        const std::string& format,
        int dpi = 96) {
        
        std::ostringstream oss;
        oss << layer << ":"
            << z << ":"
            << x << ":"
            << y << ":"
            << format << ":"
            << dpi;
        return oss.str();
    }
    
    static std::string GenerateMapKey(
        const std::string& layers,
        const BoundingBox& bbox,
        int width, int height,
        const std::string& format) {
        
        std::ostringstream oss;
        oss << layers << ":"
            << std::fixed << std::setprecision(6)
            << bbox.minX << ":" << bbox.minY << ":"
            << bbox.maxX << ":" << bbox.maxY << ":"
            << width << ":" << height << ":"
            << format;
        return oss.str();
    }
};

} // namespace cycle
```

#### 2.4.3 缓存淘汰策略

采用**LRU (Least Recently Used)** 算法：

```cpp
namespace cycle {

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t maxSize) : m_maxSize(maxSize) {}
    
    bool Get(const Key& key, Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_cache.find(key);
        if (it == m_cache.end()) {
            return false;
        }
        
        // 移动到最前面
        m_lruList.splice(m_lruList.begin(), m_lruList, it->second.second);
        value = it->second.first;
        return true;
    }
    
    void Put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            // 更新值并移动到最前面
            it->second.first = value;
            m_lruList.splice(m_lruList.begin(), m_lruList, it->second.second);
            return;
        }
        
        // 淘汰最久未使用的
        if (m_cache.size() >= m_maxSize) {
            auto last = m_lruList.back();
            m_cache.erase(last);
            m_lruList.pop_back();
        }
        
        // 插入新项
        m_lruList.push_front(key);
        m_cache[key] = {value, m_lruList.begin()};
    }
    
private:
    size_t m_maxSize;
    std::list<Key> m_lruList;
    std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> m_cache;
    std::mutex m_mutex;
};

} // namespace cycle
```

---

## 3. 接口设计

### 3.1 RESTful API设计

#### 3.1.1 健康检查API

**GET /health**

**请求示例**:
```bash
curl http://localhost:8080/health
```

**响应格式**:
```json
{
  "status": "healthy",
  "timestamp": "2026-03-09T10:30:00Z",
  "version": "1.0.0",
  "uptime": "2h30m",
  "memory_usage": "45.2MB",
  "metrics": {
    "requests_total": 10000,
    "requests_success": 9850,
    "cache_hit_rate": 0.95,
    "avg_render_time_ms": 285
  }
}
```

**状态码**:
- `200 OK`: 服务正常
- `503 Service Unavailable`: 服务异常

#### 3.1.2 图层管理API

**GET /layers**

**请求示例**:
```bash
curl http://localhost:8080/layers
```

**响应格式**:
```json
{
  "layers": [
    {
      "name": "roads",
      "type": "line",
      "description": "道路网络图层",
      "min_zoom": 0,
      "max_zoom": 18,
      "feature_count": 125000,
      "style": {
        "color": "#333333",
        "width": 2
      }
    },
    {
      "name": "buildings",
      "type": "polygon",
      "description": "建筑物图层",
      "min_zoom": 12,
      "max_zoom": 18,
      "feature_count": 45000,
      "style": {
        "fill_color": "#f0f0f0",
        "stroke_color": "#cccccc",
        "stroke_width": 1
      }
    }
  ],
  "total": 2
}
```

#### 3.1.3 地图生成API

**POST /generate**

**请求格式**:
```json
{
  "bbox": [-74.006, 40.7128, -73.935, 40.8076],
  "width": 1024,
  "height": 768,
  "layers": ["roads", "buildings"],
  "background_color": "#ffffff",
  "filter": "name LIKE '%park%'",
  "zoom_level": 12,
  "format": "png",
  "quality": 90,
  "dpi": 96
}
```

**参数说明**:

| 参数 | 类型 | 必选 | 默认值 | 说明 |
|------|------|------|--------|------|
| bbox | number[] | ✅ | - | 边界框 [minx, miny, maxx, maxy] |
| width | integer | ✅ | - | 图像宽度（像素，1-4096） |
| height | integer | ✅ | - | 图像高度（像素，1-4096） |
| layers | string[] | ❌ | 所有图层 | 要渲染的图层列表 |
| background_color | string | ❌ | "#ffffff" | 背景颜色（HEX格式） |
| filter | string | ❌ | - | SQL WHERE条件过滤 |
| zoom_level | integer | ❌ | 自动计算 | 缩放级别（0-18） |
| format | string | ❌ | "png" | 输出格式（png8/png32/webp） |
| quality | integer | ❌ | 90 | 图像质量（1-100） |
| dpi | integer | ❌ | 96 | DPI（72-600） |

**响应**:
- `Content-Type: image/png` 或 `image/webp`
- `Content-Length: <文件大小>`
- `Cache-Control: max-age=3600`
- 直接返回图像二进制流

**错误响应**:
```json
{
  "error": {
    "code": 400,
    "message": "Invalid parameter",
    "details": {
      "parameter": "width",
      "value": 5000,
      "constraint": "max_image_width"
    }
  }
}
```

#### 3.1.4 瓦片服务API

**GET /tile/{z}/{x}/{y}.{format}**

**URL格式**:
```
/tile/{zoom}/{x}/{y}.{format}
```

**参数说明**:
- `z`: 缩放级别（0-18）
- `x`: 瓦片X坐标
- `y`: 瓦片Y坐标
- `format`: 格式（png/webp）

**请求示例**:
```bash
curl http://localhost:8080/tile/12/1204/1540.png
```

**响应**:
- `Content-Type: image/png`
- `Cache-Control: max-age=3600`
- 返回256x256像素的瓦片图像

### 3.2 内部接口设计

#### 3.2.1 模块间调用接口

```cpp
namespace cycle {

// MapService接口
class IMapService {
public:
    virtual ~IMapService() = default;
    
    virtual std::vector<uint8_t> GenerateMap(
        const RenderRequest& request) = 0;
    
    virtual std::vector<uint8_t> GetTile(
        int z, int x, int y,
        const std::string& format,
        int dpi) = 0;
    
    virtual std::vector<LayerInfo> GetLayers() = 0;
};

// RenderEngine接口
class IRenderEngine {
public:
    virtual ~IRenderEngine() = default;
    
    virtual std::vector<uint8_t> Render(
        const RenderRequest& request) = 0;
    
    virtual std::vector<uint8_t> RenderTile(
        int z, int x, int y) = 0;
};

// CacheManager接口
class ICacheManager {
public:
    virtual ~ICacheManager() = default;
    
    virtual bool Get(const std::string& key, std::vector<uint8_t>& value) = 0;
    virtual void Put(const std::string& key, const std::vector<uint8_t>& value) = 0;
    virtual void Remove(const std::string& key) = 0;
    virtual void Clear() = 0;
    
    virtual CacheStats GetStats() const = 0;
};

} // namespace cycle
```

#### 3.2.2 数据访问接口

```cpp
namespace cycle {

class IDatabaseManager {
public:
    virtual ~IDatabaseManager() = default;
    
    virtual bool Initialize(const DatabaseConfig& config) = 0;
    virtual void Shutdown() = 0;
    
    virtual std::vector<Feature> QueryFeatures(
        const std::string& layerName,
        const BoundingBox& bbox) = 0;
    
    virtual LayerInfo GetLayerInfo(const std::string& layerName) = 0;
    virtual std::vector<LayerInfo> GetAllLayers() = 0;
    
    virtual bool ExecuteSQL(const std::string& sql) = 0;
    virtual std::string GetLastError() const = 0;
};

} // namespace cycle
```

---

## 4. 数据模型设计

### 4.1 地理数据模型

#### 4.1.1 基础几何类型

```cpp
namespace cycle {

struct Point {
    double x;
    double y;
    int srid = 4326;
    
    double DistanceTo(const Point& other) const;
    bool IsValid() const;
};

struct LineString {
    std::vector<Point> points;
    int srid = 4326;
    
    double Length() const;
    BoundingBox GetBoundingBox() const;
    bool IsValid() const;
};

struct Polygon {
    std::vector<Point> exterior;
    std::vector<std::vector<Point>> interiors;
    int srid = 4326;
    
    double Area() const;
    BoundingBox GetBoundingBox() const;
    bool IsValid() const;
    bool Contains(const Point& point) const;
};

struct BoundingBox {
    double minX;
    double minY;
    double maxX;
    double maxY;
    int srid = 4326;
    
    double Width() const { return maxX - minX; }
    double Height() const { return maxY - minY; }
    bool IsValid() const { return minX < maxX && minY < maxY; }
    bool Intersects(const BoundingBox& other) const;
    bool Contains(const Point& point) const;
};

} // namespace cycle
```

#### 4.1.2 要素模型

```cpp
namespace cycle {

enum class GeometryType {
    POINT,
    LINESTRING,
    POLYGON,
    MULTIPOINT,
    MULTILINESTRING,
    MULTIPOLYGON
};

struct Feature {
    int64_t id;
    GeometryType geometryType;
    std::variant<Point, LineString, Polygon> geometry;
    std::map<std::string, std::string> attributes;
    
    BoundingBox GetBoundingBox() const;
    bool IsValid() const;
};

} // namespace cycle
```

### 4.2 图层模型

```cpp
namespace cycle {

struct LayerStyle {
    std::string fillColor;
    std::string strokeColor;
    int strokeWidth;
    double opacity;
    std::string iconPath;
};

struct LayerInfo {
    std::string name;
    std::string type;  // "point", "line", "polygon"
    std::string description;
    int minZoom;
    int maxZoom;
    int64_t featureCount;
    LayerStyle style;
    BoundingBox extent;
    
    bool IsVisibleAtZoom(int zoom) const {
        return zoom >= minZoom && zoom <= maxZoom;
    }
};

} // namespace cycle
```

### 4.3 数据库表结构

#### 4.3.1 图层元数据表

```sql
CREATE TABLE IF NOT EXISTS layers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    type TEXT NOT NULL CHECK(type IN ('point', 'line', 'polygon')),
    description TEXT,
    min_zoom INTEGER DEFAULT 0,
    max_zoom INTEGER DEFAULT 18,
    fill_color TEXT DEFAULT '#ffffff',
    stroke_color TEXT DEFAULT '#000000',
    stroke_width INTEGER DEFAULT 1,
    opacity REAL DEFAULT 1.0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_layers_name ON layers(name);
```

#### 4.3.2 要素数据表

```sql
CREATE TABLE IF NOT EXISTS features (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    layer_id INTEGER NOT NULL,
    geometry GEOMETRY NOT NULL,
    attributes TEXT,  -- JSON格式
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (layer_id) REFERENCES layers(id) ON DELETE CASCADE
);

-- 创建空间索引
SELECT CreateSpatialIndex('features', 'geometry');

-- 创建普通索引
CREATE INDEX idx_features_layer ON features(layer_id);
```

#### 4.3.3 瓦片缓存表

```sql
CREATE TABLE IF NOT EXISTS tile_cache (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tile_key TEXT UNIQUE NOT NULL,
    tile_data BLOB NOT NULL,
    format TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    accessed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    access_count INTEGER DEFAULT 0
);

CREATE INDEX idx_tile_cache_key ON tile_cache(tile_key);
CREATE INDEX idx_tile_cache_accessed ON tile_cache(accessed_at);
```

---

## 5. 性能设计

### 5.1 性能指标

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| **单图渲染时间** | < 350ms (P95) | 从请求到响应的时间 |
| **瓦片渲染时间** | < 100ms (P95) | 单个瓦片渲染时间 |
| **并发处理能力** | > 200 req/s | 持续压力测试 |
| **缓存命中率** | > 95% | (命中数/总请求数) |
| **内存占用** | < 150MB | 稳态运行 |
| **启动时间** | < 5s | 冷启动到就绪 |
| **CPU使用率** | < 70% | 正常负载 |
| **磁盘I/O** | < 50MB/s | 正常负载 |

### 5.2 性能优化策略

#### 5.2.1 渲染优化

```cpp
namespace cycle {

class RenderOptimizer {
public:
    // 图层合并渲染
    void MergeLayers(std::vector<Layer>& layers);
    
    // 要素裁剪
    std::vector<Feature> ClipFeatures(
        const std::vector<Feature>& features,
        const BoundingBox& bbox);
    
    // LOD (Level of Detail)
    int CalculateLOD(int zoom, int featureCount);
    
    // 渲染缓存
    bool UseRenderCache(const RenderRequest& request);
};

} // namespace cycle
```

**优化策略**:
1. **空间索引**: 使用R-Tree或QuadTree加速空间查询
2. **要素裁剪**: 只渲染视口内的要素
3. **LOD策略**: 根据缩放级别调整细节层次
4. **批量渲染**: 合并多个图层的渲染操作
5. **内存池**: 重用图像缓冲区，减少内存分配

#### 5.2.2 缓存优化

```cpp
namespace cycle {

class CacheOptimizer {
public:
    // 预加载热点区域
    void PreloadHotRegion(const BoundingBox& bbox, int minZoom, int maxZoom);
    
    // 智能预取
    void PredictAndPrefetch(const std::string& currentTile);
    
    // 缓存分片
    std::string ShardCacheKey(const std::string& key);
};

} // namespace cycle
```

**优化策略**:
1. **热点预加载**: 启动时加载热门区域瓦片
2. **智能预取**: 根据用户行为预测下一张瓦片
3. **缓存分片**: 分散缓存文件，避免单目录性能瓶颈
4. **压缩存储**: 对缓存数据进行压缩
5. **异步刷新**: 后台线程刷新即将过期的缓存

#### 5.2.3 数据库优化

**索引优化**:
```sql
-- 复合索引
CREATE INDEX idx_features_layer_bbox ON features(layer_id, geometry);

-- 部分索引
CREATE INDEX idx_features_active ON features(layer_id) 
WHERE deleted_at IS NULL;

-- 表达式索引
CREATE INDEX idx_features_area ON features(ST_Area(geometry));
```

**查询优化**:
```sql
-- 使用覆盖索引
SELECT id, ST_AsBinary(geometry) as geom, attributes
FROM features
WHERE layer_id = ? AND geometry && BuildMbr(?, ?, ?, ?, 4326);

-- 分页查询
SELECT * FROM features
WHERE layer_id = ?
ORDER BY id
LIMIT ? OFFSET ?;

-- 批量插入
INSERT INTO features (layer_id, geometry, attributes)
VALUES (?, ?, ?), (?, ?, ?), ...;
```

### 5.3 性能测试方案

#### 5.3.1 压力测试

```bash
# 使用 Apache Bench
ab -n 10000 -c 100 http://localhost:8080/tile/12/1204/1540.png

# 使用 wrk
wrk -t 4 -c 100 -d 30s http://localhost:8080/health

# 使用 JMeter
jmeter -n -t map_server_test.jmx -l results.jtl
```

#### 5.3.2 性能监控

```cpp
namespace cycle {

class PerformanceMonitor {
public:
    void RecordRequest(const std::string& path, int64_t durationMs);
    void RecordCacheHit(bool hit);
    void RecordRenderTime(int64_t durationMs);
    
    PerformanceStats GetStats() const;
    
private:
    std::atomic<int64_t> m_totalRequests{0};
    std::atomic<int64_t> m_cacheHits{0};
    std::vector<int64_t> m_renderTimes;
    std::mutex m_mutex;
};

struct PerformanceStats {
    int64_t totalRequests;
    int64_t successRequests;
    int64_t failedRequests;
    double cacheHitRate;
    double avgRenderTimeMs;
    double p95RenderTimeMs;
    double p99RenderTimeMs;
};

} // namespace cycle
```

---

## 6. 并发设计

### 6.1 线程模型

```
┌─────────────────────────────────────────┐
│            主线程 (Main Thread)           │
│  - 监听端口                               │
│  - 接受连接                               │
│  - 分发请求                               │
└────────────────┬────────────────────────┘
                 │
                 ↓
┌─────────────────────────────────────────┐
│         工作线程池 (Worker Pool)          │
│  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐│
│  │Worker│  │Worker│  │Worker│  │Worker││
│  │  1   │  │  2   │  │  3   │  │  4   ││
│  └──────┘  └──────┘  └──────┘  └──────┘│
└────────────────┬────────────────────────┘
                 │
                 ↓
┌─────────────────────────────────────────┐
│          任务队列 (Task Queue)            │
│  - FIFO队列                              │
│  - 线程安全                               │
│  - 有界队列                               │
└─────────────────────────────────────────┘
```

### 6.2 线程安全设计

#### 6.2.1 互斥锁使用

```cpp
namespace cycle {

class ThreadSafeCache {
public:
    bool Get(const std::string& key, std::vector<uint8_t>& value) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);  // 读锁
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            value = it->second;
            return true;
        }
        return false;
    }
    
    void Put(const std::string& key, const std::vector<uint8_t>& value) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);  // 写锁
        m_cache[key] = value;
    }
    
private:
    std::unordered_map<std::string, std::vector<uint8_t>> m_cache;
    mutable std::shared_mutex m_mutex;  // 读写锁
};

} // namespace cycle
```

#### 6.2.2 原子操作

```cpp
namespace cycle {

class RequestCounter {
public:
    void Increment() {
        m_count.fetch_add(1, std::memory_order_relaxed);
    }
    
    int64_t Get() const {
        return m_count.load(std::memory_order_relaxed);
    }
    
private:
    std::atomic<int64_t> m_count{0};
};

} // namespace cycle
```

#### 6.2.3 线程池实现

```cpp
namespace cycle {

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            m_workers.emplace_back([this] { WorkerThread(); });
        }
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_shutdown = true;
        }
        m_cv.notify_all();
        
        for (auto& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    template<typename F>
    void Submit(F&& task) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.emplace(std::forward<F>(task));
        }
        m_cv.notify_one();
    }
    
private:
    void WorkerThread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this] { return m_shutdown || !m_tasks.empty(); });
                
                if (m_shutdown && m_tasks.empty()) {
                    return;
                }
                
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            
            task();
        }
    }
    
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_shutdown = false;
};

} // namespace cycle
```

### 6.3 死锁预防

#### 6.3.1 锁定顺序

```cpp
namespace cycle {

// 始终按照相同顺序获取锁
class DeadlockPrevention {
public:
    void Transfer(Cache& from, Cache& to, const std::string& key) {
        // 按照地址顺序锁定，避免死锁
        std::mutex* first = &from.m_mutex;
        std::mutex* second = &to.m_mutex;
        
        if (first > second) {
            std::swap(first, second);
        }
        
        std::lock_guard<std::mutex> lock1(*first);
        std::lock_guard<std::mutex> lock2(*second);
        
        // 执行转移操作
        // ...
    }
};

} // namespace cycle
```

#### 6.3.2 超时机制

```cpp
namespace cycle {

class TimeoutLock {
public:
    bool TryLockFor(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);
        return lock.try_lock_for(timeout);
    }
    
private:
    std::timed_mutex m_mutex;
};

} // namespace cycle
```

---

## 7. 安全设计

### 7.1 输入验证

#### 7.1.1 参数验证

```cpp
namespace cycle {

class InputValidator {
public:
    static bool ValidateBbox(const std::vector<double>& bbox) {
        if (bbox.size() != 4) return false;
        if (bbox[0] >= bbox[2]) return false;  // minX < maxX
        if (bbox[1] >= bbox[3]) return false;  // minY < maxY
        if (bbox[0] < -180 || bbox[2] > 180) return false;
        if (bbox[1] < -90 || bbox[3] > 90) return false;
        return true;
    }
    
    static bool ValidateImageSize(int width, int height) {
        return width > 0 && width <= 4096 &&
               height > 0 && height <= 4096;
    }
    
    static bool ValidateZoom(int zoom) {
        return zoom >= 0 && zoom <= 18;
    }
    
    static bool ValidateFormat(const std::string& format) {
        static const std::unordered_set<std::string> validFormats = {
            "png", "png8", "png32", "webp"
        };
        return validFormats.count(format) > 0;
    }
    
    static bool ValidateDpi(int dpi) {
        return dpi >= 72 && dpi <= 600;
    }
    
    static std::string SanitizeFilter(const std::string& filter) {
        // 移除危险字符
        std::string result = filter;
        // TODO: 实现SQL注入过滤
        return result;
    }
};

} // namespace cycle
```

### 7.2 SQL注入防护

#### 7.2.1 参数化查询

```cpp
namespace cycle {

class SafeQuery {
public:
    std::vector<Feature> QueryFeatures(
        const std::string& layerName,
        const BoundingBox& bbox) {
        
        // 使用参数化查询，防止SQL注入
        std::string sql = R"(
            SELECT id, ST_AsBinary(geometry) as geom, attributes
            FROM features
            WHERE layer_id = (SELECT id FROM layers WHERE name = ?)
              AND geometry && BuildMbr(?, ?, ?, ?, 4326)
        )";
        
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        
        // 绑定参数
        sqlite3_bind_text(stmt, 1, layerName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, bbox.minX);
        sqlite3_bind_double(stmt, 3, bbox.minY);
        sqlite3_bind_double(stmt, 4, bbox.maxX);
        sqlite3_bind_double(stmt, 5, bbox.maxY);
        
        // 执行查询
        std::vector<Feature> features;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // 处理结果
        // ...
        
        // 执行查询
        std::vector<Feature> features;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // 处理结果
        }
        
        sqlite3_finalize(stmt);
        return features;
    }
};

} // namespace cycle
```

### 7.3 访问控制

#### 7.3.1 API密钥认证

```cpp
namespace cycle {

class ApiKeyAuth {
public:
    bool Validate(const std::string& apiKey) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_validKeys.count(apiKey) > 0;
    }
    
    bool ValidateRequest(const httplib::Request& req) {
        std::string apiKey = req.get_header_value("X-API-Key");
        if (apiKey.empty()) {
            apiKey = req.get_param_value("api_key");
        }
        return Validate(apiKey);
    }
    
private:
    std::unordered_set<std::string> m_validKeys;
    mutable std::shared_mutex m_mutex;
};

} // namespace cycle
```

### 7.4 日志审计

```cpp
namespace cycle {

class SecurityLogger {
public:
    void LogRequest(const httplib::Request& req, int statusCode) {
        nlohmann::json log;
        log["timestamp"] = GetCurrentTimestamp();
        log["method"] = req.method;
        log["path"] = req.path;
        log["status_code"] = statusCode;
        log["client_ip"] = req.remote_addr;
        log["user_agent"] = req.get_header_value("User-Agent");
        
        // 记录到安全日志
        WriteLog(log.dump());
    }
    
    void LogFailedAuth(const std::string& clientIp, const std::string& reason) {
        nlohmann::json log;
        log["timestamp"] = GetCurrentTimestamp();
        log["event"] = "auth_failed";
        log["client_ip"] = clientIp;
        log["reason"] = reason;
        
        WriteLog(log.dump());
    }
    
private:
    void WriteLog(const std::string& message);
    std::string GetCurrentTimestamp();
};

} // namespace cycle
```

---

## 8. 可靠性设计

### 8.1 错误处理机制

#### 8.1.1 错误分类

```cpp
namespace cycle {

enum class ErrorCategory {
    NETWORK,
    DATABASE,
    RENDERING,
    CACHE,
    CONFIGURATION,
    VALIDATION
};

struct Error {
    ErrorCategory category;
    int code;
    std::string message;
    std::string details;
    std::string stackTrace;
    
    std::string ToString() const {
        std::ostringstream oss;
        oss << "[" << CategoryToString(category) << "] "
            << "Error " << code << ": " << message;
        if (!details.empty()) {
            oss << " (" << details << ")";
        }
        return oss.str();
    }
    
private:
    static std::string CategoryToString(ErrorCategory cat);
};

} // namespace cycle
```

#### 8.1.2 异常处理

```cpp
namespace cycle {

class MapServerException : public std::exception {
public:
    explicit MapServerException(const Error& error)
        : m_error(error) {}
    
    const char* what() const noexcept override {
        return m_error.message.c_str();
    }
    
    const Error& GetError() const { return m_error; }
    
private:
    Error m_error;
};

// 使用示例
void RenderMap(const RenderRequest& request) {
    try {
        // 渲染逻辑
    } catch (const DatabaseException& e) {
        throw MapServerException(Error{
            ErrorCategory::DATABASE,
            5001,
            "Database query failed",
            e.what()
        });
    } catch (const RenderException& e) {
        throw MapServerException(Error{
            ErrorCategory::RENDERING,
            6001,
            "Rendering failed",
            e.what()
        });
    }
}

} // namespace cycle
```

### 8.2 容错机制

#### 8.2.1 重试机制

```cpp
namespace cycle {

template<typename F>
auto Retry(F&& func, int maxRetries, std::chrono::milliseconds delay) 
    -> decltype(func()) {
    
    int attempts = 0;
    while (attempts < maxRetries) {
        try {
            return func();
        } catch (const std::exception& e) {
            attempts++;
            if (attempts >= maxRetries) {
                throw;
            }
            std::this_thread::sleep_for(delay);
        }
    }
    throw std::runtime_error("Max retries exceeded");
}

} // namespace cycle
```

#### 8.2.2 熔断器

```cpp
namespace cycle {

class CircuitBreaker {
public:
    explicit CircuitBreaker(int failureThreshold, std::chrono::milliseconds timeout)
        : m_failureThreshold(failureThreshold)
        , m_timeout(timeout) {}
    
    template<typename F>
    auto Execute(F&& func) -> decltype(func()) {
        if (m_state == State::OPEN) {
            if (std::chrono::steady_clock::now() - m_lastFailureTime > m_timeout) {
                m_state = State::HALF_OPEN;
            } else {
                throw std::runtime_error("Circuit breaker is open");
            }
        }
        
        try {
            auto result = func();
            OnSuccess();
            return result;
        } catch (...) {
            OnFailure();
            throw;
        }
    }
    
private:
    void OnSuccess() {
        m_failureCount = 0;
        m_state = State::CLOSED;
    }
    
    void OnFailure() {
        m_failureCount++;
        m_lastFailureTime = std::chrono::steady_clock::now();
        
        if (m_failureCount >= m_failureThreshold) {
            m_state = State::OPEN;
        }
    }
    
    enum class State { CLOSED, OPEN, HALF_OPEN };
    
    State m_state = State::CLOSED;
    int m_failureCount = 0;
    int m_failureThreshold;
    std::chrono::milliseconds m_timeout;
    std::chrono::steady_clock::time_point m_lastFailureTime;
};

} // namespace cycle
```

### 8.3 数据备份

#### 8.3.1 备份策略

```cpp
namespace cycle {

class BackupManager {
public:
    void ScheduleBackup(const std::string& dbPath, const std::string& backupDir) {
        // 每日全量备份
        // 每小时增量备份
        // 保留最近7天的备份
    }
    
    bool CreateFullBackup(const std::string& dbPath, const std::string& backupPath) {
        // 复制数据库文件
        std::ifstream src(dbPath, std::ios::binary);
        std::ofstream dst(backupPath, std::ios::binary);
        dst << src.rdbuf();
        return true;
    }
    
    bool RestoreFromBackup(const std::string& backupPath, const std::string& dbPath) {
        // 恢复数据库
        std::ifstream src(backupPath, std::ios::binary);
        std::ofstream dst(dbPath, std::ios::binary);
        dst << src.rdbuf();
        return true;
    }
};

} // namespace cycle
```

---

## 9. 可维护性设计

### 9.1 代码规范

#### 9.1.1 命名规范

```cpp
namespace cycle {

// 类名：PascalCase
class MapService {};

// 函数名：PascalCase
void RenderMap();

// 成员变量：m_前缀
class Example {
private:
    int m_count;
    std::string m_name;
};

// 局部变量：camelCase
int tileCount = 0;
std::string layerName = "";

// 常量：UPPER_CASE
const int MAX_TILE_SIZE = 4096;
const std::string DEFAULT_FORMAT = "png";

// 枚举：PascalCase
enum class ImageFormat {
    Png8,
    Png32,
    WebP
};

} // namespace cycle
```

#### 9.1.2 注释规范

```cpp
namespace cycle {

/**
 * @brief 地图服务类，提供地图渲染和瓦片服务
 * 
 * 该类负责处理地图相关的业务逻辑，包括：
 * - 地图图像生成
 * - 瓦片服务
 * - 图层管理
 * - 缓存管理
 * 
 * @example
 * MapService service;
 * service.Initialize(config);
 * auto image = service.GenerateMap(request);
 */
class MapService {
public:
    /**
     * @brief 初始化地图服务
     * 
     * @param config 配置对象
     * @return true 初始化成功
     * @return false 初始化失败
     * 
     * @throws ConfigException 配置错误时抛出
     */
    bool Initialize(const Config& config);
    
    /**
     * @brief 生成地图图像
     * 
     * @param request 渲染请求参数
     * @return std::vector<uint8_t> 图像二进制数据
     * 
     * @throws RenderException 渲染失败时抛出
     * 
     * @note 该方法会自动使用缓存
     */
    std::vector<uint8_t> GenerateMap(const RenderRequest& request);
    
private:
    std::shared_ptr<RenderEngine> m_renderEngine;
    std::shared_ptr<CacheManager> m_cacheManager;
};

} // namespace cycle
```

### 9.2 测试规范

#### 9.2.1 单元测试

```cpp
#include <gtest/gtest.h>
#include "map_service.h"

using namespace cycle;

class MapServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前准备
        Config config;
        config.database.path = ":memory:";
        m_service = std::make_unique<MapService>();
        m_service->Initialize(config);
    }
    
    void TearDown() override {
        // 测试后清理
        m_service.reset();
    }
    
    std::unique_ptr<MapService> m_service;
};

TEST_F(MapServiceTest, GenerateMap_ValidRequest_ReturnsImage) {
    RenderRequest request;
    request.bbox = {-180, -90, 180, 90};
    request.width = 256;
    request.height = 256;
    request.format = "png";
    
    auto image = m_service->GenerateMap(request);
    
    EXPECT_FALSE(image.empty());
    EXPECT_GT(image.size(), 0);
}

TEST_F(MapServiceTest, GenerateMap_InvalidBbox_ThrowsException) {
    RenderRequest request;
    request.bbox = {180, -90, -180, 90};  // 无效bbox
    
    EXPECT_THROW(m_service->GenerateMap(request), MapServerException);
}

} // namespace cycle
```

#### 9.2.2 集成测试

```cpp
#include <gtest/gtest.h>
#include <httplib.h>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 启动测试服务器
        m_serverThread = std::thread([this] {
            m_server = std::make_unique<HttpServer>();
            m_server->Start();
        });
        
        // 等待服务器启动
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    void TearDown() override {
        m_server->Stop();
        if (m_serverThread.joinable()) {
            m_serverThread.join();
        }
    }
    
    std::unique_ptr<HttpServer> m_server;
    std::thread m_serverThread;
};

TEST_F(IntegrationTest, HealthCheck_ReturnsHealthy) {
    httplib::Client cli("http://localhost:8080");
    
    auto res = cli.Get("/health");
    
    ASSERT_TRUE(res != nullptr);
    EXPECT_EQ(res->status, 200);
    
    auto json = nlohmann::json::parse(res->body);
    EXPECT_EQ(json["status"], "healthy");
}

} // namespace cycle
```

### 9.3 文档规范

#### 9.3.1 文档结构

```
docs/
├── README.md              # 项目概述
├── DESIGN.md              # 设计文档
├── API.md                 # API文档
├── DEPLOYMENT.md          # 部署文档
├── MAINTENANCE.md         # 运维文档
└── CHANGELOG.md           # 变更日志
```

#### 9.3.2 文档更新流程

1. **功能开发时**: 同步更新API文档和设计文档
2. **代码提交时**: 更新CHANGELOG.md
3. **版本发布时**: 更新README.md和部署文档
4. **问题修复时**: 更新运维文档

---

## 10. 部署方案

### 10.1 环境要求

#### 10.1.1 硬件要求

| 环境 | CPU | 内存 | 磁盘 | 网络 |
|------|-----|------|------|------|
| **开发环境** | 2核 | 4GB | 20GB | 10Mbps |
| **测试环境** | 4核 | 8GB | 50GB | 100Mbps |
| **生产环境** | 8核+ | 16GB+ | 200GB+ SSD | 1Gbps |

#### 10.1.2 软件要求

**Ubuntu/Debian**:
```bash
# 必需软件
sudo apt-get update
sudo apt-get install -y \
    cmake \
    g++ \
    libsqlite3-dev \
    libpng-dev \
    libwebp-dev

# 可选软件
sudo apt-get install -y \
    qtbase5-dev \
    libqt5gui5 \
    postgresql-14 \
    postgresql-14-postgis-3
```

**CentOS/RHEL**:
```bash
sudo yum install -y \
    cmake \
    gcc-c++ \
    sqlite-devel \
    libpng-devel \
    libwebp-devel
```

**Windows**:
```powershell
# 使用 vcpkg
vcpkg install sqlite3 libpng libwebp
vcpkg install qt5-base
```

### 10.2 部署架构

#### 10.2.1 单机部署

```
┌─────────────────────────────────────┐
│         单机部署架构                  │
│                                      │
│  ┌────────────────────────────┐    │
│  │   Nginx (反向代理)          │    │
│  │   - 静态资源                │    │
│  │   - 负载均衡                │    │
│  └──────────┬─────────────────┘    │
│             │                        │
│             ↓                        │
│  ┌────────────────────────────┐    │
│  │   Map Server               │    │
│  │   - HTTP服务               │    │
│  │   - 渲染引擎               │    │
│  │   - 缓存管理               │    │
│  └──────────┬─────────────────┘    │
│             │                        │
│             ↓                        │
│  ┌────────────────────────────┐    │
│  │   SQLite/PostgreSQL        │    │
│  │   - 空间数据                │    │
│  │   - 缓存数据                │    │
│  └────────────────────────────┘    │
└─────────────────────────────────────┘
```

#### 10.2.2 集群部署

```
┌──────────────────────────────────────────────┐
│              集群部署架构                      │
│                                               │
│  ┌─────────────────────────────────────┐    │
│  │   负载均衡器 (Nginx/HAProxy)         │    │
│  └──────────┬──────────────────────────┘    │
│             │                                 │
│     ┌───────┼───────┐                        │
│     │       │       │                        │
│     ↓       ↓       ↓                        │
│  ┌─────┐ ┌─────┐ ┌─────┐                  │
│  │Map  │ │Map  │ │Map  │                  │
│  │Server│ │Server│ │Server│                  │
│  │  1  │ │  2  │ │  3  │                  │
│  └──┬──┘ └──┬──┘ └──┬──┘                  │
│     │       │       │                        │
│     └───────┼───────┘                        │
│             │                                 │
│             ↓                                 │
│  ┌─────────────────────────────────────┐    │
│  │   PostgreSQL + PostGIS              │    │
│  │   - 主从复制                         │    │
│  │   - 连接池                           │    │
│  └─────────────────────────────────────┘    │
│                                               │
│  ┌─────────────────────────────────────┐    │
│  │   Redis (分布式缓存)                 │    │
│  └─────────────────────────────────────┘    │
└──────────────────────────────────────────────┘
```

### 10.3 Docker部署

#### 10.3.1 Dockerfile

```dockerfile
# 构建阶段
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    libsqlite3-dev \
    libpng-dev \
    libwebp-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN mkdir -p build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j$(nproc)

# 运行阶段
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libpng16-16 \
    libwebp7 \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -u 1000 cycle
USER cycle

WORKDIR /app
COPY --from=builder /build/build/map_server .
COPY --from=builder /build/config/config.example.json ./config.json

RUN mkdir -p /app/data /app/cache /app/logs

EXPOSE 8080

CMD ["./map_server", "--config", "config.json"]
```

#### 10.3.2 Docker Compose

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

### 10.4 自动化部署脚本

```bash
#!/bin/bash
# deploy.sh - 自动化部署脚本

set -e

# 配置变量
APP_NAME="map-server"
APP_DIR="/opt/${APP_NAME}"
LOG_DIR="/var/log/${APP_NAME}"
DATA_DIR="/var/lib/${APP_NAME}"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    log_info "Checking dependencies..."
    
    command -v cmake >/dev/null 2>&1 || { log_error "cmake is required"; exit 1; }
    command -v g++ >/dev/null 2>&1 || { log_error "g++ is required"; exit 1; }
    
    log_info "Dependencies OK"
}

# 创建目录
create_directories() {
    log_info "Creating directories..."
    
    sudo mkdir -p ${APP_DIR}
    sudo mkdir -p ${LOG_DIR}
    sudo mkdir -p ${DATA_DIR}
    sudo mkdir -p ${DATA_DIR}/cache
    
    log_info "Directories created"
}

# 构建应用
build_app() {
    log_info "Building application..."
    
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    
    log_info "Build completed"
}

# 安装应用
install_app() {
    log_info "Installing application..."
    
    sudo cp build/map_server ${APP_DIR}/
    sudo cp config/config.example.json ${APP_DIR}/config.json
    
    log_info "Installation completed"
}

# 配置systemd服务
setup_systemd() {
    log_info "Setting up systemd service..."
    
    cat <<EOF | sudo tee /etc/systemd/system/${APP_NAME}.service
[Unit]
Description=Map Server
After=network.target

[Service]
Type=simple
User=cycle
Group=cycle
WorkingDirectory=${APP_DIR}
ExecStart=${APP_DIR}/map_server --config ${APP_DIR}/config.json
Restart=on-failure
RestartSec=5
LimitNOFILE=65536

[Install]
WantedBy=multi-user.target
EOF

    sudo systemctl daemon-reload
    sudo systemctl enable ${APP_NAME}
    
    log_info "Systemd service configured"
}

# 启动服务
start_service() {
    log_info "Starting service..."
    
    sudo systemctl start ${APP_NAME}
    sleep 2
    
    if sudo systemctl is-active --quiet ${APP_NAME}; then
        log_info "Service started successfully"
    else
        log_error "Service failed to start"
        exit 1
    fi
}

# 主流程
main() {
    log_info "Starting deployment..."
    
    check_dependencies
    create_directories
    build_app
    install_app
    setup_systemd
    start_service
    
    log_info "Deployment completed successfully!"
}

main "$@"
```

---

## 11. 运维指南

### 11.1 配置管理

#### 11.1.1 配置文件说明

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "worker_threads": 4,
    "max_request_size": 10485760,
    "timeout_seconds": 30
  },
  "database": {
    "type": "sqlite3",
    "path": "./data/spatial_data.db",
    "connection_pool_size": 5,
    "connection_timeout": 30
  },
  "cache": {
    "memory_max_items": 100,
    "disk_cache_dir": "./cache",
    "ttl_seconds": 3600
  },
  "limits": {
    "max_image_width": 4096,
    "max_image_height": 4096,
    "max_features_per_request": 10000
  },
  "log": {
    "level": 2,
    "file": "./logs/server.log",
    "max_size": 10485760,
    "max_files": 5
  },
  "output_dir": "./output"
}
```

#### 11.1.2 配置参数说明

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| server.host | string | "0.0.0.0" | 监听地址 |
| server.port | int | 8080 | 监听端口 |
| server.worker_threads | int | 4 | 工作线程数 |
| server.max_request_size | int | 10485760 | 最大请求大小（字节） |
| server.timeout_seconds | int | 30 | 请求超时时间（秒） |
| database.type | string | "sqlite3" | 数据库类型（sqlite3/postgresql） |
| database.path | string | - | SQLite数据库路径 |
| database.connection_pool_size | int | 5 | 连接池大小 |
| cache.memory_max_items | int | 100 | 内存缓存最大条目数 |
| cache.disk_cache_dir | string | "./cache" | 磁盘缓存目录 |
| cache.ttl_seconds | int | 3600 | 缓存过期时间（秒） |
| limits.max_image_width | int | 4096 | 最大图像宽度（像素） |
| limits.max_image_height | int | 4096 | 最大图像高度（像素） |
| limits.max_features_per_request | int | 10000 | 单次请求最大要素数 |
| log.level | int | 2 | 日志级别（0-4） |
| log.file | string | - | 日志文件路径 |
| log.max_size | int | 10485760 | 日志文件最大大小（字节） |
| log.max_files | int | 5 | 日志文件最大数量 |

### 11.2 日志管理

#### 11.2.1 日志级别

| 级别 | 值 | 说明 |
|------|---|------|
| TRACE | 0 | 详细跟踪信息 |
| DEBUG | 1 | 调试信息 |
| INFO | 2 | 一般信息 |
| WARN | 3 | 警告信息 |
| ERROR | 4 | 错误信息 |

#### 11.2.2 日志格式

```
[2026-03-09 10:30:00.123] [INFO] [12345] Request: GET /health - 200 OK (5ms)
[2026-03-09 10:30:01.456] [ERROR] [12346] Database error: Connection timeout
[2026-03-09 10:30:02.789] [WARN] [12347] Cache miss for key: tile:12:1204:1540
```

#### 11.2.3 日志轮转

```bash
# 使用 logrotate
cat <<EOF | sudo tee /etc/logrotate.d/map-server
/var/log/map-server/*.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 0644 cycle cycle
}
EOF
```

### 11.3 性能监控

#### 11.3.1 监控指标

```cpp
namespace cycle {

struct MonitoringMetrics {
    // 请求指标
    int64_t totalRequests;
    int64_t successRequests;
    int64_t failedRequests;
    double requestsPerSecond;
    
    // 性能指标
    double avgResponseTimeMs;
    double p95ResponseTimeMs;
    double p99ResponseTimeMs;
    
    // 缓存指标
    int64_t cacheHits;
    int64_t cacheMisses;
    double cacheHitRate;
    
    // 资源指标
    double cpuUsage;
    int64_t memoryUsageMB;
    int64_t diskUsageGB;
    
    // 数据库指标
    int activeConnections;
    int idleConnections;
    int64_t totalQueries;
};

} // namespace cycle
```

#### 11.3.2 Prometheus集成

```yaml
# prometheus.yml
scrape_configs:
  - job_name: 'map-server'
    static_configs:
      - targets: ['localhost:8080']
    metrics_path: '/metrics'
```

**暴露指标端点**:
```cpp
// GET /metrics
void HttpServer::HandleMetrics(const httplib::Request& req, httplib::Response& res) {
    std::ostringstream oss;
    
    auto metrics = m_monitor->GetMetrics();
    
    oss << "# HELP http_requests_total Total HTTP requests\n";
    oss << "# TYPE http_requests_total counter\n";
    oss << "http_requests_total " << metrics.totalRequests << "\n\n";
    
    oss << "# HELP http_request_duration_ms HTTP request duration in milliseconds\n";
    oss << "# TYPE http_request_duration_ms summary\n";
    oss << "http_request_duration_ms{quantile=\"0.95\"} " << metrics.p95ResponseTimeMs << "\n";
    oss << "http_request_duration_ms{quantile=\"0.99\"} " << metrics.p99ResponseTimeMs << "\n\n";
    
    oss << "# HELP cache_hit_rate Cache hit rate\n";
    oss << "# TYPE cache_hit_rate gauge\n";
    oss << "cache_hit_rate " << metrics.cacheHitRate << "\n";
    
    res.set_content(oss.str(), "text/plain");
}
```

### 11.4 故障排查

#### 11.4.1 常见问题

**问题1: 端口占用**
```bash
# 查看端口占用
sudo netstat -tulpn | grep 8080

# 结束占用进程
sudo kill -9 <PID>
```

**问题2: 内存不足**
```bash
# 查看内存使用
free -h

# 查看进程内存
ps aux | grep map_server

# 重启服务
sudo systemctl restart map-server
```

**问题3: 数据库连接失败**
```bash
# 检查数据库文件
ls -lh ./data/spatial_data.db

# 检查数据库连接
sqlite3 ./data/spatial_data.db "SELECT COUNT(*) FROM features;"

# 检查日志
tail -f ./logs/server.log | grep -i database
```

**问题4: 渲染性能下降**
```bash
# 检查缓存命中率
curl http://localhost:8080/health | jq '.metrics.cache_hit_rate'

# 清理缓存
rm -rf ./cache/*

# 重启服务
sudo systemctl restart map-server
```

#### 11.4.2 故障排查流程

```
1. 检查服务状态
   └─> sudo systemctl status map-server

2. 查看日志
   └─> tail -f /var/log/map-server/server.log

3. 检查资源使用
   └─> top / htop

4. 检查网络连接
   └─> netstat -tulpn

5. 检查数据库
   └─> sqlite3 /var/lib/map-server/spatial_data.db

6. 检查配置
   └─> cat /opt/map-server/config.json

7. 重启服务
   └─> sudo systemctl restart map-server
```

---

## 12. 附录

### 12.1 参考资料

1. **OGC WMTS标准**: https://www.ogc.org/standards/wmts
2. **cpp-httplib文档**: https://github.com/yhirose/cpp-httplib
3. **SQLite文档**: https://www.sqlite.org/docs.html
4. **PostgreSQL文档**: https://www.postgresql.org/docs/
5. **PostGIS文档**: https://postgis.net/documentation/
6. **libpng文档**: http://www.libpng.org/pub/png/libpng.html
7. **WebP文档**: https://developers.google.com/speed/webp
8. **CMake文档**: https://cmake.org/documentation/

### 12.2 术语表

| 术语 | 说明 |
|------|------|
| **WMTS** | Web Map Tile Service，OGC标准的地图瓦片服务规范 |
| **DPI** | Dots Per Inch，每英寸点数，衡量图像分辨率 |
| **瓦片** | 固定尺寸的地图图像块（如256x256像素） |
| **BBOX** | Bounding Box，边界框，表示地理范围 |
| **SRID** | Spatial Reference System Identifier，空间参考系统标识符 |
| **LOD** | Level of Detail，细节层次 |
| **LRU** | Least Recently Used，最近最少使用算法 |
| **P95** | 95th Percentile，第95百分位数 |

### 12.3 版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-03-09 | 初始版本，包含完整的系统设计 |

### 12.4 联系方式

**项目维护者**: Map Server Team  
**技术支持**: support@mapserver-cycle.com  
**文档反馈**: docs@mapserver-cycle.com

---

*本文档为 Map Server Cycle 项目的详细设计文档，版本 v1.0，最后更新于 2026-03-09。*
