#include "ogc/alert/restricted_area_checker.h"
#include "ogc/base/log.h"

namespace ogc {
namespace alert {

class RestrictedAreaChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(7) {
        m_threshold.level1_threshold = 500.0;
        m_threshold.level2_threshold = 300.0;
        m_threshold.level3_threshold = 100.0;
        m_threshold.level4_threshold = 0.0;
    }
    
    std::string GetCheckerId() const { return "restricted_area_checker"; }
    AlertType GetAlertType() const { return AlertType::kRestrictedArea; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = threshold;
    }
    
    AlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetRestrictedAreaData(std::shared_ptr<void> area_data) {
        m_areaData = area_data;
    }
    
    double CalculateDistanceToBoundary(const Coordinate& position, const std::string& area_id) const {
        return 200.0;
    }
    
    bool IsInsideRestrictedArea(const Coordinate& position) const {
        return false;
    }
    
    AlertLevel DetermineRestrictedAreaLevel(double distance, bool is_inside) const {
        if (is_inside) return AlertLevel::kLevel4;
        if (distance <= m_threshold.level3_threshold) return AlertLevel::kLevel3;
        if (distance <= m_threshold.level2_threshold) return AlertLevel::kLevel2;
        if (distance <= m_threshold.level1_threshold) return AlertLevel::kLevel1;
        return AlertLevel::kNone;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        double distance = CalculateDistanceToBoundary(context.ship_position, "AREA_001");
        bool is_inside = IsInsideRestrictedArea(context.ship_position);
        
        AlertLevel level = DetermineRestrictedAreaLevel(distance, is_inside);
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<RestrictedAreaAlert>();
            alert->alert_id = "RESTRICTED_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kRestrictedArea;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->area_id = "AREA_001";
            alert->area_name = "Military Exercise Zone";
            alert->distance_to_boundary = distance;
            alert->is_inside = is_inside;
            
            alert->content.type = "RestrictedArea";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Restricted Area Alert";
            alert->content.message = is_inside ? "Inside restricted area" : "Distance to boundary: " + std::to_string(distance) + " meters";
            alert->content.position = context.ship_position;
            alert->content.action_required = "Change course immediately";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    AlertThreshold m_threshold;
    std::shared_ptr<void> m_areaData;
};

RestrictedAreaChecker::RestrictedAreaChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

RestrictedAreaChecker::~RestrictedAreaChecker() {
}

std::string RestrictedAreaChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType RestrictedAreaChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int RestrictedAreaChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool RestrictedAreaChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void RestrictedAreaChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> RestrictedAreaChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void RestrictedAreaChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold RestrictedAreaChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void RestrictedAreaChecker::SetRestrictedAreaData(std::shared_ptr<void> area_data) {
    m_impl->SetRestrictedAreaData(area_data);
}

double RestrictedAreaChecker::CalculateDistanceToBoundary(const Coordinate& position, const std::string& area_id) const {
    return m_impl->CalculateDistanceToBoundary(position, area_id);
}

bool RestrictedAreaChecker::IsInsideRestrictedArea(const Coordinate& position) const {
    return m_impl->IsInsideRestrictedArea(position);
}

AlertLevel RestrictedAreaChecker::DetermineRestrictedAreaLevel(double distance, bool is_inside) const {
    return m_impl->DetermineRestrictedAreaLevel(distance, is_inside);
}

std::unique_ptr<RestrictedAreaChecker> RestrictedAreaChecker::Create() {
    return std::unique_ptr<RestrictedAreaChecker>(new RestrictedAreaChecker());
}

}
}
