#ifndef OGC_DRAW_TILE_CACHE_H
#define OGC_DRAW_TILE_CACHE_H

#include "ogc/draw/export.h"
#include "ogc/draw/tile_key.h"
#include <memory>
#include <vector>
#include <cstdint>

namespace ogc {
namespace draw {

struct TileData {
    TileKey key;
    std::vector<uint8_t> data;
    int64_t timestamp;
    int64_t size;
    bool valid;
    
    TileData() : timestamp(0), size(0), valid(false) {}
    
    bool IsValid() const { return valid && !data.empty(); }
    size_t GetSize() const { return data.size(); }
};

class TileCache;
typedef std::shared_ptr<TileCache> TileCachePtr;

class OGC_GRAPH_API TileCache {
public:
    virtual ~TileCache() = default;
    
    virtual bool HasTile(const TileKey& key) const = 0;
    
    virtual TileData GetTile(const TileKey& key) const = 0;
    
    virtual bool PutTile(const TileKey& key, const TileData& tile) = 0;
    
    virtual bool PutTile(const TileKey& key, const std::vector<uint8_t>& data) = 0;
    
    virtual bool RemoveTile(const TileKey& key) = 0;
    
    virtual void Clear() = 0;
    
    virtual size_t GetTileCount() const = 0;
    
    virtual size_t GetSize() const = 0;
    
    virtual size_t GetMaxSize() const = 0;
    
    virtual void SetMaxSize(size_t maxSize) = 0;
    
    virtual bool IsFull() const = 0;
    
    virtual std::string GetName() const = 0;
    
    virtual void SetName(const std::string& name) = 0;
    
    virtual bool IsEnabled() const = 0;
    
    virtual void SetEnabled(bool enabled) = 0;
    
    virtual void Flush() {}
    
    virtual bool Contains(const TileKey& key) const {
        return HasTile(key);
    }
    
    virtual bool Get(const TileKey& key, TileData& tile) const {
        if (!HasTile(key)) {
            return false;
        }
        tile = GetTile(key);
        return tile.IsValid();
    }
    
    virtual bool Put(const TileKey& key, const std::vector<uint8_t>& data) {
        return PutTile(key, data);
    }
    
    static TileCachePtr CreateMemoryCache(size_t maxSize = 100 * 1024 * 1024);
    static TileCachePtr CreateDiskCache(const std::string& path, size_t maxSize = 1024 * 1024 * 1024);
};

}
}

#endif
