#include <gtest/gtest.h>
#include <ogc/symbology/symbolizer/icon_symbolizer.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include "ogc/common.h"
#include <memory>

using namespace ogc::symbology;
using namespace ogc;
using ogc::Envelope;

class IntegrationIconSymbolizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        
        DrawResult result = m_device->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
        
        m_symbolizer = IconSymbolizer::Create();
        ASSERT_NE(m_symbolizer, nullptr);
    }
    
    void TearDown() override {
        m_symbolizer.reset();
        m_context.reset();
        if (m_device) {
            m_device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
    std::shared_ptr<IconSymbolizer> m_symbolizer;
};

TEST_F(IntegrationIconSymbolizerTest, SymbolizerCreation) {
    EXPECT_NE(m_symbolizer, nullptr);
    EXPECT_EQ(m_symbolizer->GetType(), SymbolizerType::kIcon);
    EXPECT_EQ(m_symbolizer->GetName(), "IconSymbolizer");
}

TEST_F(IntegrationIconSymbolizerTest, SetIconPath) {
    m_symbolizer->SetIconPath("test_icon.png");
    EXPECT_EQ(m_symbolizer->GetIconPath(), "test_icon.png");
}

TEST_F(IntegrationIconSymbolizerTest, SetSize) {
    m_symbolizer->SetSize(32.0, 48.0);
    
    double width, height;
    m_symbolizer->GetSize(width, height);
    EXPECT_DOUBLE_EQ(width, 32.0);
    EXPECT_DOUBLE_EQ(height, 48.0);
}

TEST_F(IntegrationIconSymbolizerTest, SetWidth) {
    m_symbolizer->SetWidth(24.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetWidth(), 24.0);
}

TEST_F(IntegrationIconSymbolizerTest, SetHeight) {
    m_symbolizer->SetHeight(36.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetHeight(), 36.0);
}

TEST_F(IntegrationIconSymbolizerTest, SetOpacity) {
    m_symbolizer->SetOpacity(0.75);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetOpacity(), 0.75);
}

TEST_F(IntegrationIconSymbolizerTest, SetRotation) {
    m_symbolizer->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(m_symbolizer->GetRotation(), 45.0);
}

TEST_F(IntegrationIconSymbolizerTest, SetAnchorPoint) {
    m_symbolizer->SetAnchorPoint(0.5, 0.5);
    
    double x, y;
    m_symbolizer->GetAnchorPoint(x, y);
    EXPECT_DOUBLE_EQ(x, 0.5);
    EXPECT_DOUBLE_EQ(y, 0.5);
}

TEST_F(IntegrationIconSymbolizerTest, SetDisplacement) {
    m_symbolizer->SetDisplacement(10.0, 20.0);
    
    double dx, dy;
    m_symbolizer->GetDisplacement(dx, dy);
    EXPECT_DOUBLE_EQ(dx, 10.0);
    EXPECT_DOUBLE_EQ(dy, 20.0);
}

TEST_F(IntegrationIconSymbolizerTest, SetAllowOverlap) {
    m_symbolizer->SetAllowOverlap(true);
    EXPECT_TRUE(m_symbolizer->GetAllowOverlap());
    
    m_symbolizer->SetAllowOverlap(false);
    EXPECT_FALSE(m_symbolizer->GetAllowOverlap());
}

TEST_F(IntegrationIconSymbolizerTest, SetColorReplacement) {
    m_symbolizer->SetColorReplacement(Color::Red().GetRGBA());
    EXPECT_EQ(m_symbolizer->GetColorReplacement(), Color::Red().GetRGBA());
    EXPECT_TRUE(m_symbolizer->HasColorReplacement());
}

TEST_F(IntegrationIconSymbolizerTest, ClearColorReplacement) {
    m_symbolizer->SetColorReplacement(Color::Red().GetRGBA());
    EXPECT_TRUE(m_symbolizer->HasColorReplacement());
    
    m_symbolizer->ClearColorReplacement();
    EXPECT_FALSE(m_symbolizer->HasColorReplacement());
}

