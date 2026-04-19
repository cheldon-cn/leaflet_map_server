#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeCreate
  (JNIEnv* env, jclass clazz, jstring allowedPaths) {
    const char* cpaths = env->GetStringUTFChars(allowedPaths, nullptr);
    if (!cpaths) { return 0; }
    ogc_secure_library_loader_t* loader = ogc_secure_library_loader_create(cpaths);
    env->ReleaseStringUTFChars(allowedPaths, cpaths);
    return JniConverter::ToJLongPtr(loader);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_secure_library_loader_t* loader =
        static_cast<ogc_secure_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
    if (loader) {
        ogc_secure_library_loader_destroy(loader);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeLoad
  (JNIEnv* env, jobject obj, jlong ptr, jstring libraryPath) {
    ogc_secure_library_loader_t* loader =
        static_cast<ogc_secure_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
    if (!loader || !libraryPath) { return 0; }
    const char* cpath = env->GetStringUTFChars(libraryPath, nullptr);
    if (!cpath) { return 0; }
    ogc_library_handle_t handle = ogc_secure_library_loader_load(loader, cpath);
    env->ReleaseStringUTFChars(libraryPath, cpath);
    return JniConverter::ToJLongPtr(handle);
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeVerifySignature
  (JNIEnv* env, jobject obj, jlong ptr, jlong handlePtr, jstring publicKey) {
    ogc_secure_library_loader_t* loader =
        static_cast<ogc_secure_library_loader_t*>(JniConverter::FromJLongPtr(ptr));
    if (!loader || !publicKey) { return -1; }
    ogc_library_handle_t handle = JniConverter::FromJLongPtr(handlePtr);
    const char* ckey = env->GetStringUTFChars(publicKey, nullptr);
    if (!ckey) { return -1; }
    int ret = ogc_secure_library_loader_verify_signature(loader, handle, ckey);
    env->ReleaseStringUTFChars(publicKey, ckey);
    return ret;
}

}
