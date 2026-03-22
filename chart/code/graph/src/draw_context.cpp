#include "ogc/draw/draw_context.h"

namespace ogc {
namespace draw {

DrawContext::DrawContext()
    : m_initialized(false)
    , m_drawing(false)
    , m_clipRectDepth(0) {
    m_transformStack.push(TransformMatrix::Identity());
    m_opacityStack.push(1.0);
}

DrawContext::DrawContext(DrawDevicePtr device)
    : m_device(device)
    , m_initialized(false)
    , m_drawing(false)
    , m_clipRectDepth(0) {
    m_transformStack.push(TransformMatrix::Identity());
    m_opacityStack.push(1.0);
}

DrawContext::DrawContext(DrawDevicePtr device, DrawEnginePtr engine)
    : m_device(device)
    , m_engine(engine)
    , m_initialized(false)
    , m_drawing(false)
    , m_clipRectDepth(0) {
    m_transformStack.push(TransformMatrix::Identity());
    m_opacityStack.push(1.0);
}

DrawContext::~DrawContext() {
    if (m_initialized) {
        Finalize();
    }
}

DrawResult DrawContext::Initialize() {
    if (m_initialized) {
        return DrawResult::kSuccess;
    }
    
    if (m_device) {
        DrawResult result = m_device->Initialize();
        if (result != DrawResult::kSuccess) {
            return result;
        }
    }
    
    if (m_engine) {
        DrawResult result = m_engine->Initialize();
        if (result != DrawResult::kSuccess) {
            return result;
        }
        if (m_device) {
            m_engine->SetDevice(m_device);
        }
    }
    
    m_initialized = true;
    return DrawResult::kSuccess;
}

DrawResult DrawContext::Finalize() {
    if (!m_initialized) {
        return DrawResult::kSuccess;
    }
    
    if (m_drawing) {
        AbortDraw();
    }
    
    if (m_engine) {
        m_engine->Finalize();
    }
    
    if (m_device) {
        m_device->Finalize();
    }
    
    m_initialized = false;
    return DrawResult::kSuccess;
}

bool DrawContext::IsInitialized() const {
    return m_initialized;
}

void DrawContext::SetDevice(DrawDevicePtr device) {
    m_device = device;
}

DrawDevicePtr DrawContext::GetDevice() const {
    return m_device;
}

bool DrawContext::HasDevice() const {
    return m_device != nullptr;
}

void DrawContext::SetEngine(DrawEnginePtr engine) {
    m_engine = engine;
}

DrawEnginePtr DrawContext::GetEngine() const {
    return m_engine;
}

bool DrawContext::HasEngine() const {
    return m_engine != nullptr;
}

void DrawContext::SetDriver(DrawDriverPtr driver) {
    m_driver = driver;
}

DrawDriverPtr DrawContext::GetDriver() const {
    return m_driver;
}

bool DrawContext::HasDriver() const {
    return m_driver != nullptr;
}

DrawResult DrawContext::BeginDraw(const DrawParams& params) {
    if (!m_initialized) {
        return DrawResult::kEngineNotReady;
    }
    
    m_params = params;
    
    if (m_device) {
        DrawResult result = m_device->BeginDraw(params);
        if (result != DrawResult::kSuccess) {
            return result;
        }
    }
    
    if (m_engine) {
        DrawResult result = m_engine->BeginDraw(params);
        if (result != DrawResult::kSuccess) {
            m_device->AbortDraw();
            return result;
        }
    }
    
    m_drawing = true;
    return DrawResult::kSuccess;
}

DrawResult DrawContext::EndDraw() {
    if (!m_drawing) {
        return DrawResult::kSuccess;
    }
    
    DrawResult result = DrawResult::kSuccess;
    
    if (m_engine) {
        result = m_engine->EndDraw();
    }
    
    if (m_device) {
        DrawResult deviceResult = m_device->EndDraw();
        if (result == DrawResult::kSuccess) {
            result = deviceResult;
        }
    }
    
    m_drawing = false;
    return result;
}

DrawResult DrawContext::AbortDraw() {
    if (!m_drawing) {
        return DrawResult::kSuccess;
    }
    
    if (m_engine) {
        m_engine->AbortDraw();
    }
    
    if (m_device) {
        m_device->AbortDraw();
    }
    
    m_drawing = false;
    return DrawResult::kSuccess;
}

bool DrawContext::IsDrawing() const {
    return m_drawing;
}

void DrawContext::PushTransform() {
    m_transformStack.push(GetTransform());
}

void DrawContext::PopTransform() {
    if (m_transformStack.size() > 1) {
        m_transformStack.pop();
        if (m_device) {
            m_device->SetTransform(m_transformStack.top());
        }
    }
}

void DrawContext::SetTransform(const TransformMatrix& transform) {
    m_transformStack.top() = transform;
    if (m_device) {
        m_device->SetTransform(transform);
    }
}

TransformMatrix DrawContext::GetTransform() const {
    return m_transformStack.top();
}

void DrawContext::MultiplyTransform(const TransformMatrix& transform) {
    TransformMatrix current = GetTransform();
    SetTransform(current.Multiply(transform));
}

void DrawContext::ResetTransform() {
    SetTransform(TransformMatrix::Identity());
}

void DrawContext::Translate(double tx, double ty) {
    TransformMatrix t = TransformMatrix::CreateTranslation(tx, ty);
    MultiplyTransform(t);
}

void DrawContext::Scale(double sx, double sy) {
    TransformMatrix s = TransformMatrix::CreateScale(sx, sy);
    MultiplyTransform(s);
}

void DrawContext::Rotate(double angle) {
    TransformMatrix r = TransformMatrix::CreateRotation(angle);
    MultiplyTransform(r);
}

void DrawContext::PushClipRect(double x, double y, double width, double height) {
    if (m_device) {
        m_device->SetClipRect(x, y, width, height);
    }
    m_clipRectDepth++;
}

void DrawContext::PopClipRect() {
    if (m_clipRectDepth > 0) {
        m_clipRectDepth--;
        if (m_device) {
            m_device->ClearClipRect();
        }
    }
}

void DrawContext::ClearClipRect() {
    if (m_device) {
        m_device->ClearClipRect();
    }
    m_clipRectDepth = 0;
}

void DrawContext::PushStyle(const DrawStyle& style) {
    m_styleStack.push(style);
}

void DrawContext::PopStyle() {
    if (!m_styleStack.empty()) {
        m_styleStack.pop();
    }
}

void DrawContext::SetStyle(const DrawStyle& style) {
    m_styleStack.push(style);
}

DrawStyle DrawContext::GetStyle() const {
    if (!m_styleStack.empty()) {
        return m_styleStack.top();
    }
    return m_defaultStyle;
}

void DrawContext::PushOpacity(double opacity) {
    m_opacityStack.push(opacity);
    if (m_device) {
        m_device->SetOpacity(opacity);
    }
}

void DrawContext::PopOpacity() {
    if (m_opacityStack.size() > 1) {
        m_opacityStack.pop();
        if (m_device) {
            m_device->SetOpacity(m_opacityStack.top());
        }
    }
}

void DrawContext::SetOpacity(double opacity) {
    m_opacityStack.top() = opacity;
    if (m_device) {
        m_device->SetOpacity(opacity);
    }
}

double DrawContext::GetOpacity() const {
    return m_opacityStack.top();
}

DrawResult DrawContext::DrawGeometry(const Geometry* geometry) {
    return DrawGeometry(geometry, GetStyle());
}

DrawResult DrawContext::DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (!geometry) {
        return DrawResult::kInvalidParams;
    }
    
