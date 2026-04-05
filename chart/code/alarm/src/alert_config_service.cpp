#include "alert/alert_config_service.h"
#include "alert/alert_exception.h"
#include <fstream>
#include <sstream>

namespace alert {

AlertConfigService::AlertConfigService() {
    InitializeDefaults();
}

AlertConfigService::~AlertConfigService() {}

void AlertConfigService::InitializeDefaults() {
    ThresholdConfig depthThreshold;
    depthThreshold.SetLevel1Threshold(0.3);
    depthThreshold.SetLevel2Threshold(0.5);
    depthThreshold.SetLevel3Threshold(1.0);
    depthThreshold.SetLevel4Threshold(2.0);
    m_thresholds[AlertType::kDepthAlert] = depthThreshold;
    
    ThresholdConfig collisionThreshold;
    collisionThreshold.SetParameter("cpa_threshold", 0.5);
    collisionThreshold.SetParameter("tcpa_threshold", 600.0);
    m_thresholds[AlertType::kCollisionAlert] = collisionThreshold;
    
    m_enabledTypes[AlertType::kDepthAlert] = true;
    m_enabledTypes[AlertType::kCollisionAlert] = true;
    m_enabledTypes[AlertType::kWeatherAlert] = true;
    m_enabledTypes[AlertType::kChannelAlert] = true;
    m_enabledTypes[AlertType::kOtherAlert] = false;
}

ThresholdConfig AlertConfigService::GetThreshold(AlertType type) const {
    auto it = m_thresholds.find(type);
    if (it != m_thresholds.end()) {
        return it->second;
    }
    return ThresholdConfig();
}

void AlertConfigService::SetThreshold(AlertType type, const ThresholdConfig& config) {
    m_thresholds[type] = config;
    NotifyChange("Threshold updated for " + AlertTypeToString(type));
}

AlertRule AlertConfigService::GetRule(const std::string& ruleId) const {
    auto it = m_rules.find(ruleId);
    if (it != m_rules.end()) {
        return it->second;
    }
    return AlertRule();
}

void AlertConfigService::SetRule(const std::string& ruleId, const AlertRule& rule) {
    m_rules[ruleId] = rule;
    NotifyChange("Rule updated: " + ruleId);
}

std::vector<AlertRule> AlertConfigService::GetRulesByType(AlertType type) const {
    std::vector<AlertRule> result;
    for (const auto& pair : m_rules) {
        if (pair.second.alertType == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void AlertConfigService::DeleteRule(const std::string& ruleId) {
    auto it = m_rules.find(ruleId);
    if (it != m_rules.end()) {
        m_rules.erase(it);
        NotifyChange("Rule deleted: " + ruleId);
    }
}

NotificationConfig AlertConfigService::GetNotificationConfig() const {
    return m_notificationConfig;
}

void AlertConfigService::SetNotificationConfig(const NotificationConfig& config) {
    m_notificationConfig = config;
    NotifyChange("Notification config updated");
}

bool AlertConfigService::IsAlertTypeEnabled(AlertType type) const {
    auto it = m_enabledTypes.find(type);
    if (it != m_enabledTypes.end()) {
        return it->second;
    }
    return false;
}

void AlertConfigService::SetAlertTypeEnabled(AlertType type, bool enabled) {
    m_enabledTypes[type] = enabled;
    NotifyChange("Alert type " + AlertTypeToString(type) + (enabled ? " enabled" : " disabled"));
}

void AlertConfigService::LoadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw FileNotFoundException("Cannot open config file: " + filePath);
    }
    
    std::string line;
    while (std::getline(file, line)) {
    }
    
    NotifyChange("Config loaded from " + filePath);
}

void AlertConfigService::SaveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw FileWriteException("Cannot write to config file: " + filePath);
    }
    
    file << "# Alert System Configuration\n";
    file << "# Auto-generated\n\n";
    
    file << "[Thresholds]\n";
    for (const auto& pair : m_thresholds) {
        file << AlertTypeToString(pair.first) << ".level1=" << pair.second.GetLevel1Threshold() << "\n";
        file << AlertTypeToString(pair.first) << ".level2=" << pair.second.GetLevel2Threshold() << "\n";
        file << AlertTypeToString(pair.first) << ".level3=" << pair.second.GetLevel3Threshold() << "\n";
        file << AlertTypeToString(pair.first) << ".level4=" << pair.second.GetLevel4Threshold() << "\n";
    }
    
    file << "\n[EnabledTypes]\n";
    for (const auto& pair : m_enabledTypes) {
        file << AlertTypeToString(pair.first) << "=" << (pair.second ? "true" : "false") << "\n";
    }
}

void AlertConfigService::SetConfigChangeCallback(std::function<void(const std::string&)> callback) {
    m_changeCallback = callback;
}

void AlertConfigService::ResetToDefaults() {
    m_thresholds.clear();
    m_rules.clear();
    m_enabledTypes.clear();
    InitializeDefaults();
    NotifyChange("Config reset to defaults");
}

std::vector<AlertType> AlertConfigService::GetEnabledAlertTypes() const {
    std::vector<AlertType> result;
    for (const auto& pair : m_enabledTypes) {
        if (pair.second) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void AlertConfigService::NotifyChange(const std::string& changeDesc) {
    if (m_changeCallback) {
        m_changeCallback(changeDesc);
    }
}

}
