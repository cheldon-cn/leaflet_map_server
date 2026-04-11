#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== ChartConfig ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_chart_config_t* config = ogc_chart_config_create();
        return JniConverter::ToJLongPtr(config);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_chart_config_t* config =
            static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
        if (config) {
            ogc_chart_config_destroy(config);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeGetDisplayMode
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (!config) {
        return 0;
    }
    return ogc_chart_config_get_display_mode(config);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeSetDisplayMode
  (JNIEnv* env, jobject obj, jlong ptr, jint mode) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (config) {
        ogc_chart_config_set_display_mode(config, mode);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeGetShowGrid
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (!config) {
        return JNI_FALSE;
    }
    return ogc_chart_config_get_show_grid(config) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeSetShowGrid
  (JNIEnv* env, jobject obj, jlong ptr, jboolean show) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (config) {
        ogc_chart_config_set_show_grid(config, show ? 1 : 0);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeGetDpi
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (!config) {
        return 96.0;
    }
    return ogc_chart_config_get_dpi(config);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartConfig_nativeSetDpi
  (JNIEnv* env, jobject obj, jlong ptr, jdouble dpi) {
    ogc_chart_config_t* config =
        static_cast<ogc_chart_config_t*>(JniConverter::FromJLongPtr(ptr));
    if (config) {
        ogc_chart_config_set_dpi(config, dpi);
    }
}

}
