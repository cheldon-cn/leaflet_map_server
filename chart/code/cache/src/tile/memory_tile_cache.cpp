#include "ogc/cache/tile/memory_tile_cache.h"
#include <chrono>
#include <algorithm>

namespace ogc {
namespace cache {

MemoryTileCache::MemoryTileCache(size_t maxSize)
    : m_maxSize(maxSize)
    , m_currentSize(0)
    , m_name("MemoryTileCache")
    , m_enabled(true)
    , m_expirationTime(3600) {
}

bool MemoryTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.find(key) != m_cache.end();
}

TileData MemoryTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it == m_cache.end()) {
        return TileData();
    }
    
    UpdateAccessOrder(key);
    
    return it->second;
}

bool MemoryTileCache::PutTile(const TileKey& key, const TileData& tile) {
    if (!m_enabled) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        m_currentSize -= it->second.GetSize();
        it->second = tile;
        m_currentSize += tile.GetSize();
        UpdateAccessOrder(key);
        return true;
    }
    
    m_cache[key] = tile;
    m_currentSize += tile.GetSize();
    m_lruList.push_front(key);
    m_lruMap[key] = m_lruList.begin();
    
    EvictIfNeeded();
    
    return true;
}

bool MemoryTileCache::PutTile(const TileKey& key, const std::vector<uint8_t>& data) {
    TileData tile;
    tile.key = key;
    tile.data = data;
    tile.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    tile.size = static_cast<int64_t>(data.size());
    tile.valid = true;
    
    return PutTile(key, tile);
}

bool MemoryTileCache::RemoveTile(const TileKey& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it == m_cache.end()) {
        return false;
    }
    
    m_currentSize -= it->second.GetSize();
    m_cache.erase(it);
    
    auto lruIt = m_lruMap.find(key);
    if (lruIt != m_lruMap.end()) {
        m_lruList.erase(lruIt->second);
        m_lruMap.erase(lruIt);
    }
    
    return true;
}

void MemoryTileCache::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
    m_lruList.clear();
    m_lruMap.clear();
    m_currentSize = 0;
}

size_t MemoryTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.size();
}

size_t MemoryTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSize;
}

size_t MemoryTileCache::GetMaxSize() const {
    return m_maxSize;
}

void MemoryTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxSize = maxSize;
    EvictIfNeeded();
}

bool MemoryTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSize >= m_maxSize;
}

std::string MemoryTileCache::GetName() const {
    return m_name;
}

void MemoryTileCache::SetName(const std::string& name) {
    m_name = name;
}

bool MemoryTileCache::IsEnabled() const {
    return m_enabled;
}

void MemoryTileCache::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

void MemoryTileCache::SetExpirationTime(int64_t seconds) {
    m_expirationTime = seconds;
}

int64_t MemoryTileCache::GetExpirationTime() const {
    return m_expirationTime;
}

void MemoryTileCache::Cleanup() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_expirationTime <= 0) {
        return;
    }
    
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    auto expirationNs = m_expirationTime * 1000000000LL;
    
    std::vector<TileKey> toRemove;
    for (const auto& pair : m_cache) {
        if (now - pair.second.timestamp > expirationNs) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& key : toRemove) {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            m_currentSize -= it->second.GetSize();
            m_cache.erase(it);
            
            auto lruIt = m_lruMap.find(key);
            if (lruIt != m_lruMap.end()) {
                m_lruList.erase(lruIt->second);
                m_lruMap.erase(lruIt);
            }
        }
    }
}

void MemoryTileCache::EvictIfNeeded() {
    while (m_currentSize > m_maxSize && !m_lruList.empty()) {
        TileKey oldest = m_lruList.back();
        m_lruList.pop_back();
        m_lruMap.erase(oldest);
        
        auto it = m_cache.find(oldest);
        if (it != m_cache.end()) {
            m_currentSize -= it->second.GetSize();
            m_cache.erase(it);
        }
    }
}

void MemoryTileCache::UpdateAccessOrder(const TileKey& key) const {
    auto lruIt = m_lruMap.find(key);
    if (lruIt != m_lruMap.end()) {
        m_lruList.erase(lruIt->second);
        m_lruList.push_front(key);
        m_lruMap[key] = m_lruList.begin();
    }
}

std::shared_ptr<MemoryTileCache> MemoryTileCache::Create(size_t maxSize) {
    return std::make_shared<MemoryTileCache>(maxSize);
}

}
}
