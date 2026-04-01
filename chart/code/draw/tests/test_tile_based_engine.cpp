#include <gtest/gtest.h>
#include "ogc/draw/tile_based_engine.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/geometry.h"
#include <cmath>
#include <cstring>

using namespace ogc::draw;

class TileBasedEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new TileDevice(1000, 1000, 256);
        device->Initialize();
        engine = new TileBasedEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    TileDevice* device;
    TileBasedEngine* engine;
};

TEST_F(TileBasedEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kTile);
    EXPECT_EQ(engine->GetName(), "TileBasedEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(TileBasedEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(TileBasedEngineTest, DrawPoints) {
    engine->Begin();
    
    double x[] = { 500.0 };
    double y[] = { 500.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 5;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawLines) {
    engine->Begin();
    
    double x1[] = { 100.0 };
    double y1[] = { 100.0 };
    double x2[] = { 900.0 };
    double y2[] = { 900.0 };
    
    DrawStyle style;
    style.pen.color = Color::Green();
    
    DrawResult result = engine->DrawLines(x1, y1, x2, y2, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawLineString) {
    engine->Begin();
    
    double x[] = { 100.0, 500.0, 900.0 };
    double y[] = { 500.0, 100.0, 500.0 };
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = engine->DrawLineString(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawPolygon) {
    engine->Begin();
    
    double x[] = { 500.0, 900.0, 500.0, 100.0 };
    double y[] = { 100.0, 500.0, 900.0, 500.0 };
    
    DrawStyle style;
    style.brush.color = Color::Red();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawRect) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    
    DrawResult result = engine->DrawRect(200, 200, 600, 600, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawCircle) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Green();
    
    DrawResult result = engine->DrawCircle(500, 500, 200, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawEllipse) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawEllipse(500, 500, 300, 150, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawArc) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    const double pi = 3.14159265358979323846;
    DrawResult result = engine->DrawArc(500, 500, 200, 200, 0, pi, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, TransformPoint) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(100, 100);
    engine->SetTransform(matrix);
    
    EXPECT_EQ(engine->GetTransform(), matrix);
}

TEST_F(TileBasedEngineTest, ResetTransform) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(50, 50);
    engine->SetTransform(matrix);
    engine->ResetTransform();
    
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
}

TEST_F(TileBasedEngineTest, ClipRect) {
    engine->Begin();
    
    engine->SetClipRect(200, 200, 600, 600);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    
    engine->DrawRect(0, 0, 1000, 1000, style, true);
}

TEST_F(TileBasedEngineTest, ResetClip) {
    engine->Begin();
    
    engine->SetClipRect(200, 200, 600, 600);
    engine->ResetClip();
    
    EXPECT_TRUE(engine->GetClipRegion().IsEmpty());
}

TEST_F(TileBasedEngineTest, Opacity) {
    engine->Begin();
    
    engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
    
    engine->SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
}

TEST_F(TileBasedEngineTest, Tolerance) {
    engine->Begin();
    
    engine->SetTolerance(0.5);
    EXPECT_DOUBLE_EQ(engine->GetTolerance(), 0.5);
}

TEST_F(TileBasedEngineTest, Antialiasing) {
    engine->Begin();
    
    engine->SetAntialiasingEnabled(false);
    EXPECT_FALSE(engine->IsAntialiasingEnabled());
    
    engine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(engine->IsAntialiasingEnabled());
}

TEST_F(TileBasedEngineTest, Clear) {
    engine->Begin();
    
    engine->Clear(Color::Blue());
}

TEST_F(TileBasedEngineTest, ProgressCallback) {
    engine->Begin();
    
    float lastProgress = -1.0f;
    engine->SetProgressCallback([&lastProgress](float progress) {
        lastProgress = progress;
    });
    
    DrawStyle style;
    style.brush.color = Color::Red();
    engine->DrawRect(0, 0, 100, 100, style, true);
    
    EXPECT_GE(engine->GetProgress(), 0.0f);
}

TEST_F(TileBasedEngineTest, DrawGeometryPoint) {
    engine->Begin();
    
    auto pt = ogc::Point::Create(500.0, 500.0);
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 5;
    
    DrawResult result = engine->DrawGeometry(pt.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawGeometryLine) {
    engine->Begin();
    
    ogc::CoordinateList coords;
    coords.emplace_back(100.0, 100.0);
    coords.emplace_back(900.0, 900.0);
    auto line = ogc::LineString::Create(coords);
    
    DrawStyle style;
    style.pen.color = Color::Green();
    
    DrawResult result = engine->DrawGeometry(line.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawGeometryPolygon) {
    engine->Begin();
    
    ogc::CoordinateList coords;
    coords.emplace_back(500.0, 100.0);
    coords.emplace_back(900.0, 500.0);
    coords.emplace_back(500.0, 900.0);
    coords.emplace_back(100.0, 500.0);
    coords.emplace_back(500.0, 100.0);
    auto ring = ogc::LinearRing::Create(coords);
    auto poly = ogc::Polygon::Create(std::move(ring));
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    
    DrawResult result = engine->DrawGeometry(poly.get(), style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawGeometryRect) {
    engine->Begin();
    
    RectGeometry rect(200, 200, 600, 600);
    DrawStyle style;
    style.brush.color = Color::Red();
    
    DrawResult result = engine->DrawGeometry(&rect, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawGeometryCircle) {
    engine->Begin();
    
    CircleGeometry circle(500, 500, 200);
    DrawStyle style;
    style.brush.color = Color::Green();
    
    DrawResult result = engine->DrawGeometry(&circle, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, MeasureText) {
    Font font("Arial", 12);
    TextMetrics metrics = engine->MeasureText("Hello", font);
    
    EXPECT_GT(metrics.width, 0);
    EXPECT_GT(metrics.height, 0);
}

TEST_F(TileBasedEngineTest, DrawImage) {
    engine->Begin();
    
    Image image(10, 10, 4);
    uint8_t* data = image.GetData();
    if (data) {
        memset(data, 255, 10 * 10 * 4);
    }
    
    DrawResult result = engine->DrawImage(100, 100, image);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, DrawImageRect) {
    engine->Begin();
    
    Image image(10, 10, 4);
    uint8_t* data = image.GetData();
    if (data) {
        memset(data, 255, 10 * 10 * 4);
    }
    
    DrawResult result = engine->DrawImageRect(100, 100, 50, 50, image);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(TileBasedEngineTest, InvalidState) {
    double x[] = { 500.0 };
    double y[] = { 500.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(TileBasedEngineTest, InvalidParameters) {
    engine->Begin();
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(500, 500, -1, style, true), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawEllipse(500, 500, -1, 10, style, false), DrawResult::kInvalidParameter);
}

