#ifndef OGC_DRAW_RENDER_CACHE_H
#define OGC_DRAW_RENDER_CACHE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_style.h"
#include <ogc/geom/geometry.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono>

namespace ogc {
namespace draw {

using CacheHandle = uint64_t;

struct CacheEntry {
    CacheHandle handle;
    std::string key;
    size_t memorySize;
    std::chrono::steady_clock::time_point lastAccess;
    int accessCount;
    bool isValid;
};

class OGC_DRAW_API RenderCache {
public:
    static RenderCache& Instance();
    
    CacheHandle GetOrCreate(const Geometry& geometry, const DrawStyle& style);
    CacheHandle GetOrCreate(const std::string& key);
    
    void Invalidate(CacheHandle handle);
    void InvalidateByKey(const std::string& key);
    void InvalidateAll();
    
    void SetMaxCacheSize(size_t size);
    size_t GetMaxCacheSize() const { return m_maxCacheSize; }
    
    size_t GetCacheSize() const { return m_currentCacheSize; }
    size_t GetEntryCount() const { return m_entries.size(); }
    
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    void SetExpirationTime(int seconds);
    int GetExpirationTime() const { return m_expirationSeconds; }
    
    void Cleanup();
    void CleanupExpired();
    
    bool Contains(CacheHandle handle) const;
    bool ContainsKey(const std::string& key) const;
    
    CacheHandle GenerateHandle(const Geometry& geometry, const DrawStyle& style) const;
    
    void Touch(CacheHandle handle);
    int GetAccessCount(CacheHandle handle) const;
    
    void SetEvictionPolicy(const std::string& policy);
    std::string GetEvictionPolicy() const { return m_evictionPolicy; }

private:
    RenderCache();
    ~RenderCache();
    
    RenderCache(const RenderCache&) = delete;
    RenderCache& operator=(const RenderCache&) = delete;
    
    std::string GenerateKey(const Geometry& geometry, const DrawStyle& style) const;
    void EvictIfNeeded();
    void EvictLRU();
    void EvictOldest();
    void UpdateAccess(CacheEntry& entry);
    
    std::unordered_map<CacheHandle, CacheEntry> m_entries;
    std::unordered_map<std::string, CacheHandle> m_keyToHandle;
    size_t m_maxCacheSize;
    size_t m_currentCacheSize;
    bool m_enabled;
    int m_expirationSeconds;
    std::string m_evictionPolicy;
    CacheHandle m_nextHandle;
    mutable std::mutex m_mutex;
};

}
}

#endif
