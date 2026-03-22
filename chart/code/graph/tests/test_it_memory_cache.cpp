#include <gtest/gtest.h>
#include "ogc/draw/memory_tile_cache.h"
#include "ogc/draw/tile_key.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class IntegrationMemoryCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = MemoryTileCache::Create(100 * 1024 * 1024);
        ASSERT_NE(cache, nullptr);
        
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
    }
    
    void TearDown() override {
        cache.reset();
        if (device) {
            device->Finalize();
        }
    }
    
    std::vector<uint8_t> CreateTestData(size_t size = 256 * 256 * 4) {
        return std::vector<uint8_t>(size, 128);
    }
    
    std::shared_ptr<MemoryTileCache> cache;
    std::shared_ptr<RasterImageDevice> device;
};

TEST_F(IntegrationMemoryCacheTest, CacheCreation) {
    EXPECT_NE(cache, nullptr);
    EXPECT_EQ(cache->GetTileCount(), 0);
}

TEST_F(IntegrationMemoryCacheTest, AddAndRetrieveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData();
    
    bool added = cache->PutTile(key, data);
    EXPECT_TRUE(added);
    EXPECT_EQ(cache->GetTileCount(), 1);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), data.size());
}

TEST_F(IntegrationMemoryCacheTest, CacheMiss) {
    TileKey key(99, 99, 99);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationMemoryCacheTest, RemoveTile) {
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    
    cache->PutTile(key, data);
    EXPECT_EQ(cache->GetTileCount(), 1);
    
    bool removed = cache->RemoveTile(key);
    EXPECT_TRUE(removed);
    EXPECT_EQ(cache->GetTileCount(), 0);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationMemoryCacheTest, ClearCache) {
    for (int i = 0; i < 10; ++i) {
        TileKey key(i, 0, 5);
        cache->PutTile(key, CreateTestData());
    }
    
    EXPECT_EQ(cache->GetTileCount(), 10);
    
    cache->Clear();
    EXPECT_EQ(cache->GetTileCount(), 0);
}

TEST_F(IntegrationMemoryCacheTest, CacheMaxSize) {
    cache->SetMaxSize(1024);
    EXPECT_EQ(cache->GetMaxSize(), 1024);
    
    for (int i = 0; i < 10; ++i) {
        TileKey key(i, 0, 5);
        cache->PutTile(key, CreateTestData(256));
    }
    
    EXPECT_TRUE(cache->IsFull() || cache->GetSize() <= cache->GetMaxSize());
}

TEST_F(IntegrationMemoryCacheTest, HasTile) {
    TileKey key(1, 1, 1);
    
    EXPECT_FALSE(cache->HasTile(key));
    
    cache->PutTile(key, CreateTestData());
    
    EXPECT_TRUE(cache->HasTile(key));
}

TEST_F(IntegrationMemoryCacheTest, MultipleZoomLevels) {
    for (int z = 0; z <= 3; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                TileKey key(x, y, z);
                cache->PutTile(key, CreateTestData());
            }
        }
    }
    
    EXPECT_EQ(cache->GetTileCount(), 1 + 4 + 16 + 64);
}

TEST_F(IntegrationMemoryCacheTest, CacheWithDevice) {
    TileDevicePtr tileDevice = TileDevice::Create(256);
    tileDevice->Initialize();
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    tileDevice->BeginDraw(params);
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Blue().GetRGBA();
    style.stroke.width = 2.0;
    
    tileDevice->DrawRect(10, 10, 236, 236, style);
    tileDevice->EndDraw();
    
    TileData tileData = tileDevice->GetTileData();
    
    TileKey key(0, 0, 0);
    cache->PutTile(key, tileData.data);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    
    tileDevice->Finalize();
}

TEST_F(IntegrationMemoryCacheTest, SetGetName) {
    cache->SetName("test_memory_cache");
    EXPECT_EQ(cache->GetName(), "test_memory_cache");
}

TEST_F(IntegrationMemoryCacheTest, SetEnabled) {
    cache->SetEnabled(true);
    EXPECT_TRUE(cache->IsEnabled());
    
    cache->SetEnabled(false);
    EXPECT_FALSE(cache->IsEnabled());
}

TEST_F(IntegrationMemoryCacheTest, SetExpirationTime) {
    cache->SetExpirationTime(3600);
    EXPECT_EQ(cache->GetExpirationTime(), 3600);
}

TEST_F(IntegrationMemoryCacheTest, GetSize) {
    auto data = CreateTestData(1024);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        cache->PutTile(key, data);
    }
    
    EXPECT_GT(cache->GetSize(), 0);
}

TEST_F(IntegrationMemoryCacheTest, Cleanup) {
    cache->SetExpirationTime(1);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        cache->PutTile(key, CreateTestData());
    }
    
    cache->Cleanup();
}
