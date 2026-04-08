# Geom Module - Header Index

## 模块描述

Geom模块是OGC Simple Feature Access规范的核心实现，提供完整的几何对象模型、空间操作和空间索引功能。支持Point、LineString、Polygon及其Multi类型，包含空间分析、坐标转换、WKT/WKB/GeoJSON/GML/KML序列化等功能。

## 核心特性

- 完整的OGC SFAS 1.2.1几何类型层次结构
- 空间关系判断（Equals、Intersects、Contains、Within等）
- 空间运算（Intersection、Union、Difference、Buffer等）
- WKT/WKB/GeoJSON/GML/KML格式序列化
- R-Tree/Quadtree/Grid空间索引
- 线程安全的对象池
- 访问者模式支持
- 几何工厂模式

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/geom/export.h) | DLL导出宏定义 | `OGC_GEOM_API` |
| [common.h](ogc/geom/common.h) | 公共定义 | `GeomType`, `Dimension`, `GeomResult` |
| [coordinate.h](ogc/geom/coordinate.h) | 坐标结构 | `Coordinate`, `CoordinateList` |
| [geometry.h](ogc/geom/geometry.h) | 几何基类 | `Geometry` |
| [point.h](ogc/geom/point.h) | 点几何类 | `Point` |
| [linestring.h](ogc/geom/linestring.h) | 线几何类 | `LineString` |
| [linearring.h](ogc/geom/linearring.h) | 线环类 | `LinearRing` |
| [polygon.h](ogc/geom/polygon.h) | 多边形类 | `Polygon` |
| [multipoint.h](ogc/geom/multipoint.h) | 多点类 | `MultiPoint` |
| [multilinestring.h](ogc/geom/multilinestring.h) | 多线类 | `MultiLineString` |
| [multipolygon.h](ogc/geom/multipolygon.h) | 多多边形类 | `MultiPolygon` |
| [geometrycollection.h](ogc/geom/geometrycollection.h) | 几何集合 | `GeometryCollection` |
| [envelope.h](ogc/geom/envelope.h) | 二维边界范围 | `Envelope` |
| [envelope3d.h](ogc/geom/envelope3d.h) | 三维边界 | `Envelope3D` |
| [factory.h](ogc/geom/factory.h) | 几何工厂 | `GeometryFactory` |
| [geometry_pool.h](ogc/geom/geometry_pool.h) | 几何对象池 | `GeometryPool` |
| [precision.h](ogc/geom/precision.h) | 精度模型 | `PrecisionModel` |
| [spatial_index.h](ogc/geom/spatial_index.h) | 空间索引 | `ISpatialIndex`, `RTree` |
| [visitor.h](ogc/geom/visitor.h) | 访问者模式 | `GeometryVisitor`, `GeometryConstVisitor` |
| [exception.h](ogc/geom/exception.h) | 异常类 | `GeometryException` |
| [serialization_utils.h](ogc/geom/serialization_utils.h) | 序列化工具 | `wkb::`, `geojson::` |

---

## 类继承关系图

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

GeometryVisitor (abstract)
    └── AreaCalculator

GeometryConstVisitor (abstract)
    └── AreaCalculator

ISpatialIndex<T> (template interface)
    ├── RTree<T>
    ├── Quadtree<T>
    └── GridIndex<T>
```

---

## 依赖关系图

```
export.h
    │
    └──► common.h ──► coordinate.h
              │
              └──► geometry.h
                        │
                        ├──► point.h
                        ├──► linestring.h ──► linearring.h
                        ├──► polygon.h
                        ├──► multipoint.h
                        ├──► multilinestring.h
                        ├──► multipolygon.h
                        └──► geometrycollection.h
                        
envelope.h ──► envelope3d.h

factory.h
    ├──► point.h
    ├──► linestring.h
    ├──► polygon.h
    └──► multipoint.h

spatial_index.h ──► geometry.h
visitor.h ──► geometry.h
geometry_pool.h ──► geometry.h
precision.h
exception.h
serialization_utils.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | common.h, coordinate.h, geometry.h, point.h, linestring.h, linearring.h, polygon.h |
| **Multi** | multipoint.h, multilinestring.h, multipolygon.h, geometrycollection.h |
| **Envelope** | envelope.h, envelope3d.h |
| **Factory** | factory.h, geometry_pool.h, precision.h |
| **Index** | spatial_index.h |
| **Visitor** | visitor.h |
| **Utility** | exception.h, serialization_utils.h, export.h |

---

## 关键类

### Geometry (抽象基类)
**File**: [geometry.h](ogc/geom/geometry.h)  
**Description**: 所有几何类型的抽象基类，定义OGC Simple Feature标准接口

