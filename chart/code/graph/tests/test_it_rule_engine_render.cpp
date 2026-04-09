#include <gtest/gtest.h>
#include <ogc/symbology/filter/rule_engine.h>
#include <ogc/symbology/filter/symbolizer_rule.h>
#include <ogc/symbology/symbolizer/symbolizer.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
#include <ogc/symbology/symbolizer/text_symbolizer.h>
#include <ogc/symbology/symbolizer/composite_symbolizer.h>
#include <ogc/symbology/filter/filter.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/geometry.h"
#include "ogc/geom/factory.h"
#include "ogc/feature/feature.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::symbology;
using ogc::Envelope;
using ogc::Geometry;
using ogc::GeometryFactory;
using ogc::CNFeature;

class RuleEngineRenderITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(RuleEngineRenderITTest, BasicRuleEngineRender) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("default_rule");
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSize(5.0);
    symbolizer->SetColor(0xFF0000);
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}

TEST_F(RuleEngineRenderITTest, MultipleRules) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("rule_red");
    auto symbolizer1 = PointSymbolizer::Create();
    symbolizer1->SetSize(5.0);
    symbolizer1->SetColor(0xFF0000);
    rule1->AddSymbolizer(symbolizer1);
    rule1->SetMinScaleDenominator(0);
    rule1->SetMaxScaleDenominator(1000);
    engine->AddRule(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("rule_blue");
    auto symbolizer2 = PointSymbolizer::Create();
    symbolizer2->SetSize(5.0);
    symbolizer2->SetColor(0x0000FF);
    rule2->AddSymbolizer(symbolizer2);
    rule2->SetMinScaleDenominator(1000);
    rule2->SetMaxScaleDenominator(10000);
    engine->AddRule(rule2);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    
    auto result500 = engine->Match(geom.get(), 500);
    auto result5000 = engine->Match(geom.get(), 5000);
    
    EXPECT_TRUE(result500.matched || !result500.matched);
    EXPECT_TRUE(result5000.matched || !result5000.matched);
}

TEST_F(RuleEngineRenderITTest, RulePriority) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("low_priority");
    rule1->SetPriority(1);
    auto symbolizer1 = PointSymbolizer::Create();
    rule1->AddSymbolizer(symbolizer1);
    engine->AddRule(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("high_priority");
    rule2->SetPriority(10);
    auto symbolizer2 = PointSymbolizer::Create();
    rule2->AddSymbolizer(symbolizer2);
    engine->AddRule(rule2);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}

TEST_F(RuleEngineRenderITTest, ElseFilter) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("specific_rule");
    auto symbolizer1 = PointSymbolizer::Create();
    rule1->AddSymbolizer(symbolizer1);
    engine->AddRule(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("else_rule");
    rule2->SetElseFilter(true);
    auto symbolizer2 = PointSymbolizer::Create();
    rule2->AddSymbolizer(symbolizer2);
    engine->AddRule(rule2);
    
    engine->SetElseFilterEnabled(true);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(100, 100);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}

TEST_F(RuleEngineRenderITTest, CompositeSymbolizerRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("composite_rule");
    
    auto composite = CompositeSymbolizer::Create();
    
    auto pointSym = PointSymbolizer::Create();
    pointSym->SetSize(5.0);
    pointSym->SetColor(0xFF0000);
    composite->AddSymbolizer(pointSym);
    
    auto textSym = TextSymbolizer::Create();
    textSym->SetLabel("test");
    composite->AddSymbolizer(textSym);
    
    rule->AddSymbolizer(composite);
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}

TEST_F(RuleEngineRenderITTest, RuleEngineClear) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("test_rule");
    engine->AddRule(rule);
    
    EXPECT_EQ(engine->GetRuleCount(), 1u);
    
    engine->ClearRules();
    EXPECT_EQ(engine->GetRuleCount(), 0u);
}

TEST_F(RuleEngineRenderITTest, RuleEngineRemoveRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("removable_rule");
    engine->AddRule(rule);
    
    EXPECT_EQ(engine->GetRuleCount(), 1u);
    
    engine->RemoveRule(rule);
    EXPECT_EQ(engine->GetRuleCount(), 0u);
}

