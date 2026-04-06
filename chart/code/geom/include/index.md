# Geom Module - Header Index

## Overview

Geom模块是OGC Simple Feature Access规范的C++11实现，提供完整的几何对象模型、空间操作和空间索引功能。模块支持2D/3D坐标、测量值(M)、多种几何类型及其组合。

**核心特性**:
- 完整的OGC SFAS 1.2.1几何类型层次结构
- WKT/WKB/GeoJSON/GML/KML格式序列化
- R-Tree/Quadtree/Grid空间索引
- 线程安全的对象池
- 访问者模式支持

---

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [common.h](ogc/common.h) | 公共定义 | `GeomType`, `Dimension`, `GeomResult` |
| [export.h](ogc/export.h) | DLL导出宏 | `OGC_GEOM_API` |
| [coordinate.h](ogc/coordinate.h) | 坐标结构 | `Coordinate`, `CoordinateList` |
| [envelope.h](ogc/envelope.h) | 2D包络 | `Envelope` |
| [envelope3d.h](ogc/envelope3d.h) | 3D包络 | `Envelope3D` |
| [geometry.h](ogc/geometry.h) | 几何基类 | `Geometry` |
| [point.h](ogc/point.h) | 点几何 | `Point` |
| [linestring.h](ogc/linestring.h) | 线串 | `LineString` |
| [linearring.h](ogc/linearring.h) | 线环 | `LinearRing` |
| [polygon.h](ogc/polygon.h) | 多边形 | `Polygon` |
| [multipoint.h](ogc/multipoint.h) | 多点 | `MultiPoint` |
| [multilinestring.h](ogc/multilinestring.h) | 多线串 | `MultiLineString` |
| [multipolygon.h](ogc/multipolygon.h) | 多多边形 | `MultiPolygon` |
| [geometrycollection.h](ogc/geometrycollection.h) | 几何集合 | `GeometryCollection` |
| [visitor.h](ogc/visitor.h) | 访问者模式 | `GeometryVisitor`, `GeometryConstVisitor` |
| [factory.h](ogc/factory.h) | 几何工厂 | `GeometryFactory` |
| [spatial_index.h](ogc/spatial_index.h) | 空间索引 | `RTree`, `Quadtree`, `GridIndex` |
| [geometry_pool.h](ogc/geometry_pool.h) | 对象池 | `ObjectPool`, `GeometryPool` |
| [precision.h](ogc/precision.h) | 精度模型 | `PrecisionModel` |
| [exception.h](ogc/exception.h) | 异常类 | `GeometryException` |
| [serialization_utils.h](ogc/serialization_utils.h) | 序列化工具 | `wkb::`, `geojson::` |

---

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

Envelope
    └── Envelope3D

GeometryVisitor (abstract)
    └── AreaCalculator

GeometryConstVisitor (abstract)
    └── AreaCalculator

ISpatialIndex<T> (interface)
    ├── RTree<T>
    ├── Quadtree<T>
    └── GridIndex<T>
