#include <gtest/gtest.h>
#include "ogc/draw/multi_level_tile_cache.h"
#include "ogc/draw/tile_cache.h"
#include "ogc/draw/disk_tile_cache.h"
#include "ogc/draw/tile_key.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/envelope.h"
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace ogc::draw;
using ogc::Envelope;

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
    
    bool MakeDir(const std::string& path) {
#ifdef _WIN32
        return CreateDirectoryA(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
    }
    
    void RemoveDir(const std::string& path) {
#ifdef _WIN32
        RemoveDirectoryA(path.c_str());
#else
        rmdir(path.c_str());
#endif
    }
}

class IntegrationMultiLevelCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = GetTempDirectory() + "ogc_multi_cache";
        MakeDir(tempDir);
        
        auto memoryCache = std::make_shared<MemoryTileCache>();
        auto diskCache = std::make_shared<DiskTileCache>(tempDir);
        
        std::vector<TileCachePtr> caches;
        caches.push_back(memoryCache);
        caches.push_back(diskCache);
        
        multiCache = std::make_shared<MultiLevelTileCache>(caches);
        ASSERT_NE(multiCache, nullptr);
        
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
    }
    
    void TearDown() override {
        multiCache.reset();
        if (device) {
            device->Finalize();
        }
        RemoveDir(tempDir);
    }
    
    TileData CreateTestTileData(int width = 256, int height = 256) {
        TileData data;
        data.width = width;
        data.height = height;
        data.channels = 4;
        data.data.resize(width * height * 4, 128);
        return data;
    }
    
    std::shared_ptr<MultiLevelTileCache> multiCache;
    std::shared_ptr<RasterImageDevice> device;
    std::string tempDir;
};

TEST_F(IntegrationMultiLevelCacheTest, CacheCreation) {
    EXPECT_NE(multiCache, nullptr);
}

TEST_F(IntegrationMultiLevelCacheTest, AddAndRetrieveTile) {
    TileKey key(1, 2, 3);
    TileData data = CreateTestTileData();
    
    multiCache->Add(key, data);
    
    TileData retrieved;
    bool found = multiCache->Get(key, retrieved);
    
    EXPECT_TRUE(found);
    EXPECT_EQ(retrieved.width, data.width);
    EXPECT_EQ(retrieved.height, data.height);
}

TEST_F(IntegrationMultiLevelCacheTest, CacheMiss) {
    TileKey key(99, 99, 99);
    TileData data;
    
    bool found = multiCache->Get(key, data);
    EXPECT_FALSE(found);
}

TEST_F(IntegrationMultiLevelCacheTest, RemoveTile) {
    TileKey key(0, 0, 0);
    TileData data = CreateTestTileData();
    
    multiCache->Add(key, data);
    
    bool removed = multiCache->Remove(key);
    EXPECT_TRUE(removed);
    
    TileData retrieved;
    bool found = multiCache->Get(key, retrieved);
    EXPECT_FALSE(found);
}

TEST_F(IntegrationMultiLevelCacheTest, ClearAllLevels) {
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        multiCache->Add(key, CreateTestTileData());
    }
    
    multiCache->Clear();
    
    TileData data;
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        EXPECT_FALSE(multiCache->Get(key, data));
    }
}

TEST_F(IntegrationMultiLevelCacheTest, HasTile) {
    TileKey key(1, 1, 1);
    
    EXPECT_FALSE(multiCache->Has(key));
    
    multiCache->Add(key, CreateTestTileData());
    
    EXPECT_TRUE(multiCache->Has(key));
}

TEST_F(IntegrationMultiLevelCacheTest, LevelFallback) {
    auto memoryOnly = std::make_shared<MemoryTileCache>();
    auto diskOnly = std::make_shared<DiskTileCache>(tempDir + "_fallback");
    MakeDir(tempDir + "_fallback");
    
    TileKey key(5, 5, 5);
    TileData data = CreateTestTileData();
    
    diskOnly->Add(key, data);
    
    std::vector<TileCachePtr> caches;
    caches.push_back(memoryOnly);
    caches.push_back(diskOnly);
    
    auto cache = std::make_shared<MultiLevelTileCache>(caches);
    
    TileData retrieved;
    bool found = cache->Get(key, retrieved);
    
    EXPECT_TRUE(found);
    
    RemoveDir(tempDir + "_fallback");
}

TEST_F(IntegrationMultiLevelCacheTest, MultipleZoomLevels) {
    for (int z = 0; z <= 2; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                TileKey key(x, y, z);
                multiCache->Add(key, CreateTestTileData());
            }
        }
    }
    
    TileData data;
    EXPECT_TRUE(multiCache->Get(TileKey(0, 0, 0), data));
    EXPECT_TRUE(multiCache->Get(TileKey(0, 0, 1), data));
    EXPECT_TRUE(multiCache->Get(TileKey(1, 1, 2), data));
}

TEST_F(IntegrationMultiLevelCacheTest, CacheWithDevice) {
    TileDevicePtr tileDevice = TileDevice::Create(256);
    tileDevice->Initialize();
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    tileDevice->BeginDraw(params);
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Magenta().GetRGBA();
    style.stroke.width = 2.0;
    
    tileDevice->DrawRect(10, 10, 236, 236, style);
    tileDevice->EndDraw();
    
    TileData tileData = tileDevice->GetCurrentTileData();
    
    TileKey key(3, 4, 5);
    multiCache->Add(key, tileData);
    
    TileData retrieved;
    bool found = multiCache->Get(key, retrieved);
    EXPECT_TRUE(found);
    
    tileDevice->Finalize();
}

TEST_F(IntegrationMultiLevelCacheTest, GetCacheCount) {
    EXPECT_EQ(multiCache->GetLevelCount(), 2);
}

TEST_F(IntegrationMultiLevelCacheTest, ThreeLevelCache) {
    auto level1 = std::make_shared<MemoryTileCache>();
    auto level2 = std::make_shared<MemoryTileCache>();
    auto level3 = std::make_shared<MemoryTileCache>();
    
    std::vector<TileCachePtr> caches;
    caches.push_back(level1);
    caches.push_back(level2);
    caches.push_back(level3);
    
    auto threeLevelCache = std::make_shared<MultiLevelTileCache>(caches);
    
    EXPECT_EQ(threeLevelCache->GetLevelCount(), 3);
    
    TileKey key(0, 0, 0);
    TileData data = CreateTestTileData();
    
    threeLevelCache->Add(key, data);
    
    TileData retrieved;
    bool found = threeLevelCache->Get(key, retrieved);
    EXPECT_TRUE(found);
}

TEST_F(IntegrationMultiLevelCacheTest, PerformanceTest) {
    const int tileCount = 100;
    
    for (int i = 0; i < tileCount; ++i) {
        TileKey key(i % 10, i / 10, 5);
        multiCache->Add(key, CreateTestTileData());
    }
    
    int hitCount = 0;
    TileData data;
    for (int i = 0; i < tileCount; ++i) {
        TileKey key(i % 10, i / 10, 5);
        if (multiCache->Get(key, data)) {
            hitCount++;
        }
    }
    
    EXPECT_EQ(hitCount, tileCount);
}
