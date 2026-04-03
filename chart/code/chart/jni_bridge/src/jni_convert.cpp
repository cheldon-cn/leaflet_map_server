#include "jni_bridge/jni_convert.h"

#ifdef __ANDROID__

#include <android/log.h>
#include <cassert>
#include <cstdarg>

#define LOG_TAG "JniConverter"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace jni {

std::string JniConverter::ToString(JNIEnv* env, jstring str) {
    if (!env || !str) {
        return "";
    }
    
    const char* chars = env->GetStringUTFChars(str, nullptr);
    if (!chars) {
        return "";
    }
    
    std::string result(chars);
    env->ReleaseStringUTFChars(str, chars);
    
    return result;
}

jstring JniConverter::ToJString(JNIEnv* env, const std::string& str) {
    if (!env) {
        return nullptr;
    }
    
    return env->NewStringUTF(str.c_str());
}

std::vector<std::string> JniConverter::ToStringVector(JNIEnv* env, jobjectArray array) {
    std::vector<std::string> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.reserve(length);
    
    for (jsize i = 0; i < length; ++i) {
        jstring element = static_cast<jstring>(env->GetObjectArrayElement(array, i));
        result.push_back(ToString(env, element));
        env->DeleteLocalRef(element);
    }
    
    return result;
}

jobjectArray JniConverter::ToJStringArray(JNIEnv* env, const std::vector<std::string>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jclass stringClass = env->FindClass("java/lang/String");
    if (!stringClass) {
        return nullptr;
    }
    
    jobjectArray array = env->NewObjectArray(
        static_cast<jsize>(vec.size()), stringClass, nullptr);
    
    for (size_t i = 0; i < vec.size(); ++i) {
        jstring str = ToJString(env, vec[i]);
        env->SetObjectArrayElement(array, static_cast<jsize>(i), str);
        env->DeleteLocalRef(str);
    }
    
    env->DeleteLocalRef(stringClass);
    return array;
}

std::vector<int> JniConverter::ToIntVector(JNIEnv* env, jintArray array) {
    std::vector<int> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.resize(length);
    
    jint* elements = env->GetIntArrayElements(array, nullptr);
    if (elements) {
        for (jsize i = 0; i < length; ++i) {
            result[i] = elements[i];
        }
        env->ReleaseIntArrayElements(array, elements, JNI_ABORT);
    }
    
    return result;
}

jintArray JniConverter::ToJIntArray(JNIEnv* env, const std::vector<int>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jintArray array = env->NewIntArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetIntArrayRegion(array, 0, static_cast<jsize>(vec.size()), vec.data());
    
    return array;
}

std::vector<long> JniConverter::ToLongVector(JNIEnv* env, jlongArray array) {
    std::vector<long> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.resize(length);
    
    jlong* elements = env->GetLongArrayElements(array, nullptr);
    if (elements) {
        for (jsize i = 0; i < length; ++i) {
            result[i] = static_cast<long>(elements[i]);
        }
        env->ReleaseLongArrayElements(array, elements, JNI_ABORT);
    }
    
    return result;
}

jlongArray JniConverter::ToJLongArray(JNIEnv* env, const std::vector<long>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jlongArray array = env->NewLongArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    std::vector<jlong> jvec(vec.begin(), vec.end());
    env->SetLongArrayRegion(array, 0, static_cast<jsize>(vec.size()), jvec.data());
    
    return array;
}

std::vector<float> JniConverter::ToFloatVector(JNIEnv* env, jfloatArray array) {
    std::vector<float> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.resize(length);
    
    jfloat* elements = env->GetFloatArrayElements(array, nullptr);
    if (elements) {
        for (jsize i = 0; i < length; ++i) {
            result[i] = elements[i];
        }
        env->ReleaseFloatArrayElements(array, elements, JNI_ABORT);
    }
    
    return result;
}

jfloatArray JniConverter::ToJFloatArray(JNIEnv* env, const std::vector<float>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jfloatArray array = env->NewFloatArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetFloatArrayRegion(array, 0, static_cast<jsize>(vec.size()), vec.data());
    
    return array;
}

