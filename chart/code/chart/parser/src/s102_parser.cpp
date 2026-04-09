#include "parser/s102_parser.h"
#include "parser/error_handler.h"

#include <ogrsf_frmts.h>
#include <gdal_priv.h>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace chart {
namespace parser {

S102Parser::S102Parser() {
}

S102Parser::~S102Parser() {
}

ParseResult S102Parser::ParseChart(const std::string& filePath, const ParseConfig& config) {
    ParseResult result;
    result.filePath = filePath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG_INFO("Parsing S102 file: {}", filePath);
    
    BathymetryGrid grid;
    if (ParseBathymetryGrid(filePath, grid)) {
        for (const auto& point : grid.points) {
            Feature feature;
            feature.id = std::to_string(result.features.size());
            feature.type = FeatureType::SOUNDG;
            feature.className = "SOUNDG";
            
            feature.geometry.type = GeometryType::Point;
            feature.geometry.points.push_back(Point(point.x, point.y, point.depth));
            
            AttributeValue depthAttr;
            depthAttr.type = AttributeValue::Type::Double;
            depthAttr.doubleValue = point.depth;
            feature.attributes["DEPTH"] = depthAttr;
            
            AttributeValue uncertAttr;
            uncertAttr.type = AttributeValue::Type::Double;
            uncertAttr.doubleValue = point.uncertainty;
            feature.attributes["UNCERTAINTY"] = uncertAttr;
            
            AttributeValue qualityAttr;
            qualityAttr.type = AttributeValue::Type::Integer;
            qualityAttr.intValue = point.quality;
            feature.attributes["QUALITY"] = qualityAttr;
            
            result.features.push_back(feature);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.statistics.totalFeatureCount = static_cast<int32_t>(result.features.size());
    result.statistics.successCount = result.statistics.totalFeatureCount;
    result.statistics.parseTimeMs = static_cast<double>(duration.count());
    
    result.success = true;
    result.errorCode = ErrorCode::Success;
    
    LOG_INFO("S102 parsing completed. {} depth points parsed in {} ms", 
             result.statistics.totalFeatureCount, result.statistics.parseTimeMs);
    
    return result;
}

std::vector<ChartFormat> S102Parser::GetSupportedFormats() const {
    return { ChartFormat::S102 };
}

bool S102Parser::ParseBathymetryGrid(const std::string& filePath, BathymetryGrid& grid) {
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(
        filePath.c_str(),
        GDAL_OF_RASTER | GDAL_OF_VECTOR,
        nullptr,
        nullptr,
        nullptr
    ));
    
    if (!dataset) {
        LOG_ERROR("Failed to open S102 file: {}", filePath);
        return false;
    }
    
    double geoTransform[6];
    if (dataset->GetGeoTransform(geoTransform) == CE_None) {
        grid.cellSizeX = geoTransform[1];
        grid.cellSizeY = std::abs(geoTransform[5]);
        grid.minX = geoTransform[0];
        grid.maxY = geoTransform[3];
    }
    
    int rasterCount = dataset->GetRasterCount();
    if (rasterCount > 0) {
        GDALRasterBand* depthBand = dataset->GetRasterBand(1);
        if (depthBand) {
            grid.rows = depthBand->GetYSize();
            grid.cols = depthBand->GetXSize();
            
            grid.maxX = grid.minX + grid.cols * grid.cellSizeX;
            grid.minY = grid.maxY - grid.rows * grid.cellSizeY;
            
            ParseDepthValues(depthBand, grid, grid.rows, grid.cols);
        }
    }
    
    GDALClose(dataset);
    return true;
}

bool S102Parser::ParseDepthValues(void* band, BathymetryGrid& grid, int rows, int cols) {
    GDALRasterBand* rasterBand = static_cast<GDALRasterBand*>(band);
    
    std::vector<float> values(cols);
    grid.points.reserve(static_cast<size_t>(rows) * cols);
    
    int hasNoData;
    double noDataValue = rasterBand->GetNoDataValue(&hasNoData);
    
    for (int row = 0; row < rows; ++row) {
        CPLErr err = rasterBand->RasterIO(
            GF_Read, 0, row, cols, 1,
            values.data(), cols, 1, GDT_Float32,
            0, 0
        );
        
        if (err != CE_None) {
            LOG_WARN("Failed to read row {} of depth band", row);
            continue;
        }
        
        for (int col = 0; col < cols; ++col) {
            float depth = values[col];
            
            if (hasNoData && std::abs(depth - noDataValue) < 1e-6) {
                continue;
            }
            
            BathymetryPoint point;
            point.x = grid.minX + (col + 0.5) * grid.cellSizeX;
            point.y = grid.maxY - (row + 0.5) * grid.cellSizeY;
            point.depth = static_cast<double>(depth);
            point.quality = 1;
            
            grid.points.push_back(point);
        }
    }
    
    LOG_INFO("Parsed {} bathymetry points from grid", grid.points.size());
    return true;
}

bool S102Parser::GetDepthAtPosition(const BathymetryGrid& grid, double x, double y, double& depth) {
    if (x < grid.minX || x > grid.maxX || y < grid.minY || y > grid.maxY) {
        return false;
    }
    
    int col = static_cast<int>((x - grid.minX) / grid.cellSizeX);
    int row = static_cast<int>((grid.maxY - y) / grid.cellSizeY);
    
    if (row < 0 || row >= grid.rows || col < 0 || col >= grid.cols) {
        return false;
    }
    
    size_t idx = static_cast<size_t>(row) * grid.cols + col;
    if (idx < grid.points.size()) {
        depth = grid.points[idx].depth;
        return true;
    }
    
    return false;
}

S102Parser::Statistics S102Parser::CalculateStatistics(const BathymetryGrid& grid) {
    Statistics stats;
    
    if (grid.points.empty()) {
        return stats;
    }
    
    stats.minDepth = grid.points[0].depth;
    stats.maxDepth = grid.points[0].depth;
    stats.pointCount = static_cast<int>(grid.points.size());
    
    double sum = 0;
    
    for (const auto& point : grid.points) {
        if (std::isnan(point.depth) || std::isinf(point.depth)) {
            stats.invalidCount++;
            continue;
        }
        
        if (point.depth < stats.minDepth) {
            stats.minDepth = point.depth;
        }
        if (point.depth > stats.maxDepth) {
            stats.maxDepth = point.depth;
        }
        
        sum += point.depth;
    }
    
    if (stats.pointCount > stats.invalidCount) {
        stats.avgDepth = sum / (stats.pointCount - stats.invalidCount);
    }
    
    return stats;
}

} // namespace parser
} // namespace chart
