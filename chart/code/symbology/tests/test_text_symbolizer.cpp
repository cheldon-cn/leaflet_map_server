#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/text_symbolizer.h>
#include <ogc/draw/font.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/common.h"

using namespace ogc::symbology;
using namespace ogc;

class TextSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = TextSymbolizer::Create();
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    TextSymbolizerPtr symbolizer;
};

TEST_F(TextSymbolizerTest, Constructor) {
    auto sym = TextSymbolizer::Create();
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetType(), SymbolizerType::kText);
}

TEST_F(TextSymbolizerTest, ConstructorWithParams) {
    Font font("Arial", 12);
    auto sym = TextSymbolizer::Create("Test", font, 0x000000);
    ASSERT_NE(sym, nullptr);
    EXPECT_EQ(sym->GetLabel(), "Test");
    EXPECT_EQ(sym->GetColor(), 0x000000);
}

TEST_F(TextSymbolizerTest, SetLabel) {
    symbolizer->SetLabel("Hello World");
    EXPECT_EQ(symbolizer->GetLabel(), "Hello World");
}

TEST_F(TextSymbolizerTest, SetLabelProperty) {
    symbolizer->SetLabelProperty("name");
    EXPECT_EQ(symbolizer->GetLabelProperty(), "name");
}

TEST_F(TextSymbolizerTest, SetFont) {
    Font font("Times New Roman", 14, FontStyle::kBoldItalic);
    symbolizer->SetFont(font);
    
    Font retrieved = symbolizer->GetFont();
    EXPECT_EQ(retrieved.GetFamily(), "Times New Roman");
    EXPECT_DOUBLE_EQ(retrieved.GetSize(), 14);
    EXPECT_TRUE(retrieved.IsBold());
    EXPECT_TRUE(retrieved.IsItalic());
}

TEST_F(TextSymbolizerTest, SetColor) {
    symbolizer->SetColor(0xFF0000);
    EXPECT_EQ(symbolizer->GetColor(), 0xFF0000);
}

TEST_F(TextSymbolizerTest, SetOpacity) {
    symbolizer->SetOpacity(0.8);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.8);
}

TEST_F(TextSymbolizerTest, SetHorizontalAlignment) {
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kLeft);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kLeft);
    
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kRight);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kRight);
    
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kCenter);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kCenter);
}

TEST_F(TextSymbolizerTest, SetVerticalAlignment) {
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kTop);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kTop);
    
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kBottom);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kBottom);
    
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kMiddle);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kMiddle);
}

TEST_F(TextSymbolizerTest, SetPlacement) {
    symbolizer->SetPlacement(TextPlacement::kPoint);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kPoint);
    
    symbolizer->SetPlacement(TextPlacement::kLine);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kLine);
    
    symbolizer->SetPlacement(TextPlacement::kInterior);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kInterior);
}

TEST_F(TextSymbolizerTest, SetOffset) {
    symbolizer->SetOffset(5.0, 10.0);
    
    double dx, dy;
    symbolizer->GetOffset(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 5.0);
    EXPECT_DOUBLE_EQ(dy, 10.0);
}

TEST_F(TextSymbolizerTest, SetRotation) {
    symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), 45.0);
}

TEST_F(TextSymbolizerTest, SetMaxAngleDelta) {
    symbolizer->SetMaxAngleDelta(30.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetMaxAngleDelta(), 30.0);
}

TEST_F(TextSymbolizerTest, SetFollowLine) {
    symbolizer->SetFollowLine(true);
    EXPECT_TRUE(symbolizer->GetFollowLine());
    
    symbolizer->SetFollowLine(false);
    EXPECT_FALSE(symbolizer->GetFollowLine());
}

TEST_F(TextSymbolizerTest, SetMaxDisplacement) {
    symbolizer->SetMaxDisplacement(50.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetMaxDisplacement(), 50.0);
}

TEST_F(TextSymbolizerTest, SetRepeatDistance) {
    symbolizer->SetRepeatDistance(100.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRepeatDistance(), 100.0);
}

TEST_F(TextSymbolizerTest, SetHaloColor) {
    symbolizer->SetHaloColor(0xFFFFFF);
    EXPECT_EQ(symbolizer->GetHaloColor(), 0xFFFFFF);
}

TEST_F(TextSymbolizerTest, SetHaloRadius) {
    symbolizer->SetHaloRadius(2.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetHaloRadius(), 2.0);
}

TEST_F(TextSymbolizerTest, SetHaloOpacity) {
    symbolizer->SetHaloOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetHaloOpacity(), 0.5);
}

TEST_F(TextSymbolizerTest, SetBackgroundColor) {
    symbolizer->SetBackgroundColor(0xFFFF00);
    EXPECT_EQ(symbolizer->GetBackgroundColor(), 0xFFFF00);
}

TEST_F(TextSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(TextSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "TextSymbolizer");
}

TEST_F(TextSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kText);
}

TEST_F(TextSymbolizerTest, Clone) {
    Font font("Arial", 14);
    symbolizer->SetLabel("Test Label");
    symbolizer->SetFont(font);
    symbolizer->SetColor(0xFF0000);
    symbolizer->SetHaloColor(0xFFFFFF);
    symbolizer->SetHaloRadius(2.0);
    
    auto cloned = symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto textClone = std::dynamic_pointer_cast<TextSymbolizer>(cloned);
    ASSERT_NE(textClone, nullptr);
    EXPECT_EQ(textClone->GetLabel(), "Test Label");
    EXPECT_EQ(textClone->GetColor(), 0xFF0000);
    EXPECT_EQ(textClone->GetHaloColor(), 0xFFFFFF);
    EXPECT_DOUBLE_EQ(textClone->GetHaloRadius(), 2.0);
}

TEST_F(TextSymbolizerTest, TextHorizontalAlignmentValues) {
    EXPECT_NE(static_cast<int>(TextHorizontalAlignment::kLeft), static_cast<int>(TextHorizontalAlignment::kCenter));
    EXPECT_NE(static_cast<int>(TextHorizontalAlignment::kCenter), static_cast<int>(TextHorizontalAlignment::kRight));
}

TEST_F(TextSymbolizerTest, TextVerticalAlignmentValues) {
    EXPECT_NE(static_cast<int>(TextVerticalAlignment::kTop), static_cast<int>(TextVerticalAlignment::kMiddle));
    EXPECT_NE(static_cast<int>(TextVerticalAlignment::kMiddle), static_cast<int>(TextVerticalAlignment::kBottom));
}

TEST_F(TextSymbolizerTest, TextPlacementValues) {
    EXPECT_NE(static_cast<int>(TextPlacement::kPoint), static_cast<int>(TextPlacement::kLine));
    EXPECT_NE(static_cast<int>(TextPlacement::kLine), static_cast<int>(TextPlacement::kInterior));
}
