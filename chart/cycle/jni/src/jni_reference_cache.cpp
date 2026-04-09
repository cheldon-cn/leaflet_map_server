#include "jni_reference_cache.h"
#include <sstream>

namespace ogc {
namespace jni {

JniReferenceCache* JniReferenceCache::GetInstance() {
    static JniReferenceCache instance;
    return &instance;
}

JniReferenceCache::JniReferenceCache() : m_env(nullptr) {
}

JniReferenceCache::~JniReferenceCache() {
}

void JniReferenceCache::Initialize(JNIEnv* env) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_env = env;
}

void JniReferenceCache::Clear(JNIEnv* env) {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& pair : m_classCache) {
        if (env && pair.second) {
            env->DeleteGlobalRef(pair.second);
        }
    }
    m_classCache.clear();
    m_methodCache.clear();
    m_fieldCache.clear();
}

jclass JniReferenceCache::GetClass(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_classCache.find(name);
    if (it != m_classCache.end()) {
        return it->second;
    }

    if (!m_env) {
        return nullptr;
    }

    jclass localClass = m_env->FindClass(name.c_str());
    if (!localClass) {
        return nullptr;
    }

    jclass globalClass = static_cast<jclass>(m_env->NewGlobalRef(localClass));
    m_env->DeleteLocalRef(localClass);

    if (globalClass) {
        m_classCache[name] = globalClass;
    }

    return globalClass;
}

jmethodID JniReferenceCache::GetConstructor(jclass clazz, const std::string& signature) {
    return GetMethod(clazz, "<init>", signature);
}

jmethodID JniReferenceCache::GetMethod(jclass clazz, const std::string& name,
                                       const std::string& signature) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string key = MakeKey(clazz, name, signature);
    auto it = m_methodCache.find(key);
    if (it != m_methodCache.end()) {
        return it->second;
    }

    if (!m_env || !clazz) {
        return nullptr;
    }

    jmethodID mid = m_env->GetMethodID(clazz, name.c_str(), signature.c_str());
    if (mid) {
        m_methodCache[key] = mid;
    }

    return mid;
}

jmethodID JniReferenceCache::GetStaticMethod(jclass clazz, const std::string& name,
                                             const std::string& signature) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string key = "static_" + MakeKey(clazz, name, signature);
    auto it = m_methodCache.find(key);
    if (it != m_methodCache.end()) {
        return it->second;
    }

    if (!m_env || !clazz) {
        return nullptr;
    }

    jmethodID mid = m_env->GetStaticMethodID(clazz, name.c_str(), signature.c_str());
    if (mid) {
        m_methodCache[key] = mid;
    }

    return mid;
}

jfieldID JniReferenceCache::GetField(jclass clazz, const std::string& name,
                                     const std::string& signature) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string key = MakeKey(clazz, name, signature);
    auto it = m_fieldCache.find(key);
    if (it != m_fieldCache.end()) {
        return it->second;
    }

    if (!m_env || !clazz) {
        return nullptr;
    }

    jfieldID fid = m_env->GetFieldID(clazz, name.c_str(), signature.c_str());
    if (fid) {
        m_fieldCache[key] = fid;
    }

    return fid;
}

std::string JniReferenceCache::MakeKey(jclass clazz, const std::string& name,
                                       const std::string& sig) {
    std::ostringstream oss;
    oss << reinterpret_cast<intptr_t>(clazz) << ":" << name << "#" << sig;
    return oss.str();
}

}
}
