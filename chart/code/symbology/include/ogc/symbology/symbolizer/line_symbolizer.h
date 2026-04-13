#ifndef OGC_SYMBOLOGY_LINE_SYMBOLIZER_H
#define OGC_SYMBOLOGY_LINE_SYMBOLIZER_H

#include "ogc/symbology/symbolizer/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/draw_types.h>
#include <string>
#include <vector>

namespace ogc {
namespace symbology {

enum class DashStyle {
    kSolid,
    kDash,
    kDot,
    kDashDot,
    kDashDotDot,
    kCustom
};

class LineSymbolizer;
using LineSymbolizerPtr = std::shared_ptr<LineSymbolizer>;

class OGC_SYMBOLOGY_API LineSymbolizer : public Symbolizer {
public:
    LineSymbolizer();
    explicit LineSymbolizer(double width, uint32_t color);
    ~LineSymbolizer() override;
    
    SymbolizerType GetType() const override { return SymbolizerType::kLine; }
    std::string GetName() const override;
    
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) override;
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetWidth(double width);
    double GetWidth() const;
    
    void SetColor(uint32_t color);
    uint32_t GetColor() const;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    void SetCapStyle(ogc::draw::LineCap style);
    ogc::draw::LineCap GetCapStyle() const;
    
    void SetJoinStyle(ogc::draw::LineJoin style);
    ogc::draw::LineJoin GetJoinStyle() const;
    
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
    ogc::draw::DrawResult DrawLineString(ogc::draw::DrawContextPtr context, const ogc::LineString* lineString, const ogc::draw::DrawStyle& style);
    ogc::draw::DrawResult DrawMultiLineString(ogc::draw::DrawContextPtr context, const ogc::MultiLineString* multiLineString, const ogc::draw::DrawStyle& style);
    std::vector<double> GetDefaultDashPattern(DashStyle style) const;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
