#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"
#include <vector>

using namespace ogc::jni;

extern "C" {

/* ===== LabelEngine ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_label_engine_t* engine = ogc_label_engine_create();
        return JniConverter::ToJLongPtr(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_label_engine_t* engine =
            static_cast<ogc_label_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (engine) {
            ogc_label_engine_destroy(engine);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeSetMaxLabels
  (JNIEnv* env, jobject obj, jlong ptr, jint maxLabels) {
    ogc_label_engine_t* engine =
        static_cast<ogc_label_engine_t*>(JniConverter::FromJLongPtr(ptr));
    if (engine) {
        ogc_label_engine_set_max_labels(engine, maxLabels);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeGetMaxLabels
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_label_engine_t* engine =
        static_cast<ogc_label_engine_t*>(JniConverter::FromJLongPtr(ptr));
    return engine ? ogc_label_engine_get_max_labels(engine) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeSetCollisionDetection
  (JNIEnv* env, jobject obj, jlong ptr, jboolean enable) {
    ogc_label_engine_t* engine =
        static_cast<ogc_label_engine_t*>(JniConverter::FromJLongPtr(ptr));
    if (engine) {
        ogc_label_engine_set_collision_detection(engine, enable ? 1 : 0);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelEngine_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_label_engine_t* engine =
        static_cast<ogc_label_engine_t*>(JniConverter::FromJLongPtr(ptr));
    if (engine) {
        ogc_label_engine_clear(engine);
    }
}

/* ===== LabelInfo ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeCreate
  (JNIEnv* env, jclass clazz, jstring text, jdouble x, jdouble y) {
    try {
        const char* ctext = env->GetStringUTFChars(text, nullptr);
        if (!ctext) {
            JniException::ThrowNullPointerException(env, "Text is null");
            return 0;
        }
        ogc_label_info_t* info = ogc_label_info_create(ctext, x, y);
        env->ReleaseStringUTFChars(text, ctext);
        return JniConverter::ToJLongPtr(info);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_label_info_t* info =
            static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
        if (info) {
            ogc_label_info_destroy(info);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetText
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_label_info_t* info =
        static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
    if (!info) {
        return nullptr;
    }
    const char* text = ogc_label_info_get_text(info);
    return text ? env->NewStringUTF(text) : nullptr;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_label_info_t* info =
        static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
    return info ? ogc_label_info_get_x(info) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_label_info_t* info =
        static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
    return info ? ogc_label_info_get_y(info) : 0.0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeSetText
  (JNIEnv* env, jobject obj, jlong ptr, jstring text) {
    ogc_label_info_t* info =
        static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
    if (!info || !text) {
        return;
    }
    const char* ctext = env->GetStringUTFChars(text, nullptr);
    if (ctext) {
        ogc_label_info_set_text(info, ctext);
        env->ReleaseStringUTFChars(text, ctext);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LabelInfo_nativeSetPosition
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_label_info_t* info =
        static_cast<ogc_label_info_t*>(JniConverter::FromJLongPtr(ptr));
    if (info) {
        ogc_label_info_set_position(info, x, y);
    }
}

/* ===== LODManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_lod_manager_t* mgr = ogc_lod_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_lod_manager_t* mgr =
            static_cast<ogc_lod_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_lod_manager_destroy(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeGetCurrentLevel
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_lod_manager_t* mgr =
        static_cast<ogc_lod_manager_t*>(JniConverter::FromJLongPtr(ptr));
    return mgr ? ogc_lod_manager_get_current_level(mgr) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeSetCurrentLevel
  (JNIEnv* env, jobject obj, jlong ptr, jint level) {
    ogc_lod_manager_t* mgr =
        static_cast<ogc_lod_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_lod_manager_set_current_level(mgr, level);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeGetLevelCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_lod_manager_t* mgr =
        static_cast<ogc_lod_manager_t*>(JniConverter::FromJLongPtr(ptr));
    return mgr ? ogc_lod_manager_get_level_count(mgr) : 0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_graph_LODManager_nativeGetScaleForLevel
  (JNIEnv* env, jobject obj, jlong ptr, jint level) {
    ogc_lod_manager_t* mgr =
        static_cast<ogc_lod_manager_t*>(JniConverter::FromJLongPtr(ptr));
    return mgr ? ogc_lod_manager_get_scale_for_level(mgr, level) : 0.0;
}

/* ===== HitTest ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_HitTest_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_hit_test_t* ht = ogc_hit_test_create();
        return JniConverter::ToJLongPtr(ht);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_HitTest_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_hit_test_t* ht =
            static_cast<ogc_hit_test_t*>(JniConverter::FromJLongPtr(ptr));
        if (ht) {
            ogc_hit_test_destroy(ht);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_HitTest_nativeQueryPoint
  (JNIEnv* env, jobject obj, jlong ptr, jlong layerPtr, jdouble x, jdouble y, jdouble tolerance) {
    ogc_hit_test_t* ht =
        static_cast<ogc_hit_test_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(layerPtr));
    if (!ht || !layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_hit_test_query_point(ht, layer, x, y, tolerance);
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT jlongArray JNICALL
Java_cn_cycle_chart_api_graph_HitTest_nativeQueryRect
  (JNIEnv* env, jobject obj, jlong ptr, jlong layerPtr,
   jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    ogc_hit_test_t* ht =
        static_cast<ogc_hit_test_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(layerPtr));
    if (!ht || !layer) { return nullptr; }
    int count = 0;
    ogc_feature_t** features = ogc_hit_test_query_rect(ht, layer, minX, minY, maxX, maxY, &count);
    if (!features || count <= 0) { return nullptr; }
    jlongArray result = env->NewLongArray(count);
    if (!result) { return nullptr; }
    std::vector<jlong> ptrs(count);
    for (int i = 0; i < count; ++i) {
        ptrs[i] = JniConverter::ToJLongPtr(features[i]);
    }
    env->SetLongArrayRegion(result, 0, count, ptrs.data());
    ogc_hit_test_free_features(features);
    return result;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_HitTest_nativeFreeFeatures
  (JNIEnv* env, jclass clazz, jlong featuresPtr) {
    ogc_feature_t** features = static_cast<ogc_feature_t**>(JniConverter::FromJLongPtr(featuresPtr));
    if (features) {
        ogc_hit_test_free_features(features);
    }
}

/* ===== TransformManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_TransformManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_transform_manager_t* mgr = ogc_transform_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_TransformManager_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_transform_manager_t* mgr =
            static_cast<ogc_transform_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (mgr) {
            ogc_transform_manager_destroy(mgr);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_TransformManager_nativeSetViewport
  (JNIEnv* env, jobject obj, jlong ptr, jdouble cx, jdouble cy, jdouble scale, jdouble rotation) {
    ogc_transform_manager_t* mgr =
        static_cast<ogc_transform_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (mgr) {
        ogc_transform_manager_set_viewport(mgr, cx, cy, scale, rotation);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_graph_TransformManager_nativeGetMatrix
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_transform_manager_t* mgr =
        static_cast<ogc_transform_manager_t*>(JniConverter::FromJLongPtr(ptr));
    if (!mgr) {
        return nullptr;
    }
    double matrix[9] = {0};
    ogc_transform_manager_get_matrix(mgr, matrix);
    jdoubleArray result = env->NewDoubleArray(9);
    if (result) {
        env->SetDoubleArrayRegion(result, 0, 9, matrix);
    }
    return result;
}

/* ===== RenderTask ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_RenderTask_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_render_task_t* task = ogc_render_task_create();
        return JniConverter::ToJLongPtr(task);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderTask_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_render_task_t* task =
            static_cast<ogc_render_task_t*>(JniConverter::FromJLongPtr(ptr));
        if (task) {
            ogc_render_task_destroy(task);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_graph_RenderTask_nativeExecute
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_task_t* task =
        static_cast<ogc_render_task_t*>(JniConverter::FromJLongPtr(ptr));
    if (!task) {
        return JNI_FALSE;
    }
    return ogc_render_task_execute(task) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_graph_RenderTask_nativeIsComplete
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_task_t* task =
        static_cast<ogc_render_task_t*>(JniConverter::FromJLongPtr(ptr));
    return task ? (ogc_render_task_is_complete(task) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderTask_nativeCancel
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_task_t* task =
        static_cast<ogc_render_task_t*>(JniConverter::FromJLongPtr(ptr));
    if (task) {
        ogc_render_task_cancel(task);
    }
}

/* ===== RenderQueue ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_render_queue_t* queue = ogc_render_queue_create();
        return JniConverter::ToJLongPtr(queue);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_render_queue_t* queue =
            static_cast<ogc_render_queue_t*>(JniConverter::FromJLongPtr(ptr));
        if (queue) {
            ogc_render_queue_destroy(queue);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativePush
  (JNIEnv* env, jobject obj, jlong ptr, jlong taskPtr) {
    ogc_render_queue_t* queue =
        static_cast<ogc_render_queue_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_render_task_t* task =
        static_cast<ogc_render_task_t*>(JniConverter::FromJLongPtr(taskPtr));
    if (queue && task) {
        ogc_render_queue_push(queue, task);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativePop
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_queue_t* queue =
        static_cast<ogc_render_queue_t*>(JniConverter::FromJLongPtr(ptr));
    if (!queue) {
        return 0;
    }
    ogc_render_task_t* task = ogc_render_queue_pop(queue);
    return JniConverter::ToJLongPtr(task);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativeGetSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_queue_t* queue =
        static_cast<ogc_render_queue_t*>(JniConverter::FromJLongPtr(ptr));
    return queue ? static_cast<jlong>(ogc_render_queue_get_size(queue)) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_graph_RenderQueue_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_render_queue_t* queue =
        static_cast<ogc_render_queue_t*>(JniConverter::FromJLongPtr(ptr));
    if (queue) {
        ogc_render_queue_clear(queue);
    }
}

}
