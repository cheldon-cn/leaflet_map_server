#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RoutePlanner_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_route_planner_t* planner = ogc_route_planner_create();
        return JniConverter::ToJLongPtr(planner);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_RoutePlanner_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_route_planner_t* planner =
            static_cast<ogc_route_planner_t*>(JniConverter::FromJLongPtr(ptr));
        if (!planner) {
            return;
        }
        ogc_route_planner_destroy(planner);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_RoutePlanner_nativePlanRoute
  (JNIEnv* env, jobject obj, jlong ptr,
   jlong startPtr, jlong endPtr) {
    try {
        ogc_route_planner_t* planner =
            static_cast<ogc_route_planner_t*>(JniConverter::FromJLongPtr(ptr));
        if (!planner) {
            JniException::ThrowNullPointerException(env, "RoutePlanner is null");
            return 0;
        }
        const ogc_waypoint_t* start =
            static_cast<const ogc_waypoint_t*>(JniConverter::FromJLongPtr(startPtr));
        const ogc_waypoint_t* end =
            static_cast<const ogc_waypoint_t*>(JniConverter::FromJLongPtr(endPtr));
        if (!start || !end) {
            JniException::ThrowNullPointerException(env, "Start or End waypoint is null");
            return 0;
        }
        ogc_route_t* route = ogc_route_planner_plan_route(planner, start, end, nullptr, 0);
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

}
