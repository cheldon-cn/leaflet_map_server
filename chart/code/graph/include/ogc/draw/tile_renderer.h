#ifndef OGC_DRAW_TILE_RENDERER_H
#define OGC_DRAW_TILE_RENDERER_H

#include "ogc/draw/export.h"
#include "ogc/draw/tile_key.h"
#include "ogc/draw/tile_cache.h"
#include "ogc/draw/rule_engine.h"
#include <ogc/draw/draw_context.h>
#include "ogc/feature/feature.h"
#include "ogc/envelope.h"
#include <memory>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct TileRenderParams {
    TileKey key;
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
    TileKey key;
    std::vector<uint8_t> data;
    bool success;
    std::string errorMessage;
    int64_t renderTime;
    
    TileRenderResult()
        : success(false)
        , renderTime(0) {}
};

typedef std::function<bool(const TileKey& key, TileRenderResult& result)> TileRenderCallback;

class TileRenderer;
typedef std::shared_ptr<TileRenderer> TileRendererPtr;

class OGC_GRAPH_API TileRenderer {
public:
    TileRenderer();
    explicit TileRenderer(TilePyramidPtr pyramid);
    ~TileRenderer() = default;
    
    void SetTilePyramid(TilePyramidPtr pyramid);
    TilePyramidPtr GetTilePyramid() const;
    
    void SetRuleEngine(RuleEnginePtr engine);
    RuleEnginePtr GetRuleEngine() const;
    
    void SetTileCache(TileCachePtr cache);
    TileCachePtr GetTileCache() const;
    
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
    
    TileRenderResult RenderTile(const TileKey& key);
    TileRenderResult RenderTile(const TileKey& key, int width, int height);
    
    TileRenderResult RenderTileFromFeatures(const TileKey& key, const std::vector<const CNFeature*>& features);
    
    std::vector<TileRenderResult> RenderTiles(const std::vector<TileKey>& keys);
    
    bool RenderToImage(const TileKey& key, DrawContext& context);
    
    std::vector<TileKey> GetTilesForExtent(const Envelope& extent, double resolution) const;
    
    void ClearCache();
    
    static TileRendererPtr Create();
    static TileRendererPtr Create(TilePyramidPtr pyramid);

private:
    TileRenderResult DoRenderTile(const TileKey& key, int width, int height);
    bool InitializeDrawContext(DrawContext& context, const TileRenderParams& params);
    
    TilePyramidPtr m_pyramid;
    RuleEnginePtr m_ruleEngine;
    TileCachePtr m_cache;
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
