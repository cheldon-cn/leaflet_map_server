#ifndef OGC_JNI_BRIDGE_JNI_ENV_H
#define OGC_JNI_BRIDGE_JNI_ENV_H

#include "jni_bridge/export.h"

#ifdef __ANDROID__

#include <jni.h>
#include <string>
#include <functional>
#include <mutex>
#include <memory>

namespace ogc {
namespace jni {

class OGC_JNI_BRIDGE_API JniEnvManager {
public:
    static JniEnvManager* GetInstance();
    
    void Initialize(JavaVM* vm);
    void Shutdown();
    
    JNIEnv* GetEnv();
    JNIEnv* AttachCurrentThread();
    void DetachCurrentThread();
    
    bool IsInitialized() const;
    JavaVM* GetJavaVM() const;
    
    void RegisterNatives(const std::string& className,
                        const JNINativeMethod* methods,
                        int numMethods);
    
    void UnregisterNatives(const std::string& className);
    
    jclass FindClass(const std::string& className);
    jclass GetGlobalClassRef(const std::string& className);
    void ReleaseGlobalClassRef(jclass clazz);
    
    jmethodID GetMethodID(jclass clazz, const std::string& name, const std::string& signature);
    jmethodID GetStaticMethodID(jclass clazz, const std::string& name, const std::string& signature);
    jfieldID GetFieldID(jclass clazz, const std::string& name, const std::string& signature);
    jfieldID GetStaticFieldID(jclass clazz, const std::string& name, const std::string& signature);
    
    jobject NewGlobalRef(jobject obj);
    void DeleteGlobalRef(jobject obj);
    jobject NewLocalRef(jobject obj);
    void DeleteLocalRef(jobject obj);
    
    template<typename Func>
    void WithEnv(Func&& func) {
        JNIEnv* env = AttachCurrentThread();
        if (env) {
            func(env);
        }
    }

private:
    JniEnvManager();
    ~JniEnvManager();
    
    JniEnvManager(const JniEnvManager&) = delete;
    JniEnvManager& operator=(const JniEnvManager&) = delete;
    
    static JniEnvManager* s_instance;
    
    JavaVM* m_javaVM;
    std::mutex m_mutex;
    bool m_initialized;
};

class OGC_JNI_BRIDGE_API JniEnvScope {
public:
    JniEnvScope();
    ~JniEnvScope();
    
    JNIEnv* GetEnv() const { return m_env; }
    operator JNIEnv*() const { return m_env; }
    JNIEnv* operator->() const { return m_env; }
    
    bool IsValid() const { return m_env != nullptr; }

private:
    JNIEnv* m_env;
    bool m_attached;
};

}  
}  

#endif

#endif
