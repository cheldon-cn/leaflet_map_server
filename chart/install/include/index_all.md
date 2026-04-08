# OGC Chart Project - 总索引文件

> **快速导航**: 本索引文件提供所有模块的统一入口，帮助快速定位所需模块、类、接口和文件。

---

## 项目概述

OGC Chart是一个基于C++11的海图信息系统，遵循OGC Simple Feature Access规范，提供完整的几何对象模型、空间操作、数据存储、地图渲染、航海导航和警报系统功能。

**技术栈**: C++11 | libpq | sqlite3 | libspatialite | GDAL/OGR

---

## 模块索引

| Module | Description | Index File | Key Classes |
|--------|-------------|------------|-------------|
| **base** | 基础工具库，日志、线程安全、性能监控 | [index_base.md](index_base.md) | `Logger`, `ThreadSafe<T>`, `PerformanceMonitor` |
| **proj** | 坐标转换库，PROJ库集成 | [index_proj.md](index_proj.md) | `CoordinateTransformer`, `ProjTransformer`, `CoordSystemPreset` |
| **cache** | 缓存库，瓦片缓存、离线存储 | [index_cache.md](index_cache.md) | `TileCache`, `TileKey`, `OfflineStorageManager` |
| **symbology** | 符号化库，样式解析、过滤规则 | [index_symbology.md](index_symbology.md) | `Symbolizer`, `Filter`, `SymbolizerRule` |
| **geom** | 几何对象模型，OGC SFAS 1.2.1实现 | [index_geom.md](index_geom.md) | `Geometry`, `Point`, `LineString`, `Polygon` |
| **database** | 数据库抽象层，连接池管理 | [index_database.md](index_database.md) | `DbConnection`, `DbConnectionPool`, `DbResultSet` |
| **feature** | 要素模型，属性字段管理 | [index_feature.md](index_feature.md) | `CNFeature`, `CNFeatureDefn`, `CNFieldValue` |
| **layer** | 图层抽象，多数据源支持 | [index_layer.md](index_layer.md) | `CNLayer`, `CNVectorLayer`, `CNDataSource` |
| **draw** | 绘图引擎，多平台渲染 | [index_draw.md](index_draw.md) | `DrawEngine`, `DrawDevice`, `DrawContext` |
| **graph** | 地图渲染核心，图层管理、标签引擎 | [index_graph.md](index_graph.md) | `DrawFacade`, `RenderTask`, `LayerManager`, `LabelEngine` |
| **alarm** | 警报服务，REST/WebSocket | - | `Alert`, `AlertEngine`, `AlertRepository` |
| **alert** | 航海警报系统 | [index_alert.md](index_alert.md) | `AlertEngine`, `IAlertChecker`, `CpaCalculator` |
| **navi** | 航海导航系统 | [index_navi.md](index_navi.md) | `Route`, `AisManager`, `NavigationEngine` |
| **parser** | 海图解析器 | - | `ChartParser`, `S57Parser`, `IncrementalParser` |

---

## 核心类快速查找

### 基础工具类 (base)

| Class | Description | File |
|-------|-------------|------|
| `Logger` | 日志管理器 | [log.h](ogc/base/log.h) |
| `ThreadSafe<T>` | 线程安全封装 | [thread_safe.h](ogc/base/thread_safe.h) |
| `PerformanceMonitor` | 性能监控 | [performance_monitor.h](ogc/base/performance_monitor.h) |

### 坐标转换类 (proj)

| Class | Description | File |
|-------|-------------|------|
| `CoordinateTransformer` | 坐标转换器接口 | [coordinate_transformer.h](ogc/proj/coordinate_transformer.h) |
| `ProjTransformer` | PROJ转换器 | [proj_transformer.h](ogc/proj/proj_transformer.h) |
| `CoordSystemPreset` | 坐标系预设 | [coord_system_preset.h](ogc/proj/coord_system_preset.h) |
| `TransformMatrix` | 变换矩阵 | [transform_matrix.h](ogc/proj/transform_matrix.h) |

