#include <ogc/draw/tile_size_strategy.h>
#include <algorithm>
#include <cmath>

namespace ogc {
namespace draw {

TileConfig TileSizeStrategy::CalculateOptimalConfig(
    int totalWidth,
    int totalHeight,
    TileStrategy strategy,
    size_t availableMemory)
{
    switch (strategy) {
        case TileStrategy::kFixed:
            return GetFixedConfig(DEFAULT_TILE_SIZE);
        case TileStrategy::kAdaptive:
            return GetAdaptiveConfig(totalWidth, totalHeight);
        case TileStrategy::kMemoryAware:
            return GetMemoryAwareConfig(totalWidth, totalHeight, availableMemory);
        case TileStrategy::kGPUCapability:
            return GetGPUCapabilityConfig(totalWidth, totalHeight);
        default:
            return GetAdaptiveConfig(totalWidth, totalHeight);
    }
}

TileConfig TileSizeStrategy::GetFixedConfig(int tileSize)
{
    TileConfig config;
    config.tileSize = std::max(MIN_TILE_SIZE, std::min(MAX_TILE_SIZE, tileSize));
    config.overlap = 0;
    config.maxTilesInMemory = DEFAULT_MAX_TILES;
    return config;
}

TileConfig TileSizeStrategy::GetAdaptiveConfig(int totalWidth, int totalHeight)
{
    TileConfig config;
    
    int maxDim = std::max(totalWidth, totalHeight);
    
    if (maxDim <= 512) {
        config.tileSize = 256;
    } else if (maxDim <= 1024) {
        config.tileSize = 256;
    } else if (maxDim <= 2048) {
        config.tileSize = 512;
    } else if (maxDim <= 4096) {
        config.tileSize = 512;
    } else if (maxDim <= 8192) {
        config.tileSize = 1024;
    } else if (maxDim <= 16384) {
        config.tileSize = 1024;
    } else {
        config.tileSize = 2048;
    }
    
    int tileCountX = CalculateTileCount(totalWidth, config.tileSize);
    int tileCountY = CalculateTileCount(totalHeight, config.tileSize);
    int totalTiles = tileCountX * tileCountY;
    
    config.maxTilesInMemory = std::min(DEFAULT_MAX_TILES, totalTiles);
    config.overlap = 0;
    
    return config;
}

TileConfig TileSizeStrategy::GetMemoryAwareConfig(
    int totalWidth,
    int totalHeight,
    size_t availableMemory)
{
    TileConfig config;
    
    if (availableMemory == 0) {
        return GetAdaptiveConfig(totalWidth, totalHeight);
    }
    
    size_t targetMemoryPerTile = availableMemory / 4;
    
    int optimalTileSize = static_cast<int>(std::sqrt(
        static_cast<double>(targetMemoryPerTile) / 4.0));
    
    optimalTileSize = std::max(MIN_TILE_SIZE, std::min(MAX_TILE_SIZE, optimalTileSize));
    
    int powerOf2 = 64;
    while (powerOf2 * 2 <= optimalTileSize) {
        powerOf2 *= 2;
    }
    config.tileSize = powerOf2;
    
    size_t tileMemory = EstimateMemoryUsage(config.tileSize);
    config.maxTilesInMemory = static_cast<int>(availableMemory / tileMemory);
    config.maxTilesInMemory = std::max(1, std::min(1000, config.maxTilesInMemory));
    
    config.overlap = 0;
    
    return config;
}

TileConfig TileSizeStrategy::GetGPUCapabilityConfig(int totalWidth, int totalHeight)
{
    TileConfig config;
    
    int maxTextureSize = GetMaxTextureSize();
    
    int maxDim = std::max(totalWidth, totalHeight);
    
    if (maxDim <= maxTextureSize) {
        config.tileSize = std::min(maxTextureSize, static_cast<int>(maxDim * 0.5));
        config.tileSize = std::max(MIN_TILE_SIZE, config.tileSize);
    } else {
        int tileCount = static_cast<int>(std::ceil(
            static_cast<double>(maxDim) / maxTextureSize));
        config.tileSize = static_cast<int>(std::ceil(
            static_cast<double>(maxDim) / tileCount));
    }
    
    int powerOf2 = 64;
    while (powerOf2 * 2 <= config.tileSize) {
        powerOf2 *= 2;
    }
    config.tileSize = powerOf2;
    
    config.tileSize = std::max(MIN_TILE_SIZE, std::min(MAX_TILE_SIZE, config.tileSize));
    
    int tileCountX = CalculateTileCount(totalWidth, config.tileSize);
    int tileCountY = CalculateTileCount(totalHeight, config.tileSize);
    config.maxTilesInMemory = std::min(DEFAULT_MAX_TILES, tileCountX * tileCountY);
    
    config.overlap = 0;
    
    return config;
}

int TileSizeStrategy::CalculateTileCount(int totalSize, int tileSize)
{
    if (tileSize <= 0) return 1;
    return static_cast<int>(std::ceil(
        static_cast<double>(totalSize) / tileSize));
}

int TileSizeStrategy::CalculateOptimalTileSize(
    int totalWidth,
    int totalHeight,
    int targetTileCount)
{
    if (targetTileCount <= 0) return DEFAULT_TILE_SIZE;
    
    int totalArea = totalWidth * totalHeight;
    int tileArea = totalArea / targetTileCount;
    
    int tileSize = static_cast<int>(std::sqrt(static_cast<double>(tileArea)));
    
    int powerOf2 = 64;
    while (powerOf2 * 2 <= tileSize) {
        powerOf2 *= 2;
    }
    
    return std::max(MIN_TILE_SIZE, std::min(MAX_TILE_SIZE, powerOf2));
}

size_t TileSizeStrategy::EstimateMemoryUsage(int tileSize, int channels)
{
    return static_cast<size_t>(tileSize) * tileSize * channels;
}

int TileSizeStrategy::GetMaxTextureSize()
{
    return 4096;
}

}
}
