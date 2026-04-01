#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/transform_matrix.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class IntegrationBasicRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(100, 100, 4);
        ASSERT_NE(device, nullptr);
        
        DrawResult result = device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    void TearDown() override {
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
};

TEST_F(IntegrationBasicRenderTest, DeviceInitialization) {
    EXPECT_TRUE(device->IsReady());
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
    EXPECT_EQ(device->GetChannels(), 4);
}

TEST_F(IntegrationBasicRenderTest, BeginEndDraw) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    DrawResult beginResult = device->BeginDraw(params);
    EXPECT_EQ(beginResult, DrawResult::kSuccess);
    EXPECT_TRUE(device->IsDrawing());
    
    DrawResult endResult = device->EndDraw();
    EXPECT_EQ(endResult, DrawResult::kSuccess);
    EXPECT_FALSE(device->IsDrawing());
}

TEST_F(IntegrationBasicRenderTest, DrawPoint) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 1.0;
    
    DrawResult result = device->DrawPoint(50, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawLine) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Green().GetRGBA();
    style.pen.width = 2.0;
    
    DrawResult result = device->DrawLine(10, 10, 90, 90, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawRect) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Blue().GetRGBA();
    style.pen.width = 1.0;
    style.brush.color = Color::Yellow().GetRGBA();
    style.brush.visible = true;
    
    DrawResult result = device->DrawRect(20, 20, 60, 60, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawCircle) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Magenta().GetRGBA();
    style.pen.width = 2.0;
    style.brush.color = Color::Cyan().GetRGBA();
    style.brush.visible = true;
    
    DrawResult result = device->DrawCircle(50, 50, 25, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawEllipse) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 1.0;
    
    DrawResult result = device->DrawEllipse(50, 50, 30, 20, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawPolygon) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 1.0;
    style.brush.color = Color::Gray().GetRGBA();
    style.brush.visible = true;
    
    double x[] = {10, 50, 90, 50};
    double y[] = {50, 10, 50, 90};
    
    DrawResult result = device->DrawPolygon(x, y, 4, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, DrawPolyline) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 2.0;
    
    double x[] = {10, 30, 50, 70, 90};
    double y[] = {50, 20, 80, 20, 50};
    
    DrawResult result = device->DrawPolyline(x, y, 5, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, ClearAndFill) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    EXPECT_NO_THROW(device->Clear(Color::White()));
    
    device->EndDraw();
    
    Color pixel = device->GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 255);
    EXPECT_EQ(pixel.GetBlue(), 255);
}

TEST_F(IntegrationBasicRenderTest, SetPixelAndGetPixel) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    device->SetPixel(50, 50, Color::Red());
    
    device->EndDraw();
    
    Color pixel = device->GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 0);
    EXPECT_EQ(pixel.GetBlue(), 0);
}

TEST_F(IntegrationBasicRenderTest, ClipRect) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    device->SetClipRect(20, 20, 60, 60);
    EXPECT_TRUE(device->HasClipRect());
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 1.0;
    
    DrawResult result = device->DrawRect(0, 0, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->ClearClipRect();
    EXPECT_FALSE(device->HasClipRect());
    
    device->EndDraw();
}

TEST_F(IntegrationBasicRenderTest, Transform) {
    DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = Envelope(0, 0, 100, 100);
    
    device->BeginDraw(params);
    
    TransformMatrix transform;
    transform = TransformMatrix::CreateTranslation(10, 20);
    device->SetTransform(transform);
    
    TransformMatrix retrieved = device->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetTranslationX(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetTranslationY(), 20);
    
    device->ResetTransform();
    
    device->EndDraw();
}

