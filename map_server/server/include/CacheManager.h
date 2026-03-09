#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>

namespace cycle {

// 缓存项
struct CacheItem {
    std::vector<uint8_t> data;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastAccessed;
    size_t accessCount;

    bool IsExpired(int ttlSeconds) const {
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - createdAt);
        return age.count() > ttlSeconds;
    }
};

// 内存缓存
class MemoryCache {
public:
    MemoryCache(size_t maxItems, int ttlSeconds);

    bool Get(const std::string& key, std::vector<uint8_t>& data);
    void Put(const std::string& key, const std::vector<uint8_t>& data);
    void Remove(const std::string& key);
    void Clear();

    // 获取缓存统计
    struct Stats {
        size_t hitCount;
        size_t missCount;
        size_t itemCount;
        size_t totalSize;
        double hitRate;  // 命中率
    };
    Stats GetStats() const;

private:
    void EvictIfNeeded();

    size_t m_maxItems;
    int m_ttlSeconds;
    std::unordered_map<std::string, CacheItem> m_cache;
    mutable std::shared_mutex m_mutex;

    // 统计
    mutable std::atomic<size_t> m_hitCount{0};
    mutable std::atomic<size_t> m_missCount{0};
};

// 简化的缓存管理器（仅内存缓存）
class CacheManager {
public:
    CacheManager(size_t maxItems = 1000, int ttlSeconds = 3600);

    bool Get(const std::string& key, std::vector<uint8_t>& data);
    void Put(const std::string& key, const std::vector<uint8_t>& data);
    void Remove(const std::string& key);
    void Clear();

    // 获取统计信息
    std::string GetStatsJson() const;

private:
    std::unique_ptr<MemoryCache> m_memoryCache;
};

} // namespace cycle