### 缓存类 (cache)

| Class | Description | File |
|-------|-------------|------|
| `TileKey` | 瓦片键 | [tile_key.h](ogc/cache/tile/tile_key.h) |
| `TileCache` | 瓦片缓存基类 | [tile_cache.h](ogc/cache/tile/tile_cache.h) |
| `MemoryTileCache` | 内存缓存 | [memory_tile_cache.h](ogc/cache/tile/memory_tile_cache.h) |
| `DiskTileCache` | 磁盘缓存 | [disk_tile_cache.h](ogc/cache/tile/disk_tile_cache.h) |
| `OfflineStorageManager` | 离线存储管理 | [offline_storage_manager.h](ogc/cache/offline/offline_storage_manager.h) |

### 符号化类 (symbology)

| Class | Description | File |
|-------|-------------|------|
| `Symbolizer` | 符号化器基类 | [symbolizer.h](ogc/symbology/symbolizer/symbolizer.h) |
| `Filter` | 过滤器基类 | [filter.h](ogc/symbology/filter/filter.h) |
| `SymbolizerRule` | 符号化规则 | [symbolizer_rule.h](ogc/symbology/filter/symbolizer_rule.h) |
| `S52StyleManager` | S52海图样式管理 | [s52_style_manager.h](ogc/symbology/style/s52_style_manager.h) |

### 几何类 (geom)

| Class | Description | File |
|-------|-------------|------|
| `Geometry` | 几何对象基类 | [geometry.h](ogc/geom/geometry.h) |
| `Point` | 点几何 | [point.h](ogc/geom/point.h) |
| `LineString` | 线几何 | [linestring.h](ogc/geom/linestring.h) |
| `LinearRing` | 线环 | [linearring.h](ogc/geom/linearring.h) |
| `Polygon` | 面几何 | [polygon.h](ogc/geom/polygon.h) |
| `MultiPoint` | 多点 | [multipoint.h](ogc/geom/multipoint.h) |
| `MultiLineString` | 多线 | [multilinestring.h](ogc/geom/multilinestring.h) |
| `MultiPolygon` | 多面 | [multipolygon.h](ogc/geom/multipolygon.h) |
| `GeometryCollection` | 几何集合 | [geometrycollection.h](ogc/geom/geometrycollection.h) |
| `Envelope` | 包络矩形 | [envelope.h](ogc/geom/envelope.h) |
| `Envelope3D` | 三维包络 | [envelope3d.h](ogc/geom/envelope3d.h) |
| `Coordinate` | 坐标结构 | [coordinate.h](ogc/geom/coordinate.h) |
| `GeometryFactory` | 几何工厂 | [factory.h](ogc/geom/factory.h) |
| `GeometryPool` | 几何对象池 | [geometry_pool.h](ogc/geom/geometry_pool.h) |
| `PrecisionModel` | 精度模型 | [precision.h](ogc/geom/precision.h) |

### 数据库类 (database)

| Class | Description | File |
|-------|-------------|------|
| `DbConnection` | 数据库连接接口 | [connection.h](ogc/db/connection.h) |
| `DbConnectionPool` | 连接池 | [connection_pool.h](ogc/db/connection_pool.h) |
| `DbResultSet` | 结果集 | [resultset.h](ogc/db/resultset.h) |
| `PostGISConnection` | PostGIS连接 | [postgis_connection.h](ogc/db/postgis_connection.h) |
| `SpatiaLiteConnection` | SpatiaLite连接 | [sqlite_connection.h](ogc/db/sqlite_connection.h) |

### 要素类 (feature)

| Class | Description | File |
|-------|-------------|------|
| `CNFeature` | 要素类 | [feature.h](ogc/feature/feature.h) |
| `CNFeatureDefn` | 要素定义 | [feature_defn.h](ogc/feature/feature_defn.h) |
| `CNFieldValue` | 字段值 | [field_value.h](ogc/feature/field_value.h) |
| `CNFieldDefn` | 字段定义 | [field_defn.h](ogc/feature/field_defn.h) |

