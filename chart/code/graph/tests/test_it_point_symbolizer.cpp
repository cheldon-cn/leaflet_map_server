#include <gtest/gtest.h>
#include "ogc/draw/point_symbolizer.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/transform_matrix.h>
#include "ogc/common.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;
using ogc::Envelope;

class IntegrationPointSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
        
        context = DrawContext::Create(device);
        ASSERT_NE(context, nullptr);
        context->Initialize();
    }
    
    void TearDown() override {
        context.reset();
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
    std::shared_ptr<DrawContext> context;
};

TEST_F(IntegrationPointSymbolizerTest, SymbolizerCreation) {
    auto symbolizer = PointSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPoint);
}

TEST_F(IntegrationPointSymbolizerTest, SymbolizerWithParams) {
    auto symbolizer = PointSymbolizer::Create(10.0, 0xFF0000);
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_DOUBLE_EQ(symbolizer->GetSize(), 10.0);
    EXPECT_EQ(symbolizer->GetColor(), 0xFF0000);
}

TEST_F(IntegrationPointSymbolizerTest, SetSymbolType) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetSymbolType(PointSymbolType::kSquare);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kSquare);
    
    symbolizer->SetSymbolType(PointSymbolType::kTriangle);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kTriangle);
    
    symbolizer->SetSymbolType(PointSymbolType::kStar);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kStar);
    
    symbolizer->SetSymbolType(PointSymbolType::kCross);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kCross);
    
    symbolizer->SetSymbolType(PointSymbolType::kDiamond);
    EXPECT_EQ(symbolizer->GetSymbolType(), PointSymbolType::kDiamond);
}

TEST_F(IntegrationPointSymbolizerTest, SetSize) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetSize(15.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetSize(), 15.0);
}

TEST_F(IntegrationPointSymbolizerTest, SetColor) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetColor(0x00FF00);
    EXPECT_EQ(symbolizer->GetColor(), 0x00FF00);
}

TEST_F(IntegrationPointSymbolizerTest, SetStrokeProperties) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetStrokeColor(0x0000FF);
    EXPECT_EQ(symbolizer->GetStrokeColor(), 0x0000FF);
    
    symbolizer->SetStrokeWidth(2.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetStrokeWidth(), 2.0);
}

TEST_F(IntegrationPointSymbolizerTest, SetRotation) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetRotation(), 45.0);
}

TEST_F(IntegrationPointSymbolizerTest, SetAnchorPoint) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetAnchorPoint(0.5, 0.5);
    
    double x, y;
    symbolizer->GetAnchorPoint(x, y);
    
    EXPECT_DOUBLE_EQ(x, 0.5);
    EXPECT_DOUBLE_EQ(y, 0.5);
}

TEST_F(IntegrationPointSymbolizerTest, SetDisplacement) {
    auto symbolizer = PointSymbolizer::Create();
    
    symbolizer->SetDisplacement(10.0, 20.0);
    
    double dx, dy;
    symbolizer->GetDisplacement(dx, dy);
    
    EXPECT_DOUBLE_EQ(dx, 10.0);
    EXPECT_DOUBLE_EQ(dy, 20.0);
}

