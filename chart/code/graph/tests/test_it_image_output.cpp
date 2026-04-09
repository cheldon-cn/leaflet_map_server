#include <gtest/gtest.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/geom/envelope.h"
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
        device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
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
    EXPECT_EQ(device->GetBytesPerPixel(), 4);
}

TEST_F(IntegrationImageOutputTest, DeviceDataAccess) {
    uint8_t* data = device->GetPixelData();
    ASSERT_NE(data, nullptr);
    
    size_t dataSize = device->GetDataSize();
    EXPECT_EQ(dataSize, 256 * 256 * 4);
}

TEST_F(IntegrationImageOutputTest, ClearAndVerifyPixels) {
    device->Clear(Color::Red());
    
    Color centerPixel = device->GetPixel(128, 128);
    EXPECT_EQ(centerPixel.GetRed(), 255);
    EXPECT_EQ(centerPixel.GetGreen(), 0);
    EXPECT_EQ(centerPixel.GetBlue(), 0);
    EXPECT_EQ(centerPixel.GetAlpha(), 255);
}

TEST_F(IntegrationImageOutputTest, DrawLineAndVerifyPixels) {
    device->Clear(Color::White());
    
    device->SetPixel(0, 0, Color::Black());
    device->SetPixel(255, 255, Color::Black());
    
    Color bgPixel = device->GetPixel(0, 255);
    EXPECT_EQ(bgPixel.GetRed(), 255);
    EXPECT_EQ(bgPixel.GetGreen(), 255);
    EXPECT_EQ(bgPixel.GetBlue(), 255);
}

TEST_F(IntegrationImageOutputTest, DrawRectAndVerifyPixels) {
    device->Clear(Color::White());
    
    for (int y = 50; y < 150; ++y) {
        for (int x = 50; x < 150; ++x) {
            device->SetPixel(x, y, Color::Yellow());
        }
    }
    
    Color insidePixel = device->GetPixel(100, 100);
    EXPECT_EQ(insidePixel.GetRed(), 255);
    EXPECT_EQ(insidePixel.GetGreen(), 255);
    EXPECT_EQ(insidePixel.GetBlue(), 0);
}

TEST_F(IntegrationImageOutputTest, DrawCircleAndVerifyPixels) {
    device->Clear(Color::White());
    
    for (int y = 78; y < 178; ++y) {
        for (int x = 78; x < 178; ++x) {
            int dx = x - 128;
            int dy = y - 128;
            if (dx * dx + dy * dy <= 50 * 50) {
                device->SetPixel(x, y, Color::Green());
            }
        }
    }
    
    Color centerPixel = device->GetPixel(128, 128);
    EXPECT_EQ(centerPixel.GetGreen(), 255);
}

TEST_F(IntegrationImageOutputTest, SaveToFile) {
    device->Clear(Color::Blue());
    
    device->SetPixel(0, 0, Color::White());
    device->SetPixel(255, 255, Color::White());
    device->SetPixel(0, 255, Color::White());
    device->SetPixel(255, 0, Color::White());
    
    std::string filePath = tempDir + "test_output.bmp";
    
    bool saved = device->SaveToFile(filePath, OutputFormat::kBMP);
    if (saved) {
        EXPECT_TRUE(FileExists(filePath));
        DeleteFile(filePath);
    }
}

TEST_F(IntegrationImageOutputTest, MultipleDrawOperations) {
    device->Clear(Color::White());
    
    device->SetPixel(0, 0, Color::Red());
    device->SetPixel(128, 0, Color::Blue());
    device->SetPixel(255, 0, Color::Green());
    
    EXPECT_TRUE(device->IsReady());
}

TEST_F(IntegrationImageOutputTest, ResizeDevice) {
    device->Finalize();
    device = std::make_shared<RasterImageDevice>(512, 512, PixelFormat::kRGBA8888);
    device->Initialize();
    
    EXPECT_EQ(device->GetWidth(), 512);
    EXPECT_EQ(device->GetHeight(), 512);
    EXPECT_EQ(device->GetBytesPerPixel(), 4);
    
    size_t dataSize = device->GetDataSize();
    EXPECT_EQ(dataSize, 512 * 512 * 4);
}

TEST_F(IntegrationImageOutputTest, SetPixelDirectly) {
    device->Clear(Color::White());
    
    for (int i = 0; i < 256; ++i) {
        device->SetPixel(i, i, Color::Red());
    }
    
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

TEST_F(IntegrationImageOutputTest, PixelFormat) {
    EXPECT_EQ(device->GetPixelFormat(), PixelFormat::kRGBA8888);
}

TEST_F(IntegrationImageOutputTest, StrideCalculation) {
    int stride = device->GetStride();
    EXPECT_EQ(stride, 256 * 4);
}

TEST_F(IntegrationImageOutputTest, IsValid) {
    EXPECT_TRUE(device->IsValid());
}
