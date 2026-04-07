#include <gtest/gtest.h>
#include <ogc/draw/color.h>
#include <ogc/cache/tile/tile_key.h>
#include <ogc/cache/tile/tile_cache.h>
#include <ogc/cache/tile/memory_tile_cache.h>
#include <ogc/draw/draw_style.h>
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_result.h>
#include <ogc/draw/transform_matrix.h>
#include "ogc/graph/render/render_queue.h"
#include "ogc/graph/render/render_task.h"
#include "ogc/envelope.h"

#include <limits>
#include <memory>
#include <vector>
#include <cstdint>

using namespace ogc::graph;
using ogc::Envelope;
using ogc::draw::Color;
using ogc::cache::TileKey;
using ogc::cache::MemoryTileCache;
using ogc::cache::TileData;
using ogc::draw::DrawStyle;
using ogc::draw::TransformMatrix;
using ogc::draw::DrawResult;

class ColorBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ColorBoundaryTest, MinRGBValues) {
    Color color(0, 0, 0);
    EXPECT_EQ(color.GetRed(), 0);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
    EXPECT_EQ(color.GetAlpha(), 255);
}

TEST_F(ColorBoundaryTest, MaxRGBValues) {
    Color color(255, 255, 255);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 255);
    EXPECT_EQ(color.GetBlue(), 255);
}

TEST_F(ColorBoundaryTest, MinAlphaValue) {
    Color color(128, 128, 128, 0);
    EXPECT_EQ(color.GetAlpha(), 0);
    EXPECT_TRUE(color.IsTransparent());
}

TEST_F(ColorBoundaryTest, MaxAlphaValue) {
    Color color(128, 128, 128, 255);
    EXPECT_EQ(color.GetAlpha(), 255);
    EXPECT_TRUE(color.IsOpaque());
}

TEST_F(ColorBoundaryTest, ZeroLighter) {
    Color black(0, 0, 0);
    Color lighter = black.Lighter(2.0);
    EXPECT_GE(lighter.GetRed(), 0);
    EXPECT_GE(lighter.GetGreen(), 0);
    EXPECT_GE(lighter.GetBlue(), 0);
}

TEST_F(ColorBoundaryTest, MaxDarker) {
    Color white(255, 255, 255);
    Color darker = white.Darker(0.5);
    EXPECT_LE(darker.GetRed(), 255);
    EXPECT_LE(darker.GetGreen(), 255);
    EXPECT_LE(darker.GetBlue(), 255);
}

class TileKeyBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TileKeyBoundaryTest, MinZoomLevel) {
    TileKey key(0, 0, 0);
    EXPECT_EQ(key.z, 0);
    EXPECT_EQ(key.x, 0);
    EXPECT_EQ(key.y, 0);
}

TEST_F(TileKeyBoundaryTest, MaxZoomLevel) {
    TileKey key(0, 0, 22);
    EXPECT_EQ(key.z, 22);
}

TEST_F(TileKeyBoundaryTest, MaxTileIndexAtZoom) {
    for (int z = 0; z <= 10; ++z) {
        int maxTile = (1 << z) - 1;
        TileKey key(maxTile, maxTile, z);
        EXPECT_EQ(key.x, maxTile);
        EXPECT_EQ(key.y, maxTile);
    }
}

TEST_F(TileKeyBoundaryTest, ZeroTileIndex) {
    TileKey key(0, 0, 5);
    EXPECT_EQ(key.x, 0);
    EXPECT_EQ(key.y, 0);
}

TEST_F(TileKeyBoundaryTest, NegativeTileIndex) {
    TileKey key(-1, -1, 5);
    EXPECT_EQ(key.x, -1);
    EXPECT_EQ(key.y, -1);
}

class MemoryTileCacheBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_shared<MemoryTileCache>(1024 * 1024);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::shared_ptr<MemoryTileCache> cache;
    
    std::vector<uint8_t> CreateTestData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(i % 256);
        }
        return data;
    }
};

TEST_F(MemoryTileCacheBoundaryTest, ZeroSizeCache) {
    auto zeroCache = std::make_shared<MemoryTileCache>(0);
    EXPECT_EQ(zeroCache->GetMaxSize(), 0);
    
    TileKey key(0, 0, 0);
    auto data = CreateTestData(100);
    EXPECT_TRUE(zeroCache->PutTile(key, data));
}

TEST_F(MemoryTileCacheBoundaryTest, SingleByteCache) {
    auto tinyCache = std::make_shared<MemoryTileCache>(1);
    
    TileKey key(0, 0, 0);
    auto data = CreateTestData(1);
    EXPECT_TRUE(tinyCache->PutTile(key, data));
    EXPECT_EQ(tinyCache->GetSize(), 1);
}

