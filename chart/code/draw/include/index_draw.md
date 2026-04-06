# Draw Module - Header Index

## 模块描述

Draw模块提供跨平台的2D图形渲染引擎框架，支持多种渲染后端（Simple2D、GPU加速、WebGL、Cairo、Direct2D、GDI+、Qt、Metal、CoreGraphics等），提供统一的绘图接口、设备抽象、样式管理和批量渲染功能。

## 核心特性

- 多渲染引擎支持：Simple2D、GPU加速、WebGL、Cairo、Direct2D、GDI+、Qt、Metal、CoreGraphics
- 多设备类型支持：显示设备、栅格图像、PDF、SVG、瓦片设备
- 统一的绘图接口：DrawEngine、DrawDevice、DrawContext
- 样式系统：Pen、Brush、Font、Color、DrawStyle
- 批量渲染和缓存优化
- 异步渲染和线程安全
- 跨平台支持：Windows、macOS、Linux、Web

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/draw/export.h) | DLL导出宏 | `OGC_DRAW_API` |
| [engine_type.h](ogc/draw/engine_type.h) | 引擎类型 | `EngineType` |
| [device_type.h](ogc/draw/device_type.h) | 设备类型 | `DeviceType` |
| [device_state.h](ogc/draw/device_state.h) | 设备状态 | `DeviceState` |
| [draw_result.h](ogc/draw/draw_result.h) | 绘图结果 | `DrawResult` |
| [draw_types.h](ogc/draw/draw_types.h) | 绘图类型定义 | `DrawDevicePtr`, `DrawEnginePtr` |
| [geometry_types.h](ogc/draw/geometry_types.h) | 几何类型 | `Point`, `Size`, `Rect` |
| [color.h](ogc/draw/color.h) | 颜色类 | `Color` |
| [font.h](ogc/draw/font.h) | 字体类 | `Font` |
| [image.h](ogc/draw/image.h) | 图像类 | `Image` |
| [region.h](ogc/draw/region.h) | 区域类 | `Region` |
| [transform_matrix.h](ogc/draw/transform_matrix.h) | 变换矩阵 | `TransformMatrix` |
| [text_metrics.h](ogc/draw/text_metrics.h) | 文本度量 | `TextMetrics` |
| [draw_style.h](ogc/draw/draw_style.h) | 绘图样式 | `Pen`, `Brush`, `DrawStyle` |
| [geometry.h](ogc/draw/geometry.h) | 几何适配器 | `Geometry` |
| [draw_device.h](ogc/draw/draw_device.h) | 绘图设备基类 | `DrawDevice`, `DeviceCapabilities` |
| [draw_engine.h](ogc/draw/draw_engine.h) | 绘图引擎基类 | `DrawEngine`, `StateFlag` |
| [draw_context.h](ogc/draw/draw_context.h) | 绘图上下文 | `DrawContext` |
| [simple2d_engine.h](ogc/draw/simple2d_engine.h) | Simple2D引擎 | `Simple2DEngine` |
| [gpu_accelerated_engine.h](ogc/draw/gpu_accelerated_engine.h) | GPU加速引擎 | `GPUAcceleratedEngine` |
| [vector_engine.h](ogc/draw/vector_engine.h) | 矢量引擎 | `VectorEngine` |
| [tile_based_engine.h](ogc/draw/tile_based_engine.h) | 瓦片引擎 | `TileBasedEngine` |
| [batch_renderer.h](ogc/draw/batch_renderer.h) | 批量渲染器 | `BatchRenderer`, `BatchItem` |
| [render_cache.h](ogc/draw/render_cache.h) | 渲染缓存 | `RenderCache` |
| [engine_pool.h](ogc/draw/engine_pool.h) | 引擎池 | `EnginePool` |
| [display_device.h](ogc/draw/display_device.h) | 显示设备 | `DisplayDevice` |
| [raster_image_device.h](ogc/draw/raster_image_device.h) | 栅格图像设备 | `RasterImageDevice` |
| [pdf_device.h](ogc/draw/pdf_device.h) | PDF设备 | `PdfDevice` |
| [pdf_engine.h](ogc/draw/pdf_engine.h) | PDF引擎 | `PdfEngine` |
| [svg_device.h](ogc/draw/svg_device.h) | SVG设备 | `SvgDevice` |
| [svg_engine.h](ogc/draw/svg_engine.h) | SVG引擎 | `SvgEngine` |
| [tile_device.h](ogc/draw/tile_device.h) | 瓦片设备 | `TileDevice` |
| [webgl_device.h](ogc/draw/webgl_device.h) | WebGL设备 | `WebGLDevice` |
| [webgl_engine.h](ogc/draw/webgl_engine.h) | WebGL引擎 | `WebGLEngine` |
| [cairo_engine.h](ogc/draw/cairo_engine.h) | Cairo引擎 | `CairoEngine` |
| [direct2d_engine.h](ogc/draw/direct2d_engine.h) | Direct2D引擎 | `Direct2DEngine` |
| [gdiplus_engine.h](ogc/draw/gdiplus_engine.h) | GDI+引擎 | `GdiplusEngine` |
| [qt_engine.h](ogc/draw/qt_engine.h) | Qt引擎 | `QtEngine` |
| [qt_display_device.h](ogc/draw/qt_display_device.h) | Qt显示设备 | `QtDisplayDevice` |
| [metal_engine.h](ogc/draw/metal_engine.h) | Metal引擎 | `MetalEngine` |
| [coregraphics_engine.h](ogc/draw/coregraphics_engine.h) | CoreGraphics引擎 | `CoreGraphicsEngine` |
| [thread_safe_engine.h](ogc/draw/thread_safe_engine.h) | 线程安全引擎 | `ThreadSafeEngine` |
| [async_render_manager.h](ogc/draw/async_render_manager.h) | 异步渲染管理器 | `AsyncRenderManager` |
| [async_render_task.h](ogc/draw/async_render_task.h) | 异步渲染任务 | `AsyncRenderTask` |
| [gpu_resource_manager.h](ogc/draw/gpu_resource_manager.h) | GPU资源管理器 | `GPUResourceManager` |
| [gpu_resource_wrapper.h](ogc/draw/gpu_resource_wrapper.h) | GPU资源包装器 | `VertexBuffer`, `IndexBuffer`, `ShaderProgram` |
| [texture_cache.h](ogc/draw/texture_cache.h) | 纹理缓存 | `TextureCache` |
| [render_memory_pool.h](ogc/draw/render_memory_pool.h) | 渲染内存池 | `RenderMemoryPool` |
| [lod_strategy.h](ogc/draw/lod_strategy.h) | LOD策略 | `LODStrategy` |
| [tile_size_strategy.h](ogc/draw/tile_size_strategy.h) | 瓦片大小策略 | `TileSizeStrategy` |
| [plugin_manager.h](ogc/draw/plugin_manager.h) | 插件管理器 | `PluginManager` |
| [draw_device_plugin.h](ogc/draw/draw_device_plugin.h) | 设备插件 | `DrawDevicePlugin` |
| [draw_engine_plugin.h](ogc/draw/draw_engine_plugin.h) | 引擎插件 | `DrawEnginePlugin` |
| [windows_platform.h](ogc/draw/windows_platform.h) | Windows平台 | - |
| [macos_platform.h](ogc/draw/macos_platform.h) | macOS平台 | - |
| [web_platform.h](ogc/draw/web_platform.h) | Web平台 | - |
| [x11_display_device.h](ogc/draw/x11_display_device.h) | X11显示设备 | `X11DisplayDevice` |

