#include "ogc/polygon.h"
#include "ogc/visitor.h"
#include <sstream>
#include <iomanip>

namespace ogc {

PolygonPtr Polygon::Create() {
    return PolygonPtr(new Polygon());
}

PolygonPtr Polygon::Create(LinearRingPtr exteriorRing) {
    auto polygon = new Polygon();
    polygon->m_exteriorRing = std::move(exteriorRing);
    return PolygonPtr(polygon);
}

bool Polygon::IsEmpty() const noexcept {
    return !m_exteriorRing || m_exteriorRing->IsEmpty();
}

bool Polygon::Is3D() const noexcept {
    return m_exteriorRing && m_exteriorRing->Is3D();
}

bool Polygon::IsMeasured() const noexcept {
    return m_exteriorRing && m_exteriorRing->IsMeasured();
}

int Polygon::GetCoordinateDimension() const noexcept {
    if (!m_exteriorRing) return 2;
    return m_exteriorRing->GetCoordinateDimension();
}

void Polygon::SetExteriorRing(LinearRingPtr ring) {
    m_exteriorRing = std::move(ring);
    InvalidateCache();
}

const LinearRing* Polygon::GetExteriorRing() const noexcept {
    return m_exteriorRing.get();
}

void Polygon::AddInteriorRing(LinearRingPtr ring) {
    m_interiorRings.push_back(std::move(ring));
    InvalidateCache();
}

void Polygon::RemoveInteriorRing(size_t index) {
    if (index >= m_interiorRings.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Interior ring index out of range");
    }
    m_interiorRings.erase(m_interiorRings.begin() + index);
    InvalidateCache();
}

const LinearRing* Polygon::GetInteriorRingN(size_t index) const {
    if (index >= m_interiorRings.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Interior ring index out of range");
    }
    return m_interiorRings[index].get();
}

void Polygon::ClearRings() {
    m_exteriorRing.reset();
    m_interiorRings.clear();
    InvalidateCache();
}

size_t Polygon::GetNumCoordinates() const noexcept {
    size_t count = 0;
    if (m_exteriorRing) {
        count += m_exteriorRing->GetNumCoordinates();
    }
    for (const auto& ring : m_interiorRings) {
        count += ring->GetNumCoordinates();
    }
    return count;
}

Coordinate Polygon::GetCoordinateN(size_t index) const {
    size_t count = 0;
    if (m_exteriorRing) {
        size_t n = m_exteriorRing->GetNumCoordinates();
        if (index < count + n) {
            return m_exteriorRing->GetCoordinateN(index - count);
        }
        count += n;
    }
    for (const auto& ring : m_interiorRings) {
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
    if (m_exteriorRing) {
        auto exteriorCoords = m_exteriorRing->GetCoordinates();
        coords.insert(coords.end(), exteriorCoords.begin(), exteriorCoords.end());
    }
    for (const auto& ring : m_interiorRings) {
        auto ringCoords = ring->GetCoordinates();
        coords.insert(coords.end(), ringCoords.begin(), ringCoords.end());
    }
    return coords;
}

double Polygon::Area() const {
    if (!m_exteriorRing) return 0.0;
    
    double area = m_exteriorRing->Area();
    
    for (const auto& ring : m_interiorRings) {
        area -= ring->Area();
    }
    
    return area;
}

double Polygon::GetPerimeter() const {
    if (!m_exteriorRing) return 0.0;
    
    double perimeter = m_exteriorRing->Length();
    
    for (const auto& ring : m_interiorRings) {
        perimeter += ring->Length();
    }
    
    return perimeter;
}

double Polygon::GetExteriorArea() const {
    if (!m_exteriorRing) return 0.0;
    return m_exteriorRing->Area();
}

double Polygon::GetInteriorArea() const {
    double area = 0.0;
    for (const auto& ring : m_interiorRings) {
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
    if (!m_exteriorRing) return Create();
    return Create(LinearRingPtr(static_cast<LinearRing*>(m_exteriorRing->Clone().release())));
}

PolygonPtr Polygon::MergeHoles() const {
    return RemoveHoles();
}

std::vector<LinearRing::Triangle> Polygon::Triangulate() const {
    std::vector<LinearRing::Triangle> triangles;
    return triangles;
}

bool Polygon::ContainsRing(const LinearRing* ring) const {
    if (!m_exteriorRing || !ring) return false;
    
    for (const auto& coord : ring->GetCoordinates()) {
        if (!m_exteriorRing->ContainsPoint(coord)) {
            return false;
        }
    }
    
    for (const auto& interiorRing : m_interiorRings) {
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
    const auto& exteriorCoords = m_exteriorRing->GetCoordinates();
    for (size_t i = 0; i < exteriorCoords.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << exteriorCoords[i].x << " " << exteriorCoords[i].y;
        if (exteriorCoords[i].Is3D()) oss << " " << exteriorCoords[i].z;
    }
    oss << ")";
    
    for (const auto& ring : m_interiorRings) {
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
    return wkb;
}

GeometryPtr Polygon::Clone() const {
    auto polygon = Create();
    if (m_exteriorRing) {
        polygon->m_exteriorRing = LinearRingPtr(
            static_cast<LinearRing*>(m_exteriorRing->Clone().release()));
    }
    for (const auto& ring : m_interiorRings) {
        polygon->m_interiorRings.push_back(
            LinearRingPtr(static_cast<LinearRing*>(ring->Clone().release())));
    }
    return polygon;
}

GeometryPtr Polygon::CloneEmpty() const {
    return Create();
}

Envelope Polygon::ComputeEnvelope() const {
    if (!m_exteriorRing) return Envelope();
    return m_exteriorRing->GetEnvelope();
}

Coordinate Polygon::ComputeCentroid() const {
    if (!m_exteriorRing) return Coordinate::Empty();
    return m_exteriorRing->GetCentroid();
}

void Polygon::Apply(GeometryVisitor& visitor) {
    visitor.VisitPolygon(this);
}

void Polygon::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitPolygon(this);
}

}
