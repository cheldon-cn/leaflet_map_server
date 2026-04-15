#include "ogc/draw/font.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace draw {

struct Font::Impl {
    std::string family;
    double size;
    FontStyle style;
    FontWeight weight;
    bool italic;
    bool underline;
    bool strikethrough;
    
    Impl() : family("Arial"), size(12.0), style(FontStyle::kNormal), 
             weight(FontWeight::kNormal), italic(false), underline(false), strikethrough(false) {}
    Impl(const std::string& f, double s) : family(f), size(s), style(FontStyle::kNormal),
             weight(FontWeight::kNormal), italic(false), underline(false), strikethrough(false) {}
    Impl(const std::string& f, double s, FontStyle st) : family(f), size(s), style(st),
             weight(FontWeight::kNormal), italic(st == FontStyle::kItalic || st == FontStyle::kBoldItalic),
             underline(false), strikethrough(false) {}
    Impl(const std::string& f, double s, FontWeight w, bool i) : family(f), size(s),
             style(i ? FontStyle::kItalic : FontStyle::kNormal), weight(w), italic(i),
             underline(false), strikethrough(false) {}
};

Font::Font() : impl_(new Impl()) {}

Font::Font(const std::string& family, double size) : impl_(new Impl(family, size)) {}

Font::Font(const std::string& family, double size, FontStyle style) 
    : impl_(new Impl(family, size, style)) {}

Font::Font(const std::string& family, double size, FontWeight weight, bool italic)
    : impl_(new Impl(family, size, weight, italic)) {}

Font::~Font() = default;

Font::Font(const Font& other) : impl_(new Impl(*other.impl_)) {}

Font& Font::operator=(const Font& other) {
    if (this != &other) {
        *impl_ = *other.impl_;
    }
    return *this;
}

Font::Font(Font&& other) noexcept : impl_(std::move(other.impl_)) {
    other.impl_.reset(new Impl());
}

Font& Font::operator=(Font&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
        other.impl_.reset(new Impl());
    }
    return *this;
}

const std::string& Font::GetFamily() const { return impl_->family; }
double Font::GetSize() const { return impl_->size; }
FontStyle Font::GetStyle() const { return impl_->style; }
FontWeight Font::GetWeight() const { return impl_->weight; }
bool Font::IsItalic() const { return impl_->italic; }
bool Font::IsUnderline() const { return impl_->underline; }
bool Font::IsStrikethrough() const { return impl_->strikethrough; }
bool Font::IsBold() const { 
    return impl_->weight == FontWeight::kBold || impl_->weight == FontWeight::kExtraBold ||
           impl_->style == FontStyle::kBold || impl_->style == FontStyle::kBoldItalic;
}

void Font::SetFamily(const std::string& family) { impl_->family = family; }
void Font::SetSize(double size) { impl_->size = size; }
void Font::SetStyle(FontStyle style) { impl_->style = style; }
void Font::SetWeight(FontWeight weight) { impl_->weight = weight; }
void Font::SetItalic(bool italic) { impl_->italic = italic; }
void Font::SetBold(bool bold) {
    if (bold) {
        impl_->weight = FontWeight::kBold;
    }
}
void Font::SetUnderline(bool underline) { impl_->underline = underline; }
void Font::SetStrikethrough(bool strikethrough) { impl_->strikethrough = strikethrough; }

std::string Font::GetStyleString() const {
    switch (impl_->style) {
        case FontStyle::kNormal:     return "Normal";
        case FontStyle::kBold:       return "Bold";
        case FontStyle::kItalic:     return "Italic";
        case FontStyle::kBoldItalic: return "Bold Italic";
        default:                     return "Normal";
    }
}

std::string Font::GetWeightString() const {
    switch (impl_->weight) {
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
    oss << impl_->family << " " << impl_->size << "pt";
    if (impl_->weight != FontWeight::kNormal) {
        oss << " " << GetWeightString();
    }
    if (impl_->italic) {
        oss << " Italic";
    }
    if (impl_->underline) {
        oss << " Underline";
    }
    if (impl_->strikethrough) {
        oss << " Strikethrough";
    }
    return oss.str();
}

double Font::GetLineHeight() const { return impl_->size * 1.2; }
double Font::GetAscent() const { return impl_->size * 0.8; }
double Font::GetDescent() const { return impl_->size * 0.2; }

bool Font::IsValid() const { return !impl_->family.empty() && impl_->size > 0; }
bool Font::IsEmpty() const { return impl_->family.empty() && impl_->size == 0; }
bool Font::IsVisible() const { return IsValid() && impl_->size > 0; }

void Font::Reset() {
    impl_->family = "Arial";
    impl_->size = 12.0;
    impl_->style = FontStyle::kNormal;
    impl_->weight = FontWeight::kNormal;
    impl_->italic = false;
    impl_->underline = false;
    impl_->strikethrough = false;
}

Font Font::Default() { return Font("Arial", 12.0); }

Font Font::FromString(const std::string& str) {
    Font font;
    std::istringstream iss(str);
    std::string token;
    
    if (std::getline(iss, token, ' ')) {
        font.impl_->family = token;
    }
    if (std::getline(iss, token, ' ')) {
        size_t ptPos = token.find("pt");
        if (ptPos != std::string::npos) {
            token = token.substr(0, ptPos);
        }
        try {
            font.impl_->size = std::stod(token);
        } catch (...) {
            font.impl_->size = 12.0;
        }
    }
    
    std::string remaining;
    while (std::getline(iss, token, ' ')) {
        if (token == "Bold" || token == "Bold") {
            font.impl_->weight = FontWeight::kBold;
        } else if (token == "Italic") {
            font.impl_->italic = true;
        } else if (token == "Underline") {
            font.impl_->underline = true;
        } else if (token == "Strikethrough") {
            font.impl_->strikethrough = true;
        }
    }
    
    return font;
}

Font Font::WithFamily(const std::string& family) const {
    Font f = *this;
    f.impl_->family = family;
    return f;
}

Font Font::WithSize(double size) const {
    Font f = *this;
    f.impl_->size = size;
    return f;
}

Font Font::WithWeight(FontWeight weight) const {
    Font f = *this;
    f.impl_->weight = weight;
    return f;
}

Font Font::WithStyle(FontStyle style) const {
    Font f = *this;
    f.impl_->style = style;
    return f;
}

Font Font::WithItalic(bool italic) const {
    Font f = *this;
    f.impl_->italic = italic;
    return f;
}

Font Font::WithUnderline(bool underline) const {
    Font f = *this;
    f.impl_->underline = underline;
    return f;
}

Font Font::WithStrikethrough(bool strikethrough) const {
    Font f = *this;
    f.impl_->strikethrough = strikethrough;
    return f;
}

bool Font::operator==(const Font& other) const {
    return impl_->family == other.impl_->family &&
           impl_->size == other.impl_->size &&
           impl_->style == other.impl_->style &&
           impl_->weight == other.impl_->weight &&
           impl_->italic == other.impl_->italic &&
           impl_->underline == other.impl_->underline &&
           impl_->strikethrough == other.impl_->strikethrough;
}

bool Font::operator!=(const Font& other) const {
    return !(*this == other);
}

}  
}  
