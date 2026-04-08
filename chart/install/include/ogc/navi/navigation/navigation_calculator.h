#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"

namespace ogc {
namespace navi {

struct NavigationData {
    double distance_to_waypoint;
    double bearing_to_waypoint;
    double cross_track_error;
    double along_track_distance;
    double time_to_waypoint;
    double eta;
    int current_leg_index;
    int next_waypoint_index;
    double distance_to_destination;
    double time_to_destination;
    double course_over_ground;
    double speed_over_ground;
    NavigationStatus status;
    
    NavigationData()
        : distance_to_waypoint(0.0)
        , bearing_to_waypoint(0.0)
        , cross_track_error(0.0)
        , along_track_distance(0.0)
        , time_to_waypoint(0.0)
        , eta(0.0)
        , current_leg_index(0)
        , next_waypoint_index(0)
        , distance_to_destination(0.0)
        , time_to_destination(0.0)
        , course_over_ground(0.0)
        , speed_over_ground(0.0)
        , status(NavigationStatus::Inactive)
    {}
};

class OGC_NAVI_API NavigationCalculator {
public:
    static NavigationCalculator& Instance();
    
    NavigationData Calculate(
        const PositionData& position,
        const GeoPoint& waypoint,
        double speed);
    
    NavigationData Calculate(
        const PositionData& position,
        const GeoPoint& prev_waypoint,
        const GeoPoint& next_waypoint,
        double speed);
    
    double CalculateCrossTrackError(
        const GeoPoint& position,
        const GeoPoint& start,
        const GeoPoint& end);
    
    double CalculateAlongTrackDistance(
        const GeoPoint& position,
        const GeoPoint& start,
        const GeoPoint& end);
    
    double CalculateDistanceToWaypoint(
        const GeoPoint& position,
        const GeoPoint& waypoint);
    
    double CalculateBearingToWaypoint(
        const GeoPoint& position,
        const GeoPoint& waypoint);
    
    double CalculateTimeToWaypoint(
        double distance,
        double speed);
    
private:
    NavigationCalculator() = default;
    NavigationCalculator(const NavigationCalculator&) = delete;
    NavigationCalculator& operator=(const NavigationCalculator&) = delete;
};

}
}
