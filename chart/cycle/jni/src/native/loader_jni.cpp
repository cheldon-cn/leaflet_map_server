#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_library_loader_t* loader = ogc_library_loader_create();
        return JniConverter::ToJLongPtr(loader);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_library_loader_t* loader =
            static_cast<ogc_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
        if (!loader) { return; }
        ogc_library_loader_destroy(loader);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeLoad
  (JNIEnv* env, jobject obj, jlong ptr, jstring libraryPath) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        ogc_library_loader_t* loader =
            static_cast<ogc_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
        if (!loader) {
            JniException::ThrowNullPointerException(env, "LibraryLoader is null");
            return 0;
        }
        const char* pathStr = env->GetStringUTFChars(libraryPath, nullptr);
        if (!pathStr) { return 0; }
        ogc_library_handle_t handle = ogc_library_loader_load(loader, pathStr);
        env->ReleaseStringUTFChars(libraryPath, pathStr);
        return JniConverter::ToJLongPtr(handle);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeUnload
  (JNIEnv* env, jobject obj, jlong ptr, jlong handlePtr) {
    try {
        ogc_library_loader_t* loader =
            static_cast<ogc_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
        if (!loader) { return; }
        ogc_library_handle_t handle = JniConverter::FromJLongPtr(handlePtr);
        ogc_library_loader_unload(loader, handle);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeGetSymbol
  (JNIEnv* env, jobject obj, jlong ptr, jlong handlePtr, jstring symbolName) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        ogc_library_loader_t* loader =
            static_cast<ogc_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
        if (!loader) { return 0; }
        const char* nameStr = env->GetStringUTFChars(symbolName, nullptr);
        if (!nameStr) { return 0; }
        ogc_library_handle_t handle = JniConverter::FromJLongPtr(handlePtr);
        void* symbol = ogc_library_loader_get_symbol(loader, handle, nameStr);
        env->ReleaseStringUTFChars(symbolName, nameStr);
        return JniConverter::ToJLongPtr(symbol);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_loader_LibraryLoader_nativeGetError
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_library_loader_t* loader =
            static_cast<ogc_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
        if (!loader) { return nullptr; }
        const char* error = ogc_library_loader_get_error(loader);
        if (!error) { return nullptr; }
        return env->NewStringUTF(error);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

}
