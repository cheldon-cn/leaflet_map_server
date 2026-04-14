#include "ogc/graph/layer/layer_manager.h"
#include "ogc/layer/layer.h"
#include <algorithm>
#include <mutex>

namespace ogc {
namespace graph {

struct LayerConfig::Impl {
    std::string name;
    LayerVisibility visibility = LayerVisibility::kVisible;
    double minScale = 0.0;
    double maxScale = 0.0;
    double opacity = 1.0;
    bool selectable = true;
    bool editable = false;
    int zOrder = 0;
    std::shared_ptr<Symbolizer> symbolizer;
    std::vector<std::shared_ptr<SymbolizerRule>> rules;
    
    Impl() = default;
    explicit Impl(const std::string& n) : name(n) {}
};

LayerConfig::LayerConfig()
    : impl_(std::make_unique<Impl>())
{
}

LayerConfig::LayerConfig(const std::string& name)
    : impl_(std::make_unique<Impl>(name))
{
}

LayerConfig::~LayerConfig() = default;

LayerConfig::LayerConfig(const LayerConfig& other)
    : impl_(std::make_unique<Impl>(*other.impl_))
{
}

LayerConfig& LayerConfig::operator=(const LayerConfig& other)
{
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

const std::string& LayerConfig::GetName() const { return impl_->name; }
void LayerConfig::SetName(const std::string& name) { impl_->name = name; }

LayerVisibility LayerConfig::GetVisibility() const { return impl_->visibility; }
void LayerConfig::SetVisibility(LayerVisibility visibility) { impl_->visibility = visibility; }

double LayerConfig::GetMinScale() const { return impl_->minScale; }
void LayerConfig::SetMinScale(double scale) { impl_->minScale = scale; }

double LayerConfig::GetMaxScale() const { return impl_->maxScale; }
void LayerConfig::SetMaxScale(double scale) { impl_->maxScale = scale; }

double LayerConfig::GetOpacity() const { return impl_->opacity; }
void LayerConfig::SetOpacity(double opacity) { impl_->opacity = std::max(0.0, std::min(1.0, opacity)); }

bool LayerConfig::IsSelectable() const { return impl_->selectable; }
void LayerConfig::SetSelectable(bool selectable) { impl_->selectable = selectable; }

bool LayerConfig::IsEditable() const { return impl_->editable; }
void LayerConfig::SetEditable(bool editable) { impl_->editable = editable; }

bool LayerConfig::IsVisibleAtScale(double scale) const
{
    if (impl_->visibility != LayerVisibility::kVisible) {
        return false;
    }
    
    if (impl_->minScale > 0 && scale < impl_->minScale) {
        return false;
    }
    
    if (impl_->maxScale > 0 && scale > impl_->maxScale) {
        return false;
    }
    
    return true;
}

int LayerConfig::GetZOrder() const { return impl_->zOrder; }
void LayerConfig::SetZOrder(int zOrder) { impl_->zOrder = zOrder; }

void LayerConfig::SetSymbolizer(std::shared_ptr<Symbolizer> symbolizer)
{
    impl_->symbolizer = symbolizer;
}

std::shared_ptr<Symbolizer> LayerConfig::GetSymbolizer() const { return impl_->symbolizer; }

void LayerConfig::AddRule(std::shared_ptr<SymbolizerRule> rule)
{
    impl_->rules.push_back(rule);
}

void LayerConfig::ClearRules()
{
    impl_->rules.clear();
}

const std::vector<std::shared_ptr<SymbolizerRule>>& LayerConfig::GetRules() const { return impl_->rules; }

LayerConfigPtr LayerConfig::Clone() const
{
    LayerConfigPtr config(new LayerConfig(impl_->name));
    config->impl_->visibility = impl_->visibility;
    config->impl_->minScale = impl_->minScale;
    config->impl_->maxScale = impl_->maxScale;
    config->impl_->opacity = impl_->opacity;
    config->impl_->selectable = impl_->selectable;
    config->impl_->editable = impl_->editable;
    config->impl_->zOrder = impl_->zOrder;
    config->impl_->symbolizer = impl_->symbolizer;
    config->impl_->rules = impl_->rules;
    return config;
}

struct LayerItem::Impl {
    CNLayer* layer = nullptr;
    LayerConfig config;
    
    Impl() = default;
    Impl(CNLayer* l, const LayerConfig& c) : layer(l), config(c) {}
};

LayerItem::LayerItem()
    : impl_(std::make_unique<Impl>())
{
}

LayerItem::LayerItem(CNLayer* layer, const LayerConfig& config)
    : impl_(std::make_unique<Impl>(layer, config))
{
}

LayerItem::~LayerItem() = default;

LayerItem::LayerItem(const LayerItem& other)
    : impl_(std::make_unique<Impl>(*other.impl_))
{
}

LayerItem& LayerItem::operator=(const LayerItem& other)
{
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

CNLayer* LayerItem::GetLayer() const { return impl_->layer; }
void LayerItem::SetLayer(CNLayer* layer) { impl_->layer = layer; }

const LayerConfig& LayerItem::GetConfig() const { return impl_->config; }
LayerConfig& LayerItem::GetConfig() { return impl_->config; }
void LayerItem::SetConfig(const LayerConfig& config) { impl_->config = config; }

bool LayerItem::IsVisible() const
{
    return impl_->config.GetVisibility() == LayerVisibility::kVisible;
}

bool LayerItem::IsVisibleAtScale(double scale) const
{
    return impl_->config.IsVisibleAtScale(scale);
}

Envelope LayerItem::GetExtent() const
{
    if (!impl_->layer) {
        return Envelope();
    }
    
    Envelope extent;
    impl_->layer->GetExtent(extent, false);
    return extent;
}

LayerItemPtr LayerItem::Clone() const
{
    return LayerItemPtr(new LayerItem(impl_->layer, impl_->config));
}

struct LayerManager::Impl {
    std::vector<LayerItemPtr> layers;
    std::vector<std::shared_ptr<ILayerRenderer>> renderers;
    LayerChangedCallback layerChangedCallback;
    double currentScale = 0.0;
    mutable std::mutex mutex;
};

LayerManager::LayerManager()
    : impl_(std::make_unique<Impl>())
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
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    LayerConfig actualConfig = config;
    if (actualConfig.GetName().empty()) {
        actualConfig.SetName(layer->GetName());
    }
    
    impl_->layers.push_back(LayerItemPtr(new LayerItem(layer, actualConfig)));
    impl_->renderers.push_back(nullptr);
    
    int index = static_cast<int>(impl_->layers.size() - 1);
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index < 0 || index >= static_cast<int>(impl_->layers.size())) {
        return;
    }
    
    impl_->layers.erase(impl_->layers.begin() + index);
    impl_->renderers.erase(impl_->renderers.begin() + index);
    
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->layers.clear();
    impl_->renderers.clear();
    
    NotifyLayerChanged(-1);
}

int LayerManager::GetLayerCount() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return static_cast<int>(impl_->layers.size());
}

LayerItem* LayerManager::GetLayer(int index)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index < 0 || index >= static_cast<int>(impl_->layers.size())) {
        return nullptr;
    }
    
    return impl_->layers[index].get();
}

