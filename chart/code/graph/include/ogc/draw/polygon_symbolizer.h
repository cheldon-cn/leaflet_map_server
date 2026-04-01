#ifndef OGC_DRAW_POLYGON_SYMBOLIZER_H
#define OGC_DRAW_POLYGON_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_types.h>
#include <string>

namespace ogc {
namespace draw {

enum class FillPattern {
    kSolid,
    kHorizontal,
    kVertical,
    kCross,
    kDiagonal,
    kDiagonalCross,
    kDot,
    kCustom
};

class PolygonSymbolizer;
using PolygonSymbolizerPtr = std::shared_ptr<PolygonSymbolizer>;

class OGC_GRAPH_API PolygonSymbolizer : public Symbolizer {
public:
    PolygonSymbolizer();
    explicit PolygonSymbolizer(uint32_t fillColor);
    PolygonSymbolizer(uint32_t fillColor, uint32_t strokeColor, double strokeWidth);
    ~PolygonSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kPolygon; }
    std::string GetName() const override { return m_name.empty() ? "PolygonSymbolizer" : m_name; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetFillColor(uint32_t color);
    uint32_t GetFillColor() const;
    
    void SetFillOpacity(double opacity);
    double GetFillOpacity() const;
    
    void SetStrokeColor(uint32_t color);
    uint32_t GetStrokeColor() const;
    
    void SetStrokeWidth(double width);
    double GetStrokeWidth() const;
    
    void SetStrokeOpacity(double opacity);
    double GetStrokeOpacity() const;
    
    void SetFillPattern(FillPattern pattern);
    FillPattern GetFillPattern() const;
    
    void SetDisplacement(double dx, double dy);
    void GetDisplacement(double& dx, double& dy) const;
    
    void SetPerpendicularOffset(double offset);
    double GetPerpendicularOffset() const;
    
    void SetGraphicFill(bool enabled);
    bool HasGraphicFill() const;
    
    void SetGraphicFillSize(double size);
    double GetGraphicFillSize() const;
    
    void SetGraphicFillSpacing(double spacing);
    double GetGraphicFillSpacing() const;
    
    static PolygonSymbolizerPtr Create();
    static PolygonSymbolizerPtr Create(uint32_t fillColor);
    static PolygonSymbolizerPtr Create(uint32_t fillColor, uint32_t strokeColor, double strokeWidth);

private:
    DrawResult DrawPolygon(DrawContextPtr context, const ogc::Polygon* polygon, const DrawStyle& style);
    DrawResult DrawMultiPolygon(DrawContextPtr context, const ogc::MultiPolygon* multiPolygon, const DrawStyle& style);
    
    uint32_t m_fillColor;
    double m_fillOpacity;
    uint32_t m_strokeColor;
    double m_strokeWidth;
    double m_strokeOpacity;
    FillPattern m_fillPattern;
    double m_displacementX;
    double m_displacementY;
    double m_perpendicularOffset;
    bool m_graphicFill;
    double m_graphicFillSize;
    double m_graphicFillSpacing;
};

}  
}  

#endif  
