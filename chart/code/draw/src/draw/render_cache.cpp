#include "ogc/draw/render_cache.h"
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {
namespace draw {

RenderCache& RenderCache::Instance() {
    static RenderCache instance;
    return instance;
}

RenderCache::RenderCache()
    : m_maxCacheSize(100 * 1024 * 1024)
    , m_currentCacheSize(0)
    , m_enabled(true)
    , m_expirationSeconds(300)
    , m_evictionPolicy("LRU")
    , m_nextHandle(1) {
}

RenderCache::~RenderCache() {
    InvalidateAll();
}

CacheHandle RenderCache::GetOrCreate(const Geometry& geometry, const DrawStyle& style) {
    if (!m_enabled) {
        return 0;
    }
    
    std::string key = GenerateKey(geometry, style);
    return GetOrCreate(key);
}

CacheHandle RenderCache::GetOrCreate(const std::string& key) {
    if (!m_enabled) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_keyToHandle.find(key);
    if (it != m_keyToHandle.end()) {
        auto entryIt = m_entries.find(it->second);
        if (entryIt != m_entries.end() && entryIt->second.isValid) {
            UpdateAccess(entryIt->second);
            return it->second;
        }
    }
    
    CacheEntry entry;
    entry.handle = m_nextHandle++;
    entry.key = key;
    entry.memorySize = 1024;
    entry.lastAccess = std::chrono::steady_clock::now();
    entry.accessCount = 1;
    entry.isValid = true;
    
    m_currentCacheSize += entry.memorySize;
    m_entries[entry.handle] = entry;
    m_keyToHandle[key] = entry.handle;
    
    EvictIfNeeded();
    
    return entry.handle;
}

void RenderCache::Invalidate(CacheHandle handle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_entries.find(handle);
    if (it != m_entries.end()) {
        m_currentCacheSize -= it->second.memorySize;
        m_keyToHandle.erase(it->second.key);
        m_entries.erase(it);
    }
}

void RenderCache::InvalidateByKey(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_keyToHandle.find(key);
    if (it != m_keyToHandle.end()) {
        auto entryIt = m_entries.find(it->second);
        if (entryIt != m_entries.end()) {
            m_currentCacheSize -= entryIt->second.memorySize;
            m_entries.erase(entryIt);
        }
        m_keyToHandle.erase(it);
    }
}

void RenderCache::InvalidateAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.clear();
    m_keyToHandle.clear();
    m_currentCacheSize = 0;
}

void RenderCache::SetMaxCacheSize(size_t size) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxCacheSize = size;
    EvictIfNeeded();
}

void RenderCache::SetExpirationTime(int seconds) {
    m_expirationSeconds = seconds;
}

void RenderCache::Cleanup() {
    CleanupExpired();
}

void RenderCache::CleanupExpired() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto now = std::chrono::steady_clock::now();
    auto expirationDuration = std::chrono::seconds(m_expirationSeconds);
    
    for (auto it = m_entries.begin(); it != m_entries.end(); ) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastAccess);
        if (age > expirationDuration) {
            m_currentCacheSize -= it->second.memorySize;
            m_keyToHandle.erase(it->second.key);
            it = m_entries.erase(it);
        } else {
            ++it;
        }
    }
}

bool RenderCache::Contains(CacheHandle handle) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries.find(handle) != m_entries.end();
}

bool RenderCache::ContainsKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_keyToHandle.find(key) != m_keyToHandle.end();
}

CacheHandle RenderCache::GenerateHandle(const Geometry& geometry, const DrawStyle& style) const {
    std::string key = GenerateKey(geometry, style);
    auto it = m_keyToHandle.find(key);
    return it != m_keyToHandle.end() ? it->second : 0;
}

void RenderCache::Touch(CacheHandle handle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_entries.find(handle);
    if (it != m_entries.end()) {
        UpdateAccess(it->second);
    }
}

int RenderCache::GetAccessCount(CacheHandle handle) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_entries.find(handle);
    return it != m_entries.end() ? it->second.accessCount : 0;
}

void RenderCache::SetEvictionPolicy(const std::string& policy) {
    m_evictionPolicy = policy;
}

std::string RenderCache::GenerateKey(const Geometry& geometry, const DrawStyle& style) const {
    std::ostringstream oss;
    
    oss << geometry.GetGeometryTypeString() << "_";
    
    auto envelope = geometry.GetEnvelope();
    oss << std::fixed << std::setprecision(2) 
        << envelope.GetMinX() << "," << envelope.GetMinY() << ","
        << envelope.GetMaxX() << "," << envelope.GetMaxY() << "_";
    
    oss << static_cast<int>(style.pen.color.GetRed()) << ","
        << static_cast<int>(style.pen.color.GetGreen()) << ","
        << static_cast<int>(style.pen.color.GetBlue()) << ","
        << static_cast<int>(style.pen.color.GetAlpha()) << "_";
    oss << std::fixed << std::setprecision(2) << style.pen.width << "_";
    
    oss << static_cast<int>(style.brush.color.GetRed()) << ","
        << static_cast<int>(style.brush.color.GetGreen()) << ","
        << static_cast<int>(style.brush.color.GetBlue()) << ","
        << static_cast<int>(style.brush.color.GetAlpha());
    
    return oss.str();
}

void RenderCache::EvictIfNeeded() {
    while (m_currentCacheSize > m_maxCacheSize && !m_entries.empty()) {
        if (m_evictionPolicy == "LRU") {
            EvictLRU();
        } else {
            EvictOldest();
        }
    }
}

void RenderCache::EvictLRU() {
    if (m_entries.empty()) return;
    
    auto oldest = m_entries.begin();
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
        if (it->second.lastAccess < oldest->second.lastAccess) {
            oldest = it;
        }
    }
    
    m_currentCacheSize -= oldest->second.memorySize;
    m_keyToHandle.erase(oldest->second.key);
    m_entries.erase(oldest);
}

void RenderCache::EvictOldest() {
    if (m_entries.empty()) return;
    
    auto oldest = m_entries.begin();
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
        if (it->second.handle < oldest->second.handle) {
            oldest = it;
        }
    }
    
    m_currentCacheSize -= oldest->second.memorySize;
    m_keyToHandle.erase(oldest->second.key);
    m_entries.erase(oldest);
}

void RenderCache::UpdateAccess(CacheEntry& entry) {
    entry.lastAccess = std::chrono::steady_clock::now();
    entry.accessCount++;
}

}
}
