#include <gtest/gtest.h>
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/geometry.h"
#include <cmath>
#include <cstring>

using namespace ogc::draw;

class Simple2DEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100, PixelFormat::kRGBA8888);
        engine = new Simple2DEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    RasterImageDevice* device;
    Simple2DEngine* engine;
};

TEST_F(Simple2DEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kSimple2D);
    EXPECT_EQ(engine->GetName(), "Simple2DEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(Simple2DEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(Simple2DEngineTest, DrawPoints) {
    engine->Begin();
    
    double x[] = { 50.0, 25.0, 75.0 };
    double y[] = { 50.0, 25.0, 75.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 3;
    
    DrawResult result = engine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    Color centerPixel = device->GetPixel(50, 50);
    EXPECT_GT(centerPixel.GetRed(), 0);
}

TEST_F(Simple2DEngineTest, DrawLines) {
    engine->Begin();
    
    double x1[] = { 10.0, 20.0 };
    double y1[] = { 10.0, 20.0 };
    double x2[] = { 90.0, 80.0 };
    double y2[] = { 90.0, 80.0 };
    
    DrawStyle style;
    style.pen.color = Color::Green();
    style.pen.width = 1;
    
    DrawResult result = engine->DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawLineString) {
    engine->Begin();
    
    double x[] = { 10.0, 50.0, 90.0 };
    double y[] = { 50.0, 10.0, 50.0 };
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = engine->DrawLineString(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawPolygon) {
    engine->Begin();
    
    double x[] = { 50.0, 90.0, 50.0, 10.0 };
    double y[] = { 10.0, 50.0, 90.0, 50.0 };
    
    DrawStyle style;
    style.brush.color = Color::Red();
    style.pen.color = Color::Black();
    style.pen.width = 1;
    
    DrawResult result = engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    Color centerPixel = device->GetPixel(50, 50);
    EXPECT_GT(centerPixel.GetRed(), 0);
}

TEST_F(Simple2DEngineTest, DrawRect) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawRect(25, 25, 50, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    Color centerPixel = device->GetPixel(50, 50);
    EXPECT_GT(centerPixel.GetBlue(), 0);
}

TEST_F(Simple2DEngineTest, DrawCircle) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Green();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawCircle(50, 50, 25, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    Color centerPixel = device->GetPixel(50, 50);
    EXPECT_GT(centerPixel.GetGreen(), 0);
}

TEST_F(Simple2DEngineTest, DrawEllipse) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawEllipse(50, 50, 40, 20, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawArc) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    const double pi = 3.14159265358979323846;
    DrawResult result = engine->DrawArc(50, 50, 30, 30, 0, pi, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, TransformPoint) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    engine->SetTransform(matrix);
    
    double x[] = { 0.0 };
    double y[] = { 0.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 5;
    
    engine->DrawPoints(x, y, 1, style);
    
    Color pixel = device->GetPixel(10, 20);
    EXPECT_GT(pixel.GetRed(), 0);
}

TEST_F(Simple2DEngineTest, ResetTransform) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(50, 50);
    engine->SetTransform(matrix);
    engine->ResetTransform();
    
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
}

TEST_F(Simple2DEngineTest, ClipRect) {
    engine->Begin();
    
    engine->SetClipRect(25, 25, 50, 50);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    
    engine->DrawRect(0, 0, 100, 100, style, true);
    
    Color outsidePixel = device->GetPixel(10, 10);
    EXPECT_EQ(outsidePixel.GetAlpha(), 0);
    
    Color insidePixel = device->GetPixel(50, 50);
    EXPECT_GT(insidePixel.GetRed(), 0);
}

TEST_F(Simple2DEngineTest, ResetClip) {
    engine->Begin();
    
    engine->SetClipRect(25, 25, 50, 50);
    engine->ResetClip();
    
    EXPECT_TRUE(engine->GetClipRegion().IsEmpty());
}

TEST_F(Simple2DEngineTest, SaveRestore) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 10);
    engine->SetTransform(matrix);
    engine->SetOpacity(0.5);
    
    engine->Save();
    
    engine->SetTransform(TransformMatrix::Translate(20, 20));
    engine->SetOpacity(0.8);
    
    engine->Restore();
    
    EXPECT_EQ(engine->GetTransform(), matrix);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
}

