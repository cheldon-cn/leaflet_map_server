#include <gtest/gtest.h>
#include <limits>
#include "ogc/mokrender/annotation_generator.h"
#include "ogc/mokrender/common.h"
#include "ogc/geom/point.h"

using namespace ogc::mokrender;

class AnnotationGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = new AnnotationGenerator();
    }
    
    void TearDown() override {
        delete generator;
    }
    
    AnnotationGenerator* generator;
};

TEST_F(AnnotationGeneratorTest, InitializeWithValidParams) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(AnnotationGeneratorTest, InitializeWithBoundaryCoords) {
    MokRenderResult result = generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(AnnotationGeneratorTest, InitializeWithMaxBoundary) {
    MokRenderResult result = generator->Initialize(
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(AnnotationGeneratorTest, InitializeWithDifferentSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 3857);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(AnnotationGeneratorTest, GenerateFeature) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Point* point = static_cast<ogc::Point*>(feature);
    EXPECT_NE(point, nullptr);
}

TEST_F(AnnotationGeneratorTest, GenerateFeature_WithinBoundary) {
    generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::Point* point = static_cast<ogc::Point*>(feature);
    EXPECT_NE(point, nullptr);
    
    ogc::Coordinate coord = point->GetCoordinateN(0);
    EXPECT_GE(coord.x, -180.0);
    EXPECT_LE(coord.x, 180.0);
    EXPECT_GE(coord.y, -90.0);
    EXPECT_LE(coord.y, 90.0);
}

TEST_F(AnnotationGeneratorTest, GenerateWithoutInitialize) {
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(AnnotationGeneratorTest, GetLayerName) {
    std::string name = generator->GetLayerName();
    EXPECT_EQ(name, "annotation_layer");
}

TEST_F(AnnotationGeneratorTest, GetGeometryType) {
    std::string type = generator->GetGeometryType();
    EXPECT_EQ(type, "Point");
}

TEST_F(AnnotationGeneratorTest, GenerateMultipleFeatures) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 10; ++i) {
        void* feature = generator->GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
}
