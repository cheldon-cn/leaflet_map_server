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
| **geom** | 几何对象模型，OGC SFAS 1.2.1实现 | [index_geom.md](geom/include/index_geom.md) | `Geometry`, `Point`, `LineString`, `Polygon` |
| **database** | 数据库抽象层，连接池管理 | [index_database.md](database/include/index_database.md) | `DbConnection`, `DbConnectionPool`, `DbResultSet` |
| **feature** | 要素模型，属性字段管理 | [index_feature.md](feature/include/index_feature.md) | `CNFeature`, `CNFeatureDefn`, `CNFieldValue` |
| **layer** | 图层抽象，多数据源支持 | [index_layer.md](layer/include/index_layer.md) | `CNLayer`, `CNVectorLayer`, `CNDataSource` |
| **draw** | 绘图引擎，多平台渲染 | [index_draw.md](draw/include/index_draw.md) | `DrawEngine`, `DrawDevice`, `DrawContext` |
| **graph** | 地图渲染，符号化引擎 | [index_graph.md](graph/include/index_graph.md) | `DrawFacade`, `Symbolizer`, `LabelEngine` |
| **alarm** | 警报服务，REST/WebSocket | - | `Alert`, `AlertEngine`, `AlertRepository` |
| **alert** | 航海警报系统 | [index_alert.md](alert/include/index_alert.md) | `AlertEngine`, `IAlertChecker`, `CpaCalculator` |
| **navi** | 航海导航系统 | [index_navi.md](navi/include/index_navi.md) | `Route`, `AisManager`, `NavigationEngine` |
| **parser** | 海图解析器 | [chart/parser/include/index_parser.md](chart/parser/include/index_parser.md) | `ChartParser`, `S57Parser`, `IncrementalParser` |

---

## 核心类快速查找

### 几何类 (geom)

| Class | Description | File |
|-------|-------------|------|
| `Geometry` | 几何对象基类 | [geometry.h](geom/include/ogc/geometry.h) |
| `Point` | 点几何 | [point.h](geom/include/ogc/point.h) |
| `LineString` | 线几何 | [linestring.h](geom/include/ogc/linestring.h) |
| `Polygon` | 面几何 | [polygon.h](geom/include/ogc/polygon.h) |
| `MultiPoint` | 多点 | [multipoint.h](geom/include/ogc/multipoint.h) |
| `MultiLineString` | 多线 | [multilinestring.h](geom/include/ogc/multilinestring.h) |
| `MultiPolygon` | 多面 | [multipolygon.h](geom/include/ogc/multipolygon.h) |
| `GeometryCollection` | 几何集合 | [geometrycollection.h](geom/include/ogc/geometrycollection.h) |
| `Envelope` | 包络矩形 | [envelope.h](geom/include/ogc/envelope.h) |
| `Coordinate` | 坐标结构 | [coordinate.h](geom/include/ogc/coordinate.h) |

### 数据库类 (database)

| Class | Description | File |
|-------|-------------|------|
| `DbConnection` | 数据库连接接口 | [connection.h](database/include/ogc/db/connection.h) |
| `DbConnectionPool` | 连接池 | [connection_pool.h](database/include/ogc/db/connection_pool.h) |
| `DbResultSet` | 结果集 | [resultset.h](database/include/ogc/db/resultset.h) |
| `PostGISConnection` | PostGIS连接 | [postgis_connection.h](database/include/ogc/db/postgis_connection.h) |
| `SpatiaLiteConnection` | SpatiaLite连接 | [sqlite_connection.h](database/include/ogc/db/sqlite_connection.h) |

### 要素类 (feature)

| Class | Description | File |
|-------|-------------|------|
| `CNFeature` | 要素类 | [feature.h](feature/include/ogc/feature/feature.h) |
| `CNFeatureDefn` | 要素定义 | [feature_defn.h](feature/include/ogc/feature/feature_defn.h) |
| `CNFieldValue` | 字段值 | [field_value.h](feature/include/ogc/feature/field_value.h) |
| `CNFieldDefn` | 字段定义 | [field_defn.h](feature/include/ogc/feature/field_defn.h) |

### 图层类

