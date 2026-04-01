#include <gtest/gtest.h>
#include "ogc/draw/tile_device.h"
#include "ogc/draw/simple2d_engine.h"
#include <cmath>

using namespace ogc::draw;

class TileDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(TileDeviceTest, Construction) {
    TileDevice device(1000, 1000, 256);
    
    EXPECT_EQ(device.GetWidth(), 1000);
    EXPECT_EQ(device.GetHeight(), 1000);
    EXPECT_EQ(device.GetTileSize(), 256);
    EXPECT_EQ(device.GetType(), DeviceType::kTile);
    EXPECT_EQ(device.GetName(), "TileDevice");
}

TEST_F(TileDeviceTest, TileCount) {
    TileDevice device(1000, 1000, 256);
    
    EXPECT_EQ(device.GetTileCountX(), 4);
    EXPECT_EQ(device.GetTileCountY(), 4);
    EXPECT_EQ(device.GetTotalTileCount(), 16);
}

TEST_F(TileDeviceTest, Initialize) {
    TileDevice device(1000, 1000, 256);
    
    EXPECT_EQ(device.Initialize(), DrawResult::kSuccess);
    EXPECT_EQ(device.GetState(), DeviceState::kReady);
    EXPECT_TRUE(device.IsReady());
}

TEST_F(TileDeviceTest, Finalize) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    EXPECT_EQ(device.Finalize(), DrawResult::kSuccess);
    EXPECT_EQ(device.GetState(), DeviceState::kUninitialized);
}

TEST_F(TileDeviceTest, GetTileInfo) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    TileInfo info = device.GetTileInfo(0, 0);
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.indexX, 0);
    EXPECT_EQ(info.indexY, 0);
    EXPECT_EQ(info.pixelX, 0);
    EXPECT_EQ(info.pixelY, 0);
    EXPECT_EQ(info.width, 256);
    EXPECT_EQ(info.height, 256);
    
    TileInfo info2 = device.GetTileInfo(3, 3);
    EXPECT_TRUE(info2.isValid);
    EXPECT_EQ(info2.indexX, 3);
    EXPECT_EQ(info2.indexY, 3);
    EXPECT_EQ(info2.pixelX, 768);
    EXPECT_EQ(info2.pixelY, 768);
    EXPECT_EQ(info2.width, 232);
    EXPECT_EQ(info2.height, 232);
}

TEST_F(TileDeviceTest, GetTileInfoInvalid) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    TileInfo info = device.GetTileInfo(10, 10);
    EXPECT_FALSE(info.isValid);
}

TEST_F(TileDeviceTest, GetTileInfoAtPixel) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    TileInfo info = device.GetTileInfoAtPixel(300, 500);
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.indexX, 1);
    EXPECT_EQ(info.indexY, 1);
}

TEST_F(TileDeviceTest, GetTile) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    RasterImageDevice* tile = device.GetTile(0, 0);
    ASSERT_NE(tile, nullptr);
    EXPECT_EQ(tile->GetWidth(), 256);
    EXPECT_EQ(tile->GetHeight(), 256);
    
    RasterImageDevice* tile2 = device.GetTile(3, 3);
    ASSERT_NE(tile2, nullptr);
    EXPECT_EQ(tile2->GetWidth(), 232);
    EXPECT_EQ(tile2->GetHeight(), 232);
}

TEST_F(TileDeviceTest, GetTileInvalid) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    RasterImageDevice* tile = device.GetTile(10, 10);
    EXPECT_EQ(tile, nullptr);
}

TEST_F(TileDeviceTest, DirtyFlags) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    EXPECT_TRUE(device.IsTileDirty(0, 0));
    
    device.MarkAllTilesClean();
    EXPECT_FALSE(device.IsTileDirty(0, 0));
    
    device.SetTileDirty(1, 1, true);
    EXPECT_TRUE(device.IsTileDirty(1, 1));
    
    device.MarkAllTilesDirty();
    EXPECT_TRUE(device.IsTileDirty(0, 0));
    EXPECT_TRUE(device.IsTileDirty(3, 3));
}

TEST_F(TileDeviceTest, GetDirtyTiles) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    device.MarkAllTilesClean();
    device.SetTileDirty(0, 0, true);
    device.SetTileDirty(2, 2, true);
    
    std::vector<TileInfo> dirtyTiles = device.GetDirtyTiles();
    EXPECT_EQ(dirtyTiles.size(), 2);
}

