#include "ogc/mokrender/data_generator.h"
#include "ogc/mokrender/point_generator.h"
#include "ogc/mokrender/line_generator.h"
#include "ogc/mokrender/polygon_generator.h"
#include "ogc/mokrender/annotation_generator.h"
#include "ogc/mokrender/raster_generator.h"
#include <algorithm>
#include <random>

namespace ogc {
namespace mokrender {

class DataGeneratorImpl : public IDataGenerator {
public:
    DataGeneratorImpl();
    ~DataGeneratorImpl() override;
    
    MokRenderResult Initialize(const DataGeneratorConfig& config) override;
    MokRenderResult Generate() override;
    MokRenderResult SaveToDatabase(const std::string& dbPath) override;
    int GetGeneratedCount() const override;
    
private:
    DataGeneratorConfig m_config;
    std::unique_ptr<PointGenerator> m_pointGenerator;
    std::unique_ptr<LineGenerator> m_lineGenerator;
    std::unique_ptr<PolygonGenerator> m_polygonGenerator;
    std::unique_ptr<AnnotationGenerator> m_annotationGenerator;
    std::unique_ptr<RasterGenerator> m_rasterGenerator;
    int m_totalCount;
    bool m_initialized;
};

DataGeneratorImpl::DataGeneratorImpl()
    : m_totalCount(0)
    , m_initialized(false) {
}

DataGeneratorImpl::~DataGeneratorImpl() {
}

MokRenderResult DataGeneratorImpl::Initialize(const DataGeneratorConfig& config) {
    m_config = config;
    
    m_pointGenerator = std::unique_ptr<PointGenerator>(new PointGenerator());
    m_lineGenerator = std::unique_ptr<LineGenerator>(new LineGenerator());
    m_polygonGenerator = std::unique_ptr<PolygonGenerator>(new PolygonGenerator());
    m_annotationGenerator = std::unique_ptr<AnnotationGenerator>(new AnnotationGenerator());
    m_rasterGenerator = std::unique_ptr<RasterGenerator>(new RasterGenerator());
    
    MokRenderResult result;
    
    result = m_pointGenerator->Initialize(config.minX, config.minY, 
                                          config.maxX, config.maxY, config.srid);
    if (result.IsError()) return result;
    
    result = m_lineGenerator->Initialize(config.minX, config.minY,
                                         config.maxX, config.maxY, config.srid);
    if (result.IsError()) return result;
    
    result = m_polygonGenerator->Initialize(config.minX, config.minY,
                                            config.maxX, config.maxY, config.srid);
    if (result.IsError()) return result;
    
    result = m_annotationGenerator->Initialize(config.minX, config.minY,
                                               config.maxX, config.maxY, config.srid);
    if (result.IsError()) return result;
    
    result = m_rasterGenerator->Initialize(config.minX, config.minY,
                                           config.maxX, config.maxY, config.srid);
    if (result.IsError()) return result;
    
    m_initialized = true;
    return MokRenderResult();
}

MokRenderResult DataGeneratorImpl::Generate() {
    if (!m_initialized) {
        return MokRenderResult(MokRenderErrorCode::INVALID_PARAMETER, 
                              "Generator not initialized");
    }
    
    m_totalCount = 0;
    
    for (int i = 0; i < m_config.pointCount; ++i) {
        m_pointGenerator->GenerateFeature();
        ++m_totalCount;
    }
    
    for (int i = 0; i < m_config.lineCount; ++i) {
        m_lineGenerator->GenerateFeature();
        ++m_totalCount;
    }
    
    for (int i = 0; i < m_config.polygonCount; ++i) {
        m_polygonGenerator->GenerateFeature();
        ++m_totalCount;
    }
    
    for (int i = 0; i < m_config.annotationCount; ++i) {
        m_annotationGenerator->GenerateFeature();
        ++m_totalCount;
    }
    
    for (int i = 0; i < m_config.rasterCount; ++i) {
        m_rasterGenerator->GenerateFeature();
        ++m_totalCount;
    }
    
    return MokRenderResult();
}

MokRenderResult DataGeneratorImpl::SaveToDatabase(const std::string& dbPath) {
    if (!m_initialized) {
        return MokRenderResult(MokRenderErrorCode::INVALID_PARAMETER,
                              "Generator not initialized");
    }
    
    return MokRenderResult();
}

int DataGeneratorImpl::GetGeneratedCount() const {
    return m_totalCount;
}

IDataGenerator* IDataGenerator::Create() {
    return new DataGeneratorImpl();
}

}
}
