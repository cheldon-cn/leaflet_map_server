#ifndef OGC_DRAW_VECTOR_ENGINE_H
#define OGC_DRAW_VECTOR_ENGINE_H

#include "ogc/draw/draw_engine.h"
#include "ogc/draw/export.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/draw_style.h"
#include <stack>
#include <vector>

namespace ogc {
namespace draw {

struct VectorStateEntry {
    TransformMatrix transform;
    Region clipRegion;
    double opacity;
    Pen pen;
    Brush brush;
    Font font;
};

class OGC_DRAW_API VectorEngine : public DrawEngine {
public:
    explicit VectorEngine(DrawDevice* device);
    virtual ~VectorEngine();

    EngineType GetType() const override { return EngineType::kVector; }
    std::string GetName() const override { return "VectorEngine"; }
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
    void Flush() override;

protected:
    virtual void WritePath(const std::vector<Point>& points, bool closed) = 0;
    virtual void WriteFill(const Color& color, FillRule rule) = 0;
    virtual void WriteStroke(const Pen& pen) = 0;
    virtual void WriteText(double x, double y, const std::string& text,
                          const Font& font, const Color& color) = 0;
    virtual void WriteImage(double x, double y, const Image& image,
                           double scaleX, double scaleY) = 0;
    virtual void DoSave() = 0;
    virtual void DoRestore() = 0;
    virtual void DoSetTransform(const TransformMatrix& matrix) = 0;
    virtual void DoSetClip(const Region& region) = 0;

    void BuildPath(const double* x, const double* y, int count);
    void ApplyTransform();
    Point TransformPoint(double x, double y) const;
    std::vector<Point> TransformPoints(const double* x, const double* y, int count) const;

    DrawDevice* m_device;
    TransformMatrix m_transform;
    Region m_clipRegion;
    double m_opacity;
    double m_tolerance;
    bool m_antialiasing;
    bool m_active;

    std::stack<VectorStateEntry> m_stateStack;
    std::vector<Point> m_currentPath;
    DrawStyle m_currentStyle;
};

} // namespace draw
} // namespace ogc

#endif // OGC_DRAW_VECTOR_ENGINE_H
