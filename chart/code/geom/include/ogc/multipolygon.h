#pragma once

#include "geometry.h"
#include "polygon.h"
#include <vector>

namespace ogc {

class OGC_GEOM_API MultiPolygon : public Geometry {
public:
    static MultiPolygonPtr Create();
    static MultiPolygonPtr Create(std::vector<PolygonPtr> polygons);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kMultiPolygon; }
    const char* GetGeometryTypeString() const noexcept override { return "MULTIPOLYGON"; }
    Dimension GetDimension() const noexcept override { return Dimension::Surface; }
    
    int GetCoordinateDimension() const noexcept override;
    bool IsEmpty() const noexcept override { return m_polygons.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    size_t GetNumGeometries() const noexcept override { return m_polygons.size(); }
    const Geometry* GetGeometryN(size_t index) const override;
    
    void AddPolygon(PolygonPtr polygon);
    void RemovePolygon(size_t index);
    void Clear();
    
    const Polygon* GetPolygonN(size_t index) const;
    size_t GetNumPolygons() const noexcept { return m_polygons.size(); }
    
    double Area() const override;
    double Length() const override;
    double GetPerimeter() const;
    
    bool Contains(const Geometry* other) const override;
    bool Intersects(const Geometry* other) const override;
    
    MultiPolygonPtr Union() const;
    MultiPolygonPtr RemoveHoles() const;
    
    size_t GetNumCoordinates() const noexcept override;
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
private:
    MultiPolygon() = default;
    
    std::vector<PolygonPtr> m_polygons;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};

}
