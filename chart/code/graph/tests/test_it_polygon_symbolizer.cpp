#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
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
using ogc::Envelope;

class IntegrationPolygonSymbolizerTest : public ::testing::Test {
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

TEST_F(IntegrationPolygonSymbolizerTest, SymbolizerCreation) {
    auto symbolizer = PolygonSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPolygon);
}

TEST_F(IntegrationPolygonSymbolizerTest, SymbolizerWithFillColor) {
    auto symbolizer = PolygonSymbolizer::Create(0xFF0000);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetFillColor(), 0xFF0000);
}

TEST_F(IntegrationPolygonSymbolizerTest, SymbolizerWithFillAndStroke) {
    auto symbolizer = PolygonSymbolizer::Create(0xFF0000, 0x000000, 2.0);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetFillColor(), 0xFF0000);
    EXPECT_EQ(symbolizer->GetStrokeColor(), 0x000000);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeWidth(), 2.0);
}

TEST_F(IntegrationPolygonSymbolizerTest, SetFillProperties) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    symbolizer->SetFillColor(0x00FF00);
    EXPECT_EQ(symbolizer->GetFillColor(), 0x00FF00);
    
    symbolizer->SetFillOpacity(0.7);
    EXPECT_DOUBLE_EQ(symbolizer->GetFillOpacity(), 0.7);
    
    symbolizer->SetFillPattern(FillPattern::kDiagonal);
    EXPECT_EQ(symbolizer->GetFillPattern(), FillPattern::kDiagonal);
}

TEST_F(IntegrationPolygonSymbolizerTest, SetStrokeProperties) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    symbolizer->SetStrokeColor(0x0000FF);
    EXPECT_EQ(symbolizer->GetStrokeColor(), 0x0000FF);
    
    symbolizer->SetStrokeWidth(3.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeWidth(), 3.0);
    
    symbolizer->SetStrokeOpacity(0.8);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeOpacity(), 0.8);
}

TEST_F(IntegrationPolygonSymbolizerTest, SetDisplacement) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    symbolizer->SetDisplacement(5.0, 10.0);
    
    double dx, dy;
    symbolizer->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 5.0);
    EXPECT_DOUBLE_EQ(dy, 10.0);
}

TEST_F(IntegrationPolygonSymbolizerTest, SetPerpendicularOffset) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    symbolizer->SetPerpendicularOffset(15.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetPerpendicularOffset(), 15.0);
}

TEST_F(IntegrationPolygonSymbolizerTest, SetGraphicFill) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    symbolizer->SetGraphicFill(true);
    EXPECT_TRUE(symbolizer->HasGraphicFill());
    
    symbolizer->SetGraphicFillSize(12.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicFillSize(), 12.0);
    
    symbolizer->SetGraphicFillSpacing(25.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetGraphicFillSpacing(), 25.0);
}

TEST_F(IntegrationPolygonSymbolizerTest, CanSymbolizePolygon) {
    auto symbolizer = PolygonSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPolygon));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPolygon));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
}

TEST_F(IntegrationPolygonSymbolizerTest, Clone) {
    auto original = PolygonSymbolizer::Create();
    original->SetFillColor(0xFF0000);
    original->SetStrokeColor(0x000000);
    original->SetStrokeWidth(2.0);
    original->SetFillPattern(FillPattern::kDiagonal);
    
    auto cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto clonedPolygon = std::dynamic_pointer_cast<PolygonSymbolizer>(cloned);
    ASSERT_NE(clonedPolygon, nullptr);
    EXPECT_EQ(clonedPolygon->GetFillColor(), 0xFF0000);
    EXPECT_EQ(clonedPolygon->GetStrokeColor(), 0x000000);
    EXPECT_DOUBLE_EQ(clonedPolygon->GetStrokeWidth(), 2.0);
    EXPECT_EQ(clonedPolygon->GetFillPattern(), FillPattern::kDiagonal);
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderWithContext) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = PolygonSymbolizer::Create();
    symbolizer->SetFillColor(Color::Blue().GetRGBA());
    symbolizer->SetStrokeColor(Color::Black().GetRGBA());
    symbolizer->SetStrokeWidth(2.0);
    
    DrawStyle style;
    style.brush = Brush(Color::Blue());
    style.pen = Pen(Color::Black(), 2.0);
    context->SetStyle(style);
    
    double x[] = {50, 200, 200, 50};
    double y[] = {50, 50, 200, 200};
    
    DrawResult result = context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderMultiplePolygons) {
    context->Begin();
    device->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Green());
    style.pen = Pen(Color::Black(), 1.0);
    context->SetStyle(style);
    
    double x1[] = {10, 100, 100, 10};
    double y1[] = {10, 10, 100, 100};
    DrawResult result = context->DrawPolygon(x1, y1, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    double x2[] = {150, 240, 240, 150};
    double y2[] = {150, 150, 240, 240};
    result = context->DrawPolygon(x2, y2, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderTriangle) {
    context->Begin();
    device->Clear(Color::White());
    
    DrawStyle style;
    style.brush = Brush(Color::Red());
    style.pen = Pen(Color::Black(), 2.0);
    context->SetStyle(style);
    
    double x[] = {128, 50, 206};
    double y[] = {30, 220, 220};
    
    DrawResult result = context->DrawPolygon(x, y, 3);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderWithDifferentFillPatterns) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = PolygonSymbolizer::Create();
    symbolizer->SetFillPattern(FillPattern::kSolid);
    symbolizer->SetFillColor(Color::Yellow().GetRGBA());
    
    DrawStyle style;
    style.brush = Brush(Color::Yellow());
    context->SetStyle(style);
    
    double x[] = {20, 230, 230, 20};
    double y[] = {20, 20, 230, 230};
    
    DrawResult result = context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderSeaArea) {
    context->Begin();
    device->Clear(Color::White());
    
    auto symbolizer = PolygonSymbolizer::Create();
    symbolizer->SetFillColor(Color::Cyan().GetRGBA());
    symbolizer->SetFillOpacity(0.5);
    symbolizer->SetStrokeColor(Color::Blue().GetRGBA());
    symbolizer->SetStrokeWidth(1.0);
    
    DrawStyle style;
    style.brush = Brush(Color::Cyan());
    style.pen = Pen(Color::Blue(), 1.0);
    context->SetStyle(style);
    
    double x[] = {50, 450, 450, 300, 200, 50};
    double y[] = {50, 50, 400, 480, 480, 400};
    
    DrawResult result = context->DrawPolygon(x, y, 6);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, RenderLandArea) {
    context->Begin();
    device->Clear(Color::Cyan());
    
    auto symbolizer = PolygonSymbolizer::Create();
    symbolizer->SetFillColor(Color::LightGray().GetRGBA());
    symbolizer->SetStrokeColor(Color::Black().GetRGBA());
    symbolizer->SetStrokeWidth(2.0);
    
    DrawStyle style;
    style.brush = Brush(Color::LightGray());
    style.pen = Pen(Color::Black(), 2.0);
    context->SetStyle(style);
    
    double x[] = {100, 400, 400, 100};
    double y[] = {100, 100, 400, 400};
    
    DrawResult result = context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationPolygonSymbolizerTest, GetName) {
    auto symbolizer = PolygonSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetName(), "PolygonSymbolizer");
}

TEST_F(IntegrationPolygonSymbolizerTest, GetType) {
    auto symbolizer = PolygonSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPolygon);
}
