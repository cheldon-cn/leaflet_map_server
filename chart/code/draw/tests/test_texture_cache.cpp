#include <gtest/gtest.h>
#include "ogc/draw/texture_cache.h"
#include "ogc/draw/gpu_resource_manager.h"

using namespace ogc::draw;

class TextureCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        GPUResourceManager::Instance().SetAvailable(true);
        TextureCache::Instance().SetAvailable(true);
        TextureCache::Instance().Clear();
    }
    
    void TearDown() override {
        TextureCache::Instance().Clear();
        TextureCache::Instance().SetAvailable(false);
        GPUResourceManager::Instance().SetAvailable(false);
    }
};

TEST_F(TextureCacheTest, Instance) {
    EXPECT_NO_THROW(TextureCache::Instance());
}

TEST_F(TextureCacheTest, GetOrCreate) {
    auto& cache = TextureCache::Instance();
    
    auto texture = cache.GetOrCreate("test1", 64, 64, TextureFormat::kRGBA8);
    EXPECT_TRUE(texture.IsValid());
    
    auto texture2 = cache.GetOrCreate("test1", 64, 64, TextureFormat::kRGBA8);
    EXPECT_EQ(texture2.GetHandle(), texture.GetHandle());
}

TEST_F(TextureCacheTest, GetOrCreateDifferentKeys) {
    auto& cache = TextureCache::Instance();
    
    auto texture1 = cache.GetOrCreate("key1", 32, 32, TextureFormat::kRGBA8);
    auto texture2 = cache.GetOrCreate("key2", 64, 64, TextureFormat::kRGBA8);
    
    EXPECT_TRUE(texture1.IsValid());
    EXPECT_TRUE(texture2.IsValid());
    EXPECT_NE(texture1.GetHandle(), texture2.GetHandle());
}

TEST_F(TextureCacheTest, ReleaseByKey) {
    auto& cache = TextureCache::Instance();
    
    auto texture = cache.GetOrCreate("release_test", 64, 64);
    EXPECT_TRUE(texture.IsValid());
    
    cache.Release("release_test");
    EXPECT_FALSE(cache.Contains("release_test"));
}

TEST_F(TextureCacheTest, ReleaseByTexture) {
    auto& cache = TextureCache::Instance();
    
    auto texture = cache.GetOrCreate("release_test2", 64, 64);
    EXPECT_TRUE(texture.IsValid());
    
    cache.Release(texture);
    EXPECT_FALSE(cache.Contains("release_test2"));
}

TEST_F(TextureCacheTest, ReleaseAll) {
    auto& cache = TextureCache::Instance();
    
    cache.GetOrCreate("r1", 64, 64);
    cache.GetOrCreate("r2", 64, 64);
    cache.GetOrCreate("r3", 64, 64);
    
    EXPECT_EQ(cache.GetTextureCount(), 3u);
    
    cache.ReleaseAll();
    EXPECT_EQ(cache.GetTextureCount(), 0u);
}

TEST_F(TextureCacheTest, GetCacheSize) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    cache.GetOrCreate("size1", 64, 64);
    cache.GetOrCreate("size2", 128, 128);
    
    EXPECT_GT(cache.GetCacheSize(), 0u);
}

TEST_F(TextureCacheTest, GetTextureCount) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    EXPECT_EQ(cache.GetTextureCount(), 0u);
    
    cache.GetOrCreate("count1", 64, 64);
    EXPECT_EQ(cache.GetTextureCount(), 1u);
    
    cache.GetOrCreate("count2", 64, 64);
    EXPECT_EQ(cache.GetTextureCount(), 2u);
}

TEST_F(TextureCacheTest, GetStats) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    auto statsBefore = cache.GetStats();
    size_t initialMiss = statsBefore.missCount;
    size_t initialHit = statsBefore.hitCount;
    
    cache.GetOrCreate("stat1", 64, 64);
    cache.GetOrCreate("stat1", 64, 64);
    
    auto stats = cache.GetStats();
    EXPECT_EQ(stats.textureCount, 1u);
    EXPECT_EQ(stats.hitCount, initialHit + 1u);
    EXPECT_EQ(stats.missCount, initialMiss + 1u);
}

TEST_F(TextureCacheTest, SetMaxCacheSize) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    cache.SetMaxCacheSize(1024);
    EXPECT_EQ(cache.GetStats().maxSize, 1024u);
}

TEST_F(TextureCacheTest, Contains) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    EXPECT_FALSE(cache.Contains("nonexistent"));
    
    cache.GetOrCreate("exists", 64, 64);
    EXPECT_TRUE(cache.Contains("exists"));
}

TEST_F(TextureCacheTest, Clear) {
    auto& cache = TextureCache::Instance();
    
    cache.GetOrCreate("clear1", 64, 64);
    cache.GetOrCreate("clear2", 64, 64);
    
    EXPECT_GT(cache.GetTextureCount(), 0u);
    
    cache.Clear();
    EXPECT_EQ(cache.GetTextureCount(), 0u);
}

TEST_F(TextureCacheTest, SetEvictionPolicy) {
    auto& cache = TextureCache::Instance();
    
    cache.SetEvictionPolicy(EvictionPolicy::kLRU);
    cache.SetEvictionPolicy(EvictionPolicy::kFIFO);
    cache.SetEvictionPolicy(EvictionPolicy::kSize);
}

TEST_F(TextureCacheTest, EvictLRU) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    cache.SetMaxCacheSize(1024 * 1024);
    
    cache.GetOrCreate("evict1", 256, 256);
    cache.GetOrCreate("evict2", 256, 256);
    cache.GetOrCreate("evict3", 256, 256);
    
    EXPECT_EQ(cache.GetTextureCount(), 3u);
    
    cache.EvictLRU(512 * 1024);
    EXPECT_LE(cache.GetTextureCount(), 3u);
}

TEST_F(TextureCacheTest, GetOrCreateFromData) {
    auto& cache = TextureCache::Instance();
    cache.Clear();
    
    std::vector<uint8_t> data(64 * 64 * 4, 128);
    auto texture = cache.GetOrCreateFromData("data_test", data.data(), data.size(), 64, 64, 4);
    EXPECT_TRUE(texture.IsValid());
}

TEST_F(TextureCacheTest, Availability) {
    auto& cache = TextureCache::Instance();
    
    cache.SetAvailable(true);
    EXPECT_TRUE(cache.IsAvailable());
    
    cache.SetAvailable(false);
    EXPECT_FALSE(cache.IsAvailable());
}
