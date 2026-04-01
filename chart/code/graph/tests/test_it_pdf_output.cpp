#include <gtest/gtest.h>
#include <ogc/draw/pdf_device.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/font.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
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
        
        m_context = DrawContext::Create(m_device);
        ASSERT_NE(m_context, nullptr);
        m_context->Initialize();
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    PdfDevicePtr m_device;
    DrawContextPtr m_context;
};

TEST_F(PdfOutputITTest, BasicPdfOutput) {
    m_device->SetTitle("Test PDF");
    m_device->SetAuthor("Test Author");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    m_context->DrawRect(100, 100, 200, 150);
    m_context->EndDraw();
    
    std::string filename = "test_basic_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithText) {
    m_device->SetTitle("Text PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    Font font("Arial", 14);
    Color color(0, 0, 0);
    m_context->DrawText(100, 100, "Hello PDF", font, color);
    m_context->DrawText(100, 150, "Test Text Line 2", font, color);
    
    m_context->EndDraw();
    
    std::string filename = "test_text_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithShapes) {
    m_device->SetTitle("Shapes PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    m_context->DrawPoint(100, 100);
    m_context->DrawLine(50, 50, 200, 200);
    m_context->DrawRect(250, 100, 150, 100);
    m_context->DrawCircle(500, 300, 50);
    
    m_context->EndDraw();
    
    std::string filename = "test_shapes_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithTransform) {
    m_device->SetTitle("Transform PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    TransformMatrix transform;
    transform.Translate(100, 100);
    transform.Rotate(45);
    m_context->SetTransform(transform);
    
    m_context->DrawRect(0, 0, 100, 50);
    
    m_context->ResetTransform();
    m_context->DrawRect(300, 300, 100, 50);
    
    m_context->EndDraw();
    
    std::string filename = "test_transform_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfMultiplePages) {
    m_device->SetTitle("Multi-Page PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    m_context->DrawText(100, 100, "Page 1", Font("Arial", 24), Color(0, 0, 0));
    m_context->EndDraw();
    
    m_device->NewPage(800, 600);
    
    m_context->BeginDraw(params);
    m_context->DrawText(100, 100, "Page 2", Font("Arial", 24), Color(0, 0, 0));
    m_context->EndDraw();
    
    m_device->NewPage(800, 600);
    
    m_context->BeginDraw(params);
    m_context->DrawText(100, 100, "Page 3", Font("Arial", 24), Color(0, 0, 0));
    m_context->EndDraw();
    
    EXPECT_EQ(m_device->GetPageCount(), 3);
    
    std::string filename = "test_multipage_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithStyles) {
    m_device->SetTitle("Styles PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    DrawStyle redStroke;
    redStroke.pen.color = 0xFF0000;
    redStroke.pen.width = 2.0;
    m_context->DrawRect(50, 50, 100, 80);
    
    DrawStyle blueFill;
    blueFill.brush.color = 0x0000FF;
    m_context->DrawRect(200, 50, 100, 80);
    
    DrawStyle greenBoth;
    greenBoth.pen.color = 0x00FF00;
    greenBoth.pen.width = 3.0;
    greenBoth.brush.color = 0x00FF00;
    m_context->DrawRect(350, 50, 100, 80);
    
    m_context->EndDraw();
    
    std::string filename = "test_styles_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfWithOpacity) {
    m_device->SetTitle("Opacity PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    m_device->SetOpacity(1.0);
    m_context->DrawRect(100, 100, 200, 200);
    
    m_device->SetOpacity(0.5);
    m_context->DrawRect(200, 200, 200, 200);
    
    m_device->SetOpacity(0.25);
    m_context->DrawRect(300, 300, 200, 200);
    
    m_context->EndDraw();
    
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
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    m_context->DrawText(100, 300, "PDF with Metadata", Font("Arial", 20), Color(0, 0, 0));
    m_context->EndDraw();
    
    std::string filename = "test_metadata_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

TEST_F(PdfOutputITTest, PdfClear) {
    m_device->SetTitle("Clear PDF Test");
    
    DrawParams params;
    params.SetSize(800, 600);
    
    m_context->BeginDraw(params);
    
    Color white(255, 255, 255);
    m_context->Clear(white);
    
    m_context->DrawText(100, 300, "After Clear", Font("Arial", 16), Color(0, 0, 0));
    
    m_context->EndDraw();
    
    std::string filename = "test_clear_output.pdf";
    bool result = m_device->SaveToFile(filename);
    EXPECT_TRUE(result);
    
    std::remove(filename.c_str());
}

