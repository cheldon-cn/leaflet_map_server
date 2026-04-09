#include <gtest/gtest.h>
#include "ogc/graph/layer/layer_control_panel.h"
#include "ogc/graph/layer/layer_manager.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/geom/geometry.h"

using namespace ogc::graph;
using ogc::CNMemoryLayer;
using ogc::GeomType;

class LayerControlPanelTest : public ::testing::Test {
protected:
    void SetUp() override {
        panel = LayerControlPanel::Create();
        manager = std::make_shared<LayerManager>();
    }
    
    void TearDown() override {
        panel.reset();
        manager.reset();
    }
    
    void AddTestLayer(const std::string& name) {
        auto layer = std::make_shared<CNMemoryLayer>(name, GeomType::kPoint);
        LayerConfig config(name);
        manager->AddLayer(layer.get(), config);
        m_layers.push_back(layer);
    }
    
    std::unique_ptr<LayerControlPanel> panel;
    std::shared_ptr<LayerManager> manager;
    std::vector<std::shared_ptr<CNMemoryLayer>> m_layers;
};

TEST_F(LayerControlPanelTest, CreatePanel) {
    EXPECT_TRUE(panel != nullptr);
}

TEST_F(LayerControlPanelTest, SetLayerManager) {
    panel->SetLayerManager(manager.get());
    EXPECT_EQ(panel->GetLayerManager(), manager.get());
}

TEST_F(LayerControlPanelTest, RefreshLayerList) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    
    panel->SetLayerManager(manager.get());
    EXPECT_EQ(panel->GetLayerCount(), 2u);
}

TEST_F(LayerControlPanelTest, SetLayerVisible) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerVisible("layer1", false);
    EXPECT_FALSE(panel->IsLayerVisible("layer1"));
    
    panel->SetLayerVisible("layer1", true);
    EXPECT_TRUE(panel->IsLayerVisible("layer1"));
}

TEST_F(LayerControlPanelTest, SetLayerOpacity) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerOpacity("layer1", 0.5);
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("layer1"), 0.5);
}

TEST_F(LayerControlPanelTest, OpacityClamped) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerOpacity("layer1", 1.5);
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("layer1"), 1.0);
    
    panel->SetLayerOpacity("layer1", -0.5);
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("layer1"), 0.0);
}

TEST_F(LayerControlPanelTest, MoveLayerUp) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer1");
    EXPECT_EQ(ids[1], "layer2");
    EXPECT_EQ(ids[2], "layer3");
    
    panel->MoveLayerUp("layer2");
    ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer2");
    EXPECT_EQ(ids[1], "layer1");
    EXPECT_EQ(ids[2], "layer3");
}

TEST_F(LayerControlPanelTest, MoveLayerDown) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerDown("layer2");
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer1");
    EXPECT_EQ(ids[1], "layer2");
    EXPECT_EQ(ids[2], "layer3");
}

TEST_F(LayerControlPanelTest, MoveLayerToTop) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerToTop("layer3");
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer3");
    EXPECT_EQ(ids[1], "layer1");
    EXPECT_EQ(ids[2], "layer2");
}

TEST_F(LayerControlPanelTest, MoveLayerToBottom) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerToBottom("layer1");
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer2");
    EXPECT_EQ(ids[1], "layer1");
    EXPECT_EQ(ids[2], "layer3");
}

TEST_F(LayerControlPanelTest, SetLayerLocked) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerLocked("layer1", true);
    EXPECT_TRUE(panel->IsLayerLocked("layer1"));
    
    panel->SetLayerLocked("layer1", false);
    EXPECT_FALSE(panel->IsLayerLocked("layer1"));
}

TEST_F(LayerControlPanelTest, SelectLayer) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->SelectLayer("layer1");
    EXPECT_TRUE(panel->IsLayerSelected("layer1"));
    EXPECT_FALSE(panel->IsLayerSelected("layer2"));
    EXPECT_EQ(panel->GetSelectedLayer(), "layer1");
}

TEST_F(LayerControlPanelTest, DeselectLayer) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->SelectLayer("layer1");
    EXPECT_TRUE(panel->IsLayerSelected("layer1"));
    
    panel->DeselectLayer();
    EXPECT_FALSE(panel->IsLayerSelected("layer1"));
    EXPECT_TRUE(panel->GetSelectedLayer().empty());
}

