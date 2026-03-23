#include <gtest/gtest.h>
#include "ogc/draw/rule_engine.h"
#include "ogc/draw/symbolizer_rule.h"
#include "ogc/draw/filter.h"
#include "ogc/draw/line_symbolizer.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class RuleEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_engine = RuleEngine::Create();
        ASSERT_NE(m_engine, nullptr);
    }
    
    void TearDown() override {
        m_engine.reset();
    }
    
    RuleEnginePtr m_engine;
};

TEST_F(RuleEngineTest, EngineCreation) {
    EXPECT_NE(m_engine, nullptr);
    EXPECT_EQ(m_engine->GetRuleCount(), 0);
}

TEST_F(RuleEngineTest, AddRule) {
    auto rule = std::make_shared<SymbolizerRule>();
    m_engine->AddRule(rule);
    EXPECT_EQ(m_engine->GetRuleCount(), 1);
}

TEST_F(RuleEngineTest, AddMultipleRules) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    rule1->SetName("Rule1");
    auto rule2 = std::make_shared<SymbolizerRule>();
    rule2->SetName("Rule2");
    
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    EXPECT_EQ(m_engine->GetRuleCount(), 2);
}

TEST_F(RuleEngineTest, RemoveRule) {
    auto rule = std::make_shared<SymbolizerRule>();
    m_engine->AddRule(rule);
    m_engine->RemoveRule(rule);
    EXPECT_EQ(m_engine->GetRuleCount(), 0);
}

TEST_F(RuleEngineTest, ClearRules) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    auto rule2 = std::make_shared<SymbolizerRule>();
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    m_engine->ClearRules();
    EXPECT_EQ(m_engine->GetRuleCount(), 0);
}

TEST_F(RuleEngineTest, GetRule) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    rule1->SetName("Rule1");
    auto rule2 = std::make_shared<SymbolizerRule>();
    rule2->SetName("Rule2");
    
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    
    auto retrieved = m_engine->GetRule(0);
    EXPECT_EQ(retrieved, rule1);
    
    retrieved = m_engine->GetRule(1);
    EXPECT_EQ(retrieved, rule2);
}

TEST_F(RuleEngineTest, GetRuleByName) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    rule1->SetName("TestRule");
    m_engine->AddRule(rule1);
    
    auto retrieved = m_engine->GetRuleByName("TestRule");
    EXPECT_EQ(retrieved, rule1);
    
    retrieved = m_engine->GetRuleByName("NonExistent");
    EXPECT_EQ(retrieved, nullptr);
}

TEST_F(RuleEngineTest, GetRules) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    auto rule2 = std::make_shared<SymbolizerRule>();
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    
    const auto& rules = m_engine->GetRules();
    EXPECT_EQ(rules.size(), 2);
}

TEST_F(RuleEngineTest, SetElseFilterEnabled) {
    m_engine->SetElseFilterEnabled(true);
    EXPECT_TRUE(m_engine->IsElseFilterEnabled());
    
    m_engine->SetElseFilterEnabled(false);
    EXPECT_FALSE(m_engine->IsElseFilterEnabled());
}

TEST_F(RuleEngineTest, SortRulesByPriority) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    rule1->SetName("Rule1");
    rule1->SetPriority(2);
    
    auto rule2 = std::make_shared<SymbolizerRule>();
    rule2->SetName("Rule2");
    rule2->SetPriority(1);
    
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    
    m_engine->SortRulesByPriority();
    
    EXPECT_EQ(m_engine->GetRule(0)->GetPriority(), 1);
    EXPECT_EQ(m_engine->GetRule(1)->GetPriority(), 2);
}

TEST_F(RuleEngineTest, SortRulesByName) {
    auto rule1 = std::make_shared<SymbolizerRule>();
    rule1->SetName("Beta");
    
    auto rule2 = std::make_shared<SymbolizerRule>();
    rule2->SetName("Alpha");
    
    m_engine->AddRule(rule1);
    m_engine->AddRule(rule2);
    
    m_engine->SortRulesByName();
    
    EXPECT_EQ(m_engine->GetRule(0)->GetName(), "Alpha");
    EXPECT_EQ(m_engine->GetRule(1)->GetName(), "Beta");
}

TEST_F(RuleEngineTest, GetMatchingRulesEmpty) {
    CNFeature feature;
    auto rules = m_engine->GetMatchingRules(&feature, 1.0);
    EXPECT_TRUE(rules.empty());
}

TEST_F(RuleEngineTest, MatchEmpty) {
    CNFeature feature;
    auto result = m_engine->Match(&feature, 1.0);
    EXPECT_FALSE(result.matched);
}

TEST_F(RuleEngineTest, GetSymbolizersEmpty) {
    CNFeature feature;
    auto symbolizers = m_engine->GetSymbolizers(&feature, 1.0);
    EXPECT_TRUE(symbolizers.empty());
}

TEST_F(RuleEngineTest, GetExtent) {
    auto extent = m_engine->GetExtent();
    EXPECT_TRUE(extent.IsNull());
}

TEST_F(RuleEngineTest, GetMinScaleDenominator) {
    double minScale = m_engine->GetMinScaleDenominator();
    EXPECT_DOUBLE_EQ(minScale, 0.0);
}

TEST_F(RuleEngineTest, GetMaxScaleDenominator) {
    double maxScale = m_engine->GetMaxScaleDenominator();
    EXPECT_GE(maxScale, 0.0);
}

TEST_F(RuleEngineTest, Clone) {
    auto rule = std::make_shared<SymbolizerRule>();
    rule->SetName("TestRule");
    m_engine->AddRule(rule);
    
    auto cloned = m_engine->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetRuleCount(), 1);
}
