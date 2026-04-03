#include "android_adapter/android_platform.h"

#ifdef __ANDROID__

#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android/configuration.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#define LOG_TAG "AndroidPlatform"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace ogc {
namespace android {

AndroidPlatform* AndroidPlatform::s_instance = nullptr;

AndroidPlatform* AndroidPlatform::GetInstance() {
    if (!s_instance) {
        s_instance = new AndroidPlatform();
    }
    return s_instance;
}

AndroidPlatform::AndroidPlatform()
    : m_activity(nullptr)
    , m_assetManager(nullptr)
    , m_initialized(false) {
}

AndroidPlatform::~AndroidPlatform() {
}

void AndroidPlatform::Initialize(ANativeActivity* activity) {
    m_activity = activity;
    if (activity && activity->assetManager) {
        m_assetManager = activity->assetManager;
    }
    
    LoadDeviceInfo();
    LoadScreenInfo();
    m_initialized = true;
}

void AndroidPlatform::Shutdown() {
    m_activity = nullptr;
    m_assetManager = nullptr;
    m_initialized = false;
}

void AndroidPlatform::LoadDeviceInfo() {
    char prop[PROP_VALUE_MAX] = {0};
    
    __system_property_get("ro.product.manufacturer", prop);
    m_deviceInfo.manufacturer = prop;
    
    __system_property_get("ro.product.model", prop);
    m_deviceInfo.model = prop;
    
    __system_property_get("ro.product.brand", prop);
    m_deviceInfo.brand = prop;
    
    __system_property_get("ro.product.device", prop);
    m_deviceInfo.device = prop;
    
    __system_property_get("ro.product.product", prop);
    m_deviceInfo.product = prop;
    
    __system_property_get("ro.hardware", prop);
    m_deviceInfo.hardware = prop;
    
    __system_property_get("ro.build.version.sdk", prop);
    m_deviceInfo.sdkVersion = atoi(prop);
    
    __system_property_get("ro.build.version.release", prop);
    m_deviceInfo.releaseVersion = prop;
    
    __system_property_get("ro.build.id", prop);
    m_deviceInfo.buildId = prop;
    
    m_deviceInfo.cpuCoreCount = sysconf(_SC_NPROCESSORS_ONLN);
    
    m_deviceInfo.isEmulator = 
        m_deviceInfo.model.find("sdk") != std::string::npos ||
        m_deviceInfo.model.find("emulator") != std::string::npos ||
        m_deviceInfo.manufacturer.find("Genymotion") != std::string::npos;
}

void AndroidPlatform::LoadScreenInfo() {
    if (!m_activity) {
        return;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    
    jmethodID getWindowManagerMethod = env->GetMethodID(activityClass, "getWindowManager", "()Landroid/view/WindowManager;");
    jobject windowManager = env->CallObjectMethod(m_activity->clazz, getWindowManagerMethod);
    
    jclass windowManagerClass = env->GetObjectClass(windowManager);
    jmethodID getDefaultDisplayMethod = env->GetMethodID(windowManagerClass, "getDefaultDisplay", "()Landroid/view/Display;");
    jobject display = env->CallObjectMethod(windowManager, getDefaultDisplayMethod);
    
    jclass displayClass = env->GetObjectClass(display);
    jmethodID getMetricsMethod = env->GetMethodID(displayClass, "getMetrics", "(Landroid/util/DisplayMetrics;)V");
    
    jclass metricsClass = env->FindClass("android/util/DisplayMetrics");
    jmethodID metricsConstructor = env->GetMethodID(metricsClass, "<init>", "()V");
    jobject metrics = env->NewObject(metricsClass, metricsConstructor);
    
    env->CallVoidMethod(display, getMetricsMethod, metrics);
    
    jfieldID widthField = env->GetFieldID(metricsClass, "widthPixels", "I");
    jfieldID heightField = env->GetFieldID(metricsClass, "heightPixels", "I");
    jfieldID densityField = env->GetFieldID(metricsClass, "density", "F");
    jfieldID densityDpiField = env->GetFieldID(metricsClass, "densityDpi", "I");
    jfieldID scaledDensityField = env->GetFieldID(metricsClass, "scaledDensity", "F");
    jfieldID xdpiField = env->GetFieldID(metricsClass, "xdpi", "F");
    jfieldID ydpiField = env->GetFieldID(metricsClass, "ydpi", "F");
    
    m_screenInfo.width = env->GetIntField(metrics, widthField);
    m_screenInfo.height = env->GetIntField(metrics, heightField);
    m_screenInfo.density = env->GetFloatField(metrics, densityField);
    m_screenInfo.densityDpi = env->GetIntField(metrics, densityDpiField);
    m_screenInfo.scaledDensity = env->GetFloatField(metrics, scaledDensityField);
    m_screenInfo.xdpi = env->GetFloatField(metrics, xdpiField);
    m_screenInfo.ydpi = env->GetFloatField(metrics, ydpiField);
    
    m_deviceInfo.screenWidth = m_screenInfo.width;
    m_deviceInfo.screenHeight = m_screenInfo.height;
    m_deviceInfo.density = m_screenInfo.density;
    m_deviceInfo.densityDpi = m_screenInfo.densityDpi;
    
    env->DeleteLocalRef(metrics);
    env->DeleteLocalRef(metricsClass);
    env->DeleteLocalRef(display);
    env->DeleteLocalRef(displayClass);
    env->DeleteLocalRef(windowManager);
    env->DeleteLocalRef(windowManagerClass);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

const AndroidDeviceInfo& AndroidPlatform::GetDeviceInfo() const {
    return m_deviceInfo;
}

const AndroidScreenInfo& AndroidPlatform::GetScreenInfo() const {
    return m_screenInfo;
}

std::string AndroidPlatform::GetInternalDataPath() const {
    if (m_activity && m_activity->internalDataPath) {
        return m_activity->internalDataPath;
    }
    return "";
}

std::string AndroidPlatform::GetExternalDataPath() const {
    if (m_activity && m_activity->externalDataPath) {
        return m_activity->externalDataPath;
    }
    return "";
}

std::string AndroidPlatform::GetCachePath() const {
    std::string dataPath = GetInternalDataPath();
    if (!dataPath.empty()) {
        return dataPath + "/cache";
    }
    return "";
}

std::string AndroidPlatform::GetObbPath() const {
    if (m_activity && m_activity->obbPath) {
        return m_activity->obbPath;
    }
    return "";
}

AAssetManager* AndroidPlatform::GetAssetManager() const {
    return m_assetManager;
}

ANativeActivity* AndroidPlatform::GetActivity() const {
    return m_activity;
}

bool AndroidPlatform::HasExternalStorage() const {
    return !GetExternalDataPath().empty();
}

bool AndroidPlatform::IsExternalStorageRemovable() const {
    return true;
}

long AndroidPlatform::GetAvailableInternalStorage() const {
    std::string path = GetInternalDataPath();
    if (path.empty()) {
        return 0;
    }
    
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) == 0) {
        return stat.f_bavail * stat.f_frsize;
    }
    return 0;
}

long AndroidPlatform::GetAvailableExternalStorage() const {
    std::string path = GetExternalDataPath();
    if (path.empty()) {
        return 0;
    }
    
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) == 0) {
        return stat.f_bavail * stat.f_frsize;
    }
    return 0;
}

