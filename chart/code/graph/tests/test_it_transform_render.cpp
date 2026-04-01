#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/transform_matrix.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/draw/draw_params.h"
#include "ogc/envelope.h"
#include <cmath>
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

namespace {
    const double EPSILON = 1e-6;
}

class IntegrationTransformRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(200, 200, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
    }
    
    void TearDown() override {
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
};

TEST_F(IntegrationTransformRenderTest, TranslationTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    device->SetTransform(translate);
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 2.0;
    
    DrawResult result = device->DrawLine(0, 0, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationTransformRenderTest, ScaleTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    TransformMatrix scale = TransformMatrix::CreateScale(2.0, 2.0);
    device->SetTransform(scale);
    
    DrawStyle style;
    style.pen.color = Color::Blue().GetRGBA();
    style.pen.width = 1.0;
    
    DrawResult result = device->DrawRect(10, 10, 50, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationTransformRenderTest, RotationTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    TransformMatrix rotate = TransformMatrix::CreateRotation(45.0 * 3.14159265358979323846 / 180.0);
    device->SetTransform(rotate);
    
    DrawStyle style;
    style.pen.color = Color::Green().GetRGBA();
    style.pen.width = 2.0;
    
    DrawResult result = device->DrawLine(-50, 0, 50, 0, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationTransformRenderTest, CombinedTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(100, 100);
    TransformMatrix rotate = TransformMatrix::CreateRotation(3.14159265358979323846 / 4.0);
    TransformMatrix combined = translate.Multiply(rotate);
    
    device->SetTransform(combined);
    
    DrawStyle style;
    style.pen.color = Color::Magenta().GetRGBA();
    style.pen.width = 2.0;
    
    DrawResult result = device->DrawLine(-40, 0, 40, 0, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

TEST_F(IntegrationTransformRenderTest, ResetTransform) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    device->SetTransform(translate);
    
    device->ResetTransform();
    
    TransformMatrix identity = device->GetTransform();
    EXPECT_NEAR(identity.GetTranslationX(), 0.0, EPSILON);
    EXPECT_NEAR(identity.GetTranslationY(), 0.0, EPSILON);
    
    device->EndDraw();
}

TEST_F(IntegrationTransformRenderTest, TransformMatrixIdentity) {
    TransformMatrix identity;
    
    EXPECT_NEAR(identity.GetTranslationX(), 0.0, EPSILON);
    EXPECT_NEAR(identity.GetTranslationY(), 0.0, EPSILON);
    EXPECT_NEAR(identity.GetScaleX(), 1.0, EPSILON);
    EXPECT_NEAR(identity.GetScaleY(), 1.0, EPSILON);
    EXPECT_NEAR(identity.GetRotation(), 0.0, EPSILON);
}

TEST_F(IntegrationTransformRenderTest, TransformMatrixMultiply) {
    TransformMatrix t1 = TransformMatrix::CreateTranslation(10, 20);
    TransformMatrix t2 = TransformMatrix::CreateTranslation(30, 40);
    
    TransformMatrix result = t1.Multiply(t2);
    
    EXPECT_NEAR(result.GetTranslationX(), 40.0, EPSILON);
    EXPECT_NEAR(result.GetTranslationY(), 60.0, EPSILON);
}

TEST_F(IntegrationTransformRenderTest, TransformMatrixInverse) {
    TransformMatrix original = TransformMatrix::CreateTranslation(50, 100);
    
    TransformMatrix inverse = original.Inverse();
    
    EXPECT_NEAR(inverse.GetTranslationX(), -50.0, EPSILON);
    EXPECT_NEAR(inverse.GetTranslationY(), -100.0, EPSILON);
}

TEST_F(IntegrationTransformRenderTest, TransformPoint) {
    TransformMatrix translate = TransformMatrix::CreateTranslation(100, 50);
    
    double x = 10, y = 20;
    double outX, outY;
    translate.Transform(x, y, outX, outY);
    
    EXPECT_NEAR(outX, 110.0, EPSILON);
    EXPECT_NEAR(outY, 70.0, EPSILON);
}

TEST_F(IntegrationTransformRenderTest, ScaleAndTranslate) {
    DrawParams params;
    params.pixel_width = 200;
    params.pixel_height = 200;
    params.extent = Envelope(0, 0, 200, 200);
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    TransformMatrix scale = TransformMatrix::CreateScale(0.5, 0.5);
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    TransformMatrix combined = translate.Multiply(scale);
    
    device->SetTransform(combined);
    
    DrawStyle style;
    style.pen.color = Color::Cyan().GetRGBA();
    style.pen.width = 2.0;
    
    DrawResult result = device->DrawRect(0, 0, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    device->EndDraw();
}

