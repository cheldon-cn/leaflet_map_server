#ifndef OGC_DRAW_COLOR_H
#define OGC_DRAW_COLOR_H

#include "ogc/draw/export.h"
#include <cstdint>
#include <string>

namespace ogc {
namespace draw {

class OGC_DRAW_API Color {
public:
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    explicit Color(uint32_t rgba);
    
    uint8_t GetRed() const;
    uint8_t GetGreen() const;
    uint8_t GetBlue() const;
    uint8_t GetAlpha() const;
    
    void SetRed(uint8_t r);
    void SetGreen(uint8_t g);
    void SetBlue(uint8_t b);
    void SetAlpha(uint8_t a);
    
    uint32_t GetARGB() const;
    uint32_t GetRGBA() const;
    uint32_t GetRGB() const;
    
    void SetARGB(uint32_t argb);
    void SetRGBA(uint32_t rgba);
    void SetRGB(uint32_t rgb);
    
    double GetRedF() const;
    double GetGreenF() const;
    double GetBlueF() const;
    double GetAlphaF() const;
    
    void SetRedF(double r);
    void SetGreenF(double g);
    void SetBlueF(double b);
    void SetAlphaF(double a);
    
    void GetHSL(double& h, double& s, double& l) const;
    void SetHSL(double h, double s, double l);
    
    void GetHSV(double& h, double& s, double& v) const;
    void SetHSV(double h, double s, double v);
    
    Color WithAlpha(uint8_t a) const;
    Color WithAlphaF(double a) const;
    
    Color Lighter(double factor = 1.5) const;
    Color Darker(double factor = 0.7) const;
    
    Color ToGrayscale() const;
    Color ToInverted() const;
    
    Color Blend(const Color& other, double ratio) const;
    
    bool IsTransparent() const;
    bool IsOpaque() const;
    bool IsValid() const;
    
    std::string ToString() const;
    std::string ToHexString(bool includeAlpha = false) const;
    std::string ToRGBString() const;
    std::string ToRGBAString() const;
    
    static Color FromHexString(const std::string& hex);
    static Color FromHex(uint32_t hex);
    static Color FromHexWithAlpha(uint32_t hex);
    static Color FromRGB(uint8_t r, uint8_t g, uint8_t b);
    static Color FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static Color FromHSL(double h, double s, double l);
    static Color FromHSV(double h, double s, double v);
    
    uint32_t ToHex() const;
    uint32_t ToHexWithAlpha() const;
    
