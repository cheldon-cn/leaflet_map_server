#include "ogc/cache/tile/tile_cache.h"
#include "ogc/cache/tile/memory_tile_cache.h"

namespace ogc {
namespace cache {

TileCachePtr TileCache::CreateMemoryCache(size_t maxSize) {
    return MemoryTileCache::Create(maxSize);
}

TileCachePtr TileCache::CreateDiskCache(const std::string& path, size_t maxSize) {
    (void)path;
    (void)maxSize;
    return nullptr;
}

}
}
