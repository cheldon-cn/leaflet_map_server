# Map Server 增强设计说明

> **版本**: 2.0  
> **状态**: 设计稿  
> **最后更新**: 2026-03-09  
> **目标**: 提供稳定高效的地图服务（增强版）

---

## 🎯 增强特性概览

本增强设计在原有基础上添加以下关键特性：

### 1. 数据库抽象层
- ✅ **统一接口**：IDatabase 接口定义通用数据库操作
- ✅ **双后端支持**：SQLite3 和 PostgreSQL/PostGIS
- ✅ **无缝切换**：通过配置文件选择数据库后端
- ✅ **连接池管理**：统一的连接池和事务管理

### 2. 多格式图像编码
- ✅ **PNG8**：8 位索引色，小文件尺寸
- ✅ **PNG32**：32 位真彩色，高质量输出
- ✅ **WebP**：高压缩率，网络友好
- ✅ **质量参数**：1-100 可调质量

### 3. 高 DPI 支持
- ✅ **DPI 范围**：72-600 DPI
- ✅ **自动缩放**：根据 DPI 自动调整渲染尺寸
- ✅ **元数据**：DPI 信息写入图像

### 4. 渲染范围控制
- ✅ **最大范围限制**：防止超大请求
- ✅ **最小范围限制**：防止过小请求
- ✅ **边界验证**：完整的参数验证

### 5. 性能提升
- ✅ **渲染时间**：< 350ms（原 500ms）
- ✅ **并发请求**：> 200 req/s（原 100 req/s）
- ✅ **缓存命中率**：> 95%（原 90%）

### 6. C++11 和跨平台
- ✅ **C++11 标准**：完全兼容
- ✅ **命名空间**：cycle 命名空间组织
- ✅ **跨平台**：Windows/Linux/macOS
- ✅ **多线程**：完整线程安全

---

## 📐 架构更新

### 系统架构图

```
┌─────────────────────────────────────────────────┐
│              Client (Browser/App)               │
└───────────────────┬─────────────────────────────┘
                    │ HTTP/HTTPS
┌───────────────────▼─────────────────────────────┐
│         HTTP Server Layer                       │
│    (HttpServer - cpp-httplib 封装)               │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│       Business Logic Layer                      │
│  (RenderEngine, LayerManager, Cache)            │
└───────────────────┬─────────────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│     Database Abstraction Layer (NEW)            │
│         (IDatabase Interface)                   │
└────────────┬──────────────────────┬─────────────┘
             │                      │
┌────────────▼─────────────┐ ┌─────▼──────────────┐
│   SQLite3 Backend        │ │ PostgreSQL Backend │
│   (SqliteDatabase)       │ │ (PostgresqlDatabase)│
└──────────────────────────┘ └────────────────────┘
                    │
┌───────────────────▼─────────────────────────────┐
│      Encoder Abstraction Layer (NEW)            │
│         (IEncoder Interface)                    │
└────────────┬──────────────────────┬─────────────┘
             │                      │
┌────────────▼─────────────┐ ┌─────▼──────────────┐
│   PNG Encoder            │ │  WebP Encoder      │
│   (PNG8/PNG32)           │ │  (libwebp)         │
└──────────────────────────┘ └────────────────────┘
```

---

## 🗄️ 数据库抽象层详细设计

### IDatabase 接口

