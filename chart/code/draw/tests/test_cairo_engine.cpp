#ifdef OGC_HAS_CAIRO

#include <gtest/gtest.h>
#include "ogc/draw/cairo_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>

using namespace ogc::draw;

class CairoEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100, PixelFormat::kRGBA8888);
        engine = new CairoEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    RasterImageDevice* device;
    CairoEngine* engine;
};

TEST_F(CairoEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kCairo);
    EXPECT_EQ(engine->GetName(), "CairoEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(CairoEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(CairoEngineTest, DrawPoints) {
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

TEST_F(CairoEngineTest, DrawLines) {
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

TEST_F(CairoEngineTest, DrawPolygon) {
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

TEST_F(CairoEngineTest, DrawRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawRect(25, 25, 50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CairoEngineTest, DrawCircle) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Green();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CairoEngineTest, TransformPoint) {
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

TEST_F(CairoEngineTest, ClipRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->SetClipRect(25, 25, 50, 50);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    
    engine->DrawRect(0, 0, 100, 100, style, true);
    
    engine->End();
}

TEST_F(CairoEngineTest, Clear) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->Clear(Color::Blue());
    
    engine->End();
}

TEST_F(CairoEngineTest, InvalidState) {
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(CairoEngineTest, InvalidParameters) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(50, 50, -1, style, true), DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(CairoEngineTest, BoundaryValues_ZeroRadius) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(50, 50, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(CairoEngineTest, BoundaryValues_LargeCoordinates) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 10000.0, -10000.0 };
    double y[] = { 10000.0, -10000.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(CairoEngineTest, BoundaryValues_EmptyPolygon) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    
    DrawStyle style;
    DrawResult result = engine->DrawPolygon(x, y, 1, style, true);
    
    engine->End();
}

#else

TEST(CairoEngineSkipTest, Skipped) {
    GTEST_SKIP() << "CairoEngine tests skipped - Cairo not available";
}

#endif
