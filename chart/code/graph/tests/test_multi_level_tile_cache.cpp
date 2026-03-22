#include <gtest/gtest.h>
#include "ogc/draw/multi_level_tile_cache.h"
#include "ogc/draw/memory_tile_cache.h"
#include "ogc/draw/tile_key.h"
#include <vector>
#include <cstdint>

using namespace ogc::draw;

class MultiLevelTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto l1Cache = std::make_shared<MemoryTileCache>(1024);
        auto l2Cache = std::make_shared<MemoryTileCache>(4096);
        
        cache = std::make_shared<MultiLevelTileCache>();
        cache->AddCache(l1Cache);
        cache->AddCache(l2Cache);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::shared_ptr<MultiLevelTileCache> cache;
    
    std::vector<uint8_t> CreateTestData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(i % 256);
        }
        return data;
    }
};

TEST_F(MultiLevelTileCacheTest, Constructor) {
    auto emptyCache = std::make_shared<MultiLevelTileCache>();
    EXPECT_EQ(emptyCache->GetCacheCount(), 0);
    EXPECT_TRUE(emptyCache->IsEnabled());
}

TEST_F(MultiLevelTileCacheTest, ConstructorWithCaches) {
    std::vector<TileCachePtr> caches;
    caches.push_back(std::make_shared<MemoryTileCache>(1024));
    caches.push_back(std::make_shared<MemoryTileCache>(2048));
    
    auto multiCache = std::make_shared<MultiLevelTileCache>(caches);
    EXPECT_EQ(multiCache->GetCacheCount(), 2);
}

TEST_F(MultiLevelTileCacheTest, AddCache) {
    auto newCache = std::make_shared<MemoryTileCache>(8192);
    cache->AddCache(newCache);
    EXPECT_EQ(cache->GetCacheCount(), 3);
}

TEST_F(MultiLevelTileCacheTest, RemoveCacheByIndex) {
    EXPECT_EQ(cache->GetCacheCount(), 2);
    cache->RemoveCache(0);
    EXPECT_EQ(cache->GetCacheCount(), 1);
}

TEST_F(MultiLevelTileCacheTest, RemoveCacheByName) {
    cache->GetCache(0)->SetName("L1Cache");
    cache->RemoveCache("L1Cache");
    EXPECT_EQ(cache->GetCacheCount(), 1);
}

TEST_F(MultiLevelTileCacheTest, ClearCaches) {
    cache->ClearCaches();
    EXPECT_EQ(cache->GetCacheCount(), 0);
}

TEST_F(MultiLevelTileCacheTest, GetCacheCount) {
    EXPECT_EQ(cache->GetCacheCount(), 2);
}

TEST_F(MultiLevelTileCacheTest, GetCacheByIndex) {
    auto l1Cache = cache->GetCache(0);
    ASSERT_NE(l1Cache, nullptr);
    EXPECT_EQ(l1Cache->GetMaxSize(), 1024);
}

TEST_F(MultiLevelTileCacheTest, GetCacheByName) {
    cache->GetCache(0)->SetName("L1Cache");
    auto foundCache = cache->GetCache("L1Cache");
    ASSERT_NE(foundCache, nullptr);
    EXPECT_EQ(foundCache->GetName(), "L1Cache");
}

TEST_F(MultiLevelTileCacheTest, GetCaches) {
    auto caches = cache->GetCaches();
    EXPECT_EQ(caches.size(), 2);
}

TEST_F(MultiLevelTileCacheTest, PutAndGetTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_TRUE(cache->PutTile(key, data));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_TRUE(tile.IsValid());
    EXPECT_EQ(tile.data.size(), 100);
}

TEST_F(MultiLevelTileCacheTest, GetNonExistentTile) {
    TileKey key(99, 99, 99);
    EXPECT_FALSE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_FALSE(tile.IsValid());
}

TEST_F(MultiLevelTileCacheTest, RemoveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->HasTile(key));
    
    EXPECT_TRUE(cache->RemoveTile(key));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(MultiLevelTileCacheTest, Clear) {
    TileKey key1(1, 2, 3);
    TileKey key2(4, 5, 6);
    
    cache->PutTile(key1, CreateTestData(100));
    cache->PutTile(key2, CreateTestData(200));
    
    cache->Clear();
    
    EXPECT_EQ(cache->GetTileCount(), 0);
}

