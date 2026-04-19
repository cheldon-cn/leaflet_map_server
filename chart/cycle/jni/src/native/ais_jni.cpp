#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

#include <vector>

using namespace ogc::jni;

extern "C" {

/* ===== AisTarget ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeCreate
  (JNIEnv* env, jclass clazz, jint mmsi) {
    try {
        ogc_ais_target_t* target = ogc_ais_target_create(
            static_cast<uint32_t>(mmsi));
        return JniConverter::ToJLongPtr(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (target) {
            ogc_ais_target_destroy(target);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetMmsi
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0;
        }
        return static_cast<jint>(ogc_ais_target_get_mmsi(target));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return nullptr;
        }

        char buffer[256] = {0};
        int ret = ogc_ais_target_get_name(target, buffer, sizeof(buffer));
        if (ret != 0) {
            return nullptr;
        }
        return JniConverter::ToJString(env, std::string(buffer));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetLatitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0.0;
        }
        return ogc_ais_target_get_latitude(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetLongitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0.0;
        }
        return ogc_ais_target_get_longitude(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetSpeed
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0.0;
        }
        return ogc_ais_target_get_speed(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetCourse
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0.0;
        }
        return ogc_ais_target_get_course(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetHeading
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0.0;
        }
        return ogc_ais_target_get_heading(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeGetNavStatus
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return 0;
        }
        return ogc_ais_target_get_nav_status(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeUpdatePosition
  (JNIEnv* env, jobject obj, jlong ptr, jdouble lat, jdouble lon,
   jdouble speed, jdouble course) {
    try {
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(ptr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return;
        }
        ogc_ais_target_update_position(target, lat, lon, speed, course);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== AisManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_ais_manager_t* mgr = ogc_ais_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_ais_manager_destroy(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return -1;
        }
        return ogc_ais_manager_initialize(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_ais_manager_shutdown(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeGetTarget
  (JNIEnv* env, jobject obj, jlong ptr, jint mmsi) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return 0;
        }
        ogc_ais_target_t* target = ogc_ais_manager_get_target(mgr,
            static_cast<uint32_t>(mmsi));
        return JniConverter::ToJLongPtr(target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jintArray JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeGetAllTargetMmsi
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return nullptr;
        }

        int count = ogc_ais_manager_get_target_count(mgr);
        if (count <= 0) {
            return nullptr;
        }

        JniException::ThrowRuntimeException(env,
            "Enumeration not supported: use getTarget(mmsi) with callback");
        return nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeGetTargetCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return 0;
        }
        return ogc_ais_manager_get_target_count(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeAddTarget
  (JNIEnv* env, jobject obj, jlong ptr, jlong targetPtr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return -1;
        }
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(targetPtr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AisTarget is null");
            return -1;
        }
        return ogc_ais_manager_add_target(mgr, target);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeRemoveTarget
  (JNIEnv* env, jobject obj, jlong ptr, jint mmsi) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "AisManager is null");
            return -1;
        }
        return ogc_ais_manager_remove_target(mgr,
            static_cast<uint32_t>(mmsi));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeSetCallback
  (JNIEnv* env, jobject obj, jlong ptr, jlong callbackPtr) {
    try {
        ogc_ais_manager_t* mgr =
            static_cast<ogc_ais_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) { return; }
        typedef void (*ais_callback_t)(uint32_t, int, void*);
        ais_callback_t cb = reinterpret_cast<ais_callback_t>(JniConverter::FromJLongPtr(callbackPtr));
        ogc_ais_manager_set_callback(mgr, cb, nullptr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisManager_nativeFreeTargets
  (JNIEnv* env, jclass clazz, jlong targetsPtr) {
    ogc_ais_target_t** targets = static_cast<ogc_ais_target_t**>(JniConverter::FromJLongPtr(targetsPtr));
    if (targets) {
        ogc_ais_manager_free_targets(targets);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_AisTarget_nativeArrayDestroy
  (JNIEnv* env, jclass clazz, jlong arrayPtr, jint count) {
    ogc_ais_target_t** array = static_cast<ogc_ais_target_t**>(JniConverter::FromJLongPtr(arrayPtr));
    if (array) {
        ogc_ais_target_array_destroy(array, static_cast<size_t>(count));
    }
}

}
