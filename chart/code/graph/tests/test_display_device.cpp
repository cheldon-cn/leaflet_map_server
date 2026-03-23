#include <gtest/gtest.h>
#include "ogc/draw/display_device.h"
#include <memory>

using namespace ogc::draw;

class DisplayDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = DisplayDevice::Create(256, 256, "TestWindow");
        ASSERT_NE(m_device, nullptr);
    }
    
    void TearDown() override {
        m_device.reset();
    }
    
    DisplayDevicePtr m_device;
};

TEST_F(DisplayDeviceTest, Create) {
    EXPECT_NE(m_device, nullptr);
}

TEST_F(DisplayDeviceTest, GetType) {
    EXPECT_EQ(m_device->GetType(), DeviceType::kDisplay);
}

TEST_F(DisplayDeviceTest, GetName) {
    EXPECT_EQ(m_device->GetName(), "DisplayDevice");
}

TEST_F(DisplayDeviceTest, Initialize) {
    auto result = m_device->Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DisplayDeviceTest, Finalize) {
    m_device->Initialize();
    auto result = m_device->Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DisplayDeviceTest, IsReady) {
    m_device->Initialize();
    EXPECT_TRUE(m_device->IsReady());
}

TEST_F(DisplayDeviceTest, GetWidthHeight) {
    EXPECT_EQ(m_device->GetWidth(), 256);
    EXPECT_EQ(m_device->GetHeight(), 256);
}

TEST_F(DisplayDeviceTest, GetDpi) {
    EXPECT_GT(m_device->GetDpi(), 0);
}

TEST_F(DisplayDeviceTest, SetDpi) {
    m_device->SetDpi(150.0);
    EXPECT_DOUBLE_EQ(m_device->GetDpi(), 150.0);
}

TEST_F(DisplayDeviceTest, SetTitle) {
    m_device->SetTitle("New Title");
    EXPECT_EQ(m_device->GetTitle(), "New Title");
}

TEST_F(DisplayDeviceTest, SetAntialiasing) {
    m_device->SetAntialiasing(true);
    EXPECT_TRUE(m_device->IsAntialiasingEnabled());
    
    m_device->SetAntialiasing(false);
    EXPECT_FALSE(m_device->IsAntialiasingEnabled());
}

TEST_F(DisplayDeviceTest, SetOpacity) {
    m_device->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_device->GetOpacity(), 0.5);
}

TEST_F(DisplayDeviceTest, SetVisible) {
    m_device->SetVisible(true);
    EXPECT_TRUE(m_device->IsVisible());
    
    m_device->SetVisible(false);
    EXPECT_FALSE(m_device->IsVisible());
}

TEST_F(DisplayDeviceTest, BeginEndDraw) {
    m_device->Initialize();
    DrawParams params;
    auto result = m_device->BeginDraw(params);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = m_device->EndDraw();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DisplayDeviceTest, IsDrawing) {
    m_device->Initialize();
    EXPECT_FALSE(m_device->IsDrawing());
    
    DrawParams params;
    m_device->BeginDraw(params);
    EXPECT_TRUE(m_device->IsDrawing());
    
    m_device->EndDraw();
    EXPECT_FALSE(m_device->IsDrawing());
}

TEST_F(DisplayDeviceTest, SetTransform) {
    TransformMatrix transform;
    transform.Translate(10, 20);
    m_device->SetTransform(transform);
    
    auto retrieved = m_device->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetM02(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetM12(), 20);
}

TEST_F(DisplayDeviceTest, ResetTransform) {
    TransformMatrix transform;
    transform.Translate(10, 20);
    m_device->SetTransform(transform);
    m_device->ResetTransform();
    
    auto retrieved = m_device->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.GetM02(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetM12(), 0);
}

TEST_F(DisplayDeviceTest, SetClipRect) {
    m_device->SetClipRect(10, 10, 100, 100);
    EXPECT_TRUE(m_device->HasClipRect());
    
    m_device->ClearClipRect();
    EXPECT_FALSE(m_device->HasClipRect());
}

TEST_F(DisplayDeviceTest, SetResizeCallback) {
    m_device->SetResizeCallback([](int, int) {});
}

TEST_F(DisplayDeviceTest, SetPaintCallback) {
    m_device->SetPaintCallback([]() {});
}

TEST_F(DisplayDeviceTest, SetCloseCallback) {
    m_device->SetCloseCallback([]() {});
}

TEST_F(DisplayDeviceTest, DrawPoint) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawPoint(128, 128, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, DrawLine) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawLine(0, 0, 256, 256, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, DrawRect) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawRect(50, 50, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, DrawCircle) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    DrawStyle style;
    auto result = m_device->DrawCircle(128, 128, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, Clear) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    Color color(255, 255, 255);
    m_device->Clear(color);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, Fill) {
    m_device->Initialize();
    DrawParams params;
    m_device->BeginDraw(params);
    
    Color color(0, 0, 255);
    m_device->Fill(color);
    
    m_device->EndDraw();
}

TEST_F(DisplayDeviceTest, GetAllDisplayMetrics) {
    auto displays = DisplayDevice::GetAllDisplayMetrics();
    EXPECT_GE(displays.size(), 1);
}

TEST_F(DisplayDeviceTest, GetPrimaryDisplayMetrics) {
    auto display = DisplayDevice::GetPrimaryDisplayMetrics();
    EXPECT_GT(display.width, 0);
    EXPECT_GT(display.height, 0);
}

TEST_F(DisplayDeviceTest, GetState) {
    auto state = m_device->GetState();
    EXPECT_NE(state, DeviceState::kError);
}

TEST_F(DisplayDeviceTest, SetVSync) {
    m_device->SetVSync(VSyncMode::kEnabled);
    EXPECT_EQ(m_device->GetVSync(), VSyncMode::kEnabled);
    
    m_device->SetVSync(VSyncMode::kDisabled);
    EXPECT_EQ(m_device->GetVSync(), VSyncMode::kDisabled);
}

TEST_F(DisplayDeviceTest, SetDisplayMode) {
    m_device->SetDisplayMode(DisplayMode::kWindowed);
    EXPECT_EQ(m_device->GetDisplayMode(), DisplayMode::kWindowed);
}

TEST_F(DisplayDeviceTest, Resize) {
    m_device->Resize(512, 512);
    EXPECT_EQ(m_device->GetWidth(), 512);
    EXPECT_EQ(m_device->GetHeight(), 512);
}

TEST_F(DisplayDeviceTest, SetBackgroundColor) {
    Color color(0, 0, 255);
    m_device->SetBackgroundColor(color);
    
    auto retrieved = m_device->GetBackgroundColor();
    EXPECT_EQ(retrieved.GetBlue(), 255);
}
