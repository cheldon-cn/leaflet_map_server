#include "ogc/draw/direct2d_engine.h"
#include "ogc/draw/raster_image_device.h"

#ifdef _WIN32

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace draw {

ComPtr<ID2D1Factory> Direct2DEngine::s_factory;
ComPtr<IDWriteFactory> Direct2DEngine::s_writeFactory;
int Direct2DEngine::s_instanceCount = 0;
bool Direct2DEngine::s_initialized = false;

Direct2DEngine::Direct2DEngine(DrawDevice* device)
    : GPUAcceleratedEngine(device)
    , m_isActive(false)
{
    if (s_instanceCount == 0 && !s_initialized) {
        InitializeDirect2D();
    }
    s_instanceCount++;
}

Direct2DEngine::~Direct2DEngine() {
    if (m_isActive) {
        End();
    }
    DiscardDeviceResources();
    
    s_instanceCount--;
    if (s_instanceCount == 0 && s_initialized) {
        ShutdownDirect2D();
    }
}

bool Direct2DEngine::IsDirect2DAvailable() {
    return s_factory != nullptr;
}

HRESULT Direct2DEngine::InitializeDirect2D() {
    if (s_initialized) {
        return S_OK;
    }
    
    ID2D1Factory* pFactory = nullptr;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    if (FAILED(hr)) {
        return hr;
    }
    s_factory.Attach(pFactory);
    
    IDWriteFactory* pWriteFactory = nullptr;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
                              reinterpret_cast<IUnknown**>(&pWriteFactory));
    if (FAILED(hr)) {
        s_factory.Reset();
        return hr;
    }
    s_writeFactory.Attach(pWriteFactory);
    
    s_initialized = true;
    return S_OK;
}

void Direct2DEngine::ShutdownDirect2D() {
    s_writeFactory.Reset();
    s_factory.Reset();
    s_initialized = false;
}

DrawResult Direct2DEngine::Begin() {
    if (m_isActive) {
        return DrawResult::kSuccess;
    }
    
    if (!CreateDeviceResources()) {
        return DrawResult::kDeviceError;
    }
    
    m_renderTarget->BeginDraw();
    m_isActive = true;
    return DrawResult::kSuccess;
}

void Direct2DEngine::End() {
    if (!m_isActive || !m_renderTarget) {
        return;
    }
    
    HRESULT hr = m_renderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        DiscardDeviceResources();
    }
    
    m_isActive = false;
}

bool Direct2DEngine::CreateDeviceResources() {
    if (m_renderTarget) {
        return true;
    }
    
    if (!s_factory) {
        return false;
    }
    
    RasterImageDevice* rasterDevice = dynamic_cast<RasterImageDevice*>(GetDevice());
    if (!rasterDevice) {
        return false;
    }
    
    int width = rasterDevice->GetWidth();
    int height = rasterDevice->GetHeight();
    uint8_t* data = rasterDevice->GetPixelData();
    
    if (width <= 0 || height <= 0 || !data) {
        return false;
    }
    
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );
    
    ComPtr<ID2D1DCRenderTarget> dcRenderTarget;
    HRESULT hr = s_factory->CreateDCRenderTarget(&rtProps, &dcRenderTarget);
    
    if (FAILED(hr)) {
        return false;
    }
    
    m_renderTarget = dcRenderTarget;
    
    hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_solidBrush);
    if (FAILED(hr)) {
        m_renderTarget.Reset();
        return false;
    }
    
    return true;
}

void Direct2DEngine::DiscardDeviceResources() {
    m_solidBrush.Reset();
    m_renderTarget.Reset();
}

