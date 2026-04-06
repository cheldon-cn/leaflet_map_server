#ifndef ALARM_CHECKER_ADAPTER_H
#define ALARM_CHECKER_ADAPTER_H

#include "i_alert_engine.h"
#include "ogc/alert/alert_checker.h"
#include "ogc/alert/alert_engine.h"
#include "alert_type_adapter.h"
#include <memory>
#include <map>

namespace alarm {

class CheckerAdapter : public alert::IAlertEngine {
public:
    explicit CheckerAdapter(ogc::alert::IAlertCheckerPtr checker);
    ~CheckerAdapter() override;
    
    alert::Alert Evaluate(const alert::AlertContext& context) override;
    
    alert::AlertType GetType() const override;
    
    void SetThreshold(const alert::ThresholdConfig& config) override;
    
    alert::ThresholdConfig GetThreshold() const override;
    
    std::string GetName() const override;
    
private:
    ogc::alert::CheckContext ConvertContext(const alert::AlertContext& context);
    
    ogc::alert::IAlertCheckerPtr m_checker;
};

class CheckerRegistry {
public:
    static CheckerRegistry& Instance();
    
    void RegisterChecker(alert::AlertType type, ogc::alert::IAlertCheckerPtr checker);
    void UnregisterChecker(alert::AlertType type);
    ogc::alert::IAlertCheckerPtr GetChecker(alert::AlertType type) const;
    std::unique_ptr<alert::IAlertEngine> CreateEngine(alert::AlertType type);
    
private:
    CheckerRegistry() = default;
    CheckerRegistry(const CheckerRegistry&) = delete;
    CheckerRegistry& operator=(const CheckerRegistry&) = delete;
    
    std::map<alert::AlertType, ogc::alert::IAlertCheckerPtr> m_checkers;
};

}

#endif
