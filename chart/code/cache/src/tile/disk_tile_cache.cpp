#include "ogc/cache/tile/disk_tile_cache.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <map>
#include <mutex>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

namespace ogc {
namespace cache {

struct DiskTileCache::Impl {
    std::string cachePath;
    std::string name;
    size_t maxSize;
    size_t currentSize;
    size_t tileCount;
    bool enabled;
    bool compressionEnabled;
    int64_t expirationTime;
    mutable std::mutex mutex;
    
    mutable std::map<TileKey, CacheIndex> index;
    mutable bool indexLoaded;
    
    Impl() : maxSize(0), currentSize(0), tileCount(0), enabled(true),
             compressionEnabled(false), expirationTime(0), indexLoaded(false) {}
};

DiskTileCache::DiskTileCache(const std::string& cachePath, size_t maxSize)
    : impl_(new Impl())
{
    impl_->cachePath = cachePath;
    impl_->name = "DiskTileCache";
    impl_->maxSize = maxSize;
    EnsureDirectoryExists(impl_->cachePath);
    LoadIndex();
}

DiskTileCache::~DiskTileCache() {
    Flush();
}

bool DiskTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->enabled) {
        return false;
    }
    
    auto it = impl_->index.find(key);
    if (it == impl_->index.end()) {
        return false;
    }
    
    if (IsExpired(it->second)) {
        return false;
    }
    
    return true;
}

TileData DiskTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    TileData result;
    
    if (!impl_->enabled) {
        return result;
    }
    
    auto it = impl_->index.find(key);
    if (it == impl_->index.end()) {
        return result;
    }
    
    if (IsExpired(it->second)) {
        RemoveFromIndex(key);
        return result;
    }
    
    std::vector<uint8_t> data;
    if (ReadTileData(it->second.filePath, data)) {
        result.key = key;
        result.data = std::move(data);
        result.timestamp = it->second.timestamp;
        result.size = it->second.size;
        result.valid = true;
        
        const_cast<CacheIndex&>(it->second).lastAccess = static_cast<int64_t>(std::time(nullptr));
    }
    
    return result;
}

bool DiskTileCache::PutTile(const TileKey& key, const TileData& tile) {
    return PutTile(key, tile.data);
}

bool DiskTileCache::PutTile(const TileKey& key, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->enabled || data.empty()) {
        return false;
    }
    
    std::string filePath = GetTileFilePath(key);
    if (filePath.empty()) {
        return false;
    }
    
    auto existingIt = impl_->index.find(key);
    if (existingIt != impl_->index.end()) {
        impl_->currentSize -= existingIt->second.size;
        impl_->tileCount--;
    }
    
    while (impl_->currentSize + data.size() > impl_->maxSize && !impl_->index.empty()) {
        RemoveLRU();
    }
    
    if (!WriteTileData(filePath, data)) {
        return false;
    }
    
    UpdateIndex(key, filePath, data.size());
    impl_->currentSize += data.size();
    impl_->tileCount++;
    
    return true;
}

bool DiskTileCache::RemoveTile(const TileKey& key) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->index.find(key);
    if (it == impl_->index.end()) {
        return false;
    }
    
    std::string filePath = it->second.filePath;
    
    if (remove(filePath.c_str()) == 0 || errno == ENOENT) {
        impl_->currentSize -= it->second.size;
        impl_->tileCount--;
        impl_->index.erase(it);
        return true;
    }
    
    return false;
}

void DiskTileCache::Clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& pair : impl_->index) {
        remove(pair.second.filePath.c_str());
    }
    
    impl_->index.clear();
    impl_->currentSize = 0;
    impl_->tileCount = 0;
    
    SaveIndex();
}

size_t DiskTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->tileCount;
}

size_t DiskTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentSize;
}

size_t DiskTileCache::GetMaxSize() const {
    return impl_->maxSize;
}

void DiskTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->maxSize = maxSize;
    
    while (impl_->currentSize > impl_->maxSize && !impl_->index.empty()) {
        RemoveLRU();
    }
}

bool DiskTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentSize >= impl_->maxSize;
}

std::string DiskTileCache::GetName() const {
    return impl_->name;
}

