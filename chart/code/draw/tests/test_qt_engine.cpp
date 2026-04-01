#include <gtest/gtest.h>
#include "ogc/draw/qt_engine.h"
#include "ogc/draw/raster_image_device.h"

#ifdef DRAW_HAS_QT

#include <QImage>
#include <QPainter>

using namespace ogc::draw;

class QtEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(QtEngineTest, IsQtAvailable) {
    EXPECT_TRUE(QtEngine::IsQtAvailable());
}

TEST_F(QtEngineTest, GetName) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    EXPECT_EQ(engine.GetName(), "QtEngine");
}

TEST_F(QtEngineTest, GetType) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    EXPECT_EQ(engine.GetType(), EngineType::kSimple2D);
}

TEST_F(QtEngineTest, BeginEnd) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    EXPECT_TRUE(engine.Begin());
    engine.End();
}

TEST_F(QtEngineTest, DrawPoints) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {50, 100, 150};
    double y[] = {50, 100, 150};
    
    DrawStyle style;
    style.lineColor = Color(1.0, 0.0, 0.0, 1.0);
    style.pointSize = 5.0;
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawLines) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x1[] = {10, 50};
    double y1[] = {10, 50};
    double x2[] = {100, 150};
    double y2[] = {100, 150};
    
    DrawStyle style;
    style.lineColor = Color(0.0, 1.0, 0.0, 1.0);
    style.lineWidth = 2.0;
    
    auto result = engine.DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawPolygonFilled) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawStyle style;
    style.fillColor = Color(0.0, 0.0, 1.0, 1.0);
    style.lineColor = Color(0.0, 0.0, 0.0, 1.0);
    style.fill = true;
    
    auto result = engine.DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawPolygonOutline) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {100, 150, 100, 50};
    double y[] = {50, 100, 150, 100};
    
    DrawStyle style;
    style.lineColor = Color(1.0, 0.0, 0.0, 1.0);
    style.lineWidth = 2.0;
    
    auto result = engine.DrawPolygon(x, y, 4, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawText) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    Font font;
    font.SetFamily("Arial");
    font.SetSize(14);
    font.SetBold(false);
    
    Color color(0.0, 0.0, 0.0, 1.0);
    
    auto result = engine.DrawText(50, 100, "Hello Qt", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawTextWithBold) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    Font font;
    font.SetFamily("Arial");
    font.SetSize(16);
    font.SetBold(true);
    
    Color color(1.0, 0.0, 0.0, 1.0);
    
    auto result = engine.DrawText(50, 100, "Bold Text", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, SaveRestore) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    engine.Save(StateFlag::kAll);
    
    double x[] = {50, 100, 150};
    double y[] = {50, 100, 150};
    
    DrawStyle style;
    style.lineColor = Color(1.0, 0.0, 0.0, 1.0);
    
    engine.DrawPoints(x, y, 3, style);
    
    engine.Restore();
    
    engine.End();
}

TEST_F(QtEngineTest, SetTransform) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    TransformMatrix matrix;
    matrix.Translate(50, 50);
    matrix.Scale(2.0, 2.0);
    
    engine.SetTransform(matrix);
    
    double x[] = {0, 25, 50};
    double y[] = {0, 25, 50};
    
    DrawStyle style;
    style.lineColor = Color(0.0, 1.0, 0.0, 1.0);
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, SetClipRegion) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    Region clipRegion;
    clipRegion.x = 25;
    clipRegion.y = 25;
    clipRegion.width = 150;
    clipRegion.height = 150;
    
    engine.SetClipRegion(clipRegion);
    
    double x[] = {0, 100, 200};
    double y[] = {0, 100, 200};
    
    DrawStyle style;
    style.lineColor = Color(1.0, 0.0, 0.0, 1.0);
    style.pointSize = 10.0;
    
    auto result = engine.DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawPointsInvalidState) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(QtEngineTest, DrawPointsNullPointers) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    DrawStyle style;
    auto result = engine.DrawPoints(nullptr, nullptr, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawPointsInvalidCount) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {50};
    double y[] = {50};
    
    DrawStyle style;
    auto result = engine.DrawPoints(x, y, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawPolygonInvalidCount) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {50, 100};
    double y[] = {50, 100};
    
    DrawStyle style;
    auto result = engine.DrawPolygon(x, y, 2, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(QtEngineTest, DrawTextEmpty) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    Font font;
    Color color(0, 0, 0, 1);
    
    auto result = engine.DrawText(50, 50, "", font, color);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    engine.End();
}

TEST_F(QtEngineTest, GetPainter) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    EXPECT_EQ(engine.GetPainter(), nullptr);
    
    ASSERT_TRUE(engine.Begin());
    
    engine.End();
}

TEST_F(QtEngineTest, MultipleBeginCalls) {
    RasterImageDevice device(100, 100);
    QtEngine engine(&device);
    
    EXPECT_TRUE(engine.Begin());
    EXPECT_TRUE(engine.Begin());
    
    engine.End();
}

TEST_F(QtEngineTest, LineStyles) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x1[] = {10, 10, 10};
    double y1[] = {10, 50, 90};
    double x2[] = {100, 100, 100};
    double y2[] = {10, 50, 90};
    
    DrawStyle style;
    style.lineColor = Color(0.0, 0.0, 0.0, 1.0);
    style.lineWidth = 2.0;
    
    style.lineStyle = LineStyle::kSolid;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.lineStyle = LineStyle::kDash;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.lineStyle = LineStyle::kDot;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    style.lineStyle = LineStyle::kDashDot;
    EXPECT_EQ(engine.DrawLines(x1, y1, x2, y2, 1, style), DrawResult::kSuccess);
    
    engine.End();
}

TEST_F(QtEngineTest, Colors) {
    RasterImageDevice device(200, 200);
    QtEngine engine(&device);
    
    ASSERT_TRUE(engine.Begin());
    
    double x[] = {50, 100, 150};
    double y[] = {50, 50, 50};
    
    DrawStyle style;
    
    style.lineColor = Color(1.0, 0.0, 0.0, 1.0);
    style.pointSize = 5.0;
    EXPECT_EQ(engine.DrawPoints(x, y, 1, style), DrawResult::kSuccess);
    
    style.lineColor = Color(0.0, 1.0, 0.0, 0.5);
    EXPECT_EQ(engine.DrawPoints(x + 1, y + 1, 1, style), DrawResult::kSuccess);
    
    style.lineColor = Color(0.0, 0.0, 1.0, 0.25);
    EXPECT_EQ(engine.DrawPoints(x + 2, y + 2, 1, style), DrawResult::kSuccess);
    
    engine.End();
}

#else

TEST(QtEngineNotAvailable, Skip) {
    GTEST_SKIP() << "Qt not available, skipping QtEngine tests";
}

#endif