std::vector<double> JniConverter::ToDoubleVector(JNIEnv* env, jdoubleArray array) {
    std::vector<double> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.resize(length);
    
    jdouble* elements = env->GetDoubleArrayElements(array, nullptr);
    if (elements) {
        for (jsize i = 0; i < length; ++i) {
            result[i] = elements[i];
        }
        env->ReleaseDoubleArrayElements(array, elements, JNI_ABORT);
    }
    
    return result;
}

jdoubleArray JniConverter::ToJDoubleArray(JNIEnv* env, const std::vector<double>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jdoubleArray array = env->NewDoubleArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetDoubleArrayRegion(array, 0, static_cast<jsize>(vec.size()), vec.data());
    
    return array;
}

std::vector<uint8_t> JniConverter::ToByteVector(JNIEnv* env, jbyteArray array) {
    std::vector<uint8_t> result;
    
    if (!env || !array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    result.resize(length);
    
    jbyte* elements = env->GetByteArrayElements(array, nullptr);
    if (elements) {
        for (jsize i = 0; i < length; ++i) {
            result[i] = static_cast<uint8_t>(elements[i]);
        }
        env->ReleaseByteArrayElements(array, elements, JNI_ABORT);
    }
    
    return result;
}

jbyteArray JniConverter::ToJByteArray(JNIEnv* env, const std::vector<uint8_t>& vec) {
    if (!env) {
        return nullptr;
    }
    
    jbyteArray array = env->NewByteArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetByteArrayRegion(array, 0, static_cast<jsize>(vec.size()),
                            reinterpret_cast<const jbyte*>(vec.data()));
    
    return array;
}

bool JniConverter::ToBool(JNIEnv* env, jboolean value) {
    return value == JNI_TRUE;
}

jboolean JniConverter::ToJBoolean(bool value) {
    return value ? JNI_TRUE : JNI_FALSE;
}

int JniConverter::ToInt(JNIEnv* env, jint value) {
    return static_cast<int>(value);
}

jint JniConverter::ToJInt(int value) {
    return static_cast<jint>(value);
}

long JniConverter::ToLong(JNIEnv* env, jlong value) {
    return static_cast<long>(value);
}

jlong JniConverter::ToJLong(long value) {
    return static_cast<jlong>(value);
}

float JniConverter::ToFloat(JNIEnv* env, jfloat value) {
    return static_cast<float>(value);
}

jfloat JniConverter::ToJFloat(float value) {
    return static_cast<jfloat>(value);
}

double JniConverter::ToDouble(JNIEnv* env, jdouble value) {
    return static_cast<double>(value);
}

jdouble JniConverter::ToJDouble(double value) {
    return static_cast<jdouble>(value);
}

jobject JniConverter::ToJObject(JNIEnv* env, void* ptr) {
    return static_cast<jobject>(ptr);
}

void* JniConverter::FromJObject(JNIEnv* env, jobject obj) {
    return static_cast<void*>(obj);
}

jlong JniConverter::ToJLongPtr(void* ptr) {
    return reinterpret_cast<jlong>(ptr);
}

void* JniConverter::FromJLongPtr(jlong ptr) {
    return reinterpret_cast<void*>(ptr);
}

std::map<std::string, std::string> JniConverter::ToStringMap(JNIEnv* env, jobject map) {
    std::map<std::string, std::string> result;
    
    if (!env || !map) {
        return result;
    }
    
    jclass mapClass = env->GetObjectClass(map);
    jmethodID entrySetMethod = env->GetMethodID(mapClass, "entrySet", "()Ljava/util/Set;");
    jobject entrySet = env->CallObjectMethod(map, entrySetMethod);
    
    jclass setClass = env->GetObjectClass(entrySet);
    jmethodID iteratorMethod = env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
    jobject iterator = env->CallObjectMethod(entrySet, iteratorMethod);
    
    jclass iteratorClass = env->GetObjectClass(iterator);
    jmethodID hasNextMethod = env->GetMethodID(iteratorClass, "hasNext", "()Z");
    jmethodID nextMethod = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
    
    jclass entryClass = env->FindClass("java/util/Map$Entry");
    jmethodID getKeyMethod = env->GetMethodID(entryClass, "getKey", "()Ljava/lang/Object;");
    jmethodID getValueMethod = env->GetMethodID(entryClass, "getValue", "()Ljava/lang/Object;");
    
    while (env->CallBooleanMethod(iterator, hasNextMethod)) {
        jobject entry = env->CallObjectMethod(iterator, nextMethod);
        jstring key = static_cast<jstring>(env->CallObjectMethod(entry, getKeyMethod));
        jstring value = static_cast<jstring>(env->CallObjectMethod(entry, getValueMethod));
        
        result[ToString(env, key)] = ToString(env, value);
        
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
        env->DeleteLocalRef(entry);
    }
    
    env->DeleteLocalRef(entryClass);
    env->DeleteLocalRef(iteratorClass);
    env->DeleteLocalRef(iterator);
    env->DeleteLocalRef(setClass);
    env->DeleteLocalRef(entrySet);
    env->DeleteLocalRef(mapClass);
    
    return result;
}

jobject JniConverter::ToJMap(JNIEnv* env, const std::map<std::string, std::string>& map) {
    if (!env) {
        return nullptr;
    }
    
    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID constructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jobject hashMap = env->NewObject(hashMapClass, constructor);
    
    jmethodID putMethod = env->GetMethodID(hashMapClass, "put",
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    
    for (const auto& pair : map) {
        jstring key = ToJString(env, pair.first);
        jstring value = ToJString(env, pair.second);
        env->CallObjectMethod(hashMap, putMethod, key, value);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }
    
    env->DeleteLocalRef(hashMapClass);
    return hashMap;
}

jobject JniConverter::CreateJObject(JNIEnv* env, const std::string& className) {
    if (!env) {
        return nullptr;
    }
    
    jclass clazz = env->FindClass(className.c_str());
    if (!clazz) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject obj = env->NewObject(clazz, constructor);
    
    env->DeleteLocalRef(clazz);
    return obj;
}

jobject JniConverter::CreateJObject(JNIEnv* env, jclass clazz, jmethodID constructor, ...) {
    if (!env || !clazz || !constructor) {
        return nullptr;
    }
    
    va_list args;
    va_start(args, constructor);
    jobject obj = env->NewObjectV(clazz, constructor, args);
    va_end(args);
    
    return obj;
}

jobject JniConverter::CreateArrayList(JNIEnv* env) {
    if (!env) {
        return nullptr;
    }
    
    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jmethodID constructor = env->GetMethodID(arrayListClass, "<init>", "()V");
    jobject arrayList = env->NewObject(arrayListClass, constructor);
    
    env->DeleteLocalRef(arrayListClass);
    return arrayList;
}

void JniConverter::ArrayListAdd(JNIEnv* env, jobject list, jobject element) {
    if (!env || !list || !element) {
        return;
    }
    
    jclass listClass = env->GetObjectClass(list);
    jmethodID addMethod = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");
    env->CallBooleanMethod(list, addMethod, element);
    env->DeleteLocalRef(listClass);
}

int JniConverter::ArrayListSize(JNIEnv* env, jobject list) {
    if (!env || !list) {
        return 0;
    }
    
    jclass listClass = env->GetObjectClass(list);
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jint size = env->CallIntMethod(list, sizeMethod);
    env->DeleteLocalRef(listClass);
    
    return static_cast<int>(size);
}

jobject JniConverter::ArrayListGet(JNIEnv* env, jobject list, int index) {
    if (!env || !list) {
        return nullptr;
    }
    
    jclass listClass = env->GetObjectClass(list);
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    jobject element = env->CallObjectMethod(list, getMethod, static_cast<jint>(index));
    env->DeleteLocalRef(listClass);
    
    return element;
}

jobject JniConverter::CreateHashMap(JNIEnv* env) {
    if (!env) {
        return nullptr;
    }
    
    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID constructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jobject hashMap = env->NewObject(hashMapClass, constructor);
    
    env->DeleteLocalRef(hashMapClass);
    return hashMap;
}

void JniConverter::HashMapPut(JNIEnv* env, jobject map, jobject key, jobject value) {
    if (!env || !map || !key || !value) {
        return;
    }
    
    jclass mapClass = env->GetObjectClass(map);
    jmethodID putMethod = env->GetMethodID(mapClass, "put",
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    env->CallObjectMethod(map, putMethod, key, value);
    env->DeleteLocalRef(mapClass);
}

int JniConverter::HashMapSize(JNIEnv* env, jobject map) {
    if (!env || !map) {
        return 0;
    }
    
    jclass mapClass = env->GetObjectClass(map);
    jmethodID sizeMethod = env->GetMethodID(mapClass, "size", "()I");
    jint size = env->CallIntMethod(map, sizeMethod);
    env->DeleteLocalRef(mapClass);
    
    return static_cast<int>(size);
}

jclass JniConverter::GetObjectClass(JNIEnv* env, jobject obj) {
    if (!env || !obj) {
        return nullptr;
    }
    
    return env->GetObjectClass(obj);
}

bool JniConverter::IsInstanceOf(JNIEnv* env, jobject obj, jclass clazz) {
    if (!env || !obj || !clazz) {
        return false;
    }
    
    return env->IsInstanceOf(obj, clazz) == JNI_TRUE;
}

jobject JniConverter::CallObjectMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return nullptr;
    }
    
    va_list args;
    va_start(args, methodID);
    jobject result = env->CallObjectMethodV(obj, methodID, args);
    va_end(args);
    
    return result;
}

void JniConverter::CallVoidMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return;
    }
    
    va_list args;
    va_start(args, methodID);
    env->CallVoidMethodV(obj, methodID, args);
    va_end(args);
}

