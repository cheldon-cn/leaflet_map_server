#include "ogc/graph/interaction/pan_zoom_handler.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace graph {

struct PanZoomHandler::Impl {
    std::string name;
    int priority = 100;
    bool enabled = true;
    
    PanZoomState state;
    InertiaParams inertia;
    ZoomParams zoomParams;
    
    int viewportWidth = 800;
    int viewportHeight = 600;
    
    bool isPanning = false;
    bool isZooming = false;
    bool isAnimating = false;
    bool animationEnabled = true;
    
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    double panStartX = 0.0;
    double panStartY = 0.0;
    
    double velocityX = 0.0;
    double velocityY = 0.0;
    std::chrono::steady_clock::time_point lastMoveTime;
    
    double targetZoom = 1.0;
    double startZoom = 1.0;
    double zoomCenterX = 0.0;
    double zoomCenterY = 0.0;
    std::chrono::steady_clock::time_point animationStartTime;
    int animationDurationMs = 200;
    
    double initialPinchDistance = 0.0;
    double initialPinchZoom = 1.0;
    Coordinate initialPinchCenter;
    bool isPinching = false;
    
    ExtentChangedCallback extentChangedCallback;
    ZoomChangedCallback zoomChangedCallback;
    CenterChangedCallback centerChangedCallback;
    RotationChangedCallback rotationChangedCallback;
    
    std::function<Coordinate(double, double)> screenToWorld;
    std::function<Coordinate(double, double)> worldToScreen;
    
    Impl(const std::string& n) : name(n) {
        lastMoveTime = std::chrono::steady_clock::now();
        animationStartTime = std::chrono::steady_clock::now();
    }
};

std::unique_ptr<PanZoomHandler> PanZoomHandler::Create(const std::string& name) {
    return std::unique_ptr<PanZoomHandler>(new PanZoomHandler(name));
}

PanZoomHandler::PanZoomHandler(const std::string& name)
    : impl_(std::make_unique<Impl>(name))
{
}

PanZoomHandler::~PanZoomHandler() {
    CancelAnimation();
}

std::string PanZoomHandler::GetName() const { return impl_->name; }
int PanZoomHandler::GetPriority() const { return impl_->priority; }
void PanZoomHandler::SetPriority(int priority) { impl_->priority = priority; }
bool PanZoomHandler::IsEnabled() const { return impl_->enabled; }
void PanZoomHandler::SetEnabled(bool enabled) { impl_->enabled = enabled; }
Envelope PanZoomHandler::GetExtent() const { return impl_->state.extent; }
Coordinate PanZoomHandler::GetCenter() const { return Coordinate(impl_->state.centerX, impl_->state.centerY); }
double PanZoomHandler::GetZoom() const { return impl_->state.zoom; }
double PanZoomHandler::GetRotation() const { return impl_->state.rotation; }
bool PanZoomHandler::HasConstraints() const { return impl_->state.hasConstraints; }
void PanZoomHandler::SetMinZoom(double minZoom) { impl_->state.minZoom = minZoom; }
double PanZoomHandler::GetMinZoom() const { return impl_->state.minZoom; }
void PanZoomHandler::SetMaxZoom(double maxZoom) { impl_->state.maxZoom = maxZoom; }
double PanZoomHandler::GetMaxZoom() const { return impl_->state.maxZoom; }
int PanZoomHandler::GetViewportWidth() const { return impl_->viewportWidth; }
int PanZoomHandler::GetViewportHeight() const { return impl_->viewportHeight; }
void PanZoomHandler::SetInertiaEnabled(bool enabled) { impl_->inertia.enabled = enabled; }
bool PanZoomHandler::IsInertiaEnabled() const { return impl_->inertia.enabled; }
void PanZoomHandler::SetInertiaFriction(double friction) { impl_->inertia.friction = friction; }
double PanZoomHandler::GetInertiaFriction() const { return impl_->inertia.friction; }
void PanZoomHandler::SetAnimationEnabled(bool enabled) { impl_->animationEnabled = enabled; }
bool PanZoomHandler::IsAnimationEnabled() const { return impl_->animationEnabled; }
void PanZoomHandler::SetZoomParams(const ZoomParams& params) { impl_->zoomParams = params; }
ZoomParams PanZoomHandler::GetZoomParams() const { return impl_->zoomParams; }
bool PanZoomHandler::IsAnimating() const { return impl_->isAnimating; }
InteractionState PanZoomHandler::GetState() const { return impl_->isPanning ? InteractionState::kPan : InteractionState::kNone; }
const PanZoomState& PanZoomHandler::GetPanZoomState() const { return impl_->state; }

