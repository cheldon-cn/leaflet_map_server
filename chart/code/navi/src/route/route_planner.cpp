#include "ogc/navi/route/route_planner.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <functional>

namespace ogc {
namespace navi {

namespace {
    void CalculateDestination(double start_lat, double start_lon, 
                             double bearing_deg, double distance_m,
                             double& dest_lat, double& dest_lon) {
        const double R = 6371000.0;
        const double PI = 3.14159265358979323846;
        
        double lat1 = start_lat * PI / 180.0;
        double lon1 = start_lon * PI / 180.0;
        double brng = bearing_deg * PI / 180.0;
        
        double angular_dist = distance_m / R;
        
        double lat2 = std::asin(std::sin(lat1) * std::cos(angular_dist) +
                                std::cos(lat1) * std::sin(angular_dist) * std::cos(brng));
        double lon2 = lon1 + std::atan2(std::sin(brng) * std::sin(angular_dist) * std::cos(lat1),
                                        std::cos(angular_dist) - std::sin(lat1) * std::sin(lat2));
        
        dest_lat = lat2 * 180.0 / PI;
        dest_lon = lon2 * 180.0 / PI;
    }
}

class SimpleRoutePlanner : public IRoutePlanner {
public:
    SimpleRoutePlanner() 
        : initialized_(false)
    {}
    
    ~SimpleRoutePlanner() override = default;
    
    bool Initialize() override {
        initialized_ = true;
        return true;
    }
    
    void Shutdown() override {
        initialized_ = false;
    }
    
    RoutePlanningResult PlanRoute(
        const GeoPoint& start,
        const GeoPoint& end,
        const RoutePlanningConstraints& constraints) override {
        
        RoutePlanningResult result;
        
        if (!initialized_) {
            result.error_message = "Route planner not initialized";
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Route* route = Route::Create();
        route->SetName("Planned Route");
        route->SetDeparture("Start");
        route->SetDestination("End");
        
        Waypoint* start_wp = Waypoint::Create();
        start_wp->SetName("Start");
        start_wp->SetPosition(start.longitude, start.latitude);
        start_wp->SetSequence(0);
        route->AddWaypoint(start_wp);
        
        double total_distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
            start, end);
        
        int num_points = std::max(2, static_cast<int>(total_distance / 10000.0));
        num_points = std::min(num_points, 100);
        
        for (int i = 1; i < num_points; ++i) {
            double fraction = static_cast<double>(i) / num_points;
            GeoPoint intermediate = Interpolate(start, end, fraction);
            
            Waypoint* wp = Waypoint::Create();
            wp->SetName("Waypoint " + std::to_string(i));
            wp->SetPosition(intermediate.longitude, intermediate.latitude);
            wp->SetSequence(i);
            route->AddWaypoint(wp);
        }
        
        Waypoint* end_wp = Waypoint::Create();
        end_wp->SetName("End");
        end_wp->SetPosition(end.longitude, end.latitude);
        end_wp->SetSequence(num_points);
        route->AddWaypoint(end_wp);
        
        route->CalculateTotalDistance();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.planning_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        result.nodes_explored = num_points + 2;
        result.success = true;
        result.route = route;
        
        return result;
    }
    
