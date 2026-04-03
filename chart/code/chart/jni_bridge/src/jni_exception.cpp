#include "jni_bridge/jni_exception.h"

#ifdef __ANDROID__

#include <android/log.h>
#include <sstream>

#define LOG_TAG "JniException"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace jni {

std::function<void(JNIEnv*, jthrowable)> JniExceptionHandler::s_exceptionHandler;

JniException::JniException(const std::string& message)
    : std::runtime_error(message)
    , m_className("java/lang/Exception") {
}

JniException::JniException(const std::string& className, const std::string& message)
    : std::runtime_error(message)
    , m_className(className) {
}

bool JniExceptionHandler::CheckAndClear(JNIEnv* env) {
    if (!env) {
        return false;
    }
    
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        env->ExceptionClear();
        
        if (s_exceptionHandler) {
            s_exceptionHandler(env, exception);
        }
        
        env->DeleteLocalRef(exception);
        return true;
    }
    
    return false;
}

void JniExceptionHandler::ThrowIfPending(JNIEnv* env) {
    if (!env) {
        return;
    }
    
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        env->ExceptionClear();
        
        std::string info = GetExceptionInfo(env, exception);
        env->DeleteLocalRef(exception);
        
        throw JniException(info);
    }
}

void JniExceptionHandler::ThrowNew(JNIEnv* env, const std::string& className, const std::string& message) {
    if (!env) {
        return;
    }
    
    jclass exceptionClass = env->FindClass(className.c_str());
    if (exceptionClass) {
        env->ThrowNew(exceptionClass, message.c_str());
        env->DeleteLocalRef(exceptionClass);
    }
}

void JniExceptionHandler::ThrowRuntimeException(JNIEnv* env, const std::string& message) {
    ThrowNew(env, "java/lang/RuntimeException", message);
}

void JniExceptionHandler::ThrowIllegalArgumentException(JNIEnv* env, const std::string& message) {
    ThrowNew(env, "java/lang/IllegalArgumentException", message);
}

void JniExceptionHandler::ThrowNullPointerException(JNIEnv* env, const std::string& message) {
    ThrowNew(env, "java/lang/NullPointerException", message);
}

void JniExceptionHandler::ThrowIOException(JNIEnv* env, const std::string& message) {
    ThrowNew(env, "java/io/IOException", message);
}

