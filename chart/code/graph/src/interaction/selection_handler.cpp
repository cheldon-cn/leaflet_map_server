#include "ogc/graph/interaction/selection_handler.h"
#include "ogc/graph/layer/layer_manager.h"
#include <algorithm>

namespace ogc {
namespace graph {

std::unique_ptr<SelectionHandler> SelectionHandler::Create(const std::string& name) {
    return std::unique_ptr<SelectionHandler>(new SelectionHandler(name));
}

SelectionHandler::SelectionHandler(const std::string& name)
    : m_name(name)
{
}

SelectionHandler::~SelectionHandler() {
    ClearSelectionFeedback();
}

bool SelectionHandler::HandleEvent(const InteractionEvent& event) {
    if (!m_enabled) {
        return false;
    }
    
    m_isShiftPressed = event.isShift;
    m_isCtrlPressed = event.isCtrl;
    
    switch (event.type) {
        case InteractionEventType::kMouseDown:
            return HandleMouseDown(event);
        case InteractionEventType::kMouseMove:
            return HandleMouseMove(event);
        case InteractionEventType::kMouseUp:
            return HandleMouseUp(event);
        case InteractionEventType::kKeyDown:
            return HandleKeyDown(event);
        default:
            return false;
    }
}

void SelectionHandler::SetLayerManager(LayerManager* manager) {
    m_layerManager = manager;
}

void SelectionHandler::SetSelectionMode(SelectionMode mode) {
    m_params.mode = mode;
}

void SelectionHandler::SetTolerance(double tolerance) {
    m_params.tolerance = tolerance;
}

void SelectionHandler::SetSelectionParams(const SelectionParams& params) {
    m_params = params;
}

void SelectionHandler::SetSelectionStyle(const SelectionStyle& style) {
    m_params.style = style;
}

void SelectionHandler::SelectFeature(const std::string& featureId) {
    if (m_selectedFeatures.insert(featureId).second) {
        UpdateSelectionFeedback();
        NotifySelectionChanged();
    }
}

void SelectionHandler::DeselectFeature(const std::string& featureId) {
    if (m_selectedFeatures.erase(featureId) > 0) {
        UpdateSelectionFeedback();
        NotifySelectionChanged();
    }
}

void SelectionHandler::ToggleFeature(const std::string& featureId) {
    if (IsFeatureSelected(featureId)) {
        DeselectFeature(featureId);
    } else {
        SelectFeature(featureId);
    }
}

void SelectionHandler::SelectFeatures(const std::set<std::string>& featureIds) {
    bool changed = false;
    for (const auto& id : featureIds) {
        if (m_selectedFeatures.insert(id).second) {
            changed = true;
        }
    }
    if (changed) {
        UpdateSelectionFeedback();
        NotifySelectionChanged();
    }
}

void SelectionHandler::ClearSelection() {
    if (!m_selectedFeatures.empty()) {
        m_selectedFeatures.clear();
        m_selectionEnvelope = Envelope();
        ClearSelectionFeedback();
        NotifySelectionChanged();
    }
}

bool SelectionHandler::IsFeatureSelected(const std::string& featureId) const {
    return m_selectedFeatures.find(featureId) != m_selectedFeatures.end();
}

void SelectionHandler::SelectByEnvelope(const Envelope& envelope) {
    if (!m_layerManager || !m_hitTester) {
        return;
    }
    
    auto result = m_hitTester->HitTest(m_layerManager, envelope, HitTestMode::kBox);
    
    if (result && !result->IsEmpty()) {
        bool addToSelection = m_isShiftPressed || m_isCtrlPressed;
        if (!addToSelection) {
            ClearSelection();
        }
        
        std::set<std::string> featureIds;
        const auto& items = result->GetItems();
        for (const auto& item : items) {
            featureIds.insert(std::to_string(item.featureId));
        }
        SelectFeatures(featureIds);
    }
}

void SelectionHandler::SelectByPoint(double screenX, double screenY) {
    PerformPointSelection(screenX, screenY, m_isShiftPressed || m_isCtrlPressed);
}

void SelectionHandler::SelectByGeometry(const Geometry* geometry) {
    if (!geometry || !m_layerManager || !m_hitTester) {
        return;
    }
    
    auto result = m_hitTester->HitTest(m_layerManager, geometry, HitTestMode::kPolygon);
    
    if (result && !result->IsEmpty()) {
        bool addToSelection = m_isShiftPressed || m_isCtrlPressed;
        if (!addToSelection) {
            ClearSelection();
        }
        
        std::set<std::string> featureIds;
        const auto& items = result->GetItems();
        for (const auto& item : items) {
            featureIds.insert(std::to_string(item.featureId));
        }
        SelectFeatures(featureIds);
    }
}

void SelectionHandler::SetSelectionChangedCallback(SelectionChangedCallback callback) {
    m_selectionChangedCallback = callback;
}

void SelectionHandler::SetFeatureSelectedCallback(FeatureSelectedCallback callback) {
    m_featureSelectedCallback = callback;
}

void SelectionHandler::SetHitTester(HitTester* hitTester) {
    m_hitTester = hitTester;
}

void SelectionHandler::SetFeedbackManager(FeedbackManager* manager) {
    m_feedbackManager = manager;
}

void SelectionHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    m_screenToWorld = transform;
}

void SelectionHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    m_worldToScreen = transform;
}

