#include "ogc/cache/cache_manager.h"
#include <algorithm>
#include <mutex>

namespace ogc {
namespace cache {

struct CacheManager::Impl {
    struct CacheEntry {
        std::string name;
        TileCachePtr cache;
    };

    std::vector<CacheEntry> caches;
    mutable std::mutex mutex;

    size_t FindCacheIndex(const std::string& name) const {
        for (size_t i = 0; i < caches.size(); ++i) {
            if (caches[i].name == name) {
                return i;
            }
        }
        return caches.size();
    }
};

CacheManager::CacheManager() : impl_(std::make_unique<Impl>()) {
}

CacheManager::~CacheManager() = default;

TileCachePtr CacheManager::GetCache(const std::string& name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(name);
    if (idx < impl_->caches.size()) {
        return impl_->caches[idx].cache;
    }
    return nullptr;
}

void CacheManager::AddCache(const std::string& name, TileCachePtr cache) {
    if (!cache) return;
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(name);
    if (idx < impl_->caches.size()) {
        impl_->caches[idx].cache = std::move(cache);
    } else {
        impl_->caches.push_back({name, std::move(cache)});
    }
}

void CacheManager::RemoveCache(const std::string& name) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(name);
    if (idx < impl_->caches.size()) {
        impl_->caches.erase(impl_->caches.begin() + static_cast<std::ptrdiff_t>(idx));
    }
}

void CacheManager::ClearAllCaches() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& entry : impl_->caches) {
        if (entry.cache) {
            entry.cache->Clear();
        }
    }
    impl_->caches.clear();
}

size_t CacheManager::GetCacheCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->caches.size();
}

std::vector<std::string> CacheManager::GetCacheNames() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::vector<std::string> names;
    names.reserve(impl_->caches.size());
    for (const auto& entry : impl_->caches) {
        names.push_back(entry.name);
    }
    return names;
}

TileData CacheManager::GetTile(const std::string& cache_name, const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(cache_name);
    if (idx < impl_->caches.size() && impl_->caches[idx].cache) {
        return impl_->caches[idx].cache->GetTile(key);
    }
    return TileData();
}

bool CacheManager::PutTile(const std::string& cache_name, const TileKey& key, const TileData& tile) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(cache_name);
    if (idx < impl_->caches.size() && impl_->caches[idx].cache) {
        return impl_->caches[idx].cache->PutTile(key, tile);
    }
    return false;
}

bool CacheManager::HasTile(const std::string& cache_name, const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t idx = impl_->FindCacheIndex(cache_name);
    if (idx < impl_->caches.size() && impl_->caches[idx].cache) {
        return impl_->caches[idx].cache->HasTile(key);
    }
    return false;
}

size_t CacheManager::GetTotalSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t total = 0;
    for (const auto& entry : impl_->caches) {
        if (entry.cache) {
            total += entry.cache->GetSize();
        }
    }
    return total;
}

size_t CacheManager::GetTotalTileCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    size_t total = 0;
    for (const auto& entry : impl_->caches) {
        if (entry.cache) {
            total += entry.cache->GetTileCount();
        }
    }
    return total;
}

void CacheManager::FlushAll() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& entry : impl_->caches) {
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
