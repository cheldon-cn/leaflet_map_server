#include "ogc/geom/polygon.h"
#include "ogc/geom/visitor.h"
#include "ogc/geom/serialization_utils.h"
#include "geometry_impl_internal.h"
#include <sstream>
#include <iomanip>

namespace ogc {

struct Polygon::Impl {
    LinearRingPtr exteriorRing;
    std::vector<LinearRingPtr> interiorRings;
};

Polygon::Polygon() : impl_(std::make_unique<Impl>()) {
}

Polygon::~Polygon() = default;

LinearRingPtr& Polygon::GetExteriorRingRef() {
    return impl_->exteriorRing;
}

std::vector<LinearRingPtr>& Polygon::GetInteriorRingsRef() {
    return impl_->interiorRings;
}

PolygonPtr Polygon::Create() {
    return PolygonPtr(new Polygon());
}

PolygonPtr Polygon::Create(LinearRingPtr exteriorRing) {
    auto polygon = new Polygon();
    polygon->impl_->exteriorRing = std::move(exteriorRing);
    return PolygonPtr(polygon);
}

bool Polygon::IsEmpty() const noexcept {
    return !impl_->exteriorRing || impl_->exteriorRing->IsEmpty();
}

bool Polygon::Is3D() const noexcept {
    return impl_->exteriorRing && impl_->exteriorRing->Is3D();
}

bool Polygon::IsMeasured() const noexcept {
    return impl_->exteriorRing && impl_->exteriorRing->IsMeasured();
}

int Polygon::GetCoordinateDimension() const noexcept {
    if (!impl_->exteriorRing) return 2;
    return impl_->exteriorRing->GetCoordinateDimension();
}

void Polygon::SetExteriorRing(LinearRingPtr ring) {
    impl_->exteriorRing = std::move(ring);
    InvalidateCache();
}

const LinearRing* Polygon::GetExteriorRing() const noexcept {
    return impl_->exteriorRing.get();
}

void Polygon::AddInteriorRing(LinearRingPtr ring) {
    impl_->interiorRings.push_back(std::move(ring));
    InvalidateCache();
}

void Polygon::RemoveInteriorRing(size_t index) {
    if (index >= impl_->interiorRings.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Interior ring index out of range");
    }
    impl_->interiorRings.erase(impl_->interiorRings.begin() + index);
    InvalidateCache();
}

const LinearRing* Polygon::GetInteriorRingN(size_t index) const {
    if (index >= impl_->interiorRings.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Interior ring index out of range");
    }
    return impl_->interiorRings[index].get();
}

size_t Polygon::GetNumInteriorRings() const noexcept {
    return impl_->interiorRings.size();
}

size_t Polygon::GetNumRings() const noexcept {
    return (impl_->exteriorRing ? 1 : 0) + impl_->interiorRings.size();
}

void Polygon::ClearRings() {
    impl_->exteriorRing.reset();
    impl_->interiorRings.clear();
    InvalidateCache();
}

size_t Polygon::GetNumCoordinates() const noexcept {
    size_t count = 0;
    if (impl_->exteriorRing) {
        count += impl_->exteriorRing->GetNumCoordinates();
    }
    for (const auto& ring : impl_->interiorRings) {
        count += ring->GetNumCoordinates();
    }
    return count;
}

Coordinate Polygon::GetCoordinateN(size_t index) const {
    size_t count = 0;
    if (impl_->exteriorRing) {
        size_t n = impl_->exteriorRing->GetNumCoordinates();
        if (index < count + n) {
            return impl_->exteriorRing->GetCoordinateN(index - count);
        }
        count += n;
    }
    for (const auto& ring : impl_->interiorRings) {
        size_t n = ring->GetNumCoordinates();
        if (index < count + n) {
            return ring->GetCoordinateN(index - count);
        }
        count += n;
    }
    GLM_THROW(GeomResult::kOutOfRange, "Coordinate index out of range");
    return Coordinate();
}

CoordinateList Polygon::GetCoordinates() const {
    CoordinateList coords;
    if (impl_->exteriorRing) {
        auto exteriorCoords = impl_->exteriorRing->GetCoordinates();
        coords.insert(coords.end(), exteriorCoords.begin(), exteriorCoords.end());
    }
    for (const auto& ring : impl_->interiorRings) {
        auto ringCoords = ring->GetCoordinates();
        coords.insert(coords.end(), ringCoords.begin(), ringCoords.end());
    }
    return coords;
}

double Polygon::Area() const {
    if (!impl_->exteriorRing) return 0.0;
    
    double area = impl_->exteriorRing->Area();
    
    for (const auto& ring : impl_->interiorRings) {
        area -= ring->Area();
    }
    
    return area;
}

double Polygon::GetPerimeter() const {
    if (!impl_->exteriorRing) return 0.0;
    
    double perimeter = impl_->exteriorRing->Length();
    
    for (const auto& ring : impl_->interiorRings) {
        perimeter += ring->Length();
    }
    
    return perimeter;
}

double Polygon::GetExteriorArea() const {
    if (!impl_->exteriorRing) return 0.0;
    return impl_->exteriorRing->Area();
}

double Polygon::GetInteriorArea() const {
    double area = 0.0;
    for (const auto& ring : impl_->interiorRings) {
        area += ring->Area();
    }
    return area;
}

double Polygon::GetSolidity() const {
    double totalArea = GetExteriorArea();
    if (totalArea == 0.0) return 0.0;
    return Area() / totalArea;
}

double Polygon::GetCompactness() const {
    double area = Area();
    if (area <= 0.0) return 0.0;
    double perimeter = GetPerimeter();
    return 4.0 * PI * area / (perimeter * perimeter);
}

PolygonPtr Polygon::RemoveHoles() const {
    if (!impl_->exteriorRing) return Create();
    return Create(LinearRingPtr(static_cast<LinearRing*>(impl_->exteriorRing->Clone().release())));
}

PolygonPtr Polygon::MergeHoles() const {
    return RemoveHoles();
}

std::vector<LinearRing::Triangle> Polygon::Triangulate() const {
    std::vector<LinearRing::Triangle> triangles;
    return triangles;
}

bool Polygon::ContainsRing(const LinearRing* ring) const {
    if (!impl_->exteriorRing || !ring) return false;
    
    for (const auto& coord : ring->GetCoordinates()) {
        if (!impl_->exteriorRing->ContainsPoint(coord)) {
            return false;
        }
    }
    
    for (const auto& interiorRing : impl_->interiorRings) {
        for (const auto& coord : ring->GetCoordinates()) {
            if (interiorRing->ContainsPoint(coord)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Polygon::ContainsPolygon(const Polygon* polygon) const {
    if (!polygon) return false;
    return ContainsRing(polygon->GetExteriorRing());
}

GeomResult Polygon::UnionWithPolygon(const Polygon* other, PolygonPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Polygon::IntersectWithPolygon(const Polygon* other, PolygonPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Polygon::DifferenceWithPolygon(const Polygon* other, PolygonPtr& result) const {
    return GeomResult::kNotImplemented;
}

PolygonPtr Polygon::CreateRectangle(double minX, double minY, double maxX, double maxY) {
    auto ring = LinearRing::CreateRectangle(minX, minY, maxX, maxY);
    return Create(std::move(ring));
}

PolygonPtr Polygon::CreateCircle(double centerX, double centerY, double radius, int segments) {
    auto ring = LinearRing::CreateCircle(centerX, centerY, radius, segments);
    return Create(std::move(ring));
}

PolygonPtr Polygon::CreateRegularPolygon(double centerX, double centerY, double radius, int sides) {
    if (sides < 3) return nullptr;
    
    CoordinateList coords;
    double angleStep = 2.0 * 3.14159265358979323846 / sides;
    
    for (int i = 0; i <= sides; ++i) {
        double angle = i * angleStep - 3.14159265358979323846 / 2.0;
        double x = centerX + radius * std::cos(angle);
        double y = centerY + radius * std::sin(angle);
        coords.push_back(Coordinate(x, y));
    }
    
    auto ring = LinearRing::Create(coords, false);
    return Create(std::move(ring));
}

PolygonPtr Polygon::CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3) {
    CoordinateList coords;
    coords.push_back(p1);
    coords.push_back(p2);
    coords.push_back(p3);
    coords.push_back(p1);
    auto ring = LinearRing::Create(coords, false);
    return Create(std::move(ring));
}

std::string Polygon::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "POLYGON EMPTY";
    }
    
    oss << "POLYGON(";
    
    oss << "(";
    const auto& exteriorCoords = impl_->exteriorRing->GetCoordinates();
    for (size_t i = 0; i < exteriorCoords.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << exteriorCoords[i].x << " " << exteriorCoords[i].y;
        if (exteriorCoords[i].Is3D()) oss << " " << exteriorCoords[i].z;
    }
    oss << ")";
    
    for (const auto& ring : impl_->interiorRings) {
        oss << ", (";
        const auto& ringCoords = ring->GetCoordinates();
        for (size_t i = 0; i < ringCoords.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << ringCoords[i].x << " " << ringCoords[i].y;
            if (ringCoords[i].Is3D()) oss << " " << ringCoords[i].z;
        }
        oss << ")";
    }
    
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> Polygon::AsBinary() const {
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kPolygon));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kPolygon), Is3D(), IsMeasured());
    
    uint32_t numRings = 1 + static_cast<uint32_t>(impl_->interiorRings.size());
    wkb::WriteUInt32LE(wkb, numRings);
    
    auto writeRing = [this](std::vector<uint8_t>& wkb, const LinearRing* ring) {
        if (!ring) return;
        const auto& coords = ring->GetCoordinates();
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(coords.size()));
        for (const auto& coord : coords) {
            wkb::WriteCoordinate(wkb, coord.x, coord.y, coord.z, coord.m, Is3D(), IsMeasured());
        }
    };
    
