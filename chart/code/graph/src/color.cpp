#include "ogc/draw/color.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace ogc {
namespace draw {

Color::Color() : m_r(0), m_g(0), m_b(0), m_a(255) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b), m_a(255) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : m_r(r), m_g(g), m_b(b), m_a(a) {}

Color::Color(uint32_t rgba) {
    m_r = static_cast<uint8_t>((rgba >> 24) & 0xFF);
    m_g = static_cast<uint8_t>((rgba >> 16) & 0xFF);
    m_b = static_cast<uint8_t>((rgba >> 8) & 0xFF);
    m_a = static_cast<uint8_t>(rgba & 0xFF);
}

uint32_t Color::GetARGB() const {
    return (static_cast<uint32_t>(m_a) << 24) |
           (static_cast<uint32_t>(m_r) << 16) |
           (static_cast<uint32_t>(m_g) << 8) |
           static_cast<uint32_t>(m_b);
}

uint32_t Color::GetRGBA() const {
    return (static_cast<uint32_t>(m_r) << 24) |
           (static_cast<uint32_t>(m_g) << 16) |
           (static_cast<uint32_t>(m_b) << 8) |
           static_cast<uint32_t>(m_a);
}

uint32_t Color::GetRGB() const {
    return (static_cast<uint32_t>(m_r) << 16) |
           (static_cast<uint32_t>(m_g) << 8) |
           static_cast<uint32_t>(m_b);
}

void Color::SetARGB(uint32_t argb) {
    m_a = static_cast<uint8_t>((argb >> 24) & 0xFF);
    m_r = static_cast<uint8_t>((argb >> 16) & 0xFF);
    m_g = static_cast<uint8_t>((argb >> 8) & 0xFF);
    m_b = static_cast<uint8_t>(argb & 0xFF);
}

void Color::SetRGBA(uint32_t rgba) {
    m_r = static_cast<uint8_t>((rgba >> 24) & 0xFF);
    m_g = static_cast<uint8_t>((rgba >> 16) & 0xFF);
    m_b = static_cast<uint8_t>((rgba >> 8) & 0xFF);
    m_a = static_cast<uint8_t>(rgba & 0xFF);
}

void Color::SetRGB(uint32_t rgb) {
    m_r = static_cast<uint8_t>((rgb >> 16) & 0xFF);
    m_g = static_cast<uint8_t>((rgb >> 8) & 0xFF);
    m_b = static_cast<uint8_t>(rgb & 0xFF);
    m_a = 255;
}

double Color::GetRedF() const { return m_r / 255.0; }
double Color::GetGreenF() const { return m_g / 255.0; }
double Color::GetBlueF() const { return m_b / 255.0; }
double Color::GetAlphaF() const { return m_a / 255.0; }

void Color::SetRedF(double r) { m_r = static_cast<uint8_t>(std::round(std::max(0.0, std::min(1.0, r)) * 255.0)); }
void Color::SetGreenF(double g) { m_g = static_cast<uint8_t>(std::round(std::max(0.0, std::min(1.0, g)) * 255.0)); }
void Color::SetBlueF(double b) { m_b = static_cast<uint8_t>(std::round(std::max(0.0, std::min(1.0, b)) * 255.0)); }
void Color::SetAlphaF(double a) { m_a = static_cast<uint8_t>(std::round(std::max(0.0, std::min(1.0, a)) * 255.0)); }

void Color::GetHSL(double& h, double& s, double& l) const {
    double r = GetRedF();
    double g = GetGreenF();
    double b = GetBlueF();
    
    double maxVal = std::max({r, g, b});
    double minVal = std::min({r, g, b});
    double delta = maxVal - minVal;
    
    l = (maxVal + minVal) / 2.0;
    
    if (delta < 1e-10) {
        h = 0.0;
        s = 0.0;
        return;
    }
    
    s = l > 0.5 ? delta / (2.0 - maxVal - minVal) : delta / (maxVal + minVal);
    
    if (maxVal == r) {
        h = (g - b) / delta + (g < b ? 6.0 : 0.0);
    } else if (maxVal == g) {
        h = (b - r) / delta + 2.0;
    } else {
        h = (r - g) / delta + 4.0;
    }
    h /= 6.0;
}

