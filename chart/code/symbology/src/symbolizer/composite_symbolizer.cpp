#include "ogc/symbology/symbolizer/composite_symbolizer.h"
#include <algorithm>

namespace ogc {
namespace symbology {

CompositeSymbolizer::CompositeSymbolizer()
    : m_compositionMode("normal") {
}

CompositeSymbolizer::~CompositeSymbolizer() = default;

std::string CompositeSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "CompositeSymbolizer" : name;
}

ogc::draw::DrawResult CompositeSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult CompositeSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
    if (!context || !geometry) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    if (!IsEnabled()) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    double scale = context->GetTransform().GetScaleX();
    if (!IsVisibleAtScale(scale)) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    if (m_symbolizers.empty()) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    std::vector<SymbolizerPtr> sortedSymbolizers = m_symbolizers;
    std::sort(sortedSymbolizers.begin(), sortedSymbolizers.end(),
              [](const SymbolizerPtr& a, const SymbolizerPtr& b) {
                  return a->GetZIndex() < b->GetZIndex();
              });
    
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    
    for (auto& symbolizer : sortedSymbolizers) {
        if (symbolizer && symbolizer->IsEnabled()) {
            ogc::draw::DrawResult r = symbolizer->Symbolize(context, geometry, style);
            if (r != ogc::draw::DrawResult::kSuccess) {
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
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    sym->SetOpacity(GetOpacity());
    return sym;
}

CompositeSymbolizerPtr CompositeSymbolizer::Create() {
    return std::make_shared<CompositeSymbolizer>();
}

}
}