---

## 类继承关系图

```
DrawEngine (abstract)
    ├── Simple2DEngine
    ├── GPUAcceleratedEngine
    ├── VectorEngine
    ├── TileBasedEngine
    ├── PdfEngine
    ├── SvgEngine
    ├── WebGLEngine
    ├── CairoEngine
    ├── Direct2DEngine
    ├── GdiplusEngine
    ├── QtEngine
    ├── MetalEngine
    ├── CoreGraphicsEngine
    └── ThreadSafeEngine (装饰器)

DrawDevice (abstract)
    ├── DisplayDevice
    │       ├── QtDisplayDevice
    │       └── X11DisplayDevice
    ├── RasterImageDevice
    ├── PdfDevice
    ├── SvgDevice
    ├── TileDevice
    └── WebGLDevice

DrawContext (abstract)
    └── DrawContextImpl
```

---

## 依赖关系图

```
export.h
    │
    ├──► engine_type.h
    ├──► device_type.h
    ├──► device_state.h
    ├──► draw_result.h
    │
    ├──► geometry_types.h ──► color.h ──► font.h
    │                              │
    │                              └──► draw_style.h
    │
    ├──► image.h
    ├──► region.h
    ├──► transform_matrix.h
    ├──► text_metrics.h
    │
    └──► draw_device.h ──► draw_engine.h ──► draw_context.h
             │                   │
             │                   ├──► simple2d_engine.h
             │                   ├──► gpu_accelerated_engine.h
             │                   ├──► vector_engine.h
             │                   ├──► tile_based_engine.h
             │                   └──► [platform engines]
             │
             ├──► display_device.h
             ├──► raster_image_device.h
             ├──► pdf_device.h ──► pdf_engine.h
             ├──► svg_device.h ──► svg_engine.h
             ├──► tile_device.h
             └──► webgl_device.h ──► webgl_engine.h

batch_renderer.h ──► render_cache.h
async_render_manager.h ──► async_render_task.h
gpu_resource_manager.h ──► gpu_resource_wrapper.h ──► texture_cache.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core Types** | export.h, engine_type.h, device_type.h, device_state.h, draw_result.h, draw_types.h |
| **Geometry** | geometry_types.h, geometry.h, region.h, transform_matrix.h |
| **Style** | color.h, font.h, image.h, text_metrics.h, draw_style.h |
| **Engine** | draw_engine.h, simple2d_engine.h, gpu_accelerated_engine.h, vector_engine.h, tile_based_engine.h |
| **Device** | draw_device.h, display_device.h, raster_image_device.h, pdf_device.h, svg_device.h, tile_device.h, webgl_device.h |
| **Context** | draw_context.h |
| **Platform Engines** | cairo_engine.h, direct2d_engine.h, gdiplus_engine.h, qt_engine.h, metal_engine.h, coregraphics_engine.h, webgl_engine.h |
| **Batch & Cache** | batch_renderer.h, render_cache.h, engine_pool.h |
| **GPU Resources** | gpu_resource_manager.h, gpu_resource_wrapper.h, texture_cache.h, render_memory_pool.h |
| **Async** | async_render_manager.h, async_render_task.h, thread_safe_engine.h |
| **Strategy** | lod_strategy.h, tile_size_strategy.h |
| **Plugin** | plugin_manager.h, draw_device_plugin.h, draw_engine_plugin.h |
| **Platform** | windows_platform.h, macos_platform.h, web_platform.h, x11_display_device.h |

---

## 关键类

### DrawEngine (抽象基类)
**File**: [draw_engine.h](ogc/draw/draw_engine.h)  
**Description**: 绘图引擎抽象基类

```cpp
class DrawEngine {
public:
    virtual ~DrawEngine() = default;
    
