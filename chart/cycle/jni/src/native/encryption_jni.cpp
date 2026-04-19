#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_DataEncryption_nativeCreate
  (JNIEnv* env, jclass clazz, jstring key) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        const char* keyStr = env->GetStringUTFChars(key, nullptr);
        if (!keyStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_data_encryption_t* encryption = ogc_data_encryption_create(keyStr);
        env->ReleaseStringUTFChars(key, keyStr);
        return JniConverter::ToJLongPtr(encryption);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_DataEncryption_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_data_encryption_t* encryption =
            static_cast<ogc_data_encryption_t*>(JniConverter::FromJLongPtr(ptr));
        if (!encryption) { return; }
        ogc_data_encryption_destroy(encryption);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jbyteArray JNICALL
Java_cn_cycle_chart_api_cache_DataEncryption_nativeEncrypt
  (JNIEnv* env, jobject obj, jlong ptr, jbyteArray inputData) {
    try {
        ogc_data_encryption_t* encryption =
            static_cast<ogc_data_encryption_t*>(JniConverter::FromJLongPtr(ptr));
        if (!encryption) {
            JniException::ThrowNullPointerException(env, "DataEncryption is null");
            return nullptr;
        }
        jsize inputLen = env->GetArrayLength(inputData);
        jbyte* inputBytes = env->GetByteArrayElements(inputData, nullptr);
        if (!inputBytes) { return nullptr; }
        size_t outputSize = 0;
        std::vector<char> outputBuf(inputLen * 2 + 64);
        int result = ogc_data_encryption_encrypt(
            encryption, inputBytes, static_cast<size_t>(inputLen),
            outputBuf.data(), &outputSize);
        env->ReleaseByteArrayElements(inputData, inputBytes, JNI_ABORT);
        if (result != 0 || outputSize == 0) { return nullptr; }
        jbyteArray outputArray = env->NewByteArray(static_cast<jsize>(outputSize));
        if (!outputArray) { return nullptr; }
        env->SetByteArrayRegion(outputArray, 0, static_cast<jsize>(outputSize),
            reinterpret_cast<const jbyte*>(outputBuf.data()));
        return outputArray;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jbyteArray JNICALL
Java_cn_cycle_chart_api_cache_DataEncryption_nativeDecrypt
  (JNIEnv* env, jobject obj, jlong ptr, jbyteArray inputData) {
    try {
        ogc_data_encryption_t* encryption =
            static_cast<ogc_data_encryption_t*>(JniConverter::FromJLongPtr(ptr));
        if (!encryption) {
            JniException::ThrowNullPointerException(env, "DataEncryption is null");
            return nullptr;
        }
        jsize inputLen = env->GetArrayLength(inputData);
        jbyte* inputBytes = env->GetByteArrayElements(inputData, nullptr);
        if (!inputBytes) { return nullptr; }
        size_t outputSize = 0;
        std::vector<char> outputBuf(inputLen + 64);
        int result = ogc_data_encryption_decrypt(
            encryption, inputBytes, static_cast<size_t>(inputLen),
            outputBuf.data(), &outputSize);
        env->ReleaseByteArrayElements(inputData, inputBytes, JNI_ABORT);
        if (result != 0 || outputSize == 0) { return nullptr; }
        jbyteArray outputArray = env->NewByteArray(static_cast<jsize>(outputSize));
        if (!outputArray) { return nullptr; }
        env->SetByteArrayRegion(outputArray, 0, static_cast<jsize>(outputSize),
            reinterpret_cast<const jbyte*>(outputBuf.data()));
        return outputArray;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

}
