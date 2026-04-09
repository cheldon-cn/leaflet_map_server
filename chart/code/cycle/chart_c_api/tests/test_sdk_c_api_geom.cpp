/**
 * @file test_sdk_c_api_geom.cpp
 * @brief Unit tests for SDK C API geometry functions
 */

#include <gtest/gtest.h>

extern "C" {
#include "sdk_c_api.h"
}

class SdkCApiGeomTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SdkCApiGeomTest, Coordinate_Create2D) {
    ogc_coordinate_t* coord = ogc_coordinate_create(1.0, 2.0);
    ASSERT_NE(coord, nullptr);
    EXPECT_DOUBLE_EQ(coord->x, 1.0);
    EXPECT_DOUBLE_EQ(coord->y, 2.0);
    EXPECT_DOUBLE_EQ(coord->z, 0.0);
    EXPECT_DOUBLE_EQ(coord->m, 0.0);
    ogc_coordinate_destroy(coord);
}

TEST_F(SdkCApiGeomTest, Coordinate_Create3D) {
    ogc_coordinate_t* coord = ogc_coordinate_create_3d(1.0, 2.0, 3.0);
    ASSERT_NE(coord, nullptr);
    EXPECT_DOUBLE_EQ(coord->x, 1.0);
    EXPECT_DOUBLE_EQ(coord->y, 2.0);
    EXPECT_DOUBLE_EQ(coord->z, 3.0);
    EXPECT_DOUBLE_EQ(coord->m, 0.0);
    ogc_coordinate_destroy(coord);
}

TEST_F(SdkCApiGeomTest, Coordinate_CreateWithMeasure) {
    ogc_coordinate_t* coord = ogc_coordinate_create_m(1.0, 2.0, 3.0, 4.0);
    ASSERT_NE(coord, nullptr);
    EXPECT_DOUBLE_EQ(coord->x, 1.0);
    EXPECT_DOUBLE_EQ(coord->y, 2.0);
    EXPECT_DOUBLE_EQ(coord->z, 3.0);
    EXPECT_DOUBLE_EQ(coord->m, 4.0);
    ogc_coordinate_destroy(coord);
}

TEST_F(SdkCApiGeomTest, Coordinate_DestroyNull) {
    ogc_coordinate_destroy(nullptr);
}

TEST_F(SdkCApiGeomTest, Coordinate_Distance) {
    ogc_coordinate_t* a = ogc_coordinate_create(0.0, 0.0);
    ogc_coordinate_t* b = ogc_coordinate_create(3.0, 4.0);
    
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    
    double dist = ogc_coordinate_distance(a, b);
    EXPECT_DOUBLE_EQ(dist, 5.0);
    
    ogc_coordinate_destroy(a);
    ogc_coordinate_destroy(b);
}

TEST_F(SdkCApiGeomTest, Envelope_Create) {
    ogc_envelope_t* env = ogc_envelope_create();
    ASSERT_NE(env, nullptr);
    ogc_envelope_destroy(env);
}

TEST_F(SdkCApiGeomTest, Envelope_CreateFromCoords) {
    ogc_envelope_t* env = ogc_envelope_create_from_coords(0.0, 0.0, 10.0, 20.0);
    ASSERT_NE(env, nullptr);
    
    EXPECT_DOUBLE_EQ(ogc_envelope_get_min_x(env), 0.0);
    EXPECT_DOUBLE_EQ(ogc_envelope_get_min_y(env), 0.0);
    EXPECT_DOUBLE_EQ(ogc_envelope_get_max_x(env), 10.0);
    EXPECT_DOUBLE_EQ(ogc_envelope_get_max_y(env), 20.0);
    
    ogc_envelope_destroy(env);
}

TEST_F(SdkCApiGeomTest, Envelope_GetWidthHeight) {
    ogc_envelope_t* env = ogc_envelope_create_from_coords(0.0, 0.0, 10.0, 20.0);
    ASSERT_NE(env, nullptr);
    
    EXPECT_DOUBLE_EQ(ogc_envelope_get_width(env), 10.0);
    EXPECT_DOUBLE_EQ(ogc_envelope_get_height(env), 20.0);
    EXPECT_DOUBLE_EQ(ogc_envelope_get_area(env), 200.0);
    
    ogc_envelope_destroy(env);
}

TEST_F(SdkCApiGeomTest, Envelope_Contains) {
    ogc_envelope_t* env = ogc_envelope_create_from_coords(0.0, 0.0, 10.0, 10.0);
    ASSERT_NE(env, nullptr);
    
    EXPECT_EQ(ogc_envelope_contains(env, 5.0, 5.0), 1);
    EXPECT_EQ(ogc_envelope_contains(env, 15.0, 5.0), 0);
    
    ogc_envelope_destroy(env);
}

