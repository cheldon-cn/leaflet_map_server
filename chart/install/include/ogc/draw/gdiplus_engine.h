#ifndef OGC_DRAW_GDIPLUS_ENGINE_H
#define OGC_DRAW_GDIPLUS_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/simple2d_engine.h"

#ifdef _WIN32

#include <algorithm>
#include <windows.h>

#ifdef DrawText
#undef DrawText
#endif
#ifdef DrawTextA
#undef DrawTextA
#endif
#ifdef DrawTextW
#undef DrawTextW
#endif

using std::min;
using std::max;

#include <gdiplus.h>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API GDIPlusEngine : public Simple2DEngine {
public:
    explicit GDIPlusEngine(RasterImageDevice* device);
    ~GDIPlusEngine() override;
    
    std::string GetName() const override { return "GDIPlusEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    
    DrawResult Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) override;
    void Restore() override;
    
    Gdiplus::Graphics* GetGraphics() const { return m_graphics; }
    
    static bool IsGDIPlusAvailable();
    static ULONG_PTR InitializeGDIPlus();
    static void ShutdownGDIPlus(ULONG_PTR token);

protected:
    void ApplyStyle(const DrawStyle& style);
    Gdiplus::Pen* CreatePen(const DrawStyle& style);
    Gdiplus::Brush* CreateBrush(const DrawStyle& style);
    Gdiplus::Font* CreateFont(const Font& font);
    Gdiplus::Color ToGdiColor(const Color& color);
    Gdiplus::PointF ToPointF(double x, double y);
    
    bool InitializeGraphics();
    void CleanupGraphics();

private:
    Gdiplus::Graphics* m_graphics = nullptr;
    std::unique_ptr<Gdiplus::Pen> m_pen;
    std::unique_ptr<Gdiplus::SolidBrush> m_brush;
    std::unique_ptr<Gdiplus::Font> m_font;
    ULONG_PTR m_gdiplusToken = 0;
    bool m_isActive = false;
    bool m_ownsGraphics = false;
    std::vector<Gdiplus::GraphicsState> m_stateStack;
    
    static int s_instanceCount;
    static ULONG_PTR s_gdiplusToken;
    static bool s_gdiplusInitialized;
};

}  
}  

#endif

#endif
