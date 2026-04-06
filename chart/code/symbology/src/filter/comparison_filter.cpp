#include "ogc/symbology/filter/comparison_filter.h"
#include "ogc/feature/feature.h"
#include <algorithm>
#include <sstream>
#include <cmath>
#include <cctype>

namespace ogc {
namespace symbology {

ComparisonFilter::ComparisonFilter(ComparisonOperator op, 
                                   const std::string& propertyName,
                                   const std::string& literal)
    : m_operator(op)
    , m_propertyName(propertyName)
    , m_literal(literal)
{
}

ComparisonFilter::ComparisonFilter(ComparisonOperator op,
                                   const std::string& propertyName,
                                   const std::string& literal1,
                                   const std::string& literal2)
    : m_operator(op)
    , m_propertyName(propertyName)
    , m_literal(literal1)
    , m_literal2(literal2)
{
}

bool ComparisonFilter::Evaluate(const CNFeature* feature) const {
    if (!feature) {
        return false;
    }
    
    std::string actualValue;
    try {
        actualValue = feature->GetFieldAsString(m_propertyName.c_str());
    } catch (...) {
        return m_operator == ComparisonOperator::kIsNull;
    }
    
    if (actualValue.empty()) {
        return m_operator == ComparisonOperator::kIsNull;
    }
    
    return CompareValues(actualValue);
}

bool ComparisonFilter::Evaluate(const Geometry* geometry) const {
    (void)geometry;
    return false;
}

std::string ComparisonFilter::ToString() const {
    std::ostringstream oss;
    oss << "[" << m_propertyName << "] " 
        << OperatorToString(m_operator) << " ";
    
    if (m_operator == ComparisonOperator::kIsNull) {
        oss << "null";
    } else if (m_operator == ComparisonOperator::kBetween) {
        oss << m_literal << " AND " << m_literal2;
    } else {
        oss << m_literal;
    }
    
    return oss.str();
}

FilterPtr ComparisonFilter::Clone() const {
    return std::make_shared<ComparisonFilter>(*this);
}

std::string ComparisonFilter::OperatorToString(ComparisonOperator op) {
    switch (op) {
        case ComparisonOperator::kEqual: return "=";
        case ComparisonOperator::kNotEqual: return "<>";
        case ComparisonOperator::kLessThan: return "<";
        case ComparisonOperator::kGreaterThan: return ">";
        case ComparisonOperator::kLessThanOrEqual: return "<=";
        case ComparisonOperator::kGreaterThanOrEqual: return ">=";
        case ComparisonOperator::kLike: return "LIKE";
        case ComparisonOperator::kIsNull: return "IS NULL";
        case ComparisonOperator::kBetween: return "BETWEEN";
        default: return "?";
    }
}

ComparisonOperator ComparisonFilter::StringToOperator(const std::string& str) {
    if (str == "=" || str == "==" || str == "eq") {
        return ComparisonOperator::kEqual;
    } else if (str == "<>" || str == "!=" || str == "ne") {
        return ComparisonOperator::kNotEqual;
    } else if (str == "<" || str == "lt") {
        return ComparisonOperator::kLessThan;
    } else if (str == ">" || str == "gt") {
        return ComparisonOperator::kGreaterThan;
    } else if (str == "<=" || str == "le") {
        return ComparisonOperator::kLessThanOrEqual;
    } else if (str == ">=" || str == "ge") {
        return ComparisonOperator::kGreaterThanOrEqual;
    } else if (str == "LIKE" || str == "like") {
        return ComparisonOperator::kLike;
    } else if (str == "IS NULL" || str == "isnull") {
        return ComparisonOperator::kIsNull;
    } else if (str == "BETWEEN" || str == "between") {
        return ComparisonOperator::kBetween;
    }
    return ComparisonOperator::kEqual;
}

bool ComparisonFilter::CompareValues(const std::string& actualValue) const {
    switch (m_operator) {
        case ComparisonOperator::kEqual:
            return actualValue == m_literal;
            
        case ComparisonOperator::kNotEqual:
            return actualValue != m_literal;
            
        case ComparisonOperator::kLessThan:
        case ComparisonOperator::kGreaterThan:
        case ComparisonOperator::kLessThanOrEqual:
        case ComparisonOperator::kGreaterThanOrEqual:
            return CompareNumeric(actualValue);
            
        case ComparisonOperator::kLike:
            return MatchLikePattern(actualValue, m_literal);
            
        case ComparisonOperator::kIsNull:
            return actualValue.empty();
            
        case ComparisonOperator::kBetween:
            return CompareNumeric(actualValue);
            
        default:
            return false;
    }
}

bool ComparisonFilter::CompareNumeric(const std::string& actualValue) const {
    try {
        double actual = std::stod(actualValue);
        double literal = std::stod(m_literal);
        
        switch (m_operator) {
            case ComparisonOperator::kLessThan:
                return actual < literal;
            case ComparisonOperator::kGreaterThan:
                return actual > literal;
            case ComparisonOperator::kLessThanOrEqual:
                return actual <= literal;
            case ComparisonOperator::kGreaterThanOrEqual:
                return actual >= literal;
            case ComparisonOperator::kBetween: {
                double literal2 = std::stod(m_literal2);
                return actual >= literal && actual <= literal2;
            }
            default:
                return false;
        }
    } catch (...) {
        switch (m_operator) {
            case ComparisonOperator::kLessThan:
                return actualValue < m_literal;
            case ComparisonOperator::kGreaterThan:
                return actualValue > m_literal;
            case ComparisonOperator::kLessThanOrEqual:
                return actualValue <= m_literal;
            case ComparisonOperator::kGreaterThanOrEqual:
                return actualValue >= m_literal;
            default:
                return false;
        }
    }
}

bool ComparisonFilter::MatchLikePattern(const std::string& value, 
                                        const std::string& pattern) const {
    std::string regexPattern;
    for (char c : pattern) {
        switch (c) {
            case '%':
                regexPattern += ".*";
                break;
            case '_':
                regexPattern += ".";
                break;
            case '.': case '*': case '+': case '?':
            case '[': case ']': case '(': case ')':
            case '{': case '}': case '\\': case '^': case '$': case '|':
                regexPattern += '\\';
                regexPattern += c;
                break;
            default:
                regexPattern += c;
                break;
        }
    }
    
    std::string lowerValue = value;
    std::string lowerPattern = regexPattern;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
    
    size_t valuePos = 0;
    size_t patternPos = 0;
    
    while (valuePos < lowerValue.size() && patternPos < lowerPattern.size()) {
        if (lowerPattern[patternPos] == '.') {
            if (patternPos + 1 < lowerPattern.size() && lowerPattern[patternPos + 1] == '*') {
                patternPos += 2;
                if (patternPos >= lowerPattern.size()) {
                    return true;
                }
                while (valuePos < lowerValue.size() && 
                       lowerValue[valuePos] != lowerPattern[patternPos]) {
                    valuePos++;
                }
            } else {
                valuePos++;
                patternPos++;
            }
        } else if (lowerValue[valuePos] == lowerPattern[patternPos]) {
            valuePos++;
            patternPos++;
        } else {
            return false;
        }
    }
    
    return valuePos >= lowerValue.size() && patternPos >= lowerPattern.size();
}

}
}
