#ifndef OGC_DRAW_CAIRO_ENGINE_H
#define OGC_DRAW_CAIRO_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/simple2d_engine.h"

#ifdef OGC_HAS_CAIRO

#include <cairo/cairo.h>

namespace ogc {
namespace draw {

class OGC_DRAW_API CairoEngine : public Simple2DEngine {
public:
    explicit CairoEngine(DrawDevice* device);
    ~CairoEngine() override;
    
    std::string GetName() const override { return "CairoEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    cairo_t* m_cr = nullptr;
    cairo_surface_t* m_surface = nullptr;
    
    void ApplyStyle(const DrawStyle& style);
    void SetSourceColor(const Color& color);
    void ApplyPen(const Pen& pen);
    void ApplyBrush(const Brush& brush);
};

} 
}

#endif

#endif
