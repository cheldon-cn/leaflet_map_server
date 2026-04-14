#include "ogc/graph/layer/layer_control_panel.h"
#include "ogc/graph/layer/layer_manager.h"
#include <algorithm>
#include <map>

namespace ogc {
namespace graph {

struct LayerControlPanel::Impl {
    LayerManager* layerManager = nullptr;
    
    std::vector<LayerControlInfo> layers;
    std::vector<LayerGroupInfo> groups;
    std::string selectedLayerId;
    std::string currentPreset;
    
    LayerChangedCallback layerChangedCallback;
    OrderChangedCallback orderChangedCallback;
    SelectionChangedCallback selectionChangedCallback;
};

std::unique_ptr<LayerControlPanel> LayerControlPanel::Create() {
    return std::unique_ptr<LayerControlPanel>(new LayerControlPanel());
}

LayerControlPanel::LayerControlPanel()
    : impl_(std::make_unique<Impl>())
{
}

LayerControlPanel::~LayerControlPanel() {
}

void LayerControlPanel::SetLayerManager(LayerManager* manager) {
    impl_->layerManager = manager;
    if (manager) {
        RefreshLayerList();
    }
}

LayerManager* LayerControlPanel::GetLayerManager() const { return impl_->layerManager; }

void LayerControlPanel::RefreshLayerList() {
    impl_->layers.clear();
    
    if (!impl_->layerManager) {
        return;
    }
    
    int count = impl_->layerManager->GetLayerCount();
    
    for (int i = 0; i < count; ++i) {
        const LayerItem* item = impl_->layerManager->GetLayer(i);
        if (!item) {
            continue;
        }
        
        LayerControlInfo info;
        info.layerId = item->GetConfig().GetName();
        info.layerName = item->GetConfig().GetName();
        info.visible = item->IsVisible();
        info.opacity = item->GetConfig().GetOpacity();
        info.order = i;
        info.isLocked = false;
        info.isExpanded = true;
        
        impl_->layers.push_back(info);
    }
}

const std::vector<LayerControlInfo>& LayerControlPanel::GetLayers() const { return impl_->layers; }

void LayerControlPanel::SetLayerVisible(const std::string& layerId, bool visible) {
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0) {
        impl_->layerManager->SetLayerVisibility(index, 
            visible ? LayerVisibility::kVisible : LayerVisibility::kHidden);
    }
    
    auto* layer = FindLayer(layerId);
    if (layer) {
        layer->visible = visible;
        NotifyLayerChanged(layerId);
    }
}

bool LayerControlPanel::IsLayerVisible(const std::string& layerId) const {
    const auto* layer = FindLayer(layerId);
    return layer ? layer->visible : false;
}

void LayerControlPanel::SetLayerOpacity(const std::string& layerId, double opacity) {
    opacity = std::max(0.0, std::min(1.0, opacity));
    
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0) {
        impl_->layerManager->SetLayerOpacity(index, opacity);
    }
    
    auto* layer = FindLayer(layerId);
    if (layer) {
        layer->opacity = opacity;
        NotifyLayerChanged(layerId);
    }
}

double LayerControlPanel::GetLayerOpacity(const std::string& layerId) const {
    const auto* layer = FindLayer(layerId);
    return layer ? layer->opacity : 1.0;
}

