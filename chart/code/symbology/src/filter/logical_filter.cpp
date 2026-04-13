#include "ogc/symbology/filter/logical_filter.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace symbology {

struct LogicalFilter::Impl {
    LogicalOperator op = LogicalOperator::kAnd;
    std::vector<FilterPtr> filters;
};

LogicalFilter::LogicalFilter(LogicalOperator op) : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
}

LogicalFilter::LogicalFilter(LogicalOperator op, const std::vector<FilterPtr>& filters)
    : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
    impl_->filters = filters;
}

LogicalFilter::LogicalFilter(LogicalOperator op, FilterPtr filter1, FilterPtr filter2)
    : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
    impl_->filters.push_back(filter1);
    impl_->filters.push_back(filter2);
}

LogicalFilter::~LogicalFilter() = default;

LogicalOperator LogicalFilter::GetOperator() const {
    return impl_->op;
}

void LogicalFilter::SetOperator(LogicalOperator op) {
    impl_->op = op;
}

const std::vector<FilterPtr>& LogicalFilter::GetFilters() const {
    return impl_->filters;
}

size_t LogicalFilter::GetFilterCount() const {
    return impl_->filters.size();
}

bool LogicalFilter::Evaluate(const CNFeature* feature) const {
    if (impl_->filters.empty()) {
        return impl_->op == LogicalOperator::kOr;
    }
    
    switch (impl_->op) {
        case LogicalOperator::kAnd: {
            for (const auto& filter : impl_->filters) {
                if (filter && !filter->Evaluate(feature)) {
                    return false;
                }
            }
            return true;
        }
        
        case LogicalOperator::kOr: {
            for (const auto& filter : impl_->filters) {
                if (filter && filter->Evaluate(feature)) {
                    return true;
                }
            }
            return false;
        }
        
        case LogicalOperator::kNot: {
            if (impl_->filters.empty()) {
                return true;
            }
            return !impl_->filters[0]->Evaluate(feature);
        }
        
        default:
            return false;
    }
}

bool LogicalFilter::Evaluate(const Geometry* geometry) const {
    if (impl_->filters.empty()) {
        return impl_->op == LogicalOperator::kOr;
    }
    
    switch (impl_->op) {
        case LogicalOperator::kAnd: {
            for (const auto& filter : impl_->filters) {
                if (filter && !filter->Evaluate(geometry)) {
                    return false;
                }
            }
            return true;
        }
        
        case LogicalOperator::kOr: {
            for (const auto& filter : impl_->filters) {
                if (filter && filter->Evaluate(geometry)) {
                    return true;
                }
            }
            return false;
        }
        
        case LogicalOperator::kNot: {
            if (impl_->filters.empty()) {
                return true;
            }
            return !impl_->filters[0]->Evaluate(geometry);
        }
        
        default:
            return false;
    }
}

std::string LogicalFilter::ToString() const {
    std::ostringstream oss;
    
    if (impl_->op == LogicalOperator::kNot) {
        oss << "NOT ";
        if (!impl_->filters.empty()) {
            oss << impl_->filters[0]->ToString();
        }
        return oss.str();
    }
    
    oss << "(";
    for (size_t i = 0; i < impl_->filters.size(); ++i) {
        if (i > 0) {
            oss << " " << OperatorToString(impl_->op) << " ";
        }
        if (impl_->filters[i]) {
            oss << impl_->filters[i]->ToString();
        }
    }
    oss << ")";
    
    return oss.str();
}

FilterPtr LogicalFilter::Clone() const {
    std::vector<FilterPtr> clonedFilters;
    for (const auto& filter : impl_->filters) {
        if (filter) {
            clonedFilters.push_back(filter->Clone());
        }
    }
    return std::make_shared<LogicalFilter>(impl_->op, clonedFilters);
}

void LogicalFilter::AddFilter(FilterPtr filter) {
    if (filter) {
        impl_->filters.push_back(filter);
    }
}

void LogicalFilter::RemoveFilter(FilterPtr filter) {
    impl_->filters.erase(
        std::remove(impl_->filters.begin(), impl_->filters.end(), filter),
        impl_->filters.end()
    );
}

void LogicalFilter::ClearFilters() {
    impl_->filters.clear();
}

FilterPtr LogicalFilter::GetFilter(size_t index) const {
    if (index < impl_->filters.size()) {
        return impl_->filters[index];
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
