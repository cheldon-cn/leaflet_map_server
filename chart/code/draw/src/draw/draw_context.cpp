#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include <cassert>
#include <stack>

namespace ogc {
namespace draw {

struct StateEntry {
    TransformMatrix transform;
    DrawStyle style;
    Region clipRegion;
    double opacity;
    StateFlags savedFlags;

    StateEntry()
        : opacity(1.0)
        , savedFlags(static_cast<StateFlags>(StateFlag::kAll)) {}
};

class DrawContextImpl final : public DrawContext {
public:
    explicit DrawContextImpl(DrawDevice* device);
    ~DrawContextImpl() override;

    DrawResult Begin() override;
    void End() override;

    DrawDevice* GetDevice() const override { return m_device; }
    DrawEngine* GetEngine() const override { return m_engine.get(); }

    void Save(StateFlags flags) override;
    void Restore() override;

    DrawResult DrawGeometry(const Geometry* geometry) override;
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;

    DrawResult DrawPoint(double x, double y) override;
    DrawResult DrawPoint(const Point& pt) override;
    DrawResult DrawPoints(const double* x, const double* y, int count) override;
    DrawResult DrawPoints(const Point* points, int count) override;

    DrawResult DrawLine(double x1, double y1, double x2, double y2) override;
    DrawResult DrawLine(const Point& start, const Point& end) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2, int count) override;

    DrawResult DrawLineString(const double* x, const double* y, int count) override;
    DrawResult DrawLineString(const Point* points, int count) override;
    DrawResult DrawLineString(const std::vector<Point>& points) override;

    DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill) override;
    DrawResult DrawPolygon(const Point* points, int count, bool fill) override;
    DrawResult DrawPolygon(const std::vector<Point>& points, bool fill) override;

    DrawResult DrawRect(double x, double y, double w, double h, bool fill) override;
    DrawResult DrawRect(const Rect& rect, bool fill) override;

    DrawResult DrawCircle(double cx, double cy, double radius, bool fill) override;
    DrawResult DrawCircle(const Point& center, double radius, bool fill) override;

    DrawResult DrawEllipse(double cx, double cy, double rx, double ry, bool fill) override;
    DrawResult DrawEllipse(const Point& center, double rx, double ry, bool fill) override;

    DrawResult DrawArc(double cx, double cy, double rx, double ry,
                        double startAngle, double sweepAngle) override;

    DrawResult DrawText(double x, double y, const std::string& text) override;
    DrawResult DrawText(const Point& pt, const std::string& text) override;
    DrawResult DrawText(double x, double y, const std::string& text,
                         const Font& font, const Color& color) override;

    DrawResult DrawImage(double x, double y, const Image& image,
                          double scaleX, double scaleY) override;
    DrawResult DrawImage(const Point& pt, const Image& image,
                          double scaleX, double scaleY) override;
    DrawResult DrawImageRect(double x, double y, double w, double h,
                              const Image& image) override;
    DrawResult DrawImageRect(const Rect& rect, const Image& image) override;

    void SetTransform(const TransformMatrix& matrix) override;
    TransformMatrix GetTransform() const override;
    void ResetTransform() override;

    void Translate(double dx, double dy) override;
    void Rotate(double angleRadians) override;
    void RotateDegrees(double angleDegrees) override;
    void Scale(double sx, double sy) override;
    void Shear(double shx, double shy) override;

    void SetStyle(const DrawStyle& style) override;
    DrawStyle GetStyle() const override;

    void SetPen(const Pen& pen) override;
    Pen GetPen() const override;

    void SetBrush(const Brush& brush) override;
    Brush GetBrush() const override;

    void SetFont(const Font& font) override;
    Font GetFont() const override;

    void SetOpacity(double opacity) override;
    double GetOpacity() const override;

    void SetClipRect(double x, double y, double w, double h) override;
    void SetClipRect(const Rect& rect) override;
    void SetClipRegion(const Region& region) override;
    void ResetClip() override;
    Region GetClipRegion() const override;

    void Clear(const Color& color) override;
    void Flush() override;

    TextMetrics MeasureText(const std::string& text) override;
    TextMetrics MeasureText(const std::string& text, const Font& font) override;