    virtual std::string GetName() const = 0;
    virtual EngineType GetType() const = 0;
    virtual DrawDevice* GetDevice() const = 0;
    
    virtual DrawResult Begin() = 0;
    virtual void End() = 0;
    virtual bool IsActive() const = 0;
    
    // 绘图方法
    virtual DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawLineString(const double* x, const double* y, int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawRect(double x, double y, double w, double h, const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) = 0;
    virtual DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) = 0;
    
    // 变换
    virtual void SetTransform(const TransformMatrix& matrix) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;
    
    // 裁剪
    virtual void SetClipRect(double x, double y, double w, double h) = 0;
    virtual void SetClipRegion(const Region& region) = 0;
    virtual void ResetClip() = 0;
    
    // 状态管理
    virtual void Save(StateFlags flags = StateFlag::kAll) = 0;
    virtual void Restore() = 0;
    virtual void SetOpacity(double opacity) = 0;
};
```

### DrawDevice (抽象基类)
**File**: [draw_device.h](ogc/draw/draw_device.h)  
**Description**: 绘图设备抽象基类

```cpp
class DrawDevice {
public:
    virtual ~DrawDevice() = default;
    
    virtual DeviceType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual double GetDpi() const = 0;
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    virtual DeviceState GetState() const = 0;
    virtual bool IsReady() const = 0;
    
