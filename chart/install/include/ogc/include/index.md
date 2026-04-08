# Graph模块 - 头文件索引

## 概述

Graph模块是OGC海图系统的绘图渲染模块，负责将几何数据转换为可视化图像。支持多种输出设备（光栅图像、PDF、瓦片）、符号化渲染、瓦片缓存等核心功能。

### 模块职责
- **绘图设备**: 提供统一的绘图设备抽象，支持光栅图像、PDF、瓦片等多种输出
- **绘图引擎**: 提供绘图引擎抽象，支持简单2D、瓦片化、GPU加速等渲染模式
- **符号化渲染**: 支持点、线、面、文字、图标等多种符号化方式
- **瓦片系统**: 提供瓦片索引、瓦片缓存、瓦片渲染等完整的瓦片处理能力
- **样式系统**: 支持样式定义、规则引擎、SLD/Mapbox样式解析

---

## 头文件列表

### 核心绘图设备

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [draw_device.h](ogc/draw/draw_device.h) | 绘图设备抽象基类 | `DrawDevice` |
| [raster_image_device.h](ogc/draw/raster_image_device.h) | 光栅图像设备 | `RasterImageDevice`, `ImageFormat` |
| [tile_device.h](ogc/draw/tile_device.h) | 瓦片绘图设备 | `TileDevice` |
| [pdf_device.h](ogc/draw/pdf_device.h) | PDF输出设备 | `PdfDevice` |
| [display_device.h](ogc/draw/display_device.h) | 显示设备 | `DisplayDevice` |

### 绘图引擎与上下文

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [draw_engine.h](ogc/draw/draw_engine.h) | 绘图引擎抽象基类 | `DrawEngine` |
| [draw_context.h](ogc/draw/draw_context.h) | 绘图上下文 | `DrawContext` |
| [draw_driver.h](ogc/draw/draw_driver.h) | 绘图驱动 | `DrawDriver` |
| [draw_facade.h](ogc/draw/draw_facade.h) | 绘图门面 | `DrawFacade` |

### 样式系统

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [draw_style.h](ogc/draw/draw_style.h) | 绘图样式 | `DrawStyle`, `StrokeStyle`, `FillStyle` |
| [color.h](ogc/draw/color.h) | 颜色类 | `Color` |
| [font.h](ogc/draw/font.h) | 字体类 | `Font`, `FontStyle`, `FontWeight` |
| [symbolizer.h](ogc/draw/symbolizer.h) | 符号化基类 | `Symbolizer`, `SymbolizerType` |
| [line_symbolizer.h](ogc/draw/line_symbolizer.h) | 线符号化 | `LineSymbolizer` |
| [polygon_symbolizer.h](ogc/draw/polygon_symbolizer.h) | 面符号化 | `PolygonSymbolizer` |
| [point_symbolizer.h](ogc/draw/point_symbolizer.h) | 点符号化 | `PointSymbolizer` |
| [text_symbolizer.h](ogc/draw/text_symbolizer.h) | 文字符号化 | `TextSymbolizer` |
| [raster_symbolizer.h](ogc/draw/raster_symbolizer.h) | 栅格符号化 | `RasterSymbolizer` |
| [icon_symbolizer.h](ogc/draw/icon_symbolizer.h) | 图标符号化 | `IconSymbolizer` |
| [composite_symbolizer.h](ogc/draw/composite_symbolizer.h) | 组合符号化 | `CompositeSymbolizer` |

### 瓦片系统

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [tile_key.h](ogc/draw/tile_key.h) | 瓦片索引键 | `TileKey`, `TilePyramid` |
| [tile_cache.h](ogc/draw/tile_cache.h) | 瓦片缓存接口 | `TileCache`, `TileData` |
| [memory_tile_cache.h](ogc/draw/memory_tile_cache.h) | 内存瓦片缓存 | `MemoryTileCache` |
| [disk_tile_cache.h](ogc/draw/disk_tile_cache.h) | 磁盘瓦片缓存 | `DiskTileCache` |
| [multi_level_tile_cache.h](ogc/draw/multi_level_tile_cache.h) | 多级瓦片缓存 | `MultiLevelTileCache` |
| [tile_renderer.h](ogc/draw/tile_renderer.h) | 瓦片渲染器 | `TileRenderer` |

