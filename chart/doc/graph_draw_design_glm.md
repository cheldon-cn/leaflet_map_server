# 图形绘制框架设计文档 v2.1

**版本**: 2.1  
**日期**: 2026年3月18日  
**状态**: 设计中  
**C++标准**: C++11  
**设计目标**: 稳定、正确、高效、易扩展、跨平台、多线程安全、GIS标准兼容

---

[TOC]

---

## 1. 概述

### 1.1 设计目标

本图形绘制框架采用**设备-引擎-驱动三层架构**，致力于构建一个**生产级**的跨平台图形绘制系统。

**核心设计目标**:

| 目标 | 说明 | 实现策略 |
|------|------|----------|
| **稳定性** | 健壮的错误处理、资源管理 | Result模式、RAII、异常安全 |
| **正确性** | DPI正确处理、坐标精确变换 | 完整测试、交叉验证 |
| **高效性** | 批量绘制、硬件加速、缓存 | 空间索引、延迟渲染、内存池 |
| **扩展性** | 易于添加新设备和引擎 | 抽象工厂、策略模式 |
| **跨平台** | Windows/Linux/macOS | 纯C++11、平台抽象层 |
| **多线程安全** | 并发绘制、资源隔离 | 读写锁、线程局部存储 |

### 1.2 核心特性

- ✅ **三层架构**: 设备、引擎、驱动清晰分层
- ✅ **多种输出设备**: 栅格图像、显示窗口、PDF、矢量文件
- ✅ **多种渲染引擎**: Qt、Cairo、GDI、OpenGL等
- ✅ **完整符号化**: 点、线、面符号化器
- ✅ **DPI感知**: 逻辑尺寸与物理尺寸分离
- ✅ **图层管理**: 多图层、透明度、混合模式
- ✅ **线程安全**: 并发绘制、资源池
- ✅ **批量优化**: 批量绘制接口、空间索引
- ✅ **瓦片渲染**: 矢量瓦片/栅格瓦片支持
- ✅ **空间参考**: 坐标系统转换支持
- ✅ **符号化规则**: SLD/SE标准支持
- ✅ **LOD机制**: 细节层次动态调整
- ✅ **异步渲染**: 后台渲染+进度回调
- ✅ **性能监控**: 渲染性能指标采集

### 1.3 架构总览

```
┌─────────────────────────────────────────┐
│       DrawFacade (门面层)                │
│   提供简化的高层绘制接口                  │
└────────────┬────────────────────────────┘
             │
┌────────────▼────────────────────────────┐
│       DrawDriver (驱动层)                │
│   工厂 + 要素绘制 + 管理器                │
└────┬─────────────────────┬───────────────┘
     │                     │
┌────▼─────┐         ┌─────▼────────┐
│  Device  │◄────────│    Engine    │
│ (设备层) │ weak_ptr│   (引擎层)    │
│          │─────────►│              │
│ 图片/显示 │         │ Qt/Cairo/GDI │
│ /PDF/矢量 │         │              │
└──────────┘         └──────────────┘
```

**架构职责**:

- **设备层 (Device)**: 描述"画到哪里"，管理输出目标（图片、窗口、PDF等）
- **引擎层 (Engine)**: 描述"怎么画"，封装具体图形库的绘制逻辑
- **驱动层 (Driver)**: 工厂角色，协调设备和引擎的创建与协作
- **门面层 (Facade)**: 简化接口，提供一键绘制的便捷方法

---

## 2. 命名规范

### 2.1 命名风格

本框架采用 **Google C++ Style** 命名规范：

| 类型 | 风格 | 示例 |
|------|------|------|
| 枚举值 | k + PascalCase | `kSuccess`, `kInvalidParams` |
| 函数/方法 | PascalCase | `GetWidth()`, `DrawGeometry()` |
| 变量 | snake_case | `width`, `device_type` |
| 成员变量 | 下划线后缀 | `width_`, `type_` |
| 类/结构体 | PascalCase | `DrawParams`, `DrawDevice` |
| 常量 | k + PascalCase | `kMaxBufferSize`, `kDefaultDpi` |
| 命名空间 | 小写 | `ogc`, `draw` |

### 2.2 命名空间

```cpp
namespace ogc {
namespace draw {
    // 所有绘制相关类和工具
}
}
```

---

## 3. 公共定义

### 3.1 错误码定义

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 绘制结果枚举
 */
enum class DrawResult {
    // 成功
    kSuccess = 0,
    
    // 通用错误 (1-99)
    kFailed = 1,
    kInvalidParams = 2,
    kNullPointer = 3,
    
    // 设备错误 (100-199)
    kDeviceError = 100,
    kDeviceNotInitialized = 101,
    kDeviceAlreadyActive = 102,
    kDeviceCreationFailed = 103,
    
    // 引擎错误 (200-299)
    kEngineError = 200,
    kEngineNotInitialized = 201,
    kEngineNotSupported = 202,
    kEngineCreationFailed = 203,
    
    // 驱动错误 (300-399)
    kDriverError = 300,
    kDriverNotFound = 301,
    kDriverNotInitialized = 302,
    
    // 绘制错误 (400-499)
    kDrawError = 400,
    kInvalidGeometry = 401,
    kInvalidStyle = 402,
    kCoordinateTransformFailed = 403,
    
    // I/O错误 (500-599)
    kIOError = 500,
    kFileNotFound = 501,
    kFileWriteFailed = 502,
    kInvalidFormat = 503,
    
    // 资源错误 (600-699)
    kOutOfMemory = 600,
    kResourceExhausted = 601,
    
    // 其他
    kNotSupported = 999,
    kNotImplemented = 1000
};

/**
 * @brief 获取错误描述
 */
const char* getResultString(DrawResult result);
std::string getResultDescription(DrawResult result);

} // namespace draw
} // namespace ogc
```

### 3.2 设备类型

```cpp
/**
 * @brief 设备类型枚举
 */
enum class DeviceType {
    kUnknown = 0,
    
    // 栅格设备
    kRasterImage,    // PNG, JPG, BMP等
    
    // 显示设备
    kDisplayDevice,  // 屏幕、窗口
    
    // 矢量设备
    kVectorFile,     // SVG, DXF等
    kPdfDevice,      // PDF文档
    
    // 高级设备
    kPrinterDevice,  // 打印机
    kMemoryBuffer,   // 内存缓冲
    kTexture,        // GPU纹理
    
    // 扩展
    kCustomDevice
};

/**
 * @brief 图片格式
 */
enum class ImageFormat {
    PNG,
    JPEG,
    BMP,
    TIFF,
    GIF,
    WebP
};

/**
 * @brief 矢量格式
 */
enum class VectorFormat {
    SVG,
    DXF,
    EMF,
    WMF,
    PDF,
    EPS
};
```

### 3.3 引擎类型

```cpp
/**
 * @brief 引擎类型枚举
 */
enum class EngineType {
    kUnknown = 0,
    
    // 软件渲染
    kQtEngine,       // Qt QPainter
    kCairoEngine,    // Cairo图形库
    kAggEngine,      // Anti-Grain Geometry
    
    // 平台特定
    kGdiEngine,      // Windows GDI/GDI+
    kDirect2DEngine, // Windows Direct2D
    kCoreGraphics,   // macOS/iOS Core Graphics
    
    // 硬件加速
    kOpenGLEngine,   // OpenGL
    kVulkanEngine,   // Vulkan
    kMetalEngine,    // Metal (macOS/iOS)
    
    // 其他
    kSkiaEngine,     // Skia
    kCustomEngine
};
```

### 3.4 绘制参数

```cpp
/**
 * @brief 绘制参数结构
 */
struct DrawParams {
    // 空间范围
    double extentMinX = 0.0;
    double extentMinY = 0.0;
    double extentMaxX = 100.0;
    double extentMaxY = 100.0;
    
    // 逻辑尺寸（基于96 DPI）
    int width = 800;
    int height = 600;
    
    // DPI设置
    double dpi = 96.0;
    bool useDpiScaling = true;
    
    // 背景
    std::string backgroundColor = "#FFFFFF";
    
    // 抗锯齿
    bool antiAlias = true;
    int antiAliasSamples = 4;
    
    // 线程安全
    bool threadSafe = true;
    
    // 默认构造
    DrawParams() = default;
    
    // 便捷构造
    DrawParams(double minX, double minY, double maxX, double maxY,
               int w = 800, int h = 600, double d = 96.0)
        : extentMinX(minX), extentMinY(minY)
        , extentMaxX(maxX), extentMaxY(maxY)
        , width(w), height(h), dpi(d) {}
    
    // DPI比率
    double getDpiRatio() const {
        return (dpi > 0 && useDpiScaling) ? dpi / 96.0 : 1.0;
    }
    
    // 物理尺寸
    int getPhysicalWidth() const {
        return static_cast<int>(width * getDpiRatio() + 0.5);
    }
    
    int getPhysicalHeight() const {
        return static_cast<int>(height * getDpiRatio() + 0.5);
    }
    
    // 范围宽度/高度
    double getExtentWidth() const {
        return extentMaxX - extentMinX;
    }
    
    double getExtentHeight() const {
        return extentMaxY - extentMinY;
    }
    
    // 比例尺
    double getScaleX() const {
        return width / getExtentWidth();
    }
    
    double getScaleY() const {
        return height / getExtentHeight();
    }
};
```

### 3.5 绘制样式

```cpp
/**
 * @brief 线条端点样式
 */
enum class LineCapStyle {
    Butt,    // 平头
    Round,   // 圆头
    Square   // 方头
};

/**
 * @brief 线条连接样式
 */
enum class LineJoinStyle {
    Miter,   // 尖角
    Round,   // 圆角
    Bevel    // 斜角
};

/**
 * @brief 填充规则
 */
enum class FillRule {
    EvenOdd,  // 奇偶规则
    NonZero   // 非零规则
};

/**
 * @brief 填充图案
 */
enum class FillPattern {
    Solid,       // 纯色
    Horizontal,  // 水平线
    Vertical,    // 垂直线
    Cross,       // 十字线
    Diagonal,    // 斜线
    Dots,        // 点状
    Image        // 图片图案
};

/**
 * @brief 混合模式
 */
enum class BlendMode {
    Normal,     // 正常
    Multiply,   // 正片叠底
    Screen,     // 滤色
    Overlay,    // 叠加
    Darken,     // 变暗
    Lighten     // 变亮
};

/**
 * @brief 绘制样式结构
 */
struct DrawStyle {
    // 边框样式
    std::string strokeColor = "#000000";
    double strokeWidth = 1.0;
    LineCapStyle strokeCap = LineCapStyle::Round;
    LineJoinStyle strokeJoin = LineJoinStyle::Round;
    std::vector<double> dashPattern;  // 虚线模式
    double dashOffset = 0.0;
    
    // 填充样式
    std::string fillColor = "#808080";
    FillRule fillRule = FillRule::EvenOdd;
    FillPattern fillPattern = FillPattern::Solid;
    std::string fillImagePath;  // 图案填充图片
    
    // 透明度
    double opacity = 1.0;
    double strokeOpacity = 1.0;
    double fillOpacity = 1.0;
    
    // 显示控制
    bool fill = true;
    bool stroke = true;
    
    // 图层
    int layer = 0;
    
    // 混合模式
    BlendMode blendMode = BlendMode::Normal;
    
    // 默认构造
    DrawStyle() = default;
    
    // 便捷构造
    DrawStyle(const std::string& stroke, const std::string& fill, double width = 1.0)
        : strokeColor(stroke), fillColor(fill), strokeWidth(width) {}
    
    // 静态工厂
    static DrawStyle solid(const std::string& color) {
        return DrawStyle(color, color);
    }
    
    static DrawStyle strokeOnly(const std::string& color, double width = 1.0) {
        DrawStyle style;
        style.strokeColor = color;
        style.strokeWidth = width;
        style.fill = false;
        return style;
    }
    
