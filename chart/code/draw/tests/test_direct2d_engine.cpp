#include <gtest/gtest.h>
#include "ogc/draw/direct2d_engine.h"
#include "ogc/draw/raster_image_device.h"

#ifdef _WIN32

using namespace ogc::draw;

class Direct2DEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        Direct2DEngine::InitializeDirect2D();
    }
    
    void TearDown() override {
    }
};

TEST_F(Direct2DEngineTest, IsDirect2DAvailable) {
    EXPECT_TRUE(Direct2DEngine::IsDirect2DAvailable());
}

TEST_F(Direct2DEngineTest, GetName) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    EXPECT_EQ(engine.GetName(), "Direct2DEngine");
}

TEST_F(Direct2DEngineTest, GetType) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    EXPECT_EQ(engine.GetType(), EngineType::kGPU);
}

TEST_F(Direct2DEngineTest, BeginEnd) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPoints) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50, 100, 150};
    double y[] = {50, 100, 150};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    style.pointSize = 5.0;
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawLines) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x1[] = {10, 50};
    double y1[] = {10, 50};
    double x2[] = {100, 150};
    double y2[] = {100, 150};
    
    DrawStyle style;
    style.pen = Pen(Color(0, 255, 0, 255), 2.0);
    
    auto result = engine.DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawLineString) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {10, 50, 100, 150};
    double y[] = {10, 50, 100, 150};
    
    DrawStyle style;
    style.pen = Pen(Color(0, 0, 255, 255), 2.0);
    
    auto result = engine.DrawLineString(x, y, 4, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPolygonFilled) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawStyle style;
    style.brush = Brush(Color(0, 0, 255, 255));
    style.pen = Pen(Color(0, 0, 0, 255), 1.0);
    
    auto result = engine.DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPolygonOutline) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 2.0);
    
    auto result = engine.DrawPolygon(x, y, 4, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawRect) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush = Brush(Color(255, 0, 0, 128));
    style.pen = Pen(Color(0, 0, 0, 255), 1.0);
    
    auto result = engine.DrawRect(50, 50, 100, 100, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawCircle) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush = Brush(Color(0, 255, 0, 200));
    style.pen = Pen(Color(0, 0, 0, 255), 2.0);
    
    auto result = engine.DrawCircle(100, 100, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawEllipse) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    style.brush = Brush(Color(0, 0, 255, 200));
    style.pen = Pen(Color(0, 0, 0, 255), 1.0);
    
    auto result = engine.DrawEllipse(100, 100, 80, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawText) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Font font;
    font.SetFamily("Arial");
    font.SetSize(14);
    font.SetWeight(FontWeight::kNormal);
    
    Color color(0, 0, 0, 255);
    
    auto result = engine.DrawText(50, 100, "Hello Direct2D", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawTextWithBold) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Font font;
    font.SetFamily("Arial");
    font.SetSize(16);
    font.SetWeight(FontWeight::kBold);
    
    Color color(255, 0, 0, 255);
    
    auto result = engine.DrawText(50, 100, "Bold Text", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, SaveRestore) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    engine.Save(static_cast<StateFlags>(StateFlag::kAll));
    
    double x[] = {50, 100, 150};
    double y[] = {50, 100, 150};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    
    engine.DrawPoints(x, y, 3, style);
    
    engine.Restore();
    
    engine.End();
}

TEST_F(Direct2DEngineTest, SetTransform) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    TransformMatrix matrix;
    matrix.Translate(50, 50);
    matrix.Scale(2.0, 2.0);
    
    engine.SetTransform(matrix);
    
    double x[] = {0, 25, 50};
    double y[] = {0, 25, 50};
    
    DrawStyle style;
    style.pen = Pen(Color(0, 255, 0, 255), 1.0);
    style.brush = Brush(Color(0, 255, 0, 255));
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, SetClipRegion) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Region clipRegion;
    clipRegion.AddRect(Rect(25, 25, 150, 150));
    
    engine.SetClipRegion(clipRegion);
    
    double x[] = {0, 100, 200};
    double y[] = {0, 100, 200};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    style.pointSize = 10.0;
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, Clear) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    engine.Clear(Color(255, 255, 255, 255));
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPointsInvalidState) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(Direct2DEngineTest, DrawPointsNullPointers) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    auto result = engine.DrawPoints(nullptr, nullptr, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPointsInvalidCount) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawPolygonInvalidCount) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50, 100};
    double y[] = {50, 100};
    
    DrawStyle style;
    auto result = engine.DrawPolygon(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, DrawTextEmpty) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Font font;
    Color color(0, 0, 0, 255);
    
    auto result = engine.DrawText(50, 50, "", font, color);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, MultipleBeginCalls) {
    RasterImageDevice device(100, 100);
    Direct2DEngine engine(&device);
    
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, LineStyles) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x1[] = {10, 10, 10};
    double y1[] = {10, 50, 90};
    double x2[] = {100, 100, 100};
    double y2[] = {10, 50, 90};
    
    DrawStyle style;
    style.pen = Pen(Color(0, 0, 0, 255), 2.0);
    
    style.pen.style = PenStyle::kSolid;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.pen.style = PenStyle::kDash;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.pen.style = PenStyle::kDot;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.pen.style = PenStyle::kDashDot;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(Direct2DEngineTest, Colors) {
    RasterImageDevice device(200, 200);
    Direct2DEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50, 100, 150};
    double y[] = {50, 50, 50};
    
    DrawStyle style;
    
    style.pen = Pen(Color(255, 0, 0, 255), 1.0);
    style.brush = Brush(Color(255, 0, 0, 255));
    style.pointSize = 5.0;
    EXPECT_EQ(engine.DrawPoints(x, y, 1, style), DrawResult::kSuccess);
    
    style.pen = Pen(Color(0, 255, 0, 128), 1.0);
    style.brush = Brush(Color(0, 255, 0, 128));
    EXPECT_EQ(engine.DrawPoints(x + 1, y + 1, 1, style), DrawResult::kSuccess);
    
    style.pen = Pen(Color(0, 0, 255, 64), 1.0);
    style.brush = Brush(Color(0, 0, 255, 64));
    EXPECT_EQ(engine.DrawPoints(x + 2, y + 2, 1, style), DrawResult::kSuccess);
    
    engine.End();
}

#else

TEST(Direct2DEngineNotAvailable, Skip) {
    GTEST_SKIP() << "Direct2D not available (non-Windows), skipping Direct2DEngine tests";
}

#endif
