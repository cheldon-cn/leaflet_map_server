#ifndef OGC_DRAW_S52_SYMBOL_RENDERER_H
#define OGC_DRAW_S52_SYMBOL_RENDERER_H

#include "ogc/draw/export.h"
#include "ogc/draw/s52_style_manager.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_context.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <map>
#include <mutex>

namespace ogc {
namespace draw {

class S52SymbolRenderer;
using S52SymbolRendererPtr = std::shared_ptr<S52SymbolRenderer>;

class OGC_GRAPH_API S52SymbolRenderer {
public:
    static S52SymbolRenderer& Instance();
    
    bool Initialize();
    
    DrawResult RenderSymbol(DrawContextPtr context, 
                           const std::string& symbolId,
                           const Geometry* geometry,
                           const StyleRule* rule);
    
    DrawResult RenderPointSymbol(DrawContextPtr context,
                                 const std::string& symbolId,
                                 double x, double y,
                                 const StyleRule* rule);
    
    DrawResult RenderLineSymbol(DrawContextPtr context,
                                const std::string& symbolId,
                                const LineString* lineString,
                                const StyleRule* rule);
    
    DrawResult RenderPolygonSymbol(DrawContextPtr context,
                                   const std::string& symbolId,
                                   const Polygon* polygon,
                                   const StyleRule* rule);
    
    SymbolizerPtr CreateSymbolizerFromDefinition(const SymbolDefinition* symbolDef,
                                                  const StyleRule* rule);
    
    SymbolizerPtr CreatePointSymbolizer(const SymbolDefinition* symbolDef,
                                         const StyleRule* rule);
    
    SymbolizerPtr CreateLineSymbolizer(const SymbolDefinition* symbolDef,
                                        const StyleRule* rule);
    
    SymbolizerPtr CreatePolygonSymbolizer(const SymbolDefinition* symbolDef,
                                           const StyleRule* rule);
    
    void SetDefaultSymbolSize(double size);
    double GetDefaultSymbolSize() const;
    
    void SetDefaultLineWidth(double width);
    double GetDefaultLineWidth() const;
    
    void CacheSymbolizer(const std::string& key, SymbolizerPtr symbolizer);
    SymbolizerPtr GetCachedSymbolizer(const std::string& key) const;
    void ClearCache();
    
    std::string GenerateCacheKey(const std::string& symbolId, 
                                  const StyleRule* rule) const;
    
    bool IsInitialized() const { return m_initialized; }

private:
    S52SymbolRenderer();
    ~S52SymbolRenderer();
    
    DrawResult RenderVectorSymbol(DrawContextPtr context,
                                  const SymbolDefinition* symbolDef,
                                  double x, double y,
                                  const StyleRule* rule);
    
    DrawResult RenderRasterSymbol(DrawContextPtr context,
                                  const SymbolDefinition* symbolDef,
                                  double x, double y,
                                  const StyleRule* rule);
    
    DrawResult RenderCompositeSymbol(DrawContextPtr context,
                                     const SymbolDefinition* symbolDef,
                                     double x, double y,
                                     const StyleRule* rule);
    
    void ExecuteVectorCommand(DrawContextPtr context,
                              const VectorCommand& cmd,
                              double x, double y,
                              double scale);
    
    bool m_initialized;
    double m_defaultSymbolSize;
    double m_defaultLineWidth;
    std::map<std::string, SymbolizerPtr> m_symbolizerCache;
    mutable std::mutex m_mutex;
};

}
}

#endif
