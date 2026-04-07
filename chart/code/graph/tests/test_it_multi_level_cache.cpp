#include <gtest/gtest.h>
#include <ogc/cache/tile/multi_level_tile_cache.h>
#include <ogc/cache/tile/memory_tile_cache.h>
#include <ogc/cache/tile/disk_tile_cache.h>
#include <ogc/cache/tile/tile_key.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace ogc::cache;
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
        
        auto memoryCache = MemoryTileCache::Create();
        auto diskCache = DiskTileCache::Create(tempDir);
        
        std::vector<TileCachePtr> caches;
        caches.push_back(memoryCache);
        caches.push_back(diskCache);
        
        multiCache = MultiLevelTileCache::Create(caches);
        ASSERT_NE(multiCache, nullptr);
        
        device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
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
    
    std::vector<uint8_t> CreateTestData(size_t size = 256 * 256 * 4) {
        return std::vector<uint8_t>(size, 128);
    }
    
    MultiLevelTileCachePtr multiCache;
    std::shared_ptr<RasterImageDevice> device;
    std::string tempDir;
};

TEST_F(IntegrationMultiLevelCacheTest, CacheCreation) {
    EXPECT_NE(multiCache, nullptr);
}

TEST_F(IntegrationMultiLevelCacheTest, AddAndRetrieveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData();
    
    multiCache->PutTile(key, data);
    
    TileData retrieved = multiCache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), data.size());
}

TEST_F(IntegrationMultiLevelCacheTest, CacheMiss) {
    TileKey key(99, 99, 99);
    
    TileData retrieved = multiCache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationMultiLevelCacheTest, RemoveTile) {
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    
    multiCache->PutTile(key, data);
    
    bool removed = multiCache->RemoveTile(key);
    EXPECT_TRUE(removed);
    
    TileData retrieved = multiCache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationMultiLevelCacheTest, ClearAllLevels) {
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        multiCache->PutTile(key, CreateTestData());
    }
    
    multiCache->Clear();
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        TileData data = multiCache->GetTile(key);
        EXPECT_FALSE(data.IsValid());
    }
}

TEST_F(IntegrationMultiLevelCacheTest, HasTile) {
    TileKey key(1, 1, 1);
    
    EXPECT_FALSE(multiCache->HasTile(key));
    
    multiCache->PutTile(key, CreateTestData());
    
    EXPECT_TRUE(multiCache->HasTile(key));
}

TEST_F(IntegrationMultiLevelCacheTest, GetCacheCount) {
    EXPECT_EQ(multiCache->GetCacheCount(), 2);
}

TEST_F(IntegrationMultiLevelCacheTest, MultipleZoomLevels) {
    for (int z = 0; z <= 2; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                TileKey key(x, y, z);
                multiCache->PutTile(key, CreateTestData());
            }
        }
    }
    
    TileData data;
    EXPECT_TRUE(multiCache->HasTile(TileKey(0, 0, 0)));
    EXPECT_TRUE(multiCache->HasTile(TileKey(0, 0, 1)));
    EXPECT_TRUE(multiCache->HasTile(TileKey(1, 1, 2)));
}

TEST_F(IntegrationMultiLevelCacheTest, CacheWithDevice) {
    auto context = DrawContext::Create(device.get());
    ASSERT_NE(context, nullptr);
    
    context->Begin();
    context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Magenta(), 2.0);
    context->SetStyle(style);
    
    context->DrawRect(10, 10, 236, 236);
    context->End();
    
    const uint8_t* imageData = device->GetPixelData();
    size_t dataSize = device->GetDataSize();
    std::vector<uint8_t> imageDataVec(imageData, imageData + dataSize);
    
    TileKey key(3, 4, 5);
    multiCache->PutTile(key, imageDataVec);
    
    TileData retrieved = multiCache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
}

TEST_F(IntegrationMultiLevelCacheTest, ThreeLevelCache) {
    auto level1 = MemoryTileCache::Create();
    auto level2 = MemoryTileCache::Create();
    auto level3 = MemoryTileCache::Create();
    
    std::vector<TileCachePtr> caches;
    caches.push_back(level1);
    caches.push_back(level2);
    caches.push_back(level3);
    
    auto threeLevelCache = MultiLevelTileCache::Create(caches);
    
    EXPECT_EQ(threeLevelCache->GetCacheCount(), 3);
    
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    
    threeLevelCache->PutTile(key, data);
    
    TileData retrieved = threeLevelCache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
}

TEST_F(IntegrationMultiLevelCacheTest, SetGetName) {
    multiCache->SetName("test_multi_cache");
    EXPECT_EQ(multiCache->GetName(), "test_multi_cache");
}

TEST_F(IntegrationMultiLevelCacheTest, SetEnabled) {
    multiCache->SetEnabled(true);
    EXPECT_TRUE(multiCache->IsEnabled());
    
    multiCache->SetEnabled(false);
    EXPECT_FALSE(multiCache->IsEnabled());
}

TEST_F(IntegrationMultiLevelCacheTest, SetPromoteOnHit) {
    multiCache->SetPromoteOnHit(true);
    EXPECT_TRUE(multiCache->IsPromoteOnHit());
    
    multiCache->SetPromoteOnHit(false);
    EXPECT_FALSE(multiCache->IsPromoteOnHit());
}

TEST_F(IntegrationMultiLevelCacheTest, SetWriteThrough) {
    multiCache->SetWriteThrough(true);
    EXPECT_TRUE(multiCache->IsWriteThrough());
    
    multiCache->SetWriteThrough(false);
    EXPECT_FALSE(multiCache->IsWriteThrough());
}

TEST_F(IntegrationMultiLevelCacheTest, SetWriteBack) {
    multiCache->SetWriteBack(true);
    EXPECT_TRUE(multiCache->IsWriteBack());
    
    multiCache->SetWriteBack(false);
    EXPECT_FALSE(multiCache->IsWriteBack());
}

TEST_F(IntegrationMultiLevelCacheTest, Flush) {
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        multiCache->PutTile(key, CreateTestData());
    }
    
    multiCache->Flush();
}

TEST_F(IntegrationMultiLevelCacheTest, GetCache) {
    TileCachePtr cache0 = multiCache->GetCache(0);
    EXPECT_NE(cache0, nullptr);
    
    TileCachePtr cache1 = multiCache->GetCache(1);
    EXPECT_NE(cache1, nullptr);
}

TEST_F(IntegrationMultiLevelCacheTest, AddCache) {
    auto newCache = MemoryTileCache::Create();
    
    multiCache->AddCache(newCache);
    
    EXPECT_EQ(multiCache->GetCacheCount(), 3);
}
