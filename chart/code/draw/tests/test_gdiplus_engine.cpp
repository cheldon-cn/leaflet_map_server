#include <gtest/gtest.h>
#include "ogc/draw/gdiplus_engine.h"
#include "ogc/draw/raster_image_device.h"

#ifdef _WIN32

using namespace ogc::draw;

class GDIPlusEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        GDIPlusEngine::InitializeGDIPlus();
    }
    
    void TearDown() override {
    }
};

TEST_F(GDIPlusEngineTest, IsGDIPlusAvailable) {
    EXPECT_TRUE(GDIPlusEngine::IsGDIPlusAvailable());
}

TEST_F(GDIPlusEngineTest, GetName) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    EXPECT_EQ(engine.GetName(), "GDIPlusEngine");
}

TEST_F(GDIPlusEngineTest, GetType) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    EXPECT_EQ(engine.GetType(), EngineType::kSimple2D);
}

TEST_F(GDIPlusEngineTest, BeginEnd) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawPoints) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, DrawLines) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, DrawPolygonFilled) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, DrawPolygonOutline) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0, 255), 2.0);
    
    auto result = engine.DrawPolygon(x, y, 4, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawText) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Font font;
    font.SetFamily("Arial");
    font.SetSize(14);
    font.SetWeight(FontWeight::kNormal);
    
    Color color(0, 0, 0, 255);
    
    auto result = engine.DrawText(50, 100, "Hello GDI+", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawTextWithBold) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, SaveRestore) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, SetTransform) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, SetClipRegion) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, DrawPointsInvalidState) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(GDIPlusEngineTest, DrawPointsNullPointers) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    DrawStyle style;
    auto result = engine.DrawPoints(nullptr, nullptr, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawPointsInvalidCount) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawPolygonInvalidCount) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    double x[] = {50, 100};
    double y[] = {50, 100};
    
    DrawStyle style;
    auto result = engine.DrawPolygon(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, DrawTextEmpty) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    Font font;
    Color color(0, 0, 0, 255);
    
    auto result = engine.DrawText(50, 50, "", font, color);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, GetGraphics) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    EXPECT_EQ(engine.GetGraphics(), nullptr);
    
    ASSERT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, MultipleBeginCalls) {
    RasterImageDevice device(100, 100);
    GDIPlusEngine engine(&device);
    
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    EXPECT_EQ(engine.Begin(), DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(GDIPlusEngineTest, LineStyles) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, Colors) {
    RasterImageDevice device(200, 200);
    GDIPlusEngine engine(&device);
    
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

TEST_F(GDIPlusEngineTest, MultipleInstances) {
    RasterImageDevice device1(100, 100);
    RasterImageDevice device2(100, 100);
    
    GDIPlusEngine engine1(&device1);
    GDIPlusEngine engine2(&device2);
    
    ASSERT_EQ(engine1.Begin(), DrawResult::kSuccess);
    ASSERT_EQ(engine2.Begin(), DrawResult::kSuccess);
    
    engine1.End();
    engine2.End();
}

#else

TEST(GDIPlusEngineNotAvailable, Skip) {
    GTEST_SKIP() << "GDI+ not available (non-Windows), skipping GDIPlusEngine tests";
}

#endif
