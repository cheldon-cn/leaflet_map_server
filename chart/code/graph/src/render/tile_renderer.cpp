#include "ogc/graph/render/tile_renderer.h"
#include <ogc/draw/raster_image_device.h>
#include <chrono>
#include <cstring>

namespace ogc {
namespace draw {

TileRenderer::TileRenderer()
    : m_tileWidth(256)
    , m_tileHeight(256)
    , m_outputFormat("png")
    , m_outputQuality(85)
    , m_backgroundColor(0xFFFFFFFF)
    , m_enableCache(true) {
}

TileRenderer::TileRenderer(cache::TilePyramidPtr pyramid)
    : m_pyramid(pyramid)
    , m_tileWidth(256)
    , m_tileHeight(256)
    , m_outputFormat("png")
    , m_outputQuality(85)
    , m_backgroundColor(0xFFFFFFFF)
    , m_enableCache(true) {
}

void TileRenderer::SetTilePyramid(cache::TilePyramidPtr pyramid) {
    m_pyramid = pyramid;
}

cache::TilePyramidPtr TileRenderer::GetTilePyramid() const {
    return m_pyramid;
}

void TileRenderer::SetRuleEngine(ogc::symbology::RuleEnginePtr engine) {
    m_ruleEngine = engine;
}

ogc::symbology::RuleEnginePtr TileRenderer::GetRuleEngine() const {
    return m_ruleEngine;
}

void TileRenderer::SetTileCache(cache::TileCachePtr cache) {
    m_cache = cache;
}

cache::TileCachePtr TileRenderer::GetTileCache() const {
    return m_cache;
}

void TileRenderer::SetTileSize(int width, int height) {
    m_tileWidth = width;
    m_tileHeight = height;
}

void TileRenderer::GetTileSize(int& width, int& height) const {
    width = m_tileWidth;
    height = m_tileHeight;
}

void TileRenderer::SetOutputFormat(const std::string& format) {
    m_outputFormat = format;
}

std::string TileRenderer::GetOutputFormat() const {
    return m_outputFormat;
}

void TileRenderer::SetOutputQuality(int quality) {
    m_outputQuality = quality;
}

int TileRenderer::GetOutputQuality() const {
    return m_outputQuality;
}

void TileRenderer::SetBackgroundColor(uint32_t color) {
    m_backgroundColor = color;
}

uint32_t TileRenderer::GetBackgroundColor() const {
    return m_backgroundColor;
}

void TileRenderer::SetEnableCache(bool enable) {
    m_enableCache = enable;
}

bool TileRenderer::IsCacheEnabled() const {
    return m_enableCache;
}

void TileRenderer::SetRenderCallback(TileRenderCallback callback) {
    m_renderCallback = callback;
}

TileRenderResult TileRenderer::RenderTile(const cache::TileKey& key) {
    return RenderTile(key, m_tileWidth, m_tileHeight);
}

TileRenderResult TileRenderer::RenderTile(const cache::TileKey& key, int width, int height) {
    if (m_enableCache && m_cache) {
        cache::TileData cachedTile;
        if (m_cache->Get(key, cachedTile)) {
            TileRenderResult result;
            result.key = key;
            result.data = cachedTile.data;
            result.success = true;
            return result;
        }
    }
    
    TileRenderResult result = DoRenderTile(key, width, height);
    
    if (result.success && m_enableCache && m_cache) {
        m_cache->Put(key, result.data);
    }
    
    return result;
}

TileRenderResult TileRenderer::DoRenderTile(const cache::TileKey& key, int width, int height) {
    TileRenderResult result;
    result.key = key;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!m_pyramid) {
        result.errorMessage = "Tile pyramid not set";
        return result;
    }
    
    Envelope tileExtent = m_pyramid->GetTileExtent(key);
    
    auto device = std::make_shared<RasterImageDevice>(width, height);
    auto context = DrawContext::Create(device.get());
    
    context->Begin();
    context->Clear(Color(m_backgroundColor));
    
    if (m_ruleEngine) {
        double resolution = m_pyramid->GetResolution(key.z);
        (void)resolution;
    }
    
    if (m_renderCallback) {
        m_renderCallback(key, result);
    }
    
    context->End();
    
    const uint8_t* imageData = device->GetPixelData();
    size_t dataSize = device->GetDataSize();
    if (imageData && dataSize > 0) {
        result.data.resize(dataSize);
        std::memcpy(result.data.data(), imageData, dataSize);
        result.success = true;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    return result;
}

TileRenderResult TileRenderer::RenderTileFromFeatures(const cache::TileKey& key, const std::vector<const CNFeature*>& features) {
    TileRenderResult result;
    result.key = key;
    
    if (!m_pyramid) {
        result.errorMessage = "Tile pyramid not set";
        return result;
    }
    
    if (!m_ruleEngine) {
        result.errorMessage = "Rule engine not set";
        return result;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Envelope tileExtent = m_pyramid->GetTileExtent(key);
    
    auto device = std::make_shared<RasterImageDevice>(m_tileWidth, m_tileHeight);
    auto context = DrawContext::Create(device.get());
    
    context->Begin();
    context->Clear(Color(m_backgroundColor));
    
    double resolution = m_pyramid->GetResolution(key.z);
    
    for (const auto& feature : features) {
        if (!feature) continue;
        
        GeometryPtr geom = feature->GetGeometry();
        if (!geom) continue;
        
        Envelope featEnv = geom->GetEnvelope();
        if (!tileExtent.Intersects(featEnv)) continue;
        
        m_ruleEngine->Render(feature, *context, resolution);
    }
    
    context->End();
    
    const uint8_t* imageData = device->GetPixelData();
    size_t dataSize = device->GetDataSize();
    if (imageData && dataSize > 0) {
        result.data.resize(dataSize);
        std::memcpy(result.data.data(), imageData, dataSize);
        result.success = true;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    return result;
}

std::vector<TileRenderResult> TileRenderer::RenderTiles(const std::vector<cache::TileKey>& keys) {
    std::vector<TileRenderResult> results;
    results.reserve(keys.size());
    
    for (const auto& key : keys) {
        results.push_back(RenderTile(key));
    }
    
    return results;
}

bool TileRenderer::RenderToImage(const cache::TileKey& key, DrawContext& context) {
    if (!m_pyramid) {
        return false;
    }
    
    Envelope tileExtent = m_pyramid->GetTileExtent(key);
    
    context.Begin();
    context.Clear(Color(m_backgroundColor));
    
    return true;
}

std::vector<cache::TileKey> TileRenderer::GetTilesForExtent(const Envelope& extent, double resolution) const {
    if (!m_pyramid) {
        return std::vector<cache::TileKey>();
    }
    
    int level = m_pyramid->GetLevelForResolution(resolution);
    return m_pyramid->GetTilesForExtent(extent, level);
}

void TileRenderer::ClearCache() {
    if (m_cache) {
        m_cache->Clear();
    }
}

TileRendererPtr TileRenderer::Create() {
    return std::make_shared<TileRenderer>();
}

TileRendererPtr TileRenderer::Create(cache::TilePyramidPtr pyramid) {
    return std::make_shared<TileRenderer>(pyramid);
}

}
}
