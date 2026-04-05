#ifndef ALERT_SYSTEM_ALERT_CONFIG_SERVICE_H
#define ALERT_SYSTEM_ALERT_CONFIG_SERVICE_H

#include "alert/alert_types.h"
#include "alert/threshold_config.h"
#include <string>
#include <map>
#include <vector>
#include <functional>

namespace alert {

struct AlertRule {
    std::string ruleId;
    std::string name;
    AlertType alertType;
    bool enabled;
    int priority;
    std::map<std::string, std::string> parameters;
    
    AlertRule() : alertType(AlertType::kDepthAlert), enabled(true), priority(0) {}
};

struct NotificationConfig {
    bool enableSound;
    bool enablePopup;
    bool enableEmail;
    bool enableSms;
    std::vector<std::string> emailRecipients;
    std::vector<std::string> smsRecipients;
    
    NotificationConfig()
        : enableSound(true)
        , enablePopup(true)
        , enableEmail(false)
        , enableSms(false) {}
};

class IAlertConfigService {
public:
    virtual ~IAlertConfigService() {}
    
    virtual ThresholdConfig GetThreshold(AlertType type) const = 0;
    virtual void SetThreshold(AlertType type, const ThresholdConfig& config) = 0;
    
    virtual AlertRule GetRule(const std::string& ruleId) const = 0;
    virtual void SetRule(const std::string& ruleId, const AlertRule& rule) = 0;
    virtual std::vector<AlertRule> GetRulesByType(AlertType type) const = 0;
    virtual void DeleteRule(const std::string& ruleId) = 0;
    
    virtual NotificationConfig GetNotificationConfig() const = 0;
    virtual void SetNotificationConfig(const NotificationConfig& config) = 0;
    
    virtual bool IsAlertTypeEnabled(AlertType type) const = 0;
    virtual void SetAlertTypeEnabled(AlertType type, bool enabled) = 0;
    
    virtual void LoadFromFile(const std::string& filePath) = 0;
    virtual void SaveToFile(const std::string& filePath) const = 0;
    
    virtual void SetConfigChangeCallback(std::function<void(const std::string&)> callback) = 0;
};

class AlertConfigService : public IAlertConfigService {
public:
    AlertConfigService();
    virtual ~AlertConfigService();
    
    ThresholdConfig GetThreshold(AlertType type) const override;
    void SetThreshold(AlertType type, const ThresholdConfig& config) override;
    
    AlertRule GetRule(const std::string& ruleId) const override;
    void SetRule(const std::string& ruleId, const AlertRule& rule) override;
    std::vector<AlertRule> GetRulesByType(AlertType type) const override;
    void DeleteRule(const std::string& ruleId) override;
    
    NotificationConfig GetNotificationConfig() const override;
    void SetNotificationConfig(const NotificationConfig& config) override;
    
    bool IsAlertTypeEnabled(AlertType type) const override;
    void SetAlertTypeEnabled(AlertType type, bool enabled) override;
    
    void LoadFromFile(const std::string& filePath) override;
    void SaveToFile(const std::string& filePath) const override;
    
    void SetConfigChangeCallback(std::function<void(const std::string&)> callback) override;
    
    void ResetToDefaults();
    std::vector<AlertType> GetEnabledAlertTypes() const;
    
private:
    std::map<AlertType, ThresholdConfig> m_thresholds;
    std::map<std::string, AlertRule> m_rules;
    std::map<AlertType, bool> m_enabledTypes;
    NotificationConfig m_notificationConfig;
    std::function<void(const std::string&)> m_changeCallback;
    
    void NotifyChange(const std::string& changeDesc);
    void InitializeDefaults();
};

}

#endif