```cpp
class OGC_GEOM_API Geometry {
public:
    virtual ~Geometry() = default;
    
    // 类型信息
    virtual GeomType GetGeometryType() const noexcept = 0;
    virtual const char* GetGeometryTypeString() const noexcept = 0;
    virtual Dimension GetDimension() const noexcept = 0;
    virtual int GetCoordinateDimension() const noexcept = 0;
    
    // 有效性检查
    virtual bool IsEmpty() const noexcept = 0;
    virtual bool IsValid(std::string* reason = nullptr) const;
    virtual bool IsSimple() const;
    virtual bool Is3D() const noexcept = 0;
    virtual bool IsMeasured() const noexcept = 0;
    
    // 空间参考
    int GetSRID() const noexcept;
    void SetSRID(int srid) noexcept;
    
    // 度量计算
    virtual double Distance(const Geometry* other) const;
    virtual double Area() const;
    virtual double Length() const;
    
    // 边界和中心
    const Envelope& GetEnvelope() const;
    virtual Coordinate GetCentroid() const;
    
    // 坐标访问
    virtual size_t GetNumCoordinates() const noexcept = 0;
    virtual CoordinateList GetCoordinates() const = 0;
    
    // 空间关系
    virtual bool Equals(const Geometry* other, double tolerance = DEFAULT_TOLERANCE) const;
    virtual bool Intersects(const Geometry* other) const;
    virtual bool Contains(const Geometry* other) const;
    virtual bool Within(const Geometry* other) const;
    // ... 更多空间关系方法
    
    // 空间运算
    virtual GeomResult Intersection(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult Union(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult Buffer(double distance, GeometryPtr& result, int segments = 8) const;
    
    // 序列化
    virtual std::string AsText(int precision = DEFAULT_WKT_PRECISION) const = 0;
    virtual std::vector<uint8_t> AsBinary() const = 0;
    virtual std::string AsGeoJSON(int precision = DEFAULT_WKT_PRECISION) const;
    
    // 克隆
    virtual GeometryPtr Clone() const = 0;
};
```

### Point
**File**: [point.h](ogc/geom/point.h)  
**Base**: `Geometry`  
**Description**: 点几何类，表示二维或三维空间中的单个点

```cpp
class OGC_GEOM_API Point : public Geometry {
public:
    static PointPtr Create(double x, double y);
    static PointPtr Create(double x, double y, double z);
    static PointPtr Create(double x, double y, double z, double m);
    static PointPtr Create(const Coordinate& coord);
    
    GeomType GetGeometryType() const noexcept override;
    Dimension GetDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override;
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    Coordinate GetCoordinate() const noexcept;
    void SetCoordinate(const Coordinate& coord);
    
    double GetX() const noexcept;
    double GetY() const noexcept;
    double GetZ() const noexcept;
    double GetM() const noexcept;
    
    void SetX(double x);
    void SetY(double y);
    void SetZ(double z);
    void SetM(double m);
    
    PointPtr operator+(const Point& rhs) const;
    PointPtr operator-(const Point& rhs) const;
    PointPtr operator*(double scalar) const;
    
    double Dot(const Point& other) const;
    double Cross(const Point& other) const;
    
    bool IsCollinear(const Point& p1, const Point& p2, double tolerance = DEFAULT_TOLERANCE) const;
    bool IsBetween(const Point& p1, const Point& p2) const;
    
    PointPtr Normalize() const;
    
    static PointPtr FromPolar(double radius, double angle);
    static PointPtr Centroid(const CoordinateList& coords);
};
```

### LineString
**File**: [linestring.h](ogc/geom/linestring.h)  
**Base**: `Geometry`  
**Description**: 线几何类，由一系列坐标点组成

