#ifndef CYCLE_IRENDER_ENGINE_H
#define CYCLE_IRENDER_ENGINE_H

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <chrono>

namespace cycle {

enum class ImageFormat {
    PNG8,
    PNG32,
    WEBP
};

enum class RenderErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = 1001,
    INVALID_BBOX = 1002,
    INVALID_SIZE = 1003,
    INVALID_FORMAT = 1004,
    INVALID_DPI = 1005,
    RESOURCE_ALLOCATION_FAILED = 2001,
    MEMORY_ALLOCATION_FAILED = 2002,
    RENDERING_FAILED = 3001,
    ENCODING_FAILED = 3002,
    LAYER_NOT_FOUND = 3003,
    FEATURE_QUERY_FAILED = 3004,
    TIMEOUT = 4001,
    THREAD_POOL_ERROR = 4002
};

struct BoundingBox {
    double minX;
    double minY;
    double maxX;
    double maxY;
    int srid;
    
    BoundingBox() : minX(0), minY(0), maxX(0), maxY(0), srid(4326) {}
    
    BoundingBox(double minx, double miny, double maxx, double maxy, int s = 4326)
        : minX(minx), minY(miny), maxX(maxx), maxY(maxy), srid(s) {}
    
    double Width() const { return maxX - minX; }
    double Height() const { return maxY - minY; }
    bool IsValid() const { return minX < maxX && minY < maxY; }
};

struct RenderRequest {
    BoundingBox bbox;
    int width;
    int height;
    std::vector<std::string> layers;
    std::string backgroundColor;
    std::string filter;
    int zoomLevel;
    ImageFormat format;
    int quality;
    int dpi;
    
    RenderRequest()
        : width(256)
        , height(256)
        , backgroundColor("#ffffff")
        , zoomLevel(-1)
        , format(ImageFormat::PNG32)
        , quality(90)
        , dpi(96) {}
    
    bool IsValid() const;
    std::string GetCacheKey() const;
};

struct RenderMetadata {
    int width;
    int height;
    ImageFormat format;
    int dpi;
    int quality;
    size_t dataSize;
    std::chrono::milliseconds renderTime;
    std::chrono::milliseconds encodeTime;
    bool fromCache;
    
    RenderMetadata()
        : width(0)
        , height(0)
        , format(ImageFormat::PNG32)
        , dpi(96)
        , quality(90)
        , dataSize(0)
        , fromCache(false) {}
};

struct RenderResult {
    bool success;
    RenderErrorCode errorCode;
    std::string errorMessage;
    std::vector<uint8_t> imageData;
    RenderMetadata metadata;
    
    RenderResult() : success(false), errorCode(RenderErrorCode::SUCCESS) {}
    
    static RenderResult Success(std::vector<uint8_t> data, const RenderMetadata& meta);
    static RenderResult Failure(RenderErrorCode code, const std::string& message);
};

struct TileCoordinate {
    int z;
    int x;
    int y;
    
    TileCoordinate() : z(0), x(0), y(0) {}
    TileCoordinate(int zoom, int tileX, int tileY) : z(zoom), x(tileX), y(tileY) {}
    
    bool IsValid() const { return z >= 0 && x >= 0 && y >= 0; }
    BoundingBox ToBoundingBox() const;
    std::string ToString() const;
};

struct RenderStatistics {
    int64_t totalRenders;
    int64_t successfulRenders;
    int64_t failedRenders;
    double avgRenderTimeMs;
    double p95RenderTimeMs;
    double p99RenderTimeMs;
    double avgEncodeTimeMs;
    int64_t cacheHits;
    int64_t cacheMisses;
    double cacheHitRate;
    
    RenderStatistics()
        : totalRenders(0)
        , successfulRenders(0)
        , failedRenders(0)
        , avgRenderTimeMs(0)
        , p95RenderTimeMs(0)
        , p99RenderTimeMs(0)
        , avgEncodeTimeMs(0)
        , cacheHits(0)
        , cacheMisses(0)
        , cacheHitRate(0) {}
};

class IRenderEngine {
public:
    virtual ~IRenderEngine() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    virtual RenderResult RenderMap(const RenderRequest& request) = 0;
    virtual RenderResult RenderTile(const TileCoordinate& coord,
                                    ImageFormat format,
                                    int dpi) = 0;
    
    virtual bool SupportsFormat(ImageFormat format) const = 0;
    virtual bool SupportsDpi(int dpi) const = 0;
    
    virtual RenderStatistics GetStatistics() const = 0;
    virtual void ResetStatistics() = 0;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
};

} // namespace cycle

#endif // CYCLE_IRENDER_ENGINE_H
