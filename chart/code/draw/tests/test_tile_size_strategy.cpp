#include <gtest/gtest.h>
#include <ogc/draw/tile_size_strategy.h>

class TileSizeStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(TileSizeStrategyTest, FixedConfig) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetFixedConfig(256);
    
    EXPECT_EQ(config.tileSize, 256);
    EXPECT_EQ(config.overlap, 0);
    EXPECT_EQ(config.maxTilesInMemory, 100);
}

TEST_F(TileSizeStrategyTest, FixedConfigSmallSize) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetFixedConfig(32);
    
    EXPECT_EQ(config.tileSize, 64);
}

TEST_F(TileSizeStrategyTest, FixedConfigLargeSize) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetFixedConfig(8192);
    
    EXPECT_EQ(config.tileSize, 4096);
}

TEST_F(TileSizeStrategyTest, AdaptiveConfigSmall) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetAdaptiveConfig(512, 512);
    
    EXPECT_EQ(config.tileSize, 256);
}

TEST_F(TileSizeStrategyTest, AdaptiveConfigMedium) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetAdaptiveConfig(2048, 2048);
    
    EXPECT_EQ(config.tileSize, 512);
}

TEST_F(TileSizeStrategyTest, AdaptiveConfigLarge) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetAdaptiveConfig(8192, 8192);
    
    EXPECT_EQ(config.tileSize, 1024);
}

TEST_F(TileSizeStrategyTest, AdaptiveConfigVeryLarge) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetAdaptiveConfig(32768, 32768);
    
    EXPECT_EQ(config.tileSize, 2048);
}

TEST_F(TileSizeStrategyTest, MemoryAwareConfig) {
    size_t availableMemory = 100 * 1024 * 1024;
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetMemoryAwareConfig(4096, 4096, availableMemory);
    
    EXPECT_GE(config.tileSize, 64);
    EXPECT_LE(config.tileSize, 4096);
    EXPECT_GT(config.maxTilesInMemory, 0);
}

TEST_F(TileSizeStrategyTest, MemoryAwareConfigZeroMemory) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetMemoryAwareConfig(2048, 2048, 0);
    
    EXPECT_EQ(config.tileSize, 512);
}

TEST_F(TileSizeStrategyTest, GPUCapabilityConfig) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetGPUCapabilityConfig(4096, 4096);
    
    EXPECT_GE(config.tileSize, 64);
    EXPECT_LE(config.tileSize, 4096);
}

TEST_F(TileSizeStrategyTest, GPUCapabilityConfigLarge) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetGPUCapabilityConfig(16384, 16384);
    
    EXPECT_GE(config.tileSize, 64);
    EXPECT_LE(config.tileSize, 4096);
}

TEST_F(TileSizeStrategyTest, CalculateTileCount) {
    EXPECT_EQ(ogc::draw::TileSizeStrategy::CalculateTileCount(256, 256), 1);
    EXPECT_EQ(ogc::draw::TileSizeStrategy::CalculateTileCount(512, 256), 2);
    EXPECT_EQ(ogc::draw::TileSizeStrategy::CalculateTileCount(257, 256), 2);
    EXPECT_EQ(ogc::draw::TileSizeStrategy::CalculateTileCount(1000, 256), 4);
}

TEST_F(TileSizeStrategyTest, CalculateOptimalTileSize) {
    int tileSize = ogc::draw::TileSizeStrategy::CalculateOptimalTileSize(1024, 1024, 4);
    
    EXPECT_GE(tileSize, 64);
    EXPECT_LE(tileSize, 4096);
}

TEST_F(TileSizeStrategyTest, CalculateOptimalConfigDefault) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::CalculateOptimalConfig(
        2048, 2048, ogc::draw::TileStrategy::kAdaptive, 0);
    
    EXPECT_EQ(config.tileSize, 512);
}

TEST_F(TileSizeStrategyTest, CalculateOptimalConfigFixed) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::CalculateOptimalConfig(
        2048, 2048, ogc::draw::TileStrategy::kFixed, 0);
    
    EXPECT_EQ(config.tileSize, 256);
}

TEST_F(TileSizeStrategyTest, CalculateOptimalConfigMemoryAware) {
    size_t memory = 50 * 1024 * 1024;
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::CalculateOptimalConfig(
        2048, 2048, ogc::draw::TileStrategy::kMemoryAware, memory);
    
    EXPECT_GE(config.tileSize, 64);
    EXPECT_LE(config.tileSize, 4096);
}

TEST_F(TileSizeStrategyTest, TileSizeIsPowerOf2) {
    for (int size = 64; size <= 4096; size *= 2) {
        ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetFixedConfig(size);
        EXPECT_TRUE((config.tileSize & (config.tileSize - 1)) == 0);
    }
}

TEST_F(TileSizeStrategyTest, AdaptiveConfigNonSquare) {
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetAdaptiveConfig(4096, 1024);
    
    EXPECT_EQ(config.tileSize, 512);
}

TEST_F(TileSizeStrategyTest, MemoryAwareConfigSmallMemory) {
    size_t smallMemory = 1024 * 1024;
    ogc::draw::TileConfig config = ogc::draw::TileSizeStrategy::GetMemoryAwareConfig(4096, 4096, smallMemory);
    
    EXPECT_GE(config.tileSize, 64);
    EXPECT_GT(config.maxTilesInMemory, 0);
}
