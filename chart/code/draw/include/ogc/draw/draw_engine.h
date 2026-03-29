#ifndef OGC_DRAW_ENGINE_H
#define OGC_DRAW_ENGINE_H

#include "ogc/draw/engine_type.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_types.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/geometry.h"
#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace ogc {
namespace draw {

class DrawDevice;

struct Image {
    int width;
    int height;
    int channels;
    std::vector<uint8_t> data;

    Image() : width(0), height(0), channels(0) {}
    
    Image(int w, int h, int c)
        : width(w), height(h), channels(c) {
        data.resize(static_cast<size_t>(w) * h * c);
    }

    bool IsValid() const {
        return width > 0 && height > 0 && channels > 0 && !data.empty();
    }

    size_t GetDataSize() const {
        return data.size();
    }

    uint8_t* GetData() { return data.empty() ? nullptr : data.data(); }
    const uint8_t* GetData() const { return data.empty() ? nullptr : data.data(); }
};

struct TextMetrics {
    double width;
    double height;
    double ascent;
    double descent;
    double lineHeight;

    TextMetrics() : width(0), height(0), ascent(0), descent(0), lineHeight(0) {}
};

struct Region {
    std::vector<Rect> rects;

    Region() {}

    explicit Region(const Rect& rect) {
        rects.push_back(rect);
    }

    bool IsEmpty() const {
        return rects.empty();
    }

    void AddRect(const Rect& rect) {
        if (!rect.IsEmpty()) {
            rects.push_back(rect);
        }
    }

    void Clear() {
        rects.clear();
    }

    Rect GetBounds() const {
        if (rects.empty()) {
            return Rect();
        }

        Rect result = rects[0];
        for (size_t i = 1; i < rects.size(); ++i) {
            result = result.Union(rects[i]);
        }
        return result;
    }

    bool Contains(const Point& pt) const {
        for (const auto& rect : rects) {
            if (rect.Contains(pt)) {
                return true;
            }
        }
        return false;
    }
};

enum class StateFlag : uint32_t {
    kNone = 0,
    kTransform = 1 << 0,
    kStyle = 1 << 1,
    kOpacity = 1 << 2,
    kClip = 1 << 3,
    kFont = 1 << 4,
    kAll = 0xFFFFFFFF
};

using StateFlags = uint32_t;

class DrawEngine {
public:
    virtual ~DrawEngine() = default;

    virtual std::string GetName() const = 0;
    virtual EngineType GetType() const = 0;
    virtual DrawDevice* GetDevice() const = 0;

    virtual DrawResult Begin() = 0;
    virtual void End() = 0;
    virtual bool IsActive() const = 0;

    virtual DrawResult DrawPoints(const double* x, const double* y, int count,
                                   const DrawStyle& style) = 0;
    virtual DrawResult DrawLines(const double* x1, const double* y1,
                                  const double* x2, const double* y2, int count,
                                  const DrawStyle& style) = 0;
    virtual DrawResult DrawLineString(const double* x, const double* y, int count,
                                       const DrawStyle& style) = 0;
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count,
                                    const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawRect(double x, double y, double w, double h,
                                 const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius,
                                   const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawEllipse(double cx, double cy, double rx, double ry,
                                    const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawArc(double cx, double cy, double rx, double ry,
                                double startAngle, double sweepAngle,
                                const DrawStyle& style) = 0;
    virtual DrawResult DrawText(double x, double y, const std::string& text,
                                 const Font& font, const Color& color) = 0;
    virtual DrawResult DrawImage(double x, double y, const Image& image,
                                  double scaleX = 1.0, double scaleY = 1.0) = 0;
    virtual DrawResult DrawImageRect(double x, double y, double w, double h,
                                      const Image& image) = 0;
    virtual DrawResult DrawGeometry(const Geometry* geometry,
                                     const DrawStyle& style) = 0;

    virtual void SetTransform(const TransformMatrix& matrix) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;

    virtual void SetClipRect(double x, double y, double w, double h) = 0;
    virtual void SetClipRegion(const Region& region) = 0;
    virtual void ResetClip() = 0;
    virtual Region GetClipRegion() const = 0;

    virtual void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) = 0;
    virtual void Restore() = 0;

    virtual void SetOpacity(double opacity) = 0;
    virtual double GetOpacity() const = 0;

    virtual void SetTolerance(double tolerance) {
        (void)tolerance;
    }
    virtual double GetTolerance() const {
        return 0.0;
    }

    virtual void SetAntialiasingEnabled(bool enabled) {
        (void)enabled;
    }
    virtual bool IsAntialiasingEnabled() const {
        return true;
    }

    virtual TextMetrics MeasureText(const std::string& text, const Font& font) = 0;

    virtual void Clear(const Color& color) = 0;
    virtual void Flush() = 0;

    virtual bool SupportsFeature(const std::string& featureName) const {
        (void)featureName;
        return false;
    }
};

}  
}  

#endif  
