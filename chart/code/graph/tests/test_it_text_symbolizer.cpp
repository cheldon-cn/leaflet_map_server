#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/text_symbolizer.h>
#include <ogc/draw/font.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/transform_matrix.h>
#include "ogc/geom/common.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc;
using namespace ogc::draw;
using ogc::Envelope;

class IntegrationTextSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(device, nullptr);
        device->Initialize();
        
        context = DrawContext::Create(device.get());
        ASSERT_NE(context, nullptr);
    }
    
    void TearDown() override {
        context.reset();
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
    std::unique_ptr<DrawContext> context;
};

TEST_F(IntegrationTextSymbolizerTest, SymbolizerCreation) {
    auto symbolizer = TextSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kText);
}

TEST_F(IntegrationTextSymbolizerTest, SymbolizerWithParams) {
    Font font("Arial", 12);
    auto symbolizer = TextSymbolizer::Create("Test", font, 0x000000);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetLabel(), "Test");
    EXPECT_EQ(symbolizer->GetColor(), 0x000000);
}

TEST_F(IntegrationTextSymbolizerTest, SetLabel) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetLabel("Hello World");
    EXPECT_EQ(symbolizer->GetLabel(), "Hello World");
}

TEST_F(IntegrationTextSymbolizerTest, SetLabelProperty) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetLabelProperty("name");
    EXPECT_EQ(symbolizer->GetLabelProperty(), "name");
}

TEST_F(IntegrationTextSymbolizerTest, SetFont) {
    auto symbolizer = TextSymbolizer::Create();
    
    Font font("Times New Roman", 14, FontStyle::kBoldItalic);
    symbolizer->SetFont(font);
    
    Font retrieved = symbolizer->GetFont();
    EXPECT_EQ(retrieved.GetFamily(), "Times New Roman");
    EXPECT_DOUBLE_EQ(retrieved.GetSize(), 14);
    EXPECT_TRUE(retrieved.IsBold());
    EXPECT_TRUE(retrieved.IsItalic());
}

TEST_F(IntegrationTextSymbolizerTest, SetColor) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetColor(0xFF0000);
    EXPECT_EQ(symbolizer->GetColor(), 0xFF0000);
}

TEST_F(IntegrationTextSymbolizerTest, SetOpacity) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetOpacity(0.8);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.8);
}

TEST_F(IntegrationTextSymbolizerTest, SetHorizontalAlignment) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kLeft);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kLeft);
    
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kRight);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kRight);
    
    symbolizer->SetHorizontalAlignment(TextHorizontalAlignment::kCenter);
    EXPECT_EQ(symbolizer->GetHorizontalAlignment(), TextHorizontalAlignment::kCenter);
}

TEST_F(IntegrationTextSymbolizerTest, SetVerticalAlignment) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kTop);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kTop);
    
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kBottom);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kBottom);
    
    symbolizer->SetVerticalAlignment(TextVerticalAlignment::kMiddle);
    EXPECT_EQ(symbolizer->GetVerticalAlignment(), TextVerticalAlignment::kMiddle);
}

TEST_F(IntegrationTextSymbolizerTest, SetPlacement) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetPlacement(TextPlacement::kPoint);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kPoint);
    
    symbolizer->SetPlacement(TextPlacement::kLine);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kLine);
    
    symbolizer->SetPlacement(TextPlacement::kInterior);
    EXPECT_EQ(symbolizer->GetPlacement(), TextPlacement::kInterior);
}

TEST_F(IntegrationTextSymbolizerTest, SetOffset) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetOffset(5.0, 10.0);
    
    double dx, dy;
    symbolizer->GetOffset(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 5.0);
    EXPECT_DOUBLE_EQ(dy, 10.0);
}

TEST_F(IntegrationTextSymbolizerTest, SetRotation) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), 45.0);
}

TEST_F(IntegrationTextSymbolizerTest, SetFollowLine) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetFollowLine(true);
    EXPECT_TRUE(symbolizer->GetFollowLine());
    
    symbolizer->SetFollowLine(false);
    EXPECT_FALSE(symbolizer->GetFollowLine());
}

TEST_F(IntegrationTextSymbolizerTest, SetHaloProperties) {
    auto symbolizer = TextSymbolizer::Create();
    
    symbolizer->SetHaloColor(0xFFFFFF);
    EXPECT_EQ(symbolizer->GetHaloColor(), 0xFFFFFF);
    
    symbolizer->SetHaloRadius(2.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetHaloRadius(), 2.0);
    
    symbolizer->SetHaloOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetHaloOpacity(), 0.5);
}

