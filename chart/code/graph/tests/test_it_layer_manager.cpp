#include <gtest/gtest.h>
#include "ogc/draw/layer_manager.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;
using ogc::CNLayer;
using ogc::CNMemoryLayer;
using ogc::GeomType;

class DummyLayerRenderer : public ILayerRenderer {
public:
    DrawResult Render(CNLayer* layer, DrawContext& context) override {
        return DrawResult::kSuccess;
    }
    
    DrawResult RenderSelection(CNLayer* layer, DrawContext& context, 
                                const std::vector<int64_t>& featureIds) override {
        return DrawResult::kSuccess;
    }
    
    DrawResult RenderLabels(CNLayer* layer, DrawContext& context) override {
        return DrawResult::kSuccess;
    }
};

class IntegrationLayerManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(m_device, nullptr);
        
        DrawResult result = m_device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        m_context = DrawContext::Create(m_device);
        ASSERT_NE(m_context, nullptr);
        
        m_manager = std::make_shared<LayerManager>();
        ASSERT_NE(m_manager, nullptr);
    }
    
    void TearDown() override {
        m_manager.reset();
        m_context.reset();
        if (m_device) {
            m_device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::shared_ptr<DrawContext> m_context;
    std::shared_ptr<LayerManager> m_manager;
};

TEST_F(IntegrationLayerManagerTest, ManagerCreation) {
    EXPECT_NE(m_manager, nullptr);
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(IntegrationLayerManagerTest, AddLayer) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    config.SetVisibility(LayerVisibility::kVisible);
    config.SetOpacity(1.0);
    
    int index = m_manager->AddLayer(layer.get(), config);
    EXPECT_EQ(index, 0);
    EXPECT_EQ(m_manager->GetLayerCount(), 1);
    
    LayerItem* item = m_manager->GetLayer(0);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->GetConfig().GetName(), "TestLayer");
}

TEST_F(IntegrationLayerManagerTest, AddMultipleLayers) {
    for (int i = 0; i < 5; ++i) {
        std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("Layer" + std::to_string(i), GeomType::kPoint));
        LayerConfig config("Layer" + std::to_string(i));
        m_manager->AddLayer(layer.get(), config);
    }
    
    EXPECT_EQ(m_manager->GetLayerCount(), 5);
    
    for (int i = 0; i < 5; ++i) {
        LayerItem* item = m_manager->GetLayer(i);
        ASSERT_NE(item, nullptr);
        EXPECT_EQ(item->GetConfig().GetName(), "Layer" + std::to_string(i));
    }
}

TEST_F(IntegrationLayerManagerTest, RemoveLayer) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    EXPECT_EQ(m_manager->GetLayerCount(), 1);
    
    m_manager->RemoveLayer(0);
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(IntegrationLayerManagerTest, RemoveLayerByName) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    m_manager->RemoveLayer("TestLayer");
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(IntegrationLayerManagerTest, RemoveAllLayers) {
    for (int i = 0; i < 3; ++i) {
        std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("Layer" + std::to_string(i), GeomType::kPoint));
        LayerConfig config("Layer" + std::to_string(i));
        m_manager->AddLayer(layer.get(), config);
    }
    
    EXPECT_EQ(m_manager->GetLayerCount(), 3);
    
    m_manager->RemoveAllLayers();
    EXPECT_EQ(m_manager->GetLayerCount(), 0);
}

TEST_F(IntegrationLayerManagerTest, GetLayerByName) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    LayerItem* item = m_manager->GetLayer("TestLayer");
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->GetConfig().GetName(), "TestLayer");
    
    item = m_manager->GetLayer("NonExistent");
    EXPECT_EQ(item, nullptr);
}

TEST_F(IntegrationLayerManagerTest, GetLayerIndex) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    int index = m_manager->GetLayerIndex("TestLayer");
    EXPECT_EQ(index, 0);
    
    index = m_manager->GetLayerIndex("NonExistent");
    EXPECT_EQ(index, -1);
}

TEST_F(IntegrationLayerManagerTest, MoveLayer) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer3(new CNMemoryLayer("Layer3", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    LayerConfig config3("Layer3");
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    m_manager->AddLayer(layer3.get(), config3);
    
    EXPECT_EQ(m_manager->GetLayer(0)->GetConfig().GetName(), "Layer1");
    EXPECT_EQ(m_manager->GetLayer(1)->GetConfig().GetName(), "Layer2");
    EXPECT_EQ(m_manager->GetLayer(2)->GetConfig().GetName(), "Layer3");
    
    m_manager->MoveLayer(0, 2);
    
    EXPECT_EQ(m_manager->GetLayer(0)->GetConfig().GetName(), "Layer2");
    EXPECT_EQ(m_manager->GetLayer(1)->GetConfig().GetName(), "Layer1");
    EXPECT_EQ(m_manager->GetLayer(2)->GetConfig().GetName(), "Layer3");
}

TEST_F(IntegrationLayerManagerTest, MoveLayerUp) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    
    m_manager->MoveLayerUp(1);
    
    EXPECT_EQ(m_manager->GetLayer(0)->GetConfig().GetName(), "Layer2");
    EXPECT_EQ(m_manager->GetLayer(1)->GetConfig().GetName(), "Layer1");
}

