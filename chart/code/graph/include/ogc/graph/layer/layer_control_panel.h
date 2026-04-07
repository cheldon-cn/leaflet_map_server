#ifndef OGC_GRAPH_LAYER_CONTROL_PANEL_H
#define OGC_GRAPH_LAYER_CONTROL_PANEL_H

#include "ogc/graph/export.h"
#include "ogc/graph/layer/layer_manager.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace graph {

struct LayerControlInfo {
    std::string layerId;
    std::string layerName;
    bool visible = true;
    double opacity = 1.0;
    int order = 0;
    bool isLocked = false;
    bool isExpanded = true;
    std::string group;
};

struct LayerGroupInfo {
    std::string groupId;
    std::string groupName;
    bool expanded = true;
    std::vector<std::string> layerIds;
};

class OGC_GRAPH_API LayerControlPanel {
public:
    using LayerChangedCallback = std::function<void(const std::string& layerId)>;
    using OrderChangedCallback = std::function<void()>;
    using SelectionChangedCallback = std::function<void(const std::string& layerId)>;
    
    static std::unique_ptr<LayerControlPanel> Create();
    
    ~LayerControlPanel();
    
    void SetLayerManager(LayerManager* manager);
    LayerManager* GetLayerManager() const { return m_layerManager; }
    
    void RefreshLayerList();
    const std::vector<LayerControlInfo>& GetLayers() const { return m_layers; }
    
    void SetLayerVisible(const std::string& layerId, bool visible);
    bool IsLayerVisible(const std::string& layerId) const;
    
    void SetLayerOpacity(const std::string& layerId, double opacity);
    double GetLayerOpacity(const std::string& layerId) const;
    
    void MoveLayerUp(const std::string& layerId);
    void MoveLayerDown(const std::string& layerId);
    void MoveLayerToTop(const std::string& layerId);
    void MoveLayerToBottom(const std::string& layerId);
    
    void SetLayerLocked(const std::string& layerId, bool locked);
    bool IsLayerLocked(const std::string& layerId) const;
    
    void SelectLayer(const std::string& layerId);
    void DeselectLayer();
    std::string GetSelectedLayer() const { return m_selectedLayerId; }
    bool IsLayerSelected(const std::string& layerId) const;
    
    void ToggleLayerExpansion(const std::string& layerId);
    bool IsLayerExpanded(const std::string& layerId) const;
    
    void SetLayerGroup(const std::string& layerId, const std::string& groupId);
    std::string GetLayerGroup(const std::string& layerId) const;
    
    void CreateGroup(const std::string& groupId, const std::string& groupName);
    void DeleteGroup(const std::string& groupId);
    void ToggleGroupExpansion(const std::string& groupId);
    bool IsGroupExpanded(const std::string& groupId) const;
    const std::vector<LayerGroupInfo>& GetGroups() const { return m_groups; }
    
    void SetGroupVisible(const std::string& groupId, bool visible);
    bool IsGroupVisible(const std::string& groupId) const;
    
    void SetGroupOpacity(const std::string& groupId, double opacity);
    double GetGroupOpacity(const std::string& groupId) const;
    
    void ShowAllLayers();
    void HideAllLayers();
    void LockAllLayers();
    void UnlockAllLayers();
    
    void SetLayerChangedCallback(LayerChangedCallback callback);
    void SetOrderChangedCallback(OrderChangedCallback callback);
    void SetSelectionChangedCallback(SelectionChangedCallback callback);
    
    size_t GetLayerCount() const { return m_layers.size(); }
    bool HasLayer(const std::string& layerId) const;
    
    LayerControlInfo* FindLayer(const std::string& layerId);
    const LayerControlInfo* FindLayer(const std::string& layerId) const;
    
    void SortLayersByOrder();
    void SortLayersByName();
    
    std::vector<std::string> GetVisibleLayerIds() const;
    std::vector<std::string> GetAllLayerIds() const;
    
    void ApplyPreset(const std::string& presetName);
    std::string GetCurrentPreset() const { return m_currentPreset; }
    
    void SavePreset(const std::string& presetName);
    void DeletePreset(const std::string& presetName);
    std::vector<std::string> GetAvailablePresets() const;
    
private:
    LayerControlPanel();
    
    void NotifyLayerChanged(const std::string& layerId);
    void NotifyOrderChanged();
    void NotifySelectionChanged(const std::string& layerId);
    void UpdateLayerOrder();
    
    LayerManager* m_layerManager = nullptr;
    
    std::vector<LayerControlInfo> m_layers;
    std::vector<LayerGroupInfo> m_groups;
    std::string m_selectedLayerId;
    std::string m_currentPreset;
    
    LayerChangedCallback m_layerChangedCallback;
    OrderChangedCallback m_orderChangedCallback;
    SelectionChangedCallback m_selectionChangedCallback;
};

}  
}  

#endif
