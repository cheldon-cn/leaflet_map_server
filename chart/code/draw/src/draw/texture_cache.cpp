#include "ogc/draw/texture_cache.h"
#include "ogc/draw/gpu_resource_manager.h"
#include <algorithm>

namespace ogc {
namespace draw {

TextureCache::TextureCache()
    : m_maxCacheSize(256 * 1024 * 1024)
    , m_currentSize(0)
    , m_policy(EvictionPolicy::kLRU)
    , m_hitCount(0)
    , m_missCount(0)
    , m_available(false)
{
}

TextureCache::~TextureCache() {
    ReleaseAll();
}

TextureCache& TextureCache::Instance() {
    static TextureCache instance;
    return instance;
}

Texture TextureCache::GetOrCreate(const std::string& key, int width, int height,
                                   TextureFormat format, TextureFilter filter) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        m_hitCount++;
        UpdateAccessTime(key);
        TextureDesc desc;
        desc.width = it->second.width;
        desc.height = it->second.height;
        desc.format = it->second.format;
        return Texture(it->second.handle, it->second.size, desc);
    }
    
    m_missCount++;
    
    size_t texSize = EstimateTextureSize(width, height, 4);
    EvictIfNeeded(texSize);
    
    if (!m_available) {
        return Texture();
    }
    
    auto& mgr = GPUResourceManager::Instance();
    Texture texture = mgr.CreateTexture2D(width, height, format);
    
    (void)filter;
    
    if (texture.IsValid()) {
        TextureEntry entry;
        entry.handle = texture.GetHandle();
        entry.width = width;
        entry.height = height;
        entry.format = format;
        entry.lastAccessTime = 0;
        entry.accessCount = 1;
        entry.size = texSize;
        
        m_cache[key] = entry;
        m_lruList.push_back(key);
        m_currentSize += texSize;
    }
    
    return texture;
}

Texture TextureCache::GetOrCreateFromFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(filePath);
    if (it != m_cache.end()) {
        m_hitCount++;
        UpdateAccessTime(filePath);
        TextureDesc desc;
        desc.width = it->second.width;
        desc.height = it->second.height;
        desc.format = it->second.format;
        return Texture(it->second.handle, it->second.size, desc);
    }
    
    m_missCount++;
    
    (void)filePath;
    
    return Texture();
}

Texture TextureCache::GetOrCreateFromData(const std::string& key, const uint8_t* data,
                                           size_t size, int width, int height, int channels) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        m_hitCount++;
        UpdateAccessTime(key);
        TextureDesc desc;
        desc.width = it->second.width;
        desc.height = it->second.height;
        desc.format = it->second.format;
        return Texture(it->second.handle, it->second.size, desc);
    }
    
    m_missCount++;
    
    size_t texSize = EstimateTextureSize(width, height, channels);
    EvictIfNeeded(texSize);
    
    if (!m_available) {
        return Texture();
    }
    
    auto& mgr = GPUResourceManager::Instance();
    TextureFormat format = (channels == 4) ? TextureFormat::kRGBA8 : TextureFormat::kRGB8;
    Texture texture = mgr.CreateTexture2DFromImage(data, width, height, channels);
    
    (void)size;
    
    if (texture.IsValid()) {
        TextureEntry entry;
        entry.handle = texture.GetHandle();
        entry.width = width;
        entry.height = height;
        entry.format = format;
        entry.lastAccessTime = 0;
        entry.accessCount = 1;
        entry.size = texSize;
        
        m_cache[key] = entry;
        m_lruList.push_back(key);
        m_currentSize += texSize;
    }
    
    return texture;
}

void TextureCache::Release(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        m_currentSize -= it->second.size;
        m_cache.erase(it);
        m_lruList.remove(key);
    }
}

void TextureCache::Release(const Texture& texture) {
    if (!texture.IsValid()) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (it->second.handle == texture.GetHandle()) {
            m_currentSize -= it->second.size;
            m_cache.erase(it);
            m_lruList.remove(it->first);
            break;
        }
    }
}

void TextureCache::ReleaseAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_cache.clear();
    m_lruList.clear();
    m_currentSize = 0;
}

void TextureCache::SetMaxCacheSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxCacheSize = maxSize;
    EvictIfNeeded(0);
}

size_t TextureCache::GetCacheSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSize;
}

size_t TextureCache::GetTextureCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.size();
}

TextureCacheStats TextureCache::GetStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    TextureCacheStats stats;
    stats.currentSize = m_currentSize;
    stats.maxSize = m_maxCacheSize;
    stats.textureCount = m_cache.size();
    stats.hitCount = m_hitCount;
    stats.missCount = m_missCount;
    
    size_t total = m_hitCount + m_missCount;
    if (total > 0) {
        stats.hitRate = static_cast<float>(m_hitCount) / total;
    }
    
    return stats;
}

void TextureCache::SetEvictionPolicy(EvictionPolicy policy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_policy = policy;
}

void TextureCache::EvictLRU(size_t targetSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t goalSize = (targetSize > 0) ? targetSize : m_maxCacheSize;
    
    while (m_currentSize > goalSize && !m_lruList.empty()) {
        std::string key = m_lruList.front();
        m_lruList.pop_front();
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            m_currentSize -= it->second.size;
            m_cache.erase(it);
        }
    }
}

bool TextureCache::Contains(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.find(key) != m_cache.end();
}

void TextureCache::Clear() {
    ReleaseAll();
}

void TextureCache::SetAvailable(bool available) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available = available;
}

bool TextureCache::IsAvailable() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_available;
}

void TextureCache::UpdateAccessTime(const std::string& key) {
    m_lruList.remove(key);
    m_lruList.push_back(key);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        it->second.accessCount++;
    }
}

void TextureCache::EvictIfNeeded(size_t newTextureSize) {
    while (m_currentSize + newTextureSize > m_maxCacheSize && !m_lruList.empty()) {
        std::string key = m_lruList.front();
        m_lruList.pop_front();
        
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            m_currentSize -= it->second.size;
            m_cache.erase(it);
        }
    }
}

size_t TextureCache::EstimateTextureSize(int width, int height, int channels) const {
    return static_cast<size_t>(width) * height * channels;
}

}
}
