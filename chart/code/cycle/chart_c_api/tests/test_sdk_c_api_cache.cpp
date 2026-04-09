/**
 * @file test_sdk_c_api_cache.cpp
 * @brief Unit tests for SDK C API Cache module
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiTileCacheTest, TileCacheCreate_Destroy) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheCreate_ZeroCapacity) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(0);
    EXPECT_EQ(cache, nullptr);
}

TEST_F(SdkCApiTileCacheTest, TileCachePut_Get) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.level = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t tile_data;
    tile_data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("test_data"));
    tile_data.size = 9;
    
    int result = ogc_tile_cache_put(cache, &key, &tile_data);
    EXPECT_EQ(result, 0);
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get(cache, &key);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->size, 9);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheGet_NotFound) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.level = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get(cache, &key);
    EXPECT_EQ(retrieved, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheRemove) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.level = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t tile_data;
    tile_data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("test_data"));
    tile_data.size = 9;
    
    ogc_tile_cache_put(cache, &key, &tile_data);
    
    int result = ogc_tile_cache_remove(cache, &key);
    EXPECT_EQ(result, 0);
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get(cache, &key);
    EXPECT_EQ(retrieved, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheClear) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_tile_key_t key;
        key.level = 10;
        key.x = i;
        key.y = i;
        
        ogc_tile_data_t tile_data;
        tile_data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("test_data"));
        tile_data.size = 9;
        
        ogc_tile_cache_put(cache, &key, &tile_data);
    }
    
    size_t count = ogc_tile_cache_get_count(cache);
    EXPECT_EQ(count, 5);
    
    ogc_tile_cache_clear(cache);
    
    count = ogc_tile_cache_get_count(cache);
    EXPECT_EQ(count, 0);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheGetCapacity) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    size_t capacity = ogc_tile_cache_get_capacity(cache);
    EXPECT_EQ(capacity, 100);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheSetCapacity) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_cache_set_capacity(cache, 200);
    
    size_t capacity = ogc_tile_cache_get_capacity(cache);
    EXPECT_EQ(capacity, 200);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheNullPointer) {
    EXPECT_EQ(ogc_tile_cache_get(nullptr, nullptr), nullptr);
    EXPECT_NE(ogc_tile_cache_put(nullptr, nullptr, nullptr), 0);
    EXPECT_EQ(ogc_tile_cache_get_count(nullptr), 0);
    EXPECT_EQ(ogc_tile_cache_get_capacity(nullptr), 0);
}

class SdkCApiMemoryTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCacheCreate_Destroy) {
    ogc_memory_tile_cache_t* cache = ogc_memory_tile_cache_create(100, 1024 * 1024);
    ASSERT_NE(cache, nullptr);
    
    ogc_memory_tile_cache_destroy(cache);
}

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCachePut_Get) {
    ogc_memory_tile_cache_t* cache = ogc_memory_tile_cache_create(100, 1024 * 1024);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.level = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t tile_data;
    tile_data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("test_data"));
    tile_data.size = 9;
    
    int result = ogc_memory_tile_cache_put(cache, &key, &tile_data);
    EXPECT_EQ(result, 0);
    
    ogc_tile_data_t* retrieved = ogc_memory_tile_cache_get(cache, &key);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_memory_tile_cache_destroy(cache);
}

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCacheGetMemoryUsage) {
    ogc_memory_tile_cache_t* cache = ogc_memory_tile_cache_create(100, 1024 * 1024);
    ASSERT_NE(cache, nullptr);
    
    size_t usage = ogc_memory_tile_cache_get_memory_usage(cache);
    
    ogc_memory_tile_cache_destroy(cache);
}

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCacheNullPointer) {
    EXPECT_EQ(ogc_memory_tile_cache_get(nullptr, nullptr), nullptr);
    EXPECT_NE(ogc_memory_tile_cache_put(nullptr, nullptr, nullptr), 0);
    EXPECT_EQ(ogc_memory_tile_cache_get_memory_usage(nullptr), 0);
}

class SdkCApiDiskTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiDiskTileCacheTest, DiskTileCacheCreate_Destroy) {
    ogc_disk_tile_cache_t* cache = ogc_disk_tile_cache_create("/tmp/tile_cache", 1024 * 1024 * 100);
    if (cache != nullptr) {
        ogc_disk_tile_cache_destroy(cache);
    }
}

TEST_F(SdkCApiDiskTileCacheTest, DiskTileCachePut_Get) {
    ogc_disk_tile_cache_t* cache = ogc_disk_tile_cache_create("/tmp/tile_cache", 1024 * 1024 * 100);
    if (cache == nullptr) {
        return;
    }
    
    ogc_tile_key_t key;
    key.level = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t tile_data;
    tile_data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("test_data"));
    tile_data.size = 9;
    
    int result = ogc_disk_tile_cache_put(cache, &key, &tile_data);
    
    ogc_tile_data_t* retrieved = ogc_disk_tile_cache_get(cache, &key);
    
    ogc_disk_tile_cache_destroy(cache);
}

TEST_F(SdkCApiDiskTileCacheTest, DiskTileCacheNullPointer) {
    EXPECT_EQ(ogc_disk_tile_cache_get(nullptr, nullptr), nullptr);
    EXPECT_NE(ogc_disk_tile_cache_put(nullptr, nullptr, nullptr), 0);
}

class SdkCApiOfflineStorageTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageCreate_Destroy) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage != nullptr) {
        ogc_offline_storage_destroy(storage);
    }
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageStoreChart) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    int result = ogc_offline_storage_store_chart(storage, "chart_001", "/path/to/chart");
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageGetChart) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    const char* path = ogc_offline_storage_get_chart_path(storage, "chart_001");
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageRemoveChart) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    int result = ogc_offline_storage_remove_chart(storage, "chart_001");
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageGetChartList) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    char** list = nullptr;
    int count = 0;
    int result = ogc_offline_storage_get_chart_list(storage, &list, &count);
    
    if (list != nullptr) {
        ogc_string_array_destroy(list, count);
    }
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageGetStorageSize) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    size_t size = ogc_offline_storage_get_storage_size(storage);
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageNullPointer) {
    EXPECT_EQ(ogc_offline_storage_get_chart_path(nullptr, "test"), nullptr);
    EXPECT_NE(ogc_offline_storage_store_chart(nullptr, "test", "/path"), 0);
    EXPECT_EQ(ogc_offline_storage_get_storage_size(nullptr), 0);
}

class SdkCApiCacheManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiCacheManagerTest, CacheManagerGet_Instance) {
    ogc_cache_manager_t* mgr = ogc_cache_manager_get_instance();
    if (mgr != nullptr) {
    }
}

TEST_F(SdkCApiCacheManagerTest, CacheManagerRegisterCache) {
    ogc_cache_manager_t* mgr = ogc_cache_manager_get_instance();
    if (mgr == nullptr) {
        return;
    }
    
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    if (cache == nullptr) {
        return;
    }
    
    int result = ogc_cache_manager_register_cache(mgr, "test_cache", cache);
    
    ogc_cache_manager_unregister_cache(mgr, "test_cache");
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiCacheManagerTest, CacheManagerGetCache) {
    ogc_cache_manager_t* mgr = ogc_cache_manager_get_instance();
    if (mgr == nullptr) {
        return;
    }
    
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    if (cache == nullptr) {
        return;
    }
    
    ogc_cache_manager_register_cache(mgr, "test_cache", cache);
    
    ogc_tile_cache_t* retrieved = ogc_cache_manager_get_cache(mgr, "test_cache");
    
    ogc_cache_manager_unregister_cache(mgr, "test_cache");
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiCacheManagerTest, CacheManagerClearAll) {
    ogc_cache_manager_t* mgr = ogc_cache_manager_get_instance();
    if (mgr == nullptr) {
        return;
    }
    
    ogc_cache_manager_clear_all(mgr);
}

TEST_F(SdkCApiCacheManagerTest, CacheManagerNullPointer) {
    EXPECT_EQ(ogc_cache_manager_get_cache(nullptr, "test"), nullptr);
    EXPECT_NE(ogc_cache_manager_register_cache(nullptr, "test", nullptr), 0);
}
