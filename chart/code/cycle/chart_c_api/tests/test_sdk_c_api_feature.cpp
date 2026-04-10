/**
 * @file test_sdk_c_api_feature.cpp
 * @brief Unit tests for SDK C API Feature module
 * 
 * Note: Some tests have been moved to pending_tests/test_sdk_c_api_feature_pending.cpp
 * because they use APIs that don't exist in the current SDK:
 * - ogc_feature_create(geometry) - should use ogc_feature_create(feature_defn)
 * - ogc_feature_set_id/get_id - should use ogc_feature_set_fid/get_fid
 * - ogc_feature_set_attribute_* - should use ogc_feature_set_field_*
 * - ogc_feature_get_attribute_* - should use ogc_feature_get_field_as_*
 * - ogc_feature_get_attribute_count - should use ogc_feature_get_field_count
 * - ogc_feature_get_attribute_name - should use ogc_feature_defn_get_field_defn
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiFeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiFeatureTest, FeatureDefnCreate_Destroy) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    const char* name = ogc_feature_defn_get_name(defn);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "TestFeature");
    
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureDefnFieldCount) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    size_t count = ogc_feature_defn_get_field_count(defn);
    EXPECT_EQ(count, 0);
    
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureCreate_Destroy) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureGetFid) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_set_fid(feature, 12345);
    
    long long fid = ogc_feature_get_fid(feature);
    EXPECT_EQ(fid, 12345);
    
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureGetDefn) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_defn_t* retrieved_defn = ogc_feature_get_defn(feature);
    ASSERT_NE(retrieved_defn, nullptr);
    
    const char* name = ogc_feature_defn_get_name(retrieved_defn);
    EXPECT_STREQ(name, "TestFeature");
    
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureGetFieldCount) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    size_t count = ogc_feature_get_field_count(feature);
    EXPECT_EQ(count, 0);
    
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureGeometry) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_set_geometry(feature, geom);
    
    ogc_geometry_t* retrieved_geom = ogc_feature_get_geometry(feature);
    ASSERT_NE(retrieved_geom, nullptr);
    
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureClone) {
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ASSERT_NE(defn, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_set_fid(feature, 12345);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ogc_feature_set_geometry(feature, geom);
    
    ogc_feature_t* cloned = ogc_feature_clone(feature);
    ASSERT_NE(cloned, nullptr);
    
    long long fid = ogc_feature_get_fid(cloned);
    EXPECT_EQ(fid, 12345);
    
    ogc_feature_destroy(cloned);
    ogc_feature_destroy(feature);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiFeatureTest, FeatureNullPointer) {
    EXPECT_EQ(ogc_feature_get_fid(nullptr), 0);
    EXPECT_EQ(ogc_feature_get_defn(nullptr), nullptr);
    EXPECT_EQ(ogc_feature_get_geometry(nullptr), nullptr);
    EXPECT_EQ(ogc_feature_get_field_count(nullptr), 0);
    EXPECT_EQ(ogc_feature_clone(nullptr), nullptr);
}
