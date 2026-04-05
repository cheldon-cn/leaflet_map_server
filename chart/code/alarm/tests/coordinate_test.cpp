#include <gtest/gtest.h>
#include "alert/coordinate.h"
#include <cmath>

class CoordinateTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CoordinateTest, DefaultConstructor) {
    alert::Coordinate coord;
    EXPECT_DOUBLE_EQ(coord.GetX(), 0.0);
    EXPECT_DOUBLE_EQ(coord.GetY(), 0.0);
    EXPECT_DOUBLE_EQ(coord.GetZ(), 0.0);
}

TEST_F(CoordinateTest, TwoParamConstructor) {
    alert::Coordinate coord(120.5, 31.2);
    EXPECT_DOUBLE_EQ(coord.GetX(), 120.5);
    EXPECT_DOUBLE_EQ(coord.GetY(), 31.2);
    EXPECT_DOUBLE_EQ(coord.GetZ(), 0.0);
}

TEST_F(CoordinateTest, ThreeParamConstructor) {
    alert::Coordinate coord(120.5, 31.2, 10.0);
    EXPECT_DOUBLE_EQ(coord.GetX(), 120.5);
    EXPECT_DOUBLE_EQ(coord.GetY(), 31.2);
    EXPECT_DOUBLE_EQ(coord.GetZ(), 10.0);
}

TEST_F(CoordinateTest, LongitudeLatitude) {
    alert::Coordinate coord(120.5, 31.2, 10.0);
    EXPECT_DOUBLE_EQ(coord.GetLongitude(), 120.5);
    EXPECT_DOUBLE_EQ(coord.GetLatitude(), 31.2);
    EXPECT_DOUBLE_EQ(coord.GetAltitude(), 10.0);
    
    coord.SetLongitude(121.0);
    coord.SetLatitude(32.0);
    coord.SetAltitude(15.0);
    
    EXPECT_DOUBLE_EQ(coord.GetLongitude(), 121.0);
    EXPECT_DOUBLE_EQ(coord.GetLatitude(), 32.0);
    EXPECT_DOUBLE_EQ(coord.GetAltitude(), 15.0);
}

TEST_F(CoordinateTest, IsNull) {
    alert::Coordinate coord1;
    EXPECT_TRUE(coord1.IsNull());
    
    alert::Coordinate coord2(0, 0, 0);
    EXPECT_TRUE(coord2.IsNull());
    
    alert::Coordinate coord3(1, 0, 0);
    EXPECT_FALSE(coord3.IsNull());
}

TEST_F(CoordinateTest, IsValid) {
    alert::Coordinate coord1(120.5, 31.2);
    EXPECT_TRUE(coord1.IsValid());
    
    alert::Coordinate coord2(181.0, 31.2);
    EXPECT_FALSE(coord2.IsValid());
    
    alert::Coordinate coord3(120.5, 91.0);
    EXPECT_FALSE(coord3.IsValid());
    
    alert::Coordinate coord4(-181.0, 31.2);
    EXPECT_FALSE(coord4.IsValid());
    
    alert::Coordinate coord5(120.5, -91.0);
    EXPECT_FALSE(coord5.IsValid());
}

TEST_F(CoordinateTest, Equality) {
    alert::Coordinate coord1(120.5, 31.2, 10.0);
    alert::Coordinate coord2(120.5, 31.2, 10.0);
    alert::Coordinate coord3(120.5, 31.2, 11.0);
    alert::Coordinate coord4(121.0, 31.2, 10.0);
    
    EXPECT_TRUE(coord1 == coord2);
    EXPECT_FALSE(coord1 == coord3);
    EXPECT_FALSE(coord1 == coord4);
    EXPECT_TRUE(coord1 != coord3);
    EXPECT_TRUE(coord1 != coord4);
}

TEST_F(CoordinateTest, DistanceTo) {
    alert::Coordinate coord1(0, 0);
    alert::Coordinate coord2(0, 1);
    
    double distance = coord1.DistanceTo(coord2);
    EXPECT_GT(distance, 110000);
    EXPECT_LT(distance, 112000);
}

TEST_F(CoordinateTest, BearingTo) {
    alert::Coordinate coord1(0, 0);
    alert::Coordinate coord2(0, 1);
    
    double bearing = coord1.BearingTo(coord2);
    EXPECT_DOUBLE_EQ(bearing, 0.0);
    
    alert::Coordinate coord3(1, 0);
    bearing = coord1.BearingTo(coord3);
    EXPECT_GT(bearing, 89.0);
    EXPECT_LT(bearing, 91.0);
}

TEST_F(CoordinateTest, Destination) {
    alert::Coordinate coord(0, 0);
    alert::Coordinate dest = coord.Destination(111319, 0);
    
    EXPECT_GT(dest.GetLatitude(), 0.9);
    EXPECT_LT(dest.GetLatitude(), 1.1);
}

TEST_F(CoordinateTest, ToString) {
    alert::Coordinate coord1(120.5, 31.2);
    std::string str = coord1.ToString();
    EXPECT_TRUE(str.find("120.5") != std::string::npos);
    EXPECT_TRUE(str.find("31.2") != std::string::npos);
    
    alert::Coordinate coord2(120.5, 31.2, 10.0);
    str = coord2.ToString();
    EXPECT_TRUE(str.find("10") != std::string::npos);
}

TEST_F(CoordinateTest, CalculateDistance) {
    double distance = alert::Coordinate::CalculateDistance(0, 0, 0, 1);
    EXPECT_GT(distance, 110000);
    EXPECT_LT(distance, 112000);
}

TEST_F(CoordinateTest, CalculateBearing) {
    double bearing = alert::Coordinate::CalculateBearing(0, 0, 0, 1);
    EXPECT_DOUBLE_EQ(bearing, 0.0);
}
