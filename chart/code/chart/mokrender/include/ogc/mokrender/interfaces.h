#pragma once

#include "export.h"
#include "common.h"
#include <memory>
#include <vector>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API IDataGenerator {
public:
    virtual ~IDataGenerator() = default;
    
    virtual MokRenderResult Initialize(const DataGeneratorConfig& config) = 0;
    virtual MokRenderResult Generate() = 0;
    virtual MokRenderResult SaveToDatabase(const std::string& dbPath) = 0;
    virtual int GetGeneratedCount() const = 0;
    
    static IDataGenerator* Create();
};

class OGC_MOKRENDER_API IFeatureGenerator {
public:
    virtual ~IFeatureGenerator() = default;
    
    virtual MokRenderResult Initialize(double minX, double minY, 
                                       double maxX, double maxY,
                                       int srid) = 0;
    virtual void* GenerateFeature() = 0;
    virtual std::string GetLayerName() const = 0;
    virtual std::string GetGeometryType() const = 0;
};

class OGC_MOKRENDER_API ISpatialQueryEngine {
public:
    virtual ~ISpatialQueryEngine() = default;
    
    virtual MokRenderResult Initialize(const std::string& dbPath) = 0;
    virtual std::vector<void*> QueryByExtent(double minX, double minY,
                                             double maxX, double maxY,
                                             const std::string& layerName) = 0;
    virtual void Close() = 0;
    
    static ISpatialQueryEngine* Create();
};

class OGC_MOKRENDER_API IRenderEngine {
public:
    virtual ~IRenderEngine() = default;
    
    virtual MokRenderResult Initialize(const RenderConfig& config) = 0;
    virtual MokRenderResult RenderToRaster(const std::string& outputPath) = 0;
    virtual MokRenderResult RenderToPdf(const std::string& outputPath) = 0;
    virtual MokRenderResult RenderToDisplay() = 0;
    
    static IRenderEngine* Create();
};

}
}
