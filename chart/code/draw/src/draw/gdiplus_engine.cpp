#include "ogc/draw/gdiplus_engine.h"
#include "ogc/draw/raster_image_device.h"

#ifdef _WIN32

#include <algorithm>

namespace ogc {
namespace draw {

int GDIPlusEngine::s_instanceCount = 0;
ULONG_PTR GDIPlusEngine::s_gdiplusToken = 0;
bool GDIPlusEngine::s_gdiplusInitialized = false;

GDIPlusEngine::GDIPlusEngine(RasterImageDevice* device)
    : Simple2DEngine(device)
    , m_graphics(nullptr)
    , m_isActive(false)
    , m_ownsGraphics(false)
{
    if (s_instanceCount == 0 && !s_gdiplusInitialized) {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, nullptr);
        s_gdiplusInitialized = true;
    }
    s_instanceCount++;
}

GDIPlusEngine::~GDIPlusEngine() {
    if (m_isActive) {
        End();
    }
    CleanupGraphics();
    
    s_instanceCount--;
    if (s_instanceCount == 0 && s_gdiplusInitialized) {
        Gdiplus::GdiplusShutdown(s_gdiplusToken);
        s_gdiplusInitialized = false;
    }
}

bool GDIPlusEngine::IsGDIPlusAvailable() {
    return true;
}

ULONG_PTR GDIPlusEngine::InitializeGDIPlus() {
    if (!s_gdiplusInitialized) {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, nullptr);
        s_gdiplusInitialized = true;
    }
    return s_gdiplusToken;
}

void GDIPlusEngine::ShutdownGDIPlus(ULONG_PTR token) {
    if (s_gdiplusInitialized && token == s_gdiplusToken) {
        Gdiplus::GdiplusShutdown(s_gdiplusToken);
        s_gdiplusInitialized = false;
    }
}

DrawResult GDIPlusEngine::Begin() {
    if (m_isActive) {
        return DrawResult::kSuccess;
    }
    
    if (!InitializeGraphics()) {
        return DrawResult::kDeviceError;
    }
    
    DrawResult result = Simple2DEngine::Begin();
    if (result != DrawResult::kSuccess) {
        return result;
    }
    
    m_isActive = true;
    return DrawResult::kSuccess;
}

void GDIPlusEngine::End() {
    CleanupGraphics();
    m_isActive = false;
    Simple2DEngine::End();
}

DrawResult GDIPlusEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_graphics || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    
    double pointSize = style.pointSize > 0 ? style.pointSize : 3.0;
    
    for (int i = 0; i < count; ++i) {
        Gdiplus::PointF center = ToPointF(x[i], y[i]);
        Gdiplus::RectF rect(
            center.X - static_cast<Gdiplus::REAL>(pointSize),
            center.Y - static_cast<Gdiplus::REAL>(pointSize),
            static_cast<Gdiplus::REAL>(pointSize * 2),
            static_cast<Gdiplus::REAL>(pointSize * 2)
        );
        
        if (m_brush) {
            m_graphics->FillEllipse(m_brush.get(), rect);
        }
        if (m_pen) {
            m_graphics->DrawEllipse(m_pen.get(), rect);
        }
    }
    
    return DrawResult::kSuccess;
}

DrawResult GDIPlusEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_graphics || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x1 || !y1 || !x2 || !y2 || count <= 0) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    
    if (!m_pen) {
        return DrawResult::kInvalidState;
    }
    
    for (int i = 0; i < count; ++i) {
        Gdiplus::PointF p1 = ToPointF(x1[i], y1[i]);
        Gdiplus::PointF p2 = ToPointF(x2[i], y2[i]);
        m_graphics->DrawLine(m_pen.get(), p1, p2);
    }
    
    return DrawResult::kSuccess;
}

DrawResult GDIPlusEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_graphics || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (!x || !y || count < 3) {
        return DrawResult::kInvalidParameter;
    }
    
    ApplyStyle(style);
    
    std::vector<Gdiplus::PointF> points(count);
    for (int i = 0; i < count; ++i) {
        points[i] = ToPointF(x[i], y[i]);
    }
    
    if (fill && m_brush) {
        m_graphics->FillPolygon(m_brush.get(), points.data(), count);
    }
    
    if (m_pen) {
        m_graphics->DrawPolygon(m_pen.get(), points.data(), count);
    }
    
    return DrawResult::kSuccess;
}

DrawResult GDIPlusEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_graphics || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    if (text.empty()) {
        return DrawResult::kInvalidParameter;
    }
    
    std::unique_ptr<Gdiplus::Font> gdiFont(CreateFont(font));
    if (!gdiFont) {
        return DrawResult::kInvalidParameter;
    }
    
    Gdiplus::SolidBrush textBrush(ToGdiColor(color));
    
    Gdiplus::PointF position = ToPointF(x, y);
    
    std::wstring wtext(text.begin(), text.end());
    m_graphics->DrawString(wtext.c_str(), static_cast<int>(wtext.length()), 
                           gdiFont.get(), position, &textBrush);
    
    return DrawResult::kSuccess;
}

DrawResult GDIPlusEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_graphics || !m_isActive) {
        return DrawResult::kInvalidState;
    }
    
    return DrawResult::kNotImplemented;
}

void GDIPlusEngine::SetTransform(const TransformMatrix& matrix) {
    if (!m_graphics || !m_isActive) {
        return;
    }
    
    Gdiplus::Matrix gdiMatrix(
        static_cast<Gdiplus::REAL>(matrix.m[0][0]),
        static_cast<Gdiplus::REAL>(matrix.m[0][1]),
        static_cast<Gdiplus::REAL>(matrix.m[1][0]),
        static_cast<Gdiplus::REAL>(matrix.m[1][1]),
        static_cast<Gdiplus::REAL>(matrix.m[0][2]),
        static_cast<Gdiplus::REAL>(matrix.m[1][2])
    );
    
    m_graphics->SetTransform(&gdiMatrix);
    Simple2DEngine::SetTransform(matrix);
}

