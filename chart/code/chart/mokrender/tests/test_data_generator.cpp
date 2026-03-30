#include <gtest/gtest.h>
#include "ogc/mokrender/data_generator.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class DataGeneratorTest : public ::testing::Test {
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
        config.randomSeed = 42;
    }
    
    DataGeneratorConfig config;
};

TEST_F(DataGeneratorTest, CreateInstance) {
    IDataGenerator* generator = IDataGenerator::Create();
    ASSERT_NE(generator, nullptr);
    delete generator;
}

TEST_F(DataGeneratorTest, InitializeWithValidConfig) {
    IDataGenerator* generator = IDataGenerator::Create();
    MokRenderResult result = generator->Initialize(config);
    EXPECT_TRUE(result.IsSuccess());
    delete generator;
}

TEST_F(DataGeneratorTest, GenerateData) {
    IDataGenerator* generator = IDataGenerator::Create();
    generator->Initialize(config);
    
    MokRenderResult result = generator->Generate();
    EXPECT_TRUE(result.IsSuccess());
    
    int count = generator->GetGeneratedCount();
    EXPECT_EQ(count, 25);
    
    delete generator;
}

TEST_F(DataGeneratorTest, GenerateWithoutInitialize) {
    IDataGenerator* generator = IDataGenerator::Create();
    
    MokRenderResult result = generator->Generate();
    EXPECT_TRUE(result.IsError());
    EXPECT_EQ(result.code, MokRenderErrorCode::INVALID_PARAMETER);
    
    delete generator;
}

TEST_F(DataGeneratorTest, SaveToDatabaseWithoutGenerate) {
    IDataGenerator* generator = IDataGenerator::Create();
    generator->Initialize(config);
    
    MokRenderResult result = generator->SaveToDatabase("test.db");
    EXPECT_TRUE(result.IsSuccess());
    
    delete generator;
}