int JniConverter::CallIntMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return 0;
    }
    
    va_list args;
    va_start(args, methodID);
    jint result = env->CallIntMethodV(obj, methodID, args);
    va_end(args);
    
    return static_cast<int>(result);
}

long JniConverter::CallLongMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return 0;
    }
    
    va_list args;
    va_start(args, methodID);
    jlong result = env->CallLongMethodV(obj, methodID, args);
    va_end(args);
    
    return static_cast<long>(result);
}

float JniConverter::CallFloatMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return 0.0f;
    }
    
    va_list args;
    va_start(args, methodID);
    jfloat result = env->CallFloatMethodV(obj, methodID, args);
    va_end(args);
    
    return static_cast<float>(result);
}

double JniConverter::CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return 0.0;
    }
    
    va_list args;
    va_start(args, methodID);
    jdouble result = env->CallDoubleMethodV(obj, methodID, args);
    va_end(args);
    
    return static_cast<double>(result);
}

bool JniConverter::CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    if (!env || !obj || !methodID) {
        return false;
    }
    
    va_list args;
    va_start(args, methodID);
    jboolean result = env->CallBooleanMethodV(obj, methodID, args);
    va_end(args);
    
    return result == JNI_TRUE;
}

jobject JniConverter::CallStaticObjectMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return nullptr;
    }
    
    va_list args;
    va_start(args, methodID);
    jobject result = env->CallStaticObjectMethodV(clazz, methodID, args);
    va_end(args);
    
    return result;
}