```cpp
class OGC_GEOM_API LineString : public Geometry {
public:
    static LineStringPtr Create();
    static LineStringPtr Create(const CoordinateList& coords);
    static LineStringPtr Create(CoordinateList&& coords);
    
    GeomType GetGeometryType() const noexcept override;
    Dimension GetDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override;
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    bool IsClosed() const noexcept;
    bool IsRing() const noexcept;
    
    size_t GetNumPoints() const noexcept;
    size_t GetNumCoordinates() const noexcept override;
    size_t GetNumSegments() const noexcept;
    
    Coordinate GetCoordinateN(size_t index) const override;
    Coordinate GetPointN(size_t index) const;
    Coordinate GetStartPoint() const;
    Coordinate GetEndPoint() const;
    CoordinateList GetCoordinates() const override;
    
    void SetCoordinates(const CoordinateList& coords);
    void SetCoordinates(CoordinateList&& coords);
    void AddPoint(const Coordinate& coord);
    void InsertPoint(size_t index, const Coordinate& coord);
    void RemovePoint(size_t index);
    void Clear();
    
    Segment GetSegment(size_t index) const;
    
    double Length() const override;
    double Length3D() const;
    
    Coordinate Interpolate(double distance) const;
    double LocatePoint(const Coordinate& point) const;
    LineStringPtr GetSubLine(double startDistance, double endDistance) const;
    std::pair<LineStringPtr, LineStringPtr> Split(const Coordinate& point) const;
    LineStringPtr Offset(double distance) const;
    
    double GetCurvatureAt(size_t index) const;
    double GetBearingAt(size_t index) const;
    
    void RemoveDuplicatePoints(double tolerance = DEFAULT_TOLERANCE);
    LineStringPtr Smooth(int iterations = 1) const;
    bool IsStraight(double tolerance = DEFAULT_TOLERANCE) const;
};
```

### LinearRing
**File**: [linearring.h](ogc/geom/linearring.h)  
**Base**: `LineString`  
**Description**: 线环类，闭合的线串，用于多边形边界

```cpp
class OGC_GEOM_API LinearRing : public LineString {
public:
    static LinearRingPtr Create();
    static LinearRingPtr Create(const CoordinateList& coords, bool autoClose = true);
    
    bool IsValidRing(std::string* reason = nullptr) const;
    bool IsClockwise() const;
    bool IsCounterClockwise() const;
    void Reverse();
    void ForceClockwise();
    void ForceCounterClockwise();
    bool ContainsPoint(const Coordinate& point) const;
    
    static LinearRingPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    static LinearRingPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
};
```

### Polygon
**File**: [polygon.h](ogc/geom/polygon.h)  
**Base**: `Geometry`  
**Description**: 多边形类，由外环和零或多个内环组成

```cpp
class OGC_GEOM_API Polygon : public Geometry {
public:
    static PolygonPtr Create();
    static PolygonPtr Create(LinearRingPtr exterior);
    
    GeomType GetGeometryType() const noexcept override;
    Dimension GetDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override;
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    void SetExteriorRing(LinearRingPtr ring);
    const LinearRing* GetExteriorRing() const noexcept;
    
    void AddInteriorRing(LinearRingPtr ring);
    void RemoveInteriorRing(size_t index);
    const LinearRing* GetInteriorRingN(size_t index) const;
    size_t GetNumInteriorRings() const noexcept;
    size_t GetNumRings() const noexcept;
    void ClearRings();
    
    double Area() const override;
    double Length() const override;
    double GetPerimeter() const;
    
    double GetExteriorArea() const;
    double GetInteriorArea() const;
    double GetSolidity() const;
    double GetCompactness() const;
    
    PolygonPtr RemoveHoles() const;
    PolygonPtr MergeHoles() const;
    std::vector<LinearRing::Triangle> Triangulate() const;
    
    bool ContainsRing(const LinearRing* ring) const;
    bool ContainsPolygon(const Polygon* polygon) const;
    
    GeomResult UnionWithPolygon(const Polygon* other, PolygonPtr& result) const;
    GeomResult IntersectWithPolygon(const Polygon* other, PolygonPtr& result) const;
    GeomResult DifferenceWithPolygon(const Polygon* other, PolygonPtr& result) const;
    
    static PolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    static PolygonPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    static PolygonPtr CreateRegularPolygon(double centerX, double centerY, double radius, int sides);
    static PolygonPtr CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3);
};
```

### GeometryFactory
**File**: [factory.h](ogc/geom/factory.h)  
**Description**: 几何工厂类，提供创建各种几何对象的静态方法

```cpp
class OGC_GEOM_API GeometryFactory {
public:
    static GeometryFactory& GetInstance();
    
    GeomResult FromWKT(const std::string& wkt, GeometryPtr& result);
    GeomResult FromWKB(const std::vector<uint8_t>& wkb, GeometryPtr& result);
    GeomResult FromGeoJSON(const std::string& json, GeometryPtr& result);
    
    PointPtr CreatePoint(double x, double y);
    LineStringPtr CreateLineString(const CoordinateList& coords);
    PolygonPtr CreatePolygon(const CoordinateList& exteriorRing);
    
    PolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    PolygonPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
};
```

---

## 接口

### ISpatialIndex (模板接口)
**File**: [spatial_index.h](ogc/geom/spatial_index.h)  
**Description**: 空间索引接口