| Class | Description | File |
|-------|-------------|------|
| `CNLayer` | 图层基类 | [layer.h](layer/include/ogc/layer/layer.h) |
| `CNVectorLayer` | 矢量图层 | [vector_layer.h](layer/include/ogc/layer/vector_layer.h) |
| `CNDataSource` | 数据源 | [datasource.h](layer/include/ogc/layer/datasource.h) |
| `CNLayerGroup` | 图层组 | [layer_group.h](layer/include/ogc/layer/layer_group.h) |

### 绘图类 (draw)

| Class | Description | File |
|-------|-------------|------|
| `DrawEngine` | 绘图引擎基类 | [draw_engine.h](draw/include/ogc/draw/draw_engine.h) |
| `DrawDevice` | 绘图设备基类 | [draw_device.h](draw/include/ogc/draw/draw_device.h) |
| `DrawContext` | 绘图上下文 | [draw_context.h](draw/include/ogc/draw/draw_context.h) |
| `Color` | 颜色类 | [color.h](draw/include/ogc/draw/color.h) |

### 渲染类 (graph)

| Class | Description | File |
|-------|-------------|------|
| `DrawFacade` | 绘图门面 | [draw_facade.h](graph/include/ogc/draw/draw_facade.h) |
| `Symbolizer` | 符号化器基类 | [symbolizer.h](graph/include/ogc/draw/symbolizer.h) |
| `LabelEngine` | 标签引擎 | [label_engine.h](graph/include/ogc/draw/label_engine.h) |
| `TileCache` | 瓦片缓存 | [tile_cache.h](graph/include/ogc/draw/tile_cache.h) |

### 警报类 (alert)

| Class | Description | File |
|-------|-------------|------|
| `AlertEngine` | 警报引擎 | [alert_engine.h](alert/include/ogc/alert/alert_engine.h) |
| `IAlertChecker` | 警报检查器接口 | [alert_checker.h](alert/include/ogc/alert/alert_checker.h) |
| `CpaCalculator` | CPA计算器 | [cpa_calculator.h](alert/include/ogc/alert/cpa_calculator.h) |
| `UkcCalculator` | UKC计算器 | [ukc_calculator.h](alert/include/ogc/alert/ukc_calculator.h) |

### 导航类 (navi)

| Class | Description | File |
|-------|-------------|------|
| `Route` | 航线 | [route.h](navi/include/ogc/navi/route/route.h) |
| `AisManager` | AIS管理器 | [ais_manager.h](navi/include/ogc/navi/ais/ais_manager.h) |
| `NavigationEngine` | 导航引擎 | [navigation_engine.h](navi/include/ogc/navi/navigation/navigation_engine.h) |
| `PositionManager` | 定位管理器 | [position_manager.h](navi/include/ogc/navi/positioning/position_manager.h) |

### 解析类 (parser)

| Class | Description | File |
|-------|-------------|------|
| `ChartParser` | 解析器门面 | [chart_parser.h](chart/parser/include/chart_parser/chart_parser.h) |
| `S57Parser` | S-57解析器 | [s57_parser.h](chart/parser/include/chart_parser/s57_parser.h) |
| `IncrementalParser` | 增量解析器 | [incremental_parser.h](chart/parser/include/chart_parser/incremental_parser.h) |

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
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│     alert     │     │    graph      │     │     draw      │
│  (警报系统)   │     │  (地图渲染)   │     │  (绘图引擎)   │
└───────────────┘     └───────────────┘     └───────────────┘
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
│   database    │     │     geom      │     │   (其他)      │
│  (数据库)     │     │   (几何)      │     │               │
└───────────────┘     └───────────────┘     └───────────────┘
```

---

## 命名空间结构

```cpp
ogc                          // 顶层命名空间
├── ogc::geom               // 几何模块
├── ogc::db                 // 数据库模块
├── ogc::feature            // 要素模块
├── ogc::layer              // 图层模块
├── ogc::draw               // 绘图模块
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

**快速跳转**: [geom](geom/include/index_geom.md) | [database](database/include/index_database.md) | [feature](feature/include/index_feature.md) | [layer](layer/include/index_layer.md) | [draw](draw/include/index_draw.md) | [graph](graph/include/index_graph.md) | [alarm](alarm/) | [alert](alert/include/index_alert.md) | [navi](navi/include/index_navi.md) | [parser](chart/parser/include/index_parser.md)