void DiskTileCache::SetName(const std::string& name) {
    impl_->name = name;
}

bool DiskTileCache::IsEnabled() const {
    return impl_->enabled;
}

void DiskTileCache::SetEnabled(bool enabled) {
    impl_->enabled = enabled;
}

void DiskTileCache::Flush() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SaveIndex();
}

bool DiskTileCache::SetCachePath(const std::string& path) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!EnsureDirectoryExists(path)) {
        return false;
    }
    
    impl_->cachePath = path;
    impl_->indexLoaded = false;
    impl_->index.clear();
    impl_->currentSize = 0;
    impl_->tileCount = 0;
    
    return LoadIndex();
}

std::string DiskTileCache::GetCachePath() const {
    return impl_->cachePath;
}

void DiskTileCache::SetCompressionEnabled(bool enabled) {
    impl_->compressionEnabled = enabled;
}

bool DiskTileCache::IsCompressionEnabled() const {
    return impl_->compressionEnabled;
}

void DiskTileCache::SetExpirationTime(int64_t seconds) {
    impl_->expirationTime = seconds;
}

int64_t DiskTileCache::GetExpirationTime() const {
    return impl_->expirationTime;
}

void DiskTileCache::Cleanup() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    RemoveExpired();
}

std::shared_ptr<DiskTileCache> DiskTileCache::Create(const std::string& cachePath, size_t maxSize) {
    return std::make_shared<DiskTileCache>(cachePath, maxSize);
}

std::string DiskTileCache::GetTileFilePath(const TileKey& key) const {
    return GetTileFilePath(impl_->cachePath, key.x, key.y, key.z);
}

std::string DiskTileCache::GetTileFilePath(const std::string& cachePath, int x, int y, int z) const {
    std::ostringstream oss;
    oss << cachePath;
    if (!cachePath.empty() && cachePath.back() != '/' && cachePath.back() != '\\') {
        oss << "/";
    }
    oss << z << "/" << x << "/" << y << ".tile";
    return oss.str();
}