TEST_F(MemoryTileCacheBoundaryTest, MaxSizeCache) {
    auto largeCache = std::make_shared<MemoryTileCache>(std::numeric_limits<size_t>::max());
    EXPECT_EQ(largeCache->GetMaxSize(), std::numeric_limits<size_t>::max());
}

TEST_F(MemoryTileCacheBoundaryTest, EmptyData) {
    TileKey key(0, 0, 0);
    std::vector<uint8_t> emptyData;
    
    EXPECT_TRUE(cache->PutTile(key, emptyData));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_FALSE(tile.IsValid());
    EXPECT_EQ(tile.data.size(), 0);
}

TEST_F(MemoryTileCacheBoundaryTest, LargeData) {
    TileKey key(0, 0, 0);
    auto largeData = CreateTestData(512 * 1024);
    
    EXPECT_TRUE(cache->PutTile(key, largeData));
    EXPECT_TRUE(cache->HasTile(key));
    
    TileData tile = cache->GetTile(key);
    EXPECT_EQ(tile.data.size(), 512 * 1024);
}

TEST_F(MemoryTileCacheBoundaryTest, CacheFullEviction) {
    auto smallCache = std::make_shared<MemoryTileCache>(100);
    
    for (int i = 0; i < 10; ++i) {
        TileKey key(i, 0, 0);
        auto data = CreateTestData(50);
        smallCache->PutTile(key, data);
    }
    
    EXPECT_LE(smallCache->GetSize(), 100);
}

TEST_F(MemoryTileCacheBoundaryTest, OverwriteExistingTile) {
    TileKey key(0, 0, 0);
    
    auto data1 = CreateTestData(100);
    cache->PutTile(key, data1);
    
    auto data2 = CreateTestData(200);
    cache->PutTile(key, data2);
    
    TileData tile = cache->GetTile(key);
    EXPECT_EQ(tile.data.size(), 200);
    EXPECT_EQ(cache->GetTileCount(), 1);
}

class DrawStyleBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DrawStyleBoundaryTest, ZeroStrokeWidth) {
    DrawStyle style;
    style.pen.width = 0.0;
    EXPECT_DOUBLE_EQ(style.pen.width, 0.0);
}

TEST_F(DrawStyleBoundaryTest, NegativeStrokeWidth) {
    DrawStyle style;
    style.pen.width = -1.0;
    EXPECT_DOUBLE_EQ(style.pen.width, -1.0);
}

TEST_F(DrawStyleBoundaryTest, LargeStrokeWidth) {
    DrawStyle style;
    style.pen.width = 1000.0;
    EXPECT_DOUBLE_EQ(style.pen.width, 1000.0);
}

TEST_F(DrawStyleBoundaryTest, ZeroOpacity) {
    DrawStyle style;
    style.opacity = 0.0;
    EXPECT_DOUBLE_EQ(style.opacity, 0.0);
}

TEST_F(DrawStyleBoundaryTest, FullOpacity) {
    DrawStyle style;
    style.opacity = 1.0;
    EXPECT_DOUBLE_EQ(style.opacity, 1.0);
}

TEST_F(DrawStyleBoundaryTest, OverOpacity) {
    DrawStyle style;
    style.opacity = 2.0;
    EXPECT_DOUBLE_EQ(style.opacity, 2.0);
}

TEST_F(DrawStyleBoundaryTest, NegativeOpacity) {
    DrawStyle style;
    style.opacity = -0.5;
    EXPECT_DOUBLE_EQ(style.opacity, -0.5);
}

class TransformMatrixBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TransformMatrixBoundaryTest, ZeroTranslation) {
    auto transform = TransformMatrix::CreateTranslation(0.0, 0.0);
    EXPECT_DOUBLE_EQ(transform.GetTranslationX(), 0.0);
    EXPECT_DOUBLE_EQ(transform.GetTranslationY(), 0.0);
}

TEST_F(TransformMatrixBoundaryTest, LargeTranslation) {
    auto transform = TransformMatrix::CreateTranslation(1e10, 1e10);
    EXPECT_DOUBLE_EQ(transform.GetTranslationX(), 1e10);
    EXPECT_DOUBLE_EQ(transform.GetTranslationY(), 1e10);
}

TEST_F(TransformMatrixBoundaryTest, NegativeTranslation) {
    auto transform = TransformMatrix::CreateTranslation(-1000.0, -2000.0);
    EXPECT_DOUBLE_EQ(transform.GetTranslationX(), -1000.0);
    EXPECT_DOUBLE_EQ(transform.GetTranslationY(), -2000.0);
}

