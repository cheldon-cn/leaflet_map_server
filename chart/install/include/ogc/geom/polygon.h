#pragma once

/**
 * @file polygon.h
 * @brief 多边形几何类定义
 */

#include "linearring.h"
#include <vector>

namespace ogc {

/**
 * @brief 多边形几何类
 * 
 * 由一个外环和零个或多个内环（孔洞）组成的多边�?
 */
class OGC_GEOM_API Polygon : public Geometry {
public:
    static PolygonPtr Create();
    static PolygonPtr Create(LinearRingPtr exteriorRing);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kPolygon; }
    const char* GetGeometryTypeString() const noexcept override { return "POLYGON"; }
    Dimension GetDimension() const noexcept override { return Dimension::Surface; }
    int GetCoordinateDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override;
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    void SetExteriorRing(LinearRingPtr ring);
    const LinearRing* GetExteriorRing() const noexcept;
    
    void AddInteriorRing(LinearRingPtr ring);
    void RemoveInteriorRing(size_t index);
    const LinearRing* GetInteriorRingN(size_t index) const;
    size_t GetNumInteriorRings() const noexcept { return m_interiorRings.size(); }
    size_t GetNumRings() const noexcept { return (m_exteriorRing ? 1 : 0) + m_interiorRings.size(); }
    
    void ClearRings();
    
    size_t GetNumCoordinates() const noexcept override;
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    double Area() const override;
    double Length() const override { return GetPerimeter(); }
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
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::string AsGeoJSON(int precision = DEFAULT_WKT_PRECISION) const override;
    std::string AsGML() const override;
    std::string AsKML() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
protected:
    Polygon() = default;
    
    LinearRingPtr m_exteriorRing;
    std::vector<LinearRingPtr> m_interiorRings;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};

}