### 规则与过滤

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [filter.h](ogc/draw/filter.h) | 过滤器基类 | `Filter` |
| [comparison_filter.h](ogc/draw/comparison_filter.h) | 比较过滤器 | `ComparisonFilter` |
| [logical_filter.h](ogc/draw/logical_filter.h) | 逻辑过滤器 | `LogicalFilter` |
| [spatial_filter.h](ogc/draw/spatial_filter.h) | 空间过滤器 | `SpatialFilter` |
| [rule_engine.h](ogc/draw/rule_engine.h) | 规则引擎 | `RuleEngine` |
| [symbolizer_rule.h](ogc/draw/symbolizer_rule.h) | 符号化规则 | `SymbolizerRule` |

### 样式解析

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [sld_parser.h](ogc/draw/sld_parser.h) | SLD样式解析器 | `SldParser` |
| [mapbox_style_parser.h](ogc/draw/mapbox_style_parser.h) | Mapbox样式解析器 | `MapboxStyleParser` |

### 标注系统

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [label_placement.h](ogc/draw/label_placement.h) | 标注位置 | `LabelPlacement` |
| [label_conflict.h](ogc/draw/label_conflict.h) | 标注冲突检测 | `LabelConflict` |
| [label_engine.h](ogc/draw/label_engine.h) | 标注引擎 | `LabelEngine` |

### 坐标转换

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [transform_matrix.h](ogc/draw/transform_matrix.h) | 变换矩阵 | `TransformMatrix` |
| [transform_manager.h](ogc/draw/transform_manager.h) | 变换管理器 | `TransformManager` |
| [coordinate_transform.h](ogc/draw/coordinate_transform.h) | 坐标转换 | `CoordinateTransform` |
| [coordinate_transformer.h](ogc/draw/coordinate_transformer.h) | 坐标转换器 | `CoordinateTransformer` |
| [proj_transformer.h](ogc/draw/proj_transformer.h) | 投影转换 | `ProjTransformer` |

### 渲染任务

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [render_task.h](ogc/draw/render_task.h) | 渲染任务 | `RenderTask` |
| [basic_render_task.h](ogc/draw/basic_render_task.h) | 基础渲染任务 | `BasicRenderTask` |
| [render_queue.h](ogc/draw/render_queue.h) | 渲染队列 | `RenderQueue` |
| [async_renderer.h](ogc/draw/async_renderer.h) | 异步渲染器 | `AsyncRenderer` |

### 辅助工具

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [clipper.h](ogc/draw/clipper.h) | 多边形裁剪 | `Clipper` |
| [image_draw.h](ogc/draw/image_draw.h) | 图像绘制工具 | `ImageDraw` |
| [layer_manager.h](ogc/draw/layer_manager.h) | 图层管理器 | `LayerManager` |
| [lod.h](ogc/draw/lod.h) | 细节层次 | `Lod` |
| [lod_manager.h](ogc/draw/lod_manager.h) | LOD管理器 | `LodManager` |

### 性能与监控

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [performance_metrics.h](ogc/draw/performance_metrics.h) | 性能指标 | `PerformanceMetrics` |
| [performance_monitor.h](ogc/draw/performance_monitor.h) | 性能监控 | `PerformanceMonitor` |

### 类型与枚举

| 文件 | 描述 | 核心类型 |
|------|------|----------|
| [types.h](ogc/draw/types.h) | 类型定义 | `DrawDevicePtr`, `DrawEnginePtr`等 |
| [device_type.h](ogc/draw/device_type.h) | 设备类型枚举 | `DeviceType` |
| [engine_type.h](ogc/draw/engine_type.h) | 引擎类型枚举 | `EngineType` |
| [device_state.h](ogc/draw/device_state.h) | 设备状态枚举 | `DeviceState` |
| [draw_result.h](ogc/draw/draw_result.h) | 绘图结果枚举 | `DrawResult` |
| [draw_error.h](ogc/draw/draw_error.h) | 绘图错误类 | `DrawError` |
| [draw_params.h](ogc/draw/draw_params.h) | 绘图参数 | `DrawParams` |

### 其他

| 文件 | 描述 | 核心类 |
|------|------|--------|
| [driver_manager.h](ogc/draw/driver_manager.h) | 驱动管理器 | `DriverManager` |
| [thread_safe.h](ogc/draw/thread_safe.h) | 线程安全工具 | `ThreadSafe` |
| [log.h](ogc/draw/log.h) | 日志工具 | `Log` |

---

## 类继承关系

