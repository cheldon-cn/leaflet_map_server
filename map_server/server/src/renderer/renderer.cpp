#include "renderer.h"
#include "../utils/logger.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#define M_PI       3.14159265358979323846
namespace cycle {
namespace renderer {

Renderer::Renderer(std::shared_ptr<database::IDatabase> db,
                   std::shared_ptr<encoder::IEncoder> encoder,
                   std::shared_ptr<cache::MemoryCache> cache)
    : database_(db)
    , encoder_(encoder)
    , cache_(cache) {
    
    LOG_INFO("Renderer initialized");
}

Renderer::~Renderer() {
    LOG_INFO("Renderer destroyed");
}

RenderResult Renderer::RenderMap(const RenderRequest& request) {
    if (!ValidateRequest(request)) {
        return RenderResult::Failure("Invalid render request");
    }
    
    std::string cacheKey = request.GetCacheKey();
    
    if (cache_) {
        std::vector<uint8_t> cachedData;
        if (cache_->Get(cacheKey, cachedData)) {
            LOG_DEBUG("Cache hit for key: " + cacheKey);
            return RenderResult::Success(cachedData, true);
        }
    }
    
    auto image = CreateImageBuffer(request.width, request.height, 4);
    
    if (!FillBackground(image, request.background_color)) {
        return RenderResult::Failure("Failed to fill background");
    }
    
    if (!RenderLayers(image, request, request.layers)) {
        return RenderResult::Failure("Failed to render layers");
    }
    
    encoder::EncodeOptions options;
    options.format = request.format;
    options.quality = request.quality;
    options.dpi_x = request.dpi;
    options.dpi_y = request.dpi;
    
    auto encoded = encoder_->Encode(image, options);
    
    if (!encoded.IsValid()) {
        return RenderResult::Failure("Failed to encode image");
    }
    
    if (cache_) {
        cache_->Put(cacheKey, encoded.data);
    }
    
    LOG_DEBUG("Rendered map: " + std::to_string(request.width) + "x" + 
              std::to_string(request.height));
    
    return RenderResult::Success(encoded.data, false);
}

RenderResult Renderer::RenderTile(int z, int x, int y, 
                                  encoder::ImageFormat format, int dpi) {
    RenderRequest request;
    request.bbox = TileToBoundingBox(z, x, y);
    request.width = 256;
    request.height = 256;
    request.format = format;
    request.dpi = dpi;
    request.zoom_level = z;
    
    return RenderMap(request);
}

void Renderer::SetDatabase(std::shared_ptr<database::IDatabase> db) {
    database_ = db;
}

void Renderer::SetEncoder(std::shared_ptr<encoder::IEncoder> encoder) {
    encoder_ = encoder;
}

void Renderer::SetCache(std::shared_ptr<cache::MemoryCache> cache) {
    cache_ = cache;
}

bool Renderer::ValidateRequest(const RenderRequest& request) const {
    if (!request.bbox.IsValid()) {
        LOG_ERROR("Invalid bounding box");
        return false;
    }
    
    if (request.width <= 0 || request.height <= 0) {
        LOG_ERROR("Invalid image size");
        return false;
    }
    
    if (range_limit_.enabled) {
        if (!range_limit_.ValidateBoundingBox(request.bbox)) {
            LOG_ERROR("Bounding box out of range");
            return false;
        }
        
        if (!range_limit_.ValidateImageSize(request.width, request.height)) {
            LOG_ERROR("Image size out of range");
            return false;
        }
        
        if (!range_limit_.ValidateDpi(request.dpi)) {
            LOG_ERROR("DPI out of range");
            return false;
        }
    }
    
    return true;
}

BoundingBox Renderer::TileToBoundingBox(int z, int x, int y) const {
    double n = std::pow(2.0, z);
    double minLon = x / n * 360.0 - 180.0;
    double maxLon = (x + 1) / n * 360.0 - 180.0;
    
    double minLat = std::atan(std::sinh(M_PI * (1 - 2 * y / n))) * 180.0 / M_PI;
    double maxLat = std::atan(std::sinh(M_PI * (1 - 2 * (y + 1) / n))) * 180.0 / M_PI;
    
    return BoundingBox(minLon, minLat, maxLon, maxLat);
}

encoder::RawImage Renderer::CreateImageBuffer(int width, int height, int channels) {
    return encoder::RawImage::Create(width, height, channels);
}

bool Renderer::FillBackground(encoder::RawImage& image, const std::string& color) {
    if (color.empty() || color[0] != '#') {
        return false;
    }
    
    uint8_t r = 255, g = 255, b = 255, a = 255;
    
    if (color.length() == 7) {
        r = static_cast<uint8_t>(std::stoul(color.substr(1, 2), nullptr, 16));
        g = static_cast<uint8_t>(std::stoul(color.substr(3, 2), nullptr, 16));
        b = static_cast<uint8_t>(std::stoul(color.substr(5, 2), nullptr, 16));
    } else if (color.length() == 9) {
        r = static_cast<uint8_t>(std::stoul(color.substr(1, 2), nullptr, 16));
        g = static_cast<uint8_t>(std::stoul(color.substr(3, 2), nullptr, 16));
        b = static_cast<uint8_t>(std::stoul(color.substr(5, 2), nullptr, 16));
        a = static_cast<uint8_t>(std::stoul(color.substr(7, 2), nullptr, 16));
    } else {
        return false;
    }
    
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            size_t idx = (y * image.width + x) * image.channels;
            image.data[idx] = r;
            image.data[idx + 1] = g;
            image.data[idx + 2] = b;
            if (image.channels == 4) {
                image.data[idx + 3] = a;
            }
        }
    }
    
    return true;
}

bool Renderer::RenderLayers(encoder::RawImage& image,
                            const RenderRequest& request,
                            const std::vector<std::string>& layers) {
    if (!database_) {
        LOG_ERROR("Database not initialized");
        return false;
    }
    
    for (const auto& layerName : layers) {
        auto result = database_->QuerySpatial(layerName, request.bbox);
        
        if (!result) {
            LOG_WARN("Failed to query layer: " + layerName);
            continue;
        }
        
        while (result->Next()) {
            auto row = result->GetCurrentRow();
            if (!row) {
                continue;
            }
            
            // TODO: 实现要素渲染逻辑
            // 这里需要根据几何类型和样式进行渲染
        }
    }
    
    return true;
}

std::string RenderRequest::GetCacheKey() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "map:" << bbox.minX << ":" << bbox.minY << ":"
        << bbox.maxX << ":" << bbox.maxY << ":"
        << width << ":" << height << ":"
        << static_cast<int>(format) << ":" << dpi;
    
    for (const auto& layer : layers) {
        oss << ":" << layer;
    }
    
    return oss.str();
}

bool RenderRequest::IsValid() const {
    return bbox.IsValid() && width > 0 && height > 0;
}

RenderResult RenderResult::Success(std::vector<uint8_t> data, bool cached) {
    RenderResult result;
    result.success = true;
    result.image_data = std::move(data);
    result.from_cache = cached;
    return result;
}

RenderResult RenderResult::Failure(const std::string& error) {
    RenderResult result;
    result.success = false;
    result.error_message = error;
    return result;
}

} // namespace renderer
} // namespace cycle