TEST_F(LayerControlPanelTest, ToggleLayerExpansion) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    EXPECT_TRUE(panel->IsLayerExpanded("layer1"));
    
    panel->ToggleLayerExpansion("layer1");
    EXPECT_FALSE(panel->IsLayerExpanded("layer1"));
    
    panel->ToggleLayerExpansion("layer1");
    EXPECT_TRUE(panel->IsLayerExpanded("layer1"));
}

TEST_F(LayerControlPanelTest, LayerGroups) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->CreateGroup("group1", "Group 1");
    panel->SetLayerGroup("layer1", "group1");
    panel->SetLayerGroup("layer2", "group1");
    
    EXPECT_EQ(panel->GetLayerGroup("layer1"), "group1");
    EXPECT_EQ(panel->GetLayerGroup("layer2"), "group1");
}

TEST_F(LayerControlPanelTest, DeleteGroup) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    panel->CreateGroup("group1", "Group 1");
    panel->SetLayerGroup("layer1", "group1");
    
    panel->DeleteGroup("group1");
    EXPECT_TRUE(panel->GetLayerGroup("layer1").empty());
}

TEST_F(LayerControlPanelTest, ToggleGroupExpansion) {
    panel->CreateGroup("group1", "Group 1");
    
    EXPECT_TRUE(panel->IsGroupExpanded("group1"));
    
    panel->ToggleGroupExpansion("group1");
    EXPECT_FALSE(panel->IsGroupExpanded("group1"));
}

TEST_F(LayerControlPanelTest, SetGroupVisible) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->CreateGroup("group1", "Group 1");
    panel->SetLayerGroup("layer1", "group1");
    panel->SetLayerGroup("layer2", "group1");
    
    panel->SetGroupVisible("group1", false);
    EXPECT_FALSE(panel->IsLayerVisible("layer1"));
    EXPECT_FALSE(panel->IsLayerVisible("layer2"));
}

TEST_F(LayerControlPanelTest, SetGroupOpacity) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->CreateGroup("group1", "Group 1");
    panel->SetLayerGroup("layer1", "group1");
    panel->SetLayerGroup("layer2", "group1");
    
    panel->SetGroupOpacity("group1", 0.5);
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("layer1"), 0.5);
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("layer2"), 0.5);
}

TEST_F(LayerControlPanelTest, ShowAllLayers) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerVisible("layer1", false);
    panel->SetLayerVisible("layer2", false);
    
    panel->ShowAllLayers();
    EXPECT_TRUE(panel->IsLayerVisible("layer1"));
    EXPECT_TRUE(panel->IsLayerVisible("layer2"));
}

TEST_F(LayerControlPanelTest, HideAllLayers) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->HideAllLayers();
    EXPECT_FALSE(panel->IsLayerVisible("layer1"));
    EXPECT_FALSE(panel->IsLayerVisible("layer2"));
}

TEST_F(LayerControlPanelTest, LockAllLayers) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->LockAllLayers();
    EXPECT_TRUE(panel->IsLayerLocked("layer1"));
    EXPECT_TRUE(panel->IsLayerLocked("layer2"));
}

TEST_F(LayerControlPanelTest, UnlockAllLayers) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->LockAllLayers();
    panel->UnlockAllLayers();
    EXPECT_FALSE(panel->IsLayerLocked("layer1"));
    EXPECT_FALSE(panel->IsLayerLocked("layer2"));
}

TEST_F(LayerControlPanelTest, FindLayer) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    auto* layer = panel->FindLayer("layer1");
    EXPECT_TRUE(layer != nullptr);
    EXPECT_EQ(layer->layerId, "layer1");
    
    auto* notFound = panel->FindLayer("nonexistent");
    EXPECT_TRUE(notFound == nullptr);
}

TEST_F(LayerControlPanelTest, FindLayerConst) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    const auto& constPanel = panel;
    const auto* layer = constPanel->FindLayer("layer1");
    EXPECT_TRUE(layer != nullptr);
    EXPECT_EQ(layer->layerId, "layer1");
}

TEST_F(LayerControlPanelTest, SortLayersByOrder) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerToTop("layer3");
    panel->SortLayersByOrder();
    
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer3");
}

TEST_F(LayerControlPanelTest, SortLayersByName) {
    AddTestLayer("layer_c");
    AddTestLayer("layer_a");
    AddTestLayer("layer_b");
    panel->SetLayerManager(manager.get());
    
    panel->SortLayersByName();
    
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer_a");
    EXPECT_EQ(ids[1], "layer_b");
    EXPECT_EQ(ids[2], "layer_c");
}

