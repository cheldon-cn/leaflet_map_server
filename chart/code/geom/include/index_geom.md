# Geometry Module - Header Index

## Overview

Geometry模块是OGC Simple Feature规范的核心实现，提供完整的几何对象模型。支持Point、LineString、Polygon及其Multi类型，包含空间分析、坐标转换、WKT/WKB序列化等功能。

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [geometry.h](ogc/geometry.h) | 几何基类定义 | `Geometry` |
| [point.h](ogc/point.h) | 点几何类 | `Point` |
| [linestring.h](ogc/linestring.h) | 线几何类 | `LineString` |
| [linearring.h](ogc/linearring.h) | 线环类 | `LinearRing` |
| [polygon.h](ogc/polygon.h) | 多边形类 | `Polygon` |
| [multipoint.h](ogc/multipoint.h) | 多点类 | `MultiPoint` |
| [multilinestring.h](ogc/multilinestring.h) | 多线类 | `MultiLineString` |
| [multipolygon.h](ogc/multipolygon.h) | 多多边形类 | `MultiPolygon` |
| [geometrycollection.h](ogc/geometrycollection.h) | 几何集合 | `GeometryCollection` |
| [coordinate.h](ogc/coordinate.h) | 坐标类 | `Coordinate` |
| [envelope.h](ogc/envelope.h) | 边界范围 | `Envelope` |
| [envelope3d.h](ogc/envelope3d.h) | 三维边界 | `Envelope3D` |
| [factory.h](ogc/factory.h) | 几何工厂 | `GeometryFactory` |
| [geometry_pool.h](ogc/geometry_pool.h) | 几何对象池 | `GeometryPool` |
| [precision.h](ogc/precision.h) | 精度模型 | `PrecisionModel` |
| [spatial_index.h](ogc/spatial_index.h) | 空间索引 | `ISpatialIndex`, `RTree` |
| [visitor.h](ogc/visitor.h) | 访问者模式 | `GeometryVisitor` |
| [exception.h](ogc/exception.h) | 异常类 | `GeometryException` |
| [common.h](ogc/common.h) | 公共定义 | `GeomType`, `Dimension` |
| [export.h](ogc/export.h) | 导出宏定义 | `OGC_GEOM_API` |

## Class Inheritance Diagram

```
Geometry (abstract base)
    ├── Point
    ├── LineString
    │   └── LinearRing
    ├── Polygon
    ├── MultiPoint
    ├── MultiLineString
    ├── MultiPolygon
    └── GeometryCollection
        ├── MultiPoint
        ├── MultiLineString
        └── MultiPolygon
```

## Core Classes Detail

### Geometry

**File**: [geometry.h](ogc/geometry.h)

**Base Classes**: None (abstract interface)

**Purpose**: 所有几何类型的抽象基类，定义OGC Simple Feature标准接口。

**Key Methods**:
- `GetGeometryType()` / `GetGeometryTypeString()` - 几何类型
- `GetDimension()` / `GetCoordinateDimension()` - 维度信息
- `IsEmpty()` / `IsValid()` / `IsSimple()` - 有效性检查
- `Is3D()` / `IsMeasured()` - 3D/测量值检查
- `GetSRID()` / `SetSRID()` - 空间参考
- `GetEnvelope()` / `GetCentroid()` - 边界和中心
- `GetNumCoordinates()` / `GetCoordinates()` - 坐标访问
- `Distance()` / `Area()` / `Length()` - 度量计算
- `Equals()` / `Intersects()` / `Contains()` - 空间关系
- `Intersection()` / `Union()` / `Difference()` - 空间运算
- `Buffer()` / `ConvexHull()` / `Simplify()` - 几何操作
- `AsText()` / `AsBinary()` / `AsGeoJSON()` - 序列化
- `Clone()` - 克隆几何

### Point

**File**: [point.h](ogc/point.h)

**Base Classes**: `Geometry`

**Purpose**: 点几何类，表示二维或三维空间中的单个点。

