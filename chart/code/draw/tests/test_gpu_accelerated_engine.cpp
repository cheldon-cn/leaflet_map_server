#include <gtest/gtest.h>
#include "ogc/draw/gpu_accelerated_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <memory>

using namespace ogc::draw;

class GPUAcceleratedEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::unique_ptr<RasterImageDevice>(new RasterImageDevice(800, 600));
        m_engine = std::unique_ptr<GPUAcceleratedEngine>(new GPUAcceleratedEngine(m_device.get()));
    }
    
    void TearDown() override {
        m_engine.reset();
        m_device.reset();
    }
    
    std::unique_ptr<RasterImageDevice> m_device;
    std::unique_ptr<GPUAcceleratedEngine> m_engine;
};

TEST_F(GPUAcceleratedEngineTest, Create) {
    EXPECT_NE(m_engine, nullptr);
}

TEST_F(GPUAcceleratedEngineTest, GetName) {
    EXPECT_EQ(m_engine->GetName(), "GPUAcceleratedEngine");
}

TEST_F(GPUAcceleratedEngineTest, GetType) {
    EXPECT_EQ(m_engine->GetType(), EngineType::kGPU);
}

TEST_F(GPUAcceleratedEngineTest, GetDevice) {
    EXPECT_EQ(m_engine->GetDevice(), m_device.get());
}

TEST_F(GPUAcceleratedEngineTest, BeginEnd) {
    auto result = m_engine->Begin();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(m_engine->IsActive());
    
    m_engine->End();
    EXPECT_FALSE(m_engine->IsActive());
}

TEST_F(GPUAcceleratedEngineTest, SetTransform) {
    m_engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_engine->SetTransform(matrix);
    
    auto retrieved = m_engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 20);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, ResetTransform) {
    m_engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_engine->SetTransform(matrix);
    m_engine->ResetTransform();
    
    auto retrieved = m_engine->GetTransform();
    EXPECT_TRUE(retrieved.IsIdentity());
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SetOpacity) {
    m_engine->Begin();
    m_engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 0.5);
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SetTolerance) {
    m_engine->Begin();
    m_engine->SetTolerance(0.01);
    EXPECT_DOUBLE_EQ(m_engine->GetTolerance(), 0.01);
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SetAntialiasing) {
    m_engine->Begin();
    m_engine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(m_engine->IsAntialiasingEnabled());
    
    m_engine->SetAntialiasingEnabled(false);
    EXPECT_FALSE(m_engine->IsAntialiasingEnabled());
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SaveRestore) {
    m_engine->Begin();
    
    m_engine->SetOpacity(0.5);
    m_engine->Save();
    
    m_engine->SetOpacity(1.0);
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 1.0);
    
    m_engine->Restore();
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 0.5);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SetClipRect) {
    m_engine->Begin();
    m_engine->SetClipRect(10, 20, 100, 50);
    
    auto clip = m_engine->GetClipRegion();
    EXPECT_FALSE(clip.IsEmpty());
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, ResetClip) {
    m_engine->Begin();
    m_engine->SetClipRect(10, 20, 100, 50);
    m_engine->ResetClip();
    
    auto clip = m_engine->GetClipRegion();
    EXPECT_TRUE(clip.IsEmpty());
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawPoints) {
    m_engine->Begin();
    
    double x[] = {100, 200, 300};
    double y[] = {100, 200, 300};
    DrawStyle style;
    
    auto result = m_engine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawLines) {
    m_engine->Begin();
    
    double x1[] = {0, 100};
    double y1[] = {0, 100};
    double x2[] = {100, 200};
    double y2[] = {100, 200};
    DrawStyle style;
    
    auto result = m_engine->DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawLineString) {
    m_engine->Begin();
    
    double x[] = {0, 100, 200, 300};
    double y[] = {0, 100, 50, 150};
    DrawStyle style;
    
    auto result = m_engine->DrawLineString(x, y, 4, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawPolygon) {
    m_engine->Begin();
    
    double x[] = {100, 200, 200, 100};
    double y[] = {100, 100, 200, 200};
    DrawStyle style;
    
    auto result = m_engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawRect) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawRect(50, 50, 100, 80, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawCircle) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawCircle(400, 300, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawEllipse) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawEllipse(400, 300, 100, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawArc) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawArc(400, 300, 100, 50, 0, 3.14159, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, MeasureText) {
    m_engine->Begin();
    
    Font font("Arial", 12);
    auto metrics = m_engine->MeasureText("Hello World", font);
    
    EXPECT_GE(metrics.width, 0);
    EXPECT_GE(metrics.height, 0);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, Clear) {
    m_engine->Begin();
    
    Color color(255, 255, 255);
    m_engine->Clear(color);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, Flush) {
    m_engine->Begin();
    m_engine->Flush();
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, SupportsFeature) {
    EXPECT_TRUE(m_engine->SupportsFeature("gpu_acceleration"));
    EXPECT_TRUE(m_engine->SupportsFeature("batch_rendering"));
    EXPECT_TRUE(m_engine->SupportsFeature("shader_programs"));
    EXPECT_FALSE(m_engine->SupportsFeature("nonexistent"));
}

TEST_F(GPUAcceleratedEngineTest, DrawTextNotImplemented) {
    m_engine->Begin();
    
    Font font("Arial", 12);
    Color color(0, 0, 0);
    auto result = m_engine->DrawText(100, 100, "Test", font, color);
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, DrawImageNotImplemented) {
    m_engine->Begin();
    
    Image img(100, 100, 4);
    auto result = m_engine->DrawImage(100, 100, img);
    EXPECT_EQ(result, DrawResult::kNotImplemented);
    
    m_engine->End();
}

TEST_F(GPUAcceleratedEngineTest, InvalidState) {
    double x[] = {100};
    double y[] = {100};
    DrawStyle style;
    
    auto result = m_engine->DrawPoints(x, y, 1, style);
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(GPUAcceleratedEngineTest, InvalidParameters) {
    m_engine->Begin();
    
    DrawStyle style;
    
    auto result = m_engine->DrawPoints(nullptr, nullptr, 0, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    result = m_engine->DrawCircle(0, 0, -1, style);
    EXPECT_EQ(result, DrawResult::kInvalidParameter);
    
    m_engine->End();
}
