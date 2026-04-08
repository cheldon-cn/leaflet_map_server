#ifndef OGC_DRAW_CONTEXT_H
#define OGC_DRAW_CONTEXT_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/geometry.h"
#include <memory>
#include <vector>
#include <string>

namespace ogc {
namespace draw {

class OGC_DRAW_API DrawContext {
public:
    static std::unique_ptr<DrawContext> Create(DrawDevice* device);

    virtual ~DrawContext() = default;

    virtual DrawResult Begin() = 0;
    virtual void End() = 0;

    virtual DrawDevice* GetDevice() const = 0;
    virtual DrawEngine* GetEngine() const = 0;

    virtual void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) = 0;
    virtual void Restore() = 0;

    virtual DrawResult DrawGeometry(const Geometry* geometry) = 0;
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;

    virtual DrawResult DrawPoint(double x, double y) = 0;
    virtual DrawResult DrawPoint(const Point& pt) = 0;
    virtual DrawResult DrawPoints(const double* x, const double* y, int count) = 0;
    virtual DrawResult DrawPoints(const Point* points, int count) = 0;

    virtual DrawResult DrawLine(double x1, double y1, double x2, double y2) = 0;
    virtual DrawResult DrawLine(const Point& start, const Point& end) = 0;
    virtual DrawResult DrawLines(const double* x1, const double* y1,
                                  const double* x2, const double* y2, int count) = 0;

    virtual DrawResult DrawLineString(const double* x, const double* y, int count) = 0;
    virtual DrawResult DrawLineString(const Point* points, int count) = 0;
    virtual DrawResult DrawLineString(const std::vector<Point>& points) = 0;

    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill = true) = 0;
    virtual DrawResult DrawPolygon(const Point* points, int count, bool fill = true) = 0;
    virtual DrawResult DrawPolygon(const std::vector<Point>& points, bool fill = true) = 0;

    virtual DrawResult DrawRect(double x, double y, double w, double h, bool fill = true) = 0;
    virtual DrawResult DrawRect(const Rect& rect, bool fill = true) = 0;

    virtual DrawResult DrawCircle(double cx, double cy, double radius, bool fill = true) = 0;
    virtual DrawResult DrawCircle(const Point& center, double radius, bool fill = true) = 0;

    virtual DrawResult DrawEllipse(double cx, double cy, double rx, double ry, bool fill = true) = 0;
    virtual DrawResult DrawEllipse(const Point& center, double rx, double ry, bool fill = true) = 0;

    virtual DrawResult DrawArc(double cx, double cy, double rx, double ry,
                                double startAngle, double sweepAngle) = 0;

    virtual DrawResult DrawText(double x, double y, const std::string& text) = 0;
    virtual DrawResult DrawText(const Point& pt, const std::string& text) = 0;
    virtual DrawResult DrawText(double x, double y, const std::string& text,
                                 const Font& font, const Color& color) = 0;

    virtual DrawResult DrawImage(double x, double y, const Image& image,
                                  double scaleX = 1.0, double scaleY = 1.0) = 0;
    virtual DrawResult DrawImage(const Point& pt, const Image& image,
                                  double scaleX = 1.0, double scaleY = 1.0) = 0;
    virtual DrawResult DrawImageRect(double x, double y, double w, double h,
                                      const Image& image) = 0;
    virtual DrawResult DrawImageRect(const Rect& rect, const Image& image) = 0;

    virtual void SetTransform(const TransformMatrix& matrix) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;

    virtual void Translate(double dx, double dy) = 0;
    virtual void Rotate(double angleRadians) = 0;
    virtual void RotateDegrees(double angleDegrees) = 0;
    virtual void Scale(double sx, double sy) = 0;
    virtual void Shear(double shx, double shy) = 0;

    virtual void SetStyle(const DrawStyle& style) = 0;
    virtual DrawStyle GetStyle() const = 0;

    virtual void SetPen(const Pen& pen) = 0;
    virtual Pen GetPen() const = 0;

    virtual void SetBrush(const Brush& brush) = 0;
    virtual Brush GetBrush() const = 0;

    virtual void SetFont(const Font& font) = 0;
    virtual Font GetFont() const = 0;

    virtual void SetOpacity(double opacity) = 0;
    virtual double GetOpacity() const = 0;

    virtual void SetClipRect(double x, double y, double w, double h) = 0;
    virtual void SetClipRect(const Rect& rect) = 0;
    virtual void SetClipRegion(const Region& region) = 0;
    virtual void ResetClip() = 0;
    virtual Region GetClipRegion() const = 0;

    virtual void Clear(const Color& color) = 0;
    virtual void Flush() = 0;

    virtual TextMetrics MeasureText(const std::string& text) = 0;
    virtual TextMetrics MeasureText(const std::string& text, const Font& font) = 0;

    virtual bool IsActive() const = 0;

protected:
    DrawContext() = default;
};

}  
}  

#endif  
