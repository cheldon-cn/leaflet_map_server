#include <gtest/gtest.h>
#include <ogc/cache/offline/offline_storage_manager.h>
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#undef GetFreeSpace
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

using namespace ogc::cache;

namespace {

void DeleteDirectoryRecursive(const std::string& path) {
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0) {
                std::string fullPath = path + "\\" + findData.cFileName;
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    DeleteDirectoryRecursive(fullPath);
                } else {
                    _unlink(fullPath.c_str());
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    _rmdir(path.c_str());
#else
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                std::string fullPath = path + "/" + entry->d_name;
                struct stat statBuf;
                if (stat(fullPath.c_str(), &statBuf) == 0) {
                    if (S_ISDIR(statBuf.st_mode)) {
                        DeleteDirectoryRecursive(fullPath);
                    } else {
                        unlink(fullPath.c_str());
                    }
                }
            }
        }
        closedir(dir);
    }
    rmdir(path.c_str());
#endif
}

}

class OfflineStorageManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_testPath = "./test_offline_storage";
        DeleteDirectoryRecursive(m_testPath);
        manager = OfflineStorageManager::Create(m_testPath);
    }
    
    void TearDown() override {
        manager.reset();
    }
    
    std::unique_ptr<OfflineStorageManager> manager;
    std::string m_testPath;
};

TEST_F(OfflineStorageManagerTest, CreateManager) {
    ASSERT_NE(manager, nullptr);
}

TEST_F(OfflineStorageManagerTest, Initialize) {
    EXPECT_TRUE(manager->Initialize());
    EXPECT_TRUE(manager->IsInitialized());
}

TEST_F(OfflineStorageManagerTest, Shutdown) {
    manager->Initialize();
    manager->Shutdown();
    EXPECT_FALSE(manager->IsInitialized());
}

TEST_F(OfflineStorageManagerTest, CreateRegion) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    EXPECT_FALSE(regionId.empty());
    EXPECT_TRUE(manager->HasRegion(regionId));
}

TEST_F(OfflineStorageManagerTest, GetRegion) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    OfflineRegion region = manager->GetRegion(regionId);
    EXPECT_EQ(region.id, regionId);
    EXPECT_EQ(region.name, "TestRegion");
    EXPECT_DOUBLE_EQ(region.minLon, -180.0);
    EXPECT_DOUBLE_EQ(region.maxLon, 180.0);
    EXPECT_DOUBLE_EQ(region.minLat, -90.0);
    EXPECT_DOUBLE_EQ(region.maxLat, 90.0);
    EXPECT_EQ(region.minZoom, 0);
    EXPECT_EQ(region.maxZoom, 5);
}

TEST_F(OfflineStorageManagerTest, GetAllRegions) {
    manager->Initialize();
    
    manager->CreateRegion("Region1", -180, 180, -90, 90, 0, 5);
    manager->CreateRegion("Region2", -90, 90, -45, 45, 0, 10);
    
    auto regions = manager->GetAllRegions();
    EXPECT_EQ(regions.size(), 2u);
}

TEST_F(OfflineStorageManagerTest, DeleteRegion) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    EXPECT_TRUE(manager->DeleteRegion(regionId));
    EXPECT_FALSE(manager->HasRegion(regionId));
}

TEST_F(OfflineStorageManagerTest, DeleteNonExistentRegion) {
    manager->Initialize();
    
    EXPECT_FALSE(manager->DeleteRegion("nonexistent"));
}

TEST_F(OfflineStorageManagerTest, UpdateRegion) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    OfflineRegion updates;
    updates.name = "UpdatedRegion";
    
    EXPECT_TRUE(manager->UpdateRegion(regionId, updates));
    
    OfflineRegion region = manager->GetRegion(regionId);
    EXPECT_EQ(region.name, "UpdatedRegion");
}

TEST_F(OfflineStorageManagerTest, GetRegionCount) {
    manager->Initialize();
    
    EXPECT_EQ(manager->GetRegionCount(), 0u);
    
    manager->CreateRegion("Region1", -180, 180, -90, 90, 0, 5);
    EXPECT_EQ(manager->GetRegionCount(), 1u);
    
    manager->CreateRegion("Region2", -90, 90, -45, 45, 0, 10);
    EXPECT_EQ(manager->GetRegionCount(), 2u);
}

