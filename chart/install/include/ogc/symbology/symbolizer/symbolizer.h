#ifndef OGC_SYMBOLOGY_SYMBOLIZER_SYMBOLIZER_H
#define OGC_SYMBOLOGY_SYMBOLIZER_SYMBOLIZER_H

#include "ogc/symbology/export.h"
#include <ogc/draw/draw_result.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_types.h>
#include "ogc/geom/geometry.h"
#include "ogc/geom/common.h"
#include <memory>
#include <string>

namespace ogc {
namespace symbology {

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

class OGC_SYMBOLOGY_API Symbolizer {
public:
    virtual ~Symbolizer() = default;
    
    virtual SymbolizerType GetType() const = 0;
    virtual std::string GetName() const = 0;
    
    virtual ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) = 0;
    virtual ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) = 0;
    
    virtual bool CanSymbolize(GeomType geomType) const = 0;
    
    virtual SymbolizerPtr Clone() const = 0;
    
    void SetDefaultStyle(const ogc::draw::DrawStyle& style);
    ogc::draw::DrawStyle GetDefaultStyle() const;
    
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
    
    ogc::draw::DrawStyle MergeStyle(const ogc::draw::DrawStyle& base, const ogc::draw::DrawStyle& override) const;
    
    std::string m_name;
    ogc::draw::DrawStyle m_defaultStyle;
    bool m_enabled;
    double m_minScale;
    double m_maxScale;
    int m_zIndex;
    double m_opacity;
};

}
}

#endif
