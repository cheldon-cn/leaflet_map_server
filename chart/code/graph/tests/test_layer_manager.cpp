#include <gtest/gtest.h>
#include "ogc/draw/layer_manager.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;
using ogc::CNLayer;

class LayerManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager = std::make_shared<LayerManager>();
        ASSERT_NE(m_manager, nullptr);
    }
    
    void TearDown() override {
        m_manager.reset();
    }
    
    std::shared_ptr<LayerManager> m_manager;
};

TEST_F(LayerManagerTest, DefaultConstructor) {
    std::shared_ptr<LayerManager> manager = std::make_shared<LayerManager>();
    EXPECT_NE(manager, nullptr);
    EXPECT_EQ(manager->GetLayerCount(), 0);
}

TEST_F(LayerManagerTest, AddLayer) {
    LayerConfig config("TestLayer");
    config.SetVisibility(LayerVisibility::kVisible);
    config.SetOpacity(1.0);
    
    int index = m_manager->AddLayer(nullptr, config);
    EXPECT_EQ(index, -1);
}

TEST_F(LayerManagerTest, AddMultipleLayers) {
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    LayerConfig config3("Layer3");
    
    m_manager->AddLayer(nullptr, config1);
    m_manager->AddLayer(nullptr, config2);
    m_manager->AddLayer(nullptr, config3);
    
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(LayerManagerTest, RemoveLayer) {
    LayerConfig config("TestLayer");
    m_manager->AddLayer(nullptr, config);
    
    m_manager->RemoveLayer(0);
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(LayerManagerTest, RemoveLayerInvalidIndex) {
    m_manager->RemoveLayer(0);
    m_manager->RemoveLayer(-1);
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(LayerManagerTest, GetLayer) {
    LayerConfig config("TestLayer");
    m_manager->AddLayer(nullptr, config);
    
    LayerItem* layer = m_manager->GetLayer(0);
    EXPECT_EQ(layer, nullptr);
}

TEST_F(LayerManagerTest, GetLayerInvalidIndex) {
    LayerItem* layer = m_manager->GetLayer(0);
    EXPECT_EQ(layer, nullptr);
}

TEST_F(LayerManagerTest, GetLayerByName) {
    LayerConfig config("TestLayer");
    m_manager->AddLayer(nullptr, config);
    
    LayerItem* layer = m_manager->GetLayer("TestLayer");
}

TEST_F(LayerManagerTest, GetLayerByNameNotFound) {
    LayerItem* layer = m_manager->GetLayer("NonExistent");
    EXPECT_EQ(layer, nullptr);
}

TEST_F(LayerManagerTest, SetLayerVisibility) {
    LayerConfig config("TestLayer");
    m_manager->AddLayer(nullptr, config);
    
    m_manager->SetLayerVisibility(0, LayerVisibility::kHidden);
}

TEST_F(LayerManagerTest, SetLayerOpacity) {
    LayerConfig config("TestLayer");
    m_manager->AddLayer(nullptr, config);
    
    m_manager->SetLayerOpacity(0, 0.5);
}

TEST_F(LayerManagerTest, MoveLayer) {
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    LayerConfig config3("Layer3");
    
    m_manager->AddLayer(nullptr, config1);
    m_manager->AddLayer(nullptr, config2);
    m_manager->AddLayer(nullptr, config3);
    
    m_manager->MoveLayer(0, 2);
}

TEST_F(LayerManagerTest, GetVisibleLayers) {
    LayerConfig config1("VisibleLayer");
    config1.SetVisibility(LayerVisibility::kVisible);
    
    LayerConfig config2("HiddenLayer");
    config2.SetVisibility(LayerVisibility::kHidden);
    
    m_manager->AddLayer(nullptr, config1);
    m_manager->AddLayer(nullptr, config2);
    
    std::vector<LayerItem*> visible = m_manager->GetVisibleLayers();
}

TEST_F(LayerManagerTest, GetExtent) {
    Envelope extent = m_manager->GetExtent();
    EXPECT_TRUE(extent.IsNull());
}

TEST_F(LayerManagerTest, GetLayerInfos) {
    LayerConfig config1("Layer1");
    config1.SetVisibility(LayerVisibility::kVisible);
    config1.SetOpacity(0.8);
    
    LayerConfig config2("Layer2");
    config2.SetVisibility(LayerVisibility::kHidden);
    config2.SetOpacity(1.0);
    
    m_manager->AddLayer(nullptr, config1);
    m_manager->AddLayer(nullptr, config2);
    
    std::vector<LayerInfo> infos = m_manager->GetLayerInfos();
}

TEST_F(LayerManagerTest, LayerConfigDefaultValues) {
    LayerConfig config;
    
    EXPECT_TRUE(config.GetName().empty());
    EXPECT_EQ(config.GetVisibility(), LayerVisibility::kVisible);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 1.0);
    EXPECT_DOUBLE_EQ(config.GetMinScale(), 0.0);
    EXPECT_DOUBLE_EQ(config.GetMaxScale(), 0.0);
}

TEST_F(LayerManagerTest, LayerConfigWithName) {
    LayerConfig config("TestLayer");
    EXPECT_EQ(config.GetName(), "TestLayer");
}

TEST_F(LayerManagerTest, LayerConfigSetMinScale) {
    LayerConfig config("Test");
    config.SetMinScale(1000.0);
    EXPECT_DOUBLE_EQ(config.GetMinScale(), 1000.0);
}

TEST_F(LayerManagerTest, LayerConfigSetMaxScale) {
    LayerConfig config("Test");
    config.SetMaxScale(10000.0);
    EXPECT_DOUBLE_EQ(config.GetMaxScale(), 10000.0);
}

TEST_F(LayerManagerTest, LayerConfigIsVisibleAtScale) {
    LayerConfig config("Test");
    config.SetMinScale(1000.0);
    config.SetMaxScale(10000.0);
    
    EXPECT_TRUE(config.IsVisibleAtScale(5000.0));
    EXPECT_FALSE(config.IsVisibleAtScale(500.0));
    EXPECT_FALSE(config.IsVisibleAtScale(15000.0));
}

TEST_F(LayerManagerTest, LayerConfigSetSelectable) {
    LayerConfig config("Test");
    config.SetSelectable(true);
    EXPECT_TRUE(config.IsSelectable());
}

TEST_F(LayerManagerTest, LayerConfigSetEditable) {
    LayerConfig config("Test");
    config.SetEditable(true);
    EXPECT_TRUE(config.IsEditable());
}

TEST_F(LayerManagerTest, LayerConfigOpacityClamp) {
    LayerConfig config("Test");
    
    config.SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 0.0);
    
    config.SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 1.0);
}

TEST_F(LayerManagerTest, LayerConfigClone) {
    LayerConfig config("Original");
    config.SetVisibility(LayerVisibility::kHidden);
    config.SetOpacity(0.7);
    config.SetMinScale(1000.0);
    config.SetMaxScale(10000.0);
    config.SetSelectable(true);
    config.SetEditable(true);
    
    LayerConfigPtr cloned = config.Clone();
    
    EXPECT_EQ(cloned->GetName(), "Original");
    EXPECT_EQ(cloned->GetVisibility(), LayerVisibility::kHidden);
    EXPECT_DOUBLE_EQ(cloned->GetOpacity(), 0.7);
    EXPECT_DOUBLE_EQ(cloned->GetMinScale(), 1000.0);
    EXPECT_DOUBLE_EQ(cloned->GetMaxScale(), 10000.0);
    EXPECT_TRUE(cloned->IsSelectable());
    EXPECT_TRUE(cloned->IsEditable());
}

TEST_F(LayerManagerTest, LayerInfoHasName) {
    LayerInfo info;
    info.name = "TestLayer";
    EXPECT_EQ(info.name, "TestLayer");
}