TEST_F(IntegrationIconSymbolizerTest, SetIconData) {
    std::vector<uint8_t> data(32 * 32 * 4, 255);
    m_symbolizer->SetIconData(data.data(), data.size(), 32, 32, 4);
    
    EXPECT_TRUE(m_symbolizer->HasIconData());
}

TEST_F(IntegrationIconSymbolizerTest, CanSymbolize) {
    EXPECT_TRUE(m_symbolizer->CanSymbolize(GeomType::kPoint));
    EXPECT_FALSE(m_symbolizer->CanSymbolize(GeomType::kLineString));
    EXPECT_FALSE(m_symbolizer->CanSymbolize(GeomType::kPolygon));
}

TEST_F(IntegrationIconSymbolizerTest, Clone) {
    m_symbolizer->SetIconPath("test_icon.png");
    m_symbolizer->SetSize(32.0, 32.0);
    m_symbolizer->SetOpacity(0.8);
    m_symbolizer->SetRotation(30.0);
    
    SymbolizerPtr cloned = m_symbolizer->Clone();
    ASSERT_NE(cloned, nullptr);
    
    IconSymbolizer* clonedIcon = dynamic_cast<IconSymbolizer*>(cloned.get());
    ASSERT_NE(clonedIcon, nullptr);
    
    EXPECT_EQ(clonedIcon->GetIconPath(), "test_icon.png");
    EXPECT_DOUBLE_EQ(clonedIcon->GetWidth(), 32.0);
    EXPECT_DOUBLE_EQ(clonedIcon->GetHeight(), 32.0);
    EXPECT_DOUBLE_EQ(clonedIcon->GetOpacity(), 0.8);
    EXPECT_DOUBLE_EQ(clonedIcon->GetRotation(), 30.0);
}

TEST_F(IntegrationIconSymbolizerTest, CreateWithPath) {
    std::shared_ptr<IconSymbolizer> pathSymbolizer = IconSymbolizer::Create("icon.png");
    ASSERT_NE(pathSymbolizer, nullptr);
    EXPECT_EQ(pathSymbolizer->GetIconPath(), "icon.png");
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeWithIconData) {
    std::vector<uint8_t> data(16 * 16 * 4, 200);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    double x[] = {128};
    double y[] = {128};
    
    DrawResult result = m_context->DrawPoint(x[0], y[0]);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeMultiplePoints) {
    std::vector<uint8_t> data(16 * 16 * 4, 150);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 1.0);
    m_context->SetStyle(style);
    
    double x[] = {50, 100, 150, 200};
    double y[] = {50, 100, 150, 200};
    
    for (int i = 0; i < 4; ++i) {
        DrawResult result = m_context->DrawPoint(x[i], y[i]);
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    m_context->End();
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeWithRotation) {
    std::vector<uint8_t> data(16 * 16 * 4, 180);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    m_symbolizer->SetRotation(45.0);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    DrawResult result = m_context->DrawPoint(128, 128);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeWithOpacity) {
    std::vector<uint8_t> data(16 * 16 * 4, 255);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    m_symbolizer->SetOpacity(0.5);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    DrawResult result = m_context->DrawPoint(128, 128);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeWithDisplacement) {
    std::vector<uint8_t> data(16 * 16 * 4, 200);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    m_symbolizer->SetDisplacement(10.0, 10.0);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    DrawResult result = m_context->DrawPoint(128, 128);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}

TEST_F(IntegrationIconSymbolizerTest, SymbolizeWithAnchorPoint) {
    std::vector<uint8_t> data(16 * 16 * 4, 200);
    m_symbolizer->SetIconData(data.data(), data.size(), 16, 16, 4);
    m_symbolizer->SetSize(16.0, 16.0);
    m_symbolizer->SetAnchorPoint(0.5, 0.5);
    
    m_context->Begin();
    m_device->Clear(Color::White());
    
    DrawResult result = m_context->DrawPoint(128, 128);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_context->End();
}
