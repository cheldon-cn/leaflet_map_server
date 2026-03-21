#pragma once

#include "ogc/layer/layer.h"

#include <memory>
#include <string>
#include <vector>

namespace ogc {

enum class CNLayerNodeType {
    kLayer,
    kGroup
};

class OGC_LAYER_API CNLayerNode {
public:
    virtual ~CNLayerNode() = default;

    virtual CNLayerNodeType GetNodeType() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;

    virtual bool IsVisible() const = 0;
    virtual void SetVisible(bool visible) = 0;

    virtual CNLayerNode* GetParent() = 0;
    virtual const CNLayerNode* GetParent() const = 0;
    virtual void SetParent(CNLayerNode* parent) = 0;
};

class OGC_LAYER_API CNLayerWrapper : public CNLayerNode {
public:
    explicit CNLayerWrapper(std::unique_ptr<CNLayer> layer);
    CNLayerWrapper(const CNLayerWrapper&) = delete;
    CNLayerWrapper& operator=(const CNLayerWrapper&) = delete;
    CNLayerWrapper(CNLayerWrapper&&) = default;
    CNLayerWrapper& operator=(CNLayerWrapper&&) = default;
    
    CNLayerNodeType GetNodeType() const override {
        return CNLayerNodeType::kLayer;
    }
    
    const std::string& GetName() const override;
    void SetName(const std::string& name) override;
    
    bool IsVisible() const override;
    void SetVisible(bool visible) override;
    
    CNLayerNode* GetParent() override;
    const CNLayerNode* GetParent() const override;
    void SetParent(CNLayerNode* parent) override;
    
    CNLayer* GetLayer();
    const CNLayer* GetLayer() const;
    
private:
    std::unique_ptr<CNLayer> layer_;
    bool visible_ = true;
    CNLayerNode* parent_ = nullptr;
};

class OGC_LAYER_API CNLayerGroup : public CNLayerNode {
public:
    CNLayerGroup() = default;
    explicit CNLayerGroup(const std::string& name);
    CNLayerGroup(const CNLayerGroup&) = delete;
    CNLayerGroup& operator=(const CNLayerGroup&) = delete;
    CNLayerGroup(CNLayerGroup&&) = default;
    CNLayerGroup& operator=(CNLayerGroup&&) = default;

    CNLayerNodeType GetNodeType() const override {
        return CNLayerNodeType::kGroup;
    }

    const std::string& GetName() const override;
    void SetName(const std::string& name) override;

    bool IsVisible() const override;
    void SetVisible(bool visible) override;

    CNLayerNode* GetParent() override;
    const CNLayerNode* GetParent() const override;
    void SetParent(CNLayerNode* parent) override;

    size_t GetChildCount() const;
    CNLayerNode* GetChild(size_t index);
    const CNLayerNode* GetChild(size_t index) const;

    void AddChild(std::unique_ptr<CNLayerNode> child);
    void InsertChild(size_t index, std::unique_ptr<CNLayerNode> child);
    std::unique_ptr<CNLayerNode> RemoveChild(size_t index);
    CNLayerNode* FindChild(const std::string& name);
    void ClearChildren();

    int GetLayerCount() const;
    CNLayer* GetLayer(size_t index);

    int GetGroupCount() const;
    CNLayerGroup* GetGroup(size_t index);
    const CNLayerGroup* GetGroup(size_t index) const;

    void AddLayer(std::unique_ptr<CNLayer> layer);
    void AddGroup(std::unique_ptr<CNLayerGroup> group);

    CNStatus StartTransaction();
    CNStatus CommitTransaction();
    CNStatus RollbackTransaction();

private:
    std::string name_;
    bool visible_ = true;
    CNLayerNode* parent_ = nullptr;
    std::vector<std::unique_ptr<CNLayerWrapper>> layers_;
    std::vector<std::unique_ptr<CNLayerGroup>> groups_;
};

using CNLayerGroupPtr = std::unique_ptr<CNLayerGroup>;
using CNLayerNodePtr = std::unique_ptr<CNLayerNode>;

} // namespace ogc