```

---

## Core Classes Detail

### Geometry
**File**: [geometry.h](ogc/geometry.h)  
**Base**: None (abstract base)  
**Description**: 所有几何类型的抽象基类，定义OGC标准接口

**Key Methods**:
```cpp
virtual GeomType GetGeometryType() const noexcept = 0;
virtual Dimension GetDimension() const noexcept = 0;
virtual bool IsEmpty() const noexcept = 0;
virtual bool IsValid(std::string* reason = nullptr) const;
virtual Envelope GetEnvelope() const noexcept;
virtual Coordinate GetCentroid() const;
virtual GeometryPtr Clone() const = 0;
virtual std::string AsText(int precision = 10) const = 0;
virtual std::vector<uint8_t> AsBinary() const = 0;
```

### Point
**File**: [point.h](ogc/point.h)  
**Base**: `Geometry`  
**Description**: 表示二维或三维空间中的单个点

**Key Methods**:
```cpp
static PointPtr Create(double x, double y);
static PointPtr Create(double x, double y, double z);
Coordinate GetCoordinate() const noexcept;
double GetX() const noexcept;
double GetY() const noexcept;
double GetZ() const noexcept;
PointPtr Normalize() const;
```

### LineString
**File**: [linestring.h](ogc/linestring.h)  
**Base**: `Geometry`  
**Description**: 由一系列连续坐标点组成的线

**Key Methods**:
```cpp
static LineStringPtr Create(const CoordinateList& coords);
size_t GetNumPoints() const noexcept;
Coordinate GetPointN(size_t index) const;
double Length() const override;
Coordinate Interpolate(double distance) const;
LineStringPtr GetSubLine(double start, double end) const;
```

### LinearRing
**File**: [linearring.h](ogc/linearring.h)  
**Base**: `LineString`  
**Description**: 闭合线串，用于多边形边界

**Key Methods**:
```cpp
static LinearRingPtr Create(const CoordinateList& coords, bool autoClose = true);
bool IsClockwise() const;
bool IsCounterClockwise() const;
void ForceClockwise();
double Area() const;
bool ContainsPoint(const Coordinate& point) const;
std::vector<Triangle> Triangulate() const;
```

### Polygon
**File**: [polygon.h](ogc/polygon.h)  
**Base**: `Geometry`  
**Description**: 由外环和内环(孔洞)组成的多边形

**Key Methods**:
```cpp
static PolygonPtr Create(LinearRingPtr exteriorRing);
void SetExteriorRing(LinearRingPtr ring);
const LinearRing* GetExteriorRing() const noexcept;
void AddInteriorRing(LinearRingPtr ring);
size_t GetNumInteriorRings() const noexcept;
double Area() const override;
static PolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
static PolygonPtr CreateCircle(double cx, double cy, double radius, int segments = 32);
```

### MultiPoint / MultiLineString / MultiPolygon
**Base**: `Geometry`  
**Description**: 同类几何对象的集合

**Key Methods**:
```cpp
void AddPoint(PointPtr point);           // MultiPoint
void AddLineString(LineStringPtr line);  // MultiLineString
void AddPolygon(PolygonPtr polygon);     // MultiPolygon
size_t GetNumGeometries() const noexcept override;
const Geometry* GetGeometryN(size_t index) const override;
```

### GeometryCollection
**File**: [geometrycollection.h](ogc/geometrycollection.h)  
**Base**: `Geometry`  
**Description**: 异构几何对象集合

**Key Methods**:
```cpp
void AddGeometry(GeometryPtr geometry);
GeometryPtr GetGeometryNPtr(size_t index) const;
GeometryCollectionPtr Flatten() const;
```

### Envelope
**File**: [envelope.h](ogc/envelope.h)  
**Description**: 最小边界矩形(MBR)，用于空间索引

**Key Methods**:
```cpp
Envelope(double minX, double minY, double maxX, double maxY);
void ExpandToInclude(const Coordinate& coord);
bool Intersects(const Envelope& other) const noexcept;
bool Contains(const Coordinate& coord) const noexcept;
Coordinate GetCenter() const noexcept;
double GetArea() const noexcept;
```

### GeometryFactory
**File**: [factory.h](ogc/factory.h)  
**Description**: 几何对象工厂，支持多种格式解析

**Key Methods**:
```cpp
static GeometryFactory& GetInstance();
GeomResult FromWKT(const std::string& wkt, GeometryPtr& result);
GeomResult FromWKB(const std::vector<uint8_t>& wkb, GeometryPtr& result);
GeomResult FromGeoJSON(const std::string& json, GeometryPtr& result);
PointPtr CreatePoint(double x, double y);
PolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
```

### RTree<T>
**File**: [spatial_index.h](ogc/spatial_index.h)  
**Base**: `ISpatialIndex<T>`  
**Description**: R-Tree空间索引实现

**Key Methods**:
```cpp
GeomResult Insert(const Envelope& envelope, const T& item);
std::vector<T> Query(const Envelope& envelope) const;
std::vector<T> QueryNearest(const Coordinate& point, size_t k) const;
GeomResult SaveToFile(const std::string& filepath) const;
```

---

## Type Aliases

```cpp
// Unique pointer types
using GeometryPtr = std::unique_ptr<Geometry>;
using PointPtr = std::unique_ptr<Point>;
using LineStringPtr = std::unique_ptr<LineString>;
using LinearRingPtr = std::unique_ptr<LinearRing>;
using PolygonPtr = std::unique_ptr<Polygon>;
using MultiPointPtr = std::unique_ptr<MultiPoint>;
using MultiLineStringPtr = std::unique_ptr<MultiLineString>;
using MultiPolygonPtr = std::unique_ptr<MultiPolygon>;
using GeometryCollectionPtr = std::unique_ptr<GeometryCollection>;

// Shared pointer types
using GeometrySharedPtr = std::shared_ptr<Geometry>;
using PointSharedPtr = std::shared_ptr<Point>;
// ... (同上)

