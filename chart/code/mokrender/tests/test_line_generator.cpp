#include <gtest/gtest.h>
#include <limits>
#include "ogc/mokrender/line_generator.h"
#include "ogc/mokrender/common.h"
#include "ogc/geom/linestring.h"

using namespace ogc::mokrender;

class LineGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = new LineGenerator();
    }
    
    void TearDown() override {
        delete generator;
    }
    
    LineGenerator* generator;
};

TEST_F(LineGeneratorTest, InitializeWithValidParams) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(LineGeneratorTest, InitializeWithBoundaryCoords) {
    MokRenderResult result = generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(LineGeneratorTest, InitializeWithMaxBoundary) {
    MokRenderResult result = generator->Initialize(
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), 4326);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(LineGeneratorTest, InitializeWithDifferentSRID) {
    MokRenderResult result = generator->Initialize(0.0, 0.0, 100.0, 100.0, 3857);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(LineGeneratorTest, GenerateFeature) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::LineString* line = static_cast<ogc::LineString*>(feature);
    EXPECT_NE(line, nullptr);
    EXPECT_GE(line->GetNumPoints(), 2);
}

TEST_F(LineGeneratorTest, GenerateFeature_WithinBoundary) {
    generator->Initialize(-180.0, -90.0, 180.0, 90.0, 4326);
    
    void* feature = generator->GenerateFeature();
    ASSERT_NE(feature, nullptr);
    
    ogc::LineString* line = static_cast<ogc::LineString*>(feature);
    EXPECT_NE(line, nullptr);
    EXPECT_GE(line->GetNumPoints(), 2);
}

TEST_F(LineGeneratorTest, GenerateWithoutInitialize) {
    void* feature = generator->GenerateFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(LineGeneratorTest, GetLayerName) {
    std::string name = generator->GetLayerName();
    EXPECT_EQ(name, "line_layer");
}

TEST_F(LineGeneratorTest, GetGeometryType) {
    std::string type = generator->GetGeometryType();
    EXPECT_EQ(type, "LineString");
}

TEST_F(LineGeneratorTest, GenerateMultipleFeatures) {
    generator->Initialize(0.0, 0.0, 100.0, 100.0, 4326);
    
    for (int i = 0; i < 10; ++i) {
        void* feature = generator->GenerateFeature();
        ASSERT_NE(feature, nullptr);
    }
}
