#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace navi {

struct NavigationAlert {
    AlertType type;
    AlertLevel level;
    std::string message;
    double timestamp;
    GeoPoint position;
    double value;
    double threshold;
    bool acknowledged;
    
    NavigationAlert()
        : type(AlertType::OffCourse)
        , level(AlertLevel::Info)
        , timestamp(0.0)
        , value(0.0)
        , threshold(0.0)
        , acknowledged(false)
    {}
};

using AlertCallback = std::function<void(const NavigationAlert&)>;

class OGC_NAVI_API NavigationAlerter {
public:
    static NavigationAlerter& Instance();
    
    void SetOffCourseThreshold(double threshold_nm);
    void SetWaypointApproachRadius(double radius_nm);
    void SetWaypointArrivalRadius(double radius_nm);
    void SetShallowWaterDepth(double depth_m);
    
    void SetAlertCallback(AlertCallback callback);
    
    std::vector<NavigationAlert> CheckAlerts(
        const PositionData& position,
        const NavigationData& nav_data,
        const GeoPoint& next_waypoint);
    
    void AcknowledgeAlert(const NavigationAlert& alert);
    void ClearAlerts();
    
    std::vector<NavigationAlert> GetActiveAlerts() const;
    std::vector<NavigationAlert> GetAlertHistory() const;
    
private:
    NavigationAlerter();
    NavigationAlerter(const NavigationAlerter&) = delete;
    NavigationAlerter& operator=(const NavigationAlerter&) = delete;
    
    bool CheckOffCourse(const NavigationData& nav_data, NavigationAlert& alert);
    bool CheckWaypointApproach(
        const PositionData& position,
        const GeoPoint& waypoint,
        NavigationAlert& alert);
    bool CheckWaypointArrival(
        const PositionData& position,
        const GeoPoint& waypoint,
        NavigationAlert& alert);
    
    double off_course_threshold_;
    double waypoint_approach_radius_;
    double waypoint_arrival_radius_;
    double shallow_water_depth_;
    AlertCallback alert_callback_;
    std::vector<NavigationAlert> active_alerts_;
    std::vector<NavigationAlert> alert_history_;
};

}
}
