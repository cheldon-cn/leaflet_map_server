#ifndef CYCLE_RENDERER_RENDER_CONTEXT_H
#define CYCLE_RENDERER_RENDER_CONTEXT_H

#include <vector>
#include <stack>
#include <string>
#include <memory>
#include "../config/config.h"

namespace cycle {
namespace renderer {

struct PointD {
    double x;
    double y;
    
    PointD() : x(0), y(0) {}
    PointD(double x_, double y_) : x(x_), y(y_) {}
};

struct Transform2D {
    double m11, m12, m21, m22, dx, dy;
    
    Transform2D() : m11(1), m12(0), m21(0), m22(1), dx(0), dy(0) {}
    
    Transform2D(double m11_, double m12_, double m21_, double m22_, 
                double dx_, double dy_)
        : m11(m11_), m12(m12_), m21(m21_), m22(m22_), dx(dx_), dy(dy_) {}
    
    static Transform2D Identity() {
        return Transform2D(1, 0, 0, 1, 0, 0);
    }
    
    static Transform2D Translate(double dx, double dy) {
        return Transform2D(1, 0, 0, 1, dx, dy);
    }
    
    static Transform2D Scale(double sx, double sy) {
        return Transform2D(sx, 0, 0, sy, 0, 0);
    }
    
    PointD Map(double x, double y) const {
        return PointD(m11 * x + m21 * y + dx, m12 * x + m22 * y + dy);
    }
    
    Transform2D operator*(const Transform2D& other) const {
        return Transform2D(
            m11 * other.m11 + m12 * other.m21,
            m11 * other.m12 + m12 * other.m22,
            m21 * other.m11 + m22 * other.m21,
            m21 * other.m12 + m22 * other.m22,
            m11 * other.dx + m12 * other.dy + dx,
            m21 * other.dx + m22 * other.dy + dy
        );
    }
};

struct Style {
    std::string fill_color = "#000000";
    std::string stroke_color = "#000000";
    double stroke_width = 1.0;
    double opacity = 1.0;
    
    bool IsValid() const {
        return !fill_color.empty() && !stroke_color.empty() &&
               stroke_width >= 0 && opacity >= 0 && opacity <= 1;
    }
};

class RenderContext {
public:
    RenderContext(int width, int height, const BoundingBox& bounds);
    ~RenderContext();
    
    void PushState();
    void PopState();
    
    void SetTransform(const Transform2D& transform);
    Transform2D GetTransform() const;
    
    PointD ToScreen(double x, double y) const;
    PointD ToWorld(int x, int y) const;
    
    void SetStyle(const Style& style);
    const Style& GetStyle() const;
    
    bool IsValid() const;
    
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    const BoundingBox& GetBounds() const { return bounds_; }
    
    bool ValidatePoint(double x, double y) const;
    bool ValidateScreenPoint(int x, int y) const;
    
private:
    void CalculateWorldToScreenTransform();
    
    int width_;
    int height_;
    BoundingBox bounds_;
    Transform2D transform_;
    std::stack<Transform2D> transformStack_;
    Style currentStyle_;
    std::stack<Style> styleStack_;
    
    Transform2D worldToScreen_;
    Transform2D screenToWorld_;
};

} // namespace renderer
} // namespace cycle

#endif // CYCLE_RENDERER_RENDER_CONTEXT_H