#include "parser/parse_cache.h"
#include "parser/error_handler.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace chart {
namespace parser {

ParseCache& ParseCache::Instance() {
    static ParseCache instance;
    return instance;
}

ParseCache::ParseCache()
    : m_maxSize(100)
    , m_maxAge(3600)
    , m_enabled(true) {
}

ParseCache::~ParseCache() {
}

bool ParseCache::Get(const std::string& filePath, ParseResult& result) {
    if (!m_enabled) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(filePath);
    if (it == m_cache.end()) {
        m_stats.missCount++;
        return false;
    }
    
    if (IsExpired(it->second)) {
        m_cache.erase(it);
        m_stats.missCount++;
        m_stats.evictionCount++;
        return false;
    }
    
    result = it->second.result;
    m_stats.hitCount++;
    
    if (m_stats.hitCount + m_stats.missCount > 0) {
        m_stats.hitRate = static_cast<double>(m_stats.hitCount) / 
                          (m_stats.hitCount + m_stats.missCount);
    }
    
    LOG_DEBUG("Cache hit for file: {}", filePath);
    return true;
}

void ParseCache::Put(const std::string& filePath, const ParseResult& result) {
    if (!m_enabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    EvictIfNeeded();
    
    CacheEntry entry;
    entry.result = result;
    entry.timestamp = std::chrono::system_clock::now();
    entry.fileHash = ComputeFileHash(filePath);
    
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    if (GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &fileAttr)) {
        LARGE_INTEGER fileSize;
        fileSize.HighPart = fileAttr.nFileSizeHigh;
        fileSize.LowPart = fileAttr.nFileSizeLow;
        entry.fileSize = static_cast<size_t>(fileSize.QuadPart);
    }
#else
    struct stat st;
    if (stat(filePath.c_str(), &st) == 0) {
        entry.fileSize = static_cast<size_t>(st.st_size);
    }
#endif
    
    m_cache[filePath] = entry;
    LOG_DEBUG("Cached result for file: {}", filePath);
}

void ParseCache::Remove(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.erase(filePath);
    LOG_DEBUG("Removed cache entry for file: {}", filePath);
}

void ParseCache::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
    LOG_INFO("Cache cleared");
}

size_t ParseCache::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.size();
}

bool ParseCache::HasEntry(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.find(filePath) != m_cache.end();
}

void ParseCache::ResetStatistics() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stats = Statistics();
}

bool ParseCache::IsExpired(const CacheEntry& entry) const {
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - entry.timestamp);
    return age > m_maxAge;
}

void ParseCache::EvictIfNeeded() {
    while (m_cache.size() >= m_maxSize) {
        auto oldest = m_cache.begin();
        for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
            if (it->second.timestamp < oldest->second.timestamp) {
                oldest = it;
            }
        }
        m_cache.erase(oldest);
        m_stats.evictionCount++;
    }
}

std::string ParseCache::ComputeFileHash(const std::string& filePath) const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    std::stringstream ss;
    ss << filePath;
    
    file.seekg(0, std::ios::end);
    ss << file.tellg();
    file.seekg(0, std::ios::beg);
    
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        for (size_t i = 0; i < sizeof(buffer); ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') 
               << static_cast<int>(static_cast<unsigned char>(buffer[i]));
        }
    }
    
    size_t bytesRead = static_cast<size_t>(file.gcount());
    for (size_t i = 0; i < bytesRead; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<int>(static_cast<unsigned char>(buffer[i]));
    }
    
    return ss.str();
}

bool ParseCache::SaveToFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        LOG_ERROR("Failed to open cache file for writing: {}", filePath);
        return false;
    }
    
    size_t count = m_cache.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (const auto& pair : m_cache) {
        size_t pathLen = pair.first.size();
        file.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
        file.write(pair.first.c_str(), pathLen);
        
        const CacheEntry& entry = pair.second;
        
        auto timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
        file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        
        file.write(reinterpret_cast<const char*>(&entry.fileSize), sizeof(entry.fileSize));
        
        size_t hashLen = entry.fileHash.size();
        file.write(reinterpret_cast<const char*>(&hashLen), sizeof(hashLen));
        file.write(entry.fileHash.c_str(), hashLen);
    }
    
    LOG_INFO("Cache saved to file: {}", filePath);
    return true;
}

bool ParseCache::LoadFromFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        LOG_ERROR("Failed to open cache file for reading: {}", filePath);
        return false;
    }
    
    m_cache.clear();
    
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (size_t i = 0; i < count; ++i) {
        size_t pathLen;
        file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
        
        std::string path(pathLen, '\0');
        file.read(&path[0], pathLen);
        
        CacheEntry entry;
        
        time_t timestamp;
        file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        entry.timestamp = std::chrono::system_clock::from_time_t(timestamp);
        
        file.read(reinterpret_cast<char*>(&entry.fileSize), sizeof(entry.fileSize));
        
        size_t hashLen;
        file.read(reinterpret_cast<char*>(&hashLen), sizeof(hashLen));
        
        entry.fileHash.resize(hashLen);
        file.read(&entry.fileHash[0], hashLen);
        
        m_cache[path] = entry;
    }
    
    LOG_INFO("Cache loaded from file: {} ({} entries)", filePath, count);
    return true;
}

} // namespace parser
} // namespace chart
