#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== ComparisonFilter Extended ===== */

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_comparison_filter_t* filter =
            static_cast<ogc_comparison_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (filter) {
            ogc_comparison_filter_destroy(filter);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeEvaluate
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    ogc_comparison_filter_t* filter =
        static_cast<ogc_comparison_filter_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    if (!filter || !feature) {
        return JNI_FALSE;
    }
    return ogc_comparison_filter_evaluate(filter, feature) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetPropertyName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_comparison_filter_t* filter =
        static_cast<ogc_comparison_filter_t*>(JniConverter::FromJLongPtr(ptr));
    if (!filter) {
        return nullptr;
    }
    const char* name = ogc_comparison_filter_get_property_name(filter);
    return name ? env->NewStringUTF(name) : nullptr;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetOperator
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_comparison_filter_t* filter =
        static_cast<ogc_comparison_filter_t*>(JniConverter::FromJLongPtr(ptr));
    return filter ? static_cast<jint>(ogc_comparison_filter_get_operator(filter)) : 0;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetValue
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_comparison_filter_t* filter =
        static_cast<ogc_comparison_filter_t*>(JniConverter::FromJLongPtr(ptr));
    if (!filter) {
        return nullptr;
    }
    const char* val = ogc_comparison_filter_get_value(filter);
    return val ? env->NewStringUTF(val) : nullptr;
}

/* ===== Symbolizer Extended ===== */

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSymbolize
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr, jlong devicePtr) {
    ogc_symbolizer_t* symbolizer =
        static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    ogc_draw_device_t* device =
        static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(devicePtr));
    if (!symbolizer || !feature || !device) {
        return JNI_FALSE;
    }
    return ogc_symbolizer_symbolize(symbolizer, feature, device) ? JNI_TRUE : JNI_FALSE;
}

}
