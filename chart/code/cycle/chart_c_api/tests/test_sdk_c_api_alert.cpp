/**
 * @file test_sdk_c_api_alert.cpp
 * @brief Unit tests for SDK C API Alert module
 * 
 * Note: Some tests have been moved to pending_tests/test_sdk_c_api_alert_pending.cpp
 * because they use APIs that don't exist in the current SDK:
 * - ogc_alert_set_id/get_id - not implemented
 * - ogc_alert_set_severity/get_severity - not implemented
 * - ogc_alert_set_message - not implemented
 * - ogc_alert_set_timestamp - not implemented
 * - ogc_alert_set_acknowledged/is_acknowledged - not implemented
 * - ogc_alert_engine_add_alert/remove_alert/get_alert - not implemented
 * - ogc_alert_engine_get_alert_count/get_alerts/clear_alerts - not implemented
 * - ogc_alert_engine_set_callback - not implemented
 * - ogc_alert_zone_* - not implemented
 * - ogc_ais_manager_add_target/remove_target/get_target_count/set_callback - not implemented
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
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_COLLISION, OGC_ALERT_LEVEL_ALARM, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertGetType) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_DEPTH, OGC_ALERT_LEVEL_WARNING, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_type_e type = ogc_alert_get_type(alert);
    EXPECT_EQ(type, OGC_ALERT_TYPE_DEPTH);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertGetLevel) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_WEATHER, OGC_ALERT_LEVEL_ALARM, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_level_e level = ogc_alert_get_level(alert);
    EXPECT_EQ(level, OGC_ALERT_LEVEL_ALARM);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertGetMessage) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_COLLISION, OGC_ALERT_LEVEL_WARNING, "Test alert message");
    ASSERT_NE(alert, nullptr);
    
    const char* message = ogc_alert_get_message(alert);
    ASSERT_NE(message, nullptr);
    EXPECT_STREQ(message, "Test alert message");
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertGetPosition) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_SPEED, OGC_ALERT_LEVEL_INFO, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    ogc_coordinate_t pos = ogc_alert_get_position(alert);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertSetPosition) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_RESTRICTED_AREA, OGC_ALERT_LEVEL_ALARM, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    ogc_coordinate_t pos;
    pos.x = 116.0;
    pos.y = 39.0;
    ogc_alert_set_position(alert, &pos);
    
    ogc_coordinate_t retrieved = ogc_alert_get_position(alert);
    EXPECT_DOUBLE_EQ(retrieved.x, 116.0);
    EXPECT_DOUBLE_EQ(retrieved.y, 39.0);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertGetTimestamp) {
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_CHANNEL, OGC_ALERT_LEVEL_WARNING, "Test alert");
    ASSERT_NE(alert, nullptr);
    
    double timestamp = ogc_alert_get_timestamp(alert);
    
    ogc_alert_destroy(alert);
}

TEST_F(SdkCApiAlertTest, AlertArrayDestroy) {
    ogc_alert_t* alerts[3];
    
    for (int i = 0; i < 3; ++i) {
        alerts[i] = ogc_alert_create(OGC_ALERT_TYPE_DEPTH, OGC_ALERT_LEVEL_INFO, "Test");
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

TEST_F(SdkCApiAlertEngineTest, AlertEngineShutdown) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_engine_shutdown(engine);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineCheckAll) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_engine_check_all(engine);
    
    ogc_alert_engine_destroy(engine);
}

TEST_F(SdkCApiAlertEngineTest, AlertEngineAcknowledgeAlert) {
    ogc_alert_engine_t* engine = ogc_alert_engine_create();
    ASSERT_NE(engine, nullptr);
    
    ogc_alert_t* alert = ogc_alert_create(OGC_ALERT_TYPE_COLLISION, OGC_ALERT_LEVEL_ALARM, "Test");
    ASSERT_NE(alert, nullptr);
    
    ogc_alert_engine_acknowledge_alert(engine, alert);
    
    ogc_alert_destroy(alert);
    ogc_alert_engine_destroy(engine);
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

TEST_F(SdkCApiAisManagerTest, AisManagerShutdown) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_manager_shutdown(mgr);
    
    ogc_ais_manager_destroy(mgr);
}

TEST_F(SdkCApiAisManagerTest, AisManagerGetTarget) {
    ogc_ais_manager_t* mgr = ogc_ais_manager_create();
    ASSERT_NE(mgr, nullptr);
    
    ogc_ais_target_t* target = ogc_ais_manager_get_target(mgr, 123456789);
    
    ogc_ais_manager_destroy(mgr);
}
