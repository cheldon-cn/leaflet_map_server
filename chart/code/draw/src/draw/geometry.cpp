#include "ogc/draw/geometry.h"
#include <ogc/geom/visitor.h>
#include <ogc/geom/linearring.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace draw {

std::unique_ptr<RectGeometry> RectGeometry::Create() {
    return std::unique_ptr<RectGeometry>(new RectGeometry());
}

std::unique_ptr<RectGeometry> RectGeometry::Create(const Rect& rect) {
    return std::unique_ptr<RectGeometry>(new RectGeometry(rect));
}

std::unique_ptr<RectGeometry> RectGeometry::Create(double x, double y, double w, double h) {
    return std::unique_ptr<RectGeometry>(new RectGeometry(x, y, w, h));
}

RectGeometry::RectGeometry() : m_rect() {
}

RectGeometry::RectGeometry(const Rect& rect) : m_rect(rect) {
    UpdatePolygon();
}

RectGeometry::RectGeometry(double x, double y, double w, double h) 
    : m_rect(x, y, w, h) {
    UpdatePolygon();
}

void RectGeometry::SetRect(const Rect& rect) {
    m_rect = rect;
    UpdatePolygon();
}

void RectGeometry::UpdatePolygon() {
    if (m_rect.IsEmpty()) {
        return;
    }
    
    auto ring = LinearRing::CreateRectangle(
        m_rect.x, m_rect.y,
        m_rect.x + m_rect.w, m_rect.y + m_rect.h);
    SetExteriorRing(std::move(ring));
}

void RectGeometry::TransformByMatrix(const TransformMatrix& matrix) {
    m_rect = matrix.TransformRect(m_rect);
    UpdatePolygon();
}

GeometryPtr RectGeometry::Clone() const {
    return GeometryPtr(new RectGeometry(m_rect));
}

std::unique_ptr<CircleGeometry> CircleGeometry::Create() {
    return std::unique_ptr<CircleGeometry>(new CircleGeometry());
}

std::unique_ptr<CircleGeometry> CircleGeometry::Create(const Point& center, double radius, int segments) {
    return std::unique_ptr<CircleGeometry>(new CircleGeometry(center, radius, segments));
}

std::unique_ptr<CircleGeometry> CircleGeometry::Create(double cx, double cy, double radius, int segments) {
    return std::unique_ptr<CircleGeometry>(new CircleGeometry(cx, cy, radius, segments));
}

CircleGeometry::CircleGeometry() : m_center(), m_radius(0), m_segments(32) {
}

CircleGeometry::CircleGeometry(const Point& center, double radius, int segments)
    : m_center(center), m_radius(radius), m_segments(segments) {
    UpdatePolygon();
}

CircleGeometry::CircleGeometry(double cx, double cy, double radius, int segments)
    : m_center(cx, cy), m_radius(radius), m_segments(segments) {
    UpdatePolygon();
}

void CircleGeometry::SetCenter(const Point& center) {
    m_center = center;
    UpdatePolygon();
}

void CircleGeometry::SetRadius(double radius) {
    m_radius = radius;
    UpdatePolygon();
}

void CircleGeometry::SetSegments(int segments) {
    m_segments = segments;
    UpdatePolygon();
}

void CircleGeometry::UpdatePolygon() {
    if (m_radius <= 0) {
        return;
    }
    
    auto ring = LinearRing::CreateCircle(m_center.x, m_center.y, m_radius, m_segments);
    SetExteriorRing(std::move(ring));
}

void CircleGeometry::TransformByMatrix(const TransformMatrix& matrix) {
    m_center = matrix.TransformPoint(m_center);
    double scaleX = matrix.GetScaleX();
    double scaleY = matrix.GetScaleY();
    m_radius *= std::max(scaleX, scaleY);
    UpdatePolygon();
}

GeometryPtr CircleGeometry::Clone() const {
    return GeometryPtr(new CircleGeometry(m_center, m_radius, m_segments));
}

std::unique_ptr<EllipseGeometry> EllipseGeometry::Create() {
    return std::unique_ptr<EllipseGeometry>(new EllipseGeometry());
}

std::unique_ptr<EllipseGeometry> EllipseGeometry::Create(const Point& center, double rx, double ry, int segments) {
    return std::unique_ptr<EllipseGeometry>(new EllipseGeometry(center, rx, ry, segments));
}

std::unique_ptr<EllipseGeometry> EllipseGeometry::Create(double cx, double cy, double rx, double ry, int segments) {
    return std::unique_ptr<EllipseGeometry>(new EllipseGeometry(cx, cy, rx, ry, segments));
}

EllipseGeometry::EllipseGeometry() : m_center(), m_rx(0), m_ry(0), m_segments(32) {
}

EllipseGeometry::EllipseGeometry(const Point& center, double rx, double ry, int segments)
    : m_center(center), m_rx(rx), m_ry(ry), m_segments(segments) {
    UpdatePolygon();
}

EllipseGeometry::EllipseGeometry(double cx, double cy, double rx, double ry, int segments)
    : m_center(cx, cy), m_rx(rx), m_ry(ry), m_segments(segments) {
    UpdatePolygon();
}

void EllipseGeometry::SetCenter(const Point& center) {
    m_center = center;
    UpdatePolygon();
}

void EllipseGeometry::SetRadiusX(double rx) {
    m_rx = rx;
    UpdatePolygon();
}

void EllipseGeometry::SetRadiusY(double ry) {
    m_ry = ry;
    UpdatePolygon();
}

void EllipseGeometry::SetSegments(int segments) {
    m_segments = segments;
    UpdatePolygon();
}

void EllipseGeometry::UpdatePolygon() {
    if (m_rx <= 0 || m_ry <= 0) {
        return;
    }
    
    CoordinateList coords;
    double angleStep = 2.0 * M_PI / m_segments;
    
    for (int i = 0; i <= m_segments; ++i) {
        double angle = i * angleStep;
        double x = m_center.x + m_rx * std::cos(angle);
        double y = m_center.y + m_ry * std::sin(angle);
        coords.push_back(Coordinate(x, y));
    }
    
    auto ring = LinearRing::Create(coords);
    SetExteriorRing(std::move(ring));
}

void EllipseGeometry::TransformByMatrix(const TransformMatrix& matrix) {
    m_center = matrix.TransformPoint(m_center);
    double scaleX = matrix.GetScaleX();
    double scaleY = matrix.GetScaleY();
    m_rx *= scaleX;
    m_ry *= scaleY;
    UpdatePolygon();
}

GeometryPtr EllipseGeometry::Clone() const {
    return GeometryPtr(new EllipseGeometry(m_center, m_rx, m_ry, m_segments));
}

}  
}  
