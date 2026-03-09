#include "CacheManager.h"
#include <algorithm>
#include <sstream>

namespace cycle {

MemoryCache::MemoryCache(size_t maxItems, int ttlSeconds)
    : m_maxItems(maxItems), m_ttlSeconds(ttlSeconds) {
}

bool MemoryCache::Get(const std::string& key, std::vector<uint8_t>& data) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto it = m_cache.find(key);
    if (it == m_cache.end()) {
        m_missCount++;
        return false;
    }

    // 检查是否过期
    if (it->second.IsExpired(m_ttlSeconds)) {
        lock.unlock();
        std::unique_lock<std::shared_mutex> writeLock(m_mutex);
        m_cache.erase(key);
        m_missCount++;
        return false;
    }

    data = it->second.data;
    it->second.lastAccessed = std::chrono::system_clock::now();
    it->second.accessCount++;

    m_hitCount++;
    return true;
}

void MemoryCache::Put(const std::string& key, const std::vector<uint8_t>& data) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);

    // 检查是否需要淘汰
    if (m_cache.size() >= m_maxItems) {
        EvictIfNeeded();
    }

    CacheItem item;
    item.data = data;
    item.createdAt = std::chrono::system_clock::now();
    item.lastAccessed = item.createdAt;
    item.accessCount = 0;

    m_cache[key] = item;
}

void MemoryCache::Remove(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_cache.erase(key);
}

void MemoryCache::Clear() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_cache.clear();
    m_hitCount = 0;
    m_missCount = 0;
}

MemoryCache::Stats MemoryCache::GetStats() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    Stats stats;
    stats.hitCount = m_hitCount.load();
    stats.missCount = m_missCount.load();
    stats.itemCount = m_cache.size();

    size_t totalSize = 0;
    for (const auto& pair : m_cache) {
        totalSize += pair.second.data.size();
    }
    stats.totalSize = totalSize;

    size_t totalRequests = stats.hitCount + stats.missCount;
    stats.hitRate = totalRequests > 0 ?
        (static_cast<double>(stats.hitCount) / totalRequests) * 100.0 : 0.0;

    return stats;
}

void MemoryCache::EvictIfNeeded() {
    // 简单淘汰策略：移除最少访问且最旧的项
    if (m_cache.empty()) return;

    auto oldestIt = m_cache.begin();
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (it->second.accessCount < oldestIt->second.accessCount ||
            (it->second.accessCount == oldestIt->second.accessCount &&
             it->second.lastAccessed < oldestIt->second.lastAccessed)) {
            oldestIt = it;
        }
    }

    m_cache.erase(oldestIt);
}

// CacheManager 实现
CacheManager::CacheManager(size_t maxItems, int ttlSeconds) {
    m_memoryCache = std::make_unique<MemoryCache>(maxItems, ttlSeconds);
}

bool CacheManager::Get(const std::string& key, std::vector<uint8_t>& data) {
    if (!m_memoryCache) return false;
    return m_memoryCache->Get(key, data);
}

void CacheManager::Put(const std::string& key, const std::vector<uint8_t>& data) {
    if (m_memoryCache) {
        m_memoryCache->Put(key, data);
    }
}

void CacheManager::Remove(const std::string& key) {
    if (m_memoryCache) {
        m_memoryCache->Remove(key);
    }
}

void CacheManager::Clear() {
    if (m_memoryCache) {
        m_memoryCache->Clear();
    }
}

std::string CacheManager::GetStatsJson() const {
    if (!m_memoryCache) {
        return "{\"error\":\"Cache not initialized\"}";
    }

    auto stats = m_memoryCache->GetStats();

    std::ostringstream oss;
    oss << "{"
        << "\"cache_type\":\"memory\","
        << "\"hit_count\":" << stats.hitCount << ","
        << "\"miss_count\":" << stats.missCount << ","
        << "\"item_count\":" << stats.itemCount << ","
        << "\"total_size_bytes\":" << stats.totalSize << ","
        << "\"hit_rate\":" << std::fixed << std::setprecision(2) << stats.hitRate
        << "}";

    return oss.str();
}

} // namespace cycle
