#include "ogc/multipoint.h"
#include "ogc/visitor.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {

MultiPointPtr MultiPoint::Create() {
    return MultiPointPtr(new MultiPoint());
}

MultiPointPtr MultiPoint::Create(const CoordinateList& coords) {
    auto mp = new MultiPoint();
    for (const auto& coord : coords) {
        mp->m_points.push_back(Point::Create(coord));
    }
    return MultiPointPtr(mp);
}

int MultiPoint::GetCoordinateDimension() const noexcept {
    if (m_points.empty()) return 2;
    return m_points.front()->GetCoordinateDimension();
}

bool MultiPoint::Is3D() const noexcept {
    for (const auto& point : m_points) {
        if (point->Is3D()) return true;
    }
    return false;
}

bool MultiPoint::IsMeasured() const noexcept {
    for (const auto& point : m_points) {
        if (point->IsMeasured()) return true;
    }
    return false;
}

const Geometry* MultiPoint::GetGeometryN(size_t index) const {
    if (index >= m_points.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Geometry index out of range");
    }
    return m_points[index].get();
}

void MultiPoint::AddPoint(PointPtr point) {
    m_points.push_back(std::move(point));
    InvalidateCache();
}

void MultiPoint::AddPoint(const Coordinate& coord) {
    m_points.push_back(Point::Create(coord));
    InvalidateCache();
}

void MultiPoint::RemovePoint(size_t index) {
    if (index >= m_points.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Point index out of range");
    }
    m_points.erase(m_points.begin() + index);
    InvalidateCache();
}

void MultiPoint::Clear() {
    m_points.clear();
    InvalidateCache();
}

const Point* MultiPoint::GetPointN(size_t index) const {
    if (index >= m_points.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Point index out of range");
    }
    return m_points[index].get();
}

void MultiPoint::Merge(const MultiPoint* other) {
    if (!other) return;
    for (const auto& point : other->m_points) {
        m_points.push_back(Point::Create(point->GetCoordinate()));
    }
    InvalidateCache();
}

void MultiPoint::UniquePoints(double tolerance) {
    std::vector<PointPtr> unique;
    for (const auto& point : m_points) {
        bool found = false;
        for (const auto& existing : unique) {
            if (existing->GetCoordinate().Equals(point->GetCoordinate(), tolerance)) {
                found = true;
                break;
            }
        }
        if (!found) {
            unique.push_back(Point::Create(point->GetCoordinate()));
        }
    }
    m_points = std::move(unique);
    InvalidateCache();
}

bool MultiPoint::ContainsPoint(const Coordinate& coord, double tolerance) const {
    for (const auto& point : m_points) {
        if (point->GetCoordinate().Equals(coord, tolerance)) {
            return true;
        }
    }
    return false;
}

bool MultiPoint::ContainsMultiPoint(const MultiPoint* other) const {
    if (!other) return false;
    for (const auto& point : other->m_points) {
        if (!ContainsPoint(point->GetCoordinate())) {
            return false;
        }
    }
    return true;
}

void MultiPoint::SortByX() {
    std::sort(m_points.begin(), m_points.end(),
              [](const PointPtr& a, const PointPtr& b) {
                  return a->GetX() < b->GetX();
              });
}

void MultiPoint::SortByY() {
    std::sort(m_points.begin(), m_points.end(),
              [](const PointPtr& a, const PointPtr& b) {
                  return a->GetY() < b->GetY();
              });
}

Coordinate MultiPoint::GetCoordinateN(size_t index) const {
    return GetPointN(index)->GetCoordinate();
}

CoordinateList MultiPoint::GetCoordinates() const {
    CoordinateList coords;
    for (const auto& point : m_points) {
        coords.push_back(point->GetCoordinate());
    }
    return coords;
}

std::string MultiPoint::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "MULTIPOINT EMPTY";
    }
    
    oss << "MULTIPOINT(";
    for (size_t i = 0; i < m_points.size(); ++i) {
        if (i > 0) oss << ", ";
        const Coordinate& coord = m_points[i]->GetCoordinate();
        oss << "(" << coord.x << " " << coord.y;
        if (coord.Is3D()) oss << " " << coord.z;
        if (coord.IsMeasured()) oss << " " << coord.m;
        oss << ")";
    }
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> MultiPoint::AsBinary() const {
    return std::vector<uint8_t>();
}

GeometryPtr MultiPoint::Clone() const {
    auto clone = Create();
    for (const auto& point : m_points) {
        clone->m_points.push_back(Point::Create(point->GetCoordinate()));
    }
    return clone;
}

GeometryPtr MultiPoint::CloneEmpty() const {
    return Create();
}

void MultiPoint::Apply(GeometryVisitor& visitor) {
    visitor.VisitMultiPoint(this);
}

void MultiPoint::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitMultiPoint(this);
}

Envelope MultiPoint::ComputeEnvelope() const {
    Envelope env;
    for (const auto& point : m_points) {
        env.ExpandToInclude(point->GetCoordinate());
    }
    return env;
}

Coordinate MultiPoint::ComputeCentroid() const {
    if (m_points.empty()) return Coordinate::Empty();
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
