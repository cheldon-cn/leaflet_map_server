#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

#include <vector>

using namespace ogc::jni;

extern "C" {

/* ===== Alert ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeCreate
  (JNIEnv* env, jclass clazz, jint type, jint level, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        std::string msg = JniConverter::ToString(env, message);
        ogc_alert_t* alert = ogc_alert_create(
            static_cast<ogc_alert_type_e>(type),
            static_cast<ogc_alert_level_e>(level),
            msg.c_str());
        return JniConverter::ToJLongPtr(alert);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (alert) {
            ogc_alert_destroy(alert);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return 0;
        }
        return static_cast<jint>(ogc_alert_get_type(alert));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetLevel
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return 0;
        }
        return static_cast<jint>(ogc_alert_get_level(alert));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetMessage
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return nullptr;
        }
        const char* msg = ogc_alert_get_message(alert);
        return msg ? JniConverter::ToJString(env, std::string(msg)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetTimestamp
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return 0.0;
        }
        return ogc_alert_get_timestamp(alert);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetId
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return 0;
        }
        return static_cast<jlong>(ogc_alert_get_id(alert));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetSeverity
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            return 0;
        }
        return static_cast<jint>(ogc_alert_get_severity(alert));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetSeverity
  (JNIEnv* env, jobject obj, jlong ptr, jint severity) {
    try {
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
        if (!alert) {
            JniException::ThrowNullPointerException(env, "Alert is null");
            return;
        }
        ogc_alert_set_severity(alert, static_cast<ogc_alert_severity_e>(severity));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== AlertEngine ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_alert_engine_t* engine = ogc_alert_engine_create();
        return JniConverter::ToJLongPtr(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (engine) {
            ogc_alert_engine_destroy(engine);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return -1;
        }
        return ogc_alert_engine_initialize(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (engine) {
            ogc_alert_engine_shutdown(engine);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeCheckAll
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return;
        }
        ogc_alert_engine_check_all(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlongArray JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetActiveAlertIds
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return nullptr;
        }

        size_t count = ogc_alert_engine_get_alert_count(engine);
        if (count == 0) {
            return nullptr;
        }

        JniException::ThrowRuntimeException(env,
            "Enumeration not supported: use getAlert(id) with callback");
        return nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeAcknowledgeAlert
  (JNIEnv* env, jobject obj, jlong ptr, jlong alertPtr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return;
        }
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(alertPtr));
        if (!alert) {
            JniException::ThrowNullPointerException(env, "Alert is null");
            return;
        }
        ogc_alert_engine_acknowledge_alert(engine, alert);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeAddAlert
  (JNIEnv* env, jobject obj, jlong ptr, jlong alertPtr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return -1;
        }
        ogc_alert_t* alert =
            static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(alertPtr));
        if (!alert) {
            JniException::ThrowNullPointerException(env, "Alert is null");
            return -1;
        }
        return ogc_alert_engine_add_alert(engine, alert);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeRemoveAlert
  (JNIEnv* env, jobject obj, jlong ptr, jlong alertId) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            JniException::ThrowNullPointerException(env, "AlertEngine is null");
            return -1;
        }
        return ogc_alert_engine_remove_alert(engine,
            static_cast<int64_t>(alertId));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlert
  (JNIEnv* env, jobject obj, jlong ptr, jlong alertId) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            return 0;
        }
        ogc_alert_t* alert = ogc_alert_engine_get_alert(engine,
            static_cast<int64_t>(alertId));
        return JniConverter::ToJLongPtr(alert);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlertCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (!engine) {
            return 0;
        }
        return static_cast<jint>(ogc_alert_engine_get_alert_count(engine));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeClearAlerts
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_alert_engine_t* engine =
            static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (engine) {
            ogc_alert_engine_clear_alerts(engine);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== CPA Calculation ===== */

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_alert_CpaCalculator_nativeCalculate
  (JNIEnv* env, jclass clazz,
   jdouble ownLat, jdouble ownLon, jdouble ownSpeed, jdouble ownCourse,
   jdouble tgtLat, jdouble tgtLon, jdouble tgtSpeed, jdouble tgtCourse) {
    try {
        ogc_cpa_result_t result;
        ogc_cpa_calculate(ownLat, ownLon, ownSpeed, ownCourse,
                          tgtLat, tgtLon, tgtSpeed, tgtCourse, &result);

        jclass cpaClass = env->FindClass("cn/cycle/chart/api/alert/CpaResult");
        if (!cpaClass) {
            JniException::ThrowRuntimeException(env, "CpaResult class not found");
            return nullptr;
        }

        jmethodID ctor = env->GetMethodID(cpaClass, "<init>", "(DDDI)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "CpaResult constructor not found");
            return nullptr;
        }

        return env->NewObject(cpaClass, ctor,
            result.cpa, result.tcpa, result.bearing, result.danger_level);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

/* ===== UKC Calculation ===== */

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_alert_UkcCalculator_nativeCalculate
  (JNIEnv* env, jclass clazz,
   jdouble chartedDepth, jdouble tideHeight, jdouble draft,
   jdouble speed, jdouble squatCoeff) {
    try {
        ogc_ukc_result_t result;
        ogc_ukc_calculate(chartedDepth, tideHeight, draft,
                          speed, squatCoeff, &result);

        jclass ukcClass = env->FindClass("cn/cycle/chart/api/alert/UkcResult");
        if (!ukcClass) {
            JniException::ThrowRuntimeException(env, "UkcResult class not found");
            return nullptr;
        }

        jmethodID ctor = env->GetMethodID(ukcClass, "<init>", "(DDDDDI)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "UkcResult constructor not found");
            return nullptr;
        }

        return env->NewObject(ukcClass, ctor,
            result.water_depth, result.charted_depth,
            result.tide_height, result.squat,
            result.ukc, result.safe);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeGetPosition
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (!alert) { return nullptr; }
    ogc_coordinate_t pos = ogc_alert_get_position(alert);
    jdoubleArray result = env->NewDoubleArray(2);
    if (!result) { return nullptr; }
    jdouble coords[2] = {pos.x, pos.y};
    env->SetDoubleArrayRegion(result, 0, 2, coords);
    return result;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetPosition
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (!alert) { return; }
    ogc_coordinate_t pos;
    pos.x = x;
    pos.y = y;
    ogc_alert_set_position(alert, &pos);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetId
  (JNIEnv* env, jobject obj, jlong ptr, jlong id) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (alert) {
        ogc_alert_set_id(alert, id);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetMessage
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (!alert || !message) { return; }
    const char* cmsg = env->GetStringUTFChars(message, nullptr);
    if (!cmsg) { return; }
    ogc_alert_set_message(alert, cmsg);
    env->ReleaseStringUTFChars(message, cmsg);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetTimestamp
  (JNIEnv* env, jobject obj, jlong ptr, jlong timestamp) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (alert) {
        ogc_alert_set_timestamp(alert, timestamp);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeSetAcknowledged
  (JNIEnv* env, jobject obj, jlong ptr, jboolean acknowledged) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (alert) {
        ogc_alert_set_acknowledged(alert, acknowledged ? 1 : 0);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeIsAcknowledged
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_alert_t* alert =
        static_cast<ogc_alert_t*>(JniConverter::FromJLongPtr(ptr));
    if (!alert) { return JNI_FALSE; }
    return ogc_alert_is_acknowledged(alert) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeSetCallback
  (JNIEnv* env, jobject obj, jlong ptr, jlong callbackPtr) {
    ogc_alert_engine_t* engine =
        static_cast<ogc_alert_engine_t*>(JniConverter::FromJLongPtr(ptr));
    if (!engine) { return; }
    typedef void (*alert_callback_t)(int, int, const char*, void*);
    alert_callback_t cb = reinterpret_cast<alert_callback_t>(JniConverter::FromJLongPtr(callbackPtr));
    ogc_alert_engine_set_callback(engine, cb, nullptr);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_AlertEngine_nativeFreeAlerts
  (JNIEnv* env, jclass clazz, jlong alertsPtr) {
    ogc_alert_t** alerts = static_cast<ogc_alert_t**>(JniConverter::FromJLongPtr(alertsPtr));
    if (alerts) {
        ogc_alert_engine_free_alerts(alerts);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_alert_Alert_nativeArrayDestroy
  (JNIEnv* env, jclass clazz, jlong arrayPtr, jint count) {
    ogc_alert_t** array = static_cast<ogc_alert_t**>(JniConverter::FromJLongPtr(arrayPtr));
    if (array) {
        ogc_alert_array_destroy(array, static_cast<size_t>(count));
    }
}

}
