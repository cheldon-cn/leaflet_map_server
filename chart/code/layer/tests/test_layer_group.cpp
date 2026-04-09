#include "gtest/gtest.h"
#include "ogc/layer/layer_group.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/factory.h"

using namespace ogc;

class CNLayerGroupTest : public ::testing::Test {
protected:
    void SetUp() override {
        root_ = std::make_unique<CNLayerGroup>("root");
        
        auto layer1 = std::make_unique<CNMemoryLayer>("layer1", GeomType::kPoint);
        auto* field1 = CreateCNFieldDefn("name");
        field1->SetType(CNFieldType::kString);
        layer1->CreateField(field1);
        
        auto layer2 = std::make_unique<CNMemoryLayer>("layer2", GeomType::kLineString);
        auto* field2 = CreateCNFieldDefn("id");
        field2->SetType(CNFieldType::kInteger);
        layer2->CreateField(field2);
        
        root_->AddLayer(std::move(layer1));
        root_->AddLayer(std::move(layer2));
    }

    std::unique_ptr<CNLayerGroup> root_;
};

TEST_F(CNLayerGroupTest, GetName) {
    EXPECT_EQ(root_->GetName(), "root");
}

TEST_F(CNLayerGroupTest, SetName) {
    root_->SetName("new_name");
    EXPECT_EQ(root_->GetName(), "new_name");
}

TEST_F(CNLayerGroupTest, GetNodeType) {
    EXPECT_EQ(root_->GetNodeType(), CNLayerNodeType::kGroup);
}

TEST_F(CNLayerGroupTest, IsVisible) {
    EXPECT_TRUE(root_->IsVisible());
}

TEST_F(CNLayerGroupTest, SetVisible) {
    root_->SetVisible(false);
    EXPECT_FALSE(root_->IsVisible());
}

TEST_F(CNLayerGroupTest, GetParent) {
    EXPECT_EQ(root_->GetParent(), nullptr);
}

TEST_F(CNLayerGroupTest, GetChildCount) {
    EXPECT_EQ(root_->GetChildCount(), 2);
}

TEST_F(CNLayerGroupTest, GetChild) {
    auto* child0 = root_->GetChild(0);
    ASSERT_NE(child0, nullptr);
    EXPECT_EQ(child0->GetName(), "layer1");
    
    auto* child1 = root_->GetChild(1);
    ASSERT_NE(child1, nullptr);
    EXPECT_EQ(child1->GetName(), "layer2");
}

TEST_F(CNLayerGroupTest, GetLayerCount) {
    EXPECT_EQ(root_->GetLayerCount(), 2);
}

TEST_F(CNLayerGroupTest, GetLayer) {
    auto* layer0 = root_->GetLayer(0);
    ASSERT_NE(layer0, nullptr);
    EXPECT_EQ(layer0->GetName(), "layer1");
    
    auto* layer1 = root_->GetLayer(1);
    ASSERT_NE(layer1, nullptr);
    EXPECT_EQ(layer1->GetName(), "layer2");
}

TEST_F(CNLayerGroupTest, AddGroup) {
    auto subgroup = std::make_unique<CNLayerGroup>("subgroup");
    auto subgroup_ptr = subgroup.get();
    root_->AddGroup(std::move(subgroup));
    
    EXPECT_EQ(root_->GetGroupCount(), 1);
    EXPECT_EQ(root_->GetGroup(0), subgroup_ptr);
    EXPECT_EQ(subgroup_ptr->GetParent(), root_.get());
}

TEST_F(CNLayerGroupTest, NestedGroups) {
    auto subgroup = std::make_unique<CNLayerGroup>("subgroup");
    auto sublayer = std::make_unique<CNMemoryLayer>("sublayer", GeomType::kPolygon);
    subgroup->AddLayer(std::move(sublayer));
    
    root_->AddGroup(std::move(subgroup));
    
    EXPECT_EQ(root_->GetLayerCount(), 3);
    EXPECT_EQ(root_->GetGroupCount(), 1);
}

