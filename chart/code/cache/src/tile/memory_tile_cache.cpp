#include "ogc/cache/tile/memory_tile_cache.h"
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <mutex>

namespace ogc {
namespace cache {

struct MemoryTileCache::Impl {
    mutable std::mutex mutex;
    std::unordered_map<TileKey, TileData> cache;
    mutable std::list<TileKey> lruList;
    mutable std::unordered_map<TileKey, std::list<TileKey>::iterator> lruMap;
    size_t maxSize;
    size_t currentSize;
    std::string name;
    bool enabled;
    int64_t expirationTime;
    
    Impl() : maxSize(0), currentSize(0), enabled(true), expirationTime(3600) {}
};

MemoryTileCache::MemoryTileCache(size_t maxSize)
    : impl_(new Impl())
{
    impl_->maxSize = maxSize;
    impl_->name = "MemoryTileCache";
}

MemoryTileCache::~MemoryTileCache() = default;

bool MemoryTileCache::HasTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->cache.find(key) != impl_->cache.end();
}

TileData MemoryTileCache::GetTile(const TileKey& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->cache.find(key);
    if (it == impl_->cache.end()) {
        return TileData();
    }
    
    UpdateAccessOrder(key);
    
    return it->second;
}

bool MemoryTileCache::PutTile(const TileKey& key, const TileData& tile) {
    if (!impl_->enabled) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->cache.find(key);
    if (it != impl_->cache.end()) {
        impl_->currentSize -= it->second.GetSize();
        it->second = tile;
        impl_->currentSize += tile.GetSize();
        UpdateAccessOrder(key);
        return true;
    }
    
    impl_->cache[key] = tile;
    impl_->currentSize += tile.GetSize();
    impl_->lruList.push_front(key);
    impl_->lruMap[key] = impl_->lruList.begin();
    
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->cache.find(key);
    if (it == impl_->cache.end()) {
        return false;
    }
    
    impl_->currentSize -= it->second.GetSize();
    impl_->cache.erase(it);
    
    auto lruIt = impl_->lruMap.find(key);
    if (lruIt != impl_->lruMap.end()) {
        impl_->lruList.erase(lruIt->second);
        impl_->lruMap.erase(lruIt);
    }
    
    return true;
}

void MemoryTileCache::Clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->cache.clear();
    impl_->lruList.clear();
    impl_->lruMap.clear();
    impl_->currentSize = 0;
}

size_t MemoryTileCache::GetTileCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->cache.size();
}

size_t MemoryTileCache::GetSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentSize;
}

size_t MemoryTileCache::GetMaxSize() const {
    return impl_->maxSize;
}

void MemoryTileCache::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->maxSize = maxSize;
    EvictIfNeeded();
}

bool MemoryTileCache::IsFull() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->currentSize >= impl_->maxSize;
}

std::string MemoryTileCache::GetName() const {
    return impl_->name;
}

void MemoryTileCache::SetName(const std::string& name) {
    impl_->name = name;
}

bool MemoryTileCache::IsEnabled() const {
    return impl_->enabled;
}

void MemoryTileCache::SetEnabled(bool enabled) {
    impl_->enabled = enabled;
}

void MemoryTileCache::SetExpirationTime(int64_t seconds) {
    impl_->expirationTime = seconds;
}

int64_t MemoryTileCache::GetExpirationTime() const {
    return impl_->expirationTime;
}

void MemoryTileCache::Cleanup() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (impl_->expirationTime <= 0) {
        return;
    }
    
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    auto expirationNs = impl_->expirationTime * 1000000000LL;
    
    std::vector<TileKey> toRemove;
    for (const auto& pair : impl_->cache) {
        if (now - pair.second.timestamp > expirationNs) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& key : toRemove) {
        auto it = impl_->cache.find(key);
        if (it != impl_->cache.end()) {
            impl_->currentSize -= it->second.GetSize();
            impl_->cache.erase(it);
            
            auto lruIt = impl_->lruMap.find(key);
            if (lruIt != impl_->lruMap.end()) {
                impl_->lruList.erase(lruIt->second);
                impl_->lruMap.erase(lruIt);
            }
        }
    }
}

void MemoryTileCache::EvictIfNeeded() {
    while (impl_->currentSize > impl_->maxSize && !impl_->lruList.empty()) {
        TileKey oldest = impl_->lruList.back();
        impl_->lruList.pop_back();
        impl_->lruMap.erase(oldest);
        
        auto it = impl_->cache.find(oldest);
        if (it != impl_->cache.end()) {
            impl_->currentSize -= it->second.GetSize();
            impl_->cache.erase(it);
        }
    }
}

void MemoryTileCache::UpdateAccessOrder(const TileKey& key) const {
    auto lruIt = impl_->lruMap.find(key);
    if (lruIt != impl_->lruMap.end()) {
        impl_->lruList.erase(lruIt->second);
        impl_->lruList.push_front(key);
        impl_->lruMap[key] = impl_->lruList.begin();
    }
}

std::shared_ptr<MemoryTileCache> MemoryTileCache::Create(size_t maxSize) {
    return std::make_shared<MemoryTileCache>(maxSize);
}

}
}
