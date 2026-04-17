/**
 * @file sdk_c_api_navi.cpp
 * @brief OGC Chart SDK C API - Navigation Module Implementation
 * @version v2.0
 * @date 2026-04-17
 */

#define _USE_MATH_DEFINES
#include "sdk_c_api.h"

#include <ogc/navi/ais/ais_manager.h>
#include <ogc/navi/ais/ais_target.h>
#include <ogc/navi/ais/collision_assessor.h>
#include <ogc/navi/types.h>
#include <ogc/navi/navigation/navigation_engine.h>
#include <ogc/navi/navigation/off_course_detector.h>
#include <ogc/navi/positioning/position_manager.h>
#include <ogc/navi/positioning/position_provider.h>
#include <ogc/navi/positioning/nmea_parser.h>
#include <ogc/navi/route/route.h>
#include <ogc/navi/route/route_manager.h>
#include <ogc/navi/route/route_planner.h>
#include <ogc/navi/route/waypoint.h>
#include <ogc/navi/track/track.h>
#include <ogc/navi/track/track_recorder.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace ogc;
using namespace ogc::navi;

#ifdef __cplusplus
extern "C" {
#endif

namespace { 

static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

struct LocalWaypointData {
    double lat;
    double lon;
    std::string name;
    std::string description;
    double arrivalRadius;
    double turnRadius;
    bool isArrival;
    ogc_waypoint_type_e type;
};

struct LocalRouteData {
    std::string id;
    std::string name;
    std::vector<LocalWaypointData*> waypoints;
    ogc_route_status_e status;
    int currentWaypointIndex;
    double totalDistance;
    
    LocalRouteData() : status(OGC_ROUTE_STATUS_PLANNING), currentWaypointIndex(0), totalDistance(0.0) {}
};

struct RouteManagerContext {
    RouteManager* manager;
    std::string activeRouteId;
    std::map<std::string, LocalRouteData*> routes;
    bool initialized;
    
    RouteManagerContext() : manager(nullptr), initialized(false) {}
    ~RouteManagerContext() {
        for (auto& pair : routes) {
            delete pair.second;
        }
        routes.clear();
    }
};

static RouteManagerContext* g_routeManagerCtx = nullptr;

struct RoutePlannerContext {
    IRoutePlanner* planner;
    bool initialized;
    
    RoutePlannerContext() : planner(nullptr), initialized(false) {}
    ~RoutePlannerContext() {
        delete planner;
    }
};

static RoutePlannerContext* g_routePlannerCtx = nullptr;

static double CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000.0;
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    double a = sin(dLat/2) * sin(dLat/2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

}  

ogc_waypoint_t* ogc_waypoint_create(double lat, double lon) {
    LocalWaypointData* wp = new LocalWaypointData();
    wp->lat = lat;
    wp->lon = lon;
    wp->arrivalRadius = 0.0;
    wp->turnRadius = 0.0;
    wp->isArrival = false;
    wp->type = OGC_WAYPOINT_TYPE_NORMAL;
    return reinterpret_cast<ogc_waypoint_t*>(wp);
}

void ogc_waypoint_destroy(ogc_waypoint_t* wp) {
    delete reinterpret_cast<LocalWaypointData*>(wp);
}

double ogc_waypoint_get_latitude(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->lat;
    }
    return 0.0;
}

double ogc_waypoint_get_longitude(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->lon;
    }
    return 0.0;
}

const char* ogc_waypoint_get_name(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->name.c_str();
    }
    return nullptr;
}

void ogc_waypoint_set_name(ogc_waypoint_t* wp, const char* name) {
    if (wp) {
        reinterpret_cast<LocalWaypointData*>(wp)->name = SafeString(name);
    }
}

int ogc_waypoint_is_arrival(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->isArrival ? 1 : 0;
    }
    return 0;
}

void ogc_waypoint_set_arrival_radius(ogc_waypoint_t* wp, double radius) {
    if (wp) {
        reinterpret_cast<LocalWaypointData*>(wp)->arrivalRadius = radius;
    }
}

double ogc_waypoint_get_arrival_radius(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->arrivalRadius;
    }
    return 0.0;
}

void ogc_waypoint_set_turn_radius(ogc_waypoint_t* wp, double radius) {
    if (wp) {
        reinterpret_cast<LocalWaypointData*>(wp)->turnRadius = radius;
    }
}

