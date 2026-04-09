#ifndef JNI_ENV_MANAGER_H
#define JNI_ENV_MANAGER_H

#include <jni.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <thread>

namespace ogc {
namespace jni {

class JniEnvManager {
public:
    static JniEnvManager* GetInstance();

    void Initialize(JavaVM* vm);
    void Shutdown();

    JNIEnv* GetEnv();
    JNIEnv* AttachCurrentThread();
    void DetachCurrentThread();

    bool IsInitialized() const { return m_initialized; }
    JavaVM* GetJavaVM() const { return m_javaVM; }

    jclass FindClass(const std::string& className);
    jclass GetGlobalClassRef(const std::string& className);
    void ReleaseGlobalClassRef(jclass clazz);

    jmethodID GetMethodID(jclass clazz, const std::string& name,
                          const std::string& signature);
    jmethodID GetStaticMethodID(jclass clazz, const std::string& name,
                                const std::string& signature);
    jfieldID GetFieldID(jclass clazz, const std::string& name,
                        const std::string& signature);

    jobject NewGlobalRef(jobject obj);
    void DeleteGlobalRef(jobject obj);

private:
    JniEnvManager();
    ~JniEnvManager();

    JavaVM* m_javaVM;
    std::mutex m_mutex;
    bool m_initialized;

    std::unordered_map<std::string, jclass> m_globalClassRefs;
    std::unordered_map<std::string, jmethodID> m_methodIDCache;
    std::unordered_map<std::string, jfieldID> m_fieldIDCache;
    std::unordered_map<std::thread::id, bool> m_attachedThreads;
};

}
}

#endif
