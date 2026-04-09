/**
 * @file test_sdk_c_api_alert.cpp
 * @brief Unit tests for SDK C API Alert module
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiAlertTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiAlertTest, AlertCreate_Destroy) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetId) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_id(alert, 12345);
    
    int64_t id = ogc_alert_get_id(alert);
    EXPECT_EQ(id, 12345);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetType) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_type(alert, OGC_ALERT_TYPE_DANGER);
    
    ogc_alert_type_e type = ogc_alert_get_type(alert);
    EXPECT_EQ(type, OGC_ALERT_TYPE_DANGER);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetSeverity) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_severity(alert, OGC_ALERT_SEVERITY_HIGH);
    
    ogc_alert_severity_e severity = ogc_alert_get_severity(alert);
    EXPECT_EQ(severity, OGC_ALERT_SEVERITY_HIGH);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetMessage) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_message(alert, "Test alert message");
    
    const char* message = ogc_alert_get_message(alert);
    ASSERT_NE(message, nullptr);
    EXPECT_STREQ(message, "Test alert message");
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetPosition) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_position(alert, 116.0, 39.0);
    
    double lat, lon;
    ogc_alert_get_position(alert, &lat, &lon);
    EXPECT_DOUBLE_EQ(lat, 116.0);
    EXPECT_DOUBLE_EQ(lon, 39.0);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetTimestamp) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    int64_t timestamp = 1234567890;
    ogc_alert_set_timestamp(alert, timestamp);
    
    int64_t retrieved = ogc_alert_get_timestamp(alert);
    EXPECT_EQ(retrieved, timestamp);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetAcknowledged) {
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_set_acknowledged(alert, 1);
    
    int acknowledged = ogc_alert_is_acknowledged(alert);
    EXPECT_NE(acknowledged, 0);
    
    ogc_alert_set_acknowledged(alert, 0);
    acknowledged = ogc_alert_is_acknowledged(alert);
    EXPECT_EQ(acknowledged, 0);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertNullPointer) {
    EXPECT_EQ(ogc_alert_get_id(nullptr), 0);
    EXPECT_EQ(ogc_alert_get_type(nullptr), OGC_ALERT_TYPE_INFO);
    EXPECT_EQ(ogc_alert_get_severity(nullptr), OGC_ALERT_SEVERITY_LOW);
    EXPECT_EQ(ogc_alert_get_message(nullptr), nullptr);
    EXPECT_EQ(ogc_alert_is_acknowledged(nullptr), 0);
}

TEST_F(SdkCApiAlertTest, AlertArrayDestroy) {
    ogc_alert_t* alerts[3];
    
    for (int i = 0; i < 3; ++i) {
        alerts[i] = ogc_alert_create();
        ASSERT_NE(alerts[i], nullptr);
    }
    
    ogc_alert_array_destroy(alerts, 3);
}

TEST_F(SdkCApiAlertTest, AlertArrayDestroy_NullArray) {
    ogc_alert_array_destroy(nullptr, 5);
}

class SdkCApiAlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiAlertEngineTest, AlertEngineCreate_Destroy) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineInitialize) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    int result = ogc_alert_engine_initialize(engine);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineAddAlert) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    
    int result = ogc_alert_engine_add_alert(engine, alert);
    EXPECT_EQ(result, 0);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineRemoveAlert) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    ogc_alert_set_id(alert, 123);
    
    ogc_alert_engine_add_alert(engine, alert);
    
    int count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 1);
    
    int result = ogc_alert_engine_remove_alert(engine, 123);
    EXPECT_EQ(result, 0);
    
    count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 0);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineGetAlert) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_t* alert = ogc_alert_create();
    ASSERT_NE(alert, nullptr);
    ogc_alert_set_id(alert, 123);
    ogc_alert_set_message(alert, "Test alert");
    
    ogc_alert_engine_add_alert(engine, alert);
    
    ogc_alert_t* retrieved = ogc_alert_engine_get_alert(engine, 123);
    ASSERT_NE(retrieved, nullptr);
    
    const char* message = ogc_alert_get_message(retrieved);
    EXPECT_STREQ(message, "Test alert");
    
    ogc_alert_t* invalid = ogc_alert_engine_get_alert(engine, 999);
    EXPECT_EQ(invalid, nullptr);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineGetAlertCount) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    int count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 5; ++i) {
        ogc_alert_t* alert = ogc_alert_create();
        ASSERT_NE(alert, nullptr);
        ogc_alert_set_id(alert, i + 1);
        ogc_alert_engine_add_alert(engine, alert);
    }
    
    count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 5);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineGetAlerts) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_alert_t* alert = ogc_alert_create();
        ASSERT_NE(alert, nullptr);
        ogc_alert_set_id(alert, i + 1);
        ogc_alert_engine_add_alert(engine, alert);
    }
    
    ogc_alert_t** alerts = nullptr;
    int count = 0;
    int result = ogc_alert_engine_get_alerts(engine, &alerts, &count);
    
    if (alerts != nullptr && count > 0) {
        EXPECT_EQ(count, 5);
    }
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineClearAlerts) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_alert_t* alert = ogc_alert_create();
        ASSERT_NE(alert, nullptr);
        ogc_alert_set_id(alert, i + 1);
        ogc_alert_engine_add_alert(engine, alert);
    }
    
    int count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 5);
    
    ogc_alert_engine_clear_alerts(engine);
    
    count = ogc_alert_engine_get_alert_count(engine);
    EXPECT_EQ(count, 0);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineSetCallback) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_engine_set_callback(engine, nullptr, nullptr);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineNullPointer) {
    EXPECT_EQ(ogc_alert_engine_get_alert_count(nullptr), 0);
    EXPECT_EQ(ogc_alert_engine_get_alert(nullptr, 123), nullptr);
    EXPECT_NE(ogc_alert_engine_add_alert(nullptr, nullptr), 0);
}

class SdkCApiAlertZoneTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiAlertZoneTest, AlertZoneCreate_Destroy) {
    ogc_alert_zone_t* zone = ogc_alert_zone_create();
    ASSERT_NE(zone, nullptr);
    
    ogc_alert_zone_destroy(zone);
}

TEST_F(SdkCApiAlertZoneTest, AlertZoneSetGeometry) {
    ogc_alert_zone_t* zone = ogc_alert_zone_create();
    ASSERT_NE(zone, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    int result = ogc_alert_zone_set_geometry(zone, geom);
    EXPECT_EQ(result, 0);
    
    ogc_alert_zone_destroy(zone);
}

TEST_F(SdkCApiAlertZoneTest, AlertZoneSetName) {
    ogc_alert_zone_t* zone = ogc_alert_zone_create();
    ASSERT_NE(zone, nullptr);
    
    ogc_alert_zone_set_name(zone, "Danger Zone");
    
    const char* name = ogc_alert_zone_get_name(zone);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "Danger Zone");
    
    ogc_alert_zone_destroy(zone);
}

TEST_F(SdkCApiAlertZoneTest, AlertZoneSetAlertType) {
    ogc_alert_zone_t* zone = ogc_alert_zone_create();
    ASSERT_NE(zone, nullptr);
    
    ogc_alert_zone_set_alert_type(zone, OGC_ALERT_TYPE_CAUTION);
    
    ogc_alert_type_e type = ogc_alert_zone_get_alert_type(zone);
    EXPECT_EQ(type, OGC_ALERT_TYPE_CAUTION);
    
    ogc_alert_zone_destroy(zone);
}

TEST_F(SdkCApiAlertZoneTest, AlertZoneSetEnabled) {
    ogc_alert_zone_t* zone = ogc_alert_zone_create();
    ASSERT_NE(zone, nullptr);
    
    ogc_alert_zone_set_enabled(zone, 1);
    
    int enabled = ogc_alert_zone_is_enabled(zone);
    EXPECT_NE(enabled, 0);
    
    ogc_alert_zone_set_enabled(zone, 0);
    enabled = ogc_alert_zone_is_enabled(zone);
    EXPECT_EQ(enabled, 0);
    
    ogc_alert_zone_destroy(zone);
}

TEST_F(SdkCApiAlertZoneTest, AlertZoneNullPointer) {
    EXPECT_EQ(ogc_alert_zone_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_alert_zone_get_alert_type(nullptr), OGC_ALERT_TYPE_INFO);
    EXPECT_EQ(ogc_alert_zone_is_enabled(nullptr), 0);
}

class SdkCApiAisManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiAisManagerTest, AisManagerCreate_Destroy) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerInitialize) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    int result = ogc_ais_manager_initialize(mgr);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerAddTarget) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_target_t target;
    target.mmsi = 123456789;
    target.latitude = 116.0;
    target.longitude = 39.0;
    target.speed = 10.5;
    target.course = 180.0;
    
    int result = ogc_ais_manager_add_target(mgr, &target);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerRemoveTarget) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_target_t target;
    target.mmsi = 123456789;
    target.latitude = 116.0;
    target.longitude = 39.0;
    target.speed = 10.5;
    target.course = 180.0;
    
    ogc_ais_manager_add_target(mgr, &target);
    
    int result = ogc_ais_manager_remove_target(mgr, 123456789);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerGetTarget) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_target_t target;
    target.mmsi = 123456789;
    target.latitude = 116.0;
    target.longitude = 39.0;
    target.speed = 10.5;
    target.course = 180.0;
    
    ogc_ais_manager_add_target(mgr, &target);
    
    ogc_ais_target_t* retrieved = ogc_ais_manager_get_target(mgr, 123456789);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerGetTargetCount) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    int count = ogc_ais_manager_get_target_count(mgr);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerSetCallback) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_manager_set_callback(mgr, nullptr, nullptr);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerNullPointer) {
    EXPECT_EQ(ogc_ais_manager_get_target_count(nullptr), 0);
    EXPECT_EQ(ogc_ais_manager_get_target(nullptr, 123), nullptr);
}
