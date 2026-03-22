#include <gtest/gtest.h>
#include "ogc/draw/polygon_symbolizer.h"
#include "ogc/draw/draw_style.h"
#include "ogc/common.h"

using namespace ogc::draw;
using namespace ogc;

class PolygonSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = PolygonSymbolizer::Create();
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    PolygonSymbolizerPtr symbolizer;
};

TEST_F(PolygonSymbolizerTest, Constructor) {
    auto sym = PolygonSymbolizer::Create();
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetType(), SymbolizerType::kPolygon);
}

TEST_F(PolygonSymbolizerTest, ConstructorWithFillColor) {
    auto sym = PolygonSymbolizer::Create(0xFF0000);
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetFillColor(), 0xFF0000);
}

TEST_F(PolygonSymbolizerTest, ConstructorWithFillAndStroke) {
    auto sym = PolygonSymbolizer::Create(0xFF0000, 0x000000, 2.0);
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetFillColor(), 0xFF0000);
    EXPECT_EQ(sym->GetStrokeColor(), 0x000000);
    EXPECT_DOUBLE_EQ(sym->GetStrokeWidth(), 2.0);
}

TEST_F(PolygonSymbolizerTest, SetFillColor) {
    symbolizer->SetFillColor(0x00FF00);
    EXPECT_EQ(symbolizer->GetFillColor(), 0x00FF00);
}

TEST_F(PolygonSymbolizerTest, SetFillOpacity) {
    symbolizer->SetFillOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetFillOpacity(), 0.5);
}

TEST_F(PolygonSymbolizerTest, SetStrokeColor) {
    symbolizer->SetStrokeColor(0x0000FF);
    EXPECT_EQ(symbolizer->GetStrokeColor(), 0x0000FF);
}

TEST_F(PolygonSymbolizerTest, SetStrokeWidth) {
    symbolizer->SetStrokeWidth(3.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeWidth(), 3.0);
}

TEST_F(PolygonSymbolizerTest, SetStrokeOpacity) {
    symbolizer->SetStrokeOpacity(0.7);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeOpacity(), 0.7);
}

TEST_F(PolygonSymbolizerTest, SetFillPattern) {
    symbolizer->SetFillPattern(FillPattern::kHorizontal);
    EXPECT_EQ(symbolizer->GetFillPattern(), FillPattern::kHorizontal);
    
    symbolizer->SetFillPattern(FillPattern::kCross);
    EXPECT_EQ(symbolizer->GetFillPattern(), FillPattern::kCross);
}

TEST_F(PolygonSymbolizerTest, SetDisplacement) {
    symbolizer->SetDisplacement(5.0, 10.0);
    
    double dx, dy;
    symbolizer->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 5.0);
    EXPECT_DOUBLE_EQ(dy, 10.0);
}

TEST_F(PolygonSymbolizerTest, SetPerpendicularOffset) {
    symbolizer->SetPerpendicularOffset(15.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetPerpendicularOffset(), 15.0);
}

TEST_F(PolygonSymbolizerTest, SetGraphicFill) {
    symbolizer->SetGraphicFill(true);
    EXPECT_TRUE(symbolizer->HasGraphicFill());
    
    symbolizer->SetGraphicFill(false);
    EXPECT_FALSE(symbolizer->HasGraphicFill());
}

TEST_F(PolygonSymbolizerTest, SetGraphicFillSize) {
    symbolizer->SetGraphicFillSize(12.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicFillSize(), 12.0);
}

TEST_F(PolygonSymbolizerTest, SetGraphicFillSpacing) {
    symbolizer->SetGraphicFillSpacing(25.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicFillSpacing(), 25.0);
}

TEST_F(PolygonSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPolygon));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kMultiLineString));
}

TEST_F(PolygonSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "PolygonSymbolizer");
}

TEST_F(PolygonSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPolygon);
}

TEST_F(PolygonSymbolizerTest, Clone) {
    symbolizer->SetFillColor(0xFF0000);
    symbolizer->SetStrokeColor(0x000000);
    symbolizer->SetStrokeWidth(2.0);
    symbolizer->SetFillPattern(FillPattern::kDiagonal);
    
    auto cloned = symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto polygonClone = std::dynamic_pointer_cast<PolygonSymbolizer>(cloned);
    ASSERT_NE(polygonClone, nullptr);
    EXPECT_EQ(polygonClone->GetFillColor(), 0xFF0000);
    EXPECT_EQ(polygonClone->GetStrokeColor(), 0x000000);
    EXPECT_DOUBLE_EQ(polygonClone->GetStrokeWidth(), 2.0);
    EXPECT_EQ(polygonClone->GetFillPattern(), FillPattern::kDiagonal);
}

TEST_F(PolygonSymbolizerTest, FillPatternValues) {
    EXPECT_NE(static_cast<int>(FillPattern::kSolid), static_cast<int>(FillPattern::kHorizontal));
    EXPECT_NE(static_cast<int>(FillPattern::kHorizontal), static_cast<int>(FillPattern::kVertical));
    EXPECT_NE(static_cast<int>(FillPattern::kVertical), static_cast<int>(FillPattern::kCross));
    EXPECT_NE(static_cast<int>(FillPattern::kCross), static_cast<int>(FillPattern::kDiagonal));
    EXPECT_NE(static_cast<int>(FillPattern::kDiagonal), static_cast<int>(FillPattern::kDiagonalCross));
    EXPECT_NE(static_cast<int>(FillPattern::kDiagonalCross), static_cast<int>(FillPattern::kDot));
    EXPECT_NE(static_cast<int>(FillPattern::kDot), static_cast<int>(FillPattern::kCustom));
}
