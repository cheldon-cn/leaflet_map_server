#include <gtest/gtest.h>
#include "ogc/navi/navigation/navigation_calculator.h"

using namespace ogc::navi;

class NavigationCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        calculator = &NavigationCalculator::Instance();
    }
    
    NavigationCalculator* calculator;
    
    PositionData CreatePosition(double lat, double lon, double speed, double course) {
        PositionData pos;
        pos.latitude = lat;
        pos.longitude = lon;
        pos.speed = speed;
        pos.course = course;
        pos.timestamp = 1000.0;
        pos.valid = true;
        return pos;
    }
};

TEST_F(NavigationCalculatorTest, Calculate_BasicNavigation) {
    PositionData pos = CreatePosition(31.0, 121.0, 10.0, 45.0);
    GeoPoint waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_GT(data.distance_to_waypoint, 0.0);
    EXPECT_GE(data.bearing_to_waypoint, 0.0);
    EXPECT_LT(data.bearing_to_waypoint, 360.0);
}

TEST_F(NavigationCalculatorTest, Calculate_DistanceToWaypoint) {
    PositionData pos = CreatePosition(31.2304, 121.4737, 10.0, 45.0);
    GeoPoint waypoint(121.4737, 31.2304);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_NEAR(data.distance_to_waypoint, 0.0, 100.0);
}

TEST_F(NavigationCalculatorTest, Calculate_BearingToWaypoint) {
    PositionData pos = CreatePosition(31.0, 121.0, 10.0, 45.0);
    GeoPoint waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_GE(data.bearing_to_waypoint, 0.0);
    EXPECT_LT(data.bearing_to_waypoint, 360.0);
}

TEST_F(NavigationCalculatorTest, Calculate_TTG) {
    PositionData pos = CreatePosition(31.0, 121.0, 10.0, 45.0);
    GeoPoint waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_GT(data.time_to_waypoint, 0.0);
}

TEST_F(NavigationCalculatorTest, Calculate_ETA) {
    PositionData pos = CreatePosition(31.0, 121.0, 10.0, 45.0);
    pos.timestamp = 1000.0;
    GeoPoint waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_GT(data.eta, pos.timestamp);
}

TEST_F(NavigationCalculatorTest, Calculate_ZeroSpeed) {
    PositionData pos = CreatePosition(31.0, 121.0, 0.0, 45.0);
    GeoPoint waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 0.0);
    
    EXPECT_DOUBLE_EQ(data.time_to_waypoint, 0.0);
}

TEST_F(NavigationCalculatorTest, CalculateCrossTrackError) {
    GeoPoint position(121.5, 31.0);
    GeoPoint start(121.0, 31.0);
    GeoPoint end(122.0, 31.0);
    
    double xtd = calculator->CalculateCrossTrackError(position, start, end);
    
    EXPECT_NEAR(xtd, 0.0, 200.0);
}

TEST_F(NavigationCalculatorTest, CalculateCrossTrackError_OffCourse) {
    GeoPoint position(121.5, 32.0);
    GeoPoint start(121.0, 31.0);
    GeoPoint end(122.0, 31.0);
    
    double xtd = calculator->CalculateCrossTrackError(position, start, end);
    
    EXPECT_GT(std::abs(xtd), 0.0);
}

TEST_F(NavigationCalculatorTest, CalculateAlongTrackDistance) {
    GeoPoint position(121.5, 31.0);
    GeoPoint start(121.0, 31.0);
    GeoPoint end(122.0, 31.0);
    
    double atd = calculator->CalculateAlongTrackDistance(position, start, end);
    
    EXPECT_GT(atd, 0.0);
}

TEST_F(NavigationCalculatorTest, CalculateDistanceToWaypoint) {
    GeoPoint position(121.0, 31.0);
    GeoPoint waypoint(122.0, 32.0);
    
    double distance = calculator->CalculateDistanceToWaypoint(position, waypoint);
    
    EXPECT_GT(distance, 0.0);
}

TEST_F(NavigationCalculatorTest, CalculateBearingToWaypoint) {
    GeoPoint position(121.0, 31.0);
    GeoPoint waypoint(122.0, 32.0);
    
    double bearing = calculator->CalculateBearingToWaypoint(position, waypoint);
    
    EXPECT_GE(bearing, 0.0);
    EXPECT_LT(bearing, 360.0);
}

TEST_F(NavigationCalculatorTest, CalculateTimeToWaypoint) {
    double distance = 10000.0;  // 10 km
    double speed = 10.0;  // 10 m/s
    
    double time = calculator->CalculateTimeToWaypoint(distance, speed);
    
    EXPECT_GT(time, 0.0);
}

TEST_F(NavigationCalculatorTest, CalculateTimeToWaypoint_ZeroSpeed) {
    double distance = 10000.0;
    double speed = 0.0;
    
    double time = calculator->CalculateTimeToWaypoint(distance, speed);
    
    EXPECT_DOUBLE_EQ(time, 0.0);
}

TEST_F(NavigationCalculatorTest, Calculate_WithRouteLeg) {
    PositionData pos = CreatePosition(31.5, 121.5, 10.0, 45.0);
    GeoPoint prev_waypoint(121.0, 31.0);
    GeoPoint next_waypoint(122.0, 32.0);
    
    NavigationData data = calculator->Calculate(pos, prev_waypoint, next_waypoint, 10.0);
    
    EXPECT_GT(data.distance_to_waypoint, 0.0);
    EXPECT_GE(data.cross_track_error, 0.0);
    EXPECT_GE(data.along_track_distance, 0.0);
}

TEST_F(NavigationCalculatorTest, Calculate_CrossTrackError) {
    PositionData pos = CreatePosition(31.0, 121.5, 10.0, 45.0);
    GeoPoint prev_waypoint(121.0, 31.0);
    GeoPoint next_waypoint(122.0, 31.0);
    
    NavigationData data = calculator->Calculate(pos, prev_waypoint, next_waypoint, 10.0);
    
    EXPECT_GT(std::abs(data.cross_track_error), 0.0);
}

TEST_F(NavigationCalculatorTest, Calculate_SamePosition) {
    PositionData pos = CreatePosition(31.0, 121.0, 10.0, 45.0);
    GeoPoint waypoint(121.0, 31.0);
    
    NavigationData data = calculator->Calculate(pos, waypoint, 10.0);
    
    EXPECT_NEAR(data.distance_to_waypoint, 0.0, 10.0);
}
