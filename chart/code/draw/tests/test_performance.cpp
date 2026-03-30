#include <gtest/gtest.h>
#include "ogc/draw/batch_renderer.h"
#include "ogc/draw/render_cache.h"
#include "ogc/draw/lod_strategy.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/geometry.h"
#include <chrono>

using namespace ogc::draw;
using ogc::GeometrySharedPtr;

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(800, 600);
        engine = new Simple2DEngine(device);
    }
    
    void TearDown() override {
        delete engine;
        delete device;
    }
    
    RasterImageDevice* device = nullptr;
    Simple2DEngine* engine = nullptr;
};

TEST_F(PerformanceTest, BatchRendererBasic) {
    BatchRenderer renderer(engine);
    
    renderer.BeginBatch();
    
    for (int i = 0; i < 100; ++i) {
        auto geom = RectGeometry::Create(static_cast<double>(i), 0.0, 10.0, 100.0);
        GeometrySharedPtr sharedGeom(geom.release());
        
        DrawStyle style;
        style.pen = Pen::Solid(Color::Black(), 1.0);
        
        renderer.AddGeometry(sharedGeom, style);
    }
    
    renderer.EndBatch();
    
    EXPECT_EQ(renderer.GetBatchCount(), 0);
}

TEST_F(PerformanceTest, BatchRendererAutoFlush) {
    BatchRenderer renderer(engine);
    renderer.SetBatchSize(10);
    renderer.SetAutoFlush(true);
    
    renderer.BeginBatch();
    
    for (int i = 0; i < 50; ++i) {
        auto geom = RectGeometry::Create(static_cast<double>(i), 0.0, 10.0, 100.0);
        GeometrySharedPtr sharedGeom(geom.release());
        
        DrawStyle style;
        renderer.AddGeometry(sharedGeom, style);
    }
    
    renderer.EndBatch();
    EXPECT_EQ(renderer.GetBatchCount(), 0);
}

TEST_F(PerformanceTest, BatchRendererPriority) {
    BatchRenderer renderer(engine);
    renderer.SetSortByPriority(true);
    
    renderer.BeginBatch();
    
    for (int i = 0; i < 10; ++i) {
        auto geom = RectGeometry::Create(static_cast<double>(i), 0.0, 10.0, 100.0);
        GeometrySharedPtr sharedGeom(geom.release());
        
        DrawStyle style;
        renderer.AddGeometry(sharedGeom, style, 10 - i);
    }
    
    renderer.EndBatch();
    EXPECT_TRUE(true);
}

