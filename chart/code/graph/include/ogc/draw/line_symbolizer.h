#ifndef OGC_DRAW_LINE_SYMBOLIZER_H
#define OGC_DRAW_LINE_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include "ogc/draw/color.h"
#include "ogc/draw/draw_style.h"
#include <string>
#include <vector>

namespace ogc {
namespace draw {

enum class DashStyle {
    kSolid,
    kDash,
    kDot,
    kDashDot,
    kDashDotDot,
    kCustom
};

class LineSymbolizer : public Symbolizer {
public:
    LineSymbolizer();
    explicit LineSymbolizer(double width, uint32_t color);
    ~LineSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kLine; }
    std::string GetName() const override { return "LineSymbolizer"; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetWidth(double width);
    double GetWidth() const;
    
    void SetColor(uint32_t color);
    uint32_t GetColor() const;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    void SetCapStyle(LineCapStyle style);
    LineCapStyle GetCapStyle() const;
    
    void SetJoinStyle(LineJoinStyle style);
    LineJoinStyle GetJoinStyle() const;
    
    void SetDashStyle(DashStyle style);
    DashStyle GetDashStyle() const;
    
    void SetDashPattern(const std::vector<double>& pattern);
    std::vector<double> GetDashPattern() const;
    
    void SetDashOffset(double offset);
    double GetDashOffset() const;
    
    void SetOffset(double offset);
    double GetOffset() const;
    
    void SetPerpendicularOffset(double offset);
    double GetPerpendicularOffset() const;
    
    void SetGraphicStroke(bool enabled);
    bool HasGraphicStroke() const;
    
    void SetGraphicStrokeSize(double size);
    double GetGraphicStrokeSize() const;
    
    void SetGraphicStrokeSpacing(double spacing);
    double GetGraphicStrokeSpacing() const;
    
    static LineSymbolizerPtr Create();
    static LineSymbolizerPtr Create(double width, uint32_t color);

private:
    DrawResult DrawLineString(DrawContextPtr context, const ogc::LineString* lineString, const DrawStyle& style);
    DrawResult DrawMultiLineString(DrawContextPtr context, const ogc::MultiLineString* multiLineString, const DrawStyle& style);
    std::vector<double> GetDefaultDashPattern(DashStyle style) const;
    
    double m_width;
    uint32_t m_color;
    double m_opacity;
    LineCapStyle m_capStyle;
    LineJoinStyle m_joinStyle;
    DashStyle m_dashStyle;
    std::vector<double> m_dashPattern;
    double m_dashOffset;
    double m_offset;
    double m_perpendicularOffset;
    bool m_graphicStroke;
    double m_graphicStrokeSize;
    double m_graphicStrokeSpacing;
};

}  
}  

#endif  
