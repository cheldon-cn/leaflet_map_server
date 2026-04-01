#ifndef OGC_DRAW_QT_EVENT_ADAPTER_H
#define OGC_DRAW_QT_EVENT_ADAPTER_H

#include "ogc/draw/export.h"
#include "ogc/draw/interaction_handler.h"

#ifdef DRAW_HAS_QT

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTouchEvent>
#include <QGestureEvent>
#include <functional>
#include <map>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtEventAdapter : public QObject {
    Q_OBJECT

public:
    using EventCallback = std::function<bool(const InteractionEvent&)>;

    explicit QtEventAdapter(QObject* parent = nullptr);
    ~QtEventAdapter() override;

    bool eventFilter(QObject* watched, QEvent* event) override;

    void SetInteractionManager(InteractionManager* manager);
    InteractionManager* GetInteractionManager() const { return m_interactionManager; }

    void SetEventCallback(InteractionEventType type, EventCallback callback);
    void ClearEventCallback(InteractionEventType type);
    void ClearAllCallbacks();

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

    void SetMouseTracking(bool tracking) { m_mouseTracking = tracking; }
    bool IsMouseTracking() const { return m_mouseTracking; }

    void SetTouchEnabled(bool enabled) { m_touchEnabled = enabled; }
    bool IsTouchEnabled() const { return m_touchEnabled; }

    void SetGestureEnabled(bool enabled) { m_gestureEnabled = enabled; }
    bool IsGestureEnabled() const { return m_gestureEnabled; }

    int GetClickCount() const { return m_clickCount; }
    void ResetClickCount() { m_clickCount = 0; }

signals:
    void InteractionEventTriggered(const InteractionEvent& event);
    void MousePressed(double x, double y, int button);
    void MouseReleased(double x, double y, int button);
    void MouseMoved(double x, double y);
    void MouseDoubleClicked(double x, double y, int button);
    void WheelScrolled(double x, double y, double delta);
    void KeyPressed(int key, int modifiers);
    void KeyReleased(int key, int modifiers);
    void TouchStarted(const std::vector<Coordinate>& points);
    void TouchMoved(const std::vector<Coordinate>& points);
    void TouchEnded(const std::vector<Coordinate>& points);

protected:
    bool HandleMouseEvent(QMouseEvent* event);
    bool HandleWheelEvent(QWheelEvent* event);
    bool HandleKeyEvent(QKeyEvent* event);
    bool HandleTouchEvent(QTouchEvent* event);
    bool HandleGestureEvent(QGestureEvent* event);

    InteractionEvent ConvertMouseEvent(QMouseEvent* event, InteractionEventType type);
    InteractionEvent ConvertWheelEvent(QWheelEvent* event);
    InteractionEvent ConvertKeyEvent(QKeyEvent* event, InteractionEventType type);
    InteractionEvent ConvertTouchEvent(QTouchEvent* event, InteractionEventType type);

    MouseButton ConvertQtMouseButton(Qt::MouseButton button);
    KeyModifier ConvertQtModifiers(Qt::KeyboardModifiers modifiers);
    int ConvertQtKey(int qtKey);

private:
    InteractionManager* m_interactionManager = nullptr;
    std::map<InteractionEventType, EventCallback> m_callbacks;
    
    bool m_enabled = true;
    bool m_mouseTracking = false;
    bool m_touchEnabled = true;
    bool m_gestureEnabled = true;
    
    QPoint m_lastMousePos;
    QPoint m_mousePressPos;
    Qt::MouseButton m_pressedButton = Qt::NoButton;
    int m_clickCount = 0;
    ulong m_lastClickTime = 0;
    static const int kDoubleClickInterval = 400;
};

}  
}  

#endif

#endif