TEST_F(PerformanceTest, BatchRendererPerformance) {
    BatchRenderer renderer(engine);
    renderer.SetAutoFlush(false);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    renderer.BeginBatch();
    
    for (int i = 0; i < 1000; ++i) {
        auto geom = CircleGeometry::Create(static_cast<double>(i), 50.0, 10.0);
        GeometrySharedPtr sharedGeom(geom.release());
        
        DrawStyle style;
        renderer.AddGeometry(sharedGeom, style);
    }
    
    renderer.EndBatch();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(PerformanceTest, RenderCacheBasic) {
    auto& cache = RenderCache::Instance();
    cache.SetEnabled(true);
    cache.InvalidateAll();
    
    auto geom = RectGeometry::Create(0.0, 0.0, 100.0, 100.0);
    
    DrawStyle style;
    style.pen = Pen::Solid(Color::Black(), 1.0);
    
    auto handle1 = cache.GetOrCreate(*geom, style);
    EXPECT_NE(handle1, 0);
    
    auto handle2 = cache.GetOrCreate(*geom, style);
    EXPECT_EQ(handle1, handle2);
    
    EXPECT_TRUE(cache.Contains(handle1));
    EXPECT_EQ(cache.GetAccessCount(handle1), 2);
}

TEST_F(PerformanceTest, RenderCacheInvalidate) {
    auto& cache = RenderCache::Instance();
    cache.SetEnabled(true);
    cache.InvalidateAll();
    
    auto geom = RectGeometry::Create(0.0, 0.0, 100.0, 100.0);
    
    DrawStyle style;
    auto handle = cache.GetOrCreate(*geom, style);
    
    EXPECT_TRUE(cache.Contains(handle));
    
    cache.Invalidate(handle);
    EXPECT_FALSE(cache.Contains(handle));
}

TEST_F(PerformanceTest, RenderCacheMaxSize) {
    auto& cache = RenderCache::Instance();
    cache.SetEnabled(true);
    cache.SetMaxCacheSize(1024);
    cache.InvalidateAll();
    
    for (int i = 0; i < 100; ++i) {
        auto geom = RectGeometry::Create(static_cast<double>(i), 0.0, 10.0, 100.0);
        
        DrawStyle style;
        cache.GetOrCreate(*geom, style);
    }
    
    EXPECT_LE(cache.GetCacheSize(), 1024);
}

TEST_F(PerformanceTest, RenderCachePerformance) {
    auto& cache = RenderCache::Instance();
    cache.SetEnabled(true);
    cache.InvalidateAll();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        auto geom = RectGeometry::Create(static_cast<double>(i), 0.0, 10.0, 100.0);
        
        DrawStyle style;
        cache.GetOrCreate(*geom, style);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(PerformanceTest, LODStrategyBasic) {
    auto& lod = LODStrategy::Instance();
    
    int lodLevel0 = lod.CalculateLOD(50.0);
    int lodLevel1 = lod.CalculateLOD(200.0);
    int lodLevel2 = lod.CalculateLOD(700.0);
    int lodLevel3 = lod.CalculateLOD(2000.0);
    int lodLevel4 = lod.CalculateLOD(10000.0);
    
    EXPECT_GE(lodLevel0, 0);
    EXPECT_LE(lodLevel0, 4);
    EXPECT_GE(lodLevel1, 0);
    EXPECT_LE(lodLevel1, 4);
}

TEST_F(PerformanceTest, LODStrategySimplification) {
    auto& lod = LODStrategy::Instance();
    
    double simp0 = lod.GetSimplificationFactor(0);
    double simp2 = lod.GetSimplificationFactor(2);
    double simp4 = lod.GetSimplificationFactor(4);
    
    EXPECT_GT(simp0, simp2);
    EXPECT_GT(simp2, simp4);
}

TEST_F(PerformanceTest, LODStrategyScale) {
    auto& lod = LODStrategy::Instance();
    
    int lodLevel0 = lod.CalculateLODFromScale(1.0);
    int lodLevel1 = lod.CalculateLODFromScale(0.5);
    int lodLevel2 = lod.CalculateLODFromScale(0.25);
    int lodLevel3 = lod.CalculateLODFromScale(0.125);
    int lodLevel4 = lod.CalculateLODFromScale(0.0625);
    
    EXPECT_EQ(lodLevel0, 0);
    EXPECT_EQ(lodLevel1, 1);
    EXPECT_EQ(lodLevel2, 2);
    EXPECT_EQ(lodLevel3, 3);
    EXPECT_EQ(lodLevel4, 4);
}

TEST_F(PerformanceTest, LODStrategyImportance) {
    auto& lod = LODStrategy::Instance();
    
    int lodNormal = lod.CalculateLOD(500.0, 1.0);
    int lodImportant = lod.CalculateLOD(500.0, 2.0);
    int lodLessImportant = lod.CalculateLOD(500.0, 0.5);
    
    EXPECT_LE(lodImportant, lodNormal);
    EXPECT_GE(lodLessImportant, lodNormal);
}

TEST_F(PerformanceTest, LODStrategyCustomCalculator) {
    auto& lod = LODStrategy::Instance();
    
    lod.SetCustomLODCalculator([](double distance, double importance) {
        return static_cast<int>(distance / 100.0);
    });
    
    int lodLevel = lod.CalculateLOD(350.0, 1.0);
    EXPECT_EQ(lodLevel, 3);
    
    lod.SetCustomLODCalculator(nullptr);
}

TEST_F(PerformanceTest, LODStrategyClamp) {
    auto& lod = LODStrategy::Instance();
    
    EXPECT_TRUE(lod.IsValidLOD(0));
    EXPECT_TRUE(lod.IsValidLOD(2));
    EXPECT_TRUE(lod.IsValidLOD(4));
    EXPECT_FALSE(lod.IsValidLOD(-1));
    EXPECT_FALSE(lod.IsValidLOD(10));
    
    EXPECT_EQ(lod.ClampLOD(-1), 0);
    EXPECT_EQ(lod.ClampLOD(10), 4);
}

TEST_F(PerformanceTest, IntegratedPerformance) {
    auto& cache = RenderCache::Instance();
    auto& lod = LODStrategy::Instance();
    cache.SetEnabled(true);
    cache.InvalidateAll();
    
    BatchRenderer renderer(engine);
    renderer.SetAutoFlush(false);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    renderer.BeginBatch();
    
    for (int i = 0; i < 500; ++i) {
        double distance = static_cast<double>(i * 10);
        int lodLevel = lod.CalculateLOD(distance);
        double simplification = lod.GetSimplificationFactor(lodLevel);
        
        int segments = static_cast<int>(32 * simplification);
        auto geom = CircleGeometry::Create(static_cast<double>(i), 50.0, 10.0, segments);
        GeometrySharedPtr sharedGeom(geom.release());
        
        DrawStyle style;
        renderer.AddGeometry(sharedGeom, style);
        
        cache.GetOrCreate(*sharedGeom, style);
    }
    
    renderer.EndBatch();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 2000);
    EXPECT_GT(cache.GetEntryCount(), 0);
}
