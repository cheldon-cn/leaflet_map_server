#ifndef OGC_SYMBOLOGY_COMPOSITE_SYMBOLIZER_H
#define OGC_SYMBOLOGY_COMPOSITE_SYMBOLIZER_H

#include "ogc/symbology/symbolizer/symbolizer.h"
#include <ogc/draw/draw_types.h>
#include <vector>

namespace ogc {
namespace symbology {

class CompositeSymbolizer;
using CompositeSymbolizerPtr = std::shared_ptr<CompositeSymbolizer>;

class OGC_SYMBOLOGY_API CompositeSymbolizer : public Symbolizer {
public:
    CompositeSymbolizer();
    ~CompositeSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kComposite; }
    std::string GetName() const override { return m_name.empty() ? "CompositeSymbolizer" : m_name; }
    
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) override;
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) override;
    
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
