#include "ogc/draw/offline_storage_manager.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <direct.h>
#undef min
#undef max
#undef GetFreeSpace
#define statFunc _stat
#define mkdirFunc(path) _mkdir(path)
#else
#include <sys/types.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <unistd.h>
#define statFunc stat
#define mkdirFunc(path) mkdir(path, 0755)
#endif

namespace ogc {
namespace draw {

namespace {

std::string GenerateId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::stringstream ss;
    ss << "region_" << ms;
    return ss.str();
}

std::string FormatTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool CreateDirectory(const std::string& path) {
    if (mkdirFunc(path.c_str()) == 0 || errno == EEXIST) {
        return true;
    }
    
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos || pos == 0) {
        return false;
    }
    
    std::string parent = path.substr(0, pos);
    if (!CreateDirectory(parent)) {
        return false;
    }
    
    return mkdirFunc(path.c_str()) == 0 || errno == EEXIST;
}

std::string JoinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    
    if (a.back() == sep || a.back() == '/') {
        return a + b;
    }
    return a + sep + b;
}

size_t GetDirectorySize(const std::string& path) {
    size_t size = 0;
    
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0) {
                std::string fullPath = JoinPath(path, findData.cFileName);
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    size += GetDirectorySize(fullPath);
                } else {
                    size += (findData.nFileSizeHigh * (MAXDWORD + 1)) + findData.nFileSizeLow;
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                std::string fullPath = JoinPath(path, entry->d_name);
                struct stat statBuf;
                if (stat(fullPath.c_str(), &statBuf) == 0) {
                    if (S_ISDIR(statBuf.st_mode)) {
                        size += GetDirectorySize(fullPath);
                    } else {
                        size += statBuf.st_size;
                    }
                }
            }
        }
        closedir(dir);
    }
#endif
    
    return size;
}

