#include "ogc/draw/vector_engine.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/geometry.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

VectorEngine::VectorEngine(DrawDevice* device)
    : m_device(device)
    , m_opacity(1.0)
    , m_tolerance(0.1)
    , m_antialiasing(true)
    , m_active(false)
{
}

VectorEngine::~VectorEngine()
{
    if (m_active) {
        End();
    }
}

DrawResult VectorEngine::Begin()
{
    if (!m_device) {
        return DrawResult::kDeviceError;
    }

    m_active = true;
    m_transform = TransformMatrix::Identity();
    m_clipRegion = Region();
    m_opacity = 1.0;
    m_tolerance = 0.1;
    m_antialiasing = true;

    while (!m_stateStack.empty()) {
        m_stateStack.pop();
    }
    m_currentPath.clear();

    return DrawResult::kSuccess;
}

void VectorEngine::End()
{
    m_active = false;
    Flush();
}

DrawResult VectorEngine::DrawPoints(const double* x, const double* y, int count,
                                   const DrawStyle& style)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count <= 0) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);

        std::vector<Point> pointPath;
        double r = style.pen.width / 2.0;
        
        const int segments = 16;
        for (int j = 0; j < segments; ++j) {
            double angle = 2.0 * 3.14159265358979323846 * j / segments;
            double px = pt.x + r * std::cos(angle);
            double py = pt.y + r * std::sin(angle);
            pointPath.push_back(Point(px, py));
        }
        pointPath.push_back(pointPath.front());

        WritePath(pointPath, true);
        WriteFill(style.pen.color, FillRule::kNonZero);
    }

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawLines(const double* x1, const double* y1,
                                  const double* x2, const double* y2, int count,
                                  const DrawStyle& style)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    for (int i = 0; i < count; ++i) {
        std::vector<Point> linePath;
        linePath.push_back(TransformPoint(x1[i], y1[i]));
        linePath.push_back(TransformPoint(x2[i], y2[i]));

        WritePath(linePath, false);
        WriteStroke(style.pen);
    }

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawLineString(const double* x, const double* y, int count,
                                       const DrawStyle& style)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 2) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    std::vector<Point> linePath = TransformPoints(x, y, count);
    WritePath(linePath, false);
    WriteStroke(style.pen);

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawPolygon(const double* x, const double* y, int count,
                                    const DrawStyle& style, bool fill)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 3) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    std::vector<Point> polyPath = TransformPoints(x, y, count);
    WritePath(polyPath, true);

    if (fill && style.brush.color.GetAlpha() > 0) {
        WriteFill(style.brush.color, FillRule::kNonZero);
    }
    if (style.pen.width > 0 && style.pen.color.GetAlpha() > 0) {
        WriteStroke(style.pen);
    }

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawRect(double x, double y, double w, double h,
                                 const DrawStyle& style, bool fill)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (w <= 0 || h <= 0) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    double rx[] = { x, x + w, x + w, x };
    double ry[] = { y, y, y + h, y + h };

    std::vector<Point> rectPath = TransformPoints(rx, ry, 4);
    WritePath(rectPath, true);

    if (fill && style.brush.color.GetAlpha() > 0) {
        WriteFill(style.brush.color, FillRule::kNonZero);
    }
    if (style.pen.width > 0 && style.pen.color.GetAlpha() > 0) {
        WriteStroke(style.pen);
    }

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawCircle(double cx, double cy, double radius,
                                   const DrawStyle& style, bool fill)
{
    return DrawEllipse(cx, cy, radius, radius, style, fill);
}

