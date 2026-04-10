/**
 * @file test_sdk_c_api_navigation.cpp
 * @brief Unit tests for SDK C API Navigation module
 * 
 * Note: Some tests have been moved to pending_tests/test_sdk_c_api_navigation_pending.cpp
 * because they use APIs that don't exist in the current SDK:
 * - ogc_waypoint_get_description/set_description
 * - ogc_waypoint_get_arrival_radius
 * - ogc_waypoint_set_turn_radius/get_turn_radius
 * - ogc_waypoint_set_type/get_type
 * - OGC_WAYPOINT_TYPE_*
 * - ogc_route_insert_waypoint
 * - ogc_route_clear
 * - ogc_route_reverse
 * - ogc_position_manager_set_position/get_position
 * - ogc_position_manager_set_callback
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiWaypointTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiWaypointTest, WaypointCreate_Destroy) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointGetPosition) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    double lat = ogc_waypoint_get_latitude(wp);
    double lon = ogc_waypoint_get_longitude(wp);
    EXPECT_DOUBLE_EQ(lat, 116.0);
    EXPECT_DOUBLE_EQ(lon, 39.0);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointSetName) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_name(wp, "Beijing");
    
    const char* name = ogc_waypoint_get_name(wp);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "Beijing");
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointSetArrivalRadius) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_arrival_radius(wp, 0.5);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointIsArrival) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    int is_arrival = ogc_waypoint_is_arrival(wp);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointNullPointer) {
    EXPECT_EQ(ogc_waypoint_get_name(nullptr), nullptr);
    EXPECT_DOUBLE_EQ(ogc_waypoint_get_latitude(nullptr), 0.0);
    EXPECT_DOUBLE_EQ(ogc_waypoint_get_longitude(nullptr), 0.0);
}

TEST_F(SdkCApiWaypointTest, WaypointArrayDestroy) {
    ogc_waypoint_t* waypoints[3];
    
    for (int i = 0; i < 3; ++i) {
        waypoints[i] = ogc_waypoint_create(116.0 + i, 39.0 + i);
        ASSERT_NE(waypoints[i], nullptr);
    }
    
    ogc_waypoint_array_destroy(waypoints, 3);
}

TEST_F(SdkCApiWaypointTest, WaypointArrayDestroy_NullArray) {
    ogc_waypoint_array_destroy(nullptr, 5);
}

class SdkCApiRouteTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiRouteTest, RouteCreate_Destroy) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetName_SetName) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_route_set_name(route, "Test Route");
    
    const char* name = ogc_route_get_name(route);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "Test Route");
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetId) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    const char* id = ogc_route_get_id(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetStatus) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_route_status_e status = ogc_route_get_status(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetTotalDistance) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    double distance = ogc_route_get_total_distance(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteAddWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    
    ogc_route_add_waypoint(route, wp1);
    
    size_t count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 1);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* retrieved = ogc_route_get_waypoint(route, 0);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteRemoveWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    
    ogc_route_add_waypoint(route, wp1);
    
    size_t count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 1);
    
    ogc_route_remove_waypoint(route, 0);
    
    count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 0);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetCurrentWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* current = ogc_route_get_current_waypoint(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteAdvanceToNextWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ogc_waypoint_t* wp2 = ogc_waypoint_create(117.0, 40.0);
    ASSERT_NE(wp1, nullptr);
    ASSERT_NE(wp2, nullptr);
    
    ogc_route_add_waypoint(route, wp1);
    ogc_route_add_waypoint(route, wp2);
    
    int result = ogc_route_advance_to_next_waypoint(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteNullPointer) {
    EXPECT_EQ(ogc_route_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_route_get_waypoint_count(nullptr), 0);
    EXPECT_EQ(ogc_route_get_waypoint(nullptr, 0), nullptr);
    EXPECT_DOUBLE_EQ(ogc_route_get_total_distance(nullptr), 0.0);
}

class SdkCApiNavigationEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineCreate_Destroy) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetBearingToWaypoint) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double bearing = ogc_navigation_engine_get_bearing_to_waypoint(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetDistanceToWaypoint) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double distance = ogc_navigation_engine_get_distance_to_waypoint(engine);
    
    ogc_navigation_engine_destroy(engine);
}
