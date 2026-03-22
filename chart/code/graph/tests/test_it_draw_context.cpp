#include <gtest/gtest.h>
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/transform_matrix.h"
#include <memory>

using namespace ogc::draw;

class IntegrationDrawContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(200, 200, 4);
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

TEST_F(IntegrationDrawContextTest, ContextCreation) {
    EXPECT_NE(context, nullptr);
    DrawDevicePtr dev = context->GetDevice();
    EXPECT_NE(dev, nullptr);
}

TEST_F(IntegrationDrawContextTest, BeginEndDraw) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    DrawResult result = context->BeginDraw(params);
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(context->IsDrawing());
    
    result = context->EndDraw();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_FALSE(context->IsDrawing());
}

TEST_F(IntegrationDrawContextTest, StatePushPopTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->PushTransform();
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(50, 50);
    context->SetTransform(transform);
    
    context->PopTransform();
    
    TransformMatrix restored = context->GetTransform();
    EXPECT_NEAR(restored.GetTranslationX(), 0.0, 1e-6);
    EXPECT_NEAR(restored.GetTranslationY(), 0.0, 1e-6);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, MultiplePushPopTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->PushTransform();
    TransformMatrix t1 = TransformMatrix::CreateTranslation(10, 10);
    context->SetTransform(t1);
    
    context->PushTransform();
    TransformMatrix t2 = TransformMatrix::CreateTranslation(20, 20);
    context->SetTransform(t2);
    
    context->PopTransform();
    TransformMatrix r1 = context->GetTransform();
    EXPECT_NEAR(r1.GetTranslationX(), 10.0, 1e-6);
    
    context->PopTransform();
    TransformMatrix r2 = context->GetTransform();
    EXPECT_NEAR(r2.GetTranslationX(), 0.0, 1e-6);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, SetTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(100, 50);
    context->SetTransform(transform);
    
    TransformMatrix retrieved = context->GetTransform();
    EXPECT_NEAR(retrieved.GetTranslationX(), 100.0, 1e-6);
    EXPECT_NEAR(retrieved.GetTranslationY(), 50.0, 1e-6);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, ResetTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(100, 50);
    context->SetTransform(transform);
    
    context->ResetTransform();
    
    TransformMatrix identity = context->GetTransform();
    EXPECT_NEAR(identity.GetTranslationX(), 0.0, 1e-6);
    EXPECT_NEAR(identity.GetTranslationY(), 0.0, 1e-6);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, PushPopClipRect) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->PushClipRect(50, 50, 100, 100);
    
    context->PopClipRect();
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, DrawWithClipRect) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    context->PushClipRect(50, 50, 100, 100);
    
    DrawResult result = context->DrawLine(0, 0, 200, 200);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->PopClipRect();
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, SetBackground) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->SetBackground(Color::Blue());
    
    Color bg = context->GetBackground();
    EXPECT_EQ(bg.GetRed(), 0);
    EXPECT_EQ(bg.GetGreen(), 0);
    EXPECT_EQ(bg.GetBlue(), 255);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, Clear) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    EXPECT_NO_THROW(context->Clear(Color::Yellow()));
    
    context->EndDraw();
    
    Color pixel = device->GetPixel(100, 100);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 255);
    EXPECT_EQ(pixel.GetBlue(), 0);
}

TEST_F(IntegrationDrawContextTest, DrawPrimitives) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    
    context->SetStyle(style);
    
    DrawResult result;
    
    result = context->DrawPoint(100, 100);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = context->DrawLine(0, 0, 200, 200);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = context->DrawRect(50, 50, 100, 100);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = context->DrawCircle(100, 100, 50);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, DrawPolygon) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    context->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    style.fill.color = Color::Red().GetRGBA();
    style.fill.visible = true;
    
    context->SetStyle(style);
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawResult result = context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, DeviceAccess) {
    DrawDevicePtr dev = context->GetDevice();
    EXPECT_NE(dev, nullptr);
    
    EXPECT_EQ(dev->GetWidth(), 200);
    EXPECT_EQ(dev->GetHeight(), 200);
}

TEST_F(IntegrationDrawContextTest, TranslateRotateScale) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->Translate(50, 50);
    TransformMatrix t1 = context->GetTransform();
    EXPECT_NEAR(t1.GetTranslationX(), 50.0, 1e-6);
    
    context->Rotate(0.5);
    
    context->Scale(2.0, 2.0);
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, PushPopStyle) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    DrawStyle style1;
    style1.stroke.color = Color::Red().GetRGBA();
    context->SetStyle(style1);
    
    context->PushStyle(style1);
    
    DrawStyle style2;
    style2.stroke.color = Color::Blue().GetRGBA();
    context->SetStyle(style2);
    
    context->PopStyle();
    
    context->EndDraw();
}

TEST_F(IntegrationDrawContextTest, PushPopOpacity) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    
    context->BeginDraw(params);
    
    context->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.5);
    
    context->PushOpacity(0.3);
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.3);
    
    context->PopOpacity();
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.5);
    
    context->EndDraw();
}
