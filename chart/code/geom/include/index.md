# OGC Geometry Library - 头文件索引

## 概述

本目录包含 OGC Simple Feature Access 规范实现的几何库头文件，提供完整的空间几何类型支持。

---

## 头文件清单

| 文件 | 说明 | 核心类/结构 |
|------|------|-------------|
| [common.h](ogc/common.h) | 公共定义 | `GeomType`, `Dimension`, `GeomResult` |
| [export.h](ogc/export.h) | 导出宏 | `OGC_API` |
| [coordinate.h](ogc/coordinate.h) | 坐标结构 | `Coordinate` |
| [geometry.h](ogc/geometry.h) | 几何基类 | `Geometry` |
| [point.h](ogc/point.h) | 点几何 | `Point` |
| [linestring.h](ogc/linestring.h) | 线串 | `LineString` |
| [linearring.h](ogc/linearring.h) | 线环 | `LinearRing` |
| [polygon.h](ogc/polygon.h) | 多边形 | `Polygon` |
| [multipoint.h](ogc/multipoint.h) | 多点 | `MultiPoint` |
| [multilinestring.h](ogc/multilinestring.h) | 多线串 | `MultiLineString` |
| [multipolygon.h](ogc/multipolygon.h) | 多多边形 | `MultiPolygon` |
| [geometrycollection.h](ogc/geometrycollection.h) | 几何集合 | `GeometryCollection` |
| [envelope.h](ogc/envelope.h) | 包络(边界框) | `Envelope` |
| [envelope3d.h](ogc/envelope3d.h) | 3D包络 | `Envelope3D` |
| [visitor.h](ogc/visitor.h) | 访问者模式 | `GeometryVisitor`, `GeometryConstVisitor` |
| [factory.h](ogc/factory.h) | 几何工厂 | `GeometryFactory` |
| [geometry_pool.h](ogc/geometry_pool.h) | 对象池 | `GeometryPool`, `ObjectPool` |
| [spatial_index.h](ogc/spatial_index.h) | 空间索引 | `RTree`, `Quadtree`, `GridIndex` |
| [precision.h](ogc/precision.h) | 精度模型 | `PrecisionModel` |
| [exception.h](ogc/exception.h) | 异常类 | `GeometryException` |

---

## 类继承关系图

```
┌─────────────────────────────────────────────────────────────────┐
│                        Geometry (抽象基类)                        │
│  - GetGeometryType(), GetDimension(), IsEmpty(), IsValid()      │
│  - 空间关系: Equals, Intersects, Contains, Within, Crosses...   │
│  - 空间操作: Buffer, Intersection, Union, Difference...         │
│  - 导出格式: AsText(WKT), AsBinary(WKB), AsGeoJSON, AsGML       │
└─────────────────────────────────────────────────────────────────┘
         │           │           │           │           │
         ▼           ▼           ▼           ▼           ▼
    ┌─────────┐ ┌───────────┐ ┌─────────┐ ┌─────────┐ ┌───────────────────┐
    │  Point  │ │ LineString│ │ Polygon │ │MultiXxx │ │GeometryCollection │
    │ (0维)   │ │  (1维)    │ │ (2维)   │ │(组合类) │ │    (异构集合)     │
    └─────────┘ └─────┬─────┘ └─────────┘ └─────────┘ └───────────────────┘
                      │
                      ▼
               ┌────────────┐
               │ LinearRing │
               │ (闭合线串)  │
               └────────────┘
                      │
                      │ 用于构建
                      ▼
               ┌────────────┐
               │  Polygon   │
               │ 外环+内环   │
               └────────────┘
```

### Multi 类型继承关系

```
Geometry
    ├── MultiPoint          : 点集合 (Dimension::Point)
    ├── MultiLineString     : 线串集合 (Dimension::Curve)
    ├── MultiPolygon        : 多边形集合 (Dimension::Surface)
    └── GeometryCollection  : 异构几何集合 (动态维度)
```

### 包络类继承关系

```
Envelope (2D边界框)
    │
    └── Envelope3D (3D边界框, 扩展Z维度)
```

---

## 核心类详解

### 1. 基础类型 ([common.h](ogc/common.h))

```cpp
enum class GeomType : uint8_t {
    kPoint, kLineString, kPolygon,
    kMultiPoint, kMultiLineString, kMultiPolygon,
    kGeometryCollection, ...
};

enum class Dimension : uint8_t { Point = 0, Curve = 1, Surface = 2 };

enum class GeomResult : int32_t { kSuccess = 0, kInvalidGeometry = 1, ... };
```

