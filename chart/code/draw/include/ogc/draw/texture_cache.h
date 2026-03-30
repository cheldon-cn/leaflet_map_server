#ifndef OGC_DRAW_TEXTURE_CACHE_H
#define OGC_DRAW_TEXTURE_CACHE_H

#include <ogc/draw/export.h>
#include <ogc/draw/gpu_resource_manager.h>
#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <cstdint>

namespace ogc {
namespace draw {

enum class TextureFilter {
    kNearest = 0,
    kLinear = 2,
    kMipmap = 3
};

enum class EvictionPolicy {
    kLRU = 0,
    kFIFO = 1,
    kSize = 2
};

struct TextureEntry {
    GPUHandle handle;
    int width;
    int height;
    TextureFormat format;
    size_t lastAccessTime;
    int accessCount;
    size_t size;
    
    bool IsValid() const { return handle != INVALID_GPU_HANDLE; }
};

struct TextureCacheStats {
    size_t currentSize = 0;
    size_t maxSize = 0;
    size_t textureCount = 0;
    size_t hitCount = 0;
    size_t missCount = 0;
    float hitRate = 0.0f;
};

class OGC_DRAW_API TextureCache {
public:
    static TextureCache& Instance();
    
    Texture GetOrCreate(const std::string& key, int width, int height, 
                       TextureFormat format = TextureFormat::kRGBA8,
                       TextureFilter filter = TextureFilter::kLinear);
    
    Texture GetOrCreateFromFile(const std::string& filePath);
    Texture GetOrCreateFromData(const std::string& key, const uint8_t* data, 
                                size_t size, int width, int height, int channels);
    
    void Release(const std::string& key);
    void Release(const Texture& texture);
    void ReleaseAll();
    
    void SetMaxCacheSize(size_t maxSize);
    size_t GetCacheSize() const;
    size_t GetTextureCount() const;
    
    TextureCacheStats GetStats() const;
    
    void SetEvictionPolicy(EvictionPolicy policy);
    void EvictLRU(size_t targetSize = 0);
    
    bool Contains(const std::string& key) const;
    void Clear();
    
    void SetAvailable(bool available);
    bool IsAvailable() const;

private:
    TextureCache();
    ~TextureCache();
    
    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
    
    void UpdateAccessTime(const std::string& key);
    void EvictIfNeeded(size_t newTextureSize);
    size_t EstimateTextureSize(int width, int height, int channels) const;
    
    std::unordered_map<std::string, TextureEntry> m_cache;
    std::list<std::string> m_lruList;
    
    size_t m_maxCacheSize;
    size_t m_currentSize;
    EvictionPolicy m_policy;
    
    mutable size_t m_hitCount;
    mutable size_t m_missCount;
    
    bool m_available;
    mutable std::mutex m_mutex;
};

}
}

#endif
