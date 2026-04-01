#include "ogc/draw/composite_symbolizer.h"
#include <algorithm>

namespace ogc {
namespace draw {

CompositeSymbolizer::CompositeSymbolizer()
    : m_compositionMode("normal") {
}

DrawResult CompositeSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

DrawResult CompositeSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) {
    if (!context || !geometry) {
        return DrawResult::kInvalidParameter;
    }
    
    if (!m_enabled) {
        return DrawResult::kSuccess;
    }
    
    double scale = context->GetTransform().GetScaleX();
    if (!IsVisibleAtScale(scale)) {
        return DrawResult::kSuccess;
    }
    
    if (m_symbolizers.empty()) {
        return DrawResult::kSuccess;
    }
    
    std::vector<SymbolizerPtr> sortedSymbolizers = m_symbolizers;
    std::sort(sortedSymbolizers.begin(), sortedSymbolizers.end(),
              [](const SymbolizerPtr& a, const SymbolizerPtr& b) {
                  return a->GetZIndex() < b->GetZIndex();
              });
    
    DrawResult result = DrawResult::kSuccess;
    
    for (auto& symbolizer : sortedSymbolizers) {
        if (symbolizer && symbolizer->IsEnabled()) {
            DrawResult r = symbolizer->Symbolize(context, geometry, style);
            if (r != DrawResult::kSuccess) {
                result = r;
            }
        }
    }
    
    return result;
}

bool CompositeSymbolizer::CanSymbolize(GeomType geomType) const {
    if (m_symbolizers.empty()) {
        return true;
    }
    
    for (const auto& symbolizer : m_symbolizers) {
        if (symbolizer && symbolizer->CanSymbolize(geomType)) {
            return true;
        }
    }
    return false;
}

void CompositeSymbolizer::AddSymbolizer(SymbolizerPtr symbolizer) {
    if (symbolizer) {
        m_symbolizers.push_back(symbolizer);
    }
}

void CompositeSymbolizer::RemoveSymbolizer(size_t index) {
    if (index < m_symbolizers.size()) {
        m_symbolizers.erase(m_symbolizers.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

void CompositeSymbolizer::ClearSymbolizers() {
    m_symbolizers.clear();
}

size_t CompositeSymbolizer::GetSymbolizerCount() const {
    return m_symbolizers.size();
}

SymbolizerPtr CompositeSymbolizer::GetSymbolizer(size_t index) const {
    if (index < m_symbolizers.size()) {
        return m_symbolizers[index];
    }
    return nullptr;
}

std::vector<SymbolizerPtr> CompositeSymbolizer::GetSymbolizers() const {
    return m_symbolizers;
}

void CompositeSymbolizer::SetSymbolizers(const std::vector<SymbolizerPtr>& symbolizers) {
    m_symbolizers = symbolizers;
}

void CompositeSymbolizer::SetCompositionMode(const std::string& mode) {
    m_compositionMode = mode;
}

std::string CompositeSymbolizer::GetCompositionMode() const {
    return m_compositionMode;
}

SymbolizerPtr CompositeSymbolizer::Clone() const {
    auto sym = std::make_shared<CompositeSymbolizer>();
    for (const auto& s : m_symbolizers) {
        if (s) {
            sym->AddSymbolizer(s->Clone());
        }
    }
    sym->m_compositionMode = m_compositionMode;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    sym->m_opacity = m_opacity;
    return sym;
}

CompositeSymbolizerPtr CompositeSymbolizer::Create() {
    return std::make_shared<CompositeSymbolizer>();
}

}  
}  
