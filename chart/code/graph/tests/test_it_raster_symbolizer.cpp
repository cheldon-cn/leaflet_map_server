#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/raster_symbolizer.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include "ogc/common.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc;
using ogc::Envelope;

class IntegrationRasterSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        
        DrawResult result = m_device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
        
        m_symbolizer = RasterSymbolizer::Create();
        ASSERT_NE(m_symbolizer, nullptr);
    }
    
    void TearDown() override {
        m_symbolizer.reset();
        m_context.reset();
        if (m_device) {
            m_device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
    std::shared_ptr<RasterSymbolizer> m_symbolizer;
};

TEST_F(IntegrationRasterSymbolizerTest, SymbolizerCreation) {
    EXPECT_NE(m_symbolizer, nullptr);
    EXPECT_EQ(m_symbolizer->GetType(), SymbolizerType::kRaster);
    EXPECT_EQ(m_symbolizer->GetName(), "RasterSymbolizer");
}

TEST_F(IntegrationRasterSymbolizerTest, SetOpacity) {
    m_symbolizer->SetOpacity(0.75);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetOpacity(), 0.75);
}

TEST_F(IntegrationRasterSymbolizerTest, SetChannelSelection) {
    m_symbolizer->SetChannelSelection(RasterChannelSelection::kRGB);
    EXPECT_EQ(m_symbolizer->GetChannelSelection(), RasterChannelSelection::kRGB);
    
    m_symbolizer->SetChannelSelection(RasterChannelSelection::kGrayscale);
    EXPECT_EQ(m_symbolizer->GetChannelSelection(), RasterChannelSelection::kGrayscale);
    
    m_symbolizer->SetChannelSelection(RasterChannelSelection::kPseudoColor);
    EXPECT_EQ(m_symbolizer->GetChannelSelection(), RasterChannelSelection::kPseudoColor);
}

TEST_F(IntegrationRasterSymbolizerTest, SetRedChannel) {
    m_symbolizer->SetRedChannel(0);
    EXPECT_EQ(m_symbolizer->GetRedChannel(), 0);
}

TEST_F(IntegrationRasterSymbolizerTest, SetGreenChannel) {
    m_symbolizer->SetGreenChannel(1);
    EXPECT_EQ(m_symbolizer->GetGreenChannel(), 1);
}

TEST_F(IntegrationRasterSymbolizerTest, SetBlueChannel) {
    m_symbolizer->SetBlueChannel(2);
    EXPECT_EQ(m_symbolizer->GetBlueChannel(), 2);
}

TEST_F(IntegrationRasterSymbolizerTest, SetGrayChannel) {
    m_symbolizer->SetGrayChannel(0);
    EXPECT_EQ(m_symbolizer->GetGrayChannel(), 0);
}

TEST_F(IntegrationRasterSymbolizerTest, SetContrastEnhancement) {
    m_symbolizer->SetContrastEnhancement(true);
    EXPECT_TRUE(m_symbolizer->HasContrastEnhancement());
    
    m_symbolizer->SetContrastEnhancement(false);
    EXPECT_FALSE(m_symbolizer->HasContrastEnhancement());
}

TEST_F(IntegrationRasterSymbolizerTest, SetContrastValue) {
    m_symbolizer->SetContrastValue(1.5);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetContrastValue(), 1.5);
}

TEST_F(IntegrationRasterSymbolizerTest, SetBrightnessValue) {
    m_symbolizer->SetBrightnessValue(0.2);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetBrightnessValue(), 0.2);
}

TEST_F(IntegrationRasterSymbolizerTest, SetGammaValue) {
    m_symbolizer->SetGammaValue(2.2);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetGammaValue(), 2.2);
}

TEST_F(IntegrationRasterSymbolizerTest, AddColorMapEntry) {
    ColorMapEntry entry1(0.0, Color::Blue().GetRGBA(), 1.0, "Low");
    ColorMapEntry entry2(128.0, Color::Green().GetRGBA(), 1.0, "Medium");
    ColorMapEntry entry3(255.0, Color::Red().GetRGBA(), 1.0, "High");
    
    m_symbolizer->AddColorMapEntry(entry1);
    m_symbolizer->AddColorMapEntry(entry2);
    m_symbolizer->AddColorMapEntry(entry3);
    
    std::vector<ColorMapEntry> colorMap = m_symbolizer->GetColorMap();
    EXPECT_EQ(colorMap.size(), 3);
    EXPECT_DOUBLE_EQ(colorMap[0].value, 0.0);
    EXPECT_DOUBLE_EQ(colorMap[1].value, 128.0);
    EXPECT_DOUBLE_EQ(colorMap[2].value, 255.0);
}

