#include <gtest/gtest.h>
#include "ogc/navi/positioning/position_filter.h"

using namespace ogc::navi;

class PositionFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
        filter.SetMaxSpeed(30.0);
        filter.SetMaxAcceleration(10.0);
        filter.SetSmoothWindowSize(5);
        filter.SetMaxHdop(5.0);
        filter.SetMinSatellites(4);
    }
    
    PositionFilter filter;
    
    PositionData CreateValidPosition(double lat, double lon, double speed, 
                                     double heading, double timestamp) {
        PositionData data;
        data.latitude = lat;
        data.longitude = lon;
        data.speed = speed;
        data.heading = heading;
        data.timestamp = timestamp;
        data.hdop = 1.0;
        data.satellite_count = 8;
        data.quality = PositionQuality::Gps;
        data.valid = true;
        return data;
    }
};

TEST_F(PositionFilterTest, Filter_ValidData) {
    PositionData input = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    
    PositionData output = filter.Filter(input);
    
    EXPECT_TRUE(output.valid);
    EXPECT_NEAR(output.latitude, 31.2304, 0.0001);
    EXPECT_NEAR(output.longitude, 121.4737, 0.0001);
}

TEST_F(PositionFilterTest, Filter_InvalidLatitude) {
    PositionData input = CreateValidPosition(100.0, 121.4737, 10.0, 45.0, 1000.0);
    
    PositionData output = filter.Filter(input);
    
    EXPECT_FALSE(output.valid);
}

TEST_F(PositionFilterTest, Filter_InvalidLongitude) {
    PositionData input = CreateValidPosition(31.2304, 200.0, 10.0, 45.0, 1000.0);
    
    PositionData output = filter.Filter(input);
    
    EXPECT_FALSE(output.valid);
}

TEST_F(PositionFilterTest, Filter_HighHdop) {
    PositionData input = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    input.hdop = 10.0;
    
    PositionData output = filter.Filter(input);
    
    EXPECT_FALSE(output.valid);
}

TEST_F(PositionFilterTest, Filter_LowSatellites) {
    PositionData input = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    input.satellite_count = 2;
    
    PositionData output = filter.Filter(input);
    
    EXPECT_FALSE(output.valid);
}

TEST_F(PositionFilterTest, Filter_InvalidQuality) {
    PositionData input = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    input.quality = PositionQuality::Invalid;
    
    PositionData output = filter.Filter(input);
    
    EXPECT_FALSE(output.valid);
}

TEST_F(PositionFilterTest, Filter_OutlierDetection) {
    PositionData pos1 = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData pos2 = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1010.0);
    PositionData pos3 = CreateValidPosition(35.0, 125.0, 10.0, 45.0, 1020.0);
    
    filter.Filter(pos1);
    filter.Filter(pos2);
    PositionData output = filter.Filter(pos3);
    
    FilterStatistics stats = filter.GetStatistics();
    EXPECT_GT(stats.outlier_count, 0);
}

TEST_F(PositionFilterTest, Filter_Smoothing) {
    PositionData pos1 = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData pos2 = CreateValidPosition(31.2305, 121.4738, 10.1, 45.5, 1010.0);
    PositionData pos3 = CreateValidPosition(31.2306, 121.4739, 10.2, 46.0, 1020.0);
    PositionData pos4 = CreateValidPosition(31.2307, 121.4740, 10.3, 46.5, 1030.0);
    PositionData pos5 = CreateValidPosition(31.2308, 121.4741, 10.4, 47.0, 1040.0);
    
    filter.Filter(pos1);
    filter.Filter(pos2);
    filter.Filter(pos3);
    filter.Filter(pos4);
    PositionData output = filter.Filter(pos5);
    
    EXPECT_NEAR(output.latitude, 31.2308, 0.001);
    EXPECT_NEAR(output.longitude, 121.4741, 0.001);
}

TEST_F(PositionFilterTest, Filter_Statistics) {
    PositionData pos1 = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData pos2 = CreateValidPosition(31.2305, 121.4738, 10.0, 45.0, 1010.0);
    
    filter.Filter(pos1);
    filter.Filter(pos2);
    
    FilterStatistics stats = filter.GetStatistics();
    EXPECT_EQ(stats.total_received, 2);
    EXPECT_EQ(stats.valid_count, 2);
    EXPECT_EQ(stats.outlier_count, 0);
}

TEST_F(PositionFilterTest, Reset) {
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    filter.Filter(pos);
    
    filter.Reset();
    
    FilterStatistics stats = filter.GetStatistics();
    EXPECT_EQ(stats.total_received, 0);
    EXPECT_EQ(stats.valid_count, 0);
}

TEST_F(PositionFilterTest, SetMaxSpeed) {
    filter.SetMaxSpeed(50.0);
    
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData output = filter.Filter(pos);
    
    EXPECT_TRUE(output.valid);
}

TEST_F(PositionFilterTest, SetMaxAcceleration) {
    filter.SetMaxAcceleration(5.0);
    
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData output = filter.Filter(pos);
    
    EXPECT_TRUE(output.valid);
}

TEST_F(PositionFilterTest, SetSmoothWindowSize) {
    filter.SetSmoothWindowSize(10);
    
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    PositionData output = filter.Filter(pos);
    
    EXPECT_TRUE(output.valid);
}

TEST_F(PositionFilterTest, SetMaxHdop) {
    filter.SetMaxHdop(10.0);
    
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    pos.hdop = 8.0;
    PositionData output = filter.Filter(pos);
    
    EXPECT_TRUE(output.valid);
}

TEST_F(PositionFilterTest, SetMinSatellites) {
    filter.SetMinSatellites(3);
    
    PositionData pos = CreateValidPosition(31.2304, 121.4737, 10.0, 45.0, 1000.0);
    pos.satellite_count = 4;
    PositionData output = filter.Filter(pos);
    
    EXPECT_TRUE(output.valid);
}
