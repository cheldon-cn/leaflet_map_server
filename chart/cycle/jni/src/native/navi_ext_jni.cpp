#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== PositionProvider ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeCreateSerial
  (JNIEnv* env, jclass clazz, jstring port, jint baudRate) {
    try {
        const char* cport = env->GetStringUTFChars(port, nullptr);
        if (!cport) {
            JniException::ThrowNullPointerException(env, "Port is null");
            return 0;
        }
        ogc_position_provider_t* provider = ogc_position_provider_create_serial(cport, baudRate);
        env->ReleaseStringUTFChars(port, cport);
        return JniConverter::ToJLongPtr(provider);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeCreateNetwork
  (JNIEnv* env, jclass clazz, jstring host, jint port) {
    try {
        const char* chost = env->GetStringUTFChars(host, nullptr);
        if (!chost) {
            JniException::ThrowNullPointerException(env, "Host is null");
            return 0;
        }
        ogc_position_provider_t* provider = ogc_position_provider_create_network(chost, port);
        env->ReleaseStringUTFChars(host, chost);
        return JniConverter::ToJLongPtr(provider);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_position_provider_t* provider =
            static_cast<ogc_position_provider_t*>(JniConverter::FromJLongPtr(ptr));
        if (provider) {
            ogc_position_provider_destroy(provider);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeConnect
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_position_provider_t* provider =
        static_cast<ogc_position_provider_t*>(JniConverter::FromJLongPtr(ptr));
    if (!provider) {
        return JNI_FALSE;
    }
    return ogc_position_provider_connect(provider) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeDisconnect
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_position_provider_t* provider =
        static_cast<ogc_position_provider_t*>(JniConverter::FromJLongPtr(ptr));
    if (provider) {
        ogc_position_provider_disconnect(provider);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_navi_PositionProvider_nativeIsConnected
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_position_provider_t* provider =
        static_cast<ogc_position_provider_t*>(JniConverter::FromJLongPtr(ptr));
    return provider ? (ogc_position_provider_is_connected(provider) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

}