TEST_F(Simple2DEngineTest, Opacity) {
    engine->Begin();
    
    engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
    
    engine->SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
}

TEST_F(Simple2DEngineTest, Tolerance) {
    engine->Begin();
    
    engine->SetTolerance(0.5);
    EXPECT_DOUBLE_EQ(engine->GetTolerance(), 0.5);
}

TEST_F(Simple2DEngineTest, Antialiasing) {
    engine->Begin();
    
    engine->SetAntialiasingEnabled(false);
    EXPECT_FALSE(engine->IsAntialiasingEnabled());
    
    engine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(engine->IsAntialiasingEnabled());
}

TEST_F(Simple2DEngineTest, Clear) {
    engine->Begin();
    
    engine->Clear(Color::Blue());
    
    Color pixel = device->GetPixel(50, 50);
    EXPECT_GT(pixel.GetBlue(), 0);
}

TEST_F(Simple2DEngineTest, DrawGeometryPoint) {
    engine->Begin();
    
    auto pt = ogc::Point::Create(50.0, 50.0);
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 3;
    
    DrawResult result = engine->DrawGeometry(pt.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawGeometryLine) {
    engine->Begin();
    
    ogc::CoordinateList coords;
    coords.emplace_back(10.0, 10.0);
    coords.emplace_back(90.0, 90.0);
    auto line = ogc::LineString::Create(coords);
    
    DrawStyle style;
    style.pen.color = Color::Green();
    
    DrawResult result = engine->DrawGeometry(line.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawGeometryPolygon) {
    engine->Begin();
    
    ogc::CoordinateList coords;
    coords.emplace_back(50.0, 10.0);
    coords.emplace_back(90.0, 50.0);
    coords.emplace_back(50.0, 90.0);
    coords.emplace_back(10.0, 50.0);
    coords.emplace_back(50.0, 10.0);
    auto ring = ogc::LinearRing::Create(coords);
    auto poly = ogc::Polygon::Create(std::move(ring));
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    
    DrawResult result = engine->DrawGeometry(poly.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawGeometryRect) {
    engine->Begin();
    
    RectGeometry rect(25, 25, 50, 50);
    DrawStyle style;
    style.brush.color = Color::Red();
    
    DrawResult result = engine->DrawGeometry(&rect, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawGeometryCircle) {
    engine->Begin();
    
    CircleGeometry circle(50, 50, 25);
    DrawStyle style;
    style.brush.color = Color::Green();
    
    DrawResult result = engine->DrawGeometry(&circle, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, MeasureText) {
    Font font("Arial", 12);
    TextMetrics metrics = engine->MeasureText("Hello", font);
    
    EXPECT_GT(metrics.width, 0);
    EXPECT_GT(metrics.height, 0);
}

TEST_F(Simple2DEngineTest, DrawImage) {
    engine->Begin();
    
    Image image(10, 10, 4);
    uint8_t* data = image.GetData();
    if (data) {
        memset(data, 255, 10 * 10 * 4);
    }
    
    DrawResult result = engine->DrawImage(10, 10, image);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, DrawImageRect) {
    engine->Begin();
    
    Image image(10, 10, 4);
    uint8_t* data = image.GetData();
    if (data) {
        memset(data, 255, 10 * 10 * 4);
    }
    
    DrawResult result = engine->DrawImageRect(10, 10, 20, 20, image);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(Simple2DEngineTest, InvalidState) {
    double x[] = { 50.0 };
    double y[] = { 50.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(Simple2DEngineTest, InvalidParameters) {
    engine->Begin();
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(50, 50, -1, style, true), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawEllipse(50, 50, -1, 10, style, false), DrawResult::kInvalidParameter);
}

