/**
 * @file sdk_c_api_navi.cpp
 * @brief OGC Chart SDK C API - Navigation Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/navi/route/route.h>
#include <ogc/navi/route/waypoint.h>
#include <ogc/navi/ais/ais_manager.h>
#include <ogc/navi/navigation/navigation_engine.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_route_t* ogc_route_create(const char* name) {
    return reinterpret_cast<ogc_route_t*>(Route::Create(SafeString(name)).release());
}

void ogc_route_destroy(ogc_route_t* route) {
    delete reinterpret_cast<Route*>(route);
}

const char* ogc_route_get_name(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const Route*>(route)->GetName().c_str();
    }
    return "";
}

void ogc_route_set_name(ogc_route_t* route, const char* name) {
    if (route) {
        reinterpret_cast<Route*>(route)->SetName(SafeString(name));
    }
}

size_t ogc_route_get_waypoint_count(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const Route*>(route)->GetWaypointCount();
    }
    return 0;
}

ogc_waypoint_t* ogc_route_get_waypoint(ogc_route_t* route, size_t index) {
    if (route) {
        return reinterpret_cast<ogc_waypoint_t*>(
            reinterpret_cast<Route*>(route)->GetWaypoint(index));
    }
    return nullptr;
}

int ogc_route_add_waypoint(ogc_route_t* route, ogc_waypoint_t* waypoint) {
    if (route && waypoint) {
        return reinterpret_cast<Route*>(route)->AddWaypoint(
            WaypointPtr(reinterpret_cast<Waypoint*>(waypoint))) ? 1 : 0;
    }
    return 0;
}

int ogc_route_insert_waypoint(ogc_route_t* route, size_t index, ogc_waypoint_t* waypoint) {
    if (route && waypoint) {
        return reinterpret_cast<Route*>(route)->InsertWaypoint(index,
            WaypointPtr(reinterpret_cast<Waypoint*>(waypoint))) ? 1 : 0;
    }
    return 0;
}

int ogc_route_remove_waypoint(ogc_route_t* route, size_t index) {
    if (route) {
        return reinterpret_cast<Route*>(route)->RemoveWaypoint(index) ? 1 : 0;
    }
    return 0;
}

double ogc_route_get_total_distance(const ogc_route_t* route) {
    if (route) {
        return reinterpret_cast<const Route*>(route)->GetTotalDistance();
    }
    return 0.0;
}

double ogc_route_get_total_time(const ogc_route_t* route, double speed) {
    if (route) {
        return reinterpret_cast<const Route*>(route)->GetTotalTime(speed);
    }
    return 0.0;
}

ogc_waypoint_t* ogc_waypoint_create(double x, double y) {
    return reinterpret_cast<ogc_waypoint_t*>(Waypoint::Create(x, y).release());
}

void ogc_waypoint_destroy(ogc_waypoint_t* waypoint) {
    delete reinterpret_cast<Waypoint*>(waypoint);
}

double ogc_waypoint_get_x(const ogc_waypoint_t* waypoint) {
    if (waypoint) {
        return reinterpret_cast<const Waypoint*>(waypoint)->GetX();
    }
    return 0.0;
}

double ogc_waypoint_get_y(const ogc_waypoint_t* waypoint) {
    if (waypoint) {
        return reinterpret_cast<const Waypoint*>(waypoint)->GetY();
    }
    return 0.0;
}

void ogc_waypoint_set_position(ogc_waypoint_t* waypoint, double x, double y) {
    if (waypoint) {
        reinterpret_cast<Waypoint*>(waypoint)->SetPosition(x, y);
    }
}

const char* ogc_waypoint_get_name(const ogc_waypoint_t* waypoint) {
    if (waypoint) {
        return reinterpret_cast<const Waypoint*>(waypoint)->GetName().c_str();
    }
    return "";
}

void ogc_waypoint_set_name(ogc_waypoint_t* waypoint, const char* name) {
    if (waypoint) {
        reinterpret_cast<Waypoint*>(waypoint)->SetName(SafeString(name));
    }
}

int ogc_waypoint_get_type(const ogc_waypoint_t* waypoint) {
    if (waypoint) {
        return static_cast<int>(reinterpret_cast<const Waypoint*>(waypoint)->GetType());
    }
    return 0;
}

void ogc_waypoint_set_type(ogc_waypoint_t* waypoint, int type) {
    if (waypoint) {
        reinterpret_cast<Waypoint*>(waypoint)->SetType(static_cast<WaypointType>(type));
    }
}

double ogc_waypoint_get_arrival_radius(const ogc_waypoint_t* waypoint) {
    if (waypoint) {
        return reinterpret_cast<const Waypoint*>(waypoint)->GetArrivalRadius();
    }
    return 0.0;
}

void ogc_waypoint_set_arrival_radius(ogc_waypoint_t* waypoint, double radius) {
    if (waypoint) {
        reinterpret_cast<Waypoint*>(waypoint)->SetArrivalRadius(radius);
    }
}

ogc_ais_manager_t* ogc_ais_manager_create(void) {
    return reinterpret_cast<ogc_ais_manager_t*>(AisManager::Create().release());
}

void ogc_ais_manager_destroy(ogc_ais_manager_t* manager) {
    delete reinterpret_cast<AisManager*>(manager);
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

size_t ogc_ais_manager_get_vessel_count(const ogc_ais_manager_t* manager) {
    if (manager) {
        return reinterpret_cast<const AisManager*>(manager)->GetVesselCount();
    }
    return 0;
}

ogc_ais_vessel_t* ogc_ais_manager_get_vessel(ogc_ais_manager_t* manager, int mmsi) {
    if (manager) {
        return reinterpret_cast<ogc_ais_vessel_t*>(
            reinterpret_cast<AisManager*>(manager)->GetVessel(mmsi));
    }
    return nullptr;
}

int ogc_ais_manager_update_vessel(ogc_ais_manager_t* manager, const ogc_ais_vessel_t* vessel) {
    if (manager && vessel) {
        AisVessel v;
        v.mmsi = vessel->mmsi;
        v.position.x = vessel->position.x;
        v.position.y = vessel->position.y;
        v.course = vessel->course;
        v.speed = vessel->speed;
        v.heading = vessel->heading;
        v.ship_type = vessel->ship_type;
        v.name = SafeString(vessel->name);
        return reinterpret_cast<AisManager*>(manager)->UpdateVessel(v) ? 1 : 0;
    }
    return 0;
}

int ogc_ais_manager_remove_vessel(ogc_ais_manager_t* manager, int mmsi) {
    if (manager) {
        return reinterpret_cast<AisManager*>(manager)->RemoveVessel(mmsi) ? 1 : 0;
    }
    return 0;
}

void ogc_ais_manager_clear(ogc_ais_manager_t* manager) {
    if (manager) {
        reinterpret_cast<AisManager*>(manager)->Clear();
    }
}

ogc_navigation_engine_t* ogc_navigation_engine_create(void) {
    return reinterpret_cast<ogc_navigation_engine_t*>(NavigationEngine::Create().release());
}

void ogc_navigation_engine_destroy(ogc_navigation_engine_t* engine) {
    delete reinterpret_cast<NavigationEngine*>(engine);
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

int ogc_navigation_engine_set_route(ogc_navigation_engine_t* engine, ogc_route_t* route) {
    if (engine && route) {
        return reinterpret_cast<NavigationEngine*>(engine)->SetRoute(
            RoutePtr(reinterpret_cast<Route*>(route))) ? 1 : 0;
    }
    return 0;
}

ogc_route_t* ogc_navigation_engine_get_route(ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<ogc_route_t*>(
            reinterpret_cast<NavigationEngine*>(engine)->GetRoute());
    }
    return nullptr;
}

int ogc_navigation_engine_start(ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<NavigationEngine*>(engine)->Start() ? 1 : 0;
    }
    return 0;
}

void ogc_navigation_engine_stop(ogc_navigation_engine_t* engine) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->Stop();
    }
}

int ogc_navigation_engine_is_active(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const NavigationEngine*>(engine)->IsActive() ? 1 : 0;
    }
    return 0;
}

void ogc_navigation_engine_set_own_ship_position(ogc_navigation_engine_t* engine, double x, double y, double course, double speed) {
    if (engine) {
        reinterpret_cast<NavigationEngine*>(engine)->SetOwnShipPosition(x, y, course, speed);
    }
}

size_t ogc_navigation_engine_get_current_waypoint_index(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const NavigationEngine*>(engine)->GetCurrentWaypointIndex();
    }
    return 0;
}

double ogc_navigation_engine_get_distance_to_waypoint(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const NavigationEngine*>(engine)->GetDistanceToWaypoint();
    }
    return 0.0;
}

double ogc_navigation_engine_get_time_to_waypoint(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const NavigationEngine*>(engine)->GetTimeToWaypoint();
    }
    return 0.0;
}

double ogc_navigation_engine_get_cross_track_error(const ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const NavigationEngine*>(engine)->GetCrossTrackError();
    }
    return 0.0;
}

int ogc_navigation_engine_advance_to_next_waypoint(ogc_navigation_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<NavigationEngine*>(engine)->AdvanceToNextWaypoint() ? 1 : 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
