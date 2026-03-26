#ifndef OGC_DRAW_COLOR_H
#define OGC_DRAW_COLOR_H

#include "ogc/draw/export.h"
#include <cstdint>
#include <string>

namespace ogc {
namespace draw {

class OGC_GRAPH_API Color {
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
    
    std::string ToHexString(bool includeAlpha = false) const;
    std::string ToRGBString() const;
    std::string ToRGBAString() const;
    
    static Color FromHexString(const std::string& hex);
    static Color FromRGB(uint8_t r, uint8_t g, uint8_t b);
    static Color FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static Color FromHSL(double h, double s, double l);
    static Color FromHSV(double h, double s, double v);
    
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

}  
}  

#endif  