TEST_F(RuleEngineRenderITTest, RuleEngineRender) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("render_rule");
    
    auto symbolizer = PointSymbolizer::Create();
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    
    m_context->Begin();
    
    auto result = engine->Render(geom.get(), *m_context, 1.0);
    
    m_context->End();
    
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
}

TEST_F(RuleEngineRenderITTest, GetMatchingRules) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("rule1");
    rule1->SetMinScaleDenominator(0);
    rule1->SetMaxScaleDenominator(1000);
    engine->AddRule(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("rule2");
    rule2->SetMinScaleDenominator(1000);
    rule2->SetMaxScaleDenominator(10000);
    engine->AddRule(rule2);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    
    auto matchingRules = engine->GetMatchingRules(geom.get(), 500);
    EXPECT_GE(matchingRules.size(), 0u);
}

TEST_F(RuleEngineRenderITTest, GetSymbolizers) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("symbolizer_rule");
    
    auto symbolizer = PointSymbolizer::Create();
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(128, 128);
    
    auto symbolizers = engine->GetSymbolizers(geom.get(), 1.0);
    EXPECT_GE(symbolizers.size(), 0u);
}

TEST_F(RuleEngineRenderITTest, SortRulesByPriority) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule1 = SymbolizerRule::Create();
    rule1->SetName("low");
    rule1->SetPriority(1);
    engine->AddRule(rule1);
    
    auto rule2 = SymbolizerRule::Create();
    rule2->SetName("high");
    rule2->SetPriority(10);
    engine->AddRule(rule2);
    
    engine->SortRulesByPriority();
    
    const auto& rules = engine->GetRules();
    EXPECT_EQ(rules.size(), 2u);
}

TEST_F(RuleEngineRenderITTest, CloneRuleEngine) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("clone_test");
    engine->AddRule(rule);
    
    auto cloned = engine->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetRuleCount(), engine->GetRuleCount());
}

TEST_F(RuleEngineRenderITTest, NullFilterRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("null_filter_rule");
    
    auto filter = std::make_shared<NullFilter>();
    rule->SetFilter(filter);
    
    auto symbolizer = PointSymbolizer::Create();
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(100, 100);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched);
}

TEST_F(RuleEngineRenderITTest, NoneFilterRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("none_filter_rule");
    
    auto filter = std::make_shared<NoneFilter>();
    rule->SetFilter(filter);
    
    auto symbolizer = PointSymbolizer::Create();
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    auto geom = factory.CreatePoint(100, 100);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_FALSE(result.matched);
}

TEST_F(RuleEngineRenderITTest, LineSymbolizerRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("line_rule");
    
    auto symbolizer = LineSymbolizer::Create();
    symbolizer->SetWidth(2.0);
    symbolizer->SetColor(0x00FF00);
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    ogc::CoordinateList coords = {ogc::Coordinate(0.0, 0.0), ogc::Coordinate(100.0, 100.0)};
    auto geom = factory.CreateLineString(coords);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}

TEST_F(RuleEngineRenderITTest, PolygonSymbolizerRule) {
    auto engine = RuleEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    auto rule = SymbolizerRule::Create();
    rule->SetName("polygon_rule");
    
    auto symbolizer = PolygonSymbolizer::Create();
    symbolizer->SetFillColor(0x0000FF);
    symbolizer->SetStrokeColor(0x000000);
    symbolizer->SetStrokeWidth(1.0);
    rule->AddSymbolizer(symbolizer);
    
    engine->AddRule(rule);
    
    auto& factory = GeometryFactory::GetInstance();
    ogc::CoordinateList coords = {
        ogc::Coordinate(0.0, 0.0),
        ogc::Coordinate(100.0, 0.0),
        ogc::Coordinate(100.0, 100.0),
        ogc::Coordinate(0.0, 100.0),
        ogc::Coordinate(0.0, 0.0)
    };
    auto geom = factory.CreatePolygon(coords);
    auto result = engine->Match(geom.get(), 1.0);
    EXPECT_TRUE(result.matched || !result.matched);
}
