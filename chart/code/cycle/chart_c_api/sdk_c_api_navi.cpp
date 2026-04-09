/**
 * @file sdk_c_api_navi.cpp
 * @brief OGC Chart SDK C API - Navigation Module Implementation (Stub)
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/navi/ais/ais_manager.h>
#include <ogc/navi/ais/ais_target.h>
#include <ogc/navi/types.h>
#include <ogc/navi/navigation/navigation_engine.h>
#include <ogc/navi/route/route.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;
using namespace ogc::navi;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

struct LocalWaypointData {
    double lat;
    double lon;
    std::string name;
    double arrivalRadius;
    bool isArrival;
};

struct LocalRouteData {
    std::string id;
    std::string name;
    std::vector<LocalWaypointData*> waypoints;
    ogc_route_status_e status;
};

}  

ogc_waypoint_t* ogc_waypoint_create(double lat, double lon) {
    LocalWaypointData* wp = new LocalWaypointData();
    wp->lat = lat;
    wp->lon = lon;
    wp->arrivalRadius = 0.0;
    wp->isArrival = false;
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
    return "";
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
    return "";
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
    return nullptr;
}

int ogc_route_advance_to_next_waypoint(ogc_route_t* route) {
    return 0;
}

ogc_route_manager_t* ogc_route_manager_create(void) {
    return nullptr;
}

void ogc_route_manager_destroy(ogc_route_manager_t* mgr) {
}

size_t ogc_route_manager_get_route_count(const ogc_route_manager_t* mgr) {
    return 0;
}

ogc_route_t* ogc_route_manager_get_route(const ogc_route_manager_t* mgr, size_t index) {
    return nullptr;
}

ogc_route_t* ogc_route_manager_get_route_by_id(const ogc_route_manager_t* mgr, const char* id) {
    return nullptr;
}

void ogc_route_manager_add_route(ogc_route_manager_t* mgr, ogc_route_t* route) {
}

void ogc_route_manager_remove_route(ogc_route_manager_t* mgr, const char* id) {
}

ogc_route_t* ogc_route_manager_get_active_route(const ogc_route_manager_t* mgr) {
    return nullptr;
}

void ogc_route_manager_set_active_route(ogc_route_manager_t* mgr, const char* id) {
}

ogc_route_planner_t* ogc_route_planner_create(void) {
    return nullptr;
}

void ogc_route_planner_destroy(ogc_route_planner_t* planner) {
}

ogc_route_t* ogc_route_planner_plan_route(ogc_route_planner_t* planner, 
                                           const ogc_waypoint_t* start, 
                                           const ogc_waypoint_t* end,
                                           const ogc_envelope_t* avoid_areas,
                                           int avoid_count) {
    return nullptr;
}

ogc_ais_target_t* ogc_ais_target_create(uint32_t mmsi) {
    return nullptr;
}

void ogc_ais_target_destroy(ogc_ais_target_t* target) {
}

uint32_t ogc_ais_target_get_mmsi(const ogc_ais_target_t* target) {
    return 0;
}

const char* ogc_ais_target_get_name(const ogc_ais_target_t* target) {
    return "";
}

double ogc_ais_target_get_latitude(const ogc_ais_target_t* target) {
    return 0.0;
}

double ogc_ais_target_get_longitude(const ogc_ais_target_t* target) {
    return 0.0;
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
#ifdef __cplusplus
}
#endif
