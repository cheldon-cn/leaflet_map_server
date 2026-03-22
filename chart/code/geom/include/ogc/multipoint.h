#pragma once

#include "geometry.h"
#include "point.h"
#include <vector>

namespace ogc {

class OGC_GEOM_API MultiPoint : public Geometry {
public:
    static MultiPointPtr Create();
    static MultiPointPtr Create(const CoordinateList& coords);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kMultiPoint; }
    const char* GetGeometryTypeString() const noexcept override { return "MULTIPOINT"; }
    Dimension GetDimension() const noexcept override { return Dimension::Point; }
    
    int GetCoordinateDimension() const noexcept override;
    bool IsEmpty() const noexcept override { return m_points.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    size_t GetNumGeometries() const noexcept override { return m_points.size(); }
    const Geometry* GetGeometryN(size_t index) const override;
    
    void AddPoint(PointPtr point);
    void AddPoint(const Coordinate& coord);
    void RemovePoint(size_t index);
    void Clear();
    
    const Point* GetPointN(size_t index) const;
    size_t GetNumPoints() const noexcept { return m_points.size(); }
    
    void Merge(const MultiPoint* other);
    void UniquePoints(double tolerance = DEFAULT_TOLERANCE);
    
    bool ContainsPoint(const Coordinate& coord, double tolerance = DEFAULT_TOLERANCE) const;
    bool ContainsMultiPoint(const MultiPoint* other) const;
    
    void SortByX();
    void SortByY();
    
    size_t GetNumCoordinates() const noexcept override { return m_points.size(); }
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
private:
    MultiPoint() = default;
    
    std::vector<PointPtr> m_points;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};

}