TEST_F(IntegrationRasterSymbolizerTest, ClearColorMap) {
    ColorMapEntry entry(0.0, Color::Blue().GetRGBA());
    m_symbolizer->AddColorMapEntry(entry);
    
    EXPECT_EQ(m_symbolizer->GetColorMap().size(), 1);
    
    m_symbolizer->ClearColorMap();
    EXPECT_EQ(m_symbolizer->GetColorMap().size(), 0);
}

TEST_F(IntegrationRasterSymbolizerTest, SetColorMapType) {
    m_symbolizer->SetColorMapType(RasterChannelSelection::kPseudoColor);
    EXPECT_EQ(m_symbolizer->GetColorMapType(), RasterChannelSelection::kPseudoColor);
}

TEST_F(IntegrationRasterSymbolizerTest, SetMinValue) {
    m_symbolizer->SetMinValue(0.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetMinValue(), 0.0);
}

TEST_F(IntegrationRasterSymbolizerTest, SetMaxValue) {
    m_symbolizer->SetMaxValue(255.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetMaxValue(), 255.0);
}

TEST_F(IntegrationRasterSymbolizerTest, SetResampling) {
    m_symbolizer->SetResampling("bilinear");
    EXPECT_EQ(m_symbolizer->GetResampling(), "bilinear");
    
    m_symbolizer->SetResampling("nearest");
    EXPECT_EQ(m_symbolizer->GetResampling(), "nearest");
}

TEST_F(IntegrationRasterSymbolizerTest, SetOverlapBehavior) {
    m_symbolizer->SetOverlapBehavior("LATEST_ON_TOP");
    EXPECT_EQ(m_symbolizer->GetOverlapBehavior(), "LATEST_ON_TOP");
}

