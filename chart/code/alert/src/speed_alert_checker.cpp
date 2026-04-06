#include "ogc/alert/speed_alert_checker.h"
#include "ogc/base/log.h"

namespace ogc {
namespace alert {

class SpeedAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(6) {
        m_speedThreshold.level1_ratio = 1.1;
        m_speedThreshold.level2_ratio = 1.2;
        m_speedThreshold.level3_ratio = 1.3;
    }
    
    std::string GetCheckerId() const { return "speed_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kSpeed; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = threshold;
    }
    
    AlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetSpeedLimitData(std::shared_ptr<void> speed_limit_data) {
        m_speedLimitData = speed_limit_data;
    }
    
    double GetSpeedLimit(const Coordinate& position) const {
        return 10.0;
    }
    
    AlertLevel DetermineSpeedLevel(double current_speed, double speed_limit) const {
        if (speed_limit <= 0) return AlertLevel::kNone;
        
        double ratio = current_speed / speed_limit;
        
        if (ratio >= m_speedThreshold.level3_ratio) return AlertLevel::kLevel4;
        if (ratio >= m_speedThreshold.level2_ratio) return AlertLevel::kLevel3;
        if (ratio >= m_speedThreshold.level1_ratio) return AlertLevel::kLevel2;
        return AlertLevel::kNone;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        double speed_limit = GetSpeedLimit(context.ship_position);
        double current_speed = context.ship_speed;
        
        AlertLevel level = DetermineSpeedLevel(current_speed, speed_limit);
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<SpeedAlert>();
            alert->alert_id = "SPEED_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kSpeed;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->current_speed = current_speed;
            alert->speed_limit = speed_limit;
            alert->over_speed_ratio = current_speed / speed_limit;
            alert->zone_id = "ZONE_001";
            
            alert->content.type = "Speed";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Speed Alert";
            alert->content.message = "Speed: " + std::to_string(current_speed) + " kn, Limit: " + std::to_string(speed_limit) + " kn";
            alert->content.position = context.ship_position;
            alert->content.action_required = "Reduce speed";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    AlertThreshold m_threshold;
    SpeedThreshold m_speedThreshold;
    std::shared_ptr<void> m_speedLimitData;
};

SpeedAlertChecker::SpeedAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

SpeedAlertChecker::~SpeedAlertChecker() {
}

std::string SpeedAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType SpeedAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int SpeedAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool SpeedAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void SpeedAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> SpeedAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void SpeedAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold SpeedAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void SpeedAlertChecker::SetSpeedLimitData(std::shared_ptr<void> speed_limit_data) {
    m_impl->SetSpeedLimitData(speed_limit_data);
}

double SpeedAlertChecker::GetSpeedLimit(const Coordinate& position) const {
    return m_impl->GetSpeedLimit(position);
}

AlertLevel SpeedAlertChecker::DetermineSpeedLevel(double current_speed, double speed_limit) const {
    return m_impl->DetermineSpeedLevel(current_speed, speed_limit);
}

std::unique_ptr<SpeedAlertChecker> SpeedAlertChecker::Create() {
    return std::unique_ptr<SpeedAlertChecker>(new SpeedAlertChecker());
}

}
}