TEST_F(SdkCApiGeomTest, Point_Create) {
    ogc_geometry_t* point = ogc_point_create(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(point), OGC_GEOM_TYPE_POINT);
    EXPECT_EQ(ogc_geometry_get_dimension(point), 0);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Point_Create3D) {
    ogc_geometry_t* point = ogc_point_create_3d(1.0, 2.0, 3.0);
    ASSERT_NE(point, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(point), OGC_GEOM_TYPE_POINT);
    EXPECT_EQ(ogc_geometry_is_3d(point), 1);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Point_GetCoordinates) {
    ogc_geometry_t* point = ogc_point_create(1.5, 2.5);
    ASSERT_NE(point, nullptr);
    
    EXPECT_DOUBLE_EQ(ogc_point_get_x(point), 1.5);
    EXPECT_DOUBLE_EQ(ogc_point_get_y(point), 2.5);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Point_SetCoordinates) {
    ogc_geometry_t* point = ogc_point_create(0.0, 0.0);
    ASSERT_NE(point, nullptr);
    
    ogc_point_set_x(point, 5.0);
    ogc_point_set_y(point, 6.0);
    
    EXPECT_DOUBLE_EQ(ogc_point_get_x(point), 5.0);
    EXPECT_DOUBLE_EQ(ogc_point_get_y(point), 6.0);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, LineString_Create) {
    ogc_geometry_t* line = ogc_linestring_create();
    ASSERT_NE(line, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(line), OGC_GEOM_TYPE_LINESTRING);
    EXPECT_EQ(ogc_linestring_get_num_points(line), 0u);
    
    ogc_geometry_destroy(line);
}

TEST_F(SdkCApiGeomTest, LineString_AddPoints) {
    ogc_geometry_t* line = ogc_linestring_create();
    ASSERT_NE(line, nullptr);
    
    ogc_linestring_add_point(line, 0.0, 0.0);
    ogc_linestring_add_point(line, 1.0, 1.0);
    ogc_linestring_add_point(line, 2.0, 2.0);
    
    EXPECT_EQ(ogc_linestring_get_num_points(line), 3u);
    
    ogc_geometry_destroy(line);
}

TEST_F(SdkCApiGeomTest, Polygon_Create) {
    ogc_geometry_t* polygon = ogc_polygon_create();
    ASSERT_NE(polygon, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(polygon), OGC_GEOM_TYPE_POLYGON);
    
    ogc_geometry_destroy(polygon);
}

TEST_F(SdkCApiGeomTest, Geometry_GetTypeName) {
    ogc_geometry_t* point = ogc_point_create(0.0, 0.0);
    ASSERT_NE(point, nullptr);
    
    const char* name = ogc_geometry_get_type_name(point);
    ASSERT_NE(name, nullptr);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Geometry_IsEmpty) {
    ogc_geometry_t* point = ogc_point_create(0.0, 0.0);
    ASSERT_NE(point, nullptr);
    
    EXPECT_EQ(ogc_geometry_is_empty(point), 0);
    
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Geometry_Clone) {
    ogc_geometry_t* point = ogc_point_create(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    
    ogc_geometry_t* clone = ogc_geometry_clone(point);
    ASSERT_NE(clone, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(clone), OGC_GEOM_TYPE_POINT);
    EXPECT_DOUBLE_EQ(ogc_point_get_x(clone), 1.0);
    EXPECT_DOUBLE_EQ(ogc_point_get_y(clone), 2.0);
    
    ogc_geometry_destroy(point);
    ogc_geometry_destroy(clone);
}

TEST_F(SdkCApiGeomTest, Geometry_AsText) {
    ogc_geometry_t* point = ogc_point_create(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    
    char* wkt = ogc_geometry_as_text(point, 2);
    ASSERT_NE(wkt, nullptr);
    
    ogc_geometry_free_string(wkt);
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Geometry_AsGeoJson) {
    ogc_geometry_t* point = ogc_point_create(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    
    char* geojson = ogc_geometry_as_geojson(point, 2);
    ASSERT_NE(geojson, nullptr);
    
    ogc_geometry_free_string(geojson);
    ogc_geometry_destroy(point);
}

TEST_F(SdkCApiGeomTest, Geometry_SpatialOperations) {
    ogc_geometry_t* point1 = ogc_point_create(0.0, 0.0);
    ogc_geometry_t* point2 = ogc_point_create(1.0, 1.0);
    
    ASSERT_NE(point1, nullptr);
    ASSERT_NE(point2, nullptr);
    
    double dist = ogc_geometry_distance(point1, point2);
    EXPECT_GT(dist, 0.0);
    
    int intersects = ogc_geometry_intersects(point1, point2);
    EXPECT_EQ(intersects, 0);
    
    ogc_geometry_destroy(point1);
    ogc_geometry_destroy(point2);
}

TEST_F(SdkCApiGeomTest, Geometry_Buffer) {
    ogc_geometry_t* point = ogc_point_create(0.0, 0.0);
    ASSERT_NE(point, nullptr);
    
    ogc_geometry_t* buffer = ogc_geometry_buffer(point, 10.0, 16);
    ASSERT_NE(buffer, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(buffer), OGC_GEOM_TYPE_POLYGON);
    
    ogc_geometry_destroy(point);
    ogc_geometry_destroy(buffer);
}

TEST_F(SdkCApiGeomTest, MultiPoint_Create) {
    ogc_geometry_t* mp = ogc_multipoint_create();
    ASSERT_NE(mp, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(mp), OGC_GEOM_TYPE_MULTIPOINT);
    
    ogc_geometry_destroy(mp);
}

TEST_F(SdkCApiGeomTest, GeometryCollection_Create) {
    ogc_geometry_t* gc = ogc_geometry_collection_create();
    ASSERT_NE(gc, nullptr);
    
    EXPECT_EQ(ogc_geometry_get_type(gc), OGC_GEOM_TYPE_GEOMETRYCOLLECTION);
    
    ogc_geometry_destroy(gc);
}
