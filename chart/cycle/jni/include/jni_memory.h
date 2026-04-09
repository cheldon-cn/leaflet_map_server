#ifndef JNI_MEMORY_H
#define JNI_MEMORY_H

#include <jni.h>

namespace ogc {
namespace jni {

template<typename T>
class JniLocalRef {
public:
    JniLocalRef(JNIEnv* env, T ref) : m_env(env), m_ref(ref) {}
    ~JniLocalRef() {
        if (m_ref) {
            m_env->DeleteLocalRef(m_ref);
        }
    }

    JniLocalRef(const JniLocalRef&) = delete;
    JniLocalRef& operator=(const JniLocalRef&) = delete;

    JniLocalRef(JniLocalRef&& other) noexcept
        : m_env(other.m_env), m_ref(other.m_ref) {
        other.m_ref = nullptr;
    }

    JniLocalRef& operator=(JniLocalRef&& other) noexcept {
        if (this != &other) {
            if (m_ref) {
                m_env->DeleteLocalRef(m_ref);
            }
            m_env = other.m_env;
            m_ref = other.m_ref;
            other.m_ref = nullptr;
        }
        return *this;
    }

    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    T operator->() const { return m_ref; }
    bool operator!() const { return m_ref == nullptr; }
    operator bool() const { return m_ref != nullptr; }

private:
    JNIEnv* m_env;
    T m_ref;
};

template<typename T>
class JniGlobalRef {
public:
    JniGlobalRef(JNIEnv* env, T ref) : m_env(env), m_ref(nullptr) {
        if (ref) {
            m_ref = static_cast<T>(env->NewGlobalRef(ref));
        }
    }

    ~JniGlobalRef() {
        if (m_ref) {
            m_env->DeleteGlobalRef(m_ref);
        }
    }

    JniGlobalRef(const JniGlobalRef&) = delete;
    JniGlobalRef& operator=(const JniGlobalRef&) = delete;

    JniGlobalRef(JniGlobalRef&& other) noexcept
        : m_env(other.m_env), m_ref(other.m_ref) {
        other.m_ref = nullptr;
    }

    JniGlobalRef& operator=(JniGlobalRef&& other) noexcept {
        if (this != &other) {
            if (m_ref) {
                m_env->DeleteGlobalRef(m_ref);
            }
            m_env = other.m_env;
            m_ref = other.m_ref;
            other.m_ref = nullptr;
        }
        return *this;
    }

    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    bool operator!() const { return m_ref == nullptr; }
    operator bool() const { return m_ref != nullptr; }

private:
    JNIEnv* m_env;
    T m_ref;
};

class JniLocalRefScope {
public:
    JniLocalRefScope(JNIEnv* env, jint capacity = 16)
        : m_env(env), m_success(false) {
        m_success = (env->PushLocalFrame(capacity) == 0);
    }

    ~JniLocalRefScope() {
        if (m_success) {
            m_env->PopLocalFrame(nullptr);
        }
    }

    bool Success() const { return m_success; }

private:
    JNIEnv* m_env;
    bool m_success;
};

}
}

#endif
