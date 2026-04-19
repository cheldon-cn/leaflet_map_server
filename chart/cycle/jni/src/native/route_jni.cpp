#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Waypoint ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeCreate
  (JNIEnv* env, jclass clazz, jdouble lat, jdouble lon) {
    try {
        ogc_waypoint_t* wp = ogc_waypoint_create(lat, lon);
        return JniConverter::ToJLongPtr(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (wp) {
            ogc_waypoint_destroy(wp);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLatitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return 0.0;
        }
        return ogc_waypoint_get_latitude(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLongitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return 0.0;
        }
        return ogc_waypoint_get_longitude(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return nullptr;
        }
        const char* name = ogc_waypoint_get_name(wp);
        return name ? JniConverter::ToJString(env, std::string(name)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeSetName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        std::string n = JniConverter::ToString(env, name);
        ogc_waypoint_set_name(wp, n.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeIsArrival
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            return JNI_FALSE;
        }
        return ogc_waypoint_is_arrival(wp) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeSetArrivalRadius
  (JNIEnv* env, jobject obj, jlong ptr, jdouble radius) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        ogc_waypoint_set_arrival_radius(wp, radius);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetArrivalRadius
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            return 0.0;
        }
        return ogc_waypoint_get_arrival_radius(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeSetTurnRadius
  (JNIEnv* env, jobject obj, jlong ptr, jdouble radius) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        ogc_waypoint_set_turn_radius(wp, radius);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetTurnRadius
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            return 0.0;
        }
        return ogc_waypoint_get_turn_radius(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeSetType
  (JNIEnv* env, jobject obj, jlong ptr, jint type) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        ogc_waypoint_set_type(wp, static_cast<ogc_waypoint_type_e>(type));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            return 0;
        }
        return static_cast<jint>(ogc_waypoint_get_type(wp));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeSetDescription
  (JNIEnv* env, jobject obj, jlong ptr, jstring description) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        std::string desc = JniConverter::ToString(env, description);
        ogc_waypoint_set_description(wp, desc.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Waypoint_nativeGetDescription
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(ptr));
        if (!wp) {
            return nullptr;
        }
        const char* desc = ogc_waypoint_get_description(wp);
        return desc ? JniConverter::ToJString(env, std::string(desc)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

/* ===== Route ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_route_t* route = ogc_route_create();
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (route) {
            ogc_route_destroy(route);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetId
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return nullptr;
        }
        const char* id = ogc_route_get_id(route);
        return id ? JniConverter::ToJString(env, std::string(id)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return nullptr;
        }
        const char* name = ogc_route_get_name(route);
        return name ? JniConverter::ToJString(env, std::string(name)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeSetName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return;
        }
        std::string n = JniConverter::ToString(env, name);
        ogc_route_set_name(route, n.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetStatus
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return 0;
        }
        return static_cast<jint>(ogc_route_get_status(route));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetTotalDistance
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return 0.0;
        }
        return ogc_route_get_total_distance(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetWaypointCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return 0;
        }
        return static_cast<jint>(ogc_route_get_waypoint_count(route));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetWaypoint
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return 0;
        }
        ogc_waypoint_t* wp = ogc_route_get_waypoint(route,
            static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeAddWaypoint
  (JNIEnv* env, jobject obj, jlong ptr, jlong wpPtr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return;
        }
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(wpPtr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        ogc_route_add_waypoint(route, wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeRemoveWaypoint
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return;
        }
        ogc_route_remove_waypoint(route, static_cast<size_t>(index));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetCurrentWaypoint
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return 0;
        }
        ogc_waypoint_t* wp = ogc_route_get_current_waypoint(route);
        return JniConverter::ToJLongPtr(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeAdvanceToNextWaypoint
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return -1;
        }
        return ogc_route_advance_to_next_waypoint(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeInsertWaypoint
  (JNIEnv* env, jobject obj, jlong ptr, jint index, jlong wpPtr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return;
        }
        ogc_waypoint_t* wp =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(wpPtr));
        if (!wp) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return;
        }
        ogc_route_insert_waypoint(route, static_cast<size_t>(index), wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return;
        }
        ogc_route_clear(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeReverse
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return;
        }
        ogc_route_reverse(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Route_nativeGetEta
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(ptr));
        if (!route) {
            return 0;
        }
        return static_cast<jlong>(ogc_route_get_eta(route));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== RouteManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_route_manager_t* mgr = ogc_route_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_route_manager_destroy(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRouteCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            return 0;
        }
        return static_cast<jint>(ogc_route_manager_get_route_count(mgr));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRoute
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            return 0;
        }
        ogc_route_t* route = ogc_route_manager_get_route(mgr,
            static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRouteById
  (JNIEnv* env, jobject obj, jlong ptr, jstring id) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "RouteManager is null");
            return 0;
        }
        std::string routeId = JniConverter::ToString(env, id);
        ogc_route_t* route = ogc_route_manager_get_route_by_id(mgr, routeId.c_str());
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeAddRoute
  (JNIEnv* env, jobject obj, jlong ptr, jlong routePtr) {
    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "RouteManager is null");
            return;
        }
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(routePtr));
        if (!route) {
            JniException::ThrowNullPointerException(env, "Route is null");
            return;
        }
        ogc_route_manager_add_route(mgr, route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeRemoveRoute
  (JNIEnv* env, jobject obj, jlong ptr, jstring id) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "RouteManager is null");
            return;
        }
        std::string routeId = JniConverter::ToString(env, id);
        ogc_route_manager_remove_route(mgr, routeId.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeGetActiveRoute
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            return 0;
        }
        ogc_route_t* route = ogc_route_manager_get_active_route(mgr);
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_RouteManager_nativeSetActiveRoute
  (JNIEnv* env, jobject obj, jlong ptr, jstring id) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_route_manager_t* mgr =
            static_cast<ogc_route_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "RouteManager is null");
            return;
        }
        std::string routeId = JniConverter::ToString(env, id);
        ogc_route_manager_set_active_route(mgr, routeId.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