TEST_F(IntegrationPointSymbolizerTest, CanSymbolizePoint) {
    auto symbolizer = PointSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kMultiPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(IntegrationPointSymbolizerTest, Clone) {
    auto original = PointSymbolizer::Create();
    original->SetSize(12.0);
    original->SetColor(0xFF0000);
    original->SetSymbolType(PointSymbolType::kStar);
    original->SetRotation(30.0);
    
    auto cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto clonedPoint = std::dynamic_pointer_cast<PointSymbolizer>(cloned);
    ASSERT_NE(clonedPoint, nullptr);
    EXPECT_DOUBLE_EQ(clonedPoint->GetSize(), 12.0);
    EXPECT_EQ(clonedPoint->GetColor(), 0xFF0000);
    EXPECT_EQ(clonedPoint->GetSymbolType(), PointSymbolType::kStar);
    EXPECT_DOUBLE_EQ(clonedPoint->GetRotation(), 30.0);
}

TEST_F(IntegrationPointSymbolizerTest, RenderWithContext) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSize(10.0);
    symbolizer->SetColor(Color::Red().GetRGBA());
    
    DrawStyle style;
    style.brush.color = Color::Red().GetRGBA();
    style.brush.visible = true;
    context->SetStyle(style);
    
    DrawResult result = context->DrawCircle(128, 128, 10);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, RenderMultiplePoints) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.brush.color = Color::Blue().GetRGBA();
    style.brush.visible = true;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 1.0;
    style.pen.visible = true;
    context->SetStyle(style);
    
    double px[] = {50, 128, 200, 50, 200};
    double py[] = {50, 50, 50, 200, 200};
    
    for (int i = 0; i < 5; i++) {
        DrawResult result = context->DrawCircle(px[i], py[i], 8);
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, RenderLighthouse) {
    DrawParams params;
    params.pixel_width = 512;
    params.pixel_height = 512;
    params.extent = Envelope(0, 0, 512, 512);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSymbolType(PointSymbolType::kStar);
    symbolizer->SetSize(20.0);
    symbolizer->SetColor(Color::Yellow().GetRGBA());
    symbolizer->SetStrokeColor(Color::Black().GetRGBA());
    symbolizer->SetStrokeWidth(2.0);
    
    DrawStyle style;
    style.brush.color = Color::Yellow().GetRGBA();
    style.brush.visible = true;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 2.0;
    style.pen.visible = true;
    context->SetStyle(style);
    
    DrawResult result = context->DrawCircle(256, 256, 20);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, RenderBuoy) {
    DrawParams params;
    params.pixel_width = 512;
    params.pixel_height = 512;
    params.extent = Envelope(0, 0, 512, 512);
    
    context->BeginDraw(params);
    context->Clear(Color::Cyan());
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSymbolType(PointSymbolType::kCircle);
    symbolizer->SetSize(15.0);
    symbolizer->SetColor(Color::Red().GetRGBA());
    symbolizer->SetStrokeColor(Color::White().GetRGBA());
    symbolizer->SetStrokeWidth(1.0);
    
    DrawStyle style;
    style.brush.color = Color::Red().GetRGBA();
    style.brush.visible = true;
    style.pen.color = Color::White().GetRGBA();
    style.pen.width = 1.0;
    style.pen.visible = true;
    context->SetStyle(style);
    
    double px[] = {100, 200, 300, 400};
    double py[] = {100, 200, 300, 400};
    
    for (int i = 0; i < 4; i++) {
        DrawResult result = context->DrawCircle(px[i], py[i], 15);
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, RenderBeacon) {
    DrawParams params;
    params.pixel_width = 512;
    params.pixel_height = 512;
    params.extent = Envelope(0, 0, 512, 512);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetSymbolType(PointSymbolType::kTriangle);
    symbolizer->SetSize(18.0);
    symbolizer->SetColor(Color::Green().GetRGBA());
    symbolizer->SetStrokeColor(Color::Black().GetRGBA());
    symbolizer->SetStrokeWidth(1.0);
    
    DrawStyle style;
    style.brush.color = Color::Green().GetRGBA();
    style.brush.visible = true;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 1.0;
    style.pen.visible = true;
    context->SetStyle(style);
    
    DrawResult result = context->DrawCircle(256, 256, 18);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, RenderWithRotation) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = PointSymbolizer::Create();
    symbolizer->SetRotation(45.0);
    symbolizer->SetSize(15.0);
    symbolizer->SetColor(Color::Magenta().GetRGBA());
    
    DrawStyle style;
    style.brush.color = Color::Magenta().GetRGBA();
    style.brush.visible = true;
    context->SetStyle(style);
    
    DrawResult result = context->DrawCircle(128, 128, 15);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationPointSymbolizerTest, GetName) {
    auto symbolizer = PointSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetName(), "PointSymbolizer");
}

TEST_F(IntegrationPointSymbolizerTest, GetType) {
    auto symbolizer = PointSymbolizer::Create();
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kPoint);
}

