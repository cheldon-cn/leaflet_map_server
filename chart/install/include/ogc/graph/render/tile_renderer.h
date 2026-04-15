#ifndef OGC_DRAW_TILE_RENDERER_H
#define OGC_DRAW_TILE_RENDERER_H

#include "ogc/graph/export.h"
#include <ogc/cache/tile/tile_key.h>
#include <ogc/cache/tile/tile_cache.h>
#include "ogc/symbology/filter/rule_engine.h"
#include <ogc/draw/draw_context.h>
#include "ogc/feature/feature.h"
#include <ogc/geom/envelope.h>
#include <memory>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct TileRenderParams {
    cache::TileKey key;
    Envelope extent;
    int width;
    int height;
    double resolution;
    std::string format;
    int quality;
    
    TileRenderParams()
        : width(256)
        , height(256)
        , resolution(1.0)
        , format("png")
        , quality(85) {}
};

struct TileRenderResult {
    cache::TileKey key;
    std::vector<uint8_t> data;
    bool success;
    std::string errorMessage;
    int64_t renderTime;
    
    TileRenderResult()
        : success(false)
        , renderTime(0) {}
};

typedef std::function<bool(const cache::TileKey& key, TileRenderResult& result)> TileRenderCallback;

class TileRenderer;
typedef std::shared_ptr<TileRenderer> TileRendererPtr;

class OGC_GRAPH_API TileRenderer {
public:
    TileRenderer();
    explicit TileRenderer(cache::TilePyramidPtr pyramid);
    ~TileRenderer() = default;
    
    void SetTilePyramid(cache::TilePyramidPtr pyramid);
    cache::TilePyramidPtr GetTilePyramid() const;
    
    void SetRuleEngine(ogc::symbology::RuleEnginePtr engine);
    ogc::symbology::RuleEnginePtr GetRuleEngine() const;
    
    void SetTileCache(cache::TileCachePtr cache);
    cache::TileCachePtr GetTileCache() const;
    
    void SetTileSize(int width, int height);
    void GetTileSize(int& width, int& height) const;
    
    void SetOutputFormat(const std::string& format);
    std::string GetOutputFormat() const;
    
    void SetOutputQuality(int quality);
    int GetOutputQuality() const;
    
    void SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    
    void SetEnableCache(bool enable);
    bool IsCacheEnabled() const;
    
    void SetRenderCallback(TileRenderCallback callback);
    
    TileRenderResult RenderTile(const cache::TileKey& key);
    TileRenderResult RenderTile(const cache::TileKey& key, int width, int height);
    
    TileRenderResult RenderTileFromFeatures(const cache::TileKey& key, const std::vector<const CNFeature*>& features);
    
    std::vector<TileRenderResult> RenderTiles(const std::vector<cache::TileKey>& keys);
    
    bool RenderToImage(const cache::TileKey& key, DrawContext& context);
    
    std::vector<cache::TileKey> GetTilesForExtent(const Envelope& extent, double resolution) const;
    
    void ClearCache();
    
    static TileRendererPtr Create();
    static TileRendererPtr Create(cache::TilePyramidPtr pyramid);

private:
    TileRenderResult DoRenderTile(const cache::TileKey& key, int width, int height);
    bool InitializeDrawContext(DrawContext& context, const TileRenderParams& params);
    
    cache::TilePyramidPtr m_pyramid;
    ogc::symbology::RuleEnginePtr m_ruleEngine;
    cache::TileCachePtr m_cache;
    int m_tileWidth;
    int m_tileHeight;
    std::string m_outputFormat;
    int m_outputQuality;
    uint32_t m_backgroundColor;
    bool m_enableCache;
    TileRenderCallback m_renderCallback;
};

}
}

#endif
