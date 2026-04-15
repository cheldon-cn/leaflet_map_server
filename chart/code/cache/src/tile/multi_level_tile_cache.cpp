#include "ogc/cache/tile/multi_level_tile_cache.h"
#include <algorithm>
#include <mutex>

namespace ogc {
namespace cache {

struct MultiLevelTileCache::Impl {
    std::string name;
    bool enabled;
    bool promoteOnHit;
    bool writeThrough;
    bool writeBack;
    std::vector<TileCachePtr> caches;
    mutable std::mutex mutex;
    
    Impl() : name("MultiLevelTileCache"), enabled(true), promoteOnHit(true),
             writeThrough(true), writeBack(false) {}
};

MultiLevelTileCache::MultiLevelTileCache() : impl_(new Impl()) {}

MultiLevelTileCache::MultiLevelTileCache(const std::vector<TileCachePtr>& caches)
    : impl_(new Impl()) {
    impl_->caches = caches;
}

MultiLevelTileCache::~MultiLevelTileCache() = default;

void MultiLevelTileCache::AddCache(const TileCachePtr& cache, int priority) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!cache) {
        return;
    }
    
    if (priority < 0 || static_cast<size_t>(priority) >= impl_->caches.size()) {
        impl_->caches.push_back(cache);
    } else {
        impl_->caches.insert(impl_->caches.begin() + priority, cache);
    }
}

void MultiLevelTileCache::RemoveCache(size_t index) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index < impl_->caches.size()) {
        impl_->caches.erase(impl_->caches.begin() + index);
    }
}

void MultiLevelTileCache::RemoveCache(const std::string& name) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->caches.erase(
        std::remove_if(impl_->caches.begin(), impl_->caches.end(),
            [&name](const TileCachePtr& cache) {
                return cache && cache->GetName() == name;
            }),
        impl_->caches.end());
}

void MultiLevelTileCache::ClearCaches() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->caches.clear();
}

size_t MultiLevelTileCache::GetCacheCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->caches.size();
}

TileCachePtr MultiLevelTileCache::GetCache(size_t index) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (index < impl_->caches.size()) {
        return impl_->caches[index];
    }
    
    return nullptr;
}

TileCachePtr MultiLevelTileCache::GetCache(const std::string& name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& cache : impl_->caches) {
        if (cache && cache->GetName() == name) {
            return cache;
        }
    }
    
    return nullptr;
}

std::vector<TileCachePtr> MultiLevelTileCache::GetCaches() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->caches;
}

bool MultiLevelTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->enabled) {
        return false;
    }
    
    for (const auto& cache : impl_->caches) {
        if (cache && cache->IsEnabled() && cache->HasTile(key)) {
            return true;
        }
    }
    
    return false;
}

TileData MultiLevelTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    TileData result;
    
    if (!impl_->enabled) {
        return result;
    }
    
    for (size_t i = 0; i < impl_->caches.size(); ++i) {
        const auto& cache = impl_->caches[i];
        if (!cache || !cache->IsEnabled()) {
            continue;
        }
        
        if (cache->HasTile(key)) {
            result = cache->GetTile(key);
            if (result.IsValid()) {
                if (impl_->promoteOnHit && i > 0) {
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->enabled || data.empty() || impl_->caches.empty()) {
        return false;
    }
    
    bool success = false;
    
    if (impl_->writeThrough) {
        for (const auto& cache : impl_->caches) {
            if (cache && cache->IsEnabled()) {
                if (cache->PutTile(key, data)) {
                    success = true;
                }
            }
        }
    } else {
        if (impl_->caches[0] && impl_->caches[0]->IsEnabled()) {
            success = impl_->caches[0]->PutTile(key, data);
        }
    }
    
    return success;
}

bool MultiLevelTileCache::RemoveTile(const TileKey& key) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    bool removed = false;
    
    for (const auto& cache : impl_->caches) {
        if (cache && cache->RemoveTile(key)) {
            removed = true;
        }
    }
    
    return removed;
}

void MultiLevelTileCache::Clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& cache : impl_->caches) {
        if (cache) {
            cache->Clear();
        }
    }
}

size_t MultiLevelTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    size_t count = 0;
    for (const auto& cache : impl_->caches) {
        if (cache) {
            count += cache->GetTileCount();
        }
    }
    
    return count;
}

size_t MultiLevelTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    size_t size = 0;
    for (const auto& cache : impl_->caches) {
        if (cache) {
            size += cache->GetSize();
        }
    }
    
    return size;
}

size_t MultiLevelTileCache::GetMaxSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    size_t maxSize = 0;
    for (const auto& cache : impl_->caches) {
        if (cache) {
            maxSize += cache->GetMaxSize();
        }
    }
    
    return maxSize;
}

void MultiLevelTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (impl_->caches.empty()) {
        return;
    }
    
    size_t perCacheSize = maxSize / impl_->caches.size();
    for (const auto& cache : impl_->caches) {
        if (cache) {
            cache->SetMaxSize(perCacheSize);
        }
    }
}

bool MultiLevelTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& cache : impl_->caches) {
        if (cache && !cache->IsFull()) {
            return false;
        }
    }
    
    return !impl_->caches.empty();
}

std::string MultiLevelTileCache::GetName() const {
    return impl_->name;
}

void MultiLevelTileCache::SetName(const std::string& name) {
    impl_->name = name;
}

bool MultiLevelTileCache::IsEnabled() const {
    return impl_->enabled;
}

void MultiLevelTileCache::SetEnabled(bool enabled) {
    impl_->enabled = enabled;
}

void MultiLevelTileCache::Flush() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& cache : impl_->caches) {
        if (cache) {
            cache->Flush();
        }
    }
}

void MultiLevelTileCache::SetPromoteOnHit(bool promote) {
    impl_->promoteOnHit = promote;
}

bool MultiLevelTileCache::IsPromoteOnHit() const {
    return impl_->promoteOnHit;
}

void MultiLevelTileCache::SetWriteThrough(bool writeThrough) {
    impl_->writeThrough = writeThrough;
}

bool MultiLevelTileCache::IsWriteThrough() const {
    return impl_->writeThrough;
}

void MultiLevelTileCache::SetWriteBack(bool writeBack) {
    impl_->writeBack = writeBack;
}

bool MultiLevelTileCache::IsWriteBack() const {
    return impl_->writeBack;
}

std::shared_ptr<MultiLevelTileCache> MultiLevelTileCache::Create() {
    return std::make_shared<MultiLevelTileCache>();
}

std::shared_ptr<MultiLevelTileCache> MultiLevelTileCache::Create(const std::vector<TileCachePtr>& caches) {
    return std::make_shared<MultiLevelTileCache>(caches);
}

void MultiLevelTileCache::PromoteTile(const TileKey& key, const TileData& tile, size_t fromLevel) const {
    for (size_t i = 0; i < fromLevel; ++i) {
        const auto& cache = impl_->caches[i];
        if (cache && cache->IsEnabled() && !cache->HasTile(key)) {
            cache->PutTile(key, tile);
        }
    }
}

void MultiLevelTileCache::WriteToHigherLevels(const TileKey& key, const TileData& tile, size_t startLevel) const {
    for (size_t i = startLevel; i > 0; --i) {
        const auto& cache = impl_->caches[i - 1];
        if (cache && cache->IsEnabled() && !cache->HasTile(key)) {
            cache->PutTile(key, tile);
        }
    }
}

}
}