```cpp
namespace cycle {
namespace database {

class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    // 连接管理
    virtual bool Open(const std::string& connection_string) = 0;
    virtual void Close() = 0;
    virtual bool IsOpen() const = 0;
    
    // 事务管理
    virtual bool BeginTransaction() = 0;
    virtual bool Commit() = 0;
    virtual bool Rollback() = 0;
    
    // 查询执行
    virtual std::unique_ptr<IStatement> Prepare(const std::string& sql) = 0;
    virtual std::unique_ptr<IResultSet> ExecuteQuery(const std::string& sql) = 0;
    virtual int ExecuteUpdate(const std::string& sql) = 0;
    
    // 空间查询
    virtual std::vector<Feature> QueryFeatures(
        const BoundingBox& bbox,
        const std::string& layer,
        const std::string& filter = ""
    ) = 0;
    
    // 图层管理
    virtual std::vector<LayerInfo> GetLayers() = 0;
    virtual bool AddLayer(const LayerInfo& layer) = 0;
    
    // 错误信息
    virtual std::string GetLastError() const = 0;
};

} // namespace database
} // namespace cycle
```

### 数据库工厂

```cpp
// 使用示例
DatabaseConfig config;
config.type = "postgresql";  // 或 "sqlite3"
config.connection_string = "host=localhost port=5432 dbname=mapdb";
config.connection_pool_size = 10;

auto db = DatabaseFactory::Create(config);
db->Open(config.connection_string);
```

### SQLite3 vs PostgreSQL

| 特性 | SQLite3 | PostgreSQL |
|------|---------|------------|
| **配置** | 零配置 | 需要服务器 |
| **部署** | 单文件 | 客户端/服务器 |
| **并发** | 文件锁 | 行级锁 |
| **性能** | 适合读多写少 | 高并发 |
| **适用场景** | 开发/中小规模 | 生产/大规模 |

---

## 🖼️ 多格式编码器详细设计

### 图像格式枚举

```cpp
namespace cycle {

enum class ImageFormat {
    PNG8,    // 8 位索引色
    PNG32,   // 32 位真彩色
    WEBP     // WebP 格式
};

struct EncodeOptions {
    ImageFormat format = ImageFormat::PNG32;
    int quality = 90;           // 1-100
    int dpi = 96;               // 72-600
    bool antialias = true;      // 抗锯齿
    bool interlaced = false;    // 隔行扫描（仅 PNG）
    std::string background_color = "#ffffff";
};

} // namespace cycle
```

### 编码器接口

```cpp
namespace cycle {
namespace encoder {

class IEncoder {
public:
    virtual ~IEncoder() = default;
    
    virtual std::vector<uint8_t> Encode(
        const PngImage& image,
        const EncodeOptions& options
    ) = 0;
    
    virtual std::string GetMimeType() const = 0;
    virtual std::string GetFileExtension() const = 0;
    virtual ImageFormat GetFormat() const = 0;
};

// 使用示例
EncodeOptions options;
options.format = ImageFormat::WEBP;
options.quality = 85;
options.dpi = 192;

auto encoder = EncoderFactory::Create(ImageFormat::WEBP);
auto data = encoder->Encode(image, options);
```

### 格式对比

| 格式 | 优点 | 缺点 | 适用场景 |
|------|------|------|---------|
| **PNG8** | 文件小 (<100KB) | 颜色有限 (256 色) | 简单地图、移动端 |
| **PNG32** | 高质量、支持 alpha | 文件较大 | 标准地图、高质量 |
| **WebP** | 高压缩率 (小 30%) | 编码稍慢 | 网络传输、存储 |

---

## 📏 渲染范围控制

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

---

## ⚡ 性能优化策略

### 渲染管线优化

1. **任务队列**：RenderTaskQueue 管理渲染任务
2. **工作线程池**：多线程并行处理
3. **批量渲染**：合并绘制调用
4. **视口裁剪**：只渲染可见范围

### 并发控制

1. **Semaphore**：信号量控制并发数
2. **ReadWriteLock**：读写锁优化读多写少场景
3. **细粒度锁**：减少锁竞争

### 性能指标对比

| 指标 | 原设计 | 增强设计 | 提升 |
|------|--------|----------|------|
| 渲染时间 | < 500ms | **< 350ms** | 30% |
| 并发请求 | > 100 req/s | **> 200 req/s** | 100% |
| 缓存命中率 | > 90% | **> 95%** | 5% |
| 支持格式 | PNG | **PNG8/PNG32/WebP** | 3 种 |

