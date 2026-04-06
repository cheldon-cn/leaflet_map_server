#ifndef OGC_SYMBOLOGY_POLYGON_SYMBOLIZER_H
#define OGC_SYMBOLOGY_POLYGON_SYMBOLIZER_H

#include "ogc/symbology/symbolizer/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_types.h>
#include <string>

namespace ogc {
namespace symbology {

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

class OGC_SYMBOLOGY_API PolygonSymbolizer : public Symbolizer {
public:
    PolygonSymbolizer();
    explicit PolygonSymbolizer(uint32_t fillColor);
    PolygonSymbolizer(uint32_t fillColor, uint32_t strokeColor, double strokeWidth);
    ~PolygonSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kPolygon; }
    std::string GetName() const override { return m_name.empty() ? "PolygonSymbolizer" : m_name; }
    
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) override;
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) override;
    
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
    ogc::draw::DrawResult DrawPolygon(ogc::draw::DrawContextPtr context, const ogc::Polygon* polygon, const ogc::draw::DrawStyle& style);
    ogc::draw::DrawResult DrawMultiPolygon(ogc::draw::DrawContextPtr context, const ogc::MultiPolygon* multiPolygon, const ogc::draw::DrawStyle& style);
    
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
