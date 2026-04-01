#ifndef OGC_DRAW_GEOMETRY_TYPES_H
#define OGC_DRAW_GEOMETRY_TYPES_H

#include <cstdint>
#include <string>
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

struct Point {
    double x;
    double y;

    Point() : x(0.0), y(0.0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}

    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < 1e-10 && 
               std::abs(y - other.y) < 1e-10;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }

    Point operator*(double scale) const {
        return Point(x * scale, y * scale);
    }

    double DistanceTo(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    std::string ToString() const {
        return "Point(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

struct Size {
    double w;
    double h;

    Size() : w(0.0), h(0.0) {}
    Size(double w_, double h_) : w(w_), h(h_) {}

    bool operator==(const Size& other) const {
        return std::abs(w - other.w) < 1e-10 && 
               std::abs(h - other.h) < 1e-10;
    }

    bool operator!=(const Size& other) const {
        return !(*this == other);
    }

    bool IsEmpty() const {
        return w <= 0 || h <= 0;
    }

    double GetArea() const {
        return w * h;
    }

    std::string ToString() const {
        return "Size(" + std::to_string(w) + ", " + std::to_string(h) + ")";
    }
};

struct Rect {
    double x;
    double y;
    double w;
    double h;

    Rect() : x(0.0), y(0.0), w(0.0), h(0.0) {}
    
    Rect(double x_, double y_, double w_, double h_)
        : x(x_), y(y_), w(w_), h(h_) {}

    static Rect FromLTRB(double left, double top, double right, double bottom) {
        return Rect(left, top, right - left, bottom - top);
    }

    static Rect FromPoints(const Point& p1, const Point& p2) {
        double minX = std::min(p1.x, p2.x);
        double maxX = std::max(p1.x, p2.x);
        double minY = std::min(p1.y, p2.y);
        double maxY = std::max(p1.y, p2.y);
        return Rect(minX, minY, maxX - minX, maxY - minY);
    }

    double GetLeft() const { return x; }
    double GetTop() const { return y; }
    double GetRight() const { return x + w; }
    double GetBottom() const { return y + h; }

    Point GetTopLeft() const { return Point(x, y); }
    Point GetTopRight() const { return Point(x + w, y); }
    Point GetBottomLeft() const { return Point(x, y + h); }
    Point GetBottomRight() const { return Point(x + w, y + h); }
    Point GetCenter() const { return Point(x + w / 2.0, y + h / 2.0); }

    Size GetSize() const { return Size(w, h); }

    bool IsEmpty() const {
        return w <= 0 || h <= 0;
    }

    bool Contains(const Point& pt) const {
        return pt.x >= x && pt.x <= x + w &&
               pt.y >= y && pt.y <= y + h;
    }

    bool Contains(const Rect& other) const {
        return other.x >= x && other.y >= y &&
               other.x + other.w <= x + w &&
               other.y + other.h <= y + h;
    }

    bool Intersects(const Rect& other) const {
        return x < other.x + other.w &&
               x + w > other.x &&
               y < other.y + other.h &&
               y + h > other.y;
    }

    Rect Intersect(const Rect& other) const {
        double left = std::max(x, other.x);
        double top = std::max(y, other.y);
        double right = std::min(x + w, other.x + other.w);
        double bottom = std::min(y + h, other.y + other.h);

        if (left < right && top < bottom) {
            return Rect(left, top, right - left, bottom - top);
        }
        return Rect();
    }

    Rect Union(const Rect& other) const {
        if (IsEmpty()) return other;
        if (other.IsEmpty()) return *this;

        double left = std::min(x, other.x);
        double top = std::min(y, other.y);
        double right = std::max(x + w, other.x + other.w);
        double bottom = std::max(y + h, other.y + other.h);

        return Rect(left, top, right - left, bottom - top);
    }

    Rect Inflate(double dx, double dy) const {
        return Rect(x - dx, y - dy, w + 2 * dx, h + 2 * dy);
    }

    Rect Offset(double dx, double dy) const {
        return Rect(x + dx, y + dy, w, h);
    }

    bool operator==(const Rect& other) const {
        return std::abs(x - other.x) < 1e-10 &&
               std::abs(y - other.y) < 1e-10 &&
               std::abs(w - other.w) < 1e-10 &&
               std::abs(h - other.h) < 1e-10;
    }

    bool operator!=(const Rect& other) const {
        return !(*this == other);
    }

    std::string ToString() const {
        return "Rect(" + std::to_string(x) + ", " + std::to_string(y) + 
               ", " + std::to_string(w) + ", " + std::to_string(h) + ")";
    }
};

}  
}  

#endif  
