/**
 * @file test_sdk_c_api_common.cpp
 * @brief Unit tests for SDK C API common utility functions
 */

#include <gtest/gtest.h>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiCommonTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiCommonTest, ErrorGetMessage_Success) {
    const char* msg = ogc_error_get_message(OGC_SUCCESS);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Success");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_InvalidParam) {
    const char* msg = ogc_error_get_message(OGC_ERROR_INVALID_PARAM);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Invalid parameter");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_NullPointer) {
    const char* msg = ogc_error_get_message(OGC_ERROR_NULL_POINTER);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Null pointer");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_OutOfMemory) {
    const char* msg = ogc_error_get_message(OGC_ERROR_OUT_OF_MEMORY);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Out of memory");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_NotFound) {
    const char* msg = ogc_error_get_message(OGC_ERROR_NOT_FOUND);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Not found");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_OperationFailed) {
    const char* msg = ogc_error_get_message(OGC_ERROR_OPERATION_FAILED);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Operation failed");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_InvalidState) {
    const char* msg = ogc_error_get_message(OGC_ERROR_INVALID_STATE);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Invalid state");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_IoError) {
    const char* msg = ogc_error_get_message(OGC_ERROR_IO_ERROR);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "I/O error");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_ParseError) {
    const char* msg = ogc_error_get_message(OGC_ERROR_PARSE_ERROR);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Parse error");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_Unsupported) {
    const char* msg = ogc_error_get_message(OGC_ERROR_UNSUPPORTED);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Unsupported operation");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_Timeout) {
    const char* msg = ogc_error_get_message(OGC_ERROR_TIMEOUT);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Timeout");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_PermissionDenied) {
    const char* msg = ogc_error_get_message(OGC_ERROR_PERMISSION_DENIED);
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Permission denied");
}

TEST_F(SdkCApiCommonTest, ErrorGetMessage_UnknownError) {
    const char* msg = ogc_error_get_message(static_cast<ogc_error_code_e>(-999));
    ASSERT_NE(msg, nullptr);
    EXPECT_STREQ(msg, "Unknown error");
}

TEST_F(SdkCApiCommonTest, GeometryArrayDestroy_NullArray) {
    ogc_geometry_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, GeometryArrayDestroy_EmptyArray) {
    ogc_geometry_t* array[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    ogc_geometry_array_destroy(array, 5);
}

TEST_F(SdkCApiCommonTest, GeometryArrayDestroy_WithPoints) {
    ogc_geometry_t* array[3];
    array[0] = ogc_point_create(0.0, 0.0);
    array[1] = ogc_point_create(1.0, 1.0);
    array[2] = ogc_point_create(2.0, 2.0);
    
    ASSERT_NE(array[0], nullptr);
    ASSERT_NE(array[1], nullptr);
    ASSERT_NE(array[2], nullptr);
    
    ogc_geometry_array_destroy(array, 3);
    
    EXPECT_EQ(array[0], nullptr);
    EXPECT_EQ(array[1], nullptr);
    EXPECT_EQ(array[2], nullptr);
}

TEST_F(SdkCApiCommonTest, GeometryArrayDestroy_PartialNull) {
    ogc_geometry_t* array[3];
    array[0] = ogc_point_create(0.0, 0.0);
    array[1] = nullptr;
    array[2] = ogc_point_create(2.0, 2.0);
    
    ogc_geometry_array_destroy(array, 3);
    
    EXPECT_EQ(array[0], nullptr);
    EXPECT_EQ(array[1], nullptr);
    EXPECT_EQ(array[2], nullptr);
}

TEST_F(SdkCApiCommonTest, FeatureArrayDestroy_NullArray) {
    ogc_feature_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, FeatureArrayDestroy_EmptyArray) {
    ogc_feature_t* array[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    ogc_feature_array_destroy(array, 5);
}

TEST_F(SdkCApiCommonTest, StringArrayDestroy_NullArray) {
    ogc_string_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, StringArrayDestroy_EmptyArray) {
    char* array[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    ogc_string_array_destroy(array, 5);
}

TEST_F(SdkCApiCommonTest, LayerArrayDestroy_NullArray) {
    ogc_layer_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, AlertArrayDestroy_NullArray) {
    ogc_alert_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, WaypointArrayDestroy_NullArray) {
    ogc_waypoint_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, AisTargetArrayDestroy_NullArray) {
    ogc_ais_target_array_destroy(nullptr, 5);
}

TEST_F(SdkCApiCommonTest, Version_GetVersion) {
    const char* version = ogc_sdk_get_version();
    ASSERT_NE(version, nullptr);
    EXPECT_NE(strlen(version), 0u);
}

TEST_F(SdkCApiCommonTest, Version_GetBuildDate) {
    const char* date = ogc_sdk_get_build_date();
    ASSERT_NE(date, nullptr);
}

TEST_F(SdkCApiCommonTest, Version_GetMajor) {
    int major = ogc_sdk_get_version_major();
    EXPECT_EQ(major, OGC_SDK_VERSION_MAJOR);
}

TEST_F(SdkCApiCommonTest, Version_GetMinor) {
    int minor = ogc_sdk_get_version_minor();
    EXPECT_EQ(minor, OGC_SDK_VERSION_MINOR);
}

TEST_F(SdkCApiCommonTest, Version_GetPatch) {
    int patch = ogc_sdk_get_version_patch();
    EXPECT_EQ(patch, OGC_SDK_VERSION_PATCH);
}
