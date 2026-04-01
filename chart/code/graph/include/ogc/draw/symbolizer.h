#ifndef OGC_DRAW_SYMBOLIZER_H
#define OGC_DRAW_SYMBOLIZER_H

#include "ogc/draw/export.h"
#include <ogc/draw/draw_result.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_types.h>
#include "ogc/geometry.h"
#include "ogc/common.h"
#include <memory>
#include <string>

namespace ogc {
namespace draw {

enum class SymbolizerType {
    kPoint,
    kLine,
    kPolygon,
    kText,
    kRaster,
    kComposite,
    kIcon
};

class Symbolizer;
using SymbolizerPtr = std::shared_ptr<Symbolizer>;

class OGC_GRAPH_API Symbolizer {
public:
    virtual ~Symbolizer() = default;
    
    virtual SymbolizerType GetType() const = 0;
    virtual std::string GetName() const = 0;
    
    virtual DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) = 0;
    virtual DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) = 0;
    
    virtual bool CanSymbolize(GeomType geomType) const = 0;
    
    virtual SymbolizerPtr Clone() const = 0;
    
    void SetDefaultStyle(const DrawStyle& style);
    DrawStyle GetDefaultStyle() const;
    
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
    void SetName(const std::string& name);
    
    void SetMinScale(double scale);
    double GetMinScale() const;
    
    void SetMaxScale(double scale);
    double GetMaxScale() const;
    
    bool IsVisibleAtScale(double scale) const;
    
    void SetZIndex(int index);
    int GetZIndex() const;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    static SymbolizerPtr Create(SymbolizerType type);

protected:
    Symbolizer();
    
    DrawStyle MergeStyle(const DrawStyle& base, const DrawStyle& override) const;
    
    std::string m_name;
    DrawStyle m_defaultStyle;
    bool m_enabled;
    double m_minScale;
    double m_maxScale;
    int m_zIndex;
    double m_opacity;
};

}  
}  

#endif  
