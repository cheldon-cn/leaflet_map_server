#include "ogc/draw/pan_zoom_handler.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace draw {

std::unique_ptr<PanZoomHandler> PanZoomHandler::Create(const std::string& name) {
    return std::unique_ptr<PanZoomHandler>(new PanZoomHandler(name));
}

PanZoomHandler::PanZoomHandler(const std::string& name)
    : m_name(name)
{
    m_lastMoveTime = std::chrono::steady_clock::now();
    m_animationStartTime = std::chrono::steady_clock::now();
}

PanZoomHandler::~PanZoomHandler() {
    CancelAnimation();
}

bool PanZoomHandler::HandleEvent(const InteractionEvent& event) {
    if (!m_enabled) {
        return false;
    }
    
    switch (event.type) {
        case InteractionEventType::kMouseDown:
            return HandleMouseDown(event);
        case InteractionEventType::kMouseMove:
            return HandleMouseMove(event);
        case InteractionEventType::kMouseUp:
            return HandleMouseUp(event);
        case InteractionEventType::kMouseWheel:
            return HandleWheel(event);
        case InteractionEventType::kKeyDown:
            return HandleKeyDown(event);
        case InteractionEventType::kTouchBegin:
            return HandleTouchStart(event);
        case InteractionEventType::kTouchMove:
            return HandleTouchMove(event);
        case InteractionEventType::kTouchEnd:
            return HandleTouchEnd(event);
        case InteractionEventType::kMouseDoubleClick:
            return HandleDoubleClick(event);
        default:
            return false;
    }
}

