#include "ogc/navi/navigation/navigation_alerter.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace navi {

NavigationAlerter::NavigationAlerter()
    : off_course_threshold_(0.5)
    , waypoint_approach_radius_(1.0)
    , waypoint_arrival_radius_(0.25)
    , shallow_water_depth_(10.0)
{
}

NavigationAlerter& NavigationAlerter::Instance() {
    static NavigationAlerter instance;
    return instance;
}

void NavigationAlerter::SetOffCourseThreshold(double threshold_nm) {
    off_course_threshold_ = threshold_nm;
}

void NavigationAlerter::SetWaypointApproachRadius(double radius_nm) {
    waypoint_approach_radius_ = radius_nm;
}

void NavigationAlerter::SetWaypointArrivalRadius(double radius_nm) {
    waypoint_arrival_radius_ = radius_nm;
}

void NavigationAlerter::SetShallowWaterDepth(double depth_m) {
    shallow_water_depth_ = depth_m;
}

void NavigationAlerter::SetAlertCallback(AlertCallback callback) {
    alert_callback_ = callback;
}

std::vector<NavigationAlert> NavigationAlerter::CheckAlerts(
    const PositionData& position,
    const NavigationData& nav_data,
    const GeoPoint& next_waypoint) {
    
    std::vector<NavigationAlert> alerts;
    
    NavigationAlert off_course_alert;
    if (CheckOffCourse(nav_data, off_course_alert)) {
        alerts.push_back(off_course_alert);
        active_alerts_.push_back(off_course_alert);
        
        if (alert_callback_) {
            alert_callback_(off_course_alert);
        }
    }
    
    NavigationAlert approach_alert;
    if (CheckWaypointApproach(position, next_waypoint, approach_alert)) {
        alerts.push_back(approach_alert);
        active_alerts_.push_back(approach_alert);
        
        if (alert_callback_) {
            alert_callback_(approach_alert);
        }
    }
    
    NavigationAlert arrival_alert;
    if (CheckWaypointArrival(position, next_waypoint, arrival_alert)) {
        alerts.push_back(arrival_alert);
        active_alerts_.push_back(arrival_alert);
        
        if (alert_callback_) {
            alert_callback_(arrival_alert);
        }
    }
    
    return alerts;
}

void NavigationAlerter::AcknowledgeAlert(const NavigationAlert& alert) {
    for (auto& a : active_alerts_) {
        if (a.type == alert.type && 
            std::abs(a.timestamp - alert.timestamp) < 1.0) {
            a.acknowledged = true;
            break;
        }
    }
    
    alert_history_.push_back(alert);
    alert_history_.back().acknowledged = true;
}

void NavigationAlerter::ClearAlerts() {
    for (const auto& alert : active_alerts_) {
        alert_history_.push_back(alert);
    }
    active_alerts_.clear();
}

std::vector<NavigationAlert> NavigationAlerter::GetActiveAlerts() const {
    return active_alerts_;
}

std::vector<NavigationAlert> NavigationAlerter::GetAlertHistory() const {
    return alert_history_;
}

bool NavigationAlerter::CheckOffCourse(const NavigationData& nav_data, NavigationAlert& alert) {
    double xtd_nm = std::abs(nav_data.cross_track_error) / 1852.0;
    
    if (xtd_nm > off_course_threshold_) {
        alert.type = AlertType::OffCourse;
        alert.level = xtd_nm > off_course_threshold_ * 2.0 ? 
            AlertLevel::Critical : AlertLevel::Warning;
        alert.message = "Off course by " + std::to_string(xtd_nm) + " nm";
        alert.timestamp = 0.0;
        alert.position = GeoPoint(0.0, 0.0);
        alert.value = xtd_nm;
        alert.threshold = off_course_threshold_;
        alert.acknowledged = false;
        return true;
    }
    
    return false;
}

bool NavigationAlerter::CheckWaypointApproach(
    const PositionData& position,
    const GeoPoint& waypoint,
    NavigationAlert& alert) {
    
    GeoPoint pos(position.longitude, position.latitude);
    double distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        pos, waypoint);
    double distance_nm = distance / 1852.0;
    
    if (distance_nm <= waypoint_approach_radius_ && 
        distance_nm > waypoint_arrival_radius_) {
        alert.type = AlertType::WaypointApproach;
        alert.level = AlertLevel::Info;
        alert.message = "Approaching waypoint, distance: " + 
            std::to_string(distance_nm) + " nm";
        alert.timestamp = position.timestamp;
        alert.position = GeoPoint(position.longitude, position.latitude);
        alert.value = distance_nm;
        alert.threshold = waypoint_approach_radius_;
        alert.acknowledged = false;
        return true;
    }
    
    return false;
}

bool NavigationAlerter::CheckWaypointArrival(
    const PositionData& position,
    const GeoPoint& waypoint,
    NavigationAlert& alert) {
    
    GeoPoint pos(position.longitude, position.latitude);
    double distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        pos, waypoint);
    double distance_nm = distance / 1852.0;
    
    if (distance_nm <= waypoint_arrival_radius_) {
        alert.type = AlertType::WaypointArrival;
        alert.level = AlertLevel::Warning;
        alert.message = "Arrived at waypoint";
        alert.timestamp = position.timestamp;
        alert.position = GeoPoint(position.longitude, position.latitude);
        alert.value = distance_nm;
        alert.threshold = waypoint_arrival_radius_;
        alert.acknowledged = false;
        return true;
    }
    
    return false;
}

}
}
