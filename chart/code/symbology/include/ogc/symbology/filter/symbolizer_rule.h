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
    ~SymbolizerRule();
    
    const std::string& GetName() const;
    void SetName(const std::string& name);
    
    const std::string& GetTitle() const;
    void SetTitle(const std::string& title);
    
    const std::string& GetAbstract() const;
    void SetAbstract(const std::string& abstract);
    
    FilterPtr GetFilter() const;
    void SetFilter(FilterPtr filter);
    bool HasFilter() const;
    
    bool Evaluate(const CNFeature* feature) const;
    bool Evaluate(const Geometry* geometry) const;
    
    double GetMinScaleDenominator() const;
    void SetMinScaleDenominator(double scale);
    
    double GetMaxScaleDenominator() const;
    void SetMaxScaleDenominator(double scale);
    
    bool IsScaleInRange(double scale) const;
    
    void AddSymbolizer(SymbolizerPtr symbolizer);
    void RemoveSymbolizer(SymbolizerPtr symbolizer);
    void ClearSymbolizers();
    
    const std::vector<SymbolizerPtr>& GetSymbolizers() const;
    size_t GetSymbolizerCount() const;
    SymbolizerPtr GetSymbolizer(size_t index) const;
    
    bool HasSymbolizers() const;
    
    const Envelope& GetExtent() const;
    void SetExtent(const Envelope& extent);
    bool HasExtent() const;
    
    bool IsElseFilter() const;
    void SetElseFilter(bool isElse);
    
    int GetPriority() const;
    void SetPriority(int priority);
    
    SymbolizerRulePtr Clone() const;
    
    static SymbolizerRulePtr Create();
    static SymbolizerRulePtr Create(const std::string& name);
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