    bool IsActive() const override { return m_active; }

private:
    DrawDevice* m_device;
    std::unique_ptr<DrawEngine> m_engine;
    DrawStyle m_currentStyle;
    TransformMatrix m_currentTransform;
    Region m_currentClipRegion;
    double m_currentOpacity;
    std::stack<StateEntry> m_stateStack;
    bool m_active;
};

std::unique_ptr<DrawContext> DrawContext::Create(DrawDevice* device) {
    if (!device) {
        return nullptr;
    }
    return std::unique_ptr<DrawContext>(new DrawContextImpl(device));
}

DrawContextImpl::DrawContextImpl(DrawDevice* device)
    : m_device(device)
    , m_engine(nullptr)
    , m_currentOpacity(1.0)
    , m_active(false) {
    assert(device != nullptr);
}

DrawContextImpl::~DrawContextImpl() {
    if (m_active) {
        End();
    }
}

DrawResult DrawContextImpl::Begin() {
    if (!m_device || !m_device->IsReady()) {
        return DrawResult::kDeviceNotReady;
    }

    m_engine = m_device->CreateEngine();
    if (!m_engine) {
        return DrawResult::kEngineError;
    }

    DrawResult result = m_engine->Begin();
    if (result != DrawResult::kSuccess) {
        m_engine.reset();
        return result;
    }

    m_currentStyle = DrawStyle::Default();
    m_currentTransform = TransformMatrix::Identity();
    m_currentClipRegion = Region();
    m_currentOpacity = 1.0;
    
    while (!m_stateStack.empty()) {
        m_stateStack.pop();
    }

    m_active = true;
    return DrawResult::kSuccess;
}

void DrawContextImpl::End() {
    if (m_engine) {
        m_engine->End();
        m_engine.reset();
    }
    m_active = false;
}

void DrawContextImpl::Save(StateFlags flags) {
    StateEntry entry;
    entry.savedFlags = flags;
    
    if (flags & static_cast<StateFlags>(StateFlag::kTransform)) {
        entry.transform = m_currentTransform;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kStyle)) {
        entry.style = m_currentStyle;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        entry.opacity = m_currentOpacity;
    }
    if (flags & static_cast<StateFlags>(StateFlag::kClip)) {
        entry.clipRegion = m_currentClipRegion;
    }
    
    m_stateStack.push(entry);
    
    if (m_engine) {
        m_engine->Save(flags);
    }
}

void DrawContextImpl::Restore() {
    if (m_stateStack.empty()) {
        return;
    }

    StateEntry entry = m_stateStack.top();
    m_stateStack.pop();

    if (entry.savedFlags & static_cast<StateFlags>(StateFlag::kTransform)) {
        m_currentTransform = entry.transform;
    }
    if (entry.savedFlags & static_cast<StateFlags>(StateFlag::kStyle)) {
        m_currentStyle = entry.style;
    }
    if (entry.savedFlags & static_cast<StateFlags>(StateFlag::kOpacity)) {
        m_currentOpacity = entry.opacity;
    }
    if (entry.savedFlags & static_cast<StateFlags>(StateFlag::kClip)) {
        m_currentClipRegion = entry.clipRegion;
    }

    if (m_engine) {
        m_engine->Restore();
    }
}

DrawResult DrawContextImpl::DrawGeometry(const Geometry* geometry) {
    return DrawGeometry(geometry, m_currentStyle);
}

DrawResult DrawContextImpl::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!m_engine || !geometry) {
        return DrawResult::kInvalidParameter;
    }
    return m_engine->DrawGeometry(geometry, style);
}

DrawResult DrawContextImpl::DrawPoint(double x, double y) {
    return DrawPoints(&x, &y, 1);
}

DrawResult DrawContextImpl::DrawPoint(const Point& pt) {
    return DrawPoint(pt.x, pt.y);
}

DrawResult DrawContextImpl::DrawPoints(const double* x, const double* y, int count) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawPoints(x, y, count, m_currentStyle);
}

DrawResult DrawContextImpl::DrawPoints(const Point* points, int count) {
    if (!points || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    std::vector<double> x(count);
    std::vector<double> y(count);
    for (int i = 0; i < count; ++i) {
        x[i] = points[i].x;
        y[i] = points[i].y;
    }
    return DrawPoints(x.data(), y.data(), count);
}

DrawResult DrawContextImpl::DrawLine(double x1, double y1, double x2, double y2) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawLines(&x1, &y1, &x2, &y2, 1, m_currentStyle);
}

