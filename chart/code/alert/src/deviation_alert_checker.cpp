#include "ogc/alert/deviation_alert_checker.h"
#include "ogc/base/log.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace alert {

class DeviationAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(5) {
        m_threshold.level1_threshold = 100.0;
        m_threshold.level2_threshold = 200.0;
        m_threshold.level3_threshold = 300.0;
        m_threshold.level4_threshold = 500.0;
    }
    
    std::string GetCheckerId() const { return "deviation_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kDeviation; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = static_cast<const DeviationThreshold&>(threshold);
    }
    
    DeviationThreshold GetThreshold() const { return m_threshold; }
    
    void SetRouteData(std::shared_ptr<void> route_data) {
        m_routeData = route_data;
    }
    
    double CalculateDeviation(const Coordinate& position, const std::vector<Coordinate>& waypoints) const {
        if (waypoints.empty()) return 0.0;
        
        double min_distance = std::numeric_limits<double>::max();
        
        for (size_t i = 0; i < waypoints.size() - 1; ++i) {
            double distance = PointToSegmentDistance(position, waypoints[i], waypoints[i + 1]);
            min_distance = std::min(min_distance, distance);
        }
        
        return min_distance;
    }
    
    Coordinate FindNearestWaypoint(const Coordinate& position, const std::vector<Coordinate>& waypoints) const {
        if (waypoints.empty()) return Coordinate();
        
        double min_distance = std::numeric_limits<double>::max();
        Coordinate nearest = waypoints[0];
        
        for (const auto& wp : waypoints) {
            double distance = position.DistanceTo(wp);
            if (distance < min_distance) {
                min_distance = distance;
                nearest = wp;
            }
        }
        
        return nearest;
    }
    
    AlertLevel DetermineDeviationLevel(double deviation) const {
        if (deviation >= m_threshold.level4_threshold) return AlertLevel::kLevel4;
        if (deviation >= m_threshold.level3_threshold) return AlertLevel::kLevel3;
        if (deviation >= m_threshold.level2_threshold) return AlertLevel::kLevel2;
        if (deviation >= m_threshold.level1_threshold) return AlertLevel::kLevel1;
        return AlertLevel::kNone;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        double deviation = 150.0;
        
        AlertLevel level = DetermineDeviationLevel(deviation);
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<DeviationAlert>();
            alert->alert_id = "DEVIATION_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kDeviation;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->deviation_distance = deviation;
            alert->max_deviation = m_threshold.level4_threshold;
            alert->route_id = "ROUTE_001";
            
            alert->content.type = "Deviation";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Route Deviation Alert";
            alert->content.message = "Deviation: " + std::to_string(deviation) + " meters";
            alert->content.position = context.ship_position;
            alert->content.action_required = "Return to planned route";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    double PointToSegmentDistance(const Coordinate& point, const Coordinate& line_start, const Coordinate& line_end) const {
        double dx = line_end.longitude - line_start.longitude;
        double dy = line_end.latitude - line_start.latitude;
        
        if (dx == 0.0 && dy == 0.0) {
            return point.DistanceTo(line_start);
        }
        
        double t = ((point.longitude - line_start.longitude) * dx + (point.latitude - line_start.latitude) * dy) / (dx * dx + dy * dy);
        t = std::max(0.0, std::min(1.0, t));
        
        Coordinate projection(line_start.longitude + t * dx, line_start.latitude + t * dy);
        return point.DistanceTo(projection);
    }
    
    bool m_enabled;
    int m_priority;
    DeviationThreshold m_threshold;
    std::shared_ptr<void> m_routeData;
};

DeviationAlertChecker::DeviationAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

DeviationAlertChecker::~DeviationAlertChecker() {
}

std::string DeviationAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType DeviationAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int DeviationAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool DeviationAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void DeviationAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> DeviationAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void DeviationAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold DeviationAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void DeviationAlertChecker::SetRouteData(std::shared_ptr<void> route_data) {
    m_impl->SetRouteData(route_data);
}

double DeviationAlertChecker::CalculateDeviation(const Coordinate& position, const std::vector<Coordinate>& waypoints) const {
    return m_impl->CalculateDeviation(position, waypoints);
}

Coordinate DeviationAlertChecker::FindNearestWaypoint(const Coordinate& position, const std::vector<Coordinate>& waypoints) const {
    return m_impl->FindNearestWaypoint(position, waypoints);
}

AlertLevel DeviationAlertChecker::DetermineDeviationLevel(double deviation) const {
    return m_impl->DetermineDeviationLevel(deviation);
}

std::unique_ptr<DeviationAlertChecker> DeviationAlertChecker::Create() {
    return std::unique_ptr<DeviationAlertChecker>(new DeviationAlertChecker());
}

}
}