    RoutePlanningResult PlanRoute(
        const GeoPoint& start,
        const GeoPoint& end,
        const std::vector<GeoPoint>& waypoints,
        const RoutePlanningConstraints& constraints) override {
        
        RoutePlanningResult result;
        
        if (!initialized_) {
            result.error_message = "Route planner not initialized";
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Route* route = Route::Create();
        route->SetName("Planned Route");
        route->SetDeparture("Start");
        route->SetDestination("End");
        
        Waypoint* start_wp = Waypoint::Create();
        start_wp->SetName("Start");
        start_wp->SetPosition(start.longitude, start.latitude);
        start_wp->SetSequence(0);
        route->AddWaypoint(start_wp);
        
        int seq = 1;
        for (const auto& pt : waypoints) {
            Waypoint* wp = Waypoint::Create();
            wp->SetName("Waypoint " + std::to_string(seq));
            wp->SetPosition(pt.longitude, pt.latitude);
            wp->SetSequence(seq++);
            route->AddWaypoint(wp);
        }
        
        Waypoint* end_wp = Waypoint::Create();
        end_wp->SetName("End");
        end_wp->SetPosition(end.longitude, end.latitude);
        end_wp->SetSequence(seq);
        route->AddWaypoint(end_wp);
        
        route->CalculateTotalDistance();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.planning_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        result.nodes_explored = static_cast<int>(waypoints.size()) + 2;
        result.success = true;
        result.route = route;
        
        return result;
    }
    
    RoutePlanningResult ReplanRoute(
        Route* current_route,
        const GeoPoint& current_position,
        const GeoPoint& obstacle_position,
        const RoutePlanningConstraints& constraints) override {
        
        RoutePlanningResult result;
        
        if (!initialized_) {
            result.error_message = "Route planner not initialized";
            return result;
        }
        
        if (!current_route) {
            result.error_message = "Current route is null";
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Route* new_route = Route::Create();
        new_route->SetName("Replanned Route");
        new_route->SetDeparture(current_route->GetDeparture());
        new_route->SetDestination(current_route->GetDestination());
        
        Waypoint* current_wp = Waypoint::Create();
        current_wp->SetName("Current Position");
        current_wp->SetPosition(current_position.longitude, current_position.latitude);
        current_wp->SetSequence(0);
        new_route->AddWaypoint(current_wp);
        
        double avoid_bearing = NavigationCalculator::Instance().CalculateBearingToWaypoint(
            current_position, obstacle_position);
        double avoid_angle = 30.0;
        double new_bearing = avoid_bearing + avoid_angle;
        if (new_bearing >= 360.0) new_bearing -= 360.0;
        
        double avoid_distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
            current_position, obstacle_position);
        double safe_distance = avoid_distance * 1.5;
        
        double lat, lon;
        CalculateDestination(
            current_position.latitude, current_position.longitude,
            new_bearing, safe_distance, lat, lon);
        
        GeoPoint avoid_point(lon, lat);
        
        Waypoint* avoid_wp = Waypoint::Create();
        avoid_wp->SetName("Avoid Point");
        avoid_wp->SetPosition(avoid_point.longitude, avoid_point.latitude);
        avoid_wp->SetSequence(1);
        new_route->AddWaypoint(avoid_wp);
        
        int wp_count = current_route->GetWaypointCount();
        if (wp_count > 0) {
            const Waypoint* last_wp = current_route->GetWaypoint(wp_count - 1);
            Waypoint* end_wp = Waypoint::Create();
            end_wp->SetName(last_wp->GetName());
            GeoPoint pos = last_wp->GetPosition();
            end_wp->SetPosition(pos.longitude, pos.latitude);
            end_wp->SetSequence(2);
            new_route->AddWaypoint(end_wp);
        }
        
        new_route->CalculateTotalDistance();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.planning_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        result.nodes_explored = 3;
        result.success = true;
        result.route = new_route;
        
        return result;
    }
    
    void SetProgressCallback(
        std::function<void(double progress, const std::string& status)> callback) override {
        progress_callback_ = callback;
    }
    
private:
    GeoPoint Interpolate(const GeoPoint& start, const GeoPoint& end, double fraction) {
        double lat = start.latitude + fraction * (end.latitude - start.latitude);
        double lon = start.longitude + fraction * (end.longitude - start.longitude);
        return GeoPoint(lon, lat);
    }
    
    bool initialized_;
    std::function<void(double progress, const std::string& status)> progress_callback_;
};

IRoutePlanner* IRoutePlanner::Create(const std::string& algorithm_type) {
    return new SimpleRoutePlanner();
}

}
}