double ogc_waypoint_get_turn_radius(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->turnRadius;
    }
    return 0.0;
}

void ogc_waypoint_set_type(ogc_waypoint_t* wp, ogc_waypoint_type_e type) {
    if (wp) {
        reinterpret_cast<LocalWaypointData*>(wp)->type = type;
    }
}

ogc_waypoint_type_e ogc_waypoint_get_type(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->type;
    }
    return OGC_WAYPOINT_TYPE_NORMAL;
}

void ogc_waypoint_set_description(ogc_waypoint_t* wp, const char* description) {
    if (wp) {
        reinterpret_cast<LocalWaypointData*>(wp)->description = SafeString(description);
    }
}

const char* ogc_waypoint_get_description(const ogc_waypoint_t* wp) {
    if (wp) {
        return reinterpret_cast<const LocalWaypointData*>(wp)->description.c_str();
    }
    return nullptr;
}

ogc_route_t* ogc_route_create(void) {
    LocalRouteData* route = new LocalRouteData();
    route->status = OGC_ROUTE_STATUS_PLANNING;
    return reinterpret_cast<ogc_route_t*>(route);
}

void ogc_route_destroy(ogc_route_t* route) {
    if (route) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        for (auto wp : data->waypoints) {
            delete wp;
        }
        delete data;
    }
}

const char* ogc_route_get_id(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const LocalRouteData*>(route)->id.c_str();
    }
    return "";
}

const char* ogc_route_get_name(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const LocalRouteData*>(route)->name.c_str();
    }
    return nullptr;
}

void ogc_route_set_name(ogc_route_t* route, const char* name) {
    if (route) {
        reinterpret_cast<LocalRouteData*>(route)->name = SafeString(name);
    }
}

ogc_route_status_e ogc_route_get_status(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const LocalRouteData*>(route)->status;
    }
    return OGC_ROUTE_STATUS_PLANNING;
}

double ogc_route_get_total_distance(const ogc_route_t* route) {
    if (route) {
        const LocalRouteData* data = reinterpret_cast<const LocalRouteData*>(route);
        if (data->waypoints.size() < 2) {
            return 0.0;
        }
        double totalDist = 0.0;
        for (size_t i = 1; i < data->waypoints.size(); ++i) {
            const LocalWaypointData* wp1 = data->waypoints[i-1];
            const LocalWaypointData* wp2 = data->waypoints[i];
            totalDist += CalculateDistance(wp1->lat, wp1->lon, wp2->lat, wp2->lon);
        }
        return totalDist;
    }
    return 0.0;
}

size_t ogc_route_get_waypoint_count(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const LocalRouteData*>(route)->waypoints.size();
    }
    return 0;
}

ogc_waypoint_t* ogc_route_get_waypoint(const ogc_route_t* route, size_t index) {
    if (route) {
        const LocalRouteData* data = reinterpret_cast<const LocalRouteData*>(route);
        if (index < data->waypoints.size()) {
            return reinterpret_cast<ogc_waypoint_t*>(data->waypoints[index]);
        }
    }
    return nullptr;
}

void ogc_route_add_waypoint(ogc_route_t* route, ogc_waypoint_t* wp) {
    if (route && wp) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        LocalWaypointData* wpData = reinterpret_cast<LocalWaypointData*>(wp);
        data->waypoints.push_back(wpData);
    }
}

void ogc_route_remove_waypoint(ogc_route_t* route, size_t index) {
    if (route) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        if (index < data->waypoints.size()) {
            data->waypoints.erase(data->waypoints.begin() + index);
        }
    }
}

ogc_waypoint_t* ogc_route_get_current_waypoint(const ogc_route_t* route) {
    if (route) {
        const LocalRouteData* data = reinterpret_cast<const LocalRouteData*>(route);
        if (data->currentWaypointIndex >= 0 && 
            static_cast<size_t>(data->currentWaypointIndex) < data->waypoints.size()) {
            return reinterpret_cast<ogc_waypoint_t*>(data->waypoints[data->currentWaypointIndex]);
        }
    }
    return nullptr;
}