void DeleteDirectoryRecursive(const std::string& path) {
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0) {
                std::string fullPath = JoinPath(path, findData.cFileName);
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
                std::string fullPath = JoinPath(path, entry->d_name);
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

class OfflineStorageManagerImpl : public OfflineStorageManager {
public:
    OfflineStorageManagerImpl(const std::string& storagePath)
        : m_storagePath(storagePath)
        , m_initialized(false)
        , m_maxConcurrentDownloads(4)
        , m_downloadTimeout(30)
        , m_retryCount(3)
        , m_storageLimit(0)
    {
    }
    
    ~OfflineStorageManagerImpl() override {
        Shutdown();
    }
    
    bool Initialize() override {
        if (m_initialized) {
            return true;
        }
        
        if (!CreateDirectory(m_storagePath)) {
            return false;
        }
        
        m_regionsPath = JoinPath(m_storagePath, "regions");
        m_tilesPath = JoinPath(m_storagePath, "tiles");
        m_cachePath = JoinPath(m_storagePath, "cache");
        
        if (!CreateDirectory(m_regionsPath) ||
            !CreateDirectory(m_tilesPath) ||
            !CreateDirectory(m_cachePath)) {
            return false;
        }
        
        LoadRegions();
        m_initialized = true;
        return true;
    }
    
    void Shutdown() override {
        if (!m_initialized) {
            return;
        }
        
        m_regions.clear();
        m_initialized = false;
    }
    
    std::string CreateRegion(const std::string& name,
                              double minLon, double maxLon,
                              double minLat, double maxLat,
                              int minZoom, int maxZoom) override {
        std::string id = GenerateId();
        
        OfflineRegion region;
        region.id = id;
        region.name = name;
        region.minLon = minLon;
        region.maxLon = maxLon;
        region.minLat = minLat;
        region.maxLat = maxLat;
        region.minZoom = minZoom;
        region.maxZoom = maxZoom;
        region.createdAt = FormatTime();
        region.updatedAt = region.createdAt;
        region.dataSize = 0;
        region.tileCount = 0;
        region.isDownloading = false;
        region.downloadProgress = 0.0;
        
        m_regions[id] = region;
        SaveRegion(region);
        
        if (m_regionChangedCallback) {
            m_regionChangedCallback(region);
        }
        
        return id;
    }
    
    bool DeleteRegion(const std::string& regionId) override {
        auto it = m_regions.find(regionId);
        if (it == m_regions.end()) {
            return false;
        }
        
        std::string regionPath = JoinPath(m_tilesPath, regionId);
        DeleteDirectoryRecursive(regionPath);
        
        std::string metaPath = JoinPath(m_regionsPath, regionId + ".json");
#ifdef _WIN32
        _unlink(metaPath.c_str());
#else
        unlink(metaPath.c_str());
#endif
        
        m_regions.erase(it);
        return true;
    }
    
    bool UpdateRegion(const std::string& regionId,
                      const OfflineRegion& updates) override {
        auto it = m_regions.find(regionId);
        if (it == m_regions.end()) {
            return false;
        }
        
        it->second.name = updates.name;
        it->second.updatedAt = FormatTime();
        
        SaveRegion(it->second);
        
        if (m_regionChangedCallback) {
            m_regionChangedCallback(it->second);
        }
        
        return true;
    }
    
    OfflineRegion GetRegion(const std::string& regionId) const override {
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            return it->second;
        }
        return OfflineRegion();
    }
    
    std::vector<OfflineRegion> GetAllRegions() const override {
        std::vector<OfflineRegion> result;
        for (const auto& pair : m_regions) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    bool HasRegion(const std::string& regionId) const override {
        return m_regions.find(regionId) != m_regions.end();
    }
    
    size_t GetRegionCount() const override {
        return m_regions.size();
    }
    
    void StartDownload(const std::string& regionId,
                       ProgressCallback progressCallback,
                       CompletionCallback completionCallback) override {
        auto it = m_regions.find(regionId);
        if (it == m_regions.end()) {
            if (completionCallback) {
                completionCallback(StorageError::kRegionNotFound);
            }
            return;
        }
        
        it->second.isDownloading = true;
        it->second.downloadProgress = 0.0;
        
        DownloadProgress progress;
        progress.regionId = regionId;
        progress.totalTiles = CalculateTileCount(
            it->second.minLon, it->second.maxLon,
            it->second.minLat, it->second.maxLat,
            it->second.minZoom, it->second.maxZoom);
        progress.downloadedTiles = 0;
        progress.failedTiles = 0;
        progress.progress = 0.0;
        progress.status = "Started";
        
        if (progressCallback) {
            progressCallback(progress);
        }
        
        if (completionCallback) {
            completionCallback(StorageError::kNone);
        }
    }
    
    void PauseDownload(const std::string& regionId) override {
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            it->second.isDownloading = false;
        }
    }
    
    void ResumeDownload(const std::string& regionId) override {
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            it->second.isDownloading = true;
        }
    }
    
    void CancelDownload(const std::string& regionId) override {
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            it->second.isDownloading = false;
            it->second.downloadProgress = 0.0;
        }
    }
    
    bool IsDownloading(const std::string& regionId) const override {
        auto it = m_regions.find(regionId);
        return it != m_regions.end() && it->second.isDownloading;
    }
    
    double GetDownloadProgress(const std::string& regionId) const override {
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            return it->second.downloadProgress;
        }
        return 0.0;
    }
    
    bool StoreTile(const std::string& regionId,
                   const TileKey& key,
                   const std::vector<uint8_t>& data) override {
        if (!m_initialized || !HasRegion(regionId)) {
            return false;
        }
        
        std::string tilePath = GetTilePath(regionId, key);
        std::string dirPath = tilePath.substr(0, tilePath.find_last_of("/\\"));
        
        CreateDirectory(dirPath);
        
        std::ofstream file(tilePath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        
        auto it = m_regions.find(regionId);
        if (it != m_regions.end()) {
            it->second.dataSize += data.size();
            it->second.tileCount++;
            SaveRegion(it->second);
        }
        
        return true;
    }
    
    bool HasTile(const std::string& regionId, const TileKey& key) const override {
        std::string tilePath = GetTilePath(regionId, key);
        
        struct stat statBuf;
        return stat(tilePath.c_str(), &statBuf) == 0;
    }
    
    std::vector<uint8_t> GetTile(const std::string& regionId,
                                 const TileKey& key) const override {
        std::vector<uint8_t> data;
        std::string tilePath = GetTilePath(regionId, key);
        
        std::ifstream file(tilePath, std::ios::binary | std::ios::ate);
        if (!file) {
            return data;
        }
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        
        return data;
    }
    
    bool DeleteTile(const std::string& regionId, const TileKey& key) override {
        std::string tilePath = GetTilePath(regionId, key);
        
#ifdef _WIN32
        return _unlink(tilePath.c_str()) == 0;
#else
        return unlink(tilePath.c_str()) == 0;
#endif
    }
    
    StorageInfo GetStorageInfo() const override {
        StorageInfo info;
        info.totalSpace = GetTotalSpace();
        info.usedSpace = GetUsedSpace();
        info.freeSpace = GetFreeSpace();
        info.cacheSize = GetDirectorySize(m_cachePath);
        info.offlineDataSize = GetDirectorySize(m_tilesPath);
        info.regionCount = static_cast<int>(m_regions.size());
        info.tileCount = 0;
        
        for (const auto& pair : m_regions) {
            info.tileCount += static_cast<int>(pair.second.tileCount);
        }
        
        return info;
    }
    
    size_t GetTotalSpace() const override {
#ifdef _WIN32
        ULARGE_INTEGER totalBytes;
        if (GetDiskFreeSpaceExA(m_storagePath.c_str(), nullptr, &totalBytes, nullptr)) {
            return totalBytes.QuadPart;
        }
#else
        struct statvfs statBuf;
        if (statvfs(m_storagePath.c_str(), &statBuf) == 0) {
            return statBuf.f_blocks * statBuf.f_frsize;
        }
#endif
        return 0;
    }
    
    size_t GetUsedSpace() const override {
        return GetDirectorySize(m_storagePath);
    }
    
    size_t GetFreeSpace() const override {
#ifdef _WIN32
        ULARGE_INTEGER freeBytes;
        if (GetDiskFreeSpaceExA(m_storagePath.c_str(), &freeBytes, nullptr, nullptr)) {
            return freeBytes.QuadPart;
        }
#else
        struct statvfs statBuf;
        if (statvfs(m_storagePath.c_str(), &statBuf) == 0) {
            return statBuf.f_bavail * statBuf.f_frsize;
        }
#endif
        return 0;
    }
    
    bool SetStorageLimit(size_t maxBytes) override {
        m_storageLimit = maxBytes;
        return true;
    }
    
    size_t GetStorageLimit() const override {
        return m_storageLimit;
    }
    
    void ClearCache() override {
        DeleteDirectoryRecursive(m_cachePath);
        CreateDirectory(m_cachePath);
    }
    
    void ClearAllOfflineData() override {
        for (auto& pair : m_regions) {
            std::string regionPath = JoinPath(m_tilesPath, pair.first);
            DeleteDirectoryRecursive(regionPath);
            pair.second.dataSize = 0;
            pair.second.tileCount = 0;
        }
    }
    
    bool CompactStorage() override {
        return true;
    }
    
    bool ValidateStorage() override {
        for (const auto& pair : m_regions) {
            std::string metaPath = JoinPath(m_regionsPath, pair.first + ".json");
            
            struct stat statBuf;
            if (stat(metaPath.c_str(), &statBuf) != 0) {
                return false;
            }
        }
        return true;
    }
    
    void SetMaxConcurrentDownloads(int max) override {
        m_maxConcurrentDownloads = std::max(1, max);
    }
    
    int GetMaxConcurrentDownloads() const override {
        return m_maxConcurrentDownloads;
    }
    
    void SetDownloadTimeout(int seconds) override {
        m_downloadTimeout = std::max(1, seconds);
    }
    
    int GetDownloadTimeout() const override {
        return m_downloadTimeout;
    }
    
    void SetRetryCount(int count) override {
        m_retryCount = std::max(0, count);
    }
    
    int GetRetryCount() const override {
        return m_retryCount;
    }
    
    std::string GetStoragePath() const override {
        return m_storagePath;
    }
    
    bool IsInitialized() const override {
        return m_initialized;
    }
    
    void SetRegionChangedCallback(RegionCallback callback) override {
        m_regionChangedCallback = callback;
    }
    
private:
    std::string m_storagePath;
    std::string m_regionsPath;
    std::string m_tilesPath;
    std::string m_cachePath;
    bool m_initialized;
    int m_maxConcurrentDownloads;
    int m_downloadTimeout;
    int m_retryCount;
    size_t m_storageLimit;
    
    std::map<std::string, OfflineRegion> m_regions;
    RegionCallback m_regionChangedCallback;
    
    std::string GetTilePath(const std::string& regionId, const TileKey& key) const {
        std::stringstream ss;
        ss << m_tilesPath << "/" << regionId << "/" 
           << key.z << "/" << key.x << "/" << key.y << ".tile";
        return ss.str();
    }
    
    void LoadRegions() {
        m_regions.clear();
        
#ifdef _WIN32
        std::string searchPath = m_regionsPath + "\\*.json";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::string filename = findData.cFileName;
                std::string id = filename.substr(0, filename.length() - 5);
                LoadRegion(id);
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
#else
        DIR* dir = opendir(m_regionsPath.c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string filename = entry->d_name;
                if (filename.length() > 5 && 
                    filename.substr(filename.length() - 5) == ".json") {
                    std::string id = filename.substr(0, filename.length() - 5);
                    LoadRegion(id);
                }
            }
            closedir(dir);
        }
#endif
    }
    
    void LoadRegion(const std::string& regionId) {
        std::string metaPath = JoinPath(m_regionsPath, regionId + ".json");
        std::ifstream file(metaPath);
        if (!file) {
            return;
        }
        
        OfflineRegion region;
        region.id = regionId;
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (key == "name") region.name = value;
                else if (key == "minLon") region.minLon = std::stod(value);
                else if (key == "maxLon") region.maxLon = std::stod(value);
                else if (key == "minLat") region.minLat = std::stod(value);
                else if (key == "maxLat") region.maxLat = std::stod(value);
                else if (key == "minZoom") region.minZoom = std::stoi(value);
                else if (key == "maxZoom") region.maxZoom = std::stoi(value);
                else if (key == "createdAt") region.createdAt = value;
                else if (key == "updatedAt") region.updatedAt = value;
                else if (key == "dataSize") region.dataSize = std::stoull(value);
                else if (key == "tileCount") region.tileCount = std::stoull(value);
            }
        }
        
        m_regions[regionId] = region;
    }
    
    void SaveRegion(const OfflineRegion& region) {
        std::string metaPath = JoinPath(m_regionsPath, region.id + ".json");
        std::ofstream file(metaPath);
        if (!file) {
            return;
        }
        
        file << "name:" << region.name << "\n";
        file << "minLon:" << region.minLon << "\n";
        file << "maxLon:" << region.maxLon << "\n";
        file << "minLat:" << region.minLat << "\n";
        file << "maxLat:" << region.maxLat << "\n";
        file << "minZoom:" << region.minZoom << "\n";
        file << "maxZoom:" << region.maxZoom << "\n";
        file << "createdAt:" << region.createdAt << "\n";
        file << "updatedAt:" << region.updatedAt << "\n";
        file << "dataSize:" << region.dataSize << "\n";
        file << "tileCount:" << region.tileCount << "\n";
    }
};