    static DrawStyle fillOnly(const std::string& color) {
        DrawStyle style;
        style.fillColor = color;
        style.stroke = false;
        return style;
    }
};
```

### 3.6 类型别名

```cpp
namespace ogc {
namespace draw {

// 前向声明
class DrawDevice;
class DrawEngine;
class DrawDriver;
class DrawContext;
class DrawFacade;

// 智能指针
using DrawDevicePtr = std::shared_ptr<DrawDevice>;
using DrawEnginePtr = std::shared_ptr<DrawEngine>;
using DrawDriverPtr = std::shared_ptr<DrawDriver>;
using DrawContextPtr = std::unique_ptr<DrawContext>;

} // namespace draw
} // namespace ogc
```

---

## 4. GIS核心特性设计

### 4.1 瓦片渲染支持

#### 4.1.1 瓦片抽象

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 瓦片键（唯一标识一个瓦片）
 */
struct TileKey {
    int z = 0;  // 缩放级别
    int x = 0;  // 列号
    int y = 0;  // 行号
    
    TileKey() = default;
    TileKey(int level, int col, int row) : z(level), x(col), y(row) {}
    
    bool operator==(const TileKey& other) const {
        return z == other.z && x == other.x && y == other.y;
    }
    
    size_t hash() const {
        return std::hash<int>()(z) ^ (std::hash<int>()(x) << 1) ^ (std::hash<int>()(y) << 2);
    }
    
    std::string toString() const {
        return std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y);
    }
};

/**
 * @brief 瓦片金字塔
 */
class TilePyramid {
public:
    TilePyramid(int minLevel = 0, int maxLevel = 22);
    
    // 坐标与瓦片转换
    TileKey coordToTile(double x, double y, int z) const;
    ogc::Envelope tileToEnvelope(const TileKey& key) const;
    std::pair<double, double> tileToCoord(const TileKey& key, double px, double py) const;
    
    // 瓦片查询
    std::vector<TileKey> getTilesForExtent(const ogc::Envelope& extent, int z) const;
    
    // 瓦片尺寸
    int getTileSize() const { return tileSize_; }
    void setTileSize(int size) { tileSize_ = size; }
    
    // 级别范围
    int getMinLevel() const { return minLevel_; }
    int getMaxLevel() const { return maxLevel_; }
    
private:
    int tileSize_ = 256;
    int minLevel_ = 0;
    int maxLevel_ = 22;
};

} // namespace draw
} // namespace ogc
```

#### 4.1.2 瓦片设备

```cpp
/**
 * @brief 瓦片设备
 */
class TileDevice : public DrawDevice {
public:
    static DrawDevicePtr create(const TileKey& key, 
                                int tileSize = 256,
                                double dpi = 96.0);
    
    // 实现基类接口
    int getWidth() const override { return tileSize_; }
    int getHeight() const override { return height_; }
    double getDpi() const override { return dpi_; }
    
    DrawResult initialize(const DrawParams& params) override;
    DrawResult beginDraw() override;
    DrawResult endDraw() override;
    DrawResult clear() override;
    DrawResult saveToFile(const std::string& filePath) override;
    
    // 瓦片特有接口
    const TileKey& getTileKey() const { return tileKey_; }
    void setTileKey(const TileKey& key);
    
    // 获取瓦片数据（用于缓存）
    const unsigned char* getTileData() const;
    size_t getTileDataSize() const;
    
    // 设置瓦片金字塔
    void setPyramid(std::shared_ptr<TilePyramid> pyramid);
    
private:
    explicit TileDevice(const TileKey& key, int tileSize, double dpi);
    
    TileKey tileKey_;
    int tileSize_ = 256;
    double dpi_ = 96.0;
    std::shared_ptr<TilePyramid> pyramid_;
};

/**
 * @brief 瓦片渲染器
 */
class TileRenderer {
public:
    TileRenderer();
    
    // 渲染单个瓦片
    DrawResult renderTile(const TileKey& key,
                          const std::vector<ogc::FeaturePtr>& features,
                          SymbolizerPtr symbolizer,
                          std::vector<unsigned char>& tileData);
    
    // 批量渲染瓦片
    DrawResult renderTiles(const std::vector<TileKey>& keys,
                           const std::vector<ogc::FeaturePtr>& features,
                           SymbolizerPtr symbolizer,
                           std::map<TileKey, std::vector<unsigned char>>& tiles);
    
    // 设置瓦片金字塔
    void setPyramid(std::shared_ptr<TilePyramid> pyramid);
    
    // 设置渲染引擎
    void setEngine(DrawEnginePtr engine);
    
private:
    std::shared_ptr<TilePyramid> pyramid_;
    DrawEnginePtr engine_;
};
```

#### 4.1.3 瓦片缓存

```cpp
/**
 * @brief 瓦片缓存接口
 */
class TileCache {
public:
    virtual ~TileCache() = default;
    
    // 获取瓦片
    virtual bool get(const TileKey& key, std::vector<unsigned char>& data) = 0;
    
    // 存储瓦片
    virtual void put(const TileKey& key, const std::vector<unsigned char>& data) = 0;
    
    // 检查是否存在
    virtual bool exists(const TileKey& key) = 0;
    
    // 删除瓦片
    virtual void remove(const TileKey& key) = 0;
    
    // 清空缓存
    virtual void clear() = 0;
    
    // 缓存统计
    virtual size_t size() const = 0;
    virtual size_t memoryUsage() const = 0;
};

/**
 * @brief 内存瓦片缓存
 */
class MemoryTileCache : public TileCache {
public:
    explicit MemoryTileCache(size_t maxSize = 100 * 1024 * 1024);  // 默认100MB
    
    bool get(const TileKey& key, std::vector<unsigned char>& data) override;
    void put(const TileKey& key, const std::vector<unsigned char>& data) override;
    bool exists(const TileKey& key) override;
    void remove(const TileKey& key) override;
    void clear() override;
    size_t size() const override;
    size_t memoryUsage() const override;
    
    // 缓存策略
    void setMaxSize(size_t size);
    void setEvictionPolicy(EvictionPolicy policy);  // LRU, LFU, FIFO
    
private:
    std::unordered_map<TileKey, std::vector<unsigned char>, TileKeyHash> cache_;
    size_t maxSize_;
    size_t currentSize_ = 0;
    std::mutex mutex_;
};

/**
 * @brief 磁盘瓦片缓存
 */
class DiskTileCache : public TileCache {
public:
    explicit DiskTileCache(const std::string& cacheDir, 
                           size_t maxSize = 1024 * 1024 * 1024);  // 默认1GB
    
    bool get(const TileKey& key, std::vector<unsigned char>& data) override;
    void put(const TileKey& key, const std::vector<unsigned char>& data) override;
    bool exists(const TileKey& key) override;
    void remove(const TileKey& key) override;
    void clear() override;
    size_t size() const override;
    size_t memoryUsage() const override;
    
private:
    std::string cacheDir_;
    size_t maxSize_;
    std::string getCachePath(const TileKey& key) const;
};

/**
 * @brief 多级瓦片缓存
 */
class MultiLevelTileCache : public TileCache {
public:
    void addCache(std::shared_ptr<TileCache> cache, int priority);
    
    bool get(const TileKey& key, std::vector<unsigned char>& data) override;
    void put(const TileKey& key, const std::vector<unsigned char>& data) override;
    
private:
    std::vector<std::pair<int, std::shared_ptr<TileCache>>> caches_;
};
```

---

### 4.2 空间参考系统

#### 4.2.1 坐标转换接口

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 坐标转换器接口
 */
class CoordinateTransformer {
public:
    virtual ~CoordinateTransformer() = default;
    
    // 单点转换
    virtual bool transform(double& x, double& y) const = 0;
    virtual bool transformInverse(double& x, double& y) const = 0;
    
    // 范围转换
    virtual ogc::Envelope transformExtent(const ogc::Envelope& extent) const = 0;
    
    // 批量转换（性能优化）
    virtual bool transformBatch(double* coords, size_t count) const = 0;
    virtual bool transformInverseBatch(double* coords, size_t count) const = 0;
    
    // 坐标系信息
    virtual std::string getSourceSrs() const = 0;
    virtual std::string getTargetSrs() const = 0;
    virtual bool isValid() const = 0;
};

/**
 * @brief Proj库坐标转换器
 */
class ProjTransformer : public CoordinateTransformer {
public:
    /**
     * @brief 创建坐标转换器
     * @param sourceSrs 源坐标系 (如 "EPSG:4326", "EPSG:3857")
     * @param targetSrs 目标坐标系
     */
    static std::unique_ptr<ProjTransformer> create(
        const std::string& sourceSrs,
        const std::string& targetSrs);
    
    bool transform(double& x, double& y) const override;
    bool transformInverse(double& x, double& y) const override;
    ogc::Envelope transformExtent(const ogc::Envelope& extent) const override;
    bool transformBatch(double* coords, size_t count) const override;
    bool transformInverseBatch(double* coords, size_t count) const override;
    
    std::string getSourceSrs() const override { return sourceSrs_; }
    std::string getTargetSrs() const override { return targetSrs_; }
    bool isValid() const override { return valid_; }
    
private:
    ProjTransformer(const std::string& sourceSrs, const std::string& targetSrs);
    
    std::string sourceSrs_;
    std::string targetSrs_;
    void* projContext_ = nullptr;  // PJ_CONTEXT*
    void* projTransform_ = nullptr; // PJ*
    bool valid_ = false;
};

/**
 * @brief 坐标转换管理器
 */
class TransformManager {
public:
    static TransformManager& getInstance();
    
    // 获取或创建转换器（带缓存）
    std::shared_ptr<CoordinateTransformer> getTransformer(
        const std::string& sourceSrs,
        const std::string& targetSrs);
    
    // 清除缓存
    void clearCache();
    
    // 常用坐标系
    static const std::string WGS84;      // EPSG:4326
    static const std::string WebMercator; // EPSG:3857
    static const std::string GCJ02;      // 国测局坐标
    static const std::string BD09;       // 百度坐标
    
private:
    std::map<std::pair<std::string, std::string>, 
             std::weak_ptr<CoordinateTransformer>> cache_;
    std::mutex mutex_;
};

} // namespace draw
} // namespace ogc
```

#### 4.2.2 扩展DrawParams

```cpp
/**
 * @brief 绘制参数结构（扩展版）
 */
struct DrawParams {
    // 空间范围
    double extentMinX = 0.0;
    double extentMinY = 0.0;
    double extentMaxX = 100.0;
    double extentMaxY = 100.0;
    
    // 逻辑尺寸（基于96 DPI）
    int width = 800;
    int height = 600;
    
    // DPI设置
    double dpi = 96.0;
    bool useDpiScaling = true;
    
    // 背景
    std::string backgroundColor = "#FFFFFF";
    
    // 抗锯齿
    bool antiAlias = true;
    int antiAliasSamples = 4;
    
    // 线程安全
    bool threadSafe = true;
    
    // ========== GIS扩展字段 ==========
    
    // 空间参考系统
    std::string sourceSrs;      // 源坐标系 (如 "EPSG:4326")
    std::string targetSrs;      // 目标坐标系 (如 "EPSG:3857")
    bool enableTransform = true; // 是否启用坐标转换
    
    // 坐标转换器（可选，用于自定义转换）
    std::shared_ptr<CoordinateTransformer> transformer;
    
    // LOD设置
    double currentScale = 1.0;  // 当前比例尺
    bool enableLOD = true;      // 是否启用LOD
    double simplificationTolerance = 0.0; // 简化容差
    
    // 瓦片设置
    bool isTileMode = false;    // 是否瓦片模式
    TileKey tileKey;            // 当前瓦片键
    
    // 默认构造
    DrawParams() = default;
    
    // 便捷构造
    DrawParams(double minX, double minY, double maxX, double maxY,
               int w = 800, int h = 600, double d = 96.0)
        : extentMinX(minX), extentMinY(minY)
        , extentMaxX(maxX), extentMaxY(maxY)
        , width(w), height(h), dpi(d) {}
    
    // DPI比率
    double getDpiRatio() const {
        return (dpi > 0 && useDpiScaling) ? dpi / 96.0 : 1.0;
    }
    
    // 物理尺寸
    int getPhysicalWidth() const {
        return static_cast<int>(width * getDpiRatio() + 0.5);
    }
    
    int getPhysicalHeight() const {
        return static_cast<int>(height * getDpiRatio() + 0.5);
    }
    
    // 范围宽度/高度
    double getExtentWidth() const {
        return extentMaxX - extentMinX;
    }
    
    double getExtentHeight() const {
        return extentMaxY - extentMinY;
    }
    
    // 比例尺
    double getScaleX() const {
        return width / getExtentWidth();
    }
    
    double getScaleY() const {
        return height / getExtentHeight();
    }
    