int ogc_route_advance_to_next_waypoint(ogc_route_t* route) {
    if (route) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        if (static_cast<size_t>(data->currentWaypointIndex + 1) < data->waypoints.size()) {
            data->currentWaypointIndex++;
            return 1;
        }
    }
    return 0;
}

void ogc_route_insert_waypoint(ogc_route_t* route, size_t index, ogc_waypoint_t* wp) {
    if (route && wp) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        LocalWaypointData* wpData = reinterpret_cast<LocalWaypointData*>(wp);
        if (index >= data->waypoints.size()) {
            data->waypoints.push_back(wpData);
        } else {
            data->waypoints.insert(data->waypoints.begin() + index, wpData);
        }
    }
}

void ogc_route_clear(ogc_route_t* route) {
    if (route) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        for (auto wp : data->waypoints) {
            delete wp;
        }
        data->waypoints.clear();
    }
}

void ogc_route_reverse(ogc_route_t* route) {
    if (route) {
        LocalRouteData* data = reinterpret_cast<LocalRouteData*>(route);
        std::reverse(data->waypoints.begin(), data->waypoints.end());
    }
}

int64_t ogc_route_get_eta(const ogc_route_t* route) {
    (void)route;
    return -1;
}

ogc_route_manager_t* ogc_route_manager_create(void) {
    if (!g_routeManagerCtx) {
        g_routeManagerCtx = new RouteManagerContext();
        g_routeManagerCtx->manager = &RouteManager::Instance();
        g_routeManagerCtx->initialized = true;
    }
    return reinterpret_cast<ogc_route_manager_t*>(g_routeManagerCtx);
}

void ogc_route_manager_destroy(ogc_route_manager_t* mgr) {
    if (mgr && g_routeManagerCtx) {
        delete g_routeManagerCtx;
        g_routeManagerCtx = nullptr;
    }
}

size_t ogc_route_manager_get_route_count(const ogc_route_manager_t* mgr) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(const_cast<ogc_route_manager_t*>(mgr));
    if (ctx && ctx->manager) {
        return static_cast<size_t>(ctx->manager->GetRouteCount());
    }
    return 0;
}

ogc_route_t* ogc_route_manager_get_route(const ogc_route_manager_t* mgr, size_t index) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(const_cast<ogc_route_manager_t*>(mgr));
    if (ctx && ctx->manager) {
        auto routes = ctx->manager->GetAllRoutes();
        if (index < routes.size()) {
            return reinterpret_cast<ogc_route_t*>(routes[index]);
        }
    }
    return nullptr;
}

ogc_route_t* ogc_route_manager_get_route_by_id(const ogc_route_manager_t* mgr, const char* id) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(const_cast<ogc_route_manager_t*>(mgr));
    if (ctx && ctx->manager && id) {
        Route* route = ctx->manager->GetRoute(SafeString(id));
        return reinterpret_cast<ogc_route_t*>(route);
    }
    return nullptr;
}

void ogc_route_manager_add_route(ogc_route_manager_t* mgr, ogc_route_t* route) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(mgr);
    if (ctx && ctx->manager && route) {
        Route* r = reinterpret_cast<Route*>(route);
        ctx->manager->SaveRoute(r);
    }
}

void ogc_route_manager_remove_route(ogc_route_manager_t* mgr, const char* id) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(mgr);
    if (ctx && ctx->manager && id) {
        ctx->manager->DeleteRoute(SafeString(id));
    }
}

ogc_route_t* ogc_route_manager_get_active_route(const ogc_route_manager_t* mgr) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(const_cast<ogc_route_manager_t*>(mgr));
    if (ctx && ctx->manager) {
        Route* route = ctx->manager->GetActiveRoute();
        return reinterpret_cast<ogc_route_t*>(route);
    }
    return nullptr;
}

void ogc_route_manager_set_active_route(ogc_route_manager_t* mgr, const char* id) {
    RouteManagerContext* ctx = reinterpret_cast<RouteManagerContext*>(mgr);
    if (ctx && ctx->manager && id) {
        Route* route = ctx->manager->GetRoute(SafeString(id));
        if (route) {
            ctx->manager->SetActiveRoute(route);
            ctx->activeRouteId = SafeString(id);
        }
    }
}

