#include "ogc/graph/util/transform_manager.h"
#include <ogc/proj/coordinate_transformer.h>
#include <algorithm>

namespace ogc {
namespace graph {

using ogc::proj::CoordinateTransformer;
using ogc::proj::CoordinateTransformerPtr;

TransformManagerPtr TransformManager::s_instance = nullptr;
std::mutex TransformManager::s_instanceMutex;

TransformManager::TransformManager()
    : m_maxCacheSize(100)
    , m_enableCache(true) {
}

TransformManagerPtr TransformManager::GetInstance() {
    if (!s_instance) {
        std::lock_guard<std::mutex> lock(s_instanceMutex);
        if (!s_instance) {
            s_instance = TransformManagerPtr(new TransformManager());
        }
    }
    return s_instance;
}

std::string TransformManager::MakeKey(const std::string& sourceCRS, const std::string& targetCRS) const {
    return sourceCRS + "->" + targetCRS;
}

CoordinateTransformerPtr TransformManager::GetTransformer(const std::string& sourceCRS, const std::string& targetCRS) {
    std::string key = MakeKey(sourceCRS, targetCRS);
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_enableCache) {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            return it->second;
        }
    }
    
    auto transformer = CreateTransformer(sourceCRS, targetCRS);
    if (transformer && m_enableCache) {
        if (m_cache.size() >= m_maxCacheSize) {
            m_cache.clear();
        }
        m_cache[key] = transformer;
    }
    
    return transformer;
}

void TransformManager::RegisterTransformer(const std::string& sourceCRS, const std::string& targetCRS, CoordinateTransformerPtr transformer) {
    if (!transformer) return;
    
    std::string key = MakeKey(sourceCRS, targetCRS);
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_enableCache) {
        if (m_cache.size() >= m_maxCacheSize && m_cache.find(key) == m_cache.end()) {
            m_cache.clear();
        }
        m_cache[key] = transformer;
    }
}

void TransformManager::UnregisterTransformer(const std::string& sourceCRS, const std::string& targetCRS) {
    std::string key = MakeKey(sourceCRS, targetCRS);
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.erase(key);
}

bool TransformManager::HasTransformer(const std::string& sourceCRS, const std::string& targetCRS) const {
    std::string key = MakeKey(sourceCRS, targetCRS);
    
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.find(key) != m_cache.end();
}

void TransformManager::ClearCache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
}

size_t TransformManager::GetCacheSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cache.size();
}

void TransformManager::SetMaxCacheSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxCacheSize = maxSize;
}

size_t TransformManager::GetMaxCacheSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_maxCacheSize;
}

void TransformManager::SetEnableCache(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enableCache = enable;
    if (!m_enableCache) {
        m_cache.clear();
    }
}

bool TransformManager::IsCacheEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_enableCache;
}

CoordinateTransformerPtr TransformManager::CreateTransformer(const std::string& sourceCRS, const std::string& targetCRS) {
    return CoordinateTransformer::Create(sourceCRS, targetCRS);
}

Coordinate TransformManager::Transform(const std::string& sourceCRS, const std::string& targetCRS, const Coordinate& coord) {
    auto transformer = GetTransformer(sourceCRS, targetCRS);
    if (!transformer) {
        return coord;
    }
    return transformer->Transform(coord);
}

void TransformManager::Transform(const std::string& sourceCRS, const std::string& targetCRS, double& x, double& y) {
    auto transformer = GetTransformer(sourceCRS, targetCRS);
    if (transformer) {
        transformer->Transform(x, y);
    }
}

Envelope TransformManager::Transform(const std::string& sourceCRS, const std::string& targetCRS, const Envelope& env) {
    auto transformer = GetTransformer(sourceCRS, targetCRS);
    if (!transformer) {
        return env;
    }
    return transformer->Transform(env);
}

GeometryPtr TransformManager::Transform(const std::string& sourceCRS, const std::string& targetCRS, const Geometry* geometry) {
    if (!geometry) return nullptr;
    
    auto transformer = GetTransformer(sourceCRS, targetCRS);
    if (!transformer) {
        return nullptr;
    }
    return transformer->Transform(geometry);
}

std::vector<std::string> TransformManager::GetSupportedCRS() const {
    std::vector<std::string> crsList;
    crsList.push_back("EPSG:4326");
    crsList.push_back("EPSG:3857");
    crsList.push_back("WGS84");
    crsList.push_back("WebMercator");
    return crsList;
}

bool TransformManager::IsTransformationSupported(const std::string& sourceCRS, const std::string& targetCRS) const {
    auto supportedCRS = GetSupportedCRS();
    
    auto findCRS = [&supportedCRS](const std::string& crs) {
        for (const auto& supported : supportedCRS) {
            if (supported == crs) {
                return true;
            }
        }
        return false;
    };
    
    return findCRS(sourceCRS) && findCRS(targetCRS);
}

CoordinateTransformerPtr TransformManager::GetWGS84ToWebMercator() {
    return GetInstance()->GetTransformer("EPSG:4326", "EPSG:3857");
}

CoordinateTransformerPtr TransformManager::GetWebMercatorToWGS84() {
    return GetInstance()->GetTransformer("EPSG:3857", "EPSG:4326");
}

}
}
