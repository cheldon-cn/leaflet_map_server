#include "ogc/navi/navigation/off_course_detector.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <cmath>
#include <vector>

namespace ogc {
namespace navi {

OffCourseDetector::OffCourseDetector()
    : max_cross_track_error_(500.0)
    , max_distance_to_route_(1000.0)
    , replan_threshold_(2000.0)
{
}

OffCourseDetector& OffCourseDetector::Instance() {
    static OffCourseDetector instance;
    return instance;
}

void OffCourseDetector::SetMaxCrossTrackError(double max_error) {
    max_cross_track_error_ = max_error;
}

void OffCourseDetector::SetMaxDistanceToRoute(double max_distance) {
    max_distance_to_route_ = max_distance;
}

void OffCourseDetector::SetReplanThreshold(double threshold) {
    replan_threshold_ = threshold;
}

OffCourseResult OffCourseDetector::Detect(const PositionData& position,
                                       const GeoPoint& route_start,
                                       const GeoPoint& route_end) {
    OffCourseResult result;
    
    GeoPoint pos(position.longitude, position.latitude);
    
    double xtd = NavigationCalculator::Instance().CalculateCrossTrackError(
        pos, route_start, route_end);
    
    result.cross_track_error = std::abs(xtd);
    result.distance_to_route = result.cross_track_error;
    result.nearest_leg_index = 0;
    
    result.nearest_point_on_route = FindNearestPointOnRoute(pos, route_start, route_end);
    
    double bearing_to_nearest = NavigationCalculator::Instance().CalculateBearingToWaypoint(
        pos, result.nearest_point_on_route);
    result.recommended_bearing = bearing_to_nearest;
    
    result.is_off_course = result.cross_track_error > max_cross_track_error_;
    result.should_replan = result.cross_track_error > replan_threshold_;
    
    return result;
}

OffCourseResult OffCourseDetector::Detect(const PositionData& position,
                                       const std::vector<GeoPoint>& route_points,
                                       int current_leg_index) {
    OffCourseResult result;
    
    if (route_points.size() < 2) {
        result.is_off_course = false;
        return result;
    }
    
    GeoPoint pos(position.longitude, position.latitude);
    
    int nearest_leg = FindNearestLeg(pos, route_points);
    if (nearest_leg < 0 || nearest_leg >= static_cast<int>(route_points.size()) - 1) {
        result.is_off_course = false;
        return result;
    }
    
    const GeoPoint& start = route_points[nearest_leg];
    const GeoPoint& end = route_points[nearest_leg + 1];
    
    double xtd = NavigationCalculator::Instance().CalculateCrossTrackError(pos, start, end);
    
    result.cross_track_error = std::abs(xtd);
    result.distance_to_route = result.cross_track_error;
    result.nearest_leg_index = nearest_leg;
    
    result.nearest_point_on_route = FindNearestPointOnRoute(pos, start, end);
    
    double bearing_to_nearest = NavigationCalculator::Instance().CalculateBearingToWaypoint(
        pos, result.nearest_point_on_route);
    result.recommended_bearing = bearing_to_nearest;
    
    result.is_off_course = result.cross_track_error > max_cross_track_error_;
    result.should_replan = result.cross_track_error > replan_threshold_;
    
    return result;
}

GeoPoint OffCourseDetector::FindNearestPointOnRoute(const GeoPoint& position,
                                                  const GeoPoint& start,
                                                  const GeoPoint& end) {
    double distance_to_start = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        position, start);
    
    double distance_to_end = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        position, end);
    
    double leg_distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(start, end);
    
    if (leg_distance < 0.001) {
        return start;
    }
    
    double dot = ((position.longitude - start.longitude) * (end.longitude - start.longitude) +
                  (position.latitude - start.latitude) * (end.latitude - start.latitude)) /
                  (leg_distance * leg_distance);
    
    if (dot < 0.0) {
        return start;
    } else if (dot > 1.0) {
        return end;
    }
    
    double nearest_lon = start.longitude + dot * (end.longitude - start.longitude);
    double nearest_lat = start.latitude + dot * (end.latitude - start.latitude);
    
    return GeoPoint(nearest_lon, nearest_lat);
}

int OffCourseDetector::FindNearestLeg(const GeoPoint& position,
                                     const std::vector<GeoPoint>& route_points) {
    if (route_points.size() < 2) {
        return -1;
    }
    
    int nearest_leg = 0;
    double min_distance = 1e18;
    
    for (size_t i = 0; i + 1 < route_points.size(); ++i) {
        double xtd = NavigationCalculator::Instance().CalculateCrossTrackError(
            position, route_points[i], route_points[i + 1]);
        
        double abs_xtd = std::abs(xtd);
        
        if (abs_xtd < min_distance) {
            min_distance = abs_xtd;
            nearest_leg = static_cast<int>(i);
        }
    }
    
    return nearest_leg;
}

}
}
