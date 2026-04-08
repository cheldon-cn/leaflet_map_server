#pragma once

#include "geometry.h"
#include "linestring.h"
#include <vector>

namespace ogc {

class OGC_GEOM_API MultiLineString : public Geometry {
public:
    static MultiLineStringPtr Create();
    static MultiLineStringPtr Create(std::vector<LineStringPtr> lines);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kMultiLineString; }
    const char* GetGeometryTypeString() const noexcept override { return "MULTILINESTRING"; }
    Dimension GetDimension() const noexcept override { return Dimension::Curve; }
    
    int GetCoordinateDimension() const noexcept override;
    bool IsEmpty() const noexcept override { return m_lines.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    size_t GetNumGeometries() const noexcept override { return m_lines.size(); }
    const Geometry* GetGeometryN(size_t index) const override;
    
    void AddLineString(LineStringPtr line);
    void RemoveLineString(size_t index);
    void Clear();
    
    const LineString* GetLineStringN(size_t index) const;
    size_t GetNumLineStrings() const noexcept { return m_lines.size(); }
    
    double Length() const override;
    double Length3D() const;
    
    bool IsClosed() const;
    
    MultiLineStringPtr Merge() const;
    
    size_t GetNumCoordinates() const noexcept override;
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::string AsGeoJSON(int precision = DEFAULT_WKT_PRECISION) const override;
    std::string AsGML() const override;
    std::string AsKML() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
private:
    MultiLineString() = default;
    
    std::vector<LineStringPtr> m_lines;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};

}
