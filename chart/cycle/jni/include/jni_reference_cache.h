#ifndef JNI_REFERENCE_CACHE_H
#define JNI_REFERENCE_CACHE_H

#include <jni.h>
#include <string>
#include <unordered_map>
#include <mutex>

namespace ogc {
namespace jni {

class JniReferenceCache {
public:
    static JniReferenceCache* GetInstance();

    void Initialize(JNIEnv* env);
    void Clear(JNIEnv* env);

    jclass GetClass(const std::string& name);
    jmethodID GetConstructor(jclass clazz, const std::string& signature);
    jmethodID GetMethod(jclass clazz, const std::string& name, const std::string& signature);
    jmethodID GetStaticMethod(jclass clazz, const std::string& name, const std::string& signature);
    jfieldID GetField(jclass clazz, const std::string& name, const std::string& signature);

private:
    JniReferenceCache();
    ~JniReferenceCache();

    std::string MakeKey(jclass clazz, const std::string& name, const std::string& sig);

    std::mutex m_mutex;
    JNIEnv* m_env;
    std::unordered_map<std::string, jclass> m_classCache;
    std::unordered_map<std::string, jmethodID> m_methodCache;
    std::unordered_map<std::string, jfieldID> m_fieldCache;
};

}
}

#endif
