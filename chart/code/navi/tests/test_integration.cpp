#include <gtest/gtest.h>
#include "ogc/navi/positioning/nmea_parser.h"
#include "ogc/navi/positioning/position_filter.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include "ogc/navi/route/waypoint.h"
#include "ogc/navi/route/route.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include "ogc/navi/track/track.h"
#include "ogc/navi/track/track_point.h"
#include "test_constants.h"

using namespace ogc::navi;
namespace tc = ogc::navi::test_constants;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = &NmeaParser::Instance();
        filter = new PositionFilter();
        converter = &CoordinateConverter::Instance();
        nav_calculator = &NavigationCalculator::Instance();
    }
    
    void TearDown() override {
        delete filter;
        if (route) {
            route->ReleaseReference();
            route = nullptr;
        }
        if (track) {
            track->ReleaseReference();
            track = nullptr;
        }
    }
    
    NmeaParser* parser;
    PositionFilter* filter;
    CoordinateConverter* converter;
    NavigationCalculator* nav_calculator;
    Route* route = nullptr;
    Track* track = nullptr;
};

TEST_F(IntegrationTest, NmeaToPositionFilter_Workflow) {
    std::string gga_sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    
    GgaData gga_data;
    ASSERT_TRUE(parser->ParseGGA(gga_sentence, gga_data));
    
    PositionData position;
    position.latitude = gga_data.latitude;
    position.longitude = gga_data.longitude;
    position.altitude = gga_data.altitude;
    position.timestamp = gga_data.utc_time;
    position.hdop = gga_data.hdop;
    position.satellite_count = gga_data.satellite_count;
    position.quality = PositionQuality::Gps;
    position.gps_quality = gga_data.gps_quality;
    
    PositionData filtered = filter->Filter(position);
    
    EXPECT_NEAR(filtered.latitude, gga_data.latitude, tc::EPSILON);
    EXPECT_NEAR(filtered.longitude, gga_data.longitude, tc::EPSILON);
    EXPECT_NEAR(filtered.altitude, gga_data.altitude, tc::EPSILON);
}

TEST_F(IntegrationTest, NmeaRmcToNavigation_Workflow) {
    std::string rmc_sentence = "$GPRMC,092751.000,A,5321.6802,N,00630.3372,W,12.5,45.0,191124,,,A*5C";
    
    RmcData rmc_data;
    ASSERT_TRUE(parser->ParseRMC(rmc_sentence, rmc_data));
    
    PositionData position;
    position.latitude = rmc_data.latitude;
    position.longitude = rmc_data.longitude;
    position.speed = rmc_data.speed_knots;
    position.course = rmc_data.track_angle;
    position.timestamp = rmc_data.utc_time;
    position.hdop = 1.0;
    position.satellite_count = 8;
    position.quality = PositionQuality::Gps;
    position.gps_quality = 1;
    
    PositionData filtered = filter->Filter(position);
    
    EXPECT_NEAR(filtered.latitude, rmc_data.latitude, tc::EPSILON);
    EXPECT_NEAR(filtered.longitude, rmc_data.longitude, tc::EPSILON);
    EXPECT_NEAR(filtered.speed, rmc_data.speed_knots, tc::EPSILON);
    EXPECT_NEAR(filtered.course, rmc_data.track_angle, tc::EPSILON);
}

