#ifndef OGC_DRAW_TYPES_H
#define OGC_DRAW_TYPES_H

#include <cstdint>
#include <string>
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color() : r(0), g(0), b(0), a(255) {}
    
    Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    static Color FromHex(uint32_t hex) {
        return Color(
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            255
        );
    }

    static Color FromHexWithAlpha(uint32_t hex) {
        return Color(
            static_cast<uint8_t>((hex >> 24) & 0xFF),
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF)
        );
    }

    static Color FromHSV(double h, double s, double v) {
        h = std::fmod(h, 360.0);
        if (h < 0) h += 360.0;
        s = std::max(0.0, std::min(1.0, s));
        v = std::max(0.0, std::min(1.0, v));

        double c = v * s;
        double x = c * (1 - std::abs(std::fmod(h / 60.0, 2.0) - 1));
        double m = v - c;

        double r1, g1, b1;
        if (h < 60) {
            r1 = c; g1 = x; b1 = 0;
        } else if (h < 120) {
            r1 = x; g1 = c; b1 = 0;
        } else if (h < 180) {
            r1 = 0; g1 = c; b1 = x;
        } else if (h < 240) {
            r1 = 0; g1 = x; b1 = c;
        } else if (h < 300) {
            r1 = x; g1 = 0; b1 = c;
        } else {
            r1 = c; g1 = 0; b1 = x;
        }

        return Color(
            static_cast<uint8_t>((r1 + m) * 255),
            static_cast<uint8_t>((g1 + m) * 255),
            static_cast<uint8_t>((b1 + m) * 255),
            255
        );
    }

    static Color FromRGB(uint8_t r, uint8_t g, uint8_t b) {
        return Color(r, g, b, 255);
    }

    static Color FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        return Color(r, g, b, a);
    }

    static Color Transparent() { return Color(0, 0, 0, 0); }
    static Color Black() { return Color(0, 0, 0, 255); }
    static Color White() { return Color(255, 255, 255, 255); }
    static Color Red() { return Color(255, 0, 0, 255); }
    static Color Green() { return Color(0, 255, 0, 255); }
    static Color Blue() { return Color(0, 0, 255, 255); }
    static Color Yellow() { return Color(255, 255, 0, 255); }
    static Color Cyan() { return Color(0, 255, 255, 255); }
    static Color Magenta() { return Color(255, 0, 255, 255); }

    uint32_t ToHex() const {
        return (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) |
               static_cast<uint32_t>(b);
    }

    uint32_t ToHexWithAlpha() const {
        return (static_cast<uint32_t>(r) << 24) |
               (static_cast<uint32_t>(g) << 16) |
               (static_cast<uint32_t>(b) << 8) |
               static_cast<uint32_t>(a);
    }

    void ToHSV(double& h, double& s, double& v) const {
        double rn = r / 255.0;
        double gn = g / 255.0;
        double bn = b / 255.0;

        double maxVal = std::max({rn, gn, bn});
        double minVal = std::min({rn, gn, bn});
        double delta = maxVal - minVal;

        v = maxVal;

        if (delta < 1e-10) {
            h = 0;
            s = 0;
            return;
        }

        s = delta / maxVal;

        if (maxVal == rn) {
            h = 60.0 * std::fmod((gn - bn) / delta, 6.0);
        } else if (maxVal == gn) {
            h = 60.0 * ((bn - rn) / delta + 2.0);
        } else {
            h = 60.0 * ((rn - gn) / delta + 4.0);
        }

        if (h < 0) h += 360.0;
    }

    std::string ToString() const {
        return "Color(" + std::to_string(r) + ", " + 
               std::to_string(g) + ", " + 
               std::to_string(b) + ", " + 
               std::to_string(a) + ")";
    }

    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && 
               b == other.b && a == other.a;
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }

    Color WithAlpha(uint8_t newAlpha) const {
        return Color(r, g, b, newAlpha);
    }

    Color Blend(const Color& other, double factor) const {
        factor = std::max(0.0, std::min(1.0, factor));
        double invFactor = 1.0 - factor;
        return Color(
            static_cast<uint8_t>(r * invFactor + other.r * factor),
            static_cast<uint8_t>(g * invFactor + other.g * factor),
            static_cast<uint8_t>(b * invFactor + other.b * factor),
            static_cast<uint8_t>(a * invFactor + other.a * factor)
        );
    }
};

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
