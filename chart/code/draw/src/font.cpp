#include "ogc/draw/font.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace draw {

Font::Font()
    : m_family("Arial")
    , m_size(12.0)
    , m_style(FontStyle::kNormal)
    , m_weight(FontWeight::kNormal)
    , m_italic(false)
    , m_underline(false)
    , m_strikethrough(false) {
}

Font::Font(const std::string& family, double size)
    : m_family(family)
    , m_size(size)
    , m_style(FontStyle::kNormal)
    , m_weight(FontWeight::kNormal)
    , m_italic(false)
    , m_underline(false)
    , m_strikethrough(false) {
}

Font::Font(const std::string& family, double size, FontStyle style)
    : m_family(family)
    , m_size(size)
    , m_style(style)
    , m_weight(FontWeight::kNormal)
    , m_italic(style == FontStyle::kItalic || style == FontStyle::kBoldItalic)
    , m_underline(false)
    , m_strikethrough(false) {
}

Font::Font(const std::string& family, double size, FontWeight weight, bool italic)
    : m_family(family)
    , m_size(size)
    , m_style(italic ? FontStyle::kItalic : FontStyle::kNormal)
    , m_weight(weight)
    , m_italic(italic)
    , m_underline(false)
    , m_strikethrough(false) {
}

void Font::SetFamily(const std::string& family) {
    m_family = family;
}

void Font::SetSize(double size) {
    m_size = size;
}

void Font::SetStyle(FontStyle style) {
    m_style = style;
}

void Font::SetWeight(FontWeight weight) {
    m_weight = weight;
}

void Font::SetItalic(bool italic) {
    m_italic = italic;
}

void Font::SetBold(bool bold) {
    if (bold) {
        m_weight = FontWeight::kBold;
    }
}

void Font::SetUnderline(bool underline) {
    m_underline = underline;
}

void Font::SetStrikethrough(bool strikethrough) {
    m_strikethrough = strikethrough;
}

std::string Font::GetStyleString() const {
    switch (m_style) {
        case FontStyle::kNormal:     return "Normal";
        case FontStyle::kBold:       return "Bold";
        case FontStyle::kItalic:     return "Italic";
        case FontStyle::kBoldItalic: return "Bold Italic";
        default:                     return "Normal";
    }
}

std::string Font::GetWeightString() const {
    switch (m_weight) {
        case FontWeight::kThin:       return "Thin";
        case FontWeight::kExtraLight: return "ExtraLight";
        case FontWeight::kLight:      return "Light";
        case FontWeight::kNormal:     return "Normal";
        case FontWeight::kMedium:     return "Medium";
        case FontWeight::kSemiBold:   return "SemiBold";
        case FontWeight::kBold:       return "Bold";
        case FontWeight::kExtraBold:  return "ExtraBold";
        case FontWeight::kBlack:      return "Black";
        default:                      return "Normal";
    }
}

std::string Font::ToString() const {
    std::ostringstream oss;
    oss << m_family << " " << m_size << "pt";
    if (m_weight != FontWeight::kNormal) {
        oss << " " << GetWeightString();
    }
    if (m_italic) {
        oss << " Italic";
    }
    if (m_underline) {
        oss << " Underline";
    }
    if (m_strikethrough) {
        oss << " Strikethrough";
    }
    return oss.str();
}

double Font::GetLineHeight() const {
    return m_size * 1.2;
}

double Font::GetAscent() const {
    return m_size * 0.8;
}

double Font::GetDescent() const {
    return m_size * 0.2;
}

bool Font::IsValid() const {
    return !m_family.empty() && m_size > 0;
}

bool Font::IsEmpty() const {
    return m_family.empty() && m_size == 0;
}

bool Font::IsVisible() const {
    return IsValid() && m_size > 0;
}

void Font::Reset() {
    m_family = "Arial";
    m_size = 12.0;
    m_style = FontStyle::kNormal;
    m_weight = FontWeight::kNormal;
    m_italic = false;
    m_underline = false;
    m_strikethrough = false;
}

Font Font::Default() {
    return Font("Arial", 12.0);
}

Font Font::FromString(const std::string& str) {
    Font font;
    std::istringstream iss(str);
    std::string token;
    
    if (std::getline(iss, token, ' ')) {
        font.m_family = token;
    }
    if (std::getline(iss, token, ' ')) {
        size_t ptPos = token.find("pt");
        if (ptPos != std::string::npos) {
            token = token.substr(0, ptPos);
        }
        try {
            font.m_size = std::stod(token);
        } catch (...) {
            font.m_size = 12.0;
        }
    }
    
    std::string remaining;
    while (std::getline(iss, token, ' ')) {
        if (token == "Bold" || token == "Bold") {
            font.m_weight = FontWeight::kBold;
        } else if (token == "Italic") {
            font.m_italic = true;
        } else if (token == "Underline") {
            font.m_underline = true;
        } else if (token == "Strikethrough") {
            font.m_strikethrough = true;
        }
    }
    
    return font;
}

Font Font::WithFamily(const std::string& family) const {
    Font f = *this;
    f.m_family = family;
    return f;
}

Font Font::WithSize(double size) const {
    Font f = *this;
    f.m_size = size;
    return f;
}

Font Font::WithWeight(FontWeight weight) const {
    Font f = *this;
    f.m_weight = weight;
    return f;
}

Font Font::WithStyle(FontStyle style) const {
    Font f = *this;
    f.m_style = style;
    return f;
}

Font Font::WithItalic(bool italic) const {
    Font f = *this;
    f.m_italic = italic;
    return f;
}

Font Font::WithUnderline(bool underline) const {
    Font f = *this;
    f.m_underline = underline;
    return f;
}

Font Font::WithStrikethrough(bool strikethrough) const {
    Font f = *this;
    f.m_strikethrough = strikethrough;
    return f;
}

bool Font::operator==(const Font& other) const {
    return m_family == other.m_family &&
           m_size == other.m_size &&
           m_style == other.m_style &&
           m_weight == other.m_weight &&
           m_italic == other.m_italic &&
           m_underline == other.m_underline &&
           m_strikethrough == other.m_strikethrough;
}

bool Font::operator!=(const Font& other) const {
    return !(*this == other);
}

}  
}  