**Key Methods**:
- `Create(x, y)` / `Create(x, y, z)` - 工厂方法
- `GetX()` / `GetY()` / `GetZ()` / `GetM()` - 坐标访问
- `SetX()` / `SetY()` / `SetZ()` / `SetM()` - 坐标设置
- `Dot()` / `Cross()` - 向量运算
- `Normalize()` - 归一化

### LineString

**File**: [linestring.h](ogc/linestring.h)

**Base Classes**: `Geometry`

**Purpose**: 线几何类，由一系列坐标点组成。

**Key Methods**:
- `Create(coords)` - 工厂方法
- `GetNumPoints()` - 点数量
- `GetPointN()` / `GetCoordinateN()` - 点访问
- `SetPointN()` - 点设置
- `AddPoint()` - 添加点

### LinearRing

**File**: [linearring.h](ogc/linearring.h)

**Base Classes**: `LineString`

**Purpose**: 线环类，闭合的线串，用于多边形边界。

**Key Methods**:
- `Create(coords)` - 工厂方法
- `IsClosed()` - 是否闭合
- `IsClockwise()` - 是否顺时针
- `Reverse()` - 反转方向

### Polygon

**File**: [polygon.h](ogc/polygon.h)

**Base Classes**: `Geometry`

**Purpose**: 多边形类，由外环和零或多个内环组成。

**Key Methods**:
- `Create(exterior)` - 工厂方法
- `GetExteriorRing()` - 外环
- `GetNumInteriorRings()` / `GetInteriorRingN()` - 内环
- `AddInteriorRing()` - 添加内环

### Envelope

**File**: [envelope.h](ogc/envelope.h)

**Purpose**: 二维边界范围类。

**Key Methods**:
- `MinX()` / `MaxX()` / `MinY()` / `MaxY()` - 边界值
- `Width()` / `Height()` - 宽高
- `Center()` - 中心点
- `Intersects()` / `Contains()` - 空间关系
- `Expand()` / `Merge()` - 扩展操作

## Type Aliases

```cpp
using GeometryPtr = std::unique_ptr<Geometry>;
using PointPtr = std::unique_ptr<Point>;
using LineStringPtr = std::unique_ptr<LineString>;
using LinearRingPtr = std::unique_ptr<LinearRing>;
using PolygonPtr = std::unique_ptr<Polygon>;
using MultiPointPtr = std::unique_ptr<MultiPoint>;
using MultiLineStringPtr = std::unique_ptr<MultiLineString>;
using MultiPolygonPtr = std::unique_ptr<MultiPolygon>;
using GeometryCollectionPtr = std::unique_ptr<GeometryCollection>;

using CoordinateList = std::vector<Coordinate>;
```

## Enumerations

### GeomType

```cpp
enum class GeomType {
    kUnknown,
    kPoint,
    kLineString,
    kLinearRing,
    kPolygon,
    kMultiPoint,
    kMultiLineString,
    kMultiPolygon,
    kGeometryCollection
};
```

### Dimension

```cpp
enum class Dimension {
    Point = 0,
    Curve = 1,
    Surface = 2,
    Volume = 3
};
```

### GeomResult

```cpp
enum class GeomResult {
    kSuccess,
    kInvalidInput,
    kNotImplemented,
    kTopologyError,
    kFailure
};
```

## Dependencies

```
geom
  └── (no internal dependencies, standalone module)
```

## Quick Usage Examples

```cpp
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/polygon.h"
#include "ogc/factory.h"

// 创建点
auto point = Point::Create(116.4, 39.9);

// 创建线
CoordinateList coords = {{0,0}, {1,0}, {1,1}, {0,1}};
auto line = LineString::Create(coords);

// 创建多边形
auto ring = LinearRing::Create(coords);
auto polygon = Polygon::Create(std::move(ring));

// 空间运算
auto buffer = polygon->Buffer(10.0);
auto centroid = polygon->GetCentroid();

// 空间关系
if (point->Within(polygon.get())) {
    // 点在多边形内
}

// 序列化
std::string wkt = polygon->AsText();
// "POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))"

// 使用工厂
auto geom = GeometryFactory::FromWKT("POINT(116.4 39.9)");
```
