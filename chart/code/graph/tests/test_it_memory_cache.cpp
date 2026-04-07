#include <gtest/gtest.h>
#include <ogc/cache/tile/memory_tile_cache.h>
#include <ogc/cache/tile/tile_key.h>
#include <ogc/draw/tile_device.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_context.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::cache;
using ogc::Envelope;

class IntegrationMemoryCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_cache = MemoryTileCache::Create(100 * 1024 * 1024);
        ASSERT_NE(m_cache, nullptr);
        
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
    }
    
    void TearDown() override {
        m_cache.reset();
        m_device.reset();
    }
    
    std::vector<uint8_t> CreateTestData(size_t size = 256 * 256 * 4) {
        return std::vector<uint8_t>(size, 128);
    }
    
    std::shared_ptr<MemoryTileCache> m_cache;
    std::shared_ptr<RasterImageDevice> m_device;
};

TEST_F(IntegrationMemoryCacheTest, CreateCache) {
    EXPECT_NE(m_cache, nullptr);
}

TEST_F(IntegrationMemoryCacheTest, PutAndGetTile) {
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    
    m_cache->PutTile(key, data);
    EXPECT_EQ(m_cache->GetTileCount(), 1);
    
    TileData retrieved = m_cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
    EXPECT_EQ(retrieved.data.size(), data.size());
}

TEST_F(IntegrationMemoryCacheTest, CacheMiss) {
    TileKey key(99, 99, 99);
    
    TileData retrieved = m_cache->GetTile(key);
    EXPECT_FALSE(retrieved.IsValid());
}

TEST_F(IntegrationMemoryCacheTest, RemoveTile) {
    TileKey key(1, 2, 3);
    auto data = CreateTestData();
    
    m_cache->PutTile(key, data);
    EXPECT_TRUE(m_cache->HasTile(key));
    
    m_cache->RemoveTile(key);
    EXPECT_FALSE(m_cache->HasTile(key));
}

TEST_F(IntegrationMemoryCacheTest, ClearCache) {
    for (int i = 0; i < 10; ++i) {
        TileKey key(i, 0, 5);
        m_cache->PutTile(key, CreateTestData());
    }
    
    EXPECT_EQ(m_cache->GetTileCount(), 10);
    
    m_cache->Clear();
    EXPECT_EQ(m_cache->GetTileCount(), 0);
}

TEST_F(IntegrationMemoryCacheTest, HasTile) {
    TileKey key(5, 5, 5);
    
    EXPECT_FALSE(m_cache->HasTile(key));
    
    m_cache->PutTile(key, CreateTestData());
    EXPECT_TRUE(m_cache->HasTile(key));
}

TEST_F(IntegrationMemoryCacheTest, MaxSize) {
    m_cache->SetMaxSize(1024);
    EXPECT_EQ(m_cache->GetMaxSize(), 1024);
}

TEST_F(IntegrationMemoryCacheTest, TileCount) {
    EXPECT_EQ(m_cache->GetTileCount(), 0);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        m_cache->PutTile(key, CreateTestData());
    }
    
    EXPECT_EQ(m_cache->GetTileCount(), 5);
}

TEST_F(IntegrationMemoryCacheTest, MultipleZoomLevels) {
    int totalTiles = 0;
    for (int z = 0; z <= 3; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                TileKey key(x, y, z);
                m_cache->PutTile(key, CreateTestData());
                totalTiles++;
            }
        }
    }
    
    EXPECT_EQ(m_cache->GetTileCount(), totalTiles);
}

TEST_F(IntegrationMemoryCacheTest, RenderAndCache) {
    auto context = DrawContext::Create(m_device.get());
    ASSERT_NE(context, nullptr);
    
    context->Begin();
    context->Clear(Color::White());
    
    DrawStyle style;
    style.pen = Pen(Color::Blue(), 2.0);
    context->SetStyle(style);
    
    DrawResult result = context->DrawRect(10, 10, 236, 236);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    context->End();
    
    TileKey key(0, 0, 0);
    auto data = CreateTestData();
    m_cache->PutTile(key, data);
    
    TileData retrieved = m_cache->GetTile(key);
    EXPECT_TRUE(retrieved.IsValid());
}

TEST_F(IntegrationMemoryCacheTest, SetGetName) {
    m_cache->SetName("test_memory_cache");
    EXPECT_EQ(m_cache->GetName(), "test_memory_cache");
}

TEST_F(IntegrationMemoryCacheTest, SetEnabled) {
    m_cache->SetEnabled(true);
    EXPECT_TRUE(m_cache->IsEnabled());
    
    m_cache->SetEnabled(false);
    EXPECT_FALSE(m_cache->IsEnabled());
}

TEST_F(IntegrationMemoryCacheTest, SetExpirationTime) {
    m_cache->SetExpirationTime(3600);
    EXPECT_EQ(m_cache->GetExpirationTime(), 3600);
}

TEST_F(IntegrationMemoryCacheTest, GetSize) {
    auto data = CreateTestData(1024);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        m_cache->PutTile(key, data);
    }
    
    EXPECT_GT(m_cache->GetSize(), 0);
}

TEST_F(IntegrationMemoryCacheTest, Cleanup) {
    m_cache->SetExpirationTime(1);
    
    for (int i = 0; i < 5; ++i) {
        TileKey key(i, 0, 3);
        m_cache->PutTile(key, CreateTestData());
    }
    
    m_cache->Cleanup();
}