TEST_F(IntegrationTest, RoutePlanningToNavigation_Workflow) {
    route = Route::Create();
    route->SetName("TestRoute");
    
    WaypointData wp1_data;
    wp1_data.id = "WP001";
    wp1_data.name = "Start";
    wp1_data.longitude = tc::SHANGHAI_LON;
    wp1_data.latitude = tc::SHANGHAI_LAT;
    Waypoint* wp1 = Waypoint::Create(wp1_data);
    
    WaypointData wp2_data;
    wp2_data.id = "WP002";
    wp2_data.name = "End";
    wp2_data.longitude = tc::BEIJING_LON;
    wp2_data.latitude = tc::BEIJING_LAT;
    Waypoint* wp2 = Waypoint::Create(wp2_data);
    
    route->AddWaypoint(wp1);
    route->AddWaypoint(wp2);
    wp1->ReleaseReference();
    wp2->ReleaseReference();
    wp1 = nullptr;
    wp2 = nullptr;
    
    EXPECT_EQ(route->GetWaypointCount(), 2);
    
    double total_distance = route->CalculateTotalDistance();
    EXPECT_GT(total_distance, 0);
    EXPECT_NEAR(total_distance, tc::EXPECTED_SHANGHAI_BEIJING_DISTANCE, tc::DISTANCE_EPSILON);
    
    PositionData current_pos;
    current_pos.latitude = tc::SHANGHAI_LAT + 0.5;
    current_pos.longitude = tc::SHANGHAI_LON + 0.5;
    current_pos.speed = 15.0;
    current_pos.course = 0.0;
    current_pos.timestamp = 1000.0;
    
    Waypoint* next_wp = route->GetWaypoint(1);
    ASSERT_NE(next_wp, nullptr);
    
    double bearing = converter->CalculateBearing(
        current_pos.latitude, current_pos.longitude,
        next_wp->GetLatitude(), next_wp->GetLongitude()
    );
    
    EXPECT_GE(bearing, 0.0);
    EXPECT_LE(bearing, 360.0);
    
    double distance_to_wp = converter->CalculateGreatCircleDistance(
        current_pos.latitude, current_pos.longitude,
        next_wp->GetLatitude(), next_wp->GetLongitude()
    );
    
    EXPECT_GT(distance_to_wp, 0);
    
    double ttg = nav_calculator->CalculateTimeToWaypoint(distance_to_wp, current_pos.speed);
    EXPECT_GT(ttg, 0);
}

TEST_F(IntegrationTest, TrackRecordingWorkflow) {
    track = Track::Create();
    track->SetName("TestTrack");
    
    for (int i = 0; i < 10; ++i) {
        TrackPointData tp_data;
        tp_data.longitude = tc::SHANGHAI_LON + i * 0.01;
        tp_data.latitude = tc::SHANGHAI_LAT + i * 0.01;
        tp_data.timestamp = 1000.0 + i * 60.0;
        tp_data.speed = 10.0 + i * 0.5;
        tp_data.course = 45.0;
        
        TrackPoint* tp = TrackPoint::Create(tp_data);
        track->AddPoint(tp);
        tp->ReleaseReference();
    }
    
    EXPECT_EQ(track->GetPointCount(), 10);
    
    track->Simplify(50.0);
    EXPECT_LE(track->GetPointCount(), 10);
}

TEST_F(IntegrationTest, FullNavigationWorkflow) {
    route = Route::Create();
    route->SetName("FullTestRoute");
    
    WaypointData wp_data;
    wp_data.id = "WP_START";
    wp_data.name = "Start Point";
    wp_data.longitude = tc::EQUATOR_LON;
    wp_data.latitude = tc::EQUATOR_LAT;
    Waypoint* wp = Waypoint::Create(wp_data);
    route->AddWaypoint(wp);
    wp->ReleaseReference();
    wp = nullptr;
    
    wp_data.id = "WP_END";
    wp_data.name = "End Point";
    wp_data.longitude = tc::EQUATOR_LON + 1.0;
    wp_data.latitude = tc::EQUATOR_LAT + 1.0;
    wp = Waypoint::Create(wp_data);
    route->AddWaypoint(wp);
    wp->ReleaseReference();
    wp = nullptr;
    
    PositionData current_pos;
    current_pos.latitude = tc::EQUATOR_LAT + 0.1;
    current_pos.longitude = tc::EQUATOR_LON + 0.1;
    current_pos.speed = 10.0;
    current_pos.course = 45.0;
    current_pos.timestamp = 1000.0;
    current_pos.hdop = 1.0;
    current_pos.satellite_count = 8;
    current_pos.quality = PositionQuality::Gps;
    current_pos.gps_quality = 1;
    
    PositionData filtered = filter->Filter(current_pos);
    
    Waypoint* dest = route->GetWaypoint(1);
    ASSERT_NE(dest, nullptr);
    
    double distance = converter->CalculateGreatCircleDistance(
        filtered.latitude, filtered.longitude,
        dest->GetLatitude(), dest->GetLongitude()
    );
    
    double bearing = converter->CalculateBearing(
        filtered.latitude, filtered.longitude,
        dest->GetLatitude(), dest->GetLongitude()
    );
    
    GeoPoint position(filtered.longitude, filtered.latitude);
    GeoPoint start(tc::EQUATOR_LON, tc::EQUATOR_LAT);
    GeoPoint end(tc::EQUATOR_LON + 1.0, tc::EQUATOR_LAT + 1.0);
    
    double xtd = nav_calculator->CalculateCrossTrackError(position, start, end);
    double ttg = nav_calculator->CalculateTimeToWaypoint(distance, filtered.speed);
    
    EXPECT_GT(distance, 0);
    EXPECT_GE(bearing, 0);
    EXPECT_LE(bearing, 360);
    EXPECT_NE(xtd, 0);
    EXPECT_GT(ttg, 0);
}

