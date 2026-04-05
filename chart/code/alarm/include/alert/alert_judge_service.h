#ifndef ALERT_SYSTEM_ALERT_JUDGE_SERVICE_H
#define ALERT_SYSTEM_ALERT_JUDGE_SERVICE_H

#include "alert/alert.h"
#include "alert/alert_context.h"
#include "alert/alert_types.h"
#include "alert/i_alert_engine.h"
#include "alert/threshold_config.h"
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

class IAlertJudgeService {
public:
    virtual ~IAlertJudgeService() {}
    
    virtual std::vector<Alert> Evaluate(const AlertContext& context) = 0;
    virtual std::vector<Alert> EvaluateByType(const AlertContext& context, AlertType type) = 0;
    virtual std::vector<Alert> EvaluateByTypes(const AlertContext& context, const std::vector<AlertType>& types) = 0;
    
    virtual void EnableEngine(AlertType type, bool enable) = 0;
    virtual bool IsEngineEnabled(AlertType type) const = 0;
    
    virtual void SetEngineThreshold(AlertType type, const ThresholdConfig& config) = 0;
    virtual ThresholdConfig GetEngineThreshold(AlertType type) const = 0;
    
    virtual std::vector<AlertType> GetSupportedTypes() const = 0;
};

class AlertJudgeService : public IAlertJudgeService {
public:
    AlertJudgeService();
    virtual ~AlertJudgeService();
    
    std::vector<Alert> Evaluate(const AlertContext& context) override;
    std::vector<Alert> EvaluateByType(const AlertContext& context, AlertType type) override;
    std::vector<Alert> EvaluateByTypes(const AlertContext& context, const std::vector<AlertType>& types) override;
    
    void EnableEngine(AlertType type, bool enable) override;
    bool IsEngineEnabled(AlertType type) const override;
    
    void SetEngineThreshold(AlertType type, const ThresholdConfig& config) override;
    ThresholdConfig GetEngineThreshold(AlertType type) const override;
    
    std::vector<AlertType> GetSupportedTypes() const override;
    
    void RegisterEngine(AlertType type, IAlertEngine* engine);
    void UnregisterEngine(AlertType type);
    
    void SetMaxAlertCount(size_t count);
    size_t GetMaxAlertCount() const;
    
    void SetAlertFilter(std::function<bool(const Alert&)> filter);
    
private:
    std::map<AlertType, IAlertEngine*> m_engines;
    std::map<AlertType, bool> m_enabledEngines;
    std::map<AlertType, ThresholdConfig> m_thresholds;
    size_t m_maxAlertCount;
    std::function<bool(const Alert&)> m_filter;
    
    bool ShouldIncludeAlert(const Alert& alert) const;
    std::vector<Alert> SortAlertsByPriority(const std::vector<Alert>& alerts) const;
};

}

#endif
