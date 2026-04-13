#include "ogc/geom/linearring.h"
#include "geometry_impl_internal.h"
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
    if (m_coords.size() < 4) {
        return Create();
    }
    
    CoordinateList offsetCoords;
    size_t n = m_coords.size() - 1;
    
    for (size_t i = 0; i < n; ++i) {
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        
        double dx1 = m_coords[i].x - m_coords[prev].x;
        double dy1 = m_coords[i].y - m_coords[prev].y;
        double len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
        
        double dx2 = m_coords[next].x - m_coords[i].x;
        double dy2 = m_coords[next].y - m_coords[i].y;
        double len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
        
        if (len1 < 1e-10 || len2 < 1e-10) {
            offsetCoords.push_back(m_coords[i]);
            continue;
        }
        
        double nx1 = -dy1 / len1;
        double ny1 = dx1 / len1;
        
        double nx2 = -dy2 / len2;
        double ny2 = dx2 / len2;
        
        double bisectX = nx1 + nx2;
        double bisectY = ny1 + ny2;
        double bisectLen = std::sqrt(bisectX * bisectX + bisectY * bisectY);
        
        if (bisectLen < 1e-10) {
            offsetCoords.emplace_back(
                m_coords[i].x + distance * nx1,
                m_coords[i].y + distance * ny1
            );
        } else {
            double dot = nx1 * nx2 + ny1 * ny2;
            double scale = distance / (1.0 + dot);
            
            offsetCoords.emplace_back(
                m_coords[i].x + scale * bisectX,
                m_coords[i].y + scale * bisectY
            );
        }
    }
    
    if (!offsetCoords.empty()) {
        offsetCoords.push_back(offsetCoords.front());
    }
    
    return Create(offsetCoords, false);
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
    
    if (m_coords.size() < 4) {
        return triangles;
    }
    
    std::vector<Coordinate> polygon;
    for (size_t i = 0; i < m_coords.size() - 1; ++i) {
        polygon.push_back(m_coords[i]);
    }
    
    auto crossProduct = [](const Coordinate& o, const Coordinate& a, const Coordinate& b) -> double {
        return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
    };
    
    auto isConvex = [&](size_t i, size_t n) -> bool {
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        double cross = crossProduct(polygon[prev], polygon[i], polygon[next]);
        return cross > 0;
    };
    
    auto isInsideTriangle = [](const Coordinate& p, const Coordinate& a, 
                               const Coordinate& b, const Coordinate& c) -> bool {
        double d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
        double d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
        double d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);
        
        bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
        
        return !(hasNeg && hasPos);
    };
    
    auto isEar = [&](size_t i, size_t n) -> bool {
        if (!isConvex(i, n)) {
            return false;
        }
        
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        
        for (size_t j = 0; j < n; ++j) {
            if (j == prev || j == i || j == next) {
                continue;
            }
            
            if (isInsideTriangle(polygon[j], polygon[prev], polygon[i], polygon[next])) {
                return false;
            }
        }
        
        return true;
    };
    
    while (polygon.size() > 3) {
        bool earFound = false;
        
        for (size_t i = 0; i < polygon.size(); ++i) {
            if (isEar(i, polygon.size())) {
                size_t prev = (i + polygon.size() - 1) % polygon.size();
                size_t next = (i + 1) % polygon.size();
                
                Triangle t;
                t.p1 = polygon[prev];
                t.p2 = polygon[i];
                t.p3 = polygon[next];
                triangles.push_back(t);
                
                polygon.erase(polygon.begin() + i);
                earFound = true;
                break;
            }
        }
        
        if (!earFound) {
            break;
        }
    }
    
    if (polygon.size() == 3) {
        Triangle t;
        t.p1 = polygon[0];
        t.p2 = polygon[1];
        t.p3 = polygon[2];
        triangles.push_back(t);
    }
    
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
