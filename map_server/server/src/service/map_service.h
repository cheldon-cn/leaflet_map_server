#ifndef CYCLE_SERVICE_MAP_SERVICE_H
#define CYCLE_SERVICE_MAP_SERVICE_H

#include "../config/config.h"
#include "../cache/memory_cache.h"
#include "../encoder/iencoder.h"
#include "../renderer/renderer.h"
#include <atomic>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <cmath>

namespace cycle {
namespace service {

struct MapRequest {
    double min_lon;
    double min_lat;
    double max_lon;
    double max_lat;
    
    BoundingBox bbox;
    
    int width;
    int height;
    
    int zoom;
    
    std::string projection;
    encoder::ImageFormat format;
    int dpi;
    
    bool simplify;
    double simplification_tolerance;
    
    bool include_labels;
    bool include_boundaries;
    bool include_roads;
    
    std::vector<std::string> layers;
    std::string background_color;
    std::string filter;
    int quality;
    
    MapRequest()
        : min_lon(-180.0)
        , min_lat(-90.0)
        , max_lon(180.0)
        , max_lat(90.0)
        , bbox{-180.0, -90.0, 180.0, 90.0}
        , width(800)
        , height(600)
        , zoom(1)
        , projection("EPSG:3857")
        , format(encoder::ImageFormat::PNG32)
        , dpi(96)
        , simplify(true)
        , simplification_tolerance(0.0001)
        , include_labels(true)
        , include_boundaries(true)
        , include_roads(true)
        , background_color("#FFFFFF")
        , quality(85) {}
    
    bool IsValid() const {
        return width > 0 && height > 0 &&
               min_lon < max_lon && min_lat < max_lat;
    }
    
    std::string GetCacheKey() const {
        return std::to_string(min_lon) + "_" + std::to_string(min_lat) + "_" +
               std::to_string(max_lon) + "_" + std::to_string(max_lat) + "_" +
               std::to_string(width) + "_" + std::to_string(height) + "_" +
               std::to_string(zoom);
    }
};

struct MapResponse {
    bool success;
    std::vector<uint8_t> image_data;
    std::string mime_type;
    int width;
    int height;
    std::string error_message;
    int processing_time_ms;
    
    MapResponse()
        : success(false)
        , width(0)
        , height(0)
        , processing_time_ms(0) {}
};

struct TileRequest {
    int x;
    int y;
    int z;
    
    encoder::ImageFormat format;
    int dpi;
    
    TileRequest()
        : x(0)
        , y(0)
        , z(0)
        , format(encoder::ImageFormat::PNG32)
        , dpi(96) {}
    
    bool IsValid() const {
        int max = (1 << z);
        return x >= 0 && x < max && y >= 0 && y < max;
    }
};

struct TileResponse {
    bool success;
    std::vector<uint8_t> image_data;
    std::string mime_type;
    int x;
    int y;
    int z;
    std::string error_message;
    
    TileResponse()
        : success(false)
        , x(0)
        , y(0)
        , z(0) {}
};

struct ServiceResult {
    bool success;
    std::vector<uint8_t> data;
    bool from_cache;
    int processing_time;
    std::string error_message;
    
    // 批量操作相关字段
    std::string task_id;
    int estimated_tiles;
    int estimated_time;
    
    ServiceResult()
        : success(false)
        , from_cache(false)
        , processing_time(0)
        , estimated_tiles(0)
        , estimated_time(0) {}
    
    static ServiceResult Success(const std::vector<uint8_t>& data, bool from_cache = false) {
        ServiceResult result;
        result.success = true;
        result.data = data;
        result.from_cache = from_cache;
        return result;
    }
    
    static ServiceResult Failure(const std::string& error_message) {
        ServiceResult result;
        result.success = false;
        result.error_message = error_message;
        return result;
    }
};

struct ServiceMetrics {
    std::atomic<uint64_t> total_requests;
    std::atomic<uint64_t> cache_hits;
    std::atomic<uint64_t> cache_misses;
    std::atomic<uint64_t> error_count;
    std::atomic<uint64_t> total_processing_time_ms;
    
    ServiceMetrics() 
        : total_requests(0)
        , cache_hits(0)
        , cache_misses(0)
        , error_count(0)
        , total_processing_time_ms(0) {}
    
    ServiceMetrics(const ServiceMetrics&) = delete;
    ServiceMetrics& operator=(const ServiceMetrics&) = delete;
    
