#ifndef OGC_DRAW_COMPOSITE_SYMBOLIZER_H
#define OGC_DRAW_COMPOSITE_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include <vector>

namespace ogc {
namespace draw {

class OGC_GRAPH_API CompositeSymbolizer : public Symbolizer {
public:
    CompositeSymbolizer();
    ~CompositeSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kComposite; }
    std::string GetName() const override { return m_name.empty() ? "CompositeSymbolizer" : m_name; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void AddSymbolizer(SymbolizerPtr symbolizer);
    void RemoveSymbolizer(size_t index);
    void ClearSymbolizers();
    
    size_t GetSymbolizerCount() const;
    SymbolizerPtr GetSymbolizer(size_t index) const;
    std::vector<SymbolizerPtr> GetSymbolizers() const;
    
    void SetSymbolizers(const std::vector<SymbolizerPtr>& symbolizers);
    
    void SetCompositionMode(const std::string& mode);
    std::string GetCompositionMode() const;
    
    static CompositeSymbolizerPtr Create();

private:
    std::vector<SymbolizerPtr> m_symbolizers;
    std::string m_compositionMode;
};

}  
}  

#endif  
