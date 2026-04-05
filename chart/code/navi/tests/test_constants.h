#ifndef OGC_NAVI_TESTS_TEST_CONSTANTS_H
#define OGC_NAVI_TESTS_TEST_CONSTANTS_H

namespace ogc {
namespace navi {
namespace test_constants {

constexpr double EPSILON = 0.001;
constexpr double DISTANCE_EPSILON = 1000.0;
constexpr double POSITION_EPSILON = 0.0001;

constexpr double SHANGHAI_LAT = 31.2304;
constexpr double SHANGHAI_LON = 121.4737;

constexpr double BEIJING_LAT = 39.9042;
constexpr double BEIJING_LON = 116.4074;

constexpr double EQUATOR_LAT = 0.0;
constexpr double EQUATOR_LON = 0.0;

constexpr double NORTH_POLE_LAT = 90.0;
constexpr double SOUTH_POLE_LAT = -90.0;

constexpr double DATE_LINE_LON = 180.0;
constexpr double PRIME_MERIDIAN_LON = 0.0;

constexpr double MAX_LATITUDE = 90.0;
constexpr double MIN_LATITUDE = -90.0;
constexpr double MAX_LONGITUDE = 180.0;
constexpr double MIN_LONGITUDE = -180.0;

constexpr double EARTH_RADIUS = 6371000.0;
constexpr double DEGREE_TO_METERS = 111319.0;

constexpr double DEFAULT_SPEED = 10.0;
constexpr double DEFAULT_COURSE = 45.0;
constexpr double DEFAULT_HEADING = 45.0;
constexpr double DEFAULT_TIMESTAMP = 1000.0;

constexpr double DEFAULT_ARRIVAL_RADIUS = 0.5;
constexpr double DEFAULT_SPEED_LIMIT = 15.0;

constexpr int DEFAULT_SATELLITE_COUNT = 8;
constexpr double DEFAULT_HDOP = 1.0;

constexpr int TEST_MMSI = 123456789;
constexpr const char* TEST_SHIP_NAME = "TestShip";
constexpr const char* TEST_CALLSIGN = "TEST";
constexpr const char* TEST_DESTINATION = "Shanghai";

constexpr double EXPECTED_SHANGHAI_BEIJING_DISTANCE = 1068000.0;
constexpr double EXPECTED_ONE_DEGREE_DISTANCE = 111195.0;
constexpr double EXPECTED_HALF_EARTH_CIRCUMFERENCE = 20015087.0;
constexpr double EXPECTED_QUARTER_EARTH_CIRCUMFERENCE = 10007543.0;

}
}
}

#endif
