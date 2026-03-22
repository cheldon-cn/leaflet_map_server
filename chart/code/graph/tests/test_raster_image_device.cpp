#include <gtest/gtest.h>
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/color.h"
#include "ogc/draw/draw_style.h"
#include "ogc/envelope.h"

TEST(RasterImageDeviceTest, DefaultConstructor) {
    ogc::draw::RasterImageDevice device;
    EXPECT_EQ(device.GetWidth(), 0);
    EXPECT_EQ(device.GetHeight(), 0);
    EXPECT_EQ(device.GetChannels(), 4);
    EXPECT_FALSE(device.IsReady());
}

TEST(RasterImageDeviceTest, ParameterizedConstructor) {
    ogc::draw::RasterImageDevice device(800, 600, 4);
    EXPECT_EQ(device.GetWidth(), 800);
    EXPECT_EQ(device.GetHeight(), 600);
    EXPECT_EQ(device.GetChannels(), 4);
}

TEST(RasterImageDeviceTest, Initialize) {
    ogc::draw::RasterImageDevice device(100, 100);
    auto result = device.Initialize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(device.IsReady());
}

TEST(RasterImageDeviceTest, Finalize) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    auto result = device.Finalize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(device.IsReady());
}

TEST(RasterImageDeviceTest, GetType) {
    ogc::draw::RasterImageDevice device;
    EXPECT_EQ(device.GetType(), ogc::draw::DeviceType::kRasterImage);
}

TEST(RasterImageDeviceTest, GetName) {
    ogc::draw::RasterImageDevice device;
    EXPECT_EQ(device.GetName(), "RasterImageDevice");
}

TEST(RasterImageDeviceTest, BeginEndDraw) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    auto result = device.BeginDraw(params);
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(device.IsDrawing());
    
    result = device.EndDraw();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(device.IsDrawing());
}

TEST(RasterImageDeviceTest, Clear) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    device.BeginDraw(params);
    device.Clear(ogc::draw::Color::Blue());
    device.EndDraw();
    
    ogc::draw::Color pixel = device.GetPixel(50, 50);
    EXPECT_EQ(pixel.GetRed(), 0);
    EXPECT_EQ(pixel.GetGreen(), 0);
    EXPECT_EQ(pixel.GetBlue(), 255);
}

TEST(RasterImageDeviceTest, DrawRect) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    device.BeginDraw(params);
    device.Clear(ogc::draw::Color::White());
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Red().GetRGBA();
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    
    device.DrawRect(10, 10, 30, 30, style);
    device.EndDraw();
    
    ogc::draw::Color insidePixel = device.GetPixel(20, 20);
    EXPECT_EQ(insidePixel.GetRed(), 255);
}

TEST(RasterImageDeviceTest, DrawLine) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    device.BeginDraw(params);
    device.Clear(ogc::draw::Color::White());
    
    ogc::draw::DrawStyle style;
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    
    device.DrawLine(10, 10, 90, 90, style);
    device.EndDraw();
}

TEST(RasterImageDeviceTest, DrawCircle) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    device.BeginDraw(params);
    device.Clear(ogc::draw::Color::White());
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Blue().GetRGBA();
    
    device.DrawCircle(50, 50, 20, style);
    device.EndDraw();
}

TEST(RasterImageDeviceTest, SetDpi) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 150.0);
}

TEST(RasterImageDeviceTest, Transform) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    ogc::draw::TransformMatrix transform;
    transform.Translate(10.0, 20.0);
    device.SetTransform(transform);
    
    ogc::draw::TransformMatrix retrieved = device.GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.Get(0, 2), 10.0);
    EXPECT_DOUBLE_EQ(retrieved.Get(1, 2), 20.0);
    
    device.ResetTransform();
    retrieved = device.GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.Get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(retrieved.Get(1, 2), 0.0);
}

TEST(RasterImageDeviceTest, ClipRect) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    EXPECT_FALSE(device.HasClipRect());
    
    device.SetClipRect(10, 10, 50, 50);
    EXPECT_TRUE(device.HasClipRect());
    
    device.ClearClipRect();
    EXPECT_FALSE(device.HasClipRect());
}

TEST(RasterImageDeviceTest, GetData) {
    ogc::draw::RasterImageDevice device(100, 100, 4);
    device.Initialize();
    
    const uint8_t* data = device.GetData();
    EXPECT_NE(data, nullptr);
    
    size_t size = device.GetDataSize();
    EXPECT_EQ(size, static_cast<size_t>(100 * 100 * 4));
}

TEST(RasterImageDeviceTest, Resize) {
    ogc::draw::RasterImageDevice device(100, 100);
    device.Initialize();
    
    device.Resize(200, 150, 3);
    EXPECT_EQ(device.GetWidth(), 200);
    EXPECT_EQ(device.GetHeight(), 150);
    EXPECT_EQ(device.GetChannels(), 3);
}

TEST(RasterImageDeviceTest, StaticCreate) {
    auto device = ogc::draw::RasterImageDevice::Create(100, 100, 4);
    EXPECT_NE(device, nullptr);
    EXPECT_EQ(device->GetWidth(), 100);
    EXPECT_EQ(device->GetHeight(), 100);
}