bool PanZoomHandler::HandleEvent(const InteractionEvent& event) {
    if (!impl_->enabled) {
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
    impl_->state.extent = extent;
    
    double width = extent.GetWidth();
    double height = extent.GetHeight();
    
    if (width > 0 && height > 0) {
        impl_->state.centerX = extent.GetCentre().x;
        impl_->state.centerY = extent.GetCentre().y;
        
        double scaleX = impl_->viewportWidth / width;
        double scaleY = impl_->viewportHeight / height;
        impl_->state.zoom = std::min(scaleX, scaleY) * 0.9;
        impl_->state.zoom = ClampZoom(impl_->state.zoom);
    }
    
    ApplyConstraints();
    UpdateExtent();
    
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::SetCenter(double x, double y) {
    auto clamped = ClampCenter(x, y);
    impl_->state.centerX = clamped.x;
    impl_->state.centerY = clamped.y;
    
    UpdateExtent();
    
    if (impl_->centerChangedCallback) {
        impl_->centerChangedCallback(impl_->state.centerX, impl_->state.centerY);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::SetZoom(double zoom) {
    impl_->state.zoom = ClampZoom(zoom);
    
    UpdateExtent();
    
    if (impl_->zoomChangedCallback) {
        impl_->zoomChangedCallback(impl_->state.zoom);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::SetRotation(double degrees) {
    impl_->state.rotation = ClampRotation(degrees);
    
    UpdateExtent();
    
    if (impl_->rotationChangedCallback) {
        impl_->rotationChangedCallback(impl_->state.rotation);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::SetConstraints(const Envelope& constraints) {
    impl_->state.constraints = constraints;
    impl_->state.hasConstraints = true;
    ApplyConstraints();
}

void PanZoomHandler::ClearConstraints() {
    impl_->state.hasConstraints = false;
}

void PanZoomHandler::Pan(double dx, double dy) {
    double worldDx = dx / impl_->state.zoom;
    double worldDy = dy / impl_->state.zoom;
    
    SetCenter(impl_->state.centerX - worldDx, impl_->state.centerY + worldDy);
}

void PanZoomHandler::PanTo(double x, double y) {
    SetCenter(x, y);
}

void PanZoomHandler::ZoomIn(double factor) {
    double newZoom = impl_->state.zoom * factor;
    SetZoom(newZoom);
}

void PanZoomHandler::ZoomOut(double factor) {
    double newZoom = impl_->state.zoom / factor;
    SetZoom(newZoom);
}

void PanZoomHandler::ZoomTo(double zoom) {
    SetZoom(zoom);
}

void PanZoomHandler::ZoomToPoint(double screenX, double screenY, double factor) {
    Coordinate worldBefore = ScreenToWorld(screenX, screenY);
    
    double newZoom = impl_->state.zoom * factor;
    newZoom = ClampZoom(newZoom);
    
    impl_->state.zoom = newZoom;
    
    Coordinate worldAfter = ScreenToWorld(screenX, screenY);
    
    double dx = worldAfter.x - worldBefore.x;
    double dy = worldAfter.y - worldBefore.y;
    
    impl_->state.centerX -= dx;
    impl_->state.centerY -= dy;
    
    ApplyConstraints();
    UpdateExtent();
    
    if (impl_->zoomChangedCallback) {
        impl_->zoomChangedCallback(impl_->state.zoom);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::Rotate(double degrees) {
    SetRotation(impl_->state.rotation + degrees);
}

void PanZoomHandler::RotateTo(double degrees) {
    SetRotation(degrees);
}

void PanZoomHandler::ResetRotation() {
    SetRotation(0.0);
}

void PanZoomHandler::SetViewportSize(int width, int height) {
    impl_->viewportWidth = width;
    impl_->viewportHeight = height;
    UpdateExtent();
}

void PanZoomHandler::SetExtentChangedCallback(ExtentChangedCallback callback) {
    impl_->extentChangedCallback = callback;
}

void PanZoomHandler::SetZoomChangedCallback(ZoomChangedCallback callback) {
    impl_->zoomChangedCallback = callback;
}

void PanZoomHandler::SetCenterChangedCallback(CenterChangedCallback callback) {
    impl_->centerChangedCallback = callback;
}

void PanZoomHandler::SetRotationChangedCallback(RotationChangedCallback callback) {
    impl_->rotationChangedCallback = callback;
}

void PanZoomHandler::UpdateAnimation() {
    if (impl_->isPanning && impl_->inertia.enabled) {
        UpdateInertia();
    }
    
    if (impl_->isZooming && impl_->animationEnabled) {
        UpdateZoomAnimation();
    }
}

void PanZoomHandler::CancelAnimation() {
    impl_->isAnimating = false;
    impl_->isZooming = false;
    impl_->velocityX = 0.0;
    impl_->velocityY = 0.0;
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
    
    double scaleX = impl_->viewportWidth / width;
    double scaleY = impl_->viewportHeight / height;
    double scale = std::min(scaleX, scaleY) * 0.9;
    
    impl_->state.zoom = ClampZoom(scale);
    impl_->state.centerX = extent.GetCentre().x;
    impl_->state.centerY = extent.GetCentre().y;
    
    ApplyConstraints();
    UpdateExtent();
    
    if (impl_->zoomChangedCallback) {
        impl_->zoomChangedCallback(impl_->state.zoom);
    }
    if (impl_->centerChangedCallback) {
        impl_->centerChangedCallback(impl_->state.centerX, impl_->state.centerY);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

void PanZoomHandler::FitToAll() {
    if (impl_->state.hasConstraints) {
        FitToExtent(impl_->state.constraints);
    }
}

void PanZoomHandler::SetPanZoomState(const PanZoomState& state) {
    impl_->state = state;
    impl_->state.zoom = ClampZoom(impl_->state.zoom);
    impl_->state.rotation = ClampRotation(impl_->state.rotation);
    ApplyConstraints();
    UpdateExtent();
}

void PanZoomHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    impl_->screenToWorld = transform;
}

void PanZoomHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    impl_->worldToScreen = transform;
}

Coordinate PanZoomHandler::ScreenToWorld(double screenX, double screenY) const {
    if (impl_->screenToWorld) {
        return impl_->screenToWorld(screenX, screenY);
    }
    
    double x = impl_->state.centerX + (screenX - impl_->viewportWidth / 2.0) / impl_->state.zoom;
    double y = impl_->state.centerY - (screenY - impl_->viewportHeight / 2.0) / impl_->state.zoom;
    return Coordinate(x, y);
}

Coordinate PanZoomHandler::WorldToScreen(double worldX, double worldY) const {
    if (impl_->worldToScreen) {
        return impl_->worldToScreen(worldX, worldY);
    }
    
    double x = (worldX - impl_->state.centerX) * impl_->state.zoom + impl_->viewportWidth / 2.0;
    double y = impl_->viewportHeight / 2.0 - (worldY - impl_->state.centerY) * impl_->state.zoom;
    return Coordinate(x, y);
}

void PanZoomHandler::UpdateExtent() {
    double halfWidth = impl_->viewportWidth / (2.0 * impl_->state.zoom);
    double halfHeight = impl_->viewportHeight / (2.0 * impl_->state.zoom);
    
    impl_->state.extent = Envelope(
        impl_->state.centerX - halfWidth,
        impl_->state.centerY - halfHeight,
        impl_->state.centerX + halfWidth,
        impl_->state.centerY + halfHeight
    );
}

void PanZoomHandler::ApplyConstraints() {
    if (!impl_->state.hasConstraints) {
        return;
    }
    
    auto clamped = ClampCenter(impl_->state.centerX, impl_->state.centerY);
    impl_->state.centerX = clamped.x;
    impl_->state.centerY = clamped.y;
}

void PanZoomHandler::StartInertia(double velocityX, double velocityY) {
    impl_->velocityX = velocityX;
    impl_->velocityY = velocityY;
    impl_->isPanning = true;
    impl_->isAnimating = true;
}

void PanZoomHandler::UpdateInertia() {
    if (!impl_->isAnimating) {
        return;
    }
    
    if (std::abs(impl_->velocityX) < impl_->inertia.velocityThreshold && 
        std::abs(impl_->velocityY) < impl_->inertia.velocityThreshold) {
        impl_->velocityX = 0.0;
        impl_->velocityY = 0.0;
        impl_->isAnimating = false;
        return;
    }
    
    Pan(-impl_->velocityX, impl_->velocityY);
    
    impl_->velocityX *= impl_->inertia.friction;
    impl_->velocityY *= impl_->inertia.friction;
}

void PanZoomHandler::StartZoomAnimation(double targetZoom, double screenX, double screenY) {
    impl_->targetZoom = ClampZoom(targetZoom);
    impl_->startZoom = impl_->state.zoom;
    impl_->zoomCenterX = screenX;
    impl_->zoomCenterY = screenY;
    impl_->animationStartTime = std::chrono::steady_clock::now();
    impl_->isZooming = true;
    impl_->isAnimating = true;
}

void PanZoomHandler::UpdateZoomAnimation() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - impl_->animationStartTime).count();
    
    if (elapsed >= impl_->animationDurationMs) {
        impl_->state.zoom = impl_->targetZoom;
        impl_->isZooming = false;
        impl_->isAnimating = false;
        UpdateExtent();
        
        if (impl_->zoomChangedCallback) {
            impl_->zoomChangedCallback(impl_->state.zoom);
        }
        if (impl_->extentChangedCallback) {
            impl_->extentChangedCallback(impl_->state.extent);
        }
        return;
    }
    
    double t = static_cast<double>(elapsed) / impl_->animationDurationMs;
    t = t * t * (3 - 2 * t);
    
    double currentZoom = impl_->startZoom + (impl_->targetZoom - impl_->startZoom) * t;
    
    Coordinate worldBefore = ScreenToWorld(impl_->zoomCenterX, impl_->zoomCenterY);
    impl_->state.zoom = currentZoom;
    Coordinate worldAfter = ScreenToWorld(impl_->zoomCenterX, impl_->zoomCenterY);
    
    impl_->state.centerX -= (worldAfter.x - worldBefore.x);
    impl_->state.centerY -= (worldAfter.y - worldBefore.y);
    
    ApplyConstraints();
    UpdateExtent();
    
    if (impl_->zoomChangedCallback) {
        impl_->zoomChangedCallback(impl_->state.zoom);
    }
    if (impl_->extentChangedCallback) {
        impl_->extentChangedCallback(impl_->state.extent);
    }
}

bool PanZoomHandler::HandleMouseDown(const InteractionEvent& event) {
    if (event.button == MouseButton::kLeft || event.button == MouseButton::kMiddle) {
        impl_->isPanning = true;
        impl_->panStartX = event.screenX;
        impl_->panStartY = event.screenY;
        impl_->lastMouseX = event.screenX;
        impl_->lastMouseY = event.screenY;
        impl_->velocityX = 0.0;
        impl_->velocityY = 0.0;
        impl_->lastMoveTime = std::chrono::steady_clock::now();
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleMouseMove(const InteractionEvent& event) {
    if (impl_->isPanning) {
        double dx = event.screenX - impl_->lastMouseX;
        double dy = event.screenY - impl_->lastMouseY;
        
        Pan(dx, dy);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - impl_->lastMoveTime).count();
        
        if (elapsed > 0) {
            impl_->velocityX = dx / elapsed * 16.0;
            impl_->velocityY = dy / elapsed * 16.0;
        }
        
        impl_->lastMouseX = event.screenX;
        impl_->lastMouseY = event.screenY;
        impl_->lastMoveTime = now;
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleMouseUp(const InteractionEvent& event) {
    if (impl_->isPanning && (event.button == MouseButton::kLeft || event.button == MouseButton::kMiddle)) {
        impl_->isPanning = false;
        
        if (impl_->inertia.enabled && 
            (std::abs(impl_->velocityX) > impl_->inertia.velocityThreshold || 
             std::abs(impl_->velocityY) > impl_->inertia.velocityThreshold)) {
            impl_->isAnimating = true;
        }
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleWheel(const InteractionEvent& event) {
    double factor = event.delta > 0 ? impl_->zoomParams.factor : 1.0 / impl_->zoomParams.factor;
    
    if (impl_->zoomParams.animate && impl_->animationEnabled) {
        StartZoomAnimation(impl_->state.zoom * factor, event.screenX, event.screenY);
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
        case 37:
            Pan(impl_->viewportWidth * 0.1, 0);
            return true;
        case 38:
            Pan(0, impl_->viewportHeight * 0.1);
            return true;
        case 39:
            Pan(-impl_->viewportWidth * 0.1, 0);
            return true;
        case 40:
            Pan(0, -impl_->viewportHeight * 0.1);
            return true;
        default:
            return false;
    }
}

bool PanZoomHandler::HandleTouchStart(const InteractionEvent& event) {
    if (event.touchPositions.size() == 1) {
        impl_->isPanning = true;
        impl_->lastMouseX = event.touchPositions[0].x;
        impl_->lastMouseY = event.touchPositions[0].y;
        impl_->velocityX = 0.0;
        impl_->velocityY = 0.0;
        impl_->lastMoveTime = std::chrono::steady_clock::now();
        return true;
    } else if (event.touchPositions.size() == 2) {
        impl_->isPinching = true;
        impl_->isPanning = false;
        
        double dx = event.touchPositions[1].x - event.touchPositions[0].x;
        double dy = event.touchPositions[1].y - event.touchPositions[0].y;
        impl_->initialPinchDistance = std::sqrt(dx * dx + dy * dy);
        impl_->initialPinchZoom = impl_->state.zoom;
        
        impl_->initialPinchCenter = Coordinate(
            (event.touchPositions[0].x + event.touchPositions[1].x) / 2.0,
            (event.touchPositions[0].y + event.touchPositions[1].y) / 2.0
        );
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleTouchMove(const InteractionEvent& event) {
    if (impl_->isPanning && event.touchPositions.size() == 1) {
        double dx = event.touchPositions[0].x - impl_->lastMouseX;
        double dy = event.touchPositions[0].y - impl_->lastMouseY;
        
        Pan(dx, dy);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - impl_->lastMoveTime).count();
        
        if (elapsed > 0) {
            impl_->velocityX = dx / elapsed * 16.0;
            impl_->velocityY = dy / elapsed * 16.0;
        }
        
        impl_->lastMouseX = event.touchPositions[0].x;
        impl_->lastMouseY = event.touchPositions[0].y;
        impl_->lastMoveTime = now;
        
        return true;
    } else if (impl_->isPinching && event.touchPositions.size() == 2) {
        double dx = event.touchPositions[1].x - event.touchPositions[0].x;
        double dy = event.touchPositions[1].y - event.touchPositions[0].y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        if (impl_->initialPinchDistance > 0) {
            double scale = distance / impl_->initialPinchDistance;
            double newZoom = impl_->initialPinchZoom * scale;
            newZoom = ClampZoom(newZoom);
            
            ZoomToPoint(impl_->initialPinchCenter.x, impl_->initialPinchCenter.y, newZoom / impl_->state.zoom);
        }
        
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleTouchEnd(const InteractionEvent& event) {
    if (impl_->isPanning) {
        impl_->isPanning = false;
        
        if (impl_->inertia.enabled && 
            (std::abs(impl_->velocityX) > impl_->inertia.velocityThreshold || 
             std::abs(impl_->velocityY) > impl_->inertia.velocityThreshold)) {
            impl_->isAnimating = true;
        }
        
        return true;
    } else if (impl_->isPinching) {
        impl_->isPinching = false;
        return true;
    }
    return false;
}

bool PanZoomHandler::HandleDoubleClick(const InteractionEvent& event) {
    if (event.button == MouseButton::kLeft) {
        ZoomToPoint(event.screenX, event.screenY, impl_->zoomParams.factor);
        return true;
    }
    return false;
}

double PanZoomHandler::ClampZoom(double zoom) const {
    return std::max(impl_->state.minZoom, std::min(impl_->state.maxZoom, zoom));
}

double PanZoomHandler::ClampRotation(double rotation) const {
    while (rotation > 180.0) rotation -= 360.0;
    while (rotation < -180.0) rotation += 360.0;
    return rotation;
}

Coordinate PanZoomHandler::ClampCenter(double x, double y) const {
    if (!impl_->state.hasConstraints) {
        return Coordinate(x, y);
    }
    
    double halfWidth = impl_->viewportWidth / (2.0 * impl_->state.zoom);
    double halfHeight = impl_->viewportHeight / (2.0 * impl_->state.zoom);
    
    double minX = impl_->state.constraints.GetMinX() + halfWidth;
    double maxX = impl_->state.constraints.GetMaxX() - halfWidth;
    double minY = impl_->state.constraints.GetMinY() + halfHeight;
    double maxY = impl_->state.constraints.GetMaxY() - halfHeight;
    
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
