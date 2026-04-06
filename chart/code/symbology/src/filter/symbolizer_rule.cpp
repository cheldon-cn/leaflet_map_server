#include "ogc/symbology/filter/symbolizer_rule.h"
#include <algorithm>

namespace ogc {
namespace symbology {

SymbolizerRule::SymbolizerRule()
    : m_minScaleDenominator(0.0)
    , m_maxScaleDenominator(std::numeric_limits<double>::max())
    , m_isElseFilter(false)
    , m_priority(0)
{
}

SymbolizerRule::SymbolizerRule(const std::string& name)
    : m_name(name)
    , m_minScaleDenominator(0.0)
    , m_maxScaleDenominator(std::numeric_limits<double>::max())
    , m_isElseFilter(false)
    , m_priority(0)
{
}

bool SymbolizerRule::Evaluate(const CNFeature* feature) const {
    if (m_isElseFilter) {
        return true;
    }
    
    if (!m_filter) {
        return true;
    }
    
    return m_filter->Evaluate(feature);
}

bool SymbolizerRule::Evaluate(const Geometry* geometry) const {
    if (m_isElseFilter) {
        return true;
    }
    
    if (!m_filter) {
        return true;
    }
    
    return m_filter->Evaluate(geometry);
}

bool SymbolizerRule::IsScaleInRange(double scale) const {
    return scale >= m_minScaleDenominator && scale <= m_maxScaleDenominator;
}

void SymbolizerRule::AddSymbolizer(SymbolizerPtr symbolizer) {
    if (symbolizer) {
        m_symbolizers.push_back(symbolizer);
    }
}

void SymbolizerRule::RemoveSymbolizer(SymbolizerPtr symbolizer) {
    auto it = std::find(m_symbolizers.begin(), m_symbolizers.end(), symbolizer);
    if (it != m_symbolizers.end()) {
        m_symbolizers.erase(it);
    }
}

void SymbolizerRule::ClearSymbolizers() {
    m_symbolizers.clear();
}

SymbolizerPtr SymbolizerRule::GetSymbolizer(size_t index) const {
    if (index < m_symbolizers.size()) {
        return m_symbolizers[index];
    }
    return nullptr;
}

SymbolizerRulePtr SymbolizerRule::Clone() const {
    SymbolizerRulePtr rule = std::make_shared<SymbolizerRule>(m_name);
    rule->SetTitle(m_title);
    rule->SetAbstract(m_abstract);
    
    if (m_filter) {
        rule->SetFilter(m_filter->Clone());
    }
    
    rule->SetMinScaleDenominator(m_minScaleDenominator);
    rule->SetMaxScaleDenominator(m_maxScaleDenominator);
    
    for (const auto& sym : m_symbolizers) {
        if (sym) {
            rule->AddSymbolizer(sym->Clone());
        }
    }
    
    rule->SetElseFilter(m_isElseFilter);
    rule->SetPriority(m_priority);
    
    return rule;
}

SymbolizerRulePtr SymbolizerRule::Create() {
    return std::make_shared<SymbolizerRule>();
}

SymbolizerRulePtr SymbolizerRule::Create(const std::string& name) {
    return std::make_shared<SymbolizerRule>(name);
}

}
}