void GDIPlusEngine::SetClipRegion(const Region& region) {
    if (!m_graphics || !m_isActive) {
        return;
    }
    
    Rect bounds = region.GetBounds();
    Gdiplus::RectF clipRect(
        static_cast<Gdiplus::REAL>(bounds.x),
        static_cast<Gdiplus::REAL>(bounds.y),
        static_cast<Gdiplus::REAL>(bounds.w),
        static_cast<Gdiplus::REAL>(bounds.h)
    );
    
    m_graphics->SetClip(clipRect);
    Simple2DEngine::SetClipRegion(region);
}

void GDIPlusEngine::Save(StateFlags flags) {
    if (!m_graphics || !m_isActive) {
        return;
    }
    
    Gdiplus::GraphicsState state = m_graphics->Save();
    m_stateStack.push_back(state);
    Simple2DEngine::Save(flags);
}

void GDIPlusEngine::Restore() {
    if (!m_graphics || !m_isActive) {
        return;
    }
    
    if (!m_stateStack.empty()) {
        Gdiplus::GraphicsState state = m_stateStack.back();
        m_stateStack.pop_back();
        m_graphics->Restore(state);
        Simple2DEngine::Restore();
    }
}

void GDIPlusEngine::ApplyStyle(const DrawStyle& style) {
    m_pen.reset(CreatePen(style));
    m_brush.reset(new Gdiplus::SolidBrush(ToGdiColor(style.brush.color)));
}

Gdiplus::Pen* GDIPlusEngine::CreatePen(const DrawStyle& style) {
    Gdiplus::Color color = ToGdiColor(style.pen.color);
    Gdiplus::Pen* pen = new Gdiplus::Pen(color, static_cast<Gdiplus::REAL>(style.pen.width > 0 ? style.pen.width : 1.0));
    
    switch (style.pen.style) {
        case PenStyle::kSolid:
            pen->SetDashStyle(Gdiplus::DashStyleSolid);
            break;
        case PenStyle::kDash:
            pen->SetDashStyle(Gdiplus::DashStyleDash);
            break;
        case PenStyle::kDot:
            pen->SetDashStyle(Gdiplus::DashStyleDot);
            break;
        case PenStyle::kDashDot:
            pen->SetDashStyle(Gdiplus::DashStyleDashDot);
            break;
        case PenStyle::kDashDotDot:
            pen->SetDashStyle(Gdiplus::DashStyleDashDotDot);
            break;
        default:
            pen->SetDashStyle(Gdiplus::DashStyleSolid);
            break;
    }
    
    return pen;
}

Gdiplus::Brush* GDIPlusEngine::CreateBrush(const DrawStyle& style) {
    return new Gdiplus::SolidBrush(ToGdiColor(style.brush.color));
}

Gdiplus::Font* GDIPlusEngine::CreateFont(const Font& font) {
    std::wstring familyName;
    std::string fontFamily = font.GetFamily();
    if (!fontFamily.empty()) {
        familyName = std::wstring(fontFamily.begin(), fontFamily.end());
    } else {
        familyName = L"Arial";
    }
    
    Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
    bool isBold = font.IsBold();
    bool isItalic = font.IsItalic();
    
    if (isBold && isItalic) {
        fontStyle = Gdiplus::FontStyleBoldItalic;
    } else if (isBold) {
        fontStyle = Gdiplus::FontStyleBold;
    } else if (isItalic) {
        fontStyle = Gdiplus::FontStyleItalic;
    }
    
    Gdiplus::REAL size = static_cast<Gdiplus::REAL>(font.GetSize() > 0 ? font.GetSize() : 12.0);
    
    Gdiplus::Font* gdiFont = new Gdiplus::Font(familyName.c_str(), size, fontStyle);
    return gdiFont;
}

Gdiplus::Color GDIPlusEngine::ToGdiColor(const Color& color) {
    return Gdiplus::Color(
        static_cast<BYTE>(color.GetAlpha()),
        static_cast<BYTE>(color.GetRed()),
        static_cast<BYTE>(color.GetGreen()),
        static_cast<BYTE>(color.GetBlue())
    );
}

Gdiplus::PointF GDIPlusEngine::ToPointF(double x, double y) {
    return Gdiplus::PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y));
}

bool GDIPlusEngine::InitializeGraphics() {
    if (m_graphics) {
        return true;
    }
    
    RasterImageDevice* rasterDevice = dynamic_cast<RasterImageDevice*>(GetDevice());
    if (!rasterDevice) {
        return false;
    }
    
    int width = rasterDevice->GetWidth();
    int height = rasterDevice->GetHeight();
    uint8_t* data = rasterDevice->GetPixelData();
    
    if (width > 0 && height > 0 && data) {
        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(
            width, height, width * 4, PixelFormat32bppARGB, data
        );
        
        m_graphics = new Gdiplus::Graphics(bitmap);
        m_ownsGraphics = true;
        
        delete bitmap;
        return true;
    }
    
    return false;
}

void GDIPlusEngine::CleanupGraphics() {
    if (m_ownsGraphics && m_graphics) {
        delete m_graphics;
        m_graphics = nullptr;
    }
    m_pen.reset();
    m_brush.reset();
    m_font.reset();
    m_stateStack.clear();
    m_ownsGraphics = false;
}

}  
}  

#endif
