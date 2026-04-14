#include "ogc/graph/interaction/interaction_handler.h"
#include "ogc/graph/layer/layer_manager.h"
#include <algorithm>
#include <chrono>

namespace ogc {
namespace graph {

struct InteractionHandler::Impl {
    InteractionState state;
    std::map<InteractionEventType, EventCallback> eventCallbacks;
    std::map<InteractionState, EventCallback> stateCallbacks;
    mutable std::mutex mutex;
    
    Impl()
        : state(InteractionState::kNone) {
    }
};

InteractionHandlerPtr InteractionHandler::Create(const std::string& name)
{
    return InteractionHandlerPtr(new InteractionHandler(name));
}

InteractionHandler::InteractionHandler(const std::string& name)
    : m_name(name)
    , m_enabled(true)
    , m_priority(0)
    , impl_(std::make_unique<Impl>())
{
}

InteractionHandler::~InteractionHandler() = default;

InteractionState InteractionHandler::GetState() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->state;
}

void InteractionHandler::SetState(InteractionState state) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->state = state;
}

std::string InteractionHandler::GetName() const {
    return m_name;
}

bool InteractionHandler::HandleEvent(const InteractionEvent& event)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!m_enabled) {
        return false;
    }
    
    auto it = impl_->eventCallbacks.find(event.type);
    if (it != impl_->eventCallbacks.end() && it->second) {
        return it->second(event);
    }
    
    auto stateIt = impl_->stateCallbacks.find(impl_->state);
    if (stateIt != impl_->stateCallbacks.end() && stateIt->second) {
        return stateIt->second(event);
    }
    
    return false;
}

void InteractionHandler::SetEventCallback(InteractionEventType type, EventCallback callback)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->eventCallbacks[type] = callback;
}

void InteractionHandler::ClearEventCallback(InteractionEventType type)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->eventCallbacks.erase(type);
}

void InteractionHandler::SetStateCallback(InteractionState state, EventCallback callback)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->stateCallbacks[state] = callback;
}

void InteractionHandler::ClearStateCallback(InteractionState state)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->stateCallbacks.erase(state);
}

struct InteractionManager::Impl {
    bool initialized;
    InteractionState currentState;
    std::vector<IInteractionHandler*> handlers;
    std::function<Coordinate(double, double)> screenToWorld;
    std::function<Coordinate(double, double)> worldToScreen;
    int viewportWidth;
    int viewportHeight;
    Envelope extent;
    LayerManager* layerManager;
    mutable std::mutex mutex;
    
    Impl()
        : initialized(false)
        , currentState(InteractionState::kNone)
        , viewportWidth(800)
        , viewportHeight(600)
        , layerManager(nullptr) {
    }
};

InteractionManager& InteractionManager::Instance()
{
    static InteractionManager instance;
    return instance;
}

InteractionManager::InteractionManager()
    : impl_(std::make_unique<Impl>())
{
}

InteractionManager::~InteractionManager()
{
    Finalize();
}

bool InteractionManager::IsInitialized() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->initialized;
}

InteractionState InteractionManager::GetCurrentState() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentState;
}

int InteractionManager::GetViewportWidth() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->viewportWidth;
}

int InteractionManager::GetViewportHeight() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->viewportHeight;
}

Envelope InteractionManager::GetExtent() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->extent;
}

LayerManager* InteractionManager::GetLayerManager() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->layerManager;
}

bool InteractionManager::Initialize()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->initialized) {
        return true;
    }
    
    impl_->handlers.clear();
    impl_->currentState = InteractionState::kNone;
    impl_->initialized = true;
    return true;
}

void InteractionManager::Finalize()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized) {
        return;
    }
    
    impl_->handlers.clear();
    impl_->screenToWorld = nullptr;
    impl_->worldToScreen = nullptr;
    impl_->layerManager = nullptr;
    impl_->initialized = false;
}

void InteractionManager::AddHandler(IInteractionHandler* handler)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (handler) {
        auto it = std::find(impl_->handlers.begin(), impl_->handlers.end(), handler);
        if (it == impl_->handlers.end()) {
            impl_->handlers.push_back(handler);
        }
    }
}

void InteractionManager::RemoveHandler(IInteractionHandler* handler)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = std::find(impl_->handlers.begin(), impl_->handlers.end(), handler);
    if (it != impl_->handlers.end()) {
        impl_->handlers.erase(it);
    }
}

void InteractionManager::RemoveAllHandlers()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->handlers.clear();
}

IInteractionHandler* InteractionManager::GetHandler(const std::string& name)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto handler : impl_->handlers) {
        if (handler && handler->GetName() == name) {
            return handler;
        }
    }
    return nullptr;
}

const IInteractionHandler* InteractionManager::GetHandler(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto handler : impl_->handlers) {
        if (handler && handler->GetName() == name) {
            return handler;
        }
    }
    return nullptr;
}

std::vector<IInteractionHandler*> InteractionManager::GetHandlers() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->handlers;
}

std::vector<IInteractionHandler*> InteractionManager::GetHandlersByPriority() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::vector<IInteractionHandler*> sorted = impl_->handlers;
    std::sort(sorted.begin(), sorted.end(), 
              [](const IInteractionHandler* a, const IInteractionHandler* b) {
                  return a->GetPriority() > b->GetPriority();
              });
    return sorted;
}

bool InteractionManager::ProcessEvent(const InteractionEvent& event)
{
    InteractionEvent processedEvent = event;
    
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->screenToWorld && event.type != InteractionEventType::kKeyDown && 
            event.type != InteractionEventType::kKeyUp) {
            Coordinate world = impl_->screenToWorld(event.screenX, event.screenY);
            processedEvent.worldX = world.x;
            processedEvent.worldY = world.y;
        }
    }
    
    auto sortedHandlers = GetHandlersByPriority();
    for (auto handler : sortedHandlers) {
        if (handler && handler->IsEnabled()) {
            if (handler->HandleEvent(processedEvent)) {
                return true;
            }
        }
    }
    
    return false;
}

void InteractionManager::SetCurrentState(InteractionState state)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->currentState = state;
}

void InteractionManager::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->screenToWorld = transform;
}

void InteractionManager::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->worldToScreen = transform;
}

Coordinate InteractionManager::ScreenToWorld(double screenX, double screenY) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->screenToWorld) {
        return impl_->screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate InteractionManager::WorldToScreen(double worldX, double worldY) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->worldToScreen) {
        return impl_->worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void InteractionManager::SetViewportSize(int width, int height)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->viewportWidth = width;
    impl_->viewportHeight = height;
}

void InteractionManager::SetExtent(const Envelope& extent)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->extent = extent;
}

void InteractionManager::SetLayerManager(LayerManager* manager)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->layerManager = manager;
}

}
}
