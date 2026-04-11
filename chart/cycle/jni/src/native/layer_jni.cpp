#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Layer ===== */

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_layer_t* layer =
            static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
        if (layer) {
            ogc_layer_destroy(layer);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return nullptr;
    }
    const char* name = ogc_layer_get_name(layer);
    return name ? env->NewStringUTF(name) : nullptr;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    return static_cast<jint>(ogc_layer_get_type(layer));
}

/* ===== VectorLayer ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    return static_cast<jlong>(ogc_vector_layer_get_feature_count(layer));
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_layer_get_feature(layer, static_cast<long long>(fid));
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetNextFeature
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_vector_layer_get_next_feature(layer);
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilterRect
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (layer) {
        ogc_layer_set_spatial_filter_rect(layer, minX, minY, maxX, maxY);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetExtent
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray extent) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer || !extent) {
        return JNI_FALSE;
    }
    ogc_envelope_t* env_ = ogc_envelope_create();
    if (!env_) {
        return JNI_FALSE;
    }
    int result = ogc_layer_get_extent(layer, env_);
    if (result == 0) {
        jdouble buf[4];
        buf[0] = ogc_envelope_get_min_x(env_);
        buf[1] = ogc_envelope_get_min_y(env_);
        buf[2] = ogc_envelope_get_max_x(env_);
        buf[3] = ogc_envelope_get_max_y(env_);
        env->SetDoubleArrayRegion(extent, 0, 4, buf);
        ogc_envelope_destroy(env_);
        return JNI_TRUE;
    }
    ogc_envelope_destroy(env_);
    return JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeResetReading
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (layer) {
        ogc_vector_layer_reset_reading(layer);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeAddFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    if (!layer || !feature) {
        return OGC_ERROR_NULL_POINTER;
    }
    return ogc_vector_layer_add_feature(layer, feature);
}

/* ===== LayerManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_layer_manager_t* mgr = ogc_layer_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_layer_manager_t* mgr =
            static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_layer_manager_destroy(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return 0;
    }
    return static_cast<jlong>(ogc_layer_manager_get_layer_count(mgr));
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return 0;
    }
    ogc_layer_t* layer = ogc_layer_manager_get_layer(mgr, static_cast<size_t>(index));
    return JniConverter::ToJLongPtr(layer);
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerType
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return 0;
    }
    ogc_layer_t* layer = ogc_layer_manager_get_layer(mgr, static_cast<size_t>(index));
    if (!layer) {
        return 0;
    }
    return static_cast<jint>(ogc_layer_get_type(layer));
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeAddLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong layerPtr) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(layerPtr));
    if (mgr && layer) {
        ogc_layer_manager_add_layer(mgr, layer);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeRemoveLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_layer_manager_remove_layer(mgr, static_cast<size_t>(index));
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeMoveLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong fromIndex, jlong toIndex) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_layer_manager_move_layer(mgr, static_cast<size_t>(fromIndex),
                                     static_cast<size_t>(toIndex));
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerVisible
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return JNI_FALSE;
    }
    return ogc_layer_manager_get_layer_visible(mgr, static_cast<size_t>(index))
           ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerVisible
  (JNIEnv* env, jobject obj, jlong ptr, jlong index, jboolean visible) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_layer_manager_set_layer_visible(mgr, static_cast<size_t>(index),
                                            visible ? 1 : 0);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerOpacity
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return 1.0;
    }
    return ogc_layer_manager_get_layer_opacity(mgr, static_cast<size_t>(index));
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerOpacity
  (JNIEnv* env, jobject obj, jlong ptr, jlong index, jdouble opacity) {
    ogc_layer_manager_t* mgr =
        static_cast<ogc_layer_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_layer_manager_set_layer_opacity(mgr, static_cast<size_t>(index), opacity);
    }
}

}
