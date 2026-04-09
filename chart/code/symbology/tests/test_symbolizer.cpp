#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include <ogc/symbology/symbolizer/text_symbolizer.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/common.h"

using namespace ogc::symbology;
using namespace ogc;

class SymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SymbolizerTest, SymbolizerType) {
    EXPECT_NE(static_cast<int>(SymbolizerType::kPoint), static_cast<int>(SymbolizerType::kLine));
    EXPECT_NE(static_cast<int>(SymbolizerType::kLine), static_cast<int>(SymbolizerType::kPolygon));
    EXPECT_NE(static_cast<int>(SymbolizerType::kPolygon), static_cast<int>(SymbolizerType::kText));
    EXPECT_NE(static_cast<int>(SymbolizerType::kText), static_cast<int>(SymbolizerType::kRaster));
}

TEST_F(SymbolizerTest, LineSymbolizerType) {
    auto symbolizer = LineSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kLine);
    EXPECT_EQ(symbolizer->GetName(), "LineSymbolizer");
}

TEST_F(SymbolizerTest, PolygonSymbolizerType) {
    auto symbolizer = PolygonSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPolygon);
    EXPECT_EQ(symbolizer->GetName(), "PolygonSymbolizer");
}

TEST_F(SymbolizerTest, PointSymbolizerType) {
    auto symbolizer = PointSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPoint);
    EXPECT_EQ(symbolizer->GetName(), "PointSymbolizer");
}

TEST_F(SymbolizerTest, TextSymbolizerType) {
    auto symbolizer = TextSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kText);
    EXPECT_EQ(symbolizer->GetName(), "TextSymbolizer");
}

TEST_F(SymbolizerTest, SymbolizerSetEnabled) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetEnabled(true);
    EXPECT_TRUE(symbolizer->IsEnabled());
    
    symbolizer->SetEnabled(false);
    EXPECT_FALSE(symbolizer->IsEnabled());
}

TEST_F(SymbolizerTest, SymbolizerSetMinScale) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetMinScale(1000.0);
    EXPECT_EQ(symbolizer->GetMinScale(), 1000.0);
}

TEST_F(SymbolizerTest, SymbolizerSetMaxScale) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetMaxScale(100000.0);
    EXPECT_EQ(symbolizer->GetMaxScale(), 100000.0);
}

TEST_F(SymbolizerTest, SymbolizerIsVisibleAtScale) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetMinScale(1000.0);
    symbolizer->SetMaxScale(100000.0);
    
    EXPECT_TRUE(symbolizer->IsVisibleAtScale(50000.0));
    EXPECT_FALSE(symbolizer->IsVisibleAtScale(500.0));
    EXPECT_FALSE(symbolizer->IsVisibleAtScale(200000.0));
}

TEST_F(SymbolizerTest, SymbolizerSetZIndex) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetZIndex(10);
    EXPECT_EQ(symbolizer->GetZIndex(), 10);
}

TEST_F(SymbolizerTest, SymbolizerSetOpacity) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.5);
}

TEST_F(SymbolizerTest, SymbolizerSetDefaultStyle) {
    auto symbolizer = LineSymbolizer::Create();
    
    DrawStyle style;
    style.pen.color = Color(255, 0, 0, 255);
    style.pen.width = 2.0;
    
    symbolizer->SetDefaultStyle(style);
    DrawStyle retrieved = symbolizer->GetDefaultStyle();
    
    EXPECT_EQ(retrieved.pen.color, Color(255, 0, 0, 255));
    EXPECT_DOUBLE_EQ(retrieved.pen.width, 2.0);
}

TEST_F(SymbolizerTest, LineSymbolizerCanSymbolize) {
    auto symbolizer = LineSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(SymbolizerTest, PolygonSymbolizerCanSymbolize) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPolygon));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
}

TEST_F(SymbolizerTest, PointSymbolizerCanSymbolize) {
    auto symbolizer = PointSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(SymbolizerTest, TextSymbolizerCanSymbolize) {
    auto symbolizer = TextSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(SymbolizerTest, CreateSymbolizerByType) {
    auto pointSym = PointSymbolizer::Create();
    ASSERT_NE(pointSym, nullptr);
    EXPECT_EQ(pointSym->GetType(), SymbolizerType::kPoint);
    
    auto lineSym = LineSymbolizer::Create();
    ASSERT_NE(lineSym, nullptr);
    EXPECT_EQ(lineSym->GetType(), SymbolizerType::kLine);
    
    auto polygonSym = PolygonSymbolizer::Create();
    ASSERT_NE(polygonSym, nullptr);
    EXPECT_EQ(polygonSym->GetType(), SymbolizerType::kPolygon);
    
    auto textSym = TextSymbolizer::Create();
    ASSERT_NE(textSym, nullptr);
    EXPECT_EQ(textSym->GetType(), SymbolizerType::kText);
}
