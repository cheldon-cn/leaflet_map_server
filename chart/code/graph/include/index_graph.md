# Graph Module - Header Index

## Overview

Graph模块提供地图渲染和绑制功能。实现了DrawDevice抽象接口和Symbolizer符号化系统，支持多种输出设备（光栅图像、PDF、瓦片），包含样式解析（SLD、Mapbox Style）、标签放置、LOD管理等功能。

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [draw_device.h](ogc/draw/draw_device.h) | 绑制设备基类 | `DrawDevice` |
| [draw_context.h](ogc/draw/draw_context.h) | 绑制上下文 | `DrawContext` |
| [draw_engine.h](ogc/draw/draw_engine.h) | 绑制引擎 | `DrawEngine` |
| [draw_driver.h](ogc/draw/draw_driver.h) | 绑制驱动 | `DrawDriver` |
| [draw_facade.h](ogc/draw/draw_facade.h) | 绑制门面 | `DrawFacade` |
| [draw_params.h](ogc/draw/draw_params.h) | 绑制参数 | `DrawParams` |
| [draw_result.h](ogc/draw/draw_result.h) | 绑制结果 | `DrawResult` |
| [draw_style.h](ogc/draw/draw_style.h) | 绑制样式 | `DrawStyle` |
| [symbolizer.h](ogc/draw/symbolizer.h) | 符号化基类 | `Symbolizer` |
| [point_symbolizer.h](ogc/draw/point_symbolizer.h) | 点符号化 | `PointSymbolizer` |
| [line_symbolizer.h](ogc/draw/line_symbolizer.h) | 线符号化 | `LineSymbolizer` |
| [polygon_symbolizer.h](ogc/draw/polygon_symbolizer.h) | 多边形符号化 | `PolygonSymbolizer` |
| [text_symbolizer.h](ogc/draw/text_symbolizer.h) | 文本符号化 | `TextSymbolizer` |
| [raster_symbolizer.h](ogc/draw/raster_symbolizer.h) | 栅格符号化 | `RasterSymbolizer` |
| [icon_symbolizer.h](ogc/draw/icon_symbolizer.h) | 图标符号化 | `IconSymbolizer` |
| [composite_symbolizer.h](ogc/draw/composite_symbolizer.h) | 组合符号化 | `CompositeSymbolizer` |
| [raster_image_device.h](ogc/draw/raster_image_device.h) | 光栅设备 | `RasterImageDevice` |
| [pdf_device.h](ogc/draw/pdf_device.h) | PDF设备 | `PdfDevice` |
| [tile_device.h](ogc/draw/tile_device.h) | 瓦片设备 | `TileDevice` |
| [tile_cache.h](ogc/draw/tile_cache.h) | 瓦片缓存 | `TileCache` |
| [tile_key.h](ogc/draw/tile_key.h) | 瓦片键 | `TileKey` |
| [tile_renderer.h](ogc/draw/tile_renderer.h) | 瓦片渲染 | `TileRenderer` |
| [filter.h](ogc/draw/filter.h) | 过滤器基类 | `Filter` |
| [comparison_filter.h](ogc/draw/comparison_filter.h) | 比较过滤器 | `ComparisonFilter` |
| [logical_filter.h](ogc/draw/logical_filter.h) | 逻辑过滤器 | `LogicalFilter` |
| [spatial_filter.h](ogc/draw/spatial_filter.h) | 空间过滤器 | `SpatialFilter` |
| [rule_engine.h](ogc/draw/rule_engine.h) | 规则引擎 | `RuleEngine` |
| [symbolizer_rule.h](ogc/draw/symbolizer_rule.h) | 符号化规则 | `SymbolizerRule` |
| [sld_parser.h](ogc/draw/sld_parser.h) | SLD解析器 | `SLDParser` |
| [mapbox_style_parser.h](ogc/draw/mapbox_style_parser.h) | Mapbox样式解析 | `MapboxStyleParser` |
| [label_engine.h](ogc/draw/label_engine.h) | 标签引擎 | `LabelEngine` |
| [label_placement.h](ogc/draw/label_placement.h) | 标签放置 | `LabelPlacement` |
| [label_conflict.h](ogc/draw/label_conflict.h) | 标签冲突 | `LabelConflict` |
| [lod.h](ogc/draw/lod.h) | LOD定义 | `LODLevel` |
| [lod_manager.h](ogc/draw/lod_manager.h) | LOD管理 | `LODManager` |
| [layer_manager.h](ogc/draw/layer_manager.h) | 图层管理 | `LayerManager` |
| [render_queue.h](ogc/draw/render_queue.h) | 渲染队列 | `RenderQueue` |
| [render_task.h](ogc/draw/render_task.h) | 渲染任务 | `RenderTask` |
| [async_renderer.h](ogc/draw/async_renderer.h) | 异步渲染 | `AsyncRenderer` |
| [transform_matrix.h](ogc/draw/transform_matrix.h) | 变换矩阵 | `TransformMatrix` |
| [transform_manager.h](ogc/draw/transform_manager.h) | 变换管理 | `TransformManager` |
| [coordinate_transform.h](ogc/draw/coordinate_transform.h) | 坐标转换 | `CoordinateTransform` |
| [coordinate_transformer.h](ogc/draw/coordinate_transformer.h) | 坐标转换器 | `CoordinateTransformer` |
| [proj_transformer.h](ogc/draw/proj_transformer.h) | PROJ转换器 | `ProjTransformer` |
| [color.h](ogc/draw/color.h) | 颜色类 | `Color` |
| [font.h](ogc/draw/font.h) | 字体类 | `Font` |
| [clipper.h](ogc/draw/clipper.h) | 裁剪器 | `Clipper` |
| [image_draw.h](ogc/draw/image_draw.h) | 图像绘制 | `ImageDraw` |
| [performance_monitor.h](ogc/draw/performance_monitor.h) | 性能监控 | `PerformanceMonitor` |
| [performance_metrics.h](ogc/draw/performance_metrics.h) | 性能指标 | `PerformanceMetrics` |
| [types.h](ogc/draw/types.h) | 类型定义 | `DeviceType`, `EngineType` |
| [device_type.h](ogc/draw/device_type.h) | 设备类型 | `DeviceType` |
| [device_state.h](ogc/draw/device_state.h) | 设备状态 | `DeviceState` |
| [engine_type.h](ogc/draw/engine_type.h) | 引擎类型 | `EngineType` |
| [driver_manager.h](ogc/draw/driver_manager.h) | 驱动管理 | `DrawDriverManager` |
| [draw_error.h](ogc/draw/draw_error.h) | 错误定义 | `DrawError` |
| [log.h](ogc/draw/log.h) | 日志工具 | `Log` |
| [thread_safe.h](ogc/draw/thread_safe.h) | 线程安全 | `ThreadSafe` |
| [export.h](ogc/draw/export.h) | 导出宏定义 | `OGC_GRAPH_API` |

