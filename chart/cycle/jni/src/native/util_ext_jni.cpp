#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== SDK Version ===== */

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_SDKVersion_nativeGetVersion
  (JNIEnv* env, jclass clazz) {
    char* ver = ogc_sdk_get_version();
    if (!ver) {
        return nullptr;
    }
    jstring result = env->NewStringUTF(ver);
    free(ver);
    return result;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionMajor
  (JNIEnv* env, jclass clazz) {
    return ogc_sdk_get_version_major();
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionMinor
  (JNIEnv* env, jclass clazz) {
    return ogc_sdk_get_version_minor();
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionPatch
  (JNIEnv* env, jclass clazz) {
    return ogc_sdk_get_version_patch();
}

/* ===== Memory Utilities ===== */

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_MemoryUtils_nativeFree
  (JNIEnv* env, jclass clazz, jlong ptr) {
    void* p = JniConverter::FromJLongPtr(ptr);
    if (p) {
        free(p);
    }
}

/* ===== PerformanceMonitor Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    ogc_performance_monitor_t* monitor = ogc_performance_monitor_get_instance();
    return JniConverter::ToJLongPtr(monitor);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeStartFrame
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_performance_monitor_t* monitor =
        static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
    if (monitor) {
        ogc_performance_monitor_start_frame(monitor);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeEndFrame
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_performance_monitor_t* monitor =
        static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
    if (monitor) {
        ogc_performance_monitor_end_frame(monitor);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetFPS
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_performance_monitor_t* monitor =
        static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
    return monitor ? ogc_performance_monitor_get_fps(monitor) : 0.0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetMemoryUsed
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_performance_monitor_t* monitor =
        static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
    return monitor ? static_cast<jlong>(ogc_performance_monitor_get_memory_used(monitor)) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeReset
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_performance_monitor_t* monitor =
        static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
    if (monitor) {
        ogc_performance_monitor_reset(monitor);
    }
}

/* ===== Error Extended ===== */

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_Error_nativeGetMessage
  (JNIEnv* env, jclass clazz, jint code) {
    const char* msg = ogc_error_get_message(static_cast<ogc_error_code_e>(code));
    return msg ? env->NewStringUTF(msg) : nullptr;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_MemoryUtils_nativeFreeMemory
  (JNIEnv* env, jclass clazz, jlong ptr) {
    void* mem = JniConverter::FromJLongPtr(ptr);
    if (mem) {
        ogc_free_memory(mem);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_MemoryUtils_nativeFreeString
  (JNIEnv* env, jclass clazz, jlong ptr) {
    char* str = static_cast<char*>(JniConverter::FromJLongPtr(ptr));
    if (str) {
        ogc_free_string(str);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_SDKVersion_nativeGetBuildDate
  (JNIEnv* env, jclass clazz) {
    char* date = ogc_sdk_get_build_date();
    if (!date) { return nullptr; }
    jstring result = env->NewStringUTF(date);
    ogc_free_string(date);
    return result;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_MemoryUtils_nativeStringArrayDestroy
  (JNIEnv* env, jclass clazz, jlong arrayPtr, jint count) {
    char** array = static_cast<char**>(JniConverter::FromJLongPtr(arrayPtr));
    if (array) {
        ogc_string_array_destroy(array, static_cast<size_t>(count));
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_MemoryUtils_nativeGeometryArrayDestroy
  (JNIEnv* env, jclass clazz, jlong arrayPtr, jint count) {
    ogc_geometry_t** array = static_cast<ogc_geometry_t**>(JniConverter::FromJLongPtr(arrayPtr));
    if (array) {
        ogc_geometry_array_destroy(array, static_cast<size_t>(count));
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetStats
  (JNIEnv* env, jobject obj, jlong statsPtr) {
    ogc_performance_stats_t* stats =
        static_cast<ogc_performance_stats_t*>(JniConverter::FromJLongPtr(statsPtr));
    if (stats) {
        ogc_performance_stats_get(stats);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_perf_RenderStats_nativeReset
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_stats_t* stats =
        static_cast<ogc_render_stats_t*>(JniConverter::FromJLongPtr(ptr));
    if (stats) {
        ogc_render_stats_reset(stats);
    }
}

}
