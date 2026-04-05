#ifndef OGC_ALERT_PROCESSOR_H
#define OGC_ALERT_PROCESSOR_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

class IDeduplicationStrategy;
using IDeduplicationStrategyPtr = std::shared_ptr<IDeduplicationStrategy>;

class IAggregationStrategy;
using IAggregationStrategyPtr = std::shared_ptr<IAggregationStrategy>;

class OGC_ALERT_API IDeduplicationStrategy {
public:
    virtual ~IDeduplicationStrategy() = default;
    
    virtual bool IsDuplicate(const AlertPtr& alert1, const AlertPtr& alert2) const = 0;
    virtual AlertPtr Merge(const AlertPtr& alert1, const AlertPtr& alert2) const = 0;
    
    static IDeduplicationStrategyPtr CreateDefault();
};

class OGC_ALERT_API IAggregationStrategy {
public:
    virtual ~IAggregationStrategy() = default;
    
    virtual std::vector<AlertPtr> Aggregate(const std::vector<AlertPtr>& alerts) const = 0;
    
    static IAggregationStrategyPtr CreateDefault();
};

class OGC_ALERT_API IAlertProcessor {
public:
    virtual ~IAlertProcessor() = default;
    
    virtual std::vector<AlertPtr> Process(const std::vector<AlertPtr>& alerts) = 0;
    
    virtual void SetDeduplicationStrategy(IDeduplicationStrategyPtr strategy) = 0;
    virtual IDeduplicationStrategyPtr GetDeduplicationStrategy() const = 0;
    
    virtual void SetAggregationStrategy(IAggregationStrategyPtr strategy) = 0;
    virtual IAggregationStrategyPtr GetAggregationStrategy() const = 0;
    
    virtual void AddFilter(std::function<bool(const AlertPtr&)> filter) = 0;
    virtual void ClearFilters() = 0;
    
    static std::unique_ptr<IAlertProcessor> Create();
};

}
}

#endif
