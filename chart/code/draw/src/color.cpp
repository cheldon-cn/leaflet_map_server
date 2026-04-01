#include "ogc/draw/color.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ogc {
namespace draw {

Color::Color() : m_r(0), m_g(0), m_b(0), m_a(255) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b) 
    : m_r(r), m_g(g), m_b(b), m_a(255) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
    : m_r(r), m_g(g), m_b(b), m_a(a) {}

Color::Color(uint32_t rgba) {
    m_r = static_cast<uint8_t>((rgba >> 24) & 0xFF);
    m_g = static_cast<uint8_t>((rgba >> 16) & 0xFF);
    m_b = static_cast<uint8_t>((rgba >> 8) & 0xFF);
    m_a = static_cast<uint8_t>(rgba & 0xFF);
}

void Color::GetHSL(double& h, double& s, double& l) const {
    double rn = m_r / 255.0;
    double gn = m_g / 255.0;
    double bn = m_b / 255.0;

    double maxVal = std::max({rn, gn, bn});
    double minVal = std::min({rn, gn, bn});
    double delta = maxVal - minVal;

    l = (maxVal + minVal) / 2.0;

    if (delta < 1e-10) {
        h = 0;
        s = 0;
        return;
    }

    s = l > 0.5 ? delta / (2.0 - maxVal - minVal) : delta / (maxVal + minVal);

    if (maxVal == rn) {
        h = 60.0 * std::fmod((gn - bn) / delta, 6.0);
    } else if (maxVal == gn) {
        h = 60.0 * ((bn - rn) / delta + 2.0);
    } else {
        h = 60.0 * ((rn - gn) / delta + 4.0);
    }

    if (h < 0) h += 360.0;
}

void Color::SetHSL(double h, double s, double l) {
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;
    s = std::max(0.0, std::min(1.0, s));
    l = std::max(0.0, std::min(1.0, l));

    auto hue2rgb = [](double p, double q, double t) {
        if (t < 0) t += 1.0;
        if (t > 1) t -= 1.0;
        if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
        if (t < 1.0/2.0) return q;
        if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
        return p;
    };

    if (s == 0) {
        m_r = m_g = m_b = static_cast<uint8_t>(l * 255.0 + 0.5);
    } else {
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        m_r = static_cast<uint8_t>(hue2rgb(p, q, h / 360.0 + 1.0/3.0) * 255.0 + 0.5);
        m_g = static_cast<uint8_t>(hue2rgb(p, q, h / 360.0) * 255.0 + 0.5);
        m_b = static_cast<uint8_t>(hue2rgb(p, q, h / 360.0 - 1.0/3.0) * 255.0 + 0.5);
    }
}

void Color::GetHSV(double& h, double& s, double& v) const {
    double rn = m_r / 255.0;
    double gn = m_g / 255.0;
    double bn = m_b / 255.0;

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

void Color::SetHSV(double h, double s, double v) {
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

    m_r = static_cast<uint8_t>((r1 + m) * 255.0 + 0.5);
    m_g = static_cast<uint8_t>((g1 + m) * 255.0 + 0.5);
    m_b = static_cast<uint8_t>((b1 + m) * 255.0 + 0.5);
}

Color Color::Lighter(double factor) const {
    if (factor <= 0) return *this;
    
    double h, s, l;
    GetHSL(h, s, l);
    
    l = std::min(1.0, l * factor);
    
    Color result;
    result.SetHSL(h, s, l);
    result.m_a = m_a;
    return result;
}

Color Color::Darker(double factor) const {
    if (factor <= 0) return *this;
    
    double h, s, l;
    GetHSL(h, s, l);
    
    l = l * factor;
    
    Color result;
    result.SetHSL(h, s, l);
    result.m_a = m_a;
    return result;
}

Color Color::ToGrayscale() const {
    uint8_t gray = static_cast<uint8_t>(0.299 * m_r + 0.587 * m_g + 0.114 * m_b + 0.5);
    return Color(gray, gray, gray, m_a);
}

Color Color::ToInverted() const {
    return Color(255 - m_r, 255 - m_g, 255 - m_b, m_a);
}

Color Color::Blend(const Color& other, double ratio) const {
    ratio = std::max(0.0, std::min(1.0, ratio));
    double invRatio = 1.0 - ratio;
    return Color(
        static_cast<uint8_t>(m_r * invRatio + other.m_r * ratio + 0.5),
        static_cast<uint8_t>(m_g * invRatio + other.m_g * ratio + 0.5),
        static_cast<uint8_t>(m_b * invRatio + other.m_b * ratio + 0.5),
        static_cast<uint8_t>(m_a * invRatio + other.m_a * ratio + 0.5)
    );
}

std::string Color::ToString() const {
    return "Color(" + std::to_string(m_r) + ", " + 
           std::to_string(m_g) + ", " + 
           std::to_string(m_b) + ", " + 
           std::to_string(m_a) + ")";
}

std::string Color::ToHexString(bool includeAlpha) const {
    std::ostringstream oss;
    oss << "#";
    if (includeAlpha) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(m_a);
    }
    oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(m_r)
        << std::setw(2) << static_cast<int>(m_g)
        << std::setw(2) << static_cast<int>(m_b);
    return oss.str();
}

std::string Color::ToRGBString() const {
    return "rgb(" + std::to_string(m_r) + ", " + 
           std::to_string(m_g) + ", " + 
           std::to_string(m_b) + ")";
}

std::string Color::ToRGBAString() const {
    return "rgba(" + std::to_string(m_r) + ", " + 
           std::to_string(m_g) + ", " + 
           std::to_string(m_b) + ", " + 
           std::to_string(static_cast<double>(m_a) / 255.0) + ")";
}

Color Color::FromHexString(const std::string& hex) {
    std::string cleanHex = hex;
    
    if (!cleanHex.empty() && cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1);
    }
    
    if (cleanHex.length() == 6) {
        uint32_t value = std::stoul(cleanHex, nullptr, 16);
        return FromHex(value);
    } else if (cleanHex.length() == 8) {
        uint8_t a = static_cast<uint8_t>(std::stoul(cleanHex.substr(0, 2), nullptr, 16));
        uint8_t r = static_cast<uint8_t>(std::stoul(cleanHex.substr(2, 2), nullptr, 16));
        uint8_t g = static_cast<uint8_t>(std::stoul(cleanHex.substr(4, 2), nullptr, 16));
        uint8_t b = static_cast<uint8_t>(std::stoul(cleanHex.substr(6, 2), nullptr, 16));
        return Color(r, g, b, a);
    } else if (cleanHex.length() == 3) {
        uint8_t r = static_cast<uint8_t>(std::stoul(cleanHex.substr(0, 1), nullptr, 16) * 17);
        uint8_t g = static_cast<uint8_t>(std::stoul(cleanHex.substr(1, 1), nullptr, 16) * 17);
        uint8_t b = static_cast<uint8_t>(std::stoul(cleanHex.substr(2, 1), nullptr, 16) * 17);
        return Color(r, g, b, 255);
    }
    
    return Color();
}

Color Color::FromHSL(double h, double s, double l) {
    Color result;
    result.SetHSL(h, s, l);
    return result;
}

Color Color::FromHSV(double h, double s, double v) {
    Color result;
    result.SetHSV(h, s, v);
    return result;
}

}  
}  