TEST_F(LayerControlPanelTest, GetVisibleLayerIds) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    AddTestLayer("layer3");
    panel->SetLayerManager(manager.get());
    
    panel->SetLayerVisible("layer2", false);
    
    auto visibleIds = panel->GetVisibleLayerIds();
    EXPECT_EQ(visibleIds.size(), 2u);
    EXPECT_EQ(visibleIds[0], "layer1");
    EXPECT_EQ(visibleIds[1], "layer3");
}

TEST_F(LayerControlPanelTest, GetAllLayerIds) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids.size(), 2u);
    EXPECT_EQ(ids[0], "layer1");
    EXPECT_EQ(ids[1], "layer2");
}

TEST_F(LayerControlPanelTest, LayerChangedCallback) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    std::string changedId;
    panel->SetLayerChangedCallback([&changedId](const std::string& id) {
        changedId = id;
    });
    
    panel->SetLayerVisible("layer1", false);
    EXPECT_EQ(changedId, "layer1");
}

TEST_F(LayerControlPanelTest, OrderChangedCallback) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    bool orderChanged = false;
    panel->SetOrderChangedCallback([&orderChanged]() {
        orderChanged = true;
    });
    
    panel->MoveLayerUp("layer2");
    EXPECT_TRUE(orderChanged);
}

TEST_F(LayerControlPanelTest, SelectionChangedCallback) {
    AddTestLayer("layer1");
    panel->SetLayerManager(manager.get());
    
    std::string selectedId;
    panel->SetSelectionChangedCallback([&selectedId](const std::string& id) {
        selectedId = id;
    });
    
    panel->SelectLayer("layer1");
    EXPECT_EQ(selectedId, "layer1");
}

TEST_F(LayerControlPanelTest, HasLayers) {
    panel->SetLayerManager(manager.get());
    EXPECT_FALSE(panel->HasLayers());
    
    AddTestLayer("layer1");
    panel->RefreshLayerList();
    
    EXPECT_TRUE(panel->HasLayers());
}

TEST_F(LayerControlPanelTest, GetLayerCount) {
    panel->SetLayerManager(manager.get());
    EXPECT_EQ(panel->GetLayerCount(), 0u);
    
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->RefreshLayerList();
    
    EXPECT_EQ(panel->GetLayerCount(), 2u);
}

TEST_F(LayerControlPanelTest, ApplyPreset) {
    panel->ApplyPreset("preset1");
    EXPECT_EQ(panel->GetCurrentPreset(), "preset1");
}

TEST_F(LayerControlPanelTest, SavePreset) {
    panel->SavePreset("preset1");
    EXPECT_EQ(panel->GetCurrentPreset(), "preset1");
}

TEST_F(LayerControlPanelTest, DeletePreset) {
    panel->SavePreset("preset1");
    panel->DeletePreset("preset1");
    EXPECT_TRUE(panel->GetCurrentPreset().empty());
}

TEST_F(LayerControlPanelTest, GetAvailablePresets) {
    auto presets = panel->GetAvailablePresets();
    EXPECT_TRUE(presets.empty());
}

TEST_F(LayerControlPanelTest, MoveLayerUpAtTop) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerUp("layer1");
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer1");
    EXPECT_EQ(ids[1], "layer2");
}

TEST_F(LayerControlPanelTest, MoveLayerDownAtBottom) {
    AddTestLayer("layer1");
    AddTestLayer("layer2");
    panel->SetLayerManager(manager.get());
    
    panel->MoveLayerDown("layer2");
    auto ids = panel->GetAllLayerIds();
    EXPECT_EQ(ids[0], "layer1");
    EXPECT_EQ(ids[1], "layer2");
}

TEST_F(LayerControlPanelTest, GetGroups) {
    panel->CreateGroup("group1", "Group 1");
    panel->CreateGroup("group2", "Group 2");
    
    const auto& groups = panel->GetGroups();
    EXPECT_EQ(groups.size(), 2u);
}

TEST_F(LayerControlPanelTest, NonExistentLayer) {
    panel->SetLayerManager(manager.get());
    EXPECT_FALSE(panel->IsLayerVisible("nonexistent"));
    EXPECT_DOUBLE_EQ(panel->GetLayerOpacity("nonexistent"), 1.0);
    EXPECT_FALSE(panel->IsLayerLocked("nonexistent"));
    EXPECT_TRUE(panel->IsLayerExpanded("nonexistent"));
    EXPECT_TRUE(panel->GetLayerGroup("nonexistent").empty());
}
