#include <gtest/gtest.h>
#include <chrono>
#include "ogc/mokrender/point_generator.h"
#include "ogc/mokrender/line_generator.h"
#include "ogc/mokrender/polygon_generator.h"
#include "ogc/mokrender/annotation_generator.h"
#include "ogc/mokrender/raster_generator.h"
#include "ogc/mokrender/database_manager.h"
#include "ogc/mokrender/spatial_query_engine.h"

using namespace ogc::mokrender;

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(PerformanceTest, PointGenerator_LargeAmount) {
    PointGenerator generator;
    generator.Initialize(0.0, 0.0, 1000.0, 1000.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        void* feature = generator.GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(PerformanceTest, LineGenerator_LargeAmount) {
    LineGenerator generator;
    generator.Initialize(0.0, 0.0, 1000.0, 1000.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        void* feature = generator.GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500);
}

TEST_F(PerformanceTest, PolygonGenerator_LargeAmount) {
    PolygonGenerator generator;
    generator.Initialize(0.0, 0.0, 1000.0, 1000.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        void* feature = generator.GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500);
}

TEST_F(PerformanceTest, AnnotationGenerator_LargeAmount) {
    AnnotationGenerator generator;
    generator.Initialize(0.0, 0.0, 1000.0, 1000.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        void* feature = generator.GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(PerformanceTest, DatabaseManager_BatchInsert) {
    std::string dbName = "test_perf_" + std::to_string(rand()) + ".db";
    
    DatabaseManager dbManager;
    dbManager.Initialize(dbName);
    dbManager.CreateTable("points", "Point");
    
    PointGenerator generator;
    generator.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        void* feature = generator.GenerateFeature();
        dbManager.InsertFeature("points", feature);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 5000);
    
    dbManager.Close();
}

TEST_F(PerformanceTest, SpatialQuery_LargeDataset) {
    std::string dbName = "test_perf_query_" + std::to_string(rand()) + ".db";
    
    DatabaseManager dbManager;
    dbManager.Initialize(dbName);
    dbManager.CreateTable("points", "Point");
    
    PointGenerator generator;
    generator.Initialize(0.0, 0.0, 1000.0, 1000.0, 4326);
    
    for (int i = 0; i < 500; ++i) {
        void* feature = generator.GenerateFeature();
        dbManager.InsertFeature("points", feature);
    }
    
    dbManager.Close();
    
    SpatialQueryEngine engine;
    engine.Initialize(dbName);
    
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<void*> results = engine.QueryByExtent(0, 0, 500, 500, "points");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
    
    engine.Close();
}

TEST_F(PerformanceTest, MixedGenerators_AllTypes) {
    PointGenerator pointGen;
    LineGenerator lineGen;
    PolygonGenerator polyGen;
    AnnotationGenerator annotGen;
    
    pointGen.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    lineGen.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    polyGen.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    annotGen.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        void* p = pointGen.GenerateFeature();
        void* l = lineGen.GenerateFeature();
        void* pg = polyGen.GenerateFeature();
        void* a = annotGen.GenerateFeature();
        
        ASSERT_NE(p, nullptr);
        ASSERT_NE(l, nullptr);
        ASSERT_NE(pg, nullptr);
        ASSERT_NE(a, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500);
}
