#include "ogc/layer/layer_group.h"

namespace ogc {

CNLayerWrapper::CNLayerWrapper(std::unique_ptr<CNLayer> layer)
    : layer_(std::move(layer)) {
}

const std::string& CNLayerWrapper::GetName() const {
    static const std::string empty;
    return layer_ ? layer_->GetName() : empty;
}

void CNLayerWrapper::SetName(const std::string& name) {
    (void)name;
}

bool CNLayerWrapper::IsVisible() const {
    return visible_;
}

void CNLayerWrapper::SetVisible(bool visible) {
    visible_ = visible;
}

CNLayerNode* CNLayerWrapper::GetParent() {
    return parent_;
}

const CNLayerNode* CNLayerWrapper::GetParent() const {
    return parent_;
}

void CNLayerWrapper::SetParent(CNLayerNode* parent) {
    parent_ = parent;
}

CNLayer* CNLayerWrapper::GetLayer() {
    return layer_.get();
}

const CNLayer* CNLayerWrapper::GetLayer() const {
    return layer_.get();
}

CNLayerGroup::CNLayerGroup(const std::string& name)
    : name_(name) {
}

const std::string& CNLayerGroup::GetName() const {
    return name_;
}

void CNLayerGroup::SetName(const std::string& name) {
    name_ = name;
}

bool CNLayerGroup::IsVisible() const {
    return visible_;
}

void CNLayerGroup::SetVisible(bool visible) {
    visible_ = visible;
}

CNLayerNode* CNLayerGroup::GetParent() {
    return parent_;
}

const CNLayerNode* CNLayerGroup::GetParent() const {
    return parent_;
}

void CNLayerGroup::SetParent(CNLayerNode* node) {
    parent_ = node;
}

size_t CNLayerGroup::GetChildCount() const {
    return layers_.size() + groups_.size();
}

CNLayerNode* CNLayerGroup::GetChild(size_t index) {
    if (index < layers_.size()) {
        return layers_[index].get();
    }
    index -= layers_.size();
    if (index < groups_.size()) {
        return groups_[index].get();
    }
    return nullptr;
}

const CNLayerNode* CNLayerGroup::GetChild(size_t index) const {
    if (index < layers_.size()) {
        return layers_[index].get();
    }
    index -= layers_.size();
    if (index < groups_.size()) {
        return groups_[index].get();
    }
    return nullptr;
}

void CNLayerGroup::AddChild(std::unique_ptr<CNLayerNode> child) {
    if (!child) return;

    if (child->GetNodeType() == CNLayerNodeType::kLayer) {
        CNLayerWrapper* wrapper = dynamic_cast<CNLayerWrapper*>(child.get());
        if (wrapper) {
            auto raw = child.release();
            std::unique_ptr<CNLayerWrapper> owned_wrapper(wrapper);
            owned_wrapper->SetParent(this);
            layers_.push_back(std::move(owned_wrapper));
            return;
        }
    } else if (child->GetNodeType() == CNLayerNodeType::kGroup) {
        CNLayerGroup* group = dynamic_cast<CNLayerGroup*>(child.get());
        if (group) {
            auto raw = child.release();
            std::unique_ptr<CNLayerGroup> owned_group(group);
            owned_group->SetParent(this);
            groups_.push_back(std::move(owned_group));
            return;
        }
    }
}

void CNLayerGroup::InsertChild(size_t index, std::unique_ptr<CNLayerNode> child) {
    if (!child) return;

    if (child->GetNodeType() == CNLayerNodeType::kLayer) {
        CNLayerWrapper* wrapper = dynamic_cast<CNLayerWrapper*>(child.get());
        if (wrapper && index <= layers_.size()) {
            auto raw = child.release();
            std::unique_ptr<CNLayerWrapper> owned_wrapper(wrapper);
            owned_wrapper->SetParent(this);
            layers_.insert(layers_.begin() + index, std::move(owned_wrapper));
            return;
        }
    } else if (child->GetNodeType() == CNLayerNodeType::kGroup) {
        CNLayerGroup* group = dynamic_cast<CNLayerGroup*>(child.get());
        if (group) {
            index = (index <= layers_.size() ? layers_.size() : index - layers_.size());
            auto raw = child.release();
            std::unique_ptr<CNLayerGroup> owned_group(group);
            owned_group->SetParent(this);
            groups_.insert(groups_.begin() + index, std::move(owned_group));
            return;
        }
    }
}

std::unique_ptr<CNLayerNode> CNLayerGroup::RemoveChild(size_t index) {
    if (index < layers_.size()) {
        layers_[index]->SetParent(nullptr);
        auto wrapper = layers_[index].release();
        layers_.erase(layers_.begin() + index);
        return std::unique_ptr<CNLayerNode>(wrapper);
    }
    index -= layers_.size();
    if (index < groups_.size()) {
        groups_[index]->SetParent(nullptr);
        auto group = groups_[index].release();
        groups_.erase(groups_.begin() + index);
        return std::unique_ptr<CNLayerNode>(group);
    }
    return nullptr;
}

CNLayerNode* CNLayerGroup::FindChild(const std::string& name) {
    for (auto& layer : layers_) {
        if (layer->GetName() == name) {
            return layer.get();
        }
    }
    for (auto& group : groups_) {
        if (group->GetName() == name) {
            return group.get();
        }
    }
    return nullptr;
}

void CNLayerGroup::ClearChildren() {
    layers_.clear();
    groups_.clear();
}

int CNLayerGroup::GetLayerCount() const {
    int count = static_cast<int>(layers_.size());
    for (auto& group : groups_) {
        count += group->GetLayerCount();
    }
    return count;
}

CNLayer* CNLayerGroup::GetLayer(size_t index) {
    if (index < layers_.size()) {
        return layers_[index]->GetLayer();
    }
    index -= layers_.size();
    for (auto& group : groups_) {
        if (index < static_cast<size_t>(group->GetLayerCount())) {
            return group->GetLayer(index);
        }
        index -= group->GetLayerCount();
    }
    return nullptr;
}

int CNLayerGroup::GetGroupCount() const {
    return static_cast<int>(groups_.size());
}

CNLayerGroup* CNLayerGroup::GetGroup(size_t index) {
    if (index < groups_.size()) {
        return groups_[index].get();
    }
    return nullptr;
}

const CNLayerGroup* CNLayerGroup::GetGroup(size_t index) const {
    if (index < groups_.size()) {
        return groups_[index].get();
    }
    return nullptr;
}

void CNLayerGroup::AddLayer(std::unique_ptr<CNLayer> layer) {
    if (layer) {
        auto wrapper = std::make_unique<CNLayerWrapper>(std::move(layer));
        wrapper->SetParent(this);
        layers_.push_back(std::move(wrapper));
    }
}

void CNLayerGroup::AddGroup(std::unique_ptr<CNLayerGroup> group) {
    if (group) {
        group->SetParent(this);
        groups_.push_back(std::move(group));
    }
}

CNStatus CNLayerGroup::StartTransaction() {
    for (auto& layer : layers_) {
        CNStatus status = layer->GetLayer()->StartTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : groups_) {
        CNStatus status = group->StartTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

CNStatus CNLayerGroup::CommitTransaction() {
    for (auto& layer : layers_) {
        CNStatus status = layer->GetLayer()->CommitTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : groups_) {
        CNStatus status = group->CommitTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

CNStatus CNLayerGroup::RollbackTransaction() {
    for (auto& layer : layers_) {
        CNStatus status = layer->GetLayer()->RollbackTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    for (auto& group : groups_) {
        CNStatus status = group->RollbackTransaction();
        if (status != CNStatus::kSuccess) {
            return status;
        }
    }
    return CNStatus::kSuccess;
}

} // namespace ogc
