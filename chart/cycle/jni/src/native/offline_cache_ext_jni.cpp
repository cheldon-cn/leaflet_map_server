#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== OfflineRegion ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeCreate
  (JNIEnv* env, jclass clazz, jlong storagePtr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY, jint minZoom, jint maxZoom) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(storagePtr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        ogc_envelope_t* bounds = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
        if (!bounds) {
            return 0;
        }
        ogc_offline_region_t* region = ogc_offline_storage_create_region(storage, bounds, minZoom, maxZoom);
        ogc_envelope_destroy(bounds);
        return JniConverter::ToJLongPtr(region);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_offline_region_t* region =
            static_cast<ogc_offline_region_t*>(JniConverter::FromJLongPtr(ptr));
        if (region) {
            ogc_offline_region_destroy(region);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeDownload
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_offline_region_t* region =
        static_cast<ogc_offline_region_t*>(JniConverter::FromJLongPtr(ptr));
    if (!region) {
        return JNI_FALSE;
    }
    return ogc_offline_region_download(region) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jfloat JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetProgress
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_offline_region_t* region =
        static_cast<ogc_offline_region_t*>(JniConverter::FromJLongPtr(ptr));
    return region ? ogc_offline_region_get_progress(region) : 0.0f;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetId
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_offline_region_t* region =
        static_cast<ogc_offline_region_t*>(JniConverter::FromJLongPtr(ptr));
    if (!region) {
        return nullptr;
    }
    const char* id = ogc_offline_region_get_id(region);
    return id ? env->NewStringUTF(id) : nullptr;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_offline_region_t* region =
        static_cast<ogc_offline_region_t*>(JniConverter::FromJLongPtr(ptr));
    if (!region) {
        return nullptr;
    }
    const char* name = ogc_offline_region_get_name(region);
    return name ? env->NewStringUTF(name) : nullptr;
}

/* ===== OfflineStorage New Methods ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegionById
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (!storage || !regionId) {
        return 0;
    }
    const char* cid = env->GetStringUTFChars(regionId, nullptr);
    if (!cid) {
        return 0;
    }
    ogc_offline_region_t* region = ogc_offline_storage_get_region_by_id(storage, cid);
    env->ReleaseStringUTFChars(regionId, cid);
    return JniConverter::ToJLongPtr(region);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveRegion
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (storage) {
        ogc_offline_storage_remove_region(storage, static_cast<size_t>(index));
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveRegionById
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (!storage || !regionId) {
        return JNI_FALSE;
    }
    const char* cid = env->GetStringUTFChars(regionId, nullptr);
    if (!cid) {
        return JNI_FALSE;
    }
    int result = ogc_offline_storage_remove_region_by_id(storage, cid);
    env->ReleaseStringUTFChars(regionId, cid);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeStoreChart
  (JNIEnv* env, jobject obj, jlong ptr, jstring chartId, jbyteArray data) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (!storage || !chartId || !data) {
        return JNI_FALSE;
    }
    const char* cid = env->GetStringUTFChars(chartId, nullptr);
    if (!cid) {
        return JNI_FALSE;
    }
    jsize len = env->GetArrayLength(data);
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    int result = -1;
    if (bytes) {
        result = ogc_offline_storage_store_chart(storage, cid, bytes, static_cast<size_t>(len));
        env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
    }
    env->ReleaseStringUTFChars(chartId, cid);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetChartPath
  (JNIEnv* env, jobject obj, jlong ptr, jstring chartId) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (!storage || !chartId) {
        return nullptr;
    }
    const char* cid = env->GetStringUTFChars(chartId, nullptr);
    if (!cid) {
        return nullptr;
    }
    const char* path = ogc_offline_storage_get_chart_path(storage, cid);
    env->ReleaseStringUTFChars(chartId, cid);
    return path ? env->NewStringUTF(path) : nullptr;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveChart
  (JNIEnv* env, jobject obj, jlong ptr, jstring chartId) {
    ogc_offline_storage_t* storage =
        static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
    if (!storage || !chartId) {
        return JNI_FALSE;
    }
    const char* cid = env->GetStringUTFChars(chartId, nullptr);
    if (!cid) {
        return JNI_FALSE;
    }
    int result = ogc_offline_storage_remove_chart(storage, cid);
    env->ReleaseStringUTFChars(chartId, cid);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

}