    // 获取坐标转换器
    std::shared_ptr<CoordinateTransformer> getOrCreateTransformer() const {
        if (transformer) return transformer;
        if (sourceSrs.empty() || targetSrs.empty()) return nullptr;
        if (sourceSrs == targetSrs) return nullptr;
        return TransformManager::getInstance().getTransformer(sourceSrs, targetSrs);
    }
};
```

---

### 4.3 符号化规则引擎

#### 4.3.1 过滤器

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 过滤器接口（SLD Filter）
 */
class Filter {
public:
    virtual ~Filter() = default;
    
    // 评估过滤器
    virtual bool evaluate(const ogc::Feature* feature) const = 0;
    
    // 克隆
    virtual std::unique_ptr<Filter> clone() const = 0;
    
    // 序列化
    virtual std::string toXml() const = 0;
    
    // 工厂方法
    static std::unique_ptr<Filter> parse(const std::string& expression);
};

/**
 * @brief 属性比较过滤器
 */
class PropertyIsFilter : public Filter {
public:
    enum class Operator {
        EqualTo,
        NotEqualTo,
        LessThan,
        GreaterThan,
        LessThanOrEqualTo,
        GreaterThanOrEqualTo,
        Like,
        IsNull,
        IsBetween
    };
    
    PropertyIsFilter(const std::string& propertyName,
                     Operator op,
                     const std::string& value);
    
    bool evaluate(const ogc::Feature* feature) const override;
    std::unique_ptr<Filter> clone() const override;
    std::string toXml() const override;
    
private:
    std::string propertyName_;
    Operator operator_;
    std::string value_;
    std::string lowerValue_;  // for Between
    std::string upperValue_;
};

/**
 * @brief 逻辑过滤器
 */
class LogicFilter : public Filter {
public:
    enum class Operator {
        And,
        Or,
        Not
    };
    
    LogicFilter(Operator op);
    
    void addFilter(std::unique_ptr<Filter> filter);
    bool evaluate(const ogc::Feature* feature) const override;
    std::unique_ptr<Filter> clone() const override;
    std::string toXml() const override;
    
private:
    Operator operator_;
    std::vector<std::unique_ptr<Filter>> filters_;
};

/**
 * @brief 空间过滤器
 */
class SpatialFilter : public Filter {
public:
    enum class Operator {
        Intersects,
        Within,
        Contains,
        Crosses,
        Overlaps,
        Touches,
        Equals,
        Disjoint,
        BBox
    };
    
    SpatialFilter(Operator op,
                  const std::string& propertyName,
                  const ogc::Geometry* geometry);
    
    bool evaluate(const ogc::Feature* feature) const override;
    std::unique_ptr<Filter> clone() const override;
    std::string toXml() const override;
    
private:
    Operator operator_;
    std::string propertyName_;
    std::unique_ptr<ogc::Geometry> geometry_;
};

} // namespace draw
} // namespace ogc
```

#### 4.3.2 符号化规则

```cpp
/**
 * @brief 符号化规则
 */
class SymbolizerRule {
public:
    SymbolizerRule();
    
    // 规则条件
    void setFilter(std::unique_ptr<Filter> filter);
    bool isApplicable(const ogc::Feature* feature) const;
    
    // 比例尺范围
    void setScaleRange(double minScale, double maxScale);
    bool isScaleApplicable(double scale) const;
    
    // 获取符号化器
    void setSymbolizer(SymbolizerPtr symbolizer);
    SymbolizerPtr getSymbolizer(const ogc::Feature* feature) const;
    
    // 规则属性
    std::string name;
    std::string title;
    std::string abstract;
    int priority = 0;
    bool elseFilter = false;  // 默认规则
    
private:
    std::unique_ptr<Filter> filter_;
    double minScale_ = 0.0;
    double maxScale_ = DBL_MAX;
    SymbolizerPtr symbolizer_;
};

/**
 * @brief 规则引擎
 */
class RuleEngine {
public:
    RuleEngine();
    
    // 规则管理
    void addRule(std::shared_ptr<SymbolizerRule> rule);
    void removeRule(const std::string& name);
    void clearRules();
    
    // 匹配符号化器
    SymbolizerPtr matchSymbolizer(const ogc::Feature* feature, double scale) const;
    
    // 获取所有适用规则
    std::vector<SymbolizerPtr> matchAllSymbolizers(
        const ogc::Feature* feature, double scale) const;
    
    // 规则排序
    void sortRulesByPriority();
    
private:
    std::vector<std::shared_ptr<SymbolizerRule>> rules_;
    std::shared_ptr<SymbolizerRule> elseRule_;
};
```

#### 4.3.3 SLD解析器

```cpp
/**
 * @brief SLD（Styled Layer Descriptor）解析器
 */
class SLDParser {
public:
    /**
     * @brief 解析SLD XML字符串
     */
    static std::unique_ptr<RuleEngine> parse(const std::string& sldXml);
    
    /**
     * @brief 解析SLD文件
     */
    static std::unique_ptr<RuleEngine> parseFile(const std::string& filePath);
    
    /**
     * @brief 解析SE（Symbology Encoding）字符串
     */
    static SymbolizerPtr parseSymbolizer(const std::string& seXml);
    
    /**
     * @brief 导出为SLD XML
     */
    static std::string toXml(const RuleEngine& engine);
    
private:
    // 解析各种符号化器
    static SymbolizerPtr parsePointSymbolizer(const void* node);
    static SymbolizerPtr parseLineSymbolizer(const void* node);
    static SymbolizerPtr parsePolygonSymbolizer(const void* node);
    static SymbolizerPtr parseTextSymbolizer(const void* node);
    static SymbolizerPtr parseRasterSymbolizer(const void* node);
    
    // 解析过滤器
    static std::unique_ptr<Filter> parseFilter(const void* node);
};

/**
 * @brief Mapbox Style解析器
 */
class MapboxStyleParser {
public:
    struct StyleConfig {
        std::string name;
        std::string version;
        std::vector<std::string> sources;
        std::vector<LayerConfig> layers;
        std::string sprite;
        std::string glyphs;
    };
    
    struct LayerConfig {
        std::string id;
        std::string type;  // fill, line, symbol, circle, etc.
        std::string source;
        std::string sourceLayer;
        int minZoom = 0;
        int maxZoom = 22;
        
        // Paint属性
        std::map<std::string, std::string> paint;
        
        // Layout属性
        std::map<std::string, std::string> layout;
        
        // 过滤器
        std::string filter;
    };
    
    /**
     * @brief 解析Mapbox Style JSON
     */
    static std::unique_ptr<StyleConfig> parse(const std::string& json);
    static std::unique_ptr<StyleConfig> parseFile(const std::string& filePath);
    
    /**
     * @brief 转换为规则引擎
     */
    static std::unique_ptr<RuleEngine> toRuleEngine(const StyleConfig& config);
};
```

---

### 4.4 LOD机制

#### 4.4.1 LOD级别定义

```cpp
namespace ogc {
namespace draw {

/**
 * @brief LOD级别
 */
struct LODLevel {
    double minScale = 0.0;      // 最小比例尺
    double maxScale = DBL_MAX;  // 最大比例尺
    double simplification = 0.0; // 简化容差
    int minFeatureSize = 0;     // 最小要素尺寸（像素）
    bool enableLabel = true;    // 是否显示标注
    int maxLabelCount = 100;    // 最大标注数量
    double labelMinScale = 0.0; // 标注最小比例尺
    
    bool isApplicable(double scale) const {
        return scale >= minScale && scale < maxScale;
    }
};

/**
 * @brief LOD策略接口
 */
class LODStrategy {
public:
    virtual ~LODStrategy() = default;
    
    // 获取LOD级别
    virtual LODLevel getLODLevel(double scale) const = 0;
    
    // 判断是否应该渲染
    virtual bool shouldRender(const ogc::Feature* feature, 
                              const LODLevel& level,
                              const DrawParams& params) const = 0;
    
    // 简化几何
    virtual ogc::GeometryPtr simplify(const ogc::Geometry* geometry,
                                       double tolerance) const = 0;
    
    // 简化坐标序列
    virtual std::vector<double> simplifyCoords(
        const std::vector<double>& coords,
        double tolerance) const = 0;
};

/**
 * @brief 默认LOD策略
 */
class DefaultLODStrategy : public LODStrategy {
public:
    DefaultLODStrategy() {
        initializeDefaultLevels();
    }
    
    LODLevel getLODLevel(double scale) const override;
    bool shouldRender(const ogc::Feature* feature, 
                      const LODLevel& level,
                      const DrawParams& params) const override;
    ogc::GeometryPtr simplify(const ogc::Geometry* geometry,
                               double tolerance) const override;
    std::vector<double> simplifyCoords(
        const std::vector<double>& coords,
        double tolerance) const override;
    
    // 自定义级别
    void addLODLevel(const LODLevel& level);
    void clearLevels();
    
private:
    std::vector<LODLevel> levels_;
    
    void initializeDefaultLevels();
};

/**
 * @brief LOD管理器
 */
class LODManager {
public:
    static LODManager& getInstance();
    
    // 设置策略
    void setStrategy(std::shared_ptr<LODStrategy> strategy);
    std::shared_ptr<LODStrategy> getStrategy() const;
    
    // 便捷方法
    LODLevel getLODLevel(double scale) const;
    bool shouldRender(const ogc::Feature* feature,
                      double scale,
                      const DrawParams& params) const;
    ogc::GeometryPtr simplify(const ogc::Geometry* geometry,
                               double scale) const;
    
private:
    std::shared_ptr<LODStrategy> strategy_;
};
```

---

### 4.5 异步渲染

#### 4.5.1 渲染任务

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 渲染任务状态
 */
enum class TaskState {
    kPending,    // 等待中
    kRunning,    // 运行中
    kCompleted,  // 已完成
    kCancelled,  // 已取消
    kFailed      // 失败
};

/**
 * @brief 渲染任务
 */
class RenderTask : public std::enable_shared_from_this<RenderTask> {
public:
    using ProgressCallback = std::function<void(int percent, const std::string& stage)>;
    using CompleteCallback = std::function<void(DrawResult result, const std::string& message)>;
    
    RenderTask();
    ~RenderTask();
    
    // 回调设置
    void setProgressCallback(ProgressCallback callback);
    void setCompleteCallback(CompleteCallback callback);
    
    // 控制
    void cancel();
    bool isCancelled() const;
    bool isRunning() const;
    bool isCompleted() const;
    
    // 状态
    TaskState getState() const;
    int getProgress() const;  // 0-100
    std::string getCurrentStage() const;
    DrawResult getResult() const;
    std::string getErrorMessage() const;
    
    // 内部方法（由渲染器调用）
    void setProgress(int percent, const std::string& stage);
    void complete(DrawResult result, const std::string& message = "");
    void fail(const std::string& error);
    
private:
    std::atomic<TaskState> state_{TaskState::kPending};
    std::atomic<int> progress_{0};
    std::atomic<bool> cancelled_{false};
    std::string currentStage_;
    DrawResult result_ = DrawResult::kSuccess;
    std::string errorMessage_;
    
    ProgressCallback progressCallback_;
    CompleteCallback completeCallback_;
    mutable std::mutex mutex_;
};

/**
 * @brief 渲染队列
 */
class RenderQueue {
public:
    static RenderQueue& getInstance();
    
    // 提交任务
    void submit(std::shared_ptr<RenderTask> task,
                std::function<void()> work);
    
    // 批量提交
    void submitBatch(const std::vector<std::shared_ptr<RenderTask>>& tasks,
                     const std::vector<std::function<void()>>& works);
    
    // 取消所有任务
    void cancelAll();
    
    // 等待所有任务完成
    void waitForAll();
    
    // 状态
    size_t pendingCount() const;
    size_t runningCount() const;
    size_t completedCount() const;
    
    // 配置
    void setMaxConcurrent(size_t count);  // 最大并发数
    size_t getMaxConcurrent() const;
    
private:
    RenderQueue();
    ~RenderQueue();
    
    void workerThread();
    
    std::queue<std::pair<std::shared_ptr<RenderTask>, std::function<void()>>> queue_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_{true};
    size_t maxConcurrent_ = 4;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};
```

#### 4.5.2 异步渲染接口

```cpp
/**
 * @brief 绘制引擎扩展（异步接口）
 */
class DrawEngine {
public:
    // ... 现有同步接口 ...
    
    // ========== 异步渲染接口 ==========
    
    /**
     * @brief 异步绘制几何
     */
    virtual std::shared_ptr<RenderTask> drawGeometryAsync(
        const ogc::Geometry* geometry,
        const DrawStyle& style) = 0;
    
    /**
     * @brief 异步批量绘制要素
     */
    virtual std::shared_ptr<RenderTask> batchDrawFeaturesAsync(
        const std::vector<ogc::FeaturePtr>& features,
        SymbolizerPtr symbolizer) = 0;
    
    /**
     * @brief 异步渲染瓦片
     */
    virtual std::shared_ptr<RenderTask> renderTileAsync(
        const TileKey& key,
        const std::vector<ogc::FeaturePtr>& features,
        SymbolizerPtr symbolizer) = 0;
};

/**
 * @brief 使用示例
 */
