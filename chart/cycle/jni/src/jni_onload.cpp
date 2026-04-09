#include "jni_env_manager.h"
#include "jni_reference_cache.h"
#include <jni.h>

using namespace ogc::jni;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8) != JNI_OK) {
        return JNI_ERR;
    }

    JniEnvManager::GetInstance()->Initialize(vm);
    JniReferenceCache::GetInstance()->Initialize(env);

    return JNI_VERSION_1_8;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8) == JNI_OK) {
        JniReferenceCache::GetInstance()->Clear(env);
    }
    JniEnvManager::GetInstance()->Shutdown();
}

}
