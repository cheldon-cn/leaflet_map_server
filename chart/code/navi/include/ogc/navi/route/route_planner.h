#pragma once

#include "ogc/navi/route/route.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace navi {

struct RoutePlanningConstraints {
    double vessel_draft;
    double vessel_beam;
    double vessel_length;
    double safety_depth;
    double min_water_depth;
    double max_speed;
    double min_turn_radius;
    std::vector<GeoPoint> avoid_areas;
    bool avoid_shallow_water;
    bool avoid_traffic_lanes;
    
    RoutePlanningConstraints()
        : vessel_draft(0.0)
        , vessel_beam(0.0)
        , vessel_length(0.0)
        , safety_depth(3.0)
        , min_water_depth(5.0)
        , max_speed(20.0)
        , min_turn_radius(100.0)
        , avoid_shallow_water(true)
        , avoid_traffic_lanes(false)
    {}
};

struct RoutePlanningResult {
    bool success;
    std::string error_message;
    Route* route;
    double planning_time_ms;
    int nodes_explored;
    
    RoutePlanningResult()
        : success(false)
        , route(nullptr)
        , planning_time_ms(0.0)
        , nodes_explored(0)
    {}
};

class OGC_NAVI_API IRoutePlanner {
public:
    virtual ~IRoutePlanner() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual RoutePlanningResult PlanRoute(
        const GeoPoint& start,
        const GeoPoint& end,
        const RoutePlanningConstraints& constraints) = 0;
    
    virtual RoutePlanningResult PlanRoute(
        const GeoPoint& start,
        const GeoPoint& end,
        const std::vector<GeoPoint>& waypoints,
        const RoutePlanningConstraints& constraints) = 0;
    
    virtual RoutePlanningResult ReplanRoute(
        Route* current_route,
        const GeoPoint& current_position,
        const GeoPoint& obstacle_position,
        const RoutePlanningConstraints& constraints) = 0;
    
    virtual void SetProgressCallback(
        std::function<void(double progress, const std::string& status)> callback) = 0;
    
    static IRoutePlanner* Create(const std::string& algorithm_type);
};

}
}