const LayerItem* LayerManager::GetLayer(int index) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index < 0 || index >= static_cast<int>(impl_->layers.size())) {
        return nullptr;
    }
    
    return impl_->layers[index].get();
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i]->GetConfig().GetName() == name) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

void LayerManager::MoveLayer(int fromIndex, int toIndex)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (fromIndex < 0 || fromIndex >= static_cast<int>(impl_->layers.size()) ||
        toIndex < 0 || toIndex >= static_cast<int>(impl_->layers.size())) {
        return;
    }
    
    if (fromIndex == toIndex) {
        return;
    }
    
    LayerItemPtr layer = std::move(impl_->layers[fromIndex]);
    std::shared_ptr<ILayerRenderer> renderer = std::move(impl_->renderers[fromIndex]);
    
    impl_->layers.erase(impl_->layers.begin() + fromIndex);
    impl_->renderers.erase(impl_->renderers.begin() + fromIndex);
    
    int insertIndex = toIndex;
    if (toIndex > fromIndex) {
        insertIndex--;
    }
    
    impl_->layers.insert(impl_->layers.begin() + insertIndex, std::move(layer));
    impl_->renderers.insert(impl_->renderers.begin() + insertIndex, std::move(renderer));
    
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->layers.size())) {
        impl_->layers[index]->GetConfig().SetVisibility(visibility);
        NotifyLayerChanged(index);
    }
}

