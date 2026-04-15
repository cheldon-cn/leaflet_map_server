#ifndef OGC_DRAW_FONT_H
#define OGC_DRAW_FONT_H

#include "ogc/draw/export.h"
#include <string>
#include <memory>

namespace ogc {
namespace draw {

enum class FontStyle {
    kNormal = 0,
    kBold = 1,
    kItalic = 2,
    kBoldItalic = 3
};

enum class FontWeight {
    kThin = 100,
    kExtraLight = 200,
    kLight = 300,
    kNormal = 400,
    kMedium = 500,
    kSemiBold = 600,
    kBold = 700,
    kExtraBold = 800,
    kBlack = 900
};

class OGC_DRAW_API Font {
public:
    Font();
    Font(const std::string& family, double size);
    Font(const std::string& family, double size, FontStyle style);
    Font(const std::string& family, double size, FontWeight weight, bool italic = false);
    ~Font();
    
    Font(const Font& other);
    Font& operator=(const Font& other);
    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;
    
    const std::string& GetFamily() const;
    double GetSize() const;
    FontStyle GetStyle() const;
    FontWeight GetWeight() const;
    bool IsItalic() const;
    bool IsBold() const;
    bool IsUnderline() const;
    bool IsStrikethrough() const;
    
    void SetFamily(const std::string& family);
    void SetSize(double size);
    void SetStyle(FontStyle style);
    void SetWeight(FontWeight weight);
    void SetItalic(bool italic);
    void SetBold(bool bold);
    void SetUnderline(bool underline);
    void SetStrikethrough(bool strikethrough);
    
    std::string GetStyleString() const;
    std::string GetWeightString() const;
    std::string ToString() const;
    
    double GetLineHeight() const;
    double GetAscent() const;
    double GetDescent() const;
    
    bool IsValid() const;
    bool IsEmpty() const;
    bool IsVisible() const;
    
    void Reset();
    
    static Font Default();
    static Font FromString(const std::string& str);
    
    Font WithFamily(const std::string& family) const;
    Font WithSize(double size) const;
    Font WithWeight(FontWeight weight) const;
    Font WithStyle(FontStyle style) const;
    Font WithItalic(bool italic) const;
    Font WithUnderline(bool underline) const;
    Font WithStrikethrough(bool strikethrough) const;
    
    bool operator==(const Font& other) const;
    bool operator!=(const Font& other) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  
}  

#endif  
