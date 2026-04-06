#ifndef OGC_SYMBOLOGY_FILTER_SYMBOLIZER_RULE_H
#define OGC_SYMBOLOGY_FILTER_SYMBOLIZER_RULE_H

#include "ogc/symbology/export.h"
#include "ogc/symbology/filter/filter.h"
#include "ogc/symbology/symbolizer/symbolizer.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace symbology {

class SymbolizerRule;
typedef std::shared_ptr<SymbolizerRule> SymbolizerRulePtr;

class OGC_SYMBOLOGY_API SymbolizerRule {
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
    bool m_isElseFilter;
    int m_priority;
};

}
}

#endif
