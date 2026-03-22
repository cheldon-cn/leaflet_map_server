#ifndef OGC_DRAW_FONT_H
#define OGC_DRAW_FONT_H

#include <string>

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

class Font {
public:
    Font();
    Font(const std::string& family, double size);
    Font(const std::string& family, double size, FontStyle style);
    Font(const std::string& family, double size, FontWeight weight, bool italic = false);
    
    const std::string& GetFamily() const;
    double GetSize() const;
    FontStyle GetStyle() const;
    FontWeight GetWeight() const;
    bool IsItalic() const;
    bool IsBold() const;
    
    void SetFamily(const std::string& family);
    void SetSize(double size);
    void SetStyle(FontStyle style);
    void SetWeight(FontWeight weight);
    void SetItalic(bool italic);
    void SetBold(bool bold);
    
    std::string GetStyleString() const;
    std::string GetWeightString() const;
    std::string ToString() const;
    
    double GetLineHeight() const;
    double GetAscent() const;
    double GetDescent() const;
    
    bool IsValid() const;
    bool IsEmpty() const;
    
    void Reset();
    
    static Font Default();
    static Font FromString(const std::string& str);
    
    bool operator==(const Font& other) const;
    bool operator!=(const Font& other) const;

private:
    std::string m_family;
    double m_size;
    FontStyle m_style;
    FontWeight m_weight;
    bool m_italic;
};

inline const std::string& Font::GetFamily() const { return m_family; }
inline double Font::GetSize() const { return m_size; }
inline FontStyle Font::GetStyle() const { return m_style; }
inline FontWeight Font::GetWeight() const { return m_weight; }
inline bool Font::IsItalic() const { return m_italic; }
inline bool Font::IsBold() const { 
    return m_weight == FontWeight::kBold || m_weight == FontWeight::kExtraBold ||
           m_style == FontStyle::kBold || m_style == FontStyle::kBoldItalic;
}

}  
}  

#endif  
