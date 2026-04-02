#include <gtest/gtest.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/transform_matrix.h>
#include <memory>

using namespace ogc::draw;

class IntegrationDrawContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = std::make_shared<RasterImageDevice>(200, 200, PixelFormat::kRGBA8888);
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

TEST_F(IntegrationDrawContextTest, ContextCreation) {
    EXPECT_NE(context, nullptr);
    DrawDevice* dev = context->GetDevice();
    EXPECT_NE(dev, nullptr);
}

TEST_F(IntegrationDrawContextTest, BeginEndDraw) {
    DrawResult result = context->Begin();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(context->IsActive());
    
    context->End();
    EXPECT_FALSE(context->IsActive());
}

TEST_F(IntegrationDrawContextTest, StatePushPopTransform) {
    context->Begin();
    
    context->Save(StateFlags(StateFlag::kTransform));
    
    TransformMatrix transform = TransformMatrix::Translate(50, 50);
    context->SetTransform(transform);
    
    context->Restore();
    
    TransformMatrix restored = context->GetTransform();
    EXPECT_NEAR(restored.GetTranslationX(), 0.0, 1e-6);
    EXPECT_NEAR(restored.GetTranslationY(), 0.0, 1e-6);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, MultiplePushPopTransform) {
    context->Begin();
    
    context->Save(StateFlags(StateFlag::kTransform));
    TransformMatrix t1 = TransformMatrix::Translate(10, 10);
    context->SetTransform(t1);
    
    context->Save(StateFlags(StateFlag::kTransform));
    TransformMatrix t2 = TransformMatrix::Translate(20, 20);
    context->SetTransform(t2);
    
    context->Restore();
    TransformMatrix r1 = context->GetTransform();
    EXPECT_NEAR(r1.GetTranslationX(), 10.0, 1e-6);
    
    context->Restore();
    TransformMatrix r2 = context->GetTransform();
    EXPECT_NEAR(r2.GetTranslationX(), 0.0, 1e-6);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, SetTransform) {
    context->Begin();
    
    TransformMatrix transform = TransformMatrix::Translate(100, 50);
    context->SetTransform(transform);
    
    TransformMatrix retrieved = context->GetTransform();
    EXPECT_NEAR(retrieved.GetTranslationX(), 100.0, 1e-6);
    EXPECT_NEAR(retrieved.GetTranslationY(), 50.0, 1e-6);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, ResetTransform) {
    context->Begin();
    
    TransformMatrix transform = TransformMatrix::Translate(100, 50);
    context->SetTransform(transform);
    
    context->ResetTransform();
    
    TransformMatrix identity = context->GetTransform();
    EXPECT_NEAR(identity.GetTranslationX(), 0.0, 1e-6);
    EXPECT_NEAR(identity.GetTranslationY(), 0.0, 1e-6);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, PushPopClipRect) {
    context->Begin();
    
    context->SetClipRect(50, 50, 100, 100);
    
    context->ResetClip();
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, DrawWithClipRect) {
    context->Begin();
    context->Clear(Color::White());
    
    context->SetClipRect(50, 50, 100, 100);
    
    DrawResult result = context->DrawLine(0, 0, 200, 200);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->ResetClip();
    context->End();
}

TEST_F(IntegrationDrawContextTest, Clear) {
    context->Begin();
    
    EXPECT_NO_THROW(context->Clear(Color::Yellow()));
    
    context->End();
    
    Color pixel = device->GetPixel(100, 100);
    EXPECT_EQ(pixel.GetRed(), 255);
    EXPECT_EQ(pixel.GetGreen(), 255);
    EXPECT_EQ(pixel.GetBlue(), 0);
}

TEST_F(IntegrationDrawContextTest, DrawPrimitives) {
    context->Begin();
    context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Black(), 1.0);
    
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
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, DrawPolygon) {
    context->Begin();
    context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Black(), 1.0);
    style.brush = Brush(Color::Red());
    
    context->SetStyle(style);
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawResult result = context->DrawPolygon(x, y, 4);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, DeviceAccess) {
    DrawDevice* dev = context->GetDevice();
    EXPECT_NE(dev, nullptr);
    
    EXPECT_EQ(dev->GetWidth(), 200);
    EXPECT_EQ(dev->GetHeight(), 200);
}

TEST_F(IntegrationDrawContextTest, TranslateRotateScale) {
    context->Begin();
    
    context->Translate(50, 50);
    TransformMatrix t1 = context->GetTransform();
    EXPECT_NEAR(t1.GetTranslationX(), 50.0, 1e-6);
    
    context->Rotate(0.5);
    
    context->Scale(2.0, 2.0);
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, PushPopStyle) {
    context->Begin();
    
    DrawStyle style1;
    style1.pen = Pen(Color::Red(), 1.0);
    context->SetStyle(style1);
    
    context->Save(StateFlags(StateFlag::kStyle));
    
    DrawStyle style2;
    style2.pen = Pen(Color::Blue(), 1.0);
    context->SetStyle(style2);
    
    context->Restore();
    
    context->End();
}

TEST_F(IntegrationDrawContextTest, PushPopOpacity) {
    context->Begin();
    
    context->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.5);
    
    context->Save(StateFlags(StateFlag::kOpacity));
    context->SetOpacity(0.3);
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.3);
    
    context->Restore();
    EXPECT_DOUBLE_EQ(context->GetOpacity(), 0.5);
    
    context->End();
}
