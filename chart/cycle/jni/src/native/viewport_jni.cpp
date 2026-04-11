#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Viewport ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_viewport_t* viewport = ogc_viewport_create();
        return JniConverter::ToJLongPtr(viewport);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_viewport_t* viewport =
            static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
        if (viewport) {
            ogc_viewport_destroy(viewport);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeGetCenterX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (!viewport) {
        return 0.0;
    }
    return ogc_viewport_get_center_x(viewport);
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeGetCenterY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (!viewport) {
        return 0.0;
    }
    return ogc_viewport_get_center_y(viewport);
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeGetScale
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (!viewport) {
        return 1.0;
    }
    return ogc_viewport_get_scale(viewport);
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeGetRotation
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (!viewport) {
        return 0.0;
    }
    return ogc_viewport_get_rotation(viewport);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeSetCenter
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_set_center(viewport, x, y);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeSetScale
  (JNIEnv* env, jobject obj, jlong ptr, jdouble scale) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_set_scale(viewport, scale);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeSetRotation
  (JNIEnv* env, jobject obj, jlong ptr, jdouble rotation) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_set_rotation(viewport, rotation);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeGetExtent
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray outExtent) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (!viewport || !outExtent) {
        return;
    }
    
    double min_x, min_y, max_x, max_y;
    ogc_viewport_get_extent(viewport, &min_x, &min_y, &max_x, &max_y);
    
    jdouble extent[4] = {min_x, min_y, max_x, max_y};
    env->SetDoubleArrayRegion(outExtent, 0, 4, extent);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeSetExtent
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_set_extent(viewport, minX, minY, maxX, maxY);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativePan
  (JNIEnv* env, jobject obj, jlong ptr, jdouble dx, jdouble dy) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_pan(viewport, dx, dy);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeZoom
  (JNIEnv* env, jobject obj, jlong ptr, jdouble factor) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_zoom(viewport, factor);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_Viewport_nativeZoomAt
  (JNIEnv* env, jobject obj, jlong ptr, jdouble factor, jdouble centerX, jdouble centerY) {
    ogc_viewport_t* viewport =
        static_cast<ogc_viewport_t*>(JniConverter::FromJLongPtr(ptr));
    if (viewport) {
        ogc_viewport_zoom_at(viewport, factor, centerX, centerY);
    }
}

}
