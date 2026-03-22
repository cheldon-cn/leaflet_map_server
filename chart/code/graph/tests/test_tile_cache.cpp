#include <gtest/gtest.h>
#include "ogc/draw/tile_cache.h"
#include "ogc/draw/memory_tile_cache.h"
#include "ogc/draw/tile_key.h"
#include <vector>
#include <cstdint>
#include <thread>
#include <chrono>

using namespace ogc::draw;

class MemoryTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_shared<MemoryTileCache>(1024 * 1024);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::shared_ptr<MemoryTileCache> cache;
    
    std::vector<uint8_t> CreateTestData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(i % 256);
        }
        return data;
    }
};

TEST_F(MemoryTileCacheTest, DefaultConstructor) {
    auto cache = std::make_shared<MemoryTileCache>();
    EXPECT_EQ(cache->GetMaxSize(), 100 * 1024 * 1024);
    EXPECT_EQ(cache->GetTileCount(), 0);
    EXPECT_EQ(cache->GetSize(), 0);
    EXPECT_TRUE(cache->IsEnabled());
}

TEST_F(MemoryTileCacheTest, ConstructorWithMaxSize) {
    auto cache = std::make_shared<MemoryTileCache>(1024);
    EXPECT_EQ(cache->GetMaxSize(), 1024);
}

TEST_F(MemoryTileCacheTest, PutAndGetTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_TRUE(cache->PutTile(key, data));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_TRUE(tile.IsValid());
    EXPECT_EQ(tile.data.size(), 100);
    EXPECT_EQ(tile.data, data);
}

TEST_F(MemoryTileCacheTest, PutTileData) {
    TileKey key(1, 2, 3);
    TileData tileData;
    tileData.key = key;
    tileData.data = CreateTestData(200);
    tileData.valid = true;
    
    EXPECT_TRUE(cache->PutTile(key, tileData));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData retrieved = cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), 200);
}

TEST_F(MemoryTileCacheTest, GetNonExistentTile) {
    TileKey key(99, 99, 99);
    EXPECT_FALSE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_FALSE(tile.IsValid());
}

TEST_F(MemoryTileCacheTest, RemoveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->HasTile(key));
    
    EXPECT_TRUE(cache->RemoveTile(key));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(MemoryTileCacheTest, RemoveNonExistentTile) {
    TileKey key(99, 99, 99);
    EXPECT_FALSE(cache->RemoveTile(key));
}

TEST_F(MemoryTileCacheTest, Clear) {
    TileKey key1(1, 2, 3);
    TileKey key2(4, 5, 6);
    
    cache->PutTile(key1, CreateTestData(100));
    cache->PutTile(key2, CreateTestData(200));
    
    EXPECT_EQ(cache->GetTileCount(), 2);
    
    cache->Clear();
    
    EXPECT_EQ(cache->GetTileCount(), 0);
    EXPECT_EQ(cache->GetSize(), 0);
    EXPECT_FALSE(cache->HasTile(key1));
    EXPECT_FALSE(cache->HasTile(key2));
}

TEST_F(MemoryTileCacheTest, GetTileCount) {
    EXPECT_EQ(cache->GetTileCount(), 0);
    
    cache->PutTile(TileKey(1, 2, 3), CreateTestData(100));
    EXPECT_EQ(cache->GetTileCount(), 1);
    
    cache->PutTile(TileKey(4, 5, 6), CreateTestData(200));
    EXPECT_EQ(cache->GetTileCount(), 2);
}

TEST_F(MemoryTileCacheTest, GetSize) {
    EXPECT_EQ(cache->GetSize(), 0);
    
    auto data1 = CreateTestData(100);
    auto data2 = CreateTestData(200);
    
    cache->PutTile(TileKey(1, 2, 3), data1);
    EXPECT_EQ(cache->GetSize(), 100);
    
    cache->PutTile(TileKey(4, 5, 6), data2);
    EXPECT_EQ(cache->GetSize(), 300);
}

TEST_F(MemoryTileCacheTest, SetMaxSize) {
    cache->SetMaxSize(2048);
    EXPECT_EQ(cache->GetMaxSize(), 2048);
}

