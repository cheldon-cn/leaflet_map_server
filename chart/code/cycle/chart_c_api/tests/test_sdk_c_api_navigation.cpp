/**
 * @file test_sdk_c_api_navigation.cpp
 * @brief Unit tests for SDK C API Navigation module
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
    
    double lat, lon;
    ogc_waypoint_get_position(wp, &lat, &lon);
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

TEST_F(SdkCApiWaypointTest, WaypointSetDescription) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_description(wp, "Capital of China");
    
    const char* desc = ogc_waypoint_get_description(wp);
    ASSERT_NE(desc, nullptr);
    EXPECT_STREQ(desc, "Capital of China");
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointSetArrivalRadius) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_arrival_radius(wp, 0.5);
    
    double radius = ogc_waypoint_get_arrival_radius(wp);
    EXPECT_DOUBLE_EQ(radius, 0.5);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointSetTurnRadius) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_turn_radius(wp, 0.3);
    
    double radius = ogc_waypoint_get_turn_radius(wp);
    EXPECT_DOUBLE_EQ(radius, 0.3);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointSetType) {
    ogc_waypoint_t* wp = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp, nullptr);
    
    ogc_waypoint_set_type(wp, OGC_WAYPOINT_TYPE_TURN);
    
    ogc_waypoint_type_e type = ogc_waypoint_get_type(wp);
    EXPECT_EQ(type, OGC_WAYPOINT_TYPE_TURN);
    
    ogc_waypoint_destroy(wp);
}

TEST_F(SdkCApiWaypointTest, WaypointNullPointer) {
    EXPECT_EQ(ogc_waypoint_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_waypoint_get_description(nullptr), nullptr);
    EXPECT_DOUBLE_EQ(ogc_waypoint_get_arrival_radius(nullptr), 0.0);
    EXPECT_EQ(ogc_waypoint_get_type(nullptr), OGC_WAYPOINT_TYPE_NORMAL);
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

TEST_F(SdkCApiRouteTest, RouteSetName) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_route_set_name(route, "Test Route");
    
    const char* name = ogc_route_get_name(route);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "Test Route");
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteAddWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    
    int result = ogc_route_add_waypoint(route, wp1);
    EXPECT_EQ(result, 0);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteInsertWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* wp2 = ogc_waypoint_create(117.0, 40.0);
    ASSERT_NE(wp2, nullptr);
    
    int result = ogc_route_insert_waypoint(route, 0, wp2);
    EXPECT_EQ(result, 0);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteRemoveWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    ogc_route_add_waypoint(route, wp1);
    
    int count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 1);
    
    int result = ogc_route_remove_waypoint(route, 0);
    EXPECT_EQ(result, 0);
    
    count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 0);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetWaypoint) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    ogc_waypoint_set_name(wp1, "WP1");
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* retrieved = ogc_route_get_waypoint(route, 0);
    ASSERT_NE(retrieved, nullptr);
    
    const char* name = ogc_waypoint_get_name(retrieved);
    EXPECT_STREQ(name, "WP1");
    
    ogc_waypoint_t* invalid = ogc_route_get_waypoint(route, 99);
    EXPECT_EQ(invalid, nullptr);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetWaypointCount) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    int count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 5; ++i) {
        ogc_waypoint_t* wp = ogc_waypoint_create(116.0 + i, 39.0 + i);
        ASSERT_NE(wp, nullptr);
        ogc_route_add_waypoint(route, wp);
    }
    
    count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 5);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteGetTotalDistance) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* wp2 = ogc_waypoint_create(117.0, 40.0);
    ASSERT_NE(wp2, nullptr);
    ogc_route_add_waypoint(route, wp2);
    
    double distance = ogc_route_get_total_distance(route);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteClear) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_waypoint_t* wp = ogc_waypoint_create(116.0 + i, 39.0 + i);
        ASSERT_NE(wp, nullptr);
        ogc_route_add_waypoint(route, wp);
    }
    
    int count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 5);
    
    ogc_route_clear(route);
    
    count = ogc_route_get_waypoint_count(route);
    EXPECT_EQ(count, 0);
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteReverse) {
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(116.0, 39.0);
    ASSERT_NE(wp1, nullptr);
    ogc_waypoint_set_name(wp1, "WP1");
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* wp2 = ogc_waypoint_create(117.0, 40.0);
    ASSERT_NE(wp2, nullptr);
    ogc_waypoint_set_name(wp2, "WP2");
    ogc_route_add_waypoint(route, wp2);
    
    ogc_route_reverse(route);
    
    ogc_waypoint_t* first = ogc_route_get_waypoint(route, 0);
    ASSERT_NE(first, nullptr);
    EXPECT_STREQ(ogc_waypoint_get_name(first), "WP2");
    
    ogc_route_destroy(route);
}

TEST_F(SdkCApiRouteTest, RouteNullPointer) {
    EXPECT_EQ(ogc_route_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_route_get_waypoint_count(nullptr), 0);
    EXPECT_EQ(ogc_route_get_waypoint(nullptr, 0), nullptr);
    EXPECT_DOUBLE_EQ(ogc_route_get_total_distance(nullptr), 0.0);
}

class SdkCApiRouteManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiRouteManagerTest, RouteManagerCreate_Destroy) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerAddRoute) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    ogc_route_set_name(route, "Test Route");
    
    int result = ogc_route_manager_add_route(rm, route);
    EXPECT_EQ(result, 0);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerRemoveRoute) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    ogc_route_set_name(route, "Test Route");
    ogc_route_manager_add_route(rm, route);
    
    int count = ogc_route_manager_get_route_count(rm);
    EXPECT_EQ(count, 1);
    
    int result = ogc_route_manager_remove_route(rm, "Test Route");
    EXPECT_EQ(result, 0);
    
    count = ogc_route_manager_get_route_count(rm);
    EXPECT_EQ(count, 0);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerGetRoute) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    ogc_route_set_name(route, "Test Route");
    ogc_route_manager_add_route(rm, route);
    
    ogc_route_t* retrieved = ogc_route_manager_get_route(rm, "Test Route");
    ASSERT_NE(retrieved, nullptr);
    
    ogc_route_t* invalid = ogc_route_manager_get_route(rm, "Nonexistent");
    EXPECT_EQ(invalid, nullptr);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerGetRouteCount) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    int count = ogc_route_manager_get_route_count(rm);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 3; ++i) {
        ogc_route_t* route = ogc_route_create();
        ASSERT_NE(route, nullptr);
        char name[32];
        snprintf(name, sizeof(name), "Route_%d", i);
        ogc_route_set_name(route, name);
        ogc_route_manager_add_route(rm, route);
    }
    
    count = ogc_route_manager_get_route_count(rm);
    EXPECT_EQ(count, 3);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerSetActiveRoute) {
    ogc_route_manager_t* rm = ogc_route_manager_create();
    ASSERT_NE(rm, nullptr);
    
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    ogc_route_set_name(route, "Test Route");
    ogc_route_manager_add_route(rm, route);
    
    int result = ogc_route_manager_set_active_route(rm, "Test Route");
    EXPECT_EQ(result, 0);
    
    ogc_route_t* active = ogc_route_manager_get_active_route(rm);
    ASSERT_NE(active, nullptr);
    
    ogc_route_manager_destroy(rm);
}

TEST_F(SdkCApiRouteManagerTest, RouteManagerNullPointer) {
    EXPECT_EQ(ogc_route_manager_get_route_count(nullptr), 0);
    EXPECT_EQ(ogc_route_manager_get_route(nullptr, "test"), nullptr);
    EXPECT_EQ(ogc_route_manager_get_active_route(nullptr), nullptr);
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

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineInitialize) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    int result = ogc_navigation_engine_initialize(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineSetRoute) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_route_t* route = ogc_route_create();
    ASSERT_NE(route, nullptr);
    
    int result = ogc_navigation_engine_set_route(engine, route);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineStart) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    int result = ogc_navigation_engine_start(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineStop) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_navigation_engine_stop(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetPosition) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double lat, lon;
    int result = ogc_navigation_engine_get_position(engine, &lat, &lon);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetBearing) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double bearing = ogc_navigation_engine_get_bearing(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetDistanceToWaypoint) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double distance = ogc_navigation_engine_get_distance_to_waypoint(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineGetCrossTrackError) {
    ogc_navigation_engine_t* engine = ogc_navigation_engine_create();
    ASSERT_NE(engine, nullptr);
    
    double xte = ogc_navigation_engine_get_cross_track_error(engine);
    
    ogc_navigation_engine_destroy(engine);
}

TEST_F(SdkCApiNavigationEngineTest, NavigationEngineNullPointer) {
    EXPECT_DOUBLE_EQ(ogc_navigation_engine_get_bearing(nullptr), 0.0);
    EXPECT_DOUBLE_EQ(ogc_navigation_engine_get_distance_to_waypoint(nullptr), 0.0);
    EXPECT_DOUBLE_EQ(ogc_navigation_engine_get_cross_track_error(nullptr), 0.0);
}

class SdkCApiPositionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiPositionManagerTest, PositionManagerCreate_Destroy) {
    ogc_position_manager_t* pm = ogc_position_manager_create();
    ASSERT_NE(pm, nullptr);
    
    ogc_position_manager_destroy(pm);
}

TEST_F(SdkCApiPositionManagerTest, PositionManagerInitialize) {
    ogc_position_manager_t* pm = ogc_position_manager_create();
    ASSERT_NE(pm, nullptr);
    
    int result = ogc_position_manager_initialize(pm);
    
    ogc_position_manager_destroy(pm);
}

TEST_F(SdkCApiPositionManagerTest, PositionManagerSetPosition) {
    ogc_position_manager_t* pm = ogc_position_manager_create();
    ASSERT_NE(pm, nullptr);
    
    ogc_position_manager_set_position(pm, 116.0, 39.0, 10.5, 180.0);
    
    double lat, lon, speed, course;
    ogc_position_manager_get_position(pm, &lat, &lon, &speed, &course);
    EXPECT_DOUBLE_EQ(lat, 116.0);
    EXPECT_DOUBLE_EQ(lon, 39.0);
    EXPECT_DOUBLE_EQ(speed, 10.5);
    EXPECT_DOUBLE_EQ(course, 180.0);
    
    ogc_position_manager_destroy(pm);
}

TEST_F(SdkCApiPositionManagerTest, PositionManagerSetCallback) {
    ogc_position_manager_t* pm = ogc_position_manager_create();
    ASSERT_NE(pm, nullptr);
    
    ogc_position_manager_set_callback(pm, nullptr, nullptr);
    
    ogc_position_manager_destroy(pm);
}

TEST_F(SdkCApiPositionManagerTest, PositionManagerNullPointer) {
    EXPECT_NE(ogc_position_manager_initialize(nullptr), 0);
}