void PanZoomHandler::SetExtent(const Envelope& extent) {
    m_state.extent = extent;
    
    double width = extent.GetWidth();
    double height = extent.GetHeight();
    
    if (width > 0 && height > 0) {
        m_state.centerX = extent.GetCentre().x;
        m_state.centerY = extent.GetCentre().y;
        
        double scaleX = m_viewportWidth / width;
        double scaleY = m_viewportHeight / height;
        m_state.zoom = std::min(scaleX, scaleY) * 0.9;
        m_state.zoom = ClampZoom(m_state.zoom);
    }
    
    ApplyConstraints();
    UpdateExtent();
    
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::SetCenter(double x, double y) {
    auto clamped = ClampCenter(x, y);
    m_state.centerX = clamped.x;
    m_state.centerY = clamped.y;
    
    UpdateExtent();
    
    if (m_centerChangedCallback) {
        m_centerChangedCallback(m_state.centerX, m_state.centerY);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::SetZoom(double zoom) {
    m_state.zoom = ClampZoom(zoom);
    
    UpdateExtent();
    
    if (m_zoomChangedCallback) {
        m_zoomChangedCallback(m_state.zoom);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::SetRotation(double degrees) {
    m_state.rotation = ClampRotation(degrees);
    
    UpdateExtent();
    
    if (m_rotationChangedCallback) {
        m_rotationChangedCallback(m_state.rotation);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::SetConstraints(const Envelope& constraints) {
    m_state.constraints = constraints;
    m_state.hasConstraints = true;
    ApplyConstraints();
}

void PanZoomHandler::ClearConstraints() {
    m_state.hasConstraints = false;
}

void PanZoomHandler::Pan(double dx, double dy) {
    double worldDx = dx / m_state.zoom;
    double worldDy = dy / m_state.zoom;
    
    SetCenter(m_state.centerX - worldDx, m_state.centerY + worldDy);
}

void PanZoomHandler::PanTo(double x, double y) {
    SetCenter(x, y);
}

void PanZoomHandler::ZoomIn(double factor) {
    double newZoom = m_state.zoom * factor;
    SetZoom(newZoom);
}

void PanZoomHandler::ZoomOut(double factor) {
    double newZoom = m_state.zoom / factor;
    SetZoom(newZoom);
}

void PanZoomHandler::ZoomTo(double zoom) {
    SetZoom(zoom);
}

void PanZoomHandler::ZoomToPoint(double screenX, double screenY, double factor) {
    Coordinate worldBefore = ScreenToWorld(screenX, screenY);
    
    double newZoom = m_state.zoom * factor;
    newZoom = ClampZoom(newZoom);
    
    m_state.zoom = newZoom;
    
    Coordinate worldAfter = ScreenToWorld(screenX, screenY);
    
    double dx = worldAfter.x - worldBefore.x;
    double dy = worldAfter.y - worldBefore.y;
    
    m_state.centerX -= dx;
    m_state.centerY -= dy;
    
    ApplyConstraints();
    UpdateExtent();
    
    if (m_zoomChangedCallback) {
        m_zoomChangedCallback(m_state.zoom);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::Rotate(double degrees) {
    SetRotation(m_state.rotation + degrees);
}

void PanZoomHandler::RotateTo(double degrees) {
    SetRotation(degrees);
}

void PanZoomHandler::ResetRotation() {
    SetRotation(0.0);
}

void PanZoomHandler::SetViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    UpdateExtent();
}

void PanZoomHandler::SetExtentChangedCallback(ExtentChangedCallback callback) {
    m_extentChangedCallback = callback;
}

void PanZoomHandler::SetZoomChangedCallback(ZoomChangedCallback callback) {
    m_zoomChangedCallback = callback;
}

void PanZoomHandler::SetCenterChangedCallback(CenterChangedCallback callback) {
    m_centerChangedCallback = callback;
}

void PanZoomHandler::SetRotationChangedCallback(RotationChangedCallback callback) {
    m_rotationChangedCallback = callback;
}

void PanZoomHandler::UpdateAnimation() {
    if (m_isPanning && m_inertia.enabled) {
        UpdateInertia();
    }
    
    if (m_isZooming && m_animationEnabled) {
        UpdateZoomAnimation();
    }
}

void PanZoomHandler::CancelAnimation() {
    m_isAnimating = false;
    m_isZooming = false;
    m_velocityX = 0.0;
    m_velocityY = 0.0;
}

void PanZoomHandler::FitToExtent(const Envelope& extent) {
    if (extent.IsNull()) {
        return;
    }
    
    double width = extent.GetWidth();
    double height = extent.GetHeight();
    
    if (width <= 0 || height <= 0) {
        return;
    }
    
    double scaleX = m_viewportWidth / width;
    double scaleY = m_viewportHeight / height;
    double scale = std::min(scaleX, scaleY) * 0.9;
    
    m_state.zoom = ClampZoom(scale);
    m_state.centerX = extent.GetCentre().x;
    m_state.centerY = extent.GetCentre().y;
    
    ApplyConstraints();
    UpdateExtent();
    
    if (m_zoomChangedCallback) {
        m_zoomChangedCallback(m_state.zoom);
    }
    if (m_centerChangedCallback) {
        m_centerChangedCallback(m_state.centerX, m_state.centerY);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

void PanZoomHandler::FitToAll() {
    if (m_state.hasConstraints) {
        FitToExtent(m_state.constraints);
    }
}

void PanZoomHandler::SetPanZoomState(const PanZoomState& state) {
    m_state = state;
    m_state.zoom = ClampZoom(m_state.zoom);
    m_state.rotation = ClampRotation(m_state.rotation);
    ApplyConstraints();
    UpdateExtent();
}

void PanZoomHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    m_screenToWorld = transform;
}

void PanZoomHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    m_worldToScreen = transform;
}

Coordinate PanZoomHandler::ScreenToWorld(double screenX, double screenY) const {
    if (m_screenToWorld) {
        return m_screenToWorld(screenX, screenY);
    }
    
    double x = m_state.centerX + (screenX - m_viewportWidth / 2.0) / m_state.zoom;
    double y = m_state.centerY - (screenY - m_viewportHeight / 2.0) / m_state.zoom;
    return Coordinate(x, y);
}

Coordinate PanZoomHandler::WorldToScreen(double worldX, double worldY) const {
    if (m_worldToScreen) {
        return m_worldToScreen(worldX, worldY);
    }
    
    double x = (worldX - m_state.centerX) * m_state.zoom + m_viewportWidth / 2.0;
    double y = m_viewportHeight / 2.0 - (worldY - m_state.centerY) * m_state.zoom;
    return Coordinate(x, y);
}

void PanZoomHandler::UpdateExtent() {
    double halfWidth = m_viewportWidth / (2.0 * m_state.zoom);
    double halfHeight = m_viewportHeight / (2.0 * m_state.zoom);
    
    m_state.extent = Envelope(
        m_state.centerX - halfWidth,
        m_state.centerY - halfHeight,
        m_state.centerX + halfWidth,
        m_state.centerY + halfHeight
    );
}

void PanZoomHandler::ApplyConstraints() {
    if (!m_state.hasConstraints) {
        return;
    }
    
    auto clamped = ClampCenter(m_state.centerX, m_state.centerY);
    m_state.centerX = clamped.x;
    m_state.centerY = clamped.y;
}

void PanZoomHandler::StartInertia(double velocityX, double velocityY) {
    m_velocityX = velocityX;
    m_velocityY = velocityY;
    m_isPanning = true;
    m_isAnimating = true;
}

void PanZoomHandler::UpdateInertia() {
    if (!m_isAnimating) {
        return;
    }
    
    if (std::abs(m_velocityX) < m_inertia.velocityThreshold && 
        std::abs(m_velocityY) < m_inertia.velocityThreshold) {
        m_velocityX = 0.0;
        m_velocityY = 0.0;
        m_isAnimating = false;
        return;
    }
    
    Pan(-m_velocityX, m_velocityY);
    
    m_velocityX *= m_inertia.friction;
    m_velocityY *= m_inertia.friction;
}

void PanZoomHandler::StartZoomAnimation(double targetZoom, double screenX, double screenY) {
    m_targetZoom = ClampZoom(targetZoom);
    m_startZoom = m_state.zoom;
    m_zoomCenterX = screenX;
    m_zoomCenterY = screenY;
    m_animationStartTime = std::chrono::steady_clock::now();
    m_isZooming = true;
    m_isAnimating = true;
}

void PanZoomHandler::UpdateZoomAnimation() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_animationStartTime).count();
    
    if (elapsed >= m_animationDurationMs) {
        m_state.zoom = m_targetZoom;
        m_isZooming = false;
        m_isAnimating = false;
        UpdateExtent();
        
        if (m_zoomChangedCallback) {
            m_zoomChangedCallback(m_state.zoom);
        }
        if (m_extentChangedCallback) {
            m_extentChangedCallback(m_state.extent);
        }
        return;
    }
    
    double t = static_cast<double>(elapsed) / m_animationDurationMs;
    t = t * t * (3 - 2 * t);
    
    double currentZoom = m_startZoom + (m_targetZoom - m_startZoom) * t;
    
    Coordinate worldBefore = ScreenToWorld(m_zoomCenterX, m_zoomCenterY);
    m_state.zoom = currentZoom;
    Coordinate worldAfter = ScreenToWorld(m_zoomCenterX, m_zoomCenterY);
    
    m_state.centerX -= (worldAfter.x - worldBefore.x);
    m_state.centerY -= (worldAfter.y - worldBefore.y);
    
    ApplyConstraints();
    UpdateExtent();
    
    if (m_zoomChangedCallback) {
        m_zoomChangedCallback(m_state.zoom);
    }
    if (m_extentChangedCallback) {
        m_extentChangedCallback(m_state.extent);
    }
}

bool PanZoomHandler::HandleMouseDown(const InteractionEvent& event) {
    if (event.button == MouseButton::kLeft || event.button == MouseButton::kMiddle) {
        m_isPanning = true;
        m_panStartX = event.screenX;
        m_panStartY = event.screenY;
        m_lastMouseX = event.screenX;
        m_lastMouseY = event.screenY;
        m_velocityX = 0.0;
        m_velocityY = 0.0;
        m_lastMoveTime = std::chrono::steady_clock::now();
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleMouseMove(const InteractionEvent& event) {
    if (m_isPanning) {
        double dx = event.screenX - m_lastMouseX;
        double dy = event.screenY - m_lastMouseY;
        
        Pan(dx, dy);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_lastMoveTime).count();
        
        if (elapsed > 0) {
            m_velocityX = dx / elapsed * 16.0;
            m_velocityY = dy / elapsed * 16.0;
        }
        
        m_lastMouseX = event.screenX;
        m_lastMouseY = event.screenY;
        m_lastMoveTime = now;
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleMouseUp(const InteractionEvent& event) {
    if (m_isPanning && (event.button == MouseButton::kLeft || event.button == MouseButton::kMiddle)) {
        m_isPanning = false;
        
        if (m_inertia.enabled && 
            (std::abs(m_velocityX) > m_inertia.velocityThreshold || 
             std::abs(m_velocityY) > m_inertia.velocityThreshold)) {
            m_isAnimating = true;
        }
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleWheel(const InteractionEvent& event) {
    double factor = event.delta > 0 ? m_zoomParams.factor : 1.0 / m_zoomParams.factor;
    
    if (m_zoomParams.animate && m_animationEnabled) {
        StartZoomAnimation(m_state.zoom * factor, event.screenX, event.screenY);
    } else {
        ZoomToPoint(event.screenX, event.screenY, factor);
    }
    
    return true;
}

bool PanZoomHandler::HandleKeyDown(const InteractionEvent& event) {
    switch (event.key) {
        case '+':
        case '=':
            ZoomIn();
            return true;
        case '-':
            ZoomOut();
            return true;
        case 37:  // Left
            Pan(m_viewportWidth * 0.1, 0);
            return true;
        case 38:  // Up
            Pan(0, m_viewportHeight * 0.1);
            return true;
        case 39:  // Right
            Pan(-m_viewportWidth * 0.1, 0);
            return true;
        case 40:  // Down
            Pan(0, -m_viewportHeight * 0.1);
            return true;
        default:
            return false;
    }
}

bool PanZoomHandler::HandleTouchStart(const InteractionEvent& event) {
    if (event.touchPositions.size() == 1) {
        m_isPanning = true;
        m_lastMouseX = event.touchPositions[0].x;
        m_lastMouseY = event.touchPositions[0].y;
        m_velocityX = 0.0;
        m_velocityY = 0.0;
        m_lastMoveTime = std::chrono::steady_clock::now();
        return true;
    } else if (event.touchPositions.size() == 2) {
        m_isPinching = true;
        m_isPanning = false;
        
        double dx = event.touchPositions[1].x - event.touchPositions[0].x;
        double dy = event.touchPositions[1].y - event.touchPositions[0].y;
        m_initialPinchDistance = std::sqrt(dx * dx + dy * dy);
        m_initialPinchZoom = m_state.zoom;
        
        m_initialPinchCenter = Coordinate(
            (event.touchPositions[0].x + event.touchPositions[1].x) / 2.0,
            (event.touchPositions[0].y + event.touchPositions[1].y) / 2.0
        );
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleTouchMove(const InteractionEvent& event) {
    if (m_isPanning && event.touchPositions.size() == 1) {
        double dx = event.touchPositions[0].x - m_lastMouseX;
        double dy = event.touchPositions[0].y - m_lastMouseY;
        
        Pan(dx, dy);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_lastMoveTime).count();
        
        if (elapsed > 0) {
            m_velocityX = dx / elapsed * 16.0;
            m_velocityY = dy / elapsed * 16.0;
        }
        
        m_lastMouseX = event.touchPositions[0].x;
        m_lastMouseY = event.touchPositions[0].y;
        m_lastMoveTime = now;
        
        return true;
    } else if (m_isPinching && event.touchPositions.size() == 2) {
        double dx = event.touchPositions[1].x - event.touchPositions[0].x;
        double dy = event.touchPositions[1].y - event.touchPositions[0].y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        if (m_initialPinchDistance > 0) {
            double scale = distance / m_initialPinchDistance;
            double newZoom = m_initialPinchZoom * scale;
            newZoom = ClampZoom(newZoom);
            
            ZoomToPoint(m_initialPinchCenter.x, m_initialPinchCenter.y, newZoom / m_state.zoom);
        }
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleTouchEnd(const InteractionEvent& event) {
    if (m_isPanning) {
        m_isPanning = false;
        
        if (m_inertia.enabled && 
            (std::abs(m_velocityX) > m_inertia.velocityThreshold || 
             std::abs(m_velocityY) > m_inertia.velocityThreshold)) {
            m_isAnimating = true;
        }
        
        return true;
    } else if (m_isPinching) {
        m_isPinching = false;
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleDoubleClick(const InteractionEvent& event) {
    if (event.button == MouseButton::kLeft) {
        ZoomToPoint(event.screenX, event.screenY, m_zoomParams.factor);
        return true;
    }
    return false;
}

double PanZoomHandler::ClampZoom(double zoom) const {
    return std::max(m_state.minZoom, std::min(m_state.maxZoom, zoom));
}

double PanZoomHandler::ClampRotation(double rotation) const {
    while (rotation > 180.0) rotation -= 360.0;
    while (rotation < -180.0) rotation += 360.0;
    return rotation;
}

Coordinate PanZoomHandler::ClampCenter(double x, double y) const {
    if (!m_state.hasConstraints) {
        return Coordinate(x, y);
    }
    
    double halfWidth = m_viewportWidth / (2.0 * m_state.zoom);
    double halfHeight = m_viewportHeight / (2.0 * m_state.zoom);
    
    double minX = m_state.constraints.GetMinX() + halfWidth;
    double maxX = m_state.constraints.GetMaxX() - halfWidth;
    double minY = m_state.constraints.GetMinY() + halfHeight;
    double maxY = m_state.constraints.GetMaxY() - halfHeight;
    
    if (minX > maxX) {
        x = (minX + maxX) / 2.0;
    } else {
        x = std::max(minX, std::min(maxX, x));
    }
    
    if (minY > maxY) {
        y = (minY + maxY) / 2.0;
    } else {
        y = std::max(minY, std::min(maxY, y));
    }
    
    return Coordinate(x, y);
}

}  
}  
