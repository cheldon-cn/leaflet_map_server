#include <gtest/gtest.h>
#include <ogc/symbology/filter/symbolizer_rule.h>
#include <ogc/symbology/filter/filter.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>
#include "ogc/geom/point.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc;

class SymbolizerRuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_rule = SymbolizerRule::Create();
        ASSERT_NE(m_rule, nullptr);
    }
    
    void TearDown() override {
        m_rule.reset();
    }
    
    SymbolizerRulePtr m_rule;
};

TEST_F(SymbolizerRuleTest, Create) {
    EXPECT_NE(m_rule, nullptr);
}

TEST_F(SymbolizerRuleTest, CreateWithName) {
    auto rule = SymbolizerRule::Create("TestRule");
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->GetName(), "TestRule");
}

TEST_F(SymbolizerRuleTest, SetGetName) {
    m_rule->SetName("MyRule");
    EXPECT_EQ(m_rule->GetName(), "MyRule");
}

TEST_F(SymbolizerRuleTest, SetGetTitle) {
    m_rule->SetTitle("Rule Title");
    EXPECT_EQ(m_rule->GetTitle(), "Rule Title");
}

TEST_F(SymbolizerRuleTest, SetGetAbstract) {
    m_rule->SetAbstract("Rule description");
    EXPECT_EQ(m_rule->GetAbstract(), "Rule description");
}

TEST_F(SymbolizerRuleTest, SetGetFilter) {
    auto filter = Filter::CreateAll();
    m_rule->SetFilter(filter);
    EXPECT_EQ(m_rule->GetFilter(), filter);
    EXPECT_TRUE(m_rule->HasFilter());
}

TEST_F(SymbolizerRuleTest, HasFilterFalse) {
    EXPECT_FALSE(m_rule->HasFilter());
}

TEST_F(SymbolizerRuleTest, SetGetMinScaleDenominator) {
    m_rule->SetMinScaleDenominator(1000.0);
    EXPECT_DOUBLE_EQ(m_rule->GetMinScaleDenominator(), 1000.0);
}

TEST_F(SymbolizerRuleTest, SetGetMaxScaleDenominator) {
    m_rule->SetMaxScaleDenominator(100000.0);
    EXPECT_DOUBLE_EQ(m_rule->GetMaxScaleDenominator(), 100000.0);
}

TEST_F(SymbolizerRuleTest, IsScaleInRange) {
    m_rule->SetMinScaleDenominator(1000.0);
    m_rule->SetMaxScaleDenominator(100000.0);
    
    EXPECT_TRUE(m_rule->IsScaleInRange(50000.0));
    EXPECT_TRUE(m_rule->IsScaleInRange(1000.0));
    EXPECT_TRUE(m_rule->IsScaleInRange(100000.0));
    EXPECT_FALSE(m_rule->IsScaleInRange(500.0));
    EXPECT_FALSE(m_rule->IsScaleInRange(200000.0));
}

TEST_F(SymbolizerRuleTest, AddSymbolizer) {
    auto symbolizer = LineSymbolizer::Create();
    m_rule->AddSymbolizer(symbolizer);
    EXPECT_EQ(m_rule->GetSymbolizerCount(), 1);
    EXPECT_TRUE(m_rule->HasSymbolizers());
}

TEST_F(SymbolizerRuleTest, AddMultipleSymbolizers) {
    auto sym1 = LineSymbolizer::Create();
    auto sym2 = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym1);
    m_rule->AddSymbolizer(sym2);
    EXPECT_EQ(m_rule->GetSymbolizerCount(), 2);
}

TEST_F(SymbolizerRuleTest, RemoveSymbolizer) {
    auto sym1 = LineSymbolizer::Create();
    auto sym2 = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym1);
    m_rule->AddSymbolizer(sym2);
    m_rule->RemoveSymbolizer(sym1);
    EXPECT_EQ(m_rule->GetSymbolizerCount(), 1);
}

TEST_F(SymbolizerRuleTest, ClearSymbolizers) {
    auto sym1 = LineSymbolizer::Create();
    auto sym2 = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym1);
    m_rule->AddSymbolizer(sym2);
    m_rule->ClearSymbolizers();
    EXPECT_EQ(m_rule->GetSymbolizerCount(), 0);
    EXPECT_FALSE(m_rule->HasSymbolizers());
}

TEST_F(SymbolizerRuleTest, GetSymbolizer) {
    auto sym1 = LineSymbolizer::Create();
    auto sym2 = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym1);
    m_rule->AddSymbolizer(sym2);
    
    auto retrieved = m_rule->GetSymbolizer(0);
    EXPECT_EQ(retrieved, sym1);
    
    retrieved = m_rule->GetSymbolizer(1);
    EXPECT_EQ(retrieved, sym2);
}

TEST_F(SymbolizerRuleTest, GetSymbolizers) {
    auto sym1 = LineSymbolizer::Create();
    auto sym2 = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym1);
    m_rule->AddSymbolizer(sym2);
    
    const auto& symbolizers = m_rule->GetSymbolizers();
    EXPECT_EQ(symbolizers.size(), 2);
}

TEST_F(SymbolizerRuleTest, SetGetExtent) {
    Envelope extent(0, 0, 100, 100);
    m_rule->SetExtent(extent);
    EXPECT_TRUE(m_rule->HasExtent());
}

TEST_F(SymbolizerRuleTest, HasExtentFalse) {
    EXPECT_FALSE(m_rule->HasExtent());
}

TEST_F(SymbolizerRuleTest, SetGetElseFilter) {
    m_rule->SetElseFilter(true);
    EXPECT_TRUE(m_rule->IsElseFilter());
    
    m_rule->SetElseFilter(false);
    EXPECT_FALSE(m_rule->IsElseFilter());
}

TEST_F(SymbolizerRuleTest, SetGetPriority) {
    m_rule->SetPriority(10);
    EXPECT_EQ(m_rule->GetPriority(), 10);
}

TEST_F(SymbolizerRuleTest, Clone) {
    m_rule->SetName("TestRule");
    m_rule->SetPriority(5);
    auto sym = LineSymbolizer::Create();
    m_rule->AddSymbolizer(sym);
    
    auto cloned = m_rule->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), "TestRule");
    EXPECT_EQ(cloned->GetPriority(), 5);
    EXPECT_EQ(cloned->GetSymbolizerCount(), 1);
}

TEST_F(SymbolizerRuleTest, EvaluateFeatureNoFilter) {
    CNFeature feature;
    EXPECT_TRUE(m_rule->Evaluate(&feature));
}

TEST_F(SymbolizerRuleTest, EvaluateFeatureWithFilter) {
    auto filter = Filter::CreateNone();
    m_rule->SetFilter(filter);
    
    CNFeature feature;
    EXPECT_FALSE(m_rule->Evaluate(&feature));
}

TEST_F(SymbolizerRuleTest, EvaluateGeometryNoFilter) {
    auto geom = ogc::Point::Create(0, 0);
    EXPECT_TRUE(m_rule->Evaluate(geom.get()));
}
