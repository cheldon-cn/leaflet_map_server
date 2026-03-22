#include <gtest/gtest.h>
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

class IntegrationDiskCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = GetTempDirectory() + "ogc_test_cache";
        MakeDir(tempDir);
        
        cache = DiskTileCache::Create(tempDir);
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
        RemoveDir(tempDir);
    }
    
    std::vector<uint8_t> CreateTestData(size_t size = 256 * 256 * 4) {
        return std::vector<uint8_t>(size, 128);
    }
    
    std::shared_ptr<DiskTileCache> cache;
    std::shared_ptr<RasterImageDevice> device;
    std::string tempDir;
};

TEST_F(IntegrationDiskCacheTest, CacheCreation) {
    EXPECT_NE(cache, nullptr);
}

TEST_F(IntegrationDiskCacheTest, AddAndRetrieveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData();
    
    bool added = cache->PutTile(key, data);
    EXPECT_TRUE(added);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), data.size());
}

TEST_F(IntegrationDiskCacheTest, CacheMiss) {
    TileKey key(99, 99, 99);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationDiskCacheTest, RemoveTile) {
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    
    cache->PutTile(key, data);
    
    bool removed = cache->RemoveTile(key);
    EXPECT_TRUE(removed);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationDiskCacheTest, ClearCache) {
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        cache->PutTile(key, CreateTestData());
    }
    
    cache->Clear();
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        TileData data = cache->GetTile(key);
        EXPECT_FALSE(data.IsValid());
    }
}

TEST_F(IntegrationDiskCacheTest, HasTile) {
    TileKey key(1, 1, 1);
    
    EXPECT_FALSE(cache->HasTile(key));
    
    cache->PutTile(key, CreateTestData());
    
    EXPECT_TRUE(cache->HasTile(key));
}

TEST_F(IntegrationDiskCacheTest, MultipleZoomLevels) {
    for (int z = 0; z <= 2; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                TileKey key(x, y, z);
                cache->PutTile(key, CreateTestData());
            }
        }
    }
    
    TileData data;
    EXPECT_TRUE(cache->HasTile(TileKey(0, 0, 0)));
    EXPECT_TRUE(cache->HasTile(TileKey(0, 0, 1)));
    EXPECT_TRUE(cache->HasTile(TileKey(1, 1, 2)));
}

TEST_F(IntegrationDiskCacheTest, CacheWithDevice) {
    TileDevicePtr tileDevice = TileDevice::Create(256);
    tileDevice->Initialize();
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    tileDevice->BeginDraw(params);
    tileDevice->Clear(Color::White());
    
    DrawStyle style;
    style.stroke.color = Color::Green().GetRGBA();
    style.stroke.width = 2.0;
    
    tileDevice->DrawRect(10, 10, 236, 236, style);
    tileDevice->EndDraw();
    
    TileData tileData = tileDevice->GetTileData();
    
    TileKey key(2, 3, 4);
    cache->PutTile(key, tileData.data);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    
    tileDevice->Finalize();
}

TEST_F(IntegrationDiskCacheTest, LargeTileData) {
    TileKey key(0, 0, 0);
    auto data = CreateTestData(512 * 512 * 4);
    
    cache->PutTile(key, data);
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), data.size());
}

TEST_F(IntegrationDiskCacheTest, SetGetName) {
    cache->SetName("test_disk_cache");
    EXPECT_EQ(cache->GetName(), "test_disk_cache");
}

TEST_F(IntegrationDiskCacheTest, SetEnabled) {
    cache->SetEnabled(true);
    EXPECT_TRUE(cache->IsEnabled());
    
    cache->SetEnabled(false);
    EXPECT_FALSE(cache->IsEnabled());
}

TEST_F(IntegrationDiskCacheTest, GetCachePath) {
    EXPECT_EQ(cache->GetCachePath(), tempDir);
}

TEST_F(IntegrationDiskCacheTest, SetCompressionEnabled) {
    cache->SetCompressionEnabled(true);
    EXPECT_TRUE(cache->IsCompressionEnabled());
    
    cache->SetCompressionEnabled(false);
    EXPECT_FALSE(cache->IsCompressionEnabled());
}

TEST_F(IntegrationDiskCacheTest, SetExpirationTime) {
    cache->SetExpirationTime(3600);
    EXPECT_EQ(cache->GetExpirationTime(), 3600);
}

TEST_F(IntegrationDiskCacheTest, Flush) {
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        cache->PutTile(key, CreateTestData());
    }
    
    cache->Flush();
}

TEST_F(IntegrationDiskCacheTest, Cleanup) {
    cache->SetExpirationTime(1);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        cache->PutTile(key, CreateTestData());
    }
    
    cache->Cleanup();
}
