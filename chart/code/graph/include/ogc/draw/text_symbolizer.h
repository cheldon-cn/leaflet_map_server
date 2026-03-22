#ifndef OGC_DRAW_TEXT_SYMBOLIZER_H
#define OGC_DRAW_TEXT_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include <string>

namespace ogc {
namespace draw {

enum class TextHorizontalAlignment {
    kLeft,
    kCenter,
    kRight
};

enum class TextVerticalAlignment {
    kTop,
    kMiddle,
    kBottom
};

enum class TextPlacement {
    kPoint,
    kLine,
    kInterior
};

class TextSymbolizer : public Symbolizer {
public:
    TextSymbolizer();
    explicit TextSymbolizer(const std::string& label, const Font& font, uint32_t color);
    ~TextSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kText; }
    std::string GetName() const override { return "TextSymbolizer"; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetLabel(const std::string& label);
    std::string GetLabel() const;
    
    void SetLabelProperty(const std::string& propertyName);
    std::string GetLabelProperty() const;
    
    void SetFont(const Font& font);
    Font GetFont() const;
    
    void SetColor(uint32_t color);
    uint32_t GetColor() const;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    void SetHorizontalAlignment(TextHorizontalAlignment alignment);
    TextHorizontalAlignment GetHorizontalAlignment() const;
    
    void SetVerticalAlignment(TextVerticalAlignment alignment);
    TextVerticalAlignment GetVerticalAlignment() const;
    
    void SetPlacement(TextPlacement placement);
    TextPlacement GetPlacement() const;
    
    void SetOffset(double dx, double dy);
    void GetOffset(double& dx, double& dy) const;
    
    void SetRotation(double angle);
    double GetRotation() const;
    
    void SetMaxAngleDelta(double delta);
    double GetMaxAngleDelta() const;
    
    void SetFollowLine(bool follow);
    bool GetFollowLine() const;
    
    void SetMaxDisplacement(double displacement);
    double GetMaxDisplacement() const;
    
    void SetRepeatDistance(double distance);
    double GetRepeatDistance() const;
    
    void SetHaloColor(uint32_t color);
    uint32_t GetHaloColor() const;
    
    void SetHaloRadius(double radius);
    double GetHaloRadius() const;
    
    void SetHaloOpacity(double opacity);
    double GetHaloOpacity() const;
    
    void SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    
    static TextSymbolizerPtr Create();
    static TextSymbolizerPtr Create(const std::string& label, const Font& font, uint32_t color);

private:
    DrawResult DrawTextAtPoint(DrawContextPtr context, double x, double y, const std::string& text);
    DrawResult DrawTextAlongLine(DrawContextPtr context, const ogc::LineString* lineString, const std::string& text);
    DrawResult DrawTextInPolygon(DrawContextPtr context, const ogc::Polygon* polygon, const std::string& text);
    std::string GetLabelText(const Geometry* geometry) const;
    
    std::string m_label;
    std::string m_labelProperty;
    Font m_font;
    uint32_t m_color;
    double m_opacity;
    TextHorizontalAlignment m_horizontalAlignment;
    TextVerticalAlignment m_verticalAlignment;
    TextPlacement m_placement;
    double m_offsetX;
    double m_offsetY;
    double m_rotation;
    double m_maxAngleDelta;
    bool m_followLine;
    double m_maxDisplacement;
    double m_repeatDistance;
    uint32_t m_haloColor;
    double m_haloRadius;
    double m_haloOpacity;
    uint32_t m_backgroundColor;
};

}  
}  

#endif  
