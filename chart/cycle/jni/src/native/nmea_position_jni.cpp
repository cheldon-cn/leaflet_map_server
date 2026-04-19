#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== NmeaParser ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_NmeaParser_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_nmea_parser_t* parser = ogc_nmea_parser_create();
        return JniConverter::ToJLongPtr(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_NmeaParser_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_nmea_parser_t* parser =
            static_cast<ogc_nmea_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            return;
        }
        ogc_nmea_parser_destroy(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_NmeaParser_nativeParse
  (JNIEnv* env, jobject obj, jlong ptr, jstring sentence) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }

    try {
        ogc_nmea_parser_t* parser =
            static_cast<ogc_nmea_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "NmeaParser is null");
            return -1;
        }
        const char* sentenceStr = env->GetStringUTFChars(sentence, nullptr);
        if (!sentenceStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return -1;
        }
        int result = ogc_nmea_parser_parse(parser, sentenceStr);
        env->ReleaseStringUTFChars(sentence, sentenceStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_NmeaParser_nativeGetSentenceType
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_nmea_parser_t* parser =
            static_cast<ogc_nmea_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "NmeaParser is null");
            return nullptr;
        }
        const char* type = ogc_nmea_parser_get_sentence_type(parser);
        if (!type) {
            return nullptr;
        }
        return env->NewStringUTF(type);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

/* ===== PositionManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_position_manager_t* mgr = ogc_position_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            return;
        }
        ogc_position_manager_destroy(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return -1;
        }
        return ogc_position_manager_initialize(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            return;
        }
        ogc_position_manager_shutdown(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetLatitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0.0;
        }
        return ogc_position_manager_get_latitude(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetLongitude
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0.0;
        }
        return ogc_position_manager_get_longitude(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetSpeed
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0.0;
        }
        return ogc_position_manager_get_speed(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetCourse
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0.0;
        }
        return ogc_position_manager_get_course(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetHeading
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0.0;
        }
        return ogc_position_manager_get_heading(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetFixQuality
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0;
        }
        return ogc_position_manager_get_fix_quality(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetSatelliteCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return 0;
        }
        return ogc_position_manager_get_satellite_count(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeSetPosition
  (JNIEnv* env, jobject obj, jlong ptr, jdouble latitude, jdouble longitude) {
    try {
        ogc_position_manager_t* mgr =
            static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PositionManager is null");
            return -1;
        }
        return ogc_position_manager_set_position(mgr, latitude, longitude);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeGetPosition
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_position_manager_t* mgr =
        static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) { return nullptr; }
    double lat = 0.0, lon = 0.0;
    int ret = ogc_position_manager_get_position(mgr, &lat, &lon);
    if (ret != 0) { return nullptr; }
    jdoubleArray result = env->NewDoubleArray(2);
    if (!result) { return nullptr; }
    jdouble vals[2] = {lat, lon};
    env->SetDoubleArrayRegion(result, 0, 2, vals);
    return result;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_PositionManager_nativeSetCallback
  (JNIEnv* env, jobject obj, jlong ptr, jlong callbackPtr) {
    ogc_position_manager_t* mgr =
        static_cast<ogc_position_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) { return; }
    typedef void (*pos_callback_t)(double, double, double, double, void*);
    pos_callback_t cb = reinterpret_cast<pos_callback_t>(JniConverter::FromJLongPtr(callbackPtr));
    ogc_position_manager_set_callback(mgr, cb, nullptr);
}

}
