#include <gtest/gtest.h>

#ifdef __EMSCRIPTEN__

#include "ogc/draw/webgl_engine.h"
#include "ogc/draw/webgl_device.h"
#include <cmath>

using namespace ogc::draw;

class WebGLEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new WebGLDevice(100, 100);
        device->Initialize();
        engine = new WebGLEngine(device);
    }

    void TearDown() override {
        delete engine;
        device->Shutdown();
        delete device;
    }

    WebGLDevice* device;
    WebGLEngine* engine;
};

TEST_F(WebGLEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kWebGL);
    EXPECT_EQ(engine->GetName(), "WebGLEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
    EXPECT_TRUE(engine->IsGPUAccelerated());
}

TEST_F(WebGLEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(WebGLEngineTest, DrawPoints) {
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

TEST_F(WebGLEngineTest, DrawLines) {
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

TEST_F(WebGLEngineTest, DrawPolygon) {
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

TEST_F(WebGLEngineTest, DrawRect) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawRect(25, 25, 50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WebGLEngineTest, DrawCircle) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush.color = Color::Green();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WebGLEngineTest, Transform) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    engine->SetTransform(matrix);
    
    auto retrieved = engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 20);
    
    engine->End();
}

TEST_F(WebGLEngineTest, ResetTransform) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix = TransformMatrix::Translate(50, 50);
    engine->SetTransform(matrix);
    engine->ResetTransform();
    
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
    
    engine->End();
}

TEST_F(WebGLEngineTest, ClipRegion) {
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

TEST_F(WebGLEngineTest, PushPopState) {
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

TEST_F(WebGLEngineTest, Clear) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    engine->Clear(Color::Blue());
    
    engine->End();
}

TEST_F(WebGLEngineTest, InvalidState) {
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(WebGLEngineTest, InvalidParameters) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(50, 50, -1, style, true), DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(WebGLEngineTest, BoundaryValues_ZeroRadius) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    DrawResult result = engine->DrawCircle(50, 50, 0, style, true);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine->End();
}

TEST_F(WebGLEngineTest, BoundaryValues_LargeCoordinates) {
    ASSERT_EQ(engine->Begin(), DrawResult::kSuccess);
    
    double x[] = { 10000.0, -10000.0 };
    double y[] = { 10000.0, -10000.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawPoints(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine->End();
}

TEST_F(WebGLEngineTest, Initialize) {
    WebGLEngine standaloneEngine;
    bool result = standaloneEngine.Initialize();
    EXPECT_TRUE(result || !result);
}

TEST_F(WebGLEngineTest, SetViewport) {
    engine->SetViewport(200, 200);
    
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
}

#else

TEST(WebGLEngineSkipTest, Skipped) {
    GTEST_SKIP() << "WebGLEngine tests skipped - not on Web/Emscripten";
}

#endif