void asyncRenderExample() {
    auto device = DrawFacade::getInstance().createImageDevice(1920, 1080);
    auto driver = DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(EngineType::kQtEngine, device, DrawParams{});
    
    // 准备数据
    std::vector<ogc::FeaturePtr> features = loadFeatures();
    auto symbolizer = std::make_shared<PointSymbolizer>();
    
    // 创建任务
    auto task = engine->batchDrawFeaturesAsync(features, symbolizer);
    
    // 设置进度回调
    task->setProgressCallback([](int percent, const std::string& stage) {
        std::cout << "Progress: " << percent << "% - " << stage << std::endl;
    });
    
    // 设置完成回调
    task->setCompleteCallback([&device](DrawResult result, const std::string& message) {
        if (result == DrawResult::kSuccess) {
            device->saveToFile("output.png");
            std::cout << "Render completed!" << std::endl;
        } else {
            std::cerr << "Render failed: " << message << std::endl;
        }
    });
    
    // 可以在渲染过程中做其他事情
    doOtherWork();
    
    // 等待完成
    while (!task->isCompleted() && !task->isCancelled()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

---

### 4.6 性能监控

#### 4.6.1 性能指标

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 性能指标
 */
struct PerformanceMetrics {
    // 渲染统计
    uint64_t totalDrawCalls = 0;
    uint64_t totalVertices = 0;
    uint64_t totalFeatures = 0;
    uint64_t totalGeometries = 0;
    
    // 时间统计（毫秒）
    double totalRenderTime = 0.0;
    double avgFrameTime = 0.0;
    double avgDrawTime = 0.0;
    double maxFrameTime = 0.0;
    double minFrameTime = DBL_MAX;
    
    // 内存统计（字节）
    size_t cpuMemoryUsed = 0;
    size_t gpuMemoryUsed = 0;
    size_t peakMemoryUsed = 0;
    
    // 缓存统计
    double cacheHitRate = 0.0;
    uint64_t cacheHits = 0;
    uint64_t cacheMisses = 0;
    size_t cacheSize = 0;
    
    // LOD统计
    uint64_t featuresSimplified = 0;
    uint64_t featuresFiltered = 0;
    
    // 帧统计
    uint64_t totalFrames = 0;
    uint64_t droppedFrames = 0;
    
    // 重置
    void reset();
    
    // 合并
    void merge(const PerformanceMetrics& other);
};

/**
 * @brief 性能监控器
 */
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    
    // 帧控制
    void beginFrame();
    void endFrame();
    
    // 记录
    void recordDrawCall(size_t vertices, size_t features);
    void recordGeometry(const ogc::Geometry* geometry);
    void recordCacheHit(bool hit);
    void recordMemoryUsage(size_t cpu, size_t gpu);
    void recordSimplification(bool simplified);
    void recordFiltering(bool filtered);
    
    // 获取指标
    PerformanceMetrics getMetrics() const;
    void resetMetrics();
    
    // 警告阈值
    void setWarningThreshold(double frameTimeMs);
    void setMemoryWarningThreshold(size_t bytes);
    
    // 回调
    using WarningCallback = std::function<void(const std::string& warning)>;
    void setWarningCallback(WarningCallback callback);
    
    // 启用/禁用
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    PerformanceMonitor();
    
    PerformanceMetrics metrics_;
    std::chrono::high_resolution_clock::time_point frameStartTime_;
    double warningThreshold_ = 33.33;  // ~30fps
    size_t memoryWarningThreshold_ = 1024 * 1024 * 1024;  // 1GB
    WarningCallback warningCallback_;
    bool enabled_ = true;
    mutable std::mutex mutex_;
};

/**
 * @brief 性能计时器（RAII）
 */
class ScopedTimer {
public:
    explicit ScopedTimer(const std::string& name);
    ~ScopedTimer();
    
    double elapsed() const;
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// 便捷宏
#define PERF_BEGIN_FRAME() PerformanceMonitor::getInstance().beginFrame()
#define PERF_END_FRAME() PerformanceMonitor::getInstance().endFrame()
#define PERF_RECORD_DRAW(v, f) PerformanceMonitor::getInstance().recordDrawCall(v, f)
#define PERF_SCOPED_TIMER(name) ScopedTimer timer_##__LINE__(name)

} // namespace draw
} // namespace ogc
```

---

### 4.7 GPU资源管理

#### 4.7.1 GPU资源抽象

```cpp
namespace ogc {
namespace draw {

/**
 * @brief GPU资源类型
 */
enum class GPUResourceType {
    kVertexBuffer,
    kIndexBuffer,
    kTexture,
    kShader,
    kFramebuffer,
    kVertexArray
};

/**
 * @brief GPU资源接口
 */
class GPUResource {
public:
    virtual ~GPUResource() = default;
    
    virtual GPUResourceType getType() const = 0;
    virtual size_t getGPUMemorySize() const = 0;
    virtual bool isValid() const = 0;
    
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    size_t getId() const { return id_; }
    
protected:
    size_t id_ = 0;
};

/**
 * @brief 顶点缓冲
 */
class VertexBuffer : public GPUResource {
public:
    GPUResourceType getType() const override { return GPUResourceType::kVertexBuffer; }
    
    virtual void setData(const float* vertices, size_t count) = 0;
    virtual void setSubData(const float* vertices, size_t offset, size_t count) = 0;
    virtual void setAttribute(int index, int size, int stride, int offset) = 0;
    
    size_t getVertexCount() const { return vertexCount_; }
    
protected:
    size_t vertexCount_ = 0;
};

/**
 * @brief 纹理缓冲
 */
class TextureBuffer : public GPUResource {
public:
    GPUResourceType getType() const override { return GPUResourceType::kTexture; }
    
    enum class FilterMode {
        kNearest,
        kLinear,
        kNearestMipmapNearest,
        kLinearMipmapLinear
    };
    
    enum class WrapMode {
        kRepeat,
        kMirroredRepeat,
        kClampToEdge,
        kClampToBorder
    };
    
    virtual void setImage(const unsigned char* data, 
                          int width, int height, int channels) = 0;
    virtual void setSubImage(const unsigned char* data,
                             int x, int y, int width, int height) = 0;
    virtual void setFilter(FilterMode minFilter, FilterMode magFilter) = 0;
    virtual void setWrap(WrapMode s, WrapMode t) = 0;
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
protected:
    int width_ = 0;
    int height_ = 0;
};

/**
 * @brief GPU资源管理器
 */
class GPUResourceManager {
public:
    static GPUResourceManager& getInstance();
    
    // 创建资源
    std::shared_ptr<VertexBuffer> createVertexBuffer();
    std::shared_ptr<TextureBuffer> createTexture();
    
    // 资源查询
    std::shared_ptr<GPUResource> getResource(size_t id) const;
    
    // 内存管理
    void garbageCollect();  // 清理未使用的资源
    void releaseUnused();   // 立即释放未使用的资源
    
    // 内存统计
    size_t getTotalGPUMemory() const;
    size_t getAvailableGPUMemory() const;
    size_t getResourceCount() const;
    
    // 内存限制
    void setMemoryLimit(size_t bytes);
    size_t getMemoryLimit() const;
    
private:
    GPUResourceManager();
    
    std::unordered_map<size_t, std::weak_ptr<GPUResource>> resources_;
    size_t nextId_ = 1;
    size_t memoryLimit_ = 1024 * 1024 * 1024;  // 1GB
    size_t currentMemory_ = 0;
    mutable std::mutex mutex_;
};

} // namespace draw
} // namespace ogc
```

---

### 4.8 日志系统

```cpp
namespace ogc {
namespace draw {

/**
 * @brief 日志级别
 */
enum class LogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarning,
    kError,
    kFatal
};

/**
 * @brief 日志记录
 */
struct LogRecord {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
};

/**
 * @brief 日志接口
 */
class Logger {
public:
    virtual ~Logger() = default;
    
    virtual void log(const LogRecord& record) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void flush() = 0;
};

/**
 * @brief 控制台日志器
 */
class ConsoleLogger : public Logger {
public:
    explicit ConsoleLogger(LogLevel level = LogLevel::kInfo);
    
    void log(const LogRecord& record) override;
    void setLevel(LogLevel level) override { level_ = level; }
    LogLevel getLevel() const override { return level_; }
    void flush() override;
    
    void setColored(bool colored) { colored_ = colored; }
    
private:
    LogLevel level_;
    bool colored_ = true;
};

/**
 * @brief 文件日志器
 */
class FileLogger : public Logger {
public:
    explicit FileLogger(const std::string& filePath,
                        LogLevel level = LogLevel::kInfo);
    ~FileLogger();
    
    void log(const LogRecord& record) override;
    void setLevel(LogLevel level) override { level_ = level; }
    LogLevel getLevel() const override { return level_; }
    void flush() override;
    
    void setMaxSize(size_t bytes);
    void setRotationCount(int count);
    
private:
    std::ofstream file_;
    std::string filePath_;
    LogLevel level_;
    size_t maxSize_ = 10 * 1024 * 1024;  // 10MB
    int rotationCount_ = 5;
    
    void rotateIfNeeded();
};

/**
 * @brief 日志管理器
 */
class LogManager {
public:
    static LogManager& getInstance();
    
    void setLogger(std::shared_ptr<Logger> logger);
    void addLogger(std::shared_ptr<Logger> logger);
    void removeLogger(std::shared_ptr<Logger> logger);
    void clearLoggers();
    
    void log(LogLevel level, 
             const char* file, 
             int line,
             const char* function,
             const char* format, 
             ...);
    
    void setGlobalLevel(LogLevel level);
    LogLevel getGlobalLevel() const;
    
private:
    LogManager();
    
    std::vector<std::shared_ptr<Logger>> loggers_;
    LogLevel globalLevel_ = LogLevel::kInfo;
    mutable std::mutex mutex_;
};

// 便捷宏
#define LOG_TRACE(...) LogManager::getInstance().log( \
    LogLevel::kTrace, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) LogManager::getInstance().log( \
    LogLevel::kDebug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)  LogManager::getInstance().log( \
    LogLevel::kInfo,  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)  LogManager::getInstance().log( \
    LogLevel::kWarning, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) LogManager::getInstance().log( \
    LogLevel::kError, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_FATAL(...) LogManager::getInstance().log( \
    LogLevel::kFatal, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

} // namespace draw
} // namespace ogc
```

---

## 5. 设备层设计

### 4.1 设备状态

```cpp
/**
 * @brief 设备状态枚举
 */
enum class DeviceState {
    kCreated,      // 已创建
    kInitialized,  // 已初始化
    kActive,       // 活动中（正在绘制）
    kPaused,       // 已暂停
    kStopped,      // 已停止
    kError         // 错误状态
};
```

### 4.2 设备基类

```cpp
/**
 * @brief 绘制设备抽象基类
 * 
 * 线程安全保证：
 * - 所有const方法都是线程安全的
 * - 修改方法需要外部同步
 */
class DrawDevice {
public:
    virtual ~DrawDevice() = default;
    
    // 禁止拷贝，支持移动
    DrawDevice(const DrawDevice&) = delete;
    DrawDevice& operator=(const DrawDevice&) = delete;
    DrawDevice(DrawDevice&&) = default;
    DrawDevice& operator=(DrawDevice&&) = default;
    
    // ========== 基本信息 ==========
    
    /**
     * @brief 获取设备类型
     */
    DeviceType getType() const noexcept {
        return type_;
    }
    
    /**
     * @brief 获取设备ID
     */
    const std::string& getDeviceId() const noexcept {
        return deviceId_;
    }
    
    /**
     * @brief 获取设备状态
     */
    DeviceState getState() const noexcept {
        return state_.load(std::memory_order_relaxed);
    }
    
    /**
     * @brief 是否已初始化
     */
    bool isInitialized() const noexcept {
        return getState() >= DeviceState::Initialized;
    }
    
    /**
     * @brief 是否活动
     */
    bool isActive() const noexcept {
        return getState() == DeviceState::Active;
    }
    
    // ========== 设备信息 ==========
    
    /**
     * @brief 获取物理宽度（像素）
     */
    virtual int getWidth() const = 0;
    
    /**
     * @brief 获取物理高度（像素）
     */
    virtual int getHeight() const = 0;
    
    /**
     * @brief 获取当前DPI
     */
    virtual double getDpi() const = 0;
    
    /**
     * @brief 获取设备参数
     */
    const DrawParams& getDrawParams() const noexcept {
        return params_;
    }
    
    // ========== 生命周期 ==========
    
    /**
     * @brief 初始化设备
     * @param params 绘制参数
     * @return 操作结果
     */
    virtual DrawResult initialize(const DrawParams& params) = 0;
    
    /**
     * @brief 开始绘制
     * @return 操作结果
     */
    virtual DrawResult beginDraw() = 0;
    
    /**
     * @brief 结束绘制
     * @return 操作结果
     */
    virtual DrawResult endDraw() = 0;
    
    /**
     * @brief 清空设备
     * @return 操作结果
     */
    virtual DrawResult clear() = 0;
    
    /**
     * @brief 保存到文件
     * @param filePath 文件路径
     * @return 操作结果
     */
    virtual DrawResult saveToFile(const std::string& filePath) = 0;
    
    /**
     * @brief 保存到文件（指定格式）
     */
    virtual DrawResult saveToFile(const std::string& filePath, 
                                   ImageFormat format) = 0;
    
    // ========== 平台接口 ==========
    
    /**
     * @brief 获取原生句柄
     * @return 平台相关的句柄
     */
    virtual void* getNativeHandle() { return nullptr; }
    
    /**
     * @brief 获取原生设备上下文
     */
    virtual void* getNativeContext() { return nullptr; }
    
