#include "ogc/multipolygon.h"
#include "ogc/visitor.h"
#include "ogc/serialization_utils.h"
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
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kMultiPolygon));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kMultiPolygon), Is3D(), IsMeasured());
    wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(m_polygons.size()));
    
    for (const auto& polygon : m_polygons) {
        auto polygonWkb = polygon->AsBinary();
        wkb.insert(wkb.end(), polygonWkb.begin(), polygonWkb.end());
    }
    
    return wkb;
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

std::string MultiPolygon::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"MultiPolygon\",\"coordinates\":[]}";
    }
    
    auto writeRing = [this, precision](std::ostringstream& oss, const LinearRing* ring) {
        if (!ring) return;
        const auto& coords = ring->GetCoordinates();
        oss << "[";
        for (size_t i = 0; i < coords.size(); ++i) {
            if (i > 0) oss << ",";
            if (IsMeasured()) {
                oss << geojson::Coordinate4D(coords[i].x, coords[i].y, coords[i].z, coords[i].m, precision);
            } else if (Is3D()) {
                oss << geojson::Coordinate3D(coords[i].x, coords[i].y, coords[i].z, precision);
            } else {
                oss << geojson::Coordinate(coords[i].x, coords[i].y, precision);
            }
        }
        oss << "]";
    };
    
    auto writePolygon = [this, &writeRing, precision](std::ostringstream& oss, const Polygon* polygon) {
        oss << "[";
        writeRing(oss, polygon->GetExteriorRing());
        for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
            oss << ",";
            writeRing(oss, polygon->GetInteriorRingN(i));
        }
        oss << "]";
    };
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_polygons.size(); ++i) {
        if (i > 0) oss << ",";
        writePolygon(oss, m_polygons[i].get());
    }
    oss << "]";
    
    return geojson::MultiPolygon(oss.str(), precision);
}

std::string MultiPolygon::AsGML() const {
    if (IsEmpty()) {
        return "<gml:MultiSurface/>";
    }
    
    std::ostringstream oss;
    for (const auto& polygon : m_polygons) {
        oss << gml::SurfaceMember(polygon->AsGML());
    }
    
    return gml::MultiPolygon(oss.str());
}

std::string MultiPolygon::AsKML() const {
    if (IsEmpty()) {
        return "<MultiGeometry/>";
    }
    
    std::ostringstream oss;
    for (const auto& polygon : m_polygons) {
        oss << polygon->AsKML();
    }
    
    return kml::MultiGeometry(oss.str());
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