    static Color Transparent();
    static Color Black();
    static Color White();
    static Color Red();
    static Color Green();
    static Color Blue();
    static Color Yellow();
    static Color Cyan();
    static Color Magenta();
    static Color Gray();
    static Color LightGray();
    static Color DarkGray();
    
    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

private:
    uint8_t m_r;
    uint8_t m_g;
    uint8_t m_b;
    uint8_t m_a;
};

inline uint8_t Color::GetRed() const { return m_r; }
inline uint8_t Color::GetGreen() const { return m_g; }
inline uint8_t Color::GetBlue() const { return m_b; }
inline uint8_t Color::GetAlpha() const { return m_a; }

inline void Color::SetRed(uint8_t r) { m_r = r; }
inline void Color::SetGreen(uint8_t g) { m_g = g; }
inline void Color::SetBlue(uint8_t b) { m_b = b; }
inline void Color::SetAlpha(uint8_t a) { m_a = a; }

inline uint32_t Color::GetARGB() const {
    return (static_cast<uint32_t>(m_a) << 24) |
           (static_cast<uint32_t>(m_r) << 16) |
           (static_cast<uint32_t>(m_g) << 8) |
           static_cast<uint32_t>(m_b);
}

inline uint32_t Color::GetRGBA() const {
    return (static_cast<uint32_t>(m_r) << 24) |
           (static_cast<uint32_t>(m_g) << 16) |
           (static_cast<uint32_t>(m_b) << 8) |
           static_cast<uint32_t>(m_a);
}

inline uint32_t Color::GetRGB() const {
    return (static_cast<uint32_t>(m_r) << 16) |
           (static_cast<uint32_t>(m_g) << 8) |
           static_cast<uint32_t>(m_b);
}

inline void Color::SetARGB(uint32_t argb) {
    m_a = static_cast<uint8_t>((argb >> 24) & 0xFF);
    m_r = static_cast<uint8_t>((argb >> 16) & 0xFF);
    m_g = static_cast<uint8_t>((argb >> 8) & 0xFF);
    m_b = static_cast<uint8_t>(argb & 0xFF);
}

inline void Color::SetRGBA(uint32_t rgba) {
    m_r = static_cast<uint8_t>((rgba >> 24) & 0xFF);
    m_g = static_cast<uint8_t>((rgba >> 16) & 0xFF);
    m_b = static_cast<uint8_t>((rgba >> 8) & 0xFF);
    m_a = static_cast<uint8_t>(rgba & 0xFF);
}

inline void Color::SetRGB(uint32_t rgb) {
    m_r = static_cast<uint8_t>((rgb >> 16) & 0xFF);
    m_g = static_cast<uint8_t>((rgb >> 8) & 0xFF);
    m_b = static_cast<uint8_t>(rgb & 0xFF);
    m_a = 255;
}

inline double Color::GetRedF() const { return m_r / 255.0; }
inline double Color::GetGreenF() const { return m_g / 255.0; }
inline double Color::GetBlueF() const { return m_b / 255.0; }
inline double Color::GetAlphaF() const { return m_a / 255.0; }

inline void Color::SetRedF(double r) { m_r = static_cast<uint8_t>(r * 255.0 + 0.5); }
inline void Color::SetGreenF(double g) { m_g = static_cast<uint8_t>(g * 255.0 + 0.5); }
inline void Color::SetBlueF(double b) { m_b = static_cast<uint8_t>(b * 255.0 + 0.5); }
inline void Color::SetAlphaF(double a) { m_a = static_cast<uint8_t>(a * 255.0 + 0.5); }

inline Color Color::WithAlpha(uint8_t a) const {
    return Color(m_r, m_g, m_b, a);
}

inline Color Color::WithAlphaF(double a) const {
    return Color(m_r, m_g, m_b, static_cast<uint8_t>(a * 255.0 + 0.5));
}

inline bool Color::IsTransparent() const { return m_a == 0; }
inline bool Color::IsOpaque() const { return m_a == 255; }
inline bool Color::IsValid() const { return true; }

inline uint32_t Color::ToHex() const { return GetRGB(); }
inline uint32_t Color::ToHexWithAlpha() const { return GetRGBA(); }

inline Color Color::FromHex(uint32_t hex) {
    return Color(
        static_cast<uint8_t>((hex >> 16) & 0xFF),
        static_cast<uint8_t>((hex >> 8) & 0xFF),
        static_cast<uint8_t>(hex & 0xFF),
        255
    );
}

inline Color Color::FromHexWithAlpha(uint32_t hex) {
    return Color(
        static_cast<uint8_t>((hex >> 24) & 0xFF),
        static_cast<uint8_t>((hex >> 16) & 0xFF),
        static_cast<uint8_t>((hex >> 8) & 0xFF),
        static_cast<uint8_t>(hex & 0xFF)
    );
}

inline Color Color::FromRGB(uint8_t r, uint8_t g, uint8_t b) {
    return Color(r, g, b, 255);
}

inline Color Color::FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return Color(r, g, b, a);
}

inline Color Color::Transparent() { return Color(0, 0, 0, 0); }
inline Color Color::Black() { return Color(0, 0, 0, 255); }
inline Color Color::White() { return Color(255, 255, 255, 255); }
inline Color Color::Red() { return Color(255, 0, 0, 255); }
inline Color Color::Green() { return Color(0, 255, 0, 255); }
inline Color Color::Blue() { return Color(0, 0, 255, 255); }
inline Color Color::Yellow() { return Color(255, 255, 0, 255); }
inline Color Color::Cyan() { return Color(0, 255, 255, 255); }
inline Color Color::Magenta() { return Color(255, 0, 255, 255); }
inline Color Color::Gray() { return Color(128, 128, 128, 255); }
inline Color Color::LightGray() { return Color(192, 192, 192, 255); }
inline Color Color::DarkGray() { return Color(64, 64, 64, 255); }

inline bool Color::operator==(const Color& other) const {
    return m_r == other.m_r && m_g == other.m_g && 
           m_b == other.m_b && m_a == other.m_a;
}

inline bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

}  
}  

#endif  
