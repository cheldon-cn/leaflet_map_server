#ifndef OGC_GRAPH_PAN_ZOOM_HANDLER_H
#define OGC_GRAPH_PAN_ZOOM_HANDLER_H

#include "ogc/graph/export.h"
#include "ogc/graph/interaction/interaction_handler.h"
#include "ogc/envelope.h"
#include "ogc/coordinate.h"
#include <memory>
#include <functional>
#include <chrono>

namespace ogc {
namespace graph {

struct PanZoomState {
    double centerX = 0.0;
    double centerY = 0.0;
    double zoom = 1.0;
    double rotation = 0.0;
    
    double minZoom = 0.01;
    double maxZoom = 100000.0;
    double minRotation = -360.0;
    double maxRotation = 360.0;
    
    Envelope extent;
    Envelope constraints;
    bool hasConstraints = false;
};

struct InertiaParams {
    bool enabled = true;
    double friction = 0.95;
    double velocityThreshold = 0.1;
    int maxIterations = 100;
};

struct ZoomParams {
    double factor = 1.2;
    bool animate = true;
    int animationDurationMs = 200;
};

class OGC_GRAPH_API PanZoomHandler : public IInteractionHandler {
public:
    using ExtentChangedCallback = std::function<void(const Envelope&)>;
    using ZoomChangedCallback = std::function<void(double)>;
    using CenterChangedCallback = std::function<void(double, double)>;
    using RotationChangedCallback = std::function<void(double)>;
    
    static std::unique_ptr<PanZoomHandler> Create(const std::string& name = "PanZoom");
    
    ~PanZoomHandler() override;
    
    std::string GetName() const override { return m_name; }
    int GetPriority() const override { return m_priority; }
    void SetPriority(int priority) override { m_priority = priority; }
    
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    
    bool HandleEvent(const InteractionEvent& event) override;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const { return m_state.extent; }
    
    void SetCenter(double x, double y);
    Coordinate GetCenter() const { return Coordinate(m_state.centerX, m_state.centerY); }
    
    void SetZoom(double zoom);
    double GetZoom() const { return m_state.zoom; }
    
    void SetRotation(double degrees);
    double GetRotation() const { return m_state.rotation; }
    
    void SetConstraints(const Envelope& constraints);
    void ClearConstraints();
    bool HasConstraints() const { return m_state.hasConstraints; }
    
    void SetMinZoom(double minZoom) { m_state.minZoom = minZoom; }
    double GetMinZoom() const { return m_state.minZoom; }
    
    void SetMaxZoom(double maxZoom) { m_state.maxZoom = maxZoom; }
    double GetMaxZoom() const { return m_state.maxZoom; }
    
    void Pan(double dx, double dy);
    void PanTo(double x, double y);
    
    void ZoomIn(double factor = 1.2);
    void ZoomOut(double factor = 1.2);
    void ZoomTo(double zoom);
    void ZoomToPoint(double screenX, double screenY, double factor);
    
    void Rotate(double degrees);
    void RotateTo(double degrees);
    void ResetRotation();
    
    void SetViewportSize(int width, int height);
    int GetViewportWidth() const { return m_viewportWidth; }
    int GetViewportHeight() const { return m_viewportHeight; }
    
    void SetInertiaEnabled(bool enabled) { m_inertia.enabled = enabled; }
    bool IsInertiaEnabled() const { return m_inertia.enabled; }
    
    void SetInertiaFriction(double friction) { m_inertia.friction = friction; }
    double GetInertiaFriction() const { return m_inertia.friction; }
    
    void SetAnimationEnabled(bool enabled) { m_animationEnabled = enabled; }
    bool IsAnimationEnabled() const { return m_animationEnabled; }
    
    void SetZoomParams(const ZoomParams& params) { m_zoomParams = params; }
    ZoomParams GetZoomParams() const { return m_zoomParams; }
    
    void SetExtentChangedCallback(ExtentChangedCallback callback);
    void SetZoomChangedCallback(ZoomChangedCallback callback);
    void SetCenterChangedCallback(CenterChangedCallback callback);
    void SetRotationChangedCallback(RotationChangedCallback callback);
    
    void UpdateAnimation();
    bool IsAnimating() const { return m_isAnimating; }
    void CancelAnimation();
    
    void FitToExtent(const Envelope& extent);
    void FitToAll();
    
    InteractionState GetState() const override { return m_isPanning ? InteractionState::kPan : InteractionState::kNone; }
    
    const PanZoomState& GetPanZoomState() const { return m_state; }
    void SetPanZoomState(const PanZoomState& state);
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    
    Coordinate ScreenToWorld(double screenX, double screenY) const;
    Coordinate WorldToScreen(double worldX, double worldY) const;

private:
    explicit PanZoomHandler(const std::string& name);
    
    void UpdateExtent();
    void ApplyConstraints();
    void StartInertia(double velocityX, double velocityY);
    void UpdateInertia();
    void StartZoomAnimation(double targetZoom, double screenX, double screenY);
    void UpdateZoomAnimation();
    
    bool HandleMouseDown(const InteractionEvent& event);
    bool HandleMouseMove(const InteractionEvent& event);
    bool HandleMouseUp(const InteractionEvent& event);
    bool HandleWheel(const InteractionEvent& event);
    bool HandleKeyDown(const InteractionEvent& event);
    bool HandleTouchStart(const InteractionEvent& event);
    bool HandleTouchMove(const InteractionEvent& event);
    bool HandleTouchEnd(const InteractionEvent& event);
    bool HandleDoubleClick(const InteractionEvent& event);
    
    double ClampZoom(double zoom) const;
    double ClampRotation(double rotation) const;
    Coordinate ClampCenter(double x, double y) const;
    
    std::string m_name;
    int m_priority = 100;
    bool m_enabled = true;
    
    PanZoomState m_state;
    InertiaParams m_inertia;
    ZoomParams m_zoomParams;
    
    int m_viewportWidth = 800;
    int m_viewportHeight = 600;
    
    bool m_isPanning = false;
    bool m_isZooming = false;
    bool m_isAnimating = false;
    bool m_animationEnabled = true;
    
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    double m_panStartX = 0.0;
    double m_panStartY = 0.0;
    
    double m_velocityX = 0.0;
    double m_velocityY = 0.0;
    std::chrono::steady_clock::time_point m_lastMoveTime;
    
    double m_targetZoom = 1.0;
    double m_startZoom = 1.0;
    double m_zoomCenterX = 0.0;
    double m_zoomCenterY = 0.0;
    std::chrono::steady_clock::time_point m_animationStartTime;
    int m_animationDurationMs = 200;
    
    double m_initialPinchDistance = 0.0;
    double m_initialPinchZoom = 1.0;
    Coordinate m_initialPinchCenter;
    bool m_isPinching = false;
    
    ExtentChangedCallback m_extentChangedCallback;
    ZoomChangedCallback m_zoomChangedCallback;
    CenterChangedCallback m_centerChangedCallback;
    RotationChangedCallback m_rotationChangedCallback;
    
    std::function<Coordinate(double, double)> m_screenToWorld;
    std::function<Coordinate(double, double)> m_worldToScreen;
};

}  
}  

#endif
