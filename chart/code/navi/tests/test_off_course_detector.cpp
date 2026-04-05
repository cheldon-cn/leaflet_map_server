#include <gtest/gtest.h>
#include "ogc/navi/navigation/off_course_detector.h"

using namespace ogc::navi;

class OffCourseDetectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        detector = &OffCourseDetector::Instance();
        detector->SetMaxCrossTrackError(500.0);
        detector->SetMaxDistanceToRoute(1000.0);
        detector->SetReplanThreshold(2000.0);
        
        route_start.longitude = 121.0;
        route_start.latitude = 31.0;
        route_end.longitude = 122.0;
        route_end.latitude = 32.0;
    }
    
    OffCourseDetector* detector;
    GeoPoint route_start;
    GeoPoint route_end;
    
    PositionData CreatePosition(double lat, double lon) {
        PositionData pos;
        pos.latitude = lat;
        pos.longitude = lon;
        pos.timestamp = 1000.0;
        pos.valid = true;
        return pos;
    }
};

TEST_F(OffCourseDetectorTest, OnCourse) {
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_FALSE(result.is_off_course);
}

TEST_F(OffCourseDetectorTest, OffCourse) {
    PositionData pos = CreatePosition(31.5, 120.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_TRUE(result.is_off_course);
}

TEST_F(OffCourseDetectorTest, SetMaxCrossTrackError) {
    detector->SetMaxCrossTrackError(100000.0);
    
    PositionData pos = CreatePosition(31.5, 120.5);
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_FALSE(result.is_off_course);
}

TEST_F(OffCourseDetectorTest, SetMaxDistanceToRoute) {
    detector->SetMaxCrossTrackError(100000.0);
    
    PositionData pos = CreatePosition(31.5, 120.5);
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_FALSE(result.is_off_course);
}

TEST_F(OffCourseDetectorTest, SetReplanThreshold) {
    detector->SetReplanThreshold(100.0);
    
    PositionData pos = CreatePosition(31.5, 121.5);
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_TRUE(result.should_replan);
}

TEST_F(OffCourseDetectorTest, CrossTrackError) {
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_GE(result.cross_track_error, 0.0);
}

TEST_F(OffCourseDetectorTest, DistanceToRoute) {
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_GE(result.distance_to_route, 0.0);
}

TEST_F(OffCourseDetectorTest, NearestLegIndex) {
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_EQ(result.nearest_leg_index, 0);
}

TEST_F(OffCourseDetectorTest, NearestPointOnRoute) {
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_GE(result.nearest_point_on_route.longitude, route_start.longitude);
    EXPECT_LE(result.nearest_point_on_route.longitude, route_end.longitude);
}

TEST_F(OffCourseDetectorTest, RecommendedBearing) {
    PositionData pos = CreatePosition(31.5, 120.5);
    
    OffCourseResult result = detector->Detect(pos, route_start, route_end);
    
    EXPECT_GE(result.recommended_bearing, 0.0);
    EXPECT_LT(result.recommended_bearing, 360.0);
}

TEST_F(OffCourseDetectorTest, FindNearestPointOnRoute) {
    GeoPoint position(121.5, 31.5);
    
    GeoPoint nearest = detector->FindNearestPointOnRoute(position, route_start, route_end);
    
    EXPECT_GE(nearest.longitude, route_start.longitude);
    EXPECT_LE(nearest.longitude, route_end.longitude);
}

TEST_F(OffCourseDetectorTest, DetectWithRoutePoints) {
    std::vector<GeoPoint> route_points;
    route_points.push_back(route_start);
    route_points.push_back(route_end);
    
    PositionData pos = CreatePosition(31.5, 121.5);
    
    OffCourseResult result = detector->Detect(pos, route_points, 0);
    
    EXPECT_FALSE(result.is_off_course);
}

TEST_F(OffCourseDetectorTest, FindNearestLeg) {
    std::vector<GeoPoint> route_points;
    route_points.push_back(route_start);
    route_points.push_back(route_end);
    
    GeoPoint position(121.5, 31.5);
    
    int leg = detector->FindNearestLeg(position, route_points);
    
    EXPECT_EQ(leg, 0);
}
