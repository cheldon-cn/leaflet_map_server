#include "parser/s57_parser.h"
#include "parser/s57_geometry_converter.h"
#include "parser/s57_attribute_parser.h"
#include "parser/s57_feature_type_mapper.h"
#include "parser/error_handler.h"

#include <ogrsf_frmts.h>
#include <chrono>

namespace chart {
namespace parser {

S57Parser::S57Parser()
    : m_geometryConverter(new S57GeometryConverter())
    , m_attributeParser(new S57AttributeParser())
    , m_featureTypeMapper(new S57FeatureTypeMapper()) {
}

S57Parser::~S57Parser() {
}

ParseResult S57Parser::ParseChart(const std::string& filePath, const ParseConfig& config) {
    ParseResult result;
    result.filePath = filePath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG_INFO("Parsing S57 file: %s", filePath.c_str());
    
    if (!GDALInitializer::Instance().Initialize()) {
        result.SetError(ErrorCode::ErrGDALInitFailed, "GDAL initialization failed");
        return result;
    }
    
    GDALDataset* dataset = nullptr;
    if (!OpenDataset(filePath, reinterpret_cast<void**>(&dataset))) {
        result.SetError(ErrorCode::ErrGDALDatasetOpenFailed, "Failed to open dataset: " + filePath);
        return result;
    }
    
    int layerCount = dataset->GetLayerCount();
    LOG_INFO("Dataset has %d layers", layerCount);
    
    for (int i = 0; i < layerCount; ++i) {
        OGRLayer* layer = dataset->GetLayer(i);
        if (!layer) {
            LOG_WARN("Failed to get layer %d", i);
            continue;
        }
        
        std::string layerName = layer->GetName();
        LOG_DEBUG("Processing layer: %s", layerName.c_str());
        
        if (!ParseLayer(dataset, layerName, result.features, config)) {
            LOG_WARN("Failed to parse layer: %s", layerName.c_str());
        }
    }
    
    CloseDataset(dataset);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.statistics.totalFeatureCount = static_cast<int32_t>(result.features.size());
    result.statistics.successCount = result.statistics.totalFeatureCount;
    result.statistics.parseTimeMs = static_cast<double>(duration.count());
    
    result.success = true;
    result.errorCode = ErrorCode::Success;
    
    LOG_INFO("S57 parsing completed. %d features parsed in %.2f ms", 
             result.statistics.totalFeatureCount, result.statistics.parseTimeMs);
    
    return result;
}

bool S57Parser::ParseFeature(const std::string& data, Feature& feature) {
    LOG_ERROR("ParseFeature from string not supported for S57 format");
    return false;
}

std::vector<ChartFormat> S57Parser::GetSupportedFormats() const {
    return { ChartFormat::S57 };
}

bool S57Parser::OpenDataset(const std::string& filePath, void** dataset) {
    GDALDataset* ds = static_cast<GDALDataset*>(GDALOpenEx(
        filePath.c_str(),
        GDAL_OF_VECTOR,
        nullptr,
        nullptr,
        nullptr
    ));
    
    if (!ds) {
        LOG_ERROR("Failed to open S57 file: %s", filePath.c_str());
        return false;
    }
    
    *dataset = ds;
    return true;
}

void S57Parser::CloseDataset(void* dataset) {
    if (dataset) {
        GDALClose(static_cast<GDALDataset*>(dataset));
    }
}

bool S57Parser::ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config) {
    GDALDataset* ds = static_cast<GDALDataset*>(dataset);
    OGRLayer* layer = ds->GetLayerByName(layerName.c_str());
    
    if (!layer) {
        LOG_ERROR("Layer not found: %s", layerName.c_str());
        return false;
    }
    
    layer->ResetReading();
    
    OGRFeature* feature;
    while ((feature = layer->GetNextFeature()) != nullptr) {
        Feature outFeature;
        if (ParseOGRFeature(feature, outFeature, config)) {
            features.push_back(outFeature);
        }
        OGRFeature::DestroyFeature(feature);
    }
    
    return true;
}

bool S57Parser::ParseOGRFeature(void* feature, Feature& outFeature, const ParseConfig& config) {
    OGRFeature* ogrFeature = static_cast<OGRFeature*>(feature);
    
    outFeature.rcid = ogrFeature->GetFID();
    outFeature.id = std::to_string(outFeature.rcid);
    
    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    if (featureDefn) {
        outFeature.className = featureDefn->GetName();
        outFeature.type = m_featureTypeMapper->MapFeatureType(outFeature.className);
    }
    
    OGRGeometry* geometry = ogrFeature->GetGeometryRef();
    if (geometry) {
        if (!m_geometryConverter->ConvertGeometry(geometry, outFeature.geometry)) {
            if (config.strictMode) {
                return false;
            }
        }
    }
    
    m_attributeParser->ParseAttributes(ogrFeature, outFeature.attributes);
    
    return true;
}

} // namespace parser
} // namespace chart