TEST_F(IntegrationTest, CoordinateConverterConsistency) {
    double lat1 = tc::SHANGHAI_LAT;
    double lon1 = tc::SHANGHAI_LON;
    double lat2 = tc::BEIJING_LAT;
    double lon2 = tc::BEIJING_LON;
    
    double distance1 = converter->CalculateGreatCircleDistance(lat1, lon1, lat2, lon2);
    
    double bearing = converter->CalculateBearing(lat1, lon1, lat2, lon2);
    
    double dest_lat, dest_lon;
    converter->CalculateDestination(lat1, lon1, bearing, distance1, dest_lat, dest_lon);
    
    EXPECT_NEAR(dest_lat, lat2, 0.01);
    EXPECT_NEAR(dest_lon, lon2, 0.01);
}

TEST_F(IntegrationTest, PositionFilterWithMultipleUpdates) {
    PositionData pos1;
    pos1.latitude = tc::SHANGHAI_LAT;
    pos1.longitude = tc::SHANGHAI_LON;
    pos1.speed = 10.0;
    pos1.course = 45.0;
    pos1.timestamp = 1000.0;
    pos1.hdop = 1.0;
    pos1.satellite_count = 8;
    pos1.quality = PositionQuality::Gps;
    pos1.gps_quality = 1;
    
    filter->Filter(pos1);
    
    for (int i = 1; i <= 5; ++i) {
        PositionData pos;
        pos.latitude = tc::SHANGHAI_LAT + i * 0.001;
        pos.longitude = tc::SHANGHAI_LON + i * 0.001;
        pos.speed = 10.0 + i * 0.5;
        pos.course = 45.0 + i;
        pos.timestamp = 1000.0 + i * 60.0;
        pos.hdop = 1.0;
        pos.satellite_count = 8;
        pos.quality = PositionQuality::Gps;
        pos.gps_quality = 1;
        
        filter->Filter(pos);
    }
    
    FilterStatistics stats = filter->GetStatistics();
    EXPECT_EQ(stats.total_received, 6);
    EXPECT_GT(stats.valid_count, 0);
}

TEST_F(IntegrationTest, RouteWithMultipleWaypoints) {
    route = Route::Create();
    route->SetName("MultiWaypointRoute");
    
    std::vector<std::pair<double, double>> coords = {
        {tc::SHANGHAI_LAT, tc::SHANGHAI_LON},
        {tc::SHANGHAI_LAT + 1.0, tc::SHANGHAI_LON + 1.0},
        {tc::SHANGHAI_LAT + 2.0, tc::SHANGHAI_LON + 2.0},
        {tc::BEIJING_LAT, tc::BEIJING_LON}
    };
    
    for (size_t i = 0; i < coords.size(); ++i) {
        WaypointData wp_data;
        wp_data.id = "WP" + std::to_string(i);
        wp_data.name = "Waypoint " + std::to_string(i);
        wp_data.latitude = coords[i].first;
        wp_data.longitude = coords[i].second;
        
        Waypoint* wp = Waypoint::Create(wp_data);
        route->AddWaypoint(wp);
        wp->ReleaseReference();
    }
    
    EXPECT_EQ(route->GetWaypointCount(), 4);
    
    double total_distance = route->CalculateTotalDistance();
    EXPECT_GT(total_distance, 0);
    
    double segment1 = converter->CalculateGreatCircleDistance(
        coords[0].first, coords[0].second,
        coords[1].first, coords[1].second
    );
    double segment2 = converter->CalculateGreatCircleDistance(
        coords[1].first, coords[1].second,
        coords[2].first, coords[2].second
    );
    
    EXPECT_GT(total_distance, segment1 + segment2);
}
