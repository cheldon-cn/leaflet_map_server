#ifndef OGC_ANDROID_ADAPTER_ANDROID_LIFECYCLE_H
#define OGC_ANDROID_ADAPTER_ANDROID_LIFECYCLE_H

#include "android_adapter/export.h"

#ifdef __ANDROID__

#include <functional>
#include <string>
#include <memory>

struct ANativeActivity;
struct ANativeWindow;

namespace ogc {
namespace android {

enum class LifecycleState {
    kUnknown = 0,
    kStarted = 1,
    kResumed = 2,
    kPaused = 3,
    kStopped = 4,
    kDestroyed = 5
};

struct LifecycleCallbacks {
    std::function<void()> onStart;
    std::function<void()> onResume;
    std::function<void()> onPause;
    std::function<void()> onStop;
    std::function<void()> onDestroy;
    std::function<void(int, int)> onWindowResized;
    std::function<void(ANativeWindow*)> onWindowCreated;
    std::function<void()> onWindowDestroyed;
    std::function<void()> onLowMemory;
    std::function<void(const std::string&)> onConfigurationChanged;
};

class OGC_ANDROID_ADAPTER_API AndroidLifecycle {
public:
    static AndroidLifecycle* GetInstance();
    
    void Initialize(ANativeActivity* activity);
    void Shutdown();
    
    void SetCallbacks(const LifecycleCallbacks& callbacks);
    void ClearCallbacks();
    
    LifecycleState GetState() const;
    bool IsRunning() const;
    bool IsVisible() const;
    
    ANativeActivity* GetActivity() const;
    ANativeWindow* GetWindow() const;
    
    void OnStart();
    void OnResume();
    void OnPause();
    void OnStop();
    void OnDestroy();
    
    void OnWindowCreated(ANativeWindow* window);
    void OnWindowDestroyed();
    void OnWindowResized(int width, int height);
    
    void OnLowMemory();
    void OnConfigurationChanged(const std::string& config);
    
    std::string GetInternalDataPath() const;
    std::string GetExternalDataPath() const;
    std::string GetObbPath() const;
    
    int GetSdkVersion() const;
    std::string GetPackageName() const;

private:
    AndroidLifecycle();
    ~AndroidLifecycle();
    
    AndroidLifecycle(const AndroidLifecycle&) = delete;
    AndroidLifecycle& operator=(const AndroidLifecycle&) = delete;
    
    static AndroidLifecycle* s_instance;
    
    ANativeActivity* m_activity;
    ANativeWindow* m_window;
    LifecycleState m_state;
    LifecycleCallbacks m_callbacks;
    int m_windowWidth;
    int m_windowHeight;
};

}  
}  

#endif

#endif