ogc_route_planner_t* ogc_route_planner_create(void) {
    if (!g_routePlannerCtx) {
        g_routePlannerCtx = new RoutePlannerContext();
        g_routePlannerCtx->planner = IRoutePlanner::Create("astar");
        if (g_routePlannerCtx->planner) {
            g_routePlannerCtx->planner->Initialize();
            g_routePlannerCtx->initialized = true;
        }
    }
    return reinterpret_cast<ogc_route_planner_t*>(g_routePlannerCtx);
}

void ogc_route_planner_destroy(ogc_route_planner_t* planner) {
    if (planner && g_routePlannerCtx) {
        delete g_routePlannerCtx;
        g_routePlannerCtx = nullptr;
    }
}

ogc_route_t* ogc_route_planner_plan_route(ogc_route_planner_t* planner, 
                                           const ogc_waypoint_t* start, 
                                           const ogc_waypoint_t* end,
                                           const ogc_envelope_t* avoid_areas,
                                           int avoid_count) {
    RoutePlannerContext* ctx = reinterpret_cast<RoutePlannerContext*>(planner);
    if (ctx && ctx->planner && start && end) {
        const LocalWaypointData* startWp = reinterpret_cast<const LocalWaypointData*>(start);
        const LocalWaypointData* endWp = reinterpret_cast<const LocalWaypointData*>(end);
        
        RoutePlanningConstraints constraints;
        constraints.avoid_shallow_water = true;
        
        if (avoid_areas && avoid_count > 0) {
            const ogc_envelope_t* const* areas = reinterpret_cast<const ogc_envelope_t* const*>(avoid_areas);
            for (int i = 0; i < avoid_count; ++i) {
                GeoPoint pt;
                pt.longitude = (ogc_envelope_get_min_x(areas[i]) + ogc_envelope_get_max_x(areas[i])) / 2.0;
                pt.latitude = (ogc_envelope_get_min_y(areas[i]) + ogc_envelope_get_max_y(areas[i])) / 2.0;
                constraints.avoid_areas.push_back(pt);
            }
        }
        
        GeoPoint startPoint, endPoint;
        startPoint.latitude = startWp->lat;
        startPoint.longitude = startWp->lon;
        endPoint.latitude = endWp->lat;
        endPoint.longitude = endWp->lon;
        
        RoutePlanningResult result = ctx->planner->PlanRoute(startPoint, endPoint, constraints);
        if (result.success && result.route) {
            return reinterpret_cast<ogc_route_t*>(result.route);
        }
    }
    return nullptr;
}

ogc_ais_target_t* ogc_ais_target_create(uint32_t mmsi) {
    return reinterpret_cast<ogc_ais_target_t*>(AisTarget::Create(mmsi));
}

void ogc_ais_target_destroy(ogc_ais_target_t* target) {
    if (target) {
        reinterpret_cast<AisTarget*>(target)->ReleaseReference();
    }
}

uint32_t ogc_ais_target_get_mmsi(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetMmsi();
    }
    return 0;
}

int ogc_ais_target_get_name(const ogc_ais_target_t* target, char* buffer, size_t buffer_size) {
    if (!target || !buffer || buffer_size == 0) {
        return -1;
    }
    
    std::string name = reinterpret_cast<const AisTarget*>(target)->GetShipName();
    size_t copy_len = (name.length() < buffer_size - 1) ? name.length() : buffer_size - 1;
    strncpy(buffer, name.c_str(), copy_len);
    buffer[copy_len] = '\0';
    
    return static_cast<int>(name.length());
}

double ogc_ais_target_get_latitude(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetLatitude();
    }
    return 0.0;
}

double ogc_ais_target_get_longitude(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetLongitude();
    }
    return 0.0;
}

double ogc_ais_target_get_speed(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetSpeedOverGround();
    }
    return 0.0;
}

double ogc_ais_target_get_course(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetCourseOverGround();
    }
    return 0.0;
}

double ogc_ais_target_get_heading(const ogc_ais_target_t* target) {
    if (target) {
        return reinterpret_cast<const AisTarget*>(target)->GetHeading();
    }
    return 0.0;
}

int ogc_ais_target_get_nav_status(const ogc_ais_target_t* target) {
    if (target) {
        return static_cast<int>(reinterpret_cast<const AisTarget*>(target)->GetNavigationStatus());
    }
    return 0;
}

