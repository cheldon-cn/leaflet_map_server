#include "ogc/draw/qt_event_adapter.h"

#ifdef DRAW_HAS_QT

#include <QDateTime>
#include <QDebug>

namespace ogc {
namespace draw {

QtEventAdapter::QtEventAdapter(QObject* parent)
    : QObject(parent)
{
}

QtEventAdapter::~QtEventAdapter() {
}

bool QtEventAdapter::eventFilter(QObject* watched, QEvent* event) {
    if (!m_enabled) {
        return QObject::eventFilter(watched, event);
    }

    switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
            return HandleMouseEvent(static_cast<QMouseEvent*>(event));

        case QEvent::Wheel:
            return HandleWheelEvent(static_cast<QWheelEvent*>(event));

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            return HandleKeyEvent(static_cast<QKeyEvent*>(event));

        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            if (m_touchEnabled) {
                return HandleTouchEvent(static_cast<QTouchEvent*>(event));
            }
            break;

        case QEvent::Gesture:
            if (m_gestureEnabled) {
                return HandleGestureEvent(static_cast<QGestureEvent*>(event));
            }
            break;

        default:
            break;
    }

    return QObject::eventFilter(watched, event);
}

void QtEventAdapter::SetInteractionManager(InteractionManager* manager) {
    m_interactionManager = manager;
}

void QtEventAdapter::SetEventCallback(InteractionEventType type, EventCallback callback) {
    m_callbacks[type] = callback;
}

void QtEventAdapter::ClearEventCallback(InteractionEventType type) {
    m_callbacks.erase(type);
}

void QtEventAdapter::ClearAllCallbacks() {
    m_callbacks.clear();
}

bool QtEventAdapter::HandleMouseEvent(QMouseEvent* event) {
    InteractionEventType eventType = InteractionEventType::kNone;
    
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            eventType = InteractionEventType::kMouseDown;
            m_mousePressPos = event->pos();
            m_pressedButton = event->button();
            break;
        case QEvent::MouseButtonRelease:
            eventType = InteractionEventType::kMouseUp;
            m_pressedButton = Qt::NoButton;
            break;
        case QEvent::MouseMove:
            eventType = InteractionEventType::kMouseMove;
            break;
        case QEvent::MouseButtonDblClick:
            eventType = InteractionEventType::kDoubleClick;
            break;
        default:
            return false;
    }

    auto interactionEvent = ConvertMouseEvent(event, eventType);
    
    emit InteractionEventTriggered(interactionEvent);
    
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            emit MousePressed(event->pos().x(), event->pos().y(), 
                            static_cast<int>(event->button()));
            break;
        case QEvent::MouseButtonRelease:
            emit MouseReleased(event->pos().x(), event->pos().y(), 
                             static_cast<int>(event->button()));
            break;
        case QEvent::MouseMove:
            emit MouseMoved(event->pos().x(), event->pos().y());
            break;
        case QEvent::MouseButtonDblClick:
            emit MouseDoubleClicked(event->pos().x(), event->pos().y(), 
                                   static_cast<int>(event->button()));
            break;
        default:
            break;
    }

    m_lastMousePos = event->pos();

    if (m_interactionManager) {
        return m_interactionManager->ProcessEvent(interactionEvent);
    }

    auto it = m_callbacks.find(eventType);
    if (it != m_callbacks.end() && it->second) {
        return it->second(interactionEvent);
    }

    return false;
}

bool QtEventAdapter::HandleWheelEvent(QWheelEvent* event) {
    auto interactionEvent = ConvertWheelEvent(event);
    
    emit InteractionEventTriggered(interactionEvent);
    emit WheelScrolled(event->pos().x(), event->pos().y(), 
                       event->angleDelta().y() / 120.0);

    if (m_interactionManager) {
        return m_interactionManager->ProcessEvent(interactionEvent);
    }

    auto it = m_callbacks.find(InteractionEventType::kWheel);
    if (it != m_callbacks.end() && it->second) {
        return it->second(interactionEvent);
    }

    return false;
}

bool QtEventAdapter::HandleKeyEvent(QKeyEvent* event) {
    InteractionEventType eventType = (event->type() == QEvent::KeyPress)
        ? InteractionEventType::kKeyDown
        : InteractionEventType::kKeyUp;

    auto interactionEvent = ConvertKeyEvent(event, eventType);
    
    emit InteractionEventTriggered(interactionEvent);
    
    if (event->type() == QEvent::KeyPress) {
        emit KeyPressed(event->key(), static_cast<int>(event->modifiers()));
    } else {
        emit KeyReleased(event->key(), static_cast<int>(event->modifiers()));
    }

    if (m_interactionManager) {
        return m_interactionManager->ProcessEvent(interactionEvent);
    }

    auto it = m_callbacks.find(eventType);
    if (it != m_callbacks.end() && it->second) {
        return it->second(interactionEvent);
    }

    return false;
}

