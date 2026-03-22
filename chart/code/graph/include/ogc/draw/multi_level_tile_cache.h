#ifndef OGC_DRAW_MULTI_LEVEL_TILE_CACHE_H
#define OGC_DRAW_MULTI_LEVEL_TILE_CACHE_H

#include "ogc/draw/tile_cache.h"
#include <vector>
#include <mutex>

namespace ogc {
namespace draw {

class MultiLevelTileCache : public TileCache {
public:
    MultiLevelTileCache();
    explicit MultiLevelTileCache(const std::vector<TileCachePtr>& caches);
    ~MultiLevelTileCache() override = default;
    
    void AddCache(const TileCachePtr& cache, int priority = -1);
    void RemoveCache(size_t index);
    void RemoveCache(const std::string& name);
    void ClearCaches();
    
    size_t GetCacheCount() const;
    TileCachePtr GetCache(size_t index) const;
    TileCachePtr GetCache(const std::string& name) const;
    std::vector<TileCachePtr> GetCaches() const;
    
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
    
    void Flush() override;
    
    void SetPromoteOnHit(bool promote);
    bool IsPromoteOnHit() const;
    
    void SetWriteThrough(bool writeThrough);
    bool IsWriteThrough() const;
    
    void SetWriteBack(bool writeBack);
    bool IsWriteBack() const;
    
    static std::shared_ptr<MultiLevelTileCache> Create();
    static std::shared_ptr<MultiLevelTileCache> Create(const std::vector<TileCachePtr>& caches);
    
private:
    std::string m_name;
    bool m_enabled;
    bool m_promoteOnHit;
    bool m_writeThrough;
    bool m_writeBack;
    std::vector<TileCachePtr> m_caches;
    mutable std::mutex m_mutex;
    
    void PromoteTile(const TileKey& key, const TileData& tile, size_t fromLevel) const;
    void WriteToHigherLevels(const TileKey& key, const TileData& tile, size_t startLevel) const;
};

typedef std::shared_ptr<MultiLevelTileCache> MultiLevelTileCachePtr;

}
}

#endif