void ogc_ais_target_update_position(ogc_ais_target_t* target, double lat, double lon, double speed, double course) {
    if (target) {
        AisTarget* t = reinterpret_cast<AisTarget*>(target);
        t->SetLatitude(lat);
        t->SetLongitude(lon);
        t->SetSpeedOverGround(speed);
        t->SetCourseOverGround(course);
    }
}


ogc_ais_manager_t* ogc_ais_manager_create(void) {
    return reinterpret_cast<ogc_ais_manager_t*>(&AisManager::Instance());
}

void ogc_ais_manager_destroy(ogc_ais_manager_t* manager) {
    (void)manager;
}

int ogc_ais_manager_initialize(ogc_ais_manager_t* manager) {
    if (manager) {
        return reinterpret_cast<AisManager*>(manager)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_ais_manager_shutdown(ogc_ais_manager_t* manager) {
    if (manager) {
        reinterpret_cast<AisManager*>(manager)->Shutdown();
    }
}

ogc_ais_target_t* ogc_ais_manager_get_target(ogc_ais_manager_t* manager, uint32_t mmsi) {
    if (manager) {
        return reinterpret_cast<ogc_ais_target_t*>(
            reinterpret_cast<AisManager*>(manager)->GetTarget(mmsi));
    }
    return nullptr;
}

ogc_ais_target_t** ogc_ais_manager_get_all_targets(ogc_ais_manager_t* manager, int* count) {
    (void)manager;
    if (count) *count = 0;
    return nullptr;
}

void ogc_ais_manager_free_targets(ogc_ais_target_t** targets) {
    (void)targets;
}

ogc_ais_target_t** ogc_ais_manager_get_targets_in_range(ogc_ais_manager_t* manager, double lat, double lon, double range_nm, int* count) {
    (void)manager;
    (void)lat;
    (void)lon;
    (void)range_nm;
    if (count) *count = 0;
    return nullptr;
}

int ogc_ais_manager_get_target_count(ogc_ais_manager_t* manager) {
    if (manager) {
        auto targets = reinterpret_cast<AisManager*>(manager)->GetAllTargets();
        return static_cast<int>(targets.size());
    }
    return 0;
}

int ogc_ais_manager_add_target(ogc_ais_manager_t* manager, ogc_ais_target_t* target) {
    (void)manager;
    (void)target;
    return -1;
}

int ogc_ais_manager_remove_target(ogc_ais_manager_t* manager, uint32_t mmsi) {
    (void)manager;
    (void)mmsi;
    return -1;
}

int ogc_ais_manager_set_callback(ogc_ais_manager_t* mgr,
    void (*callback)(uint32_t mmsi, int event_type, void* user_data),
    void* user_data) {
    if (mgr && callback) {
        (void)callback;
        (void)user_data;
        return 0;
    }
    return -1;
}

ogc_navigation_engine_t* ogc_navigation_engine_create(void) {
    return reinterpret_cast<ogc_navigation_engine_t*>(&NavigationEngine::Instance());
}

void ogc_navigation_engine_destroy(ogc_navigation_engine_t* engine) {
    (void)engine;
}

int ogc_navigation_engine_initialize(ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<NavigationEngine*>(engine)->Initialize() ? 1 : 0;
    }
    return 0;
}

void ogc_navigation_engine_shutdown(ogc_navigation_engine_t* engine) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->Shutdown();
    }
}

void ogc_navigation_engine_set_route(ogc_navigation_engine_t* engine, ogc_route_t* route) {
    if (engine && route) {
        reinterpret_cast<NavigationEngine*>(engine)->StartNavigation(
            reinterpret_cast<Route*>(route));
    }
}

ogc_route_t* ogc_navigation_engine_get_route(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<ogc_route_t*>(
            reinterpret_cast<const NavigationEngine*>(engine)->GetActiveRoute());
    }
    return nullptr;
}

void ogc_navigation_engine_start(ogc_navigation_engine_t* engine) {
    (void)engine;
}

void ogc_navigation_engine_stop(ogc_navigation_engine_t* engine) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->StopNavigation();
    }
}

void ogc_navigation_engine_pause(ogc_navigation_engine_t* engine) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->PauseNavigation();
    }
}

void ogc_navigation_engine_resume(ogc_navigation_engine_t* engine) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->ResumeNavigation();
    }
}

