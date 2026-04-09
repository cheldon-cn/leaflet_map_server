#ifndef JNI_EXCEPTION_H
#define JNI_EXCEPTION_H

#include <jni.h>
#include <string>
#include <exception>

namespace ogc {
namespace jni {

class JniException {
public:
    static void CheckException(JNIEnv* env);
    static void ThrowException(JNIEnv* env, const std::string& className,
                               const std::string& message);
    static void ThrowRuntimeException(JNIEnv* env, const std::string& message);
    static void ThrowIOException(JNIEnv* env, const std::string& message);
    static void ThrowIllegalArgumentException(JNIEnv* env, const std::string& message);
    static void ThrowNullPointerException(JNIEnv* env, const std::string& message);
    static void ThrowOutOfMemoryError(JNIEnv* env, const std::string& message);

    static bool HasException(JNIEnv* env);
    static void ClearException(JNIEnv* env);

    static void TranslateAndThrow(JNIEnv* env, const std::exception& e);
    static void TranslateCError(JNIEnv* env, int errorCode, const std::string& context);

    static std::string GetExceptionMessage(JNIEnv* env, jthrowable exception);
};

class JniExceptionGuard {
public:
    explicit JniExceptionGuard(JNIEnv* env) : m_env(env), m_hasException(false) {
        m_hasException = env->ExceptionCheck() == JNI_TRUE;
        if (m_hasException) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }

    ~JniExceptionGuard() {
        if (!m_hasException && m_env->ExceptionCheck() == JNI_TRUE) {
            m_env->ExceptionDescribe();
        }
    }

    bool HasException() const { return m_hasException; }

private:
    JNIEnv* m_env;
    bool m_hasException;
};

}
}

#endif
