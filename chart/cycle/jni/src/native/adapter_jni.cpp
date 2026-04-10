#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_adapter_ImageDevice_nativeCreate
  (JNIEnv* env, jclass clazz, jint width, jint height) {
    try {
        ogc_image_device_t* device = ogc_image_device_create(
            static_cast<size_t>(width), static_cast<size_t>(height));
        return JniConverter::ToJLongPtr(device);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_adapter_ImageDevice_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_image_device_t* device =
            static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
        if (device) {
            ogc_image_device_destroy(device);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_adapter_ImageDevice_nativeResize
  (JNIEnv* env, jclass clazz, jlong ptr, jint width, jint height) {
    try {
        ogc_image_device_t* device =
            static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
        if (device) {
            ogc_image_device_resize(device, 
                static_cast<size_t>(width), static_cast<size_t>(height));
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_adapter_ImageDevice_nativeClear
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_image_device_t* device =
            static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
        if (device) {
            ogc_image_device_clear(device);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jbyteArray JNICALL
Java_cn_cycle_chart_api_adapter_ImageDevice_nativeGetPixels
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    if (!device) {
        return nullptr;
    }

    size_t size = 0;
    const unsigned char* pixels = ogc_image_device_get_pixels(device, &size);
    if (!pixels || size == 0) {
        return nullptr;
    }

    jbyteArray result = env->NewByteArray(static_cast<jsize>(size));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(size), 
            reinterpret_cast<const jbyte*>(pixels));
    }
    return result;
}

}