ogc_navigation_status_e ogc_navigation_engine_get_status(const ogc_navigation_engine_t* engine) {
    (void)engine;
    return OGC_NAV_STATUS_STANDBY;
}

ogc_waypoint_t* ogc_navigation_engine_get_current_waypoint(const ogc_navigation_engine_t* engine) {
    (void)engine;
    return nullptr;
}

double ogc_navigation_engine_get_distance_to_waypoint(const ogc_navigation_engine_t* engine) {
    (void)engine;
    return 0.0;
}

double ogc_navigation_engine_get_bearing_to_waypoint(const ogc_navigation_engine_t* engine) {
    (void)engine;
    return 0.0;
}

double ogc_navigation_engine_get_cross_track_error(const ogc_navigation_engine_t* engine) {
    (void)engine;
    return 0.0;
}

ogc_position_manager_t* ogc_position_manager_create(void) {
    return reinterpret_cast<ogc_position_manager_t*>(&PositionManager::Instance());
}

void ogc_position_manager_destroy(ogc_position_manager_t* mgr) {
    (void)mgr;
}

int ogc_position_manager_initialize(ogc_position_manager_t* mgr) {
    (void)mgr;
    return 0;
}

void ogc_position_manager_shutdown(ogc_position_manager_t* mgr) {
    if (mgr) {
        reinterpret_cast<PositionManager*>(mgr)->Shutdown();
    }
}

double ogc_position_manager_get_latitude(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().latitude;
    }
    return 0.0;
}

double ogc_position_manager_get_longitude(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().longitude;
    }
    return 0.0;
}

double ogc_position_manager_get_speed(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().speed;
    }
    return 0.0;
}

double ogc_position_manager_get_course(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().course;
    }
    return 0.0;
}

double ogc_position_manager_get_heading(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().heading;
    }
    return 0.0;
}

int ogc_position_manager_get_fix_quality(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().gps_quality;
    }
    return 0;
}

int ogc_position_manager_get_satellite_count(const ogc_position_manager_t* mgr) {
    if (mgr) {
        return reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition().satellite_count;
    }
    return 0;
}

int ogc_position_manager_set_position(ogc_position_manager_t* mgr, double latitude, double longitude) {
    if (mgr) {
        return 0;
    }
    return -1;
}

int ogc_position_manager_get_position(const ogc_position_manager_t* mgr, double* latitude, double* longitude) {
    if (mgr && latitude && longitude) {
        auto pos = reinterpret_cast<const PositionManager*>(mgr)->GetCurrentPosition();
        *latitude = pos.latitude;
        *longitude = pos.longitude;
        return 0;
    }
    return -1;
}

int ogc_position_manager_set_callback(ogc_position_manager_t* mgr,
    void (*callback)(double lat, double lon, double speed, double course, void* user_data),
    void* user_data) {
    if (mgr && callback) {
        reinterpret_cast<PositionManager*>(mgr)->SetPositionCallback(
            [callback, user_data](const PositionData& data) {
                callback(data.latitude, data.longitude, data.speed, data.course, user_data);
            });
        return 0;
    }
    return -1;
}

ogc_track_t* ogc_track_create(void) {
    return reinterpret_cast<ogc_track_t*>(Track::Create());
}

void ogc_track_destroy(ogc_track_t* track) {
    if (track) {
        reinterpret_cast<Track*>(track)->ReleaseReference();
    }
}

int ogc_track_get_id(const ogc_track_t* track, char* buffer, size_t buffer_size) {
    if (!track || !buffer || buffer_size == 0) {
        return -1;
    }
    
    std::string id = reinterpret_cast<const Track*>(track)->GetId();
    size_t copy_len = (id.length() < buffer_size - 1) ? id.length() : buffer_size - 1;
    strncpy(buffer, id.c_str(), copy_len);
    buffer[copy_len] = '\0';
    
    return static_cast<int>(id.length());
}

int ogc_track_get_name(const ogc_track_t* track, char* buffer, size_t buffer_size) {
    if (!track || !buffer || buffer_size == 0) {
        return -1;
    }
    
    std::string name = reinterpret_cast<const Track*>(track)->GetName();
    size_t copy_len = (name.length() < buffer_size - 1) ? name.length() : buffer_size - 1;
    strncpy(buffer, name.c_str(), copy_len);
    buffer[copy_len] = '\0';
    
    return static_cast<int>(name.length());
}