    // ========== 线程安全 ==========
    
    /**
     * @brief 获取设备锁
     * @return RAII锁对象
     */
    std::unique_lock<std::mutex> getLock() {
        return std::unique_lock<std::mutex>(mutex_);
    }
    
    /**
     * @brief 尝试获取设备锁
     */
    std::unique_lock<std::mutex> tryGetLock() {
        return std::unique_lock<std::mutex>(mutex_, std::try_to_lock);
    }
    
    // ========== 引擎关联 ==========
    
    /**
     * @brief 获取关联引擎
     */
    std::weak_ptr<DrawEngine> getEngine() const noexcept {
        return weakEngine_;
    }
    
    /**
     * @brief 设置关联引擎
     */
    void setEngine(DrawEnginePtr engine);
    
protected:
    DrawDevice(DeviceType type) : type_(type) {}
    
    void setState(DeviceState state) {
        state_.store(state, std::memory_order_relaxed);
    }
    
    void setDrawParams(const DrawParams& params) {
        params_ = params;
    }
    
    void invalidateDeviceId() {
        deviceId_ = generateDeviceId();
    }
    
private:
    DeviceType type_ = DeviceType::Unknown;
    std::string deviceId_;
    std::atomic<DeviceState> state_{DeviceState::Created};
    DrawParams params_;
    std::weak_ptr<DrawEngine> weakEngine_;
    mutable std::mutex mutex_;
    
    static std::string generateDeviceId();
};
```

### 4.3 栅格图像设备

```cpp
/**
 * @brief 栅格图像设备
 */
class RasterImageDevice : public DrawDevice {
public:
    /**
     * @brief 创建栅格图像设备
     */
    static DrawDevicePtr create(int width, int height, 
                                double dpi = 96.0,
                                const std::string& backgroundColor = "#FFFFFF");
    
    // 实现基类接口
    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }
    double getDpi() const override { return dpi_; }
    
    DrawResult initialize(const DrawParams& params) override;
    DrawResult beginDraw() override;
    DrawResult endDraw() override;
    DrawResult clear() override;
    DrawResult saveToFile(const std::string& filePath) override;
    DrawResult saveToFile(const std::string& filePath, ImageFormat format) override;
    
    // 特有接口
    /**
     * @brief 获取图像数据
     */
    const unsigned char* getImageData() const;
    
    /**
     * @brief 获取图像数据大小
     */
    size_t getImageDataSize() const;
    
    /**
     * @brief 获取像素（RGBA）
     */
    bool getPixel(int x, int y, unsigned char& r, 
                  unsigned char& g, unsigned char& b, 
                  unsigned char& a) const;
    
    /**
     * @brief 设置像素（RGBA）
     */
    bool setPixel(int x, int y, unsigned char r, 
                  unsigned char g, unsigned char b, 
                  unsigned char a);
    
private:
    explicit RasterImageDevice(int width, int height, double dpi);
    
    int width_ = 0;
    int height_ = 0;
    double dpi_ = 96.0;
    std::string backgroundColor_;
    
    // 图像数据（RGBA格式）
    std::vector<unsigned char> imageData_;
};
```

### 4.4 PDF设备

```cpp
/**
 * @brief PDF设备
 */
class PdfDevice : public DrawDevice {
public:
    static DrawDevicePtr create(const std::string& filePath,
                                int width, int height,
                                double dpi = 72.0);
    
    // 实现基类接口
    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }
    double getDpi() const override { return dpi_; }
    
    DrawResult initialize(const DrawParams& params) override;
    DrawResult beginDraw() override;
    DrawResult endDraw() override;
    DrawResult clear() override;
    DrawResult saveToFile(const std::string& filePath) override;
    
    // PDF特有接口
    /**
     * @brief 添加新页面
     */
    DrawResult addPage(int width, int height);
    
    /**
     * @brief 设置页面索引
     */
    bool setPageIndex(size_t index);
    
    /**
     * @brief 获取页数
     */
    size_t getPageCount() const;
    
    /**
     * @brief 设置PDF元数据
     */
    void setMetadata(const std::string& title,
                    const std::string& author,
                    const std::string& subject);
    
private:
    explicit PdfDevice(const std::string& filePath, int width, int height, double dpi);
    
    std::string filePath_;
    int width_ = 0;
    int height_ = 0;
    double dpi_ = 72.0;
    size_t currentPage_ = 0;
};
```

---

## 5. 引擎层设计

### 5.1 引擎基类

```cpp
/**
 * @brief 绘制引擎抽象基类
 */
class DrawEngine {
public:
    virtual ~DrawEngine() = default;
    
    // 禁止拷贝，支持移动
    DrawEngine(const DrawEngine&) = delete;
    DrawEngine& operator=(const DrawEngine&) = delete;
    DrawEngine(DrawEngine&&) = default;
    DrawEngine& operator=(DrawEngine&&) = default;
    
    // ========== 基本信息 ==========
    
    EngineType getType() const noexcept {
        return type_;
    }
    
    bool isInitialized() const noexcept {
        return initialized_.load(std::memory_order_relaxed);
    }
    
    // ========== 设备关联 ==========
    
    std::weak_ptr<DrawDevice> getDevice() const noexcept {
        return weakDevice_;
    }
    
    // ========== 生命周期 ==========
    
    virtual DrawResult initialize(DrawDevicePtr device, 
                                   const DrawParams& params) = 0;
    virtual DrawResult uninitialize() = 0;
    
    // ========== 基本图元绘制 ==========
    
    /**
     * @brief 绘制点
     */
    virtual DrawResult drawPoint(double x, double y, 
                                  const DrawStyle& style) = 0;
    
    /**
     * @brief 绘制线段
     */
    virtual DrawResult drawLine(double x1, double y1, 
                                 double x2, double y2,
                                 const DrawStyle& style) = 0;
    
    /**
     * @brief 绘制折线
     */
    virtual DrawResult drawPolyline(const std::vector<double>& coords,
                                     const DrawStyle& style) = 0;
    
    /**
     * @brief 绘制多边形
     */
    virtual DrawResult drawPolygon(const std::vector<double>& coords,
                                    const DrawStyle& style) = 0;
    
    /**
     * @brief 绘制矩形
     */
    virtual DrawResult drawRect(double x, double y, 
                                 double width, double height,
                                 const DrawStyle& style);
    
    /**
     * @brief 绘制圆
     */
    virtual DrawResult drawCircle(double x, double y, double radius,
                                   const DrawStyle& style);
    
    /**
     * @brief 绘制椭圆
     */
    virtual DrawResult drawEllipse(double x, double y,
                                    double width, double height,
                                    const DrawStyle& style);
    
    // ========== 几何对象绘制 ==========
    
    /**
     * @brief 绘制几何对象
     */
    virtual DrawResult drawGeometry(const ogc::Geometry* geometry,
                                     const DrawStyle& style) = 0;
    
    /**
     * @brief 绘制要素
     */
    virtual DrawResult drawFeature(const ogc::Feature* feature,
                                    SymbolizerPtr symbolizer = nullptr);
    
    // ========== 文本绘制 ==========
    
    /**
     * @brief 绘制文本
     */
    virtual DrawResult drawText(double x, double y,
                                 const std::string& text,
                                 const std::string& fontFamily,
                                 double fontSize,
                                 const std::string& color) = 0;
    
    /**
     * @brief 绘制文本（带样式）
     */
    virtual DrawResult drawText(double x, double y,
                                 const std::string& text,
                                 const TextStyle& style);
    
    // ========== 图像绘制 ==========
    
    /**
     * @brief 绘制图像
     */
    virtual DrawResult drawImage(double x, double y,
                                  const std::string& imagePath,
                                  double width = 0, double height = 0);
    
    virtual DrawResult drawImage(double x, double y,
                                  const unsigned char* data,
                                  size_t dataSize,
                                  int width, int height);
    
    // ========== 样式管理 ==========
    
    virtual DrawResult setStyle(const DrawStyle& style) = 0;
    virtual DrawResult saveState() = 0;
    virtual DrawResult restoreState() = 0;
    
    // ========== 坐标变换 ==========
    
    virtual void worldToDevice(double& x, double& y) const = 0;
    virtual void deviceToWorld(double& x, double& y) const = 0;
    
    virtual void setTransform(const TransformMatrix& matrix) = 0;
    virtual TransformMatrix getTransform() const = 0;
    virtual void multiplyTransform(const TransformMatrix& matrix) = 0;
    
    virtual void pushTransform() = 0;
    virtual void popTransform() = 0;
    
    // 便捷变换方法
    void translate(double dx, double dy);
    void scale(double sx, double sy);
    void rotate(double angle);
    
    // ========== 裁剪 ==========
    
    virtual void setClipRect(double minX, double minY, 
                             double maxX, double maxY) = 0;
    virtual void setClipGeometry(const ogc::Geometry* geometry) = 0;
    virtual void clearClip() = 0;
    
    // ========== 图层 ==========
    
    virtual int createLayer(const std::string& name = "") = 0;
    virtual void setActiveLayer(int layerId) = 0;
    virtual void mergeLayer(int layerId) = 0;
    virtual void setLayerOpacity(int layerId, double opacity) = 0;
    virtual void setLayerVisible(int layerId, bool visible) = 0;
    
protected:
    DrawEngine(EngineType type) : type_(type) {}
    
    void setInitialized(bool init) {
        initialized_.store(init, std::memory_order_relaxed);
    }
    
    void setDevice(DrawDevicePtr device) {
        weakDevice_ = device;
    }
    
    // 坐标变换辅助方法
    void transformCoordinates(std::vector<double>& coords) const;
    
private:
    EngineType type_ = EngineType::Unknown;
    std::atomic<bool> initialized_{false};
    std::weak_ptr<DrawDevice> weakDevice_;
    DrawParams params_;
    
    friend class DrawDriver;
    friend class DrawContext;
};
```

### 5.2 变换矩阵

```cpp
/**
 * @brief 2D变换矩阵
 */
struct TransformMatrix {
    double m[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    
    // 单位矩阵
    static TransformMatrix identity() {
        return TransformMatrix();
    }
    
    // 平移矩阵
    static TransformMatrix translation(double dx, double dy) {
        TransformMatrix mat;
        mat.m[0][2] = dx;
        mat.m[1][2] = dy;
        return mat;
    }
    
    // 缩放矩阵
    static TransformMatrix scaling(double sx, double sy) {
        TransformMatrix mat;
        mat.m[0][0] = sx;
        mat.m[1][1] = sy;
        return mat;
    }
    
    // 旋转矩阵
    static TransformMatrix rotation(double angle) {
        TransformMatrix mat;
        double c = std::cos(angle);
        double s = std::sin(angle);
        mat.m[0][0] = c;
        mat.m[0][1] = -s;
        mat.m[1][0] = s;
        mat.m[1][1] = c;
        return mat;
    }
    
    // 矩阵乘法
    TransformMatrix operator*(const TransformMatrix& other) const {
        TransformMatrix result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    // 变换点
    void transform(double& x, double& y) const {
        double newX = m[0][0] * x + m[0][1] * y + m[0][2];
        double newY = m[1][0] * x + m[1][1] * y + m[1][2];
        x = newX;
        y = newY;
    }
    
    // 求逆矩阵
    TransformMatrix inverse() const {
        TransformMatrix result;
        double det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
                   - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                   + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
        
        if (std::abs(det) < 1e-10) {
            return identity();
        }
        
        double invDet = 1.0 / det;
        result.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
        result.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
        result.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
        result.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
        result.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
        result.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;
        
        return result;
    }
    
    // 判断是否为单位矩阵
    bool isIdentity(double tolerance = 1e-10) const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                double expected = (i == j) ? 1.0 : 0.0;
                if (std::abs(m[i][j] - expected) > tolerance) {
                    return false;
                }
            }
        }
        return true;
    }
};
```

---

## 6. 驱动层设计

### 6.1 驱动基类

```cpp
/**
 * @brief 绘制驱动抽象基类
 */
class DrawDriver {
public:
    virtual ~DrawDriver() = default;
    
    // 禁止拷贝，支持移动
    DrawDriver(const DrawDriver&) = delete;
    DrawDriver& operator=(const DrawDriver&) = delete;
    DrawDriver(DrawDriver&&) = default;
    DrawDriver& operator=(DrawDriver&&) = default;
    
    // ========== 驱动信息 ==========
    
    const std::string& getName() const noexcept {
        return name_;
    }
    
    const std::string& getVersion() const noexcept {
        return version_;
    }
    
    bool isAvailable() const noexcept {
        return available_.load(std::memory_order_relaxed);
    }
    
    bool isInitialized() const noexcept {
        return initialized_.load(std::memory_order_relaxed);
    }
    
    // ========== 能力查询 ==========
    
    virtual bool supportsDevice(DeviceType type) const = 0;
    virtual bool supportsEngine(EngineType type) const = 0;
    