Coordinate SelectionHandler::ScreenToWorld(double screenX, double screenY) const {
    if (m_screenToWorld) {
        return m_screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate SelectionHandler::WorldToScreen(double worldX, double worldY) const {
    if (m_worldToScreen) {
        return m_worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void SelectionHandler::SetViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

bool SelectionHandler::HandleMouseDown(const InteractionEvent& event) {
    if (event.button != MouseButton::kLeft) {
        return false;
    }
    
    m_isSelecting = true;
    m_startX = event.screenX;
    m_startY = event.screenY;
    m_currentX = event.screenX;
    m_currentY = event.screenY;
    
    if (m_params.enableBoxSelection && !m_isShiftPressed) {
        m_isBoxSelecting = false;
    }
    
    return true;
}

bool SelectionHandler::HandleMouseMove(const InteractionEvent& event) {
    if (!m_isSelecting) {
        return false;
    }
    
    m_currentX = event.screenX;
    m_currentY = event.screenY;
    
    double dx = std::abs(m_currentX - m_startX);
    double dy = std::abs(m_currentY - m_startY);
    
    if (m_params.enableBoxSelection && (dx > m_params.tolerance || dy > m_params.tolerance)) {
        if (!m_isBoxSelecting) {
            m_isBoxSelecting = true;
            StartBoxSelection(m_startX, m_startY);
        }
        UpdateBoxSelection(m_currentX, m_currentY);
    }
    
    return true;
}

bool SelectionHandler::HandleMouseUp(const InteractionEvent& event) {
    if (event.button != MouseButton::kLeft || !m_isSelecting) {
        return false;
    }
    
    m_currentX = event.screenX;
    m_currentY = event.screenY;
    
    double dx = std::abs(m_currentX - m_startX);
    double dy = std::abs(m_currentY - m_startY);
    
    if (m_isBoxSelecting) {
        EndBoxSelection();
    } else if (dx <= m_params.tolerance && dy <= m_params.tolerance) {
        PerformPointSelection(m_startX, m_startY, m_isShiftPressed || m_isCtrlPressed);
    }
    
    m_isSelecting = false;
    m_isBoxSelecting = false;
    
    return true;
}

bool SelectionHandler::HandleKeyDown(const InteractionEvent& event) {
    switch (event.key) {
        case 27:  // Escape
            CancelSelection();
            return true;
        case 'A':
        case 'a':
            if (m_isCtrlPressed) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

void SelectionHandler::StartBoxSelection(double screenX, double screenY) {
    if (m_params.showFeedback && m_feedbackManager) {
        auto item = FeedbackItem::Create(FeedbackType::kSelection);
        if (item) {
            item->SetEnvelope(Envelope(screenX, screenY, screenX, screenY));
            FeedbackConfig config;
            config.strokeColor = m_params.style.strokeColor;
            config.strokeWidth = m_params.style.strokeWidth;
            config.opacity = m_params.style.strokeOpacity;
            config.fillColor = m_params.style.fillColor;
            config.visible = true;
            item->SetConfig(config);
            m_feedbackId = m_feedbackManager->AddFeedback(item);
        }
    }
}

void SelectionHandler::UpdateBoxSelection(double screenX, double screenY) {
    if (m_params.showFeedback && m_feedbackManager && m_feedbackId > 0) {
        auto item = m_feedbackManager->GetFeedback(m_feedbackId);
        if (item) {
            double minX = std::min(m_startX, screenX);
            double minY = std::min(m_startY, screenY);
            double maxX = std::max(m_startX, screenX);
            double maxY = std::max(m_startY, screenY);
            item->SetEnvelope(Envelope(minX, minY, maxX, maxY));
        }
    }
}

void SelectionHandler::EndBoxSelection() {
    double minX = std::min(m_startX, m_currentX);
    double minY = std::min(m_startY, m_currentY);
    double maxX = std::max(m_startX, m_currentX);
    double maxY = std::max(m_startY, m_currentY);
    
    Envelope screenEnvelope(minX, minY, maxX, maxY);
    
    Coordinate worldMin = ScreenToWorld(minX, maxY);
    Coordinate worldMax = ScreenToWorld(maxX, minY);
    Envelope worldEnvelope(worldMin.x, worldMin.y, worldMax.x, worldMax.y);
    
    ClearSelectionFeedback();
    
    PerformBoxSelection(worldEnvelope, m_isShiftPressed || m_isCtrlPressed);
}

void SelectionHandler::CancelSelection() {
    m_isSelecting = false;
    m_isBoxSelecting = false;
    ClearSelectionFeedback();
}

void SelectionHandler::PerformPointSelection(double screenX, double screenY, bool addToSelection) {
    if (!m_layerManager || !m_hitTester) {
        return;
    }
    
    auto result = m_hitTester->HitTest(m_layerManager, screenX, screenY, m_params.tolerance);
    
    if (result && !result->IsEmpty()) {
        const auto& items = result->GetItems();
        if (!items.empty()) {
            const auto& item = items[0];
            std::string featureId = std::to_string(item.featureId);
            
            if (addToSelection) {
                ToggleFeature(featureId);
            } else {
                ClearSelection();
                SelectFeature(featureId);
            }
        } else if (!addToSelection) {
            ClearSelection();
        }
    } else if (!addToSelection) {
        ClearSelection();
    }
}

void SelectionHandler::PerformBoxSelection(const Envelope& envelope, bool addToSelection) {
    if (!m_layerManager || !m_hitTester) {
        return;
    }
    
    auto result = m_hitTester->HitTest(m_layerManager, envelope, HitTestMode::kBox);
    
    if (result && !result->IsEmpty()) {
        if (!addToSelection) {
            ClearSelection();
        }
        
        std::set<std::string> featureIds;
        const auto& items = result->GetItems();
        for (const auto& item : items) {
            featureIds.insert(std::to_string(item.featureId));
        }
        SelectFeatures(featureIds);
    } else if (!addToSelection) {
        ClearSelection();
    }
}

void SelectionHandler::UpdateSelectionFeedback() {
    if (!m_params.showFeedback || !m_feedbackManager || m_selectedFeatures.empty()) {
        return;
    }
    
    ClearSelectionFeedback();
    
    auto item = FeedbackItem::Create(FeedbackType::kSelection);
    if (item) {
        FeedbackConfig config;
        config.strokeColor = m_params.style.strokeColor;
        config.strokeWidth = m_params.style.strokeWidth;
        config.opacity = m_params.style.strokeOpacity;
        config.fillColor = m_params.style.fillColor;
        config.visible = true;
        item->SetConfig(config);
        
        m_feedbackId = m_feedbackManager->AddFeedback(item);
    }
}

void SelectionHandler::ClearSelectionFeedback() {
    if (m_feedbackManager && m_feedbackId > 0) {
        m_feedbackManager->RemoveFeedback(m_feedbackId);
        m_feedbackId = 0;
    }
}

void SelectionHandler::NotifySelectionChanged() {
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(m_selectedFeatures);
    }
}

}  
}  