// Coordinate list
using CoordinateList = std::vector<Coordinate>;
```

---

## Enums

### GeomType
```cpp
enum class GeomType : uint8_t {
    kUnknown = 0,
    kPoint = 1,
    kLineString = 2,
    kPolygon = 3,
    kMultiPoint = 4,
    kMultiLineString = 5,
    kMultiPolygon = 6,
    kGeometryCollection = 7,
    // ... 扩展类型
};
```

### Dimension
```cpp
enum class Dimension : uint8_t {
    Unknown = 255,
    Point = 0,
    Curve = 1,
    Surface = 2
};
```

### GeomResult
```cpp
enum class GeomResult : int32_t {
    kSuccess = 0,
    kInvalidGeometry = 1,
    kEmptyGeometry = 2,
    kInvalidParameters = 100,
    kNotSupported = 200,
    // ... 更多错误码
};
```

---

## Dependency Graph

```
common.h ─────────────────────────────────────────────────┐
    │                                                      │
    ├──► coordinate.h ──► envelope.h ──► envelope3d.h     │
    │         │                  │                         │
    │         └──────────────────┼───────────────────────┐ │
    │                            │                       │ │
    └────────────────────────────┼───────────────────────┼─┘
                                 │                       │
                           geometry.h ◄──────────────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
    point.h               linestring.h              polygon.h
        │                        │                        │
        │                        └──► linearring.h ───────┘
        │                                 │
        ├──► multipoint.h                 │
        │                                 │
        ├──► multilinestring.h ◄──────────┘
        │
        └──► multipolygon.h ◄──────── polygon.h
                    │
                    └──► geometrycollection.h
                              │
                              ├──► visitor.h
                              ├──► factory.h
                              └──► spatial_index.h
```

---

## Quick Usage Examples

### 创建几何对象

```cpp
#include "ogc/factory.h"
#include "ogc/point.h"
#include "ogc/polygon.h"

using namespace ogc;

// 使用工厂创建
auto& factory = GeometryFactory::GetInstance();
GeometryPtr geom;
factory.FromWKT("POINT(116.4 39.9)", geom);

// 直接创建
auto point = Point::Create(116.4, 39.9);
auto rect = Polygon::CreateRectangle(0, 0, 100, 100);

// 创建多边形
auto ring = LinearRing::Create({{0,0}, {10,0}, {10,10}, {0,10}, {0,0}});
auto polygon = Polygon::Create(std::move(ring));
```

### 空间操作

```cpp
// 获取包络
Envelope env = polygon->GetEnvelope();

// 计算面积
double area = polygon->Area();

// 空间关系判断
if (polygon->Contains(point.get())) {
    // 点在多边形内
}

// 序列化
std::string wkt = polygon->AsText();
std::vector<uint8_t> wkb = polygon->AsBinary();
std::string geojson = polygon->AsGeoJSON();
```

### 空间索引

```cpp
#include "ogc/spatial_index.h"

RTree<int> index;
index.Insert(polygon->GetEnvelope(), 1);
index.Insert(point->GetEnvelope(), 2);

// 范围查询
auto results = index.Query(Envelope(0, 0, 50, 50));

// 最近邻查询
auto nearest = index.QueryNearest(Coordinate(25, 25), 5);
```

### 访问者模式

```cpp
#include "ogc/visitor.h"

class PrintVisitor : public GeometryConstVisitor {
public:
    void VisitPoint(const Point* p) override {
        std::cout << "Point: " << p->GetX() << ", " << p->GetY() << std::endl;
    }
    void VisitPolygon(const Polygon* p) override {
        std::cout << "Polygon area: " << p->Area() << std::endl;
    }
};

PrintVisitor visitor;
geometry->Apply(visitor);
```

---

## File Classification

| Category | Files |
|----------|-------|
| **Core** | common.h, coordinate.h, geometry.h, envelope.h |
| **Primitives** | point.h, linestring.h, linearring.h, polygon.h |
| **Collections** | multipoint.h, multilinestring.h, multipolygon.h, geometrycollection.h |
| **Utilities** | factory.h, visitor.h, precision.h, exception.h |
| **Index** | spatial_index.h |
| **Memory** | geometry_pool.h |
| **Serialization** | serialization_utils.h |
| **Export** | export.h |

---

**Generated**: 2026-04-06  
**Module Version**: v1.0  
**C++ Standard**: C++11
