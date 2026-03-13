#ifndef CYCLE_RENDERER_RENDERER_H
#define CYCLE_RENDERER_RENDERER_H

#include "../database/idatabase.h"
#include "../encoder/iencoder.h"
#include "../cache/memory_cache.h"
#include "../config/config.h"
#include <memory>
#include <vector>
#include <string>

namespace cycle {
namespace renderer {

struct RenderRequest {
    BoundingBox bbox;
    int width;
    int height;
    std::vector<std::string> layers;
    std::string background_color;
    std::string filter;
    int zoom_level;
    encoder::ImageFormat format;
    int quality;
    int dpi;
    
    RenderRequest()
        : width(256)
        , height(256)
        , background_color("#ffffff")
        , zoom_level(-1)
        , format(encoder::ImageFormat::PNG32)
        , quality(90)
        , dpi(96) {}
    
    std::string GetCacheKey() const;
    bool IsValid() const;
};

struct RenderResult {
    bool success;
    std::vector<uint8_t> image_data;
    std::string error_message;
    bool from_cache;
    
    RenderResult() : success(false), from_cache(false) {}
    
    static RenderResult Success(std::vector<uint8_t> data, bool cached = false);
    static RenderResult Failure(const std::string& error);
};

class Renderer {
public:
    Renderer(std::shared_ptr<database::IDatabase> db,
             std::shared_ptr<encoder::IEncoder> encoder,
             std::shared_ptr<cache::MemoryCache> cache = nullptr);
    
    ~Renderer();
    
    RenderResult RenderMap(const RenderRequest& request);
    RenderResult RenderTile(int z, int x, int y, 
                            encoder::ImageFormat format = encoder::ImageFormat::PNG32,
                            int dpi = 96);
    
    void SetDatabase(std::shared_ptr<database::IDatabase> db);
    void SetEncoder(std::shared_ptr<encoder::IEncoder> encoder);
    void SetCache(std::shared_ptr<cache::MemoryCache> cache);
    
private:
    bool ValidateRequest(const RenderRequest& request) const;
    BoundingBox TileToBoundingBox(int z, int x, int y) const;
    
    encoder::RawImage CreateImageBuffer(int width, int height, int channels);
    bool FillBackground(encoder::RawImage& image, const std::string& color);
    bool RenderLayers(encoder::RawImage& image, 
                     const RenderRequest& request,
                     const std::vector<std::string>& layers);
    
    std::shared_ptr<database::IDatabase> database_;
    std::shared_ptr<encoder::IEncoder> encoder_;
    std::shared_ptr<cache::MemoryCache> cache_;
    
    RangeLimitConfig range_limit_;
};

} // namespace renderer
} // namespace cycle

#endif // CYCLE_RENDERER_RENDERER_H
