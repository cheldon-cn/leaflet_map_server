#include "ogc/multipolygon.h"
#include "ogc/visitor.h"
#include <sstream>
#include <iomanip>

namespace ogc {

MultiPolygonPtr MultiPolygon::Create() {
    return MultiPolygonPtr(new MultiPolygon());
}

MultiPolygonPtr MultiPolygon::Create(std::vector<PolygonPtr> polygons) {
    auto mp = new MultiPolygon();
    mp->m_polygons = std::move(polygons);
    return MultiPolygonPtr(mp);
}

int MultiPolygon::GetCoordinateDimension() const noexcept {
    if (m_polygons.empty()) return 2;
    return m_polygons.front()->GetCoordinateDimension();
}

bool MultiPolygon::Is3D() const noexcept {
    for (const auto& polygon : m_polygons) {
        if (polygon->Is3D()) return true;
    }
    return false;
}

bool MultiPolygon::IsMeasured() const noexcept {
    for (const auto& polygon : m_polygons) {
        if (polygon->IsMeasured()) return true;
    }
    return false;
}

const Geometry* MultiPolygon::GetGeometryN(size_t index) const {
    if (index >= m_polygons.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Geometry index out of range");
    }
    return m_polygons[index].get();
}

void MultiPolygon::AddPolygon(PolygonPtr polygon) {
    m_polygons.push_back(std::move(polygon));
    InvalidateCache();
}

void MultiPolygon::RemovePolygon(size_t index) {
    if (index >= m_polygons.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Polygon index out of range");
    }
    m_polygons.erase(m_polygons.begin() + index);
    InvalidateCache();
}

void MultiPolygon::Clear() {
    m_polygons.clear();
    InvalidateCache();
}

const Polygon* MultiPolygon::GetPolygonN(size_t index) const {
    if (index >= m_polygons.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Polygon index out of range");
    }
    return m_polygons[index].get();
}

double MultiPolygon::Area() const {
    double total = 0.0;
    for (const auto& polygon : m_polygons) {
        total += polygon->Area();
    }
    return total;
}

double MultiPolygon::Length() const {
    return GetPerimeter();
}

double MultiPolygon::GetPerimeter() const {
    double total = 0.0;
    for (const auto& polygon : m_polygons) {
        total += polygon->GetPerimeter();
    }
    return total;
}

bool MultiPolygon::Contains(const Geometry* other) const {
    if (!other) return false;
    
    for (const auto& polygon : m_polygons) {
        if (polygon->Contains(other)) {
            return true;
        }
    }
    return false;
}

bool MultiPolygon::Intersects(const Geometry* other) const {
    if (!other) return false;
    
    for (const auto& polygon : m_polygons) {
        if (polygon->Intersects(other)) {
            return true;
        }
    }
    return false;
}

MultiPolygonPtr MultiPolygon::Union() const {
    return Create();
}

MultiPolygonPtr MultiPolygon::RemoveHoles() const {
    auto result = Create();
    for (const auto& polygon : m_polygons) {
        result->m_polygons.push_back(polygon->RemoveHoles());
    }
    return result;
}

size_t MultiPolygon::GetNumCoordinates() const noexcept {
    size_t count = 0;
    for (const auto& polygon : m_polygons) {
        count += polygon->GetNumCoordinates();
    }
    return count;
}

Coordinate MultiPolygon::GetCoordinateN(size_t index) const {
    size_t count = 0;
    for (const auto& polygon : m_polygons) {
        size_t n = polygon->GetNumCoordinates();
        if (index < count + n) {
            return polygon->GetCoordinateN(index - count);
        }
        count += n;
    }
    GLM_THROW(GeomResult::kOutOfRange, "Coordinate index out of range");
    return Coordinate();
}

CoordinateList MultiPolygon::GetCoordinates() const {
    CoordinateList coords;
    for (const auto& polygon : m_polygons) {
        auto polyCoords = polygon->GetCoordinates();
        coords.insert(coords.end(), polyCoords.begin(), polyCoords.end());
    }
    return coords;
}

std::string MultiPolygon::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "MULTIPOLYGON EMPTY";
    }
    
    oss << "MULTIPOLYGON(";
    for (size_t i = 0; i < m_polygons.size(); ++i) {
        if (i > 0) oss << ", ";
        
        const Polygon* poly = m_polygons[i].get();
        oss << "(";
        
        const LinearRing* exterior = poly->GetExteriorRing();
        if (exterior) {
            oss << "(";
            const auto& extCoords = exterior->GetCoordinates();
            for (size_t j = 0; j < extCoords.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << extCoords[j].x << " " << extCoords[j].y;
            }
            oss << ")";
        }
        
        for (size_t h = 0; h < poly->GetNumInteriorRings(); ++h) {
            oss << ", (";
            const LinearRing* hole = poly->GetInteriorRingN(h);
            const auto& holeCoords = hole->GetCoordinates();
            for (size_t j = 0; j < holeCoords.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << holeCoords[j].x << " " << holeCoords[j].y;
            }
            oss << ")";
        }
        
        oss << ")";
    }
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> MultiPolygon::AsBinary() const {
    return std::vector<uint8_t>();
}

GeometryPtr MultiPolygon::Clone() const {
    auto clone = Create();
    for (const auto& polygon : m_polygons) {
        clone->m_polygons.push_back(PolygonPtr(
            static_cast<Polygon*>(polygon->Clone().release())));
    }
    return clone;
}

GeometryPtr MultiPolygon::CloneEmpty() const {
    return Create();
}

void MultiPolygon::Apply(GeometryVisitor& visitor) {
    visitor.VisitMultiPolygon(this);
}

void MultiPolygon::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitMultiPolygon(this);
}

Envelope MultiPolygon::ComputeEnvelope() const {
    Envelope env;
    for (const auto& polygon : m_polygons) {
        env.ExpandToInclude(polygon->GetEnvelope());
    }
    return env;
}

Coordinate MultiPolygon::ComputeCentroid() const {
    if (m_polygons.empty()) return Coordinate::Empty();
    
    double totalArea = 0.0;
    double cx = 0.0, cy = 0.0;
    
    for (const auto& polygon : m_polygons) {
        double area = polygon->Area();
        Coordinate centroid = polygon->GetCentroid();
        
        cx += centroid.x * area;
        cy += centroid.y * area;
        totalArea += area;
    }
    
    if (totalArea == 0.0) return Coordinate::Empty();
    return Coordinate(cx / totalArea, cy / totalArea);
}

}