### 图层类

| Class | Description | File |
|-------|-------------|------|
| `CNLayer` | 图层基类 | [layer.h](ogc/layer/layer.h) |
| `CNVectorLayer` | 矢量图层 | [vector_layer.h](ogc/layer/vector_layer.h) |
| `CNDataSource` | 数据源 | [datasource.h](ogc/layer/datasource.h) |
| `CNLayerGroup` | 图层组 | [layer_group.h](ogc/layer/layer_group.h) |

### 绘图类 (draw)

| Class | Description | File |
|-------|-------------|------|
| `DrawEngine` | 绘图引擎基类 | [draw_engine.h](ogc/draw/draw_engine.h) |
| `DrawDevice` | 绘图设备基类 | [draw_device.h](ogc/draw/draw_device.h) |
| `DrawContext` | 绘图上下文 | [draw_context.h](ogc/draw/draw_context.h) |
| `Color` | 颜色类 | [color.h](ogc/draw/color.h) |

### 渲染类 (graph)

| Class | Description | File |
|-------|-------------|------|
| `DrawFacade` | 绘图门面 | [draw_facade.h](ogc/graph/draw_facade.h) |
| `Symbolizer` | 符号化器基类 | [symbolizer.h](ogc/symbology/symbolizer/symbolizer.h) |
| `LabelEngine` | 标签引擎 | [label_engine.h](ogc/graph/label_engine.h) |
| `TileCache` | 瓦片缓存 | [tile_cache.h](ogc/cache/tile/tile_cache.h) |

### 警报类 (alert)

| Class | Description | File |
|-------|-------------|------|
| `AlertEngine` | 警报引擎 | [alert_engine.h](ogc/alert/alert_engine.h) |
| `IAlertChecker` | 警报检查器接口 | [alert_checker.h](ogc/alert/alert_checker.h) |
| `CpaCalculator` | CPA计算器 | [cpa_calculator.h](ogc/alert/cpa_calculator.h) |
| `UkcCalculator` | UKC计算器 | [ukc_calculator.h](ogc/alert/ukc_calculator.h) |

### 导航类 (navi)

| Class | Description | File |
|-------|-------------|------|
| `Route` | 航线 | [route.h](ogc/navi/route/route.h) |
| `AisManager` | AIS管理器 | [ais_manager.h](ogc/navi/ais/ais_manager.h) |
| `NavigationEngine` | 导航引擎 | [navigation_engine.h](ogc/navi/navigation/navigation_engine.h) |
| `PositionManager` | 定位管理器 | [position_manager.h](ogc/navi/positioning/position_manager.h) |

### 解析类 (parser)

| Class | Description | File |
|-------|-------------|------|
| `ChartParser` | 解析器门面 | - |
| `S57Parser` | S-57解析器 | - |
| `IncrementalParser` | 增量解析器 | - |

---

## 接口快速查找

| Interface | Module | Description |
|-----------|--------|-------------|
| `IGeometryVisitor` | geom | 几何访问者接口 |
| `ISpatialIndex` | geom | 空间索引接口 |
| `IDbConnection` | database | 数据库连接接口 |
| `IAlertChecker` | alert | 警报检查器接口 |
| `IParser` | parser | 解析器接口 |
| `IPositionProvider` | navi | 定位源接口 |

---

## 枚举类型快速查找

| Enum | Module | Description |
|------|--------|-------------|
| `GeomType` | geom | 几何类型 |
| `Dimension` | geom | 维度 |
| `CNLayerType` | layer | 图层类型 |
| `CNStatus` | layer | 状态码 |
| `AlertType` | alert | 警报类型 |
| `AlertLevel` | alert | 警报等级 |
| `NavigationStatus` | navi | 导航状态 |
| `RouteStatus` | navi | 航线状态 |
| `ChartFormat` | parser | 海图格式 |
| `ErrorCode` | parser | 错误码 |

---

## 模块依赖关系

