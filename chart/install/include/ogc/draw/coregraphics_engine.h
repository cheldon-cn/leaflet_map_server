#ifndef OGC_DRAW_COREGRAPHICS_ENGINE_H
#define OGC_DRAW_COREGRAPHICS_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/simple2d_engine.h"

#ifdef __APPLE__

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

namespace ogc {
namespace draw {

class OGC_DRAW_API CoreGraphicsEngine : public Simple2DEngine {
public:
    explicit CoreGraphicsEngine(DrawDevice* device);
    ~CoreGraphicsEngine() override;
    
    std::string GetName() const override { return "CoreGraphicsEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    void SetContext(CGContextRef context);
    CGContextRef GetContext() const { return m_context; }

private:
    CGContextRef m_context = nullptr;
    bool m_ownsContext = false;
    
    void ApplyStyle(const DrawStyle& style);
    void SetStrokeColor(const Color& color);
    void SetFillColor(const Color& color);
    void ApplyPen(const Pen& pen);
    void ApplyBrush(const Brush& brush);
    CGAffineTransform CreateAffineTransform(const TransformMatrix& matrix);
};

}
}

#endif

#endif
