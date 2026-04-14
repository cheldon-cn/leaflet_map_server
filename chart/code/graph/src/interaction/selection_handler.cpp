#include "ogc/graph/interaction/selection_handler.h"
#include "ogc/graph/layer/layer_manager.h"
#include <algorithm>

namespace ogc {
namespace graph {

struct SelectionHandler::Impl {
    std::string name;
    int priority = 90;
    bool enabled = true;
    
    SelectionParams params;
    std::set<std::string> selectedFeatures;
    Envelope selectionEnvelope;
    
    LayerManager* layerManager = nullptr;
    HitTester* hitTester = nullptr;
    FeedbackManager* feedbackManager = nullptr;
    
    bool isSelecting = false;
    bool isBoxSelecting = false;
    bool isShiftPressed = false;
    bool isCtrlPressed = false;
    
    double startX = 0.0;
    double startY = 0.0;
    double currentX = 0.0;
    double currentY = 0.0;
    
    int viewportWidth = 800;
    int viewportHeight = 600;
    
    uint64_t feedbackId = 0;
    
    SelectionChangedCallback selectionChangedCallback;
    FeatureSelectedCallback featureSelectedCallback;
    
    std::function<Coordinate(double, double)> screenToWorld;
    std::function<Coordinate(double, double)> worldToScreen;
    
