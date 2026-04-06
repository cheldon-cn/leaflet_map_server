#include "ogc/graph/layer/layer_manager.h"
#include "ogc/layer/layer.h"
#include <algorithm>

namespace ogc {
namespace graph {

LayerConfig::LayerConfig()
    : m_visibility(LayerVisibility::kVisible)
    , m_minScale(0.0)
    , m_maxScale(0.0)
    , m_opacity(1.0)
    , m_selectable(true)
    , m_editable(false)
{
}

LayerConfig::LayerConfig(const std::string& name)
    : m_name(name)
    , m_visibility(LayerVisibility::kVisible)
    , m_minScale(0.0)
    , m_maxScale(0.0)
    , m_opacity(1.0)
    , m_selectable(true)
    , m_editable(false)
{
}

bool LayerConfig::IsVisibleAtScale(double scale) const
{
    if (m_visibility != LayerVisibility::kVisible) {
        return false;
    }
    
    if (m_minScale > 0 && scale < m_minScale) {
        return false;
    }
    
    if (m_maxScale > 0 && scale > m_maxScale) {
        return false;
    }
    
    return true;
}

void LayerConfig::SetSymbolizer(std::shared_ptr<void> symbolizer)
{
    m_symbolizer = symbolizer;
}

void LayerConfig::AddRule(std::shared_ptr<void> rule)
{
    m_rules.push_back(rule);
}

void LayerConfig::ClearRules()
{
    m_rules.clear();
}

LayerConfigPtr LayerConfig::Clone() const
{
    LayerConfigPtr config(new LayerConfig(m_name));
    config->m_visibility = m_visibility;
    config->m_minScale = m_minScale;
    config->m_maxScale = m_maxScale;
    config->m_opacity = m_opacity;
    config->m_selectable = m_selectable;
    config->m_editable = m_editable;
    config->m_symbolizer = m_symbolizer;
    config->m_rules = m_rules;
    return config;
}

LayerItem::LayerItem()
    : m_layer(nullptr)
{
}

LayerItem::LayerItem(CNLayer* layer, const LayerConfig& config)
    : m_layer(layer)
    , m_config(config)
{
}

bool LayerItem::IsVisible() const
{
    return m_config.GetVisibility() == LayerVisibility::kVisible;
}

bool LayerItem::IsVisibleAtScale(double scale) const
{
    return m_config.IsVisibleAtScale(scale);
}

Envelope LayerItem::GetExtent() const
{
    if (!m_layer) {
        return Envelope();
    }
    
    Envelope extent;
    m_layer->GetExtent(extent, false);
    return extent;
}

LayerItemPtr LayerItem::Clone() const
{
    return LayerItemPtr(new LayerItem(m_layer, m_config));
}

LayerManager::LayerManager()
    : m_currentScale(0.0)
{
}

LayerManager::~LayerManager()
{
}

int LayerManager::AddLayer(CNLayer* layer, const LayerConfig& config)
{
    if (!layer) {
        return -1;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    LayerConfig actualConfig = config;
    if (actualConfig.GetName().empty()) {
        actualConfig.SetName(layer->GetName());
    }
    
    m_layers.push_back(LayerItemPtr(new LayerItem(layer, actualConfig)));
    m_renderers.push_back(nullptr);
    
    int index = static_cast<int>(m_layers.size() - 1);
    NotifyLayerChanged(index);
    
    return index;
}

int LayerManager::AddLayer(CNLayer* layer, const std::string& name)
{
    LayerConfig config(name);
    return AddLayer(layer, config);
}

void LayerManager::RemoveLayer(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index < 0 || index >= static_cast<int>(m_layers.size())) {
        return;
    }
    
    m_layers.erase(m_layers.begin() + index);
    m_renderers.erase(m_renderers.begin() + index);
    
    NotifyLayerChanged(-1);
}

void LayerManager::RemoveLayer(const std::string& name)
{
    int index = GetLayerIndex(name);
    if (index >= 0) {
        RemoveLayer(index);
    }
}

void LayerManager::RemoveAllLayers()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_layers.clear();
    m_renderers.clear();
    
    NotifyLayerChanged(-1);
}

int LayerManager::GetLayerCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_layers.size());
}

LayerItem* LayerManager::GetLayer(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index < 0 || index >= static_cast<int>(m_layers.size())) {
        return nullptr;
    }
    
    return m_layers[index].get();
}

const LayerItem* LayerManager::GetLayer(int index) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index < 0 || index >= static_cast<int>(m_layers.size())) {
        return nullptr;
    }
    
    return m_layers[index].get();
}

LayerItem* LayerManager::GetLayer(const std::string& name)
{
    int index = GetLayerIndex(name);
    if (index >= 0) {
        return GetLayer(index);
    }
    return nullptr;
}

const LayerItem* LayerManager::GetLayer(const std::string& name) const
{
    int index = GetLayerIndex(name);
    if (index >= 0) {
        return GetLayer(index);
    }
    return nullptr;
}

int LayerManager::GetLayerIndex(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->GetConfig().GetName() == name) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

