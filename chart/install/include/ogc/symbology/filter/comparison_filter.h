#ifndef OGC_SYMBOLOGY_COMPARISON_FILTER_H
#define OGC_SYMBOLOGY_COMPARISON_FILTER_H

#include "ogc/symbology/filter/filter.h"
#include <string>
#include <vector>

namespace ogc {
namespace symbology {

enum class ComparisonOperator {
    kEqual,
    kNotEqual,
    kLessThan,
    kGreaterThan,
    kLessThanOrEqual,
    kGreaterThanOrEqual,
    kLike,
    kIsNull,
    kBetween
};

class OGC_SYMBOLOGY_API ComparisonFilter : public Filter {
public:
    ComparisonFilter(ComparisonOperator op, 
                     const std::string& propertyName,
                     const std::string& literal);
    
    ComparisonFilter(ComparisonOperator op,
                     const std::string& propertyName,
                     const std::string& literal1,
                     const std::string& literal2);
    
    FilterType GetType() const override { return FilterType::kComparison; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    std::string ToString() const override;
    FilterPtr Clone() const override;
    
    ComparisonOperator GetOperator() const { return m_operator; }
    const std::string& GetPropertyName() const { return m_propertyName; }
    const std::string& GetLiteral() const { return m_literal; }
    const std::string& GetLiteral2() const { return m_literal2; }
    
    void SetOperator(ComparisonOperator op) { m_operator = op; }
    void SetPropertyName(const std::string& name) { m_propertyName = name; }
    void SetLiteral(const std::string& literal) { m_literal = literal; }
    void SetLiteral2(const std::string& literal) { m_literal2 = literal; }
    
    static std::string OperatorToString(ComparisonOperator op);
    static ComparisonOperator StringToOperator(const std::string& str);
    
private:
    bool CompareValues(const std::string& actualValue) const;
    bool CompareNumeric(const std::string& actualValue) const;
    bool MatchLikePattern(const std::string& value, const std::string& pattern) const;
    
    ComparisonOperator m_operator;
    std::string m_propertyName;
    std::string m_literal;
    std::string m_literal2;
};

typedef std::shared_ptr<ComparisonFilter> ComparisonFilterPtr;

}
}

#endif
