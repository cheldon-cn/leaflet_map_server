#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <vector>

namespace ogc {
namespace navi {

struct OffCourseResult {
    bool is_off_course;
    double cross_track_error;
    double distance_to_route;
    int nearest_leg_index;
    GeoPoint nearest_point_on_route;
    double recommended_bearing;
    bool should_replan;
    
    OffCourseResult()
        : is_off_course(false)
        , cross_track_error(0.0)
        , distance_to_route(0.0)
        , nearest_leg_index(0)
        , recommended_bearing(0.0)
        , should_replan(false)
    {}
};

class OGC_NAVI_API OffCourseDetector {
public:
    static OffCourseDetector& Instance();
    
    void SetMaxCrossTrackError(double max_error);
    void SetMaxDistanceToRoute(double max_distance);
    void SetReplanThreshold(double threshold);
    
    OffCourseResult Detect(
        const PositionData& position,
        const GeoPoint& route_start,
        const GeoPoint& route_end);
    
    OffCourseResult Detect(
        const PositionData& position,
        const std::vector<GeoPoint>& route_points,
        int current_leg_index);
    
    GeoPoint FindNearestPointOnRoute(
        const GeoPoint& position,
        const GeoPoint& start,
        const GeoPoint& end);
    
    int FindNearestLeg(
        const GeoPoint& position,
        const std::vector<GeoPoint>& route_points);
    
private:
    OffCourseDetector();
    OffCourseDetector(const OffCourseDetector&) = delete;
    OffCourseDetector& operator=(const OffCourseDetector&) = delete;
    
    double max_cross_track_error_;
    double max_distance_to_route_;
    double replan_threshold_;
};

}
}
