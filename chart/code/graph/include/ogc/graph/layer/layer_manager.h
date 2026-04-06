#ifndef OGC_GRAPH_LAYER_MANAGER_H
#define OGC_GRAPH_LAYER_MANAGER_H

#include "ogc/graph/export.h"
#include <ogc/draw/draw_result.h>
#include "ogc/envelope.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace ogc {

class CNLayer;

namespace graph {

class DrawContext;
using DrawContextPtr = std::shared_ptr<DrawContext>;

enum class LayerVisibility {
    kVisible,
    kHidden,
    kConditional
};

struct LayerInfo {
    std::string name;
    int index;
    LayerVisibility visibility;
    double minScale;
    double maxScale;
    double opacity;
    bool selectable;
    bool editable;
    bool hasLabels;
};

class OGC_GRAPH_API ILayerRenderer {
public:
    virtual ~ILayerRenderer() = default;
    
    virtual ogc::draw::DrawResult Render(CNLayer* layer, DrawContext& context) = 0;
    virtual ogc::draw::DrawResult RenderSelection(CNLayer* layer, DrawContext& context, 
                                        const std::vector<int64_t>& featureIds) = 0;
    virtual ogc::draw::DrawResult RenderLabels(CNLayer* layer, DrawContext& context) = 0;
};

class LayerConfig;
using LayerConfigPtr = std::unique_ptr<LayerConfig>;

class OGC_GRAPH_API LayerConfig {
public:
    LayerConfig();
    explicit LayerConfig(const std::string& name);
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    LayerVisibility GetVisibility() const { return m_visibility; }
    void SetVisibility(LayerVisibility visibility) { m_visibility = visibility; }
    
    double GetMinScale() const { return m_minScale; }
    void SetMinScale(double scale) { m_minScale = scale; }
    
    double GetMaxScale() const { return m_maxScale; }
    void SetMaxScale(double scale) { m_maxScale = scale; }
    
    double GetOpacity() const { return m_opacity; }
    void SetOpacity(double opacity) { m_opacity = std::max(0.0, std::min(1.0, opacity)); }
    
    bool IsSelectable() const { return m_selectable; }
    void SetSelectable(bool selectable) { m_selectable = selectable; }
    
    bool IsEditable() const { return m_editable; }
    void SetEditable(bool editable) { m_editable = editable; }
    
    bool IsVisibleAtScale(double scale) const;
    
    void SetSymbolizer(std::shared_ptr<void> symbolizer);
    std::shared_ptr<void> GetSymbolizer() const { return m_symbolizer; }
    
    void AddRule(std::shared_ptr<void> rule);
    void ClearRules();
    const std::vector<std::shared_ptr<void>>& GetRules() const { return m_rules; }
    
    LayerConfigPtr Clone() const;
    
private:
    std::string m_name;
    LayerVisibility m_visibility;
    double m_minScale;
    double m_maxScale;
    double m_opacity;
    bool m_selectable;
    bool m_editable;
    std::shared_ptr<void> m_symbolizer;
    std::vector<std::shared_ptr<void>> m_rules;
};

class LayerItem;
using LayerItemPtr = std::unique_ptr<LayerItem>;

class OGC_GRAPH_API LayerItem {
public:
    LayerItem();
    explicit LayerItem(CNLayer* layer, const LayerConfig& config);
    
    CNLayer* GetLayer() const { return m_layer; }
    void SetLayer(CNLayer* layer) { m_layer = layer; }
    
    const LayerConfig& GetConfig() const { return m_config; }
    LayerConfig& GetConfig() { return m_config; }
    void SetConfig(const LayerConfig& config) { m_config = config; }
    
    bool IsVisible() const;
    bool IsVisibleAtScale(double scale) const;
    
    Envelope GetExtent() const;
    
    LayerItemPtr Clone() const;
    
private:
    CNLayer* m_layer;
    LayerConfig m_config;
};

class LayerManager;
using LayerManagerPtr = std::unique_ptr<LayerManager>;

class OGC_GRAPH_API LayerManager {
public:
    using LayerChangedCallback = std::function<void(int index)>;
    
    LayerManager();
    ~LayerManager();
    
    int AddLayer(CNLayer* layer, const LayerConfig& config = LayerConfig());
    int AddLayer(CNLayer* layer, const std::string& name);
    
    void RemoveLayer(int index);
    void RemoveLayer(const std::string& name);
    void RemoveAllLayers();
    
    int GetLayerCount() const;
    
    LayerItem* GetLayer(int index);
    const LayerItem* GetLayer(int index) const;
    LayerItem* GetLayer(const std::string& name);
    const LayerItem* GetLayer(const std::string& name) const;
    
    int GetLayerIndex(const std::string& name) const;
    
    void MoveLayer(int fromIndex, int toIndex);
    void MoveLayerUp(int index);
    void MoveLayerDown(int index);
    
    void SetLayerVisibility(int index, LayerVisibility visibility);
    void SetLayerOpacity(int index, double opacity);
    void SetLayerScaleRange(int index, double minScale, double maxScale);
    
    std::vector<LayerItem*> GetVisibleLayers() const;
    std::vector<LayerItem*> GetVisibleLayersAtScale(double scale) const;
    
    Envelope GetExtent() const;
    Envelope GetVisibleExtent() const;
    
    void SetLayerRenderer(int index, std::shared_ptr<ILayerRenderer> renderer);
    std::shared_ptr<ILayerRenderer> GetLayerRenderer(int index) const;
    
    void SetLayerChangedCallback(LayerChangedCallback callback);
    
    void SetCurrentScale(double scale);
    double GetCurrentScale() const { return m_currentScale; }
    
    std::vector<LayerInfo> GetLayerInfos() const;
    
    LayerManagerPtr Clone() const;
    
private:
    std::vector<LayerItemPtr> m_layers;
    std::vector<std::shared_ptr<ILayerRenderer>> m_renderers;
    LayerChangedCallback m_layerChangedCallback;
    double m_currentScale;
    mutable std::mutex m_mutex;
    
    void NotifyLayerChanged(int index);
};

}
}

#endif
