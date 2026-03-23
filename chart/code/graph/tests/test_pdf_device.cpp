#include <gtest/gtest.h>
#include "ogc/draw/pdf_device.h"
#include <memory>

using namespace ogc::draw;

class PdfDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = PdfDevice::Create(800, 600);
        ASSERT_NE(m_device, nullptr);
    }
    
    void TearDown() override {
        m_device.reset();
    }
    
    PdfDevicePtr m_device;
};

TEST_F(PdfDeviceTest, Create) {
    EXPECT_NE(m_device, nullptr);
}

TEST_F(PdfDeviceTest, GetType) {
    EXPECT_EQ(m_device->GetType(), DeviceType::kPdf);
}

TEST_F(PdfDeviceTest, GetName) {
    EXPECT_EQ(m_device->GetName(), "PdfDevice");
}

TEST_F(PdfDeviceTest, Initialize) {
    auto result = m_device->Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, Finalize) {
    m_device->Initialize();
    auto result = m_device->Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, IsReady) {
    m_device->Initialize();
    EXPECT_TRUE(m_device->IsReady());
}

TEST_F(PdfDeviceTest, GetWidthHeight) {
    EXPECT_EQ(m_device->GetWidth(), 800);
    EXPECT_EQ(m_device->GetHeight(), 600);
}

TEST_F(PdfDeviceTest, GetDpi) {
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 72.0);
}

TEST_F(PdfDeviceTest, SetDpi) {
    m_device->SetDpi(150.0);
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 150.0);
}

TEST_F(PdfDeviceTest, SetTitle) {
    m_device->SetTitle("Test PDF");
}

TEST_F(PdfDeviceTest, SetAuthor) {
    m_device->SetAuthor("Test Author");
}

TEST_F(PdfDeviceTest, SetSubject) {
    m_device->SetSubject("Test Subject");
}

TEST_F(PdfDeviceTest, SetCreator) {
    m_device->SetCreator("Test Creator");
}

TEST_F(PdfDeviceTest, BeginEndDraw) {
    m_device->Initialize();
    DrawParams params;
    auto result = m_device->BeginDraw(params);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = m_device->EndDraw();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(PdfDeviceTest, IsDrawing) {
    m_device->Initialize();
    EXPECT_FALSE(m_device->IsDrawing());
    
    DrawParams params;
    m_device->BeginDraw(params);
    EXPECT_TRUE(m_device->IsDrawing());
    
    m_device->EndDraw();
    EXPECT_FALSE(m_device->IsDrawing());
}

TEST_F(PdfDeviceTest, SetTransform) {
    TransformMatrix transform;
    transform.Translate(10, 20);
    m_device->SetTransform(transform);
    
    auto retrieved = m_device->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetM02(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetM12(), 20);
}

TEST_F(PdfDeviceTest, ResetTransform) {
    TransformMatrix transform;
    transform.Translate(10, 20);
    m_device->SetTransform(transform);
    m_device->ResetTransform();
    
    auto retrieved = m_device->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetM02(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetM12(), 0);
}

TEST_F(PdfDeviceTest, SetClipRect) {
    m_device->SetClipRect(10, 10, 100, 100);
    EXPECT_TRUE(m_device->HasClipRect());
    
    m_device->ClearClipRect();
    EXPECT_FALSE(m_device->HasClipRect());
}

TEST_F(PdfDeviceTest, SetAntialiasing) {
    m_device->SetAntialiasing(true);
    EXPECT_TRUE(m_device->IsAntialiasingEnabled());
    
    m_device->SetAntialiasing(false);
    EXPECT_FALSE(m_device->IsAntialiasingEnabled());
}

TEST_F(PdfDeviceTest, SetOpacity) {
    m_device->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_device->GetOpacity(), 0.5);
}

TEST_F(PdfDeviceTest, DrawPoint) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawPoint(400, 300, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, DrawLine) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawLine(0, 0, 800, 600, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, DrawRect) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawRect(100, 100, 200, 150, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, DrawCircle) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawCircle(400, 300, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, DrawEllipse) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawEllipse(400, 300, 100, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, DrawText) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    Font font("Arial", 12);
    Color color(0, 0, 0);
    auto result = m_device->DrawText(100, 100, "Hello World", font, color);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, Clear) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    Color color(255, 255, 255);
    m_device->Clear(color);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, Fill) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    Color color(0, 0, 255);
    m_device->Fill(color);
    
    m_device->EndDraw();
}

TEST_F(PdfDeviceTest, NewPage) {
    m_device->Initialize();
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetPageCount(), 2);
}

TEST_F(PdfDeviceTest, GetCurrentPage) {
    m_device->Initialize();
    EXPECT_EQ(m_device->GetCurrentPage(), 0);
    
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetCurrentPage(), 1);
}

TEST_F(PdfDeviceTest, GetPageCount) {
    m_device->Initialize();
    EXPECT_EQ(m_device->GetPageCount(), 1);
    
    m_device->NewPage(800, 600);
    EXPECT_EQ(m_device->GetPageCount(), 2);
}

TEST_F(PdfDeviceTest, SaveToFile) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    m_device->DrawLine(0, 0, 800, 600, style);
    
    m_device->EndDraw();
    
    bool result = m_device->SaveToFile("test_output.pdf");
    EXPECT_TRUE(result);
}

TEST_F(PdfDeviceTest, GetState) {
    auto state = m_device->GetState();
    EXPECT_NE(state, DeviceState::kError);
}
