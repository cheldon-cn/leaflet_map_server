#include "parser/s101_parser.h"
#include "parser/s101_gml_parser.h"
#include "parser/error_handler.h"

#include <ogrsf_frmts.h>
#include <chrono>

namespace chart {
namespace parser {

S101Parser::S101Parser()
    : m_gmlParser(new S101GMLParser()) {
}

S101Parser::~S101Parser() {
}

ParseResult S101Parser::ParseChart(const std::string& filePath, const ParseConfig& config) {
    ParseResult result;
    result.filePath = filePath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG_INFO("Parsing S101 file: {}", filePath);
    
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
    LOG_INFO("Dataset has {} layers", layerCount);
    
    for (int i = 0; i < layerCount; ++i) {
        OGRLayer* layer = dataset->GetLayer(i);
        if (!layer) {
            LOG_WARN("Failed to get layer {}", i);
            continue;
        }
        
        std::string layerName = layer->GetName();
        LOG_DEBUG("Processing layer: {}", layerName);
        
        if (!ParseLayer(dataset, layerName, result.features, config)) {
            LOG_WARN("Failed to parse layer: {}", layerName);
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
    
    LOG_INFO("S101 parsing completed. {} features parsed in {} ms", 
             result.statistics.totalFeatureCount, result.statistics.parseTimeMs);
    
    return result;
}

bool S101Parser::ParseFeature(const std::string& data, Feature& feature) {
    LOG_ERROR("ParseFeature from string not implemented for S101 format");
    return false;
}

std::vector<ChartFormat> S101Parser::GetSupportedFormats() const {
    return { ChartFormat::S101 };
}

bool S101Parser::OpenDataset(const std::string& filePath, void** dataset) {
    GDALDataset* ds = static_cast<GDALDataset*>(GDALOpenEx(
        filePath.c_str(),
        GDAL_OF_VECTOR,
        nullptr,
        nullptr,
        nullptr
    ));
    
    if (!ds) {
        LOG_ERROR("Failed to open S101 file: {}", filePath);
        return false;
    }
    
    *dataset = ds;
    return true;
}

void S101Parser::CloseDataset(void* dataset) {
    if (dataset) {
        GDALClose(static_cast<GDALDataset*>(dataset));
    }
}

bool S101Parser::ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config) {
    GDALDataset* ds = static_cast<GDALDataset*>(dataset);
    OGRLayer* layer = ds->GetLayerByName(layerName.c_str());
    
    if (!layer) {
        LOG_ERROR("Layer not found: {}", layerName);
        return false;
    }
    
    layer->ResetReading();
    
    OGRFeature* feature;
    while ((feature = layer->GetNextFeature()) != nullptr) {
        Feature outFeature;
        if (ParseGMLFeature(feature, outFeature, config)) {
            features.push_back(outFeature);
        }
        OGRFeature::DestroyFeature(feature);
    }
    
    return true;
}

bool S101Parser::ParseGMLFeature(void* feature, Feature& outFeature, const ParseConfig& config) {
    OGRFeature* ogrFeature = static_cast<OGRFeature*>(feature);
    
    outFeature.rcid = ogrFeature->GetFID();
    outFeature.id = std::to_string(outFeature.rcid);
    
    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    if (featureDefn) {
        outFeature.className = featureDefn->GetName();
    }
    
    OGRGeometry* geometry = ogrFeature->GetGeometryRef();
    if (geometry) {
        if (!m_gmlParser->ConvertGeometry(geometry, outFeature.geometry)) {
            if (config.strictMode) {
                return false;
            }
        }
    }
    
    m_gmlParser->ParseAttributes(ogrFeature, outFeature.attributes);
    
    return true;
}

} // namespace parser
} // namespace chart