TEST_F(MultiLevelTileCacheTest, GetTileCount) {
    EXPECT_EQ(cache->GetTileCount(), 0);
    
    cache->PutTile(TileKey(1, 2, 3), CreateTestData(100));
    EXPECT_GE(cache->GetTileCount(), 1);
}

TEST_F(MultiLevelTileCacheTest, GetSize) {
    EXPECT_EQ(cache->GetSize(), 0);
    
    cache->PutTile(TileKey(1, 2, 3), CreateTestData(100));
    EXPECT_GE(cache->GetSize(), 100);
}

TEST_F(MultiLevelTileCacheTest, SetMaxSize) {
    cache->SetMaxSize(8192);
    EXPECT_EQ(cache->GetMaxSize(), 8192);
}

TEST_F(MultiLevelTileCacheTest, GetName) {
    EXPECT_EQ(cache->GetName(), "MultiLevelTileCache");
}

TEST_F(MultiLevelTileCacheTest, SetName) {
    cache->SetName("TestMultiCache");
    EXPECT_EQ(cache->GetName(), "TestMultiCache");
}

TEST_F(MultiLevelTileCacheTest, IsEnabled) {
    EXPECT_TRUE(cache->IsEnabled());
}

TEST_F(MultiLevelTileCacheTest, SetEnabled) {
    cache->SetEnabled(false);
    EXPECT_FALSE(cache->IsEnabled());
    
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->PutTile(key, data));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(MultiLevelTileCacheTest, SetPromoteOnHit) {
    cache->SetPromoteOnHit(true);
    EXPECT_TRUE(cache->IsPromoteOnHit());
    
    cache->SetPromoteOnHit(false);
    EXPECT_FALSE(cache->IsPromoteOnHit());
}

TEST_F(MultiLevelTileCacheTest, SetWriteThrough) {
    cache->SetWriteThrough(true);
    EXPECT_TRUE(cache->IsWriteThrough());
    
    cache->SetWriteThrough(false);
    EXPECT_FALSE(cache->IsWriteThrough());
}

TEST_F(MultiLevelTileCacheTest, SetWriteBack) {
    cache->SetWriteBack(true);
    EXPECT_TRUE(cache->IsWriteBack());
    
    cache->SetWriteBack(false);
    EXPECT_FALSE(cache->IsWriteBack());
}

TEST_F(MultiLevelTileCacheTest, Flush) {
    TileKey key(1, 2, 3);
    cache->PutTile(key, CreateTestData(100));
    
    EXPECT_NO_THROW(cache->Flush());
}

TEST_F(MultiLevelTileCacheTest, StaticCreate) {
    auto newCache = MultiLevelTileCache::Create();
    ASSERT_NE(newCache, nullptr);
    EXPECT_EQ(newCache->GetCacheCount(), 0);
}

TEST_F(MultiLevelTileCacheTest, StaticCreateWithCaches) {
    std::vector<TileCachePtr> caches;
    caches.push_back(std::make_shared<MemoryTileCache>(1024));
    
    auto newCache = MultiLevelTileCache::Create(caches);
    ASSERT_NE(newCache, nullptr);
    EXPECT_EQ(newCache->GetCacheCount(), 1);
}

TEST_F(MultiLevelTileCacheTest, Contains) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->Contains(key));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Contains(key));
}

TEST_F(MultiLevelTileCacheTest, Get) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    TileData tile;
    EXPECT_FALSE(cache->Get(key, tile));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Get(key, tile));
    EXPECT_TRUE(tile.IsValid());
}

TEST_F(MultiLevelTileCacheTest, MultiLevelLookup) {
    auto l1Cache = std::make_shared<MemoryTileCache>(1024);
    auto l2Cache = std::make_shared<MemoryTileCache>(4096);
    
    auto multiCache = std::make_shared<MultiLevelTileCache>();
    multiCache->AddCache(l1Cache);
    multiCache->AddCache(l2Cache);
    
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    l2Cache->PutTile(key, data);
    
    EXPECT_TRUE(multiCache->HasTile(key));
    
    TileData tile = multiCache->GetTile(key);
    EXPECT_TRUE(tile.IsValid());
}
