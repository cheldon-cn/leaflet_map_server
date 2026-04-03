#include "android_adapter/android_permission.h"

#ifdef __ANDROID__

#include <android/native_activity.h>
#include <android/log.h>
#include <map>

#define LOG_TAG "AndroidPermission"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace android {

AndroidPermission* AndroidPermission::s_instance = nullptr;

AndroidPermission* AndroidPermission::GetInstance() {
    if (!s_instance) {
        s_instance = new AndroidPermission();
    }
    return s_instance;
}

AndroidPermission::AndroidPermission()
    : m_activity(nullptr)
    , m_nextRequestCode(1000) {
    m_permissionMap[PermissionType::kLocation] = "android.permission.ACCESS_FINE_LOCATION";
    m_permissionMap[PermissionType::kStorage] = "android.permission.READ_EXTERNAL_STORAGE";
    m_permissionMap[PermissionType::kCamera] = "android.permission.CAMERA";
    m_permissionMap[PermissionType::kNetwork] = "android.permission.INTERNET";
    m_permissionMap[PermissionType::kPhone] = "android.permission.READ_PHONE_STATE";
}

AndroidPermission::~AndroidPermission() {
}

void AndroidPermission::Initialize(ANativeActivity* activity) {
    m_activity = activity;
}

bool AndroidPermission::CheckPermission(const std::string& permission) const {
    if (!m_activity) {
        return false;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return false;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID checkSelfPermissionMethod = env->GetMethodID(
        activityClass, "checkSelfPermission", "(Ljava/lang/String;)I");
    
    jstring permissionStr = env->NewStringUTF(permission.c_str());
    jint result = env->CallIntMethod(m_activity->clazz, checkSelfPermissionMethod, permissionStr);
    
    env->DeleteLocalRef(permissionStr);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
    
    return result == 0;
}

bool AndroidPermission::CheckPermission(PermissionType type) const {
    auto it = m_permissionMap.find(type);
    if (it != m_permissionMap.end()) {
        return CheckPermission(it->second);
    }
    return false;
}

void AndroidPermission::RequestPermission(const std::string& permission, PermissionCallback callback) {
    RequestPermissions({permission}, callback);
}

void AndroidPermission::RequestPermissions(const std::vector<std::string>& permissions, PermissionCallback callback) {
    if (!m_activity || permissions.empty()) {
        if (callback) {
            std::vector<PermissionResult> results;
            for (const auto& perm : permissions) {
                results.push_back({perm, PermissionStatus::kDenied, false});
            }
            callback(results);
        }
        return;
    }
    
    int requestCode = GenerateRequestCode();
    m_pendingCallbacks[requestCode] = std::move(callback);
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID requestPermissionsMethod = env->GetMethodID(
        activityClass, "requestPermissions", "([Ljava/lang/String;I)V");
    
    jobjectArray permissionArray = env->NewObjectArray(
        permissions.size(), env->FindClass("java/lang/String"), nullptr);
    
    for (size_t i = 0; i < permissions.size(); ++i) {
        jstring permStr = env->NewStringUTF(permissions[i].c_str());
        env->SetObjectArrayElement(permissionArray, i, permStr);
        env->DeleteLocalRef(permStr);
    }
    
    env->CallVoidMethod(m_activity->clazz, requestPermissionsMethod, permissionArray, requestCode);
    
    env->DeleteLocalRef(permissionArray);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

void AndroidPermission::RequestPermission(PermissionType type, PermissionCallback callback) {
    auto it = m_permissionMap.find(type);
    if (it != m_permissionMap.end()) {
        RequestPermission(it->second, std::move(callback));
    } else if (callback) {
        callback({{"", PermissionStatus::kDenied, false}});
    }
}

bool AndroidPermission::ShouldShowRequestPermissionRationale(const std::string& permission) const {
    if (!m_activity) {
        return false;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return false;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID shouldShowMethod = env->GetMethodID(
        activityClass, "shouldShowRequestPermissionRationale", "(Ljava/lang/String;)Z");
    
    jstring permissionStr = env->NewStringUTF(permission.c_str());
    jboolean result = env->CallBooleanMethod(m_activity->clazz, shouldShowMethod, permissionStr);
    
    env->DeleteLocalRef(permissionStr);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
    
    return result;
}

void AndroidPermission::OpenAppSettings() {
    if (!m_activity) {
        return;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID startActivityMethod = env->GetMethodID(
        activityClass, "startActivity", "(Landroid/content/Intent;)V");
    
    jclass intentClass = env->FindClass("android/content/Intent");
    jmethodID intentConstructor = env->GetMethodID(
        intentClass, "<init>", "(Landroid/content/Context;Ljava/lang/Class;)V");
    
    jclass settingsClass = env->FindClass("android/provider/Settings");
    jfieldID actionField = env->GetStaticFieldID(
        settingsClass, "ACTION_APPLICATION_DETAILS_SETTINGS", "Ljava/lang/String;");
    jstring actionStr = (jstring)env->GetStaticObjectField(settingsClass, actionField);
    
    jmethodID parseUriMethod = env->GetStaticMethodID(
        intentClass, "parseUri", "(Ljava/lang/String;I)Landroid/net/Uri;");
    jstring packageStr = env->NewStringUTF(("package:" + GetPackageName()).c_str());
    jobject uri = env->CallStaticObjectMethod(intentClass, parseUriMethod, packageStr, 0);
    
    jmethodID intentActionMethod = env->GetMethodID(intentClass, "setAction", "(Ljava/lang/String;)V");
    jmethodID intentDataMethod = env->GetMethodID(intentClass, "setData", "(Landroid/net/Uri;)V");
    
    jobject intent = env->NewObject(intentClass, intentConstructor, m_activity->clazz, nullptr);
    env->CallVoidMethod(intent, intentActionMethod, actionStr);
    env->CallVoidMethod(intent, intentDataMethod, uri);
    
    env->CallVoidMethod(m_activity->clazz, startActivityMethod, intent);
    
    env->DeleteLocalRef(intent);
    env->DeleteLocalRef(uri);
    env->DeleteLocalRef(packageStr);
    env->DeleteLocalRef(actionStr);
    env->DeleteLocalRef(settingsClass);
    env->DeleteLocalRef(intentClass);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

std::string AndroidPermission::GetPermissionName(PermissionType type) const {
    auto it = m_permissionMap.find(type);
    if (it != m_permissionMap.end()) {
        return it->second;
    }
    return "";
}

PermissionType AndroidPermission::GetPermissionType(const std::string& name) const {
    for (const auto& pair : m_permissionMap) {
        if (pair.second == name) {
            return pair.first;
        }
    }
    return PermissionType::kCustom;
}

void AndroidPermission::OnRequestPermissionsResult(int requestCode,
                                                    const std::vector<std::string>& permissions,
                                                    const std::vector<int>& grantResults) {
    auto it = m_pendingCallbacks.find(requestCode);
    if (it == m_pendingCallbacks.end()) {
        return;
    }
    
    PermissionCallback callback = std::move(it->second);
    m_pendingCallbacks.erase(it);
    
    std::vector<PermissionResult> results;
    for (size_t i = 0; i < permissions.size() && i < grantResults.size(); ++i) {
        PermissionResult result;
        result.name = permissions[i];
        result.status = (grantResults[i] == 0) ? PermissionStatus::kGranted : PermissionStatus::kDenied;
        result.shouldShowRationale = ShouldShowRequestPermissionRationale(permissions[i]);
        results.push_back(result);
    }
    
    if (callback) {
        callback(results);
    }
}

bool AndroidPermission::HasLocationPermission() const {
    return CheckPermission(PermissionType::kLocation);
}

bool AndroidPermission::HasStoragePermission() const {
    return CheckPermission(PermissionType::kStorage);
}

bool AndroidPermission::HasCameraPermission() const {
    return CheckPermission(PermissionType::kCamera);
}

bool AndroidPermission::HasNetworkPermission() const {
    return CheckPermission(PermissionType::kNetwork);
}

int AndroidPermission::GenerateRequestCode() {
    return m_nextRequestCode++;
}

std::string AndroidPermission::GetPackageName() const {
    if (m_activity && m_activity->packageName) {
        return m_activity->packageName;
    }
    return "";
}

}  
}  

#endif
