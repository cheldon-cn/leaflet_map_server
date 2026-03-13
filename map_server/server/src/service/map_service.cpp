#include "map_service.h"
#include "../utils/logger.h"
#include <chrono>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cycle {
namespace service {

MapService::MapService(std::shared_ptr<renderer::Renderer> renderer,
                       std::shared_ptr<cache::MemoryCache> cache,
                       const Config& config)
    : renderer_(renderer)
    , cache_(cache)
    , rate_limiter_(config.server.thread_count * 10, config.server.thread_count * 20) {

   // config_ = (config);

    if (!renderer_) {
        LOG_ERROR("MapService initialized with null renderer");
    }
    
    if (!cache_ && config_.cache.enabled) {
        LOG_WARN("MapService cache is enabled but no cache instance provided");
    }
    
    LOG_INFO("MapService initialized with rate limit: " + 
             std::to_string(config_.server.thread_count * 10) + " req/s");
}

MapService::~MapService() {
    LOG_INFO("MapService destroyed");
}

ServiceResult MapService::GetTile(int z, int x, int y, 
                                 encoder::ImageFormat format, int dpi) {
    auto start_time = std::chrono::steady_clock::now();
    
    if (!CheckRateLimit()) {
        LOG_WARN("Rate limit exceeded for tile request: " + 
                 std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y));
        return ServiceResult::Failure("Rate limit exceeded");
    }
    
    if (!ValidateTileRequest(z, x, y)) {
        LOG_WARN("Invalid tile request: " + 
                 std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y));
        return ServiceResult::Failure("Invalid tile coordinates");
    }
    
    metrics_.total_requests++;
    
    ServiceResult result = ProcessTileRequest(z, x, y, format, dpi);
    
    auto end_time = std::chrono::steady_clock::now();
    auto processing_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    result.processing_time = static_cast<int>(processing_time.count());
    metrics_.total_processing_time_ms += processing_time.count();
    
    if (!result.success) {
        metrics_.error_count++;
        LOG_ERROR("Tile request failed: " + result.error_message);
    } else {
        LOG_DEBUG("Tile request completed in " + 
                  std::to_string(processing_time.count()) + "ms");
    }
    
    return result;
}

ServiceResult MapService::GenerateMap(const MapRequest& request) {
    auto start_time = std::chrono::steady_clock::now();
    
    if (!CheckRateLimit()) {
        LOG_WARN("Rate limit exceeded for map generation request");
        return ServiceResult::Failure("Rate limit exceeded");
    }
    
    if (!ValidateMapRequest(request)) {
        LOG_WARN("Invalid map generation request");
        return ServiceResult::Failure("Invalid map request parameters");
    }
    
    metrics_.total_requests++;
    
    ServiceResult result = ProcessMapRequest(request);
    
    auto end_time = std::chrono::steady_clock::now();
    auto processing_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    result.processing_time = static_cast<int>(processing_time.count());
    metrics_.total_processing_time_ms += processing_time.count();
    
    if (!result.success) {
        metrics_.error_count++;
        LOG_ERROR("Map generation request failed: " + result.error_message);
    } else {
        LOG_DEBUG("Map generation completed in " + 
                  std::to_string(processing_time.count()) + "ms");
    }
    
    return result;
}

bool MapService::ValidateTileRequest(int z, int x, int y) const {
    if (z < 0 || x < 0 || y < 0) {
        return false;
    }
    
    if (config_.range_limit.enabled) {
        if (z > config_.range_limit.max_zoom) {
            return false;
        }
        
        int max_tile = (1 << z) - 1;
        if (x > max_tile || y > max_tile) {
            return false;
        }
    }
    
    return true;
}

bool MapService::ValidateMapRequest(const MapRequest& request) const {
    if (!request.IsValid()) {
        return false;
    }
    
    if (config_.range_limit.enabled) {
        if (!config_.range_limit.ValidateBoundingBox(request.bbox)) {
            return false;
        }
        
        if (!config_.range_limit.ValidateImageSize(request.width, request.height)) {
            return false;
        }
        
        if (!config_.range_limit.ValidateDpi(request.dpi)) {
            return false;
        }
    }
    
    return true;
}

const ServiceMetrics& MapService::GetMetrics() {
    return metrics_;
}

void MapService::ResetMetrics() {
    metrics_.total_requests = 0;
    metrics_.cache_hits = 0;
    metrics_.cache_misses = 0;
    metrics_.error_count = 0;
    metrics_.total_processing_time_ms = 0;
    
    LOG_INFO("MapService metrics reset");
}

