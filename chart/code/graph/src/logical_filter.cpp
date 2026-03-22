#include "ogc/draw/logical_filter.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace draw {

LogicalFilter::LogicalFilter(LogicalOperator op)
    : m_operator(op)
{
}

LogicalFilter::LogicalFilter(LogicalOperator op, const std::vector<FilterPtr>& filters)
    : m_operator(op)
    , m_filters(filters)
{
}

LogicalFilter::LogicalFilter(LogicalOperator op, FilterPtr filter1, FilterPtr filter2)
    : m_operator(op)
{
    m_filters.push_back(filter1);
    m_filters.push_back(filter2);
}

bool LogicalFilter::Evaluate(const CNFeature* feature) const {
    if (m_filters.empty()) {
        return m_operator == LogicalOperator::kOr;
    }
    
    switch (m_operator) {
        case LogicalOperator::kAnd: {
            for (const auto& filter : m_filters) {
                if (filter && !filter->Evaluate(feature)) {
                    return false;
                }
            }
            return true;
        }
        
        case LogicalOperator::kOr: {
            for (const auto& filter : m_filters) {
                if (filter && filter->Evaluate(feature)) {
                    return true;
                }
            }
            return false;
        }
        
        case LogicalOperator::kNot: {
            if (m_filters.empty()) {
                return true;
            }
            return !m_filters[0]->Evaluate(feature);
        }
        
        default:
            return false;
    }
}

bool LogicalFilter::Evaluate(const Geometry* geometry) const {
    if (m_filters.empty()) {
        return m_operator == LogicalOperator::kOr;
    }
    
    switch (m_operator) {
        case LogicalOperator::kAnd: {
            for (const auto& filter : m_filters) {
                if (filter && !filter->Evaluate(geometry)) {
                    return false;
                }
            }
            return true;
        }
        
        case LogicalOperator::kOr: {
            for (const auto& filter : m_filters) {
                if (filter && filter->Evaluate(geometry)) {
                    return true;
                }
            }
            return false;
        }
        
        case LogicalOperator::kNot: {
            if (m_filters.empty()) {
                return true;
            }
            return !m_filters[0]->Evaluate(geometry);
        }
        
        default:
            return false;
    }
}

std::string LogicalFilter::ToString() const {
    std::ostringstream oss;
    
    if (m_operator == LogicalOperator::kNot) {
        oss << "NOT ";
        if (!m_filters.empty()) {
            oss << m_filters[0]->ToString();
        }
        return oss.str();
    }
    
    oss << "(";
    for (size_t i = 0; i < m_filters.size(); ++i) {
        if (i > 0) {
            oss << " " << OperatorToString(m_operator) << " ";
        }
        if (m_filters[i]) {
            oss << m_filters[i]->ToString();
        }
    }
    oss << ")";
    
    return oss.str();
}

FilterPtr LogicalFilter::Clone() const {
    std::vector<FilterPtr> clonedFilters;
    for (const auto& filter : m_filters) {
        if (filter) {
            clonedFilters.push_back(filter->Clone());
        }
    }
    return std::make_shared<LogicalFilter>(m_operator, clonedFilters);
}

void LogicalFilter::AddFilter(FilterPtr filter) {
    if (filter) {
        m_filters.push_back(filter);
    }
}

void LogicalFilter::RemoveFilter(FilterPtr filter) {
    m_filters.erase(
        std::remove(m_filters.begin(), m_filters.end(), filter),
        m_filters.end()
    );
}

void LogicalFilter::ClearFilters() {
    m_filters.clear();
}

FilterPtr LogicalFilter::GetFilter(size_t index) const {
    if (index < m_filters.size()) {
        return m_filters[index];
    }
    return nullptr;
}

std::string LogicalFilter::OperatorToString(LogicalOperator op) {
    switch (op) {
        case LogicalOperator::kAnd: return "AND";
        case LogicalOperator::kOr: return "OR";
        case LogicalOperator::kNot: return "NOT";
        default: return "?";
    }
}

LogicalOperator LogicalFilter::StringToOperator(const std::string& str) {
    if (str == "AND" || str == "and" || str == "&&") {
        return LogicalOperator::kAnd;
    } else if (str == "OR" || str == "or" || str == "||") {
        return LogicalOperator::kOr;
    } else if (str == "NOT" || str == "not" || str == "!") {
        return LogicalOperator::kNot;
    }
    return LogicalOperator::kAnd;
}

}
}