void LayerManager::SetLayerOpacity(int index, double opacity)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->layers.size())) {
        impl_->layers[index]->GetConfig().SetOpacity(opacity);
        NotifyLayerChanged(index);
    }
}

void LayerManager::SetLayerScaleRange(int index, double minScale, double maxScale)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->layers.size())) {
        impl_->layers[index]->GetConfig().SetMinScale(minScale);
        impl_->layers[index]->GetConfig().SetMaxScale(maxScale);
        NotifyLayerChanged(index);
    }
}

void LayerManager::SetLayerZOrder(int index, int zOrder)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->layers.size())) {
        impl_->layers[index]->GetConfig().SetZOrder(zOrder);
        NotifyLayerChanged(index);
    }
}

int LayerManager::GetLayerZOrder(int index) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->layers.size())) {
        return impl_->layers[index]->GetConfig().GetZOrder();
    }
    return 0;
}

void LayerManager::SortLayersByZOrder()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    std::stable_sort(impl_->layers.begin(), impl_->layers.end(),
        [](const LayerItemPtr& a, const LayerItemPtr& b) {
            return a->GetConfig().GetZOrder() < b->GetConfig().GetZOrder();
        });
    
    NotifyLayerChanged(-1);
}

std::vector<LayerItem*> LayerManager::GetVisibleLayers() const
{
    std::vector<LayerItem*> result;
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i]->IsVisible()) {
            result.push_back(impl_->layers[i].get());
        }
    }
    
    return result;
}

std::vector<LayerItem*> LayerManager::GetVisibleLayersAtScale(double scale) const
{
    std::vector<LayerItem*> result;
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i]->IsVisibleAtScale(scale)) {
            result.push_back(impl_->layers[i].get());
        }
    }
    
    return result;
}

Envelope LayerManager::GetExtent() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    Envelope result;
    bool first = true;
    
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        Envelope layerExtent = impl_->layers[i]->GetExtent();
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    Envelope result;
    bool first = true;
    
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i]->IsVisible()) {
            Envelope layerExtent = impl_->layers[i]->GetExtent();
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->renderers.size())) {
        impl_->renderers[index] = renderer;
    }
}

std::shared_ptr<ILayerRenderer> LayerManager::GetLayerRenderer(int index) const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index >= 0 && index < static_cast<int>(impl_->renderers.size())) {
        return impl_->renderers[index];
    }
    
    return nullptr;
}

void LayerManager::SetLayerChangedCallback(LayerChangedCallback callback)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->layerChangedCallback = callback;
}

void LayerManager::SetCurrentScale(double scale)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->currentScale = scale;
}

double LayerManager::GetCurrentScale() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentScale;
}

std::vector<LayerInfo> LayerManager::GetLayerInfos() const
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    std::vector<LayerInfo> result;
    result.reserve(impl_->layers.size());
    
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        const LayerItem* item = impl_->layers[i].get();
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    LayerManagerPtr manager(new LayerManager());
    manager->impl_->currentScale = impl_->currentScale;
    manager->impl_->layerChangedCallback = impl_->layerChangedCallback;
    
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        manager->impl_->layers.push_back(impl_->layers[i]->Clone());
    }
    
    manager->impl_->renderers = impl_->renderers;
    
    return manager;
}

void LayerManager::NotifyLayerChanged(int index)
{
    if (impl_->layerChangedCallback) {
        impl_->layerChangedCallback(index);
    }
}

}
}
