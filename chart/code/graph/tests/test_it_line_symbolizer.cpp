#include <gtest/gtest.h>
#include "ogc/draw/line_symbolizer.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/common.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;
using ogc::Envelope;

class IntegrationLineSymbolizerTest : public ::testing::Test {
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

TEST_F(IntegrationLineSymbolizerTest, SymbolizerCreation) {
    auto symbolizer = LineSymbolizer::Create();
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetType(), SymbolizerType::kLine);
}

TEST_F(IntegrationLineSymbolizerTest, SymbolizerWithParams) {
    auto symbolizer = LineSymbolizer::Create(2.0, Color::Red().GetRGBA());
    ASSERT_NE(symbolizer, nullptr);
    EXPECT_EQ(symbolizer->GetWidth(), 2.0);
    EXPECT_EQ(symbolizer->GetColor(), Color::Red().GetRGBA());
}

TEST_F(IntegrationLineSymbolizerTest, SetWidthAndColor) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetWidth(3.0);
    EXPECT_EQ(symbolizer->GetWidth(), 3.0);
    
    symbolizer->SetColor(Color::Blue().GetRGBA());
    EXPECT_EQ(symbolizer->GetColor(), Color::Blue().GetRGBA());
}

TEST_F(IntegrationLineSymbolizerTest, SetOpacity) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetOpacity(), 0.5);
}

TEST_F(IntegrationLineSymbolizerTest, SetCapStyle) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetCapStyle(LineCapStyle::kRound);
    EXPECT_EQ(symbolizer->GetCapStyle(), LineCapStyle::kRound);
    
    symbolizer->SetCapStyle(LineCapStyle::kSquare);
    EXPECT_EQ(symbolizer->GetCapStyle(), LineCapStyle::kSquare);
}

TEST_F(IntegrationLineSymbolizerTest, SetJoinStyle) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetJoinStyle(LineJoinStyle::kRound);
    EXPECT_EQ(symbolizer->GetJoinStyle(), LineJoinStyle::kRound);
    
    symbolizer->SetJoinStyle(LineJoinStyle::kMiter);
    EXPECT_EQ(symbolizer->GetJoinStyle(), LineJoinStyle::kMiter);
}

TEST_F(IntegrationLineSymbolizerTest, SetDashStyle) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetDashStyle(DashStyle::kDash);
    EXPECT_EQ(symbolizer->GetDashStyle(), DashStyle::kDash);
    
    symbolizer->SetDashStyle(DashStyle::kDot);
    EXPECT_EQ(symbolizer->GetDashStyle(), DashStyle::kDot);
}

TEST_F(IntegrationLineSymbolizerTest, SetDashPattern) {
    auto symbolizer = LineSymbolizer::Create();
    
    std::vector<double> pattern = {5.0, 3.0, 1.0, 3.0};
    symbolizer->SetDashPattern(pattern);
    
    auto retrieved = symbolizer->GetDashPattern();
    EXPECT_EQ(retrieved.size(), 4);
    EXPECT_DOUBLE_EQ(retrieved[0], 5.0);
    EXPECT_DOUBLE_EQ(retrieved[1], 3.0);
}

TEST_F(IntegrationLineSymbolizerTest, SetDashOffset) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetDashOffset(2.5);
    EXPECT_DOUBLE_EQ(symbolizer->GetDashOffset(), 2.5);
}

TEST_F(IntegrationLineSymbolizerTest, SetOffset) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetOffset(5.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetOffset(), 5.0);
}

TEST_F(IntegrationLineSymbolizerTest, SetPerpendicularOffset) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetPerpendicularOffset(10.0);
    EXPECT_DOUBLE_EQ(symbolizer->GetPerpendicularOffset(), 10.0);
}

TEST_F(IntegrationLineSymbolizerTest, CanSymbolizeLine) {
    auto symbolizer = LineSymbolizer::Create();
    
    EXPECT_TRUE(symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(IntegrationLineSymbolizerTest, Clone) {
    auto original = LineSymbolizer::Create();
    original->SetWidth(2.5);
    original->SetColor(Color::Green().GetRGBA());
    original->SetOpacity(0.7);
    
    auto cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    
    auto clonedLine = std::dynamic_pointer_cast<LineSymbolizer>(cloned);
    ASSERT_NE(clonedLine, nullptr);
    EXPECT_EQ(clonedLine->GetWidth(), 2.5);
    EXPECT_EQ(clonedLine->GetColor(), Color::Green().GetRGBA());
    EXPECT_DOUBLE_EQ(clonedLine->GetOpacity(), 0.7);
}

TEST_F(IntegrationLineSymbolizerTest, RenderWithContext) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = LineSymbolizer::Create();
    symbolizer->SetWidth(2.0);
    symbolizer->SetColor(Color::Blue().GetRGBA());
    
    DrawStyle style;
    style.stroke.color = Color::Blue().GetRGBA();
    style.stroke.width = 2.0;
    context->SetStyle(style);
    
    double x[] = {10, 100, 200, 246};
    double y[] = {128, 50, 200, 128};
    
    DrawResult result = context->DrawPolyline(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, RenderMultipleLines) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Red().GetRGBA();
    style.stroke.width = 2.0;
    context->SetStyle(style);
    
    double x1[] = {0, 256};
    double y1[] = {0, 256};
    DrawResult result = context->DrawPolyline(x1, y1, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    double x2[] = {0, 256};
    double y2[] = {256, 0};
    result = context->DrawPolyline(x2, y2, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, RenderWithTransform) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(50, 50);
    context->SetTransform(transform);
    
    DrawStyle style;
    style.stroke.color = Color::Magenta().GetRGBA();
    style.stroke.width = 2.0;
    context->SetStyle(style);
    
    double x[] = {0, 100};
    double y[] = {0, 100};
    
    DrawResult result = context->DrawPolyline(x, y, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, RenderDashedLine) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = LineSymbolizer::Create();
    symbolizer->SetWidth(1.0);
    symbolizer->SetColor(Color::Black().GetRGBA());
    symbolizer->SetDashStyle(DashStyle::kDash);
    
    DrawStyle style;
    style.stroke.color = Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    context->SetStyle(style);
    
    double x[] = {0, 256};
    double y[] = {128, 128};
    
    DrawResult result = context->DrawPolyline(x, y, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, RenderThickLine) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    auto symbolizer = LineSymbolizer::Create();
    symbolizer->SetWidth(10.0);
    symbolizer->SetColor(Color::Green().GetRGBA());
    
    DrawStyle style;
    style.stroke.color = Color::Green().GetRGBA();
    style.stroke.width = 10.0;
    context->SetStyle(style);
    
    double x[] = {50, 206};
    double y[] = {50, 206};
    
    DrawResult result = context->DrawPolyline(x, y, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, SetGetName) {
    auto symbolizer = LineSymbolizer::Create();
    
    symbolizer->SetName("test_line_symbolizer");
    EXPECT_EQ(symbolizer->GetName(), "test_line_symbolizer");
}

TEST_F(IntegrationLineSymbolizerTest, RenderVerticalLine) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Cyan().GetRGBA();
    style.stroke.width = 3.0;
    context->SetStyle(style);
    
    double x[] = {128, 128};
    double y[] = {0, 256};
    
    DrawResult result = context->DrawPolyline(x, y, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationLineSymbolizerTest, RenderHorizontalLine) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Yellow().GetRGBA();
    style.stroke.width = 3.0;
    context->SetStyle(style);
    
    double x[] = {0, 256};
    double y[] = {128, 128};
    
    DrawResult result = context->DrawPolyline(x, y, 2);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}
