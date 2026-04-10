/**
 * @file test_sdk_c_api_cache.cpp
 * @brief Unit tests for SDK C API Cache module
 * 
 * Note: Some tests have been moved to pending_tests/test_sdk_c_api_cache_pending.cpp
 * because they use APIs that don't exist in the current SDK:
 * - ogc_tile_cache_create(capacity) - SDK uses ogc_tile_cache_create(void)
 * - ogc_tile_cache_get_count - not implemented
 * - ogc_tile_cache_get_capacity/set_capacity - SDK uses get_max_size/set_max_size
 * - ogc_memory_tile_cache_create(count, bytes) - SDK uses ogc_memory_tile_cache_create(bytes)
 * - ogc_memory_tile_cache_get_memory_usage - not implemented
 * - ogc_offline_storage_store_chart/get_chart_path/remove_chart - not implemented
 * - ogc_offline_storage_get_chart_list/get_storage_size - not implemented
 * - ogc_cache_manager_* - not implemented
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
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheHasTile) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    int has_tile = ogc_tile_cache_has_tile(cache, &key);
    EXPECT_EQ(has_tile, 0);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCachePut_Get) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    const char* test_data = "test_data";
    ogc_tile_cache_put_tile(cache, &key, test_data, 9);
    
    int has_tile = ogc_tile_cache_has_tile(cache, &key);
    EXPECT_NE(has_tile, 0);
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get_tile(cache, &key);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheGet_NotFound) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get_tile(cache, &key);
    EXPECT_EQ(retrieved, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheRemoveTile) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    const char* test_data = "test_data";
    ogc_tile_cache_put_tile(cache, &key, test_data, 9);
    
    ogc_tile_cache_remove_tile(cache, &key);
    
    int has_tile = ogc_tile_cache_has_tile(cache, &key);
    EXPECT_EQ(has_tile, 0);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheClear) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_tile_key_t key;
        key.z = 10;
        key.x = i;
        key.y = i;
        
        const char* test_data = "test_data";
        ogc_tile_cache_put_tile(cache, &key, test_data, 9);
    }
    
    ogc_tile_cache_clear(cache);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheGetSize) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    size_t size = ogc_tile_cache_get_size(cache);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheGetMaxSize) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    size_t max_size = ogc_tile_cache_get_max_size(cache);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiTileCacheTest, TileCacheSetMaxSize) {
    ogc_tile_cache_t* cache = ogc_tile_cache_create();
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_cache_set_max_size(cache, 1024 * 1024);
    
    size_t max_size = ogc_tile_cache_get_max_size(cache);
    EXPECT_EQ(max_size, 1024 * 1024);
    
    ogc_tile_cache_destroy(cache);
}

class SdkCApiMemoryTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCacheCreate_Destroy) {
    ogc_tile_cache_t* cache = ogc_memory_tile_cache_create(1024 * 1024);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

TEST_F(SdkCApiMemoryTileCacheTest, MemoryTileCachePut_Get) {
    ogc_tile_cache_t* cache = ogc_memory_tile_cache_create(1024 * 1024);
    ASSERT_NE(cache, nullptr);
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    const char* test_data = "test_data";
    ogc_tile_cache_put_tile(cache, &key, test_data, 9);
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get_tile(cache, &key);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_tile_cache_destroy(cache);
}

class SdkCApiDiskTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiDiskTileCacheTest, DiskTileCacheCreate_Destroy) {
    ogc_tile_cache_t* cache = ogc_disk_tile_cache_create("/tmp/tile_cache", 1024 * 1024 * 100);
    if (cache != nullptr) {
        ogc_tile_cache_destroy(cache);
    }
}

TEST_F(SdkCApiDiskTileCacheTest, DiskTileCachePut_Get) {
    ogc_tile_cache_t* cache = ogc_disk_tile_cache_create("/tmp/tile_cache", 1024 * 1024 * 100);
    if (cache == nullptr) {
        return;
    }
    
    ogc_tile_key_t key;
    key.z = 10;
    key.x = 100;
    key.y = 200;
    
    const char* test_data = "test_data";
    ogc_tile_cache_put_tile(cache, &key, test_data, 9);
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get_tile(cache, &key);
    
    ogc_tile_cache_destroy(cache);
}

class SdkCApiMultiLevelTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiMultiLevelTileCacheTest, MultiLevelTileCacheCreate_Destroy) {
    ogc_tile_cache_t* memory_cache = ogc_memory_tile_cache_create(1024 * 1024);
    ASSERT_NE(memory_cache, nullptr);
    
    ogc_tile_cache_t* disk_cache = ogc_disk_tile_cache_create("/tmp/tile_cache", 1024 * 1024 * 100);
    
    ogc_tile_cache_t* multi_cache = ogc_multi_level_tile_cache_create(memory_cache, disk_cache);
    if (multi_cache != nullptr) {
        ogc_tile_cache_destroy(multi_cache);
    } else {
        ogc_tile_cache_destroy(memory_cache);
        if (disk_cache != nullptr) {
            ogc_tile_cache_destroy(disk_cache);
        }
    }
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

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageGetRegionCount) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    size_t count = ogc_offline_storage_get_region_count(storage);
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageGetRegion) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    ogc_offline_region_t* region = ogc_offline_storage_get_region(storage, 0);
    
    ogc_offline_storage_destroy(storage);
}

TEST_F(SdkCApiOfflineStorageTest, OfflineStorageRemoveRegion) {
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/offline_storage");
    if (storage == nullptr) {
        return;
    }
    
    ogc_offline_storage_remove_region(storage, 0);
    
    ogc_offline_storage_destroy(storage);
}