bool QtEventAdapter::HandleTouchEvent(QTouchEvent* event) {
    InteractionEventType eventType = InteractionEventType::kNone;
    
    switch (event->type()) {
        case QEvent::TouchBegin:
            eventType = InteractionEventType::kTouchStart;
            break;
        case QEvent::TouchUpdate:
            eventType = InteractionEventType::kTouchMove;
            break;
        case QEvent::TouchEnd:
            eventType = InteractionEventType::kTouchEnd;
            break;
        default:
            return false;
    }

    auto interactionEvent = ConvertTouchEvent(event, eventType);
    
    emit InteractionEventTriggered(interactionEvent);
    
    std::vector<Coordinate> points;
    const auto& touchPoints = event->touchPoints();
    for (const auto& tp : touchPoints) {
        points.emplace_back(tp.pos().x(), tp.pos().y());
    }
    
    switch (event->type()) {
        case QEvent::TouchBegin:
            emit TouchStarted(points);
            break;
        case QEvent::TouchUpdate:
            emit TouchMoved(points);
            break;
        case QEvent::TouchEnd:
            emit TouchEnded(points);
            break;
        default:
            break;
    }

    if (m_interactionManager) {
        return m_interactionManager->ProcessEvent(interactionEvent);
    }

    auto it = m_callbacks.find(eventType);
    if (it != m_callbacks.end() && it->second) {
        return it->second(interactionEvent);
    }

    return false;
}

bool QtEventAdapter::HandleGestureEvent(QGestureEvent* event) {
    return false;
}

InteractionEvent QtEventAdapter::ConvertMouseEvent(QMouseEvent* event, InteractionEventType type) {
    InteractionEvent ie;
    ie.type = type;
    ie.screenX = event->pos().x();
    ie.screenY = event->pos().y();
    ie.button = ConvertQtMouseButton(event->button());
    ie.modifiers = ConvertQtModifiers(event->modifiers());
    ie.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    return ie;
}

InteractionEvent QtEventAdapter::ConvertWheelEvent(QWheelEvent* event) {
    InteractionEvent ie;
    ie.type = InteractionEventType::kWheel;
    ie.screenX = event->pos().x();
    ie.screenY = event->pos().y();
    ie.wheelDelta = event->angleDelta().y() / 120.0;
    ie.modifiers = ConvertQtModifiers(event->modifiers());
    ie.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    return ie;
}

InteractionEvent QtEventAdapter::ConvertKeyEvent(QKeyEvent* event, InteractionEventType type) {
    InteractionEvent ie;
    ie.type = type;
    ie.keyCode = ConvertQtKey(event->key());
    ie.modifiers = ConvertQtModifiers(event->modifiers());
    ie.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    return ie;
}

InteractionEvent QtEventAdapter::ConvertTouchEvent(QTouchEvent* event, InteractionEventType type) {
    InteractionEvent ie;
    ie.type = type;
    ie.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    const auto& touchPoints = event->touchPoints();
    ie.touchPoints.reserve(touchPoints.size());
    for (const auto& tp : touchPoints) {
        ie.touchPoints.emplace_back(tp.pos().x(), tp.pos().y());
    }
    
    return ie;
}

MouseButton QtEventAdapter::ConvertQtMouseButton(Qt::MouseButton button) {
    switch (button) {
        case Qt::LeftButton: return MouseButton::kLeft;
        case Qt::RightButton: return MouseButton::kRight;
        case Qt::MiddleButton: return MouseButton::kMiddle;
        case Qt::BackButton: return MouseButton::kBack;
        case Qt::ForwardButton: return MouseButton::kForward;
        default: return MouseButton::kNone;
    }
}

KeyModifier QtEventAdapter::ConvertQtModifiers(Qt::KeyboardModifiers modifiers) {
    int result = 0;
    if (modifiers & Qt::ShiftModifier) result |= static_cast<int>(KeyModifier::kShift);
    if (modifiers & Qt::ControlModifier) result |= static_cast<int>(KeyModifier::kCtrl);
    if (modifiers & Qt::AltModifier) result |= static_cast<int>(KeyModifier::kAlt);
    if (modifiers & Qt::MetaModifier) result |= static_cast<int>(KeyModifier::kMeta);
    return static_cast<KeyModifier>(result);
}

int QtEventAdapter::ConvertQtKey(int qtKey) {
    return qtKey;
}

}  
}  

#endif