DrawResult DrawContextImpl::DrawLine(const Point& start, const Point& end) {
    return DrawLine(start.x, start.y, end.x, end.y);
}

DrawResult DrawContextImpl::DrawLines(const double* x1, const double* y1,
                                       const double* x2, const double* y2, int count) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawLines(x1, y1, x2, y2, count, m_currentStyle);
}

DrawResult DrawContextImpl::DrawLineString(const double* x, const double* y, int count) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawLineString(x, y, count, m_currentStyle);
}

DrawResult DrawContextImpl::DrawLineString(const Point* points, int count) {
    if (!points || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    std::vector<double> x(count);
    std::vector<double> y(count);
    for (int i = 0; i < count; ++i) {
        x[i] = points[i].x;
        y[i] = points[i].y;
    }
    return DrawLineString(x.data(), y.data(), count);
}

DrawResult DrawContextImpl::DrawLineString(const std::vector<Point>& points) {
    if (points.empty()) {
        return DrawResult::kInvalidParameter;
    }
    return DrawLineString(points.data(), static_cast<int>(points.size()));
}

DrawResult DrawContextImpl::DrawPolygon(const double* x, const double* y, int count, bool fill) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawPolygon(x, y, count, m_currentStyle, fill);
}

DrawResult DrawContextImpl::DrawPolygon(const Point* points, int count, bool fill) {
    if (!points || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    std::vector<double> x(count);
    std::vector<double> y(count);
    for (int i = 0; i < count; ++i) {
        x[i] = points[i].x;
        y[i] = points[i].y;
    }
    return DrawPolygon(x.data(), y.data(), count, fill);
}

DrawResult DrawContextImpl::DrawPolygon(const std::vector<Point>& points, bool fill) {
    if (points.empty()) {
        return DrawResult::kInvalidParameter;
    }
    return DrawPolygon(points.data(), static_cast<int>(points.size()), fill);
}

DrawResult DrawContextImpl::DrawRect(double x, double y, double w, double h, bool fill) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawRect(x, y, w, h, m_currentStyle, fill);
}

DrawResult DrawContextImpl::DrawRect(const Rect& rect, bool fill) {
    return DrawRect(rect.x, rect.y, rect.w, rect.h, fill);
}

DrawResult DrawContextImpl::DrawCircle(double cx, double cy, double radius, bool fill) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawCircle(cx, cy, radius, m_currentStyle, fill);
}

DrawResult DrawContextImpl::DrawCircle(const Point& center, double radius, bool fill) {
    return DrawCircle(center.x, center.y, radius, fill);
}

DrawResult DrawContextImpl::DrawEllipse(double cx, double cy, double rx, double ry, bool fill) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawEllipse(cx, cy, rx, ry, m_currentStyle, fill);
}

DrawResult DrawContextImpl::DrawEllipse(const Point& center, double rx, double ry, bool fill) {
    return DrawEllipse(center.x, center.y, rx, ry, fill);
}

DrawResult DrawContextImpl::DrawArc(double cx, double cy, double rx, double ry,
                                     double startAngle, double sweepAngle) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawArc(cx, cy, rx, ry, startAngle, sweepAngle, m_currentStyle);
}

DrawResult DrawContextImpl::DrawText(double x, double y, const std::string& text) {
    return DrawText(x, y, text, m_currentStyle.font, m_currentStyle.pen.color);
}

DrawResult DrawContextImpl::DrawText(const Point& pt, const std::string& text) {
    return DrawText(pt.x, pt.y, text);
}

DrawResult DrawContextImpl::DrawText(double x, double y, const std::string& text,
                                      const Font& font, const Color& color) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawText(x, y, text, font, color);
}

DrawResult DrawContextImpl::DrawImage(double x, double y, const Image& image,
                                       double scaleX, double scaleY) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawImage(x, y, image, scaleX, scaleY);
}

DrawResult DrawContextImpl::DrawImage(const Point& pt, const Image& image,
                                       double scaleX, double scaleY) {
    return DrawImage(pt.x, pt.y, image, scaleX, scaleY);
}

DrawResult DrawContextImpl::DrawImageRect(double x, double y, double w, double h,
                                           const Image& image) {
    if (!m_engine) {
        return DrawResult::kEngineError;
    }
    return m_engine->DrawImageRect(x, y, w, h, image);
}