void Color::SetHSL(double h, double s, double l) {
    h = std::fmod(h, 1.0);
    if (h < 0) h += 1.0;
    s = std::max(0.0, std::min(1.0, s));
    l = std::max(0.0, std::min(1.0, l));
    
    if (s < 1e-10) {
        SetRedF(l);
        SetGreenF(l);
        SetBlueF(l);
        return;
    }
    
    auto hue2rgb = [](double p, double q, double t) {
        if (t < 0) t += 1.0;
        if (t > 1) t -= 1.0;
        if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
        if (t < 1.0/2.0) return q;
        if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
        return p;
    };
    
    double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
    double p = 2.0 * l - q;
    
    SetRedF(hue2rgb(p, q, h + 1.0/3.0));
    SetGreenF(hue2rgb(p, q, h));
    SetBlueF(hue2rgb(p, q, h - 1.0/3.0));
}

void Color::GetHSV(double& h, double& s, double& v) const {
    double r = GetRedF();
    double g = GetGreenF();
    double b = GetBlueF();
    
    double maxVal = std::max({r, g, b});
    double minVal = std::min({r, g, b});
    double delta = maxVal - minVal;
    
    v = maxVal;
    
    if (delta < 1e-10) {
        h = 0.0;
        s = 0.0;
        return;
    }
    
    s = maxVal > 1e-10 ? delta / maxVal : 0.0;
    
    if (maxVal == r) {
        h = (g - b) / delta + (g < b ? 6.0 : 0.0);
    } else if (maxVal == g) {
        h = (b - r) / delta + 2.0;
    } else {
        h = (r - g) / delta + 4.0;
    }
    h /= 6.0;
}

void Color::SetHSV(double h, double s, double v) {
    h = std::fmod(h, 1.0);
    if (h < 0) h += 1.0;
    s = std::max(0.0, std::min(1.0, s));
    v = std::max(0.0, std::min(1.0, v));
    
    int i = static_cast<int>(h * 6.0);
    double f = h * 6.0 - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - f * s);
    double t = v * (1.0 - (1.0 - f) * s);
    
    switch (i % 6) {
        case 0: SetRedF(v); SetGreenF(t); SetBlueF(p); break;
        case 1: SetRedF(q); SetGreenF(v); SetBlueF(p); break;
        case 2: SetRedF(p); SetGreenF(v); SetBlueF(t); break;
        case 3: SetRedF(p); SetGreenF(q); SetBlueF(v); break;
        case 4: SetRedF(t); SetGreenF(p); SetBlueF(v); break;
        case 5: SetRedF(v); SetGreenF(p); SetBlueF(q); break;
    }
}

Color Color::WithAlpha(uint8_t a) const {
    return Color(m_r, m_g, m_b, a);
}

Color Color::WithAlphaF(double a) const {
    return Color(m_r, m_g, m_b, static_cast<uint8_t>(std::round(std::max(0.0, std::min(1.0, a)) * 255.0)));
}

Color Color::Lighter(double factor) const {
    double h, s, l;
    GetHSL(h, s, l);
    l = std::min(1.0, l * factor);
    Color result;
    result.SetHSL(h, s, l);
    result.SetAlpha(m_a);
    return result;
}

Color Color::Darker(double factor) const {
    double h, s, l;
    GetHSL(h, s, l);
    l = std::max(0.0, l * factor);
    Color result;
    result.SetHSL(h, s, l);
    result.SetAlpha(m_a);
    return result;
}

Color Color::ToGrayscale() const {
    uint8_t gray = static_cast<uint8_t>(std::round(0.299 * m_r + 0.587 * m_g + 0.114 * m_b));
    return Color(gray, gray, gray, m_a);
}

Color Color::ToInverted() const {
    return Color(255 - m_r, 255 - m_g, 255 - m_b, m_a);
}

