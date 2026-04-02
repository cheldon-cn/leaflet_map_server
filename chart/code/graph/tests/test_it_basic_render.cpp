#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/simple2d_engine.h>
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
        device = std::make_unique<RasterImageDevice>(100, 100, PixelFormat::kRGBA8888);
        ASSERT_NE(device, nullptr);
        
        DrawResult result = device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        engine = std::make_unique<Simple2DEngine>(device.get());
        ASSERT_NE(engine, nullptr);
    }
    
    void TearDown() override {
        engine.reset();
        if (device) {
            device->Finalize();
        }
    }
    
    std::unique_ptr<RasterImageDevice> device;
    std::unique_ptr<Simple2DEngine> engine;
};

TEST_F(IntegrationBasicRenderTest, DeviceInitialization) {
    EXPECT_TRUE(device->IsReady());
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
    EXPECT_EQ(device->GetBytesPerPixel(), 4);
}

TEST_F(IntegrationBasicRenderTest, EngineCreation) {
    EXPECT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), EngineType::kSimple2D);
}

TEST_F(IntegrationBasicRenderTest, BeginEndRender) {
    DrawResult beginResult = engine->Begin();
    EXPECT_EQ(beginResult, DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(IntegrationBasicRenderTest, DrawPoint) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 1.0);
    
    double x[] = {50};
    double y[] = {50};
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawLine) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Green(), 2.0);
    
    double x1[] = {10};
    double y1[] = {10};
    double x2[] = {90};
    double y2[] = {90};
    DrawResult result = engine->DrawLines(x1, y1, x2, y2, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawRect) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 1.0);
    style.brush = Brush(Color::Yellow());
    
    DrawResult result = engine->DrawRect(20, 20, 60, 60, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawCircle) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Magenta(), 2.0);
    style.brush = Brush(Color::Cyan());
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawEllipse) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 1.0);
    style.brush = Brush::NoBrush();
    
    DrawResult result = engine->DrawEllipse(50, 50, 30, 20, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawPolygon) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Black(), 1.0);
    style.brush = Brush(Color::Gray());
    
    double x[] = {10, 50, 90, 50};
    double y[] = {50, 10, 50, 90};
    
    DrawResult result = engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, DrawPolyline) {
    engine->Begin();
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 2.0);
    
    double x[] = {10, 30, 50, 70, 90};
    double y[] = {50, 20, 80, 20, 50};
    
    DrawResult result = engine->DrawLineString(x, y, 5, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, ClearAndFill) {
    engine->Begin();
    
    EXPECT_NO_THROW(engine->Clear(Color::White()));
    
    engine->End();
    
    Color pixel = device->GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 255);
    EXPECT_EQ(pixel.GetBlue(), 255);
}

TEST_F(IntegrationBasicRenderTest, SetPixelAndGetPixel) {
    engine->Begin();
    engine->Clear(Color::White());
    
    device->SetPixel(50, 50, Color::Red());
    
    engine->End();
    
    Color pixel = device->GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 0);
    EXPECT_EQ(pixel.GetBlue(), 0);
}

TEST_F(IntegrationBasicRenderTest, ClipRect) {
    engine->Begin();
    
    engine->SetClipRect(20, 20, 60, 60);
    Region clipRegion = engine->GetClipRegion();
    EXPECT_FALSE(clipRegion.IsEmpty());
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 1.0);
    
    DrawResult result = engine->DrawRect(0, 0, 100, 100, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->ResetClip();
    clipRegion = engine->GetClipRegion();
    EXPECT_TRUE(clipRegion.IsEmpty());
    
    engine->End();
}

TEST_F(IntegrationBasicRenderTest, Transform) {
    engine->Begin();
    
    TransformMatrix transform = TransformMatrix::Translate(10, 20);
    engine->SetTransform(transform);
    
    TransformMatrix retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetTranslationX(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetTranslationY(), 20);
    
    engine->ResetTransform();
    
    engine->End();
}
