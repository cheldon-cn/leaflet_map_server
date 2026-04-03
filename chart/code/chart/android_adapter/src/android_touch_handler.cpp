#include "android_adapter/android_touch_handler.h"

#ifdef __ANDROID__

#include <cmath>
#include <algorithm>

namespace ogc {
namespace android {

AndroidTouchHandler* AndroidTouchHandler::s_instance = nullptr;

AndroidTouchHandler* AndroidTouchHandler::GetInstance() {
    if (!s_instance) {
        s_instance = new AndroidTouchHandler();
    }
    return s_instance;
}

AndroidTouchHandler::AndroidTouchHandler()
    : m_tapThreshold(10.0f)
    , m_longPressThreshold(500)
    , m_swipeThreshold(100.0f)
    , m_isTracking(false) {
}

AndroidTouchHandler::~AndroidTouchHandler() {
}

void AndroidTouchHandler::ProcessTouchEvent(int action, int pointerCount,
                                            const float* x, const float* y,
                                            const int* ids, const float* pressures) {
    m_currentEvent.action = static_cast<TouchAction>(action);
    m_currentEvent.points.clear();
    m_currentEvent.timestamp = std::chrono::steady_clock::now();
    
    for (int i = 0; i < pointerCount; ++i) {
        TouchPoint point;
        point.id = ids ? ids[i] : i;
        point.x = x[i];
        point.y = y[i];
        point.pressure = pressures ? pressures[i] : 1.0f;
        m_currentEvent.points.push_back(point);
    }
    
    if (action == static_cast<int>(TouchAction::kDown)) {
        m_startEvent = m_currentEvent;
        m_isTracking = true;
        ResetGestureState();
    } else if (action == static_cast<int>(TouchAction::kUp) ||
               action == static_cast<int>(TouchAction::kCancel)) {
        m_isTracking = false;
    }
    
    if (m_isTracking && pointerCount >= 2) {
        DetectGestures();
    }
    
    if (m_touchCallback) {
        m_touchCallback(m_currentEvent);
    }
}

void AndroidTouchHandler::SetTouchCallback(TouchCallback callback) {
    m_touchCallback = std::move(callback);
}

void AndroidTouchHandler::SetGestureCallback(GestureCallback callback) {
    m_gestureCallback = std::move(callback);
}

void AndroidTouchHandler::SetTapThreshold(float distance) {
    m_tapThreshold = distance;
}

void AndroidTouchHandler::SetLongPressThreshold(std::chrono::milliseconds duration) {
    m_longPressThreshold = duration;
}

void AndroidTouchHandler::SetSwipeThreshold(float velocity) {
    m_swipeThreshold = velocity;
}

const GestureState& AndroidTouchHandler::GetGestureState() const {
    return m_gestureState;
}

void AndroidTouchHandler::ResetGestureState() {
    m_gestureState = GestureState();
}

bool AndroidTouchHandler::IsTap(const TouchEvent& start, const TouchEvent& end) const {
    if (start.points.empty() || end.points.empty()) {
        return false;
    }
    
    float dx = end.points[0].x - start.points[0].x;
    float dy = end.points[0].y - start.points[0].y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return distance < m_tapThreshold;
}

bool AndroidTouchHandler::IsLongPress(const TouchEvent& start, const TouchEvent& end) const {
    if (start.points.empty() || end.points.empty()) {
        return false;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end.timestamp - start.timestamp);
    
    float dx = end.points[0].x - start.points[0].x;
    float dy = end.points[0].y - start.points[0].y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return duration >= m_longPressThreshold && distance < m_tapThreshold;
}

bool AndroidTouchHandler::IsSwipe(const TouchEvent& start, const TouchEvent& end) const {
    if (start.points.empty() || end.points.empty()) {
        return false;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end.timestamp - start.timestamp);
    
    if (duration.count() == 0) {
        return false;
    }
    
    float dx = end.points[0].x - start.points[0].x;
    float dy = end.points[0].y - start.points[0].y;
    float distance = std::sqrt(dx * dx + dy * dy);
    float velocity = distance / (duration.count() / 1000.0f);
    
    return velocity > m_swipeThreshold;
}

void AndroidTouchHandler::DetectGestures() {
    if (m_currentEvent.points.size() >= 2) {
        UpdatePinchGesture();
        UpdateRotationGesture();
    }
    UpdatePanGesture();
    
    if (m_gestureCallback) {
        m_gestureCallback(m_gestureState);
    }
}

void AndroidTouchHandler::UpdatePinchGesture() {
    if (m_currentEvent.points.size() < 2 || m_startEvent.points.size() < 2) {
        return;
    }
    
    float currentDistance = CalculateDistance(
        m_currentEvent.points[0].x, m_currentEvent.points[0].y,
        m_currentEvent.points[1].x, m_currentEvent.points[1].y);
    
    float startDistance = CalculateDistance(
        m_startEvent.points[0].x, m_startEvent.points[0].y,
        m_startEvent.points[1].x, m_startEvent.points[1].y);
    
    if (startDistance > 0) {
        m_gestureState.scale = currentDistance / startDistance;
        m_gestureState.isPinching = std::abs(m_gestureState.scale - 1.0f) > 0.01f;
    }
    
    m_gestureState.focusX = (m_currentEvent.points[0].x + m_currentEvent.points[1].x) / 2;
    m_gestureState.focusY = (m_currentEvent.points[0].y + m_currentEvent.points[1].y) / 2;
}

void AndroidTouchHandler::UpdatePanGesture() {
    if (m_currentEvent.points.empty() || m_startEvent.points.empty()) {
        return;
    }
    
    float dx = m_currentEvent.points[0].x - m_startEvent.points[0].x;
    float dy = m_currentEvent.points[0].y - m_startEvent.points[0].y;
    
    m_gestureState.panX = dx;
    m_gestureState.panY = dy;
    m_gestureState.isPanning = std::sqrt(dx * dx + dy * dy) > m_tapThreshold;
}

void AndroidTouchHandler::UpdateRotationGesture() {
    if (m_currentEvent.points.size() < 2 || m_startEvent.points.size() < 2) {
        return;
    }
    
    float currentAngle = CalculateAngle(
        m_currentEvent.points[0].x, m_currentEvent.points[0].y,
        m_currentEvent.points[1].x, m_currentEvent.points[1].y);
    
    float startAngle = CalculateAngle(
        m_startEvent.points[0].x, m_startEvent.points[0].y,
        m_startEvent.points[1].x, m_startEvent.points[1].y);
    
    m_gestureState.rotation = currentAngle - startAngle;
    m_gestureState.isRotating = std::abs(m_gestureState.rotation) > 0.01f;
}

float AndroidTouchHandler::CalculateDistance(float x1, float y1, float x2, float y2) const {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

float AndroidTouchHandler::CalculateAngle(float x1, float y1, float x2, float y2) const {
    return std::atan2(y2 - y1, x2 - x1);
}

}  
}  

#endif
