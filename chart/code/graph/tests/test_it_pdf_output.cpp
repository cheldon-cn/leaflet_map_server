#include <gtest/gtest.h>
#include <ogc/draw/pdf_device.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/font.h>
#include <ogc/draw/color.h>
#include "ogc/geom/envelope.h"
#include <memory>
#include <cstdio>

using namespace ogc::draw;
using ogc::Envelope;

class PdfOutputITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = PdfDevice::Create(800, 600);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    PdfDevicePtr m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(PdfOutputITTest, BasicPdfOutput) {
    m_device->SetTitle("Test PDF");
    m_device->SetAuthor("Test Author");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Black(), 1.0);
    m_context->SetStyle(style);
    
    m_context->DrawRect(100, 100, 200, 150);
    m_context->End();
    
    std::string filename = "test_basic_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithText) {
    m_device->SetTitle("Text PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    Font font("Arial", 14);
    Color color(0, 0, 0);
    m_context->DrawText(100, 100, "Hello PDF", font, color);
    m_context->DrawText(100, 150, "Test Text Line 2", font, color);
    
    m_context->End();
    
    std::string filename = "test_text_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithShapes) {
    m_device->SetTitle("Shapes PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Black(), 1.0);
    m_context->SetStyle(style);
    
    m_context->DrawPoint(100, 100);
    m_context->DrawLine(50, 50, 200, 200);
    m_context->DrawRect(250, 100, 150, 100);
    m_context->DrawCircle(500, 300, 50);
    
    m_context->End();
    
    std::string filename = "test_shapes_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithTransform) {
    m_device->SetTitle("Transform PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(100, 100);
    transform = transform.Rotate(45);
    m_context->SetTransform(transform);
    
    m_context->DrawRect(0, 0, 100, 50);
    
    m_context->ResetTransform();
    m_context->DrawRect(300, 300, 100, 50);
    
    m_context->End();
    
    std::string filename = "test_transform_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfMultiplePages) {
    m_device->SetTitle("Multi-Page PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    m_context->DrawText(100, 100, "Page 1", Font("Arial", 24), Color(0, 0, 0));
    m_context->End();
    
    m_device->NewPage(800, 600);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    m_context->DrawText(100, 100, "Page 2", Font("Arial", 24), Color(0, 0, 0));
    m_context->End();
    
    m_device->NewPage(800, 600);
    
    m_context->Begin();
    m_context->Clear(Color::White());
    m_context->DrawText(100, 100, "Page 3", Font("Arial", 24), Color(0, 0, 0));
    m_context->End();
    
    EXPECT_EQ(m_device->GetPageCount(), 3);
    
    std::string filename = "test_multipage_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithStyles) {
    m_device->SetTitle("Styles PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    DrawStyle redStroke;
    redStroke.pen = Pen(Color::Red(), 2.0);
    m_context->SetStyle(redStroke);
    m_context->DrawRect(50, 50, 100, 80);
    
    DrawStyle blueFill;
    blueFill.brush = Brush(Color::Blue());
    m_context->SetStyle(blueFill);
    m_context->DrawRect(200, 50, 100, 80);
    
    DrawStyle greenBoth;
    greenBoth.pen = Pen(Color::Green(), 3.0);
    greenBoth.brush = Brush(Color::Green());
    m_context->SetStyle(greenBoth);
    m_context->DrawRect(350, 50, 100, 80);
    
    m_context->End();
    
    std::string filename = "test_styles_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithOpacity) {
    m_device->SetTitle("Opacity PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    
    m_context->DrawRect(100, 100, 200, 200);
    m_context->DrawRect(200, 200, 200, 200);
    m_context->DrawRect(300, 300, 200, 200);
    
    m_context->End();
    
    std::string filename = "test_opacity_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfMetadata) {
    m_device->SetTitle("Metadata Test Title");
    m_device->SetAuthor("Test Author Name");
    m_device->SetSubject("Test Subject");
    m_device->SetCreator("Test Creator");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    m_context->DrawText(100, 300, "PDF with Metadata", Font("Arial", 20), Color(0, 0, 0));
    m_context->End();
    
    std::string filename = "test_metadata_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfClear) {
    m_device->SetTitle("Clear PDF Test");
    
    m_context->Begin();
    m_context->Clear(Color::White());
    m_context->DrawText(100, 300, "After Clear", Font("Arial", 16), Color(0, 0, 0));
    m_context->End();
    
    std::string filename = "test_clear_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}
