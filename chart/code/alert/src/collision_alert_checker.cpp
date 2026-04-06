#include "ogc/alert/collision_alert_checker.h"
#include "ogc/draw/log.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <cmath>

namespace ogc {
namespace alert {

class CollisionAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(2) {
        m_threshold.level1_threshold = 6.0;
        m_threshold.level2_threshold = 4.0;
        m_threshold.level3_threshold = 2.0;
        m_threshold.level4_threshold = 1.0;
        m_threshold.cpa_threshold_level1 = 6.0;
        m_threshold.cpa_threshold_level2 = 4.0;
        m_threshold.cpa_threshold_level3 = 2.0;
        m_threshold.tcpa_threshold_level1 = 30.0;
        m_threshold.tcpa_threshold_level2 = 20.0;
        m_threshold.tcpa_threshold_level3 = 10.0;
        
        m_cpaCalculator.SetCPAThresholds(6.0, 4.0, 2.0, 1.0);
        m_cpaCalculator.SetTCPAThresholds(30.0, 20.0, 10.0, 5.0);
    }
    
    std::string GetCheckerId() const { return "collision_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kCollision; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = static_cast<const CollisionAlertThreshold&>(threshold);
        m_cpaCalculator.SetCPAThresholds(
            m_threshold.cpa_threshold_level1,
            m_threshold.cpa_threshold_level2,
            m_threshold.cpa_threshold_level3,
            m_threshold.level4_threshold
        );
        m_cpaCalculator.SetTCPAThresholds(
            m_threshold.tcpa_threshold_level1,
            m_threshold.tcpa_threshold_level2,
            m_threshold.tcpa_threshold_level3,
            5.0
        );
    }
    
    CollisionAlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetAisData(std::shared_ptr<void> ais_data) {
        m_aisData = ais_data;
    }
    
    void CalculateCPA_TCPA(const Coordinate& own_pos, double own_speed, double own_heading,
                           const Coordinate& target_pos, double target_speed, double target_heading,
                           double& cpa, double& tcpa) const {
        CPAResult result = m_cpaCalculator.Calculate(
            own_pos, own_speed, own_heading,
            target_pos, target_speed, target_heading
        );
        cpa = result.cpa;
        tcpa = result.tcpa;
    }
    
    CPAResult CalculateCPAWithDetails(const Coordinate& own_pos, double own_speed, double own_heading,
                                       const Coordinate& target_pos, double target_speed, double target_heading) const {
        return m_cpaCalculator.Calculate(
            own_pos, own_speed, own_heading,
            target_pos, target_speed, target_heading
        );
    }
    
    AlertLevel DetermineCollisionLevel(double cpa, double tcpa) const {
        if (tcpa < 0) return AlertLevel::kNone;
        
        if (cpa <= m_threshold.cpa_threshold_level3 && tcpa <= m_threshold.tcpa_threshold_level3) {
            return AlertLevel::kLevel4;
        }
        if (cpa <= m_threshold.cpa_threshold_level2 && tcpa <= m_threshold.tcpa_threshold_level2) {
            return AlertLevel::kLevel3;
        }
        if (cpa <= m_threshold.cpa_threshold_level1 && tcpa <= m_threshold.tcpa_threshold_level1) {
            return AlertLevel::kLevel2;
        }
        return AlertLevel::kLevel1;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        ShipMotion ownShip;
        ownShip.position = context.ship_position;
        ownShip.speed = context.ship_speed;
        ownShip.heading = context.ship_heading;
        ownShip.UpdateVelocity();
        
        ShipMotion targetShip;
        targetShip.position = Coordinate(context.ship_position.longitude + 0.01, context.ship_position.latitude + 0.01);
        targetShip.speed = 10.0;
        targetShip.heading = 180.0;
        targetShip.UpdateVelocity();
        
        CPAResult cpaResult = m_cpaCalculator.Calculate(ownShip, targetShip);
        
        AlertLevel level = cpaResult.alert_level;
        if (level == AlertLevel::kNone && cpaResult.is_dangerous) {
            level = AlertLevel::kLevel2;
        }
        
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<CollisionAlert>();
            alert->alert_id = "COLLISION_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kCollision;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->cpa = cpaResult.cpa;
            alert->tcpa = cpaResult.tcpa;
            alert->target_ship_id = "TARGET_001";
            alert->target_ship_name = "Unknown Vessel";
            alert->relative_bearing = cpaResult.relative_bearing;
            alert->relative_speed = cpaResult.relative_speed;
            alert->target_position = targetShip.position;
            
            alert->content.type = "Collision";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Collision Risk Alert";
            alert->content.message = cpaResult.warning_message;
            alert->content.position = context.ship_position;
            alert->content.action_required = "Monitor target vessel";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    CollisionAlertThreshold m_threshold;
    std::shared_ptr<void> m_aisData;
    CPACalculator m_cpaCalculator;
};

CollisionAlertChecker::CollisionAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

CollisionAlertChecker::~CollisionAlertChecker() {
}

std::string CollisionAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType CollisionAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int CollisionAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool CollisionAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void CollisionAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> CollisionAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void CollisionAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold CollisionAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void CollisionAlertChecker::SetAisData(std::shared_ptr<void> ais_data) {
    m_impl->SetAisData(ais_data);
}

void CollisionAlertChecker::CalculateCPA_TCPA(const Coordinate& own_pos, double own_speed, double own_heading,
                                               const Coordinate& target_pos, double target_speed, double target_heading,
                                               double& cpa, double& tcpa) const {
    m_impl->CalculateCPA_TCPA(own_pos, own_speed, own_heading, target_pos, target_speed, target_heading, cpa, tcpa);
}

CPAResult CollisionAlertChecker::CalculateCPAWithDetails(const Coordinate& own_pos, double own_speed, double own_heading,
                                                          const Coordinate& target_pos, double target_speed, double target_heading) const {
    return m_impl->CalculateCPAWithDetails(own_pos, own_speed, own_heading, target_pos, target_speed, target_heading);
}

AlertLevel CollisionAlertChecker::DetermineCollisionLevel(double cpa, double tcpa) const {
    return m_impl->DetermineCollisionLevel(cpa, tcpa);
}

std::unique_ptr<CollisionAlertChecker> CollisionAlertChecker::Create() {
    return std::unique_ptr<CollisionAlertChecker>(new CollisionAlertChecker());
}

}
}
