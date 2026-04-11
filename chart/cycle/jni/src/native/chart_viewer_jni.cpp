#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeCreate
  (JNIEnv* env, jobject obj) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_chart_viewer_t* viewer = ogc_chart_viewer_create();
        return JniConverter::ToJLongPtr(viewer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return OGC_ERROR_NULL_POINTER;
        }

        return ogc_chart_viewer_initialize(viewer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return OGC_ERROR_OPERATION_FAILED;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (viewer) {
            ogc_chart_viewer_shutdown(viewer);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeDisposeImpl
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (viewer) {
            ogc_chart_viewer_destroy(viewer);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeLoadChart
  (JNIEnv* env, jobject obj, jlong ptr, jstring filePath) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return OGC_ERROR_OUT_OF_MEMORY;
    }

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return OGC_ERROR_NULL_POINTER;
        }

        std::string path = JniConverter::ToString(env, filePath);
        int result = ogc_chart_viewer_load_chart(viewer, path.c_str());

        if (result != OGC_SUCCESS) {
            JniException::TranslateCError(env, result, "Load chart");
        }

        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return OGC_ERROR_OPERATION_FAILED;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeRender
  (JNIEnv* env, jobject obj, jlong ptr, jlong devicePtr, jint width, jint height) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return OGC_ERROR_NULL_POINTER;
        }

        ogc_draw_device_t* device =
            static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(devicePtr));

        return ogc_chart_viewer_render(viewer, device, width, height);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return OGC_ERROR_OPERATION_FAILED;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeSetViewport
  (JNIEnv* env, jobject obj, jlong ptr, jdouble centerX, jdouble centerY, jdouble scale) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        ogc_chart_viewer_set_viewport(viewer, centerX, centerY, scale);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewport
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray outViewport) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        double centerX, centerY, scale;
        ogc_chart_viewer_get_viewport(viewer, &centerX, &centerY, &scale);

        jdouble viewport[3] = {centerX, centerY, scale};
        env->SetDoubleArrayRegion(outViewport, 0, 3, viewport);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativePan
  (JNIEnv* env, jobject obj, jlong ptr, jdouble dx, jdouble dy) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        ogc_chart_viewer_pan(viewer, dx, dy);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeZoom
  (JNIEnv* env, jobject obj, jlong ptr, jdouble factor, jdouble centerX, jdouble centerY) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        ogc_chart_viewer_zoom(viewer, factor, centerX, centerY);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeQueryFeature
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return 0;
        }

        ogc_feature_t* feature = ogc_chart_viewer_query_feature(viewer, x, y);
        return JniConverter::ToJLongPtr(feature);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeScreenToWorld
  (JNIEnv* env, jobject obj, jlong ptr, jdouble screenX, jdouble screenY, jdoubleArray outCoord) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        double worldX, worldY;
        ogc_chart_viewer_screen_to_world(viewer, screenX, screenY, &worldX, &worldY);

        jdouble coord[2] = {worldX, worldY};
        env->SetDoubleArrayRegion(outCoord, 0, 2, coord);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeWorldToScreen
  (JNIEnv* env, jobject obj, jlong ptr, jdouble worldX, jdouble worldY, jdoubleArray outCoord) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        double screenX, screenY;
        ogc_chart_viewer_world_to_screen(viewer, worldX, worldY, &screenX, &screenY);

        jdouble coord[2] = {screenX, screenY};
        env->SetDoubleArrayRegion(outCoord, 0, 2, coord);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewportPtr
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return 0;
        }

        ogc_viewport_t* viewport = ogc_chart_viewer_get_viewport_ptr(viewer);
        return JniConverter::ToJLongPtr(viewport);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetFullExtent
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray outExtent) {
    JniLocalRefScope scope(env);

    try {
        ogc_chart_viewer_t* viewer =
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }

        double min_x, min_y, max_x, max_y;
        ogc_chart_viewer_get_full_extent(viewer, &min_x, &min_y, &max_x, &max_y);

        jdouble extent[4] = {min_x, min_y, max_x, max_y};
        env->SetDoubleArrayRegion(outExtent, 0, 4, extent);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