### 2. 坐标 ([coordinate.h](ogc/coordinate.h))

```cpp
struct Coordinate {
    double x, y, z, m;           // 支持 2D/3D/Measured
    bool Is3D() const;
    bool IsMeasured() const;
    double Distance(const Coordinate& other) const;
    static Coordinate Empty();
};
using CoordinateList = std::vector<Coordinate>;
```

### 3. 几何基类 ([geometry.h](ogc/geometry.h))

```cpp
class Geometry {
public:
    // 类型信息
    virtual GeomType GetGeometryType() const = 0;
    virtual Dimension GetDimension() const = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsValid(std::string* reason = nullptr) const;
    
    // 空间关系 (DE-9IM)
    virtual bool Equals(const Geometry* other, double tolerance) const;
    virtual bool Intersects(const Geometry* other) const;
    virtual bool Contains(const Geometry* other) const;
    virtual bool Within(const Geometry* other) const;
    virtual bool Crosses(const Geometry* other) const;
    virtual bool Touches(const Geometry* other) const;
    virtual bool Overlaps(const Geometry* other) const;
    
    // 空间操作
    virtual GeomResult Buffer(double distance, GeometryPtr& result) const;
    virtual GeomResult Intersection(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult Union(const Geometry* other, GeometryPtr& result) const;
    virtual GeometryPtr ConvexHull() const;
    
    // 导出格式
    virtual std::string AsText(int precision) const = 0;  // WKT
    virtual std::vector<uint8_t> AsBinary() const = 0;    // WKB
    virtual std::string AsGeoJSON(int precision) const;
    
    // 访问者模式
    virtual void Apply(GeometryVisitor& visitor);
    virtual void Apply(GeometryConstVisitor& visitor) const;
};
```

### 4. 点 ([point.h](ogc/point.h))

```cpp
class Point : public Geometry {
    static PointPtr Create(double x, double y);
    static PointPtr Create(double x, double y, double z);
    double GetX() const, GetY() const, GetZ() const, GetM() const;
    PointPtr operator+(const Point& rhs) const;
    double Dot(const Point& other) const;
};
```

### 5. 线串 ([linestring.h](ogc/linestring.h))

```cpp
class LineString : public Geometry {
    size_t GetNumPoints() const;
    Coordinate GetStartPoint() const, GetEndPoint() const;
    double Length() const override;
    Coordinate Interpolate(double distance) const;
    LineStringPtr GetSubLine(double start, double end) const;
};
```

### 6. 线环 ([linearring.h](ogc/linearring.h))

```cpp
class LinearRing : public LineString {  // 闭合线串
    bool IsClockwise() const;
    bool IsCounterClockwise() const;
    void ForceClockwise();
    bool ContainsPoint(const Coordinate& point) const;
    double Area() const;  // 有符号面积
    std::vector<Triangle> Triangulate() const;
};
```

### 7. 多边形 ([polygon.h](ogc/polygon.h))

```cpp
class Polygon : public Geometry {
    void SetExteriorRing(LinearRingPtr ring);
    void AddInteriorRing(LinearRingPtr ring);  // 添加孔洞
    size_t GetNumInteriorRings() const;
    double Area() const override;
    
    static PolygonPtr CreateRectangle(minX, minY, maxX, maxY);
    static PolygonPtr CreateCircle(cx, cy, radius, segments);
};
```

### 8. 包络 ([envelope.h](ogc/envelope.h))

```cpp
class Envelope {
    double GetMinX(), GetMinY(), GetMaxX(), GetMaxY();
    double GetWidth(), GetHeight(), GetArea();
    Coordinate GetCentre() const;
    
    void ExpandToInclude(const Coordinate& coord);
    bool Contains(const Coordinate& coord) const;
    bool Intersects(const Envelope& other) const;
    Envelope Intersection(const Envelope& other) const;
};
```

---

## 辅助组件

### 几何工厂 ([factory.h](ogc/factory.h))

