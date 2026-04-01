#include <gtest/gtest.h>
#include <ogc/draw/tile_device.h>
#include "ogc/draw/tile_key.h"
#include "ogc/draw/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class IntegrationTileRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        tileDevice = TileDevice::Create(256);
        ASSERT_NE(tileDevice, nullptr);
        
        DrawResult result = tileDevice->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    void TearDown() override {
        if (tileDevice) {
            tileDevice->Finalize();
        }
    }
    
    TileDevicePtr tileDevice;
};

TEST_F(IntegrationTileRenderTest, DeviceCreation) {
    EXPECT_NE(tileDevice, nullptr);
    EXPECT_EQ(tileDevice->GetType(), DeviceType::kTile);
    EXPECT_EQ(tileDevice->GetTileSize(), 256);
}

TEST_F(IntegrationTileRenderTest, DeviceInitialization) {
    EXPECT_TRUE(tileDevice->IsReady());
}

TEST_F(IntegrationTileRenderTest, BeginEndTile) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    EXPECT_TRUE(tileDevice->IsDrawing());
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    EXPECT_FALSE(tileDevice->IsDrawing());
}

TEST_F(IntegrationTileRenderTest, RenderSingleTile) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 2.0;
    
    result = tileDevice->DrawRect(10, 10, 236, 236, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, RenderMultipleTiles) {
    for (int z = 0; z <= 2; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                DrawResult result = tileDevice->BeginTile(x, y, z);
                EXPECT_EQ(result, DrawResult::kSuccess);
                
                tileDevice->Clear(Color::White());
                
                DrawStyle style;
                style.pen.color = Color::Blue().GetRGBA();
                style.pen.width = 1.0;
                
                tileDevice->DrawRect(0, 0, 256, 256, style);
                
                result = tileDevice->EndTile();
                EXPECT_EQ(result, DrawResult::kSuccess);
            }
        }
    }
}

TEST_F(IntegrationTileRenderTest, DrawPrimitives) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Black().GetRGBA();
    style.pen.width = 1.0;
    
    result = tileDevice->DrawPoint(128, 128, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->DrawLine(0, 0, 256, 256, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->DrawRect(50, 50, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->DrawCircle(128, 128, 50, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, DrawPolyline) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Green().GetRGBA();
    style.pen.width = 2.0;
    
    double x[] = {10, 50, 100, 200, 246};
    double y[] = {128, 50, 200, 50, 128};
    
    result = tileDevice->DrawPolyline(x, y, 5, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, DrawPolygon) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 1.0;
    style.brush.color = Color::Yellow().GetRGBA();
    style.brush.visible = true;
    
    double x[] = {128, 200, 200, 128, 56, 56};
    double y[] = {50, 100, 200, 250, 200, 100};
    
    result = tileDevice->DrawPolygon(x, y, 6, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, TransformOperations) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    TransformMatrix transform = TransformMatrix::CreateTranslation(50, 50);
    tileDevice->SetTransform(transform);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Magenta().GetRGBA();
    style.pen.width = 2.0;
    
    result = tileDevice->DrawRect(0, 0, 100, 100, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->ResetTransform();
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, ClipRect) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->SetClipRect(50, 50, 156, 156);
    EXPECT_TRUE(tileDevice->HasClipRect());
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Blue().GetRGBA();
    style.pen.width = 2.0;
    
    result = tileDevice->DrawRect(0, 0, 256, 256, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->ClearClipRect();
    EXPECT_FALSE(tileDevice->HasClipRect());
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, Opacity) {
    DrawResult result = tileDevice->BeginTile(0, 0, 0);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(tileDevice->GetOpacity(), 0.5);
    
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.pen.color = Color::Red().GetRGBA();
    style.pen.width = 5.0;
    
    result = tileDevice->DrawLine(0, 0, 256, 256, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndTile();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(IntegrationTileRenderTest, Antialiasing) {
    tileDevice->SetAntialiasing(true);
    EXPECT_TRUE(tileDevice->IsAntialiasingEnabled());
    
    tileDevice->SetAntialiasing(false);
    EXPECT_FALSE(tileDevice->IsAntialiasingEnabled());
}

TEST_F(IntegrationTileRenderTest, TileSize) {
    EXPECT_EQ(tileDevice->GetTileSize(), 256);
    
    tileDevice->SetTileSize(512);
    EXPECT_EQ(tileDevice->GetTileSize(), 512);
    
    tileDevice->SetTileSize(256);
}

TEST_F(IntegrationTileRenderTest, CurrentTileKey) {
    tileDevice->SetCurrentTile(1, 2, 3);
    
    TileKey key = tileDevice->GetCurrentTileKey();
    EXPECT_EQ(key.x, 1);
    EXPECT_EQ(key.y, 2);
    EXPECT_EQ(key.z, 3);
}

TEST_F(IntegrationTileRenderTest, Dpi) {
    tileDevice->SetDpi(96.0);
    EXPECT_DOUBLE_EQ(tileDevice->GetDpi(), 96.0);
    
    tileDevice->SetDpi(150.0);
    EXPECT_DOUBLE_EQ(tileDevice->GetDpi(), 150.0);
}

TEST_F(IntegrationTileRenderTest, TileFormat) {
    tileDevice->SetTileFormat("png");
    EXPECT_EQ(tileDevice->GetTileFormat(), "png");
    
    tileDevice->SetTileFormat("jpg");
    EXPECT_EQ(tileDevice->GetTileFormat(), "jpg");
}

TEST_F(IntegrationTileRenderTest, MultipleZoomLevels) {
    for (int z = 0; z <= 3; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                DrawResult result = tileDevice->BeginTile(x, y, z);
                EXPECT_EQ(result, DrawResult::kSuccess);
                
                Color fillColor(static_cast<uint8_t>(255 - z * 50), 
                               static_cast<uint8_t>(z * 50), 
                               static_cast<uint8_t>(128));
                tileDevice->Clear(fillColor);
                
                DrawStyle style;
                style.pen.color = Color::White().GetRGBA();
                style.pen.width = 1.0;
                
                tileDevice->DrawRect(10, 10, 236, 236, style);
                
                result = tileDevice->EndTile();
                EXPECT_EQ(result, DrawResult::kSuccess);
            }
        }
    }
}

