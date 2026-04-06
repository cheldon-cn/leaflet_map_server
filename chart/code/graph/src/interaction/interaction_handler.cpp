#include "ogc/graph/interaction/interaction_handler.h"
#include "ogc/graph/layer/layer_manager.h"
#include <algorithm>
#include <chrono>

namespace ogc {
namespace graph {

InteractionHandlerPtr InteractionHandler::Create(const std::string& name)
{
    return InteractionHandlerPtr(new InteractionHandler(name));
}

InteractionHandler::InteractionHandler(const std::string& name)
    : m_name(name)
    , m_state(InteractionState::kNone)
    , m_enabled(true)
    , m_priority(0)
{
}

bool InteractionHandler::HandleEvent(const InteractionEvent& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled) {
        return false;
    }
    
    auto it = m_eventCallbacks.find(event.type);
    if (it != m_eventCallbacks.end() && it->second) {
        return it->second(event);
    }
    
    auto stateIt = m_stateCallbacks.find(m_state);
    if (stateIt != m_stateCallbacks.end() && stateIt->second) {
        return stateIt->second(event);
    }
    
    return false;
}

void InteractionHandler::SetEventCallback(InteractionEventType type, EventCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eventCallbacks[type] = callback;
}

void InteractionHandler::ClearEventCallback(InteractionEventType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eventCallbacks.erase(type);
}

void InteractionHandler::SetStateCallback(InteractionState state, EventCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stateCallbacks[state] = callback;
}

void InteractionHandler::ClearStateCallback(InteractionState state)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stateCallbacks.erase(state);
}

InteractionManager& InteractionManager::Instance()
{
    static InteractionManager instance;
    return instance;
}

InteractionManager::InteractionManager()
    : m_initialized(false)
    , m_currentState(InteractionState::kNone)
    , m_viewportWidth(800)
    , m_viewportHeight(600)
    , m_layerManager(nullptr)
{
}

InteractionManager::~InteractionManager()
{
    Finalize();
}

bool InteractionManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return true;
    }
    
    m_handlers.clear();
    m_currentState = InteractionState::kNone;
    m_initialized = true;
    return true;
}

void InteractionManager::Finalize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) {
        return;
    }
    
    m_handlers.clear();
    m_screenToWorld = nullptr;
    m_worldToScreen = nullptr;
    m_layerManager = nullptr;
    m_initialized = false;
}

void InteractionManager::AddHandler(IInteractionHandler* handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (handler) {
        auto it = std::find(m_handlers.begin(), m_handlers.end(), handler);
        if (it == m_handlers.end()) {
            m_handlers.push_back(handler);
        }
    }
}

void InteractionManager::RemoveHandler(IInteractionHandler* handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_handlers.begin(), m_handlers.end(), handler);
    if (it != m_handlers.end()) {
        m_handlers.erase(it);
    }
}

void InteractionManager::RemoveAllHandlers()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handlers.clear();
}

IInteractionHandler* InteractionManager::GetHandler(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto handler : m_handlers) {
        if (handler && handler->GetName() == name) {
            return handler;
        }
    }
    return nullptr;
}

const IInteractionHandler* InteractionManager::GetHandler(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto handler : m_handlers) {
        if (handler && handler->GetName() == name) {
            return handler;
        }
    }
    return nullptr;
}

std::vector<IInteractionHandler*> InteractionManager::GetHandlers() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handlers;
}

std::vector<IInteractionHandler*> InteractionManager::GetHandlersByPriority() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<IInteractionHandler*> sorted = m_handlers;
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
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_screenToWorld && event.type != InteractionEventType::kKeyDown && 
            event.type != InteractionEventType::kKeyUp) {
            Coordinate world = m_screenToWorld(event.screenX, event.screenY);
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
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentState = state;
}

void InteractionManager::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_screenToWorld = transform;
}

void InteractionManager::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_worldToScreen = transform;
}

Coordinate InteractionManager::ScreenToWorld(double screenX, double screenY) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_screenToWorld) {
        return m_screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate InteractionManager::WorldToScreen(double worldX, double worldY) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_worldToScreen) {
        return m_worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void InteractionManager::SetViewportSize(int width, int height)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void InteractionManager::SetExtent(const Envelope& extent)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_extent = extent;
}

void InteractionManager::SetLayerManager(LayerManager* manager)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_layerManager = manager;
}

}
}
