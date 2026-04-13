#ifndef OGC_CACHE_CACHE_MANAGER_H
#define OGC_CACHE_CACHE_MANAGER_H

#include "ogc/cache/export.h"
#include "ogc/cache/tile/tile_cache.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace cache {

class OGC_CACHE_API CacheManager {
public:
    CacheManager();
    ~CacheManager();

    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;

    TileCachePtr GetCache(const std::string& name) const;
    void AddCache(const std::string& name, TileCachePtr cache);
    void RemoveCache(const std::string& name);
    void ClearAllCaches();

    size_t GetCacheCount() const;
    std::vector<std::string> GetCacheNames() const;

    TileData GetTile(const std::string& cache_name, const TileKey& key) const;
    bool PutTile(const std::string& cache_name, const TileKey& key, const TileData& tile);
    bool HasTile(const std::string& cache_name, const TileKey& key) const;

    size_t GetTotalSize() const;
    size_t GetTotalTileCount() const;

    void FlushAll();

    static CacheManager* Create();
    static void Destroy(CacheManager* mgr);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

typedef std::unique_ptr<CacheManager> CacheManagerPtr;

}
}

#endif
