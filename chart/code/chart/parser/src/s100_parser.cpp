#include "parser/s100_parser.h"
#include "parser/error_handler.h"
#include "parser/ogr_data_converter.h"

#include <ogrsf_frmts.h>
#include <chrono>

namespace chart {
namespace parser {

S100Parser::S100Parser() {
}

S100Parser::~S100Parser() {
}

ParseResult S100Parser::ParseChart(const std::string& filePath, const ParseConfig& config) {
    ParseResult result;
    result.filePath = filePath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG_INFO("Parsing S100 file: {}", filePath);
    
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
    
    LOG_INFO("S100 parsing completed. {} features parsed in {} ms", 
             result.statistics.totalFeatureCount, result.statistics.parseTimeMs);
    
    return result;
}

bool S100Parser::ParseFeature(const std::string& data, Feature& feature) {
    LOG_ERROR("ParseFeature from string not implemented for S100 format");
    return false;
}

std::vector<ChartFormat> S100Parser::GetSupportedFormats() const {
    return { ChartFormat::S100 };
}

bool S100Parser::GetDatasetInfo(const std::string& filePath, S100DatasetInfo& info) {
    GDALDataset* dataset = nullptr;
    if (!OpenDataset(filePath, reinterpret_cast<void**>(&dataset))) {
        return false;
    }
    
    bool result = ParseMetadata(dataset, info);
    CloseDataset(dataset);
    
    return result;
}

bool S100Parser::IsS100File(const std::string& filePath) {
    if (filePath.find(".h5") != std::string::npos ||
        filePath.find(".hdf5") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool S100Parser::OpenDataset(const std::string& filePath, void** dataset) {
    GDALDataset* ds = static_cast<GDALDataset*>(GDALOpenEx(
        filePath.c_str(),
        GDAL_OF_VECTOR,
        nullptr,
        nullptr,
        nullptr
    ));
    
    if (!ds) {
        LOG_ERROR("Failed to open S100 file: {}", filePath);
        return false;
    }
    
    *dataset = ds;
    return true;
}

void S100Parser::CloseDataset(void* dataset) {
    if (dataset) {
        GDALClose(static_cast<GDALDataset*>(dataset));
    }
}

bool S100Parser::ParseMetadata(void* dataset, S100DatasetInfo& info) {
    GDALDataset* ds = static_cast<GDALDataset*>(dataset);
    
    char** metadata = ds->GetMetadata();
    if (metadata) {
        for (int i = 0; metadata[i] != nullptr; ++i) {
            std::string entry = metadata[i];
            size_t pos = entry.find('=');
            if (pos != std::string::npos) {
                std::string key = entry.substr(0, pos);
                std::string value = entry.substr(pos + 1);
                info.metadata[key] = value;
                
                if (key == "product_id" || key == "PRODUCT_ID") {
                    info.productId = value;
                } else if (key == "edition_number" || key == "EDITION_NUMBER") {
                    info.editionNumber = value;
                } else if (key == "update_number" || key == "UPDATE_NUMBER") {
                    info.updateNumber = value;
                } else if (key == "issue_date" || key == "ISSUE_DATE") {
                    info.issueDate = value;
                }
            }
        }
    }
    
    return true;
}

bool S100Parser::ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config) {
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
        if (ParseOGRFeature(feature, outFeature, config)) {
            features.push_back(outFeature);
        }
        OGRFeature::DestroyFeature(feature);
    }
    
    return true;
}

bool S100Parser::ParseOGRFeature(void* feature, Feature& outFeature, const ParseConfig& config) {
    OGRFeature* ogrFeature = static_cast<OGRFeature*>(feature);
    
    outFeature.rcid = ogrFeature->GetFID();
    outFeature.id = std::to_string(outFeature.rcid);
    
    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    if (featureDefn) {
        outFeature.className = featureDefn->GetName();
    }
    
    OGRGeometry* geometry = ogrFeature->GetGeometryRef();
    if (geometry) {
        OGRDataConverter converter;
        if (!converter.ConvertGeometry(geometry, outFeature.geometry)) {
            if (config.strictMode) {
                return false;
            }
        }
    }
    
    OGRDataConverter converter;
    converter.ConvertAttributes(ogrFeature, outFeature.attributes);
    
    return true;
}

std::string S100Parser::DetectProductType(void* dataset) {
    S100DatasetInfo info;
    ParseMetadata(dataset, info);
    return info.productId;
}

} // namespace parser
} // namespace chart
