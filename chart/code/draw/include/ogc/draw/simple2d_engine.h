#ifndef OGC_DRAW_SIMPLE2D_ENGINE_H
#define OGC_DRAW_SIMPLE2D_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <stack>
#include <memory>

namespace ogc {
namespace draw {

struct EngineStateEntry {
    TransformMatrix transform;
    Region clipRegion;
    double opacity;
    double tolerance;
    bool antialiasing;
    StateFlags flags;
};

class OGC_DRAW_API Simple2DEngine : public DrawEngine {
public:
    explicit Simple2DEngine(RasterImageDevice* device);
    ~Simple2DEngine() override;

    std::string GetName() const override { return "Simple2DEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    DrawDevice* GetDevice() const override { return m_device; }

    DrawResult Begin() override;
    void End() override;
    bool IsActive() const override { return m_active; }

    DrawResult DrawPoints(const double* x, const double* y, int count,
                           const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2, int count,
                          const DrawStyle& style) override;
    DrawResult DrawLineString(const double* x, const double* y, int count,
                               const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count,
                            const DrawStyle& style, bool fill = true) override;
    DrawResult DrawRect(double x, double y, double w, double h,
                         const DrawStyle& style, bool fill = true) override;
    DrawResult DrawCircle(double cx, double cy, double radius,
                           const DrawStyle& style, bool fill = true) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry,
                            const DrawStyle& style, bool fill = true) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry,
                        double startAngle, double sweepAngle,
                        const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text,
                         const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image,
                          double scaleX = 1.0, double scaleY = 1.0) override;
    DrawResult DrawImageRect(double x, double y, double w, double h,
                              const Image& image) override;
    DrawResult DrawGeometry(const Geometry* geometry,
                             const DrawStyle& style) override;

    void SetTransform(const TransformMatrix& matrix) override;
    TransformMatrix GetTransform() const override { return m_transform; }
    void ResetTransform() override;

    void SetClipRect(double x, double y, double w, double h) override;
    void SetClipRegion(const Region& region) override;
    void ResetClip() override;
    Region GetClipRegion() const override { return m_clipRegion; }

    void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) override;
    void Restore() override;

    void SetOpacity(double opacity) override;
    double GetOpacity() const override { return m_opacity; }

    void SetTolerance(double tolerance) override { m_tolerance = tolerance; }
    double GetTolerance() const override { return m_tolerance; }

    void SetAntialiasingEnabled(bool enabled) override { m_antialiasing = enabled; }
    bool IsAntialiasingEnabled() const override { return m_antialiasing; }

    TextMetrics MeasureText(const std::string& text, const Font& font) override;

    void Clear(const Color& color) override;
    void Flush() override {}

protected:
    Point TransformPoint(double x, double y) const;
    bool IsPointVisible(int x, int y) const;
    void BlendPixel(int x, int y, const Color& color);
    void DrawLineBresenham(int x1, int y1, int x2, int y2, const Color& color);
    void DrawHorizontalLine(int x1, int x2, int y, const Color& color);
    void FillScanline(int y, int x1, int x2, const Color& color);
    void FillCircle(int cx, int cy, int radius, const Color& color);
    void StrokeCircle(int cx, int cy, int radius, const Color& color, int width);

private:
    RasterImageDevice* m_device;
    TransformMatrix m_transform;
    Region m_clipRegion;
    double m_opacity;
    double m_tolerance;
    bool m_antialiasing;
    bool m_active;
    std::stack<EngineStateEntry> m_stateStack;
};

}  
}  

#endif  