TEST_F(TransformMatrixBoundaryTest, ZeroScale) {
    auto transform = TransformMatrix::CreateScale(0.0, 0.0);
    EXPECT_DOUBLE_EQ(transform.GetScaleX(), 0.0);
    EXPECT_DOUBLE_EQ(transform.GetScaleY(), 0.0);
}

TEST_F(TransformMatrixBoundaryTest, NegativeScale) {
    auto transform = TransformMatrix::CreateScale(-1.0, -2.0);
    EXPECT_DOUBLE_EQ(transform.GetScaleX(), 1.0);
    EXPECT_DOUBLE_EQ(transform.GetScaleY(), 2.0);
}

TEST_F(TransformMatrixBoundaryTest, LargeScale) {
    auto transform = TransformMatrix::CreateScale(1e6, 1e6);
    EXPECT_DOUBLE_EQ(transform.GetScaleX(), 1e6);
    EXPECT_DOUBLE_EQ(transform.GetScaleY(), 1e6);
}

TEST_F(TransformMatrixBoundaryTest, ZeroRotation) {
    auto transform = TransformMatrix::CreateRotation(0.0);
    EXPECT_DOUBLE_EQ(transform.GetRotation(), 0.0);
}

TEST_F(TransformMatrixBoundaryTest, FullRotation) {
    auto transform = TransformMatrix::CreateRotation(2 * 3.14159265358979323846);
}

TEST_F(TransformMatrixBoundaryTest, NegativeRotation) {
    auto transform = TransformMatrix::CreateRotation(-3.14159265358979323846 / 2);
}

class RenderQueueBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        queue = RenderQueue::Create();
    }
    
    void TearDown() override {
        queue.reset();
    }
    
    RenderQueuePtr queue;
};

class BoundaryRenderTask : public RenderTask {
public:
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<BoundaryRenderTask> Create() {
        return std::make_shared<BoundaryRenderTask>();
    }
};

TEST_F(RenderQueueBoundaryTest, ZeroMaxSize) {
    queue->SetMaxSize(0);
    EXPECT_EQ(queue->GetMaxSize(), 0);
    
    auto task = BoundaryRenderTask::Create();
    EXPECT_FALSE(queue->Enqueue(task));
}

TEST_F(RenderQueueBoundaryTest, SingleMaxSize) {
    queue->SetMaxSize(1);
    
    auto task1 = BoundaryRenderTask::Create();
    EXPECT_TRUE(queue->Enqueue(task1));
    
    auto task2 = BoundaryRenderTask::Create();
    EXPECT_FALSE(queue->Enqueue(task2));
}

TEST_F(RenderQueueBoundaryTest, LargeMaxSize) {
    queue->SetMaxSize(100000);
    EXPECT_EQ(queue->GetMaxSize(), 100000);
}

TEST_F(RenderQueueBoundaryTest, EmptyQueueDequeue) {
    auto task = queue->Dequeue();
    EXPECT_EQ(task, nullptr);
}

TEST_F(RenderQueueBoundaryTest, EmptyQueuePeek) {
    auto task = queue->Peek();
    EXPECT_EQ(task, nullptr);
}

TEST_F(RenderQueueBoundaryTest, EmptyQueueClear) {
    queue->Clear();
    EXPECT_TRUE(queue->IsEmpty());
}

class EnvelopeBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EnvelopeBoundaryTest, ZeroSize) {
    Envelope env(0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(env.GetWidth(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetHeight(), 0.0);
}

TEST_F(EnvelopeBoundaryTest, NegativeSize) {
    Envelope env(100, 100, 0, 0);
    EXPECT_DOUBLE_EQ(env.GetWidth(), -100.0);
    EXPECT_DOUBLE_EQ(env.GetHeight(), -100.0);
}

TEST_F(EnvelopeBoundaryTest, LargeCoordinates) {
    Envelope env(0, 0, 1e10, 1e10);
    EXPECT_DOUBLE_EQ(env.GetWidth(), 1e10);
    EXPECT_DOUBLE_EQ(env.GetHeight(), 1e10);
}

TEST_F(EnvelopeBoundaryTest, NegativeCoordinates) {
    Envelope env(-1000, -2000, 1000, 2000);
    EXPECT_DOUBLE_EQ(env.GetWidth(), 2000.0);
    EXPECT_DOUBLE_EQ(env.GetHeight(), 4000.0);
}

TEST_F(EnvelopeBoundaryTest, MinMaxValues) {
    Envelope env(std::numeric_limits<double>::min(), 
                 std::numeric_limits<double>::min(),
                 std::numeric_limits<double>::max(), 
                 std::numeric_limits<double>::max());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

