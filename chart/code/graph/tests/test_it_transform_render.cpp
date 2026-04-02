#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
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
        m_device = std::make_shared<RasterImageDevice>(200, 200, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(IntegrationTransformRenderTest, TranslationTransform) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    m_context->SetTransform(translate);
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 2.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawLine(0, 0, 100, 100);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationTransformRenderTest, ScaleTransform) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix scale = TransformMatrix::CreateScale(2.0, 2.0);
    m_context->SetTransform(scale);
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 1.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawRect(10, 10, 50, 50);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationTransformRenderTest, RotationTransform) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix rotate = TransformMatrix::CreateRotation(45.0 * 3.14159265358979323846 / 180.0);
    m_context->SetTransform(rotate);
    
    DrawStyle style;
    style.pen = Pen(Color::Green(), 2.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawLine(-50, 0, 50, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationTransformRenderTest, CombinedTransform) {
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(100, 100);
    TransformMatrix rotate = TransformMatrix::CreateRotation(3.14159265358979323846 / 4.0);
    TransformMatrix combined = translate.Multiply(rotate);
    
    m_context->SetTransform(combined);
    
    DrawStyle style;
    style.pen = Pen(Color::Magenta(), 2.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawLine(-40, 0, 40, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationTransformRenderTest, ResetTransform) {
    m_context->Begin();
    
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    m_context->SetTransform(translate);
    
    m_context->ResetTransform();
    
    TransformMatrix identity = m_context->GetTransform();
    EXPECT_NEAR(identity.GetTranslationX(), 0.0, EPSILON);
    EXPECT_NEAR(identity.GetTranslationY(), 0.0, EPSILON);
    
    m_context->End();
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
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix scale = TransformMatrix::CreateScale(0.5, 0.5);
    TransformMatrix translate = TransformMatrix::CreateTranslation(50, 50);
    TransformMatrix combined = translate.Multiply(scale);
    
    m_context->SetTransform(combined);
    
    DrawStyle style;
    style.pen = Pen(Color::Cyan(), 2.0);
    m_context->SetStyle(style);
    
    DrawResult result = m_context->DrawRect(0, 0, 100, 100);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}