    writeRing(wkb, impl_->exteriorRing.get());
    
    for (const auto& ring : impl_->interiorRings) {
        writeRing(wkb, ring.get());
    }
    
    return wkb;
}

GeometryPtr Polygon::Clone() const {
    auto polygon = Create();
    if (impl_->exteriorRing) {
        polygon->impl_->exteriorRing = LinearRingPtr(
            static_cast<LinearRing*>(impl_->exteriorRing->Clone().release()));
    }
    for (const auto& ring : impl_->interiorRings) {
        polygon->impl_->interiorRings.push_back(
            LinearRingPtr(static_cast<LinearRing*>(ring->Clone().release())));
    }
    return polygon;
}

GeometryPtr Polygon::CloneEmpty() const {
    return Create();
}

Envelope Polygon::ComputeEnvelope() const {
    if (!impl_->exteriorRing) return Envelope();
    return impl_->exteriorRing->GetEnvelope();
}

Coordinate Polygon::ComputeCentroid() const {
    if (!impl_->exteriorRing) return Coordinate::Empty();
    return impl_->exteriorRing->GetCentroid();
}

std::string Polygon::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"Polygon\",\"coordinates\":[]}";
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
    
    std::ostringstream oss;
    oss << "[";
    writeRing(oss, impl_->exteriorRing.get());
    for (size_t i = 0; i < impl_->interiorRings.size(); ++i) {
        oss << ",";
        writeRing(oss, impl_->interiorRings[i].get());
    }
    oss << "]";
    
    return geojson::Polygon(oss.str(), precision);
}