TEST_F(IntegrationTextSymbolizerTest, CanSymbolize) {
    auto symbolizer = TextSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(IntegrationTextSymbolizerTest, Clone) {
    auto original = TextSymbolizer::Create();
    original->SetLabel("Test Label");
    original->SetColor(0xFF0000);
    Font font("Arial", 16, FontStyle::kBold);
    original->SetFont(font);
    original->SetRotation(30.0);
    
    auto cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto clonedText = std::dynamic_pointer_cast<TextSymbolizer>(cloned);
    ASSERT_NE(clonedText, nullptr);
    EXPECT_EQ(clonedText->GetLabel(), "Test Label");
    EXPECT_EQ(clonedText->GetColor(), 0xFF0000);
    EXPECT_DOUBLE_EQ(clonedText->GetRotation(), 30.0);
}

TEST_F(IntegrationTextSymbolizerTest, RenderWithContext) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = TextSymbolizer::Create();
    symbolizer->SetLabel("Test Label");
    symbolizer->SetColor(Color::Black().GetRGBA());
    
    Font font("Arial", 12);
    
    DrawResult result = context->DrawText(128, 128, "Test Label", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderMultipleLabels) {
    context->Begin();
    device->Clear(Color::White());
    
    Font font("Arial", 14);
    
    DrawResult result = context->DrawText(100, 100, "Location A", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = context->DrawText(250, 250, "Location B", font, Color::Red());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = context->DrawText(400, 400, "Location C", font, Color::Blue());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderPlaceName) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = TextSymbolizer::Create();
    symbolizer->SetLabel("Shanghai Port");
    symbolizer->SetColor(Color::Black().GetRGBA());
    Font font("Arial", 18, FontStyle::kBold);
    symbolizer->SetFont(font);
    
    DrawResult result = context->DrawText(256, 256, "Shanghai Port", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderDepthLabel) {
    context->Begin();
    device->Clear(Color::Cyan());
    
    auto symbolizer = TextSymbolizer::Create();
    Font font("Arial", 10);
    symbolizer->SetFont(font);
    symbolizer->SetColor(Color::Blue().GetRGBA());
    
    double depths[] = {10.5, 15.2, 8.3, 20.1};
    double px[] = {100, 200, 300, 400};
    double py[] = {100, 200, 300, 400};
    
    for (int i = 0; i < 4; i++) {
        std::string depthText = std::to_string(depths[i]).substr(0, 4) + "m";
        DrawResult result = context->DrawText(px[i], py[i], depthText, font, Color::Blue());
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderWithHalo) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = TextSymbolizer::Create();
    symbolizer->SetLabel("Halo Text");
    symbolizer->SetColor(Color::Black().GetRGBA());
    symbolizer->SetHaloColor(Color::White().GetRGBA());
    symbolizer->SetHaloRadius(2.0);
    
    Font font("Arial", 16);
    
    DrawResult result = context->DrawText(128, 128, "Halo Text", font, Color::Black());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderWithRotation) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = TextSymbolizer::Create();
    symbolizer->SetRotation(45.0);
    symbolizer->SetColor(Color::Red().GetRGBA());
    
    Font font("Arial", 14);
    
    DrawResult result = context->DrawText(128, 128, "Rotated", font, Color::Red());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, RenderChannelName) {
    context->Begin();
    device->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 2.0);
    context->SetStyle(style);
    
    double x[] = {50, 200, 350, 480};
    double y[] = {256, 256, 256, 256};
    context->DrawLineString(x, y, 4);
    
    auto symbolizer = TextSymbolizer::Create();
    symbolizer->SetPlacement(TextPlacement::kLine);
    symbolizer->SetFollowLine(true);
    Font font("Arial", 12, FontStyle::kItalic);
    symbolizer->SetFont(font);
    symbolizer->SetColor(Color::Blue().GetRGBA());
    
    DrawResult result = context->DrawText(200, 240, "Main Channel", font, Color::Blue());
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationTextSymbolizerTest, GetName) {
    auto symbolizer = TextSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetName(), "TextSymbolizer");
}

TEST_F(IntegrationTextSymbolizerTest, GetType) {
    auto symbolizer = TextSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kText);
}
