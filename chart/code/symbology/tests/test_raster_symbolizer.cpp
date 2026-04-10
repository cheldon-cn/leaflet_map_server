#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/raster_symbolizer.h>
#include <ogc/draw/color.h>
#include "ogc/geom/common.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc::draw;
using namespace ogc;

class RasterSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        symbolizer = RasterSymbolizer::Create();
        ASSERT_NE(symbolizer, nullptr);
    }
    
    void TearDown() override {
        symbolizer.reset();
    }
    
    RasterSymbolizerPtr symbolizer;
};

TEST_F(RasterSymbolizerTest, DefaultConstructor) {
    RasterSymbolizerPtr defaultSymbolizer = RasterSymbolizer::Create();
    EXPECT_NE(defaultSymbolizer, nullptr);
}

TEST_F(RasterSymbolizerTest, GetType) {
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kRaster);
}

TEST_F(RasterSymbolizerTest, GetName) {
    EXPECT_EQ(symbolizer->GetName(), "RasterSymbolizer");
}

TEST_F(RasterSymbolizerTest, SetOpacity) {
    symbolizer->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.5);
    
    symbolizer->SetOpacity(0.8);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.8);
}

TEST_F(RasterSymbolizerTest, SetChannelSelection) {
    symbolizer->SetChannelSelection(RasterChannelSelection::kRGB);
    EXPECT_EQ(symbolizer->GetChannelSelection(), RasterChannelSelection::kRGB);
    
    symbolizer->SetChannelSelection(RasterChannelSelection::kGrayscale);
    EXPECT_EQ(symbolizer->GetChannelSelection(), RasterChannelSelection::kGrayscale);
    
    symbolizer->SetChannelSelection(RasterChannelSelection::kPseudoColor);
    EXPECT_EQ(symbolizer->GetChannelSelection(), RasterChannelSelection::kPseudoColor);
}

TEST_F(RasterSymbolizerTest, SetRedChannel) {
    symbolizer->SetRedChannel(1);
    EXPECT_EQ(symbolizer->GetRedChannel(), 1);
}

TEST_F(RasterSymbolizerTest, SetGreenChannel) {
    symbolizer->SetGreenChannel(2);
    EXPECT_EQ(symbolizer->GetGreenChannel(), 2);
}

TEST_F(RasterSymbolizerTest, SetBlueChannel) {
    symbolizer->SetBlueChannel(3);
    EXPECT_EQ(symbolizer->GetBlueChannel(), 3);
}

TEST_F(RasterSymbolizerTest, SetGrayChannel) {
    symbolizer->SetGrayChannel(1);
    EXPECT_EQ(symbolizer->GetGrayChannel(), 1);
}

TEST_F(RasterSymbolizerTest, SetContrastEnhancement) {
    symbolizer->SetContrastEnhancement(true);
    EXPECT_TRUE(symbolizer->HasContrastEnhancement());
    
    symbolizer->SetContrastEnhancement(false);
    EXPECT_FALSE(symbolizer->HasContrastEnhancement());
}

TEST_F(RasterSymbolizerTest, SetContrastValue) {
    symbolizer->SetContrastValue(1.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetContrastValue(), 1.5);
}

TEST_F(RasterSymbolizerTest, SetBrightnessValue) {
    symbolizer->SetBrightnessValue(0.2);
    EXPECT_DOUBLE_EQ(symbolizer->GetBrightnessValue(), 0.2);
}

TEST_F(RasterSymbolizerTest, SetGammaValue) {
    symbolizer->SetGammaValue(2.2);
    EXPECT_DOUBLE_EQ(symbolizer->GetGammaValue(), 2.2);
}