OfflineStorageManager::~OfflineStorageManager() {
}

std::unique_ptr<OfflineStorageManager> OfflineStorageManager::Create(const std::string& storagePath) {
    return std::unique_ptr<OfflineStorageManager>(new OfflineStorageManagerImpl(storagePath));
}

size_t OfflineStorageManager::CalculateRegionSize(double minLon, double maxLon,
                                                   double minLat, double maxLat,
                                                   int minZoom, int maxZoom) {
    int tileCount = CalculateTileCount(minLon, maxLon, minLat, maxLat, minZoom, maxZoom);
    return tileCount * 20480;
}

int OfflineStorageManager::CalculateTileCount(double minLon, double maxLon,
                                               double minLat, double maxLat,
                                               int minZoom, int maxZoom) {
    long long totalTiles = 0;
    
    for (int z = minZoom; z <= maxZoom; ++z) {
        long long minTileX = static_cast<long long>(std::floor((minLon + 180.0) / 360.0 * (1 << z)));
        long long maxTileX = static_cast<long long>(std::floor((maxLon + 180.0) / 360.0 * (1 << z)));
        long long minTileY = static_cast<long long>(std::floor((1.0 - std::log(std::tan(maxLat * M_PI / 180.0) + 
                              1.0 / std::cos(maxLat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z)));
        long long maxTileY = static_cast<long long>(std::floor((1.0 - std::log(std::tan(minLat * M_PI / 180.0) + 
                              1.0 / std::cos(minLat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z)));
        
        long long tilesX = maxTileX - minTileX + 1;
        long long tilesY = maxTileY - minTileY + 1;
        
        totalTiles += tilesX * tilesY;
    }
    
    return static_cast<int>(totalTiles);
}

}
}
