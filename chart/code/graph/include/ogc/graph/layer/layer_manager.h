#ifndef OGC_GRAPH_LAYER_MANAGER_H
#define OGC_GRAPH_LAYER_MANAGER_H

#include "ogc/graph/export.h"
#include <ogc/draw/draw_result.h>
#include "ogc/geom/envelope.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {

class CNLayer;

namespace graph {

class DrawContext;
using DrawContextPtr = std::shared_ptr<DrawContext>;
class Symbolizer;
class SymbolizerRule;

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

class ILayerRenderer {
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
    ~LayerConfig();
    
    LayerConfig(const LayerConfig& other);
    LayerConfig& operator=(const LayerConfig& other);
    
    const std::string& GetName() const;
    void SetName(const std::string& name);
    
    LayerVisibility GetVisibility() const;
    void SetVisibility(LayerVisibility visibility);
    
    double GetMinScale() const;
    void SetMinScale(double scale);
    
    double GetMaxScale() const;
    void SetMaxScale(double scale);
    
    double GetOpacity() const;
    void SetOpacity(double opacity);
    
    bool IsSelectable() const;
    void SetSelectable(bool selectable);
    
    bool IsEditable() const;
    void SetEditable(bool editable);
    
    bool IsVisibleAtScale(double scale) const;
    
    int GetZOrder() const;
    void SetZOrder(int zOrder);
    
    void SetSymbolizer(std::shared_ptr<Symbolizer> symbolizer);
    std::shared_ptr<Symbolizer> GetSymbolizer() const;
    
    void AddRule(std::shared_ptr<SymbolizerRule> rule);
    void ClearRules();
    const std::vector<std::shared_ptr<SymbolizerRule>>& GetRules() const;
    
    LayerConfigPtr Clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class LayerItem;
using LayerItemPtr = std::unique_ptr<LayerItem>;

class OGC_GRAPH_API LayerItem {
public:
    LayerItem();
    explicit LayerItem(CNLayer* layer, const LayerConfig& config);
    ~LayerItem();
    
    LayerItem(const LayerItem& other);
    LayerItem& operator=(const LayerItem& other);
    
    CNLayer* GetLayer() const;
    void SetLayer(CNLayer* layer);
    
    const LayerConfig& GetConfig() const;
    LayerConfig& GetConfig();
    void SetConfig(const LayerConfig& config);
    
    bool IsVisible() const;
    bool IsVisibleAtScale(double scale) const;
    
    Envelope GetExtent() const;
    
    LayerItemPtr Clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
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
    void SetLayerZOrder(int index, int zOrder);
    int GetLayerZOrder(int index) const;
    void SortLayersByZOrder();
    
    std::vector<LayerItem*> GetVisibleLayers() const;
    std::vector<LayerItem*> GetVisibleLayersAtScale(double scale) const;
    
    Envelope GetExtent() const;
    Envelope GetVisibleExtent() const;
    
    void SetLayerRenderer(int index, std::shared_ptr<ILayerRenderer> renderer);
    std::shared_ptr<ILayerRenderer> GetLayerRenderer(int index) const;
    
    void SetLayerChangedCallback(LayerChangedCallback callback);
    
    void SetCurrentScale(double scale);
    double GetCurrentScale() const;
    
    std::vector<LayerInfo> GetLayerInfos() const;
    
    LayerManagerPtr Clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    void NotifyLayerChanged(int index);
};

}
}

#endif
