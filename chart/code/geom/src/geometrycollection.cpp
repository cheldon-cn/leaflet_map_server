#include "ogc/geometrycollection.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/visitor.h"
#include "ogc/serialization_utils.h"
#include <sstream>
#include <iomanip>
#include <functional>

namespace ogc {

GeometryCollectionPtr GeometryCollection::Create() {
    return GeometryCollectionPtr(new GeometryCollection());
}

GeometryCollectionPtr GeometryCollection::Create(std::vector<GeometryPtr> geometries) {
    auto gc = new GeometryCollection();
    gc->m_geometries = std::move(geometries);
    return GeometryCollectionPtr(gc);
}

Dimension GeometryCollection::GetDimension() const noexcept {
    if (m_geometries.empty()) {
        return Dimension::Unknown;
    }
    Dimension maxDim = Dimension::Point;
    for (const auto& geom : m_geometries) {
        Dimension dim = geom->GetDimension();
        if (static_cast<int>(dim) > static_cast<int>(maxDim)) {
            maxDim = dim;
        }
    }
    return maxDim;
}

int GeometryCollection::GetCoordinateDimension() const noexcept {
    if (m_geometries.empty()) return 2;
    return m_geometries.front()->GetCoordinateDimension();
}

bool GeometryCollection::Is3D() const noexcept {
    for (const auto& geom : m_geometries) {
        if (geom->Is3D()) return true;
    }
    return false;
}

bool GeometryCollection::IsMeasured() const noexcept {
    for (const auto& geom : m_geometries) {
        if (geom->IsMeasured()) return true;
    }
    return false;
}

const Geometry* GeometryCollection::GetGeometryN(size_t index) const {
    if (index >= m_geometries.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Geometry index out of range");
    }
    return m_geometries[index].get();
}

void GeometryCollection::AddGeometry(GeometryPtr geometry) {
    m_geometries.push_back(std::move(geometry));
    InvalidateCache();
}

void GeometryCollection::RemoveGeometry(size_t index) {
    if (index >= m_geometries.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Geometry index out of range");
    }
    m_geometries.erase(m_geometries.begin() + index);
    InvalidateCache();
}

void GeometryCollection::Clear() {
    m_geometries.clear();
    InvalidateCache();
}

GeometryPtr GeometryCollection::GetGeometryNPtr(size_t index) const {
    if (index >= m_geometries.size()) {
        return nullptr;
    }
    return m_geometries[index]->Clone();
}

double GeometryCollection::Area() const {
    double total = 0.0;
    for (const auto& geom : m_geometries) {
        total += geom->Area();
    }
    return total;
}

double GeometryCollection::Length() const {
    double total = 0.0;
    for (const auto& geom : m_geometries) {
        total += geom->Length();
    }
    return total;
}

GeometryCollectionPtr GeometryCollection::Flatten() const {
    auto result = Create();
    
    std::function<void(const Geometry*)> flattenRecursive;
    flattenRecursive = [&](const Geometry* geom) {
        if (geom->GetGeometryType() == GeomType::kGeometryCollection) {
            const GeometryCollection* gc = static_cast<const GeometryCollection*>(geom);
            for (size_t i = 0; i < gc->GetNumGeometries(); ++i) {
                flattenRecursive(gc->GetGeometryN(i));
            }
        } else {
            result->AddGeometry(geom->Clone());
        }
    };
    
    for (const auto& geom : m_geometries) {
        flattenRecursive(geom.get());
    }
    
    return result;
}

size_t GeometryCollection::GetNumCoordinates() const noexcept {
    size_t count = 0;
    for (const auto& geom : m_geometries) {
        count += geom->GetNumCoordinates();
    }
    return count;
}

Coordinate GeometryCollection::GetCoordinateN(size_t index) const {
    size_t count = 0;
    for (const auto& geom : m_geometries) {
        size_t n = geom->GetNumCoordinates();
        if (index < count + n) {
            return geom->GetCoordinateN(index - count);
        }
        count += n;
    }
    GLM_THROW(GeomResult::kOutOfRange, "Coordinate index out of range");
    return Coordinate();
}

CoordinateList GeometryCollection::GetCoordinates() const {
    CoordinateList coords;
    for (const auto& geom : m_geometries) {
        auto geomCoords = geom->GetCoordinates();
        coords.insert(coords.end(), geomCoords.begin(), geomCoords.end());
    }
    return coords;
}

std::string GeometryCollection::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "GEOMETRYCOLLECTION EMPTY";
    }
    
    oss << "GEOMETRYCOLLECTION(";
    for (size_t i = 0; i < m_geometries.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << m_geometries[i]->AsText(precision);
    }
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> GeometryCollection::AsBinary() const {
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kGeometryCollection));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kGeometryCollection), Is3D(), IsMeasured());
    wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(m_geometries.size()));
    
    for (const auto& geom : m_geometries) {
        auto geomWkb = geom->AsBinary();
        wkb.insert(wkb.end(), geomWkb.begin(), geomWkb.end());
    }
    
    return wkb;
}

GeometryPtr GeometryCollection::Clone() const {
    auto clone = Create();
    for (const auto& geom : m_geometries) {
        clone->m_geometries.push_back(geom->Clone());
    }
    return clone;
}

GeometryPtr GeometryCollection::CloneEmpty() const {
    return Create();
}

std::string GeometryCollection::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"GeometryCollection\",\"geometries\":[]}";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < m_geometries.size(); ++i) {
        if (i > 0) oss << ",";
        oss << m_geometries[i]->AsGeoJSON(precision);
    }
    
    return geojson::GeometryCollection(oss.str());
}

std::string GeometryCollection::AsGML() const {
    if (IsEmpty()) {
        return "<gml:MultiGeometry/>";
    }
    
    std::ostringstream oss;
    for (const auto& geom : m_geometries) {
        oss << gml::GeometryMember(geom->AsGML());
    }
    
    return gml::GeometryCollection(oss.str());
}

std::string GeometryCollection::AsKML() const {
    if (IsEmpty()) {
        return "<MultiGeometry/>";
    }
    
    std::ostringstream oss;
    for (const auto& geom : m_geometries) {
        oss << geom->AsKML();
    }
    
    return kml::MultiGeometry(oss.str());
}

void GeometryCollection::Apply(GeometryVisitor& visitor) {
    visitor.VisitGeometryCollection(this);
    for (auto& geom : m_geometries) {
        geom->Apply(visitor);
    }
}

void GeometryCollection::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitGeometryCollection(this);
    for (const auto& geom : m_geometries) {
        geom->Apply(visitor);
    }
}

Envelope GeometryCollection::ComputeEnvelope() const {
    Envelope env;
    for (const auto& geom : m_geometries) {
        env.ExpandToInclude(geom->GetEnvelope());
    }
    return env;
}

Coordinate GeometryCollection::ComputeCentroid() const {
    if (m_geometries.empty()) return Coordinate::Empty();
    auto coords = GetCoordinates();
    if (coords.empty()) return Coordinate::Empty();
    
    double sumX = 0.0, sumY = 0.0;
    for (const auto& coord : coords) {
        sumX += coord.x;
        sumY += coord.y;
    }
    return Coordinate(sumX / coords.size(), sumY / coords.size());
}

}