DrawResult Direct2DEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    double pointSize = style.pointSize > 0 ? style.pointSize : 3.0;
    
    ComPtr<ID2D1Brush> brush = CreateBrush(style.brush.color);
    if (!brush) {
        return DrawResult::kInvalidParameter;
    }
    
    for (int i = 0; i < count; ++i) {
        D2D1_ELLIPSE ellipse = ToD2DEllipse(x[i], y[i], pointSize, pointSize);
        m_renderTarget->FillEllipse(ellipse, brush.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    ComPtr<ID2D1Brush> brush = CreateBrush(style.pen.color);
    if (!brush) {
        return DrawResult::kInvalidParameter;
    }
    
    float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
    ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
    
    for (int i = 0; i < count; ++i) {
        D2D1_POINT_2F p1 = ToD2DPoint(x1[i], y1[i]);
        D2D1_POINT_2F p2 = ToD2DPoint(x2[i], y2[i]);
        m_renderTarget->DrawLine(p1, p2, brush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawLineString(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count < 2) {
        return DrawResult::kInvalidParameter;
    }
    
    ComPtr<ID2D1Brush> brush = CreateBrush(style.pen.color);
    if (!brush) {
        return DrawResult::kInvalidParameter;
    }
    
    float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
    ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
    
    for (int i = 0; i < count - 1; ++i) {
        D2D1_POINT_2F p1 = ToD2DPoint(x[i], y[i]);
        D2D1_POINT_2F p2 = ToD2DPoint(x[i + 1], y[i + 1]);
        m_renderTarget->DrawLine(p1, p2, brush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count < 3) {
        return DrawResult::kInvalidParameter;
    }
    
    ComPtr<ID2D1PathGeometry> geometry;
    HRESULT hr = s_factory->CreatePathGeometry(&geometry);
    if (FAILED(hr)) {
        return DrawResult::kEngineError;
    }
    
    ComPtr<ID2D1GeometrySink> sink;
    hr = geometry->Open(&sink);
    if (FAILED(hr)) {
        return DrawResult::kEngineError;
    }
    
    sink->BeginFigure(ToD2DPoint(x[0], y[0]), D2D1_FIGURE_BEGIN_FILLED);
    
    for (int i = 1; i < count; ++i) {
        sink->AddLine(ToD2DPoint(x[i], y[i]));
    }
    
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    
    if (fill) {
        ComPtr<ID2D1Brush> brush = CreateBrush(style.brush.color);
        if (brush) {
            m_renderTarget->FillGeometry(geometry.Get(), brush.Get());
        }
    }
    
    ComPtr<ID2D1Brush> penBrush = CreateBrush(style.pen.color);
    if (penBrush) {
        float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
        ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
        m_renderTarget->DrawGeometry(geometry.Get(), penBrush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawRect(double x, double y, double w, double h, const DrawStyle& style, bool fill) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    D2D1_RECT_F rect = ToD2DRect(x, y, w, h);
    
    if (fill) {
        ComPtr<ID2D1Brush> brush = CreateBrush(style.brush.color);
        if (brush) {
            m_renderTarget->FillRectangle(rect, brush.Get());
        }
    }
    
    ComPtr<ID2D1Brush> penBrush = CreateBrush(style.pen.color);
    if (penBrush) {
        float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
        ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
        m_renderTarget->DrawRectangle(rect, penBrush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawCircle(double cx, double cy, double radius, const DrawStyle& style, bool fill) {
    return DrawEllipse(cx, cy, radius, radius, style, fill);
}

DrawResult Direct2DEngine::DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style, bool fill) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    D2D1_ELLIPSE ellipse = ToD2DEllipse(cx, cy, rx, ry);
    
    if (fill) {
        ComPtr<ID2D1Brush> brush = CreateBrush(style.brush.color);
        if (brush) {
            m_renderTarget->FillEllipse(ellipse, brush.Get());
        }
    }
    
    ComPtr<ID2D1Brush> penBrush = CreateBrush(style.pen.color);
    if (penBrush) {
        float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
        ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
        m_renderTarget->DrawEllipse(ellipse, penBrush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    ComPtr<ID2D1PathGeometry> geometry;
    HRESULT hr = s_factory->CreatePathGeometry(&geometry);
    if (FAILED(hr)) {
        return DrawResult::kEngineError;
    }
    
    ComPtr<ID2D1GeometrySink> sink;
    hr = geometry->Open(&sink);
    if (FAILED(hr)) {
        return DrawResult::kEngineError;
    }
    
    D2D1_ARC_SEGMENT arc;
    arc.point = D2D1::Point2F(
        static_cast<float>(cx + rx * std::cos(startAngle + sweepAngle)),
        static_cast<float>(cy + ry * std::sin(startAngle + sweepAngle))
    );
    arc.size = D2D1::SizeF(static_cast<float>(rx), static_cast<float>(ry));
    arc.rotationAngle = static_cast<float>(sweepAngle * 180.0 / M_PI);
    arc.sweepDirection = sweepAngle > 0 ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
    arc.arcSize = std::abs(sweepAngle) > M_PI ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
    
    D2D1_POINT_2F startPoint = D2D1::Point2F(
        static_cast<float>(cx + rx * std::cos(startAngle)),
        static_cast<float>(cy + ry * std::sin(startAngle))
    );
    
    sink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddArc(arc);
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    
    ComPtr<ID2D1Brush> brush = CreateBrush(style.pen.color);
    if (brush) {
        float strokeWidth = static_cast<float>(style.pen.width > 0 ? style.pen.width : 1.0);
        ComPtr<ID2D1StrokeStyle> strokeStyle = CreateStrokeStyle(style.pen);
        m_renderTarget->DrawGeometry(geometry.Get(), brush.Get(), strokeWidth, strokeStyle.Get());
    }
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (text.empty()) {
        return DrawResult::kInvalidParameter;
    }
    
    if (!s_writeFactory) {
        return DrawResult::kEngineError;
    }
    
    ComPtr<IDWriteTextFormat> textFormat = CreateTextFormat(font);
    if (!textFormat) {
        return DrawResult::kInvalidParameter;
    }
    
    ComPtr<ID2D1Brush> brush = CreateBrush(color);
    if (!brush) {
        return DrawResult::kInvalidParameter;
    }
    
    int wcharsNeeded = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    std::vector<wchar_t> wtext(wcharsNeeded);
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wtext.data(), wcharsNeeded);
    
    D2D1_RECT_F layoutRect = D2D1::RectF(
        static_cast<float>(x),
        static_cast<float>(y),
        FLT_MAX, FLT_MAX
    );
    
    m_renderTarget->DrawText(wtext.data(), static_cast<UINT32>(wcslen(wtext.data())), 
                              textFormat.Get(), layoutRect, brush.Get());
    
    return DrawResult::kSuccess;
}

DrawResult Direct2DEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_renderTarget || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    return DrawResult::kNotImplemented;
}

void Direct2DEngine::SetTransform(const TransformMatrix& matrix) {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    D2D1_MATRIX_3X2_F d2dMatrix = D2D1::Matrix3x2F(
        static_cast<float>(matrix.m[0][0]), static_cast<float>(matrix.m[0][1]),
        static_cast<float>(matrix.m[1][0]), static_cast<float>(matrix.m[1][1]),
        static_cast<float>(matrix.m[0][2]), static_cast<float>(matrix.m[1][2])
    );
    
    m_renderTarget->SetTransform(d2dMatrix);
    GPUAcceleratedEngine::SetTransform(matrix);
}

void Direct2DEngine::SetClipRegion(const Region& region) {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    Rect bounds = region.GetBounds();
    D2D1_RECT_F clipRect = ToD2DRect(bounds.x, bounds.y, bounds.w, bounds.h);
    
    m_renderTarget->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    GPUAcceleratedEngine::SetClipRegion(region);
}

void Direct2DEngine::Save(StateFlags flags) {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    D2D1_MATRIX_3X2_F transform;
    m_renderTarget->GetTransform(&transform);
    m_transformStack.push(transform);
    
    GPUAcceleratedEngine::Save(flags);
}

void Direct2DEngine::Restore() {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    if (!m_transformStack.empty()) {
        D2D1_MATRIX_3X2_F transform = m_transformStack.top();
        m_transformStack.pop();
        m_renderTarget->SetTransform(transform);
    }
    
    GPUAcceleratedEngine::Restore();
}

void Direct2DEngine::Clear(const Color& color) {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    m_renderTarget->Clear(ToD2DColor(color));
}

void Direct2DEngine::Flush() {
    if (!m_renderTarget || !m_isActive) {
        return;
    }
    
    m_renderTarget->Flush();
}

ComPtr<ID2D1Brush> Direct2DEngine::CreateBrush(const Color& color) {
    ComPtr<ID2D1SolidColorBrush> brush;
    HRESULT hr = m_renderTarget->CreateSolidColorBrush(ToD2DColor(color), &brush);
    if (FAILED(hr)) {
        return nullptr;
    }
    return brush;
}

ComPtr<ID2D1StrokeStyle> Direct2DEngine::CreateStrokeStyle(const Pen& pen) {
    D2D1_STROKE_STYLE_PROPERTIES props = D2D1::StrokeStyleProperties();
    
    switch (pen.cap) {
        case LineCap::kFlat:
            props.startCap = props.endCap = D2D1_CAP_STYLE_FLAT;
            break;
        case LineCap::kRound:
            props.startCap = props.endCap = D2D1_CAP_STYLE_ROUND;
            break;
        case LineCap::kSquare:
            props.startCap = props.endCap = D2D1_CAP_STYLE_SQUARE;
            break;
    }
    
    switch (pen.join) {
        case LineJoin::kMiter:
            props.lineJoin = D2D1_LINE_JOIN_MITER;
            break;
        case LineJoin::kRound:
            props.lineJoin = D2D1_LINE_JOIN_ROUND;
            break;
        case LineJoin::kBevel:
            props.lineJoin = D2D1_LINE_JOIN_BEVEL;
            break;
    }
    
    switch (pen.style) {
        case PenStyle::kSolid:
            props.dashStyle = D2D1_DASH_STYLE_SOLID;
            break;
        case PenStyle::kDash:
            props.dashStyle = D2D1_DASH_STYLE_DASH;
            break;
        case PenStyle::kDot:
            props.dashStyle = D2D1_DASH_STYLE_DOT;
            break;
        case PenStyle::kDashDot:
            props.dashStyle = D2D1_DASH_STYLE_DASH_DOT;
            break;
        case PenStyle::kDashDotDot:
            props.dashStyle = D2D1_DASH_STYLE_DASH_DOT_DOT;
            break;
    }
    
    ComPtr<ID2D1StrokeStyle> strokeStyle;
    HRESULT hr = s_factory->CreateStrokeStyle(props, nullptr, 0, &strokeStyle);
    if (FAILED(hr)) {
        return nullptr;
    }
    return strokeStyle;
}

ComPtr<IDWriteTextFormat> Direct2DEngine::CreateTextFormat(const Font& font) {
    if (!s_writeFactory) {
        return nullptr;
    }
    
    std::wstring familyName;
    std::string fontFamily = font.GetFamily();
    if (!fontFamily.empty()) {
        int wcharsNeeded = MultiByteToWideChar(CP_UTF8, 0, fontFamily.c_str(), -1, nullptr, 0);
        familyName.resize(wcharsNeeded);
        MultiByteToWideChar(CP_UTF8, 0, fontFamily.c_str(), -1, &familyName[0], wcharsNeeded);
        if (!familyName.empty() && familyName.back() == L'\0') {
            familyName.pop_back();
        }
    } else {
        familyName = L"Arial";
    }
    
    DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL;
    if (font.IsBold()) {
        weight = DWRITE_FONT_WEIGHT_BOLD;
    }
    
    DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
    if (font.IsItalic()) {
        style = DWRITE_FONT_STYLE_ITALIC;
    }
    
    float size = static_cast<float>(font.GetSize() > 0 ? font.GetSize() : 12.0);
    
    ComPtr<IDWriteTextFormat> textFormat;
    HRESULT hr = s_writeFactory->CreateTextFormat(
        familyName.c_str(),
        nullptr,
        weight,
        style,
        DWRITE_FONT_STRETCH_NORMAL,
        size,
        L"",
        &textFormat
    );
    
    if (FAILED(hr)) {
        return nullptr;
    }
    return textFormat;
}

D2D1_COLOR_F Direct2DEngine::ToD2DColor(const Color& color) const {
    return D2D1::ColorF(
        static_cast<float>(color.GetRed()) / 255.0f,
        static_cast<float>(color.GetGreen()) / 255.0f,
        static_cast<float>(color.GetBlue()) / 255.0f,
        static_cast<float>(color.GetAlpha()) / 255.0f
    );
}

D2D1_POINT_2F Direct2DEngine::ToD2DPoint(double x, double y) const {
    return D2D1::Point2F(static_cast<float>(x), static_cast<float>(y));
}

D2D1_RECT_F Direct2DEngine::ToD2DRect(double x, double y, double w, double h) const {
    return D2D1::RectF(static_cast<float>(x), static_cast<float>(y),
                       static_cast<float>(x + w), static_cast<float>(y + h));
}

D2D1_ELLIPSE Direct2DEngine::ToD2DEllipse(double cx, double cy, double rx, double ry) const {
    return D2D1::Ellipse(ToD2DPoint(cx, cy), static_cast<float>(rx), static_cast<float>(ry));
}

}

}

#endif