    Impl(const std::string& n) : name(n) {}
};

std::unique_ptr<SelectionHandler> SelectionHandler::Create(const std::string& name) {
    return std::unique_ptr<SelectionHandler>(new SelectionHandler(name));
}

SelectionHandler::SelectionHandler(const std::string& name)
    : impl_(std::make_unique<Impl>(name))
{
}

SelectionHandler::~SelectionHandler() {
    ClearSelectionFeedback();
}

std::string SelectionHandler::GetName() const { return impl_->name; }
int SelectionHandler::GetPriority() const { return impl_->priority; }
void SelectionHandler::SetPriority(int priority) { impl_->priority = priority; }
bool SelectionHandler::IsEnabled() const { return impl_->enabled; }
void SelectionHandler::SetEnabled(bool enabled) { impl_->enabled = enabled; }

InteractionState SelectionHandler::GetState() const { 
    return impl_->isSelecting ? InteractionState::kSelect : InteractionState::kNone; 
}

LayerManager* SelectionHandler::GetLayerManager() const { return impl_->layerManager; }
SelectionMode SelectionHandler::GetSelectionMode() const { return impl_->params.mode; }
double SelectionHandler::GetTolerance() const { return impl_->params.tolerance; }
SelectionParams SelectionHandler::GetSelectionParams() const { return impl_->params; }
SelectionStyle SelectionHandler::GetSelectionStyle() const { return impl_->params.style; }
const std::set<std::string>& SelectionHandler::GetSelectedFeatures() const { return impl_->selectedFeatures; }
size_t SelectionHandler::GetSelectionCount() const { return impl_->selectedFeatures.size(); }
bool SelectionHandler::HasSelection() const { return !impl_->selectedFeatures.empty(); }
HitTester* SelectionHandler::GetHitTester() const { return impl_->hitTester; }
FeedbackManager* SelectionHandler::GetFeedbackManager() const { return impl_->feedbackManager; }
const Envelope& SelectionHandler::GetSelectionEnvelope() const { return impl_->selectionEnvelope; }

bool SelectionHandler::HandleEvent(const InteractionEvent& event) {
    if (!impl_->enabled) {
        return false;
    }
    
    impl_->isShiftPressed = event.isShift;
    impl_->isCtrlPressed = event.isCtrl;
    
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
    impl_->layerManager = manager;
}

void SelectionHandler::SetSelectionMode(SelectionMode mode) {
    impl_->params.mode = mode;
}

void SelectionHandler::SetTolerance(double tolerance) {
    impl_->params.tolerance = tolerance;
}

void SelectionHandler::SetSelectionParams(const SelectionParams& params) {
    impl_->params = params;
}

void SelectionHandler::SetSelectionStyle(const SelectionStyle& style) {
    impl_->params.style = style;
}

void SelectionHandler::SelectFeature(const std::string& featureId) {
    if (impl_->selectedFeatures.insert(featureId).second) {
        UpdateSelectionFeedback();
        NotifySelectionChanged();
    }
}

void SelectionHandler::DeselectFeature(const std::string& featureId) {
    if (impl_->selectedFeatures.erase(featureId) > 0) {
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
        if (impl_->selectedFeatures.insert(id).second) {
            changed = true;
        }
    }
    if (changed) {
        UpdateSelectionFeedback();
        NotifySelectionChanged();
    }
}

void SelectionHandler::ClearSelection() {
    if (!impl_->selectedFeatures.empty()) {
        impl_->selectedFeatures.clear();
        impl_->selectionEnvelope = Envelope();
        ClearSelectionFeedback();
        NotifySelectionChanged();
    }
}

bool SelectionHandler::IsFeatureSelected(const std::string& featureId) const {
    return impl_->selectedFeatures.find(featureId) != impl_->selectedFeatures.end();
}

void SelectionHandler::SelectByEnvelope(const Envelope& envelope) {
    if (!impl_->layerManager || !impl_->hitTester) {
        return;
    }
    
    auto result = impl_->hitTester->HitTest(impl_->layerManager, envelope, HitTestMode::kBox);
    
    if (result && !result->IsEmpty()) {
        bool addToSelection = impl_->isShiftPressed || impl_->isCtrlPressed;
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
    PerformPointSelection(screenX, screenY, impl_->isShiftPressed || impl_->isCtrlPressed);
}

void SelectionHandler::SelectByGeometry(const Geometry* geometry) {
    if (!geometry || !impl_->layerManager || !impl_->hitTester) {
        return;
    }
    
    auto result = impl_->hitTester->HitTest(impl_->layerManager, geometry, HitTestMode::kPolygon);
    
    if (result && !result->IsEmpty()) {
        bool addToSelection = impl_->isShiftPressed || impl_->isCtrlPressed;
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
    impl_->selectionChangedCallback = callback;
}

void SelectionHandler::SetFeatureSelectedCallback(FeatureSelectedCallback callback) {
    impl_->featureSelectedCallback = callback;
}

void SelectionHandler::SetHitTester(HitTester* hitTester) {
    impl_->hitTester = hitTester;
}

void SelectionHandler::SetFeedbackManager(FeedbackManager* manager) {
    impl_->feedbackManager = manager;
}

void SelectionHandler::SetScreenToWorldTransform(std::function<Coordinate(double, double)> transform) {
    impl_->screenToWorld = transform;
}

void SelectionHandler::SetWorldToScreenTransform(std::function<Coordinate(double, double)> transform) {
    impl_->worldToScreen = transform;
}

Coordinate SelectionHandler::ScreenToWorld(double screenX, double screenY) const {
    if (impl_->screenToWorld) {
        return impl_->screenToWorld(screenX, screenY);
    }
    return Coordinate(screenX, screenY);
}

Coordinate SelectionHandler::WorldToScreen(double worldX, double worldY) const {
    if (impl_->worldToScreen) {
        return impl_->worldToScreen(worldX, worldY);
    }
    return Coordinate(worldX, worldY);
}

void SelectionHandler::SetViewportSize(int width, int height) {
    impl_->viewportWidth = width;
    impl_->viewportHeight = height;
}

bool SelectionHandler::HandleMouseDown(const InteractionEvent& event) {
    if (event.button != MouseButton::kLeft) {
        return false;
    }
    
    impl_->isSelecting = true;
    impl_->startX = event.screenX;
    impl_->startY = event.screenY;
    impl_->currentX = event.screenX;
    impl_->currentY = event.screenY;
    
    if (impl_->params.enableBoxSelection && !impl_->isShiftPressed) {
        impl_->isBoxSelecting = false;
    }
    
    return true;
}

bool SelectionHandler::HandleMouseMove(const InteractionEvent& event) {
    if (!impl_->isSelecting) {
        return false;
    }
    
    impl_->currentX = event.screenX;
    impl_->currentY = event.screenY;
    
    double dx = std::abs(impl_->currentX - impl_->startX);
    double dy = std::abs(impl_->currentY - impl_->startY);
    
    if (impl_->params.enableBoxSelection && (dx > impl_->params.tolerance || dy > impl_->params.tolerance)) {
        if (!impl_->isBoxSelecting) {
            impl_->isBoxSelecting = true;
            StartBoxSelection(impl_->startX, impl_->startY);
        }
        UpdateBoxSelection(impl_->currentX, impl_->currentY);
    }
    
    return true;
}

bool SelectionHandler::HandleMouseUp(const InteractionEvent& event) {
    if (event.button != MouseButton::kLeft || !impl_->isSelecting) {
        return false;
    }
    
    impl_->currentX = event.screenX;
    impl_->currentY = event.screenY;
    
    double dx = std::abs(impl_->currentX - impl_->startX);
    double dy = std::abs(impl_->currentY - impl_->startY);
    
    if (impl_->isBoxSelecting) {
        EndBoxSelection();
    } else if (dx <= impl_->params.tolerance && dy <= impl_->params.tolerance) {
        PerformPointSelection(impl_->startX, impl_->startY, impl_->isShiftPressed || impl_->isCtrlPressed);
    }
    
    impl_->isSelecting = false;
    impl_->isBoxSelecting = false;
    
    return true;
}

bool SelectionHandler::HandleKeyDown(const InteractionEvent& event) {
    switch (event.key) {
        case 27:  // Escape
            CancelSelection();
            return true;
        case 'A':
        case 'a':
            if (impl_->isCtrlPressed) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

void SelectionHandler::StartBoxSelection(double screenX, double screenY) {
    if (impl_->params.showFeedback && impl_->feedbackManager) {
        auto item = FeedbackItem::Create(FeedbackType::kSelection);
        if (item) {
            item->SetEnvelope(Envelope(screenX, screenY, screenX, screenY));
            FeedbackConfig config;
            config.strokeColor = impl_->params.style.strokeColor;
            config.strokeWidth = impl_->params.style.strokeWidth;
            config.opacity = impl_->params.style.strokeOpacity;
            config.fillColor = impl_->params.style.fillColor;
            config.visible = true;
            item->SetConfig(config);
            impl_->feedbackId = impl_->feedbackManager->AddFeedback(item);
        }
    }
}

void SelectionHandler::UpdateBoxSelection(double screenX, double screenY) {
    if (impl_->params.showFeedback && impl_->feedbackManager && impl_->feedbackId > 0) {
        auto item = impl_->feedbackManager->GetFeedback(impl_->feedbackId);
        if (item) {
            double minX = std::min(impl_->startX, screenX);
            double minY = std::min(impl_->startY, screenY);
            double maxX = std::max(impl_->startX, screenX);
            double maxY = std::max(impl_->startY, screenY);
            item->SetEnvelope(Envelope(minX, minY, maxX, maxY));
        }
    }
}

void SelectionHandler::EndBoxSelection() {
    double minX = std::min(impl_->startX, impl_->currentX);
    double minY = std::min(impl_->startY, impl_->currentY);
    double maxX = std::max(impl_->startX, impl_->currentX);
    double maxY = std::max(impl_->startY, impl_->currentY);
    
    Envelope screenEnvelope(minX, minY, maxX, maxY);
    
    Coordinate worldMin = ScreenToWorld(minX, maxY);
    Coordinate worldMax = ScreenToWorld(maxX, minY);
    Envelope worldEnvelope(worldMin.x, worldMin.y, worldMax.x, worldMax.y);
    
    ClearSelectionFeedback();
    
    PerformBoxSelection(worldEnvelope, impl_->isShiftPressed || impl_->isCtrlPressed);
}

void SelectionHandler::CancelSelection() {
    impl_->isSelecting = false;
    impl_->isBoxSelecting = false;
    ClearSelectionFeedback();
}

void SelectionHandler::PerformPointSelection(double screenX, double screenY, bool addToSelection) {
    if (!impl_->layerManager || !impl_->hitTester) {
        return;
    }
    
    auto result = impl_->hitTester->HitTest(impl_->layerManager, screenX, screenY, impl_->params.tolerance);
    
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
    if (!impl_->layerManager || !impl_->hitTester) {
        return;
    }
    
    auto result = impl_->hitTester->HitTest(impl_->layerManager, envelope, HitTestMode::kBox);
    
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
    if (!impl_->params.showFeedback || !impl_->feedbackManager || impl_->selectedFeatures.empty()) {
        return;
    }
    
    ClearSelectionFeedback();
    
    auto item = FeedbackItem::Create(FeedbackType::kSelection);
    if (item) {
        FeedbackConfig config;
        config.strokeColor = impl_->params.style.strokeColor;
        config.strokeWidth = impl_->params.style.strokeWidth;
        config.opacity = impl_->params.style.strokeOpacity;
        config.fillColor = impl_->params.style.fillColor;
        config.visible = true;
        item->SetConfig(config);
        
        impl_->feedbackId = impl_->feedbackManager->AddFeedback(item);
    }
}

void SelectionHandler::ClearSelectionFeedback() {
    if (impl_->feedbackManager && impl_->feedbackId > 0) {
        impl_->feedbackManager->RemoveFeedback(impl_->feedbackId);
        impl_->feedbackId = 0;
    }
}

void SelectionHandler::NotifySelectionChanged() {
    if (impl_->selectionChangedCallback) {
        impl_->selectionChangedCallback(impl_->selectedFeatures);
    }
}

}  
}  