TEST_F(IntegrationLayerManagerTest, MoveLayerDown) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    
    m_manager->MoveLayerDown(0);
    
    EXPECT_EQ(m_manager->GetLayer(0)->GetConfig().GetName(), "Layer1");
    EXPECT_EQ(m_manager->GetLayer(1)->GetConfig().GetName(), "Layer2");
}

TEST_F(IntegrationLayerManagerTest, SetLayerVisibility) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    m_manager->SetLayerVisibility(0, LayerVisibility::kHidden);
    
    LayerItem* item = m_manager->GetLayer(0);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->GetConfig().GetVisibility(), LayerVisibility::kHidden);
}

TEST_F(IntegrationLayerManagerTest, SetLayerOpacity) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    m_manager->SetLayerOpacity(0, 0.5);
    
    LayerItem* item = m_manager->GetLayer(0);
    ASSERT_NE(item, nullptr);
    EXPECT_DOUBLE_EQ(item->GetConfig().GetOpacity(), 0.5);
}

TEST_F(IntegrationLayerManagerTest, SetLayerScaleRange) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    m_manager->SetLayerScaleRange(0, 1000.0, 10000.0);
    
    LayerItem* item = m_manager->GetLayer(0);
    ASSERT_NE(item, nullptr);
    EXPECT_DOUBLE_EQ(item->GetConfig().GetMinScale(), 1000.0);
    EXPECT_DOUBLE_EQ(item->GetConfig().GetMaxScale(), 10000.0);
}

TEST_F(IntegrationLayerManagerTest, GetVisibleLayers) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("VisibleLayer", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("HiddenLayer", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer3(new CNMemoryLayer("VisibleLayer2", GeomType::kPoint));
    
    LayerConfig config1("VisibleLayer");
    config1.SetVisibility(LayerVisibility::kVisible);
    
    LayerConfig config2("HiddenLayer");
    config2.SetVisibility(LayerVisibility::kHidden);
    
    LayerConfig config3("VisibleLayer2");
    config3.SetVisibility(LayerVisibility::kVisible);
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    m_manager->AddLayer(layer3.get(), config3);
    
    std::vector<LayerItem*> visibleLayers = m_manager->GetVisibleLayers();
    EXPECT_EQ(visibleLayers.size(), 2);
}

TEST_F(IntegrationLayerManagerTest, GetVisibleLayersAtScale) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    config1.SetMinScale(1000.0);
    config1.SetMaxScale(5000.0);
    config1.SetVisibility(LayerVisibility::kVisible);
    
    LayerConfig config2("Layer2");
    config2.SetMinScale(5000.0);
    config2.SetMaxScale(10000.0);
    config2.SetVisibility(LayerVisibility::kVisible);
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    
    m_manager->SetCurrentScale(3000.0);
    std::vector<LayerItem*> visibleLayers = m_manager->GetVisibleLayersAtScale(3000.0);
    EXPECT_EQ(visibleLayers.size(), 1);
    EXPECT_EQ(visibleLayers[0]->GetConfig().GetName(), "Layer1");
    
    m_manager->SetCurrentScale(7000.0);
    visibleLayers = m_manager->GetVisibleLayersAtScale(7000.0);
    EXPECT_EQ(visibleLayers.size(), 1);
    EXPECT_EQ(visibleLayers[0]->GetConfig().GetName(), "Layer2");
}

TEST_F(IntegrationLayerManagerTest, GetExtent) {
    Envelope extent = m_manager->GetExtent();
    EXPECT_TRUE(extent.IsNull());
}

TEST_F(IntegrationLayerManagerTest, GetLayerInfos) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    config1.SetVisibility(LayerVisibility::kVisible);
    config1.SetOpacity(0.8);
    
    LayerConfig config2("Layer2");
    config2.SetVisibility(LayerVisibility::kHidden);
    config2.SetOpacity(1.0);
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    
    std::vector<LayerInfo> infos = m_manager->GetLayerInfos();
    EXPECT_EQ(infos.size(), 2);
    
    EXPECT_EQ(infos[0].name, "Layer1");
    EXPECT_EQ(infos[0].visibility, LayerVisibility::kVisible);
    EXPECT_DOUBLE_EQ(infos[0].opacity, 0.8);
    
    EXPECT_EQ(infos[1].name, "Layer2");
    EXPECT_EQ(infos[1].visibility, LayerVisibility::kHidden);
    EXPECT_DOUBLE_EQ(infos[1].opacity, 1.0);
}