Color Color::Blend(const Color& other, double ratio) const {
    ratio = std::max(0.0, std::min(1.0, ratio));
    double invRatio = 1.0 - ratio;
    
    return Color(
        static_cast<uint8_t>(std::round(m_r * invRatio + other.m_r * ratio)),
        static_cast<uint8_t>(std::round(m_g * invRatio + other.m_g * ratio)),
        static_cast<uint8_t>(std::round(m_b * invRatio + other.m_b * ratio)),
        static_cast<uint8_t>(std::round(m_a * invRatio + other.m_a * ratio))
    );
}

bool Color::IsTransparent() const { return m_a == 0; }
bool Color::IsOpaque() const { return m_a == 255; }
bool Color::IsValid() const { return true; }

std::string Color::ToHexString(bool includeAlpha) const {
    std::ostringstream oss;
    oss << "#" << std::hex << std::setfill('0');
    if (includeAlpha) {
        oss << std::setw(2) << static_cast<int>(m_r)
            << std::setw(2) << static_cast<int>(m_g)
            << std::setw(2) << static_cast<int>(m_b)
            << std::setw(2) << static_cast<int>(m_a);
    } else {
        oss << std::setw(2) << static_cast<int>(m_r)
            << std::setw(2) << static_cast<int>(m_g)
            << std::setw(2) << static_cast<int>(m_b);
    }
    return oss.str();
}

std::string Color::ToRGBString() const {
    std::ostringstream oss;
    oss << "rgb(" << static_cast<int>(m_r) << ", " 
        << static_cast<int>(m_g) << ", " 
        << static_cast<int>(m_b) << ")";
    return oss.str();
}

std::string Color::ToRGBAString() const {
    std::ostringstream oss;
    oss << "rgba(" << static_cast<int>(m_r) << ", " 
        << static_cast<int>(m_g) << ", " 
        << static_cast<int>(m_b) << ", " 
        << GetAlphaF() << ")";
    return oss.str();
}

Color Color::FromHexString(const std::string& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') {
        h = h.substr(1);
    }
    
    if (h.length() == 6) {
        uint32_t rgb = static_cast<uint32_t>(std::strtoul(h.c_str(), nullptr, 16));
        return Color(static_cast<uint8_t>((rgb >> 16) & 0xFF),
                     static_cast<uint8_t>((rgb >> 8) & 0xFF),
                     static_cast<uint8_t>(rgb & 0xFF));
    } else if (h.length() == 8) {
        uint32_t rgba = static_cast<uint32_t>(std::strtoul(h.c_str(), nullptr, 16));
        return Color(static_cast<uint8_t>((rgba >> 24) & 0xFF),
                     static_cast<uint8_t>((rgba >> 16) & 0xFF),
                     static_cast<uint8_t>((rgba >> 8) & 0xFF),
                     static_cast<uint8_t>(rgba & 0xFF));
    }
    return Color();
}

Color Color::FromRGB(uint8_t r, uint8_t g, uint8_t b) {
    return Color(r, g, b);
}

Color Color::FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return Color(r, g, b, a);
}

Color Color::FromHSL(double h, double s, double l) {
    Color c;
    c.SetHSL(h, s, l);
    return c;
}

Color Color::FromHSV(double h, double s, double v) {
    Color c;
    c.SetHSV(h, s, v);
    return c;
}

Color Color::Transparent() { return Color(0, 0, 0, 0); }
Color Color::Black() { return Color(0, 0, 0); }
Color Color::White() { return Color(255, 255, 255); }
Color Color::Red() { return Color(255, 0, 0); }
Color Color::Green() { return Color(0, 255, 0); }
Color Color::Blue() { return Color(0, 0, 255); }
Color Color::Yellow() { return Color(255, 255, 0); }
Color Color::Cyan() { return Color(0, 255, 255); }
Color Color::Magenta() { return Color(255, 0, 255); }
Color Color::Gray() { return Color(128, 128, 128); }
Color Color::LightGray() { return Color(192, 192, 192); }
Color Color::DarkGray() { return Color(64, 64, 64); }

bool Color::operator==(const Color& other) const {
    return m_r == other.m_r && m_g == other.m_g && m_b == other.m_b && m_a == other.m_a;
}

bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

}  
}  
