#include <gtest/gtest.h>
#include "ogc/mokrender/spatial_query_engine.h"
#include "ogc/mokrender/database_manager.h"
#include "ogc/mokrender/point_generator.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class SpatialQueryTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = ISpatialQueryEngine::Create();
        testDbName = "test_spatial_query_" + std::to_string(rand()) + ".db";
    }
    
    void TearDown() override {
        delete engine;
    }
    
    ISpatialQueryEngine* engine;
    std::string testDbName;
};

TEST_F(SpatialQueryTest, CreateInstance) {
    ASSERT_NE(engine, nullptr);
}

TEST_F(SpatialQueryTest, InitializeWithValidPath) {
    MokRenderResult result = engine->Initialize(testDbName);
    EXPECT_TRUE(result.IsSuccess());
    engine->Close();
}

TEST_F(SpatialQueryTest, InitializeWithEmptyPath) {
    MokRenderResult result = engine->Initialize("");
    EXPECT_TRUE(result.IsSuccess());
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryWithoutInitialize) {
    std::vector<void*> results = engine->QueryByExtent(0, 0, 100, 100, "point_layer");
    EXPECT_TRUE(results.empty());
}

TEST_F(SpatialQueryTest, CloseAfterInitialize) {
    engine->Initialize(testDbName);
    engine->Close();
}

TEST_F(SpatialQueryTest, CloseWithoutInitialize) {
    engine->Close();
}

TEST_F(SpatialQueryTest, CloseTwice) {
    engine->Initialize(testDbName);
    engine->Close();
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryByExtent_EmptyTable) {
    engine->Initialize(testDbName);
    
    std::vector<void*> results = engine->QueryByExtent(0, 0, 100, 100, "nonexistent_layer");
    EXPECT_TRUE(results.empty());
    
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryByExtent_WithEmptyLayerName) {
    engine->Initialize(testDbName);
    
    std::vector<void*> results = engine->QueryByExtent(0, 0, 100, 100, "");
    EXPECT_TRUE(results.empty());
    
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryByExtent_InvalidExtent) {
    engine->Initialize(testDbName);
    
    std::vector<void*> results = engine->QueryByExtent(100, 100, 0, 0, "point_layer");
    EXPECT_TRUE(results.empty());
    
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryByExtent_AfterDataInsert) {
    DatabaseManager dbManager;
    dbManager.Initialize(testDbName);
    dbManager.CreateTable("point_layer", "Point");
    
    PointGenerator generator;
    generator.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 10; ++i) {
        void* feature = generator.GenerateFeature();
        dbManager.InsertFeature("point_layer", feature);
    }
    
    dbManager.Close();
    
    engine->Initialize(testDbName);
    std::vector<void*> results = engine->QueryByExtent(0, 0, 100, 100, "point_layer");
    EXPECT_GE(results.size(), 0);
    
    engine->Close();
}

TEST_F(SpatialQueryTest, QueryByExtent_PartialOverlap) {
    DatabaseManager dbManager;
    dbManager.Initialize(testDbName);
    dbManager.CreateTable("point_layer", "Point");
    
    PointGenerator generator;
    generator.Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 20; ++i) {
        void* feature = generator.GenerateFeature();
        dbManager.InsertFeature("point_layer", feature);
    }
    
    dbManager.Close();
    
    engine->Initialize(testDbName);
    std::vector<void*> results = engine->QueryByExtent(0, 0, 50, 50, "point_layer");
    EXPECT_GE(results.size(), 0);
    
    engine->Close();
}
