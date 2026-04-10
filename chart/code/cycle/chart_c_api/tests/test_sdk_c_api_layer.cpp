/**
 * @file test_sdk_c_api_layer.cpp
 * @brief Unit tests for SDK C API Layer module
 * 
 * Note: Some tests have been moved to pending_tests/test_sdk_c_api_layer_pending.cpp
 * because they use APIs that don't exist in the current SDK:
 * - ogc_layer_create - should use ogc_vector_layer_create or ogc_memory_layer_create
 * - ogc_layer_set_visible/get_visible - should use ogc_layer_manager_*_layer_visible
 * - ogc_layer_set_opacity/get_opacity - should use ogc_layer_manager_*_layer_opacity
 * - ogc_layer_set_z_order/get_z_order - not implemented
 * - ogc_layer_add_feature - should use ogc_vector_layer_add_feature or ogc_memory_layer_add_feature
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

TEST_F(SdkCApiLayerTest, VectorLayerCreate_Destroy) {
    ogc_layer_t* layer = ogc_vector_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, MemoryLayerCreate_Destroy) {
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerGetName) {
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    const char* name = ogc_layer_get_name(layer);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "test_layer");
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, LayerGetType) {
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_type_e type = ogc_layer_get_type(layer);
    
    ogc_layer_destroy(layer);
}

TEST_F(SdkCApiLayerTest, MemoryLayerAddFeature) {
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_feature_defn_t* defn = ogc_feature_defn_create("TestFeature");
    ogc_feature_t* feature = ogc_feature_create(defn);
    ASSERT_NE(feature, nullptr);
    
    int result = ogc_memory_layer_add_feature(layer, feature);
    EXPECT_EQ(result, 0);
    
    ogc_layer_destroy(layer);
    ogc_feature_defn_destroy(defn);
}

TEST_F(SdkCApiLayerTest, LayerNullPointer) {
    EXPECT_EQ(ogc_layer_get_name(nullptr), nullptr);
}

TEST_F(SdkCApiLayerTest, LayerArrayDestroy) {
    ogc_layer_t* layers[3];
    
    for (int i = 0; i < 3; ++i) {
        layers[i] = ogc_memory_layer_create("test_layer");
        ASSERT_NE(layers[i], nullptr);
    }
    
    ogc_layer_array_destroy(layers, 3);
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
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_manager_add_layer(lm, layer);
    
    size_t count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 1);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerGetLayerCount) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    size_t count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 0);
    
    for (int i = 0; i < 3; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "layer_%d", i);
        ogc_layer_t* layer = ogc_memory_layer_create(name);
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
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    ogc_layer_t* retrieved = ogc_layer_manager_get_layer(lm, 0);
    ASSERT_NE(retrieved, nullptr);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerRemoveLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    size_t count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 1);
    
    ogc_layer_manager_remove_layer(lm, 0);
    
    count = ogc_layer_manager_get_layer_count(lm);
    EXPECT_EQ(count, 0);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerMoveLayer) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer1 = ogc_memory_layer_create("layer_1");
    ASSERT_NE(layer1, nullptr);
    ogc_layer_manager_add_layer(lm, layer1);
    
    ogc_layer_t* layer2 = ogc_memory_layer_create("layer_2");
    ASSERT_NE(layer2, nullptr);
    ogc_layer_manager_add_layer(lm, layer2);
    
    ogc_layer_manager_move_layer(lm, 0, 1);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerLayerVisible) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    ogc_layer_manager_set_layer_visible(lm, 0, 1);
    int visible = ogc_layer_manager_get_layer_visible(lm, 0);
    EXPECT_NE(visible, 0);
    
    ogc_layer_manager_set_layer_visible(lm, 0, 0);
    visible = ogc_layer_manager_get_layer_visible(lm, 0);
    EXPECT_EQ(visible, 0);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerLayerOpacity) {
    ogc_layer_manager_t* lm = ogc_layer_manager_create();
    ASSERT_NE(lm, nullptr);
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    ogc_layer_manager_add_layer(lm, layer);
    
    ogc_layer_manager_set_layer_opacity(lm, 0, 0.5);
    double opacity = ogc_layer_manager_get_layer_opacity(lm, 0);
    EXPECT_DOUBLE_EQ(opacity, 0.5);
    
    ogc_layer_manager_destroy(lm);
}

TEST_F(SdkCApiLayerManagerTest, LayerManagerNullPointer) {
    EXPECT_EQ(ogc_layer_manager_get_layer_count(nullptr), 0);
    EXPECT_EQ(ogc_layer_manager_get_layer(nullptr, 0), nullptr);
}

class SdkCApiLayerGroupTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiLayerGroupTest, LayerGroupCreate_Destroy) {
    ogc_layer_group_t* group = ogc_layer_group_create("test_group");
    ASSERT_NE(group, nullptr);
    
    ogc_layer_group_destroy(group);
}

TEST_F(SdkCApiLayerGroupTest, LayerGroupGetName) {
    ogc_layer_group_t* group = ogc_layer_group_create("test_group");
    ASSERT_NE(group, nullptr);
    
    const char* name = ogc_layer_group_get_name(group);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "test_group");
    
    ogc_layer_group_destroy(group);
}

TEST_F(SdkCApiLayerGroupTest, LayerGroupAddLayer) {
    ogc_layer_group_t* group = ogc_layer_group_create("test_group");
    ASSERT_NE(group, nullptr);
    
    ogc_layer_t* layer = ogc_memory_layer_create("test_layer");
    ASSERT_NE(layer, nullptr);
    
    ogc_layer_group_add_layer(group, layer);
    
    size_t count = ogc_layer_group_get_layer_count(group);
    EXPECT_EQ(count, 1);
    
    ogc_layer_group_destroy(group);
}

TEST_F(SdkCApiLayerGroupTest, LayerGroupNullPointer) {
    EXPECT_EQ(ogc_layer_group_get_name(nullptr), nullptr);
    EXPECT_EQ(ogc_layer_group_get_layer_count(nullptr), 0);
    EXPECT_EQ(ogc_layer_group_get_layer(nullptr, 0), nullptr);
}