    /**
     * @brief 获取支持的设备类型列表
     */
    virtual std::vector<DeviceType> getSupportedDevices() const = 0;
    
    /**
     * @brief 获取支持的引擎类型列表
     */
    virtual std::vector<EngineType> getSupportedEngines() const = 0;
    
    // ========== 工厂方法 ==========
    
    /**
     * @brief 创建设备
     */
    virtual DrawDevicePtr createDevice(DeviceType type,
                                        const DrawParams& params) = 0;
    
    /**
     * @brief 创建引擎
     */
    virtual DrawEnginePtr createEngine(EngineType type,
                                        DrawDevicePtr device,
                                        const DrawParams& params) = 0;
    
    // ========== 生命周期 ==========
    
    virtual DrawResult initialize() = 0;
    virtual DrawResult shutdown() = 0;
    
    // ========== 要素绘制 ==========
    
    /**
     * @brief 绘制要素集合
     */
    virtual DrawResult drawFeatures(DrawDevicePtr device,
                                     DrawEnginePtr engine,
                                     const std::vector<ogc::FeaturePtr>& features,
                                     SymbolizerPtr symbolizer = nullptr,
                                     const DrawParams& params = DrawParams()) = 0;
    
    /**
     * @brief 绘制单个要素
     */
    virtual DrawResult drawSingleFeature(DrawDevicePtr device,
                                          DrawEnginePtr engine,
                                          const ogc::Feature* feature,
                                          SymbolizerPtr symbolizer = nullptr) = 0;
    
    /**
     * @brief 批量绘制要素（性能优化）
     */
    virtual DrawResult batchDrawFeatures(DrawDevicePtr device,
                                          DrawEnginePtr engine,
                                          const std::vector<const ogc::Feature*>& features,
                                          SymbolizerPtr symbolizer = nullptr) = 0;
    
    // ========== 默认样式 ==========
    
    /**
     * @brief 获取几何对象的默认样式
     */
    virtual const DrawStyle& getDefaultStyle(const ogc::Geometry* geometry) const;
    
    void setDefaultPointStyle(const DrawStyle& style);
    void setDefaultLineStyle(const DrawStyle& style);
    void setDefaultPolygonStyle(const DrawStyle& style);
    
protected:
    DrawDriver(const std::string& name) : name_(name) {}
    
    void setAvailable(bool avail) {
        available_.store(avail, std::memory_order_relaxed);
    }
    
    void setInitialized(bool init) {
        initialized_.store(init, std::memory_order_relaxed);
    }
    
private:
    std::string name_;
    std::string version_ = "1.0.0";
    std::atomic<bool> available_{false};
    std::atomic<bool> initialized_{false};
    
    // 默认样式
    DrawStyle defaultPointStyle_{"#FF0000", "#FF0000", 2.0};
    DrawStyle defaultLineStyle_{"#0000FF", "#0000FF", 1.5};
    DrawStyle defaultPolygonStyle_{"#000000", "#00FF00", 1.0};
};
```

### 6.2 驱动管理器

```cpp
/**
 * @brief 驱动管理器（单例）
 */
class DriverManager {
public:
    /**
     * @brief 获取单例实例
     */
    static DriverManager& GetInstance();
    
    // ========== 驱动注册 ==========
    
    /**
     * @brief 注册驱动
     */
    bool RegisterDriver(const std::string& name, DrawDriverPtr driver);
    
    /**
     * @brief 注销驱动
     */
    bool UnregisterDriver(const std::string& name);
    
    /**
     * @brief 获取驱动
     */
    DrawDriverPtr GetDriver(const std::string& name) const;
    
    /**
     * @brief 查找合适的驱动
     */
    DrawDriverPtr FindSuitableDriver(DeviceType deviceType,
                                      EngineType engineType) const;
    
    /**
     * @brief 获取所有支持的驱动
     */
    std::vector<DrawDriverPtr> GetSupportedDrivers(DeviceType deviceType,
                                                    EngineType engineType) const;
    
    /**
     * @brief 获取所有驱动名称
     */
    std::vector<std::string> GetAllDriverNames() const;
    
    /**
     * @brief 获取驱动数量
     */
    size_t GetDriverCount() const;
    
    // ========== 生命周期管理 ==========
    
    /**
     * @brief 初始化所有驱动
     */
    DrawResult InitializeAll();
    
    /**
     * @brief 关闭所有驱动
     */
    DrawResult ShutdownAll();
    
    /**
     * @brief 检查驱动是否可用
     */
    bool IsDriverAvailable(const std::string& name) const;
    
private:
    DriverManager() = default;
    ~DriverManager() = default;
    DriverManager(const DriverManager&) = delete;
    DriverManager& operator=(const DriverManager&) = delete;
    
    std::map<std::string, DrawDriverPtr> drivers_;
    mutable std::mutex mutex_;  // C++11 使用互斥锁（无读写锁）
};
```

---

## 7. 绘制上下文

### 7.1 上下文类

```cpp
/**
 * @brief 绘制上下文
 * 
 * 管理一次绘制操作的所有状态，提供统一的绘制接口
 */
class DrawContext {
public:
    /**
     * @brief 构造上下文
     */
    explicit DrawContext(DrawDevicePtr device, DrawEnginePtr engine);
    
    /**
     * @brief 析构时自动结束绘制
     */
    ~DrawContext();
    
    // 禁止拷贝
    DrawContext(const DrawContext&) = delete;
    DrawContext& operator=(const DrawContext&) = delete;
    
    // ========== 上下文管理 ==========
    
    /**
     * @brief 开始绘制
     */
    DrawResult begin();
    
    /**
     * @brief 结束绘制
     */
    DrawResult end();
    
    /**
     * @brief 是否活动
     */
    bool isActive() const noexcept;
    
    // ========== 设备和引擎访问 ==========
    
    DrawDevice* getDevice() const noexcept { return device_.get(); }
    DrawEngine* getEngine() const noexcept { return engine_.get(); }
    
    // ========== 样式管理 ==========
    
    void setStyle(const DrawStyle& style);
    const DrawStyle& getStyle() const noexcept;
    
    void pushStyle();
    void popStyle();
    
    // ========== 变换管理 ==========
    
    void setTransform(const TransformMatrix& matrix);
    TransformMatrix getTransform() const;
    
    void multiplyTransform(const TransformMatrix& matrix);
    void pushTransform();
    void popTransform();
    
    void translate(double dx, double dy);
    void scale(double sx, double sy);
    void rotate(double angle);
    
    // ========== 裁剪 ==========
    
    void setClipRect(double minX, double minY, double maxX, double maxY);
    void setClipGeometry(const ogc::Geometry* geometry);
    void clearClip();
    
    // ========== 图层 ==========
    
    int createLayer(const std::string& name = "");
    void setActiveLayer(int layerId);
    void mergeLayer(int layerId);
    void setLayerOpacity(int layerId, double opacity);
    void setLayerVisible(int layerId, bool visible);
    
    // ========== 绘制操作 ==========
    
    DrawResult drawGeometry(const ogc::Geometry* geometry);
    DrawResult drawGeometry(const ogc::Geometry* geometry, const DrawStyle& style);
    DrawResult drawFeature(const ogc::Feature* feature);
    DrawResult drawFeature(const ogc::Feature* feature, SymbolizerPtr symbolizer);
    
    DrawResult drawPoint(double x, double y);
    DrawResult drawLine(double x1, double y1, double x2, double y2);
    DrawResult drawPolyline(const std::vector<double>& coords);
    DrawResult drawPolygon(const std::vector<double>& coords);
    
    DrawResult drawText(double x, double y, const std::string& text);
    DrawResult drawText(double x, double y, const std::string& text, 
                        const TextStyle& style);
    
    // ========== 坐标转换 ==========
    
    void worldToScreen(double& x, double& y) const;
    void screenToWorld(double& x, double& y) const;
    
    // ========== 批量操作 ==========
    
    DrawResult drawFeatures(const std::vector<ogc::FeaturePtr>& features,
                            SymbolizerPtr symbolizer = nullptr);
    
private:
    DrawDevicePtr device_;
    DrawEnginePtr engine_;
    bool active_ = false;
    
    // 状态栈
    std::stack<DrawStyle> styleStack_;
    std::stack<TransformMatrix> transformStack_;
    
    // 错误处理
    DrawResult lastError_ = DrawResult::Success;
    std::string lastErrorMessage_;
};
```

---

## 8. 门面类

### 8.1 DrawFacade

```cpp
/**
 * @brief 绘制门面类
 * 
 * 提供简化的高层绘制接口，封装设备和引擎的创建和管理
 */
class DrawFacade {
public:
    /**
     * @brief 获取单例实例
     */
    static DrawFacade& getInstance();
    
    // ========== 便捷创建方法 ==========
    
    /**
     * @brief 创建图像设备
     */
    DrawDevicePtr createImageDevice(int width, int height, 
                                     double dpi = 96.0,
                                     const std::string& backgroundColor = "#FFFFFF");
    
    /**
     * @brief 创建显示设备
     */
    DrawDevicePtr createDisplayDevice(void* nativeWindow,
                                       int width, int height);
    
    /**
     * @brief 创建PDF设备
     */
    DrawDevicePtr createPdfDevice(const std::string& filePath,
                                   int width, int height,
                                   double dpi = 72.0);
    
    /**
     * @brief 创建SVG设备
     */
    DrawDevicePtr createSvgDevice(const std::string& filePath,
                                   int width, int height);
    
    // ========== 快速绘制方法 ==========
    
    /**
     * @brief 绘制到图像文件
     */
    DrawResult drawToImage(const std::string& outputPath,
                           const std::vector<ogc::FeaturePtr>& features,
                           int width = 800,
                           int height = 600,
                           const DrawParams& params = DrawParams());
    
    /**
     * @brief 绘制到显示窗口
     */
    DrawResult drawToDisplay(void* nativeWindow,
                             const std::vector<ogc::FeaturePtr>& features,
                             const DrawParams& params = DrawParams());
    
    /**
     * @brief 绘制到PDF
     */
    DrawResult drawToPdf(const std::string& outputPath,
                         const std::vector<ogc::FeaturePtr>& features,
                         int width = 800,
                         int height = 600,
                         const DrawParams& params = DrawParams());
    
    // ========== 批量导出 ==========
    
    /**
     * @brief 导出多种格式
     */
    DrawResult exportToFormats(const std::vector<ogc::FeaturePtr>& features,
                               const std::vector<std::string>& formats,
                               const std::string& baseName,
                               int width = 800,
                               int height = 600);
    
    /**
     * @brief 导出为图像序列
     */
    DrawResult exportImageSequence(const std::vector<ogc::FeaturePtr>& features,
                                   const std::string& outputDir,
                                   int startFrame, int endFrame,
                                   int width = 800, int height = 600);
    
    // ========== 驱动和引擎管理 ==========
    
    DrawDriverPtr getDriver(const std::string& name);
    DrawDriverPtr getSuitableDriver(DeviceType deviceType, EngineType engineType);
    
    std::vector<std::string> getAvailableDrivers() const;
    std::vector<EngineType> getAvailableEngines() const;
    
    // ========== 全局设置 ==========
    
    void setDefaultEngine(EngineType engineType);
    void setDefaultDriver(const std::string& driverName);
    
    EngineType getDefaultEngine() const noexcept { return defaultEngine_; }
    const std::string& getDefaultDriver() const noexcept { return defaultDriver_; }
    
    // ========== 配置管理 ==========
    
    void setConfig(const DrawConfig& config);
    const DrawConfig& getConfig() const noexcept { return config_; }
    
    void loadConfig(const std::string& filePath);
    void saveConfig(const std::string& filePath);
    
private:
    DrawFacade() = default;
    
    EngineType defaultEngine_ = EngineType::QtEngine;
    std::string defaultDriver_ = "QtDriver";
    DrawConfig config_;
};
```

---

## 9. 符号化系统

### 9.1 符号化器基类

```cpp
/**
 * @brief 符号化器抽象基类
 */
class Symbolizer {
public:
    virtual ~Symbolizer() = default;
    
    /**
     * @brief 应用符号到要素
     */
    virtual void applySymbol(ogc::Feature* feature) = 0;
    
    /**
     * @brief 设置属性
     */
    virtual void setProperty(const std::string& key, const std::string& value) = 0;
    
    /**
     * @brief 获取属性
     */
    virtual std::string getProperty(const std::string& key) const = 0;
    
    /**
     * @brief 检查属性是否存在
     */
    virtual bool hasProperty(const std::string& key) const = 0;
};

using SymbolizerPtr = std::shared_ptr<Symbolizer>;
```

### 9.2 点符号化器

```cpp
/**
 * @brief 点符号化器
 */
class PointSymbolizer : public Symbolizer {
public:
    enum class SymbolType {
        Circle,
        Square,
        Triangle,
        Star,
        Diamond,
        Cross,
        Image
    };
    
