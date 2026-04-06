#include "alert/checker_adapter.h"
#include <mutex>

namespace alarm {

CheckerAdapter::CheckerAdapter(ogc::alert::IAlertCheckerPtr checker)
    : m_checker(checker) {
}

CheckerAdapter::~CheckerAdapter() {
}

alert::Alert CheckerAdapter::Evaluate(const alert::AlertContext& context) {
    if (!m_checker) {
        return alert::Alert();
    }
    
    ogc::alert::CheckContext check_context = ConvertContext(context);
    
    std::vector<ogc::alert::AlertPtr> alerts = m_checker->Check(check_context);
    
    if (alerts.empty()) {
        return alert::Alert();
    }
    
    return AlertTypeAdapter::FromAlertPtr(alerts[0]);
}

alert::AlertType CheckerAdapter::GetType() const {
    if (!m_checker) {
        return alert::AlertType::kOtherAlert;
    }
    
    return AlertTypeAdapter::FromAlertType(m_checker->GetAlertType());
}

void CheckerAdapter::SetThreshold(const alert::ThresholdConfig& config) {
    if (!m_checker) {
        return;
    }
    
    ogc::alert::AlertThreshold threshold;
    threshold.level1_threshold = config.GetLevel1Threshold();
    threshold.level2_threshold = config.GetLevel2Threshold();
    threshold.level3_threshold = config.GetLevel3Threshold();
    threshold.level4_threshold = config.GetLevel4Threshold();
    
    m_checker->SetThreshold(threshold);
}

alert::ThresholdConfig CheckerAdapter::GetThreshold() const {
    alert::ThresholdConfig config;
    
    if (!m_checker) {
        return config;
    }
    
    ogc::alert::AlertThreshold threshold = m_checker->GetThreshold();
    config.SetLevel1Threshold(threshold.level1_threshold);
    config.SetLevel2Threshold(threshold.level2_threshold);
    config.SetLevel3Threshold(threshold.level3_threshold);
    config.SetLevel4Threshold(threshold.level4_threshold);
    
    return config;
}

std::string CheckerAdapter::GetName() const {
    if (!m_checker) {
        return "Unknown";
    }
    return m_checker->GetCheckerId();
}

ogc::alert::CheckContext CheckerAdapter::ConvertContext(const alert::AlertContext& context) {
    ogc::alert::CheckContext result;
    
    result.ship_position.longitude = context.GetPosition().GetLongitude();
    result.ship_position.latitude = context.GetPosition().GetLatitude();
    result.ship_speed = context.GetSpeed();
    result.ship_heading = context.GetHeading();
    result.ship_id = context.GetShipInfo().GetShipId();
    result.user_id = context.GetShipInfo().GetShipId();
    
    return result;
}

CheckerRegistry& CheckerRegistry::Instance() {
    static CheckerRegistry instance;
    return instance;
}

void CheckerRegistry::RegisterChecker(alert::AlertType type, ogc::alert::IAlertCheckerPtr checker) {
    m_checkers[type] = checker;
}

void CheckerRegistry::UnregisterChecker(alert::AlertType type) {
    m_checkers.erase(type);
}

ogc::alert::IAlertCheckerPtr CheckerRegistry::GetChecker(alert::AlertType type) const {
    auto it = m_checkers.find(type);
    if (it != m_checkers.end()) {
        return it->second;
    }
    return nullptr;
}

std::unique_ptr<alert::IAlertEngine> CheckerRegistry::CreateEngine(alert::AlertType type) {
    auto checker = GetChecker(type);
    if (!checker) {
        return nullptr;
    }
    return std::unique_ptr<alert::IAlertEngine>(new CheckerAdapter(checker));
}

}