TEST_F(OfflineStorageManagerTest, StoreAndGetTile) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    TileKey key{0, 0, 0};
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    EXPECT_TRUE(manager->StoreTile(regionId, key, data));
    EXPECT_TRUE(manager->HasTile(regionId, key));
    
    std::vector<uint8_t> retrieved = manager->GetTile(regionId, key);
    EXPECT_EQ(retrieved.size(), data.size());
    EXPECT_EQ(retrieved, data);
}

TEST_F(OfflineStorageManagerTest, DeleteTile) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    TileKey key{0, 0, 0};
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    manager->StoreTile(regionId, key, data);
    EXPECT_TRUE(manager->DeleteTile(regionId, key));
    EXPECT_FALSE(manager->HasTile(regionId, key));
}

TEST_F(OfflineStorageManagerTest, GetStorageInfo) {
    manager->Initialize();
    
    StorageInfo info = manager->GetStorageInfo();
    EXPECT_GE(info.totalSpace, 0u);
    EXPECT_GE(info.freeSpace, 0u);
    EXPECT_EQ(info.regionCount, 0);
}

TEST_F(OfflineStorageManagerTest, GetTotalSpace) {
    manager->Initialize();
    
    size_t total = manager->GetTotalSpace();
    EXPECT_GT(total, 0u);
}

TEST_F(OfflineStorageManagerTest, GetFreeSpace) {
    manager->Initialize();
    
    size_t freeSpace = manager->GetFreeSpace();
    EXPECT_GT(freeSpace, 0u);
}

TEST_F(OfflineStorageManagerTest, SetStorageLimit) {
    manager->SetStorageLimit(1024 * 1024 * 100);
    EXPECT_EQ(manager->GetStorageLimit(), 1024u * 1024 * 100);
}

TEST_F(OfflineStorageManagerTest, ClearCache) {
    manager->Initialize();
    EXPECT_NO_THROW(manager->ClearCache());
}

TEST_F(OfflineStorageManagerTest, ClearAllOfflineData) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    TileKey key{0, 0, 0};
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    manager->StoreTile(regionId, key, data);
    
    EXPECT_NO_THROW(manager->ClearAllOfflineData());
}

TEST_F(OfflineStorageManagerTest, ValidateStorage) {
    manager->Initialize();
    EXPECT_TRUE(manager->ValidateStorage());
}

TEST_F(OfflineStorageManagerTest, SetMaxConcurrentDownloads) {
    manager->SetMaxConcurrentDownloads(8);
    EXPECT_EQ(manager->GetMaxConcurrentDownloads(), 8);
    
    manager->SetMaxConcurrentDownloads(0);
    EXPECT_EQ(manager->GetMaxConcurrentDownloads(), 1);
}

TEST_F(OfflineStorageManagerTest, SetDownloadTimeout) {
    manager->SetDownloadTimeout(60);
    EXPECT_EQ(manager->GetDownloadTimeout(), 60);
    
    manager->SetDownloadTimeout(0);
    EXPECT_EQ(manager->GetDownloadTimeout(), 1);
}

TEST_F(OfflineStorageManagerTest, SetRetryCount) {
    manager->SetRetryCount(5);
    EXPECT_EQ(manager->GetRetryCount(), 5);
    
    manager->SetRetryCount(-1);
    EXPECT_EQ(manager->GetRetryCount(), 0);
}

TEST_F(OfflineStorageManagerTest, GetStoragePath) {
    EXPECT_EQ(manager->GetStoragePath(), m_testPath);
}

TEST_F(OfflineStorageManagerTest, StartDownload) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    StorageError error = StorageError::kRegionNotFound;
    manager->StartDownload(regionId, nullptr, [&error](StorageError e) {
        error = e;
    });
    
    EXPECT_EQ(error, StorageError::kNone);
}

TEST_F(OfflineStorageManagerTest, StartDownloadNonExistentRegion) {
    manager->Initialize();
    
    StorageError error = StorageError::kNone;
    manager->StartDownload("nonexistent", nullptr, [&error](StorageError e) {
        error = e;
    });
    
    EXPECT_EQ(error, StorageError::kRegionNotFound);
}

TEST_F(OfflineStorageManagerTest, PauseDownload) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    manager->StartDownload(regionId, nullptr, nullptr);
    EXPECT_NO_THROW(manager->PauseDownload(regionId));
    EXPECT_FALSE(manager->IsDownloading(regionId));
}