---

## 🔧 C++11 和命名空间规范

### C++11 特性使用

```cpp
namespace cycle {

// auto 关键字
auto image = std::make_unique<PngImage>();

// 智能指针
std::unique_ptr<IDatabase> db = DatabaseFactory::Create(config);
std::shared_ptr<CacheManager> cache = GetCacheManager();

// Lambda 表达式
std::for_each(layers.begin(), layers.end(),
    [this](const std::string& layer) {
        LoadLayer(layer);
    }
);

// 范围 for 循环
for (const auto& feature : features) {
    RenderFeature(feature);
}

// 线程支持
std::thread worker(&RenderEngine::WorkerThread, this);
std::lock_guard<std::mutex> lock(mutex_);

} // namespace cycle
```

### 命名空间组织

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
├── utils                  # 工具函数
└── types                  # 公共类型
```

---

## 📋 配置更新

### 数据库配置

```json
{
  "database": {
    "type": "postgresql",
    "connection_string": "host=localhost port=5432 dbname=mapdb user=user password=pass",
    "connection_pool_size": 10,
    "enable_spatial": true,
    "statement_timeout": 30000
  }
}
```

### 编码器配置

```json
{
  "encoder": {
    "default_format": "png32",
    "default_quality": 90,
    "default_dpi": 96,
    "antialias": true,
    "webp_method": 4,
    "webp_lossless": false
  }
}
```

### 范围限制配置

```json
{
  "limits": {
    "max_bbox_width": 360.0,
    "max_bbox_height": 180.0,
    "min_bbox_width": 0.0001,
    "min_bbox_height": 0.0001,
    "max_image_width": 8192,
    "max_image_height": 8192,
    "min_dpi": 72,
    "max_dpi": 600
  }
}
```

---

## 🚀 部署说明

### PostgreSQL 部署

```bash
# Ubuntu/Debian
sudo apt-get install postgresql postgresql-contrib postgis

# 启用 PostGIS
psql -d mapdb -c "CREATE EXTENSION postgis;"
```

### libwebp 安装

```bash
# Ubuntu/Debian
sudo apt-get install libwebp-dev

# macOS
brew install webp

# Windows (vcpkg)
vcpkg install libwebp
```

### Docker 配置更新

```dockerfile
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    libsqlite3-dev \
    libsqlite3-mod-spatialite \
    libpq-dev \
    libpng-dev \
    libwebp-dev \
    && rm -rf /var/lib/apt/lists/*
```

---

## 📊 迁移指南

### 从旧版本迁移

1. **数据库迁移**
   - 保持向后兼容，无需修改现有代码
   - 通过配置文件选择数据库后端
   - SQLite 数据可迁移到 PostgreSQL

2. **API 迁移**
   - 新增 format/quality/dpi 参数（可选）
   - 默认行为保持不变（PNG32, 96 DPI）
   - 向后 100% 兼容

3. **配置迁移**
   - 添加 encoder 配置块
   - 添加 limits 配置块
   - 数据库配置支持 type 字段

---

## ✅ 验收标准

### 功能验收
- [ ] SQLite3 和 PostgreSQL 双后端正常工作
- [ ] PNG8/PNG32/WebP 格式编码正确
- [ ] DPI 设置 72-600 有效
- [ ] 范围限制验证正确
- [ ] 所有 API 向后兼容

### 性能验收
- [ ] 渲染时间 < 350ms（95% 请求）
- [ ] 并发请求 > 200 req/s
- [ ] 内存使用 < 150MB
- [ ] 缓存命中率 > 95%

### 质量验收
- [ ] C++11 编译通过（GCC/Clang/MSVC）
- [ ] Windows/Linux/macOS 全平台通过
- [ ] 单元测试覆盖率 > 85%
- [ ] 无内存泄漏

---

**文档结束**
