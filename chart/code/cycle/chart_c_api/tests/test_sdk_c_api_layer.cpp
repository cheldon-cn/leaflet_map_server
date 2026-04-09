/**
 * @file test_sdk_c_api_layer.cpp
 * @brief Unit tests for SDK C API Layer module
 */

#include <gtest/gtest.h>
#include <cstring>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiLayerTest, LayerCreate_Destroy) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerCreate_NullName) {
    ogc_layer_t* layer = ogc_layer_create(nullptr);
    EXPECT_EQ(layer, nullptr);
}

TEST_F(SdkCApiLayerTest, LayerGetName) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    const char* name = ogc_layer_get_name(layer);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "test_layer");
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerSetVisible) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_set_visible(layer, 1);
    int visible = ogc_layer_is_visible(layer);
    EXPECT_NE(visible, 0);
    
    ogc_layer_set_visible(layer, 0);
    visible = ogc_layer_is_visible(layer);
    EXPECT_EQ(visible, 0);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerSetOpacity) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_set_opacity(layer, 0.5);
    double opacity = ogc_layer_get_opacity(layer);
    EXPECT_DOUBLE_EQ(opacity, 0.5);
    
    ogc_layer_set_opacity(layer, 1.0);
    opacity = ogc_layer_get_opacity(layer);
    EXPECT_DOUBLE_EQ(opacity, 1.0);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerSetZOrder) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_set_z_order(layer, 10);
    int z_order = ogc_layer_get_z_order(layer);
    EXPECT_EQ(z_order, 10);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerAddFeature) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_layer_add_feature(layer, feature);
    EXPECT_EQ(result, 0);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerGetFeatureCount) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    int count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 5; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        ogc_feature_t* feature = ogc_feature_create(geom);
        ASSERT_NE(feature, nullptr);
        ogc_layer_add_feature(layer, feature);
    }
    
    count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 5);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerGetFeature) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    ogc_feature_set_id(feature, 123);
    
    ogc_layer_add_feature(layer, feature);
    
    ogc_feature_t* retrieved = ogc_layer_get_feature(layer, 0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(ogc_feature_get_id(retrieved), 123);
    
    ogc_feature_t* invalid = ogc_layer_get_feature(layer, 99);
    EXPECT_EQ(invalid, nullptr);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerRemoveFeature) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_geometry_t* geom = ogc_point_create(116.0, 39.0);
    ASSERT_NE(geom, nullptr);
    
    ogc_feature_t* feature = ogc_feature_create(geom);
    ASSERT_NE(feature, nullptr);
    ogc_feature_set_id(feature, 123);
    
    ogc_layer_add_feature(layer, feature);
    
    int count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 1);
    
    int result = ogc_layer_remove_feature(layer, 0);
    EXPECT_EQ(result, 0);
    
    count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 0);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerClearFeatures) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        ogc_feature_t* feature = ogc_feature_create(geom);
        ASSERT_NE(feature, nullptr);
        ogc_layer_add_feature(layer, feature);
    }
    
    int count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 5);
    
    ogc_layer_clear_features(layer);
    
    count = ogc_layer_get_feature_count(layer);
    EXPECT_EQ(count, 0);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerGetExtent) {
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_envelope_t extent;
    int result = ogc_layer_get_extent(layer, &extent);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerNullPointer) {
    EXPECT_EQ(ogc_layer_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_layer_is_visible(nullptr), 0);
    EXPECT_DOUBLE_EQ(ogc_layer_get_opacity(nullptr), 1.0);
    EXPECT_EQ(ogc_layer_get_z_order(nullptr), 0);
    EXPECT_EQ(ogc_layer_get_feature_count(nullptr), 0);
    EXPECT_EQ(ogc_layer_get_feature(nullptr, 0), nullptr);
}

TEST_F(SdkCApiLayerTest, LayerArrayDestroy) {
    ogc_layer_t* layers[3];
    
    for (int i = 0; i < 3; ++i) {
        layers[i] = ogc_layer_create("test_layer");
        ASSERT_NE(layers[i], nullptr);
    }
    
    ogc_layer_array_destroy(layers, 3);
}

TEST_F(SdkCApiLayerTest, LayerArrayDestroy_NullArray) {
    ogc_layer_array_destroy(nullptr, 5);
}

class SdkCApiLayerManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiLayerManagerTest, LayerManagerCreate_Destroy) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerAddLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    int result = ogc_layer_manager_add_layer(lm, layer);
    EXPECT_EQ(result, 0);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerGetLayerCount) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    int count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 3; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "layer_%d", i);
        ogc_layer_t* layer = ogc_layer_create(name);
        ASSERT_NE(layer, nullptr);
        ogc_layer_manager_add_layer(lm, layer);
    }
    
    count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 3);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerGetLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    ogc_layer_t* retrieved = ogc_layer_manager_get_layer(lm, "test_layer");
    ASSERT_NE(retrieved, nullptr);
    
    ogc_layer_t* invalid = ogc_layer_manager_get_layer(lm, "nonexistent");
    EXPECT_EQ(invalid, nullptr);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerRemoveLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    int count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 1);
    
    int result = ogc_layer_manager_remove_layer(lm, "test_layer");
    EXPECT_EQ(result, 0);
    
    count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 0);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerGetLayerByIndex) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer1 = ogc_layer_create("layer_1");
    ASSERT_NE(layer1, nullptr);
    ogc_layer_manager_add_layer(lm, layer1);
    
    ogc_layer_t* layer2 = ogc_layer_create("layer_2");
    ASSERT_NE(layer2, nullptr);
    ogc_layer_manager_add_layer(lm, layer2);
    
    ogc_layer_t* retrieved = ogc_layer_manager_get_layer_at(lm, 0);
    ASSERT_NE(retrieved, nullptr);
    
    retrieved = ogc_layer_manager_get_layer_at(lm, 1);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_layer_t* invalid = ogc_layer_manager_get_layer_at(lm, 99);
    EXPECT_EQ(invalid, nullptr);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerMoveLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer1 = ogc_layer_create("layer_1");
    ASSERT_NE(layer1, nullptr);
    ogc_layer_manager_add_layer(lm, layer1);
    
    ogc_layer_t* layer2 = ogc_layer_create("layer_2");
    ASSERT_NE(layer2, nullptr);
    ogc_layer_manager_add_layer(lm, layer2);
    
    int result = ogc_layer_manager_move_layer(lm, "layer_1", 1);
    EXPECT_EQ(result, 0);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerNullPointer) {
    EXPECT_EQ(ogc_layer_manager_get_layer_count(nullptr), 0);
    EXPECT_EQ(ogc_layer_manager_get_layer(nullptr, "test"), nullptr);
    EXPECT_EQ(ogc_layer_manager_get_layer_at(nullptr, 0), nullptr);
    EXPECT_NE(ogc_layer_manager_remove_layer(nullptr, "test"), 0);
}

class SdkCApiVectorLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiVectorLayerTest, VectorLayerCreate_Destroy) {
    ogc_vector_layer_t* vl = ogc_vector_layer_create("vector_layer");
    ASSERT_NE(vl, nullptr);
    
    ogc_vector_layer_destroy(vl);
}

TEST_F(SdkCApiVectorLayerTest, VectorLayerCreateFromFeatures) {
    ogc_feature_collection_t* fc = ogc_feature_collection_create();
    ASSERT_NE(fc, nullptr);
    
    for (int i = 0; i < 3; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        ogc_feature_t* feature = ogc_feature_create(geom);
        ASSERT_NE(feature, nullptr);
        ogc_feature_collection_add_feature(fc, feature);
    }
    
    ogc_vector_layer_t* vl = ogc_vector_layer_create_from_features("vector_layer", fc);
    ASSERT_NE(vl, nullptr);
    
    ogc_vector_layer_destroy(vl);
    ogc_feature_collection_destroy(fc);
}

TEST_F(SdkCApiVectorLayerTest, VectorLayerSetStyle) {
    ogc_vector_layer_t* vl = ogc_vector_layer_create("vector_layer");
    ASSERT_NE(vl, nullptr);
    
    ogc_style_t* style = ogc_style_create();
    ASSERT_NE(style, nullptr);
    
    int result = ogc_vector_layer_set_style(vl, style);
    EXPECT_EQ(result, 0);
    
    ogc_vector_layer_destroy(vl);
}

TEST_F(SdkCApiVectorLayerTest, VectorLayerGetFeatureCount) {
    ogc_feature_collection_t* fc = ogc_feature_collection_create();
    ASSERT_NE(fc, nullptr);
    
    for (int i = 0; i < 5; ++i) {
        ogc_geometry_t* geom = ogc_point_create(116.0 + i, 39.0 + i);
        ASSERT_NE(geom, nullptr);
        ogc_feature_t* feature = ogc_feature_create(geom);
        ASSERT_NE(feature, nullptr);
        ogc_feature_collection_add_feature(fc, feature);
    }
    
    ogc_vector_layer_t* vl = ogc_vector_layer_create_from_features("vector_layer", fc);
    ASSERT_NE(vl, nullptr);
    
    int count = ogc_vector_layer_get_feature_count(vl);
    EXPECT_EQ(count, 5);
    
    ogc_vector_layer_destroy(vl);
    ogc_feature_collection_destroy(fc);
}

TEST_F(SdkCApiVectorLayerTest, VectorLayerNullPointer) {
    EXPECT_EQ(ogc_vector_layer_get_feature_count(nullptr), 0);
    EXPECT_NE(ogc_vector_layer_set_style(nullptr, nullptr), 0);
}

class SdkCApiRasterLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiRasterLayerTest, RasterLayerCreate_Destroy) {
    ogc_raster_layer_t* rl = ogc_raster_layer_create("raster_layer");
    ASSERT_NE(rl, nullptr);
    
    ogc_raster_layer_destroy(rl);
}

TEST_F(SdkCApiRasterLayerTest, RasterLayerCreateFromFile) {
    ogc_raster_layer_t* rl = ogc_raster_layer_create_from_file("raster_layer", "/path/to/raster.tif");
    if (rl != nullptr) {
        ogc_raster_layer_destroy(rl);
    }
}

TEST_F(SdkCApiRasterLayerTest, RasterLayerSetOpacity) {
    ogc_raster_layer_t* rl = ogc_raster_layer_create("raster_layer");
    ASSERT_NE(rl, nullptr);
    
    ogc_raster_layer_set_opacity(rl, 0.7);
    double opacity = ogc_raster_layer_get_opacity(rl);
    EXPECT_DOUBLE_EQ(opacity, 0.7);
    
    ogc_raster_layer_destroy(rl);
}

TEST_F(SdkCApiRasterLayerTest, RasterLayerNullPointer) {
    EXPECT_DOUBLE_EQ(ogc_raster_layer_get_opacity(nullptr), 1.0);
}
