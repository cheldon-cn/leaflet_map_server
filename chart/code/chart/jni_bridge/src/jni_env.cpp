#include "jni_bridge/jni_env.h"

#ifdef __ANDROID__

#include <android/log.h>
#include <cassert>

#define LOG_TAG "JniEnvManager"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace jni {

JniEnvManager* JniEnvManager::s_instance = nullptr;

JniEnvManager* JniEnvManager::GetInstance() {
    if (!s_instance) {
        s_instance = new JniEnvManager();
    }
    return s_instance;
}

JniEnvManager::JniEnvManager()
    : m_javaVM(nullptr)
    , m_initialized(false) {
}

JniEnvManager::~JniEnvManager() {
    Shutdown();
}

void JniEnvManager::Initialize(JavaVM* vm) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return;
    }
    
    m_javaVM = vm;
    m_initialized = true;
}

void JniEnvManager::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) {
        return;
    }
    
    m_javaVM = nullptr;
    m_initialized = false;
}

JNIEnv* JniEnvManager::GetEnv() {
    if (!m_javaVM) {
        return nullptr;
    }
    
    JNIEnv* env = nullptr;
    jint result = m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    
    if (result == JNI_OK) {
        return env;
    }
    
    return nullptr;
}

JNIEnv* JniEnvManager::AttachCurrentThread() {
    if (!m_javaVM) {
        return nullptr;
    }
    
    JNIEnv* env = nullptr;
    jint result = m_javaVM->AttachCurrentThread(&env, nullptr);
    
    if (result == JNI_OK) {
        return env;
    }
    
    LOGE("Failed to attach current thread to JVM");
    return nullptr;
}

void JniEnvManager::DetachCurrentThread() {
    if (m_javaVM) {
        m_javaVM->DetachCurrentThread();
    }
}

bool JniEnvManager::IsInitialized() const {
    return m_initialized;
}

JavaVM* JniEnvManager::GetJavaVM() const {
    return m_javaVM;
}

void JniEnvManager::RegisterNatives(const std::string& className,
                                    const JNINativeMethod* methods,
                                    int numMethods) {
    JNIEnv* env = AttachCurrentThread();
    if (!env) {
        LOGE("Failed to get JNIEnv for RegisterNatives");
        return;
    }
    
    jclass clazz = FindClass(className);
    if (!clazz) {
        LOGE("Failed to find class: %s", className.c_str());
        return;
    }
    
    jint result = env->RegisterNatives(clazz, methods, numMethods);
    if (result != JNI_OK) {
        LOGE("Failed to register natives for class: %s", className.c_str());
    }
    
    env->DeleteLocalRef(clazz);
}

void JniEnvManager::UnregisterNatives(const std::string& className) {
    JNIEnv* env = AttachCurrentThread();
    if (!env) {
        return;
    }
    
    jclass clazz = FindClass(className);
    if (clazz) {
        env->UnregisterNatives(clazz);
        env->DeleteLocalRef(clazz);
    }
}

jclass JniEnvManager::FindClass(const std::string& className) {
    JNIEnv* env = AttachCurrentThread();
    if (!env) {
        return nullptr;
    }
    
    std::string normalizedClassName = className;
    for (size_t i = 0; i < normalizedClassName.length(); ++i) {
        if (normalizedClassName[i] == '.') {
            normalizedClassName[i] = '/';
        }
    }
    
    jclass clazz = env->FindClass(normalizedClassName.c_str());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    
    return clazz;
}

jclass JniEnvManager::GetGlobalClassRef(const std::string& className) {
    jclass localRef = FindClass(className);
    if (!localRef) {
        return nullptr;
    }
    
    JNIEnv* env = AttachCurrentThread();
    jclass globalRef = static_cast<jclass>(env->NewGlobalRef(localRef));
    env->DeleteLocalRef(localRef);
    
    return globalRef;
}

void JniEnvManager::ReleaseGlobalClassRef(jclass clazz) {
    if (clazz) {
        JNIEnv* env = AttachCurrentThread();
        if (env) {
            env->DeleteGlobalRef(clazz);
        }
    }
}

jmethodID JniEnvManager::GetMethodID(jclass clazz, const std::string& name, const std::string& signature) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !clazz) {
        return nullptr;
    }
    
    jmethodID methodID = env->GetMethodID(clazz, name.c_str(), signature.c_str());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    
    return methodID;
}

jmethodID JniEnvManager::GetStaticMethodID(jclass clazz, const std::string& name, const std::string& signature) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !clazz) {
        return nullptr;
    }
    
    jmethodID methodID = env->GetStaticMethodID(clazz, name.c_str(), signature.c_str());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    
    return methodID;
}

jfieldID JniEnvManager::GetFieldID(jclass clazz, const std::string& name, const std::string& signature) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !clazz) {
        return nullptr;
    }
    
    jfieldID fieldID = env->GetFieldID(clazz, name.c_str(), signature.c_str());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    
    return fieldID;
}

jfieldID JniEnvManager::GetStaticFieldID(jclass clazz, const std::string& name, const std::string& signature) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !clazz) {
        return nullptr;
    }
    
    jfieldID fieldID = env->GetStaticFieldID(clazz, name.c_str(), signature.c_str());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    
    return fieldID;
}

jobject JniEnvManager::NewGlobalRef(jobject obj) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !obj) {
        return nullptr;
    }
    
    return env->NewGlobalRef(obj);
}

void JniEnvManager::DeleteGlobalRef(jobject obj) {
    JNIEnv* env = AttachCurrentThread();
    if (env && obj) {
        env->DeleteGlobalRef(obj);
    }
}

jobject JniEnvManager::NewLocalRef(jobject obj) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !obj) {
        return nullptr;
    }
    
    return env->NewLocalRef(obj);
}

void JniEnvManager::DeleteLocalRef(jobject obj) {
    JNIEnv* env = AttachCurrentThread();
    if (env && obj) {
        env->DeleteLocalRef(obj);
    }
}

JniEnvScope::JniEnvScope()
    : m_env(nullptr)
    , m_attached(false) {
    auto* manager = JniEnvManager::GetInstance();
    if (manager && manager->IsInitialized()) {
        m_env = manager->GetEnv();
        if (!m_env) {
            m_env = manager->AttachCurrentThread();
            m_attached = (m_env != nullptr);
        }
    }
}

JniEnvScope::~JniEnvScope() {
    if (m_attached) {
        JniEnvManager::GetInstance()->DetachCurrentThread();
    }
}

}  
}  

#endif