void ogc_track_set_name(ogc_track_t* track, const char* name) {
    if (track && name) {
        reinterpret_cast<Track*>(track)->SetName(name);
    }
}

size_t ogc_track_get_point_count(const ogc_track_t* track) {
    if (track) {
        return static_cast<size_t>(reinterpret_cast<const Track*>(track)->GetPointCount());
    }
    return 0;
}

ogc_track_point_t* ogc_track_get_point(const ogc_track_t* track, size_t index) {
    if (track) {
        return const_cast<ogc_track_point_t*>(
            reinterpret_cast<const ogc_track_point_t*>(
                reinterpret_cast<const Track*>(track)->GetPoint(static_cast<int>(index))));
    }
    return nullptr;
}

void ogc_track_add_point(ogc_track_t* track, double lat, double lon, double speed, double course, double timestamp) {
    (void)track; (void)lat; (void)lon; (void)speed; (void)course; (void)timestamp;
}

void ogc_track_clear(ogc_track_t* track) {
    if (track) {
        reinterpret_cast<Track*>(track)->ClearPoints();
    }
}

ogc_geometry_t* ogc_track_to_linestring(const ogc_track_t* track) {
    (void)track;
    return nullptr;
}

ogc_track_recorder_t* ogc_track_recorder_create(void) {
    return reinterpret_cast<ogc_track_recorder_t*>(&TrackRecorder::Instance());
}

void ogc_track_recorder_destroy(ogc_track_recorder_t* recorder) {
    (void)recorder;
}

void ogc_track_recorder_start(ogc_track_recorder_t* recorder, const char* track_name) {
    if (recorder && track_name) {
        reinterpret_cast<TrackRecorder*>(recorder)->StartRecording(track_name);
    }
}

void ogc_track_recorder_stop(ogc_track_recorder_t* recorder) {
    if (recorder) {
        reinterpret_cast<TrackRecorder*>(recorder)->StopRecording();
    }
}

void ogc_track_recorder_pause(ogc_track_recorder_t* recorder) {
    if (recorder) {
        reinterpret_cast<TrackRecorder*>(recorder)->PauseRecording();
    }
}

void ogc_track_recorder_resume(ogc_track_recorder_t* recorder) {
    if (recorder) {
        reinterpret_cast<TrackRecorder*>(recorder)->ResumeRecording();
    }
}

int ogc_track_recorder_is_recording(const ogc_track_recorder_t* recorder) {
    if (recorder) {
        return reinterpret_cast<const TrackRecorder*>(recorder)->IsRecording() ? 1 : 0;
    }
    return 0;
}

ogc_track_t* ogc_track_recorder_get_current_track(const ogc_track_recorder_t* recorder) {
    if (recorder) {
        return reinterpret_cast<ogc_track_t*>(
            reinterpret_cast<const TrackRecorder*>(recorder)->GetCurrentTrack());
    }
    return nullptr;
}

void ogc_track_recorder_set_interval(ogc_track_recorder_t* recorder, int seconds) {
    if (recorder) {
        auto config = reinterpret_cast<TrackRecorder*>(recorder)->GetConfig();
        config.min_interval_seconds = static_cast<double>(seconds);
        reinterpret_cast<TrackRecorder*>(recorder)->SetConfig(config);
    }
}

void ogc_track_recorder_set_min_distance(ogc_track_recorder_t* recorder, double meters) {
    if (recorder) {
        auto config = reinterpret_cast<TrackRecorder*>(recorder)->GetConfig();
        config.min_distance_meters = meters;
        reinterpret_cast<TrackRecorder*>(recorder)->SetConfig(config);
    }
}

ogc_position_provider_t* ogc_position_provider_create_serial(const char* port, int baud_rate) {
    if (port) {
        ProviderConfig config;
        config.device_path = port;
        config.baud_rate = baud_rate;
        IPositionProvider* provider = IPositionProvider::Create(PositionSource::Serial);
        if (provider && provider->Initialize(config)) {
            return reinterpret_cast<ogc_position_provider_t*>(provider);
        }
        delete provider;
    }
    return nullptr;
}

