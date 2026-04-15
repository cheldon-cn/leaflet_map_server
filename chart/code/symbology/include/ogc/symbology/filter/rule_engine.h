#ifndef OGC_DRAW_RULE_ENGINE_H
#define OGC_DRAW_RULE_ENGINE_H

#include "ogc/symbology/export.h"
#include "ogc/symbology/filter/symbolizer_rule.h"
#include <ogc/draw/draw_context.h>
#include "ogc/feature/feature.h"
#include <ogc/geom/envelope.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace ogc {
namespace symbology {

class RuleEngine;
typedef std::shared_ptr<RuleEngine> RuleEnginePtr;

struct RuleMatchResult {
    SymbolizerRulePtr rule;
    std::vector<SymbolizerPtr> symbolizers;
    bool matched;
    
    RuleMatchResult() : matched(false) {}
};

class OGC_SYMBOLOGY_API RuleEngine {
public:
    RuleEngine();
    ~RuleEngine() = default;
    
    void AddRule(SymbolizerRulePtr rule);
    void RemoveRule(SymbolizerRulePtr rule);
    void ClearRules();
    
    const std::vector<SymbolizerRulePtr>& GetRules() const { return m_rules; }
    size_t GetRuleCount() const { return m_rules.size(); }
    SymbolizerRulePtr GetRule(size_t index) const;
    SymbolizerRulePtr GetRuleByName(const std::string& name) const;
    
    std::vector<SymbolizerRulePtr> GetMatchingRules(const CNFeature* feature, double scale) const;
    std::vector<SymbolizerRulePtr> GetMatchingRules(const Geometry* geometry, double scale) const;
    
    RuleMatchResult Match(const CNFeature* feature, double scale) const;
    RuleMatchResult Match(const Geometry* geometry, double scale) const;
    
    std::vector<SymbolizerPtr> GetSymbolizers(const CNFeature* feature, double scale) const;
    std::vector<SymbolizerPtr> GetSymbolizers(const Geometry* geometry, double scale) const;
    
    draw::DrawResult Render(const CNFeature* feature, draw::DrawContext& context, double scale) const;
    draw::DrawResult Render(const Geometry* geometry, draw::DrawContext& context, double scale) const;
    
    void SortRulesByPriority();
    void SortRulesByName();
    
    Envelope GetExtent() const;
    
    double GetMinScaleDenominator() const;
    double GetMaxScaleDenominator() const;
    
    void SetElseFilterEnabled(bool enabled) { m_elseFilterEnabled = enabled; }
    bool IsElseFilterEnabled() const { return m_elseFilterEnabled; }
    
    void SetRuleCallback(std::function<void(const SymbolizerRule*, const CNFeature*)> callback);
    
    RuleEnginePtr Clone() const;
    
    static RuleEnginePtr Create();
    
private:
    bool EvaluateRule(const SymbolizerRule& rule, const CNFeature* feature, double scale) const;
    bool EvaluateRule(const SymbolizerRule& rule, const Geometry* geometry, double scale) const;
    
    std::vector<SymbolizerRulePtr> m_rules;
    bool m_elseFilterEnabled;
    std::function<void(const SymbolizerRule*, const CNFeature*)> m_ruleCallback;
};

}
}

#endif