## Class Inheritance Diagram

```
DrawDevice (abstract base)
    ├── RasterImageDevice
    ├── PdfDevice
    └── TileDevice

Symbolizer (abstract base)
    ├── PointSymbolizer
    ├── LineSymbolizer
    ├── PolygonSymbolizer
    ├── TextSymbolizer
    ├── RasterSymbolizer
    ├── IconSymbolizer
    └── CompositeSymbolizer

Filter (abstract base)
    ├── ComparisonFilter
    ├── LogicalFilter
    └── SpatialFilter

TileCache (abstract base)
    ├── MemoryTileCache
    ├── DiskTileCache
    └── MultiLevelTileCache
```

## Core Classes Detail

### DrawDevice

**File**: [draw_device.h](ogc/draw/draw_device.h)

**Base Classes**: None (abstract interface)

**Purpose**: 绑制设备抽象基类，定义所有输出设备的通用接口。

**Key Methods**:
- `GetType()` / `GetName()` - 设备信息
- `Initialize()` / `Finalize()` - 生命周期
- `BeginDraw()` / `EndDraw()` / `AbortDraw()` - 绑制控制
- `SetTransform()` / `GetTransform()` - 变换操作
- `SetClipRect()` / `ClearClipRect()` - 裁剪区域
- `DrawPoint()` / `DrawLine()` / `DrawRect()` / `DrawCircle()` - 基本图元
- `DrawPolygon()` / `DrawPolyline()` - 多边形/多线
- `DrawGeometry()` - 几何绑制
- `DrawText()` / `DrawTextWithBackground()` - 文本绑制
- `DrawImage()` / `DrawImageRegion()` - 图像绑制
- `Clear()` / `Fill()` - 清空/填充
- `GetWidth()` / `GetHeight()` / `GetDpi()` - 设备属性
- `SetAntialiasing()` / `SetOpacity()` - 渲染设置

### Symbolizer

**File**: [symbolizer.h](ogc/draw/symbolizer.h)

**Base Classes**: None (abstract interface)

**Purpose**: 符号化抽象基类，定义几何对象到可视化表示的转换。

