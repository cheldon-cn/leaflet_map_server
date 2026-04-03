#include "android_adapter/android_lifecycle.h"

#ifdef __ANDROID__

#include <android/native_activity.h>
#include <android/configuration.h>
#include <cstdlib>
#include <cstring>

namespace ogc {
namespace android {

AndroidLifecycle* AndroidLifecycle::s_instance = nullptr;

AndroidLifecycle* AndroidLifecycle::GetInstance() {
    if (!s_instance) {
        s_instance = new AndroidLifecycle();
    }
    return s_instance;
}

AndroidLifecycle::AndroidLifecycle()
    : m_activity(nullptr)
    , m_window(nullptr)
    , m_state(LifecycleState::kUnknown)
    , m_windowWidth(0)
    , m_windowHeight(0) {
}

AndroidLifecycle::~AndroidLifecycle() {
}

void AndroidLifecycle::Initialize(ANativeActivity* activity) {
    m_activity = activity;
    m_state = LifecycleState::kStarted;
}

void AndroidLifecycle::Shutdown() {
    m_activity = nullptr;
    m_window = nullptr;
    m_state = LifecycleState::kDestroyed;
}

void AndroidLifecycle::SetCallbacks(const LifecycleCallbacks& callbacks) {
    m_callbacks = callbacks;
}

void AndroidLifecycle::ClearCallbacks() {
    m_callbacks = LifecycleCallbacks();
}

LifecycleState AndroidLifecycle::GetState() const {
    return m_state;
}

bool AndroidLifecycle::IsRunning() const {
    return m_state == LifecycleState::kResumed;
}

bool AndroidLifecycle::IsVisible() const {
    return m_state == LifecycleState::kResumed || m_state == LifecycleState::kPaused;
}

ANativeActivity* AndroidLifecycle::GetActivity() const {
    return m_activity;
}

ANativeWindow* AndroidLifecycle::GetWindow() const {
    return m_window;
}

void AndroidLifecycle::OnStart() {
    m_state = LifecycleState::kStarted;
    if (m_callbacks.onStart) {
        m_callbacks.onStart();
    }
}

void AndroidLifecycle::OnResume() {
    m_state = LifecycleState::kResumed;
    if (m_callbacks.onResume) {
        m_callbacks.onResume();
    }
}

void AndroidLifecycle::OnPause() {
    m_state = LifecycleState::kPaused;
    if (m_callbacks.onPause) {
        m_callbacks.onPause();
    }
}

void AndroidLifecycle::OnStop() {
    m_state = LifecycleState::kStopped;
    if (m_callbacks.onStop) {
        m_callbacks.onStop();
    }
}

void AndroidLifecycle::OnDestroy() {
    m_state = LifecycleState::kDestroyed;
    if (m_callbacks.onDestroy) {
        m_callbacks.onDestroy();
    }
}

void AndroidLifecycle::OnWindowCreated(ANativeWindow* window) {
    m_window = window;
    if (m_callbacks.onWindowCreated) {
        m_callbacks.onWindowCreated(window);
    }
}

void AndroidLifecycle::OnWindowDestroyed() {
    m_window = nullptr;
    if (m_callbacks.onWindowDestroyed) {
        m_callbacks.onWindowDestroyed();
    }
}

void AndroidLifecycle::OnWindowResized(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    if (m_callbacks.onWindowResized) {
        m_callbacks.onWindowResized(width, height);
    }
}

void AndroidLifecycle::OnLowMemory() {
    if (m_callbacks.onLowMemory) {
        m_callbacks.onLowMemory();
    }
}

void AndroidLifecycle::OnConfigurationChanged(const std::string& config) {
    if (m_callbacks.onConfigurationChanged) {
        m_callbacks.onConfigurationChanged(config);
    }
}

std::string AndroidLifecycle::GetInternalDataPath() const {
    if (m_activity && m_activity->internalDataPath) {
        return m_activity->internalDataPath;
    }
    return "";
}

std::string AndroidLifecycle::GetExternalDataPath() const {
    if (m_activity && m_activity->externalDataPath) {
        return m_activity->externalDataPath;
    }
    return "";
}

std::string AndroidLifecycle::GetObbPath() const {
    if (m_activity && m_activity->obbPath) {
        return m_activity->obbPath;
    }
    return "";
}

int AndroidLifecycle::GetSdkVersion() const {
    if (m_activity) {
        return m_activity->sdkVersion;
    }
    return 0;
}

std::string AndroidLifecycle::GetPackageName() const {
    if (m_activity && m_activity->packageName) {
        return m_activity->packageName;
    }
    return "";
}

}  
}  

#endif
