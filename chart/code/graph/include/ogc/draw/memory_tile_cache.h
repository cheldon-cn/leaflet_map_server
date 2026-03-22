#ifndef OGC_DRAW_MEMORY_TILE_CACHE_H
#define OGC_DRAW_MEMORY_TILE_CACHE_H

#include "ogc/draw/tile_cache.h"
#include <unordered_map>
#include <list>
#include <mutex>

namespace ogc {
namespace draw {

class MemoryTileCache : public TileCache {
public:
    explicit MemoryTileCache(size_t maxSize = 100 * 1024 * 1024);
    ~MemoryTileCache() override = default;
    
    bool HasTile(const TileKey& key) const override;
    TileData GetTile(const TileKey& key) const override;
    bool PutTile(const TileKey& key, const TileData& tile) override;
    bool PutTile(const TileKey& key, const std::vector<uint8_t>& data) override;
    bool RemoveTile(const TileKey& key) override;
    void Clear() override;
    
    size_t GetTileCount() const override;
    size_t GetSize() const override;
    size_t GetMaxSize() const override;
    void SetMaxSize(size_t maxSize) override;
    bool IsFull() const override;
    
    std::string GetName() const override;
    void SetName(const std::string& name) override;
    
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    
    void SetExpirationTime(int64_t seconds);
    int64_t GetExpirationTime() const;
    
    void Cleanup();
    
    static std::shared_ptr<MemoryTileCache> Create(size_t maxSize = 100 * 1024 * 1024);

private:
    void EvictIfNeeded();
    void UpdateAccessOrder(const TileKey& key) const;
    
    mutable std::mutex m_mutex;
    std::unordered_map<TileKey, TileData> m_cache;
    mutable std::list<TileKey> m_lruList;
    mutable std::unordered_map<TileKey, std::list<TileKey>::iterator> m_lruMap;
    size_t m_maxSize;
    size_t m_currentSize;
    std::string m_name;
    bool m_enabled;
    int64_t m_expirationTime;
};

}
}

#endif