    virtual std::unique_ptr<DrawEngine> CreateEngine() = 0;
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    
    virtual DeviceCapabilities QueryCapabilities() const = 0;
    virtual bool IsDeviceLost() const = 0;
    virtual DrawResult RecoverDevice() = 0;
};
```

### DrawContext
**File**: [draw_context.h](ogc/draw/draw_context.h)  
**Description**: 高层绘图上下文，封装设备和引擎

```cpp
class DrawContext {
public:
    static std::unique_ptr<DrawContext> Create(DrawDevice* device);
    
    virtual DrawResult Begin() = 0;
    virtual void End() = 0;
    
    virtual DrawDevice* GetDevice() const = 0;
    virtual DrawEngine* GetEngine() const = 0;
    
    // 绘图方法
    virtual DrawResult DrawGeometry(const Geometry* geometry) = 0;
    virtual DrawResult DrawLineString(const double* x, const double* y, int count) = 0;
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill = true) = 0;
    virtual DrawResult DrawRect(double x, double y, double w, double h, bool fill = true) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius, bool fill = true) = 0;
    virtual DrawResult DrawText(double x, double y, const std::string& text) = 0;
    
    // 变换
    virtual void Translate(double dx, double dy) = 0;
    virtual void Rotate(double angleRadians) = 0;
    virtual void Scale(double sx, double sy) = 0;
    
    // 样式
    virtual void SetStyle(const DrawStyle& style) = 0;
    virtual void SetPen(const Pen& pen) = 0;
    virtual void SetBrush(const Brush& brush) = 0;
};
```

### Color
**File**: [color.h](ogc/draw/color.h)  
**Description**: RGBA颜色类

```cpp
class Color {
public:
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    uint8_t GetRed() const;
    uint8_t GetGreen() const;
    uint8_t GetBlue() const;
    uint8_t GetAlpha() const;
    
    uint32_t GetARGB() const;
    uint32_t GetRGBA() const;
    uint32_t GetRGB() const;
    
    void GetHSL(double& h, double& s, double& l) const;
    void SetHSL(double h, double s, double l);
    
    Color WithAlpha(uint8_t a) const;
    Color Lighter(double factor = 1.5) const;
    Color Darker(double factor = 0.7) const;
    
    static Color FromHexString(const std::string& hex);
    static Color Transparent();
    static Color Black();
    static Color White();
    // ...
};
```

### DrawStyle
**File**: [draw_style.h](ogc/draw/draw_style.h)  
**Description**: 绘图样式组合

```cpp
struct DrawStyle {
    Pen pen;
    Brush brush;
    Font font;
    double opacity;
    FillRule fillRule;
    bool antialias;
    PointMarkerType pointMarker;
    double pointSize;
    
    static DrawStyle Default();
    static DrawStyle Stroke(const Color& color, double width = 1.0);
    static DrawStyle Fill(const Color& color);
    static DrawStyle StrokeAndFill(const Color& strokeColor, double strokeWidth, const Color& fillColor);
    
