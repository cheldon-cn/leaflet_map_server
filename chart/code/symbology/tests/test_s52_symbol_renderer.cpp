#include <gtest/gtest.h>
#include <ogc/symbology/style/s52_symbol_renderer.h>
#include <ogc/symbology/style/s52_style_manager.h>
#include <ogc/draw/draw_context.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include "ogc/geom/point.h"

using namespace ogc::symbology;

class S52SymbolRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        S52StyleManager::Instance().Initialize();
        renderer = &S52SymbolRenderer::Instance();
        renderer->Initialize();
    }
    
    void TearDown() override {
    }
    
    S52SymbolRenderer* renderer;
};

TEST_F(S52SymbolRendererTest, Instance) {
    auto& instance = S52SymbolRenderer::Instance();
    EXPECT_TRUE(true);
}

TEST_F(S52SymbolRendererTest, Initialize) {
    auto& instance = S52SymbolRenderer::Instance();
    EXPECT_TRUE(instance.IsInitialized());
}

TEST_F(S52SymbolRendererTest, SetDefaultSymbolSize) {
    renderer->SetDefaultSymbolSize(20.0);
    EXPECT_DOUBLE_EQ(renderer->GetDefaultSymbolSize(), 20.0);
}

TEST_F(S52SymbolRendererTest, SetDefaultLineWidth) {
    renderer->SetDefaultLineWidth(2.5);
    EXPECT_DOUBLE_EQ(renderer->GetDefaultLineWidth(), 2.5);
}

TEST_F(S52SymbolRendererTest, CacheSymbolizer) {
    PointSymbolizerPtr symbolizer = PointSymbolizer::Create();
    renderer->CacheSymbolizer("test_key", symbolizer);
    
    auto cached = renderer->GetCachedSymbolizer("test_key");
    EXPECT_NE(cached, nullptr);
}

TEST_F(S52SymbolRendererTest, GetCachedSymbolizerNotFound) {
    auto cached = renderer->GetCachedSymbolizer("nonexistent_key");
    EXPECT_EQ(cached, nullptr);
}

TEST_F(S52SymbolRendererTest, ClearCache) {
    PointSymbolizerPtr symbolizer = PointSymbolizer::Create();
    renderer->CacheSymbolizer("test_key", symbolizer);
    
    renderer->ClearCache();
    
    auto cached = renderer->GetCachedSymbolizer("test_key");
    EXPECT_EQ(cached, nullptr);
}

TEST_F(S52SymbolRendererTest, CreatePointSymbolizer) {
    auto symbolizer = renderer->CreatePointSymbolizer(nullptr, nullptr);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPoint);
}

TEST_F(S52SymbolRendererTest, CreatePointSymbolizerWithRule) {
    StyleRule rule;
    rule.lineColor = Color(255, 0, 0, 255);
    rule.lineWidth = 2.0;
    
    auto symbolizer = renderer->CreatePointSymbolizer(nullptr, &rule);
    ASSERT_NE(symbolizer, nullptr);
}

TEST_F(S52SymbolRendererTest, CreateLineSymbolizer) {
    auto symbolizer = renderer->CreateLineSymbolizer(nullptr, nullptr);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kLine);
}

TEST_F(S52SymbolRendererTest, CreateLineSymbolizerWithRule) {
    StyleRule rule;
    rule.lineColor = Color(0, 255, 0, 255);
    rule.lineWidth = 3.0;
    
    auto symbolizer = renderer->CreateLineSymbolizer(nullptr, &rule);
    ASSERT_NE(symbolizer, nullptr);
}

TEST_F(S52SymbolRendererTest, CreatePolygonSymbolizer) {
    auto symbolizer = renderer->CreatePolygonSymbolizer(nullptr, nullptr);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPolygon);
}

TEST_F(S52SymbolRendererTest, CreatePolygonSymbolizerWithRule) {
    StyleRule rule;
    rule.fillColor = Color(0, 0, 255, 255);
    rule.lineColor = Color(255, 0, 0, 255);
    rule.lineWidth = 1.5;
    
    auto symbolizer = renderer->CreatePolygonSymbolizer(nullptr, &rule);
    ASSERT_NE(symbolizer, nullptr);
}

TEST_F(S52SymbolRendererTest, CreateSymbolizerFromDefinition) {
    auto symbolizer = renderer->CreateSymbolizerFromDefinition(nullptr, nullptr);
    EXPECT_EQ(symbolizer, nullptr);
}

TEST_F(S52SymbolRendererTest, CreateSymbolizerFromDefinitionWithSymbol) {
    const SymbolDefinition* symbolDef = SymbolLibrary::Instance().GetSymbol("SNDMRK01");
    if (symbolDef) {
        auto symbolizer = renderer->CreateSymbolizerFromDefinition(symbolDef, nullptr);
        ASSERT_NE(symbolizer, nullptr);
    }
}

TEST_F(S52SymbolRendererTest, GenerateCacheKey) {
    std::string key = renderer->GenerateCacheKey("test_symbol", nullptr);
    EXPECT_EQ(key, "test_symbol");
}

TEST_F(S52SymbolRendererTest, GenerateCacheKeyWithRule) {
    StyleRule rule;
    rule.ruleId = "test_rule";
    
    std::string key = renderer->GenerateCacheKey("test_symbol", &rule);
    EXPECT_NE(key.find("test_symbol"), std::string::npos);
    EXPECT_NE(key.find("test_rule"), std::string::npos);
}