void LayerControlPanel::MoveLayerUp(const std::string& layerId) {
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    if (index > 0) {
        impl_->layerManager->MoveLayerUp(index);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerDown(const std::string& layerId) {
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0 && index < impl_->layerManager->GetLayerCount() - 1) {
        impl_->layerManager->MoveLayerDown(index);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerToTop(const std::string& layerId) {
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    if (index > 0) {
        impl_->layerManager->MoveLayer(index, 0);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerToBottom(const std::string& layerId) {
    int index = impl_->layerManager ? impl_->layerManager->GetLayerIndex(layerId) : -1;
    int count = impl_->layerManager ? impl_->layerManager->GetLayerCount() : 0;
    if (index >= 0 && index < count - 1) {
        impl_->layerManager->MoveLayer(index, count - 1);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::SetLayerLocked(const std::string& layerId, bool locked) {
    auto* layer = FindLayer(layerId);
    if (layer) {
        layer->isLocked = locked;
        NotifyLayerChanged(layerId);
    }
}

bool LayerControlPanel::IsLayerLocked(const std::string& layerId) const {
    const auto* layer = FindLayer(layerId);
    return layer ? layer->isLocked : false;
}

void LayerControlPanel::SelectLayer(const std::string& layerId) {
    impl_->selectedLayerId = layerId;
    NotifySelectionChanged(layerId);
}

void LayerControlPanel::DeselectLayer() {
    std::string oldId = impl_->selectedLayerId;
    impl_->selectedLayerId.clear();
    NotifySelectionChanged(oldId);
}

std::string LayerControlPanel::GetSelectedLayer() const { return impl_->selectedLayerId; }

bool LayerControlPanel::IsLayerSelected(const std::string& layerId) const {
    return impl_->selectedLayerId == layerId;
}

void LayerControlPanel::ToggleLayerExpansion(const std::string& layerId) {
    auto* layer = FindLayer(layerId);
    if (layer) {
        layer->isExpanded = !layer->isExpanded;
    }
}

bool LayerControlPanel::IsLayerExpanded(const std::string& layerId) const {
    const auto* layer = FindLayer(layerId);
    return layer ? layer->isExpanded : true;
}

void LayerControlPanel::SetLayerGroup(const std::string& layerId, const std::string& groupId) {
    auto* layer = FindLayer(layerId);
    if (layer) {
        layer->group = groupId;
    }
}

std::string LayerControlPanel::GetLayerGroup(const std::string& layerId) const {
    const auto* layer = FindLayer(layerId);
    return layer ? layer->group : "";
}

void LayerControlPanel::CreateGroup(const std::string& groupId, const std::string& groupName) {
    auto it = std::find_if(impl_->groups.begin(), impl_->groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it == impl_->groups.end()) {
        LayerGroupInfo group;
        group.groupId = groupId;
        group.groupName = groupName;
        impl_->groups.push_back(group);
    }
}

void LayerControlPanel::DeleteGroup(const std::string& groupId) {
    auto it = std::find_if(impl_->groups.begin(), impl_->groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it != impl_->groups.end()) {
        for (auto& layer : impl_->layers) {
            if (layer.group == groupId) {
                layer.group.clear();
            }
        }
        impl_->groups.erase(it);
    }
}

void LayerControlPanel::ToggleGroupExpansion(const std::string& groupId) {
    auto it = std::find_if(impl_->groups.begin(), impl_->groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it != impl_->groups.end()) {
        it->expanded = !it->expanded;
    }
}

bool LayerControlPanel::IsGroupExpanded(const std::string& groupId) const {
    auto it = std::find_if(impl_->groups.begin(), impl_->groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    return it != impl_->groups.end() ? it->expanded : true;
}

const std::vector<LayerGroupInfo>& LayerControlPanel::GetGroups() const { return impl_->groups; }

void LayerControlPanel::SetGroupVisible(const std::string& groupId, bool visible) {
    for (auto& layer : impl_->layers) {
        if (layer.group == groupId) {
            SetLayerVisible(layer.layerId, visible);
        }
    }
}

bool LayerControlPanel::IsGroupVisible(const std::string& groupId) const {
    for (const auto& layer : impl_->layers) {
        if (layer.group == groupId && layer.visible) {
            return true;
        }
    }
    return false;
}

void LayerControlPanel::SetGroupOpacity(const std::string& groupId, double opacity) {
    for (auto& layer : impl_->layers) {
        if (layer.group == groupId) {
            SetLayerOpacity(layer.layerId, opacity);
        }
    }
}

double LayerControlPanel::GetGroupOpacity(const std::string& groupId) const {
    for (const auto& layer : impl_->layers) {
        if (layer.group == groupId) {
            return layer.opacity;
        }
    }
    return 1.0;
}

void LayerControlPanel::ShowAllLayers() {
    for (auto& layer : impl_->layers) {
        SetLayerVisible(layer.layerId, true);
    }
}

void LayerControlPanel::HideAllLayers() {
    for (auto& layer : impl_->layers) {
        SetLayerVisible(layer.layerId, false);
    }
}

void LayerControlPanel::LockAllLayers() {
    for (auto& layer : impl_->layers) {
        layer.isLocked = true;
    }
}

void LayerControlPanel::UnlockAllLayers() {
    for (auto& layer : impl_->layers) {
        layer.isLocked = false;
    }
}

void LayerControlPanel::SetLayerChangedCallback(LayerChangedCallback callback) {
    impl_->layerChangedCallback = callback;
}

void LayerControlPanel::SetOrderChangedCallback(OrderChangedCallback callback) {
    impl_->orderChangedCallback = callback;
}

void LayerControlPanel::SetSelectionChangedCallback(SelectionChangedCallback callback) {
    impl_->selectionChangedCallback = callback;
}

size_t LayerControlPanel::GetLayerCount() const { return impl_->layers.size(); }

bool LayerControlPanel::HasLayer(const std::string& layerId) const {
    return FindLayer(layerId) != nullptr;
}

bool LayerControlPanel::HasLayers() const {
    return !impl_->layers.empty();
}

LayerControlInfo* LayerControlPanel::FindLayer(const std::string& layerId) {
    auto it = std::find_if(impl_->layers.begin(), impl_->layers.end(),
        [&layerId](const LayerControlInfo& info) { return info.layerId == layerId; });
    
    return it != impl_->layers.end() ? &(*it) : nullptr;
}

const LayerControlInfo* LayerControlPanel::FindLayer(const std::string& layerId) const {
    auto it = std::find_if(impl_->layers.begin(), impl_->layers.end(),
        [&layerId](const LayerControlInfo& info) { return info.layerId == layerId; });
    
    return it != impl_->layers.end() ? &(*it) : nullptr;
}

void LayerControlPanel::SortLayersByOrder() {
    std::sort(impl_->layers.begin(), impl_->layers.end(),
        [](const LayerControlInfo& a, const LayerControlInfo& b) { return a.order < b.order; });
}

void LayerControlPanel::SortLayersByName() {
    std::sort(impl_->layers.begin(), impl_->layers.end(),
        [](const LayerControlInfo& a, const LayerControlInfo& b) { return a.layerName < b.layerName; });
}

std::vector<std::string> LayerControlPanel::GetVisibleLayerIds() const {
    std::vector<std::string> ids;
    for (const auto& layer : impl_->layers) {
        if (layer.visible) {
            ids.push_back(layer.layerId);
        }
    }
    return ids;
}

std::vector<std::string> LayerControlPanel::GetAllLayerIds() const {
    std::vector<std::string> ids;
    for (const auto& layer : impl_->layers) {
        ids.push_back(layer.layerId);
    }
    return ids;
}

void LayerControlPanel::ApplyPreset(const std::string& presetName) {
    impl_->currentPreset = presetName;
}

std::string LayerControlPanel::GetCurrentPreset() const { return impl_->currentPreset; }

void LayerControlPanel::SavePreset(const std::string& presetName) {
    impl_->currentPreset = presetName;
}

void LayerControlPanel::DeletePreset(const std::string& presetName) {
    if (impl_->currentPreset == presetName) {
        impl_->currentPreset.clear();
    }
}

std::vector<std::string> LayerControlPanel::GetAvailablePresets() const {
    return std::vector<std::string>();
}

void LayerControlPanel::NotifyLayerChanged(const std::string& layerId) {
    if (impl_->layerChangedCallback) {
        impl_->layerChangedCallback(layerId);
    }
}

void LayerControlPanel::NotifyOrderChanged() {
    if (impl_->orderChangedCallback) {
        impl_->orderChangedCallback();
    }
}

void LayerControlPanel::NotifySelectionChanged(const std::string& layerId) {
    if (impl_->selectionChangedCallback) {
        impl_->selectionChangedCallback(layerId);
    }
}

void LayerControlPanel::UpdateLayerOrder() {
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        impl_->layers[i].order = static_cast<int>(i);
    }
}

}  
}  
