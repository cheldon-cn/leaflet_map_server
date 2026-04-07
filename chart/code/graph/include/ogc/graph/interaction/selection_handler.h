#ifndef OGC_GRAPH_SELECTION_HANDLER_H
#define OGC_GRAPH_SELECTION_HANDLER_H

#include "ogc/graph/export.h"
#include "ogc/graph/interaction/interaction_handler.h"
#include "ogc/graph/interaction/hit_test.h"
#include "ogc/graph/interaction/interaction_feedback.h"
#include "ogc/envelope.h"
#include "ogc/coordinate.h"
#include <memory>
#include <functional>
#include <set>

namespace ogc {
namespace graph {

class LayerManager;

enum class SelectionMode {
    kNone = 0,
    kSingle,
    kMultiple,
    kToggle,
    kAdd,
    kSubtract
};

struct SelectionStyle {
    uint32_t fillColor = 0x0000FF00;
    uint32_t strokeColor = 0x0000FF00;
    double strokeWidth = 2.0;
    double fillOpacity = 0.3;
    double strokeOpacity = 1.0;
    bool showVertices = false;
    uint32_t vertexColor = 0x00FF0000;
    double vertexSize = 5.0;
};

struct SelectionParams {
    SelectionMode mode = SelectionMode::kSingle;
    double tolerance = 5.0;
    bool enableBoxSelection = true;
    bool enableMultiSelection = true;
    bool showFeedback = true;
    SelectionStyle style;
};

class OGC_GRAPH_API SelectionHandler : public IInteractionHandler {
public:
    using SelectionChangedCallback = std::function<void(const std::set<std::string>&)>;
    using FeatureSelectedCallback = std::function<void(const std::string&, bool)>;
    
    static std::unique_ptr<SelectionHandler> Create(const std::string& name = "Selection");
    
    ~SelectionHandler() override;
    
    std::string GetName() const override { return m_name; }
    int GetPriority() const override { return m_priority; }
    void SetPriority(int priority) override { m_priority = priority; }
    
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    
    bool HandleEvent(const InteractionEvent& event) override;
    
    InteractionState GetState() const override { 
        return m_isSelecting ? InteractionState::kSelect : InteractionState::kNone; 
    }
    
    void SetLayerManager(LayerManager* manager);
    LayerManager* GetLayerManager() const { return m_layerManager; }
    
    void SetSelectionMode(SelectionMode mode);
    SelectionMode GetSelectionMode() const { return m_params.mode; }
    
    void SetTolerance(double tolerance);
    double GetTolerance() const { return m_params.tolerance; }
    
    void SetSelectionParams(const SelectionParams& params);
    SelectionParams GetSelectionParams() const { return m_params; }
    
    void SetSelectionStyle(const SelectionStyle& style);
    SelectionStyle GetSelectionStyle() const { return m_params.style; }
    
    void SelectFeature(const std::string& featureId);
    void DeselectFeature(const std::string& featureId);
    void ToggleFeature(const std::string& featureId);
    void SelectFeatures(const std::set<std::string>& featureIds);
    void ClearSelection();
    
    bool IsFeatureSelected(const std::string& featureId) const;
    const std::set<std::string>& GetSelectedFeatures() const { return m_selectedFeatures; }
    size_t GetSelectionCount() const { return m_selectedFeatures.size(); }
    bool HasSelection() const { return !m_selectedFeatures.empty(); }
    
    void SelectByEnvelope(const Envelope& envelope);
    void SelectByPoint(double screenX, double screenY);
    void SelectByGeometry(const Geometry* geometry);
    
    void SetSelectionChangedCallback(SelectionChangedCallback callback);
    void SetFeatureSelectedCallback(FeatureSelectedCallback callback);
    
    void SetHitTester(HitTester* hitTester);
    HitTester* GetHitTester() const { return m_hitTester; }
    
    void SetFeedbackManager(FeedbackManager* manager);
    FeedbackManager* GetFeedbackManager() const { return m_feedbackManager; }
    
    void SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform);
    void SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform);
    
    Coordinate ScreenToWorld(double screenX, double screenY) const;
    Coordinate WorldToScreen(double worldX, double worldY) const;
    
    void SetViewportSize(int width, int height);
    
    const Envelope& GetSelectionEnvelope() const { return m_selectionEnvelope; }

private:
    explicit SelectionHandler(const std::string& name);
    
    bool HandleMouseDown(const InteractionEvent& event);
    bool HandleMouseMove(const InteractionEvent& event);
    bool HandleMouseUp(const InteractionEvent& event);
    bool HandleKeyDown(const InteractionEvent& event);
    
    void StartBoxSelection(double screenX, double screenY);
    void UpdateBoxSelection(double screenX, double screenY);
    void EndBoxSelection();
    void CancelSelection();
    
    void PerformPointSelection(double screenX, double screenY, bool addToSelection);
    void PerformBoxSelection(const Envelope& envelope, bool addToSelection);
    
    void UpdateSelectionFeedback();
    void ClearSelectionFeedback();
    
    void NotifySelectionChanged();
    
    std::string m_name;
    int m_priority = 90;
    bool m_enabled = true;
    
    SelectionParams m_params;
    std::set<std::string> m_selectedFeatures;
    Envelope m_selectionEnvelope;
    
    LayerManager* m_layerManager = nullptr;
    HitTester* m_hitTester = nullptr;
    FeedbackManager* m_feedbackManager = nullptr;
    
    bool m_isSelecting = false;
    bool m_isBoxSelecting = false;
    bool m_isShiftPressed = false;
    bool m_isCtrlPressed = false;
    
    double m_startX = 0.0;
    double m_startY = 0.0;
    double m_currentX = 0.0;
    double m_currentY = 0.0;
    
    int m_viewportWidth = 800;
    int m_viewportHeight = 600;
    
    uint64_t m_feedbackId = 0;
    
    SelectionChangedCallback m_selectionChangedCallback;
    FeatureSelectedCallback m_featureSelectedCallback;
    
    std::function<Coordinate(double, double)> m_screenToWorld;
    std::function<Coordinate(double, double)> m_worldToScreen;
};

}  
}  

#endif
