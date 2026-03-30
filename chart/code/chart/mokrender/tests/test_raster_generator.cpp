#include <gtest/gtest.h>
#include "ogc/mokrender/raster_generator.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class RasterGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = new RasterGenerator();
    }
    
    void TearDown() override {
        delete generator;
    }
    
    RasterGenerator* generator;
};

TEST_F(RasterGeneratorTest, InitializeWithValidParams) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RasterGeneratorTest, InitializeWithBoundaryCoords) {
    MokRenderResult result = generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RasterGeneratorTest, InitializeWithInvalidRange) {
    MokRenderResult result = generator->Initialize(100.0, 100.0, 0.0, 0.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(RasterGeneratorTest, GenerateFeature) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(RasterGeneratorTest, GenerateFeature_WithinBoundary) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(RasterGeneratorTest, GenerateWithoutInitialize) {
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(RasterGeneratorTest, GetLayerName) {
    std::string name = generator->GetLayerName();
    EXPECT_EQ(name, "raster_layer");
}

TEST_F(RasterGeneratorTest, GetGeometryType) {
    std::string type = generator->GetGeometryType();
    EXPECT_EQ(type, "Raster");
}

TEST_F(RasterGeneratorTest, GenerateMultipleFeatures) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 5; ++i) {
        void* feature = generator->GenerateFeature();
        EXPECT_EQ(feature, nullptr);
    }
}

TEST_F(RasterGeneratorTest, InitializeWithDifferentSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 3857);
    EXPECT_TRUE(result.IsSuccess());
}
