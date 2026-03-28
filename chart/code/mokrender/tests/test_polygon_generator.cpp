#include <gtest/gtest.h>
#include <limits>
#include "ogc/mokrender/polygon_generator.h"
#include "ogc/mokrender/common.h"
#include "ogc/polygon.h"

using namespace ogc::mokrender;

class PolygonGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = new PolygonGenerator();
    }
    
    void TearDown() override {
        delete generator;
    }
    
    PolygonGenerator* generator;
};

TEST_F(PolygonGeneratorTest, InitializeWithValidParams) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PolygonGeneratorTest, InitializeWithBoundaryCoords) {
    MokRenderResult result = generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PolygonGeneratorTest, InitializeWithMaxBoundary) {
    MokRenderResult result = generator->Initialize(
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PolygonGeneratorTest, InitializeWithDifferentSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 3857);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PolygonGeneratorTest, GenerateFeature) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Polygon* polygon = static_cast<ogc::Polygon*>(feature);
    EXPECT_NE(polygon, nullptr);
    EXPECT_NE(polygon->GetExteriorRing(), nullptr);
}

TEST_F(PolygonGeneratorTest, GenerateFeature_WithinBoundary) {
    generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Polygon* polygon = static_cast<ogc::Polygon*>(feature);
    EXPECT_NE(polygon, nullptr);
    EXPECT_NE(polygon->GetExteriorRing(), nullptr);
}

TEST_F(PolygonGeneratorTest, GenerateWithoutInitialize) {
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(PolygonGeneratorTest, GetLayerName) {
    std::string name = generator->GetLayerName();
    EXPECT_EQ(name, "polygon_layer");
}

TEST_F(PolygonGeneratorTest, GetGeometryType) {
    std::string type = generator->GetGeometryType();
    EXPECT_EQ(type, "Polygon");
}

TEST_F(PolygonGeneratorTest, GenerateMultipleFeatures) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 10; ++i) {
        void* feature = generator->GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
}
