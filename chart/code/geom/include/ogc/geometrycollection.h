#pragma once

#include "geometry.h"
#include <vector>

namespace ogc {

class OGC_GEOM_API GeometryCollection : public Geometry {
public:
    static GeometryCollectionPtr Create();
    static GeometryCollectionPtr Create(std::vector<GeometryPtr> geometries);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kGeometryCollection; }
    const char* GetGeometryTypeString() const noexcept override { return "GEOMETRYCOLLECTION"; }
    Dimension GetDimension() const noexcept override;
    
    int GetCoordinateDimension() const noexcept override;
    bool IsEmpty() const noexcept override { return m_geometries.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    size_t GetNumGeometries() const noexcept override { return m_geometries.size(); }
    const Geometry* GetGeometryN(size_t index) const override;
    
    void AddGeometry(GeometryPtr geometry);
    void RemoveGeometry(size_t index);
    void Clear();
    
    GeometryPtr GetGeometryNPtr(size_t index) const;
    
    double Area() const override;
    double Length() const override;
    
    GeometryCollectionPtr Flatten() const;
    
    size_t GetNumCoordinates() const noexcept override;
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    std::string AsText(int precision = DEFAULT_WKT_PRECISION) const override;
    std::vector<uint8_t> AsBinary() const override;
    GeometryPtr Clone() const override;
    GeometryPtr CloneEmpty() const override;
    
    void Apply(GeometryVisitor& visitor) override;
    void Apply(GeometryConstVisitor& visitor) const override;
    
protected:
    GeometryCollection() = default;
    
    std::vector<GeometryPtr> m_geometries;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};

}
