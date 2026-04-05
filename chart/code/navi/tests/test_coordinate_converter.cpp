#include <gtest/gtest.h>
#include "ogc/navi/positioning/coordinate_converter.h"
#include <cmath>

using namespace ogc::navi;

class CoordinateConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        converter = &CoordinateConverter::Instance();
    }
    
    CoordinateConverter* converter;
    
    static constexpr double EPSILON = 0.001;
    static constexpr double DISTANCE_EPSILON = 100.0;
};

TEST_F(CoordinateConverterTest, MercatorForwardAndInverse) {
    double lat = 31.2304;
    double lon = 121.4737;
    
    double x, y;
    converter->LatLonToMercator(lat, lon, x, y);
    
    double lat_result, lon_result;
    converter->MercatorToLatLon(x, y, lat_result, lon_result);
    
    EXPECT_NEAR(lat_result, lat, EPSILON);
    EXPECT_NEAR(lon_result, lon, EPSILON);
}

TEST_F(CoordinateConverterTest, MercatorEquator) {
    double lat = 0.0;
    double lon = 0.0;
    
    double x, y;
    converter->LatLonToMercator(lat, lon, x, y);
    
    EXPECT_NEAR(x, 0.0, EPSILON);
    EXPECT_NEAR(y, 0.0, EPSILON);
}

TEST_F(CoordinateConverterTest, MercatorHighLatitude) {
    double lat = 60.0;
    double lon = 0.0;
    
    double x, y;
    converter->LatLonToMercator(lat, lon, x, y);
    
    EXPECT_NEAR(x, 0.0, EPSILON);
    EXPECT_GT(std::abs(y), 0.0);
}

TEST_F(CoordinateConverterTest, GreatCircleDistance_SamePoint) {
    double lat = 31.2304;
    double lon = 121.4737;
    
    double distance = converter->CalculateGreatCircleDistance(lat, lon, lat, lon);
    
    EXPECT_NEAR(distance, 0.0, EPSILON);
}

TEST_F(CoordinateConverterTest, GreatCircleDistance_KnownDistance) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double lat2 = 0.0;
    double lon2 = 1.0;
    
    double distance = converter->CalculateGreatCircleDistance(lat1, lon1, lat2, lon2);
    
    EXPECT_NEAR(distance, 111195.0, 200.0);
}

TEST_F(CoordinateConverterTest, GreatCircleDistance_ShanghaiToBeijing) {
    double shanghai_lat = 31.2304;
    double shanghai_lon = 121.4737;
    double beijing_lat = 39.9042;
    double beijing_lon = 116.4074;
    
    double distance = converter->CalculateGreatCircleDistance(
        shanghai_lat, shanghai_lon, beijing_lat, beijing_lon);
    
    double expected_distance = 1068000.0;
    EXPECT_NEAR(distance, expected_distance, 5000.0);
}

TEST_F(CoordinateConverterTest, Bearing_North) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double lat2 = 10.0;
    double lon2 = 0.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    
    EXPECT_NEAR(bearing, 0.0, EPSILON);
}

TEST_F(CoordinateConverterTest, Bearing_East) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double lat2 = 0.0;
    double lon2 = 10.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    
    EXPECT_NEAR(bearing, 90.0, EPSILON);
}

TEST_F(CoordinateConverterTest, Bearing_South) {
    double lat1 = 10.0;
    double lon1 = 0.0;
    double lat2 = 0.0;
    double lon2 = 0.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    
    EXPECT_NEAR(bearing, 180.0, EPSILON);
}

TEST_F(CoordinateConverterTest, Bearing_West) {
    double lat1 = 0.0;
    double lon1 = 10.0;
    double lat2 = 0.0;
    double lon2 = 0.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    
    EXPECT_NEAR(bearing, 270.0, EPSILON);
}

TEST_F(CoordinateConverterTest, Destination_North) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double bearing = 0.0;
    double distance = 111319.0;
    
    double lat2, lon2;
    converter->CalculateDestination(lat1, lon1, bearing, distance, lat2, lon2);
    
    EXPECT_NEAR(lat2, 1.0, 0.01);
    EXPECT_NEAR(lon2, 0.0, EPSILON);
}

TEST_F(CoordinateConverterTest, Destination_East) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double bearing = 90.0;
    double distance = 111319.0;
    
    double lat2, lon2;
    converter->CalculateDestination(lat1, lon1, bearing, distance, lat2, lon2);
    
    EXPECT_NEAR(lat2, 0.0, EPSILON);
    EXPECT_NEAR(lon2, 1.0, 0.01);
}

TEST_F(CoordinateConverterTest, Destination_RoundTrip) {
    double lat1 = 31.2304;
    double lon1 = 121.4737;
    double bearing = 45.0;
    double distance = 100000.0;
    
    double lat2, lon2;
    converter->CalculateDestination(lat1, lon1, bearing, distance, lat2, lon2);
    
    double return_bearing = converter->CalculateBearing(lat2, lon2, lat1, lon1);
    return_bearing = std::fmod(return_bearing + 180.0, 360.0);
    
    EXPECT_NEAR(return_bearing, bearing, 1.0);
}