void AndroidPlatform::Vibrate(long milliseconds) {
    if (!m_activity) {
        return;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID getSystemServiceMethod = env->GetMethodID(
        activityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring vibratorService = env->NewStringUTF("vibrator");
    jobject vibrator = env->CallObjectMethod(m_activity->clazz, getSystemServiceMethod, vibratorService);
    
    if (vibrator) {
        jclass vibratorClass = env->GetObjectClass(vibrator);
        jmethodID vibrateMethod = env->GetMethodID(vibratorClass, "vibrate", "(J)V");
        env->CallVoidMethod(vibrator, vibrateMethod, (jlong)milliseconds);
        
        env->DeleteLocalRef(vibratorClass);
        env->DeleteLocalRef(vibrator);
    }
    
    env->DeleteLocalRef(vibratorService);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

void AndroidPlatform::ShowToast(const std::string& message, int duration) {
    if (!m_activity) {
        return;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID makeTextMethod = env->GetStaticMethodID(
        env->FindClass("android/widget/Toast"), "makeText",
        "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    
    jstring messageStr = env->NewStringUTF(message.c_str());
    jobject toast = env->CallStaticObjectMethod(
        env->FindClass("android/widget/Toast"), makeTextMethod,
        m_activity->clazz, messageStr, duration == 0 ? 0 : 1);
    
    if (toast) {
        jmethodID showMethod = env->GetMethodID(env->GetObjectClass(toast), "show", "()V");
        env->CallVoidMethod(toast, showMethod);
        env->DeleteLocalRef(toast);
    }
    
    env->DeleteLocalRef(messageStr);
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

void AndroidPlatform::KeepScreenOn(bool keep) {
    if (!m_activity) {
        return;
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return;
    }
    
    jclass activityClass = env->GetObjectClass(m_activity->clazz);
    jmethodID getWindowMethod = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");
    jobject window = env->CallObjectMethod(m_activity->clazz, getWindowMethod);
    
    if (window) {
        jclass windowClass = env->GetObjectClass(window);
        jfieldID flagKeepScreenOn = env->GetStaticFieldID(
            env->FindClass("android/view/WindowManager$LayoutParams"),
            "FLAG_KEEP_SCREEN_ON", "I");
        int flag = env->GetStaticIntField(env->FindClass("android/view/WindowManager$LayoutParams"), flagKeepScreenOn);
        
        jmethodID addFlagsMethod = env->GetMethodID(windowClass, "addFlags", "(I)V");
        jmethodID clearFlagsMethod = env->GetMethodID(windowClass, "clearFlags", "(I)V");
        
        if (keep) {
            env->CallVoidMethod(window, addFlagsMethod, flag);
        } else {
            env->CallVoidMethod(window, clearFlagsMethod, flag);
        }
        
        env->DeleteLocalRef(windowClass);
        env->DeleteLocalRef(window);
    }
    
    env->DeleteLocalRef(activityClass);
    m_activity->vm->DetachCurrentThread();
}

std::string AndroidPlatform::GetCurrentLanguage() const {
    if (!m_activity) {
        return "en";
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return "en";
    }
    
    jclass localeClass = env->FindClass("java/util/Locale");
    jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
    jobject locale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);
    
    jmethodID getLanguageMethod = env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;");
    jstring languageStr = (jstring)env->CallObjectMethod(locale, getLanguageMethod);
    
    const char* lang = env->GetStringUTFChars(languageStr, nullptr);
    std::string result(lang);
    env->ReleaseStringUTFChars(languageStr, lang);
    
    env->DeleteLocalRef(languageStr);
    env->DeleteLocalRef(locale);
    env->DeleteLocalRef(localeClass);
    m_activity->vm->DetachCurrentThread();
    
    return result;
}

std::string AndroidPlatform::GetCurrentLocale() const {
    if (!m_activity) {
        return "en_US";
    }
    
    JNIEnv* env;
    if (m_activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        return "en_US";
    }
    
    jclass localeClass = env->FindClass("java/util/Locale");
    jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
    jobject locale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);
    
    jmethodID toStringMethod = env->GetMethodID(localeClass, "toString", "()Ljava/lang/String;");
    jstring localeStr = (jstring)env->CallObjectMethod(locale, toStringMethod);
    
    const char* loc = env->GetStringUTFChars(localeStr, nullptr);
    std::string result(loc);
    env->ReleaseStringUTFChars(localeStr, loc);
    
    env->DeleteLocalRef(localeStr);
    env->DeleteLocalRef(locale);
    env->DeleteLocalRef(localeClass);
    m_activity->vm->DetachCurrentThread();
    
    return result;
}

bool AndroidPlatform::IsLowMemoryDevice() const {
    return m_deviceInfo.totalMemory < 1024 * 1024 * 1024;
}

long AndroidPlatform::GetUsedMemory() const {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long total = 0;
    long free = 0;
    
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &total);
        } else if (line.find("MemFree:") == 0) {
            sscanf(line.c_str(), "MemFree: %ld kB", &free);
        }
    }
    
    return (total - free) * 1024;
}

void AndroidPlatform::TrimMemory(int level) {
    LOGI("TrimMemory called with level %d", level);
}

void AndroidPlatform::UpdateMemoryInfo() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            long total;
            sscanf(line.c_str(), "MemTotal: %ld kB", &total);
            m_deviceInfo.totalMemory = total * 1024;
        } else if (line.find("MemAvailable:") == 0) {
            long available;
            sscanf(line.c_str(), "MemAvailable: %ld kB", &available);
            m_deviceInfo.availableMemory = available * 1024;
        }
    }
}

}  
}  

#endif