DrawResult VectorEngine::DrawEllipse(double cx, double cy, double rx, double ry,
                                    const DrawStyle& style, bool fill)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    Point center = TransformPoint(cx, cy);

    std::vector<Point> ellipsePath;
    const int segments = 64;
    for (int i = 0; i < segments; ++i) {
        double angle = 2.0 * 3.14159265358979323846 * i / segments;
        double x = center.x + rx * std::cos(angle);
        double y = center.y + ry * std::sin(angle);
        ellipsePath.push_back(Point(x, y));
    }
    ellipsePath.push_back(ellipsePath.front());

    WritePath(ellipsePath, true);

    if (fill && style.brush.color.GetAlpha() > 0) {
        WriteFill(style.brush.color, FillRule::kNonZero);
    }
    if (style.pen.width > 0 && style.pen.color.GetAlpha() > 0) {
        WriteStroke(style.pen);
    }

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawArc(double cx, double cy, double rx, double ry,
                                double startAngle, double sweepAngle,
                                const DrawStyle& style)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;

    m_currentStyle = style;

    Point center = TransformPoint(cx, cy);

    std::vector<Point> arcPath;
    const double pi = 3.14159265358979323846;
    int segments = static_cast<int>(std::abs(sweepAngle) / (pi / 32)) + 1;
    segments = std::max(4, std::min(128, segments));

    for (int i = 0; i <= segments; ++i) {
        double angle = startAngle + sweepAngle * i / segments;
        double x = center.x + rx * std::cos(angle);
        double y = center.y + ry * std::sin(angle);
        arcPath.push_back(Point(x, y));
    }

    WritePath(arcPath, false);
    WriteStroke(style.pen);

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawText(double x, double y, const std::string& text,
                                 const Font& font, const Color& color)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (text.empty()) return DrawResult::kInvalidParameter;

    Point pt = TransformPoint(x, y);
    WriteText(pt.x, pt.y, text, font, color);

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawImage(double x, double y, const Image& image,
                                  double scaleX, double scaleY)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) {
        return DrawResult::kInvalidParameter;
    }

    Point pt = TransformPoint(x, y);
    WriteImage(pt.x, pt.y, image, scaleX, scaleY);

    return DrawResult::kSuccess;
}

DrawResult VectorEngine::DrawImageRect(double x, double y, double w, double h,
                                      const Image& image)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) {
        return DrawResult::kInvalidParameter;
    }
    if (w <= 0 || h <= 0) {
        return DrawResult::kInvalidParameter;
    }

    double scaleX = w / image.GetWidth();
    double scaleY = h / image.GetHeight();

    return DrawImage(x, y, image, scaleX, scaleY);
}

DrawResult VectorEngine::DrawGeometry(const Geometry* geometry, const DrawStyle& style)
{
    if (!m_active) return DrawResult::kInvalidState;
    if (!geometry) return DrawResult::kInvalidParameter;

    GeomType type = geometry->GetGeometryType();
    
    switch (type) {
    case GeomType::kPoint: {
        const ogc::Point* pt = dynamic_cast<const ogc::Point*>(geometry);
        if (pt) {
            double x = pt->GetX();
            double y = pt->GetY();
            return DrawPoints(&x, &y, 1, style);
        }
        break;
    }
    case GeomType::kLineString: {
        const ogc::LineString* ls = dynamic_cast<const ogc::LineString*>(geometry);
        if (ls) {
            CoordinateList coords = ls->GetCoordinates();
            std::vector<double> x(coords.size()), y(coords.size());
            for (size_t i = 0; i < coords.size(); ++i) {
                x[i] = coords[i].x;
                y[i] = coords[i].y;
            }
            return DrawLineString(x.data(), y.data(), static_cast<int>(coords.size()), style);
        }
        break;
    }
    case GeomType::kPolygon: {
        const RectGeometry* rect = dynamic_cast<const RectGeometry*>(geometry);
        if (rect) {
            Rect r = rect->GetRect();
            return DrawRect(r.x, r.y, r.w, r.h, style, true);
        }
        
        const CircleGeometry* circle = dynamic_cast<const CircleGeometry*>(geometry);
        if (circle) {
            Point center = circle->GetCenter();
            return DrawCircle(center.x, center.y, circle->GetRadius(), style, true);
        }
        
        const EllipseGeometry* ellipse = dynamic_cast<const EllipseGeometry*>(geometry);
        if (ellipse) {
            Point center = ellipse->GetCenter();
            return DrawEllipse(center.x, center.y, ellipse->GetRadiusX(), ellipse->GetRadiusY(), style, true);
        }
        
        const ogc::Polygon* poly = dynamic_cast<const ogc::Polygon*>(geometry);
        if (poly) {
            const LinearRing* ring = poly->GetExteriorRing();
            if (ring) {
                CoordinateList coords = ring->GetCoordinates();
                std::vector<double> x(coords.size()), y(coords.size());
                for (size_t i = 0; i < coords.size(); ++i) {
                    x[i] = coords[i].x;
                    y[i] = coords[i].y;
                }
                return DrawPolygon(x.data(), y.data(), static_cast<int>(coords.size()), style, true);
            }
        }
        break;
    }
    default:
        return DrawResult::kNotImplemented;
    }
    
    return DrawResult::kInvalidParameter;
}

