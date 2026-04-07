#include <gtest/gtest.h>
#include <ogc/cache/offline/offline_sync_manager.h>
#include <ogc/cache/offline/offline_storage_manager.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#undef GetFreeSpace
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
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

class OfflineSyncManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_testPath = "./test_sync_storage";
        DeleteDirectoryRecursive(m_testPath);
        
        m_storageManager = OfflineStorageManager::Create(m_testPath);
        m_storageManager->Initialize();
        
        m_syncManager = OfflineSyncManager::Create(m_storageManager.get());
    }
    
    void TearDown() override {
        m_syncManager.reset();
        m_storageManager.reset();
        DeleteDirectoryRecursive(m_testPath);
    }
    
    std::unique_ptr<OfflineStorageManager> m_storageManager;
    std::unique_ptr<OfflineSyncManager> m_syncManager;
    std::string m_testPath;
};

TEST_F(OfflineSyncManagerTest, CreateManager) {
    ASSERT_NE(m_syncManager, nullptr);
}

TEST_F(OfflineSyncManagerTest, Initialize) {
    EXPECT_TRUE(m_syncManager->Initialize());
}

TEST_F(OfflineSyncManagerTest, SetRemoteServer) {
    m_syncManager->SetRemoteServer("http://example.com/api");
    EXPECT_EQ(m_syncManager->GetRemoteServer(), "http://example.com/api");
}

TEST_F(OfflineSyncManagerTest, GetLocalVersion) {
    ChartVersion version = m_syncManager->GetLocalVersion("chart1");
    EXPECT_EQ(version.chartId, "chart1");
    EXPECT_EQ(version.majorVersion, 0);
    EXPECT_EQ(version.minorVersion, 0);
    EXPECT_EQ(version.patchVersion, 0);
}

TEST_F(OfflineSyncManagerTest, GetRemoteVersion) {
    ChartVersion version = m_syncManager->GetRemoteVersion("chart1");
    EXPECT_EQ(version.chartId, "chart1");
    EXPECT_GE(version.majorVersion, 0);
}

TEST_F(OfflineSyncManagerTest, CheckForUpdates) {
    bool hasUpdates = m_syncManager->CheckForUpdates("chart1");
    EXPECT_TRUE(hasUpdates);
    
    auto pending = m_syncManager->GetPendingUpdates();
    EXPECT_EQ(pending.size(), 1u);
    EXPECT_EQ(pending[0], "chart1");
}

TEST_F(OfflineSyncManagerTest, ClearPendingUpdates) {
    m_syncManager->CheckForUpdates("chart1");
    m_syncManager->ClearPendingUpdates();
    
    auto pending = m_syncManager->GetPendingUpdates();
    EXPECT_TRUE(pending.empty());
}

TEST_F(OfflineSyncManagerTest, SyncChart) {
    m_syncManager->Initialize();
    m_syncManager->SetRemoteServer("http://example.com/api");
    
    bool callbackCalled = false;
    bool successResult = false;
    
    m_syncManager->SyncChart("chart1",
        [&](const SyncProgress& progress) {
            callbackCalled = true;
        },
        [&](bool success) {
            successResult = success;
        });
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_TRUE(successResult);
    EXPECT_EQ(m_syncManager->GetSyncStatus("chart1"), SyncStatus::kCompleted);
}

TEST_F(OfflineSyncManagerTest, SyncProgress) {
    m_syncManager->Initialize();
    m_syncManager->SyncChart("chart1", nullptr, nullptr);
    
    SyncProgress progress = m_syncManager->GetSyncProgress("chart1");
    EXPECT_EQ(progress.chartId, "chart1");
    EXPECT_EQ(progress.status, SyncStatus::kCompleted);
    EXPECT_DOUBLE_EQ(progress.progress, 1.0);
}

TEST_F(OfflineSyncManagerTest, PauseSync) {
    m_syncManager->PauseSync("chart1");
    EXPECT_EQ(m_syncManager->GetSyncStatus("chart1"), SyncStatus::kIdle);
}

