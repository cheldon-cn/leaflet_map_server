#include <gtest/gtest.h>
#include "ogc/draw/svg_device.h"
#include "ogc/draw/svg_engine.h"
#include "ogc/draw/geometry.h"
#include <fstream>

using namespace ogc::draw;

class SvgDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new SvgDevice("test_output.svg", 800, 600);
    }

    void TearDown() override {
        delete device;
    }

    SvgDevice* device;
};

TEST_F(SvgDeviceTest, Construction) {
    EXPECT_EQ(device->GetType(), DeviceType::kSvg);
    EXPECT_EQ(device->GetWidth(), 800);
    EXPECT_EQ(device->GetHeight(), 600);
    EXPECT_DOUBLE_EQ(device->GetDpi(), 96.0);
}

TEST_F(SvgDeviceTest, Initialize) {
    EXPECT_EQ(device->Initialize(), DrawResult::kSuccess);
    EXPECT_TRUE(device->IsInitialized());
}

TEST_F(SvgDeviceTest, SupportedEngines) {
    auto engines = device->GetSupportedEngineTypes();
    EXPECT_EQ(engines.size(), 1);
    EXPECT_EQ(engines[0], EngineType::kVector);
    EXPECT_EQ(device->GetPreferredEngineType(), EngineType::kVector);
}

TEST_F(SvgDeviceTest, TitleAndDescription) {
    device->SetTitle("Test SVG");
    device->SetDescription("Test Description");
    device->Initialize();
    
    std::string content = device->GetSvgContent();
    EXPECT_TRUE(content.find("<title>Test SVG</title>") != std::string::npos);
    EXPECT_TRUE(content.find("<desc>Test Description</desc>") != std::string::npos);
}

TEST_F(SvgDeviceTest, Groups) {
    device->Initialize();
    device->BeginGroup("layer1");
    device->EndGroup();
    
    std::string content = device->GetSvgContent();
    EXPECT_TRUE(content.find("<g id=\"layer1\">") != std::string::npos);
    EXPECT_TRUE(content.find("</g>") != std::string::npos);
}

class SvgEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new SvgDevice("test_engine_output.svg", 800, 600);
        device->Initialize();
        engine = new SvgEngine(device);
    }

    void TearDown() override {
        delete engine;
        delete device;
    }

    SvgDevice* device;
    SvgEngine* engine;
};

TEST_F(SvgEngineTest, Construction) {
    EXPECT_EQ(engine->GetType(), EngineType::kVector);
    EXPECT_EQ(engine->GetName(), "SvgEngine");
    EXPECT_EQ(engine->GetDevice(), device);
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(SvgEngineTest, BeginEnd) {
    EXPECT_EQ(engine->Begin(), DrawResult::kSuccess);
    EXPECT_TRUE(engine->IsActive());
    
    engine->End();
    EXPECT_FALSE(engine->IsActive());
}

TEST_F(SvgEngineTest, DrawPoints) {
    engine->Begin();
    
    double x[] = { 400.0 };
    double y[] = { 300.0 };
    
    DrawStyle style;
    style.pen.color = Color::Red();
    style.pen.width = 5;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawLines) {
    engine->Begin();
    
    double x1[] = { 100.0 };
    double y1[] = { 100.0 };
    double x2[] = { 700.0 };
    double y2[] = { 500.0 };
    
    DrawStyle style;
    style.pen.color = Color::Blue();
    
    DrawResult result = engine->DrawLines(x1, y1, x2, y2, 1, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawLineString) {
    engine->Begin();
    
    double x[] = { 100.0, 400.0, 700.0 };
    double y[] = { 300.0, 100.0, 300.0 };
    
    DrawStyle style;
    style.pen.color = Color::Green();
    
    DrawResult result = engine->DrawLineString(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawPolygon) {
    engine->Begin();
    
    double x[] = { 400.0, 700.0, 400.0, 100.0 };
    double y[] = { 100.0, 300.0, 500.0, 300.0 };
    
    DrawStyle style;
    style.brush.color = Color::Red();
    style.pen.color = Color::Black();
    
    DrawResult result = engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawRect) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Blue();
    
    DrawResult result = engine->DrawRect(100, 100, 600, 400, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawCircle) {
    engine->Begin();
    
    DrawStyle style;
    style.brush.color = Color::Green();
    
    DrawResult result = engine->DrawCircle(400, 300, 150, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawEllipse) {
    engine->Begin();
    
    DrawStyle style;
    style.pen.color = Color::Red();
    
    DrawResult result = engine->DrawEllipse(400, 300, 200, 100, style, false);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, DrawText) {
    engine->Begin();
    
    Font font("Arial", 24);
    Color color = Color::Black();
    
    DrawResult result = engine->DrawText(400, 300, "Hello SVG", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, Transform) {
    engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(100, 100);
    engine->SetTransform(matrix);
    
    EXPECT_EQ(engine->GetTransform(), matrix);
    
    engine->ResetTransform();
    EXPECT_EQ(engine->GetTransform(), TransformMatrix::Identity());
}

TEST_F(SvgEngineTest, Opacity) {
    engine->Begin();
    
    engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
    
    engine->SetOpacity(1.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 1.0);
    
    engine->SetOpacity(-0.5);
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.0);
}

TEST_F(SvgEngineTest, ClipRect) {
    engine->Begin();
    
    engine->SetClipRect(100, 100, 600, 400);
    
    DrawStyle style;
    style.brush.color = Color::Red();
    
    engine->DrawRect(0, 0, 800, 600, style, true);
}

TEST_F(SvgEngineTest, SaveRestore) {
    engine->Begin();
    
    engine->SetOpacity(0.5);
    engine->Save();
    
    engine->SetOpacity(1.0);
    engine->Restore();
    
    EXPECT_DOUBLE_EQ(engine->GetOpacity(), 0.5);
}

TEST_F(SvgEngineTest, MeasureText) {
    Font font("Arial", 12);
    TextMetrics metrics = engine->MeasureText("Hello", font);
    
    EXPECT_GT(metrics.width, 0);
    EXPECT_GT(metrics.height, 0);
}

TEST_F(SvgEngineTest, DrawGeometry) {
    engine->Begin();
    
    RectGeometry rect(100, 100, 200, 100);
    DrawStyle style;
    style.brush.color = Color::Blue();
    
    DrawResult result = engine->DrawGeometry(&rect, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(SvgEngineTest, InvalidState) {
    double x[] = { 400.0 };
    double y[] = { 300.0 };
    DrawStyle style;
    
    DrawResult result = engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(SvgEngineTest, InvalidParameters) {
    engine->Begin();
    
    DrawStyle style;
    
    EXPECT_EQ(engine->DrawPoints(nullptr, nullptr, 0, style), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawCircle(400, 300, -1, style, true), DrawResult::kInvalidParameter);
    EXPECT_EQ(engine->DrawEllipse(400, 300, -1, 10, style, false), DrawResult::kInvalidParameter);
}
