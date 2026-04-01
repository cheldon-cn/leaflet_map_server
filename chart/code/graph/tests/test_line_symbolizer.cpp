#include <gtest/gtest.h>
#include "ogc/draw/line_symbolizer.h"
#include <ogc/draw/draw_style.h>
#include "ogc/common.h"
#include <vector>

using namespace ogc::draw;
using namespace ogc;

class LineSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = LineSymbolizer::Create();
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    LineSymbolizerPtr symbolizer;
};

TEST_F(LineSymbolizerTest, Constructor) {
    auto sym = LineSymbolizer::Create();
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetType(), SymbolizerType::kLine);
}

TEST_F(LineSymbolizerTest, ConstructorWithParams) {
    auto sym = LineSymbolizer::Create(2.0, 0xFF0000);
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetWidth(), 2.0);
    EXPECT_EQ(sym->GetColor(), 0xFF0000);
}

TEST_F(LineSymbolizerTest, SetWidth) {
    symbolizer->SetWidth(3.0);
    EXPECT_EQ(symbolizer->GetWidth(), 3.0);
}

TEST_F(LineSymbolizerTest, SetColor) {
    symbolizer->SetColor(0x00FF00);
    EXPECT_EQ(symbolizer->GetColor(), 0x00FF00);
}

TEST_F(LineSymbolizerTest, SetOpacity) {
    symbolizer->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.5);
}

TEST_F(LineSymbolizerTest, SetCapStyle) {
    symbolizer->SetCapStyle(LineCap::kRound);
    EXPECT_EQ(symbolizer->GetCapStyle(), LineCap::kRound);
    
    symbolizer->SetCapStyle(LineCap::kSquare);
    EXPECT_EQ(symbolizer->GetCapStyle(), LineCap::kSquare);
}

TEST_F(LineSymbolizerTest, SetJoinStyle) {
    symbolizer->SetJoinStyle(LineJoin::kRound);
    EXPECT_EQ(symbolizer->GetJoinStyle(), LineJoin::kRound);
    
    symbolizer->SetJoinStyle(LineJoin::kMiter);
    EXPECT_EQ(symbolizer->GetJoinStyle(), LineJoin::kMiter);
}

TEST_F(LineSymbolizerTest, SetDashStyle) {
    symbolizer->SetDashStyle(DashStyle::kDash);
    EXPECT_EQ(symbolizer->GetDashStyle(), DashStyle::kDash);
    
    symbolizer->SetDashStyle(DashStyle::kDot);
    EXPECT_EQ(symbolizer->GetDashStyle(), DashStyle::kDot);
}

TEST_F(LineSymbolizerTest, SetDashPattern) {
    std::vector<double> pattern = {5.0, 3.0, 1.0, 3.0};
    symbolizer->SetDashPattern(pattern);
    
    auto retrieved = symbolizer->GetDashPattern();
    EXPECT_EQ(retrieved.size(), 4);
    EXPECT_DOUBLE_EQ(retrieved[0], 5.0);
    EXPECT_DOUBLE_EQ(retrieved[1], 3.0);
}

TEST_F(LineSymbolizerTest, SetDashOffset) {
    symbolizer->SetDashOffset(2.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetDashOffset(), 2.5);
}

TEST_F(LineSymbolizerTest, SetOffset) {
    symbolizer->SetOffset(5.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetOffset(), 5.0);
}

TEST_F(LineSymbolizerTest, SetPerpendicularOffset) {
    symbolizer->SetPerpendicularOffset(10.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetPerpendicularOffset(), 10.0);
}

TEST_F(LineSymbolizerTest, SetGraphicStroke) {
    symbolizer->SetGraphicStroke(true);
    EXPECT_TRUE(symbolizer->HasGraphicStroke());
    
    symbolizer->SetGraphicStroke(false);
    EXPECT_FALSE(symbolizer->HasGraphicStroke());
}

TEST_F(LineSymbolizerTest, SetGraphicStrokeSize) {
    symbolizer->SetGraphicStrokeSize(8.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicStrokeSize(), 8.0);
}

TEST_F(LineSymbolizerTest, SetGraphicStrokeSpacing) {
    symbolizer->SetGraphicStrokeSpacing(20.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicStrokeSpacing(), 20.0);
}

TEST_F(LineSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kMultiPolygon));
}

TEST_F(LineSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "LineSymbolizer");
}

TEST_F(LineSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kLine);
}

TEST_F(LineSymbolizerTest, Clone) {
    symbolizer->SetWidth(2.5);
    symbolizer->SetColor(0xFF0000);
    symbolizer->SetDashStyle(DashStyle::kDash);
    
    auto cloned = symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto lineClone = std::dynamic_pointer_cast<LineSymbolizer>(cloned);
    ASSERT_NE(lineClone, nullptr);
    EXPECT_EQ(lineClone->GetWidth(), 2.5);
    EXPECT_EQ(lineClone->GetColor(), 0xFF0000);
    EXPECT_EQ(lineClone->GetDashStyle(), DashStyle::kDash);
}

TEST_F(LineSymbolizerTest, DashStyleValues) {
    EXPECT_NE(static_cast<int>(DashStyle::kSolid), static_cast<int>(DashStyle::kDash));
    EXPECT_NE(static_cast<int>(DashStyle::kDash), static_cast<int>(DashStyle::kDot));
    EXPECT_NE(static_cast<int>(DashStyle::kDot), static_cast<int>(DashStyle::kDashDot));
    EXPECT_NE(static_cast<int>(DashStyle::kDashDot), static_cast<int>(DashStyle::kDashDotDot));
    EXPECT_NE(static_cast<int>(DashStyle::kDashDotDot), static_cast<int>(DashStyle::kCustom));
}