std::string Polygon::AsGML() const {
    if (IsEmpty()) {
        return "<gml:Polygon/>";
    }
    
    auto writeRingPosList = [this](std::ostringstream& oss, const LinearRing* ring) {
        if (!ring) return;
        const auto& coords = ring->GetCoordinates();
        for (size_t i = 0; i < coords.size(); ++i) {
            if (i > 0) oss << " ";
            if (Is3D()) {
                oss << gml::Number(coords[i].x) << " " 
                    << gml::Number(coords[i].y) << " " 
                    << gml::Number(coords[i].z);
            } else {
                oss << gml::Number(coords[i].x) << " " 
                    << gml::Number(coords[i].y);
            }
        }
    };
    
    std::ostringstream exterior;
    writeRingPosList(exterior, impl_->exteriorRing.get());
    
    std::string interior;
    if (!impl_->interiorRings.empty()) {
        std::ostringstream intOss;
        for (size_t i = 0; i < impl_->interiorRings.size(); ++i) {
            std::ostringstream ringOss;
            writeRingPosList(ringOss, impl_->interiorRings[i].get());
            if (i > 0) intOss << " ";
            intOss << ringOss.str();
        }
        interior = intOss.str();
    }
    
    return gml::Polygon(exterior.str(), interior);
}

std::string Polygon::AsKML() const {
    if (IsEmpty()) {
        return "<Polygon/>";
    }
    
    auto writeRingCoords = [this](std::ostringstream& oss, const LinearRing* ring) {
        if (!ring) return;
        const auto& coords = ring->GetCoordinates();
        for (size_t i = 0; i < coords.size(); ++i) {
            if (i > 0) oss << " ";
            if (Is3D()) {
                oss << kml::Coordinate3D(coords[i].x, coords[i].y, coords[i].z);
            } else {
                oss << kml::Coordinate(coords[i].x, coords[i].y);
            }
        }
    };
    
    std::ostringstream exterior;
    writeRingCoords(exterior, impl_->exteriorRing.get());
    
    std::string interior;
    if (!impl_->interiorRings.empty()) {
        std::ostringstream intOss;
        for (size_t i = 0; i < impl_->interiorRings.size(); ++i) {
            std::ostringstream ringOss;
            writeRingCoords(ringOss, impl_->interiorRings[i].get());
            if (i > 0) intOss << " ";
            intOss << ringOss.str();
        }
        interior = intOss.str();
    }
    
    return kml::Polygon(exterior.str(), interior);
}

void Polygon::Apply(GeometryVisitor& visitor) {
    visitor.VisitPolygon(this);
}

void Polygon::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitPolygon(this);
}

}
