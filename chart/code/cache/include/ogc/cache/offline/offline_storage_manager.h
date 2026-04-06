#ifndef OGC_CACHE_OFFLINE_OFFLINE_STORAGE_MANAGER_H
#define OGC_CACHE_OFFLINE_OFFLINE_STORAGE_MANAGER_H

#include "ogc/cache/export.h"
#include "ogc/cache/tile/tile_key.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cstdint>

namespace ogc {
namespace cache {

struct OfflineRegion {
    std::string id;
    std::string name;
    double minLon;
    double maxLon;
    double minLat;
    double maxLat;
    int minZoom;
    int maxZoom;
    std::string createdAt;
    std::string updatedAt;
    size_t dataSize;
    size_t tileCount;
    bool isDownloading;
    double downloadProgress;
    
    OfflineRegion()
        : minLon(0), maxLon(0), minLat(0), maxLat(0)
        , minZoom(0), maxZoom(0)
        , dataSize(0), tileCount(0)
        , isDownloading(false), downloadProgress(0.0)
    {}
};

struct DownloadProgress {
    std::string regionId;
    int totalTiles;
    int downloadedTiles;
    int failedTiles;
    double progress;
    std::string status;
    std::string currentTile;
    std::string errorMessage;
    
    DownloadProgress()
        : totalTiles(0), downloadedTiles(0), failedTiles(0)
        , progress(0.0)
    {}
};

struct StorageInfo {
    size_t totalSpace;
    size_t usedSpace;
    size_t freeSpace;
    size_t cacheSize;
    size_t offlineDataSize;
    int regionCount;
    int tileCount;
    
    StorageInfo()
        : totalSpace(0), usedSpace(0), freeSpace(0)
        , cacheSize(0), offlineDataSize(0)
        , regionCount(0), tileCount(0)
    {}
};

enum class StorageError {
    kNone = 0,
    kInsufficientSpace,
    kInvalidRegion,
    kDownloadFailed,
    kWriteFailed,
    kReadFailed,
    kDeleteFailed,
    kRegionExists,
    kRegionNotFound
};

class OGC_CACHE_API OfflineStorageManager {
public:
    using ProgressCallback = std::function<void(const DownloadProgress&)>;
    using CompletionCallback = std::function<void(StorageError)>;
    using RegionCallback = std::function<void(const OfflineRegion&)>;
    
    static std::unique_ptr<OfflineStorageManager> Create(const std::string& storagePath);
    
    virtual ~OfflineStorageManager();
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual std::string CreateRegion(const std::string& name,
                                      double minLon, double maxLon,
                                      double minLat, double maxLat,
                                      int minZoom, int maxZoom) = 0;
    
    virtual bool DeleteRegion(const std::string& regionId) = 0;
    virtual bool UpdateRegion(const std::string& regionId,
                              const OfflineRegion& updates) = 0;
    
    virtual OfflineRegion GetRegion(const std::string& regionId) const = 0;
    virtual std::vector<OfflineRegion> GetAllRegions() const = 0;
    virtual bool HasRegion(const std::string& regionId) const = 0;
    virtual size_t GetRegionCount() const = 0;
    
    virtual void StartDownload(const std::string& regionId,
                               ProgressCallback progressCallback,
                               CompletionCallback completionCallback) = 0;
    virtual void PauseDownload(const std::string& regionId) = 0;
    virtual void ResumeDownload(const std::string& regionId) = 0;
    virtual void CancelDownload(const std::string& regionId) = 0;
    
    virtual bool IsDownloading(const std::string& regionId) const = 0;
    virtual double GetDownloadProgress(const std::string& regionId) const = 0;
    
    virtual bool StoreTile(const std::string& regionId,
                           const TileKey& key,
                           const std::vector<uint8_t>& data) = 0;
    
    virtual bool HasTile(const std::string& regionId, const TileKey& key) const = 0;
    virtual std::vector<uint8_t> GetTile(const std::string& regionId,
                                         const TileKey& key) const = 0;
    virtual bool DeleteTile(const std::string& regionId, const TileKey& key) = 0;
    
    virtual StorageInfo GetStorageInfo() const = 0;
    virtual size_t GetTotalSpace() const = 0;
    virtual size_t GetUsedSpace() const = 0;
    virtual size_t GetFreeSpace() const = 0;
    
    virtual bool SetStorageLimit(size_t maxBytes) = 0;
    virtual size_t GetStorageLimit() const = 0;
    
    virtual void ClearCache() = 0;
    virtual void ClearAllOfflineData() = 0;
    
    virtual bool CompactStorage() = 0;
    virtual bool ValidateStorage() = 0;
    
    virtual void SetMaxConcurrentDownloads(int max) = 0;
    virtual int GetMaxConcurrentDownloads() const = 0;
    
    virtual void SetDownloadTimeout(int seconds) = 0;
    virtual int GetDownloadTimeout() const = 0;
    
    virtual void SetRetryCount(int count) = 0;
    virtual int GetRetryCount() const = 0;
    
    virtual std::string GetStoragePath() const = 0;
    virtual bool IsInitialized() const = 0;
    
    virtual void SetRegionChangedCallback(RegionCallback callback) = 0;
    
    static size_t CalculateRegionSize(double minLon, double maxLon,
                                       double minLat, double maxLat,
                                       int minZoom, int maxZoom);
    
    static int CalculateTileCount(double minLon, double maxLon,
                                   double minLat, double maxLat,
                                   int minZoom, int maxZoom);
    
protected:
    OfflineStorageManager() = default;
};

}
}

#endif
