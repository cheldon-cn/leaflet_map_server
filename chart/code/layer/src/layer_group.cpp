#include "ogc/layer/layer_group.h"

#include <algorithm>

namespace ogc {

struct CNLayerWrapper::Impl {
    std::unique_ptr<CNLayer> layer;
    bool visible = true;
    int z_order = 0;
    CNLayerNode* parent = nullptr;
};

CNLayerWrapper::CNLayerWrapper(std::unique_ptr<CNLayer> layer)
    : impl_(std::make_unique<Impl>()) {
    impl_->layer = std::move(layer);
}

CNLayerWrapper::~CNLayerWrapper() = default;

const std::string& CNLayerWrapper::GetName() const {
    static const std::string empty;
    return impl_->layer ? impl_->layer->GetName() : empty;
}

void CNLayerWrapper::SetName(const std::string& name) {
    (void)name;
}

bool CNLayerWrapper::IsVisible() const {
    return impl_->visible;
}

void CNLayerWrapper::SetVisible(bool visible) {
    impl_->visible = visible;
}

CNLayerNode* CNLayerWrapper::GetParent() {
    return impl_->parent;
}

const CNLayerNode* CNLayerWrapper::GetParent() const {
    return impl_->parent;
}

void CNLayerWrapper::SetParent(CNLayerNode* parent) {
    impl_->parent = parent;
}

CNLayer* CNLayerWrapper::GetLayer() {
    return impl_->layer.get();
}

const CNLayer* CNLayerWrapper::GetLayer() const {
    return impl_->layer.get();
}

int CNLayerWrapper::GetZOrder() const {
    return impl_->z_order;
}

void CNLayerWrapper::SetZOrder(int z_order) {
    impl_->z_order = z_order;
}

struct CNLayerGroup::Impl {
    std::string name;
    bool visible = true;
    int z_order = 0;
    CNLayerNode* parent = nullptr;
    std::vector<std::unique_ptr<CNLayerWrapper>> layers;
    std::vector<std::unique_ptr<CNLayerGroup>> groups;
};

CNLayerGroup::CNLayerGroup() : impl_(std::make_unique<Impl>()) {
}

CNLayerGroup::CNLayerGroup(const std::string& name)
    : impl_(std::make_unique<Impl>()) {
    impl_->name = name;
}

CNLayerGroup::CNLayerGroup(CNLayerGroup&& other) noexcept
    : impl_(std::move(other.impl_)) {
}

CNLayerGroup& CNLayerGroup::operator=(CNLayerGroup&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

CNLayerGroup::~CNLayerGroup() = default;

const std::string& CNLayerGroup::GetName() const {
    return impl_->name;
}

void CNLayerGroup::SetName(const std::string& name) {
    impl_->name = name;
}

bool CNLayerGroup::IsVisible() const {
    return impl_->visible;
}

void CNLayerGroup::SetVisible(bool visible) {
    impl_->visible = visible;
}

CNLayerNode* CNLayerGroup::GetParent() {
    return impl_->parent;
}

const CNLayerNode* CNLayerGroup::GetParent() const {
    return impl_->parent;
}

void CNLayerGroup::SetParent(CNLayerNode* node) {
    impl_->parent = node;
}

size_t CNLayerGroup::GetChildCount() const {
    return impl_->layers.size() + impl_->groups.size();
}

CNLayerNode* CNLayerGroup::GetChild(size_t index) {
    if (index < impl_->layers.size()) {
        return impl_->layers[index].get();
    }
    index -= impl_->layers.size();
    if (index < impl_->groups.size()) {
        return impl_->groups[index].get();
    }
    return nullptr;
}

const CNLayerNode* CNLayerGroup::GetChild(size_t index) const {
    if (index < impl_->layers.size()) {
        return impl_->layers[index].get();
    }
    index -= impl_->layers.size();
    if (index < impl_->groups.size()) {
        return impl_->groups[index].get();
    }
    return nullptr;
}

void CNLayerGroup::AddChild(std::unique_ptr<CNLayerNode> child) {
    if (!child) return;

    if (child->GetNodeType() == CNLayerNodeType::kLayer) {
        CNLayerWrapper* wrapper = dynamic_cast<CNLayerWrapper*>(child.get());
        if (wrapper) {
            child.release();
            std::unique_ptr<CNLayerWrapper> owned_wrapper(wrapper);
            owned_wrapper->SetParent(this);
            impl_->layers.push_back(std::move(owned_wrapper));
            return;
        }
    } else if (child->GetNodeType() == CNLayerNodeType::kGroup) {
        CNLayerGroup* group = dynamic_cast<CNLayerGroup*>(child.get());
        if (group) {
            child.release();
            std::unique_ptr<CNLayerGroup> owned_group(group);
            owned_group->SetParent(this);
            impl_->groups.push_back(std::move(owned_group));
            return;
        }
    }
}

void CNLayerGroup::InsertChild(size_t index, std::unique_ptr<CNLayerNode> child) {
    if (!child) return;

    if (child->GetNodeType() == CNLayerNodeType::kLayer) {
        CNLayerWrapper* wrapper = dynamic_cast<CNLayerWrapper*>(child.get());
        if (wrapper && index <= impl_->layers.size()) {
            child.release();
            std::unique_ptr<CNLayerWrapper> owned_wrapper(wrapper);
            owned_wrapper->SetParent(this);
            impl_->layers.insert(impl_->layers.begin() + index, std::move(owned_wrapper));
            return;
        }
    } else if (child->GetNodeType() == CNLayerNodeType::kGroup) {
        CNLayerGroup* group = dynamic_cast<CNLayerGroup*>(child.get());
        if (group) {
            index = (index <= impl_->layers.size() ? impl_->layers.size() : index - impl_->layers.size());
            child.release();
            std::unique_ptr<CNLayerGroup> owned_group(group);
            owned_group->SetParent(this);
            impl_->groups.insert(impl_->groups.begin() + index, std::move(owned_group));
            return;
        }
    }
}

std::unique_ptr<CNLayerNode> CNLayerGroup::RemoveChild(size_t index) {
    if (index < impl_->layers.size()) {
        impl_->layers[index]->SetParent(nullptr);
        auto wrapper = impl_->layers[index].release();
        impl_->layers.erase(impl_->layers.begin() + index);
        return std::unique_ptr<CNLayerNode>(wrapper);
    }
    index -= impl_->layers.size();
    if (index < impl_->groups.size()) {
        impl_->groups[index]->SetParent(nullptr);
        auto group = impl_->groups[index].release();
        impl_->groups.erase(impl_->groups.begin() + index);
        return std::unique_ptr<CNLayerNode>(group);
    }
    return nullptr;
}

CNLayerNode* CNLayerGroup::FindChild(const std::string& name) {
    for (auto& layer : impl_->layers) {
        if (layer->GetName() == name) {
            return layer.get();
        }
    }
    for (auto& group : impl_->groups) {
        if (group->GetName() == name) {
            return group.get();
        }
    }
    return nullptr;
}

void CNLayerGroup::ClearChildren() {
    impl_->layers.clear();
    impl_->groups.clear();
}

int CNLayerGroup::GetLayerCount() const {
    int count = static_cast<int>(impl_->layers.size());
    for (auto& group : impl_->groups) {
        count += group->GetLayerCount();
    }
    return count;
}

CNLayer* CNLayerGroup::GetLayer(size_t index) {
    if (index < impl_->layers.size()) {
        return impl_->layers[index]->GetLayer();
    }
    index -= impl_->layers.size();
    for (auto& group : impl_->groups) {
        if (index < static_cast<size_t>(group->GetLayerCount())) {
            return group->GetLayer(index);
        }
        index -= group->GetLayerCount();
    }
    return nullptr;
}

int CNLayerGroup::GetGroupCount() const {
    return static_cast<int>(impl_->groups.size());
}

CNLayerGroup* CNLayerGroup::GetGroup(size_t index) {
    if (index < impl_->groups.size()) {
        return impl_->groups[index].get();
    }
    return nullptr;
}

const CNLayerGroup* CNLayerGroup::GetGroup(size_t index) const {
    if (index < impl_->groups.size()) {
        return impl_->groups[index].get();
    }
    return nullptr;
}

void CNLayerGroup::AddLayer(std::unique_ptr<CNLayer> layer) {
    if (layer) {
        auto wrapper = std::make_unique<CNLayerWrapper>(std::move(layer));
        wrapper->SetParent(this);
        impl_->layers.push_back(std::move(wrapper));
    }
}

void CNLayerGroup::AddGroup(std::unique_ptr<CNLayerGroup> group) {
    if (group) {
        group->SetParent(this);
        impl_->groups.push_back(std::move(group));
    }
}

CNStatus CNLayerGroup::StartTransaction() {
    for (auto& layer : impl_->layers) {
        CNStatus status = layer->GetLayer()->StartTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : impl_->groups) {
        CNStatus status = group->StartTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

CNStatus CNLayerGroup::CommitTransaction() {
    for (auto& layer : impl_->layers) {
        CNStatus status = layer->GetLayer()->CommitTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : impl_->groups) {
        CNStatus status = group->CommitTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

CNStatus CNLayerGroup::RollbackTransaction() {
    for (auto& layer : impl_->layers) {
        CNStatus status = layer->GetLayer()->RollbackTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : impl_->groups) {
        CNStatus status = group->RollbackTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

int CNLayerGroup::GetZOrder() const {
    return impl_->z_order;
}

void CNLayerGroup::SetZOrder(int z_order) {
    impl_->z_order = z_order;
}

void CNLayerGroup::SortByZOrder() {
    std::stable_sort(impl_->layers.begin(), impl_->layers.end(),
        [](const std::unique_ptr<CNLayerWrapper>& a,
           const std::unique_ptr<CNLayerWrapper>& b) {
            return a->GetZOrder() < b->GetZOrder();
        });
    std::stable_sort(impl_->groups.begin(), impl_->groups.end(),
        [](const std::unique_ptr<CNLayerGroup>& a,
           const std::unique_ptr<CNLayerGroup>& b) {
            return a->GetZOrder() < b->GetZOrder();
        });
    for (auto& group : impl_->groups) {
        group->SortByZOrder();
    }
}

} // namespace ogc
