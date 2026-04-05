#include "ogc/alert/alert_processor.h"
#include "ogc/draw/log.h"
#include <algorithm>
#include <map>

namespace ogc {
namespace alert {

class DefaultDeduplicationStrategy : public IDeduplicationStrategy {
public:
    bool IsDuplicate(const AlertPtr& alert1, const AlertPtr& alert2) const override {
        if (alert1->alert_type != alert2->alert_type) return false;
        if (alert1->user_id != alert2->user_id) return false;
        return alert1->position == alert2->position;
    }
    
    AlertPtr Merge(const AlertPtr& alert1, const AlertPtr& alert2) const override {
        if (alert1->alert_level > alert2->alert_level) {
            return alert1;
        }
        return alert2;
    }
};

class DefaultAggregationStrategy : public IAggregationStrategy {
public:
    std::vector<AlertPtr> Aggregate(const std::vector<AlertPtr>& alerts) const override {
        return alerts;
    }
};

IDeduplicationStrategyPtr IDeduplicationStrategy::CreateDefault() {
    return std::make_shared<DefaultDeduplicationStrategy>();
}

IAggregationStrategyPtr IAggregationStrategy::CreateDefault() {
    return std::make_shared<DefaultAggregationStrategy>();
}

class AlertProcessor : public IAlertProcessor {
public:
    AlertProcessor() 
        : m_dedupStrategy(IDeduplicationStrategy::CreateDefault()),
          m_aggStrategy(IAggregationStrategy::CreateDefault()) {}
    
    std::vector<AlertPtr> Process(const std::vector<AlertPtr>& alerts) override {
        std::vector<AlertPtr> result = alerts;
        
        result = ApplyFilters(result);
        result = Deduplicate(result);
        result = Aggregate(result);
        
        return result;
    }
    
    void SetDeduplicationStrategy(IDeduplicationStrategyPtr strategy) override {
        m_dedupStrategy = strategy;
    }
    
    IDeduplicationStrategyPtr GetDeduplicationStrategy() const override {
        return m_dedupStrategy;
    }
    
    void SetAggregationStrategy(IAggregationStrategyPtr strategy) override {
        m_aggStrategy = strategy;
    }
    
    IAggregationStrategyPtr GetAggregationStrategy() const override {
        return m_aggStrategy;
    }
    
    void AddFilter(std::function<bool(const AlertPtr&)> filter) override {
        m_filters.push_back(filter);
    }
    
    void ClearFilters() override {
        m_filters.clear();
    }
    
private:
    std::vector<AlertPtr> ApplyFilters(const std::vector<AlertPtr>& alerts) {
        if (m_filters.empty()) return alerts;
        
        std::vector<AlertPtr> result;
        for (const auto& alert : alerts) {
            bool passed = true;
            for (const auto& filter : m_filters) {
                if (!filter(alert)) {
                    passed = false;
                    break;
                }
            }
            if (passed) {
                result.push_back(alert);
            }
        }
        return result;
    }
    
    std::vector<AlertPtr> Deduplicate(const std::vector<AlertPtr>& alerts) {
        if (!m_dedupStrategy || alerts.empty()) return alerts;
        
        std::vector<AlertPtr> result;
        std::map<std::string, AlertPtr> uniqueAlerts;
        
        for (const auto& alert : alerts) {
            std::string key = std::to_string(static_cast<int>(alert->alert_type)) + "_" + alert->user_id;
            auto it = uniqueAlerts.find(key);
            if (it == uniqueAlerts.end()) {
                uniqueAlerts[key] = alert;
            } else {
                uniqueAlerts[key] = m_dedupStrategy->Merge(it->second, alert);
            }
        }
        
        for (const auto& pair : uniqueAlerts) {
            result.push_back(pair.second);
        }
        
        return result;
    }
    
    std::vector<AlertPtr> Aggregate(const std::vector<AlertPtr>& alerts) {
        if (!m_aggStrategy) return alerts;
        return m_aggStrategy->Aggregate(alerts);
    }
    
    IDeduplicationStrategyPtr m_dedupStrategy;
    IAggregationStrategyPtr m_aggStrategy;
    std::vector<std::function<bool(const AlertPtr&)>> m_filters;
};

std::unique_ptr<IAlertProcessor> IAlertProcessor::Create() {
    return std::unique_ptr<IAlertProcessor>(new AlertProcessor());
}

}
}
