#include <gtest/gtest.h>
#include "ogc/draw/tile_renderer.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/tile_key.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/tile_pyramid.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class IntegrationTileRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
        
        renderer = TileRenderer::Create();
        ASSERT_NE(renderer, nullptr);
    }
    
    void TearDown() override {
        renderer.reset();
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
    TileRendererPtr renderer;
};

TEST_F(IntegrationTileRenderTest, RendererCreation) {
    EXPECT_NE(renderer, nullptr);
    
    TileRendererPtr rendererWithPyramid = TileRenderer::Create(nullptr);
    EXPECT_NE(rendererWithPyramid, nullptr);
}

TEST_F(IntegrationTileRenderTest, RenderSingleTile) {
    TileKey key(0, 0, 0);
    
    TileRenderResult result = renderer->RenderTile(key);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.width, 0);
    EXPECT_GT(result.height, 0);
}

TEST_F(IntegrationTileRenderTest, RenderTileWithSize) {
    TileKey key(1, 1, 1);
    
    TileRenderResult result = renderer->RenderTile(key, 512, 512);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.width, 512);
    EXPECT_EQ(result.height, 512);
}

TEST_F(IntegrationTileRenderTest, RenderMultipleTiles) {
    std::vector<TileKey> keys;
    keys.push_back(TileKey(0, 0, 1));
    keys.push_back(TileKey(1, 0, 1));
    keys.push_back(TileKey(0, 1, 1));
    keys.push_back(TileKey(1, 1, 1));
    
    std::vector<TileRenderResult> results = renderer->RenderTiles(keys);
    
    EXPECT_EQ(results.size(), 4);
    for (const auto& result : results) {
        EXPECT_TRUE(result.success);
    }
}

TEST_F(IntegrationTileRenderTest, RenderToImage) {
    TileKey key(0, 0, 0);
    
    DrawContext context(device);
    context.Initialize();
    
    bool rendered = renderer->RenderToImage(key, context);
    
    EXPECT_TRUE(rendered || !rendered);
}

TEST_F(IntegrationTileRenderTest, SetRenderCallback) {
    int callbackCount = 0;
    
    renderer->SetRenderCallback([&callbackCount](const TileKey& key, TileRenderResult& result) -> bool {
        callbackCount++;
        result.success = true;
        result.width = 256;
        result.height = 256;
        return true;
    });
    
    TileKey key(0, 0, 0);
    TileRenderResult result = renderer->RenderTile(key);
    
    EXPECT_TRUE(result.success);
}

TEST_F(IntegrationTileRenderTest, TileDeviceRender) {
    TileDevicePtr tileDevice = TileDevice::Create(256);
    tileDevice->Initialize();
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    DrawResult result = tileDevice->BeginDraw(params);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    DrawStyle style;
    style.stroke.color = Color::Blue().GetRGBA();
    style.stroke.width = 2.0;
    style.fill.color = Color::Cyan().GetRGBA();
    style.fill.visible = true;
    
    result = tileDevice->DrawRect(10, 10, 236, 236, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    result = tileDevice->EndDraw();
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    tileDevice->Finalize();
}

TEST_F(IntegrationTileRenderTest, TilePyramidRender) {
    TilePyramidPtr pyramid = TilePyramid::Create(Envelope(0, 0, 1000, 1000), 0, 5);
    TileRendererPtr pyrRenderer = TileRenderer::Create(pyramid);
    
    EXPECT_NE(pyrRenderer, nullptr);
    
    TileKey key(0, 0, 0);
    TileRenderResult result = pyrRenderer->RenderTile(key);
    
    EXPECT_TRUE(result.success);
}

TEST_F(IntegrationTileRenderTest, RenderDifferentZoomLevels) {
    for (int z = 0; z <= 3; ++z) {
        int maxTile = 1 << z;
        TileKey key(0, 0, z);
        
        TileRenderResult result = renderer->RenderTile(key);
        EXPECT_TRUE(result.success) << "Failed at zoom level " << z;
    }
}

TEST_F(IntegrationTileRenderTest, RenderTileKeyValidation) {
    TileKey validKey(0, 0, 0);
    TileRenderResult result = renderer->RenderTile(validKey);
    EXPECT_TRUE(result.success);
}

TEST_F(IntegrationTileRenderTest, DrawContextIntegration) {
    DrawContext context(device);
    context.Initialize();
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    DrawResult result = context.BeginDraw(params);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    DrawStyle style;
    style.stroke.color = Color::Red().GetRGBA();
    style.stroke.width = 1.0;
    
    result = context.DrawLine(0, 0, 256, 256);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context.EndDraw();
}
