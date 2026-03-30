#ifdef __APPLE__

#include <gtest/gtest.h>
#include "ogc/draw/coregraphics_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>

using namespace ogc::draw;

class CoreGraphicsEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100, PixelFormat::kRGBA8888);
        engine = new CoreGraphicsEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    RasterImageDevice* device;
    CoreGraphicsEngine* engine;
};

TEST_F(CoreGraphicsEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kCoreGraphics);
    EXPECT_EQ(engine->GetName(), "CoreGraphicsEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(CoreGraphicsEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(CoreGraphicsEngineTest, DrawPoints) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 50.0, 25.0, 75.0 };
    double y[] = { 50.0, 25.0, 75.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 3;
    
    DrawResult result = engine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawLines) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x1[] = { 10.0, 20.0 };
    double y1[] = { 10.0, 20.0 };
    double x2[] = { 90.0, 80.0 };
    double y2[] = { 90.0, 80.0 };
    
    DrawStyle style;
    style.pen.color = Color::Green();
    style.pen.width = 1;
    
    DrawResult result = engine->DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawPolygon) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 50.0, 90.0, 50.0, 10.0 };
    double y[] = { 10.0, 50.0, 90.0, 50.0 };
    
    DrawStyle style;
    style.brush.color = Color::Red();
    style.pen.color = Color::Black();
    style.pen.width = 1;
    
    DrawResult result = engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawRect(25, 25, 50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawCircle) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Green();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawEllipse) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawEllipse(50, 50, 40, 20, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, DrawArc) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    const double pi = 3.14159265358979323846;
    DrawResult result = engine->DrawArc(50, 50, 30, 30, 0, pi, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, TransformPoint) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    engine->SetTransform(matrix);
    
    double x[] = { 0.0 };
    double y[] = { 0.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 5;
    
    engine->DrawPoints(x, y, 1, style);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, ResetTransform) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(50, 50);
    engine->SetTransform(matrix);
    engine->ResetTransform();
    
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, ClipRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->SetClipRect(25, 25, 50, 50);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    
    engine->DrawRect(0, 0, 100, 100, style, true);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, ResetClip) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->SetClipRect(25, 25, 50, 50);
    engine->ResetClip();
    
    EXPECT_TRUE(engine->GetClipRegion().IsEmpty());
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, SaveRestore) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 10);
    engine->SetTransform(matrix);
    engine->SetOpacity(0.5);
    
    engine->Save();
    
    engine->SetTransform(TransformMatrix::Translate(20, 20));
    engine->SetOpacity(0.8);
    
    engine->Restore();
    
    EXPECT_EQ(engine->GetTransform(), matrix);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, Opacity) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
    
    engine->SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, Clear) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->Clear(Color::Blue());
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, InvalidState) {
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(CoreGraphicsEngineTest, InvalidParameters) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(50, 50, -1, style, true), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawEllipse(50, 50, -1, 10, style, false), DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, BoundaryValues_ZeroRadius) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(50, 50, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, BoundaryValues_LargeCoordinates) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 10000.0, -10000.0 };
    double y[] = { 10000.0, -10000.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CoreGraphicsEngineTest, BoundaryValues_EmptyPolygon) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    
    DrawStyle style;
    DrawResult result = engine->DrawPolygon(x, y, 1, style, true);
    
    engine->End();
}

#else

TEST(CoreGraphicsEngineSkipTest, Skipped) {
    GTEST_SKIP() << "CoreGraphicsEngine tests skipped - not on macOS";
}

#endif