```cpp
class GeometryFactory {
    static GeometryFactory& GetInstance();
    
    GeomResult FromWKT(const std::string& wkt, GeometryPtr& result);
    GeomResult FromWKB(const std::vector<uint8_t>& wkb, GeometryPtr& result);
    GeomResult FromGeoJSON(const std::string& json, GeometryPtr& result);
    
    PointPtr CreatePoint(double x, double y);
    PolygonPtr CreateRectangle(...);
    PolygonPtr CreateCircle(...);
};
```

### 访问者模式 ([visitor.h](ogc/visitor.h))

```cpp
class GeometryVisitor {
    virtual void VisitPoint(Point* point);
    virtual void VisitLineString(LineString* line);
    virtual void VisitPolygon(Polygon* polygon);
    virtual void VisitMultiPolygon(MultiPolygon* multiPolygon);
    // ...
};

// 内置访问者
class AreaCalculator : public GeometryConstVisitor { ... };
class GeometryStatistics : public GeometryConstVisitor { ... };
```

### 空间索引 ([spatial_index.h](ogc/spatial_index.h))

```cpp
template<typename T>
class ISpatialIndex {
    virtual GeomResult Insert(const Envelope& envelope, const T& item) = 0;
    virtual std::vector<T> Query(const Envelope& envelope) const = 0;
    virtual std::vector<T> QueryNearest(const Coordinate& point, size_t k) const = 0;
};

// 实现
template<typename T> class RTree : public ISpatialIndex<T>;      // R树
template<typename T> class Quadtree : public ISpatialIndex<T>;   // 四叉树
template<typename T> class GridIndex : public ISpatialIndex<T>;  // 网格索引
```

### 对象池 ([geometry_pool.h](ogc/geometry_pool.h))

```cpp
class GeometryPool {
    static GeometryPool& Instance();
    PointPtr AcquirePoint();
    void ReleasePoint(PointPtr point);
    // LineString, Polygon 同理
};

template<typename T>
class ObjectPool {  // 通用对象池模板
    std::unique_ptr<T> Acquire();
    void Release(std::unique_ptr<T> obj);
};
```

---

## 类型别名 (智能指针)

```cpp
// unique_ptr 别名
using GeometryPtr = std::unique_ptr<Geometry>;
using PointPtr = std::unique_ptr<Point>;
using LineStringPtr = std::unique_ptr<LineString>;
using PolygonPtr = std::unique_ptr<Polygon>;
// ...

// shared_ptr 别名
using GeometrySharedPtr = std::shared_ptr<Geometry>;
using PointSharedPtr = std::shared_ptr<Point>;
// ...
```

---

## 依赖关系图

```
export.h
    │
    ▼
common.h ─────────────────────────────────────────┐
    │                                              │
    ▼                                              │
coordinate.h ◄─────────────────────────────────────┤
    │                                              │
    ▼                                              │
envelope.h ────────────────────────────────────────┤
    │                                              │
    ▼                                              │
geometry.h ◄───────────────────────────────────────┤
    │                                              │
    ├──────────┬──────────┬──────────┬─────────────┤
    ▼          ▼          ▼          ▼             │
 point.h  linestring.h  polygon.h  visitor.h       │
             │          │                           │
             ▼          │                           │
        linearring.h ◄──┘                           │
                                                   │
 factory.h ◄────────────────────────────────────────┘
    │
    ▼
geometry_pool.h

spatial_index.h ◄── geometry.h, envelope.h
precision.h ◄────── common.h, coordinate.h
exception.h ◄────── common.h
```

---

## 快速使用示例

```cpp
#include <ogc/factory.h>
#include <ogc/point.h>
#include <ogc/polygon.h>

using namespace ogc;

// 创建点
auto point = Point::Create(100.0, 200.0);

// 从WKT创建几何
GeometryFactory& factory = GeometryFactory::GetInstance();
GeometryPtr geom;
factory.FromWKT("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))", geom);

// 空间操作
GeometryPtr buffer;
geom->Buffer(5.0, buffer);

bool intersects = geom->Intersects(point.get());
```

---

## 文件分类

| 分类 | 头文件 |
|------|--------|
| **核心几何** | geometry.h, point.h, linestring.h, linearring.h, polygon.h |
| **组合几何** | multipoint.h, multilinestring.h, multipolygon.h, geometrycollection.h |
| **基础类型** | common.h, coordinate.h, export.h |
| **空间计算** | envelope.h, envelope3d.h, precision.h |
| **设计模式** | visitor.h, factory.h |
| **性能优化** | geometry_pool.h, spatial_index.h |
| **错误处理** | exception.h |