void MapService::SetRenderer(std::shared_ptr<renderer::Renderer> renderer) {
    std::lock_guard<std::mutex> lock(mutex_);
    renderer_ = renderer;
    LOG_INFO("MapService renderer updated");
}

void MapService::SetCache(std::shared_ptr<cache::MemoryCache> cache) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_ = cache;
    LOG_INFO("MapService cache updated");
}

ServiceResult MapService::ProcessMapRequest(const MapRequest& request) {
    std::string cacheKey = request.GetCacheKey();
    
    if (cache_ && config_.cache.enabled) {
        std::vector<uint8_t> cachedData;
        if (cache_->Get(cacheKey, cachedData)) {
            metrics_.cache_hits++;
            LOG_DEBUG("Map cache hit for key: " + cacheKey);
            return ServiceResult::Success(cachedData, true);
        }
        metrics_.cache_misses++;
    }
    
    if (!renderer_) {
        return ServiceResult::Failure("Renderer not available");
    }
    
    renderer::RenderRequest renderRequest;
    renderRequest.bbox = request.bbox;
    renderRequest.width = request.width;
    renderRequest.height = request.height;
    renderRequest.layers = request.layers;
    renderRequest.background_color = request.background_color;
    renderRequest.filter = request.filter;
    renderRequest.format = request.format;
    renderRequest.quality = request.quality;
    renderRequest.dpi = request.dpi;
    
    auto renderResult = renderer_->RenderMap(renderRequest);
    
    if (!renderResult.success) {
        return ServiceResult::Failure("Rendering failed: " + renderResult.error_message);
    }
    
    if (cache_ && config_.cache.enabled) {
        cache_->Put(cacheKey, renderResult.image_data);
    }
    
    return ServiceResult::Success(renderResult.image_data, false);
}

ServiceResult MapService::ProcessTileRequest(int z, int x, int y, 
                                            encoder::ImageFormat format, int dpi) {
    std::string cacheKey = "tile_" + std::to_string(z) + "_" + 
                          std::to_string(x) + "_" + std::to_string(y) + "_" + 
                          std::to_string(static_cast<int>(format)) + "_" + 
                          std::to_string(dpi);
    
    if (cache_ && config_.cache.enabled) {
        std::vector<uint8_t> cachedData;
        if (cache_->Get(cacheKey, cachedData)) {
            metrics_.cache_hits++;
            LOG_DEBUG("Tile cache hit for key: " + cacheKey);
            return ServiceResult::Success(cachedData, true);
        }
        metrics_.cache_misses++;
    }
    
    if (!renderer_) {
        return ServiceResult::Failure("Renderer not available");
    }
    
    auto renderResult = renderer_->RenderTile(z, x, y, format, dpi);
    
    if (!renderResult.success) {
        return ServiceResult::Failure("Tile rendering failed: " + renderResult.error_message);
    }
    
    if (cache_ && config_.cache.enabled) {
        cache_->Put(cacheKey, renderResult.image_data);
    }
    
    return ServiceResult::Success(renderResult.image_data, false);
}

bool MapService::CheckRateLimit() {
    if (config_.range_limit.enabled) {
        return rate_limiter_.AllowRequest();
    }
    return true;
}

TileBounds MapService::GetTileBounds(int z, int x, int y) {
    // 计算瓦片的边界框
    // 使用 Web Mercator 投影的边界计算
    double n = M_PI - (2.0 * M_PI * y) / (1 << z);
    double minX = x * 360.0 / (1 << z) - 180.0;
    double minY = 180.0 / M_PI * atan(sinh(n));
    
    n = M_PI - (2.0 * M_PI * (y + 1)) / (1 << z);
    double maxX = (x + 1) * 360.0 / (1 << z) - 180.0;
    double maxY = 180.0 / M_PI * atan(sinh(n));
    
    return TileBounds(minX, minY, maxX, maxY);
}

ServiceResult MapService::BatchGenerateTiles(const BatchRequest& request) {
    // 简化实现：返回任务信息
    // 实际的批量生成逻辑需要根据业务需求实现
    
    ServiceResult result;
    result.success = true;
    result.from_cache = false;
    result.processing_time = 0;
    
    // 设置任务信息
    result.task_id = "batch_" + std::to_string(std::time(nullptr));
    result.estimated_tiles = 1000; // 示例值
    result.estimated_time = 300;   // 示例值（秒）
    
    return result;
}

} // namespace service
} // namespace cycle