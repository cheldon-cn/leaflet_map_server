#include "ogc/draw/font.h"
#include <sstream>

namespace ogc {
namespace draw {

Font::Font()
    : m_family("Arial")
    , m_size(12.0)
    , m_style(FontStyle::kNormal)
    , m_weight(FontWeight::kNormal)
    , m_italic(false) {
}

Font::Font(const std::string& family, double size)
    : m_family(family)
    , m_size(size)
    , m_style(FontStyle::kNormal)
    , m_weight(FontWeight::kNormal)
    , m_italic(false) {
}

Font::Font(const std::string& family, double size, FontStyle style)
    : m_family(family)
    , m_size(size)
    , m_style(style)
    , m_weight(FontWeight::kNormal)
    , m_italic(style == FontStyle::kItalic || style == FontStyle::kBoldItalic) {
    if (style == FontStyle::kBold || style == FontStyle::kBoldItalic) {
        m_weight = FontWeight::kBold;
    }
}

Font::Font(const std::string& family, double size, FontWeight weight, bool italic)
    : m_family(family)
    , m_size(size)
    , m_style(FontStyle::kNormal)
    , m_weight(weight)
    , m_italic(italic) {
    if (weight == FontWeight::kBold || weight == FontWeight::kExtraBold) {
        m_style = italic ? FontStyle::kBoldItalic : FontStyle::kBold;
    } else if (italic) {
        m_style = FontStyle::kItalic;
    }
}

void Font::SetFamily(const std::string& family) {
    m_family = family;
}

void Font::SetSize(double size) {
    m_size = size;
}

void Font::SetStyle(FontStyle style) {
    m_style = style;
    m_italic = (style == FontStyle::kItalic || style == FontStyle::kBoldItalic);
    if (style == FontStyle::kBold || style == FontStyle::kBoldItalic) {
        m_weight = FontWeight::kBold;
    } else if (m_weight == FontWeight::kBold) {
        m_weight = FontWeight::kNormal;
    }
}

void Font::SetWeight(FontWeight weight) {
    m_weight = weight;
    if (weight == FontWeight::kBold || weight == FontWeight::kExtraBold) {
        m_style = m_italic ? FontStyle::kBoldItalic : FontStyle::kBold;
    } else {
        m_style = m_italic ? FontStyle::kItalic : FontStyle::kNormal;
    }
}

void Font::SetItalic(bool italic) {
    m_italic = italic;
    if (m_weight == FontWeight::kBold || m_weight == FontWeight::kExtraBold) {
        m_style = italic ? FontStyle::kBoldItalic : FontStyle::kBold;
    } else {
        m_style = italic ? FontStyle::kItalic : FontStyle::kNormal;
    }
}

void Font::SetBold(bool bold) {
    if (bold) {
        m_weight = FontWeight::kBold;
        m_style = m_italic ? FontStyle::kBoldItalic : FontStyle::kBold;
    } else {
        m_weight = FontWeight::kNormal;
        m_style = m_italic ? FontStyle::kItalic : FontStyle::kNormal;
    }
}

std::string Font::GetStyleString() const {
    switch (m_style) {
        case FontStyle::kNormal: return "normal";
        case FontStyle::kBold: return "bold";
        case FontStyle::kItalic: return "italic";
        case FontStyle::kBoldItalic: return "bold italic";
        default: return "normal";
    }
}

std::string Font::GetWeightString() const {
    switch (m_weight) {
        case FontWeight::kThin: return "thin";
        case FontWeight::kExtraLight: return "extra light";
        case FontWeight::kLight: return "light";
        case FontWeight::kNormal: return "normal";
        case FontWeight::kMedium: return "medium";
        case FontWeight::kSemiBold: return "semi bold";
        case FontWeight::kBold: return "bold";
        case FontWeight::kExtraBold: return "extra bold";
        case FontWeight::kBlack: return "black";
        default: return "normal";
    }
}

std::string Font::ToString() const {
    std::ostringstream oss;
    oss << m_family << " " << m_size << "pt";
    if (m_style != FontStyle::kNormal) {
        oss << " " << GetStyleString();
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
    return m_family.empty() || m_size <= 0;
}

void Font::Reset() {
    m_family = "Arial";
    m_size = 12.0;
    m_style = FontStyle::kNormal;
    m_weight = FontWeight::kNormal;
    m_italic = false;
}

Font Font::Default() {
    return Font("Arial", 12.0);
}

Font Font::FromString(const std::string& str) {
    Font font;
    size_t pos = str.find(' ');
    if (pos != std::string::npos) {
        font.m_family = str.substr(0, pos);
        std::string rest = str.substr(pos + 1);
        size_t ptPos = rest.find("pt");
        if (ptPos != std::string::npos) {
            std::string sizeStr = rest.substr(0, ptPos);
            font.m_size = std::stod(sizeStr);
        }
    }
    return font;
}

bool Font::operator==(const Font& other) const {
    return m_family == other.m_family &&
           m_size == other.m_size &&
           m_style == other.m_style &&
           m_weight == other.m_weight;
}

bool Font::operator!=(const Font& other) const {
    return !(*this == other);
}

}  
}  
