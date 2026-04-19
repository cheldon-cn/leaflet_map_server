#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== CircuitBreaker ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeCreate
  (JNIEnv* env, jclass clazz, jint failureThreshold, jint recoveryTimeoutMs) {
    try {
        ogc_circuit_breaker_t* breaker = ogc_circuit_breaker_create(
            static_cast<int>(failureThreshold),
            static_cast<int>(recoveryTimeoutMs));
        return JniConverter::ToJLongPtr(breaker);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) { return; }
        ogc_circuit_breaker_destroy(breaker);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeGetState
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) {
            JniException::ThrowNullPointerException(env, "CircuitBreaker is null");
            return 0;
        }
        return static_cast<jint>(ogc_circuit_breaker_get_state(breaker));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeAllowRequest
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) {
            JniException::ThrowNullPointerException(env, "CircuitBreaker is null");
            return JNI_FALSE;
        }
        return ogc_circuit_breaker_allow_request(breaker) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeRecordSuccess
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) {
            JniException::ThrowNullPointerException(env, "CircuitBreaker is null");
            return;
        }
        ogc_circuit_breaker_record_success(breaker);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeRecordFailure
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) {
            JniException::ThrowNullPointerException(env, "CircuitBreaker is null");
            return;
        }
        ogc_circuit_breaker_record_failure(breaker);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeReset
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_circuit_breaker_t* breaker =
            static_cast<ogc_circuit_breaker_t*>(JniConverter::FromJLongPtr(ptr));
        if (!breaker) {
            JniException::ThrowNullPointerException(env, "CircuitBreaker is null");
            return;
        }
        ogc_circuit_breaker_reset(breaker);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== GracefulDegradation ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_graceful_degradation_t* degradation = ogc_graceful_degradation_create();
        return JniConverter::ToJLongPtr(degradation);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) { return; }
        ogc_graceful_degradation_destroy(degradation);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeGetLevel
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) {
            JniException::ThrowNullPointerException(env, "GracefulDegradation is null");
            return 0;
        }
        return static_cast<jint>(ogc_graceful_degradation_get_level(degradation));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeSetLevel
  (JNIEnv* env, jobject obj, jlong ptr, jint level) {
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) {
            JniException::ThrowNullPointerException(env, "GracefulDegradation is null");
            return;
        }
        ogc_graceful_degradation_set_level(
            degradation, static_cast<ogc_degradation_level_e>(level));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeIsFeatureEnabled
  (JNIEnv* env, jobject obj, jlong ptr, jstring feature) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return JNI_FALSE;
    }
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) {
            JniException::ThrowNullPointerException(env, "GracefulDegradation is null");
            return JNI_FALSE;
        }
        const char* featureStr = env->GetStringUTFChars(feature, nullptr);
        if (!featureStr) { return JNI_FALSE; }
        int result = ogc_graceful_degradation_is_feature_enabled(degradation, featureStr);
        env->ReleaseStringUTFChars(feature, featureStr);
        return result ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeDisableFeature
  (JNIEnv* env, jobject obj, jlong ptr, jstring feature) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) {
            JniException::ThrowNullPointerException(env, "GracefulDegradation is null");
            return;
        }
        const char* featureStr = env->GetStringUTFChars(feature, nullptr);
        if (!featureStr) { return; }
        ogc_graceful_degradation_disable_feature(degradation, featureStr);
        env->ReleaseStringUTFChars(feature, featureStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeEnableFeature
  (JNIEnv* env, jobject obj, jlong ptr, jstring feature) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }
    try {
        ogc_graceful_degradation_t* degradation =
            static_cast<ogc_graceful_degradation_t*>(JniConverter::FromJLongPtr(ptr));
        if (!degradation) {
            JniException::ThrowNullPointerException(env, "GracefulDegradation is null");
            return;
        }
        const char* featureStr = env->GetStringUTFChars(feature, nullptr);
        if (!featureStr) { return; }
        ogc_graceful_degradation_enable_feature(degradation, featureStr);
        env->ReleaseStringUTFChars(feature, featureStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== ErrorRecoveryManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_error_recovery_manager_t* mgr = ogc_error_recovery_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) { return; }
        ogc_error_recovery_manager_destroy(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "ErrorRecoveryManager is null");
            return -1;
        }
        return ogc_error_recovery_manager_initialize(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "ErrorRecoveryManager is null");
            return;
        }
        ogc_error_recovery_manager_shutdown(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeHandleError
  (JNIEnv* env, jobject obj, jlong ptr, jint errorCode, jstring context) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "ErrorRecoveryManager is null");
            return -1;
        }
        const char* contextStr = env->GetStringUTFChars(context, nullptr);
        if (!contextStr) { return -1; }
        int result = ogc_error_recovery_manager_handle_error(
            mgr, static_cast<int>(errorCode), contextStr);
        env->ReleaseStringUTFChars(context, contextStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeSetDegradationLevel
  (JNIEnv* env, jobject obj, jlong ptr, jint level) {
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "ErrorRecoveryManager is null");
            return;
        }
        ogc_error_recovery_manager_set_degradation_level(mgr, static_cast<int>(level));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeRegisterStrategy
  (JNIEnv* env, jobject obj, jlong ptr, jint errorCode, jlong strategyPtr) {
    try {
        ogc_error_recovery_manager_t* mgr =
            static_cast<ogc_error_recovery_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "ErrorRecoveryManager is null");
            return;
        }
        void* strategy = JniConverter::FromJLongPtr(strategyPtr);
        ogc_error_recovery_manager_register_strategy(mgr, static_cast<int>(errorCode), strategy);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