    void setSymbolType(SymbolType type);
    SymbolType getSymbolType() const noexcept { return symbolType_; }
    
    void setSize(double size);
    double getSize() const noexcept { return size_; }
    
    void setColor(const std::string& color);
    const std::string& getColor() const noexcept { return color_; }
    
    void setOutlineColor(const std::string& color);
    void setOutlineWidth(double width);
    
    void setImagePath(const std::string& path);
    
    void applySymbol(ogc::Feature* feature) override;
    
    // 属性设置
    void setProperty(const std::string& key, const std::string& value) override;
    std::string getProperty(const std::string& key) const override;
    bool hasProperty(const std::string& key) const override;
    
private:
    SymbolType symbolType_ = SymbolType::Circle;
    double size_ = 6.0;
    std::string color_ = "#FF0000";
    std::string outlineColor_ = "#000000";
    double outlineWidth_ = 1.0;
    std::string imagePath_;
};
```

### 9.3 线符号化器

```cpp
/**
 * @brief 线符号化器
 */
class LineSymbolizer : public Symbolizer {
public:
    void setColor(const std::string& color);
    void setWidth(double width);
    void setDashPattern(const std::vector<double>& pattern);
    void setCapStyle(LineCapStyle cap);
    void setJoinStyle(LineJoinStyle join);
    
    void applySymbol(ogc::Feature* feature) override;
    
    // 属性设置
    void setProperty(const std::string& key, const std::string& value) override;
    std::string getProperty(const std::string& key) const override;
    bool hasProperty(const std::string& key) const override;
    
private:
    std::string color_ = "#0000FF";
    double width_ = 1.0;
    std::vector<double> dashPattern_;
    LineCapStyle capStyle_ = LineCapStyle::Round;
    LineJoinStyle joinStyle_ = LineJoinStyle::Round;
};
```

### 9.4 面符号化器

```cpp
/**
 * @brief 面符号化器
 */
class PolygonSymbolizer : public Symbolizer {
public:
    void setFillColor(const std::string& color);
    void setFillPattern(FillPattern pattern);
    void setStrokeColor(const std::string& color);
    void setStrokeWidth(double width);
    void setOpacity(double opacity);
    
    void applySymbol(ogc::Feature* feature) override;
    
    // 属性设置
    void setProperty(const std::string& key, const std::string& value) override;
    std::string getProperty(const std::string& key) const override;
    bool hasProperty(const std::string& key) const override;
    
private:
    std::string fillColor_ = "#808080";
    FillPattern fillPattern_ = FillPattern::Solid;
    std::string strokeColor_ = "#000000";
    double strokeWidth_ = 1.0;
    double opacity_ = 1.0;
};
```

---

## 10. 错误处理机制

### 10.1 错误信息结构

```cpp
/**
 * @brief 绘制错误信息
 */
struct DrawError {
    DrawResult code;
    std::string message;
    std::string context;      // 错误上下文
    std::string file;         // 源文件
    int line;                 // 行号
    std::exception_ptr innerException;  // 内部异常
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "DrawError[" << getResultString(code) << "]: " << message;
        if (!context.empty()) {
            oss << "\nContext: " << context;
        }
        if (!file.empty()) {
            oss << "\nLocation: " << file << ":" << line;
        }
        return oss.str();
    }
};
```

### 10.2 错误处理策略

```cpp
/**
 * @brief 错误处理策略
 */
enum class ErrorHandlingPolicy {
    ReturnCode,    // 返回错误码（默认）
    Throw          // 抛出异常
};

/**
 * @brief 全局错误处理配置
 */
class ErrorHandling {
public:
    static void setPolicy(ErrorHandlingPolicy policy);
    static ErrorHandlingPolicy getPolicy();
    
    static void setLastError(const DrawError& error);
    static DrawError getLastError();
    static void clearLastError();
    
private:
    static ErrorHandlingPolicy policy_;
    static thread_local DrawError lastError_;
};
```

### 10.3 绘制异常

```cpp
/**
 * @brief 绘制异常
 */
class DrawException : public std::runtime_error {
public:
    explicit DrawException(const DrawError& error)
        : std::runtime_error(error.toString())
        , error_(error) {}
    
    DrawResult getErrorCode() const noexcept {
        return error_.code;
    }
    
    const DrawError& getError() const noexcept {
        return error_;
    }
    
private:
    DrawError error_;
};
```

### 10.4 错误处理宏

```cpp
// 错误检查宏
#define DRAW_CHECK(result) \
    do { \
        if (result != ogc::draw::DrawResult::Success) { \
            if (ogc::draw::ErrorHandling::getPolicy() == ogc::draw::ErrorHandlingPolicy::Throw) { \
                throw ogc::draw::DrawException({result, "", "", __FILE__, __LINE__}); \
            } \
        } \
    } while(0)

// 错误设置宏
#define DRAW_SET_ERROR(code, msg) \
    ogc::draw::ErrorHandling::setLastError({code, msg, "", __FILE__, __LINE__})

// 条件检查宏
#define DRAW_CHECK_CONDITION(condition, code, msg) \
    if (!(condition)) { \
        DRAW_SET_ERROR(code, msg); \
        return code; \
    }
```

---

## 11. 线程安全设计

### 11.1 线程安全保证

```cpp
/**
 * @brief 线程安全保证说明
 * 
 * 1. 只读操作：线程安全
 *    - 所有 const 方法都是线程安全的
 *    - 例如：getWidth(), getHeight(), getDpi()
 * 
 * 2. 修改操作：需要外部同步
 *    - 修改设备状态的方法需要外部加锁
 *    - 使用 getLock() 获取 RAII 锁
 * 
 * 3. 绘制操作：不支持并发
 *    - 单个设备不支持并发绘制
 *    - 建议为每个线程创建独立设备
 * 
 * 4. 驱动管理器：线程安全
 *    - 使用读写锁保护
 *    - 支持并发读取，串行写入
 */
```

### 11.2 并发使用模式

```cpp
/**
 * @brief 并发绘制示例
 */
void concurrentDrawExample() {
    // 模式1：每个线程独立设备（推荐）
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i]() {
            // 每个线程创建自己的设备和引擎
            auto device = DrawFacade::getInstance().createImageDevice(800, 600);
            auto driver = DriverManager::getInstance().getDriver("QtDriver");
            auto engine = driver->createEngine(EngineType::QtEngine, device, DrawParams{});
            
            device->initialize(DrawParams{});
            engine->initialize(device, DrawParams{});
            
            // 绘制
            device->beginDraw();
            engine->drawGeometry(/*...*/);
            device->endDraw();
            
            device->saveToFile("output_" + std::to_string(i) + ".png");
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 模式2：共享设备，串行绘制（性能较低）
    auto device = DrawFacade::getInstance().createImageDevice(800, 600);
    auto engine = driver->createEngine(EngineType::QtEngine, device, DrawParams{});
    
    std::mutex drawMutex;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() {
            std::lock_guard<std::mutex> lock(drawMutex);
            device->beginDraw();
            engine->drawGeometry(/*...*/);
            device->endDraw();
            device->saveToFile("output_" + std::to_string(i) + ".png");
        });
    }
}
```

### 11.3 资源池

```cpp
/**
 * @brief 设备资源池（可选优化）
 */
class DevicePool {
public:
    static DevicePool& getInstance();
    
    /**
     * @brief 获取设备（复用或创建）
     */
    DrawDevicePtr acquireDevice(DeviceType type, 
                                 const DrawParams& params);
    
    /**
     * @brief 释放设备（回收到池）
     */
    void releaseDevice(DrawDevicePtr device);
    
    /**
     * @brief 设置最大设备数量
     */
    void setMaxDevices(size_t count);
    
    /**
     * @brief 清空未使用的设备
     */
    void clearUnused();
    
    /**
     * @brief 获取统计信息
     */
    size_t getActiveCount() const;
    size_t getIdleCount() const;
    
private:
    DevicePool() = default;
    
    std::map<DeviceType, std::vector<DrawDevicePtr>> idleDevices_;
    std::vector<DrawDevicePtr> activeDevices_;
    size_t maxDevices_ = 10;
    mutable std::mutex mutex_;
};
```

---

## 12. 性能优化

### 12.1 性能基准

| 操作 | 典型性能 | 优化建议 |
|------|---------|---------|
| drawGeometry (单个) | ~0.1ms | 批量绘制 |
| drawGeometry (1000个) | ~50ms | 使用 batchDrawFeatures |
| saveToFile (PNG 800x600) | ~20ms | 使用压缩优化 |
| drawText (100个) | ~5ms | 缓存字体 |

### 12.2 优化策略

```cpp
/**
 * @brief 性能优化建议
 */

// 1. 批量绘制优于逐个绘制
void optimizationExample1() {
    // 不推荐：逐个绘制
    for (const auto& feature : features) {
        driver->drawSingleFeature(device, engine, feature.get());
    }
    
    // 推荐：批量绘制
    std::vector<const ogc::Feature*> featurePtrs;
    for (const auto& feature : features) {
        featurePtrs.push_back(feature.get());
    }
    driver->batchDrawFeatures(device, engine, featurePtrs);
}

// 2. 设备复用
void optimizationExample2() {
    // 不推荐：每次创建新设备
    for (int i = 0; i < 100; ++i) {
        auto device = createImageDevice(800, 600);
        // 绘制...
        device->saveToFile("output_" + std::to_string(i) + ".png");
    }
    
    // 推荐：复用设备
    auto device = createImageDevice(800, 600);
    for (int i = 0; i < 100; ++i) {
        device->clear();
        // 绘制...
        device->saveToFile("output_" + std::to_string(i) + ".png");
    }
}

// 3. 空间索引加速裁剪
void optimizationExample3() {
    // 使用空间索引过滤不可见要素
    ogc::RTree<const ogc::Feature*> index;
    for (const auto& feature : features) {
        auto env = feature->getGeometry()->getEnvelope();
        index.insert(env, feature.get());
    }
    
    // 只绘制视口内的要素
    ogc::Envelope viewport(x1, y1, x2, y2);
    auto visibleFeatures = index.query(viewport);
    
    // 批量绘制
    driver->batchDrawFeatures(device, engine, visibleFeatures);
}

// 4. 样式缓存
void optimizationExample4() {
    // 不推荐：每次创建样式
    for (const auto& feature : features) {
        DrawStyle style;
        style.strokeColor = feature->getAttribute("color");
        // ...
        engine->drawGeometry(feature->getGeometry(), style);
    }
    
    // 推荐：缓存样式
    std::map<std::string, DrawStyle> styleCache;
    for (const auto& feature : features) {
        auto color = feature->getAttribute("color");
        if (styleCache.find(color) == styleCache.end()) {
            DrawStyle style;
            style.strokeColor = color;
            styleCache[color] = style;
        }
        engine->drawGeometry(feature->getGeometry(), styleCache[color]);
    }
}
```

---

## 13. 使用示例

### 13.1 简单图像导出

```cpp
#include <ogc/draw/draw_facade.h>
#include <ogc/geometry/point.h>
#include <ogc/geometry/polygon.h>

using namespace ogc;
using namespace ogc::draw;