TEST_F(RasterSymbolizerTest, AddColorMapEntry) {
    ColorMapEntry entry;
    entry.value = 0.0;
    entry.color = Color::Blue().GetRGBA();
    entry.opacity = 1.0;
    entry.label = "Water";
    
    symbolizer->AddColorMapEntry(entry);
    
    std::vector<ColorMapEntry> colorMap = symbolizer->GetColorMap();
    EXPECT_EQ(colorMap.size(), 1);
    EXPECT_DOUBLE_EQ(colorMap[0].value, 0.0);
    EXPECT_EQ(colorMap[0].label, "Water");
}

TEST_F(RasterSymbolizerTest, ClearColorMap) {
    ColorMapEntry entry;
    entry.value = 0.0;
    symbolizer->AddColorMapEntry(entry);
    
    symbolizer->ClearColorMap();
    EXPECT_TRUE(symbolizer->GetColorMap().empty());
}

TEST_F(RasterSymbolizerTest, SetMinValue) {
    symbolizer->SetMinValue(0.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetMinValue(), 0.0);
}

TEST_F(RasterSymbolizerTest, SetMaxValue) {
    symbolizer->SetMaxValue(255.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetMaxValue(), 255.0);
}

TEST_F(RasterSymbolizerTest, SetResampling) {
    symbolizer->SetResampling("nearest");
    EXPECT_EQ(symbolizer->GetResampling(), "nearest");
    
    symbolizer->SetResampling("bilinear");
    EXPECT_EQ(symbolizer->GetResampling(), "bilinear");
}

TEST_F(RasterSymbolizerTest, SetOverlapBehavior) {
    symbolizer->SetOverlapBehavior("LATEST_ON_TOP");
    EXPECT_EQ(symbolizer->GetOverlapBehavior(), "LATEST_ON_TOP");
}

TEST_F(RasterSymbolizerTest, CanSymbolize) {
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
}

TEST_F(RasterSymbolizerTest, Clone) {
    symbolizer->SetOpacity(0.8);
    symbolizer->SetContrastEnhancement(true);
    symbolizer->SetContrastValue(1.5);
    symbolizer->SetBrightnessValue(0.2);
    symbolizer->SetGammaValue(2.2);
    symbolizer->SetResampling("bilinear");
    
    SymbolizerPtr cloned = symbolizer->Clone();
    
    RasterSymbolizer* rasterClone = dynamic_cast<RasterSymbolizer*>(cloned.get());
    ASSERT_NE(rasterClone, nullptr);
    
    EXPECT_DOUBLE_EQ(rasterClone->GetOpacity(), 0.8);
    EXPECT_TRUE(rasterClone->HasContrastEnhancement());
    EXPECT_DOUBLE_EQ(rasterClone->GetContrastValue(), 1.5);
    EXPECT_DOUBLE_EQ(rasterClone->GetBrightnessValue(), 0.2);
    EXPECT_DOUBLE_EQ(rasterClone->GetGammaValue(), 2.2);
    EXPECT_EQ(rasterClone->GetResampling(), "bilinear");
}

TEST_F(RasterSymbolizerTest, ColorMapEntryDefaultValues) {
    ColorMapEntry entry;
    
    EXPECT_DOUBLE_EQ(entry.value, 0.0);
    EXPECT_EQ(entry.color, 0xFF000000);
    EXPECT_DOUBLE_EQ(entry.opacity, 1.0);
    EXPECT_TRUE(entry.label.empty());
}

TEST_F(RasterSymbolizerTest, ColorMapEntryParameterized) {
    ColorMapEntry entry(128.0, Color::Green().GetRGBA(), 0.8, "Vegetation");
    
    EXPECT_DOUBLE_EQ(entry.value, 128.0);
    EXPECT_EQ(entry.color, Color::Green().GetRGBA());
    EXPECT_DOUBLE_EQ(entry.opacity, 0.8);
    EXPECT_EQ(entry.label, "Vegetation");
}

TEST_F(RasterSymbolizerTest, SetColorMapType) {
    symbolizer->SetColorMapType(RasterChannelSelection::kPseudoColor);
    EXPECT_EQ(symbolizer->GetColorMapType(), RasterChannelSelection::kPseudoColor);
}