TEST_F(OfflineStorageManagerTest, ResumeDownload) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    manager->StartDownload(regionId, nullptr, nullptr);
    manager->PauseDownload(regionId);
    EXPECT_NO_THROW(manager->ResumeDownload(regionId));
}

TEST_F(OfflineStorageManagerTest, CancelDownload) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    manager->StartDownload(regionId, nullptr, nullptr);
    EXPECT_NO_THROW(manager->CancelDownload(regionId));
    EXPECT_FALSE(manager->IsDownloading(regionId));
    EXPECT_DOUBLE_EQ(manager->GetDownloadProgress(regionId), 0.0);
}

TEST_F(OfflineStorageManagerTest, RegionChangedCallback) {
    manager->Initialize();
    
    std::string callbackRegionId;
    manager->SetRegionChangedCallback([&callbackRegionId](const OfflineRegion& region) {
        callbackRegionId = region.id;
    });
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    EXPECT_EQ(callbackRegionId, regionId);
}

TEST_F(OfflineStorageManagerTest, CalculateTileCount) {
    int count = OfflineStorageManager::CalculateTileCount(
        -180, 180, -90, 90, 0, 2);
    EXPECT_GT(count, 0);
}

TEST_F(OfflineStorageManagerTest, CalculateRegionSize) {
    size_t size = OfflineStorageManager::CalculateRegionSize(
        -180, 180, -90, 90, 0, 2);
    EXPECT_GT(size, 0u);
}

TEST_F(OfflineStorageManagerTest, MultipleTiles) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    for (int z = 0; z <= 2; ++z) {
        for (int x = 0; x < (1 << z); ++x) {
            for (int y = 0; y < (1 << z); ++y) {
                TileKey key{z, x, y};
                EXPECT_TRUE(manager->StoreTile(regionId, key, data));
            }
        }
    }
    
    OfflineRegion region = manager->GetRegion(regionId);
    EXPECT_GT(region.tileCount, 0u);
    EXPECT_GT(region.dataSize, 0u);
}

TEST_F(OfflineStorageManagerTest, StoreTileNonExistentRegion) {
    manager->Initialize();
    
    TileKey key{0, 0, 0};
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    EXPECT_FALSE(manager->StoreTile("nonexistent", key, data));
}

TEST_F(OfflineStorageManagerTest, GetTileNonExistent) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    TileKey key{0, 0, 0};
    std::vector<uint8_t> data = manager->GetTile(regionId, key);
    EXPECT_TRUE(data.empty());
}

TEST_F(OfflineStorageManagerTest, CompactStorage) {
    manager->Initialize();
    EXPECT_TRUE(manager->CompactStorage());
}

TEST_F(OfflineStorageManagerTest, Reinitialize) {
    manager->Initialize();
    manager->Shutdown();
    EXPECT_TRUE(manager->Initialize());
    EXPECT_TRUE(manager->IsInitialized());
}

TEST_F(OfflineStorageManagerTest, MultipleRegionsWithTiles) {
    manager->Initialize();
    
    std::string region1 = manager->CreateRegion("Region1", -180, 0, -90, 0, 0, 5);
    std::string region2 = manager->CreateRegion("Region2", 0, 180, 0, 90, 0, 5);
    
    TileKey key1{0, 0, 0};
    TileKey key2{0, 1, 0};
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    EXPECT_TRUE(manager->StoreTile(region1, key1, data));
    EXPECT_TRUE(manager->StoreTile(region2, key2, data));
    
    EXPECT_TRUE(manager->HasTile(region1, key1));
    EXPECT_TRUE(manager->HasTile(region2, key2));
    EXPECT_FALSE(manager->HasTile(region1, key2));
    EXPECT_FALSE(manager->HasTile(region2, key1));
}

TEST_F(OfflineStorageManagerTest, ProgressCallback) {
    manager->Initialize();
    
    std::string regionId = manager->CreateRegion(
        "TestRegion", -180, 180, -90, 90, 0, 5);
    
    DownloadProgress receivedProgress;
    manager->StartDownload(regionId, 
        [&receivedProgress](const DownloadProgress& progress) {
            receivedProgress = progress;
        }, nullptr);
    
    EXPECT_EQ(receivedProgress.regionId, regionId);
    EXPECT_GT(receivedProgress.totalTiles, 0);
}
