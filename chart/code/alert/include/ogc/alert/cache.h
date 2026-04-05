#ifndef OGC_ALERT_CACHE_H
#define OGC_ALERT_CACHE_H

#include "export.h"
#include "types.h"
#include <string>
#include <map>
#include <list>
#include <mutex>
#include <memory>
#include <chrono>
#include <functional>

namespace ogc {
namespace alert {

template<typename T>
struct CacheEntry {
    T value;
    std::chrono::steady_clock::time_point createdAt;
    std::chrono::steady_clock::time_point lastAccessedAt;
    int ttlSeconds;
    int accessCount;
    std::string key;
};

template<typename T>
class OGC_ALERT_API Cache {
public:
    using EvictionCallback = std::function<void(const std::string&, const T&)>;
    
    explicit Cache(size_t maxSize = 1000, int defaultTtlSeconds = 300);
    ~Cache();
    
    bool Get(const std::string& key, T& value);
    void Set(const std::string& key, const T& value, int ttlSeconds = 0);
    bool Remove(const std::string& key);
    void Clear();
    bool Contains(const std::string& key) const;
    size_t GetSize() const;
    size_t GetMaxSize() const;
    void SetMaxSize(size_t maxSize);
    void SetDefaultTtl(int ttlSeconds);
    int GetDefaultTtl() const;
    void SetEvictionCallback(EvictionCallback callback);
    void CleanupExpired();
    size_t GetHitCount() const;
    size_t GetMissCount() const;
    double GetHitRate() const;
    void ResetStats();
    
private:
    using EntryPtr = std::shared_ptr<CacheEntry<T>>;
    using EntryList = std::list<EntryPtr>;
    using EntryMap = std::map<std::string, typename EntryList::iterator>;
    
    size_t m_maxSize;
    int m_defaultTtlSeconds;
    mutable std::mutex m_mutex;
    EntryList m_lruList;
    EntryMap m_entryMap;
    EvictionCallback m_evictionCallback;
    
    mutable size_t m_hitCount;
    mutable size_t m_missCount;
    
    void EvictLru();
    bool IsExpired(const EntryPtr& entry) const;
    void UpdateAccess(EntryPtr& entry);
};

template<typename T>
Cache<T>::Cache(size_t maxSize, int defaultTtlSeconds)
    : m_maxSize(maxSize)
    , m_defaultTtlSeconds(defaultTtlSeconds)
    , m_hitCount(0)
    , m_missCount(0) {
}

template<typename T>
Cache<T>::~Cache() {
    Clear();
}

template<typename T>
bool Cache<T>::Get(const std::string& key, T& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_entryMap.find(key);
    if (it == m_entryMap.end()) {
        m_missCount++;
        return false;
    }
    
    EntryPtr& entry = *(it->second);
    
    if (IsExpired(entry)) {
        m_lruList.erase(it->second);
        m_entryMap.erase(it);
        m_missCount++;
        return false;
    }
    
    UpdateAccess(entry);
    value = entry->value;
    m_hitCount++;
    return true;
}

template<typename T>
void Cache<T>::Set(const std::string& key, const T& value, int ttlSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_entryMap.find(key);
    if (it != m_entryMap.end()) {
        EntryPtr& entry = *(it->second);
        entry->value = value;
        entry->ttlSeconds = ttlSeconds > 0 ? ttlSeconds : m_defaultTtlSeconds;
        entry->createdAt = std::chrono::steady_clock::now();
        UpdateAccess(entry);
        return;
    }
    
    while (m_lruList.size() >= m_maxSize) {
        EvictLru();
    }
    
    auto entry = std::make_shared<CacheEntry<T>>();
    entry->key = key;
    entry->value = value;
    entry->ttlSeconds = ttlSeconds > 0 ? ttlSeconds : m_defaultTtlSeconds;
    entry->createdAt = std::chrono::steady_clock::now();
    entry->lastAccessedAt = entry->createdAt;
    entry->accessCount = 1;
    
    m_lruList.push_front(entry);
    m_entryMap[key] = m_lruList.begin();
}

template<typename T>
bool Cache<T>::Remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_entryMap.find(key);
    if (it == m_entryMap.end()) {
        return false;
    }
    
    if (m_evictionCallback) {
        EntryPtr& entry = *(it->second);
        m_evictionCallback(key, entry->value);
    }
    
    m_lruList.erase(it->second);
    m_entryMap.erase(it);
    return true;
}

template<typename T>
void Cache<T>::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_evictionCallback) {
        for (auto& entry : m_lruList) {
            m_evictionCallback(entry->key, entry->value);
        }
    }
    
    m_lruList.clear();
    m_entryMap.clear();
}

template<typename T>
bool Cache<T>::Contains(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_entryMap.find(key);
    if (it == m_entryMap.end()) {
        return false;
    }
    
    return !IsExpired(*it->second);
}

template<typename T>
size_t Cache<T>::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lruList.size();
}

template<typename T>
size_t Cache<T>::GetMaxSize() const {
    return m_maxSize;
}

template<typename T>
void Cache<T>::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxSize = maxSize;
    
    while (m_lruList.size() > m_maxSize) {
        EvictLru();
    }
}

template<typename T>
void Cache<T>::SetDefaultTtl(int ttlSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultTtlSeconds = ttlSeconds;
}

template<typename T>
int Cache<T>::GetDefaultTtl() const {
    return m_defaultTtlSeconds;
}

template<typename T>
void Cache<T>::SetEvictionCallback(EvictionCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_evictionCallback = callback;
}

template<typename T>
void Cache<T>::CleanupExpired() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_lruList.begin();
    while (it != m_lruList.end()) {
        if (IsExpired(*it)) {
            if (m_evictionCallback) {
                m_evictionCallback((*it)->key, (*it)->value);
            }
            m_entryMap.erase((*it)->key);
            it = m_lruList.erase(it);
        } else {
            ++it;
        }
    }
}

template<typename T>
size_t Cache<T>::GetHitCount() const {
    return m_hitCount;
}

template<typename T>
size_t Cache<T>::GetMissCount() const {
    return m_missCount;
}

template<typename T>
double Cache<T>::GetHitRate() const {
    size_t total = m_hitCount + m_missCount;
    if (total == 0) return 0.0;
    return static_cast<double>(m_hitCount) / static_cast<double>(total);
}

template<typename T>
void Cache<T>::ResetStats() {
    m_hitCount = 0;
    m_missCount = 0;
}

template<typename T>
void Cache<T>::EvictLru() {
    if (m_lruList.empty()) return;
    
    EntryPtr& entry = m_lruList.back();
    if (m_evictionCallback) {
        m_evictionCallback(entry->key, entry->value);
    }
    
    m_entryMap.erase(entry->key);
    m_lruList.pop_back();
}

template<typename T>
bool Cache<T>::IsExpired(const EntryPtr& entry) const {
    if (entry->ttlSeconds <= 0) return false;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - entry->createdAt).count();
    
    return elapsed >= entry->ttlSeconds;
}

template<typename T>
void Cache<T>::UpdateAccess(EntryPtr& entry) {
    entry->lastAccessedAt = std::chrono::steady_clock::now();
    entry->accessCount++;
    
    m_lruList.splice(m_lruList.begin(), m_lruList, m_entryMap[entry->key]);
}

using StringCache = Cache<std::string>;
using AisDataCache = Cache<AisData>;
using DepthDataCache = Cache<DepthData>;
using TideDataCache = Cache<TideData>;
using WeatherDataCache = Cache<WeatherData>;

}
}

#endif
