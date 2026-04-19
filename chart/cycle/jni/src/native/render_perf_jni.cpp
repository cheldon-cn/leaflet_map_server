#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_render_optimizer_t* optimizer = ogc_render_optimizer_create();
        return JniConverter::ToJLongPtr(optimizer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_render_optimizer_t* optimizer =
            static_cast<ogc_render_optimizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!optimizer) { return; }
        ogc_render_optimizer_destroy(optimizer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeSetCacheEnabled
  (JNIEnv* env, jobject obj, jlong ptr, jboolean enable) {
    try {
        ogc_render_optimizer_t* optimizer =
            static_cast<ogc_render_optimizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!optimizer) {
            JniException::ThrowNullPointerException(env, "RenderOptimizer is null");
            return;
        }
        ogc_render_optimizer_set_cache_enabled(optimizer, enable ? 1 : 0);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeIsCacheEnabled
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_render_optimizer_t* optimizer =
            static_cast<ogc_render_optimizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!optimizer) {
            JniException::ThrowNullPointerException(env, "RenderOptimizer is null");
            return JNI_FALSE;
        }
        return ogc_render_optimizer_is_cache_enabled(optimizer) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeClearCache
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_render_optimizer_t* optimizer =
            static_cast<ogc_render_optimizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!optimizer) {
            JniException::ThrowNullPointerException(env, "RenderOptimizer is null");
            return;
        }
        ogc_render_optimizer_clear_cache(optimizer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_performance_monitor_t* monitor = ogc_performance_monitor_get_instance();
        return JniConverter::ToJLongPtr(monitor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeStartFrame
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_performance_monitor_t* monitor =
            static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!monitor) {
            JniException::ThrowNullPointerException(env, "PerformanceMonitor is null");
            return;
        }
        ogc_performance_monitor_start_frame(monitor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeEndFrame
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_performance_monitor_t* monitor =
            static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!monitor) {
            JniException::ThrowNullPointerException(env, "PerformanceMonitor is null");
            return;
        }
        ogc_performance_monitor_end_frame(monitor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetFps
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_performance_monitor_t* monitor =
            static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!monitor) {
            JniException::ThrowNullPointerException(env, "PerformanceMonitor is null");
            return 0.0;
        }
        return ogc_performance_monitor_get_fps(monitor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetMemoryUsed
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_performance_monitor_t* monitor =
            static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!monitor) {
            JniException::ThrowNullPointerException(env, "PerformanceMonitor is null");
            return 0;
        }
        return static_cast<jlong>(ogc_performance_monitor_get_memory_used(monitor));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeReset
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_performance_monitor_t* monitor =
            static_cast<ogc_performance_monitor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!monitor) {
            JniException::ThrowNullPointerException(env, "PerformanceMonitor is null");
            return;
        }
        ogc_performance_monitor_reset(monitor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
