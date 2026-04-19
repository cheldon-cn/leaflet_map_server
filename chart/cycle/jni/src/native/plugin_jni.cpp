#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_plugin_manager_t* mgr = ogc_plugin_manager_get_instance();
        return JniConverter::ToJLongPtr(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeLoadPlugin
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_plugin_manager_t* mgr =
            static_cast<ogc_plugin_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PluginManager is null");
            return -1;
        }
        const char* pathStr = env->GetStringUTFChars(path, nullptr);
        if (!pathStr) { return -1; }
        int result = ogc_plugin_manager_load_plugin(mgr, pathStr);
        env->ReleaseStringUTFChars(path, pathStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeUnloadPlugin
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }
    try {
        ogc_plugin_manager_t* mgr =
            static_cast<ogc_plugin_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PluginManager is null");
            return -1;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return -1; }
        int result = ogc_plugin_manager_unload_plugin(mgr, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_plugin_manager_t* mgr =
            static_cast<ogc_plugin_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) {
            JniException::ThrowNullPointerException(env, "PluginManager is null");
            return 0;
        }
        return ogc_plugin_manager_get_plugin_count(mgr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginName
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    try {
        ogc_plugin_manager_t* mgr =
            static_cast<ogc_plugin_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr) { return nullptr; }
        const char* name = ogc_plugin_manager_get_plugin_name(mgr, static_cast<int>(index));
        if (!name) { return nullptr; }
        return env->NewStringUTF(name);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name, jstring version) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    try {
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) { return 0; }
        const char* verStr = env->GetStringUTFChars(version, nullptr);
        if (!verStr) {
            env->ReleaseStringUTFChars(name, nameStr);
            return 0;
        }
        ogc_chart_plugin_t* plugin = ogc_chart_plugin_create(nameStr, verStr);
        env->ReleaseStringUTFChars(name, nameStr);
        env->ReleaseStringUTFChars(version, verStr);
        return JniConverter::ToJLongPtr(plugin);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_plugin_t* plugin =
            static_cast<ogc_chart_plugin_t*>(JniConverter::FromJLongPtr(ptr));
        if (!plugin) { return; }
        ogc_chart_plugin_destroy(plugin);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_plugin_t* plugin =
            static_cast<ogc_chart_plugin_t*>(JniConverter::FromJLongPtr(ptr));
        if (!plugin) { return nullptr; }
        const char* name = ogc_chart_plugin_get_name(plugin);
        if (!name) { return nullptr; }
        return env->NewStringUTF(name);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetVersion
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_plugin_t* plugin =
            static_cast<ogc_chart_plugin_t*>(JniConverter::FromJLongPtr(ptr));
        if (!plugin) { return nullptr; }
        const char* ver = ogc_chart_plugin_get_version(plugin);
        if (!ver) { return nullptr; }
        return env->NewStringUTF(ver);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_plugin_t* plugin =
            static_cast<ogc_chart_plugin_t*>(JniConverter::FromJLongPtr(ptr));
        if (!plugin) {
            JniException::ThrowNullPointerException(env, "ChartPlugin is null");
            return -1;
        }
        return ogc_chart_plugin_initialize(plugin);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_plugin_t* plugin =
            static_cast<ogc_chart_plugin_t*>(JniConverter::FromJLongPtr(ptr));
        if (!plugin) { return; }
        ogc_chart_plugin_shutdown(plugin);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPlugin
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    try {
        ogc_plugin_manager_t* mgr =
            static_cast<ogc_plugin_manager_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mgr || !name) { return 0; }
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) { return 0; }
        ogc_chart_plugin_t* plugin = ogc_plugin_manager_get_plugin(mgr, cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(plugin);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

}
