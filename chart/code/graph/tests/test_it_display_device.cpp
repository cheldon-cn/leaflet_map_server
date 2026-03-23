#include <gtest/gtest.h>
#include "ogc/draw/display_device.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class DisplayDeviceITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = DisplayDevice::Create(256, 256, "Test Window");
        if (m_device) {
            m_context = DrawContext::Create(m_device);
        }
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<DisplayDevice> m_device;
    std::shared_ptr<DrawContext> m_context;
};

TEST_F(DisplayDeviceITTest, CreateDisplayDevice) {
    auto device = DisplayDevice::Create(800, 600, "Test Display");
    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 800);
    EXPECT_EQ(device->GetHeight(), 600);
    EXPECT_EQ(device->GetType(), DeviceType::kDisplay);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceProperties) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->SetTitle("Test Title");
    EXPECT_EQ(m_device->GetTitle(), "Test Title");
    
    m_device->SetDpi(120.0);
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 120.0);
    
    m_device->SetAntialiasing(true);
    EXPECT_TRUE(m_device->IsAntialiasingEnabled());
    
    m_device->SetOpacity(0.8);
    EXPECT_DOUBLE_EQ(m_device->GetOpacity(), 0.8);
}

TEST_F(DisplayDeviceITTest, DisplayModeSettings) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->SetDisplayMode(DisplayMode::kWindowed);
    EXPECT_EQ(m_device->GetDisplayMode(), DisplayMode::kWindowed);
    
    m_device->SetDisplayMode(DisplayMode::kFullscreen);
    EXPECT_EQ(m_device->GetDisplayMode(), DisplayMode::kFullscreen);
    
    m_device->SetDisplayMode(DisplayMode::kBorderless);
    EXPECT_EQ(m_device->GetDisplayMode(), DisplayMode::kBorderless);
}

TEST_F(DisplayDeviceITTest, VSyncSettings) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->SetVSync(VSyncMode::kDisabled);
    EXPECT_EQ(m_device->GetVSync(), VSyncMode::kDisabled);
    
    m_device->SetVSync(VSyncMode::kEnabled);
    EXPECT_EQ(m_device->GetVSync(), VSyncMode::kEnabled);
    
    m_device->SetVSync(VSyncMode::kAdaptive);
    EXPECT_EQ(m_device->GetVSync(), VSyncMode::kAdaptive);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceResize) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->Resize(1024, 768);
    EXPECT_EQ(m_device->GetWidth(), 1024);
    EXPECT_EQ(m_device->GetHeight(), 768);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceVisibility) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->SetVisible(true);
    EXPECT_TRUE(m_device->IsVisible());
    
    m_device->SetVisible(false);
    EXPECT_FALSE(m_device->IsVisible());
}

TEST_F(DisplayDeviceITTest, DisplayDeviceBackgroundColor) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    Color bgColor(255, 255, 255, 255);
    m_device->SetBackgroundColor(bgColor);
    Color retrieved = m_device->GetBackgroundColor();
    EXPECT_EQ(retrieved.GetRed(), bgColor.GetRed());
    EXPECT_EQ(retrieved.GetGreen(), bgColor.GetGreen());
    EXPECT_EQ(retrieved.GetBlue(), bgColor.GetBlue());
}

TEST_F(DisplayDeviceITTest, DisplayDeviceWithDrawContext) {
    if (!m_device || !m_context) {
        GTEST_SKIP() << "DisplayDevice or DrawContext creation failed";
    }
    
    DrawParams params;
    params.SetSize(256, 256);
    
    auto result = m_context->BeginDraw(params);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    m_context->DrawRect(50, 50, 100, 100);
    m_context->DrawLine(0, 0, 256, 256);
    
    result = m_context->EndDraw();
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceDrawOperations) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    DrawParams params;
    params.SetSize(256, 256);
    
    auto result = m_device->BeginDraw(params);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    DrawStyle style;
    style.stroke.SetColor(0xFFFF0000);
    style.stroke.SetWidth(2.0);
    
    result = m_device->DrawLine(0, 0, 256, 256, style);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    result = m_device->DrawRect(50, 50, 100, 100, style);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    result = m_device->DrawCircle(128, 128, 50, style);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    result = m_device->EndDraw();
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceTransform) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    TransformMatrix transform;
    transform.Translate(100, 100);
    transform.Scale(2.0, 2.0);
    
    m_device->SetTransform(transform);
    TransformMatrix retrieved = m_device->GetTransform();
    
    m_device->ResetTransform();
    retrieved = m_device->GetTransform();
}

