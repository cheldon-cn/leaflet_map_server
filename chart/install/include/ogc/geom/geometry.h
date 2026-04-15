#pragma once

/**
 * @file geometry.h
 * @brief 几何基类定义，所有几何类型的基类
 */

#include "common.h"
#include "coordinate.h"
#include "envelope.h"
#include "exception.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <atomic>

namespace ogc {

class Geometry;
class Point;
class LineString;
class LinearRing;
class Polygon;
class MultiPoint;
class MultiLineString;
class MultiPolygon;
class GeometryCollection;

using GeometryPtr = std::unique_ptr<Geometry>;
using PointPtr = std::unique_ptr<Point>;
using LineStringPtr = std::unique_ptr<LineString>;
using LinearRingPtr = std::unique_ptr<LinearRing>;
using PolygonPtr = std::unique_ptr<Polygon>;
using MultiPointPtr = std::unique_ptr<MultiPoint>;
using MultiLineStringPtr = std::unique_ptr<MultiLineString>;
using MultiPolygonPtr = std::unique_ptr<MultiPolygon>;
using GeometryCollectionPtr = std::unique_ptr<GeometryCollection>;

using GeometrySharedPtr = std::shared_ptr<Geometry>;
using PointSharedPtr = std::shared_ptr<Point>;
using LineStringSharedPtr = std::shared_ptr<LineString>;
using LinearRingSharedPtr = std::shared_ptr<LinearRing>;
using PolygonSharedPtr = std::shared_ptr<Polygon>;
using MultiPointSharedPtr = std::shared_ptr<MultiPoint>;
using MultiLineStringSharedPtr = std::shared_ptr<MultiLineString>;
using MultiPolygonSharedPtr = std::shared_ptr<MultiPolygon>;
using GeometryCollectionSharedPtr = std::shared_ptr<GeometryCollection>;

class GeometryVisitor;
class GeometryConstVisitor;

/**
 * @brief 几何基类
 * 
 * 所有几何类型的抽象基类，定义了OGC Simple Feature Access规范中的标准接口
 */
class OGC_GEOM_API Geometry {
public:
    virtual ~Geometry();
    
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;
    
    Geometry(Geometry&&) noexcept = default;
    Geometry& operator=(Geometry&&) noexcept = default;
    
    /**
     * @brief 获取几何类型
     */
    virtual GeomType GetGeometryType() const noexcept = 0;
    
    /**
     * @brief 获取几何类型名称字符�?
     */
    virtual const char* GetGeometryTypeString() const noexcept = 0;
    
    /**
     * @brief 获取几何维度
     */
    virtual Dimension GetDimension() const noexcept = 0;
    
    /**
     * @brief 获取坐标维度(2/3/4)
     */
    virtual int GetCoordinateDimension() const noexcept = 0;
    
    /**
     * @brief 判断是否为空几何
     */
    virtual bool IsEmpty() const noexcept = 0;
    
    /**
     * @brief 验证几何有效�?
     */
    virtual bool IsValid(std::string* reason = nullptr) const;
    
    /**
     * @brief 判断是否为简单几�?
     */
    virtual bool IsSimple() const;
    
    /**
     * @brief 判断是否�?D几何
     */
    virtual bool Is3D() const noexcept = 0;
    
    /**
     * @brief 判断是否包含测量�?
     */
    virtual bool IsMeasured() const noexcept = 0;
    
    /**
     * @brief 获取空间参考ID
     */
    int GetSRID() const noexcept;
    
    /**
     * @brief 设置空间参考ID
     */
    void SetSRID(int srid) noexcept;
    
    virtual GeomResult Transform(int targetSRID, GeometryPtr& result) const;
    virtual GeomResult Transform(const std::string& targetCRS, GeometryPtr& result) const;
    
    virtual double Distance(const Geometry* other) const;
    virtual bool IsWithinDistance(const Geometry* other, double distance) const;
    virtual double Area() const;
    virtual double Length() const;
    
    const Envelope& GetEnvelope() const;
    GeometryPtr GetEnvelopeGeometry() const;
    virtual Coordinate GetCentroid() const;
    virtual Coordinate GetPointOnSurface() const;
    virtual Coordinate GetInteriorPoint() const;
    