```
┌─────────────────────────────────────────────────────────────┐
│                        Application                          │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│     alarm     │     │     navi      │     │    parser     │
│  (警报服务)   │     │  (导航系统)   │     │  (海图解析)   │
└───────────────┘     └───────────────┘     └───────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    graph (地图渲染核心)                     │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│  symbology    │     │    cache      │     │     draw      │
│  (符号化库)   │     │  (缓存库)     │     │  (绘图引擎)   │
└───────────────┘     └───────────────┘     └───────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                       layer (图层)                          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      feature (要素)                         │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│   database    │     │     geom      │     │     proj      │
│  (数据库)     │     │   (几何)      │     │  (坐标转换)   │
└───────────────┘     └───────────────┘     └───────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                       base (基础工具)                       │
└─────────────────────────────────────────────────────────────┘
```

---

## 命名空间结构

```cpp
ogc                          // 顶层命名空间
├── ogc::base               // 基础工具模块
├── ogc::proj               // 坐标转换模块
├── ogc::cache              // 缓存模块
├── ogc::symbology          // 符号化模块
├── ogc::geom               // 几何模块
├── ogc::db                 // 数据库模块
├── ogc::feature            // 要素模块
├── ogc::layer              // 图层模块
├── ogc::draw               // 绘图模块
├── ogc::graph              // 地图渲染模块
├── ogc::alert              // 警报模块
└── ogc::navi               // 导航模块
    ├── ogc::navi::ais      // AIS子模块
    ├── ogc::navi::route    // 航线子模块
    ├── ogc::navi::track    // 航迹子模块
    └── ogc::navi::positioning // 定位子模块

chart::parser               // 解析器命名空间
alert                       // alarm模块命名空间 (独立服务)
```

---

## 常用操作示例

### 创建几何对象

```cpp
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"

// 创建点
Point* pt = Point::Create(121.5, 31.2);

// 创建线
LineString* line = LineString::Create();
line->AddPoint(121.5, 31.2);
line->AddPoint(121.6, 31.3);
```

### 打开数据源

```cpp
#include "ogc/layer/datasource.h"

auto ds = CNDataSource::Open("/path/to/data.shp");
CNLayer* layer = ds->GetLayer(0);
layer->ResetReading();
while (auto feature = layer->GetNextFeature()) {
    // 处理要素
}
```

### 渲染几何

```cpp
#include "ogc/draw/draw_facade.h"

auto& facade = DrawFacade::Instance();
facade.Initialize();

auto device = facade.CreateDevice(DeviceType::kRaster, 800, 600);
auto context = facade.CreateContext(device);

DrawStyle style;
style.strokeColor = Color(255, 0, 0);
facade.DrawGeometry(context, geometry, style);
```

### 解析海图

```cpp
#include "chart_parser/chart_parser.h"

auto& parser = ChartParser::Instance();
parser.Initialize();

IParser* s57 = parser.CreateParser(ChartFormat::S57);
s57->Open("/path/to/chart.000");
ParseResult result = s57->Parse();
```

---

## 文件路径约定

| 类型 | 路径模式 | 示例 |
|------|----------|------|
| 头文件 | `code/{module}/include/ogc/{subdir}/*.h` | `code/geom/include/ogc/geometry.h` |
| 源文件 | `code/{module}/src/*.cpp` | `code/geom/src/geometry.cpp` |
| 测试文件 | `code/{module}/tests/test_*.cpp` | `code/geom/tests/test_geometry.cpp` |
| 索引文件 | `code/{module}/include/index_{module}.md` | `code/geom/include/index_geom.md` |

---

## 版本信息

| 属性 | 值 |
|------|-----|
| C++标准 | C++11 |
| 项目版本 | v1.2 |
| 生成日期 | 2026-04-06 |
| 代码规范 | Google C++ Style |

---

**快速跳转**: [geom](index_geom.md) | [database](index_database.md) | [feature](index_feature.md) | [layer](index_layer.md) | [draw](index_draw.md) | [graph](index_graph.md) | [alert](index_alert.md) | [navi](index_navi.md)