std::string JniExceptionHandler::GetStackTrace(JNIEnv* env, jthrowable exception) {
    if (!env || !exception) {
        return "";
    }
    
    jclass stringWriterClass = env->FindClass("java/io/StringWriter");
    jclass printWriterClass = env->FindClass("java/io/PrintWriter");
    
    jmethodID stringWriterConstructor = env->GetMethodID(stringWriterClass, "<init>", "()V");
    jobject stringWriter = env->NewObject(stringWriterClass, stringWriterConstructor);
    
    jmethodID printWriterConstructor = env->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V");
    jobject printWriter = env->NewObject(printWriterClass, printWriterConstructor, stringWriter);
    
    jclass throwableClass = env->FindClass("java/lang/Throwable");
    jmethodID printStackTraceMethod = env->GetMethodID(throwableClass, "printStackTrace", "(Ljava/io/PrintWriter;)V");
    env->CallVoidMethod(exception, printStackTraceMethod, printWriter);
    
    jmethodID toStringMethod = env->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");
    jstring stackTrace = static_cast<jstring>(env->CallObjectMethod(stringWriter, toStringMethod));
    
    const char* chars = env->GetStringUTFChars(stackTrace, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(stackTrace, chars);
    
    env->DeleteLocalRef(stackTrace);
    env->DeleteLocalRef(printWriter);
    env->DeleteLocalRef(stringWriter);
    env->DeleteLocalRef(throwableClass);
    env->DeleteLocalRef(printWriterClass);
    env->DeleteLocalRef(stringWriterClass);
    
    return result;
}

std::string JniExceptionHandler::GetExceptionInfo(JNIEnv* env, jthrowable exception) {
    if (!env || !exception) {
        return "";
    }
    
    std::ostringstream oss;
    
    jclass throwableClass = env->GetObjectClass(exception);
    
    jmethodID getMessageMethod = env->GetMethodID(throwableClass, "getMessage", "()Ljava/lang/String;");
    jstring message = static_cast<jstring>(env->CallObjectMethod(exception, getMessageMethod));
    
    jmethodID getClassMethod = env->GetMethodID(throwableClass, "getClass", "()Ljava/lang/Class;");
    jobject exceptionClass = env->CallObjectMethod(exception, getClassMethod);
    
    jclass classClass = env->GetObjectClass(exceptionClass);
    jmethodID getNameMethod = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
    jstring className = static_cast<jstring>(env->CallObjectMethod(exceptionClass, getNameMethod));
    
    const char* classNameChars = env->GetStringUTFChars(className, nullptr);
    oss << "Exception: " << classNameChars;
    env->ReleaseStringUTFChars(className, classNameChars);
    
    if (message) {
        const char* messageChars = env->GetStringUTFChars(message, nullptr);
        oss << " - " << messageChars;
        env->ReleaseStringUTFChars(message, messageChars);
    }
    
    oss << "\n" << GetStackTrace(env, exception);
    
    env->DeleteLocalRef(className);
    env->DeleteLocalRef(classClass);
    env->DeleteLocalRef(exceptionClass);
    env->DeleteLocalRef(message);
    env->DeleteLocalRef(throwableClass);
    
    return oss.str();
}

void JniExceptionHandler::SetExceptionHandler(std::function<void(JNIEnv*, jthrowable)> handler) {
    s_exceptionHandler = std::move(handler);
}

void JniExceptionHandler::ClearExceptionHandler() {
    s_exceptionHandler = nullptr;
}

JniExceptionScope::JniExceptionScope(JNIEnv* env)
    : m_env(env)
    , m_exception(nullptr) {
    if (m_env && m_env->ExceptionCheck()) {
        m_exception = m_env->ExceptionOccurred();
        m_env->ExceptionClear();
    }
}

JniExceptionScope::~JniExceptionScope() {
    if (m_exception) {
        m_env->DeleteLocalRef(m_exception);
    }
}

bool JniExceptionScope::HasException() const {
    return m_exception != nullptr;
}

void JniExceptionScope::Clear() {
    if (m_exception) {
        m_env->DeleteLocalRef(m_exception);
        m_exception = nullptr;
    }
}

void JniExceptionScope::Rethrow() {
    if (m_exception) {
        m_env->Throw(m_exception);
    }
}

std::string JniExceptionScope::GetMessage() const {
    if (!m_exception) {
        return "";
    }
    
    jclass throwableClass = m_env->GetObjectClass(m_exception);
    jmethodID getMessageMethod = m_env->GetMethodID(throwableClass, "getMessage", "()Ljava/lang/String;");
    jstring message = static_cast<jstring>(m_env->CallObjectMethod(m_exception, getMessageMethod));
    
    std::string result;
    if (message) {
        const char* chars = m_env->GetStringUTFChars(message, nullptr);
        result = chars;
        m_env->ReleaseStringUTFChars(message, chars);
        m_env->DeleteLocalRef(message);
    }
    
    m_env->DeleteLocalRef(throwableClass);
    return result;
}

std::string JniExceptionScope::GetClassName() const {
    if (!m_exception) {
        return "";
    }
    
    jclass throwableClass = m_env->GetObjectClass(m_exception);
    jmethodID getNameMethod = m_env->GetMethodID(throwableClass, "getName", "()Ljava/lang/String;");
    jstring name = static_cast<jstring>(m_env->CallObjectMethod(throwableClass, getNameMethod));
    
    std::string result;
    if (name) {
        const char* chars = m_env->GetStringUTFChars(name, nullptr);
        result = chars;
        m_env->ReleaseStringUTFChars(name, chars);
        m_env->DeleteLocalRef(name);
    }
    
    m_env->DeleteLocalRef(throwableClass);
    return result;
}

}  
}  

#endif
