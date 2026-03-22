#include <gtest/gtest.h>
#include "ogc/draw/disk_tile_cache.h"
#include "ogc/draw/tile_key.h"
#include <vector>
#include <cstdint>
#include <ctime>

#ifdef _WIN32
#include <Windows.h>
#include <Shlobj.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace ogc::draw;

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
        return ::CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
    }
    
    bool RemoveDir(const std::string& path) {
#ifdef _WIN32
        WIN32_FIND_DATAA findData;
        std::string searchPath = path + "\\*";
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::string name = findData.cFileName;
                if (name != "." && name != "..") {
                    std::string fullPath = path + "\\" + name;
                    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        RemoveDir(fullPath);
                    } else {
                        DeleteFileA(fullPath.c_str());
                    }
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
        return ::RemoveDirectoryA(path.c_str()) != 0;
#else
        return rmdir(path.c_str()) == 0;
#endif
    }
}

class DiskTileCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        testCachePath = GetTempDirectory() + "\\test_disk_cache_" + std::to_string(std::time(nullptr));
        MakeDir(testCachePath);
        cache = std::make_shared<DiskTileCache>(testCachePath, 1024 * 1024);
    }
    
    void TearDown() override {
        cache.reset();
        RemoveDir(testCachePath);
    }
    
    std::shared_ptr<DiskTileCache> cache;
    std::string testCachePath;
    
    std::vector<uint8_t> CreateTestData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(i % 256);
        }
        return data;
    }
};

TEST_F(DiskTileCacheTest, Constructor) {
    EXPECT_EQ(cache->GetMaxSize(), 1024 * 1024);
    EXPECT_EQ(cache->GetTileCount(), 0);
    EXPECT_EQ(cache->GetSize(), 0);
    EXPECT_TRUE(cache->IsEnabled());
    EXPECT_EQ(cache->GetCachePath(), testCachePath);
}

TEST_F(DiskTileCacheTest, PutAndGetTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_TRUE(cache->PutTile(key, data));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_TRUE(tile.IsValid());
    EXPECT_EQ(tile.data.size(), 100);
    EXPECT_EQ(tile.data, data);
}

TEST_F(DiskTileCacheTest, PutTileData) {
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

TEST_F(DiskTileCacheTest, GetNonExistentTile) {
    TileKey key(99, 99, 99);
    EXPECT_FALSE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_FALSE(tile.IsValid());
}

TEST_F(DiskTileCacheTest, RemoveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->HasTile(key));
    
    EXPECT_TRUE(cache->RemoveTile(key));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(DiskTileCacheTest, RemoveNonExistentTile) {
    TileKey key(99, 99, 99);
    EXPECT_FALSE(cache->RemoveTile(key));
}

TEST_F(DiskTileCacheTest, Clear) {
    TileKey key1(1, 2, 3);
    TileKey key2(4, 5, 6);
    
    cache->PutTile(key1, CreateTestData(100));
    cache->PutTile(key2, CreateTestData(200));
    
    EXPECT_GE(cache->GetTileCount(), 2);
    
    cache->Clear();
    
    EXPECT_EQ(cache->GetTileCount(), 0);
    EXPECT_EQ(cache->GetSize(), 0);
    EXPECT_FALSE(cache->HasTile(key1));
    EXPECT_FALSE(cache->HasTile(key2));
}

TEST_F(DiskTileCacheTest, GetTileCount) {
    EXPECT_EQ(cache->GetTileCount(), 0);
    
    cache->PutTile(TileKey(1, 2, 3), CreateTestData(100));
    EXPECT_GE(cache->GetTileCount(), 1);
    
    cache->PutTile(TileKey(4, 5, 6), CreateTestData(200));
    EXPECT_GE(cache->GetTileCount(), 2);
}

TEST_F(DiskTileCacheTest, GetSize) {
    EXPECT_EQ(cache->GetSize(), 0);
    
    auto data1 = CreateTestData(100);
    cache->PutTile(TileKey(1, 2, 3), data1);
    EXPECT_GE(cache->GetSize(), 100);
}

TEST_F(DiskTileCacheTest, SetMaxSize) {
    cache->SetMaxSize(2048);
    EXPECT_EQ(cache->GetMaxSize(), 2048);
}

TEST_F(DiskTileCacheTest, GetName) {
    EXPECT_EQ(cache->GetName(), "DiskTileCache");
}

TEST_F(DiskTileCacheTest, SetName) {
    cache->SetName("TestDiskCache");
    EXPECT_EQ(cache->GetName(), "TestDiskCache");
}

TEST_F(DiskTileCacheTest, IsEnabled) {
    EXPECT_TRUE(cache->IsEnabled());
}

TEST_F(DiskTileCacheTest, SetEnabled) {
    cache->SetEnabled(false);
    EXPECT_FALSE(cache->IsEnabled());
    
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->PutTile(key, data));
    EXPECT_FALSE(cache->HasTile(key));
}

TEST_F(DiskTileCacheTest, GetCachePath) {
    EXPECT_EQ(cache->GetCachePath(), testCachePath);
}

TEST_F(DiskTileCacheTest, SetCompressionEnabled) {
    cache->SetCompressionEnabled(true);
    EXPECT_TRUE(cache->IsCompressionEnabled());
    
    cache->SetCompressionEnabled(false);
    EXPECT_FALSE(cache->IsCompressionEnabled());
}

TEST_F(DiskTileCacheTest, SetExpirationTime) {
    cache->SetExpirationTime(3600);
    EXPECT_EQ(cache->GetExpirationTime(), 3600);
}

TEST_F(DiskTileCacheTest, Flush) {
    TileKey key(1, 2, 3);
    cache->PutTile(key, CreateTestData(100));
    
    EXPECT_NO_THROW(cache->Flush());
}

TEST_F(DiskTileCacheTest, Contains) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    EXPECT_FALSE(cache->Contains(key));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Contains(key));
}

TEST_F(DiskTileCacheTest, Get) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData(100);
    
    TileData tile;
    EXPECT_FALSE(cache->Get(key, tile));
    
    cache->PutTile(key, data);
    EXPECT_TRUE(cache->Get(key, tile));
    EXPECT_TRUE(tile.IsValid());
}

TEST_F(DiskTileCacheTest, StaticCreate) {
    std::string path = testCachePath + "_factory";
    auto newCache = DiskTileCache::Create(path, 2048);
    ASSERT_NE(newCache, nullptr);
    EXPECT_EQ(newCache->GetMaxSize(), 2048);
    
    RemoveDir(path);
}

TEST_F(DiskTileCacheTest, MultipleTiles) {
    for (int z = 0; z < 2; ++z) {
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                TileKey key(z, x, y);
                auto data = CreateTestData(50);
                EXPECT_TRUE(cache->PutTile(key, data));
            }
        }
    }
    
    EXPECT_GE(cache->GetTileCount(), 8);
}

TEST_F(DiskTileCacheTest, OverwriteTile) {
    TileKey key(1, 2, 3);
    
    auto data1 = CreateTestData(100);
    cache->PutTile(key, data1);
    
    auto data2 = CreateTestData(200);
    cache->PutTile(key, data2);
    
    TileData tile = cache->GetTile(key);
    EXPECT_EQ(tile.data.size(), 200);
    EXPECT_EQ(tile.data, data2);
}
