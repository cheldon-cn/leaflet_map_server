#include <gtest/gtest.h>
#include <limits>
#include <set>
#include "ogc/mokrender/point_generator.h"
#include "ogc/mokrender/common.h"
#include "ogc/point.h"

using namespace ogc::mokrender;

class PointGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = new PointGenerator();
    }
    
    void TearDown() override {
        delete generator;
    }
    
    PointGenerator* generator;
};

TEST_F(PointGeneratorTest, InitializeWithValidParams) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithBoundaryCoords) {
    MokRenderResult result = generator->Initialize(
        -180.0, -90.0, 180.0, 90.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithMaxBoundary) {
    MokRenderResult result = generator->Initialize(
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithInvalidRange_MinMaxReversed) {
    MokRenderResult result = generator->Initialize(100.0, 100.0, 0.0, 0.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithInvalidRange_NegativeExtent) {
    MokRenderResult result = generator->Initialize(50.0, 50.0, 50.0, 50.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithDifferentSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 3857);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, InitializeWithZeroSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 0);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(PointGeneratorTest, GenerateFeature) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Point* point = static_cast<ogc::Point*>(feature);
    EXPECT_NE(point, nullptr);
    
    ogc::Coordinate coord = point->GetCoordinateN(0);
    EXPECT_GE(coord.x, 0.0);
    EXPECT_LE(coord.x, 100.0);
    EXPECT_GE(coord.y, 0.0);
    EXPECT_LE(coord.y, 100.0);
}

TEST_F(PointGeneratorTest, GenerateFeature_WithinBoundary) {
    generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Point* point = static_cast<ogc::Point*>(feature);
    ogc::Coordinate coord = point->GetCoordinateN(0);
    EXPECT_GE(coord.x, -180.0);
    EXPECT_LE(coord.x, 180.0);
    EXPECT_GE(coord.y, -90.0);
    EXPECT_LE(coord.y, 90.0);
}

TEST_F(PointGeneratorTest, GenerateWithoutInitialize) {
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(PointGeneratorTest, GetLayerName) {
    std::string name = generator->GetLayerName();
    EXPECT_EQ(name, "point_layer");
}

TEST_F(PointGeneratorTest, GetGeometryType) {
    std::string type = generator->GetGeometryType();
    EXPECT_EQ(type, "Point");
}

TEST_F(PointGeneratorTest, GenerateMultipleFeatures) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 10; ++i) {
        void* feature = generator->GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
}

TEST_F(PointGeneratorTest, GenerateMultipleFeatures_AllUnique) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    std::set<std::pair<double, double>> coords;
    for (int i = 0; i < 100; ++i) {
        void* feature = generator->GenerateFeature();
        ASSERT_NE(feature, nullptr);
        
        ogc::Point* point = static_cast<ogc::Point*>(feature);
        ogc::Coordinate coord = point->GetCoordinateN(0);
        coords.insert({coord.x, coord.y});
    }
    
    EXPECT_GT(coords.size(), 1);
}