TEST_F(IntegrationRasterSymbolizerTest, CanSymbolize) {
    EXPECT_FALSE(m_symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_FALSE(m_symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(m_symbolizer->CanSymbolize(GeomType::kLineString));
}

TEST_F(IntegrationRasterSymbolizerTest, Clone) {
    m_symbolizer->SetOpacity(0.8);
    m_symbolizer->SetContrastEnhancement(true);
    m_symbolizer->SetContrastValue(1.5);
    m_symbolizer->SetBrightnessValue(0.2);
    m_symbolizer->SetGammaValue(2.2);
    m_symbolizer->SetMinValue(0.0);
    m_symbolizer->SetMaxValue(255.0);
    
    ColorMapEntry entry(128.0, Color::Green().GetRGBA());
    m_symbolizer->AddColorMapEntry(entry);
    
    SymbolizerPtr cloned = m_symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    RasterSymbolizer* clonedRaster = dynamic_cast<RasterSymbolizer*>(cloned.get());
    ASSERT_NE(clonedRaster, nullptr);
    
    EXPECT_DOUBLE_EQ(clonedRaster->GetOpacity(), 0.8);
    EXPECT_TRUE(clonedRaster->HasContrastEnhancement());
    EXPECT_DOUBLE_EQ(clonedRaster->GetContrastValue(), 1.5);
    EXPECT_DOUBLE_EQ(clonedRaster->GetBrightnessValue(), 0.2);
    EXPECT_DOUBLE_EQ(clonedRaster->GetGammaValue(), 2.2);
    EXPECT_DOUBLE_EQ(clonedRaster->GetMinValue(), 0.0);
    EXPECT_DOUBLE_EQ(clonedRaster->GetMaxValue(), 255.0);
    EXPECT_EQ(clonedRaster->GetColorMap().size(), 1);
}

TEST_F(IntegrationRasterSymbolizerTest, ColorMapEntryDefaultValues) {
    ColorMapEntry entry;
    EXPECT_DOUBLE_EQ(entry.value, 0.0);
    EXPECT_EQ(entry.color, 0xFF000000);
    EXPECT_DOUBLE_EQ(entry.opacity, 1.0);
    EXPECT_TRUE(entry.label.empty());
}

TEST_F(IntegrationRasterSymbolizerTest, ColorMapEntryCustomValues) {
    ColorMapEntry entry(100.0, Color::Red().GetRGBA(), 0.5, "Test");
    EXPECT_DOUBLE_EQ(entry.value, 100.0);
    EXPECT_EQ(entry.color, Color::Red().GetRGBA());
    EXPECT_DOUBLE_EQ(entry.opacity, 0.5);
    EXPECT_EQ(entry.label, "Test");
}

TEST_F(IntegrationRasterSymbolizerTest, RenderWithGrayscale) {
    m_symbolizer->SetChannelSelection(RasterChannelSelection::kGrayscale);
    m_symbolizer->SetGrayChannel(0);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Gray());
    m_context->SetStyle(style);
    
    double x[] = {50, 200, 200, 50};
    double y[] = {50, 50, 200, 200};
    
    DrawResult result = m_context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationRasterSymbolizerTest, RenderWithColorMap) {
    ColorMapEntry entry1(0.0, Color::Blue().GetRGBA());
    ColorMapEntry entry2(128.0, Color::Green().GetRGBA());
    ColorMapEntry entry3(255.0, Color::Red().GetRGBA());
    
    m_symbolizer->AddColorMapEntry(entry1);
    m_symbolizer->AddColorMapEntry(entry2);
    m_symbolizer->AddColorMapEntry(entry3);
    m_symbolizer->SetColorMapType(RasterChannelSelection::kPseudoColor);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Yellow());
    m_context->SetStyle(style);
    
    double x[] = {50, 200, 200, 50};
    double y[] = {50, 50, 200, 200};
    
    DrawResult result = m_context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationRasterSymbolizerTest, RenderWithContrastEnhancement) {
    m_symbolizer->SetContrastEnhancement(true);
    m_symbolizer->SetContrastValue(1.5);
    m_symbolizer->SetBrightnessValue(0.1);
    m_symbolizer->SetGammaValue(2.2);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Gray());
    m_context->SetStyle(style);
    
    double x[] = {50, 200, 200, 50};
    double y[] = {50, 50, 200, 200};
    
    DrawResult result = m_context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationRasterSymbolizerTest, RenderWithOpacity) {
    m_symbolizer->SetOpacity(0.5);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Blue());
    m_context->SetStyle(style);
    
    double x[] = {50, 200, 200, 50};
    double y[] = {50, 50, 200, 200};
    
    DrawResult result = m_context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationRasterSymbolizerTest, RenderMultiplePolygons) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    m_context->SetStyle(style);
    
    for (int i = 0; i < 3; ++i) {
        double offset = i * 50;
        double x[] = {offset + 10, offset + 60, offset + 60, offset + 10};
        double y[] = {10, 10, 60, 60};
        
        style.brush = Brush(Color::Red());
        m_context->SetStyle(style);
        DrawResult result = m_context->DrawPolygon(x, y, 4);
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    m_context->End();
}

TEST_F(IntegrationRasterSymbolizerTest, ChannelSelectionRGB) {
    m_symbolizer->SetChannelSelection(RasterChannelSelection::kRGB);
    m_symbolizer->SetRedChannel(0);
    m_symbolizer->SetGreenChannel(1);
    m_symbolizer->SetBlueChannel(2);
    
    EXPECT_EQ(m_symbolizer->GetChannelSelection(), RasterChannelSelection::kRGB);
    EXPECT_EQ(m_symbolizer->GetRedChannel(), 0);
    EXPECT_EQ(m_symbolizer->GetGreenChannel(), 1);
    EXPECT_EQ(m_symbolizer->GetBlueChannel(), 2);
}

TEST_F(IntegrationRasterSymbolizerTest, ValueRange) {
    m_symbolizer->SetMinValue(10.0);
    m_symbolizer->SetMaxValue(200.0);
    
    EXPECT_DOUBLE_EQ(m_symbolizer->GetMinValue(), 10.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetMaxValue(), 200.0);
}