TEST_F(CNLayerGroupTest, FindChild) {
    auto* found = root_->FindChild("layer1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->GetName(), "layer1");
    
    auto* not_found = root_->FindChild("nonexistent");
    EXPECT_EQ(not_found, nullptr);
}

TEST_F(CNLayerGroupTest, RemoveChild) {
    EXPECT_EQ(root_->GetChildCount(), 2);
    
    auto removed = root_->RemoveChild(0);
    ASSERT_NE(removed, nullptr);
    EXPECT_EQ(removed->GetName(), "layer1");
    EXPECT_EQ(root_->GetChildCount(), 1);
}

TEST_F(CNLayerGroupTest, ClearChildren) {
    EXPECT_EQ(root_->GetChildCount(), 2);
    root_->ClearChildren();
    EXPECT_EQ(root_->GetChildCount(), 0);
}

TEST_F(CNLayerGroupTest, GetGroupCount) {
    EXPECT_EQ(root_->GetGroupCount(), 0);
    
    auto subgroup = std::make_unique<CNLayerGroup>("subgroup");
    root_->AddGroup(std::move(subgroup));
    
    EXPECT_EQ(root_->GetGroupCount(), 1);
}

class CNLayerWrapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto layer = std::make_unique<CNMemoryLayer>("test_layer", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        layer->CreateField(field);
        
        layer_ptr_ = layer.get();
        wrapper_ = std::make_unique<CNLayerWrapper>(std::move(layer));
    }

    CNMemoryLayer* layer_ptr_;
    std::unique_ptr<CNLayerWrapper> wrapper_;
};

TEST_F(CNLayerWrapperTest, GetName) {
    EXPECT_EQ(wrapper_->GetName(), "test_layer");
}

TEST_F(CNLayerWrapperTest, GetNodeType) {
    EXPECT_EQ(wrapper_->GetNodeType(), CNLayerNodeType::kLayer);
}

TEST_F(CNLayerWrapperTest, IsVisible) {
    EXPECT_TRUE(wrapper_->IsVisible());
}

TEST_F(CNLayerWrapperTest, SetVisible) {
    wrapper_->SetVisible(false);
    EXPECT_FALSE(wrapper_->IsVisible());
}

TEST_F(CNLayerWrapperTest, GetParent) {
    EXPECT_EQ(wrapper_->GetParent(), nullptr);
}

TEST_F(CNLayerWrapperTest, SetParent) {
    CNLayerGroup group("parent");
    wrapper_->SetParent(&group);
    EXPECT_EQ(wrapper_->GetParent(), &group);
}

TEST_F(CNLayerWrapperTest, GetLayer) {
    auto* layer = wrapper_->GetLayer();
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->GetName(), "test_layer");
}

TEST_F(CNLayerWrapperTest, GetLayerConst) {
    const auto& const_wrapper = *wrapper_;
    const auto* layer = const_wrapper.GetLayer();
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->GetName(), "test_layer");
}

class CNLayerNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        root_ = std::make_unique<CNLayerGroup>("root");
        
        auto subgroup = std::make_unique<CNLayerGroup>("subgroup");
        auto layer1 = std::make_unique<CNMemoryLayer>("layer1", GeomType::kPoint);
        auto layer2 = std::make_unique<CNMemoryLayer>("layer2", GeomType::kLineString);
        
        subgroup->AddLayer(std::move(layer2));
        root_->AddLayer(std::move(layer1));
        root_->AddGroup(std::move(subgroup));
    }

    std::unique_ptr<CNLayerGroup> root_;
};

TEST_F(CNLayerNodeTest, NodeTypeCheck) {
    EXPECT_EQ(root_->GetNodeType(), CNLayerNodeType::kGroup);
    
    auto* child = root_->GetChild(0);
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->GetNodeType(), CNLayerNodeType::kLayer);
    
    auto* group = root_->GetGroup(0);
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->GetNodeType(), CNLayerNodeType::kGroup);
}

TEST_F(CNLayerNodeTest, ParentChildRelationship) {
    auto* child = root_->GetChild(0);
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->GetParent(), root_.get());
    
    auto* group = root_->GetGroup(0);
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->GetParent(), root_.get());
}