TEST_F(OfflineSyncManagerTest, CancelSync) {
    m_syncManager->CancelSync("chart1");
    SyncProgress progress = m_syncManager->GetSyncProgress("chart1");
    EXPECT_EQ(progress.status, SyncStatus::kIdle);
}

TEST_F(OfflineSyncManagerTest, SetConflictResolution) {
    m_syncManager->SetConflictResolution(ConflictResolution::kLocalWins);
    EXPECT_EQ(m_syncManager->GetConflictResolution(), ConflictResolution::kLocalWins);
    
    m_syncManager->SetConflictResolution(ConflictResolution::kRemoteWins);
    EXPECT_EQ(m_syncManager->GetConflictResolution(), ConflictResolution::kRemoteWins);
}

TEST_F(OfflineSyncManagerTest, DetectConflicts) {
    auto conflicts = m_syncManager->DetectConflicts("chart1");
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(OfflineSyncManagerTest, ResolveAllConflicts) {
    bool resolved = m_syncManager->ResolveAllConflicts("chart1", ConflictResolution::kRemoteWins);
    EXPECT_TRUE(resolved);
}

TEST_F(OfflineSyncManagerTest, CalculateDelta) {
    ChartVersion fromVersion;
    fromVersion.majorVersion = 1;
    fromVersion.minorVersion = 0;
    fromVersion.patchVersion = 0;
    
    ChartVersion toVersion;
    toVersion.majorVersion = 1;
    toVersion.minorVersion = 1;
    toVersion.patchVersion = 0;
    
    UpdateDelta delta = m_syncManager->CalculateDelta("chart1", fromVersion, toVersion);
    EXPECT_EQ(delta.chartId, "chart1");
    EXPECT_FALSE(delta.addedFeatures.empty());
    EXPECT_FALSE(delta.modifiedFeatures.empty());
    EXPECT_FALSE(delta.deletedFeatures.empty());
    EXPECT_GT(delta.totalSize, 0u);
}

TEST_F(OfflineSyncManagerTest, ApplyDelta) {
    m_syncManager->Initialize();
    
    UpdateDelta delta;
    delta.chartId = "chart1";
    delta.addedFeatures.push_back(1);
    delta.addedFeatures.push_back(2);
    delta.totalSize = 2;
    
    bool applied = m_syncManager->ApplyDelta("chart1", delta);
    EXPECT_TRUE(applied);
}

TEST_F(OfflineSyncManagerTest, CommitOperation) {
    SyncOperation op;
    op.id = "op1";
    op.chartId = "chart1";
    op.operationType = "test";
    
    bool committed = m_syncManager->CommitOperation(op);
    EXPECT_TRUE(committed);
    
    auto operations = m_syncManager->GetPendingOperations();
    EXPECT_EQ(operations.size(), 1u);
    EXPECT_EQ(operations[0].id, "op1");
}

TEST_F(OfflineSyncManagerTest, RollbackOperation) {
    SyncOperation op;
    op.id = "op1";
    op.chartId = "chart1";
    op.operationType = "test";
    
    m_syncManager->CommitOperation(op);
    bool rolledBack = m_syncManager->RollbackOperation("op1");
    EXPECT_TRUE(rolledBack);
    
    auto operations = m_syncManager->GetPendingOperations();
    EXPECT_TRUE(operations.empty());
}

TEST_F(OfflineSyncManagerTest, IsSyncing) {
    EXPECT_FALSE(m_syncManager->IsSyncing());
    EXPECT_FALSE(m_syncManager->IsSyncing("chart1"));
}

TEST_F(OfflineSyncManagerTest, AutoSync) {
    m_syncManager->SetAutoSync(true);
    EXPECT_TRUE(m_syncManager->IsAutoSyncEnabled());
    
    m_syncManager->SetAutoSync(false);
    EXPECT_FALSE(m_syncManager->IsAutoSyncEnabled());
}

TEST_F(OfflineSyncManagerTest, SyncInterval) {
    m_syncManager->SetSyncInterval(600);
    EXPECT_EQ(m_syncManager->GetSyncInterval(), 600);
    
    m_syncManager->SetSyncInterval(30);
    EXPECT_EQ(m_syncManager->GetSyncInterval(), 60);
}

TEST_F(OfflineSyncManagerTest, MaxRetries) {
    m_syncManager->SetMaxRetries(5);
    EXPECT_EQ(m_syncManager->GetMaxRetries(), 5);
    
    m_syncManager->SetMaxRetries(-1);
    EXPECT_EQ(m_syncManager->GetMaxRetries(), 0);
}

TEST_F(OfflineSyncManagerTest, TotalSyncedData) {
    EXPECT_EQ(m_syncManager->GetTotalSyncedData(), 0u);
    EXPECT_EQ(m_syncManager->GetTotalSyncedCharts(), 0);
}

TEST_F(OfflineSyncManagerTest, ChartVersionComparison) {
    ChartVersion v1;
    v1.majorVersion = 1;
    v1.minorVersion = 0;
    v1.patchVersion = 0;
    
    ChartVersion v2;
    v2.majorVersion = 1;
    v2.minorVersion = 1;
    v2.patchVersion = 0;
    
    EXPECT_TRUE(v2.IsNewerThan(v1));
    EXPECT_FALSE(v1.IsNewerThan(v2));
    
    ChartVersion v3;
    v3.majorVersion = 2;
    v3.minorVersion = 0;
    v3.patchVersion = 0;
    
    EXPECT_TRUE(v3.IsNewerThan(v1));
    EXPECT_TRUE(v3.IsNewerThan(v2));
}

TEST_F(OfflineSyncManagerTest, ChartVersionToString) {
    ChartVersion v;
    v.majorVersion = 1;
    v.minorVersion = 2;
    v.patchVersion = 3;
    
    EXPECT_EQ(v.ToString(), "1.2.3");
}

TEST_F(OfflineSyncManagerTest, SyncAllCharts) {
    m_syncManager->CheckForUpdates("chart1");
    m_syncManager->CheckForUpdates("chart2");
    
    int syncCount = 0;
    m_syncManager->SyncAllCharts(
        [&](const SyncProgress& progress) {
            syncCount++;
        },
        nullptr);
    
    EXPECT_GT(syncCount, 0);
}

TEST_F(OfflineSyncManagerTest, MultipleSyncOperations) {
    m_syncManager->Initialize();
    
    for (int i = 0; i < 3; ++i) {
        SyncOperation op;
        op.id = "op" + std::to_string(i);
        op.chartId = "chart" + std::to_string(i);
        op.operationType = "test";
        m_syncManager->CommitOperation(op);
    }
    
    auto operations = m_syncManager->GetPendingOperations();
    EXPECT_EQ(operations.size(), 3u);
}

TEST_F(OfflineSyncManagerTest, SyncAfterUpdate) {
    m_syncManager->Initialize();
    m_syncManager->SyncChart("chart1", nullptr, nullptr);
    
    EXPECT_EQ(m_syncManager->GetTotalSyncedCharts(), 1);
    EXPECT_GT(m_syncManager->GetTotalSyncedData(), 0u);
}

TEST_F(OfflineSyncManagerTest, ConflictResolution) {
    ConflictInfo conflict;
    conflict.featureId = 123;
    conflict.type = ConflictType::kBothModified;
    conflict.localData = "local";
    conflict.remoteData = "remote";
    
    EXPECT_EQ(conflict.type, ConflictType::kBothModified);
    EXPECT_EQ(conflict.featureId, 123);
}

TEST_F(OfflineSyncManagerTest, UpdateDeltaSize) {
    UpdateDelta delta;
    delta.addedFeatures.push_back(1);
    delta.addedFeatures.push_back(2);
    delta.modifiedFeatures.push_back(3);
    delta.deletedFeatures.push_back(4);
    delta.totalSize = 4;
    
    EXPECT_EQ(delta.addedFeatures.size(), 2u);
    EXPECT_EQ(delta.modifiedFeatures.size(), 1u);
    EXPECT_EQ(delta.deletedFeatures.size(), 1u);
    EXPECT_EQ(delta.totalSize, 4u);
}
