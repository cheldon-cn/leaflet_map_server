#include <gtest/gtest.h>
#include <ogc/draw/tile_device.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include <ogc/cache/tile/tile_key.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::graph;
using ogc::draw::TileDevice;
using ogc::draw::RasterImageDevice;
using ogc::draw::DrawContext;
using ogc::draw::DrawStyle;
using ogc::draw::Pen;
using ogc::draw::Color;
using ogc::draw::PixelFormat;
using ogc::draw::DrawResult;
using ogc::draw::DeviceType;
using ogc::draw::TileInfo;
using ogc::Envelope;

class IntegrationTileRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_tileDevice = std::make_unique<TileDevice>(512, 512, 256);
        ASSERT_NE(m_tileDevice, nullptr);
        
        DrawResult result = m_tileDevice->Initialize();
        EXPECT_EQ(result, DrawResult::kSuccess);
    }
    
    void TearDown() override {
        if (m_tileDevice) {
            m_tileDevice->Finalize();
        }
    }
    
    std::unique_ptr<TileDevice> m_tileDevice;
};

TEST_F(IntegrationTileRenderTest, DeviceCreation) {
    EXPECT_NE(m_tileDevice, nullptr);
    EXPECT_EQ(m_tileDevice->GetType(), DeviceType::kTile);
    EXPECT_EQ(m_tileDevice->GetTileSize(), 256);
}

TEST_F(IntegrationTileRenderTest, DeviceInitialization) {
    EXPECT_TRUE(m_tileDevice->IsReady());
}

TEST_F(IntegrationTileRenderTest, TileInfo) {
    TileInfo info = m_tileDevice->GetTileInfo(0, 0);
    EXPECT_EQ(info.indexX, 0);
    EXPECT_EQ(info.indexY, 0);
    EXPECT_EQ(info.width, 256);
    EXPECT_EQ(info.height, 256);
}

TEST_F(IntegrationTileRenderTest, TileCount) {
    EXPECT_EQ(m_tileDevice->GetTileCountX(), 2);
    EXPECT_EQ(m_tileDevice->GetTileCountY(), 2);
    EXPECT_EQ(m_tileDevice->GetTotalTileCount(), 4);
}

TEST_F(IntegrationTileRenderTest, GetTile) {
    RasterImageDevice* tile = m_tileDevice->GetTile(0, 0);
    EXPECT_NE(tile, nullptr);
    
    tile = m_tileDevice->GetTile(1, 1);
    EXPECT_NE(tile, nullptr);
}

TEST_F(IntegrationTileRenderTest, TileDirty) {
    m_tileDevice->SetTileDirty(0, 0, true);
    EXPECT_TRUE(m_tileDevice->IsTileDirty(0, 0));
    
    m_tileDevice->SetTileDirty(0, 0, false);
    EXPECT_FALSE(m_tileDevice->IsTileDirty(0, 0));
}

TEST_F(IntegrationTileRenderTest, MarkAllTilesDirty) {
    m_tileDevice->MarkAllTilesDirty();
    
    auto dirtyTiles = m_tileDevice->GetDirtyTiles();
    EXPECT_EQ(dirtyTiles.size(), 4);
}

TEST_F(IntegrationTileRenderTest, MarkAllTilesClean) {
    m_tileDevice->MarkAllTilesDirty();
    m_tileDevice->MarkAllTilesClean();
    
    auto dirtyTiles = m_tileDevice->GetDirtyTiles();
    EXPECT_EQ(dirtyTiles.size(), 0);
}

TEST_F(IntegrationTileRenderTest, Clear) {
    m_tileDevice->Clear(Color::White());
    
    RasterImageDevice* tile = m_tileDevice->GetTile(0, 0);
    ASSERT_NE(tile, nullptr);
}

TEST_F(IntegrationTileRenderTest, ClearTile) {
    m_tileDevice->ClearTile(0, 0, Color::Red());
    
    RasterImageDevice* tile = m_tileDevice->GetTile(0, 0);
    ASSERT_NE(tile, nullptr);
}

TEST_F(IntegrationTileRenderTest, ValidTileIndex) {
    EXPECT_TRUE(m_tileDevice->IsValidTileIndex(0, 0));
    EXPECT_TRUE(m_tileDevice->IsValidTileIndex(1, 1));
    EXPECT_FALSE(m_tileDevice->IsValidTileIndex(2, 0));
    EXPECT_FALSE(m_tileDevice->IsValidTileIndex(0, 2));
}

TEST_F(IntegrationTileRenderTest, TileInfoAtPixel) {
    TileInfo info = m_tileDevice->GetTileInfoAtPixel(100, 100);
    EXPECT_EQ(info.indexX, 0);
    EXPECT_EQ(info.indexY, 0);
    
    info = m_tileDevice->GetTileInfoAtPixel(300, 300);
    EXPECT_EQ(info.indexX, 1);
    EXPECT_EQ(info.indexY, 1);
}

TEST_F(IntegrationTileRenderTest, TilesInRect) {
    auto tiles = m_tileDevice->GetTilesInRect(0, 0, 256, 256);
    EXPECT_EQ(tiles.size(), 1);
    
    tiles = m_tileDevice->GetTilesInRect(0, 0, 512, 512);
    EXPECT_EQ(tiles.size(), 4);
}

TEST_F(IntegrationTileRenderTest, Overlap) {
    m_tileDevice->SetOverlap(10);
    EXPECT_EQ(m_tileDevice->GetOverlap(), 10);
}

TEST_F(IntegrationTileRenderTest, Dpi) {
    m_tileDevice->SetDpi(96.0);
    EXPECT_DOUBLE_EQ(m_tileDevice->GetDpi(), 96.0);
    
    m_tileDevice->SetDpi(150.0);
    EXPECT_DOUBLE_EQ(m_tileDevice->GetDpi(), 150.0);
}

TEST_F(IntegrationTileRenderTest, GetWidthHeight) {
    EXPECT_EQ(m_tileDevice->GetWidth(), 512);
    EXPECT_EQ(m_tileDevice->GetHeight(), 512);
}

TEST_F(IntegrationTileRenderTest, RenderWithRasterDevice) {
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    ASSERT_NE(device, nullptr);
    device->Initialize();
    
    auto context = DrawContext::Create(device.get());
    ASSERT_NE(context, nullptr);
    
    context->Begin();
    context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Red(), 2.0);
    context->SetStyle(style);
    
    DrawResult result = context->DrawRect(10, 10, 236, 236);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
}
