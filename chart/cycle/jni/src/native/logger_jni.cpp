#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_logger_t* logger = ogc_logger_get_instance();
        return JniConverter::ToJLongPtr(logger);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeSetLevel
  (JNIEnv* env, jobject obj, jlong ptr, jint level) {
    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        ogc_logger_set_level(logger, static_cast<int>(level));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeTrace
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_trace(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeDebug
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_debug(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeInfo
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_info(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeWarning
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_warning(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeError
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_error(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeFatal
  (JNIEnv* env, jobject obj, jlong ptr, jstring message) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string msg = JniConverter::ToString(env, message);
        ogc_logger_fatal(logger, msg.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeSetLogFile
  (JNIEnv* env, jobject obj, jlong ptr, jstring filepath) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        std::string path = JniConverter::ToString(env, filepath);
        ogc_logger_set_log_file(logger, path.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_base_Logger_nativeSetConsoleOutput
  (JNIEnv* env, jobject obj, jlong ptr, jint enable) {
    try {
        ogc_logger_t* logger =
            static_cast<ogc_logger_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logger) {
            JniException::ThrowNullPointerException(env, "Logger is null");
            return;
        }
        ogc_logger_set_console_output(logger, static_cast<int>(enable));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
