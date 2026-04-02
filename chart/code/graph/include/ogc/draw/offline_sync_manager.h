#ifndef OGC_DRAW_OFFLINE_SYNC_MANAGER_H
#define OGC_DRAW_OFFLINE_SYNC_MANAGER_H

#include "ogc/draw/export.h"
#include "ogc/draw/offline_storage_manager.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <chrono>
#include <sstream>

namespace ogc {
namespace draw {

struct ChartVersion {
    std::string chartId;
    int majorVersion;
    int minorVersion;
    int patchVersion;
    std::string checksum;
    std::chrono::system_clock::time_point updateTime;
    
    ChartVersion() : majorVersion(0), minorVersion(0), patchVersion(0) {}
    
    bool IsNewerThan(const ChartVersion& other) const {
        if (majorVersion != other.majorVersion) {
            return majorVersion > other.majorVersion;
        }
        if (minorVersion != other.minorVersion) {
            return minorVersion > other.minorVersion;
        }
        return patchVersion > other.patchVersion;
    }
    
    std::string ToString() const {
        std::stringstream ss;
        ss << majorVersion << "." << minorVersion << "." << patchVersion;
        return ss.str();
    }
};

struct UpdateDelta {
    std::string chartId;
    std::vector<int64_t> addedFeatures;
    std::vector<int64_t> modifiedFeatures;
    std::vector<int64_t> deletedFeatures;
    size_t totalSize;
    
    UpdateDelta() : totalSize(0) {}
};

enum class SyncStatus {
    kIdle = 0,
    kCheckingUpdates,
    kDownloading,
    kApplying,
    kCompleted,
    kFailed,
    kConflict
};

enum class ConflictType {
    kNoConflict,
    kLocalModified,
    kRemoteModified,
    kBothModified,
    kLocalDeleted,
    kRemoteDeleted
};

enum class ConflictResolution {
    kLocalWins,
    kRemoteWins,
    kManual,
    kMerge
};

struct ConflictInfo {
    int64_t featureId;
    ConflictType type;
    std::string localData;
    std::string remoteData;
};

struct SyncOperation {
    std::string id;
    std::string chartId;
    std::string operationType;
    std::vector<uint8_t> data;
    std::chrono::system_clock::time_point timestamp;
    bool applied;
};

struct SyncProgress {
    std::string chartId;
    SyncStatus status;
    double progress;
    int totalOperations;
    int completedOperations;
    int failedOperations;
    std::string currentOperation;
    std::string errorMessage;
};

class OGC_GRAPH_API OfflineSyncManager {
public:
    using ProgressCallback = std::function<void(const SyncProgress&)>;
    using CompletionCallback = std::function<void(bool success)>;
    using ConflictCallback = std::function<ConflictResolution(const ConflictInfo&)>;
    
    static std::unique_ptr<OfflineSyncManager> Create(
        OfflineStorageManager* storageManager);
    
    virtual ~OfflineSyncManager();
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual void SetRemoteServer(const std::string& serverUrl) = 0;
    virtual std::string GetRemoteServer() const = 0;
    
    virtual ChartVersion GetLocalVersion(const std::string& chartId) const = 0;
    virtual ChartVersion GetRemoteVersion(const std::string& chartId) = 0;
    
    virtual bool CheckForUpdates(const std::string& chartId) = 0;
    virtual std::vector<std::string> GetPendingUpdates() const = 0;
    virtual void ClearPendingUpdates() = 0;
    
    virtual void SyncChart(const std::string& chartId,
                          ProgressCallback progressCallback,
                          CompletionCallback completionCallback) = 0;
    virtual void SyncAllCharts(ProgressCallback progressCallback,
                               CompletionCallback completionCallback) = 0;
    
    virtual void PauseSync(const std::string& chartId) = 0;
    virtual void ResumeSync(const std::string& chartId) = 0;
    virtual void CancelSync(const std::string& chartId) = 0;
    
    virtual SyncStatus GetSyncStatus(const std::string& chartId) const = 0;
    virtual SyncProgress GetSyncProgress(const std::string& chartId) const = 0;
    
    virtual void SetConflictResolution(ConflictResolution resolution) = 0;
    virtual ConflictResolution GetConflictResolution() const = 0;
    
    virtual void SetConflictCallback(ConflictCallback callback) = 0;
    
    virtual std::vector<ConflictInfo> DetectConflicts(const std::string& chartId) = 0;
    virtual bool ResolveConflict(const std::string& chartId,
                                int64_t featureId,
                                ConflictResolution resolution) = 0;
    virtual bool ResolveAllConflicts(const std::string& chartId,
                                    ConflictResolution resolution) = 0;
    
    virtual std::vector<SyncOperation> GetPendingOperations() const = 0;
    virtual bool CommitOperation(const SyncOperation& operation) = 0;
    virtual bool RollbackOperation(const std::string& operationId) = 0;
    
    virtual UpdateDelta CalculateDelta(const std::string& chartId,
                                       const ChartVersion& fromVersion,
                                       const ChartVersion& toVersion) = 0;
    
    virtual bool ApplyDelta(const std::string& chartId,
                           const UpdateDelta& delta) = 0;
    
    virtual bool IsSyncing() const = 0;
    virtual bool IsSyncing(const std::string& chartId) const = 0;
    
    virtual void SetAutoSync(bool enabled) = 0;
    virtual bool IsAutoSyncEnabled() const = 0;
    
    virtual void SetSyncInterval(int seconds) = 0;
    virtual int GetSyncInterval() const = 0;
    
    virtual void SetMaxRetries(int retries) = 0;
    virtual int GetMaxRetries() const = 0;
    
    virtual size_t GetTotalSyncedData() const = 0;
    virtual int GetTotalSyncedCharts() const = 0;
    
protected:
    OfflineSyncManager() = default;
};

}  
}  

#endif