TEST_F(MemoryTileCacheTest, IsFull) {
    auto smallCache = std::make_shared<MemoryTileCache>(100);
    
    EXPECT_FALSE(smallCache->IsFull());
    
    smallCache->PutTile(TileKey(1, 2, 3), CreateTestData(100));
    EXPECT_TRUE(smallCache->IsFull());
}

TEST_F(MemoryTileCacheTest, GetName) {
    EXPECT_EQ(cache->GetName(), "MemoryTileCache");
}

TEST_F(MemoryTileCacheTest, SetName) {
    cache->SetName("TestCache");
    EXPECT_EQ(cache->GetName(), "TestCache");
}

TEST_F(MemoryTileCacheTest, IsEnabled) {
    EXPECT_TRUE(cache->IsEnabled());
}

TEST_F(MemoryTileCacheTest, SetEnabled) {
    cache->SetEnabled(false);
    EXPECT_FALSE(cache->IsEnabled());
    
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->PutTile(key, data));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(MemoryTileCacheTest, Contains) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->Contains(key));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Contains(key));
}

TEST_F(MemoryTileCacheTest, Get) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    TileData tile;
    EXPECT_FALSE(cache->Get(key, tile));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Get(key, tile));
    EXPECT_TRUE(tile.IsValid());
}

TEST_F(MemoryTileCacheTest, Put) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_TRUE(cache->Put(key, data));
    EXPECT_TRUE(cache->HasTile(key));
}

TEST_F(MemoryTileCacheTest, Eviction) {
    auto smallCache = std::make_shared<MemoryTileCache>(200);
    
    smallCache->PutTile(TileKey(1, 0, 0), CreateTestData(100));
    EXPECT_TRUE(smallCache->HasTile(TileKey(1, 0, 0)));
    
    smallCache->PutTile(TileKey(1, 1, 0), CreateTestData(100));
    
    smallCache->PutTile(TileKey(1, 2, 0), CreateTestData(100));
    
    EXPECT_EQ(smallCache->GetTileCount(), 2);
}

TEST_F(MemoryTileCacheTest, SetExpirationTime) {
    cache->SetExpirationTime(60);
    EXPECT_EQ(cache->GetExpirationTime(), 60);
}

TEST_F(MemoryTileCacheTest, StaticCreate) {
    auto cache = MemoryTileCache::Create(1024);
    ASSERT_NE(cache, nullptr);
    EXPECT_EQ(cache->GetMaxSize(), 1024);
}

TEST_F(MemoryTileCacheTest, CreateMemoryCache) {
    auto cache = TileCache::CreateMemoryCache(2048);
    ASSERT_NE(cache, nullptr);
    EXPECT_EQ(cache->GetMaxSize(), 2048);
}

TEST_F(MemoryTileCacheTest, MultipleTiles) {
    for (int z = 0; z < 3; ++z) {
        for (int x = 0; x < 4; ++x) {
            for (int y = 0; y < 4; ++y) {
                TileKey key(z, x, y);
                auto data = CreateTestData(50);
                EXPECT_TRUE(cache->PutTile(key, data));
            }
        }
    }
    
    EXPECT_EQ(cache->GetTileCount(), 3 * 4 * 4);
}

TEST_F(MemoryTileCacheTest, OverwriteTile) {
    TileKey key(1, 2, 3);
    
    auto data1 = CreateTestData(100);
    cache->PutTile(key, data1);
    
    auto data2 = CreateTestData(200);
    cache->PutTile(key, data2);
    
    EXPECT_EQ(cache->GetTileCount(), 1);
    
    TileData tile = cache->GetTile(key);
    EXPECT_EQ(tile.data.size(), 200);
    EXPECT_EQ(tile.data, data2);
}

TEST_F(MemoryTileCacheTest, TileDataIsValid) {
    TileData validTile;
    validTile.data = CreateTestData(100);
    validTile.valid = true;
    EXPECT_TRUE(validTile.IsValid());
    
    TileData emptyTile;
    EXPECT_FALSE(emptyTile.IsValid());
    
    TileData invalidTile;
    invalidTile.data = CreateTestData(100);
    invalidTile.valid = false;
    EXPECT_FALSE(invalidTile.IsValid());
}

TEST_F(MemoryTileCacheTest, TileDataGetSize) {
    TileData tile;
    tile.data = CreateTestData(100);
    EXPECT_EQ(tile.GetSize(), 100);
}
