#include "ogc/draw/layer_control_panel.h"
#include "ogc/draw/layer_manager.h"
#include <algorithm>
#include <map>

namespace ogc {
namespace draw {

std::unique_ptr<LayerControlPanel> LayerControlPanel::Create() {
    return std::unique_ptr<LayerControlPanel>(new LayerControlPanel());
}

LayerControlPanel::LayerControlPanel() {
}

LayerControlPanel::~LayerControlPanel() {
}

void LayerControlPanel::SetLayerManager(LayerManager* manager) {
    m_layerManager = manager;
    if (manager) {
        RefreshLayerList();
    }
}

void LayerControlPanel::RefreshLayerList() {
    m_layers.clear();
    
    if (!m_layerManager) {
        return;
    }
    
    int count = m_layerManager->GetLayerCount();
    
    for (int i = 0; i < count; ++i) {
        const LayerItem* item = m_layerManager->GetLayer(i);
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
        
        m_layers.push_back(info);
    }
}

void LayerControlPanel::SetLayerVisible(const std::string& layerId, bool visible) {
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0) {
        m_layerManager->SetLayerVisibility(index, 
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
    
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0) {
        m_layerManager->SetLayerOpacity(index, opacity);
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
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    if (index > 0) {
        m_layerManager->MoveLayerUp(index);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerDown(const std::string& layerId) {
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    if (index >= 0 && index < m_layerManager->GetLayerCount() - 1) {
        m_layerManager->MoveLayerDown(index);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerToTop(const std::string& layerId) {
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    if (index > 0) {
        m_layerManager->MoveLayer(index, 0);
        RefreshLayerList();
        NotifyOrderChanged();
    }
}

void LayerControlPanel::MoveLayerToBottom(const std::string& layerId) {
    int index = m_layerManager ? m_layerManager->GetLayerIndex(layerId) : -1;
    int count = m_layerManager ? m_layerManager->GetLayerCount() : 0;
    if (index >= 0 && index < count - 1) {
        m_layerManager->MoveLayer(index, count - 1);
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
    m_selectedLayerId = layerId;
    NotifySelectionChanged(layerId);
}

void LayerControlPanel::DeselectLayer() {
    std::string oldId = m_selectedLayerId;
    m_selectedLayerId.clear();
    NotifySelectionChanged(oldId);
}

bool LayerControlPanel::IsLayerSelected(const std::string& layerId) const {
    return m_selectedLayerId == layerId;
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
    auto it = std::find_if(m_groups.begin(), m_groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it == m_groups.end()) {
        LayerGroupInfo group;
        group.groupId = groupId;
        group.groupName = groupName;
        m_groups.push_back(group);
    }
}

void LayerControlPanel::DeleteGroup(const std::string& groupId) {
    auto it = std::find_if(m_groups.begin(), m_groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it != m_groups.end()) {
        for (auto& layer : m_layers) {
            if (layer.group == groupId) {
                layer.group.clear();
            }
        }
        m_groups.erase(it);
    }
}

void LayerControlPanel::ToggleGroupExpansion(const std::string& groupId) {
    auto it = std::find_if(m_groups.begin(), m_groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    if (it != m_groups.end()) {
        it->expanded = !it->expanded;
    }
}

bool LayerControlPanel::IsGroupExpanded(const std::string& groupId) const {
    auto it = std::find_if(m_groups.begin(), m_groups.end(),
        [&groupId](const LayerGroupInfo& info) { return info.groupId == groupId; });
    
    return it != m_groups.end() ? it->expanded : true;
}

void LayerControlPanel::SetGroupVisible(const std::string& groupId, bool visible) {
    for (auto& layer : m_layers) {
        if (layer.group == groupId) {
            SetLayerVisible(layer.layerId, visible);
        }
    }
}

void LayerControlPanel::SetGroupOpacity(const std::string& groupId, double opacity) {
    for (auto& layer : m_layers) {
        if (layer.group == groupId) {
            SetLayerOpacity(layer.layerId, opacity);
        }
    }
}

void LayerControlPanel::ShowAllLayers() {
    for (auto& layer : m_layers) {
        SetLayerVisible(layer.layerId, true);
    }
}

void LayerControlPanel::HideAllLayers() {
    for (auto& layer : m_layers) {
        SetLayerVisible(layer.layerId, false);
    }
}

void LayerControlPanel::LockAllLayers() {
    for (auto& layer : m_layers) {
        layer.isLocked = true;
    }
}

void LayerControlPanel::UnlockAllLayers() {
    for (auto& layer : m_layers) {
        layer.isLocked = false;
    }
}

void LayerControlPanel::SetLayerChangedCallback(LayerChangedCallback callback) {
    m_layerChangedCallback = callback;
}

void LayerControlPanel::SetOrderChangedCallback(OrderChangedCallback callback) {
    m_orderChangedCallback = callback;
}

void LayerControlPanel::SetSelectionChangedCallback(SelectionChangedCallback callback) {
    m_selectionChangedCallback = callback;
}

LayerControlInfo* LayerControlPanel::FindLayer(const std::string& layerId) {
    auto it = std::find_if(m_layers.begin(), m_layers.end(),
        [&layerId](const LayerControlInfo& info) { return info.layerId == layerId; });
    
    return it != m_layers.end() ? &(*it) : nullptr;
}

const LayerControlInfo* LayerControlPanel::FindLayer(const std::string& layerId) const {
    auto it = std::find_if(m_layers.begin(), m_layers.end(),
        [&layerId](const LayerControlInfo& info) { return info.layerId == layerId; });
    
    return it != m_layers.end() ? &(*it) : nullptr;
}

void LayerControlPanel::SortLayersByOrder() {
    std::sort(m_layers.begin(), m_layers.end(),
        [](const LayerControlInfo& a, const LayerControlInfo& b) { return a.order < b.order; });
}

void LayerControlPanel::SortLayersByName() {
    std::sort(m_layers.begin(), m_layers.end(),
        [](const LayerControlInfo& a, const LayerControlInfo& b) { return a.layerName < b.layerName; });
}

std::vector<std::string> LayerControlPanel::GetVisibleLayerIds() const {
    std::vector<std::string> ids;
    for (const auto& layer : m_layers) {
        if (layer.visible) {
            ids.push_back(layer.layerId);
        }
    }
    return ids;
}

std::vector<std::string> LayerControlPanel::GetAllLayerIds() const {
    std::vector<std::string> ids;
    for (const auto& layer : m_layers) {
        ids.push_back(layer.layerId);
    }
    return ids;
}

void LayerControlPanel::ApplyPreset(const std::string& presetName) {
    m_currentPreset = presetName;
}

void LayerControlPanel::SavePreset(const std::string& presetName) {
    m_currentPreset = presetName;
}

void LayerControlPanel::DeletePreset(const std::string& presetName) {
    if (m_currentPreset == presetName) {
        m_currentPreset.clear();
    }
}

std::vector<std::string> LayerControlPanel::GetAvailablePresets() const {
    return std::vector<std::string>();
}

void LayerControlPanel::NotifyLayerChanged(const std::string& layerId) {
    if (m_layerChangedCallback) {
        m_layerChangedCallback(layerId);
    }
}

void LayerControlPanel::NotifyOrderChanged() {
    if (m_orderChangedCallback) {
        m_orderChangedCallback();
    }
}

void LayerControlPanel::NotifySelectionChanged(const std::string& layerId) {
    if (m_selectionChangedCallback) {
        m_selectionChangedCallback(layerId);
    }
}

void LayerControlPanel::UpdateLayerOrder() {
    for (size_t i = 0; i < m_layers.size(); ++i) {
        m_layers[i].order = static_cast<int>(i);
    }
}

}  
}  
