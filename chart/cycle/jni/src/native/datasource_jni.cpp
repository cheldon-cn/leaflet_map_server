#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== DataSource ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeOpen
  (JNIEnv* env, jclass clazz, jstring path) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        const char* pathStr = env->GetStringUTFChars(path, nullptr);
        if (!pathStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_datasource_t* ds = ogc_datasource_open(pathStr);
        env->ReleaseStringUTFChars(path, pathStr);
        return JniConverter::ToJLongPtr(ds);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeClose
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) { return; }
        ogc_datasource_close(ds);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeIsOpen
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return JNI_FALSE;
        }
        return ogc_datasource_is_open(ds) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeGetPath
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return nullptr;
        }
        const char* path = ogc_datasource_get_path(ds);
        if (!path) { return nullptr; }
        return env->NewStringUTF(path);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayerCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return 0;
        }
        return static_cast<jlong>(ogc_datasource_get_layer_count(ds));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return 0;
        }
        ogc_layer_t* layer = ogc_datasource_get_layer(ds, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayerByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return 0;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return 0; }
        ogc_layer_t* layer = ogc_datasource_get_layer_by_name(ds, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeCreateLayer
  (JNIEnv* env, jobject obj, jlong ptr, jstring name, jint geomType) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return 0;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return 0; }
        ogc_layer_t* layer = ogc_datasource_create_layer(ds, nameStr, static_cast<int>(geomType));
        env->ReleaseStringUTFChars(name, nameStr);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_DataSource_nativeDeleteLayer
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_datasource_t* ds =
            static_cast<ogc_datasource_t*>(JniConverter::FromJLongPtr(ptr));
        if (!ds) {
            JniException::ThrowNullPointerException(env, "DataSource is null");
            return -1;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return -1; }
        int result = ogc_datasource_delete_layer(ds, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

/* ===== DriverManager ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_driver_manager_t* mgr = ogc_driver_manager_get_instance();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeRegisterDriver
  (JNIEnv* env, jobject obj, jlong ptr, jlong driverPtr) {
    try {
        ogc_driver_manager_t* mgr =
            static_cast<ogc_driver_manager_t*>(JniConverter::FromJLongPtr(ptr));
        ogc_driver_t* driver =
            static_cast<ogc_driver_t*>(JniConverter::FromJLongPtr(driverPtr));
        if (!mgr || !driver) {
            JniException::ThrowNullPointerException(env, "DriverManager or Driver is null");
            return -1;
        }
        return ogc_driver_manager_register_driver(mgr, driver);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeUnregisterDriver
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    try {
        ogc_driver_manager_t* mgr =
            static_cast<ogc_driver_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr || !name) {
            JniException::ThrowNullPointerException(env, "DriverManager or name is null");
            return -1;
        }
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) { return -1; }
        int ret = ogc_driver_manager_unregister_driver(mgr, cname);
        env->ReleaseStringUTFChars(name, cname);
        return ret;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriver
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    try {
        ogc_driver_manager_t* mgr =
            static_cast<ogc_driver_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr || !name) { return 0; }
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) { return 0; }
        ogc_driver_t* driver = ogc_driver_manager_get_driver(mgr, cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(driver);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriverCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_driver_manager_t* mgr =
            static_cast<ogc_driver_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "DriverManager is null");
            return 0;
        }
        return ogc_driver_manager_get_driver_count(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriverName
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    try {
        ogc_driver_manager_t* mgr =
            static_cast<ogc_driver_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "DriverManager is null");
            return nullptr;
        }
        const char* name = ogc_driver_manager_get_driver_name(mgr, static_cast<int>(index));
        return name ? env->NewStringUTF(name) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

}
