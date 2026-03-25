#include "ogc/point.h"
#include "ogc/visitor.h"
#include "ogc/serialization_utils.h"
#include <sstream>
#include <iomanip>

namespace ogc {

PointPtr Point::Create(double x, double y) {
    return PointPtr(new Point(Coordinate(x, y)));
}

PointPtr Point::Create(double x, double y, double z) {
    return PointPtr(new Point(Coordinate(x, y, z)));
}

PointPtr Point::Create(double x, double y, double z, double m) {
    return PointPtr(new Point(Coordinate(x, y, z, m)));
}

PointPtr Point::Create(const Coordinate& coord) {
    return PointPtr(new Point(coord));
}

int Point::GetCoordinateDimension() const noexcept {
    int dim = 2;
    if (Is3D()) dim++;
    if (IsMeasured()) dim++;
    return dim;
}

void Point::SetCoordinate(const Coordinate& coord) {
    m_coord = coord;
    InvalidateCache();
}

void Point::SetX(double x) {
    m_coord.x = x;
    InvalidateCache();
}

void Point::SetY(double y) {
    m_coord.y = y;
    InvalidateCache();
}

void Point::SetZ(double z) {
    m_coord.z = z;
    InvalidateCache();
}

void Point::SetM(double m) {
    m_coord.m = m;
}

PointPtr Point::operator+(const Point& rhs) const {
    return Create(m_coord + rhs.m_coord);
}

PointPtr Point::operator-(const Point& rhs) const {
    return Create(m_coord - rhs.m_coord);
}

PointPtr Point::operator*(double scalar) const {
    return Create(m_coord * scalar);
}

double Point::Dot(const Point& other) const {
    return m_coord.Dot(other.m_coord);
}

double Point::Cross(const Point& other) const {
    return m_coord.Cross(other.m_coord).z;
}

bool Point::IsCollinear(const Point& p1, const Point& p2, double tolerance) const {
    double area = (p1.m_coord.x - m_coord.x) * (p2.m_coord.y - m_coord.y) -
                  (p2.m_coord.x - m_coord.x) * (p1.m_coord.y - m_coord.y);
    return std::abs(area) <= tolerance;
}

bool Point::IsBetween(const Point& p1, const Point& p2) const {
    double minX = std::min(p1.m_coord.x, p2.m_coord.x);
    double maxX = std::max(p1.m_coord.x, p2.m_coord.x);
    double minY = std::min(p1.m_coord.y, p2.m_coord.y);
    double maxY = std::max(p1.m_coord.y, p2.m_coord.y);
    
    return m_coord.x >= minX && m_coord.x <= maxX &&
           m_coord.y >= minY && m_coord.y <= maxY;
}

PointPtr Point::Normalize() const {
    return Create(m_coord.Normalize());
}

PointPtr Point::FromPolar(double radius, double angle) {
    return Create(Coordinate::FromPolar(radius, angle));
}

PointPtr Point::Centroid(const CoordinateList& coords) {
    if (coords.empty()) return Create(Coordinate::Empty());
    
    double sumX = 0, sumY = 0, sumZ = 0;
    bool hasZ = false;
    size_t count = 0;
    
    for (const auto& coord : coords) {
        if (!coord.IsEmpty()) {
            sumX += coord.x;
            sumY += coord.y;
            if (coord.Is3D()) {
                sumZ += coord.z;
                hasZ = true;
            }
            count++;
        }
    }
    
    if (count == 0) return Create(Coordinate::Empty());
    
    if (hasZ) {
        return Create(sumX / count, sumY / count, sumZ / count);
    }
    return Create(sumX / count, sumY / count);
}

Coordinate Point::GetCoordinateN(size_t index) const {
    if (index != 0) {
        GLM_THROW(GeomResult::kOutOfRange, "Point has only one coordinate");
    }
    return m_coord;
}

CoordinateList Point::GetCoordinates() const {
    if (IsEmpty()) return CoordinateList();
    return {m_coord};
}

std::string Point::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "POINT EMPTY";
    }
    
    oss << "POINT(";
    oss << m_coord.x << " " << m_coord.y;
    
    if (Is3D()) {
        oss << " " << m_coord.z;
    }
    if (IsMeasured()) {
        oss << " " << m_coord.m;
    }
    
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> Point::AsBinary() const {
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kPoint));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kPoint), Is3D(), IsMeasured());
    wkb::WriteCoordinate(wkb, m_coord.x, m_coord.y, m_coord.z, m_coord.m, Is3D(), IsMeasured());
    
    return wkb;
}

GeometryPtr Point::Clone() const {
    return Create(m_coord);
}

GeometryPtr Point::CloneEmpty() const {
    return Create(Coordinate::Empty());
}

Envelope Point::ComputeEnvelope() const {
    if (IsEmpty()) return Envelope();
    return Envelope(m_coord, m_coord);
}

std::string Point::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"Point\",\"coordinates\":[]}";
    }
    
    std::string coords;
    if (IsMeasured()) {
        coords = geojson::Coordinate4D(m_coord.x, m_coord.y, m_coord.z, m_coord.m, precision);
    } else if (Is3D()) {
        coords = geojson::Coordinate3D(m_coord.x, m_coord.y, m_coord.z, precision);
    } else {
        coords = geojson::Coordinate(m_coord.x, m_coord.y, precision);
    }
    
    return geojson::Point(coords, precision);
}

std::string Point::AsGML() const {
    if (IsEmpty()) {
        return "<gml:Point/>";
    }
    
    std::string pos;
    if (Is3D()) {
        pos = gml::Pos3D(m_coord.x, m_coord.y, m_coord.z);
    } else {
        pos = gml::Pos(m_coord.x, m_coord.y);
    }
    
    return gml::Point(pos);
}

std::string Point::AsKML() const {
    if (IsEmpty()) {
        return "<Point/>";
    }
    
    std::string coords;
    if (Is3D()) {
        coords = kml::Coordinate3D(m_coord.x, m_coord.y, m_coord.z);
    } else {
        coords = kml::Coordinate(m_coord.x, m_coord.y);
    }
    
    return kml::Point(coords);
}

void Point::Apply(GeometryVisitor& visitor) {
    visitor.VisitPoint(this);
}

void Point::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitPoint(this);
}

}
