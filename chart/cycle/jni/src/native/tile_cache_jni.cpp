#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

#include <cstring>

using namespace ogc::jni;

extern "C" {

/* ===== TileKey ===== */

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeCreate
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));

        jclass tileKeyClass = env->FindClass("cn/cycle/chart/api/cache/TileKey");
        if (!tileKeyClass) {
            JniException::ThrowRuntimeException(env, "TileKey class not found");
            return nullptr;
        }

        jmethodID ctor = env->GetMethodID(tileKeyClass, "<init>", "(IIIJ)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "TileKey constructor not found");
            return nullptr;
        }

        jlong keyPtr = 0;
        std::memcpy(&keyPtr, &key, sizeof(ogc_tile_key_t) < sizeof(jlong)
            ? sizeof(ogc_tile_key_t) : sizeof(jlong));

        return env->NewObject(tileKeyClass, ctor, x, y, z, keyPtr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeEquals
  (JNIEnv* env, jclass clazz, jint x1, jint y1, jint z1,
   jint x2, jint y2, jint z2) {
    try {
        ogc_tile_key_t a = ogc_tile_key_create(
            static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(z1));
        ogc_tile_key_t b = ogc_tile_key_create(
            static_cast<int>(x2), static_cast<int>(y2), static_cast<int>(z2));
        return ogc_tile_key_equals(&a, &b) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeToString
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        char* str = ogc_tile_key_to_string(&key);
        if (!str) {
            return nullptr;
        }
        jstring result = JniConverter::ToJString(env, std::string(str));
        free(str);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeToIndex
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        return static_cast<jlong>(ogc_tile_key_to_index(&key));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeGetParent
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        ogc_tile_key_t parent = ogc_tile_key_get_parent(&key);

        jclass tileKeyClass = env->FindClass("cn/cycle/chart/api/cache/TileKey");
        if (!tileKeyClass) {
            JniException::ThrowRuntimeException(env, "TileKey class not found");
            return nullptr;
        }
        jmethodID ctor = env->GetMethodID(tileKeyClass, "<init>", "(III)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "TileKey constructor not found");
            return nullptr;
        }
        return env->NewObject(tileKeyClass, ctor,
            static_cast<jint>(parent.x), static_cast<jint>(parent.y),
            static_cast<jint>(parent.z));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jobjectArray JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeGetChildren
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        ogc_tile_key_t children[4];
        ogc_tile_key_get_children(&key, children);

        jclass tileKeyClass = env->FindClass("cn/cycle/chart/api/cache/TileKey");
        if (!tileKeyClass) {
            JniException::ThrowRuntimeException(env, "TileKey class not found");
            return nullptr;
        }
        jmethodID ctor = env->GetMethodID(tileKeyClass, "<init>", "(III)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "TileKey constructor not found");
            return nullptr;
        }

        jobjectArray result = env->NewObjectArray(4, tileKeyClass, nullptr);
        if (!result) {
            return nullptr;
        }
        for (int i = 0; i < 4; ++i) {
            jobject child = env->NewObject(tileKeyClass, ctor,
                static_cast<jint>(children[i].x),
                static_cast<jint>(children[i].y),
                static_cast<jint>(children[i].z));
            env->SetObjectArrayElement(result, i, child);
            env->DeleteLocalRef(child);
        }
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeFromString
  (JNIEnv* env, jclass clazz, jstring str) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }
    try {
        std::string s = JniConverter::ToString(env, str);
        ogc_tile_key_t key = ogc_tile_key_from_string(s.c_str());

        jclass tileKeyClass = env->FindClass("cn/cycle/chart/api/cache/TileKey");
        if (!tileKeyClass) {
            JniException::ThrowRuntimeException(env, "TileKey class not found");
            return nullptr;
        }
        jmethodID ctor = env->GetMethodID(tileKeyClass, "<init>", "(III)V");
        if (!ctor) {
            JniException::ThrowRuntimeException(env, "TileKey constructor not found");
            return nullptr;
        }
        return env->NewObject(tileKeyClass, ctor,
            static_cast<jint>(key.x), static_cast<jint>(key.y),
            static_cast<jint>(key.z));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileKey_nativeToEnvelope
  (JNIEnv* env, jclass clazz, jint x, jint y, jint z) {
    try {
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        ogc_envelope_t* envelope = ogc_tile_key_to_envelope(&key);
        return JniConverter::ToJLongPtr(envelope);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== TileCache ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_tile_cache_t* cache = ogc_tile_cache_create();
        return JniConverter::ToJLongPtr(cache);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (cache) {
            ogc_tile_cache_destroy(cache);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeHasTile
  (JNIEnv* env, jobject obj, jlong ptr, jint x, jint y, jint z) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return JNI_FALSE;
        }
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        return ogc_tile_cache_has_tile(cache, &key) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeGetTilePtr
  (JNIEnv* env, jobject obj, jlong ptr, jint x, jint y, jint z) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        ogc_tile_data_t* tileData = ogc_tile_cache_get_tile(cache, &key);
        return JniConverter::ToJLongPtr(tileData);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativePutTile
  (JNIEnv* env, jobject obj, jlong ptr, jint x, jint y, jint z,
   jbyteArray data) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            JniException::ThrowNullPointerException(env, "TileCache is null");
            return;
        }

        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));

        jsize dataLen = env->GetArrayLength(data);
        jbyte* dataElems = env->GetByteArrayElements(data, nullptr);
        if (!dataElems) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get array elements");
            return;
        }

        ogc_tile_cache_put_tile(cache, &key, dataElems,
            static_cast<size_t>(dataLen));
        env->ReleaseByteArrayElements(data, dataElems, JNI_ABORT);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeRemoveTile
  (JNIEnv* env, jobject obj, jlong ptr, jint x, jint y, jint z) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return;
        }
        ogc_tile_key_t key = ogc_tile_key_create(
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        ogc_tile_cache_remove_tile(cache, &key);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (cache) {
            ogc_tile_cache_clear(cache);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeGetSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        return static_cast<jlong>(ogc_tile_cache_get_size(cache));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeGetMaxSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        return static_cast<jlong>(ogc_tile_cache_get_max_size(cache));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeSetMaxSize
  (JNIEnv* env, jobject obj, jlong ptr, jlong maxSize) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            JniException::ThrowNullPointerException(env, "TileCache is null");
            return;
        }
        ogc_tile_cache_set_max_size(cache, static_cast<size_t>(maxSize));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeGetCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        return static_cast<jint>(ogc_tile_cache_get_count(cache));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeGetCapacity
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        return static_cast<jlong>(ogc_tile_cache_get_capacity(cache));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_cache_TileCache_nativeSetCapacity
  (JNIEnv* env, jobject obj, jlong ptr, jlong capacity) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            JniException::ThrowNullPointerException(env, "TileCache is null");
            return;
        }
        ogc_tile_cache_set_capacity(cache, static_cast<size_t>(capacity));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== MemoryTileCache ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_MemoryTileCache_nativeCreate
  (JNIEnv* env, jclass clazz, jlong maxSizeBytes) {
    try {
        ogc_tile_cache_t* cache = ogc_memory_tile_cache_create(
            static_cast<size_t>(maxSizeBytes));
        return JniConverter::ToJLongPtr(cache);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_MemoryTileCache_nativeGetMemoryUsage
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_tile_cache_t* cache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(ptr));
        if (!cache) {
            return 0;
        }
        return static_cast<jlong>(ogc_memory_tile_cache_get_memory_usage(cache));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== DiskTileCache ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_DiskTileCache_nativeCreate
  (JNIEnv* env, jclass clazz, jstring cacheDir, jlong maxSizeBytes) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        std::string dir = JniConverter::ToString(env, cacheDir);
        ogc_tile_cache_t* cache = ogc_disk_tile_cache_create(
            dir.c_str(), static_cast<size_t>(maxSizeBytes));
        return JniConverter::ToJLongPtr(cache);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== MultiLevelTileCache ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_cache_MultiLevelTileCache_nativeCreate
  (JNIEnv* env, jclass clazz, jlong memoryCachePtr, jlong diskCachePtr) {
    try {
        ogc_tile_cache_t* memCache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(memoryCachePtr));
        ogc_tile_cache_t* diskCache =
            static_cast<ogc_tile_cache_t*>(JniConverter::FromJLongPtr(diskCachePtr));
        if (!memCache || !diskCache) {
            JniException::ThrowNullPointerException(env, "Cache pointer is null");
            return 0;
        }
        ogc_tile_cache_t* cache = ogc_multi_level_tile_cache_create(memCache, diskCache);
        return JniConverter::ToJLongPtr(cache);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

}