    virtual size_t GetNumCoordinates() const noexcept = 0;
    virtual Coordinate GetCoordinateN(size_t index) const;
    virtual CoordinateList GetCoordinates() const = 0;
    virtual bool GetCoordinateData(const double** data, size_t* count) const;
    
    virtual bool Equals(const Geometry* other, double tolerance = DEFAULT_TOLERANCE) const;
    virtual bool Disjoint(const Geometry* other) const;
    virtual bool Intersects(const Geometry* other) const;
    virtual bool Touches(const Geometry* other) const;
    virtual bool Crosses(const Geometry* other) const;
    virtual bool Within(const Geometry* other) const;
    virtual bool Contains(const Geometry* other) const;
    virtual bool Overlaps(const Geometry* other) const;
    virtual bool Covers(const Geometry* other) const;
    virtual bool CoveredBy(const Geometry* other) const;
    virtual std::string Relate(const Geometry* other) const;
    virtual bool Relate(const Geometry* other, const std::string& pattern) const;
    
    virtual GeomResult Intersection(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult Union(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult Difference(const Geometry* other, GeometryPtr& result) const;
    virtual GeomResult SymmetricDifference(const Geometry* other, GeometryPtr& result) const;
    
    virtual GeomResult Buffer(double distance, GeometryPtr& result, int segments = 8) const;
    virtual GeomResult SingleSidedBuffer(double distance, GeometryPtr& result) const;
    virtual GeomResult OffsetCurve(double distance, GeometryPtr& result) const;
    
    virtual GeometryPtr ConvexHull() const;
    virtual GeometryPtr Boundary() const;
    virtual GeomResult MakeValid(GeometryPtr& result) const;
    virtual void Normalize();
    virtual GeometryPtr Simplify(double tolerance) const;
    virtual GeometryPtr SimplifyPreserveTopology(double tolerance) const;
    virtual GeometryPtr Densify(double maxSegmentLength) const;
    virtual GeometryPtr Reverse() const;
    virtual GeometryPtr SnapToGrid(double gridSize) const;
    virtual GeometryPtr SnapTo(const Geometry* reference, double tolerance) const;
    virtual GeometryPtr AffineTransform(double a, double b, double c,
                                          double d, double e, double f) const;
    virtual GeometryPtr Translate(double dx, double dy) const;
    virtual GeometryPtr Scale(double scaleX, double scaleY) const;
    virtual GeometryPtr Rotate(double angle) const;
    
    virtual std::string AsText(int precision = DEFAULT_WKT_PRECISION) const = 0;
    virtual std::vector<uint8_t> AsBinary() const = 0;
    std::string AsHex() const;
    virtual std::string AsGeoJSON(int precision = DEFAULT_WKT_PRECISION) const;
    virtual std::string AsGML() const;
    virtual std::string AsKML() const;
    
    virtual GeometryPtr Clone() const = 0;
    virtual GeometryPtr CloneEmpty() const = 0;
    
    virtual void Reset() {}
    
    virtual void Apply(GeometryVisitor& visitor);
    virtual void Apply(GeometryConstVisitor& visitor) const;
    
    virtual bool HasCurveGeometry() const noexcept { return false; }
    virtual GeometryPtr GetLinearGeometry() const;
    virtual size_t GetNumGeometries() const noexcept { return 1; }
    virtual const Geometry* GetGeometryN(size_t index) const;
    virtual GeometryPtr GetShortestLine(const Geometry* other) const;
    virtual GeometryPtr GetMaximumDistanceLine(const Geometry* other) const;
    virtual std::pair<Coordinate, double> GetMinimumBoundingCircle() const;
    virtual GeometryPtr GetMinimumBoundingRectangle() const;
    
protected:
    Geometry();
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    virtual Envelope ComputeEnvelope() const = 0;
    virtual Coordinate ComputeCentroid() const;
    
    void InvalidateCache() const;
    bool EnvelopeDisjoint(const Geometry* other) const;
    bool EnvelopeIntersects(const Geometry* other) const;
    
    const Envelope& GetEnvelopeCache() const;
    void SetEnvelopeCache(std::unique_ptr<Envelope> env) const;
    const Coordinate& GetCentroidCache() const;
    void SetCentroidCache(std::unique_ptr<Coordinate> coord) const;
    std::mutex& GetCacheMutex() const;
};

}