    DrawStyle WithPen(const Pen& p) const;
    DrawStyle WithBrush(const Brush& b) const;
    DrawStyle WithOpacity(double o) const;
};
```

### BatchRenderer
**File**: [batch_renderer.h](ogc/draw/batch_renderer.h)  
**Description**: 批量渲染器

```cpp
class BatchRenderer {
public:
    BatchRenderer();
    explicit BatchRenderer(DrawEngine* engine);
    
    void SetEngine(DrawEngine* engine);
    void BeginBatch();
    void AddGeometry(const GeometrySharedPtr& geometry, const DrawStyle& style, int priority = 0);
    void EndBatch();
    void Flush();
    
    void SetBatchSize(int size);
    void SetAutoFlush(bool autoFlush);
    void SetSortByPriority(bool sort);
};
```

---

## 类型定义

### EngineType (引擎类型)
```cpp
enum class EngineType {
    kUnknown = 0,
    kSimple2D = 1,
    kGPU = 2,
    kVector = 3,
    kTile = 4,
    kWebGL = 5
};
```

### DeviceType (设备类型)
```cpp
enum class DeviceType {
    kUnknown = 0,
    kDisplay = 1,
    kRasterImage = 2,
    kPdf = 3,
    kSvg = 4,
    kTile = 5,
    kWebGL = 6,
    kPrinter = 7
};
```

### PenStyle (画笔样式)
```cpp
enum class PenStyle {
    kSolid = 0,
    kDash = 1,
    kDot = 2,
    kDashDot = 3,
    kDashDotDot = 4,
    kNone = 5
};
```

### BrushStyle (画刷样式)
```cpp
enum class BrushStyle {
    kSolid = 0,
    kNone = 1,
    kHorizontal = 2,
    kVertical = 3,
    kCross = 4,
    kBDiagonal = 5,
    kFDiagonal = 6,
    kDiagonalCross = 7,
    kTexture = 8
};
```

### FillRule (填充规则)
```cpp
enum class FillRule {
    kEvenOdd = 0,
    kNonZero = 1
};
```

### StateFlag (状态标志)
```cpp
enum class StateFlag : uint32_t {
    kNone = 0,
    kTransform = 1 << 0,
    kStyle = 1 << 1,
    kOpacity = 1 << 2,
    kClip = 1 << 3,
    kFont = 1 << 4,
    kAll = 0xFFFFFFFF
};
```

---

## 使用示例

### 创建绘图上下文

```cpp
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_context.h"

using namespace ogc::draw;

// 创建栅格图像设备
auto device = std::make_unique<RasterImageDevice>(800, 600, 96.0);

// 创建绘图上下文
auto context = DrawContext::Create(device.get());

// 开始绘图
context->Begin();

// 设置样式
DrawStyle style = DrawStyle::StrokeAndFill(Color::Red(), 2.0, Color::Blue());
context->SetStyle(style);

// 绘制矩形
context->DrawRect(100, 100, 200, 150, true);

// 绘制圆形
context->DrawCircle(400, 300, 50, true);

// 结束绘图
context->End();
```

### 使用引擎直接绘图

```cpp
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"

// 创建设备
RasterImageDevice device(800, 600, 96.0);
device.Initialize();

// 创建引擎
Simple2DEngine engine(&device);
engine.Begin();

// 设置变换
TransformMatrix matrix;
matrix.Translate(400, 300);
matrix.Rotate(3.14159 / 4);  // 45度
engine.SetTransform(matrix);

// 绘制
DrawStyle style = DrawStyle::Stroke(Color::Black(), 2.0);
engine.DrawRect(-50, -50, 100, 100, style, false);

engine.End();
```

### 批量渲染

```cpp
#include "ogc/draw/batch_renderer.h"

BatchRenderer renderer(&engine);

renderer.BeginBatch();

for (const auto& geom : geometries) {
    DrawStyle style = GetStyleForGeometry(geom);
    renderer.AddGeometry(geom, style, 0);
}

renderer.EndBatch();
renderer.Flush();
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | index-validator | 初始版本，生成完整索引 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.0  
**C++ Standard**: C++11
