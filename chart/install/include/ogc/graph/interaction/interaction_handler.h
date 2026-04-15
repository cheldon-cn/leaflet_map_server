#ifndef OGC_GRAPH_INTERACTION_HANDLER_H
#define OGC_GRAPH_INTERACTION_HANDLER_H

#include "ogc/graph/export.h"
#include <ogc/draw/draw_result.h>
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <map>

namespace ogc {
namespace graph {

class DrawContext;
class LayerManager;
class HitTestResult;

enum class MouseButton {
    kNone = 0,
    kLeft = 1,
    kRight = 2,
    kMiddle = 3
};

enum class InteractionState {
    kNone = 0,
    kPan,
    kZoom,
    kSelect,
    kMeasure,
    kEdit
};

enum class InteractionEventType {
    kNone = 0,
    kMouseDown,
    kMouseUp,
    kMouseMove,
    kMouseWheel,
    kMouseDoubleClick,
    kKeyDown,
    kKeyUp,
    kTouchBegin,
    kTouchMove,
    kTouchEnd,
    kGesturePinch,
    kGesturePan
};

struct InteractionEvent {
    InteractionEventType type = InteractionEventType::kNone;
    double screenX = 0.0;
    double screenY = 0.0;
    double worldX = 0.0;
    double worldY = 0.0;
    MouseButton button = MouseButton::kNone;
    int modifiers = 0;
    double delta = 0.0;
    double scale = 1.0;
    double rotation = 0.0;
    int touchPoints = 0;
    std::vector<Coordinate> touchPositions;
    int key = 0;
    bool isPressed = false;
    bool isShift = false;
    bool isCtrl = false;
    bool isAlt = false;
    bool handled = false;
};

class OGC_GRAPH_API IInteractionHandler {
public:
    virtual ~IInteractionHandler() = default;
    
    virtual bool HandleEvent(const InteractionEvent& event) = 0;
    
    virtual InteractionState GetState() const = 0;
    
    virtual void SetEnabled(bool enabled) = 0;
    virtual bool IsEnabled() const = 0;
    
    virtual void SetPriority(int priority) = 0;
    virtual int GetPriority() const = 0;
    
    virtual std::string GetName() const = 0;
};

class InteractionHandler;
using InteractionHandlerPtr = std::shared_ptr<InteractionHandler>;

class OGC_GRAPH_API InteractionHandler : public IInteractionHandler {
public:
    using EventCallback = std::function<bool(const InteractionEvent&)>;
    
    ~InteractionHandler();
    
    static InteractionHandlerPtr Create(const std::string& name);
    
    bool HandleEvent(const InteractionEvent& event) override;
    
    InteractionState GetState() const override;
    void SetState(InteractionState state);
    
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    bool IsEnabled() const override { return m_enabled; }
    
    void SetPriority(int priority) override { m_priority = priority; }
    int GetPriority() const override { return m_priority; }
    
    std::string GetName() const override;
    
    void SetEventCallback(InteractionEventType type, EventCallback callback);
    void ClearEventCallback(InteractionEventType type);
    
    void SetStateCallback(InteractionState state, EventCallback callback);
    void ClearStateCallback(InteractionState state);
    
protected:
    explicit InteractionHandler(const std::string& name);
    
private:
    std::string m_name;
    bool m_enabled;
    int m_priority;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class InteractionManager;
using InteractionManagerPtr = std::shared_ptr<InteractionManager>;

class OGC_GRAPH_API InteractionManager {
public:
    static InteractionManager& Instance();
    
    bool Initialize();
    void Finalize();
    bool IsInitialized() const;
    
    void AddHandler(IInteractionHandler* handler);
    void RemoveHandler(IInteractionHandler* handler);
    void RemoveAllHandlers();
    
    IInteractionHandler* GetHandler(const std::string& name);
    const IInteractionHandler* GetHandler(const std::string& name) const;
    
    std::vector<IInteractionHandler*> GetHandlers() const;
    std::vector<IInteractionHandler*> GetHandlersByPriority() const;
    
    bool ProcessEvent(const InteractionEvent& event);
    
    void SetCurrentState(InteractionState state);
    InteractionState GetCurrentState() const;
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    
    Coordinate ScreenToWorld(double screenX, double screenY) const;
    Coordinate WorldToScreen(double worldX, double worldY) const;
    
    void SetViewportSize(int width, int height);
    int GetViewportWidth() const;
    int GetViewportHeight() const;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    void SetLayerManager(LayerManager* manager);
    LayerManager* GetLayerManager() const;
    
private:
    InteractionManager();
    ~InteractionManager();
    
    InteractionManager(const InteractionManager&) = delete;
    InteractionManager& operator=(const InteractionManager&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}

#endif
