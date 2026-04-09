/**
 * @file sdk_c_api_examples.cpp
 * @brief Usage examples for OGC Chart SDK C API
 * 
 * This file demonstrates how to use the SDK C API for common tasks:
 * 1. Geometry creation and manipulation
 * 2. Feature and layer operations
 * 3. Chart rendering
 * 4. Navigation functions
 * 5. Error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include "sdk_c_api.h"

void example_geometry_basic() {
    printf("=== Geometry Basic Example ===\n");
    
    ogc_geometry_t* point = ogc_point_create(120.5, 31.2);
    if (point == NULL) {
        printf("Failed to create point\n");
        return;
    }
    
    printf("Created point at (%.2f, %.2f)\n", 
           ogc_point_get_x(point), 
           ogc_point_get_y(point));
    
    char* wkt = ogc_geometry_as_text(point, 6);
    printf("WKT: %s\n", wkt);
    ogc_geometry_free_string(wkt);
    
    ogc_geometry_destroy(point);
    printf("\n");
}

void example_geometry_spatial_operations() {
    printf("=== Geometry Spatial Operations Example ===\n");
    
    ogc_geometry_t* point1 = ogc_point_create(0.0, 0.0);
    ogc_geometry_t* point2 = ogc_point_create(3.0, 4.0);
    
    double distance = ogc_geometry_distance(point1, point2);
    printf("Distance between points: %.2f\n", distance);
    
    ogc_geometry_t* buffer = ogc_geometry_buffer(point1, 10.0, 16);
    if (buffer != NULL) {
        printf("Buffer area: %.2f\n", ogc_geometry_get_area(buffer));
        ogc_geometry_destroy(buffer);
    }
    
    ogc_geometry_destroy(point1);
    ogc_geometry_destroy(point2);
    printf("\n");
}

void example_linestring() {
    printf("=== LineString Example ===\n");
    
    ogc_geometry_t* line = ogc_linestring_create();
    
    ogc_linestring_add_point(line, 0.0, 0.0);
    ogc_linestring_add_point(line, 1.0, 1.0);
    ogc_linestring_add_point(line, 2.0, 0.0);
    
    printf("LineString with %zu points\n", ogc_linestring_get_num_points(line));
    printf("Length: %.2f\n", ogc_geometry_get_length(line));
    
    ogc_geometry_destroy(line);
    printf("\n");
}

void example_polygon() {
    printf("=== Polygon Example ===\n");
    
    ogc_coordinate_t coords[5] = {
        {0.0, 0.0, 0.0, 0.0},
        {10.0, 0.0, 0.0, 0.0},
        {10.0, 10.0, 0.0, 0.0},
        {0.0, 10.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0}
    };
    
    ogc_geometry_t* ring = ogc_linearring_create_from_coords(coords, 5);
    ogc_geometry_t* polygon = ogc_polygon_create_from_ring(ring);
    
    printf("Polygon area: %.2f\n", ogc_geometry_get_area(polygon));
    printf("Perimeter: %.2f\n", ogc_geometry_get_length(polygon));
    
    ogc_geometry_destroy(polygon);
    printf("\n");
}

void example_envelope() {
    printf("=== Envelope Example ===\n");
    
    ogc_envelope_t* env = ogc_envelope_create_from_coords(0.0, 0.0, 100.0, 100.0);
    
    printf("Envelope: (%.2f, %.2f) - (%.2f, %.2f)\n",
           ogc_envelope_get_min_x(env), ogc_envelope_get_min_y(env),
           ogc_envelope_get_max_x(env), ogc_envelope_get_max_y(env));
    
    printf("Width: %.2f, Height: %.2f\n", 
           ogc_envelope_get_width(env), 
           ogc_envelope_get_height(env));
    
    printf("Contains (50, 50): %s\n", 
           ogc_envelope_contains(env, 50.0, 50.0) ? "Yes" : "No");
    
    ogc_coordinate_t center = ogc_envelope_get_center(env);
    printf("Center: (%.2f, %.2f)\n", center.x, center.y);
    
    ogc_envelope_destroy(env);
    printf("\n");
}

void example_batch_memory_management() {
    printf("=== Batch Memory Management Example ===\n");
    
    ogc_geometry_t* points[5];
    for (int i = 0; i < 5; i++) {
        points[i] = ogc_point_create(i * 1.0, i * 2.0);
    }
    
    printf("Created %d points\n", 5);
    
    ogc_geometry_array_destroy(points, 5);
    printf("Batch destroyed all points\n");
    printf("\n");
}

void example_error_handling() {
    printf("=== Error Handling Example ===\n");
    
    printf("Error code %d: %s\n", OGC_SUCCESS, ogc_error_get_message(OGC_SUCCESS));
    printf("Error code %d: %s\n", OGC_ERROR_INVALID_PARAM, ogc_error_get_message(OGC_ERROR_INVALID_PARAM));
    printf("Error code %d: %s\n", OGC_ERROR_NULL_POINTER, ogc_error_get_message(OGC_ERROR_NULL_POINTER));
    printf("\n");
}

void example_version_info() {
    printf("=== Version Information ===\n");
    printf("SDK Version: %s\n", ogc_sdk_get_version());
    printf("Build Date: %s\n", ogc_sdk_get_build_date());
    printf("Major: %d, Minor: %d, Patch: %d\n",
           ogc_sdk_get_version_major(),
           ogc_sdk_get_version_minor(),
           ogc_sdk_get_version_patch());
    printf("\n");
}

void example_coordinate_transform() {
    printf("=== Coordinate Transform Example ===\n");
    
    ogc_coord_transformer_t* transformer = ogc_coord_transformer_create("EPSG:4326", "EPSG:3857");
    if (transformer != NULL && ogc_coord_transformer_is_valid(transformer)) {
        ogc_coordinate_t coord = {120.0, 31.0, 0.0, 0.0};
        ogc_coordinate_t result = ogc_coord_transformer_transform(transformer, &coord);
        printf("WGS84 (%.2f, %.2f) -> WebMercator (%.2f, %.2f)\n",
               coord.x, coord.y, result.x, result.y);
        ogc_coord_transformer_destroy(transformer);
    } else {
        printf("Failed to create coordinate transformer\n");
    }
    printf("\n");
}

void example_logging() {
    printf("=== Logging Example ===\n");
    
    ogc_logger_t* logger = ogc_logger_get_instance();
    ogc_logger_set_level(logger, 2);
    ogc_logger_set_console_output(logger, 1);
    
    ogc_logger_info(logger, "This is an info message");
    ogc_logger_warning(logger, "This is a warning message");
    ogc_logger_error(logger, "This is an error message");
    
    printf("\n");
}

void example_navigation_route() {
    printf("=== Navigation Route Example ===\n");
    
    ogc_route_t* route = ogc_route_create();
    ogc_route_set_name(route, "Test Route");
    
    ogc_waypoint_t* wp1 = ogc_waypoint_create(31.0, 121.0);
    ogc_waypoint_set_name(wp1, "Start Point");
    ogc_route_add_waypoint(route, wp1);
    
    ogc_waypoint_t* wp2 = ogc_waypoint_create(32.0, 122.0);
    ogc_waypoint_set_name(wp2, "End Point");
    ogc_route_add_waypoint(route, wp2);
    
    printf("Route '%s' with %zu waypoints\n", 
           ogc_route_get_name(route),
           ogc_route_get_waypoint_count(route));
    printf("Total distance: %.2f\n", ogc_route_get_total_distance(route));
    
    ogc_waypoint_destroy(wp1);
    ogc_waypoint_destroy(wp2);
    ogc_route_destroy(route);
    printf("\n");
}

void example_alert_system() {
    printf("=== Alert System Example ===\n");
    
    ogc_alert_t* alert = ogc_alert_create(
        OGC_ALERT_TYPE_DEPTH,
        OGC_ALERT_LEVEL_WARNING,
        "Shallow water ahead"
    );
    
    printf("Alert Type: %d, Level: %d\n", 
           ogc_alert_get_type(alert),
           ogc_alert_get_level(alert));
    printf("Message: %s\n", ogc_alert_get_message(alert));
    
    ogc_alert_destroy(alert);
    printf("\n");
}

void example_feature_basic() {
    printf("=== Feature Basic Example ===\n");
    
    ogc_geometry_t* point = ogc_point_create(116.4074, 39.9042);
    if (point == NULL) {
        printf("Failed to create point\n");
        return;
    }
    
    ogc_feature_t* feature = ogc_feature_create(point);
    if (feature == NULL) {
        printf("Failed to create feature\n");
        ogc_geometry_destroy(point);
        return;
    }
    
    ogc_feature_set_id(feature, 1001);
    ogc_feature_set_attribute_string(feature, "name", "Beijing");
    ogc_feature_set_attribute_int(feature, "population", 21540000);
    ogc_feature_set_attribute_double(feature, "area", 16410.5);
    
    printf("Feature ID: %lld\n", (long long)ogc_feature_get_id(feature));
    printf("Name: %s\n", ogc_feature_get_attribute_string(feature, "name"));
    printf("Population: %d\n", ogc_feature_get_attribute_int(feature, "population"));
    printf("Area: %.2f km2\n", ogc_feature_get_attribute_double(feature, "area"));
    printf("Attribute count: %d\n", ogc_feature_get_attribute_count(feature));
    
    ogc_feature_destroy(feature);
    printf("\n");
}

void example_feature_class() {
    printf("=== Feature Class Example ===\n");
    
    ogc_feature_class_t* fc = ogc_feature_class_create("cities");
    if (fc == NULL) {
        printf("Failed to create feature class\n");
        return;
    }
    
    ogc_feature_class_add_field(fc, "name", OGC_FIELD_TYPE_STRING);
    ogc_feature_class_add_field(fc, "population", OGC_FIELD_TYPE_INT);
    ogc_feature_class_add_field(fc, "area", OGC_FIELD_TYPE_DOUBLE);
    
    printf("Feature class: %s\n", ogc_feature_class_get_name(fc));
    printf("Field count: %d\n", ogc_feature_class_get_field_count(fc));
    
    for (int i = 0; i < ogc_feature_class_get_field_count(fc); i++) {
        const char* field_name = ogc_feature_class_get_field_name(fc, i);
        ogc_field_type_e field_type = ogc_feature_class_get_field_type(fc, i);
        printf("  Field %d: %s (type: %d)\n", i, field_name, field_type);
    }
    
    ogc_feature_class_destroy(fc);
    printf("\n");
}

void example_feature_collection() {
    printf("=== Feature Collection Example ===\n");
    
    ogc_feature_collection_t* collection = ogc_feature_collection_create();
    if (collection == NULL) {
        printf("Failed to create feature collection\n");
        return;
    }
    
    const char* cities[] = {"Beijing", "Shanghai", "Guangzhou"};
    double coords[][2] = {{116.4074, 39.9042}, {121.4737, 31.2304}, {113.2644, 23.1291}};
    
    for (int i = 0; i < 3; i++) {
        ogc_geometry_t* point = ogc_point_create(coords[i][0], coords[i][1]);
        ogc_feature_t* feature = ogc_feature_create(point);
        ogc_feature_set_id(feature, i + 1);
        ogc_feature_set_attribute_string(feature, "name", cities[i]);
        ogc_feature_collection_add_feature(collection, feature);
    }
    
    printf("Feature collection size: %d\n", ogc_feature_collection_get_size(collection));
    
    for (int i = 0; i < ogc_feature_collection_get_size(collection); i++) {
        ogc_feature_t* feature = ogc_feature_collection_get_feature(collection, i);
        printf("  Feature %d: ID=%lld, Name=%s\n", 
               i, 
               (long long)ogc_feature_get_id(feature),
               ogc_feature_get_attribute_string(feature, "name"));
    }
    
    ogc_feature_collection_destroy(collection);
    printf("\n");
}

void example_layer_basic() {
    printf("=== Layer Basic Example ===\n");
    
    ogc_layer_t* layer = ogc_layer_create("cities_layer");
    if (layer == NULL) {
        printf("Failed to create layer\n");
        return;
    }
    
    printf("Layer name: %s\n", ogc_layer_get_name(layer));
    
    ogc_layer_set_visible(layer, 1);
    ogc_layer_set_opacity(layer, 0.8);
    ogc_layer_set_z_order(layer, 10);
    
    printf("Visible: %s\n", ogc_layer_is_visible(layer) ? "Yes" : "No");
    printf("Opacity: %.2f\n", ogc_layer_get_opacity(layer));
    printf("Z-Order: %d\n", ogc_layer_get_z_order(layer));
    
    ogc_geometry_t* point = ogc_point_create(116.4074, 39.9042);
    ogc_feature_t* feature = ogc_feature_create(point);
    ogc_feature_set_attribute_string(feature, "name", "Beijing");
    
    ogc_layer_add_feature(layer, feature);
    printf("Feature count: %d\n", ogc_layer_get_feature_count(layer));
    
    ogc_layer_destroy(layer);
    printf("\n");
}

void example_layer_manager() {
    printf("=== Layer Manager Example ===\n");
    
    ogc_layer_manager_t* manager = ogc_layer_manager_create();
    if (manager == NULL) {
        printf("Failed to create layer manager\n");
        return;
    }
    
    ogc_layer_t* layer1 = ogc_layer_create("base_layer");
    ogc_layer_t* layer2 = ogc_layer_create("overlay_layer");
    ogc_layer_t* layer3 = ogc_layer_create("annotation_layer");
    
    ogc_layer_manager_add_layer(manager, layer1);
    ogc_layer_manager_add_layer(manager, layer2);
    ogc_layer_manager_add_layer(manager, layer3);
    
    printf("Layer count: %d\n", ogc_layer_manager_get_layer_count(manager));
    
    ogc_layer_t* found = ogc_layer_manager_get_layer(manager, "base_layer");
    if (found != NULL) {
        printf("Found layer: %s\n", ogc_layer_get_name(found));
    }
    
    ogc_layer_manager_remove_layer(manager, "overlay_layer");
    printf("Layer count after removal: %d\n", ogc_layer_manager_get_layer_count(manager));
    
    ogc_layer_manager_destroy(manager);
    printf("\n");
}

void example_vector_layer() {
    printf("=== Vector Layer Example ===\n");
    
    ogc_feature_collection_t* features = ogc_feature_collection_create();
    
    for (int i = 0; i < 5; i++) {
        ogc_geometry_t* point = ogc_point_create(116.0 + i * 0.1, 39.0 + i * 0.1);
        ogc_feature_t* feature = ogc_feature_create(point);
        ogc_feature_set_id(feature, i + 1);
        ogc_feature_collection_add_feature(features, feature);
    }
    
    ogc_vector_layer_t* vlayer = ogc_vector_layer_create_from_features("points", features);
    if (vlayer != NULL) {
        printf("Vector layer created with %d features\n", 
               ogc_vector_layer_get_feature_count(vlayer));
        ogc_vector_layer_destroy(vlayer);
    }
    
    ogc_feature_collection_destroy(features);
    printf("\n");
}

void example_raster_layer() {
    printf("=== Raster Layer Example ===\n");
    
    ogc_raster_layer_t* rlayer = ogc_raster_layer_create("satellite");
    if (rlayer != NULL) {
        ogc_raster_layer_set_opacity(rlayer, 0.75);
        printf("Raster layer opacity: %.2f\n", ogc_raster_layer_get_opacity(rlayer));
        ogc_raster_layer_destroy(rlayer);
    }
    printf("\n");
}

void example_render_basic() {
    printf("=== Render Basic Example ===\n");
    
    ogc_render_device_t* device = ogc_render_device_create(800, 600);
    if (device == NULL) {
        printf("Failed to create render device\n");
        return;
    }
    
    printf("Render device: %dx%d\n", 
           ogc_render_device_get_width(device),
           ogc_render_device_get_height(device));
    
    ogc_color_t bg_color = {255, 255, 255, 255};
    ogc_render_device_clear(device, &bg_color);
    
    ogc_color_t line_color = {255, 0, 0, 255};
    ogc_render_device_draw_line(device, 0, 0, 800, 600, &line_color, 2.0);
    
    ogc_color_t fill_color = {0, 255, 0, 128};
    ogc_render_device_fill_rect(device, 100, 100, 200, 150, &fill_color);
    
    printf("Render operations completed\n");
    
    ogc_render_device_destroy(device);
    printf("\n");
}

void example_render_geometry() {
    printf("=== Render Geometry Example ===\n");
    
    ogc_render_device_t* device = ogc_render_device_create(800, 600);
    if (device == NULL) {
        printf("Failed to create render device\n");
        return;
    }
    
    ogc_geometry_t* point = ogc_point_create(400.0, 300.0);
    ogc_geometry_t* line = ogc_linestring_create();
    ogc_linestring_add_point(line, 100.0, 100.0);
    ogc_linestring_add_point(line, 700.0, 500.0);
    
    ogc_style_t* style = ogc_style_create();
    ogc_style_set_line_color(style, 255, 0, 0, 255);
    ogc_style_set_line_width(style, 2.0);
    ogc_style_set_fill_color(style, 0, 0, 255, 128);
    
    ogc_render_device_draw_geometry(device, point, style);
    ogc_render_device_draw_geometry(device, line, style);
    
    printf("Geometry rendered with style\n");
    
    ogc_style_destroy(style);
    ogc_geometry_destroy(point);
    ogc_geometry_destroy(line);
    ogc_render_device_destroy(device);
    printf("\n");
}

void example_style() {
    printf("=== Style Example ===\n");
    
    ogc_style_t* style = ogc_style_create();
    if (style == NULL) {
        printf("Failed to create style\n");
        return;
    }
    
    ogc_style_set_line_color(style, 255, 0, 0, 255);
    ogc_style_set_line_width(style, 2.0);
    ogc_style_set_fill_color(style, 0, 255, 0, 128);
    ogc_style_set_point_size(style, 10.0);
    ogc_style_set_point_symbol(style, "circle");
    
    printf("Style configured:\n");
    printf("  Line: RGBA(255, 0, 0, 255), width=2.0\n");
    printf("  Fill: RGBA(0, 255, 0, 128)\n");
    printf("  Point: size=10.0, symbol=circle\n");
    
    ogc_style_destroy(style);
    printf("\n");
}

void example_cache_basic() {
    printf("=== Cache Basic Example ===\n");
    
    ogc_tile_cache_t* cache = ogc_tile_cache_create(100);
    if (cache == NULL) {
        printf("Failed to create tile cache\n");
        return;
    }
    
    printf("Cache capacity: %zu\n", ogc_tile_cache_get_capacity(cache));
    
    ogc_tile_key_t key = {10, 100, 200};
    ogc_tile_data_t data;
    data.data = (unsigned char*)"sample_tile_data";
    data.size = 16;
    
    ogc_tile_cache_put(cache, &key, &data);
    printf("Tile stored in cache\n");
    
    ogc_tile_data_t* retrieved = ogc_tile_cache_get(cache, &key);
    if (retrieved != NULL) {
        printf("Tile retrieved, size: %zu\n", retrieved->size);
    }
    
    printf("Cache count: %zu\n", ogc_tile_cache_get_count(cache));
    
    ogc_tile_cache_destroy(cache);
    printf("\n");
}

void example_offline_storage() {
    printf("=== Offline Storage Example ===\n");
    
    ogc_offline_storage_t* storage = ogc_offline_storage_create("/tmp/chart_cache");
    if (storage == NULL) {
        printf("Failed to create offline storage\n");
        return;
    }
    
    printf("Offline storage created\n");
    
    char** chart_list = NULL;
    int count = 0;
    ogc_offline_storage_get_chart_list(storage, &chart_list, &count);
    printf("Stored charts: %d\n", count);
    
    if (chart_list != NULL) {
        ogc_string_array_destroy(chart_list, count);
    }
    
    ogc_offline_storage_destroy(storage);
    printf("\n");
}

int main(int argc, char* argv[]) {
    printf("OGC Chart SDK C API Examples\n");
    printf("Version: %s\n\n", ogc_sdk_get_version());
    
    example_version_info();
    example_error_handling();
    example_geometry_basic();
    example_geometry_spatial_operations();
    example_linestring();
    example_polygon();
    example_envelope();
    example_batch_memory_management();
    example_feature_basic();
    example_feature_class();
    example_feature_collection();
    example_layer_basic();
    example_layer_manager();
    example_vector_layer();
    example_raster_layer();
    example_render_basic();
    example_render_geometry();
    example_style();
    example_cache_basic();
    example_offline_storage();
    example_coordinate_transform();
    example_logging();
    example_navigation_route();
    example_alert_system();
    
    printf("All examples completed.\n");
    return 0;
}
