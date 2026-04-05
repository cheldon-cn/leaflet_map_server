# 模拟数据生成与渲染测试需求文档

**版本**: 1.3  
**日期**: 2026年3月27日  
**状态**: 设计中  
**C++标准**: C++11  

---

## 文档修订历史

| 版本 | 日期 | 修改内容 | 评审意见 |
|------|------|----------|----------|
| 1.0 | 2026-03-27 | 初始版本创建 | - |
| 1.1 | 2026-03-27 | 补充错误处理、风险识别、渲染上下文、性能优化等章节 | 单角色评审得分96 |
| 1.2 | 2026-03-27 | 补充几何有效性验证、数据库兼容性、并发控制、性能监控、内存检测等章节 | 多角色评审得分99 |
| 1.3 | 2026-03-27 | 补充编码实施约束规则，关联compile_test_problem_summary.md避坑经验 | - |

---

## 目录

1. [需求概述](#1-需求概述)
2. [系统架构](#2-系统架构)
3. [模拟数据生成](#3-模拟数据生成)
4. [数据存储设计](#4-数据存储设计)
5. [空间查询与可见性过滤](#5-空间查询与可见性过滤)
6. [符号化渲染](#6-符号化渲染)
7. [输出设备支持](#7-输出设备支持)
8. [测试用例设计](#8-测试用例设计)
9. [性能指标](#9-性能指标)
10. [错误处理机制](#10-错误处理机制)
11. [渲染上下文管理](#11-渲染上下文管理)
12. [性能优化策略](#12-性能优化策略)
13. [坐标系处理策略](#13-坐标系处理策略)
14. [安全要求](#14-安全要求)
15. [风险与约束](#15-风险与约束)
16. [实现计划](#16-实现计划)
17. [扩展指南](#17-扩展指南)
18. [几何有效性验证规则](#18-几何有效性验证规则)
19. [数据库兼容性与并发控制](#19-数据库兼容性与并发控制)
20. [性能监控与内存检测](#20-性能监控与内存检测)

---

## 1. 需求概述

### 1.1 需求背景

为验证OGC几何模型、要素模型、图层模型、绘图引擎各模块的集成效果，需要构建一套完整的模拟数据生成与渲染测试系统。该系统应能够：

1. 基于现有模块定义的类和接口，生成覆盖所有属性类型的模拟数据
2. 将模拟数据持久化存储至SQLite数据库
3. 根据显示范围进行空间过滤，获取可见数据
4. 对可见数据进行符号化渲染，输出至多种显示设备

### 1.2 需求目标

- **数据覆盖性**：模拟数据应覆盖点、线、区、注记、栅格五种类型，尽可能覆盖各类型的所有属性
- **数据量控制**：总数据量控制在100条左右，各类型数量大体一致（约20条/类型）
- **存储持久化**：模拟数据存储至 `sqlite_demo.db` 数据库文件
- **空间过滤**：支持基于显示范围的空间查询，获取各图层的可见数据
- **多设备输出**：支持 RasterImageDevice、PdfDevice、DisplayDevice 三种输出设备

---

## 2. 系统架构

### 2.1 模块依赖关系

```
┌─────────────────────────────────────────────────────────────────┐
│                        测试应用程序                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ 模拟数据生成器 │  │ 空间查询引擎  │  │  渲染引擎    │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                 │                 │                   │
├─────────┼─────────────────┼─────────────────┼───────────────────┤
│         ▼                 ▼                 ▼                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │  Feature模块  │  │  Layer模块   │  │  Graph模块   │          │
│  │ (CNFeature)  │  │ (CNLayer)    │  │ (Symbolizer) │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                 │                 │                   │
│         ▼                 ▼                 ▼                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │  Geom模块    │  │ Database模块 │  │ DrawDevice   │          │
│  │ (Geometry)   │  │ (SQLite)     │  │ (Raster/PDF) │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 数据流程

```
模拟数据生成 ──▶ 要素构建 ──▶ 图层组织 ──▶ SQLite存储
                                              │
                                              ▼
显示范围设置 ──▶ 空间过滤 ◀── 图层读取 ◀── 数据库连接
                    │
                    ▼
              可见要素集合
                    │
                    ▼
              符号化渲染
                    │
        ┌───────────┼───────────┐
        ▼           ▼           ▼
  RasterDevice  PdfDevice  DisplayDevice
```

---

## 3. 模拟数据生成

### 3.1 数据类型与数量规划

| 数据类型 | 数量 | 图层名称 | 几何类型 | 说明 |
|---------|------|---------|---------|------|
| 点要素 | 20 | point_layer | Point/MultiPoint | 包含2D/3D点、带测量值点 |
| 线要素 | 20 | line_layer | LineString/MultiLineString | 包含简单线、复杂线、闭合线 |
| 区要素 | 20 | polygon_layer | Polygon/MultiPolygon | 包含简单区、带洞区、复杂区 |
| 注记要素 | 20 | annotation_layer | Point | 文本标注，含多种字体样式 |
| 栅格数据 | 20 | raster_layer | RasterBand | 包含单波段、多波段栅格 |
| **合计** | **100** | - | - | - |

### 3.2 点要素属性设计

基于 [Point](file:///f:/cycle/trae/chart/code/geom/include/ogc/point.h) 和 [PointSymbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/point_symbolizer.h) 类定义：

#### 3.2.1 几何属性

| 属性名 | 类型 | 取值范围 | 覆盖情况 |
|--------|------|---------|---------|
| x | double | 0.0 - 1000.0 | ✓ |
| y | double | 0.0 - 1000.0 | ✓ |
| z | double | 0.0 - 100.0 | 部分点(3D) |
| m | double | 0.0 - 1.0 | 部分点(测量值) |
| srid | int | 4326/3857 | ✓ |

#### 3.2.2 符号化属性

| 属性名 | 类型 | 取值示例 | 覆盖情况 |
|--------|------|---------|---------|
| symbolType | PointSymbolType | Circle/Square/Triangle/Star/Cross/Diamond | ✓ 全覆盖 |
| size | double | 3.0 - 20.0 | ✓ |
| color | uint32_t | 0xFFFF0000 (红) | ✓ |
| strokeColor | uint32_t | 0xFF000000 (黑) | ✓ |
| strokeWidth | double | 0.5 - 3.0 | ✓ |
| rotation | double | 0.0 - 360.0 | ✓ |
| anchorX/anchorY | double | 0.0 - 1.0 | ✓ |
| displacementX/Y | double | -10.0 - 10.0 | ✓ |

#### 3.2.3 要素属性字段

| 字段名 | 字段类型 | 说明 |
|--------|---------|------|
| id | int64 | 唯一标识 |
| name | string | 点名称 |
| category | int32 | 分类编码 |
| value | double | 属性值 |
| is_active | bool | 是否激活 |
| create_time | DateTime | 创建时间 |

### 3.3 线要素属性设计

基于 [LineString](file:///f:/cycle/trae/chart/code/geom/include/ogc/linestring.h) 和 [LineSymbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/line_symbolizer.h) 类定义：

#### 3.3.1 几何属性

| 属性名 | 类型 | 取值范围 | 覆盖情况 |
|--------|------|---------|---------|
| coordinates | CoordinateList | 2-50个坐标点 | ✓ |
| is3D | bool | true/false | 部分线 |
| isMeasured | bool | true/false | 部分线 |
| isClosed | bool | true/false | 部分线 |

#### 3.3.2 符号化属性

| 属性名 | 类型 | 取值示例 | 覆盖情况 |
|--------|------|---------|---------|
| width | double | 1.0 - 10.0 | ✓ |
| color | uint32_t | 0xFF0000FF (蓝) | ✓ |
| opacity | double | 0.3 - 1.0 | ✓ |
| capStyle | LineCapStyle | Butt/Round/Square | ✓ |
| joinStyle | LineJoinStyle | Miter/Round/Bevel | ✓ |
| dashStyle | DashStyle | Solid/Dash/Dot/DashDot/DashDotDot/Custom | ✓ |
| dashPattern | vector<double> | {5.0, 3.0} | 部分线 |
| dashOffset | double | 0.0 - 10.0 | ✓ |
| offset | double | -5.0 - 5.0 | ✓ |
| perpendicularOffset | double | -10.0 - 10.0 | ✓ |
| graphicStroke | bool | true/false | 部分线 |

#### 3.3.3 要素属性字段

| 字段名 | 字段类型 | 说明 |
|--------|---------|------|
| id | int64 | 唯一标识 |
| road_name | string | 道路名称 |
| road_class | int32 | 道路等级 |
| length | double | 长度 |
| is_oneway | bool | 是否单行 |
| speed_limit | int32 | 限速值 |

### 3.4 区要素属性设计

基于 [Polygon](file:///f:/cycle/trae/chart/code/geom/include/ogc/polygon.h) 和 [PolygonSymbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/polygon_symbolizer.h) 类定义：

#### 3.4.1 几何属性

| 属性名 | 类型 | 取值范围 | 覆盖情况 |
|--------|------|---------|---------|
| exteriorRing | LinearRing | 4-100个坐标点 | ✓ |
| interiorRings | vector<LinearRing> | 0-3个内环 | 部分区 |
| is3D | bool | true/false | 部分区 |

#### 3.4.2 符号化属性

| 属性名 | 类型 | 取值示例 | 覆盖情况 |
|--------|------|---------|---------|
| fillColor | uint32_t | 0xFF00FF00 (绿) | ✓ |
| fillOpacity | double | 0.3 - 1.0 | ✓ |
| strokeColor | uint32_t | 0xFF000000 (黑) | ✓ |
| strokeWidth | double | 0.5 - 5.0 | ✓ |
| strokeOpacity | double | 0.5 - 1.0 | ✓ |
| fillPattern | FillPattern | Solid/Horizontal/Vertical/Cross/Diagonal/DiagonalCross/Dot/Custom | ✓ |
| displacementX/Y | double | -5.0 - 5.0 | ✓ |
| perpendicularOffset | double | -5.0 - 5.0 | ✓ |
| graphicFill | bool | true/false | 部分区 |

#### 3.4.3 要素属性字段

| 字段名 | 字段类型 | 说明 |
|--------|---------|------|
| id | int64 | 唯一标识 |
| parcel_name | string | 地块名称 |
| land_use | int32 | 用地类型 |
| area | double | 面积 |
| perimeter | double | 周长 |
| owner | string | 所有者 |

### 3.5 注记要素属性设计

基于 [TextSymbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/text_symbolizer.h) 类定义：

#### 3.5.1 几何属性

| 属性名 | 类型 | 取值范围 | 覆盖情况 |
|--------|------|---------|---------|
| anchorPoint | Point | 注记锚点 | ✓ |

#### 3.5.2 符号化属性

| 属性名 | 类型 | 取值示例 | 覆盖情况 |
|--------|------|---------|---------|
| label | string | "标注文本" | ✓ |
| fontFamily | string | "Arial"/"SimSun"/"SimHei" | ✓ |
| fontSize | double | 8.0 - 24.0 | ✓ |
| fontStyle | FontStyle | Normal/Bold/Italic/BoldItalic | ✓ |
| color | uint32_t | 0xFF000000 (黑) | ✓ |
| opacity | double | 0.5 - 1.0 | ✓ |
| horizontalAlignment | TextHorizontalAlignment | Left/Center/Right | ✓ |
| verticalAlignment | TextVerticalAlignment | Top/Middle/Bottom | ✓ |
| placement | TextPlacement | Point/Line/Interior | ✓ |
| offsetX/Y | double | -20.0 - 20.0 | ✓ |
| rotation | double | 0.0 - 360.0 | ✓ |
| followLine | bool | true/false | 部分注记 |
| haloColor | uint32_t | 0xFFFFFFFF (白) | 部分注记 |
| haloRadius | double | 1.0 - 5.0 | 部分注记 |
| backgroundColor | uint32_t | 0xFFFFFF00 (黄) | 部分注记 |

#### 3.5.3 要素属性字段

| 字段名 | 字段类型 | 说明 |
|--------|---------|------|
| id | int64 | 唯一标识 |
| text_content | string | 注记内容 |
| font_family | string | 字体名称 |
| font_size | double | 字号 |
| priority | int32 | 显示优先级 |

### 3.6 栅格数据属性设计

基于 [CNRasterLayer](file:///f:/cycle/trae/chart/code/layer/include/ogc/layer/raster_layer.h) 和 [RasterSymbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/raster_symbolizer.h) 类定义：

#### 3.6.1 几何属性

| 属性名 | 类型 | 取值范围 | 覆盖情况 |
|--------|------|---------|---------|
| width | int | 64 - 512 | ✓ |
| height | int | 64 - 512 | ✓ |
| bandCount | int | 1 - 4 | ✓ |
| dataType | CNDataType | Byte/UInt16/Int16/Float32/Float64 | ✓ |
| geoTransform | double[6] | 仿射变换参数 | ✓ |

#### 3.6.2 符号化属性

| 属性名 | 类型 | 取值示例 | 覆盖情况 |
|--------|------|---------|---------|
| opacity | double | 0.5 - 1.0 | ✓ |
| channelSelection | RasterChannelSelection | RGB/Grayscale/PseudoColor/Custom | ✓ |
| redChannel | int | 0 - 3 | 多波段时 |
| greenChannel | int | 0 - 3 | 多波段时 |
| blueChannel | int | 0 - 3 | 多波段时 |
| grayChannel | int | 0 | 灰度时 |
| contrastEnhancement | bool | true/false | ✓ |
| contrastValue | double | 0.5 - 2.0 | ✓ |
| brightnessValue | double | -0.5 - 0.5 | ✓ |
| gammaValue | double | 0.5 - 2.0 | ✓ |
| colorMap | vector<ColorMapEntry> | 颜色映射表 | 伪彩色时 |
| resampling | string | "nearest"/"bilinear"/"cubic" | ✓ |

#### 3.6.3 元数据字段

| 字段名 | 字段类型 | 说明 |
|--------|---------|------|
| id | int64 | 唯一标识 |
| raster_name | string | 栅格名称 |
| description | string | 描述信息 |
| min_value | double | 最小值 |
| max_value | double | 最大值 |
| no_data_value | double | 无效值 |

---

## 4. 数据存储设计

### 4.1 数据库结构

使用 [SpatiaLiteConnection](file:///f:/cycle/trae/chart/code/database/include/ogc/db/sqlite_connection.h) 创建数据库 `sqlite_demo.db`：

#### 4.1.1 点要素表

```sql
CREATE TABLE point_layer (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    category INTEGER,
    value REAL,
    is_active INTEGER,
    create_time TEXT,
    geom BLOB
);

SELECT AddGeometryColumn('point_layer', 'geom', 4326, 'POINT', 'XY');
SELECT CreateSpatialIndex('point_layer', 'geom');
```

#### 4.1.2 线要素表

```sql
CREATE TABLE line_layer (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    road_name TEXT NOT NULL,
    road_class INTEGER,
    length REAL,
    is_oneway INTEGER,
    speed_limit INTEGER,
    geom BLOB
);

SELECT AddGeometryColumn('line_layer', 'geom', 4326, 'LINESTRING', 'XY');
SELECT CreateSpatialIndex('line_layer', 'geom');
```

#### 4.1.3 区要素表

```sql
CREATE TABLE polygon_layer (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    parcel_name TEXT NOT NULL,
    land_use INTEGER,
    area REAL,
    perimeter REAL,
    owner TEXT,
    geom BLOB
);

SELECT AddGeometryColumn('polygon_layer', 'geom', 4326, 'POLYGON', 'XY');
SELECT CreateSpatialIndex('polygon_layer', 'geom');
```

#### 4.1.4 注记要素表

```sql
CREATE TABLE annotation_layer (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    text_content TEXT NOT NULL,
    font_family TEXT,
    font_size REAL,
    priority INTEGER,
    rotation REAL,
    geom BLOB
);

SELECT AddGeometryColumn('annotation_layer', 'geom', 4326, 'POINT', 'XY');
SELECT CreateSpatialIndex('annotation_layer', 'geom');
```

#### 4.1.5 栅格元数据表

```sql
CREATE TABLE raster_layer (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    raster_name TEXT NOT NULL,
    description TEXT,
    width INTEGER,
    height INTEGER,
    band_count INTEGER,
    data_type INTEGER,
    min_value REAL,
    max_value REAL,
    no_data_value REAL,
    geo_transform TEXT,
    raster_data BLOB
);
```

### 4.2 数据插入流程

```cpp
// 伪代码示例
void InsertMockData(SpatiaLiteConnection& conn) {
    // 1. 初始化空间元数据
    conn.InitializeSpatialMetaData();
    
    // 2. 创建各图层表
    conn.CreateSpatialTable("point_layer", "geom", 
        static_cast<int>(GeomType::kPoint), 4326, 2);
    // ... 其他表
    
    // 3. 创建空间索引
    conn.CreateSpatialIndex("point_layer", "geom");
    // ... 其他索引
    
    // 4. 批量插入数据
    for (auto& feature : mockFeatures) {
        conn.InsertGeometry(tableName, "geom", 
            feature.GetGeometry(), attributes, outId);
    }
}
```

---

## 5. 空间查询与可见性过滤

### 5.1 显示范围定义

```cpp
struct DisplayExtent {
    double minX;
    double minY;
    double maxX;
    double maxY;
    int srid;
};
```

### 5.2 空间查询实现

基于 [CNLayer](file:///f:/cycle/trae/chart/code/layer/include/ogc/layer/layer.h) 的空间过滤接口：

```cpp
// 设置空间过滤器
void SetSpatialFilter(CNLayer* layer, const DisplayExtent& extent) {
    layer->SetSpatialFilterRect(extent.minX, extent.minY, 
                                 extent.maxX, extent.maxY);
}

// 获取可见要素
std::vector<std::unique_ptr<CNFeature>> GetVisibleFeatures(CNLayer* layer) {
    std::vector<std::unique_ptr<CNFeature>> features;
    layer->ResetReading();
    while (auto feature = layer->GetNextFeature()) {
        features.push_back(std::move(feature));
    }
    return features;
}
```

### 5.3 可见性判断逻辑

```
┌─────────────────────────────────────────────────────────────┐
│                     显示范围 (DisplayExtent)                  │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                                                       │  │
│  │    要素A (完全在范围内) ──▶ 可见 ✓                     │  │
│  │                                                       │  │
│  │         ┌─────────┐                                   │  │
│  │         │ 要素B   │ (部分在范围内) ──▶ 可见 ✓          │  │
│  │         └─────────┘                                   │  │
│  │                                                       │  │
│  │                            要素C (完全在范围外)        │  │
│  │                            ──▶ 不可见 ✗               │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. 符号化渲染

### 6.1 符号化器选择策略

基于 [Symbolizer](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/symbolizer.h) 基类：

```cpp
SymbolizerPtr CreateSymbolizer(GeomType geomType) {
    switch (geomType) {
        case GeomType::kPoint:
        case GeomType::kMultiPoint:
            return PointSymbolizer::Create();
        case GeomType::kLineString:
        case GeomType::kMultiLineString:
            return LineSymbolizer::Create();
        case GeomType::kPolygon:
        case GeomType::kMultiPolygon:
            return PolygonSymbolizer::Create();
        default:
            return nullptr;
    }
}
```

### 6.2 渲染流程

```cpp
void RenderVisibleFeatures(
    DrawDevicePtr device,
    const std::vector<std::unique_ptr<CNFeature>>& features,
    const std::map<std::string, SymbolizerPtr>& symbolizers) {
    
    // 1. 开始绘制
    DrawParams params;
    params.width = device->GetWidth();
    params.height = device->GetHeight();
    device->BeginDraw(params);
    
    // 2. 按图层顺序渲染
    for (const auto& feature : features) {
        auto* geom = feature->GetGeometry();
        auto layerName = GetLayerName(feature);
        auto& symbolizer = symbolizers[layerName];
        
        if (symbolizer && symbolizer->CanSymbolize(geom->GetGeometryType())) {
            symbolizer->Symbolize(context, geom);
        }
    }
    
    // 3. 结束绘制
    device->EndDraw();
}
```

### 6.3 样式配置

```cpp
// 点样式配置
void ConfigurePointSymbolizer(PointSymbolizer* sym, const CNFeature* feature) {
    sym->SetSize(feature->GetFieldAsReal("size"));
    sym->SetColor(feature->GetFieldAsInteger("color"));
    sym->SetRotation(feature->GetFieldAsReal("rotation"));
    // ...
}

// 线样式配置
void ConfigureLineSymbolizer(LineSymbolizer* sym, const CNFeature* feature) {
    sym->SetWidth(feature->GetFieldAsReal("width"));
    sym->SetColor(feature->GetFieldAsInteger("color"));
    sym->SetDashStyle(static_cast<DashStyle>(feature->GetFieldAsInteger("dash_style")));
    // ...
}

// 区样式配置
void ConfigurePolygonSymbolizer(PolygonSymbolizer* sym, const CNFeature* feature) {
    sym->SetFillColor(feature->GetFieldAsInteger("fill_color"));
    sym->SetFillOpacity(feature->GetFieldAsReal("fill_opacity"));
    sym->SetStrokeColor(feature->GetFieldAsInteger("stroke_color"));
    // ...
}
```

---

## 7. 输出设备支持

### 7.1 设备类型概述

基于 [DeviceType](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/device_type.h) 枚举：

| 设备类型 | 类名 | 用途 | 输出格式 |
|---------|------|------|---------|
| kRasterImage | RasterImageDevice | 图像输出 | PNG/JPEG/BMP/TIFF |
| kPdf | PdfDevice | PDF文档输出 | PDF |
| kDisplay | DisplayDevice | 屏幕显示 | 屏幕渲染 |

### 7.2 RasterImageDevice 配置

基于 [RasterImageDevice](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/raster_image_device.h)：

```cpp
// 创建栅格图像设备
auto rasterDevice = RasterImageDevice::Create(800, 600, 4);  // 800x600, RGBA

// 配置参数
rasterDevice->SetDpi(96.0);
rasterDevice->SetAntialiasing(true);

// 渲染完成后保存
rasterDevice->SaveToFile("output.png", ImageFormat::kPNG);
```

### 7.3 PdfDevice 配置

基于 [PdfDevice](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/pdf_device.h)：

```cpp
// 创建PDF设备
PdfDevice pdfDevice(800, 600, 72.0);  // 800x600, 72 DPI

// 配置参数
pdfDevice.SetDpi(72.0);
pdfDevice.SetAntialiasing(true);

// 渲染完成后保存
pdfDevice.SaveToFile("output.pdf");
```

### 7.4 DisplayDevice 配置

基于 [DisplayDevice](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/display_device.h)：

```cpp
// 创建显示设备
DisplayDevice displayDevice(800, 600, "OGC Render Demo");

// 配置参数
displayDevice.SetDpi(96.0);
displayDevice.SetAntialiasing(true);
displayDevice.SetDisplayMode(DisplayMode::kWindowed);

// 渲染循环
while (displayDevice.IsReady()) {
    displayDevice.BeginDraw(params);
    // 渲染操作...
    displayDevice.EndDraw();
    // 处理事件...
}
```

### 7.5 设备统一接口

```cpp
void RenderToDevice(DrawDevicePtr device, 
                    const std::vector<LayerData>& layers,
                    const DisplayExtent& extent) {
    // 统一的渲染接口，适用于所有设备类型
    DrawParams params;
    params.width = device->GetWidth();
    params.height = device->GetHeight();
    params.extent = extent;
    
    device->BeginDraw(params);
    
    for (const auto& layer : layers) {
        auto visibleFeatures = GetVisibleFeatures(layer, extent);
        auto symbolizer = GetSymbolizer(layer.type);
        for (const auto& feature : visibleFeatures) {
            symbolizer->Symbolize(context, feature->GetGeometry());
        }
    }
    
    device->EndDraw();
}
```

---

## 8. 测试用例设计

### 8.1 数据生成测试

| 测试ID | 测试名称 | 测试内容 | 预期结果 |
|--------|---------|---------|---------|
| TG-001 | 点数据生成 | 生成20条点要素 | 覆盖所有点符号类型 |
| TG-002 | 线数据生成 | 生成20条线要素 | 覆盖所有线样式类型 |
| TG-003 | 区数据生成 | 生成20条区要素 | 覆盖所有区填充模式 |
| TG-004 | 注记生成 | 生成20条注记要素 | 覆盖所有文本对齐方式 |
| TG-005 | 栅格生成 | 生成20个栅格数据 | 覆盖所有数据类型 |
| TG-006 | 数据总量 | 统计生成数据总量 | 总量=100±5 |

### 8.2 存储测试

| 测试ID | 测试名称 | 测试内容 | 预期结果 |
|--------|---------|---------|---------|
| TS-001 | 数据库创建 | 创建sqlite_demo.db | 文件存在且有效 |
| TS-002 | 表结构创建 | 创建5个图层表 | 所有表创建成功 |
| TS-003 | 空间索引创建 | 创建空间索引 | 索引创建成功 |
| TS-004 | 数据插入 | 插入所有模拟数据 | 插入成功，ID正确 |
| TS-005 | 数据完整性 | 验证数据完整性 | 数据无丢失、无损坏 |

### 8.3 空间查询测试

| 测试ID | 测试名称 | 测试内容 | 预期结果 |
|--------|---------|---------|---------|
| TQ-001 | 全范围查询 | 查询全部数据 | 返回100条数据 |
| TQ-002 | 部分范围查询 | 查询部分范围 | 返回可见数据 |
| TQ-003 | 空范围查询 | 查询无数据范围 | 返回空集合 |
| TQ-004 | 边界查询 | 查询边界区域 | 正确处理边界要素 |
| TQ-005 | 跨投影查询 | 不同SRID查询 | 正确转换坐标 |

### 8.4 渲染测试

| 测试ID | 测试名称 | 测试内容 | 预期结果 |
|--------|---------|---------|---------|
| TR-001 | 栅格图像输出 | 输出PNG图像 | 图像正确生成 |
| TR-002 | PDF输出 | 输出PDF文档 | PDF正确生成 |
| TR-003 | 屏幕显示 | 屏幕渲染 | 显示正确 |
| TR-004 | 符号化正确性 | 验证符号化效果 | 符号样式正确 |
| TR-005 | 性能测试 | 渲染性能测试 | 满足性能指标 |

---

## 9. 性能指标

### 9.1 数据生成性能

| 指标 | 目标值 | 说明 |
|------|--------|------|
| 单条数据生成时间 | < 10ms | 平均每条要素生成时间 |
| 总数据生成时间 | < 5s | 100条数据总生成时间 |
| 内存占用峰值 | < 100MB | 生成过程内存峰值 |

### 9.2 存储性能

| 指标 | 目标值 | 说明 |
|------|--------|------|
| 单条数据插入时间 | < 5ms | 平均每条要素插入时间 |
| 批量插入时间 | < 2s | 100条数据批量插入时间 |
| 数据库文件大小 | < 10MB | 最终数据库文件大小 |

### 9.3 查询性能

| 指标 | 目标值 | 说明 |
|------|--------|------|
| 空间查询响应时间 | < 50ms | 单次空间查询时间 |
| 全表扫描时间 | < 100ms | 无索引查询时间 |
| 索引查询时间 | < 10ms | 有索引查询时间 |

### 9.4 渲染性能

| 指标 | 目标值 | 说明 |
|------|--------|------|
| 首帧渲染时间 | < 500ms | 首次渲染时间 |
| 平均帧渲染时间 | < 100ms | 平均每帧渲染时间 |
| 内存占用 | < 200MB | 渲染过程内存占用 |

---

## 10. 错误处理机制

### 10.1 错误码定义

```cpp
enum class RenderErrorCode {
    kSuccess = 0,
    
    // 数据库相关错误 (1000-1099)
    kDatabaseConnectionFailed = 1001,
    kDatabaseQueryError = 1002,
    kDatabaseInsertError = 1003,
    kSpatialIndexError = 1004,
    
    // 几何相关错误 (1100-1199)
    kGeometryNull = 1101,
    kGeometryInvalid = 1102,
    kGeometryTypeMismatch = 1103,
    kCoordinateOutOfRange = 1104,
    
    // 渲染相关错误 (1200-1299)
    kSymbolizerNotFound = 1201,
    kSymbolizerTypeMismatch = 1202,
    kStyleInvalid = 1203,
    
    // 设备相关错误 (1300-1399)
    kDeviceInitFailed = 1301,
    kDeviceNotValid = 1302,
    kDeviceSaveFailed = 1303,
    kUnsupportedFormat = 1304,
    
    // 通用错误 (9000-9999)
    kUnknownError = 9999
};
```

### 10.2 结果结构定义

```cpp
struct RenderResult {
    RenderErrorCode code;
    std::string message;
    std::string context;
    std::string details;
    
    bool IsSuccess() const { return code == RenderErrorCode::kSuccess; }
    
    static RenderResult Success() {
        return {RenderErrorCode::kSuccess, "", "", ""};
    }
    
    static RenderResult Error(RenderErrorCode code, 
                               const std::string& msg, 
                               const std::string& ctx = "") {
        return {code, msg, ctx, ""};
    }
};
```

### 10.3 错误处理示例

```cpp
RenderResult InsertFeature(SpatiaLiteConnection& conn, 
                            const CNFeature* feature,
                            const std::string& tableName) {
    if (!feature) {
        return RenderResult::Error(
            RenderErrorCode::kGeometryNull,
            "Feature is null",
            "InsertFeature");
    }
    
    auto* geom = feature->GetGeometry();
    if (!geom || !geom->IsValid()) {
        return RenderResult::Error(
            RenderErrorCode::kGeometryInvalid,
            "Geometry is null or invalid",
            "InsertFeature");
    }
    
    int64_t outId = 0;
    bool success = conn.InsertGeometry(tableName, "geom", 
                                        geom, 
                                        feature->GetFieldMap(), 
                                        outId);
    if (!success) {
        return RenderResult::Error(
            RenderErrorCode::kDatabaseInsertError,
            "Failed to insert geometry into " + tableName,
            "InsertFeature");
    }
    
    return RenderResult::Success();
}

RenderResult RenderFeature(DrawDevicePtr device, 
                            const CNFeature* feature,
                            SymbolizerPtr symbolizer) {
    if (!device || !device->IsValid()) {
        return RenderResult::Error(
            RenderErrorCode::kDeviceNotValid,
            "Device is null or not valid",
            "RenderFeature");
    }
    
    if (!feature || !feature->GetGeometry()) {
        return RenderResult::Error(
            RenderErrorCode::kGeometryNull,
            "Feature or geometry is null",
            "RenderFeature");
    }
    
    if (!symbolizer) {
        return RenderResult::Error(
            RenderErrorCode::kSymbolizerNotFound,
            "Symbolizer is null",
            "RenderFeature");
    }
    
    auto* geom = feature->GetGeometry();
    if (!symbolizer->CanSymbolize(geom->GetGeometryType())) {
        return RenderResult::Error(
            RenderErrorCode::kSymbolizerTypeMismatch,
            "Symbolizer cannot handle geometry type",
            "RenderFeature");
    }
    
    symbolizer->Symbolize(context, geom);
    return RenderResult::Success();
}
```

### 10.4 异常安全策略

| 场景 | 策略 | 说明 |
|------|------|------|
| 数据库操作 | RAII事务包装 | 异常时自动回滚 |
| 内存分配 | 智能指针 | 自动释放内存 |
| 文件操作 | RAII文件句柄 | 自动关闭文件 |
| 渲染状态 | 状态栈管理 | 异常时恢复状态 |

---

## 11. 渲染上下文管理

### 11.1 渲染上下文结构

```cpp
struct RenderContext {
    // 坐标系相关
    DisplayExtent worldExtent;           // 世界坐标范围
    Viewport viewport;                   // 视口范围（像素）
    TransformMatrix worldToScreen;       // 世界到屏幕变换矩阵
    int targetSrid;                      // 目标坐标系
    
    // 渲染状态
    RenderStateStack stateStack;         // 状态栈
    ClipRegion clipRegion;               // 裁剪区域
    double currentOpacity;               // 当前透明度
    uint32_t currentColor;               // 当前颜色
    
    // 设备信息
    DrawDevicePtr device;                // 输出设备
    double dpi;                          // 设备DPI
    
    // 状态管理方法
    void PushState();
    void PopState();
    void SetClipRect(const Rect& rect);
    void ClearClipRect();
    
    // 坐标转换方法
    Point WorldToScreen(const Point& worldPt);
    Point ScreenToWorld(const Point& screenPt);
    Rect WorldToScreen(const Rect& worldRect);
    
    // 辅助方法
    double WorldToScreen(double worldDistance);
    double ScreenToWorld(double screenDistance);
};
```

### 11.2 状态栈管理

```cpp
struct RenderState {
    uint32_t fillColor;
    uint32_t strokeColor;
    double fillOpacity;
    double strokeOpacity;
    double lineWidth;
    ClipRegion clipRegion;
    TransformMatrix transform;
};

class RenderStateStack {
public:
    void Push(const RenderState& state);
    RenderState Pop();
    RenderState& Current();
    const RenderState& Current() const;
    size_t Depth() const;
    
private:
    std::vector<RenderState> stack_;
};

void RenderContext::PushState() {
    stateStack_.Push(currentState_);
}

void RenderContext::PopState() {
    if (stateStack_.Depth() > 0) {
        currentState_ = stateStack_.Pop();
    }
}
```

### 11.3 坐标转换实现

```cpp
Point RenderContext::WorldToScreen(const Point& worldPt) {
    double screenX = (worldPt.X() - worldExtent.minX) / 
                     (worldExtent.maxX - worldExtent.minX) * viewport.width;
    double screenY = viewport.height - 
                     (worldPt.Y() - worldExtent.minY) / 
                     (worldExtent.maxY - worldExtent.minY) * viewport.height;
    return Point(screenX, screenY);
}

double RenderContext::WorldToScreen(double worldDistance) {
    double scaleX = viewport.width / (worldExtent.maxX - worldExtent.minX);
    return worldDistance * scaleX;
}
```

### 11.4 渲染流程状态管理

```cpp
void RenderLayer(RenderContext& ctx, CNLayer* layer) {
    ctx.PushState();
    
    auto visibleFeatures = GetVisibleFeatures(layer, ctx.worldExtent);
    auto symbolizer = GetSymbolizer(layer->GetType());
    
    for (const auto& feature : visibleFeatures) {
        ctx.PushState();
        
        ApplyFeatureStyle(ctx, feature.get());
        symbolizer->Symbolize(ctx, feature->GetGeometry());
        
        ctx.PopState();
    }
    
    ctx.PopState();
}
```

---

## 12. 性能优化策略

### 12.1 空间索引优化

| 策略 | 适用场景 | 优化效果 |
|------|----------|----------|
| R-Tree索引 | 范围查询、邻近查询 | 查询时间 O(log N) |
| Quad-Tree索引 | 均匀分布数据 | 内存占用更小 |
| 网格索引 | 固定范围查询 | 实现简单 |

**推荐方案**: 使用SpatiaLite内置R-Tree索引

```sql
SELECT CreateSpatialIndex('point_layer', 'geom');
SELECT CreateSpatialIndex('line_layer', 'geom');
SELECT CreateSpatialIndex('polygon_layer', 'geom');
SELECT CreateSpatialIndex('annotation_layer', 'geom');
```

### 12.2 批量渲染优化

| 策略 | 描述 | 预期收益 |
|------|------|----------|
| 状态排序 | 按渲染状态分组，减少状态切换 | 渲染时间减少20-30% |
| 实例化渲染 | 相同符号批量绘制 | 减少Draw Call |
| 顶点缓冲 | 预生成顶点数据 | 减少CPU-GPU传输 |

```cpp
void OptimizedRender(RenderContext& ctx, 
                      const std::vector<CNFeature*>& features) {
    std::map<SymbolizerKey, std::vector<CNFeature*>> groupedFeatures;
    
    for (auto* feature : features) {
        auto key = GetSymbolizerKey(feature);
        groupedFeatures[key].push_back(feature);
    }
    
    for (auto& [key, group] : groupedFeatures) {
        auto symbolizer = GetSymbolizer(key);
        ApplySymbolizerStyle(ctx, symbolizer);
        
        for (auto* feature : group) {
            symbolizer->Symbolize(ctx, feature->GetGeometry());
        }
    }
}
```

### 12.3 内存优化

| 策略 | 描述 | 适用场景 |
|------|------|----------|
| 对象池 | 复用几何对象 | 频繁创建销毁场景 |
| 延迟加载 | 按需加载数据 | 大数据量场景 |
| 数据分页 | 分批处理数据 | 内存受限场景 |

```cpp
template<typename T>
class ObjectPool {
public:
    std::shared_ptr<T> Acquire() {
        if (!pool_.empty()) {
            auto obj = pool_.back();
            pool_.pop_back();
            return obj;
        }
        return std::make_shared<T>();
    }
    
    void Release(std::shared_ptr<T> obj) {
        obj->Reset();
        pool_.push_back(obj);
    }
    
private:
    std::vector<std::shared_ptr<T>> pool_;
};
```

### 12.4 数据库优化

| 策略 | 描述 | 预期收益 |
|------|------|----------|
| 批量事务 | 多条插入合并为一个事务 | 插入速度提升10倍 |
| 连接池 | 复用数据库连接 | 减少连接开销 |
| 预编译语句 | 缓存SQL语句 | 减少解析开销 |

```cpp
void BatchInsert(SpatiaLiteConnection& conn, 
                  const std::vector<CNFeature*>& features) {
    conn.BeginTransaction();
    
    for (auto* feature : features) {
        InsertFeature(conn, feature, "point_layer");
    }
    
    conn.CommitTransaction();
}
```

### 12.5 大数据量场景策略

| 场景 | 数据量 | 优化策略 |
|------|--------|----------|
| 小规模 | < 1万 | 直接渲染 |
| 中规模 | 1-10万 | 空间过滤 + 状态排序 |
| 大规模 | > 10万 | 分层加载(LOD) + 瓦片化 |
| 超大规模 | > 100万 | 异步加载 + 增量渲染 |

---

## 13. 坐标系处理策略

### 13.1 坐标系规划

| 用途 | 坐标系 | SRID | 说明 |
|------|--------|------|------|
| 数据存储 | WGS84 | 4326 | 经纬度坐标，通用标准 |
| Web显示 | Web墨卡托 | 3857 | 平面坐标，适合Web地图 |
| 局部计算 | 局部坐标系 | - | 精度更高 |

### 13.2 坐标转换接口

```cpp
class CoordinateTransformer {
public:
    static bool Transform(Geometry* geom, int sourceSrid, int targetSrid);
    static Point Transform(const Point& pt, int sourceSrid, int targetSrid);
    static Rect Transform(const Rect& rect, int sourceSrid, int targetSrid);
    
private:
    static std::map<std::pair<int,int>, TransformParams> cache_;
};
```

### 13.3 坐标转换流程

```
┌─────────────────────────────────────────────────────────────────┐
│                       坐标系处理流程                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  数据存储 (SRID 4326)                                           │
│       │                                                         │
│       ▼                                                         │
│  空间查询 (使用存储坐标系)                                        │
│       │                                                         │
│       ▼                                                         │
│  坐标转换 (4326 → 3857)                                         │
│       │                                                         │
│       ▼                                                         │
│  渲染显示 (使用显示坐标系)                                        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 13.4 跨投影查询处理

```cpp
std::vector<CNFeature*> QueryWithTransform(
    CNLayer* layer,
    const DisplayExtent& displayExtent,
    int displaySrid) {
    
    int storageSrid = layer->GetSrid();
    
    DisplayExtent queryExtent = displayExtent;
    if (displaySrid != storageSrid) {
        queryExtent = CoordinateTransformer::Transform(
            displayExtent, displaySrid, storageSrid);
    }
    
    layer->SetSpatialFilterRect(
        queryExtent.minX, queryExtent.minY,
        queryExtent.maxX, queryExtent.maxY);
    
    std::vector<CNFeature*> features;
    layer->ResetReading();
    while (auto feature = layer->GetNextFeature()) {
        if (displaySrid != storageSrid) {
            CoordinateTransformer::Transform(
                feature->GetGeometry(), storageSrid, displaySrid);
        }
        features.push_back(feature.release());
    }
    
    return features;
}
```

---

## 14. 安全要求

### 14.1 数据安全

| 要求 | 描述 | 实现方式 |
|------|------|----------|
| 数据库加密 | 敏感数据加密存储 | SQLite加密扩展 |
| 字段脱敏 | 敏感字段显示脱敏 | 字段级脱敏规则 |
| 访问日志 | 记录数据访问行为 | 操作日志表 |

### 14.2 访问控制

```cpp
enum class AccessLevel {
    kNone = 0,
    kRead = 1,
    kWrite = 2,
    kAdmin = 3
};

struct FieldAccessControl {
    std::string fieldName;
    AccessLevel readLevel;
    AccessLevel writeLevel;
    bool isSensitive;
    std::string maskRule;
};

class FeatureAccessManager {
public:
    bool CanReadField(const std::string& fieldName, AccessLevel userLevel);
    bool CanWriteField(const std::string& fieldName, AccessLevel userLevel);
    std::string MaskField(const std::string& fieldName, const std::string& value);
    
private:
    std::map<std::string, FieldAccessControl> fieldControls_;
};
```

### 14.3 数据脱敏规则

| 字段类型 | 脱敏规则 | 示例 |
|----------|----------|------|
| 姓名 | 保留首字，其余用*替代 | 张** |
| 电话 | 保留前3后4位 | 138****5678 |
| 身份证 | 保留前3后4位 | 110***********1234 |
| 地址 | 仅保留省市 | 北京市**区** |

---

## 15. 风险与约束

### 15.1 技术风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| Symbolizer接口不完整 | 中 | 高 | 开发前进行接口验证，预留适配层 |
| Device实现缺失 | 中 | 高 | 检查现有实现状态，准备Mock实现 |
| 空间索引性能不达标 | 低 | 中 | 预留索引优化时间，准备备选方案 |
| 内存泄漏 | 中 | 中 | 使用智能指针，添加内存检测工具 |
| 坐标转换精度损失 | 低 | 中 | 使用双精度计算，添加精度验证 |

### 15.2 资源风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| 依赖模块延期 | 中 | 高 | 提前沟通，准备Mock数据 |
| 开发资源不足 | 低 | 中 | 优先实现核心功能 |
| 测试环境不完整 | 低 | 低 | 准备独立测试环境 |

### 15.3 时间风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| 关键路径延期 | 中 | 高 | 预留缓冲时间，并行开发 |
| 需求变更 | 低 | 中 | 冻结需求，变更走评审流程 |
| 性能优化超时 | 中 | 中 | 分阶段优化，先保证功能 |

### 15.4 技术约束

| 约束 | 描述 | 影响 |
|------|------|------|
| C++11标准 | 不能使用C++14/17特性 | 部分现代语法不可用 |
| 跨平台要求 | 需支持Windows/Linux | 平台相关代码需抽象 |
| 内存限制 | 目标内存占用<200MB | 限制数据缓存策略 |
| 依赖模块状态 | 部分模块可能未完成 | 需要接口适配层 |

### 15.5 编码实施约束 ⚠️ 重要

> **重要提示**: 以下约束规则必须在编码实施前仔细阅读，避免重复踩坑。详细避坑经验请参考 `doc\compile_test_problem_summary.md`。

#### 15.5.1 编码前必读文档

| 步骤 | 文档 | 说明 |
|------|------|------|
| 1 | `doc\compile_test_problem_summary.md` | 查阅已有问题记录，避免重复踩坑 |
| 2 | 相关模块的 `index.md` | 了解已有类和方法 |
| 3 | 对应类的头文件 | 确认API签名和命名 |
| 4 | 对应模块的测试文件 | 参考正确用法示例 |

#### 15.5.2 API调用规范 ⚠️ 高频问题

| 规则 | 错误示例 | 正确示例 | 说明 |
|------|----------|----------|------|
| 先找头文件确认API | 脑补函数名 | 查看头文件确认 | 不要凭想象调用 |
| Getter使用Get前缀 | `Size()`, `Count()` | `GetSize()`, `GetCount()` | 遵循命名规范 |
| 索引访问使用N后缀 | `GetCoordinateAt(i)` | `GetCoordinateN(i)` | OGC标准命名 |
| 使用正确的方法名 | `AsWKT()` | `AsText()` | OGC标准方法 |

**API查找流程**:
```
需要使用某类 → 找到对应头文件 → 确认方法签名 → 参考测试用例 → 编写代码
     │              │                │                │
     ▼              ▼                ▼                ▼
 SpatiaLiteConnection → sqlite_connection.h → Connect() → connection_pool_test.cpp
```

#### 15.5.3 典型API对照表

| 类 | 头文件路径 | 常见错误调用 | 正确调用 |
|----|-----------|-------------|----------|
| SpatiaLiteConnection | `database/include/ogc/db/sqlite_connection.h` | `ConnectDb()` | `Connect()` |
| LineString | `geom/include/ogc/linestring.h` | `GetCoordinateAt(i)` | `GetCoordinateN(i)` |
| Polygon | `geom/include/ogc/polygon.h` | `GetInteriorRing(i)` | `GetInteriorRingN(i)` |
| Geometry | `geom/include/ogc/geometry.h` | `AsWKT()` | `AsText()` |
| RenderQueue | `graph/include/ogc/draw/render_queue.h` | `Size()` | `GetSize()` |
| Envelope | `geom/include/ogc/envelope.h` | `Contains(x, y)` | `Contains(Coordinate(x, y))` |

#### 15.5.4 编码实施检查清单

**编码前检查**:
- [ ] 已查阅 `doc\compile_test_problem_summary.md` 了解常见问题
- [ ] 已找到相关类的头文件，确认API签名
- [ ] 已参考相关测试文件了解正确用法
- [ ] 确认项目C++标准版本（本项目为C++11）

**编码时检查**:
- [ ] 所有使用的标准库类型都有对应的 `#include`
- [ ] API方法名与头文件定义一致，未脑补
- [ ] const方法中调用的方法都是const的
- [ ] 智能指针转换使用正确的方式（`release()` 转移所有权）
- [ ] 禁止使用C++14/17特性（如结构化绑定 `auto [a, b] = ...`）

**编译前检查**:
- [ ] CMakeLists.txt 中源文件列表已更新
- [ ] DLL导出宏配置正确

#### 15.5.5 常见错误示例与修正

**示例1: API命名错误**
```cpp
// ❌ 错误: 脑补的函数名
conn.ConnectDb("test.db");

// ✅ 正确: 查看头文件 sqlite_connection.h 确认
conn.Connect("test.db");
```

**示例2: C++17特性误用**
```cpp
// ❌ 错误: C++17结构化绑定，项目使用C++11
auto [envelope, data] = entries[i];

// ✅ 正确: 使用传统pair访问
Envelope envelope = entries[i].first;
T data = entries[i].second;
```

**示例3: 头文件缺失**
```cpp
// ❌ 错误: 依赖传递包含，可能编译失败
std::map<std::string, int> cache;  // 未显式包含<map>

// ✅ 正确: 显式包含所有使用的标准库头文件
#include <map>
#include <algorithm>
#include <sstream>
```

#### 15.5.6 参考测试文件索引

| 模块 | 测试文件路径 | 参考内容 |
|------|-------------|----------|
| Database | `code/database/test/connection_pool_test.cpp` | SpatiaLiteConnection用法 |
| Geom | `code/geom/test/test_geometry.cpp` | Geometry类用法 |
| Feature | `code/feature/test/test_feature.cpp` | CNFeature用法 |
| Layer | `code/layer/test/test_layer.cpp` | CNLayer用法 |
| Graph | `code/graph/test/test_symbolizer.cpp` | Symbolizer用法 |

### 15.6 关键路径分析

```
阶段1(数据生成) ──► 阶段2(数据库存储) ──► 阶段3(空间查询)
                                              │
                                              ▼
阶段6(测试优化) ◄── 阶段5(多设备输出) ◄── 阶段4(符号化渲染)

关键路径: 阶段1 → 阶段2 → 阶段3 → 阶段4 → 阶段5 → 阶段6
总工期: 8天
缓冲时间: 2天
```

---

## 16. 实现计划

### 16.1 开发阶段

| 阶段 | 任务 | 工作量 | 依赖 |
|------|------|--------|------|
| 阶段1 | 模拟数据生成器实现 | 2天 | Geom/Feature模块 |
| 阶段2 | 数据库存储实现 | 1天 | Database模块 |
| 阶段3 | 空间查询实现 | 1天 | Layer模块 |
| 阶段4 | 符号化渲染实现 | 2天 | Graph模块 |
| 阶段5 | 多设备输出实现 | 1天 | DrawDevice模块 |
| 阶段6 | 测试与优化 | 1天 | 全部模块 |

### 16.2 关键接口依赖

| 模块 | 关键类/接口 | 用途 |
|------|------------|------|
| Geom | Point, LineString, Polygon | 几何对象创建 |
| Feature | CNFeature, CNFieldValue | 要素构建 |
| Layer | CNLayer, CNMemoryLayer | 图层管理 |
| Database | SpatiaLiteConnection | 数据存储 |
| Graph | Symbolizer系列 | 符号化渲染 |
| Graph | DrawDevice系列 | 设备输出 |

### 16.3 模块边界说明

| 模块 | 负责功能 | 不负责功能 |
|------|----------|------------|
| 模拟数据生成器 | 生成测试数据、设置属性 | 数据验证、数据持久化 |
| 空间查询引擎 | 空间过滤、可见性判断 | 数据加载、数据存储 |
| 渲染引擎 | 符号化渲染、样式应用 | 数据获取、设备管理 |
| Feature模块 | 要素构建、属性管理 | 几何操作、渲染逻辑 |
| Layer模块 | 图层管理、数据组织 | 渲染执行、设备输出 |
| Graph模块 | 符号化器、设备接口 | 数据存储、空间查询 |

### 16.4 验收标准

1. **功能验收**
   - 成功生成100条模拟数据
   - 数据成功存储至sqlite_demo.db
   - 空间查询返回正确结果
   - 三种设备均能正确输出

2. **性能验收**
   - 所有性能指标满足要求
   - 无内存泄漏
   - 无崩溃或异常

3. **代码质量**
   - 代码符合项目编码规范
   - 单元测试覆盖率 > 80%
   - 通过静态代码分析

---

## 17. 扩展指南

### 17.1 扩展新几何类型

```cpp
// 1. 在GeomType枚举中添加新类型
enum class GeomType {
    // ... 现有类型
    kTIN,           // 新增：不规则三角网
    kPolyhedralSurface  // 新增：多面体表面
};

// 2. 创建新几何类，继承Geometry基类
class TIN : public Geometry {
public:
    // 实现纯虚函数
    GeomType GetGeometryType() const override { return GeomType::kTIN; }
    Geometry* Clone() const override;
    Envelope GetEnvelope() const override;
    bool IsValid() const override;
    
    // TIN特有方法
    void AddTriangle(const Triangle& triangle);
    size_t GetTriangleCount() const;
    
private:
    std::vector<Triangle> triangles_;
};

// 3. 在符号化器工厂中添加支持
SymbolizerPtr CreateSymbolizer(GeomType geomType) {
    switch (geomType) {
        // ... 现有case
        case GeomType::kTIN:
            return TINSymbolizer::Create();
        default:
            return nullptr;
    }
}
```

### 17.2 扩展新符号化器

```cpp
// 1. 定义新符号化器类
class HeatmapSymbolizer : public Symbolizer {
public:
    static std::unique_ptr<HeatmapSymbolizer> Create();
    
    // 实现基类接口
    SymbolizerType GetType() const override { 
        return SymbolizerType::kHeatmap; 
    }
    bool CanSymbolize(GeomType type) const override;
    void Symbolize(RenderContext& ctx, const Geometry* geom) override;
    
    // 热力图特有属性
    void SetRadius(double radius);
    void SetIntensity(double intensity);
    void SetColorGradient(const std::vector<ColorStop>& stops);
    
private:
    double radius_ = 25.0;
    double intensity_ = 1.0;
    std::vector<ColorStop> colorStops_;
};

// 2. 注册到符号化器工厂
SymbolizerFactory::Register(SymbolizerType::kHeatmap, 
    []() { return HeatmapSymbolizer::Create(); });
```

### 17.3 扩展新输出设备

```cpp
// 1. 定义新设备类
class SvgDevice : public DrawDevice {
public:
    static std::unique_ptr<SvgDevice> Create(int width, int height);
    
    // 实现基类接口
    DeviceType GetType() const override { return DeviceType::kSvg; }
    void BeginDraw(const DrawParams& params) override;
    void EndDraw() override;
    bool IsValid() const override;
    
    // 绘制方法
    void DrawPoint(const Point& pt, const PointStyle& style) override;
    void DrawLineString(const LineString* line, const LineStyle& style) override;
    void DrawPolygon(const Polygon* polygon, const PolygonStyle& style) override;
    void DrawText(const Point& anchor, const std::string& text, 
                  const TextStyle& style) override;
    void DrawRaster(const RasterData* raster, const RasterStyle& style) override;
    
    // SVG特有方法
    void SaveToFile(const std::string& filepath);
    std::string GetSvgString();
    
private:
    std::stringstream svgContent_;
};

// 2. 注册到设备工厂
DeviceFactory::Register(DeviceType::kSvg, 
    [](int w, int h) { return SvgDevice::Create(w, h); });
```

### 17.4 扩展新数据源

```cpp
// 1. 定义新图层类
class PostGISLayer : public CNLayer {
public:
    // 连接参数
    struct ConnectionParams {
        std::string host;
        int port;
        std::string database;
        std::string user;
        std::string password;
        std::string table;
        std::string geomColumn;
    };
    
    static std::unique_ptr<PostGISLayer> Create(const ConnectionParams& params);
    
    // 实现基类接口
    CNLayerType GetType() const override { return CNLayerType::kPostGIS; }
    bool Open() override;
    void Close() override;
    CNFeature* GetNextFeature() override;
    void ResetReading() override;
    void SetSpatialFilterRect(double minX, double minY, 
                               double maxX, double maxY) override;
    
private:
    ConnectionParams params_;
    PGconn* pgConn_ = nullptr;
    PGresult* currentResult_ = nullptr;
    int currentRow_ = 0;
};
```

### 17.5 扩展渲染顺序控制

```cpp
// 渲染顺序配置
struct RenderOrderConfig {
    std::string layerName;
    int zIndex;                 // 渲染层级
    bool visible;               // 是否可见
    double minScale;            // 最小显示比例尺
    double maxScale;            // 最大显示比例尺
};

class RenderOrderManager {
public:
    void AddLayer(const std::string& name, int zIndex);
    void SetLayerVisible(const std::string& name, bool visible);
    void SetLayerScaleRange(const std::string& name, 
                            double minScale, double maxScale);
    
    std::vector<std::string> GetRenderOrder(double currentScale);
    
private:
    std::map<std::string, RenderOrderConfig> layers_;
    std::vector<std::string> sortedOrder_;
};

// 使用示例
RenderOrderManager orderMgr;
orderMgr.AddLayer("polygon_layer", 1);   // 底层
orderMgr.AddLayer("line_layer", 2);      // 中层
orderMgr.AddLayer("point_layer", 3);     // 顶层
orderMgr.AddLayer("annotation_layer", 4); // 最顶层

auto renderOrder = orderMgr.GetRenderOrder(currentScale);
for (const auto& layerName : renderOrder) {
    RenderLayer(ctx, layers[layerName]);
}
```

---

## 附录

### A. 相关类索引

| 类名 | 头文件路径 | 说明 |
|------|-----------|------|
| Geometry | [geometry.h](file:///f:/cycle/trae/chart/code/geom/include/ogc/geometry.h) | 几何基类 |
| Point | [point.h](file:///f:/cycle/trae/chart/code/geom/include/ogc/point.h) | 点几何类 |
| LineString | [linestring.h](file:///f:/cycle/trae/chart/code/geom/include/ogc/linestring.h) | 线几何类 |
| Polygon | [polygon.h](file:///f:/cycle/trae/chart/code/geom/include/ogc/polygon.h) | 区几何类 |
| CNFeature | [feature.h](file:///f:/cycle/trae/chart/code/feature/include/ogc/feature/feature.h) | 要素类 |
| CNFieldValue | [field_value.h](file:///f:/cycle/trae/chart/code/feature/include/ogc/feature/field_value.h) | 字段值类 |
| CNLayer | [layer.h](file:///f:/cycle/trae/chart/code/layer/include/ogc/layer/layer.h) | 图层基类 |
| CNRasterLayer | [raster_layer.h](file:///f:/cycle/trae/chart/code/layer/include/ogc/layer/raster_layer.h) | 栅格图层类 |
| SpatiaLiteConnection | [sqlite_connection.h](file:///f:/cycle/trae/chart/code/database/include/ogc/db/sqlite_connection.h) | SQLite连接类 |
| Symbolizer | [symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/symbolizer.h) | 符号化器基类 |
| PointSymbolizer | [point_symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/point_symbolizer.h) | 点符号化器 |
| LineSymbolizer | [line_symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/line_symbolizer.h) | 线符号化器 |
| PolygonSymbolizer | [polygon_symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/polygon_symbolizer.h) | 区符号化器 |
| TextSymbolizer | [text_symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/text_symbolizer.h) | 文本符号化器 |
| RasterSymbolizer | [raster_symbolizer.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/raster_symbolizer.h) | 栅格符号化器 |
| DrawDevice | [draw_device.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/draw_device.h) | 绘图设备基类 |
| RasterImageDevice | [raster_image_device.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/raster_image_device.h) | 栅格图像设备 |
| PdfDevice | [pdf_device.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/pdf_device.h) | PDF设备 |
| DisplayDevice | [display_device.h](file:///f:/cycle/trae/chart/code/graph/include/ogc/draw/display_device.h) | 显示设备 |

### B. 枚举类型汇总

| 枚举名 | 定义文件 | 取值范围 |
|--------|---------|---------|
| GeomType | common.h | kPoint, kLineString, kPolygon, kMultiPoint, kMultiLineString, kMultiPolygon, ... |
| CNFieldType | field_type.h | kInteger, kInteger64, kReal, kString, kDateTime, kBinary, ... |
| CNLayerType | layer_type.h | kVector, kRaster, kMemory, kPostGIS, kGeoPackage, ... |
| DeviceType | device_type.h | kUnknown, kRasterImage, kPdf, kTile, kDisplay, kSvg, kPrinter |
| SymbolizerType | symbolizer.h | kPoint, kLine, kPolygon, kText, kRaster, kComposite, kIcon |
| PointSymbolType | point_symbolizer.h | kCircle, kSquare, kTriangle, kStar, kCross, kDiamond |
| DashStyle | line_symbolizer.h | kSolid, kDash, kDot, kDashDot, kDashDotDot, kCustom |
| FillPattern | polygon_symbolizer.h | kSolid, kHorizontal, kVertical, kCross, kDiagonal, kDiagonalCross, kDot, kCustom |
| TextHorizontalAlignment | text_symbolizer.h | kLeft, kCenter, kRight |
| TextVerticalAlignment | text_symbolizer.h | kTop, kMiddle, kBottom |
| TextPlacement | text_symbolizer.h | kPoint, kLine, kInterior |
| RasterChannelSelection | raster_symbolizer.h | kRGB, kGrayscale, kPseudoColor, kCustom |
| CNDataType | raster_layer.h | kByte, kUInt16, kInt16, kUInt32, kInt32, kFloat32, kFloat64, ... |
| ImageFormat | raster_image_device.h | kPNG, kJPEG, kBMP, kTIFF, kGIF, kWebP |

---

## 18. 几何有效性验证规则

### 18.1 验证规则概述

几何有效性验证是GIS数据质量保证的基础环节。不同几何类型有不同的有效性标准，遵循OGC Simple Features规范。

### 18.2 各类型有效性规则

#### 18.2.1 点(Point)有效性规则

| 规则ID | 规则描述 | 验证方法 | 错误码 |
|--------|----------|----------|--------|
| P-001 | X坐标值有效（非NaN、非Inf） | `std::isfinite(x)` | kCoordinateInvalid |
| P-002 | Y坐标值有效（非NaN、非Inf） | `std::isfinite(y)` | kCoordinateInvalid |
| P-003 | Z坐标值有效（如果存在） | `std::isfinite(z)` | kCoordinateInvalid |
| P-004 | M坐标值有效（如果存在） | `std::isfinite(m)` | kCoordinateInvalid |
| P-005 | 坐标范围在合理区间内 | 检查坐标范围 | kCoordinateOutOfRange |

```cpp
bool Point::IsValid() const {
    if (!std::isfinite(x_) || !std::isfinite(y_)) {
        return false;
    }
    if (hasZ_ && !std::isfinite(z_)) {
        return false;
    }
    if (hasM_ && !std::isfinite(m_)) {
        return false;
    }
    return true;
}

GeometryValidationResult Point::Validate() const {
    GeometryValidationResult result;
    result.isValid = true;
    
    if (!std::isfinite(x_)) {
        result.AddError("P-001", "X coordinate is invalid (NaN or Inf)");
    }
    if (!std::isfinite(y_)) {
        result.AddError("P-002", "Y coordinate is invalid (NaN or Inf)");
    }
    if (hasZ_ && !std::isfinite(z_)) {
        result.AddError("P-003", "Z coordinate is invalid (NaN or Inf)");
    }
    if (hasM_ && !std::isfinite(m_)) {
        result.AddError("P-004", "M coordinate is invalid (NaN or Inf)");
    }
    
    result.isValid = result.errors.empty();
    return result;
}
```

#### 18.2.2 线(LineString)有效性规则

| 规则ID | 规则描述 | 验证方法 | 错误码 |
|--------|----------|----------|--------|
| L-001 | 至少包含2个点 | `coordinates.size() >= 2` | kInsufficientPoints |
| L-002 | 所有点坐标有效 | 遍历验证每个点 | kCoordinateInvalid |
| L-003 | 无连续重复点 | 检查相邻点是否相同 | kDuplicatePoints |
| L-004 | 闭合线首尾点相同（如适用） | 检查首尾点 | kUnclosedRing |

```cpp
bool LineString::IsValid() const {
    if (coordinates_.size() < 2) {
        return false;
    }
    
    for (const auto& pt : coordinates_) {
        if (!std::isfinite(pt.x) || !std::isfinite(pt.y)) {
            return false;
        }
    }
    
    for (size_t i = 1; i < coordinates_.size(); ++i) {
        if (coordinates_[i] == coordinates_[i-1]) {
            return false;
        }
    }
    
    return true;
}

GeometryValidationResult LineString::Validate() const {
    GeometryValidationResult result;
    result.isValid = true;
    
    if (coordinates_.size() < 2) {
        result.AddError("L-001", "LineString must have at least 2 points");
    }
    
    for (size_t i = 0; i < coordinates_.size(); ++i) {
        const auto& pt = coordinates_[i];
        if (!std::isfinite(pt.x) || !std::isfinite(pt.y)) {
            result.AddError("L-002", "Invalid coordinate at index " + std::to_string(i));
        }
    }
    
    for (size_t i = 1; i < coordinates_.size(); ++i) {
        if (coordinates_[i] == coordinates_[i-1]) {
            result.AddError("L-003", "Duplicate consecutive points at index " + std::to_string(i));
        }
    }
    
    result.isValid = result.errors.empty();
    return result;
}
```

#### 18.2.3 区(Polygon)有效性规则

| 规则ID | 规则描述 | 验证方法 | 错误码 |
|--------|----------|----------|--------|
| PG-001 | 外环有效且闭合 | 验证外环 | kInvalidExteriorRing |
| PG-002 | 外环至少4个点（闭合） | `exterior.size() >= 4` | kInsufficientPoints |
| PG-003 | 外环首尾点相同 | 检查首尾点 | kUnclosedRing |
| PG-004 | 外环无自相交 | 使用拓扑检查 | kSelfIntersection |
| PG-005 | 内环在外环内部 | 拓扑包含检查 | kInteriorOutsideExterior |
| PG-006 | 内环之间不相交 | 拓扑相交检查 | kInteriorRingsIntersect |
| PG-007 | 内环有效且闭合 | 验证每个内环 | kInvalidInteriorRing |

```cpp
bool Polygon::IsValid() const {
    if (!exteriorRing_ || exteriorRing_->size() < 4) {
        return false;
    }
    
    if (!exteriorRing_->IsClosed()) {
        return false;
    }
    
    if (!exteriorRing_->IsValid()) {
        return false;
    }
    
    if (exteriorRing_->IsSelfIntersecting()) {
        return false;
    }
    
    for (const auto& interior : interiorRings_) {
        if (!interior->IsClosed() || !interior->IsValid()) {
            return false;
        }
        if (!exteriorRing_->Contains(interior.get())) {
            return false;
        }
    }
    
    for (size_t i = 0; i < interiorRings_.size(); ++i) {
        for (size_t j = i + 1; j < interiorRings_.size(); ++j) {
            if (interiorRings_[i]->Intersects(interiorRings_[j].get())) {
                return false;
            }
        }
    }
    
    return true;
}

GeometryValidationResult Polygon::Validate() const {
    GeometryValidationResult result;
    result.isValid = true;
    
    if (!exteriorRing_) {
        result.AddError("PG-001", "Polygon has no exterior ring");
        result.isValid = false;
        return result;
    }
    
    if (exteriorRing_->size() < 4) {
        result.AddError("PG-002", "Exterior ring must have at least 4 points");
    }
    
    if (!exteriorRing_->IsClosed()) {
        result.AddError("PG-003", "Exterior ring is not closed");
    }
    
    if (exteriorRing_->IsSelfIntersecting()) {
        result.AddError("PG-004", "Exterior ring self-intersects");
    }
    
    for (size_t i = 0; i < interiorRings_.size(); ++i) {
        const auto& interior = interiorRings_[i];
        if (!interior->IsClosed()) {
            result.AddError("PG-007", "Interior ring " + std::to_string(i) + " is not closed");
        }
        if (!exteriorRing_->Contains(interior.get())) {
            result.AddError("PG-005", "Interior ring " + std::to_string(i) + " is outside exterior");
        }
    }
    
    for (size_t i = 0; i < interiorRings_.size(); ++i) {
        for (size_t j = i + 1; j < interiorRings_.size(); ++j) {
            if (interiorRings_[i]->Intersects(interiorRings_[j].get())) {
                result.AddError("PG-006", "Interior rings " + std::to_string(i) + 
                    " and " + std::to_string(j) + " intersect");
            }
        }
    }
    
    result.isValid = result.errors.empty();
    return result;
}
```

### 18.3 验证结果结构

```cpp
struct ValidationError {
    std::string code;
    std::string message;
    std::string location;
    
    ValidationError(const std::string& c, const std::string& m, const std::string& l = "")
        : code(c), message(m), location(l) {}
};

struct GeometryValidationResult {
    bool isValid;
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    
    void AddError(const std::string& code, const std::string& message, 
                  const std::string& location = "") {
        errors.emplace_back(code, message, location);
    }
    
    void AddWarning(const std::string& code, const std::string& message,
                    const std::string& location = "") {
        warnings.emplace_back(code, message, location);
    }
    
    std::string ToString() const {
        std::stringstream ss;
        ss << "Valid: " << (isValid ? "Yes" : "No") << "\n";
        if (!errors.empty()) {
            ss << "Errors:\n";
            for (const auto& e : errors) {
                ss << "  [" << e.code << "] " << e.message;
                if (!e.location.empty()) {
                    ss << " at " << e.location;
                }
                ss << "\n";
            }
        }
        if (!warnings.empty()) {
            ss << "Warnings:\n";
            for (const auto& w : warnings) {
                ss << "  [" << w.code << "] " << w.message;
                if (!w.location.empty()) {
                    ss << " at " << w.location;
                }
                ss << "\n";
            }
        }
        return ss.str();
    }
};
```

### 18.4 验证时机

| 阶段 | 验证操作 | 处理策略 |
|------|----------|----------|
| 数据生成 | 生成后立即验证 | 无效数据重新生成 |
| 数据入库 | 入库前验证 | 拒绝无效数据，记录日志 |
| 数据读取 | 读取后验证 | 标记无效数据，跳过渲染 |
| 渲染前 | 渲染前验证 | 跳过无效几何，记录警告 |

```cpp
RenderResult ProcessFeature(CNFeature* feature) {
    auto* geom = feature->GetGeometry();
    if (!geom) {
        return RenderResult::Error(
            RenderErrorCode::kGeometryNull,
            "Feature has no geometry",
            "ProcessFeature");
    }
    
    auto validationResult = geom->Validate();
    if (!validationResult.isValid) {
        std::string errorMsg = validationResult.ToString();
        LOG_WARN("Invalid geometry: " + errorMsg);
        return RenderResult::Error(
            RenderErrorCode::kGeometryInvalid,
            "Geometry validation failed: " + errorMsg,
            "ProcessFeature");
    }
    
    return RenderResult::Success();
}
```

---

## 19. 数据库兼容性与并发控制

### 19.1 SpatiaLite版本兼容性

#### 19.1.1 版本要求

| 组件 | 最低版本 | 推荐版本 | 说明 |
|------|----------|----------|------|
| SQLite | 3.8.0+ | 3.35.0+ | 核心数据库引擎 |
| SpatiaLite | 4.3.0+ | 5.0.0+ | 空间扩展模块 |
| PROJ | 6.0+ | 8.0+ | 坐标系转换库 |
| GEOS | 3.6+ | 3.10+ | 几何操作库 |

#### 19.1.2 版本差异说明

| 功能 | SpatiaLite 4.x | SpatiaLite 5.x | 兼容处理 |
|------|----------------|----------------|----------|
| 空间索引创建 | `CreateSpatialIndex()` | `CreateSpatialIndex()` | 相同 |
| 几何列添加 | `AddGeometryColumn()` | `AddGeometryColumn()` | 相同 |
| 坐标系支持 | EPSG 4326/3857 | 扩展支持更多 | 检测可用SRID |
| GeoPackage | 部分支持 | 完整支持 | 条件编译 |
| RTTOPO | 不支持 | 支持3D拓扑 | 可选功能 |

#### 19.1.3 版本检测代码

```cpp
struct SpatiaLiteVersion {
    int major;
    int minor;
    int patch;
    std::string versionString;
    
    static SpatiaLiteVersion Detect(sqlite3* db) {
        SpatiaLiteVersion version = {0, 0, 0, ""};
        
        char* errMsg = nullptr;
        char** result = nullptr;
        int rows = 0, cols = 0;
        
        int rc = sqlite3_get_table(db,
            "SELECT spatialite_version();",
            &result, &rows, &cols, &errMsg);
        
        if (rc == SQLITE_OK && rows > 0 && cols > 0) {
            version.versionString = result[cols];
            sscanf(version.versionString.c_str(), "%d.%d.%d",
                   &version.major, &version.minor, &version.patch);
        }
        
        sqlite3_free_table(result);
        return version;
    }
    
    bool IsAtLeast(int maj, int min, int pat) const {
        if (major > maj) return true;
        if (major == maj && minor > min) return true;
        if (major == maj && minor == min && patch >= pat) return true;
        return false;
    }
    
    bool SupportsGeoPackage() const {
        return IsAtLeast(5, 0, 0);
    }
    
    bool SupportsRTopo() const {
        return IsAtLeast(5, 0, 0);
    }
};

bool CheckDatabaseCompatibility(SpatiaLiteConnection& conn) {
    auto version = SpatiaLiteVersion::Detect(conn.GetNativeHandle());
    
    if (!version.IsAtLeast(4, 3, 0)) {
        LOG_ERROR("SpatiaLite version too old: " + version.versionString);
        return false;
    }
    
    LOG_INFO("SpatiaLite version: " + version.versionString);
    
    if (version.SupportsGeoPackage()) {
        LOG_INFO("GeoPackage support available");
    }
    
    return true;
}
```

### 19.2 并发访问控制策略

#### 19.2.1 SQLite并发限制

| 模式 | 读并发 | 写并发 | 适用场景 |
|------|--------|--------|----------|
| 默认模式 | 多读 | 单写(阻塞) | 单线程应用 |
| WAL模式 | 多读 | 单写(非阻塞) | 多线程读多写少 |
| EXCLUSIVE模式 | 单读 | 单写 | 批量导入 |

#### 19.2.2 推荐配置

```cpp
class DatabaseConfig {
public:
    static void ConfigureForConcurrency(sqlite3* db) {
        char* errMsg = nullptr;
        
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA cache_size=-64000;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA busy_timeout=5000;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", nullptr, nullptr, &errMsg);
    }
    
    static void ConfigureForBatchImport(sqlite3* db) {
        char* errMsg = nullptr;
        
        sqlite3_exec(db, "PRAGMA journal_mode=OFF;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA synchronous=OFF;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA cache_size=-256000;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA locking_mode=EXCLUSIVE;", nullptr, nullptr, &errMsg);
    }
    
    static void ConfigureForQuery(sqlite3* db) {
        char* errMsg = nullptr;
        
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA cache_size=-128000;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA read_uncommitted=ON;", nullptr, nullptr, &errMsg);
    }
};
```

#### 19.2.3 连接池实现

```cpp
class SQLiteConnectionPool {
public:
    struct Config {
        std::string dbPath;
        int minConnections = 2;
        int maxConnections = 10;
        int connectionTimeout = 5000;
        int idleTimeout = 300000;
    };
    
    static std::unique_ptr<SQLiteConnectionPool> Create(const Config& config) {
        return std::unique_ptr<SQLiteConnectionPool>(new SQLiteConnectionPool(config));
    }
    
    std::shared_ptr<sqlite3> Acquire(int timeoutMs = 5000) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        auto now = std::chrono::steady_clock::now();
        auto deadline = now + std::chrono::milliseconds(timeoutMs);
        
        while (available_.empty() && activeCount_ >= config_.maxConnections) {
            if (cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
                throw std::runtime_error("Connection pool timeout");
            }
        }
        
        if (!available_.empty()) {
            auto conn = available_.front();
            available_.pop();
            lastUsed_[conn.get()] = std::chrono::steady_clock::now();
            return conn;
        }
        
        auto conn = CreateConnection();
        activeCount_++;
        lastUsed_[conn.get()] = std::chrono::steady_clock::now();
        return conn;
    }
    
    void Release(std::shared_ptr<sqlite3> conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (available_.size() < config_.minConnections) {
            available_.push(conn);
            cv_.notify_one();
        } else {
            activeCount_--;
            conn.reset();
            cv_.notify_one();
        }
    }
    
    size_t ActiveCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return activeCount_;
    }
    
    size_t AvailableCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return available_.size();
    }
    
private:
    SQLiteConnectionPool(const Config& config) : config_(config), activeCount_(0) {
        for (int i = 0; i < config_.minConnections; ++i) {
            available_.push(CreateConnection());
        }
    }
    
    std::shared_ptr<sqlite3> CreateConnection() {
        sqlite3* db = nullptr;
        int rc = sqlite3_open_v2(config_.dbPath.c_str(), &db,
                                  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                                  SQLITE_OPEN_NOMUTEX, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to open database");
        }
        
        DatabaseConfig::ConfigureForConcurrency(db);
        
        return std::shared_ptr<sqlite3>(db, [](sqlite3* p) {
            if (p) sqlite3_close(p);
        });
    }
    
    Config config_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::shared_ptr<sqlite3>> available_;
    std::map<sqlite3*, std::chrono::steady_clock::time_point> lastUsed_;
    int activeCount_;
};
```

#### 19.2.4 读写锁策略

```cpp
enum class LockType {
    kRead,
    kWrite
};

class DatabaseLockManager {
public:
    std::shared_ptr<sqlite3> AcquireReadLock(SQLiteConnectionPool& pool) {
        return pool.Acquire();
    }
    
    std::shared_ptr<sqlite3> AcquireWriteLock(SQLiteConnectionPool& pool) {
        auto conn = pool.Acquire();
        
        char* errMsg = nullptr;
        int rc = sqlite3_exec(conn.get(), "BEGIN IMMEDIATE;", nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            pool.Release(conn);
            throw std::runtime_error("Failed to acquire write lock");
        }
        
        return conn;
    }
    
    void ReleaseReadLock(SQLiteConnectionPool& pool, std::shared_ptr<sqlite3> conn) {
        pool.Release(conn);
    }
    
    void ReleaseWriteLock(SQLiteConnectionPool& pool, std::shared_ptr<sqlite3> conn, bool commit) {
        char* errMsg = nullptr;
        if (commit) {
            sqlite3_exec(conn.get(), "COMMIT;", nullptr, nullptr, &errMsg);
        } else {
            sqlite3_exec(conn.get(), "ROLLBACK;", nullptr, nullptr, &errMsg);
        }
        pool.Release(conn);
    }
    
private:
    std::mutex writeMutex_;
};

template<typename Func>
auto WithReadLock(SQLiteConnectionPool& pool, Func&& func) 
    -> decltype(func(std::declval<sqlite3*>())) {
    auto conn = pool.Acquire();
    auto guard = std::shared_ptr<sqlite3>(conn.get(), [&pool, conn](sqlite3*) mutable {
        pool.Release(conn);
    });
    return func(conn.get());
}

template<typename Func>
auto WithWriteLock(SQLiteConnectionPool& pool, Func&& func)
    -> decltype(func(std::declval<sqlite3*>())) {
    auto conn = pool.Acquire();
    char* errMsg = nullptr;
    sqlite3_exec(conn.get(), "BEGIN IMMEDIATE;", nullptr, nullptr, &errMsg);
    
    try {
        auto result = func(conn.get());
        sqlite3_exec(conn.get(), "COMMIT;", nullptr, nullptr, &errMsg);
        pool.Release(conn);
        return result;
    } catch (...) {
        sqlite3_exec(conn.get(), "ROLLBACK;", nullptr, nullptr, &errMsg);
        pool.Release(conn);
        throw;
    }
}
```

### 19.3 线程安全最佳实践

| 场景 | 推荐方案 | 注意事项 |
|------|----------|----------|
| 多线程读取 | WAL模式 + 连接池 | 每线程独立连接 |
| 多线程写入 | 单写线程队列 | 批量写入提高效率 |
| 读写混合 | 读写分离连接 | 写连接独占 |
| 批量导入 | EXCLUSIVE模式 | 禁用日志和同步 |

```cpp
class ThreadSafeDatabase {
public:
    ThreadSafeDatabase(const std::string& dbPath) {
        SQLiteConnectionPool::Config config;
        config.dbPath = dbPath;
        config.minConnections = 2;
        config.maxConnections = 8;
        pool_ = SQLiteConnectionPool::Create(config);
    }
    
    std::vector<CNFeature> QueryFeatures(const std::string& tableName,
                                          const DisplayExtent& extent) {
        return WithReadLock(*pool_, [&](sqlite3* db) {
            std::vector<CNFeature> features;
            std::string sql = BuildSpatialQuery(tableName, extent);
            
            sqlite3_stmt* stmt = nullptr;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                features.push_back(ParseFeature(stmt));
            }
            
            sqlite3_finalize(stmt);
            return features;
        });
    }
    
    bool InsertFeatures(const std::string& tableName,
                         const std::vector<CNFeature>& features) {
        return WithWriteLock(*pool_, [&](sqlite3* db) {
            std::string sql = BuildInsertSQL(tableName);
            
            sqlite3_stmt* stmt = nullptr;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            
            for (const auto& feature : features) {
                BindFeature(stmt, feature);
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    sqlite3_finalize(stmt);
                    return false;
                }
                sqlite3_reset(stmt);
            }
            
            sqlite3_finalize(stmt);
            return true;
        });
    }
    
private:
    std::unique_ptr<SQLiteConnectionPool> pool_;
};
```

---

## 20. 性能监控与内存检测

### 20.1 性能监控指标定义

#### 20.1.1 核心监控指标

| 指标类别 | 指标名称 | 单位 | 采集方式 | 目标值 |
|----------|----------|------|----------|--------|
| **渲染性能** | 帧率(FPS) | fps | 时间戳差值 | ≥ 30 |
| | 帧时间 | ms | 单帧渲染耗时 | ≤ 33ms |
| | Draw Call次数 | 次 | 渲染调用计数 | ≤ 100 |
| **查询性能** | 查询响应时间 | ms | SQL执行耗时 | ≤ 50ms |
| | 空间过滤时间 | ms | 过滤操作耗时 | ≤ 20ms |
| | 结果集大小 | 条 | 返回记录数 | - |
| **内存性能** | 内存使用峰值 | MB | 进程内存 | ≤ 200MB |
| | 内存增长率 | MB/s | 内存变化速率 | ≤ 1MB/s |
| | 对象池命中率 | % | 复用次数/总请求 | ≥ 80% |
| **数据库性能** | 连接池使用率 | % | 活跃连接/总连接 | ≤ 80% |
| | 查询缓存命中率 | % | 缓存命中/总查询 | ≥ 60% |
| | 事务平均耗时 | ms | 事务执行时间 | ≤ 10ms |

#### 20.1.2 指标采集实现

```cpp
struct PerformanceMetrics {
    struct RenderMetrics {
        double fps;
        double frameTimeMs;
        int drawCallCount;
        std::chrono::steady_clock::time_point lastFrameTime;
        int frameCount;
        
        void RecordFrame() {
            auto now = std::chrono::steady_clock::now();
            frameCount++;
            
            if (lastFrameTime.time_since_epoch().count() > 0) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - lastFrameTime);
                frameTimeMs = duration.count();
                fps = 1000.0 / frameTimeMs;
            }
            
            lastFrameTime = now;
        }
        
        void Reset() {
            fps = 0;
            frameTimeMs = 0;
            drawCallCount = 0;
            frameCount = 0;
        }
    };
    
    struct QueryMetrics {
        double avgResponseTimeMs;
        double maxResponseTimeMs;
        int queryCount;
        int64_t totalResultCount;
        
        void RecordQuery(double timeMs, int resultCount) {
            queryCount++;
            totalResultCount += resultCount;
            avgResponseTimeMs = (avgResponseTimeMs * (queryCount - 1) + timeMs) / queryCount;
            maxResponseTimeMs = std::max(maxResponseTimeMs, timeMs);
        }
    };
    
    struct MemoryMetrics {
        size_t currentUsageMB;
        size_t peakUsageMB;
        double growthRateMBps;
        double poolHitRate;
        
        void Update() {
            size_t current = GetCurrentMemoryUsage();
            currentUsageMB = current / (1024 * 1024);
            peakUsageMB = std::max(peakUsageMB, currentUsageMB);
        }
        
    private:
        size_t GetCurrentMemoryUsage() {
#ifdef _WIN32
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
                return pmc.WorkingSetSize;
            }
#else
            FILE* file = fopen("/proc/self/status", "r");
            if (file) {
                char line[128];
                while (fgets(line, sizeof(line), file)) {
                    if (strncmp(line, "VmRSS:", 6) == 0) {
                        size_t kb;
                        sscanf(line + 6, "%zu", &kb);
                        fclose(file);
                        return kb * 1024;
                    }
                }
                fclose(file);
            }
#endif
            return 0;
        }
    };
    
    struct DatabaseMetrics {
        double connectionPoolUsage;
        double cacheHitRate;
        double avgTransactionTimeMs;
        int activeConnections;
        int totalConnections;
        
        void Update(SQLiteConnectionPool& pool) {
            activeConnections = pool.ActiveCount();
            totalConnections = activeConnections + pool.AvailableCount();
            connectionPoolUsage = (double)activeConnections / totalConnections * 100;
        }
    };
    
    RenderMetrics render;
    QueryMetrics query;
    MemoryMetrics memory;
    DatabaseMetrics database;
    
    void Reset() {
        render.Reset();
        query = {};
        memory = {};
        database = {};
    }
    
    std::string ToJson() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"render\": {\n";
        ss << "    \"fps\": " << render.fps << ",\n";
        ss << "    \"frameTimeMs\": " << render.frameTimeMs << ",\n";
        ss << "    \"drawCallCount\": " << render.drawCallCount << "\n";
        ss << "  },\n";
        ss << "  \"query\": {\n";
        ss << "    \"avgResponseTimeMs\": " << query.avgResponseTimeMs << ",\n";
        ss << "    \"maxResponseTimeMs\": " << query.maxResponseTimeMs << ",\n";
        ss << "    \"queryCount\": " << query.queryCount << "\n";
        ss << "  },\n";
        ss << "  \"memory\": {\n";
        ss << "    \"currentUsageMB\": " << memory.currentUsageMB << ",\n";
        ss << "    \"peakUsageMB\": " << memory.peakUsageMB << "\n";
        ss << "  },\n";
        ss << "  \"database\": {\n";
        ss << "    \"connectionPoolUsage\": " << database.connectionPoolUsage << ",\n";
        ss << "    \"activeConnections\": " << database.activeConnections << "\n";
        ss << "  }\n";
        ss << "}";
        return ss.str();
    }
};
```

#### 20.1.3 性能监控器

```cpp
class PerformanceMonitor {
public:
    static PerformanceMonitor& Instance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void StartFrame() {
        metrics_.render.RecordFrame();
        frameStartTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void EndFrame() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - frameStartTime_);
        metrics_.render.frameTimeMs = duration.count() / 1000.0;
        metrics_.render.fps = 1000.0 / metrics_.render.frameTimeMs;
    }
    
    void RecordDrawCall() {
        metrics_.render.drawCallCount++;
    }
    
    void RecordQuery(double timeMs, int resultCount) {
        metrics_.query.RecordQuery(timeMs, resultCount);
    }
    
    void UpdateMemory() {
        metrics_.memory.Update();
    }
    
    void UpdateDatabase(SQLiteConnectionPool& pool) {
        metrics_.database.Update(pool);
    }
    
    const PerformanceMetrics& GetMetrics() const {
        return metrics_;
    }
    
    void Reset() {
        metrics_.Reset();
    }
    
    void SetAlertCallback(std::function<void(const std::string&, double)> callback) {
        alertCallback_ = callback;
    }
    
    void CheckThresholds() {
        if (metrics_.render.fps < 30.0 && alertCallback_) {
            alertCallback_("fps_low", metrics_.render.fps);
        }
        if (metrics_.memory.currentUsageMB > 200 && alertCallback_) {
            alertCallback_("memory_high", metrics_.memory.currentUsageMB);
        }
        if (metrics_.query.avgResponseTimeMs > 50 && alertCallback_) {
            alertCallback_("query_slow", metrics_.query.avgResponseTimeMs);
        }
    }
    
private:
    PerformanceMonitor() = default;
    
    PerformanceMetrics metrics_;
    std::chrono::high_resolution_clock::time_point frameStartTime_;
    std::function<void(const std::string&, double)> alertCallback_;
};

#define PERF_START_FRAME() PerformanceMonitor::Instance().StartFrame()
#define PERF_END_FRAME() PerformanceMonitor::Instance().EndFrame()
#define PERF_DRAW_CALL() PerformanceMonitor::Instance().RecordDrawCall()
#define PERF_QUERY(time, count) PerformanceMonitor::Instance().RecordQuery(time, count)
```

### 20.2 内存泄漏检测方案

#### 20.2.1 检测工具选择

| 平台 | 推荐工具 | 检测能力 | 集成方式 |
|------|----------|----------|----------|
| Windows | Dr. Memory | 内存泄漏、越界访问 | 命令行运行 |
| Windows | Visual Studio诊断工具 | 实时内存分析 | IDE集成 |
| Linux | Valgrind (Memcheck) | 内存泄漏、越界、未初始化 | 命令行运行 |
| Linux | AddressSanitizer (ASan) | 内存错误检测 | 编译器选项 |
| macOS | Leaks | 内存泄漏检测 | 命令行/Instruments |
| 跨平台 |自定义追踪器 | 对象生命周期追踪 | 代码集成 |

#### 20.2.2 AddressSanitizer集成

```cmake
# CMakeLists.txt
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    
    message(STATUS "AddressSanitizer enabled")
endif()
```

```cpp
#ifdef __SANITIZE_ADDRESS__
#include <sanitizer/asan_interface.h>

void CheckMemoryCorruption(void* ptr, size_t size) {
    if (__asan_region_is_poisoned(ptr, size)) {
        __asan_report_error(ptr, size);
    }
}
#endif
```

#### 20.2.3 自定义内存追踪器

```cpp
class MemoryTracker {
public:
    struct AllocationInfo {
        size_t size;
        std::string file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
        bool freed;
    };
    
    static MemoryTracker& Instance() {
        static MemoryTracker instance;
        return instance;
    }
    
    void* TrackAllocate(size_t size, const char* file, int line) {
        void* ptr = malloc(size);
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            allocations_[ptr] = AllocationInfo{
                size, file, line, 
                std::chrono::steady_clock::now(), false
            };
            totalAllocated_ += size;
            currentAllocated_ += size;
            peakAllocated_ = std::max(peakAllocated_, currentAllocated_);
        }
        return ptr;
    }
    
    void TrackDeallocate(void* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(ptr);
        if (it != allocations_.end()) {
            if (it->second.freed) {
                LOG_ERROR("Double free detected at " + std::to_string(ptr));
            }
            it->second.freed = true;
            currentAllocated_ -= it->second.size;
        }
        free(ptr);
    }
    
    void ReportLeaks() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        bool hasLeaks = false;
        for (const auto& [ptr, info] : allocations_) {
            if (!info.freed) {
                hasLeaks = true;
                auto age = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - info.timestamp).count();
                
                LOG_ERROR("Memory leak: " + std::to_string(info.size) + 
                    " bytes at " + info.file + ":" + std::to_string(info.line) +
                    " (age: " + std::to_string(age) + "s)");
            }
        }
        
        if (!hasLeaks) {
            LOG_INFO("No memory leaks detected");
        }
        
        LOG_INFO("Memory stats: total=" + std::to_string(totalAllocated_) +
            " peak=" + std::to_string(peakAllocated_) +
            " current=" + std::to_string(currentAllocated_));
    }
    
    size_t GetCurrentAllocated() const { return currentAllocated_; }
    size_t GetPeakAllocated() const { return peakAllocated_; }
    size_t GetAllocationCount() const { return allocations_.size(); }
    
private:
    MemoryTracker() : totalAllocated_(0), currentAllocated_(0), peakAllocated_(0) {}
    
    std::mutex mutex_;
    std::map<void*, AllocationInfo> allocations_;
    size_t totalAllocated_;
    size_t currentAllocated_;
    size_t peakAllocated_;
};

#ifdef ENABLE_MEMORY_TRACKING
#define TRACK_NEW(size) MemoryTracker::Instance().TrackAllocate(size, __FILE__, __LINE__)
#define TRACK_DELETE(ptr) MemoryTracker::Instance().TrackDeallocate(ptr)

void* operator new(size_t size) {
    return TRACK_NEW(size);
}

void operator delete(void* ptr) noexcept {
    TRACK_DELETE(ptr);
}
#else
#define TRACK_NEW(size) malloc(size)
#define TRACK_DELETE(ptr) free(ptr)
#endif
```

#### 20.2.4 检测流程

| 阶段 | 检测内容 | 工具 | 验收标准 |
|------|----------|------|----------|
| 开发阶段 | 实时内存检测 | ASan/VS诊断 | 零内存错误 |
| 单元测试 | 泄漏检测 | 自定义追踪器 | 零泄漏 |
| 集成测试 | 长时间运行稳定性 | Valgrind/Dr.Memory | 无新增泄漏 |
| 发布前 | 全面内存检测 | 全部工具 | 零泄漏 |

```cpp
class MemoryTestGuard {
public:
    MemoryTestGuard() {
#ifdef ENABLE_MEMORY_TRACKING
        MemoryTracker::Instance();
#endif
    }
    
    ~MemoryTestGuard() {
#ifdef ENABLE_MEMORY_TRACKING
        MemoryTracker::Instance().ReportLeaks();
#endif
    }
};

#define MEMORY_TEST_GUARD() MemoryTestGuard _memoryGuard

void RunMemoryTest() {
    MEMORY_TEST_GUARD();
    
    {
        auto feature = std::make_unique<CNFeature>();
        auto geom = std::make_unique<Point>(100.0, 200.0);
        feature->SetGeometry(geom.get());
    }
    
    auto& tracker = MemoryTracker::Instance();
    assert(tracker.GetCurrentAllocated() == 0);
}
```

#### 20.2.5 智能指针使用规范

| 场景 | 推荐类型 | 说明 |
|------|----------|------|
| 独占所有权 | `std::unique_ptr<T>` | 单一所有者，自动释放 |
| 共享所有权 | `std::shared_ptr<T>` | 多个所有者，引用计数 |
| 观察引用 | `T*` 或 `std::weak_ptr<T>` | 不参与生命周期管理 |
| 工厂返回 | `std::unique_ptr<T>` | 可转换为shared_ptr |
| 容器存储 | `std::unique_ptr<T>` | 避免拷贝开销 |

```cpp
class GeometryFactory {
public:
    static std::unique_ptr<Geometry> CreatePoint(double x, double y) {
        return std::make_unique<Point>(x, y);
    }
    
    static std::unique_ptr<Geometry> CreateLineString(
        const std::vector<Coordinate>& coords) {
        return std::make_unique<LineString>(coords);
    }
    
    static std::unique_ptr<Geometry> CreatePolygon(
        std::unique_ptr<LinearRing> exterior,
        std::vector<std::unique_ptr<LinearRing>> interiors = {}) {
        return std::make_unique<Polygon>(std::move(exterior), std::move(interiors));
    }
};

class FeatureManager {
public:
    void AddFeature(std::unique_ptr<CNFeature> feature) {
        features_.push_back(std::move(feature));
    }
    
    CNFeature* GetFeature(size_t index) const {
        return index < features_.size() ? features_[index].get() : nullptr;
    }
    
    std::shared_ptr<CNFeature> ShareFeature(size_t index) {
        if (index >= features_.size()) return nullptr;
        return std::shared_ptr<CNFeature>(features_[index].get(), 
            [this, index](CNFeature*) {
                LOG_WARN("Shared feature " + std::to_string(index) + " released");
            });
    }
    
private:
    std::vector<std::unique_ptr<CNFeature>> features_;
};
```
