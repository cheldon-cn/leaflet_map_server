#ifndef OGC_SYMBOLOGY_POINT_SYMBOLIZER_H
#define OGC_SYMBOLOGY_POINT_SYMBOLIZER_H

#include "ogc/symbology/symbolizer/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_types.h>
#include <string>

namespace ogc {
namespace symbology {

enum class PointSymbolType {
    kCircle,
    kSquare,
    kTriangle,
    kStar,
    kCross,
    kDiamond
};

class PointSymbolizer;
using PointSymbolizerPtr = std::shared_ptr<PointSymbolizer>;

class OGC_SYMBOLOGY_API PointSymbolizer : public Symbolizer {
public:
    PointSymbolizer();
    explicit PointSymbolizer(double size, uint32_t color);
    ~PointSymbolizer() override;
    
    SymbolizerType GetType() const override { return SymbolizerType::kPoint; }
    std::string GetName() const override;
    
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) override;
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetSymbolType(PointSymbolType type);
    PointSymbolType GetSymbolType() const;
    
    void SetSize(double size);
    double GetSize() const;
    
    void SetColor(uint32_t color);
    uint32_t GetColor() const;
    
    void SetStrokeColor(uint32_t color);
    uint32_t GetStrokeColor() const;
    
    void SetStrokeWidth(double width);
    double GetStrokeWidth() const;
    
    void SetRotation(double angle);
    double GetRotation() const;
    
    void SetAnchorPoint(double x, double y);
    void GetAnchorPoint(double& x, double& y) const;
    
    void SetDisplacement(double dx, double dy);
    void GetDisplacement(double& dx, double& dy) const;
    
    static PointSymbolizerPtr Create();
    static PointSymbolizerPtr Create(double size, uint32_t color);

private:
    ogc::draw::DrawResult DrawPoint(ogc::draw::DrawContextPtr context, double x, double y, const ogc::draw::DrawStyle& style);
    ogc::draw::DrawResult DrawSymbol(ogc::draw::DrawContextPtr context, double x, double y, double size, PointSymbolType type, const ogc::draw::DrawStyle& style);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
