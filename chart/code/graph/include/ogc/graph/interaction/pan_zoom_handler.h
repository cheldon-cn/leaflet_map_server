#ifndef OGC_GRAPH_PAN_ZOOM_HANDLER_H
#define OGC_GRAPH_PAN_ZOOM_HANDLER_H

#include "ogc/graph/export.h"
#include "ogc/graph/interaction/interaction_handler.h"
#include "ogc/geom/envelope.h"
#include "ogc/geom/coordinate.h"
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
    
    std::string GetName() const override;
    int GetPriority() const override;
    void SetPriority(int priority) override;
    
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    
    bool HandleEvent(const InteractionEvent& event) override;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    void SetCenter(double x, double y);
    Coordinate GetCenter() const;
    
    void SetZoom(double zoom);
    double GetZoom() const;
    
    void SetRotation(double degrees);
    double GetRotation() const;
    
    void SetConstraints(const Envelope& constraints);
    void ClearConstraints();
    bool HasConstraints() const;
    
    void SetMinZoom(double minZoom);
    double GetMinZoom() const;
    
    void SetMaxZoom(double maxZoom);
    double GetMaxZoom() const;
    
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
    int GetViewportWidth() const;
    int GetViewportHeight() const;
    
    void SetInertiaEnabled(bool enabled);
    bool IsInertiaEnabled() const;
    
    void SetInertiaFriction(double friction);
    double GetInertiaFriction() const;
    
    void SetAnimationEnabled(bool enabled);
    bool IsAnimationEnabled() const;
    
    void SetZoomParams(const ZoomParams& params);
    ZoomParams GetZoomParams() const;
    
    void SetExtentChangedCallback(ExtentChangedCallback callback);
    void SetZoomChangedCallback(ZoomChangedCallback callback);
    void SetCenterChangedCallback(CenterChangedCallback callback);
    void SetRotationChangedCallback(RotationChangedCallback callback);
    
    void UpdateAnimation();
    bool IsAnimating() const;
    void CancelAnimation();
    
    void FitToExtent(const Envelope& extent);
    void FitToAll();
    
    InteractionState GetState() const override;
    
    const PanZoomState& GetPanZoomState() const;
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
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  
}  

#endif
