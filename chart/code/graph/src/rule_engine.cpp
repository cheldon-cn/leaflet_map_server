#include "ogc/draw/rule_engine.h"
#include <ogc/draw/draw_result.h>
#include <algorithm>

namespace ogc {
namespace draw {

RuleEngine::RuleEngine()
    : m_elseFilterEnabled(true)
{
}

void RuleEngine::AddRule(SymbolizerRulePtr rule) {
    if (rule) {
        m_rules.push_back(rule);
    }
}

void RuleEngine::RemoveRule(SymbolizerRulePtr rule) {
    m_rules.erase(
        std::remove(m_rules.begin(), m_rules.end(), rule),
        m_rules.end()
    );
}

void RuleEngine::ClearRules() {
    m_rules.clear();
}

SymbolizerRulePtr RuleEngine::GetRule(size_t index) const {
    if (index < m_rules.size()) {
        return m_rules[index];
    }
    return nullptr;
}

SymbolizerRulePtr RuleEngine::GetRuleByName(const std::string& name) const {
    for (const auto& rule : m_rules) {
        if (rule && rule->GetName() == name) {
            return rule;
        }
    }
    return nullptr;
}

std::vector<SymbolizerRulePtr> RuleEngine::GetMatchingRules(const CNFeature* feature, double scale) const {
    std::vector<SymbolizerRulePtr> result;
    
    for (const auto& rule : m_rules) {
        if (rule && EvaluateRule(*rule, feature, scale)) {
            result.push_back(rule);
        }
    }
    
    return result;
}

std::vector<SymbolizerRulePtr> RuleEngine::GetMatchingRules(const Geometry* geometry, double scale) const {
    std::vector<SymbolizerRulePtr> result;
    
    for (const auto& rule : m_rules) {
        if (rule && EvaluateRule(*rule, geometry, scale)) {
            result.push_back(rule);
        }
    }
    
    return result;
}

RuleMatchResult RuleEngine::Match(const CNFeature* feature, double scale) const {
    RuleMatchResult result;
    
    auto matchingRules = GetMatchingRules(feature, scale);
    if (!matchingRules.empty()) {
        result.matched = true;
        result.rule = matchingRules[0];
        
        for (const auto& rule : matchingRules) {
            for (size_t i = 0; i < rule->GetSymbolizerCount(); ++i) {
                auto sym = rule->GetSymbolizer(i);
                if (sym) {
                    result.symbolizers.push_back(sym);
                }
            }
        }
    }
    
    return result;
}

RuleMatchResult RuleEngine::Match(const Geometry* geometry, double scale) const {
    RuleMatchResult result;
    
    auto matchingRules = GetMatchingRules(geometry, scale);
    if (!matchingRules.empty()) {
        result.matched = true;
        result.rule = matchingRules[0];
        
        for (const auto& rule : matchingRules) {
            for (size_t i = 0; i < rule->GetSymbolizerCount(); ++i) {
                auto sym = rule->GetSymbolizer(i);
                if (sym) {
                    result.symbolizers.push_back(sym);
                }
            }
        }
    }
    
    return result;
}

std::vector<SymbolizerPtr> RuleEngine::GetSymbolizers(const CNFeature* feature, double scale) const {
    std::vector<SymbolizerPtr> result;
    
    auto matchingRules = GetMatchingRules(feature, scale);
    for (const auto& rule : matchingRules) {
        for (size_t i = 0; i < rule->GetSymbolizerCount(); ++i) {
            auto sym = rule->GetSymbolizer(i);
            if (sym) {
                result.push_back(sym);
            }
        }
    }
    
    return result;
}

std::vector<SymbolizerPtr> RuleEngine::GetSymbolizers(const Geometry* geometry, double scale) const {
    std::vector<SymbolizerPtr> result;
    
    auto matchingRules = GetMatchingRules(geometry, scale);
    for (const auto& rule : matchingRules) {
        for (size_t i = 0; i < rule->GetSymbolizerCount(); ++i) {
            auto sym = rule->GetSymbolizer(i);
            if (sym) {
                result.push_back(sym);
            }
        }
    }
    
    return result;
}

DrawResult RuleEngine::Render(const CNFeature* feature, DrawContext& context, double scale) const {
    if (!feature) {
        return DrawResult::kInvalidParameter;
    }
    
    GeometryPtr geom = feature->GetGeometry();
    if (!geom) {
        return DrawResult::kInvalidParameter;
    }
    
    auto symbolizers = GetSymbolizers(feature, scale);
    if (symbolizers.empty()) {
        return DrawResult::kSuccess;
    }
    
    DrawResult result = DrawResult::kSuccess;
    DrawContextPtr contextPtr(&context, [](DrawContext*) {});
    for (const auto& sym : symbolizers) {
        if (sym) {
            DrawResult r = sym->Symbolize(contextPtr, geom.get());
            if (r != DrawResult::kSuccess) {
                result = r;
            }
        }
    }
    
    if (m_ruleCallback) {
        auto matchingRules = GetMatchingRules(feature, scale);
        for (const auto& rule : matchingRules) {
            m_ruleCallback(rule.get(), feature);
        }
    }
    
    return result;
}

DrawResult RuleEngine::Render(const Geometry* geometry, DrawContext& context, double scale) const {
    if (!geometry) {
        return DrawResult::kInvalidParameter;
    }
    
    auto symbolizers = GetSymbolizers(geometry, scale);
    if (symbolizers.empty()) {
        return DrawResult::kSuccess;
    }
    
    DrawResult result = DrawResult::kSuccess;
    DrawContextPtr contextPtr(&context, [](DrawContext*) {});
    for (const auto& sym : symbolizers) {
        if (sym) {
            DrawResult r = sym->Symbolize(contextPtr, geometry);
            if (r != DrawResult::kSuccess) {
                result = r;
            }
        }
    }
    
    return result;
}

void RuleEngine::SortRulesByPriority() {
    std::stable_sort(m_rules.begin(), m_rules.end(),
        [](const SymbolizerRulePtr& a, const SymbolizerRulePtr& b) {
            if (!a) return false;
            if (!b) return true;
            return a->GetPriority() < b->GetPriority();
        });
}

void RuleEngine::SortRulesByName() {
    std::sort(m_rules.begin(), m_rules.end(),
        [](const SymbolizerRulePtr& a, const SymbolizerRulePtr& b) {
            if (!a) return false;
            if (!b) return true;
            return a->GetName() < b->GetName();
        });
}

Envelope RuleEngine::GetExtent() const {
    Envelope result;
    
    for (const auto& rule : m_rules) {
        if (rule && rule->HasExtent()) {
            result.ExpandToInclude(rule->GetExtent());
        }
    }
    
    return result;
}

double RuleEngine::GetMinScaleDenominator() const {
    double minScale = std::numeric_limits<double>::max();
    
    for (const auto& rule : m_rules) {
        if (rule) {
            double scale = rule->GetMinScaleDenominator();
            if (scale < minScale) {
                minScale = scale;
            }
        }
    }
    
    return minScale == std::numeric_limits<double>::max() ? 0.0 : minScale;
}

double RuleEngine::GetMaxScaleDenominator() const {
    double maxScale = 0.0;
    
    for (const auto& rule : m_rules) {
        if (rule) {
            double scale = rule->GetMaxScaleDenominator();
            if (scale > maxScale) {
                maxScale = scale;
            }
        }
    }
    
    return maxScale;
}

void RuleEngine::SetRuleCallback(std::function<void(const SymbolizerRule*, const CNFeature*)> callback) {
    m_ruleCallback = callback;
}

RuleEnginePtr RuleEngine::Clone() const {
    auto engine = std::make_shared<RuleEngine>();
    engine->SetElseFilterEnabled(m_elseFilterEnabled);
    
    for (const auto& rule : m_rules) {
        if (rule) {
            engine->AddRule(rule->Clone());
        }
    }
    
    return engine;
}

RuleEnginePtr RuleEngine::Create() {
    return std::make_shared<RuleEngine>();
}

bool RuleEngine::EvaluateRule(const SymbolizerRule& rule, const CNFeature* feature, double scale) const {
    if (!rule.IsScaleInRange(scale)) {
        return false;
    }
    
    return rule.Evaluate(feature);
}

bool RuleEngine::EvaluateRule(const SymbolizerRule& rule, const Geometry* geometry, double scale) const {
    if (!rule.IsScaleInRange(scale)) {
        return false;
    }
    
    return rule.Evaluate(geometry);
}

}
}
