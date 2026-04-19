#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
        return JniConverter::ToJLongPtr(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            return;
        }
        ogc_navigation_engine_destroy(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return -1;
        }
        return ogc_navigation_engine_initialize(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            return;
        }
        ogc_navigation_engine_shutdown(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeSetRoute
  (JNIEnv* env, jobject obj, jlong ptr, jlong routePtr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return;
        }
        ogc_route_t* route =
            static_cast<ogc_route_t*>(JniConverter::FromJLongPtr(routePtr));
        ogc_navigation_engine_set_route(engine, route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetRoute
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0;
        }
        ogc_route_t* route = ogc_navigation_engine_get_route(engine);
        return JniConverter::ToJLongPtr(route);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStart
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return;
        }
        ogc_navigation_engine_start(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStop
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return;
        }
        ogc_navigation_engine_stop(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativePause
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return;
        }
        ogc_navigation_engine_pause(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeResume
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return;
        }
        ogc_navigation_engine_resume(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetStatus
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0;
        }
        return static_cast<jint>(ogc_navigation_engine_get_status(engine));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCurrentWaypoint
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0;
        }
        ogc_waypoint_t* wp = ogc_navigation_engine_get_current_waypoint(engine);
        return JniConverter::ToJLongPtr(wp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetDistanceToWaypoint
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0.0;
        }
        return ogc_navigation_engine_get_distance_to_waypoint(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetBearingToWaypoint
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0.0;
        }
        return ogc_navigation_engine_get_bearing_to_waypoint(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCrossTrackError
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_navigation_engine_t* engine =
            static_cast<ogc_navigation_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "NavigationEngine is null");
            return 0.0;
        }
        return ogc_navigation_engine_get_cross_track_error(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

}