DrawResult DrawContextImpl::DrawImageRect(const Rect& rect, const Image& image) {
    return DrawImageRect(rect.x, rect.y, rect.w, rect.h, image);
}

void DrawContextImpl::SetTransform(const TransformMatrix& matrix) {
    m_currentTransform = matrix;
    if (m_engine) {
        m_engine->SetTransform(matrix);
    }
}

TransformMatrix DrawContextImpl::GetTransform() const {
    return m_currentTransform;
}

void DrawContextImpl::ResetTransform() {
    m_currentTransform = TransformMatrix::Identity();
    if (m_engine) {
        m_engine->ResetTransform();
    }
}

void DrawContextImpl::Translate(double dx, double dy) {
    m_currentTransform = m_currentTransform * TransformMatrix::Translate(dx, dy);
    if (m_engine) {
        m_engine->SetTransform(m_currentTransform);
    }
}

void DrawContextImpl::Rotate(double angleRadians) {
    m_currentTransform = m_currentTransform * TransformMatrix::Rotate(angleRadians);
    if (m_engine) {
        m_engine->SetTransform(m_currentTransform);
    }
}

void DrawContextImpl::RotateDegrees(double angleDegrees) {
    Rotate(angleDegrees * 3.14159265358979323846 / 180.0);
}

void DrawContextImpl::Scale(double sx, double sy) {
    m_currentTransform = m_currentTransform * TransformMatrix::Scale(sx, sy);
    if (m_engine) {
        m_engine->SetTransform(m_currentTransform);
    }
}

void DrawContextImpl::Shear(double shx, double shy) {
    m_currentTransform = m_currentTransform * TransformMatrix::Shear(shx, shy);
    if (m_engine) {
        m_engine->SetTransform(m_currentTransform);
    }
}

void DrawContextImpl::SetStyle(const DrawStyle& style) {
    m_currentStyle = style;
}

DrawStyle DrawContextImpl::GetStyle() const {
    return m_currentStyle;
}

void DrawContextImpl::SetPen(const Pen& pen) {
    m_currentStyle.pen = pen;
}

Pen DrawContextImpl::GetPen() const {
    return m_currentStyle.pen;
}

void DrawContextImpl::SetBrush(const Brush& brush) {
    m_currentStyle.brush = brush;
}

Brush DrawContextImpl::GetBrush() const {
    return m_currentStyle.brush;
}

void DrawContextImpl::SetFont(const Font& font) {
    m_currentStyle.font = font;
}

Font DrawContextImpl::GetFont() const {
    return m_currentStyle.font;
}

void DrawContextImpl::SetOpacity(double opacity) {
    m_currentOpacity = opacity;
    if (m_engine) {
        m_engine->SetOpacity(opacity);
    }
}

double DrawContextImpl::GetOpacity() const {
    return m_currentOpacity;
}

void DrawContextImpl::SetClipRect(double x, double y, double w, double h) {
    m_currentClipRegion = Region(Rect(x, y, w, h));
    if (m_engine) {
        m_engine->SetClipRect(x, y, w, h);
    }
}

void DrawContextImpl::SetClipRect(const Rect& rect) {
    SetClipRect(rect.x, rect.y, rect.w, rect.h);
}

void DrawContextImpl::SetClipRegion(const Region& region) {
    m_currentClipRegion = region;
    if (m_engine) {
        m_engine->SetClipRegion(region);
    }
}

void DrawContextImpl::ResetClip() {
    m_currentClipRegion = Region();
    if (m_engine) {
        m_engine->ResetClip();
    }
}

Region DrawContextImpl::GetClipRegion() const {
    return m_currentClipRegion;
}

void DrawContextImpl::Clear(const Color& color) {
    if (m_engine) {
        m_engine->Clear(color);
    }
}

void DrawContextImpl::Flush() {
    if (m_engine) {
        m_engine->Flush();
    }
}

TextMetrics DrawContextImpl::MeasureText(const std::string& text) {
    return MeasureText(text, m_currentStyle.font);
}

TextMetrics DrawContextImpl::MeasureText(const std::string& text, const Font& font) {
    if (m_engine) {
        return m_engine->MeasureText(text, font);
    }
    return TextMetrics();
}

}  
}  
