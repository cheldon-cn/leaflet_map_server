#ifndef OGC_ANDROID_ADAPTER_ANDROID_TOUCH_HANDLER_H
#define OGC_ANDROID_ADAPTER_ANDROID_TOUCH_HANDLER_H

#include "android_adapter/export.h"

#ifdef __ANDROID__

#include <vector>
#include <functional>
#include <chrono>

namespace ogc {
namespace android {

enum class TouchAction {
    kDown = 0,
    kUp = 1,
    kMove = 2,
    kCancel = 3,
    kPointerDown = 4,
    kPointerUp = 5
};

struct TouchPoint {
    int id = 0;
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 1.0f;
    float size = 1.0f;
};

struct TouchEvent {
    TouchAction action = TouchAction::kDown;
    std::vector<TouchPoint> points;
    std::chrono::steady_clock::time_point timestamp;
};

struct GestureState {
    bool isPinching = false;
    bool isPanning = false;
    bool isRotating = false;
    float scale = 1.0f;
    float rotation = 0.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    float focusX = 0.0f;
    float focusY = 0.0f;
};

class OGC_ANDROID_ADAPTER_API AndroidTouchHandler {
public:
    using TouchCallback = std::function<void(const TouchEvent&)>;
    using GestureCallback = std::function<void(const GestureState&)>;
    
    static AndroidTouchHandler* GetInstance();
    
    void ProcessTouchEvent(int action, int pointerCount, 
                           const float* x, const float* y,
                           const int* ids, const float* pressures);
    
    void SetTouchCallback(TouchCallback callback);
    void SetGestureCallback(GestureCallback callback);
    
    void SetTapThreshold(float distance);
    void SetLongPressThreshold(std::chrono::milliseconds duration);
    void SetSwipeThreshold(float velocity);
    
    const GestureState& GetGestureState() const;
    void ResetGestureState();
    
    bool IsTap(const TouchEvent& start, const TouchEvent& end) const;
    bool IsLongPress(const TouchEvent& start, const TouchEvent& end) const;
    bool IsSwipe(const TouchEvent& start, const TouchEvent& end) const;

private:
    AndroidTouchHandler();
    ~AndroidTouchHandler();
    
    AndroidTouchHandler(const AndroidTouchHandler&) = delete;
    AndroidTouchHandler& operator=(const AndroidTouchHandler&) = delete;
    
    void DetectGestures();
    void UpdatePinchGesture();
    void UpdatePanGesture();
    void UpdateRotationGesture();
    float CalculateDistance(float x1, float y1, float x2, float y2) const;
    float CalculateAngle(float x1, float y1, float x2, float y2) const;
    
    static AndroidTouchHandler* s_instance;
    
    TouchCallback m_touchCallback;
    GestureCallback m_gestureCallback;
    
    TouchEvent m_currentEvent;
    TouchEvent m_startEvent;
    GestureState m_gestureState;
    
    float m_tapThreshold;
    std::chrono::milliseconds m_longPressThreshold;
    float m_swipeThreshold;
    
    bool m_isTracking;
};

}  
}  

#endif

#endif
