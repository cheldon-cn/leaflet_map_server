#ifndef OGC_DRAW_TILE_SIZE_STRATEGY_H
#define OGC_DRAW_TILE_SIZE_STRATEGY_H

#include <ogc/draw/export.h>
#include <cstddef>

namespace ogc {
namespace draw {

struct TileConfig {
    int tileSize = 256;
    int overlap = 0;
    int maxTilesInMemory = 100;
};

enum class TileStrategy {
    kFixed,         
    kAdaptive,      
    kMemoryAware,   
    kGPUCapability  
};

class OGC_DRAW_API TileSizeStrategy {
public:
    static TileConfig CalculateOptimalConfig(
        int totalWidth,
        int totalHeight,
        TileStrategy strategy = TileStrategy::kAdaptive,
        size_t availableMemory = 0
    );
    
    static TileConfig GetFixedConfig(int tileSize);
    static TileConfig GetAdaptiveConfig(int totalWidth, int totalHeight);
    static TileConfig GetMemoryAwareConfig(int totalWidth, int totalHeight, size_t availableMemory);
    static TileConfig GetGPUCapabilityConfig(int totalWidth, int totalHeight);
    
    static int CalculateTileCount(int totalSize, int tileSize);
    static int CalculateOptimalTileSize(int totalWidth, int totalHeight, int targetTileCount);
    
private:
    static size_t EstimateMemoryUsage(int tileSize, int channels = 4);
    static int GetMaxTextureSize();
    
    static const int MIN_TILE_SIZE = 64;
    static const int MAX_TILE_SIZE = 4096;
    static const int DEFAULT_TILE_SIZE = 256;
    static const int DEFAULT_MAX_TILES = 100;
};

} 
} 

#endif 
