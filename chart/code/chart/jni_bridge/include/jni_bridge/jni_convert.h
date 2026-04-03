#ifndef OGC_JNI_BRIDGE_JNI_CONVERT_H
#define OGC_JNI_BRIDGE_JNI_CONVERT_H

#include "jni_bridge/export.h"

#ifdef __ANDROID__

#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ogc {
namespace jni {

class OGC_JNI_BRIDGE_API JniConverter {
public:
    static std::string ToString(JNIEnv* env, jstring str);
    static jstring ToJString(JNIEnv* env, const std::string& str);
    
    static std::vector<std::string> ToStringVector(JNIEnv* env, jobjectArray array);
    static jobjectArray ToJStringArray(JNIEnv* env, const std::vector<std::string>& vec);
    
    static std::vector<int> ToIntVector(JNIEnv* env, jintArray array);
    static jintArray ToJIntArray(JNIEnv* env, const std::vector<int>& vec);
    
    static std::vector<long> ToLongVector(JNIEnv* env, jlongArray array);
    static jlongArray ToJLongArray(JNIEnv* env, const std::vector<long>& vec);
    
    static std::vector<float> ToFloatVector(JNIEnv* env, jfloatArray array);
    static jfloatArray ToJFloatArray(JNIEnv* env, const std::vector<float>& vec);
    
    static std::vector<double> ToDoubleVector(JNIEnv* env, jdoubleArray array);
    static jdoubleArray ToJDoubleArray(JNIEnv* env, const std::vector<double>& vec);
    
    static std::vector<uint8_t> ToByteVector(JNIEnv* env, jbyteArray array);
    static jbyteArray ToJByteArray(JNIEnv* env, const std::vector<uint8_t>& vec);
    
    static bool ToBool(JNIEnv* env, jboolean value);
    static jboolean ToJBoolean(bool value);
    
    static int ToInt(JNIEnv* env, jint value);
    static jint ToJInt(int value);
    
    static long ToLong(JNIEnv* env, jlong value);
    static jlong ToJLong(long value);
    
    static float ToFloat(JNIEnv* env, jfloat value);
    static jfloat ToJFloat(float value);
    
    static double ToDouble(JNIEnv* env, jdouble value);
    static jdouble ToJDouble(double value);
    
    static jobject ToJObject(JNIEnv* env, void* ptr);
    static void* FromJObject(JNIEnv* env, jobject obj);
    
    static jlong ToJLongPtr(void* ptr);
    static void* FromJLongPtr(jlong ptr);
    
    static std::map<std::string, std::string> ToStringMap(JNIEnv* env, jobject map);
    static jobject ToJMap(JNIEnv* env, const std::map<std::string, std::string>& map);
    
    static jobject CreateJObject(JNIEnv* env, const std::string& className);
    static jobject CreateJObject(JNIEnv* env, jclass clazz, jmethodID constructor, ...);
    
    static jobject CreateArrayList(JNIEnv* env);
    static void ArrayListAdd(JNIEnv* env, jobject list, jobject element);
    static int ArrayListSize(JNIEnv* env, jobject list);
    static jobject ArrayListGet(JNIEnv* env, jobject list, int index);
    
    static jobject CreateHashMap(JNIEnv* env);
    static void HashMapPut(JNIEnv* env, jobject map, jobject key, jobject value);
    static int HashMapSize(JNIEnv* env, jobject map);
    
    static jclass GetObjectClass(JNIEnv* env, jobject obj);
    static bool IsInstanceOf(JNIEnv* env, jobject obj, jclass clazz);
    
    static jobject CallObjectMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static void CallVoidMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static int CallIntMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static long CallLongMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static float CallFloatMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static double CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static bool CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    
    static jobject CallStaticObjectMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static void CallStaticVoidMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static int CallStaticIntMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static long CallStaticLongMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static float CallStaticFloatMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static double CallStaticDoubleMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
    static bool CallStaticBooleanMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...);
};

template<typename T>
class JniLocalRef {
public:
    JniLocalRef(JNIEnv* env, T ref) : m_env(env), m_ref(ref) {}
    ~JniLocalRef() { if (m_ref) m_env->DeleteLocalRef(m_ref); }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    T operator->() const { return m_ref; }
    bool IsValid() const { return m_ref != nullptr; }
    
private:
    JNIEnv* m_env;
    T m_ref;
    
    JniLocalRef(const JniLocalRef&) = delete;
    JniLocalRef& operator=(const JniLocalRef&) = delete;
};

template<typename T>
class JniGlobalRef {
public:
    JniGlobalRef(JNIEnv* env, T ref) : m_env(env), m_ref(nullptr) {
        if (ref) {
            m_ref = static_cast<T>(env->NewGlobalRef(ref));
        }
    }
    ~JniGlobalRef() { if (m_ref) m_env->DeleteGlobalRef(m_ref); }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    T operator->() const { return m_ref; }
    bool IsValid() const { return m_ref != nullptr; }
    
private:
    JNIEnv* m_env;
    T m_ref;
    
    JniGlobalRef(const JniGlobalRef&) = delete;
    JniGlobalRef& operator=(const JniGlobalRef&) = delete;
};

}  
}  

#endif

#endif
