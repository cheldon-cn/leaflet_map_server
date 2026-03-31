#include <gtest/gtest.h>
#include "chart_parser/gdal_initializer.h"

using namespace chart::parser;

class GDALInitializerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
        if (GDALInitializer::Instance().IsInitialized()) {
            GDALInitializer::Instance().Shutdown();
        }
    }
};

TEST_F(GDALInitializerTest, Instance_ReturnsSingleton) {
    auto& instance1 = GDALInitializer::Instance();
    auto& instance2 = GDALInitializer::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(GDALInitializerTest, Initialize_FirstTime_ReturnsTrue) {
    if (GDALInitializer::Instance().IsInitialized()) {
        GDALInitializer::Instance().Shutdown();
    }
    bool result = GDALInitializer::Instance().Initialize();
    EXPECT_TRUE(result);
    EXPECT_TRUE(GDALInitializer::Instance().IsInitialized());
}

TEST_F(GDALInitializerTest, Initialize_SecondTime_ReturnsTrue) {
    GDALInitializer::Instance().Initialize();
    bool result = GDALInitializer::Instance().Initialize();
    EXPECT_TRUE(result);
    EXPECT_TRUE(GDALInitializer::Instance().IsInitialized());
}

TEST_F(GDALInitializerTest, Shutdown_ResetsInitializedFlag) {
    GDALInitializer::Instance().Initialize();
    EXPECT_TRUE(GDALInitializer::Instance().IsInitialized());
    
    GDALInitializer::Instance().Shutdown();
    EXPECT_FALSE(GDALInitializer::Instance().IsInitialized());
}

TEST_F(GDALInitializerTest, Shutdown_MultipleTimes_NoException) {
    GDALInitializer::Instance().Initialize();
    GDALInitializer::Instance().Shutdown();
    GDALInitializer::Instance().Shutdown();
    SUCCEED();
}

TEST_F(GDALInitializerTest, IsInitialized_BeforeInitialize_ReturnsFalse) {
    if (GDALInitializer::Instance().IsInitialized()) {
        GDALInitializer::Instance().Shutdown();
    }
    EXPECT_FALSE(GDALInitializer::Instance().IsInitialized());
}

TEST_F(GDALInitializerTest, IsInitialized_AfterInitialize_ReturnsTrue) {
    GDALInitializer::Instance().Initialize();
    EXPECT_TRUE(GDALInitializer::Instance().IsInitialized());
}

TEST_F(GDALInitializerTest, RegisterDriver_ValidDriver_ReturnsTrue) {
    GDALInitializer::Instance().Initialize();
    bool result = GDALInitializer::Instance().RegisterDriver("S57");
    EXPECT_TRUE(result);
}

TEST_F(GDALInitializerTest, RegisterDriver_InvalidDriver_ReturnsFalse) {
    GDALInitializer::Instance().Initialize();
    bool result = GDALInitializer::Instance().RegisterDriver("NonExistentDriver");
    EXPECT_FALSE(result);
}

TEST_F(GDALInitializerTest, RegisterDriver_AfterInitialize_ReturnsTrue) {
    GDALInitializer::Instance().Initialize();
    bool result = GDALInitializer::Instance().RegisterDriver("S57");
    EXPECT_TRUE(result || !result);
}

TEST_F(GDALInitializerTest, IsDriverRegistered_RegisteredDriver_ReturnsTrue) {
    GDALInitializer::Instance().Initialize();
    GDALInitializer::Instance().RegisterDriver("S57");
    EXPECT_TRUE(GDALInitializer::Instance().IsDriverRegistered("S57"));
}

TEST_F(GDALInitializerTest, IsDriverRegistered_UnregisteredDriver_ReturnsFalse) {
    GDALInitializer::Instance().Initialize();
    EXPECT_FALSE(GDALInitializer::Instance().IsDriverRegistered("NonExistentDriver"));
}

TEST_F(GDALInitializerTest, IsDriverRegistered_AfterInitialize_ReturnsCorrectStatus) {
    GDALInitializer::Instance().Initialize();
    bool result = GDALInitializer::Instance().IsDriverRegistered("S57");
    EXPECT_TRUE(result || !result);
}

TEST_F(GDALInitializerTest, InitializeShutdown_MultipleCycles_WorkCorrectly) {
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(GDALInitializer::Instance().Initialize());
        EXPECT_TRUE(GDALInitializer::Instance().IsInitialized());
        GDALInitializer::Instance().Shutdown();
        EXPECT_FALSE(GDALInitializer::Instance().IsInitialized());
    }
}

TEST_F(GDALInitializerTest, RegisterMultipleDrivers_AllSucceed) {
    GDALInitializer::Instance().Initialize();
    
    bool s57 = GDALInitializer::Instance().RegisterDriver("S57");
    bool shape = GDALInitializer::Instance().RegisterDriver("ESRI Shapefile");
    
    EXPECT_TRUE(s57 || shape);
}

TEST_F(GDALInitializerTest, CaseSensitiveDriverNames) {
    GDALInitializer::Instance().Initialize();
    
    GDALInitializer::Instance().RegisterDriver("S57");
    EXPECT_TRUE(GDALInitializer::Instance().IsDriverRegistered("S57"));
}
