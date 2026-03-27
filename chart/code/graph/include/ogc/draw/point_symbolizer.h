#ifndef OGC_DRAW_POINT_SYMBOLIZER_H
#define OGC_DRAW_POINT_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include "ogc/draw/color.h"
#include <string>

namespace ogc {
namespace draw {

enum class PointSymbolType {
    kCircle,
    kSquare,
    kTriangle,
    kStar,
    kCross,
    kDiamond
};

class OGC_GRAPH_API PointSymbolizer : public Symbolizer {
public:
    PointSymbolizer();
    explicit PointSymbolizer(double size, uint32_t color);
    ~PointSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kPoint; }
    std::string GetName() const override { return m_name.empty() ? "PointSymbolizer" : m_name; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
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
    DrawResult DrawPoint(DrawContextPtr context, double x, double y, const DrawStyle& style);
    DrawResult DrawSymbol(DrawContextPtr context, double x, double y, double size, PointSymbolType type, const DrawStyle& style);
    
    PointSymbolType m_symbolType;
    double m_size;
    uint32_t m_color;
    uint32_t m_strokeColor;
    double m_strokeWidth;
    double m_rotation;
    double m_anchorX;
    double m_anchorY;
    double m_displacementX;
    double m_displacementY;
};

}  
}  

#endif  
