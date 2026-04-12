#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

Simple2DEngine::Simple2DEngine(RasterImageDevice* device)
    : m_device(device)
    , m_transform(TransformMatrix::Identity())
    , m_opacity(1.0)
    , m_tolerance(0.0)
    , m_antialiasing(true)
    , m_active(false) {
}

Simple2DEngine::~Simple2DEngine() {
    if (m_active) {
        End();
    }
}

DrawResult Simple2DEngine::Begin() {
    if (!m_device || !m_device->IsValid()) {
        return DrawResult::kDeviceError;
    }
    m_active = true;
    return DrawResult::kSuccess;
}

void Simple2DEngine::End() {
    m_active = false;
}

Point Simple2DEngine::TransformPoint(double x, double y) const {
    double tx, ty;
    m_transform.TransformPoint(x, y, tx, ty);
    return Point(tx, ty);
}

bool Simple2DEngine::IsPointVisible(int x, int y) const {
    if (m_clipRegion.IsEmpty()) {
        return x >= 0 && x < m_device->GetWidth() && 
               y >= 0 && y < m_device->GetHeight();
    }
    return m_clipRegion.Contains(Point(x, y));
}

void Simple2DEngine::BlendPixel(int x, int y, const Color& color) {
    if (!IsPointVisible(x, y)) {
        return;
    }
    
    Color existing = m_device->GetPixel(x, y);
    double alpha = (color.GetAlpha() / 255.0) * m_opacity;
    
    if (alpha >= 1.0) {
        m_device->SetPixel(x, y, color);
    } else if (alpha > 0.0) {
        uint8_t r = static_cast<uint8_t>(existing.GetRed() * (1.0 - alpha) + color.GetRed() * alpha);
        uint8_t g = static_cast<uint8_t>(existing.GetGreen() * (1.0 - alpha) + color.GetGreen() * alpha);
        uint8_t b = static_cast<uint8_t>(existing.GetBlue() * (1.0 - alpha) + color.GetBlue() * alpha);
        uint8_t a = static_cast<uint8_t>(std::min(255.0, static_cast<double>(existing.GetAlpha()) + color.GetAlpha() * alpha));
        m_device->SetPixel(x, y, Color(r, g, b, a));
    }
}

void Simple2DEngine::DrawLineBresenham(int x1, int y1, int x2, int y2, const Color& color) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        BlendPixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Simple2DEngine::DrawHorizontalLine(int x1, int x2, int y, const Color& color) {
    if (x1 > x2) std::swap(x1, x2);
    for (int x = x1; x <= x2; ++x) {
        BlendPixel(x, y, color);
    }
}

void Simple2DEngine::FillScanline(int y, int x1, int x2, const Color& color) {
    DrawHorizontalLine(x1, x2, y, color);
}

void Simple2DEngine::FillCircle(int cx, int cy, int radius, const Color& color) {
    for (int y = -radius; y <= radius; ++y) {
        int width = static_cast<int>(std::sqrt(radius * radius - y * y));
        DrawHorizontalLine(cx - width, cx + width, cy + y, color);
    }
}

void Simple2DEngine::StrokeCircle(int cx, int cy, int radius, const Color& color, int width) {
    for (int angle = 0; angle < 360; ++angle) {
        double rad = angle * 3.14159265358979323846 / 180.0;
        int x = static_cast<int>(cx + radius * std::cos(rad));
        int y = static_cast<int>(cy + radius * std::sin(rad));
        BlendPixel(x, y, color);
    }
}

