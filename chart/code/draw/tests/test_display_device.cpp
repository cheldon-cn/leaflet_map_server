#include <gtest/gtest.h>
#include <ogc/draw/display_device.h>
#include <ogc/draw/simple2d_engine.h>

class DisplayDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(DisplayDeviceTest, Construction) {
    ogc::draw::DisplayDevice device(nullptr);
    EXPECT_EQ(device.GetType(), ogc::draw::DeviceType::kDisplay);
    EXPECT_EQ(device.GetNativeWindow(), nullptr);
}

TEST_F(DisplayDeviceTest, Initialize) {
    ogc::draw::DisplayDevice device(nullptr);
    EXPECT_EQ(device.Initialize(), ogc::draw::DrawResult::kSuccess);
    device.Finalize();
}

TEST_F(DisplayDeviceTest, GetSupportedEngineTypes) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    
    std::vector<ogc::draw::EngineType> types = device.GetSupportedEngineTypes();
    EXPECT_EQ(types.size(), 1);
    EXPECT_EQ(types[0], ogc::draw::EngineType::kSimple2D);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, CreateEngine) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    
    std::unique_ptr<ogc::draw::DrawEngine> engine = device.CreateEngine();
    EXPECT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), ogc::draw::EngineType::kSimple2D);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, VSync) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    
    EXPECT_TRUE(device.IsVSyncEnabled());
    
    device.SetVSync(false);
    EXPECT_FALSE(device.IsVSyncEnabled());
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, InvalidateRect) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    device.Resize(800, 600);
    device.ClearDirtyRects();
    
    device.InvalidateRect(ogc::draw::Rect(10, 10, 100, 100));
    
    std::vector<ogc::draw::Rect> dirtyRects = device.GetDirtyRects();
    EXPECT_EQ(dirtyRects.size(), 1);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, InvalidateAll) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    device.Resize(800, 600);
    device.ClearDirtyRects();
    
    device.InvalidateAll();
    
    std::vector<ogc::draw::Rect> dirtyRects = device.GetDirtyRects();
    EXPECT_EQ(dirtyRects.size(), 1);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, ClearDirtyRects) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    device.Resize(800, 600);
    device.ClearDirtyRects();
    
    device.InvalidateRect(ogc::draw::Rect(10, 10, 100, 100));
    EXPECT_EQ(device.GetDirtyRects().size(), 1);
    
    device.ClearDirtyRects();
    EXPECT_EQ(device.GetDirtyRects().size(), 0);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, Resize) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    
    device.Resize(1024, 768);
    EXPECT_EQ(device.GetWidth(), 1024);
    EXPECT_EQ(device.GetHeight(), 768);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, PixelData) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    device.Resize(100, 100);
    
    uint8_t* buffer = device.GetPixelData();
    EXPECT_NE(buffer, nullptr);
    EXPECT_GT(device.GetStride(), 0);
    
    device.Finalize();
}

TEST_F(DisplayDeviceTest, GetDpi) {
    ogc::draw::DisplayDevice device(nullptr);
    device.Initialize();
    
    double dpi = device.GetDpi();
    EXPECT_GT(dpi, 0.0);
    
    device.Finalize();
}
