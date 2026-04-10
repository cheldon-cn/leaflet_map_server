#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/common.h"

using namespace ogc::symbology;
using namespace ogc::draw;
using namespace ogc;

class PointSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = PointSymbolizer::Create();
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    PointSymbolizerPtr symbolizer;
};

TEST_F(PointSymbolizerTest, Constructor) {
    auto sym = PointSymbolizer::Create();
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetType(), SymbolizerType::kPoint);
}

TEST_F(PointSymbolizerTest, ConstructorWithParams) {
    auto sym = PointSymbolizer::Create(10.0, 0xFF0000);
    ASSERT_NE(sym, nullptr);
    EXPECT_DOUBLE_EQ(sym->GetSize(), 10.0);
    EXPECT_EQ(sym->GetColor(), 0xFF0000);
}

TEST_F(PointSymbolizerTest, SetSymbolType) {
    symbolizer->SetSymbolType(PointSymbolType::kSquare);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kSquare);
    
    symbolizer->SetSymbolType(PointSymbolType::kTriangle);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kTriangle);
    
    symbolizer->SetSymbolType(PointSymbolType::kStar);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kStar);
}

TEST_F(PointSymbolizerTest, SetSize) {
    symbolizer->SetSize(15.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetSize(), 15.0);
}

TEST_F(PointSymbolizerTest, SetColor) {
    symbolizer->SetColor(0x00FF00);
    EXPECT_EQ(symbolizer->GetColor(), 0x00FF00);
}

TEST_F(PointSymbolizerTest, SetStrokeColor) {
    symbolizer->SetStrokeColor(0x0000FF);
    EXPECT_EQ(symbolizer->GetStrokeColor(), 0x0000FF);
}

TEST_F(PointSymbolizerTest, SetStrokeWidth) {
    symbolizer->SetStrokeWidth(2.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeWidth(), 2.0);
}

TEST_F(PointSymbolizerTest, SetRotation) {
    symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), 45.0);
}

TEST_F(PointSymbolizerTest, SetAnchorPoint) {
    symbolizer->SetAnchorPoint(0.5, 0.5);
    
    double x, y;
    symbolizer->GetAnchorPoint(x, y);
    
    EXPECT_DOUBLE_EQ(x, 0.5);
    EXPECT_DOUBLE_EQ(y, 0.5);
}

TEST_F(PointSymbolizerTest, SetDisplacement) {
    symbolizer->SetDisplacement(10.0, 20.0);
    
    double dx, dy;
    symbolizer->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 10.0);
    EXPECT_DOUBLE_EQ(dy, 20.0);
}

TEST_F(PointSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(PointSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "PointSymbolizer");
}

TEST_F(PointSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPoint);
}

TEST_F(PointSymbolizerTest, Clone) {
    symbolizer->SetSize(12.0);
    symbolizer->SetColor(0xFF0000);
    symbolizer->SetSymbolType(PointSymbolType::kStar);
    symbolizer->SetRotation(30.0);
    
    auto cloned = symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto pointClone = std::dynamic_pointer_cast<PointSymbolizer>(cloned);
    ASSERT_NE(pointClone, nullptr);
    EXPECT_DOUBLE_EQ(pointClone->GetSize(), 12.0);
    EXPECT_EQ(pointClone->GetColor(), 0xFF0000);
    EXPECT_EQ(pointClone->GetSymbolType(), PointSymbolType::kStar);
    EXPECT_DOUBLE_EQ(pointClone->GetRotation(), 30.0);
}

TEST_F(PointSymbolizerTest, PointSymbolTypeValues) {
    EXPECT_NE(static_cast<int>(PointSymbolType::kCircle), static_cast<int>(PointSymbolType::kSquare));
    EXPECT_NE(static_cast<int>(PointSymbolType::kSquare), static_cast<int>(PointSymbolType::kTriangle));
    EXPECT_NE(static_cast<int>(PointSymbolType::kTriangle), static_cast<int>(PointSymbolType::kStar));
    EXPECT_NE(static_cast<int>(PointSymbolType::kStar), static_cast<int>(PointSymbolType::kCross));
    EXPECT_NE(static_cast<int>(PointSymbolType::kCross), static_cast<int>(PointSymbolType::kDiamond));
}
