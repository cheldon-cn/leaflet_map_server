#include <gtest/gtest.h>
#include "../src/cache/memory_cache.h"
#include "../src/cache/disk_cache.h"
#include "../src/utils/file_system.h"
#include <thread>
#include <chrono>

class MemoryCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_shared<cycle::cache::MemoryCache>(256); // 256MB
    }
    
    std::shared_ptr<cycle::cache::MemoryCache> cache;
};

TEST_F(MemoryCacheTest, BasicOperations) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    // 测试 Put 和 Get
    EXPECT_TRUE(cache->Put("key1", data));
    
    std::vector<uint8_t> retrieved;
    EXPECT_TRUE(cache->Get("key1", retrieved));
    EXPECT_EQ(data, retrieved);
    
    // 测试 Remove
    EXPECT_TRUE(cache->Remove("key1"));
    EXPECT_FALSE(cache->Get("key1", retrieved));
}

TEST_F(MemoryCacheTest, LRUEviction) {
    // 插入超过缓存大小的数据
    std::vector<uint8_t> largeData(128 * 1024 * 1024, 1); // 128MB
    std::vector<uint8_t> smallData(64 * 1024 * 1024, 2);  // 64MB
    
    EXPECT_TRUE(cache->Put("large", largeData));
    EXPECT_TRUE(cache->Put("small", smallData));
    
    // 由于 LRU 策略，large 应该被淘汰
    std::vector<uint8_t> retrieved;
    EXPECT_FALSE(cache->Get("large", retrieved));
    EXPECT_TRUE(cache->Get("small", retrieved));
}

TEST_F(MemoryCacheTest, ThreadSafety) {
    auto worker = [this](int id) {
        for (int i = 0; i < 100; ++i) {
            std::string key = "thread" + std::to_string(id) + "_key" + std::to_string(i);
            std::vector<uint8_t> data = {static_cast<uint8_t>(id), static_cast<uint8_t>(i)};
            
            cache->Put(key, data);
            
            std::vector<uint8_t> retrieved;
            if (cache->Get(key, retrieved)) {
                EXPECT_EQ(data, retrieved);
            }
        }
    };
    
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);
    
    t1.join();
    t2.join();
    t3.join();
    
    // 验证缓存统计信息
    auto stats = cache->GetStats();
    EXPECT_GE(stats.total_puts, 300);
    EXPECT_GE(stats.total_gets, 300);
}

TEST_F(MemoryCacheTest, TTLExpiration) {
    cache->SetTTL(1); // 1秒TTL
    
    std::vector<uint8_t> data = {1, 2, 3};
    EXPECT_TRUE(cache->Put("key", data));
    
    // 立即获取应该成功
    std::vector<uint8_t> retrieved;
    EXPECT_TRUE(cache->Get("key", retrieved));
    
    // 等待过期
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 现在应该失败
    EXPECT_FALSE(cache->Get("key", retrieved));
}

class DiskCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cycle::cache::DiskCacheConfig config;
        config.cache_dir = "./test_cache";
        config.max_size_mb = 10; // 10MB
        config.max_file_size_mb = 1; // 1MB
        config.ttl_seconds = 60;
        config.compress = true;
        
        cache = std::make_shared<cycle::cache::DiskCache>(config);
        cache->Initialize();
    }
    
    void TearDown() override {
        if (cycle::utils::exists("./test_cache")) {
            cycle::utils::remove_all("./test_cache");
        }
    }
    
    std::shared_ptr<cycle::cache::DiskCache> cache;
};

TEST_F(DiskCacheTest, BasicFileOperations) {
    std::vector<uint8_t> data(1024, 42); // 1KB数据
    
    EXPECT_TRUE(cache->Put("test_key", data));
    
    std::vector<uint8_t> retrieved;
    EXPECT_TRUE(cache->Get("test_key", retrieved));
    EXPECT_EQ(data, retrieved);
    
    // 验证文件确实被创建
    EXPECT_TRUE(cycle::utils::exists("./test_cache"));
}

TEST_F(DiskCacheTest, Compression) {
    // 测试可压缩数据
    std::vector<uint8_t> data(1024, 0); // 全零数据，容易压缩
    
    EXPECT_TRUE(cache->Put("compressed", data));
    
    std::vector<uint8_t> retrieved;
    EXPECT_TRUE(cache->Get("compressed", retrieved));
    EXPECT_EQ(data, retrieved);
    
    // 验证压缩效果
    auto stats = cache->GetStats();
    EXPECT_GT(stats.current_size, 0);
}

TEST_F(DiskCacheTest, CacheCleanup) {
    // 插入大量小文件
    for (int i = 0; i < 50; ++i) {
        std::vector<uint8_t> data(100 * 1024, static_cast<uint8_t>(i)); // 100KB
        EXPECT_TRUE(cache->Put("file" + std::to_string(i), data));
    }
    
    // 触发清理
    cache->Cleanup();
    
    auto stats = cache->GetStats();
    EXPECT_LE(stats.current_size, 10 * 1024 * 1024); // 不超过10MB
    EXPECT_LE(stats.current_files, 50); // 文件数量限制
}

TEST_F(DiskCacheTest, Persistence) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    EXPECT_TRUE(cache->Put("persistent", data));
    
    // 重新创建缓存实例
    cycle::cache::DiskCacheConfig config;
    config.cache_dir = "./test_cache";
    config.max_size_mb = 10;
    
    auto newCache = std::make_shared<cycle::cache::DiskCache>(config);
    newCache->Initialize();
    
    // 应该能恢复之前的数据
    std::vector<uint8_t> retrieved;
    EXPECT_TRUE(newCache->Get("persistent", retrieved));
    EXPECT_EQ(data, retrieved);
}

TEST_F(DiskCacheTest, ErrorConditions) {
    // 测试无效数据
    std::vector<uint8_t> emptyData;
    EXPECT_FALSE(cache->Put("empty", emptyData));
    
    // 测试过大文件
    std::vector<uint8_t> hugeData(2 * 1024 * 1024, 1); // 2MB，超过1MB限制
    EXPECT_FALSE(cache->Put("huge", hugeData));
    
    // 测试不存在的键
    std::vector<uint8_t> retrieved;
    EXPECT_FALSE(cache->Get("nonexistent", retrieved));
}

TEST(CacheIntegrationTest, MultiLevelCache) {
    // 测试内存缓存和磁盘缓存的集成
    auto memoryCache = std::make_shared<cycle::cache::MemoryCache>(64); // 64MB
    
    cycle::cache::DiskCacheConfig diskConfig;
    diskConfig.cache_dir = "./integration_cache";
    diskConfig.max_size_mb = 100;
    
    auto diskCache = std::make_shared<cycle::cache::DiskCache>(diskConfig);
    diskCache->Initialize();
    
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    // 先放入内存缓存
    EXPECT_TRUE(memoryCache->Put("key", data));
    
    // 再放入磁盘缓存
    EXPECT_TRUE(diskCache->Put("key", data));
    
    // 测试从内存缓存获取
    std::vector<uint8_t> fromMemory;
    EXPECT_TRUE(memoryCache->Get("key", fromMemory));
    EXPECT_EQ(data, fromMemory);
    
    // 测试从磁盘缓存获取
    std::vector<uint8_t> fromDisk;
    EXPECT_TRUE(diskCache->Get("key", fromDisk));
    EXPECT_EQ(data, fromDisk);
    
    // 清理
    if (cycle::utils::exists("./integration_cache")) {
        cycle::utils::remove_all("./integration_cache");
    }
}
