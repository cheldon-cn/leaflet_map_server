#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/composite_symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
#include "ogc/point.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc;

class CompositeSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_symbolizer = CompositeSymbolizer::Create();
        ASSERT_NE(m_symbolizer, nullptr);
    }
    
    void TearDown() override {
        m_symbolizer.reset();
    }
    
    CompositeSymbolizerPtr m_symbolizer;
};

TEST_F(CompositeSymbolizerTest, Create) {
    EXPECT_NE(m_symbolizer, nullptr);
}

TEST_F(CompositeSymbolizerTest, GetType) {
    EXPECT_EQ(m_symbolizer->GetType(), SymbolizerType::kComposite);
}

TEST_F(CompositeSymbolizerTest, GetName) {
    EXPECT_EQ(m_symbolizer->GetName(), "CompositeSymbolizer");
}

TEST_F(CompositeSymbolizerTest, AddSymbolizer) {
    auto lineSym = LineSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    EXPECT_EQ(m_symbolizer->GetSymbolizerCount(), 1);
}

TEST_F(CompositeSymbolizerTest, AddMultipleSymbolizers) {
    auto lineSym = LineSymbolizer::Create();
    auto polySym = PolygonSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->AddSymbolizer(polySym);
    EXPECT_EQ(m_symbolizer->GetSymbolizerCount(), 2);
}

TEST_F(CompositeSymbolizerTest, RemoveSymbolizer) {
    auto lineSym = LineSymbolizer::Create();
    auto polySym = PolygonSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->AddSymbolizer(polySym);
    m_symbolizer->RemoveSymbolizer(0);
    EXPECT_EQ(m_symbolizer->GetSymbolizerCount(), 1);
}

TEST_F(CompositeSymbolizerTest, ClearSymbolizers) {
    auto lineSym = LineSymbolizer::Create();
    auto polySym = PolygonSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->AddSymbolizer(polySym);
    m_symbolizer->ClearSymbolizers();
    EXPECT_EQ(m_symbolizer->GetSymbolizerCount(), 0);
}

TEST_F(CompositeSymbolizerTest, GetSymbolizer) {
    auto lineSym = LineSymbolizer::Create();
    auto polySym = PolygonSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->AddSymbolizer(polySym);
    
    auto retrieved = m_symbolizer->GetSymbolizer(0);
    EXPECT_EQ(retrieved, lineSym);
    
    retrieved = m_symbolizer->GetSymbolizer(1);
    EXPECT_EQ(retrieved, polySym);
}

TEST_F(CompositeSymbolizerTest, GetSymbolizers) {
    auto lineSym = LineSymbolizer::Create();
    auto polySym = PolygonSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->AddSymbolizer(polySym);
    
    auto symbolizers = m_symbolizer->GetSymbolizers();
    EXPECT_EQ(symbolizers.size(), 2);
}

TEST_F(CompositeSymbolizerTest, SetSymbolizers) {
    std::vector<SymbolizerPtr> symbolizers;
    symbolizers.push_back(LineSymbolizer::Create());
    symbolizers.push_back(PolygonSymbolizer::Create());
    
    m_symbolizer->SetSymbolizers(symbolizers);
    EXPECT_EQ(m_symbolizer->GetSymbolizerCount(), 2);
}

TEST_F(CompositeSymbolizerTest, SetGetCompositionMode) {
    m_symbolizer->SetCompositionMode("multiply");
    EXPECT_EQ(m_symbolizer->GetCompositionMode(), "multiply");
}

TEST_F(CompositeSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(m_symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(m_symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(m_symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(CompositeSymbolizerTest, Clone) {
    auto lineSym = LineSymbolizer::Create();
    m_symbolizer->AddSymbolizer(lineSym);
    m_symbolizer->SetCompositionMode("screen");
    
    auto cloned = m_symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto* clonedSym = dynamic_cast<CompositeSymbolizer*>(cloned.get());
    ASSERT_NE(clonedSym, nullptr);
    EXPECT_EQ(clonedSym->GetSymbolizerCount(), 1);
    EXPECT_EQ(clonedSym->GetCompositionMode(), "screen");
}
