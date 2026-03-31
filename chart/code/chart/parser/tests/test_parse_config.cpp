#include <gtest/gtest.h>
#include "chart_parser/parse_config.h"

using namespace chart::parser;

class ParseConfigTest : public ::testing::Test {
};

TEST_F(ParseConfigTest, DefaultConstructor_SetsDefaultValues) {
    ParseConfig config;
    
    EXPECT_TRUE(config.validateGeometry);
    EXPECT_TRUE(config.validateAttributes);
    EXPECT_FALSE(config.strictMode);
    EXPECT_TRUE(config.includeMetadata);
    EXPECT_EQ(config.maxFeatureCount, 0);
    EXPECT_EQ(config.coordinateSystem, "EPSG:4326");
    EXPECT_DOUBLE_EQ(config.tolerance, 0.0001);
}

TEST_F(ParseConfigTest, ValidateGeometry_CanBeModified) {
    ParseConfig config;
    config.validateGeometry = false;
    EXPECT_FALSE(config.validateGeometry);
    
    config.validateGeometry = true;
    EXPECT_TRUE(config.validateGeometry);
}

TEST_F(ParseConfigTest, ValidateAttributes_CanBeModified) {
    ParseConfig config;
    config.validateAttributes = false;
    EXPECT_FALSE(config.validateAttributes);
}

TEST_F(ParseConfigTest, StrictMode_CanBeEnabled) {
    ParseConfig config;
    config.strictMode = true;
    EXPECT_TRUE(config.strictMode);
}

TEST_F(ParseConfigTest, IncludeMetadata_CanBeDisabled) {
    ParseConfig config;
    config.includeMetadata = false;
    EXPECT_FALSE(config.includeMetadata);
}

TEST_F(ParseConfigTest, MaxFeatureCount_CanBeSet) {
    ParseConfig config;
    config.maxFeatureCount = 1000;
    EXPECT_EQ(config.maxFeatureCount, 1000);
}

TEST_F(ParseConfigTest, MaxFeatureCount_ZeroMeansUnlimited) {
    ParseConfig config;
    config.maxFeatureCount = 0;
    EXPECT_EQ(config.maxFeatureCount, 0);
}

TEST_F(ParseConfigTest, CoordinateSystem_CanBeChanged) {
    ParseConfig config;
    config.coordinateSystem = "EPSG:3857";
    EXPECT_EQ(config.coordinateSystem, "EPSG:3857");
}

TEST_F(ParseConfigTest, CoordinateSystem_CustomProjection) {
    ParseConfig config;
    config.coordinateSystem = "+proj=utm +zone=33 +datum=WGS84";
    EXPECT_EQ(config.coordinateSystem, "+proj=utm +zone=33 +datum=WGS84");
}

TEST_F(ParseConfigTest, Tolerance_CanBeAdjusted) {
    ParseConfig config;
    config.tolerance = 0.001;
    EXPECT_DOUBLE_EQ(config.tolerance, 0.001);
}

TEST_F(ParseConfigTest, Tolerance_VerySmallValue) {
    ParseConfig config;
    config.tolerance = 1e-10;
    EXPECT_DOUBLE_EQ(config.tolerance, 1e-10);
}

TEST_F(ParseConfigTest, Tolerance_LargeValue) {
    ParseConfig config;
    config.tolerance = 1.0;
    EXPECT_DOUBLE_EQ(config.tolerance, 1.0);
}

TEST_F(ParseConfigTest, CopyConstructor_CopiesAllValues) {
    ParseConfig config1;
    config1.validateGeometry = false;
    config1.strictMode = true;
    config1.maxFeatureCount = 500;
    config1.coordinateSystem = "EPSG:3857";
    config1.tolerance = 0.01;
    
    ParseConfig config2 = config1;
    
    EXPECT_EQ(config2.validateGeometry, config1.validateGeometry);
    EXPECT_EQ(config2.validateAttributes, config1.validateAttributes);
    EXPECT_EQ(config2.strictMode, config1.strictMode);
    EXPECT_EQ(config2.includeMetadata, config1.includeMetadata);
    EXPECT_EQ(config2.maxFeatureCount, config1.maxFeatureCount);
    EXPECT_EQ(config2.coordinateSystem, config1.coordinateSystem);
    EXPECT_DOUBLE_EQ(config2.tolerance, config1.tolerance);
}

TEST_F(ParseConfigTest, AssignmentOperator_CopiesAllValues) {
    ParseConfig config1;
    config1.maxFeatureCount = 100;
    config1.tolerance = 0.005;
    
    ParseConfig config2;
    config2 = config1;
    
    EXPECT_EQ(config2.maxFeatureCount, 100);
    EXPECT_DOUBLE_EQ(config2.tolerance, 0.005);
}

TEST_F(ParseConfigTest, MultipleConfigs_AreIndependent) {
    ParseConfig config1;
    ParseConfig config2;
    
    config1.maxFeatureCount = 100;
    config2.maxFeatureCount = 200;
    
    EXPECT_EQ(config1.maxFeatureCount, 100);
    EXPECT_EQ(config2.maxFeatureCount, 200);
}

TEST_F(ParseConfigTest, StrictModeAffectsValidation) {
    ParseConfig strictConfig;
    strictConfig.strictMode = true;
    strictConfig.validateGeometry = true;
    strictConfig.validateAttributes = true;
    
    ParseConfig lenientConfig;
    lenientConfig.strictMode = false;
    lenientConfig.validateGeometry = false;
    lenientConfig.validateAttributes = false;
    
    EXPECT_TRUE(strictConfig.strictMode);
    EXPECT_FALSE(lenientConfig.strictMode);
}
