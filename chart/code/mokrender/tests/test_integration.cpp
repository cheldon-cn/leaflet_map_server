#include <gtest/gtest.h>
#include "ogc/mokrender/data_generator.h"
#include "ogc/mokrender/database_manager.h"
#include "ogc/mokrender/spatial_query_engine.h"
#include "ogc/mokrender/render_context.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.pointCount = 5;
        config.lineCount = 5;
        config.polygonCount = 5;
        config.annotationCount = 5;
        config.rasterCount = 5;
        config.minX = 0.0;
        config.minY = 0.0;
        config.maxX = 100.0;
        config.maxY = 100.0;
        config.srid = 4326;
    }
    
    DataGeneratorConfig config;
};

TEST_F(IntegrationTest, FullWorkflow) {
    IDataGenerator* generator = IDataGenerator::Create();
    ASSERT_NE(generator, nullptr);
    
    MokRenderResult result = generator->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
    
    result = generator->Generate();
    EXPECT_TRUE(result.IsSuccess());
    
    EXPECT_EQ(generator->GetGeneratedCount(), 25);
    
    delete generator;
}

TEST_F(IntegrationTest, DatabaseWorkflow) {
    DatabaseManager dbManager;
    
    MokRenderResult result = dbManager.Initialize("test_integration.db");
    EXPECT_TRUE(result.IsSuccess());
    
    result = dbManager.CreateTable("point_layer", "Point");
    EXPECT_TRUE(result.IsSuccess());
    
    result = dbManager.CreateTable("line_layer", "LineString");
    EXPECT_TRUE(result.IsSuccess());
    
    result = dbManager.CreateTable("polygon_layer", "Polygon");
    EXPECT_TRUE(result.IsSuccess());
    
    dbManager.Close();
}

TEST_F(IntegrationTest, SpatialQueryWorkflow) {
    ISpatialQueryEngine* engine = ISpatialQueryEngine::Create();
    ASSERT_NE(engine, nullptr);
    
    MokRenderResult result = engine->Initialize("test_integration.db");
    EXPECT_TRUE(result.IsSuccess());
    
    std::vector<void*> results = engine->QueryByExtent(0, 0, 50, 50, "point_layer");
    
    engine->Close();
    delete engine;
}

TEST_F(IntegrationTest, RenderContextWorkflow) {
    RenderContext context;
    
    RenderConfig renderConfig;
    renderConfig.outputWidth = 800;
    renderConfig.outputHeight = 600;
    renderConfig.dpi = 96.0;
    
    MokRenderResult result = context.Initialize(renderConfig);
    EXPECT_TRUE(result.IsSuccess());
    
    context.SetExtent(0.0, 0.0, 100.0, 100.0);
    
    double minX, minY, maxX, maxY;
    context.GetExtent(minX, minY, maxX, maxY);
    
    EXPECT_DOUBLE_EQ(minX, 0.0);
    EXPECT_DOUBLE_EQ(maxX, 100.0);
}