```cpp
template<typename T>
class ISpatialIndex {
public:
    virtual ~ISpatialIndex() = default;
    
    virtual GeomResult Insert(const Envelope& envelope, const T& item) = 0;
    virtual GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) = 0;
    virtual bool Remove(const Envelope& envelope, const T& item) = 0;
    virtual void Clear() noexcept = 0;
    
    virtual std::vector<T> Query(const Envelope& envelope) const = 0;
    virtual std::vector<T> Query(const Coordinate& point) const = 0;
    virtual std::vector<T> QueryIntersects(const Geometry* geom) const = 0;
    virtual std::vector<T> QueryNearest(const Coordinate& point, size_t k) const = 0;
    
    virtual size_t Size() const noexcept = 0;
    virtual bool IsEmpty() const noexcept = 0;
    virtual Envelope GetBounds() const = 0;
};
```

### GeometryVisitor
**File**: [visitor.h](ogc/geom/visitor.h)  
**Description**: 几何访问者基类（可修改）

```cpp
class OGC_GEOM_API GeometryVisitor {
public:
    virtual ~GeometryVisitor() = default;
    
    virtual void Visit(Geometry* geom) { }
    virtual void VisitPoint(Point* point) { }
    virtual void VisitLineString(LineString* line) { }
    virtual void VisitLinearRing(LinearRing* ring) { VisitLineString(ring); }
    virtual void VisitPolygon(Polygon* polygon) { }
    virtual void VisitMultiPoint(MultiPoint* multiPoint) { }
    virtual void VisitMultiLineString(MultiLineString* multiLine) { }
    virtual void VisitMultiPolygon(MultiPolygon* multiPolygon) { }
    virtual void VisitGeometryCollection(GeometryCollection* collection) { }
};
```

### GeometryConstVisitor
**File**: [visitor.h](ogc/geom/visitor.h)  
**Description**: 几何访问者基类（只读）

```cpp
class OGC_GEOM_API GeometryConstVisitor {
public:
    virtual ~GeometryConstVisitor() = default;
    
    virtual void Visit(const Geometry* geom) { }
    virtual void VisitPoint(const Point* point) { }
    virtual void VisitLineString(const LineString* line) { }
    virtual void VisitLinearRing(const LinearRing* ring) { VisitLineString(ring); }
    virtual void VisitPolygon(const Polygon* polygon) { }
    virtual void VisitMultiPoint(const MultiPoint* multiPoint) { }
    virtual void VisitMultiLineString(const MultiLineString* multiLine) { }
    virtual void VisitMultiPolygon(const MultiPolygon* multiPolygon) { }
    virtual void VisitGeometryCollection(const GeometryCollection* collection) { }
};
```

---

## 类型定义

### GeomType (几何类型枚举)
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
    kCircularString = 8,
    kCompoundCurve = 9,
    kCurvePolygon = 10,
    kMultiCurve = 11,
    kMultiSurface = 12,
    kPolyhedralSurface = 13,
    kTIN = 14,
    kTriangle = 15
};
```

### Dimension (维度枚举)
```cpp
enum class Dimension : uint8_t {
    Unknown = 255,
    Point = 0,
    Curve = 1,
    Surface = 2
};
```

### GeomResult (操作结果枚举)
```cpp
enum class GeomResult : int32_t {
    kSuccess = 0,
    
    // 几何错误 (1-99)
    kInvalidGeometry = 1,
    kEmptyGeometry = 2,
    kNullGeometry = 3,
    kTopologyError = 4,
    kSelfIntersection = 5,
    kInvalidDimension = 6,
    kInvalidCoordinate = 7,
    kInvalidRing = 8,
    kNotClosedRing = 9,
    kDegenerateGeometry = 10,
    kInvalidPolygonShell = 11,
    kInvalidHole = 12,
    kHoleOutsideShell = 13,
    kNestedHoles = 14,
    kDisconnectedInterior = 15,
    kRepeatedPoint = 16,
    kTooFewPoints = 17,
    
    // 参数错误 (100-199)
    kInvalidParameters = 100,
    kOutOfRange = 101,
    kInvalidSRID = 102,
    kInvalidTolerance = 103,
    kNullPointer = 104,
    kInvalidIndex = 105,
    kInvalidWidth = 106,
    kInvalidPrecision = 107,
    kInvalidBufferSize = 108,
    
    // 操作错误 (200-299)
    kNotSupported = 200,
    kNotImplemented = 201,
    kOperationFailed = 202,
    kInvalidOperation = 203,
    kTypeMismatch = 204,
    kReadOnly = 205,
    
    // 资源错误 (300-399)
    kOutOfMemory = 300,
    kBufferOverflow = 301,
    kResourceExhausted = 302,
    
