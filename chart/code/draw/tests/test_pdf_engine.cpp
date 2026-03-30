#include <gtest/gtest.h>
#include "ogc/draw/pdf_engine.h"
#include "ogc/draw/pdf_device.h"
#include <memory>

using namespace ogc::draw;

class PdfEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = PdfDevice::Create(800, 600);
        ASSERT_NE(m_device, nullptr);
        m_engine = std::unique_ptr<PdfEngine>(new PdfEngine(m_device.get()));
    }
    
    void TearDown() override {
        m_engine.reset();
        m_device.reset();
    }
    
    PdfDevicePtr m_device;
    std::unique_ptr<PdfEngine> m_engine;
};

TEST_F(PdfEngineTest, Create) {
    EXPECT_NE(m_engine, nullptr);
}

TEST_F(PdfEngineTest, GetName) {
    EXPECT_EQ(m_engine->GetName(), "PdfEngine");
}

TEST_F(PdfEngineTest, GetType) {
    EXPECT_EQ(m_engine->GetType(), EngineType::kVector);
}

TEST_F(PdfEngineTest, GetDevice) {
    EXPECT_EQ(m_engine->GetDevice(), m_device.get());
}

TEST_F(PdfEngineTest, BeginEnd) {
    auto result = m_engine->Begin();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(m_engine->IsActive());
    
    m_engine->End();
    EXPECT_FALSE(m_engine->IsActive());
}

TEST_F(PdfEngineTest, SetTransform) {
    m_engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_engine->SetTransform(matrix);
    
    auto retrieved = m_engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 20);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, ResetTransform) {
    m_engine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_engine->SetTransform(matrix);
    m_engine->ResetTransform();
    
    auto retrieved = m_engine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 0);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 0);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, SetOpacity) {
    m_engine->Begin();
    m_engine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 0.5);
    m_engine->End();
}

TEST_F(PdfEngineTest, SetTolerance) {
    m_engine->Begin();
    m_engine->SetTolerance(0.01);
    EXPECT_DOUBLE_EQ(m_engine->GetTolerance(), 0.01);
    m_engine->End();
}

TEST_F(PdfEngineTest, SetAntialiasing) {
    m_engine->Begin();
    m_engine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(m_engine->IsAntialiasingEnabled());
    
    m_engine->SetAntialiasingEnabled(false);
    EXPECT_FALSE(m_engine->IsAntialiasingEnabled());
    m_engine->End();
}

TEST_F(PdfEngineTest, SaveRestore) {
    m_engine->Begin();
    
    m_engine->SetOpacity(0.5);
    m_engine->Save();
    
    m_engine->SetOpacity(1.0);
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 1.0);
    
    m_engine->Restore();
    EXPECT_DOUBLE_EQ(m_engine->GetOpacity(), 0.5);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, AddLink) {
    m_engine->Begin();
    m_engine->AddLink(10, 10, 100, 50, "https://example.com");
    m_engine->End();
}

TEST_F(PdfEngineTest, AddBookmark) {
    m_engine->Begin();
    m_engine->AddBookmark("Chapter 1", 0);
    m_engine->AddBookmark("Section 1.1", 1);
    m_engine->End();
}

TEST_F(PdfEngineTest, SetLayer) {
    m_engine->Begin();
    m_engine->SetLayer("Background");
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawPoints) {
    m_engine->Begin();
    
    double x[] = {100, 200, 300};
    double y[] = {100, 200, 300};
    DrawStyle style;
    
    auto result = m_engine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawLines) {
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

TEST_F(PdfEngineTest, DrawLineString) {
    m_engine->Begin();
    
    double x[] = {0, 100, 200, 300};
    double y[] = {0, 100, 50, 150};
    DrawStyle style;
    
    auto result = m_engine->DrawLineString(x, y, 4, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawPolygon) {
    m_engine->Begin();
    
    double x[] = {100, 200, 200, 100};
    double y[] = {100, 100, 200, 200};
    DrawStyle style;
    
    auto result = m_engine->DrawPolygon(x, y, 4, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawRect) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawRect(50, 50, 100, 80, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawCircle) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawCircle(400, 300, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawEllipse) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawEllipse(400, 300, 100, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, DrawArc) {
    m_engine->Begin();
    
    DrawStyle style;
    auto result = m_engine->DrawArc(400, 300, 100, 50, 0, 3.14159, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, MeasureText) {
    m_engine->Begin();
    
    Font font("Arial", 12);
    auto metrics = m_engine->MeasureText("Hello World", font);
    
    EXPECT_GE(metrics.width, 0);
    EXPECT_GE(metrics.height, 0);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, Clear) {
    m_engine->Begin();
    
    Color color(255, 255, 255);
    m_engine->Clear(color);
    
    m_engine->End();
}

TEST_F(PdfEngineTest, Flush) {
    m_engine->Begin();
    m_engine->Flush();
    m_engine->End();
}
