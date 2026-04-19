#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_exception_ChartException_nativeCreate
  (JNIEnv* env, jclass clazz, jint errorCode, jstring message) {
    const char* cmsg = env->GetStringUTFChars(message, nullptr);
    if (!cmsg) { return 0; }
    ogc_chart_exception_t* ex = ogc_chart_exception_create(errorCode, cmsg);
    env->ReleaseStringUTFChars(message, cmsg);
    return JniConverter::ToJLongPtr(ex);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_exception_ChartException_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_chart_exception_t* ex =
        static_cast<ogc_chart_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (ex) {
        ogc_chart_exception_destroy(ex);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_exception_ChartException_nativeGetErrorCode
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_exception_t* ex =
        static_cast<ogc_chart_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (!ex) { return 0; }
    return ogc_chart_exception_get_error_code(ex);
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_exception_ChartException_nativeGetMessage
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_exception_t* ex =
        static_cast<ogc_chart_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (!ex) { return nullptr; }
    const char* msg = ogc_chart_exception_get_message(ex);
    return msg ? env->NewStringUTF(msg) : nullptr;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_exception_ChartException_nativeGetContext
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_chart_exception_t* ex =
        static_cast<ogc_chart_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (!ex) { return nullptr; }
    const char* ctx = ogc_chart_exception_get_context(ex);
    return ctx ? env->NewStringUTF(ctx) : nullptr;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_exception_JniException_nativeCreate
  (JNIEnv* env, jclass clazz, jstring message, jstring javaClass) {
    const char* cmsg = env->GetStringUTFChars(message, nullptr);
    if (!cmsg) { return 0; }
    const char* cclass = env->GetStringUTFChars(javaClass, nullptr);
    if (!cclass) {
        env->ReleaseStringUTFChars(message, cmsg);
        return 0;
    }
    ogc_jni_exception_t* ex = ogc_jni_exception_create(cmsg, cclass);
    env->ReleaseStringUTFChars(javaClass, cclass);
    env->ReleaseStringUTFChars(message, cmsg);
    return JniConverter::ToJLongPtr(ex);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_exception_JniException_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_jni_exception_t* ex =
        static_cast<ogc_jni_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (ex) {
        ogc_jni_exception_destroy(ex);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_exception_RenderException_nativeCreate
  (JNIEnv* env, jclass clazz, jstring message, jstring layerName) {
    const char* cmsg = env->GetStringUTFChars(message, nullptr);
    if (!cmsg) { return 0; }
    const char* clayer = env->GetStringUTFChars(layerName, nullptr);
    if (!clayer) {
        env->ReleaseStringUTFChars(message, cmsg);
        return 0;
    }
    ogc_render_exception_t* ex = ogc_render_exception_create(cmsg, clayer);
    env->ReleaseStringUTFChars(layerName, clayer);
    env->ReleaseStringUTFChars(message, cmsg);
    return JniConverter::ToJLongPtr(ex);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_exception_RenderException_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_render_exception_t* ex =
        static_cast<ogc_render_exception_t*>(JniConverter::FromJLongPtr(ptr));
    if (ex) {
        ogc_render_exception_destroy(ex);
    }
}

}