void VectorEngine::SetTransform(const TransformMatrix& matrix)
{
    m_transform = matrix;
    DoSetTransform(matrix);
}

void VectorEngine::ResetTransform()
{
    m_transform = TransformMatrix::Identity();
    DoSetTransform(m_transform);
}

void VectorEngine::SetClipRect(double x, double y, double w, double h)
{
    m_clipRegion = Region(Rect(x, y, w, h));
    DoSetClip(m_clipRegion);
}

void VectorEngine::SetClipRegion(const Region& region)
{
    m_clipRegion = region;
    DoSetClip(m_clipRegion);
}

void VectorEngine::ResetClip()
{
    m_clipRegion = Region();
    DoSetClip(m_clipRegion);
}

void VectorEngine::Save(StateFlags flags)
{
    VectorStateEntry entry;
    entry.transform = m_transform;
    entry.clipRegion = m_clipRegion;
    entry.opacity = m_opacity;
    entry.pen = m_currentStyle.pen;
    entry.brush = m_currentStyle.brush;
    entry.font = m_currentStyle.font;

    m_stateStack.push(entry);
    DoSave();
}

void VectorEngine::Restore()
{
    if (m_stateStack.empty()) {
        return;
    }

    VectorStateEntry entry = m_stateStack.top();
    m_stateStack.pop();

    m_transform = entry.transform;
    m_clipRegion = entry.clipRegion;
    m_opacity = entry.opacity;
    m_currentStyle.pen = entry.pen;
    m_currentStyle.brush = entry.brush;
    m_currentStyle.font = entry.font;

    DoRestore();
}

void VectorEngine::SetOpacity(double opacity)
{
    m_opacity = std::max(0.0, std::min(1.0, opacity));
}

TextMetrics VectorEngine::MeasureText(const std::string& text, const Font& font)
{
    TextMetrics metrics;
    double fontSize = font.GetSize();
    metrics.width = static_cast<double>(text.length()) * fontSize * 0.6;
    metrics.height = fontSize;
    metrics.ascent = fontSize * 0.8;
    metrics.descent = fontSize * 0.2;
    return metrics;
}

void VectorEngine::Clear(const Color& color)
{
}

void VectorEngine::Flush()
{
}

void VectorEngine::BuildPath(const double* x, const double* y, int count)
{
    m_currentPath.clear();
    for (int i = 0; i < count; ++i) {
        m_currentPath.push_back(TransformPoint(x[i], y[i]));
    }
}

void VectorEngine::ApplyTransform()
{
    DoSetTransform(m_transform);
}

Point VectorEngine::TransformPoint(double x, double y) const
{
    return m_transform.TransformPoint(Point(x, y));
}

std::vector<Point> VectorEngine::TransformPoints(const double* x, const double* y, int count) const
{
    std::vector<Point> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        result.push_back(TransformPoint(x[i], y[i]));
    }
    return result;
}

} // namespace draw
} // namespace ogc