```
DrawDevice (抽象基类)
    ├── RasterImageDevice    - 光栅图像设备
    ├── TileDevice           - 瓦片设备
    ├── PdfDevice            - PDF设备
    └── DisplayDevice        - 显示设备

DrawEngine (抽象基类)
    └── Simple2DEngine       - 简单2D引擎

Symbolizer (抽象基类)
    ├── PointSymbolizer      - 点符号化
    ├── LineSymbolizer       - 线符号化
    ├── PolygonSymbolizer    - 面符号化
    ├── TextSymbolizer       - 文字符号化
    ├── RasterSymbolizer     - 栅格符号化
    ├── IconSymbolizer       - 图标符号化
    └── CompositeSymbolizer  - 组合符号化

TileCache (抽象基类)
    ├── MemoryTileCache      - 内存缓存
    ├── DiskTileCache        - 磁盘缓存
    └── MultiLevelTileCache  - 多级缓存

Filter (抽象基类)
    ├── ComparisonFilter     - 比较过滤器
    ├── LogicalFilter        - 逻辑过滤器
    └── SpatialFilter        - 空间过滤器
```

---

## 核心类详解

### DrawDevice
绘图设备抽象基类，定义了所有绘图设备必须实现的接口。

**关键方法**:
- `Initialize()` / `Finalize()` - 设备初始化/释放
- `BeginDraw()` / `EndDraw()` - 开始/结束绘图
- `DrawPoint()`, `DrawLine()`, `DrawRect()`, `DrawCircle()` - 基本图形绘制
- `DrawGeometry()` - 几何对象绘制
- `DrawText()` - 文字绘制
- `DrawImage()` - 图像绘制
- `SetTransform()` / `GetTransform()` - 变换矩阵

### DrawEngine
绘图引擎抽象基类，负责协调绘图设备和绘图上下文。

**关键方法**:
- `SetDevice()` - 设置绘图设备
- `BeginDraw()` / `EndDraw()` - 开始/结束绘图
- `DrawGeometry()` - 几何对象渲染
- `WorldToScreen()` / `ScreenToWorld()` - 坐标转换

### DrawContext
绘图上下文，管理绘图状态栈（变换、样式、透明度等）。

**关键方法**:
- `PushTransform()` / `PopTransform()` - 变换栈管理
- `PushStyle()` / `PopStyle()` - 样式栈管理
- `PushClipRect()` / `PopClipRect()` - 裁剪栈管理
- `DrawGeometry()` - 几何对象绘制

### TileKey
瓦片索引键，用于唯一标识一个瓦片。

**成员变量**:
- `x`, `y`, `z` - 瓦片坐标和层级

**关键方法**:
- `ToString()` / `FromString()` - 字符串转换
- `ToIndex()` / `FromIndex()` - 索引转换
- `GetParent()` / `GetChild()` - 父子瓦片获取
- `IsValid()` - 有效性检查

### Symbolizer
符号化基类，定义了几何对象到可视化符号的转换规则。

**关键方法**:
- `Symbolize()` - 执行符号化渲染
- `CanSymbolize()` - 检查是否支持指定几何类型
- `SetDefaultStyle()` / `GetDefaultStyle()` - 默认样式管理

---

## 快速使用示例

### 创建光栅图像并绘制几何

```cpp
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/point.h"

using namespace ogc;
using namespace ogc::draw;

void DrawExample() {
    auto device = RasterImageDevice::Create(800, 600, 4);
    device->Initialize();
    
    DrawParams params;
    params.pixel_width = 800;
    params.pixel_height = 600;
    params.extent = Envelope(0, 0, 800, 600);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.stroke.color = Color::Red().GetRGBA();
    style.stroke.width = 2.0;
    style.fill.color = Color::Blue().GetRGBA();
    
    device->DrawRect(100, 100, 200, 150, style);
    device->DrawCircle(400, 300, 50, style);
    
    device->EndDraw();
    device->SaveToFile("output.png", ImageFormat::kPNG);
    device->Finalize();
}
```

### 使用瓦片缓存

```cpp
#include "ogc/draw/tile_cache.h"
#include "ogc/draw/memory_tile_cache.h"

using namespace ogc;
using namespace ogc::draw;

void TileCacheExample() {
    auto cache = TileCache::CreateMemoryCache(100 * 1024 * 1024);
    
    TileKey key(1, 2, 3);
    std::vector<uint8_t> tileData(1024, 0);
    
    cache->Put(key, tileData);
    
    if (cache->HasTile(key)) {
        TileData data = cache->GetTile(key);
    }
}
```

---

## 依赖关系

```
graph模块依赖:
├── ogc_geometry  - 几何对象定义
├── ogc_common    - 公共类型定义
└── 外部库
    ├── GTest      - 单元测试框架
    └── (可选) PROJ - 投影转换库
```

---

## 更新记录

| 日期 | 更新内容 | 更新人 |
|------|----------|--------|
| 2026-03-22 | 创建索引文件 | AI |
