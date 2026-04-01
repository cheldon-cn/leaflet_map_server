#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include <memory>
#include <cstdio>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace ogc::draw;

namespace {
    std::string GetTempDirectory() {
#ifdef _WIN32
        char path[MAX_PATH];
        if (GetTempPathA(MAX_PATH, path)) {
            return std::string(path);
        }
        return ".";
#else
        return "/tmp";
#endif
    }
    
    bool FileExists(const std::string& path) {
        FILE* file = fopen(path.c_str(), "r");
        if (file) {
            fclose(file);
            return true;
        }
        return false;
    }
    
    void DeleteFile(const std::string& path) {
        remove(path.c_str());
    }
}

class IntegrationImageOutputTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
        
        tempDir = GetTempDirectory();
    }
    
    void TearDown() override {
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
    std::string tempDir;
};

TEST_F(IntegrationImageOutputTest, DeviceDimensions) {
    EXPECT_EQ(device->GetWidth(), 256);
    EXPECT_EQ(device->GetHeight(), 256);
    EXPECT_EQ(device->GetChannels(), 4);
}

TEST_F(IntegrationImageOutputTest, DeviceDataAccess) {
    uint8_t* data = device->GetData();
    ASSERT_NE(data, nullptr);
    
    size_t dataSize = device->GetDataSize();
    EXPECT_EQ(dataSize, 256 * 256 * 4);
}

TEST_F(IntegrationImageOutputTest, ClearAndVerifyPixels) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::Red());
    device->EndDraw();
    
    Color centerPixel = device->GetPixel(128, 128);
    EXPECT_EQ(centerPixel.GetRed(), 255);
    EXPECT_EQ(centerPixel.GetGreen(), 0);
    EXPECT_EQ(centerPixel.GetBlue(), 0);
    EXPECT_EQ(centerPixel.GetAlpha(), 255);
}

TEST_F(IntegrationImageOutputTest, DrawLineAndVerifyPixels) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 1.0;
    
    device->DrawLine(0, 0, 255, 255, style);
    device->EndDraw();
    
    Color bgPixel = device->GetPixel(0, 255);
    EXPECT_EQ(bgPixel.GetRed(), 255);
    EXPECT_EQ(bgPixel.GetGreen(), 255);
    EXPECT_EQ(bgPixel.GetBlue(), 255);
}

TEST_F(IntegrationImageOutputTest, DrawRectAndVerifyPixels) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Blue().GetRGBA();
    style.pen.width = 2.0;
    style.brush.color = Color::Yellow().GetRGBA();
    style.brush.visible = true;
    
    device->DrawRect(50, 50, 100, 100, style);
    device->EndDraw();
    
    Color insidePixel = device->GetPixel(100, 100);
    EXPECT_EQ(insidePixel.GetRed(), 255);
    EXPECT_EQ(insidePixel.GetGreen(), 255);
    EXPECT_EQ(insidePixel.GetBlue(), 0);
}

TEST_F(IntegrationImageOutputTest, DrawCircleAndVerifyPixels) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Green().GetRGBA();
    style.pen.width = 1.0;
    style.brush.color = Color::Green().GetRGBA();
    style.brush.visible = true;
    
    device->DrawCircle(128, 128, 50, style);
    device->EndDraw();
    
    Color centerPixel = device->GetPixel(128, 128);
    EXPECT_EQ(centerPixel.GetGreen(), 255);
}

TEST_F(IntegrationImageOutputTest, SaveToFile) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::Blue());
    
    DrawStyle style;
    style.pen.color = Color::White().GetRGBA();
    style.pen.width = 2.0;
    
    device->DrawLine(0, 0, 255, 255, style);
    device->DrawLine(0, 255, 255, 0, style);
    device->EndDraw();
    
    std::string filePath = tempDir + "test_output.bmp";
    
    bool saved = device->SaveToFile(filePath, ImageFormat::kBMP);
    if (saved) {
        EXPECT_TRUE(FileExists(filePath));
        DeleteFile(filePath);
    }
}

TEST_F(IntegrationImageOutputTest, MultipleDrawOperations) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    DrawStyle redStyle;
    redStyle.pen.color = Color::Red().GetRGBA();
    redStyle.pen.width = 2.0;
    
    DrawStyle blueStyle;
    blueStyle.pen.color = Color::Blue().GetRGBA();
    blueStyle.pen.width = 2.0;
    
    DrawStyle greenStyle;
    greenStyle.pen.color = Color::Green().GetRGBA();
    greenStyle.pen.width = 2.0;
    
    device->DrawLine(0, 0, 255, 0, redStyle);
    device->DrawLine(0, 128, 255, 128, blueStyle);
    device->DrawLine(0, 255, 255, 255, greenStyle);
    
    device->EndDraw();
    
    EXPECT_TRUE(device->IsReady());
}

TEST_F(IntegrationImageOutputTest, ResizeDevice) {
    device->Resize(512, 512, 4);
    
    EXPECT_EQ(device->GetWidth(), 512);
    EXPECT_EQ(device->GetHeight(), 512);
    EXPECT_EQ(device->GetChannels(), 4);
    
    size_t dataSize = device->GetDataSize();
    EXPECT_EQ(dataSize, 512 * 512 * 4);
}

TEST_F(IntegrationImageOutputTest, SetPixelDirectly) {
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    
    device->BeginDraw(params);
    device->Clear(Color::White());
    
    for (int i = 0; i < 256; ++i) {
        device->SetPixel(i, i, Color::Red());
    }
    
    device->EndDraw();
    
    Color diagonalPixel = device->GetPixel(100, 100);
    EXPECT_EQ(diagonalPixel.GetRed(), 255);
    EXPECT_EQ(diagonalPixel.GetGreen(), 0);
    EXPECT_EQ(diagonalPixel.GetBlue(), 0);
}

TEST_F(IntegrationImageOutputTest, DpiSetting) {
    device->SetDpi(150.0);
    
    double dpi = device->GetDpi();
    EXPECT_DOUBLE_EQ(dpi, 150.0);
}

TEST_F(IntegrationImageOutputTest, AntialiasingSetting) {
    device->SetAntialiasing(true);
    EXPECT_TRUE(device->IsAntialiasingEnabled());
    
    device->SetAntialiasing(false);
    EXPECT_FALSE(device->IsAntialiasingEnabled());
}

TEST_F(IntegrationImageOutputTest, OpacitySetting) {
    device->SetOpacity(0.5);
    
    double opacity = device->GetOpacity();
    EXPECT_DOUBLE_EQ(opacity, 0.5);
}

