#include "ogc/cache/cache_manager.h"
#include <algorithm>

namespace ogc {
namespace cache {

CacheManager::CacheManager() {
}

CacheManager::~CacheManager() {
    ClearAllCaches();
}

size_t CacheManager::FindCacheIndex(const std::string& name) const {
    for (size_t i = 0; i < m_caches.size(); ++i) {
        if (m_caches[i].name == name) {
            return i;
        }
    }
    return m_caches.size();
}

TileCachePtr CacheManager::GetCache(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(name);
    if (idx < m_caches.size()) {
        return m_caches[idx].cache;
    }
    return nullptr;
}

void CacheManager::AddCache(const std::string& name, TileCachePtr cache) {
    if (!cache) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(name);
    if (idx < m_caches.size()) {
        m_caches[idx].cache = std::move(cache);
    } else {
        m_caches.push_back({name, std::move(cache)});
    }
}

void CacheManager::RemoveCache(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(name);
    if (idx < m_caches.size()) {
        m_caches.erase(m_caches.begin() + static_cast<std::ptrdiff_t>(idx));
    }
}

void CacheManager::ClearAllCaches() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& entry : m_caches) {
        if (entry.cache) {
            entry.cache->Clear();
        }
    }
    m_caches.clear();
}

size_t CacheManager::GetCacheCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_caches.size();
}

std::vector<std::string> CacheManager::GetCacheNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> names;
    names.reserve(m_caches.size());
    for (const auto& entry : m_caches) {
        names.push_back(entry.name);
    }
    return names;
}

TileData CacheManager::GetTile(const std::string& cache_name, const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(cache_name);
    if (idx < m_caches.size() && m_caches[idx].cache) {
        return m_caches[idx].cache->GetTile(key);
    }
    return TileData();
}

bool CacheManager::PutTile(const std::string& cache_name, const TileKey& key, const TileData& tile) {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(cache_name);
    if (idx < m_caches.size() && m_caches[idx].cache) {
        return m_caches[idx].cache->PutTile(key, tile);
    }
    return false;
}

bool CacheManager::HasTile(const std::string& cache_name, const TileKey& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t idx = FindCacheIndex(cache_name);
    if (idx < m_caches.size() && m_caches[idx].cache) {
        return m_caches[idx].cache->HasTile(key);
    }
    return false;
}

size_t CacheManager::GetTotalSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t total = 0;
    for (const auto& entry : m_caches) {
        if (entry.cache) {
            total += entry.cache->GetSize();
        }
    }
    return total;
}

size_t CacheManager::GetTotalTileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t total = 0;
    for (const auto& entry : m_caches) {
        if (entry.cache) {
            total += entry.cache->GetTileCount();
        }
    }
    return total;
}

void CacheManager::FlushAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& entry : m_caches) {
        if (entry.cache) {
            entry.cache->Flush();
        }
    }
}

CacheManager* CacheManager::Create() {
    return new CacheManager();
}

void CacheManager::Destroy(CacheManager* mgr) {
    delete mgr;
}

}
}
