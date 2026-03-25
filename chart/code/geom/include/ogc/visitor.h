#pragma once

#include "common.h"
#include "geometry.h"
#include "linearring.h"
#include <map>

namespace ogc {

class Point;
class LineString;
class Polygon;
class MultiPoint;
class MultiLineString;
class MultiPolygon;
class GeometryCollection;
class CircularString;
class CompoundCurve;
class CurvePolygon;
class TIN;

/**
 * @brief 几何访问者基类（可修改）
 */
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
    
    virtual void VisitCircularString(CircularString* curve) { }
    virtual void VisitCompoundCurve(CompoundCurve* curve) { }
    virtual void VisitCurvePolygon(CurvePolygon* polygon) { }
    virtual void VisitTIN(TIN* tin) { }
};

/**
 * @brief 几何访问者基类（只读）
 */
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
    
    virtual void VisitCircularString(const CircularString* curve) { }
    virtual void VisitCompoundCurve(const CompoundCurve* curve) { }
    virtual void VisitCurvePolygon(const CurvePolygon* polygon) { }
    virtual void VisitTIN(const TIN* tin) { }
};

/**
 * @brief 面积计算访问者类
 */
class OGC_GEOM_API AreaCalculator : public GeometryConstVisitor {
public:
    void VisitPoint(const Point*) override { m_area = 0.0; }
    void VisitLineString(const LineString*) override { m_area = 0.0; }
    void VisitPolygon(const Polygon* polygon) override;
    void VisitMultiPolygon(const MultiPolygon* multiPolygon) override;
    
    double GetArea() const noexcept { return m_area; }
    
private:
    double m_area = 0.0;
};

/**
 * @brief 几何统计访问者类
 */
class OGC_GEOM_API GeometryStatistics : public GeometryConstVisitor {
public:
    void Visit(const Geometry* geom) override {
        m_totalCount++;
        m_typeCounts[geom->GetGeometryType()]++;
    }
    
    size_t GetTotalCount() const noexcept { return m_totalCount; }
    size_t GetTypeCount(GeomType type) const {
        auto it = m_typeCounts.find(type);
        return it != m_typeCounts.end() ? it->second : 0;
    }
    
private:
    size_t m_totalCount = 0;
    std::map<GeomType, size_t> m_typeCounts;
};

}
