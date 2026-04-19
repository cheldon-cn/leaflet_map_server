#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_health_check_t* check = ogc_health_check_create(nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
        return JniConverter::ToJLongPtr(check);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_health_check_t* check =
            static_cast<ogc_health_check_t*>(JniConverter::FromJLongPtr(ptr));
        if (!check) { return; }
        ogc_health_check_destroy(check);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeExecute
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_health_check_t* check =
            static_cast<ogc_health_check_t*>(JniConverter::FromJLongPtr(ptr));
        if (!check) {
            JniException::ThrowNullPointerException(env, "HealthCheck is null");
            return 0;
        }
        ogc_health_check_result_t* result = ogc_health_check_execute(check);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeDestroyResult
  (JNIEnv* env, jclass clazz, jlong resultPtr) {
    try {
        ogc_health_check_result_t* result =
            static_cast<ogc_health_check_result_t*>(JniConverter::FromJLongPtr(resultPtr));
        if (!result) { return; }
        ogc_health_check_result_destroy(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultStatus
  (JNIEnv* env, jclass clazz, jlong resultPtr) {
    try {
        ogc_health_check_result_t* result =
            static_cast<ogc_health_check_result_t*>(JniConverter::FromJLongPtr(resultPtr));
        if (!result) {
            JniException::ThrowNullPointerException(env, "HealthCheckResult is null");
            return 0;
        }
        return static_cast<jint>(result->status);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultComponentName
  (JNIEnv* env, jclass clazz, jlong resultPtr) {
    try {
        ogc_health_check_result_t* result =
            static_cast<ogc_health_check_result_t*>(JniConverter::FromJLongPtr(resultPtr));
        if (!result) { return nullptr; }
        if (!result->component_name) { return nullptr; }
        return env->NewStringUTF(result->component_name);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultMessage
  (JNIEnv* env, jclass clazz, jlong resultPtr) {
    try {
        ogc_health_check_result_t* result =
            static_cast<ogc_health_check_result_t*>(JniConverter::FromJLongPtr(resultPtr));
        if (!result) { return nullptr; }
        if (!result->message) { return nullptr; }
        return env->NewStringUTF(result->message);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultDuration
  (JNIEnv* env, jclass clazz, jlong resultPtr) {
    try {
        ogc_health_check_result_t* result =
            static_cast<ogc_health_check_result_t*>(JniConverter::FromJLongPtr(resultPtr));
        if (!result) { return 0.0; }
        return result->check_duration_ms;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

}