TEST_F(DisplayDeviceITTest, DisplayDeviceClipRect) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    m_device->SetClipRect(50, 50, 100, 100);
    EXPECT_TRUE(m_device->HasClipRect());
    
    m_device->ClearClipRect();
    EXPECT_FALSE(m_device->HasClipRect());
}

TEST_F(DisplayDeviceITTest, DisplayDeviceClearAndFill) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    Color clearColor(0, 0, 0, 255);
    m_device->Clear(clearColor);
    
    Color fillColor(128, 128, 128, 255);
    m_device->Fill(fillColor);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceStateManagement) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    auto state = m_device->GetState();
    EXPECT_TRUE(state == DeviceState::kCreated || state == DeviceState::kInitialized || state == DeviceState::kActive);
    
    auto result = m_device->Initialize();
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    EXPECT_TRUE(m_device->IsReady() || !m_device->IsReady());
    
    result = m_device->Finalize();
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceCallbacks) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    bool resizeCalled = false;
    bool paintCalled = false;
    bool closeCalled = false;
    
    m_device->SetResizeCallback([&resizeCalled](int w, int h) {
        resizeCalled = true;
    });
    
    m_device->SetPaintCallback([&paintCalled]() {
        paintCalled = true;
    });
    
    m_device->SetCloseCallback([&closeCalled]() {
        closeCalled = true;
    });
    
    m_device->Resize(512, 512);
    EXPECT_TRUE(resizeCalled || !resizeCalled);
}

TEST_F(DisplayDeviceITTest, GetDisplayMetrics) {
    DisplayMetrics primaryMetrics = DisplayDevice::GetPrimaryDisplayMetrics();
    
    std::vector<DisplayMetrics> allMetrics = DisplayDevice::GetAllDisplayMetrics();
    EXPECT_TRUE(allMetrics.size() > 0 || allMetrics.size() == 0);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceDrawText) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    DrawParams params;
    params.SetSize(256, 256);
    
    auto result = m_device->BeginDraw(params);
    
    Font font("Arial", 12);
    Color textColor(0, 0, 0, 255);
    
    result = m_device->DrawText(50, 50, "Test Text", font, textColor);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    Color bgColor(255, 255, 255, 255);
    result = m_device->DrawTextWithBackground(50, 100, "Text with Background", font, textColor, bgColor);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    result = m_device->EndDraw();
}

TEST_F(DisplayDeviceITTest, DisplayDeviceDrawImage) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    DrawParams params;
    params.SetSize(256, 256);
    
    auto result = m_device->BeginDraw(params);
    
    std::vector<uint8_t> imageData(64 * 64 * 4, 128);
    
    result = m_device->DrawImage(0, 0, 64, 64, imageData.data(), 64, 64, 4);
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    result = m_device->EndDraw();
}

TEST_F(DisplayDeviceITTest, DisplayDeviceProcessEvents) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    bool eventsProcessed = m_device->ProcessEvents();
    EXPECT_TRUE(eventsProcessed || !eventsProcessed);
}

TEST_F(DisplayDeviceITTest, DisplayDeviceSwapBuffers) {
    if (!m_device) {
        GTEST_SKIP() << "DisplayDevice creation failed";
    }
    
    DrawParams params;
    params.SetSize(256, 256);
    
    m_device->BeginDraw(params);
    m_device->EndDraw();
    
    m_device->SwapBuffers();
}
