#include "jni_env_manager.h"
#include "jni_exception.h"
#include <sstream>

namespace ogc {
namespace jni {

JniEnvManager* JniEnvManager::GetInstance() {
    static JniEnvManager instance;
    return &instance;
}

JniEnvManager::JniEnvManager()
    : m_javaVM(nullptr), m_initialized(false) {
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

    JNIEnv* env = GetEnv();
    for (auto& pair : m_globalClassRefs) {
        if (env && pair.second) {
            env->DeleteGlobalRef(pair.second);
        }
    }
    m_globalClassRefs.clear();
    m_methodIDCache.clear();
    m_fieldIDCache.clear();

    for (auto& pair : m_attachedThreads) {
        if (pair.second && m_javaVM) {
            m_javaVM->DetachCurrentThread();
        }
    }
    m_attachedThreads.clear();

    m_javaVM = nullptr;
    m_initialized = false;
}

JNIEnv* JniEnvManager::GetEnv() {
    if (!m_javaVM) {
        return nullptr;
    }

    JNIEnv* env = nullptr;
    jint result = m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8);

    if (result == JNI_OK) {
        return env;
    }
    return nullptr;
}

JNIEnv* JniEnvManager::AttachCurrentThread() {
    if (!m_javaVM) {
        return nullptr;
    }

    JNIEnv* env = GetEnv();
    if (env) {
        return env;
    }

    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_attachedThreads.find(tid) == m_attachedThreads.end()) {
        JavaVMAttachArgs args = {JNI_VERSION_1_8, "JNI Thread", nullptr};
        m_javaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), &args);
        m_attachedThreads[tid] = true;
    } else {
        env = GetEnv();
    }

    return env;
}

void JniEnvManager::DetachCurrentThread() {
    if (!m_javaVM) {
        return;
    }

    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_attachedThreads.find(tid);
    if (it != m_attachedThreads.end()) {
        m_javaVM->DetachCurrentThread();
        m_attachedThreads.erase(it);
    }
}

jclass JniEnvManager::FindClass(const std::string& className) {
    JNIEnv* env = GetEnv();
    if (!env) {
        return nullptr;
    }

    auto it = m_globalClassRefs.find(className);
    if (it != m_globalClassRefs.end()) {
        return it->second;
    }

    jclass localClass = env->FindClass(className.c_str());
    if (!localClass) {
        return nullptr;
    }

    jclass globalClass = static_cast<jclass>(env->NewGlobalRef(localClass));
    env->DeleteLocalRef(localClass);

    if (globalClass) {
        m_globalClassRefs[className] = globalClass;
    }

    return globalClass;
}

jclass JniEnvManager::GetGlobalClassRef(const std::string& className) {
    return FindClass(className);
}

void JniEnvManager::ReleaseGlobalClassRef(jclass clazz) {
    JNIEnv* env = GetEnv();
    if (env && clazz) {
        env->DeleteGlobalRef(clazz);
    }
}

jmethodID JniEnvManager::GetMethodID(jclass clazz, const std::string& name,
                                     const std::string& signature) {
    JNIEnv* env = GetEnv();
    if (!env || !clazz) {
        return nullptr;
    }

    std::string key = name + "#" + signature;
    auto it = m_methodIDCache.find(key);
    if (it != m_methodIDCache.end()) {
        return it->second;
    }

    jmethodID mid = env->GetMethodID(clazz, name.c_str(), signature.c_str());
    if (mid) {
        m_methodIDCache[key] = mid;
    }

    return mid;
}

jmethodID JniEnvManager::GetStaticMethodID(jclass clazz, const std::string& name,
                                           const std::string& signature) {
    JNIEnv* env = GetEnv();
    if (!env || !clazz) {
        return nullptr;
    }

    std::string key = "static_" + name + "#" + signature;
    auto it = m_methodIDCache.find(key);
    if (it != m_methodIDCache.end()) {
        return it->second;
    }

    jmethodID mid = env->GetStaticMethodID(clazz, name.c_str(), signature.c_str());
    if (mid) {
        m_methodIDCache[key] = mid;
    }

    return mid;
}

jfieldID JniEnvManager::GetFieldID(jclass clazz, const std::string& name,
                                   const std::string& signature) {
    JNIEnv* env = GetEnv();
    if (!env || !clazz) {
        return nullptr;
    }

    std::string key = name + "#" + signature;
    auto it = m_fieldIDCache.find(key);
    if (it != m_fieldIDCache.end()) {
        return it->second;
    }

    jfieldID fid = env->GetFieldID(clazz, name.c_str(), signature.c_str());
    if (fid) {
        m_fieldIDCache[key] = fid;
    }

    return fid;
}

jobject JniEnvManager::NewGlobalRef(jobject obj) {
    JNIEnv* env = GetEnv();
    if (!env || !obj) {
        return nullptr;
    }
    return env->NewGlobalRef(obj);
}

void JniEnvManager::DeleteGlobalRef(jobject obj) {
    JNIEnv* env = GetEnv();
    if (env && obj) {
        env->DeleteGlobalRef(obj);
    }
}

}
}
