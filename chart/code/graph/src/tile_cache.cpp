#include "ogc/draw/tile_cache.h"
#include "ogc/draw/memory_tile_cache.h"

namespace ogc {
namespace draw {

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
