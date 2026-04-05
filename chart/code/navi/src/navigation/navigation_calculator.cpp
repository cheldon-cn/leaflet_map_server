#include "ogc/navi/navigation/navigation_calculator.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <cmath>

namespace ogc {
namespace navi {

namespace {
    const double KNOTS_TO_METERS_PER_SECOND = 0.514444;
    const double PI = 3.14159265358979323846;
}

NavigationCalculator& NavigationCalculator::Instance() {
    static NavigationCalculator instance;
    return instance;
}

NavigationData NavigationCalculator::Calculate(const PositionData& position,
                                             const GeoPoint& waypoint,
                                             double speed) {
    NavigationData data;
    
    data.distance_to_waypoint = CalculateDistanceToWaypoint(
        GeoPoint(position.longitude, position.latitude), waypoint);
    
    data.bearing_to_waypoint = CalculateBearingToWaypoint(
        GeoPoint(position.longitude, position.latitude), waypoint);
    
    data.course_over_ground = position.course;
    data.speed_over_ground = position.speed;
    data.status = NavigationStatus::Active;
    
    if (speed > 0.1) {
        data.time_to_waypoint = CalculateTimeToWaypoint(data.distance_to_waypoint, speed);
        data.eta = position.timestamp + data.time_to_waypoint;
    }
    
    return data;
}

NavigationData NavigationCalculator::Calculate(const PositionData& position,
                                             const GeoPoint& prev_waypoint,
                                             const GeoPoint& next_waypoint,
                                             double speed) {
    NavigationData data = Calculate(position, next_waypoint, speed);
    
    data.cross_track_error = CalculateCrossTrackError(
        GeoPoint(position.longitude, position.latitude),
        prev_waypoint, next_waypoint);
    
    data.along_track_distance = CalculateAlongTrackDistance(
        GeoPoint(position.longitude, position.latitude),
        prev_waypoint, next_waypoint);
    
    return data;
}

double NavigationCalculator::CalculateCrossTrackError(const GeoPoint& position,
                                                     const GeoPoint& start,
                                                     const GeoPoint& end) {
    double distance_to_start = CoordinateConverter::Instance().CalculateGreatCircleDistance(
        position.latitude, position.longitude,
        start.latitude, start.longitude);
    
    double bearing_start_to_end = CoordinateConverter::Instance().CalculateBearing(
        start.latitude, start.longitude,
        end.latitude, end.longitude);
    
    double bearing_start_to_point = CoordinateConverter::Instance().CalculateBearing(
        start.latitude, start.longitude,
        position.latitude, position.longitude);
    
    double bearing_diff = bearing_start_to_point - bearing_start_to_end;
    while (bearing_diff > 180.0) bearing_diff -= 360.0;
    while (bearing_diff < -180.0) bearing_diff += 360.0;
    
    double xtd = std::sin(bearing_diff * PI / 180.0) * distance_to_start;
    
    return xtd;
}

double NavigationCalculator::CalculateAlongTrackDistance(const GeoPoint& position,
                                                       const GeoPoint& start,
                                                       const GeoPoint& end) {
    double distance_to_start = CoordinateConverter::Instance().CalculateGreatCircleDistance(
        position.latitude, position.longitude,
        start.latitude, start.longitude);
    
    double xtd = CalculateCrossTrackError(position, start, end);
    
    double atd_squared = distance_to_start * distance_to_start - xtd * xtd;
    if (atd_squared < 0) {
        return 0.0;
    }
    
    return std::sqrt(atd_squared);
}

double NavigationCalculator::CalculateDistanceToWaypoint(const GeoPoint& position,
                                                       const GeoPoint& waypoint) {
    return CoordinateConverter::Instance().CalculateGreatCircleDistance(
        position.latitude, position.longitude,
        waypoint.latitude, waypoint.longitude);
}

double NavigationCalculator::CalculateBearingToWaypoint(const GeoPoint& position,
                                                        const GeoPoint& waypoint) {
    return CoordinateConverter::Instance().CalculateBearing(
        position.latitude, position.longitude,
        waypoint.latitude, waypoint.longitude);
}

double NavigationCalculator::CalculateTimeToWaypoint(double distance, double speed) {
    if (speed <= 0.1) {
        return 0.0;
    }
    
    double speed_ms = speed * KNOTS_TO_METERS_PER_SECOND;
    return distance / speed_ms;
}

}
}
