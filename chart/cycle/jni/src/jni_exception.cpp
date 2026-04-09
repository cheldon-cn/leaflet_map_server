#include "jni_exception.h"
#include "jni_env_manager.h"
#include <sstream>

namespace ogc {
namespace jni {

void JniException::CheckException(JNIEnv* env) {
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void JniException::ThrowException(JNIEnv* env, const std::string& className,
                                  const std::string& message) {
    jclass exClass = env->FindClass(className.c_str());
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    } else {
        ThrowRuntimeException(env, "Failed to find exception class: " + className);
    }
}

void JniException::ThrowRuntimeException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/RuntimeException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowIOException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/io/IOException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowIllegalArgumentException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/IllegalArgumentException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowNullPointerException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/NullPointerException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowOutOfMemoryError(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/OutOfMemoryError");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

bool JniException::HasException(JNIEnv* env) {
    return env->ExceptionCheck() == JNI_TRUE;
}

void JniException::ClearException(JNIEnv* env) {
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionClear();
    }
}

void JniException::TranslateAndThrow(JNIEnv* env, const std::exception& e) {
    std::string message = e.what();

    if (dynamic_cast<const std::bad_alloc*>(&e)) {
        ThrowOutOfMemoryError(env, message);
    } else if (dynamic_cast<const std::invalid_argument*>(&e)) {
        ThrowIllegalArgumentException(env, message);
    } else {
        ThrowRuntimeException(env, message);
    }
}

void JniException::TranslateCError(JNIEnv* env, int errorCode, const std::string& context) {
    std::ostringstream oss;
    oss << context << " failed with error code: " << errorCode;

    switch (errorCode) {
        case -1:
            ThrowIllegalArgumentException(env, oss.str());
            break;
        case -2:
            ThrowNullPointerException(env, oss.str());
            break;
        case -3:
            ThrowOutOfMemoryError(env, oss.str());
            break;
        case -7:
            ThrowIOException(env, oss.str());
            break;
        default:
            ThrowRuntimeException(env, oss.str());
            break;
    }
}

std::string JniException::GetExceptionMessage(JNIEnv* env, jthrowable exception) {
    if (!exception) {
        return "";
    }

    jclass throwableClass = env->FindClass("java/lang/Throwable");
    if (!throwableClass) {
        return "";
    }

    jmethodID getMessageMethod = env->GetMethodID(throwableClass, "getMessage",
                                                   "()Ljava/lang/String;");
    if (!getMessageMethod) {
        return "";
    }

    jstring message = static_cast<jstring>(
        env->CallObjectMethod(exception, getMessageMethod));

    if (!message) {
        return "";
    }

    const char* chars = env->GetStringUTFChars(message, nullptr);
    std::string result(chars ? chars : "");
    env->ReleaseStringUTFChars(message, chars);

    return result;
}

}
}
