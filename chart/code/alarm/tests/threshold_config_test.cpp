#include <gtest/gtest.h>
#include "alert/threshold_config.h"

class ThresholdConfigTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ThresholdConfigTest, DefaultConstructor) {
    alert::ThresholdConfig config;
    
    EXPECT_TRUE(config.GetName().empty());
    EXPECT_TRUE(config.IsEnabled());
    EXPECT_DOUBLE_EQ(config.GetLevel1Threshold(), 0.3);
    EXPECT_DOUBLE_EQ(config.GetLevel2Threshold(), 0.5);
    EXPECT_DOUBLE_EQ(config.GetLevel3Threshold(), 1.0);
    EXPECT_DOUBLE_EQ(config.GetLevel4Threshold(), 1.5);
    EXPECT_DOUBLE_EQ(config.GetWarningTime(), 600.0);
    EXPECT_DOUBLE_EQ(config.GetClearDelay(), 30.0);
}

TEST_F(ThresholdConfigTest, NameConstructor) {
    alert::ThresholdConfig config("TestConfig");
    EXPECT_EQ(config.GetName(), "TestConfig");
}

TEST_F(ThresholdConfigTest, SettersAndGetters) {
    alert::ThresholdConfig config;
    
    config.SetName("CustomConfig");
    EXPECT_EQ(config.GetName(), "CustomConfig");
    
    config.SetEnabled(false);
    EXPECT_FALSE(config.IsEnabled());
    
    config.SetLevel1Threshold(0.1);
    EXPECT_DOUBLE_EQ(config.GetLevel1Threshold(), 0.1);
    
    config.SetLevel2Threshold(0.2);
    EXPECT_DOUBLE_EQ(config.GetLevel2Threshold(), 0.2);
    
    config.SetLevel3Threshold(0.5);
    EXPECT_DOUBLE_EQ(config.GetLevel3Threshold(), 0.5);
    
    config.SetLevel4Threshold(1.0);
    EXPECT_DOUBLE_EQ(config.GetLevel4Threshold(), 1.0);
    
    config.SetWarningTime(300.0);
    EXPECT_DOUBLE_EQ(config.GetWarningTime(), 300.0);
    
    config.SetClearDelay(60.0);
    EXPECT_DOUBLE_EQ(config.GetClearDelay(), 60.0);
}

TEST_F(ThresholdConfigTest, Parameters) {
    alert::ThresholdConfig config;
    
    config.SetParameter("tcpa_level1", 6.0);
    EXPECT_DOUBLE_EQ(config.GetParameter("tcpa_level1"), 6.0);
    
    config.SetParameter("tcpa_level2", 15.0);
    EXPECT_DOUBLE_EQ(config.GetParameter("tcpa_level2"), 15.0);
    
    EXPECT_DOUBLE_EQ(config.GetParameter("nonexistent", 0.0), 0.0);
    EXPECT_DOUBLE_EQ(config.GetParameter("nonexistent", 100.0), 100.0);
    
    EXPECT_TRUE(config.HasParameter("tcpa_level1"));
    EXPECT_FALSE(config.HasParameter("nonexistent"));
}

TEST_F(ThresholdConfigTest, IsValid) {
    alert::ThresholdConfig config1;
    EXPECT_TRUE(config1.IsValid());
    
    alert::ThresholdConfig config2;
    config2.SetLevel1Threshold(0.5);
    config2.SetLevel2Threshold(0.3);
    EXPECT_FALSE(config2.IsValid());
    
    alert::ThresholdConfig config3;
    config3.SetLevel1Threshold(0.1);
    config3.SetLevel2Threshold(0.2);
    config3.SetLevel3Threshold(0.3);
    config3.SetLevel4Threshold(0.4);
    EXPECT_TRUE(config3.IsValid());
}

TEST_F(ThresholdConfigTest, ThresholdOrdering) {
    alert::ThresholdConfig config;
    
    config.SetLevel1Threshold(0.1);
    config.SetLevel2Threshold(0.2);
    config.SetLevel3Threshold(0.3);
    config.SetLevel4Threshold(0.4);
    EXPECT_TRUE(config.IsValid());
    
    config.SetLevel2Threshold(0.05);
    EXPECT_FALSE(config.IsValid());
    
    config.SetLevel2Threshold(0.2);
    config.SetLevel3Threshold(0.15);
    EXPECT_FALSE(config.IsValid());
    
    config.SetLevel3Threshold(0.3);
    config.SetLevel4Threshold(0.25);
    EXPECT_FALSE(config.IsValid());
}
