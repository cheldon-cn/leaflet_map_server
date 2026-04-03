#ifndef OGC_ANDROID_ADAPTER_ANDROID_PLATFORM_H
#define OGC_ANDROID_ADAPTER_ANDROID_PLATFORM_H

#include "android_adapter/export.h"

#ifdef __ANDROID__

#include <string>
#include <memory>
#include <functional>

struct ANativeActivity;
struct ANativeWindow;
struct AAssetManager;

namespace ogc {
namespace android {

struct AndroidDeviceInfo {
    std::string manufacturer;
    std::string model;
    std::string brand;
    std::string device;
    std::string product;
    std::string hardware;
    std::string serial;
    int sdkVersion;
    std::string releaseVersion;
    std::string buildId;
    bool isEmulator;
    bool isRooted;
    int cpuCoreCount;
    long totalMemory;
    long availableMemory;
    int screenWidth;
    int screenHeight;
    float density;
    int densityDpi;
    float scaledDensity;
    float xdpi;
    float ydpi;
};

struct AndroidScreenInfo {
    int width;
    int height;
    float density;
    int densityDpi;
    float scaledDensity;
    float xdpi;
    float ydpi;
    int refreshRate;
    bool hasTouchScreen;
    int orientation;
};

class OGC_ANDROID_ADAPTER_API AndroidPlatform {
public:
    using IntentCallback = std::function<void(int, int, void*)>;
    
    static AndroidPlatform* GetInstance();
    
    void Initialize(ANativeActivity* activity);
    void Shutdown();
    
    const AndroidDeviceInfo& GetDeviceInfo() const;
    const AndroidScreenInfo& GetScreenInfo() const;
    
    std::string GetInternalDataPath() const;
    std::string GetExternalDataPath() const;
    std::string GetCachePath() const;
    std::string GetObbPath() const;
    
    AAssetManager* GetAssetManager() const;
    ANativeActivity* GetActivity() const;
    
    bool HasExternalStorage() const;
    bool IsExternalStorageRemovable() const;
    long GetAvailableInternalStorage() const;
    long GetAvailableExternalStorage() const;
    
    void Vibrate(long milliseconds);
    void Vibrate(const std::vector<long>& pattern, int repeat);
    void CancelVibrate();
    
    void ShowToast(const std::string& message, int duration = 0);
    void ShowNotification(const std::string& title, const std::string& message);
    
    void OpenUrl(const std::string& url);
    void ShareText(const std::string& subject, const std::string& text);
    void ShareFile(const std::string& filePath, const std::string& mimeType);
    
    void KeepScreenOn(bool keep);
    void SetOrientation(int orientation);
    void SetFullScreen(bool fullScreen);
    void SetStatusBarColor(int color);
    void SetNavigationBarColor(int color);
    
    void WakeLockAcquire();
    void WakeLockRelease();
    
    bool IsNetworkAvailable() const;
    bool IsWifiConnected() const;
    bool IsMobileConnected() const;
    std::string GetNetworkType() const;
    
    void RegisterIntentCallback(int requestCode, IntentCallback callback);
    void UnregisterIntentCallback(int requestCode);
    void OnActivityResult(int requestCode, int resultCode, void* data);
    
    std::string GetLocalizedString(const std::string& key) const;
    std::string GetCurrentLanguage() const;
    std::string GetCurrentLocale() const;
    
    bool IsLowMemoryDevice() const;
    long GetUsedMemory() const;
    void TrimMemory(int level);

private:
    AndroidPlatform();
    ~AndroidPlatform();
    
    AndroidPlatform(const AndroidPlatform&) = delete;
    AndroidPlatform& operator=(const AndroidPlatform&) = delete;
    
    void LoadDeviceInfo();
    void LoadScreenInfo();
    void UpdateMemoryInfo();
    
    static AndroidPlatform* s_instance;
    
    ANativeActivity* m_activity;
    AAssetManager* m_assetManager;
    AndroidDeviceInfo m_deviceInfo;
    AndroidScreenInfo m_screenInfo;
    std::map<int, IntentCallback> m_intentCallbacks;
    bool m_initialized;
};

}  
}  

#endif

#endif
