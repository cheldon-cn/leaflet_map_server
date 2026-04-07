#include <gtest/gtest.h>
#include <ogc/graph/label/label_engine.h>
#include <ogc/graph/label/label_conflict.h>
#include <ogc/feature/feature.h>
#include <ogc/draw/font.h>
#include <ogc/draw/color.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include <memory>

using namespace ogc::graph;
using ogc::CNFeature;
using ogc::Envelope;
using ogc::draw::Color;
using ogc::draw::Font;
using ogc::draw::FontStyle;
using ogc::draw::RasterImageDevice;
using ogc::draw::PixelFormat;
using ogc::draw::DrawContext;
using ogc::draw::DrawContextPtr;

class LabelEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = LabelEngine::Create();
        ASSERT_NE(engine, nullptr);
    }
    
    void TearDown() override {
        engine.reset();
    }
    
    LabelEnginePtr engine;
};

TEST_F(LabelEngineTest, DefaultConstructor) {
    LabelEnginePtr defaultEngine = LabelEngine::Create();
    EXPECT_NE(defaultEngine, nullptr);
}

TEST_F(LabelEngineTest, SetLabelProperty) {
    engine->SetLabelProperty("name");
    EXPECT_EQ(engine->GetLabelProperty(), "name");
    
    engine->SetLabelProperty("title");
    EXPECT_EQ(engine->GetLabelProperty(), "title");
}

TEST_F(LabelEngineTest, SetFont) {
    Font font("Arial", 12, FontStyle::kBold);
    engine->SetFont(font);
    
    Font retrieved = engine->GetFont();
    EXPECT_EQ(retrieved.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(retrieved.GetSize(), 12);
    EXPECT_EQ(retrieved.GetStyle(), FontStyle::kBold);
}

TEST_F(LabelEngineTest, SetColor) {
    uint32_t color = Color::Red().GetRGBA();
    engine->SetColor(color);
    EXPECT_EQ(engine->GetColor(), color);
}

TEST_F(LabelEngineTest, SetHaloColor) {
    uint32_t haloColor = Color::White().GetRGBA();
    engine->SetHaloColor(haloColor);
    EXPECT_EQ(engine->GetHaloColor(), haloColor);
}

TEST_F(LabelEngineTest, SetHaloRadius) {
    engine->SetHaloRadius(2.0);
    EXPECT_DOUBLE_EQ(engine->GetHaloRadius(), 2.0);
    
    engine->SetHaloRadius(3.5);
    EXPECT_DOUBLE_EQ(engine->GetHaloRadius(), 3.5);
}

TEST_F(LabelEngineTest, SetMinDistance) {
    engine->SetMinDistance(10.0);
    EXPECT_DOUBLE_EQ(engine->GetMinDistance(), 10.0);
}

TEST_F(LabelEngineTest, SetMaxDistance) {
    engine->SetMaxDistance(100.0);
    EXPECT_DOUBLE_EQ(engine->GetMaxDistance(), 100.0);
}

TEST_F(LabelEngineTest, SetPriority) {
    engine->SetPriority(5);
    EXPECT_EQ(engine->GetPriority(), 5);
    
    engine->SetPriority(10);
    EXPECT_EQ(engine->GetPriority(), 10);
}

TEST_F(LabelEngineTest, SetAllowOverlap) {
    engine->SetAllowOverlap(true);
    EXPECT_TRUE(engine->GetAllowOverlap());
    
    engine->SetAllowOverlap(false);
    EXPECT_FALSE(engine->GetAllowOverlap());
}

TEST_F(LabelEngineTest, SetFollowLine) {
    engine->SetFollowLine(true);
    EXPECT_TRUE(engine->GetFollowLine());
    
    engine->SetFollowLine(false);
    EXPECT_FALSE(engine->GetFollowLine());
}

TEST_F(LabelEngineTest, SetMaxAngleDelta) {
    engine->SetMaxAngleDelta(30.0);
    EXPECT_DOUBLE_EQ(engine->GetMaxAngleDelta(), 30.0);
}

TEST_F(LabelEngineTest, SetConflictResolver) {
    LabelConflictResolverPtr resolver = LabelConflictResolver::Create();
    engine->SetConflictResolver(resolver);
    
    LabelConflictResolverPtr retrieved = engine->GetConflictResolver();
    EXPECT_EQ(retrieved, resolver);
}

TEST_F(LabelEngineTest, LabelInfoDefaultValues) {
    LabelInfo info;
    
    EXPECT_TRUE(info.text.empty());
    EXPECT_DOUBLE_EQ(info.x, 0);
    EXPECT_DOUBLE_EQ(info.y, 0);
    EXPECT_DOUBLE_EQ(info.width, 0);
    EXPECT_DOUBLE_EQ(info.height, 0);
    EXPECT_DOUBLE_EQ(info.rotation, 0);
    EXPECT_EQ(info.priority, 0);
    EXPECT_TRUE(info.visible);
    EXPECT_EQ(info.featureId, 0);
}

TEST_F(LabelEngineTest, LabelInfoGetBounds) {
    LabelInfo info;
    info.x = 10;
    info.y = 20;
    info.width = 100;
    info.height = 50;
    
    Envelope bounds = info.GetBounds();
    EXPECT_DOUBLE_EQ(bounds.GetMinX(), 10);
    EXPECT_DOUBLE_EQ(bounds.GetMinY(), 20);
    EXPECT_DOUBLE_EQ(bounds.GetMaxX(), 110);
    EXPECT_DOUBLE_EQ(bounds.GetMaxY(), 70);
}

TEST_F(LabelEngineTest, LabelInfoIntersects) {
    LabelInfo info1;
    info1.x = 0;
    info1.y = 0;
    info1.width = 50;
    info1.height = 20;
    
    LabelInfo info2;
    info2.x = 25;
    info2.y = 10;
    info2.width = 50;
    info2.height = 20;
    
    EXPECT_TRUE(info1.Intersects(info2));
    
    LabelInfo info3;
    info3.x = 100;
    info3.y = 100;
    info3.width = 50;
    info3.height = 20;
    
    EXPECT_FALSE(info1.Intersects(info3));
}

TEST_F(LabelEngineTest, LabelPlacementResultDefaultValues) {
    LabelPlacementResult result;
    
    EXPECT_FALSE(result.success);
    EXPECT_DOUBLE_EQ(result.x, 0);
    EXPECT_DOUBLE_EQ(result.y, 0);
    EXPECT_DOUBLE_EQ(result.rotation, 0);
    EXPECT_TRUE(result.message.empty());
}

TEST_F(LabelEngineTest, GenerateLabelsEmpty) {
    std::vector<const CNFeature*> features;
    
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    device->Initialize();
    DrawContextPtr context = DrawContext::Create(device.get());
    
    std::vector<LabelInfo> labels = engine->GenerateLabels(features, *context);
    EXPECT_TRUE(labels.empty());
}
