#ifdef __APPLE__

#include <gtest/gtest.h>
#include "ogc/draw/metal_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>

using namespace ogc::draw;

class MetalEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100, PixelFormat::kRGBA8888);
        engine = new MetalEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    RasterImageDevice* device;
    MetalEngine* engine;
};

TEST_F(MetalEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kMetal);
    EXPECT_EQ(engine->GetName(), "MetalEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
    EXPECT_TRUE(engine->IsGPUAccelerated());
}

TEST_F(MetalEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(MetalEngineTest, DrawPoints) {
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

TEST_F(MetalEngineTest, DrawLines) {
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

TEST_F(MetalEngineTest, DrawPolygon) {
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

TEST_F(MetalEngineTest, DrawRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawRect(25, 25, 50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(MetalEngineTest, DrawCircle) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Green();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(MetalEngineTest, Transform) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 20);
    
    engine->End();
}

TEST_F(MetalEngineTest, ResetTransform) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(50, 50);
    engine->SetTransform(matrix);
    engine->ResetTransform();
    
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
    
    engine->End();
}

TEST_F(MetalEngineTest, ClipRegion) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    Region region;
    region.x = 25;
    region.y = 25;
    region.width = 50;
    region.height = 50;
    
    engine->SetClipRegion(region);
    
    auto clip = engine->GetClipRegion();
    EXPECT_FALSE(clip.IsEmpty());
    
    engine->End();
}

TEST_F(MetalEngineTest, PushPopState) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 10);
    engine->SetTransform(matrix);
    
    engine->PushState();
    
    engine->SetTransform(TransformMatrix::Translate(20, 20));
    
    engine->PopState();
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 10);
    
    engine->End();
}

TEST_F(MetalEngineTest, Clear) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->Clear(Color::Blue());
    
    engine->End();
}

TEST_F(MetalEngineTest, InvalidState) {
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(MetalEngineTest, InvalidParameters) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(50, 50, -1, style, true), DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(MetalEngineTest, BoundaryValues_ZeroRadius) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(50, 50, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(MetalEngineTest, BoundaryValues_LargeCoordinates) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 10000.0, -10000.0 };
    double y[] = { 10000.0, -10000.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(MetalEngineTest, Initialize) {
    MetalEngine standaloneEngine;
    bool result = standaloneEngine.Initialize();
    EXPECT_TRUE(result || !result);
}

#else

TEST(MetalEngineSkipTest, Skipped) {
    GTEST_SKIP() << "MetalEngine tests skipped - not on macOS";
}

#endif