bool DiskTileCache::EnsureDirectoryExists(const std::string& path) const {
    if (path.empty()) {
        return false;
    }
    
    std::string currentPath;
    std::istringstream iss(path);
    std::string segment;
    
#ifdef _WIN32
    if (path.size() >= 2 && path[1] == ':') {
        currentPath = path.substr(0, 2);
        iss.str(path.substr(3));
        iss.clear();
    }
#endif
    
    while (std::getline(iss, segment, '/')) {
        if (segment.empty()) continue;
        
        if (!currentPath.empty()) {
            currentPath += "/";
        }
        currentPath += segment;
        
        struct stat st;
        if (stat(currentPath.c_str(), &st) != 0) {
            if (MKDIR(currentPath.c_str()) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool DiskTileCache::LoadIndex() {
    if (impl_->indexLoaded) {
        return true;
    }
    
    std::string indexPath = GetIndexPath();
    std::ifstream file(indexPath, std::ios::binary);
    
    if (!file.is_open()) {
        impl_->indexLoaded = true;
        return true;
    }
    
    impl_->index.clear();
    impl_->currentSize = 0;
    impl_->tileCount = 0;
    
    int64_t currentTime = static_cast<int64_t>(std::time(nullptr));
    
    while (file.good()) {
        int z, x, y;
        size_t size;
        int64_t timestamp, lastAccess;
        std::string filePath;
        
        file.read(reinterpret_cast<char*>(&z), sizeof(z));
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        file.read(reinterpret_cast<char*>(&lastAccess), sizeof(lastAccess));
        
        size_t pathLen;
        file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
        if (pathLen > 0) {
            filePath.resize(pathLen);
            file.read(&filePath[0], pathLen);
        }
        
        if (!file.good()) break;
        
        TileKey key(x, y, z);
        CacheIndex entry;
        entry.key = key;
        entry.filePath = filePath;
        entry.size = size;
        entry.timestamp = timestamp;
        entry.lastAccess = lastAccess;
        
        if (impl_->expirationTime > 0 && currentTime - timestamp > impl_->expirationTime) {
            remove(filePath.c_str());
            continue;
        }
        
        struct stat st;
        if (stat(filePath.c_str(), &st) != 0) {
            continue;
        }
        
        impl_->index[key] = entry;
        impl_->currentSize += size;
        impl_->tileCount++;
    }
    
    impl_->indexLoaded = true;
    return true;
}

bool DiskTileCache::SaveIndex() {
    std::string indexPath = GetIndexPath();
    
    std::string tempPath = indexPath + ".tmp";
    std::ofstream file(tempPath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : impl_->index) {
        const CacheIndex& entry = pair.second;
        
        int z = entry.key.z;
        int x = entry.key.x;
        int y = entry.key.y;
        
        file.write(reinterpret_cast<const char*>(&z), sizeof(z));
        file.write(reinterpret_cast<const char*>(&x), sizeof(x));
        file.write(reinterpret_cast<const char*>(&y), sizeof(y));
        file.write(reinterpret_cast<const char*>(&entry.size), sizeof(entry.size));
        file.write(reinterpret_cast<const char*>(&entry.timestamp), sizeof(entry.timestamp));
        file.write(reinterpret_cast<const char*>(&entry.lastAccess), sizeof(entry.lastAccess));
        
        size_t pathLen = entry.filePath.size();
        file.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
        if (pathLen > 0) {
            file.write(entry.filePath.c_str(), pathLen);
        }
    }
    
    file.close();
    
    remove(indexPath.c_str());
    if (rename(tempPath.c_str(), indexPath.c_str()) != 0) {
        remove(tempPath.c_str());
        return false;
    }
    
    return true;
}

void DiskTileCache::UpdateIndex(const TileKey& key, const std::string& filePath, size_t size) {
    CacheIndex entry;
    entry.key = key;
    entry.filePath = filePath;
    entry.size = size;
    entry.timestamp = static_cast<int64_t>(std::time(nullptr));
    entry.lastAccess = entry.timestamp;
    
    impl_->index[key] = entry;
}

void DiskTileCache::RemoveFromIndex(const TileKey& key) const {
    auto it = impl_->index.find(key);
    if (it != impl_->index.end()) {
        remove(it->second.filePath.c_str());
        const_cast<size_t&>(impl_->currentSize) -= it->second.size;
        const_cast<size_t&>(impl_->tileCount)--;
        impl_->index.erase(it);
    }
}

bool DiskTileCache::IsExpired(const CacheIndex& entry) const {
    if (impl_->expirationTime <= 0) {
        return false;
    }
    
    int64_t currentTime = static_cast<int64_t>(std::time(nullptr));
    return (currentTime - entry.timestamp) > impl_->expirationTime;
}

void DiskTileCache::RemoveExpired() {
    if (impl_->expirationTime <= 0) {
        return;
    }
    
    std::vector<TileKey> expiredKeys;
    for (const auto& pair : impl_->index) {
        if (IsExpired(pair.second)) {
            expiredKeys.push_back(pair.first);
        }
    }
    
    for (const auto& key : expiredKeys) {
        RemoveFromIndex(key);
    }
}

void DiskTileCache::RemoveLRU() {
    if (impl_->index.empty()) {
        return;
    }
    
    auto oldest = impl_->index.begin();
    for (auto it = impl_->index.begin(); it != impl_->index.end(); ++it) {
        if (it->second.lastAccess < oldest->second.lastAccess) {
            oldest = it;
        }
    }
    
    RemoveFromIndex(oldest->first);
}

std::string DiskTileCache::GetIndexPath() const {
    std::string indexPath = impl_->cachePath;
    if (!indexPath.empty() && indexPath.back() != '/' && indexPath.back() != '\\') {
        indexPath += "/";
    }
    indexPath += "cache_index.dat";
    return indexPath;
}

bool DiskTileCache::WriteTileData(const std::string& filePath, const std::vector<uint8_t>& data) {
    std::string dirPath = filePath;
    size_t lastSlash = dirPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirPath = dirPath.substr(0, lastSlash);
        if (!EnsureDirectoryExists(dirPath)) {
            return false;
        }
    }
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool DiskTileCache::ReadTileData(const std::string& filePath, std::vector<uint8_t>& data) const {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return file.good();
}

}
}
