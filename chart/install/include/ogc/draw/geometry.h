#ifndef OGC_DRAW_GEOMETRY_H
#define OGC_DRAW_GEOMETRY_H

#include <ogc/draw/export.h>
#include <ogc/draw/draw_types.h>
#include <ogc/draw/transform_matrix.h>
#include <ogc/polygon.h>
#include <ogc/point.h>
#include <ogc/linestring.h>
#include <vector>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API RectGeometry : public ogc::Polygon {
public:
    static std::unique_ptr<RectGeometry> Create();
    static std::unique_ptr<RectGeometry> Create(const Rect& rect);
    static std::unique_ptr<RectGeometry> Create(double x, double y, double w, double h);
    
    RectGeometry();
    explicit RectGeometry(const Rect& rect);
    RectGeometry(double x, double y, double w, double h);
    
    const char* GetGeometryTypeString() const noexcept override { return "RECT"; }
    
    Rect GetRect() const { return m_rect; }
    void SetRect(const Rect& rect);
    
    void TransformByMatrix(const TransformMatrix& matrix);

    GeometryPtr Clone() const override;

private:
    void UpdatePolygon();
    Rect m_rect;
};

class OGC_DRAW_API CircleGeometry : public ogc::Polygon {
public:
    static std::unique_ptr<CircleGeometry> Create();
    static std::unique_ptr<CircleGeometry> Create(const Point& center, double radius, int segments = 32);
    static std::unique_ptr<CircleGeometry> Create(double cx, double cy, double radius, int segments = 32);
    
    CircleGeometry();
    CircleGeometry(const Point& center, double radius, int segments = 32);
    CircleGeometry(double cx, double cy, double radius, int segments = 32);
    
    const char* GetGeometryTypeString() const noexcept override { return "CIRCLE"; }
    
    Point GetCenter() const { return m_center; }
    void SetCenter(const Point& center);
    
    double GetRadius() const { return m_radius; }
    void SetRadius(double radius);
    
    int GetSegments() const { return m_segments; }
    void SetSegments(int segments);
    
    void TransformByMatrix(const TransformMatrix& matrix);

    GeometryPtr Clone() const override;

private:
    void UpdatePolygon();
    Point m_center;
    double m_radius;
    int m_segments;
};

class OGC_DRAW_API EllipseGeometry : public ogc::Polygon {
public:
    static std::unique_ptr<EllipseGeometry> Create();
    static std::unique_ptr<EllipseGeometry> Create(const Point& center, double rx, double ry, int segments = 32);
    static std::unique_ptr<EllipseGeometry> Create(double cx, double cy, double rx, double ry, int segments = 32);
    
    EllipseGeometry();
    EllipseGeometry(const Point& center, double rx, double ry, int segments = 32);
    EllipseGeometry(double cx, double cy, double rx, double ry, int segments = 32);
    
    const char* GetGeometryTypeString() const noexcept override { return "ELLIPSE"; }
    
    Point GetCenter() const { return m_center; }
    void SetCenter(const Point& center);
    
    double GetRadiusX() const { return m_rx; }
    void SetRadiusX(double rx);
    
    double GetRadiusY() const { return m_ry; }
    void SetRadiusY(double ry);
    
    int GetSegments() const { return m_segments; }
    void SetSegments(int segments);
    
    void TransformByMatrix(const TransformMatrix& matrix);

    GeometryPtr Clone() const override;

private:
    void UpdatePolygon();
    Point m_center;
    double m_rx;
    double m_ry;
    int m_segments;
};

}  
}  

#endif
