#include "ogc/draw/disk_tile_cache.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

namespace ogc {
namespace draw {

DiskTileCache::DiskTileCache(const std::string& cachePath, size_t maxSize)
    : m_cachePath(cachePath)
    , m_name("DiskTileCache")
    , m_maxSize(maxSize)
    , m_currentSize(0)
    , m_tileCount(0)
    , m_enabled(true)
    , m_compressionEnabled(false)
    , m_expirationTime(0)
    , m_indexLoaded(false)
{
    EnsureDirectoryExists(m_cachePath);
    LoadIndex();
}

DiskTileCache::~DiskTileCache() {
    Flush();
}

bool DiskTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled) {
        return false;
    }
    
    auto it = m_index.find(key);
    if (it == m_index.end()) {
        return false;
    }
    
    if (IsExpired(it->second)) {
        return false;
    }
    
    return true;
}

TileData DiskTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    TileData result;
    
    if (!m_enabled) {
        return result;
    }
    
    auto it = m_index.find(key);
    if (it == m_index.end()) {
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
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled || data.empty()) {
        return false;
    }
    
    std::string filePath = GetTileFilePath(key);
    if (filePath.empty()) {
        return false;
    }
    
    auto existingIt = m_index.find(key);
    if (existingIt != m_index.end()) {
        m_currentSize -= existingIt->second.size;
        m_tileCount--;
    }
    
    while (m_currentSize + data.size() > m_maxSize && !m_index.empty()) {
        RemoveLRU();
    }
    
    if (!WriteTileData(filePath, data)) {
        return false;
    }
    
    UpdateIndex(key, filePath, data.size());
    m_currentSize += data.size();
    m_tileCount++;
    
    return true;
}

bool DiskTileCache::RemoveTile(const TileKey& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_index.find(key);
    if (it == m_index.end()) {
        return false;
    }
    
    std::string filePath = it->second.filePath;
    
    if (remove(filePath.c_str()) == 0 || errno == ENOENT) {
        m_currentSize -= it->second.size;
        m_tileCount--;
        m_index.erase(it);
        return true;
    }
    
    return false;
}

void DiskTileCache::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& pair : m_index) {
        remove(pair.second.filePath.c_str());
    }
    
    m_index.clear();
    m_currentSize = 0;
    m_tileCount = 0;
    
    SaveIndex();
}

size_t DiskTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tileCount;
}

size_t DiskTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSize;
}

size_t DiskTileCache::GetMaxSize() const {
    return m_maxSize;
}

void DiskTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxSize = maxSize;
    
    while (m_currentSize > m_maxSize && !m_index.empty()) {
        RemoveLRU();
    }
}

bool DiskTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSize >= m_maxSize;
}

std::string DiskTileCache::GetName() const {
    return m_name;
}

void DiskTileCache::SetName(const std::string& name) {
    m_name = name;
}

bool DiskTileCache::IsEnabled() const {
    return m_enabled;
}

void DiskTileCache::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

void DiskTileCache::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    SaveIndex();
}

bool DiskTileCache::SetCachePath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!EnsureDirectoryExists(path)) {
        return false;
    }
    
    m_cachePath = path;
    m_indexLoaded = false;
    m_index.clear();
    m_currentSize = 0;
    m_tileCount = 0;
    
    return LoadIndex();
}

std::string DiskTileCache::GetCachePath() const {
    return m_cachePath;
}

void DiskTileCache::SetCompressionEnabled(bool enabled) {
    m_compressionEnabled = enabled;
}

bool DiskTileCache::IsCompressionEnabled() const {
    return m_compressionEnabled;
}

void DiskTileCache::SetExpirationTime(int64_t seconds) {
    m_expirationTime = seconds;
}

int64_t DiskTileCache::GetExpirationTime() const {
    return m_expirationTime;
}

void DiskTileCache::Cleanup() {
    std::lock_guard<std::mutex> lock(m_mutex);
    RemoveExpired();
}

std::shared_ptr<DiskTileCache> DiskTileCache::Create(const std::string& cachePath, size_t maxSize) {
    return std::make_shared<DiskTileCache>(cachePath, maxSize);
}

std::string DiskTileCache::GetTileFilePath(const TileKey& key) const {
    return GetTileFilePath(m_cachePath, key.x, key.y, key.z);
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
    if (m_indexLoaded) {
        return true;
    }
    
    std::string indexPath = GetIndexPath();
    std::ifstream file(indexPath, std::ios::binary);
    
    if (!file.is_open()) {
        m_indexLoaded = true;
        return true;
    }
    
    m_index.clear();
    m_currentSize = 0;
    m_tileCount = 0;
    
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
        
        if (m_expirationTime > 0 && currentTime - timestamp > m_expirationTime) {
            remove(filePath.c_str());
            continue;
        }
        
        struct stat st;
        if (stat(filePath.c_str(), &st) != 0) {
            continue;
        }
        
        m_index[key] = entry;
        m_currentSize += size;
        m_tileCount++;
    }
    
    m_indexLoaded = true;
    return true;
}

bool DiskTileCache::SaveIndex() {
    std::string indexPath = GetIndexPath();
    
    std::string tempPath = indexPath + ".tmp";
    std::ofstream file(tempPath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : m_index) {
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
    
    m_index[key] = entry;
}

void DiskTileCache::RemoveFromIndex(const TileKey& key) const {
    auto it = m_index.find(key);
    if (it != m_index.end()) {
        remove(it->second.filePath.c_str());
        const_cast<size_t&>(m_currentSize) -= it->second.size;
        const_cast<size_t&>(m_tileCount)--;
        m_index.erase(it);
    }
}

bool DiskTileCache::IsExpired(const CacheIndex& entry) const {
    if (m_expirationTime <= 0) {
        return false;
    }
    
    int64_t currentTime = static_cast<int64_t>(std::time(nullptr));
    return (currentTime - entry.timestamp) > m_expirationTime;
}

void DiskTileCache::RemoveExpired() {
    if (m_expirationTime <= 0) {
        return;
    }
    
    std::vector<TileKey> expiredKeys;
    for (const auto& pair : m_index) {
        if (IsExpired(pair.second)) {
            expiredKeys.push_back(pair.first);
        }
    }
    
    for (const auto& key : expiredKeys) {
        RemoveFromIndex(key);
    }
}

void DiskTileCache::RemoveLRU() {
    if (m_index.empty()) {
        return;
    }
    
    auto oldest = m_index.begin();
    for (auto it = m_index.begin(); it != m_index.end(); ++it) {
        if (it->second.lastAccess < oldest->second.lastAccess) {
            oldest = it;
        }
    }
    
    RemoveFromIndex(oldest->first);
}

std::string DiskTileCache::GetIndexPath() const {
    std::string indexPath = m_cachePath;
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