    ServiceMetrics(ServiceMetrics&& other) noexcept {
        total_requests = other.total_requests.load();
        cache_hits = other.cache_hits.load();
        cache_misses = other.cache_misses.load();
        error_count = other.error_count.load();
        total_processing_time_ms = other.total_processing_time_ms.load();
    }
    
    ServiceMetrics& operator=(ServiceMetrics&& other) noexcept {
        if (this != &other) {
            total_requests = other.total_requests.load();
            cache_hits = other.cache_hits.load();
            cache_misses = other.cache_misses.load();
            error_count = other.error_count.load();
            total_processing_time_ms = other.total_processing_time_ms.load();
        }
        return *this;
    }
    
    double GetHitRate() const {
        uint64_t total = total_requests.load();
        if (total == 0) return 0.0;
        return static_cast<double>(cache_hits.load()) / total * 100.0;
    }
    
    double GetAverageProcessingTime() const {
        uint64_t total = total_requests.load();
        if (total == 0) return 0.0;
        return static_cast<double>(total_processing_time_ms.load()) / total;
    }
};

class RateLimiter {
public:
    RateLimiter(size_t max_requests, size_t time_window_ms)
        : max_requests_(max_requests)
        , time_window_ms_(time_window_ms) {
        Reset();
    }
    
    bool TryAcquire() {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(mutex_);
        
        while (!requests_.empty() && 
               std::chrono::duration_cast<std::chrono::milliseconds>(now - requests_.front()).count() > time_window_ms_) {
            requests_.pop();
        }
        
        if (requests_.size() < max_requests_) {
            requests_.push(now);
            return true;
        }
        
        return false;
    }
    
    void Reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!requests_.empty()) {
            requests_.pop();
        }
    }
    
    size_t GetCurrentCount() const {
        return requests_.size();
    }
    
    bool AllowRequest() {
        return TryAcquire();
    }
    
private:
    size_t max_requests_;
    size_t time_window_ms_;
    std::queue<std::chrono::steady_clock::time_point> requests_;
    mutable std::mutex mutex_;
};

struct TileBounds {
    double minX;
    double minY;
    double maxX;
    double maxY;
    
    TileBounds() : minX(0), minY(0), maxX(0), maxY(0) {}
    TileBounds(double minx, double miny, double maxx, double maxy)
        : minX(minx), minY(miny), maxX(maxx), maxY(maxy) {}
};

struct BatchRequest {
    BoundingBox bounds;
    std::vector<int> zoom_levels;
    encoder::ImageFormat format;
    int dpi;
    bool async;
    std::string task_id;
    int estimated_tiles;
    int estimated_time;
    
    BatchRequest()
        : format(encoder::ImageFormat::PNG32)
        , dpi(96)
        , async(true)
        , estimated_tiles(0)
        , estimated_time(0) {}
};

class MapService {
public:
    explicit MapService(std::shared_ptr<renderer::Renderer> renderer = nullptr,
                       std::shared_ptr<cache::MemoryCache> cache = nullptr,
                       const Config& config = Config());
    ~MapService();
    
    ServiceResult GenerateMap(const MapRequest& request);
    ServiceResult GenerateTile(const TileRequest& request);
    ServiceResult GetTile(int z, int x, int y, encoder::ImageFormat format, int dpi);
    
    // 新增 API 方法
    TileBounds GetTileBounds(int z, int x, int y);
    ServiceResult BatchGenerateTiles(const BatchRequest& request);
    
    const ServiceMetrics& GetMetrics();
    void ResetMetrics();
    
    void SetRenderer(std::shared_ptr<renderer::Renderer> renderer);
    void SetCache(std::shared_ptr<cache::MemoryCache> cache);
    
    bool IsRunning() const { return running_; }
    void Stop() { running_ = false; }
    
private:
    ServiceResult ProcessMapRequest(const MapRequest& request);
    ServiceResult ProcessTileRequest(int z, int x, int y, 
                                    encoder::ImageFormat format, int dpi);
    
    bool ValidateTileRequest(int z, int x, int y) const;
    bool ValidateMapRequest(const MapRequest& request) const;
    bool CheckRateLimit();
    
    std::shared_ptr<renderer::Renderer> renderer_;
    std::shared_ptr<cache::MemoryCache> cache_;
    Config config_;
    
    ServiceMetrics metrics_;
    RateLimiter rate_limiter_;
    
    mutable std::mutex mutex_;
    bool running_;
};

} // namespace service
} // namespace cycle

#endif // CYCLE_SERVICE_MAP_SERVICE_H
