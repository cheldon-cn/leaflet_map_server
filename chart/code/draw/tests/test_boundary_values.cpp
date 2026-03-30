#include <gtest/gtest.h>
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/geometry.h"
#include <cmath>
#include <limits>

using namespace ogc::draw;

class BoundaryValueTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(800, 600, PixelFormat::kRGBA8888);
        engine = new Simple2DEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    RasterImageDevice* device;
    Simple2DEngine* engine;
};

TEST_F(BoundaryValueTest, DrawPoints_EmptyArray) {
    engine->Begin();
    
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPoints_SinglePoint) {
    engine->Begin();
    
    double x[] = { 400.0 };
    double y[] = { 300.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPoints_LargeCount) {
    engine->Begin();
    
    const int count = 10000;
    std::vector<double> x(count), y(count);
    for (int i = 0; i < count; ++i) {
        x[i] = static_cast<double>(i % 800);
        y[i] = static_cast<double>(i % 600);
    }
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x.data(), y.data(), count, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPoints_NegativeCoordinates) {
    engine->Begin();
    
    double x[] = { -100.0, -50.0 };
    double y[] = { -100.0, -50.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPoints_OutOfBounds) {
    engine->Begin();
    
    double x[] = { 10000.0, -10000.0 };
    double y[] = { 10000.0, -10000.0 };
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawCircle_ZeroRadius) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(400, 300, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawCircle_NegativeRadius) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(400, 300, -10, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawCircle_MinRadius) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    DrawResult result = engine->DrawCircle(400, 300, 1, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawCircle_LargeRadius) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    DrawResult result = engine->DrawCircle(400, 300, 10000, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawEllipse_ZeroRadiusX) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawEllipse(400, 300, 0, 50, style, false);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawEllipse_ZeroRadiusY) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawEllipse(400, 300, 50, 0, style, false);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawEllipse_NegativeRadiusX) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawEllipse(400, 300, -10, 50, style, false);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawEllipse_NegativeRadiusY) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawEllipse(400, 300, 50, -10, style, false);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawRect_ZeroWidth) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawRect(100, 100, 0, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawRect_ZeroHeight) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawRect(100, 100, 50, 0, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawRect_NegativeWidth) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawRect(100, 100, -50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawRect_NegativeHeight) {
    engine->Begin();
    
    DrawStyle style;
    DrawResult result = engine->DrawRect(100, 100, 50, -50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPolygon_MinPoints) {
    engine->Begin();
    
    double x[] = { 100.0 };
    double y[] = { 100.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPolygon(x, y, 1, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPolygon_TwoPoints) {
    engine->Begin();
    
    double x[] = { 100.0, 200.0 };
    double y[] = { 100.0, 200.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPolygon(x, y, 2, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(BoundaryValueTest, DrawPolygon_ThreePoints) {
    engine->Begin();
    
    double x[] = { 100.0, 200.0, 150.0 };
    double y[] = { 100.0, 100.0, 200.0 };
    DrawStyle style;
    style.brush.color = Color::Red();
    
    DrawResult result = engine->DrawPolygon(x, y, 3, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Opacity_BoundaryZero) {
    engine->Begin();
    
    engine->SetOpacity(0.0);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Opacity_BoundaryOne) {
    engine->Begin();
    
    engine->SetOpacity(1.0);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Opacity_ExceedsMax) {
    engine->Begin();
    
    engine->SetOpacity(2.0);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Opacity_BelowMin) {
    engine->Begin();
    
    engine->SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, PenWidth_Zero) {
    engine->Begin();
    
    double x[] = { 400.0 };
    double y[] = { 300.0 };
    DrawStyle style;
    style.pen.width = 0;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, PenWidth_Large) {
    engine->Begin();
    
    double x[] = { 400.0 };
    double y[] = { 300.0 };
    DrawStyle style;
    style.pen.width = 100;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Transform_Identity) {
    engine->Begin();
    
    engine->SetTransform(TransformMatrix::Identity());
    auto matrix = engine->GetTransform();
    EXPECT_TRUE(matrix.IsIdentity());
    
    engine->End();
}

TEST_F(BoundaryValueTest, Transform_LargeTranslation) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10000, 10000);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10000);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 10000);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Transform_NegativeTranslation) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(-10000, -10000);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], -10000);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], -10000);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Transform_ScaleZero) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Scale(0, 0);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][0], 0);
    EXPECT_DOUBLE_EQ(retrieved.m[1][1], 0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Transform_ScaleNegative) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Scale(-1, -1);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][0], -1);
    EXPECT_DOUBLE_EQ(retrieved.m[1][1], -1);
    
    engine->End();
}

TEST_F(BoundaryValueTest, ClipRect_ZeroSize) {
    engine->Begin();
    
    engine->SetClipRect(100, 100, 0, 0);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    engine->DrawRect(0, 0, 800, 600, style, true);
    
    engine->End();
}

TEST_F(BoundaryValueTest, ClipRect_FullCanvas) {
    engine->Begin();
    
    engine->SetClipRect(0, 0, 800, 600);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    DrawResult result = engine->DrawRect(0, 0, 800, 600, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, ClipRect_NegativeOrigin) {
    engine->Begin();
    
    engine->SetClipRect(-100, -100, 800, 600);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    DrawResult result = engine->DrawRect(0, 0, 800, 600, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Color_Transparent) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color(255, 0, 0, 0);
    
    DrawResult result = engine->DrawRect(100, 100, 200, 200, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Color_FullyOpaque) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color(255, 0, 0, 255);
    
    DrawResult result = engine->DrawRect(100, 100, 200, 200, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Tolerance_Zero) {
    engine->Begin();
    
    engine->SetTolerance(0.0);
    EXPECT_DOUBLE_EQ(engine->GetTolerance(), 0.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Tolerance_Negative) {
    engine->Begin();
    
    engine->SetTolerance(-1.0);
    EXPECT_DOUBLE_EQ(engine->GetTolerance(), -1.0);
    
    engine->End();
}

TEST_F(BoundaryValueTest, Tolerance_Large) {
    engine->Begin();
    
    engine->SetTolerance(1000.0);
    EXPECT_DOUBLE_EQ(engine->GetTolerance(), 1000.0);
    
    engine->End();
}
