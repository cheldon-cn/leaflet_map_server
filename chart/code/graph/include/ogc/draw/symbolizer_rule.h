#ifndef OGC_DRAW_SYMBOLIZER_RULE_H
#define OGC_DRAW_SYMBOLIZER_RULE_H

#include "ogc/draw/filter.h"
#include "ogc/draw/symbolizer.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

class SymbolizerRule;
typedef std::shared_ptr<SymbolizerRule> SymbolizerRulePtr;

class SymbolizerRule {
public:
    SymbolizerRule();
    explicit SymbolizerRule(const std::string& name);
    ~SymbolizerRule() = default;
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    const std::string& GetTitle() const { return m_title; }
    void SetTitle(const std::string& title) { m_title = title; }
    
    const std::string& GetAbstract() const { return m_abstract; }
    void SetAbstract(const std::string& abstract) { m_abstract = abstract; }
    
    FilterPtr GetFilter() const { return m_filter; }
    void SetFilter(FilterPtr filter) { m_filter = filter; }
    bool HasFilter() const { return m_filter != nullptr; }
    
    bool Evaluate(const CNFeature* feature) const;
    bool Evaluate(const Geometry* geometry) const;
    
    double GetMinScaleDenominator() const { return m_minScaleDenominator; }
    void SetMinScaleDenominator(double scale) { m_minScaleDenominator = scale; }
    
    double GetMaxScaleDenominator() const { return m_maxScaleDenominator; }
    void SetMaxScaleDenominator(double scale) { m_maxScaleDenominator = scale; }
    
    bool IsScaleInRange(double scale) const;
    
    void AddSymbolizer(SymbolizerPtr symbolizer);
    void RemoveSymbolizer(SymbolizerPtr symbolizer);
    void ClearSymbolizers();
    
    const std::vector<SymbolizerPtr>& GetSymbolizers() const { return m_symbolizers; }
    size_t GetSymbolizerCount() const { return m_symbolizers.size(); }
    SymbolizerPtr GetSymbolizer(size_t index) const;
    
    bool HasSymbolizers() const { return !m_symbolizers.empty(); }
    
    const Envelope& GetExtent() const { return m_extent; }
    void SetExtent(const Envelope& extent) { m_extent = extent; }
    bool HasExtent() const { return !m_extent.IsNull(); }
    
    bool IsElseFilter() const { return m_isElseFilter; }
    void SetElseFilter(bool isElse) { m_isElseFilter = isElse; }
    
    int GetPriority() const { return m_priority; }
    void SetPriority(int priority) { m_priority = priority; }
    
    SymbolizerRulePtr Clone() const;
    
    static SymbolizerRulePtr Create();
    static SymbolizerRulePtr Create(const std::string& name);
    
private:
    std::string m_name;
    std::string m_title;
    std::string m_abstract;
    FilterPtr m_filter;
    double m_minScaleDenominator;
    double m_maxScaleDenominator;
    std::vector<SymbolizerPtr> m_symbolizers;
    Envelope m_extent;
    bool m_isElseFilter;
    int m_priority;
};

}
}

#endif