**Key Methods**:
- `GetType()` / `GetName()` - 符号化信息
- `Symbolize()` - 执行符号化
- `CanSymbolize()` - 能力检测
- `Clone()` - 克隆符号化
- `SetDefaultStyle()` / `GetDefaultStyle()` - 默认样式
- `SetMinScale()` / `SetMaxScale()` - 比例尺范围
- `SetZIndex()` / `SetOpacity()` - 渲染属性

### RasterImageDevice

**File**: [raster_image_device.h](ogc/draw/raster_image_device.h)

**Base Classes**: `DrawDevice`

**Purpose**: 光栅图像输出设备，支持PNG、JPEG等格式。

**Key Methods**:
- `Create(width, height, channels)` - 工厂方法
- `GetImageData()` - 获取图像数据
- `SaveToFile()` - 保存到文件
- `SetBackgroundColor()` - 设置背景色

### PdfDevice

**File**: [pdf_device.h](ogc/draw/pdf_device.h)

**Base Classes**: `DrawDevice`

**Purpose**: PDF输出设备，支持矢量图形和多页文档。

**Key Methods**:
- `Create(width, height, dpi)` - 工厂方法
- `NewPage()` - 创建新页
- `GetCurrentPage()` / `GetPageCount()` - 页面信息
- `SetTitle()` / `SetAuthor()` - 文档属性
- `SaveToFile()` - 保存PDF

### TileDevice

**File**: [tile_device.h](ogc/draw/tile_device.h)

**Base Classes**: `DrawDevice`

**Purpose**: 瓦片输出设备，支持金字塔瓦片生成。

**Key Methods**:
- `SetTileSize()` - 设置瓦片大小
- `SetZoomLevels()` - 设置缩放级别
- `GenerateTiles()` - 生成瓦片
- `GetTile()` - 获取瓦片

### Filter

**File**: [filter.h](ogc/draw/filter.h)

**Base Classes**: None (abstract interface)

**Purpose**: 过滤器抽象基类，用于规则匹配。

**Key Methods**:
- `Evaluate()` - 评估过滤器
- `Clone()` - 克隆过滤器

### RuleEngine

**File**: [rule_engine.h](ogc/draw/rule_engine.h)

**Purpose**: 规则引擎，管理符号化规则。

**Key Methods**:
- `AddRule()` / `RemoveRule()` - 规则管理
- `GetMatchingSymbolizers()` - 获取匹配的符号化
- `LoadFromSLD()` / `LoadFromMapboxStyle()` - 从样式文件加载

## Type Aliases

```cpp
using DrawDevicePtr = std::shared_ptr<DrawDevice>;
using SymbolizerPtr = std::shared_ptr<Symbolizer>;
using FilterPtr = std::shared_ptr<Filter>;
using DrawContextPtr = std::shared_ptr<DrawContext>;
using TileCachePtr = std::shared_ptr<TileCache>;
```

## Enumerations

### DeviceType

```cpp
enum class DeviceType {
    kUnknown,
    kRaster,
    kPdf,
    kTile,
    kDisplay
};
```

### SymbolizerType

```cpp
enum class SymbolizerType {
    kPoint,
    kLine,
    kPolygon,
    kText,
    kRaster,
    kComposite,
    kIcon
};
```

### DrawResult

```cpp
enum class DrawResult {
    kSuccess,
    kFailed,
    kInvalidParams,
    kDeviceNotReady,
    kNotSupported
};
```

## Dependencies

```
graph
  ├── geom (Geometry, Envelope)
  ├── feature (Feature)
  ├── layer (Layer)
  └── external (PROJ)
```

## Quick Usage Examples

```cpp
#include "ogc/draw/draw_device.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/draw/point_symbolizer.h"

// 创建光栅设备
auto device = RasterImageDevice::Create(800, 600, 4);
device->Initialize();
device->BeginDraw(DrawParams());

// 绑制几何
DrawStyle style;
style.fill.color = Color(255, 0, 0).GetRGBA();
device->DrawCircle(400, 300, 50, style);

// 使用符号化
auto symbolizer = PointSymbolizer::Create();
symbolizer->Symbolize(context, geometry);

// 保存图像
device->EndDraw();
device->SaveToFile("output.png");

// PDF输出
auto pdf = PdfDevice::Create(612, 792);
pdf->Initialize();
pdf->BeginDraw(DrawParams());
pdf->DrawGeometry(geometry, style);
pdf->EndDraw();
pdf->SaveToFile("output.pdf");
```
