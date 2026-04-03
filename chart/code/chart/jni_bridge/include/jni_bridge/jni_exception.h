#ifndef OGC_JNI_BRIDGE_JNI_EXCEPTION_H
#define OGC_JNI_BRIDGE_JNI_EXCEPTION_H

#include "jni_bridge/export.h"

#ifdef __ANDROID__

#include <jni.h>
#include <string>
#include <stdexcept>
#include <functional>

namespace ogc {
namespace jni {

class OGC_JNI_BRIDGE_API JniException : public std::runtime_error {
public:
    explicit JniException(const std::string& message);
    JniException(const std::string& className, const std::string& message);
    
    const std::string& GetClassName() const { return m_className; }
    
private:
    std::string m_className;
};

class OGC_JNI_BRIDGE_API JniExceptionHandler {
public:
    static bool CheckAndClear(JNIEnv* env);
    static void ThrowIfPending(JNIEnv* env);
    
    static void ThrowNew(JNIEnv* env, const std::string& className, const std::string& message);
    static void ThrowRuntimeException(JNIEnv* env, const std::string& message);
    static void ThrowIllegalArgumentException(JNIEnv* env, const std::string& message);
    static void ThrowNullPointerException(JNIEnv* env, const std::string& message);
    static void ThrowIOException(JNIEnv* env, const std::string& message);
    
    static std::string GetStackTrace(JNIEnv* env, jthrowable exception);
    static std::string GetExceptionInfo(JNIEnv* env, jthrowable exception);
    
    static void SetExceptionHandler(std::function<void(JNIEnv*, jthrowable)> handler);
    static void ClearExceptionHandler();
    
    template<typename Func>
    static bool SafeCall(JNIEnv* env, Func&& func) {
        try {
            func();
            return !env->ExceptionCheck();
        } catch (const std::exception& e) {
            ThrowRuntimeException(env, e.what());
            return false;
        }
    }
    
    template<typename Func>
    static auto SafeCallWithResult(JNIEnv* env, Func&& func) 
        -> typename std::result_of<Func()>::type {
        using ResultType = typename std::result_of<Func()>::type;
        try {
            ResultType result = func();
            if (env->ExceptionCheck()) {
                ThrowIfPending(env);
            }
            return result;
        } catch (const std::exception& e) {
            ThrowRuntimeException(env, e.what());
            return ResultType();
        }
    }

private:
    static std::function<void(JNIEnv*, jthrowable)> s_exceptionHandler;
};

class OGC_JNI_BRIDGE_API JniExceptionScope {
public:
    explicit JniExceptionScope(JNIEnv* env);
    ~JniExceptionScope();
    
    bool HasException() const;
    void Clear();
    void Rethrow();
    std::string GetMessage() const;
    std::string GetClassName() const;
    
private:
    JNIEnv* m_env;
    jthrowable m_exception;
};

}  
}  

#endif

#endif