void JniConverter::CallStaticVoidMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return;
    }
    
    va_list args;
    va_start(args, methodID);
    env->CallStaticVoidMethodV(clazz, methodID, args);
    va_end(args);
}

int JniConverter::CallStaticIntMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return 0;
    }
    
    va_list args;
    va_start(args, methodID);
    jint result = env->CallStaticIntMethodV(clazz, methodID, args);
    va_end(args);
    
    return static_cast<int>(result);
}

long JniConverter::CallStaticLongMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return 0;
    }
    
    va_list args;
    va_start(args, methodID);
    jlong result = env->CallStaticLongMethodV(clazz, methodID, args);
    va_end(args);
    
    return static_cast<long>(result);
}

float JniConverter::CallStaticFloatMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return 0.0f;
    }
    
    va_list args;
    va_start(args, methodID);
    jfloat result = env->CallStaticFloatMethodV(clazz, methodID, args);
    va_end(args);
    
    return static_cast<float>(result);
}

double JniConverter::CallStaticDoubleMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return 0.0;
    }
    
    va_list args;
    va_start(args, methodID);
    jdouble result = env->CallStaticDoubleMethodV(clazz, methodID, args);
    va_end(args);
    
    return static_cast<double>(result);
}

bool JniConverter::CallStaticBooleanMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    if (!env || !clazz || !methodID) {
        return false;
    }
    
    va_list args;
    va_start(args, methodID);
    jboolean result = env->CallStaticBooleanMethodV(clazz, methodID, args);
    va_end(args);
    
    return result == JNI_TRUE;
}

}  
}  

#endif