ogc_position_provider_t* ogc_position_provider_create_network(const char* host, int port) {
    if (host) {
        ProviderConfig config;
        config.device_path = host;
        config.baud_rate = port;
        IPositionProvider* provider = IPositionProvider::Create(PositionSource::Network);
        if (provider && provider->Initialize(config)) {
            return reinterpret_cast<ogc_position_provider_t*>(provider);
        }
        delete provider;
    }
    return nullptr;
}

void ogc_position_provider_destroy(ogc_position_provider_t* provider) {
    if (provider) {
        reinterpret_cast<IPositionProvider*>(provider)->Shutdown();
        delete reinterpret_cast<IPositionProvider*>(provider);
    }
}

int ogc_position_provider_connect(ogc_position_provider_t* provider) {
    (void)provider;
    return 0;
}

void ogc_position_provider_disconnect(ogc_position_provider_t* provider) {
    if (provider) {
        reinterpret_cast<IPositionProvider*>(provider)->Shutdown();
    }
}

int ogc_position_provider_is_connected(const ogc_position_provider_t* provider) {
    if (provider) {
        return reinterpret_cast<const IPositionProvider*>(provider)->IsConnected() ? 1 : 0;
    }
    return 0;
}

ogc_nmea_parser_t* ogc_nmea_parser_create(void) {
    return reinterpret_cast<ogc_nmea_parser_t*>(&NmeaParser::Instance());
}

void ogc_nmea_parser_destroy(ogc_nmea_parser_t* parser) {
    (void)parser;
}

int ogc_nmea_parser_parse(ogc_nmea_parser_t* parser, const char* sentence) {
    if (parser && sentence) {
        PositionData data;
        return reinterpret_cast<NmeaParser*>(parser)->Parse(sentence, data) ? 0 : -1;
    }
    return -1;
}

const char* ogc_nmea_parser_get_sentence_type(const ogc_nmea_parser_t* parser) {
    (void)parser;
    return "";
}

ogc_collision_assessor_t* ogc_collision_assessor_create(void) {
    return reinterpret_cast<ogc_collision_assessor_t*>(&CollisionAssessor::Instance());
}

void ogc_collision_assessor_destroy(ogc_collision_assessor_t* assessor) {
    (void)assessor;
}

ogc_collision_risk_t* ogc_collision_assessor_assess(ogc_collision_assessor_t* assessor,
                                                     double own_lat, double own_lon,
                                                     double own_speed, double own_course,
                                                     const ogc_ais_target_t* target) {
    (void)assessor; (void)own_lat; (void)own_lon; (void)own_speed; (void)own_course; (void)target;
    return nullptr;
}

void ogc_collision_risk_destroy(ogc_collision_risk_t* risk) {
    if (risk) {
        if (risk->risk_description) {
            free(risk->risk_description);
        }
        delete risk;
    }
}

ogc_off_course_detector_t* ogc_off_course_detector_create(void) {
    return reinterpret_cast<ogc_off_course_detector_t*>(&OffCourseDetector::Instance());
}

void ogc_off_course_detector_destroy(ogc_off_course_detector_t* detector) {
    (void)detector;
}

void ogc_off_course_detector_set_threshold(ogc_off_course_detector_t* detector, double meters) {
    if (detector) {
        reinterpret_cast<OffCourseDetector*>(detector)->SetMaxCrossTrackError(meters);
    }
}

ogc_deviation_result_t ogc_off_course_detector_check(ogc_off_course_detector_t* detector,
                                                      double lat, double lon,
                                                      const ogc_waypoint_t* from,
                                                      const ogc_waypoint_t* to) {
    ogc_deviation_result_t result = {0.0, 0.0, 0, 0};
    if (detector && from && to) {
        GeoPoint route_start(ogc_waypoint_get_longitude(from), ogc_waypoint_get_latitude(from));
        GeoPoint route_end(ogc_waypoint_get_longitude(to), ogc_waypoint_get_latitude(to));
        PositionData pos;
        pos.latitude = lat;
        pos.longitude = lon;
        OffCourseResult ocr = reinterpret_cast<OffCourseDetector*>(detector)->Detect(pos, route_start, route_end);
        result.cross_track_error = ocr.cross_track_error;
        result.distance_to_route = ocr.distance_to_route;
        result.is_off_course = ocr.is_off_course ? 1 : 0;
        result.side = 0;
    }
    return result;
}
#ifdef __cplusplus
}
#endif
