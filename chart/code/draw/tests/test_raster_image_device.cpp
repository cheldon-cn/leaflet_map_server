#include <gtest/gtest.h>
#include "ogc/draw/raster_image_device.h"
#include <cstdio>

using namespace ogc::draw;

TEST(RasterImageDeviceTest, DefaultConstruction) {
    RasterImageDevice device(100, 100);
    
    EXPECT_EQ(device.GetWidth(), 100);
    EXPECT_EQ(device.GetHeight(), 100);
    EXPECT_EQ(device.GetType(), DeviceType::kRasterImage);
    EXPECT_TRUE(device.IsValid());
}

TEST(RasterImageDeviceTest, DifferentPixelFormats) {
    RasterImageDevice rgba(50, 50, PixelFormat::kRGBA8888);
    RasterImageDevice rgb(50, 50, PixelFormat::kRGB888);
    RasterImageDevice gray(50, 50, PixelFormat::kGray8);
    
    EXPECT_EQ(rgba.GetPixelFormat(), PixelFormat::kRGBA8888);
    EXPECT_EQ(rgba.GetBytesPerPixel(), 4);
    
    EXPECT_EQ(rgb.GetPixelFormat(), PixelFormat::kRGB888);
    EXPECT_EQ(rgb.GetBytesPerPixel(), 3);
    
    EXPECT_EQ(gray.GetPixelFormat(), PixelFormat::kGray8);
    EXPECT_EQ(gray.GetBytesPerPixel(), 1);
}

TEST(RasterImageDeviceTest, GetSetPixelRGBA) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    device.SetPixel(5, 5, Color(255, 0, 0, 255));
    Color c = device.GetPixel(5, 5);
    
    EXPECT_EQ(c.GetRed(), 255);
    EXPECT_EQ(c.GetGreen(), 0);
    EXPECT_EQ(c.GetBlue(), 0);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST(RasterImageDeviceTest, GetSetPixelRGB) {
    RasterImageDevice device(10, 10, PixelFormat::kRGB888);
    
    device.SetPixel(3, 7, Color(0, 255, 128, 255));
    Color c = device.GetPixel(3, 7);
    
    EXPECT_EQ(c.GetRed(), 0);
    EXPECT_EQ(c.GetGreen(), 255);
    EXPECT_EQ(c.GetBlue(), 128);
}

TEST(RasterImageDeviceTest, GetSetPixelGray) {
    RasterImageDevice device(10, 10, PixelFormat::kGray8);
    
    device.SetPixel(2, 3, Color(128, 128, 128, 255));
    Color c = device.GetPixel(2, 3);
    
    EXPECT_EQ(c.GetRed(), c.GetGreen());
    EXPECT_EQ(c.GetGreen(), c.GetBlue());
}

TEST(RasterImageDeviceTest, Clear) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    device.Clear(Color(100, 150, 200, 255));
    
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            Color c = device.GetPixel(x, y);
            EXPECT_EQ(c.GetRed(), 100);
            EXPECT_EQ(c.GetGreen(), 150);
            EXPECT_EQ(c.GetBlue(), 200);
        }
    }
}

TEST(RasterImageDeviceTest, PixelDataAccess) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    EXPECT_NE(device.GetPixelData(), nullptr);
    EXPECT_EQ(device.GetStride(), 10 * 4);
    EXPECT_GT(device.GetDataSize(), 0u);
}

TEST(RasterImageDeviceTest, OutOfBoundsPixel) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    device.SetPixel(-1, 0, Color::Red());
    device.SetPixel(0, -1, Color::Red());
    device.SetPixel(10, 0, Color::Red());
    device.SetPixel(0, 10, Color::Red());
    
    Color c1 = device.GetPixel(-1, 0);
    Color c2 = device.GetPixel(10, 0);
    
    EXPECT_EQ(c1, Color());
    EXPECT_EQ(c2, Color());
}

TEST(RasterImageDeviceTest, SupportedEngines) {
    RasterImageDevice device(100, 100);
    
    std::vector<EngineType> engines = device.GetSupportedEngineTypes();
    EXPECT_FALSE(engines.empty());
    EXPECT_EQ(device.GetPreferredEngineType(), EngineType::kSimple2D);
}

TEST(RasterImageDeviceTest, DpiSetting) {
    RasterImageDevice device(100, 100);
    
    EXPECT_DOUBLE_EQ(device.GetDpi(), 96.0);
    
    device.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 150.0);
}

TEST(RasterImageDeviceTest, SaveToBMP) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    device.Clear(Color::Red());
    device.SetPixel(5, 5, Color::Blue());
    
    const char* filename = "test_output.bmp";
    bool result = device.SaveToFile(filename, OutputFormat::kBMP);
    EXPECT_TRUE(result);
    
    std::remove(filename);
}

TEST(RasterImageDeviceTest, SaveToPNG) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    device.Clear(Color::Green());
    
    const char* filename = "test_output.png";
    bool result = device.SaveToFile(filename, OutputFormat::kPNG);
    EXPECT_TRUE(result);
    
    std::remove(filename);
}

TEST(RasterImageDeviceTest, AutoFormatDetection) {
    RasterImageDevice device(10, 10, PixelFormat::kRGBA8888);
    
    bool result = device.SaveToFile("test_auto.png");
    EXPECT_TRUE(result);
    std::remove("test_auto.png");
    
    result = device.SaveToFile("test_auto.bmp");
    EXPECT_TRUE(result);
    std::remove("test_auto.bmp");
}

TEST(RasterImageDeviceTest, BGRAFormat) {
    RasterImageDevice device(10, 10, PixelFormat::kBGRA8888);
    
    device.SetPixel(5, 5, Color(255, 128, 64, 255));
    Color c = device.GetPixel(5, 5);
    
    EXPECT_EQ(c.GetRed(), 255);
    EXPECT_EQ(c.GetGreen(), 128);
    EXPECT_EQ(c.GetBlue(), 64);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST(RasterImageDeviceTest, InvalidDimensions) {
    RasterImageDevice device(0, 0);
    EXPECT_FALSE(device.IsValid());
    
    RasterImageDevice device2(-10, 10);
    EXPECT_FALSE(device2.IsValid());
}
