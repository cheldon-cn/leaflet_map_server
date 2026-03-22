#include <gtest/gtest.h>
#include "ogc/draw/tile_device.h"
#include "ogc/draw/tile_key.h"
#include "ogc/draw/device_state.h"
#include "ogc/draw/color.h"
#include "ogc/draw/draw_style.h"
#include "ogc/envelope.h"

TEST(TileDeviceTest, DefaultConstructor) {
    ogc::draw::TileDevice device;
    EXPECT_EQ(device.GetTileSize(), 256);
    EXPECT_EQ(device.GetType(), ogc::draw::DeviceType::kTile);
    EXPECT_EQ(device.GetName(), "TileDevice");
    EXPECT_FALSE(device.IsReady());
}

TEST(TileDeviceTest, ConstructorWithTileSize) {
    ogc::draw::TileDevice device(512);
    EXPECT_EQ(device.GetTileSize(), 512);
}

TEST(TileDeviceTest, Initialize) {
    ogc::draw::TileDevice device;
    auto result = device.Initialize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(device.IsReady());
}

TEST(TileDeviceTest, Finalize) {
    ogc::draw::TileDevice device;
    device.Initialize();
    auto result = device.Finalize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(device.IsReady());
}

TEST(TileDeviceTest, GetState) {
    ogc::draw::TileDevice device;
    EXPECT_EQ(device.GetState(), ogc::draw::DeviceState::kCreated);
    device.Initialize();
    EXPECT_EQ(device.GetState(), ogc::draw::DeviceState::kInitialized);
}

TEST(TileDeviceTest, BeginEndDraw) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    auto result = device.BeginDraw(params);
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(device.IsDrawing());
    
    result = device.EndDraw();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(device.IsDrawing());
}

TEST(TileDeviceTest, AbortDraw) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    device.BeginDraw(params);
    auto result = device.AbortDraw();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(device.IsDrawing());
}

TEST(TileDeviceTest, SetTileSize) {
    ogc::draw::TileDevice device;
    device.SetTileSize(512);
    EXPECT_EQ(device.GetTileSize(), 512);
}

TEST(TileDeviceTest, Transform) {
    ogc::draw::TileDevice device;
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

TEST(TileDeviceTest, ClipRect) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    EXPECT_FALSE(device.HasClipRect());
    
    device.SetClipRect(10, 10, 100, 100);
    EXPECT_TRUE(device.HasClipRect());
    
    device.ClearClipRect();
    EXPECT_FALSE(device.HasClipRect());
}

TEST(TileDeviceTest, Clear) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    device.BeginDraw(params);
    device.Clear(ogc::draw::Color::White());
    device.EndDraw();
}

TEST(TileDeviceTest, Fill) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    device.BeginDraw(params);
    device.Fill(ogc::draw::Color::Blue());
    device.EndDraw();
}

TEST(TileDeviceTest, DrawPoint) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Red().GetRGBA();
    
    device.BeginDraw(params);
    auto result = device.DrawPoint(128, 128, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, DrawLine) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    ogc::draw::DrawStyle style;
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    
    device.BeginDraw(params);
    auto result = device.DrawLine(10, 10, 246, 246, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, DrawRect) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Red().GetRGBA();
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    
    device.BeginDraw(params);
    auto result = device.DrawRect(50, 50, 100, 100, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, DrawCircle) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Blue().GetRGBA();
    
    device.BeginDraw(params);
    auto result = device.DrawCircle(128, 128, 50, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, DrawPolygon) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    double x[] = {128, 50, 206, 128};
    double y[] = {50, 206, 206, 50};
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Yellow().GetRGBA();
    
    device.BeginDraw(params);
    auto result = device.DrawPolygon(x, y, 4, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, DrawPolyline) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    double x[] = {10, 50, 100, 150, 200, 246};
    double y[] = {128, 50, 200, 50, 200, 128};
    
    ogc::draw::DrawStyle style;
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 2.0;
    
    device.BeginDraw(params);
    auto result = device.DrawPolyline(x, y, 6, style);
    device.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(TileDeviceTest, SetDpi) {
    ogc::draw::TileDevice device;
    device.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 150.0);
}

TEST(TileDeviceTest, SetAntialiasing) {
    ogc::draw::TileDevice device;
    device.SetAntialiasing(true);
    EXPECT_TRUE(device.IsAntialiasingEnabled());
    device.SetAntialiasing(false);
    EXPECT_FALSE(device.IsAntialiasingEnabled());
}

TEST(TileDeviceTest, SetOpacity) {
    ogc::draw::TileDevice device;
    device.SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(device.GetOpacity(), 0.5);
}

TEST(TileDeviceTest, GetWidthHeight) {
    ogc::draw::TileDevice device(512);
    device.Initialize();
    
    EXPECT_EQ(device.GetWidth(), 512);
    EXPECT_EQ(device.GetHeight(), 512);
}

TEST(TileDeviceTest, GetDrawParams) {
    ogc::draw::TileDevice device;
    device.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = ogc::Envelope(0, 0, 256, 256);
    
    device.BeginDraw(params);
    ogc::draw::DrawParams retrieved = device.GetDrawParams();
    device.EndDraw();
    
    EXPECT_EQ(retrieved.pixel_width, 256);
    EXPECT_EQ(retrieved.pixel_height, 256);
}
