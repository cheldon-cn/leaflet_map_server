#include "ogc/linearring.h"
#include <algorithm>

namespace ogc {

LinearRingPtr LinearRing::Create() {
    return LinearRingPtr(new LinearRing());
}

LinearRingPtr LinearRing::Create(const CoordinateList& coords, bool autoClose) {
    auto ring = new LinearRing();
    ring->m_coords = coords;
    if (autoClose) {
        ring->EnsureClosed();
    }
    return LinearRingPtr(ring);
}

bool LinearRing::IsValidRing(std::string* reason) const {
    if (m_coords.size() < 4) {
        if (reason) *reason = "LinearRing must have at least 4 points";
        return false;
    }
    
    if (!IsClosed()) {
        if (reason) *reason = "LinearRing must be closed";
        return false;
    }
    
    return true;
}

bool LinearRing::IsSimpleRing() const {
    return IsSimple();
}

double LinearRing::Area() const {
    if (m_coords.size() < 4) return 0.0;
    
    double area = 0.0;
    size_t n = m_coords.size() - 1;
    
    for (size_t i = 0; i < n; ++i) {
        area += m_coords[i].x * m_coords[i + 1].y;
        area -= m_coords[i + 1].x * m_coords[i].y;
    }
    
    return std::abs(area) / 2.0;
}

bool LinearRing::IsConvex() const {
    if (m_coords.size() < 4) return true;
    
    bool sign = false;
    bool hasSign = false;
    
    for (size_t i = 0; i < m_coords.size() - 1; ++i) {
        size_t j = (i + 1) % (m_coords.size() - 1);
        size_t k = (i + 2) % (m_coords.size() - 1);
        
        double cross = (m_coords[j].x - m_coords[i].x) * (m_coords[k].y - m_coords[j].y) -
                       (m_coords[j].y - m_coords[i].y) * (m_coords[k].x - m_coords[j].x);
        
        if (cross != 0) {
            bool currentSign = cross > 0;
            if (hasSign && currentSign != sign) {
                return false;
            }
            sign = currentSign;
            hasSign = true;
        }
    }
    
    return true;
}

bool LinearRing::IsClockwise() const {
    if (m_coords.size() < 4) return false;
    
    double sum = 0.0;
    size_t n = m_coords.size() - 1;
    
    for (size_t i = 0; i < n; ++i) {
        sum += (m_coords[i + 1].x - m_coords[i].x) * (m_coords[i + 1].y + m_coords[i].y);
    }
    
    return sum > 0;
}

bool LinearRing::IsCounterClockwise() const {
    return !IsClockwise();
}

void LinearRing::Reverse() {
    std::reverse(m_coords.begin(), m_coords.end());
    InvalidateCache();
}

void LinearRing::ForceClockwise() {
    if (!IsClockwise()) {
        Reverse();
    }
}

void LinearRing::ForceCounterClockwise() {
    if (IsClockwise()) {
        Reverse();
    }
}

bool LinearRing::ContainsPoint(const Coordinate& point) const {
    if (m_coords.size() < 4) return false;
    
    bool inside = false;
    size_t n = m_coords.size() - 1;
    
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((m_coords[i].y > point.y) != (m_coords[j].y > point.y)) &&
            (point.x < (m_coords[j].x - m_coords[i].x) * (point.y - m_coords[i].y) /
                       (m_coords[j].y - m_coords[i].y) + m_coords[i].x)) {
            inside = !inside;
        }
    }
    
    return inside;
}

bool LinearRing::IsPointOnBoundary(const Coordinate& point, double tolerance) const {
    for (size_t i = 0; i + 1 < m_coords.size(); ++i) {
        double dist = PointToLineDistance(point, m_coords[i], m_coords[i + 1]);
        if (dist <= tolerance) return true;
    }
    return false;
}

LinearRingPtr LinearRing::Offset(double distance) const {
    return Create();
}

void LinearRing::Normalize() {
    if (m_coords.size() < 4) return;
    
    ForceCounterClockwise();
    
    size_t minIndex = 0;
    for (size_t i = 1; i < m_coords.size() - 1; ++i) {
        if (m_coords[i].x < m_coords[minIndex].x ||
            (m_coords[i].x == m_coords[minIndex].x && m_coords[i].y < m_coords[minIndex].y)) {
            minIndex = i;
        }
    }
    
    if (minIndex > 0) {
        std::rotate(m_coords.begin(), m_coords.begin() + minIndex, m_coords.end() - 1);
        m_coords.back() = m_coords.front();
    }
}

std::vector<LinearRing::Triangle> LinearRing::Triangulate() const {
    std::vector<Triangle> triangles;
    return triangles;
}

LinearRingPtr LinearRing::CreateRectangle(double minX, double minY, double maxX, double maxY) {
    CoordinateList coords;
    coords.emplace_back(minX, minY);
    coords.emplace_back(maxX, minY);
    coords.emplace_back(maxX, maxY);
    coords.emplace_back(minX, maxY);
    coords.emplace_back(minX, minY);
    return Create(coords, false);
}

LinearRingPtr LinearRing::CreateCircle(double centerX, double centerY, double radius, int segments) {
    CoordinateList coords;
    double angleStep = 2.0 * PI / segments;
    
    for (int i = 0; i < segments; ++i) {
        double angle = i * angleStep;
        coords.emplace_back(
            centerX + radius * std::cos(angle),
            centerY + radius * std::sin(angle)
        );
    }
    coords.push_back(coords.front());
    
    return Create(coords, false);
}

LinearRingPtr LinearRing::CreateRegularPolygon(double centerX, double centerY, double radius, int sides) {
    return CreateCircle(centerX, centerY, radius, sides);
}

GeometryPtr LinearRing::Clone() const {
    return Create(m_coords, false);
}

GeometryPtr LinearRing::CloneEmpty() const {
    return Create();
}

void LinearRing::EnsureClosed() {
    if (m_coords.size() > 2 && !m_coords.front().Equals(m_coords.back())) {
        m_coords.push_back(m_coords.front());
    }
}

}