TEST_F(CoordinateConverterTest, GeoToScreenAndBack) {
    double lat = 31.2304;
    double lon = 121.4737;
    double scale = 0.001;
    double origin_x = 0.0;
    double origin_y = 0.0;
    
    double screen_x, screen_y;
    converter->GeoToScreen(lat, lon, scale, origin_x, origin_y, screen_x, screen_y);
    
    double lat_result, lon_result;
    converter->ScreenToGeo(screen_x, screen_y, scale, origin_x, origin_y, lat_result, lon_result);
    
    EXPECT_NEAR(lat_result, lat, EPSILON);
    EXPECT_NEAR(lon_result, lon, EPSILON);
}

TEST_F(CoordinateConverterTest, Wgs84ToCgcs2000_SmallDifference) {
    double wgs_lat = 31.2304;
    double wgs_lon = 121.4737;
    
    double cgcs_lat, cgcs_lon;
    converter->Wgs84ToCgcs2000(wgs_lat, wgs_lon, cgcs_lat, cgcs_lon);
    
    EXPECT_NEAR(cgcs_lat, wgs_lat, 0.1);
    EXPECT_NEAR(cgcs_lon, wgs_lon, 0.1);
}

TEST_F(CoordinateConverterTest, LatLonToMercator_MaxLatitude) {
    double lat = 85.0;
    double lon = 180.0;
    double x, y;
    EXPECT_NO_THROW(converter->LatLonToMercator(lat, lon, x, y));
}

TEST_F(CoordinateConverterTest, LatLonToMercator_MinLatitude) {
    double lat = -85.0;
    double lon = -180.0;
    double x, y;
    EXPECT_NO_THROW(converter->LatLonToMercator(lat, lon, x, y));
}

TEST_F(CoordinateConverterTest, LatLonToMercator_DateLine) {
    double lat = 0.0;
    double lon = 180.0;
    double x, y;
    EXPECT_NO_THROW(converter->LatLonToMercator(lat, lon, x, y));
}

TEST_F(CoordinateConverterTest, LatLonToMercator_PrimeMeridian) {
    double lat = 51.5;
    double lon = 0.0;
    double x, y;
    converter->LatLonToMercator(lat, lon, x, y);
    EXPECT_NEAR(x, 0.0, EPSILON);
}

TEST_F(CoordinateConverterTest, GreatCircleDistance_AntipodalPoints) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double lat2 = 0.0;
    double lon2 = 180.0;
    
    double distance = converter->CalculateGreatCircleDistance(lat1, lon1, lat2, lon2);
    EXPECT_NEAR(distance, 20015087.0, 10000.0);
}

TEST_F(CoordinateConverterTest, GreatCircleDistance_PoleToEquator) {
    double lat1 = 90.0;
    double lon1 = 0.0;
    double lat2 = 0.0;
    double lon2 = 0.0;
    
    double distance = converter->CalculateGreatCircleDistance(lat1, lon1, lat2, lon2);
    EXPECT_NEAR(distance, 10007543.0, 10000.0);
}

TEST_F(CoordinateConverterTest, Bearing_NorthPole) {
    double lat1 = 89.0;
    double lon1 = 0.0;
    double lat2 = 90.0;
    double lon2 = 0.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    EXPECT_NEAR(bearing, 0.0, 1.0);
}

TEST_F(CoordinateConverterTest, Bearing_SouthPole) {
    double lat1 = -89.0;
    double lon1 = 0.0;
    double lat2 = -90.0;
    double lon2 = 0.0;
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    EXPECT_NEAR(bearing, 180.0, 1.0);
}

TEST_F(CoordinateConverterTest, CalculateDestination_ZeroDistance) {
    double lat1 = 31.2304;
    double lon1 = 121.4737;
    double bearing = 45.0;
    double distance = 0.0;
    
    double lat2, lon2;
    converter->CalculateDestination(lat1, lon1, bearing, distance, lat2, lon2);
    
    EXPECT_NEAR(lat2, lat1, EPSILON);
    EXPECT_NEAR(lon2, lon1, EPSILON);
}

TEST_F(CoordinateConverterTest, CalculateDestination_LargeDistance) {
    double lat1 = 0.0;
    double lon1 = 0.0;
    double bearing = 0.0;
    double distance = 10000000.0;
    
    double lat2, lon2;
    converter->CalculateDestination(lat1, lon1, bearing, distance, lat2, lon2);
    
    EXPECT_GT(lat2, 80.0);
}

TEST_F(CoordinateConverterTest, GeoToScreen_ZeroScale) {
    double lat = 31.2304;
    double lon = 121.4737;
    double scale = 0.0;
    double origin_x = 0.0;
    double origin_y = 0.0;
    
    double screen_x, screen_y;
    EXPECT_NO_THROW(converter->GeoToScreen(lat, lon, scale, origin_x, origin_y, screen_x, screen_y));
}

TEST_F(CoordinateConverterTest, MercatorToLatLon_Origin) {
    double x = 0.0;
    double y = 0.0;
    double lat, lon;
    converter->MercatorToLatLon(x, y, lat, lon);
    
    EXPECT_NEAR(lat, 0.0, EPSILON);
    EXPECT_NEAR(lon, 0.0, EPSILON);
}