void simpleImageExport() {
    // 准备几何数据
    auto point = Point::create(50, 50);
    auto polygon = Polygon::createRectangle(10, 10, 90, 90);
    
    // 使用门面类一键导出
    std::vector<FeaturePtr> features;
    features.push_back(Feature::create(point));
    features.push_back(Feature::create(polygon));
    
    DrawFacade::getInstance().drawToImage("output.png", features);
}
```

### 13.2 自定义样式绘制

```cpp
void customStyleDraw() {
    // 创建设备
    auto device = DrawFacade::getInstance().createImageDevice(800, 600);
    
    // 获取驱动和引擎
    auto& driverManager = DriverManager::getInstance();
    auto driver = driverManager.getDriver("QtDriver");
    auto engine = driver->createEngine(EngineType::QtEngine, device, DrawParams{});
    
    // 初始化
    device->initialize(DrawParams{});
    engine->initialize(device, DrawParams{});
    
    // 定义样式
    DrawStyle pointStyle;
    pointStyle.strokeColor = "#FF0000";
    pointStyle.fillColor = "#FF0000";
    pointStyle.strokeWidth = 2.0;
    
    DrawStyle polygonStyle;
    polygonStyle.strokeColor = "#0000FF";
    polygonStyle.fillColor = "#00FF00";
    polygonStyle.strokeWidth = 1.5;
    
    // 绘制
    auto point = Point::create(400, 300);
    auto polygon = Polygon::createRectangle(100, 100, 700, 500);
    
    device->beginDraw();
    engine->drawGeometry(point.get(), pointStyle);
    engine->drawGeometry(polygon.get(), polygonStyle);
    device->endDraw();
    
    // 保存
    device->saveToFile("styled_output.png");
}
```

### 13.3 多图层绘制

```cpp
void multiLayerDraw() {
    auto device = DrawFacade::getInstance().createImageDevice(800, 600);
    auto driver = DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(EngineType::QtEngine, device, DrawParams{});
    
    device->initialize(DrawParams{});
    engine->initialize(device, DrawParams{});
    
    // 创建图层
    int layer1 = engine->createLayer("背景层");
    int layer2 = engine->createLayer("道路层");
    int layer3 = engine->createLayer("标注层");
    
    // 设置图层透明度
    engine->setLayerOpacity(layer2, 0.8);
    
    // 绘制到不同图层
    device->beginDraw();
    
    engine->setActiveLayer(layer1);
    engine->drawGeometry(/* 背景多边形 */);
    
    engine->setActiveLayer(layer2);
    engine->drawGeometry(/* 道路网络 */);
    
    engine->setActiveLayer(layer3);
    engine->drawText(/* 标注文字 */);
    
    device->endDraw();
    
    device->saveToFile("multilayer_output.png");
}
```

### 13.4 符号化绘制

```cpp
void symbolizedDraw() {
    auto device = DrawFacade::getInstance().createImageDevice(800, 600);
    auto driver = DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(EngineType::QtEngine, device, DrawParams{});
    
    device->initialize(DrawParams{});
    engine->initialize(device, DrawParams{});
    
    // 创建符号化器
    auto pointSymbolizer = std::make_shared<PointSymbolizer>();
    pointSymbolizer->setSymbolType(PointSymbolizer::SymbolType::Circle);
    pointSymbolizer->setSize(8.0);
    pointSymbolizer->setColor("#FF0000");
    pointSymbolizer->setOutlineColor("#000000");
    pointSymbolizer->setOutlineWidth(1.0);
    
    auto lineSymbolizer = std::make_shared<LineSymbolizer>();
    lineSymbolizer->setColor("#0000FF");
    lineSymbolizer->setWidth(2.0);
    lineSymbolizer->setDashPattern({5.0, 3.0});  // 虚线
    
    auto polygonSymbolizer = std::make_shared<PolygonSymbolizer>();
    polygonSymbolizer->setFillColor("#00FF00");
    polygonSymbolizer->setStrokeColor("#000000");
    polygonSymbolizer->setStrokeWidth(1.0);
    polygonSymbolizer->setOpacity(0.5);
    
    // 准备要素
    std::vector<FeaturePtr> features;
    // 添加点、线、面要素...
    
    // 绘制
    device->beginDraw();
    driver->drawFeatures(device, engine, features, pointSymbolizer);
    device->endDraw();
    
    device->saveToFile("symbolized_output.png");
}
```

### 13.5 批量导出多格式

```cpp
void multiFormatExport() {
    auto& facade = DrawFacade::getInstance();
    
    std::vector<FeaturePtr> features;
    // 准备要素...
    
    std::vector<std::string> formats = {"png", "jpg", "pdf", "svg"};
    facade.exportToFormats(features, formats, "map", 1920, 1080);
    // 输出: map.png, map.jpg, map.pdf, map.svg
}
```

---

## 14. 编译和依赖

### 14.1 目录结构

```
ogc_draw/
├── include/
│   └── ogc/
│       └── draw/
│           ├── draw_common.h
│           ├── draw_device.h
│           ├── draw_engine.h
│           ├── draw_driver.h
│           ├── draw_context.h
│           ├── draw_facade.h
│           ├── draw_symbolizer.h
│           ├── draw_transform.h
│           └── draw_error.h
├── src/
│   ├── draw_device.cpp
│   ├── draw_engine.cpp
│   ├── draw_driver.cpp
│   └── ...
├── engines/
│   ├── qt/
│   ├── cairo/
│   └── gdi/
├── tests/
│   ├── test_device.cpp
│   ├── test_engine.cpp
│   └── ...
├── CMakeLists.txt
└── README.md
```

### 14.2 CMake配置

```cmake
cmake_minimum_required(VERSION 3.10)
project(OGCDraw VERSION 2.0.0 LANGUAGES CXX)

# C++11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
option(BUILD_QT_ENGINE "Build Qt engine" ON)
option(BUILD_CAIRO_ENGINE "Build Cairo engine" ON)
option(BUILD_GDI_ENGINE "Build GDI engine (Windows only)" OFF)
option(ENABLE_TESTS "Build unit tests" ON)

# 查找依赖
find_package(Threads REQUIRED)

if(BUILD_QT_ENGINE)
    find_package(Qt5 COMPONENTS Core Gui REQUIRED)
endif()

if(BUILD_CAIRO_ENGINE)
    find_package(Cairo REQUIRED)
endif()

# 库目标
add_library(ogc_draw
    src/draw_common.cpp
    src/draw_device.cpp
    src/draw_engine.cpp
    src/draw_driver.cpp
    src/draw_context.cpp
    src/draw_facade.cpp
    src/draw_symbolizer.cpp
    src/draw_transform.cpp
    src/draw_error.cpp
)

target_include_directories(ogc_draw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_draw
    PUBLIC
        Threads::Threads
        ogc_geometry  # 几何类库
)

# 引擎实现
if(BUILD_QT_ENGINE)
    add_subdirectory(engines/qt)
endif()

if(BUILD_CAIRO_ENGINE)
    add_subdirectory(engines/cairo)
endif()

# 测试
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装
install(TARGETS ogc_draw
    EXPORT OGCDrawTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ogc
    DESTINATION include
)
```

### 14.3 依赖说明

| 依赖项 | 版本要求 | 用途 | 必需性 |
|--------|---------|------|--------|
| C++编译器 | C++11 | 核心语言特性 | 必需 |
| CMake | >= 3.10 | 构建系统 | 必需 |
| Threads | - | 多线程支持 | 必需 |
| ogc_geometry | >= 2.0 | 几何类库 | 必需 |
| Qt5 | >= 5.12 | Qt引擎 | 可选 |
| Cairo | >= 1.16 | Cairo引擎 | 可选 |
| GDI+ | - | GDI引擎（Windows） | 可选 |

---

## 15. 测试策略

### 15.1 单元测试

```cpp
#include <gtest/gtest.h>
#include <ogc/draw/draw_facade.h>

// 设备测试
TEST(RasterImageDeviceTest, CreationAndSize) {
    auto device = ogc::draw::DrawFacade::getInstance().createImageDevice(800, 600);
    
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->getWidth(), 800);
    EXPECT_EQ(device->getHeight(), 600);
    EXPECT_EQ(device->getType(), ogc::draw::DeviceType::RasterImage);
}

// 引擎测试
TEST(QtEngineTest, DrawGeometry) {
    auto device = ogc::draw::DrawFacade::getInstance().createImageDevice(800, 600);
    auto driver = ogc::draw::DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(ogc::draw::EngineType::QtEngine, 
                                        device, ogc::draw::DrawParams{});
    
    device->initialize(ogc::draw::DrawParams{});
    engine->initialize(device, ogc::draw::DrawParams{});
    
    auto point = ogc::Point::create(400, 300);
    
    device->beginDraw();
    auto result = engine->drawGeometry(point.get(), ogc::draw::DrawStyle{});
    device->endDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::Success);
}
```

### 15.2 性能测试

```cpp
#include <benchmark/benchmark.h>

static void BM_DrawGeometry(benchmark::State& state) {
    auto device = ogc::draw::DrawFacade::getInstance().createImageDevice(1920, 1080);
    auto driver = ogc::draw::DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(ogc::draw::EngineType::QtEngine, 
                                        device, ogc::draw::DrawParams{});
    
    device->initialize(ogc::draw::DrawParams{});
    engine->initialize(device, ogc::draw::DrawParams{});
    
    auto polygon = ogc::Polygon::createRectangle(0, 0, 1920, 1080);
    
    for (auto _ : state) {
        device->beginDraw();
        engine->drawGeometry(polygon.get(), ogc::draw::DrawStyle{});
        device->endDraw();
    }
}
BENCHMARK(BM_DrawGeometry);

static void BM_BatchDraw(benchmark::State& state) {
    auto device = ogc::draw::DrawFacade::getInstance().createImageDevice(1920, 1080);
    auto driver = ogc::draw::DriverManager::getInstance().getDriver("QtDriver");
    auto engine = driver->createEngine(ogc::draw::EngineType::QtEngine, 
                                        device, ogc::draw::DrawParams{});
    
    std::vector<ogc::FeaturePtr> features = generateFeatures(state.range(0));
    
    for (auto _ : state) {
        ogc::draw::DrawFacade::getInstance().drawToImage("test.png", features);
    }
    
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_BatchDraw)->RangeMultiplier(10)->Range(100, 100000)->Complexity();
```

### 15.3 验收标准

| 指标 | 标准 | 说明 |
|------|------|------|
| 单元测试覆盖率 | >= 85% | 代码行覆盖率 |
| 绘制正确性 | 100% | 像素级对比验证 |
| 性能基准 | <= 100ms | 1000个要素绘制时间 |
| 内存泄漏 | 0 leaks | Valgrind/ASan检测 |
| 线程安全测试 | 通过 | ThreadSanitizer检测 |
| 跨平台测试 | 3平台 | Windows/Linux/macOS |

---

## 16. 与几何类库集成

### 16.1 命名空间统一

```cpp
// 几何类库
namespace ogc {
    class Geometry;
    class Point;
    class Feature;
}

// 绘制框架
namespace ogc {
namespace draw {
    class DrawDevice;
    class DrawEngine;
}
}

// 使用时
ogc::Point point(100, 100);
ogc::draw::DrawFacade::getInstance().drawToImage("output.png", {point});
```

### 16.2 接口对接

```cpp
/**
 * @brief 几何对象绘制接口
 */
class DrawEngine {
public:
    /**
     * @brief 绘制几何对象
     * @param geometry 几何对象指针
     * @param style 绘制样式
     * @return 绘制结果
     */
    virtual DrawResult drawGeometry(const ogc::Geometry* geometry,
                                     const DrawStyle& style) = 0;
};
```

### 16.3 坐标数据传递

```cpp
/**
 * @brief 获取连续坐标数据（GPU渲染优化）
 */
bool getCoordinateData(const ogc::Geometry* geometry,
                       const double** data,
                       size_t* count) {
    // 几何类库提供连续坐标数组
    if (geometry->getCoordinateData(data, count)) {
        // 直接传递给GPU
        return true;
    }
    return false;
}
```

---

## 17. 总结

本图形绘制框架基于GIS核心架构师评审建议，设计了一个**生产级**的跨平台图形绘制系统，重点增强了GIS领域核心特性支持。

### 核心优势

1. **稳定性**: Result模式错误处理、RAII资源管理、异常安全保证
2. **正确性**: DPI感知、坐标精确变换、完整测试覆盖
3. **高效性**: 批量绘制、硬件加速支持、资源池、缓存优化
4. **扩展性**: 抽象工厂模式、策略模式、清晰的接口设计
5. **跨平台**: 纯C++11、平台抽象层、多种渲染引擎
6. **多线程安全**: 读写锁保护、并发绘制模式、资源隔离
7. **GIS标准兼容**: SLD/SE支持、Mapbox Style支持、OGC标准兼容

### 关键改进（v2.1）

- ✅ 添加瓦片渲染支持（TileDevice、TileRenderer、TileCache）
- ✅ 添加空间参考系统处理（CoordinateTransformer、ProjTransformer）
- ✅ 添加符号化规则引擎（Filter、SymbolizerRule、RuleEngine）
- ✅ 添加LOD机制（LODLevel、LODStrategy、LODManager）
- ✅ 添加异步渲染机制（RenderTask、RenderQueue）
- ✅ 添加性能监控接口（PerformanceMetrics、PerformanceMonitor）
- ✅ 添加GPU资源管理（GPUResource、VertexBuffer、TextureBuffer）
- ✅ 添加日志系统（Logger、LogManager）
- ✅ 添加SLD解析器和Mapbox Style解析器

### 已知限制

| 限制项 | 说明 | 计划版本 |
|--------|------|----------|
| 3D渲染 | 当前仅支持2D渲染 | v3.0 |
| 矢量瓦片规范 | MVT格式解析待实现 | v2.2 |
| 文字标注避让 | 自动避让算法待优化 | v2.2 |
| 动态投影 | 实时坐标转换性能待优化 | v2.3 |
| WebAssembly | 浏览器端支持待实现 | v3.0 |
| 分布式渲染 | 多节点协同渲染待设计 | v4.0 |

### 后续工作

1. 实现Qt引擎（优先）
2. 实现Cairo引擎
3. 实现Proj库集成
4. 实现SLD解析器
5. 实现Mapbox Style解析器
6. 完善单元测试和性能测试
7. 添加更多设备类型（SVG、打印机等）
8. 优化硬件加速引擎（OpenGL、Vulkan）
9. 编写用户手册和API文档

---

**文档版本**: v2.1  
**最后更新**: 2026年3月18日  
**维护者**: Graphics Drawing Framework Team