TEST_F(IntegrationLayerManagerTest, LayerConfigClone) {
    LayerConfig config("TestLayer");
    config.SetVisibility(LayerVisibility::kVisible);
    config.SetOpacity(0.75);
    config.SetMinScale(1000.0);
    config.SetMaxScale(10000.0);
    config.SetSelectable(true);
    config.SetEditable(false);
    
    LayerConfigPtr cloned = config.Clone();
    ASSERT_NE(cloned, nullptr);
    
    EXPECT_EQ(cloned->GetName(), "TestLayer");
    EXPECT_EQ(cloned->GetVisibility(), LayerVisibility::kVisible);
    EXPECT_DOUBLE_EQ(cloned->GetOpacity(), 0.75);
    EXPECT_DOUBLE_EQ(cloned->GetMinScale(), 1000.0);
    EXPECT_DOUBLE_EQ(cloned->GetMaxScale(), 10000.0);
    EXPECT_TRUE(cloned->IsSelectable());
    EXPECT_FALSE(cloned->IsEditable());
}

TEST_F(IntegrationLayerManagerTest, LayerManagerClone) {
    std::unique_ptr<CNMemoryLayer> layer1(new CNMemoryLayer("Layer1", GeomType::kPoint));
    std::unique_ptr<CNMemoryLayer> layer2(new CNMemoryLayer("Layer2", GeomType::kPoint));
    
    LayerConfig config1("Layer1");
    LayerConfig config2("Layer2");
    
    m_manager->AddLayer(layer1.get(), config1);
    m_manager->AddLayer(layer2.get(), config2);
    
    std::shared_ptr<LayerManager> cloned = m_manager->Clone();
    ASSERT_NE(cloned, nullptr);
    
    EXPECT_EQ(cloned->GetLayerCount(), 2);
    EXPECT_EQ(cloned->GetLayer(0)->GetConfig().GetName(), "Layer1");
    EXPECT_EQ(cloned->GetLayer(1)->GetConfig().GetName(), "Layer2");
}

TEST_F(IntegrationLayerManagerTest, LayerChangedCallback) {
    int callbackIndex = -1;
    m_manager->SetLayerChangedCallback([&callbackIndex](int index) {
        callbackIndex = index;
    });
    
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
}

TEST_F(IntegrationLayerManagerTest, SetLayerRenderer) {
    std::unique_ptr<CNMemoryLayer> layer(new CNMemoryLayer("TestLayer", GeomType::kPoint));
    LayerConfig config("TestLayer");
    m_manager->AddLayer(layer.get(), config);
    
    auto renderer = std::make_shared<DummyLayerRenderer>();
    m_manager->SetLayerRenderer(0, renderer);
    
    auto retrieved = m_manager->GetLayerRenderer(0);
    EXPECT_EQ(retrieved, renderer);
}

TEST_F(IntegrationLayerManagerTest, LayerItemIsVisible) {
    LayerConfig config("TestLayer");
    config.SetVisibility(LayerVisibility::kVisible);
    
    LayerItem item(nullptr, config);
    EXPECT_TRUE(item.IsVisible());
    
    item.GetConfig().SetVisibility(LayerVisibility::kHidden);
    EXPECT_FALSE(item.IsVisible());
}

TEST_F(IntegrationLayerManagerTest, LayerItemIsVisibleAtScale) {
    LayerConfig config("TestLayer");
    config.SetMinScale(1000.0);
    config.SetMaxScale(5000.0);
    config.SetVisibility(LayerVisibility::kVisible);
    
    LayerItem item(nullptr, config);
    EXPECT_TRUE(item.IsVisibleAtScale(3000.0));
    EXPECT_FALSE(item.IsVisibleAtScale(500.0));
    EXPECT_FALSE(item.IsVisibleAtScale(6000.0));
}

TEST_F(IntegrationLayerManagerTest, LayerConfigIsVisibleAtScale) {
    LayerConfig config("TestLayer");
    config.SetMinScale(1000.0);
    config.SetMaxScale(5000.0);
    
    EXPECT_TRUE(config.IsVisibleAtScale(3000.0));
    EXPECT_FALSE(config.IsVisibleAtScale(500.0));
    EXPECT_FALSE(config.IsVisibleAtScale(6000.0));
}

TEST_F(IntegrationLayerManagerTest, LayerConfigOpacityClamp) {
    LayerConfig config("TestLayer");
    
    config.SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 1.0);
    
    config.SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 0.0);
    
    config.SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(config.GetOpacity(), 0.5);
}

TEST_F(IntegrationLayerManagerTest, LayerConfigSymbolizer) {
    LayerConfig config("TestLayer");
    
    auto symbolizer = LineSymbolizer::Create();
    config.SetSymbolizer(symbolizer);
    
    auto retrieved = config.GetSymbolizer();
    EXPECT_EQ(retrieved, symbolizer);
}

TEST_F(IntegrationLayerManagerTest, CurrentScale) {
    m_manager->SetCurrentScale(2500.0);
    EXPECT_DOUBLE_EQ(m_manager->GetCurrentScale(), 2500.0);
}
