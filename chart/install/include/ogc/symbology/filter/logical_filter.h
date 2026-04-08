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
    
    FilterType GetType() const override { return FilterType::kLogical; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    std::string ToString() const override;
    FilterPtr Clone() const override;
    
    LogicalOperator GetOperator() const { return m_operator; }
    void SetOperator(LogicalOperator op) { m_operator = op; }
    
    void AddFilter(FilterPtr filter);
    void RemoveFilter(FilterPtr filter);
    void ClearFilters();
    
    const std::vector<FilterPtr>& GetFilters() const { return m_filters; }
    size_t GetFilterCount() const { return m_filters.size(); }
    FilterPtr GetFilter(size_t index) const;
    
    static std::string OperatorToString(LogicalOperator op);
    static LogicalOperator StringToOperator(const std::string& str);
    
private:
    LogicalOperator m_operator;
    std::vector<FilterPtr> m_filters;
};

typedef std::shared_ptr<LogicalFilter> LogicalFilterPtr;

}
}

#endif