    // I/O错误 (400-499)
    kIOError = 400,
    kParseError = 401,
    kFormatError = 402,
    kFileNotFound = 403,
    kWriteFailed = 404,
    kReadFailed = 405,
    
    // 坐标转换错误 (500-599)
    kTransformError = 500,
    kCRSNotFound = 501,
    kCRSConversionError = 502,
    kInvalidProjection = 503,
    kCoordinateOutOfRange = 504
};
```

### 常量定义
```cpp
constexpr double DEFAULT_TOLERANCE = 1e-9;
constexpr int DEFAULT_WKT_PRECISION = 15;
constexpr double PI = 3.14159265358979323846;
```

### 类型别名
```cpp
// unique_ptr 类型别名
using GeometryPtr = std::unique_ptr<Geometry>;
using PointPtr = std::unique_ptr<Point>;
using LineStringPtr = std::unique_ptr<LineString>;
using LinearRingPtr = std::unique_ptr<LinearRing>;
using PolygonPtr = std::unique_ptr<Polygon>;
using MultiPointPtr = std::unique_ptr<MultiPoint>;
using MultiLineStringPtr = std::unique_ptr<MultiLineString>;
using MultiPolygonPtr = std::unique_ptr<MultiPolygon>;
using GeometryCollectionPtr = std::unique_ptr<GeometryCollection>;

// shared_ptr 类型别名
using GeometrySharedPtr = std::shared_ptr<Geometry>;
using PointSharedPtr = std::shared_ptr<Point>;
using LineStringSharedPtr = std::shared_ptr<LineString>;
using LinearRingSharedPtr = std::shared_ptr<LinearRing>;
using PolygonSharedPtr = std::shared_ptr<Polygon>;
using MultiPointSharedPtr = std::shared_ptr<MultiPoint>;
using MultiLineStringSharedPtr = std::shared_ptr<MultiLineString>;
using MultiPolygonSharedPtr = std::shared_ptr<MultiPolygon>;
using GeometryCollectionSharedPtr = std::shared_ptr<GeometryCollection>;

// 坐标列表
using CoordinateList = std::vector<Coordinate>;
```

---

## 使用示例

### 创建几何对象

```cpp
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/factory.h"

using namespace ogc;

// 创建点
auto point = Point::Create(116.4, 39.9);

// 创建线
CoordinateList coords = {{0,0}, {1,0}, {1,1}, {0,1}};
auto line = LineString::Create(coords);

// 创建多边形
auto ring = LinearRing::Create(coords);
auto polygon = Polygon::Create(std::move(ring));
```

### 空间运算

```cpp
// 缓冲区
GeometryPtr buffer;
polygon->Buffer(10.0, buffer);

// 获取中心点
Coordinate centroid = polygon->GetCentroid();

// 空间关系判断
if (point->Within(polygon.get())) {
    // 点在多边形内
}

// 空间运算
GeometryPtr intersection;
polygon1->Intersection(polygon2.get(), intersection);
```

### 序列化

```cpp
// WKT
std::string wkt = polygon->AsText();
// "POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))"

// WKB
std::vector<uint8_t> wkb = polygon->AsBinary();

// GeoJSON
std::string json = polygon->AsGeoJSON();

// 从WKT解析
GeometryFactory& factory = GeometryFactory::GetInstance();
GeometryPtr geom;
factory.FromWKT("POINT(116.4 39.9)", geom);
```

### 使用空间索引

```cpp
#include "ogc/geom/spatial_index.h"

// 使用 RTree 索引存储几何对象指针
RTree<Geometry*> index;

// 插入
index.Insert(geom->GetEnvelope(), geom);

// 查询
auto results = index.Query(searchEnvelope);

// 最近邻查询
auto nearest = index.QueryNearest(point, 5);
```

### 使用访问者模式

```cpp
#include "ogc/geom/visitor.h"

class PrintVisitor : public GeometryConstVisitor {
public:
    void VisitPoint(const Point* point) override {
        std::cout << "Point: " << point->GetX() << ", " << point->GetY() << std::endl;
    }
    
    void VisitPolygon(const Polygon* polygon) override {
        std::cout << "Polygon with " << polygon->GetNumInteriorRings() << " holes" << std::endl;
    }
};

PrintVisitor visitor;
geometry->Apply(visitor);
```

---

## 修改历史

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2026-04-01 | 初始版本 |
| v1.1 | 2026-04-09 | 补充GeomResult枚举值(kInvalidWidth, kInvalidPrecision, kInvalidBufferSize, kReadOnly, kWriteFailed, kReadFailed, kCRSConversionError, kInvalidProjection, kCoordinateOutOfRange) |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