TEST_F(TileDeviceTest, GetTilesInRect) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    std::vector<TileInfo> tiles = device.GetTilesInRect(200, 200, 300, 300);
    
    EXPECT_EQ(tiles.size(), 4);
}

TEST_F(TileDeviceTest, Clear) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    device.Clear(Color::Red());
    
    RasterImageDevice* tile = device.GetTile(0, 0);
    ASSERT_NE(tile, nullptr);
    
    Color pixel = tile->GetPixel(100, 100);
    EXPECT_GT(pixel.GetRed(), 0);
}

TEST_F(TileDeviceTest, ClearTile) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    device.ClearTile(1, 1, Color::Blue());
    
    RasterImageDevice* tile = device.GetTile(1, 1);
    ASSERT_NE(tile, nullptr);
    
    Color pixel = tile->GetPixel(100, 100);
    EXPECT_GT(pixel.GetBlue(), 0);
}

TEST_F(TileDeviceTest, DpiSetting) {
    TileDevice device(1000, 1000, 256);
    
    device.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(device.GetDpi(), 150.0);
}

TEST_F(TileDeviceTest, OverlapSetting) {
    TileDevice device(1000, 1000, 256);
    
    device.SetOverlap(16);
    EXPECT_EQ(device.GetOverlap(), 16);
}

TEST_F(TileDeviceTest, SupportedEngines) {
    TileDevice device(1000, 1000, 256);
    
    std::vector<EngineType> engines = device.GetSupportedEngineTypes();
    EXPECT_FALSE(engines.empty());
    
    EXPECT_EQ(device.GetPreferredEngineType(), EngineType::kTile);
}

TEST_F(TileDeviceTest, Capabilities) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    DeviceCapabilities caps = device.QueryCapabilities();
    EXPECT_FALSE(caps.supportsGPU);
    EXPECT_TRUE(caps.supportsMultithreading);
    EXPECT_TRUE(caps.supportsPartialUpdate);
}

TEST_F(TileDeviceTest, FeatureAvailability) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    EXPECT_TRUE(device.IsFeatureAvailable("tiling"));
    EXPECT_TRUE(device.IsFeatureAvailable("partial_update"));
    EXPECT_FALSE(device.IsFeatureAvailable("gpu"));
}

TEST_F(TileDeviceTest, DeviceRecovery) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    EXPECT_FALSE(device.IsDeviceLost());
    EXPECT_EQ(device.RecoverDevice(), DrawResult::kDeviceError);
}

TEST_F(TileDeviceTest, CreateEngine) {
    TileDevice device(1000, 1000, 256);
    device.Initialize();
    
    std::unique_ptr<DrawEngine> engine = device.CreateEngine();
    ASSERT_NE(engine, nullptr);
}

TEST_F(TileDeviceTest, NonSquareImage) {
    TileDevice device(800, 600, 256);
    device.Initialize();
    
    EXPECT_EQ(device.GetTileCountX(), 4);
    EXPECT_EQ(device.GetTileCountY(), 3);
    EXPECT_EQ(device.GetTotalTileCount(), 12);
    
    RasterImageDevice* tile = device.GetTile(3, 2);
    ASSERT_NE(tile, nullptr);
    EXPECT_EQ(tile->GetWidth(), 32);
    EXPECT_EQ(tile->GetHeight(), 88);
}

TEST_F(TileDeviceTest, InvalidDimensions) {
    TileDevice device(0, 0, 256);
    EXPECT_EQ(device.Initialize(), DrawResult::kInvalidParameter);
    EXPECT_EQ(device.GetState(), DeviceState::kError);
}

TEST_F(TileDeviceTest, SmallImage) {
    TileDevice device(100, 100, 256);
    device.Initialize();
    
    EXPECT_EQ(device.GetTileCountX(), 1);
    EXPECT_EQ(device.GetTileCountY(), 1);
    EXPECT_EQ(device.GetTotalTileCount(), 1);
    
    RasterImageDevice* tile = device.GetTile(0, 0);
    ASSERT_NE(tile, nullptr);
    EXPECT_EQ(tile->GetWidth(), 100);
    EXPECT_EQ(tile->GetHeight(), 100);
}

