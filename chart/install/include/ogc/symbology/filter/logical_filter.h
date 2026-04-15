#ifndef OGC_SYMBOLOGY_LOGICAL_FILTER_H
#define OGC_SYMBOLOGY_LOGICAL_FILTER_H

#include "ogc/symbology/filter/filter.h"
#include <vector>

namespace ogc {
namespace symbology {

enum class LogicalOperator {
    kAnd,
    kOr,
    kNot
};

class OGC_SYMBOLOGY_API LogicalFilter : public Filter {
public:
    explicit LogicalFilter(LogicalOperator op);
    LogicalFilter(LogicalOperator op, const std::vector<FilterPtr>& filters);
    LogicalFilter(LogicalOperator op, FilterPtr filter1, FilterPtr filter2);
    ~LogicalFilter() override;
    
    FilterType GetType() const override { return FilterType::kLogical; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    std::string ToString() const override;
    FilterPtr Clone() const override;
    
    LogicalOperator GetOperator() const;
    void SetOperator(LogicalOperator op);
    
    void AddFilter(FilterPtr filter);
    void RemoveFilter(FilterPtr filter);
    void ClearFilters();
    
    const std::vector<FilterPtr>& GetFilters() const;
    size_t GetFilterCount() const;
    FilterPtr GetFilter(size_t index) const;
    
    static std::string OperatorToString(LogicalOperator op);
    static LogicalOperator StringToOperator(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

typedef std::shared_ptr<LogicalFilter> LogicalFilterPtr;

}
}

#endif
