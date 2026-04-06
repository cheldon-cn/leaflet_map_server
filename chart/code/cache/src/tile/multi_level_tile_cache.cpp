#include "ogc/cache/tile/multi_level_tile_cache.h"
#include <algorithm>

namespace ogc {
namespace cache {

MultiLevelTileCache::MultiLevelTileCache()
    : m_name("MultiLevelTileCache")
    , m_enabled(true)
    , m_promoteOnHit(true)
    , m_writeThrough(true)
    , m_writeBack(false)
{
}

MultiLevelTileCache::MultiLevelTileCache(const std::vector<TileCachePtr>& caches)
    : m_name("MultiLevelTileCache")
    , m_enabled(true)
    , m_promoteOnHit(true)
    , m_writeThrough(true)
    , m_writeBack(false)
    , m_caches(caches)
{
}

void MultiLevelTileCache::AddCache(const TileCachePtr& cache, int priority) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!cache) {
        return;
    }
    
    if (priority < 0 || static_cast<size_t>(priority) >= m_caches.size()) {
        m_caches.push_back(cache);
    } else {
        m_caches.insert(m_caches.begin() + priority, cache);
    }
}

void MultiLevelTileCache::RemoveCache(size_t index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index < m_caches.size()) {
        m_caches.erase(m_caches.begin() + index);
    }
}

void MultiLevelTileCache::RemoveCache(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_caches.erase(
        std::remove_if(m_caches.begin(), m_caches.end(),
            [&name](const TileCachePtr& cache) {
                return cache && cache->GetName() == name;
            }),
        m_caches.end());
}

void MultiLevelTileCache::ClearCaches() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_caches.clear();
}

size_t MultiLevelTileCache::GetCacheCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_caches.size();
}

TileCachePtr MultiLevelTileCache::GetCache(size_t index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index < m_caches.size()) {
        return m_caches[index];
    }
    
    return nullptr;
}

TileCachePtr MultiLevelTileCache::GetCache(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& cache : m_caches) {
        if (cache && cache->GetName() == name) {
            return cache;
        }
    }
    
    return nullptr;
}

std::vector<TileCachePtr> MultiLevelTileCache::GetCaches() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_caches;
}

bool MultiLevelTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled) {
        return false;
    }
    
    for (const auto& cache : m_caches) {
        if (cache && cache->IsEnabled() && cache->HasTile(key)) {
            return true;
        }
    }
    
    return false;
}

TileData MultiLevelTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    TileData result;
    
    if (!m_enabled) {
        return result;
    }
    
    for (size_t i = 0; i < m_caches.size(); ++i) {
        const auto& cache = m_caches[i];
        if (!cache || !cache->IsEnabled()) {
            continue;
        }
        
        if (cache->HasTile(key)) {
            result = cache->GetTile(key);
            if (result.IsValid()) {
                if (m_promoteOnHit && i > 0) {
                    PromoteTile(key, result, i);
                }
                return result;
            }
        }
    }
    
    return result;
}

bool MultiLevelTileCache::PutTile(const TileKey& key, const TileData& tile) {
    return PutTile(key, tile.data);
}

bool MultiLevelTileCache::PutTile(const TileKey& key, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled || data.empty() || m_caches.empty()) {
        return false;
    }
    
    bool success = false;
    
    if (m_writeThrough) {
        for (const auto& cache : m_caches) {
            if (cache && cache->IsEnabled()) {
                if (cache->PutTile(key, data)) {
                    success = true;
                }
            }
        }
    } else {
        if (m_caches[0] && m_caches[0]->IsEnabled()) {
            success = m_caches[0]->PutTile(key, data);
        }
    }
    
    return success;
}

bool MultiLevelTileCache::RemoveTile(const TileKey& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    bool removed = false;
    
    for (const auto& cache : m_caches) {
        if (cache && cache->RemoveTile(key)) {
            removed = true;
        }
    }
    
    return removed;
}

void MultiLevelTileCache::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& cache : m_caches) {
        if (cache) {
            cache->Clear();
        }
    }
}

size_t MultiLevelTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t count = 0;
    for (const auto& cache : m_caches) {
        if (cache) {
            count += cache->GetTileCount();
        }
    }
    
    return count;
}

size_t MultiLevelTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t size = 0;
    for (const auto& cache : m_caches) {
        if (cache) {
            size += cache->GetSize();
        }
    }
    
    return size;
}

size_t MultiLevelTileCache::GetMaxSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t maxSize = 0;
    for (const auto& cache : m_caches) {
        if (cache) {
            maxSize += cache->GetMaxSize();
        }
    }
    
    return maxSize;
}

void MultiLevelTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_caches.empty()) {
        return;
    }
    
    size_t perCacheSize = maxSize / m_caches.size();
    for (const auto& cache : m_caches) {
        if (cache) {
            cache->SetMaxSize(perCacheSize);
        }
    }
}

bool MultiLevelTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& cache : m_caches) {
        if (cache && !cache->IsFull()) {
            return false;
        }
    }
    
    return !m_caches.empty();
}

std::string MultiLevelTileCache::GetName() const {
    return m_name;
}

void MultiLevelTileCache::SetName(const std::string& name) {
    m_name = name;
}

bool MultiLevelTileCache::IsEnabled() const {
    return m_enabled;
}

void MultiLevelTileCache::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

void MultiLevelTileCache::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& cache : m_caches) {
        if (cache) {
            cache->Flush();
        }
    }
}

void MultiLevelTileCache::SetPromoteOnHit(bool promote) {
    m_promoteOnHit = promote;
}

bool MultiLevelTileCache::IsPromoteOnHit() const {
    return m_promoteOnHit;
}

void MultiLevelTileCache::SetWriteThrough(bool writeThrough) {
    m_writeThrough = writeThrough;
}

bool MultiLevelTileCache::IsWriteThrough() const {
    return m_writeThrough;
}

void MultiLevelTileCache::SetWriteBack(bool writeBack) {
    m_writeBack = writeBack;
}

bool MultiLevelTileCache::IsWriteBack() const {
    return m_writeBack;
}

std::shared_ptr<MultiLevelTileCache> MultiLevelTileCache::Create() {
    return std::make_shared<MultiLevelTileCache>();
}

std::shared_ptr<MultiLevelTileCache> MultiLevelTileCache::Create(const std::vector<TileCachePtr>& caches) {
    return std::make_shared<MultiLevelTileCache>(caches);
}

void MultiLevelTileCache::PromoteTile(const TileKey& key, const TileData& tile, size_t fromLevel) const {
    for (size_t i = 0; i < fromLevel; ++i) {
        const auto& cache = m_caches[i];
        if (cache && cache->IsEnabled() && !cache->HasTile(key)) {
            cache->PutTile(key, tile);
        }
    }
}

void MultiLevelTileCache::WriteToHigherLevels(const TileKey& key, const TileData& tile, size_t startLevel) const {
    for (size_t i = startLevel; i > 0; --i) {
        const auto& cache = m_caches[i - 1];
        if (cache && cache->IsEnabled() && !cache->HasTile(key)) {
            cache->PutTile(key, tile);
        }
    }
}

}
}
