#pragma once

/**
 * @file linearring.h
 * @brief 线环几何类定义，用于多边形边界
 */

#include "linestring.h"

namespace ogc {

/**
 * @brief 线环类
 * 
 * 闭合的线串，用于表示多边形的外环和内环
 */
class OGC_API LinearRing : public LineString {
public:
    static LinearRingPtr Create();
    static LinearRingPtr Create(const CoordinateList& coords, bool autoClose = true);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kLineString; }
    const char* GetGeometryTypeString() const noexcept override { return "LINEARRING"; }
    
    bool IsValidRing(std::string* reason = nullptr) const;
    bool IsSimpleRing() const;
    
    double Area() const;
    double GetPerimeter() const { return Length(); }
    
    bool IsConvex() const;
    
    bool IsClockwise() const;
    bool IsCounterClockwise() const;
    
    void Reverse();
    void ForceClockwise();
    void ForceCounterClockwise();
    
    bool ContainsPoint(const Coordinate& point) const;
    bool IsPointOnBoundary(const Coordinate& point, double tolerance = DEFAULT_TOLERANCE) const;
    
    LinearRingPtr Offset(double distance) const;
    void Normalize();
    
    struct Triangle { Coordinate p1, p2, p3; };
    std::vector<Triangle> Triangulate() const;
    
    static LinearRingPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    static LinearRingPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    static LinearRingPtr CreateRegularPolygon(double centerX, double centerY, double radius, int sides);
    
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
private:
    LinearRing() = default;
    
    void EnsureClosed();
};

}