void LayerManager::MoveLayer(int fromIndex, int toIndex)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (fromIndex < 0 || fromIndex >= static_cast<int>(m_layers.size()) ||
        toIndex < 0 || toIndex >= static_cast<int>(m_layers.size())) {
        return;
    }
    
    if (fromIndex == toIndex) {
        return;
    }
    
    LayerItemPtr layer = std::move(m_layers[fromIndex]);
    std::shared_ptr<ILayerRenderer> renderer = std::move(m_renderers[fromIndex]);
    
    m_layers.erase(m_layers.begin() + fromIndex);
    m_renderers.erase(m_renderers.begin() + fromIndex);
    
    int insertIndex = toIndex;
    if (toIndex > fromIndex) {
        insertIndex--;
    }
    
    m_layers.insert(m_layers.begin() + insertIndex, std::move(layer));
    m_renderers.insert(m_renderers.begin() + insertIndex, std::move(renderer));
    
    NotifyLayerChanged(-1);
}

void LayerManager::MoveLayerUp(int index)
{
    if (index > 0) {
        MoveLayer(index, index - 1);
    }
}

void LayerManager::MoveLayerDown(int index)
{
    MoveLayer(index, index + 1);
}

void LayerManager::SetLayerVisibility(int index, LayerVisibility visibility)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index]->GetConfig().SetVisibility(visibility);
        NotifyLayerChanged(index);
    }
}

void LayerManager::SetLayerOpacity(int index, double opacity)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index]->GetConfig().SetOpacity(opacity);
        NotifyLayerChanged(index);
    }
}

void LayerManager::SetLayerScaleRange(int index, double minScale, double maxScale)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index]->GetConfig().SetMinScale(minScale);
        m_layers[index]->GetConfig().SetMaxScale(maxScale);
        NotifyLayerChanged(index);
    }
}

std::vector<LayerItem*> LayerManager::GetVisibleLayers() const
{
    std::vector<LayerItem*> result;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->IsVisible()) {
            result.push_back(m_layers[i].get());
        }
    }
    
    return result;
}

std::vector<LayerItem*> LayerManager::GetVisibleLayersAtScale(double scale) const
{
    std::vector<LayerItem*> result;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->IsVisibleAtScale(scale)) {
            result.push_back(m_layers[i].get());
        }
    }
    
    return result;
}

Envelope LayerManager::GetExtent() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    Envelope result;
    bool first = true;
    
    for (size_t i = 0; i < m_layers.size(); ++i) {
        Envelope layerExtent = m_layers[i]->GetExtent();
        if (first) {
            result = layerExtent;
            first = false;
        } else {
            result.ExpandToInclude(layerExtent);
        }
    }
    
    return result;
}

Envelope LayerManager::GetVisibleExtent() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    Envelope result;
    bool first = true;
    
    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->IsVisible()) {
            Envelope layerExtent = m_layers[i]->GetExtent();
            if (first) {
                result = layerExtent;
                first = false;
            } else {
                result.ExpandToInclude(layerExtent);
            }
        }
    }
    
    return result;
}

void LayerManager::SetLayerRenderer(int index, std::shared_ptr<ILayerRenderer> renderer)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= 0 && index < static_cast<int>(m_renderers.size())) {
        m_renderers[index] = renderer;
    }
}

std::shared_ptr<ILayerRenderer> LayerManager::GetLayerRenderer(int index) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= 0 && index < static_cast<int>(m_renderers.size())) {
        return m_renderers[index];
    }
    
    return nullptr;
}

void LayerManager::SetLayerChangedCallback(LayerChangedCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_layerChangedCallback = callback;
}

void LayerManager::SetCurrentScale(double scale)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentScale = scale;
}

std::vector<LayerInfo> LayerManager::GetLayerInfos() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<LayerInfo> result;
    result.reserve(m_layers.size());
    
    for (size_t i = 0; i < m_layers.size(); ++i) {
        const LayerItem* item = m_layers[i].get();
        LayerInfo info;
        info.name = item->GetConfig().GetName();
        info.index = static_cast<int>(i);
        info.visibility = item->GetConfig().GetVisibility();
        info.minScale = item->GetConfig().GetMinScale();
        info.maxScale = item->GetConfig().GetMaxScale();
        info.opacity = item->GetConfig().GetOpacity();
        info.selectable = item->GetConfig().IsSelectable();
        info.editable = item->GetConfig().IsEditable();
        info.hasLabels = false;
        result.push_back(info);
    }
    
    return result;
}

LayerManagerPtr LayerManager::Clone() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    LayerManagerPtr manager(new LayerManager());
    manager->m_currentScale = m_currentScale;
    manager->m_layerChangedCallback = m_layerChangedCallback;
    
    for (size_t i = 0; i < m_layers.size(); ++i) {
        manager->m_layers.push_back(m_layers[i]->Clone());
    }
    
    manager->m_renderers = m_renderers;
    
    return manager;
}

void LayerManager::NotifyLayerChanged(int index)
{
    if (m_layerChangedCallback) {
        m_layerChangedCallback(index);
    }
}

}
}
