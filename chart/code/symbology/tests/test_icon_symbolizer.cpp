#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/icon_symbolizer.h>
#include <ogc/draw/color.h>
#include "ogc/geom/common.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc::draw;
using namespace ogc;

class IconSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = IconSymbolizer::Create();
        ASSERT_NE(symbolizer, nullptr);
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    IconSymbolizerPtr symbolizer;
};

TEST_F(IconSymbolizerTest, DefaultConstructor) {
    IconSymbolizerPtr defaultSymbolizer = IconSymbolizer::Create();
    EXPECT_NE(defaultSymbolizer, nullptr);
}

TEST_F(IconSymbolizerTest, ConstructorWithPath) {
    IconSymbolizerPtr pathSymbolizer = IconSymbolizer::Create("icon.png");
    EXPECT_NE(pathSymbolizer, nullptr);
    EXPECT_EQ(pathSymbolizer->GetIconPath(), "icon.png");
}

TEST_F(IconSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kIcon);
}

TEST_F(IconSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "IconSymbolizer");
}

TEST_F(IconSymbolizerTest, SetIconPath) {
    symbolizer->SetIconPath("test_icon.png");
    EXPECT_EQ(symbolizer->GetIconPath(), "test_icon.png");
}

TEST_F(IconSymbolizerTest, SetIconData) {
    std::vector<uint8_t> data(16 * 16 * 4, 200);
    symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    
    EXPECT_TRUE(symbolizer->HasIconData());
}

TEST_F(IconSymbolizerTest, HasIconDataFalse) {
    EXPECT_FALSE(symbolizer->HasIconData());
}

TEST_F(IconSymbolizerTest, SetSize) {
    symbolizer->SetSize(32.0, 48.0);
    
    double width, height;
    symbolizer->GetSize(width, height);
    
    EXPECT_DOUBLE_EQ(width, 32.0);
    EXPECT_DOUBLE_EQ(height, 48.0);
}

TEST_F(IconSymbolizerTest, SetWidth) {
    symbolizer->SetWidth(24.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetWidth(), 24.0);
}

TEST_F(IconSymbolizerTest, SetHeight) {
    symbolizer->SetHeight(36.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetHeight(), 36.0);
}

TEST_F(IconSymbolizerTest, SetOpacity) {
    symbolizer->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.5);
    
    symbolizer->SetOpacity(0.8);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.8);
}

TEST_F(IconSymbolizerTest, SetRotation) {
    symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), 45.0);
    
    symbolizer->SetRotation(-30.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), -30.0);
}

TEST_F(IconSymbolizerTest, SetAnchorPoint) {
    symbolizer->SetAnchorPoint(0.5, 0.5);
    
    double x, y;
    symbolizer->GetAnchorPoint(x, y);
    
    EXPECT_DOUBLE_EQ(x, 0.5);
    EXPECT_DOUBLE_EQ(y, 0.5);
}

TEST_F(IconSymbolizerTest, SetDisplacement) {
    symbolizer->SetDisplacement(10.0, 20.0);
    
    double dx, dy;
    symbolizer->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 10.0);
    EXPECT_DOUBLE_EQ(dy, 20.0);
}

TEST_F(IconSymbolizerTest, SetAllowOverlap) {
    symbolizer->SetAllowOverlap(true);
    EXPECT_TRUE(symbolizer->GetAllowOverlap());
    
    symbolizer->SetAllowOverlap(false);
    EXPECT_FALSE(symbolizer->GetAllowOverlap());
}

TEST_F(IconSymbolizerTest, SetColorReplacement) {
    uint32_t color = Color::Red().GetRGBA();
    symbolizer->SetColorReplacement(color);
    
    EXPECT_TRUE(symbolizer->HasColorReplacement());
    EXPECT_EQ(symbolizer->GetColorReplacement(), color);
}

TEST_F(IconSymbolizerTest, ClearColorReplacement) {
    symbolizer->SetColorReplacement(Color::Blue().GetRGBA());
    EXPECT_TRUE(symbolizer->HasColorReplacement());
    
    symbolizer->ClearColorReplacement();
    EXPECT_FALSE(symbolizer->HasColorReplacement());
}

TEST_F(IconSymbolizerTest, HasColorReplacementFalse) {
    EXPECT_FALSE(symbolizer->HasColorReplacement());
}

TEST_F(IconSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(IconSymbolizerTest, Clone) {
    symbolizer->SetIconPath("original.png");
    symbolizer->SetSize(32.0, 32.0);
    symbolizer->SetOpacity(0.7);
    symbolizer->SetRotation(45.0);
    symbolizer->SetAnchorPoint(0.5, 0.5);
    symbolizer->SetDisplacement(5.0, 10.0);
    symbolizer->SetAllowOverlap(true);
    symbolizer->SetColorReplacement(Color::Red().GetRGBA());
    
    SymbolizerPtr cloned = symbolizer->Clone();
    
    IconSymbolizer* iconClone = dynamic_cast<IconSymbolizer*>(cloned.get());
    ASSERT_NE(iconClone, nullptr);
    
    EXPECT_EQ(iconClone->GetIconPath(), "original.png");
    EXPECT_DOUBLE_EQ(iconClone->GetWidth(), 32.0);
    EXPECT_DOUBLE_EQ(iconClone->GetHeight(), 32.0);
    EXPECT_DOUBLE_EQ(iconClone->GetOpacity(), 0.7);
    EXPECT_DOUBLE_EQ(iconClone->GetRotation(), 45.0);
    EXPECT_TRUE(iconClone->GetAllowOverlap());
    EXPECT_TRUE(iconClone->HasColorReplacement());
}
