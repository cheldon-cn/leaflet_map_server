#ifndef OGC_GRAPH_TRANSFORM_MANAGER_H
#define OGC_GRAPH_TRANSFORM_MANAGER_H

#include "ogc/graph/export.h"
#include <ogc/proj/coordinate_transformer.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace ogc {
namespace graph {

class TransformManager;
typedef std::shared_ptr<TransformManager> TransformManagerPtr;

class OGC_GRAPH_API TransformManager {
public:
    ~TransformManager();
    
    static TransformManagerPtr GetInstance();
    
    ogc::proj::CoordinateTransformerPtr GetTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    
    void RegisterTransformer(const std::string& sourceCRS, const std::string& targetCRS, ogc::proj::CoordinateTransformerPtr transformer);
    
    void UnregisterTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    
    bool HasTransformer(const std::string& sourceCRS, const std::string& targetCRS) const;
    
    void ClearCache();
    
    size_t GetCacheSize() const;
    
    void SetMaxCacheSize(size_t maxSize);
    size_t GetMaxCacheSize() const;
    
    void SetEnableCache(bool enable);
    bool IsCacheEnabled() const;
    
    ogc::proj::CoordinateTransformerPtr CreateTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    
    Coordinate Transform(const std::string& sourceCRS, const std::string& targetCRS, const Coordinate& coord);
    
    void Transform(const std::string& sourceCRS, const std::string& targetCRS, double& x, double& y);
    
    Envelope Transform(const std::string& sourceCRS, const std::string& targetCRS, const Envelope& env);
    
    GeometryPtr Transform(const std::string& sourceCRS, const std::string& targetCRS, const Geometry* geometry);
    
    std::vector<std::string> GetSupportedCRS() const;
    
    bool IsTransformationSupported(const std::string& sourceCRS, const std::string& targetCRS) const;
    
    static ogc::proj::CoordinateTransformerPtr GetWGS84ToWebMercator();
    static ogc::proj::CoordinateTransformerPtr GetWebMercatorToWGS84();
    
private:
    TransformManager();
    
    TransformManager(const TransformManager&) = delete;
    TransformManager& operator=(const TransformManager&) = delete;
    
    std::string MakeKey(const std::string& sourceCRS, const std::string& targetCRS) const;
    
    static TransformManagerPtr s_instance;
    static std::mutex s_instanceMutex;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  
}  

#endif