    if (m_engine) {
        return m_engine->DrawGeometry(geometry, style);
    }
    
    if (m_device) {
        return m_device->DrawGeometry(geometry, style);
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawPoint(double x, double y) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawPoint(x, y, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawLine(double x1, double y1, double x2, double y2) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawLine(x1, y1, x2, y2, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawRect(double x, double y, double width, double height) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawRect(x, y, width, height, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawCircle(double cx, double cy, double radius) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawCircle(cx, cy, radius, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawPolygon(const double* x, const double* y, int count) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawPolygon(x, y, count, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawPolyline(const double* x, const double* y, int count) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawPolyline(x, y, count, GetStyle());
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawText(double x, double y, const std::string& text) {
    return DrawText(x, y, text, m_defaultFont, m_defaultColor);
}

DrawResult DrawContext::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawText(x, y, text, font, color);
    }
    
    return DrawResult::kDeviceNotReady;
}

DrawResult DrawContext::DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) {
    if (!m_drawing) {
        return DrawResult::kEngineNotReady;
    }
    
    if (m_device) {
        return m_device->DrawImage(x, y, width, height, data, dataWidth, dataHeight, channels);
    }
    
    return DrawResult::kDeviceNotReady;
}

void DrawContext::Clear(const Color& color) {
    if (m_device) {
        m_device->Clear(color);
    }
}

void DrawContext::Flush() {
    if (m_engine) {
        m_engine->Flush();
    }
}

void DrawContext::WorldToScreen(double worldX, double worldY, double& screenX, double& screenY) const {
    if (m_engine) {
        m_engine->WorldToScreen(worldX, worldY, screenX, screenY);
    } else {
        GetTransform().Transform(worldX, worldY, screenX, screenY);
    }
}

void DrawContext::ScreenToWorld(double screenX, double screenY, double& worldX, double& worldY) const {
    if (m_engine) {
        m_engine->ScreenToWorld(screenX, screenY, worldX, worldY);
    } else {
        TransformMatrix inverse = GetTransform().Inverse();
        inverse.Transform(screenX, screenY, worldX, worldY);
    }
}

Envelope DrawContext::GetVisibleExtent() const {
    if (m_engine) {
        return m_engine->GetVisibleExtent();
    }
    return m_params.extent;
}

double DrawContext::GetScale() const {
    return m_params.scale;
}

double DrawContext::GetDpi() const {
    return m_params.dpi;
}

const DrawParams& DrawContext::GetDrawParams() const {
    return m_params;
}

void DrawContext::SetBackground(const Color& color) {
    m_background = color;
}

Color DrawContext::GetBackground() const {
    return m_background;
}

void DrawContext::SetDefaultStyle(const DrawStyle& style) {
    m_defaultStyle = style;
}

DrawStyle DrawContext::GetDefaultStyle() const {
    return m_defaultStyle;
}

void DrawContext::SetDefaultFont(const Font& font) {
    m_defaultFont = font;
}

Font DrawContext::GetDefaultFont() const {
    return m_defaultFont;
}

void DrawContext::SetDefaultColor(const Color& color) {
    m_defaultColor = color;
}

Color DrawContext::GetDefaultColor() const {
    return m_defaultColor;
}

DrawContextPtr DrawContext::Create(DrawDevicePtr device) {
    return std::make_shared<DrawContext>(device);
}

DrawContextPtr DrawContext::Create(DrawDevicePtr device, DrawEnginePtr engine) {
    return std::make_shared<DrawContext>(device, engine);
}

}  
}  
