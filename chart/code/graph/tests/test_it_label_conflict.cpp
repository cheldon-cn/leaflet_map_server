#include <gtest/gtest.h>
#include "ogc/draw/label_engine.h"
#include "ogc/draw/label_conflict.h"
#include "ogc/draw/label_placement.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/font.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class IntegrationLabelConflictTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(m_device, nullptr);
        
        DrawResult result = m_device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        m_context = DrawContext::Create(m_device);
        ASSERT_NE(m_context, nullptr);
        
        result = m_context->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        m_engine = LabelEngine::Create();
        ASSERT_NE(m_engine, nullptr);
        
        m_resolver = LabelConflictResolver::Create();
        ASSERT_NE(m_resolver, nullptr);
    }
    
    void TearDown() override {
        m_resolver.reset();
        m_engine.reset();
        if (m_context) {
            m_context->Finalize();
        }
        if (m_device) {
            m_device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::shared_ptr<DrawContext> m_context;
    std::shared_ptr<LabelEngine> m_engine;
    std::shared_ptr<LabelConflictResolver> m_resolver;
};

TEST_F(IntegrationLabelConflictTest, EngineCreation) {
    EXPECT_NE(m_engine, nullptr);
}

TEST_F(IntegrationLabelConflictTest, SetLabelProperty) {
    m_engine->SetLabelProperty("name");
    EXPECT_EQ(m_engine->GetLabelProperty(), "name");
}

TEST_F(IntegrationLabelConflictTest, SetFont) {
    Font font("Arial", 12, FontWeight::kBold, false);
    m_engine->SetFont(font);
    
    Font retrieved = m_engine->GetFont();
    EXPECT_EQ(retrieved.GetFamily(), "Arial");
    EXPECT_EQ(retrieved.GetSize(), 12);
    EXPECT_TRUE(retrieved.IsBold());
    EXPECT_FALSE(retrieved.IsItalic());
}

TEST_F(IntegrationLabelConflictTest, SetColor) {
    m_engine->SetColor(Color::Red().GetRGBA());
    EXPECT_EQ(m_engine->GetColor(), Color::Red().GetRGBA());
}

TEST_F(IntegrationLabelConflictTest, SetHaloColor) {
    m_engine->SetHaloColor(Color::White().GetRGBA());
    EXPECT_EQ(m_engine->GetHaloColor(), Color::White().GetRGBA());
}

TEST_F(IntegrationLabelConflictTest, SetHaloRadius) {
    m_engine->SetHaloRadius(2.0);
    EXPECT_DOUBLE_EQ(m_engine->GetHaloRadius(), 2.0);
}

TEST_F(IntegrationLabelConflictTest, SetMinDistance) {
    m_engine->SetMinDistance(5.0);
    EXPECT_DOUBLE_EQ(m_engine->GetMinDistance(), 5.0);
}

TEST_F(IntegrationLabelConflictTest, SetMaxDistance) {
    m_engine->SetMaxDistance(100.0);
    EXPECT_DOUBLE_EQ(m_engine->GetMaxDistance(), 100.0);
}

TEST_F(IntegrationLabelConflictTest, SetPriority) {
    m_engine->SetPriority(10);
    EXPECT_EQ(m_engine->GetPriority(), 10);
}

TEST_F(IntegrationLabelConflictTest, SetAllowOverlap) {
    m_engine->SetAllowOverlap(true);
    EXPECT_TRUE(m_engine->GetAllowOverlap());
    
    m_engine->SetAllowOverlap(false);
    EXPECT_FALSE(m_engine->GetAllowOverlap());
}

TEST_F(IntegrationLabelConflictTest, SetFollowLine) {
    m_engine->SetFollowLine(true);
    EXPECT_TRUE(m_engine->GetFollowLine());
    
    m_engine->SetFollowLine(false);
    EXPECT_FALSE(m_engine->GetFollowLine());
}

TEST_F(IntegrationLabelConflictTest, SetMaxAngleDelta) {
    m_engine->SetMaxAngleDelta(30.0);
    EXPECT_DOUBLE_EQ(m_engine->GetMaxAngleDelta(), 30.0);
}

TEST_F(IntegrationLabelConflictTest, ClearCache) {
    m_engine->ClearCache();
}

TEST_F(IntegrationLabelConflictTest, LabelInfoDefaultValues) {
    LabelInfo info;
    EXPECT_EQ(info.x, 0);
    EXPECT_EQ(info.y, 0);
    EXPECT_EQ(info.width, 0);
    EXPECT_EQ(info.height, 0);
    EXPECT_EQ(info.rotation, 0);
    EXPECT_EQ(info.priority, 0);
    EXPECT_TRUE(info.visible);
    EXPECT_EQ(info.featureId, 0);
}

TEST_F(IntegrationLabelConflictTest, LabelInfoGetBounds) {
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

TEST_F(IntegrationLabelConflictTest, LabelInfoIntersects) {
    LabelInfo info1;
    info1.x = 0;
    info1.y = 0;
    info1.width = 100;
    info1.height = 50;
    
    LabelInfo info2;
    info2.x = 50;
    info2.y = 25;
    info2.width = 100;
    info2.height = 50;
    
    EXPECT_TRUE(info1.Intersects(info2));
    EXPECT_TRUE(info2.Intersects(info1));
    
    LabelInfo info3;
    info3.x = 200;
    info3.y = 200;
    info3.width = 100;
    info3.height = 50;
    
    EXPECT_FALSE(info1.Intersects(info3));
    EXPECT_FALSE(info3.Intersects(info1));
}

TEST_F(IntegrationLabelConflictTest, LabelPlacementResultDefaultValues) {
    LabelPlacementResult result;
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
    EXPECT_EQ(result.rotation, 0);
    EXPECT_TRUE(result.message.empty());
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsEmpty) {
    std::vector<LabelInfo> labels;
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsSingle) {
    std::vector<LabelInfo> labels;
    LabelInfo info;
    info.text = "Test Label";
    info.x = 128;
    info.y = 128;
    info.width = 80;
    info.height = 20;
    info.visible = true;
    labels.push_back(info);
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    m_context->BeginDraw(params);
    m_context->Clear(Color::White());
    
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->EndDraw();
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsMultiple) {
    std::vector<LabelInfo> labels;
    
    for (int i = 0; i < 5; ++i) {
        LabelInfo info;
        info.text = "Label " + std::to_string(i);
        info.x = 50 + i * 40;
        info.y = 128;
        info.width = 60;
        info.height = 20;
        info.visible = true;
        info.priority = i;
        labels.push_back(info);
    }
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    m_context->BeginDraw(params);
    m_context->Clear(Color::White());
    
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->EndDraw();
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsWithHalo) {
    m_engine->SetHaloColor(Color::White().GetRGBA());
    m_engine->SetHaloRadius(2.0);
    
    std::vector<LabelInfo> labels;
    LabelInfo info;
    info.text = "Halo Label";
    info.x = 128;
    info.y = 128;
    info.width = 80;
    info.height = 20;
    info.visible = true;
    labels.push_back(info);
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    m_context->BeginDraw(params);
    m_context->Clear(Color::Black());
    
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->EndDraw();
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsInvisible) {
    std::vector<LabelInfo> labels;
    LabelInfo info;
    info.text = "Invisible Label";
    info.x = 128;
    info.y = 128;
    info.width = 80;
    info.height = 20;
    info.visible = false;
    labels.push_back(info);
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    m_context->BeginDraw(params);
    m_context->Clear(Color::White());
    
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->EndDraw();
}

TEST_F(IntegrationLabelConflictTest, RenderLabelsWithRotation) {
    std::vector<LabelInfo> labels;
    LabelInfo info;
    info.text = "Rotated Label";
    info.x = 128;
    info.y = 128;
    info.width = 80;
    info.height = 20;
    info.rotation = 45.0;
    info.visible = true;
    labels.push_back(info);
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    m_context->BeginDraw(params);
    m_context->Clear(Color::White());
    
    DrawResult result = m_engine->RenderLabels(labels, *m_context);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->EndDraw();
}

TEST_F(IntegrationLabelConflictTest, GenerateLabelsEmpty) {
    std::vector<const ogc::CNFeature*> features;
    std::vector<LabelInfo> labels = m_engine->GenerateLabels(features, *m_context);
    EXPECT_TRUE(labels.empty());
}

TEST_F(IntegrationLabelConflictTest, SetConflictResolver) {
    auto resolver = LabelConflictResolver::Create();
    m_engine->SetConflictResolver(resolver);
    
    auto retrieved = m_engine->GetConflictResolver();
    EXPECT_EQ(retrieved, resolver);
}

TEST_F(IntegrationLabelConflictTest, ConflictResolverHasConflict) {
    LabelInfo info1;
    info1.x = 0;
    info1.y = 0;
    info1.width = 100;
    info1.height = 50;
    
    LabelInfo info2;
    info2.x = 50;
    info2.y = 25;
    info2.width = 100;
    info2.height = 50;
    
    EXPECT_TRUE(m_resolver->HasConflict(info1, info2));
    
    LabelInfo info3;
    info3.x = 200;
    info3.y = 200;
    info3.width = 100;
    info3.height = 50;
    
    EXPECT_FALSE(m_resolver->HasConflict(info1, info3));
}

TEST_F(IntegrationLabelConflictTest, ConflictResolverResolve) {
    std::vector<LabelInfo> labels;
    
    LabelInfo info1;
    info1.x = 0;
    info1.y = 0;
    info1.width = 100;
    info1.height = 50;
    info1.priority = 1;
    info1.visible = true;
    labels.push_back(info1);
    
    LabelInfo info2;
    info2.x = 50;
    info2.y = 25;
    info2.width = 100;
    info2.height = 50;
    info2.priority = 2;
    info2.visible = true;
    labels.push_back(info2);
    
    std::vector<LabelInfo> resolved = m_resolver->ResolveConflicts(labels);
    EXPECT_GE(resolved.size(), 1);
}
