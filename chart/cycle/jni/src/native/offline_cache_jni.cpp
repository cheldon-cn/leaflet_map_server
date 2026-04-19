#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCreate
  (JNIEnv* env, jclass clazz, jstring storagePath) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        const char* pathStr = env->GetStringUTFChars(storagePath, nullptr);
        if (!pathStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_offline_storage_t* storage = ogc_offline_storage_create(pathStr);
        env->ReleaseStringUTFChars(storagePath, pathStr);
        return JniConverter::ToJLongPtr(storage);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) { return; }
        ogc_offline_storage_destroy(storage);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCreateRegion
  (JNIEnv* env, jobject obj, jlong ptr,
   jdouble minX, jdouble minY, jdouble maxX, jdouble maxY,
   jint minZoom, jint maxZoom) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        ogc_envelope_t* bounds = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
        if (!bounds) {
            JniException::ThrowRuntimeException(env, "Failed to create envelope");
            return 0;
        }
        ogc_offline_region_t* region = ogc_offline_storage_create_region(
            storage, bounds, static_cast<int>(minZoom), static_cast<int>(maxZoom));
        ogc_envelope_destroy(bounds);
        return JniConverter::ToJLongPtr(region);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegionCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        return static_cast<jlong>(ogc_offline_storage_get_region_count(storage));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegion
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        ogc_offline_region_t* region = ogc_offline_storage_get_region(
            storage, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(region);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeStartDownload
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return -1;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return -1;
        }
        int result = ogc_offline_storage_start_download(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativePauseDownload
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return -1;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) { return -1; }
        int result = ogc_offline_storage_pause_download(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeResumeDownload
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return -1;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) { return -1; }
        int result = ogc_offline_storage_resume_download(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCancelDownload
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return -1;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) { return -1; }
        int result = ogc_offline_storage_cancel_download(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeIsDownloading
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return JNI_FALSE;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return JNI_FALSE;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) { return JNI_FALSE; }
        int result = ogc_offline_storage_is_downloading(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return result ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetDownloadProgress
  (JNIEnv* env, jobject obj, jlong ptr, jstring regionId) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0.0;
    }
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0.0;
        }
        const char* idStr = env->GetStringUTFChars(regionId, nullptr);
        if (!idStr) { return 0.0; }
        double progress = ogc_offline_storage_get_download_progress(storage, idStr);
        env->ReleaseStringUTFChars(regionId, idStr);
        return progress;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetStorageSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        return static_cast<jlong>(ogc_offline_storage_get_storage_size(storage));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetUsedSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        return static_cast<jlong>(ogc_offline_storage_get_used_size(storage));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetChartCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_offline_storage_t* storage =
            static_cast<ogc_offline_storage_t*>(JniConverter::FromJLongPtr(ptr));
        if (!storage) {
            JniException::ThrowNullPointerException(env, "OfflineStorage is null");
            return 0;
        }
        return static_cast<jlong>(ogc_offline_storage_get_chart_count(storage));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== CacheManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_cache_manager_t* mgr = ogc_cache_manager_create();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) { return; }
        ogc_cache_manager_destroy(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeGetCacheCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return 0;
        }
        return static_cast<jlong>(ogc_cache_manager_get_cache_count(mgr));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeGetCache
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return 0;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return 0; }
        ogc_tile_cache_t* cache = ogc_cache_manager_get_cache(mgr, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
        return JniConverter::ToJLongPtr(cache);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeAddCache
  (JNIEnv* env, jobject obj, jlong ptr, jstring name, jlong cachePtr) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return; }
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(cachePtr));
        ogc_cache_manager_add_cache(mgr, nameStr, cache);
        env->ReleaseStringUTFChars(name, nameStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeRemoveCache
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return; }
        ogc_cache_manager_remove_cache(mgr, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeClearAll
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return;
        }
        ogc_cache_manager_clear_all(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeGetTotalSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return 0;
        }
        return static_cast<jlong>(ogc_cache_manager_get_total_size(mgr));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeGetTotalTileCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return 0;
        }
        return static_cast<jlong>(ogc_cache_manager_get_total_tile_count(mgr));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_CacheManager_nativeFlushAll
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_cache_manager_t* mgr =
            static_cast<ogc_cache_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "CacheManager is null");
            return;
        }
        ogc_cache_manager_flush_all(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
