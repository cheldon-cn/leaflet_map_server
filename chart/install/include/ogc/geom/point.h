#pragma once

/**
 * @file point.h
 * @brief 点几何类定义
 */

#include "geometry.h"

namespace ogc {

/**
 * @brief 点几何类
 * 
 * 表示二维或三维空间中的单个点
 */
class OGC_GEOM_API Point : public Geometry {
public:
    static PointPtr Create(double x, double y);
    static PointPtr Create(double x, double y, double z);
    static PointPtr Create(double x, double y, double z, double m);
    static PointPtr Create(const Coordinate& coord);
    
    GeomType GetGeometryType() const noexcept override { return GeomType::kPoint; }
    const char* GetGeometryTypeString() const noexcept override { return "POINT"; }
    Dimension GetDimension() const noexcept override { return Dimension::Point; }
    int GetCoordinateDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override { return m_coord.IsEmpty(); }
    bool Is3D() const noexcept override { return m_coord.Is3D(); }
    bool IsMeasured() const noexcept override { return m_coord.IsMeasured(); }
    
    Coordinate GetCoordinate() const noexcept { return m_coord; }
    void SetCoordinate(const Coordinate& coord);
    
    double GetX() const noexcept { return m_coord.x; }
    double GetY() const noexcept { return m_coord.y; }
    double GetZ() const noexcept { return m_coord.z; }
    double GetM() const noexcept { return m_coord.m; }
    
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
    
    size_t GetNumCoordinates() const noexcept override { return IsEmpty() ? 0 : 1; }
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
    explicit Point(const Coordinate& coord) : m_coord(coord) {}
    
    Coordinate m_coord;
    Envelope ComputeEnvelope() const override;
};

}
