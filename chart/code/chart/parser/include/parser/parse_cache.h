#ifndef PARSE_CACHE_H
#define PARSE_CACHE_H

#include "parse_result.h"
#include <string>
#include <map>
#include <mutex>
#include <memory>
#include <chrono>

namespace chart {
namespace parser {

struct CacheEntry {
    ParseResult result;
    std::chrono::system_clock::time_point timestamp;
    std::string fileHash;
    size_t fileSize;
    
    CacheEntry() : fileSize(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

class ParseCache {
public:
    static ParseCache& Instance();
    
    bool Get(const std::string& filePath, ParseResult& result);
    void Put(const std::string& filePath, const ParseResult& result);
    void Remove(const std::string& filePath);
    void Clear();
    
    void SetMaxSize(size_t maxSize) { m_maxSize = maxSize; }
    size_t GetMaxSize() const { return m_maxSize; }
    
    void SetMaxAge(std::chrono::seconds maxAge) { m_maxAge = maxAge; }
    std::chrono::seconds GetMaxAge() const { return m_maxAge; }
    
    size_t GetSize() const;
    bool HasEntry(const std::string& filePath) const;
    
    void Enable(bool enable) { m_enabled = enable; }
    bool IsEnabled() const { return m_enabled; }
    
    void SetCacheDirectory(const std::string& dir) { m_cacheDirectory = dir; }
    std::string GetCacheDirectory() const { return m_cacheDirectory; }
    
    bool SaveToFile(const std::string& filePath);
    bool LoadFromFile(const std::string& filePath);
    
    struct Statistics {
        size_t hitCount;
        size_t missCount;
        size_t evictionCount;
        double hitRate;
        
        Statistics() : hitCount(0), missCount(0), evictionCount(0), hitRate(0.0) {}
    };
    
    Statistics GetStatistics() const { return m_stats; }
    void ResetStatistics();
    
private:
    ParseCache();
    ~ParseCache();
    
    ParseCache(const ParseCache&) = delete;
    ParseCache& operator=(const ParseCache&) = delete;
    
    bool IsExpired(const CacheEntry& entry) const;
    void EvictIfNeeded();
    std::string ComputeFileHash(const std::string& filePath) const;
    
    std::map<std::string, CacheEntry> m_cache;
    mutable std::mutex m_mutex;
    size_t m_maxSize;
    std::chrono::seconds m_maxAge;
    bool m_enabled;
    std::string m_cacheDirectory;
    Statistics m_stats;
};

} // namespace parser
} // namespace chart

#endif // PARSE_CACHE_H
