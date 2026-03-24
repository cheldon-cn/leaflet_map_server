#ifndef OGC_DRAW_DISK_TILE_CACHE_H
#define OGC_DRAW_DISK_TILE_CACHE_H

#include "ogc/draw/tile_cache.h"
#include <string>
#include <map>
#include <mutex>
#include <cstdint>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DiskTileCache : public TileCache {
public:
    explicit DiskTileCache(const std::string& cachePath, size_t maxSize = 1024 * 1024 * 1024);
    ~DiskTileCache() override;
    
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
    
    bool SetCachePath(const std::string& path);
    std::string GetCachePath() const;
    
    void SetCompressionEnabled(bool enabled);
    bool IsCompressionEnabled() const;
    
    void SetExpirationTime(int64_t seconds);
    int64_t GetExpirationTime() const;
    
    void Cleanup();
    
    static std::shared_ptr<DiskTileCache> Create(const std::string& cachePath, size_t maxSize = 1024 * 1024 * 1024);
    
private:
    std::string m_cachePath;
    std::string m_name;
    size_t m_maxSize;
    size_t m_currentSize;
    size_t m_tileCount;
    bool m_enabled;
    bool m_compressionEnabled;
    int64_t m_expirationTime;
    mutable std::mutex m_mutex;
    
    struct CacheIndex {
        TileKey key;
        std::string filePath;
        size_t size;
        int64_t timestamp;
        int64_t lastAccess;
    };
    
    mutable std::map<TileKey, CacheIndex> m_index;
    mutable bool m_indexLoaded;
    
    std::string GetTileFilePath(const TileKey& key) const;
    std::string GetTileFilePath(const std::string& cachePath, int x, int y, int z) const;
    bool EnsureDirectoryExists(const std::string& path) const;
    bool LoadIndex();
    bool SaveIndex();
    void UpdateIndex(const TileKey& key, const std::string& filePath, size_t size);
    void RemoveFromIndex(const TileKey& key) const;
    bool IsExpired(const CacheIndex& entry) const;
    void RemoveExpired();
    void RemoveLRU();
    std::string GetIndexPath() const;
    bool WriteTileData(const std::string& filePath, const std::vector<uint8_t>& data);
    bool ReadTileData(const std::string& filePath, std::vector<uint8_t>& data) const;
};

typedef std::shared_ptr<DiskTileCache> DiskTileCachePtr;

}
}

#endif
