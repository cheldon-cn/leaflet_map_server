/**
 * @file test_sdk_c_api_feature.cpp
 * @brief Unit tests for SDK C API Feature module
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

TEST_F(SdkCApiFeatureTest, FeatureCreate_Destroy) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureCreate_NullGeometry) {
    ogc_feature_t* feature = ogc_feature_create(nullptr);
    EXPECT_EQ(feature, nullptr);
}

TEST_F(SdkCApiFeatureTest, FeatureGetGeometry) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    ogc_geometry_t* retrieved_geom = ogc_feature_get_geometry(feature);
    ASSERT_NE(retrieved_geom, nullptr);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureSetGeometry) {
    ogc_geometry_t* geom1 = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom1, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom1);
    ASSERT_NE(feature, nullptr);
    
    ogc_geometry_t* geom2 = ogc_point_create(117.0, 40.0);
    ASSERT_NE(geom2, nullptr);
    
    ogc_feature_set_geometry(feature, geom2);
    
    ogc_geometry_t* retrieved_geom = ogc_feature_get_geometry(feature);
    ASSERT_NE(retrieved_geom, nullptr);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureGetId) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_set_id(feature, 12345);
    
    int64_t id = ogc_feature_get_id(feature);
    EXPECT_EQ(id, 12345);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureSetAttributeString) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_feature_set_attribute_string(feature, "name", "Beijing");
    EXPECT_EQ(result, 0);
    
    const char* value = ogc_feature_get_attribute_string(feature, "name");
    ASSERT_NE(value, nullptr);
    EXPECT_STREQ(value, "Beijing");
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureSetAttributeInt) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_feature_set_attribute_int(feature, "population", 21540000);
    EXPECT_EQ(result, 0);
    
    int value = ogc_feature_get_attribute_int(feature, "population");
    EXPECT_EQ(value, 21540000);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureSetAttributeDouble) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_feature_set_attribute_double(feature, "area", 16410.5);
    EXPECT_EQ(result, 0);
    
    double value = ogc_feature_get_attribute_double(feature, "area");
    EXPECT_DOUBLE_EQ(value, 16410.5);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureGetAttributeNotFound) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    const char* str_value = ogc_feature_get_attribute_string(feature, "nonexistent");
    EXPECT_EQ(str_value, nullptr);
    
    int int_value = ogc_feature_get_attribute_int(feature, "nonexistent");
    EXPECT_EQ(int_value, 0);
    
    double double_value = ogc_feature_get_attribute_double(feature, "nonexistent");
    EXPECT_DOUBLE_EQ(double_value, 0.0);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureGetAttributeCount) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int count = ogc_feature_get_attribute_count(feature);
    EXPECT_EQ(count, 0);
    
    ogc_feature_set_attribute_string(feature, "name", "Beijing");
    ogc_feature_set_attribute_int(feature, "population", 21540000);
    ogc_feature_set_attribute_double(feature, "area", 16410.5);
    
    count = ogc_feature_get_attribute_count(feature);
    EXPECT_EQ(count, 3);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureGetAttributeName) {
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    ogc_feature_set_attribute_string(feature, "name", "Beijing");
    ogc_feature_set_attribute_int(feature, "population", 21540000);
    
    const char* attr_name = ogc_feature_get_attribute_name(feature, 0);
    ASSERT_NE(attr_name, nullptr);
    
    attr_name = ogc_feature_get_attribute_name(feature, 1);
    ASSERT_NE(attr_name, nullptr);
    
    attr_name = ogc_feature_get_attribute_name(feature, 99);
    EXPECT_EQ(attr_name, nullptr);
    
    ogc_feature_destroy(feature);
}

TEST_F(SdkCApiFeatureTest, FeatureNullPointer) {
    EXPECT_EQ(ogc_feature_get_geometry(nullptr), nullptr);
    EXPECT_EQ(ogc_feature_get_id(nullptr), 0);
    EXPECT_NE(ogc_feature_set_attribute_string(nullptr, "name", "test"), 0);
    EXPECT_EQ(ogc_feature_get_attribute_string(nullptr, "name"), nullptr);
    EXPECT_EQ(ogc_feature_get_attribute_count(nullptr), 0);
}

TEST_F(SdkCApiFeatureTest, FeatureArrayDestroy) {
    ogc_feature_t* features[3];
    
    for (int i = 0; i < 3; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        features[i] = ogc_feature_create(geom);
        ASSERT_NE(features[i], nullptr);
    }
    
    ogc_feature_array_destroy(features, 3);
}

TEST_F(SdkCApiFeatureTest, FeatureArrayDestroy_NullArray) {
    ogc_feature_array_destroy(nullptr, 5);
}

class SdkCApiFeatureClassTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiFeatureClassTest, FeatureClassCreate_Destroy) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassCreate_NullName) {
    ogc_feature_class_t* fc = ogc_feature_class_create(nullptr);
    EXPECT_EQ(fc, nullptr);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassGetName) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    const char* name = ogc_feature_class_get_name(fc);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "cities");
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassAddField) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    int result = ogc_feature_class_add_field(fc, "name", OGC_FIELD_TYPE_STRING);
    EXPECT_EQ(result, 0);
    
    result = ogc_feature_class_add_field(fc, "population", OGC_FIELD_TYPE_INT);
    EXPECT_EQ(result, 0);
    
    result = ogc_feature_class_add_field(fc, "area", OGC_FIELD_TYPE_DOUBLE);
    EXPECT_EQ(result, 0);
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassGetFieldCount) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    int count = ogc_feature_class_get_field_count(fc);
    EXPECT_EQ(count, 0);
    
    ogc_feature_class_add_field(fc, "name", OGC_FIELD_TYPE_STRING);
    ogc_feature_class_add_field(fc, "population", OGC_FIELD_TYPE_INT);
    
    count = ogc_feature_class_get_field_count(fc);
    EXPECT_EQ(count, 2);
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassGetFieldName) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    ogc_feature_class_add_field(fc, "name", OGC_FIELD_TYPE_STRING);
    ogc_feature_class_add_field(fc, "population", OGC_FIELD_TYPE_INT);
    
    const char* field_name = ogc_feature_class_get_field_name(fc, 0);
    ASSERT_NE(field_name, nullptr);
    
    field_name = ogc_feature_class_get_field_name(fc, 1);
    ASSERT_NE(field_name, nullptr);
    
    field_name = ogc_feature_class_get_field_name(fc, 99);
    EXPECT_EQ(field_name, nullptr);
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassGetFieldType) {
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    ASSERT_NE(fc, nullptr);
    
    ogc_feature_class_add_field(fc, "name", OGC_FIELD_TYPE_STRING);
    ogc_feature_class_add_field(fc, "population", OGC_FIELD_TYPE_INT);
    ogc_feature_class_add_field(fc, "area", OGC_FIELD_TYPE_DOUBLE);
    
    ogc_field_type_e type = ogc_feature_class_get_field_type(fc, 0);
    EXPECT_EQ(type, OGC_FIELD_TYPE_STRING);
    
    type = ogc_feature_class_get_field_type(fc, 1);
    EXPECT_EQ(type, OGC_FIELD_TYPE_INT);
    
    type = ogc_feature_class_get_field_type(fc, 2);
    EXPECT_EQ(type, OGC_FIELD_TYPE_DOUBLE);
    
    ogc_feature_class_destroy(fc);
}

TEST_F(SdkCApiFeatureClassTest, FeatureClassNullPointer) {
    EXPECT_EQ(ogc_feature_class_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_feature_class_get_field_count(nullptr), 0);
    EXPECT_EQ(ogc_feature_class_get_field_name(nullptr, 0), nullptr);
    EXPECT_EQ(ogc_feature_class_get_field_type(nullptr, 0), OGC_FIELD_TYPE_UNKNOWN);
}

class SdkCApiFeatureCollectionTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiFeatureCollectionTest, FeatureCollectionCreate_Destroy) {
    ogc_feature_collection_t* fc = ogc_feature_collection_create();
    ASSERT_NE(fc, nullptr);
    
    ogc_feature_collection_destroy(fc);
}

TEST_F(SdkCApiFeatureCollectionTest, FeatureCollectionAddFeature) {
    ogc_feature_collection_t* collection = ogc_feature_collection_create();
    ASSERT_NE(collection, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_feature_collection_add_feature(collection, feature);
    EXPECT_EQ(result, 0);
    
    ogc_feature_collection_destroy(collection);
}

TEST_F(SdkCApiFeatureCollectionTest, FeatureCollectionGetSize) {
    ogc_feature_collection_t* collection = ogc_feature_collection_create();
    ASSERT_NE(collection, nullptr);
    
    int size = ogc_feature_collection_get_size(collection);
    EXPECT_EQ(size, 0);
    
    for (int i = 0; i < 5; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        ogc_feature_t* feature = ogc_feature_create(geom);
        ASSERT_NE(feature, nullptr);
        ogc_feature_collection_add_feature(collection, feature);
    }
    
    size = ogc_feature_collection_get_size(collection);
    EXPECT_EQ(size, 5);
    
    ogc_feature_collection_destroy(collection);
}

TEST_F(SdkCApiFeatureCollectionTest, FeatureCollectionGetFeature) {
    ogc_feature_collection_t* collection = ogc_feature_collection_create();
    ASSERT_NE(collection, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    ogc_feature_set_id(feature, 123);
    
    ogc_feature_collection_add_feature(collection, feature);
    
    ogc_feature_t* retrieved = ogc_feature_collection_get_feature(collection, 0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(ogc_feature_get_id(retrieved), 123);
    
    ogc_feature_t* invalid = ogc_feature_collection_get_feature(collection, 99);
    EXPECT_EQ(invalid, nullptr);
    
    ogc_feature_collection_destroy(collection);
}

TEST_F(SdkCApiFeatureCollectionTest, FeatureCollectionNullPointer) {
    EXPECT_EQ(ogc_feature_collection_get_size(nullptr), 0);
    EXPECT_EQ(ogc_feature_collection_get_feature(nullptr, 0), nullptr);
    EXPECT_NE(ogc_feature_collection_add_feature(nullptr, nullptr), 0);
}