DrawResult Simple2DEngine::DrawPoints(const double* x, const double* y, int count,
                                       const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count <= 0) return DrawResult::kInvalidParameter;
    
    Color color = style.pen.color;
    int size = static_cast<int>(std::max(1.0, style.pen.width));
    
    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);
        int px = static_cast<int>(pt.x);
        int py = static_cast<int>(pt.y);
        
        if (size <= 1) {
            BlendPixel(px, py, color);
        } else {
            FillCircle(px, py, size / 2, color);
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawLines(const double* x1, const double* y1,
                                      const double* x2, const double* y2, int count,
                                      const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) return DrawResult::kInvalidParameter;
    
    Color color = style.pen.color;
    
    for (int i = 0; i < count; ++i) {
        Point p1 = TransformPoint(x1[i], y1[i]);
        Point p2 = TransformPoint(x2[i], y2[i]);
        
        DrawLineBresenham(static_cast<int>(p1.x), static_cast<int>(p1.y),
                          static_cast<int>(p2.x), static_cast<int>(p2.y), color);
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawLineString(const double* x, const double* y, int count,
                                           const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 2) return DrawResult::kInvalidParameter;
    
    Color color = style.pen.color;
    
    for (int i = 0; i < count - 1; ++i) {
        Point p1 = TransformPoint(x[i], y[i]);
        Point p2 = TransformPoint(x[i + 1], y[i + 1]);
        
        DrawLineBresenham(static_cast<int>(p1.x), static_cast<int>(p1.y),
                          static_cast<int>(p2.x), static_cast<int>(p2.y), color);
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawPolygon(const double* x, const double* y, int count,
                                        const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!x || !y || count < 3) return DrawResult::kInvalidParameter;
    
    std::vector<Point> points;
    points.reserve(count);
    
    int minY = INT_MAX, maxY = INT_MIN;
    for (int i = 0; i < count; ++i) {
        Point pt = TransformPoint(x[i], y[i]);
        points.push_back(pt);
        minY = std::min(minY, static_cast<int>(pt.y));
        maxY = std::max(maxY, static_cast<int>(pt.y));
    }
    
    if (fill && style.HasFill()) {
        Color fillColor = style.brush.color;
        
        for (int y = minY; y <= maxY; ++y) {
            std::vector<int> intersections;
            
            for (size_t i = 0; i < points.size(); ++i) {
                size_t j = (i + 1) % points.size();
                int y1 = static_cast<int>(points[i].y);
                int y2 = static_cast<int>(points[j].y);
                
                if ((y1 <= y && y < y2) || (y2 <= y && y < y1)) {
                    int x1 = static_cast<int>(points[i].x);
                    int x2 = static_cast<int>(points[j].x);
                    double t = (y - y1) / static_cast<double>(y2 - y1);
                    int xIntersect = static_cast<int>(x1 + t * (x2 - x1));
                    intersections.push_back(xIntersect);
                }
            }
            
            std::sort(intersections.begin(), intersections.end());
            
            for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
                FillScanline(y, intersections[i], intersections[i + 1], fillColor);
            }
        }
    }
    
    if (style.HasStroke()) {
        Color strokeColor = style.pen.color;
        for (size_t i = 0; i < points.size(); ++i) {
            size_t j = (i + 1) % points.size();
            DrawLineBresenham(static_cast<int>(points[i].x), static_cast<int>(points[i].y),
                              static_cast<int>(points[j].x), static_cast<int>(points[j].y), strokeColor);
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawRect(double x, double y, double w, double h,
                                     const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    
    double coords[] = { x, x + w, x + w, x };
    double ys[] = { y, y, y + h, y + h };
    
    return DrawPolygon(coords, ys, 4, style, fill);
}

DrawResult Simple2DEngine::DrawCircle(double cx, double cy, double radius,
                                       const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (radius <= 0) return DrawResult::kInvalidParameter;
    
    Point center = TransformPoint(cx, cy);
    
    double scaleX = std::sqrt(m_transform.m[0][0] * m_transform.m[0][0] + 
                              m_transform.m[0][1] * m_transform.m[0][1]);
    int r = static_cast<int>(radius * scaleX);
    if (r < 1) r = 1;
    
    Color color = fill ? style.brush.color : style.pen.color;
    
    if (fill) {
        FillCircle(static_cast<int>(center.x), static_cast<int>(center.y), r, color);
    } else {
        StrokeCircle(static_cast<int>(center.x), static_cast<int>(center.y), r, color,
                     static_cast<int>(style.pen.width));
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawEllipse(double cx, double cy, double rx, double ry,
                                        const DrawStyle& style, bool fill) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;
    
    Point center = TransformPoint(cx, cy);
    Color color = fill ? style.brush.color : style.pen.color;
    
    const int steps = 360;
    double prevX = center.x + rx;
    double prevY = center.y;
    
    for (int i = 1; i <= steps; ++i) {
        double angle = i * 2.0 * 3.14159265358979323846 / steps;
        double px = center.x + rx * std::cos(angle);
        double py = center.y + ry * std::sin(angle);
        
        DrawLineBresenham(static_cast<int>(prevX), static_cast<int>(prevY),
                          static_cast<int>(px), static_cast<int>(py), color);
        
        prevX = px;
        prevY = py;
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawArc(double cx, double cy, double rx, double ry,
                                    double startAngle, double sweepAngle,
                                    const DrawStyle& style) {
    if (!m_active) return DrawResult::kInvalidState;
    if (rx <= 0 || ry <= 0) return DrawResult::kInvalidParameter;
    
    Point center = TransformPoint(cx, cy);
    Color color = style.pen.color;
    
    const double pi = 3.14159265358979323846;
    double endAngle = startAngle + sweepAngle;
    int steps = static_cast<int>(std::abs(sweepAngle) * 180.0 / pi);
    steps = std::max(10, steps);
    
    double prevX = center.x + rx * std::cos(startAngle);
    double prevY = center.y + ry * std::sin(startAngle);
    
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        double angle = startAngle + t * sweepAngle;
        double px = center.x + rx * std::cos(angle);
        double py = center.y + ry * std::sin(angle);
        
        DrawLineBresenham(static_cast<int>(prevX), static_cast<int>(prevY),
                          static_cast<int>(px), static_cast<int>(py), color);
        
        prevX = px;
        prevY = py;
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawText(double x, double y, const std::string& text,
                                     const Font& font, const Color& color) {
    if (!m_active) return DrawResult::kInvalidState;
    (void)font;
    (void)text;
    (void)color;
    return DrawResult::kNotImplemented;
}

DrawResult Simple2DEngine::DrawImage(double x, double y, const Image& image,
                                      double scaleX, double scaleY) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) return DrawResult::kInvalidParameter;
    
    Point pt = TransformPoint(x, y);
    int startX = static_cast<int>(pt.x);
    int startY = static_cast<int>(pt.y);
    
    int imgWidth = image.GetWidth();
    int imgHeight = image.GetHeight();
    int imgChannels = image.GetChannels();
    const uint8_t* imgData = image.GetData();
    
    for (int iy = 0; iy < imgHeight; ++iy) {
        for (int ix = 0; ix < imgWidth; ++ix) {
            size_t idx = (iy * imgWidth + ix) * imgChannels;
            
            uint8_t r = imgData[idx];
            uint8_t g = imgChannels > 1 ? imgData[idx + 1] : r;
            uint8_t b = imgChannels > 2 ? imgData[idx + 2] : r;
            uint8_t a = imgChannels > 3 ? imgData[idx + 3] : 255;
            
            int destX = static_cast<int>(startX + ix * scaleX);
            int destY = static_cast<int>(startY + iy * scaleY);
            
            BlendPixel(destX, destY, Color(r, g, b, a));
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult Simple2DEngine::DrawImageRect(double x, double y, double w, double h,
                                          const Image& image) {
    if (!m_active) return DrawResult::kInvalidState;
    if (!image.IsValid()) return DrawResult::kInvalidParameter;
    
    double scaleX = w / image.GetWidth();
    double scaleY = h / image.GetHeight();
    
    return DrawImage(x, y, image, scaleX, scaleY);
}

DrawResult Simple2DEngine::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
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
                const Rect& r = rect->GetRect();
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
                return DrawEllipse(center.x, center.y, 
                                   ellipse->GetRadiusX(), ellipse->GetRadiusY(), style, true);
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

void Simple2DEngine::SetTransform(const TransformMatrix& matrix) {
    m_transform = matrix;
}

void Simple2DEngine::ResetTransform() {
    m_transform = TransformMatrix::Identity();
}

void Simple2DEngine::SetClipRect(double x, double y, double w, double h) {
    m_clipRegion = Region(Rect(x, y, w, h));
}

void Simple2DEngine::SetClipRegion(const Region& region) {
    m_clipRegion = region;
}

void Simple2DEngine::ResetClip() {
    m_clipRegion.Clear();
}

void Simple2DEngine::Save(StateFlags flags) {
    EngineStateEntry state;
    state.transform = m_transform;
    state.clipRegion = m_clipRegion;
    state.opacity = m_opacity;
    state.tolerance = m_tolerance;
    state.antialiasing = m_antialiasing;
    state.flags = flags;
    m_stateStack.push(state);
}

void Simple2DEngine::Restore() {
    if (m_stateStack.empty()) {
        return;
    }
    
    EngineStateEntry state = m_stateStack.top();
    m_stateStack.pop();
    
    if (state.flags & static_cast<StateFlags>(StateFlag::kTransform)) {
        m_transform = state.transform;
    }
    if (state.flags & static_cast<StateFlags>(StateFlag::kClip)) {
        m_clipRegion = state.clipRegion;
    }
    if (state.flags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        m_opacity = state.opacity;
    }
    m_tolerance = state.tolerance;
    m_antialiasing = state.antialiasing;
}

void Simple2DEngine::SetOpacity(double opacity) {
    m_opacity = std::max(0.0, std::min(1.0, opacity));
}

TextMetrics Simple2DEngine::MeasureText(const std::string& text, const Font& font) {
    (void)text;
    TextMetrics metrics;
    double fontSize = font.GetSize();
    metrics.width = text.length() * fontSize * 0.6;
    metrics.height = fontSize;
    metrics.ascent = fontSize * 0.8;
    metrics.descent = fontSize * 0.2;
    metrics.lineHeight = fontSize * 1.2;
    return metrics;
}

void Simple2DEngine::Clear(const Color& color) {
    if (m_device) {
        m_device->Clear(color);
    }
}

}  
}  
