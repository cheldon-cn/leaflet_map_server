/**
 * @file sdk_c_api_benchmark.cpp
 * @brief Performance benchmark tests for OGC Chart SDK C API
 */

#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "sdk_c_api.h"

class BenchmarkTimer {
public:
    BenchmarkTimer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double ElapsedMs() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

void benchmark_point_creation(int count) {
    printf("=== Point Creation Benchmark (%d points) ===\n", count);
    
    BenchmarkTimer timer;
    std::vector<ogc_geometry_t*> points;
    points.reserve(count);
    
    for (int i = 0; i < count; i++) {
        points.push_back(ogc_point_create(i * 0.001, i * 0.001));
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto p : points) {
        ogc_geometry_destroy(p);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create: %.2f ms (%.2f ops/sec)\n", create_time, count / create_time * 1000);
    printf("Destroy: %.2f ms (%.2f ops/sec)\n", destroy_time, count / destroy_time * 1000);
    printf("\n");
}

void benchmark_linestring_operations(int num_lines, int points_per_line) {
    printf("=== LineString Operations Benchmark (%d lines, %d points each) ===\n", 
           num_lines, points_per_line);
    
    BenchmarkTimer timer;
    std::vector<ogc_geometry_t*> lines;
    lines.reserve(num_lines);
    
    for (int i = 0; i < num_lines; i++) {
        ogc_geometry_t* line = ogc_linestring_create();
        for (int j = 0; j < points_per_line; j++) {
            ogc_linestring_add_point(line, j * 0.001, j * 0.001);
        }
        lines.push_back(line);
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    double total_length = 0;
    for (auto line : lines) {
        total_length += ogc_geometry_get_length(line);
    }
    double length_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto line : lines) {
        ogc_geometry_destroy(line);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create: %.2f ms\n", create_time);
    printf("Length calculation: %.2f ms\n", length_time);
    printf("Destroy: %.2f ms\n", destroy_time);
    printf("Total length: %.2f\n", total_length);
    printf("\n");
}

void benchmark_polygon_area(int num_polygons, int points_per_polygon) {
    printf("=== Polygon Area Benchmark (%d polygons, %d points each) ===\n",
           num_polygons, points_per_polygon);
    
    BenchmarkTimer timer;
    std::vector<ogc_geometry_t*> polygons;
    polygons.reserve(num_polygons);
    
    for (int i = 0; i < num_polygons; i++) {
        std::vector<ogc_coordinate_t> coords;
        for (int j = 0; j < points_per_polygon; j++) {
            double angle = 2.0 * 3.14159265359 * j / points_per_polygon;
            coords.push_back({cos(angle) * 100, sin(angle) * 100, 0.0, 0.0});
        }
        coords.push_back(coords[0]);
        
        ogc_geometry_t* ring = ogc_linearring_create_from_coords(coords.data(), coords.size());
        ogc_geometry_t* polygon = ogc_polygon_create_from_ring(ring);
        polygons.push_back(polygon);
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    double total_area = 0;
    for (auto poly : polygons) {
        total_area += ogc_geometry_get_area(poly);
    }
    double area_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto poly : polygons) {
        ogc_geometry_destroy(poly);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create: %.2f ms\n", create_time);
    printf("Area calculation: %.2f ms\n", area_time);
    printf("Destroy: %.2f ms\n", destroy_time);
    printf("Total area: %.2f\n", total_area);
    printf("\n");
}

void benchmark_spatial_operations(int count) {
    printf("=== Spatial Operations Benchmark (%d operations) ===\n", count);
    
    std::vector<ogc_geometry_t*> points1;
    std::vector<ogc_geometry_t*> points2;
    
    for (int i = 0; i < count; i++) {
        points1.push_back(ogc_point_create(i * 0.001, i * 0.001));
        points2.push_back(ogc_point_create(i * 0.001 + 1.0, i * 0.001 + 1.0));
    }
    
    BenchmarkTimer timer;
    double total_distance = 0;
    for (int i = 0; i < count; i++) {
        total_distance += ogc_geometry_distance(points1[i], points2[i]);
    }
    double distance_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int total_intersects = 0;
    for (int i = 0; i < count; i++) {
        total_intersects += ogc_geometry_intersects(points1[i], points2[i]);
    }
    double intersects_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    std::vector<ogc_geometry_t*> buffers;
    for (int i = 0; i < count; i++) {
        buffers.push_back(ogc_geometry_buffer(points1[i], 10.0, 16));
    }
    double buffer_time = timer.ElapsedMs();
    
    for (auto p : points1) ogc_geometry_destroy(p);
    for (auto p : points2) ogc_geometry_destroy(p);
    for (auto b : buffers) ogc_geometry_destroy(b);
    
    printf("Distance: %.2f ms (%.2f ops/sec)\n", distance_time, count / distance_time * 1000);
    printf("Intersects: %.2f ms (%.2f ops/sec)\n", intersects_time, count / intersects_time * 1000);
    printf("Buffer: %.2f ms (%.2f ops/sec)\n", buffer_time, count / buffer_time * 1000);
    printf("\n");
}

void benchmark_wkt_conversion(int count) {
    printf("=== WKT Conversion Benchmark (%d conversions) ===\n", count);
    
    std::vector<ogc_geometry_t*> geometries;
    for (int i = 0; i < count; i++) {
        ogc_geometry_t* line = ogc_linestring_create();
        for (int j = 0; j < 10; j++) {
            ogc_linestring_add_point(line, j * 0.001, j * 0.001);
        }
        geometries.push_back(line);
    }
    
    BenchmarkTimer timer;
    std::vector<char*> wkt_strings;
    for (auto geom : geometries) {
        wkt_strings.push_back(ogc_geometry_as_text(geom, 6));
    }
    double to_wkt_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto wkt : wkt_strings) {
        ogc_geometry_free_string(wkt);
    }
    double free_time = timer.ElapsedMs();
    
    for (auto geom : geometries) {
        ogc_geometry_destroy(geom);
    }
    
    printf("To WKT: %.2f ms (%.2f ops/sec)\n", to_wkt_time, count / to_wkt_time * 1000);
    printf("Free strings: %.2f ms\n", free_time);
    printf("\n");
}

void benchmark_batch_destroy(int count) {
    printf("=== Batch Destroy Benchmark (%d objects) ===\n", count);
    
    std::vector<ogc_geometry_t*> points;
    for (int i = 0; i < count; i++) {
        points.push_back(ogc_point_create(i * 0.001, i * 0.001));
    }
    
    BenchmarkTimer timer;
    ogc_geometry_array_destroy(points.data(), count);
    double batch_time = timer.ElapsedMs();
    
    printf("Batch destroy: %.2f ms (%.2f ops/sec)\n", batch_time, count / batch_time * 1000);
    printf("\n");
}

void benchmark_envelope_operations(int count) {
    printf("=== Envelope Operations Benchmark (%d operations) ===\n", count);
    
    BenchmarkTimer timer;
    std::vector<ogc_envelope_t*> envelopes;
    for (int i = 0; i < count; i++) {
        envelopes.push_back(ogc_envelope_create_from_coords(
            i * 0.1, i * 0.1, i * 0.1 + 10.0, i * 0.1 + 10.0));
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int contains_count = 0;
    for (auto env : envelopes) {
        contains_count += ogc_envelope_contains(env, 5.0, 5.0);
    }
    double contains_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int intersects_count = 0;
    for (size_t i = 0; i < envelopes.size() - 1; i++) {
        intersects_count += ogc_envelope_intersects(envelopes[i], envelopes[i + 1]);
    }
    double intersects_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto env : envelopes) {
        ogc_envelope_destroy(env);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create: %.2f ms\n", create_time);
    printf("Contains: %.2f ms\n", contains_time);
    printf("Intersects: %.2f ms\n", intersects_time);
    printf("Destroy: %.2f ms\n", destroy_time);
    printf("\n");
}

void benchmark_error_message_lookup(int count) {
    printf("=== Error Message Lookup Benchmark (%d lookups) ===\n", count);
    
    ogc_error_code_e codes[] = {
        OGC_SUCCESS,
        OGC_ERROR_INVALID_PARAM,
        OGC_ERROR_NULL_POINTER,
        OGC_ERROR_OUT_OF_MEMORY,
        OGC_ERROR_NOT_FOUND
    };
    int num_codes = sizeof(codes) / sizeof(codes[0]);
    
    BenchmarkTimer timer;
    for (int i = 0; i < count; i++) {
        const char* msg = ogc_error_get_message(codes[i % num_codes]);
        (void)msg;
    }
    double lookup_time = timer.ElapsedMs();
    
    printf("Lookup: %.2f ms (%.2f ops/sec)\n", lookup_time, count / lookup_time * 1000);
    printf("\n");
}

void benchmark_feature_creation(int count) {
    printf("=== Feature Creation Benchmark (%d features) ===\n", count);
    
    BenchmarkTimer timer;
    std::vector<ogc_feature_t*> features;
    features.reserve(count);
    
    for (int i = 0; i < count; i++) {
        ogc_geometry_t* point = ogc_point_create(i * 0.001, i * 0.001);
        ogc_feature_t* feature = ogc_feature_create(point);
        ogc_feature_set_id(feature, i);
        ogc_feature_set_attribute_int(feature, "index", i);
        ogc_feature_set_attribute_double(feature, "value", i * 0.1);
        features.push_back(feature);
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto f : features) {
        ogc_feature_destroy(f);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create: %.2f ms (%.2f ops/sec)\n", create_time, count / create_time * 1000);
    printf("Destroy: %.2f ms (%.2f ops/sec)\n", destroy_time, count / destroy_time * 1000);
    printf("\n");
}

void benchmark_feature_attributes(int count) {
    printf("=== Feature Attributes Benchmark (%d operations) ===\n", count);
    
    ogc_geometry_t* point = ogc_point_create(116.0, 39.0);
    ogc_feature_t* feature = ogc_feature_create(point);
    
    BenchmarkTimer timer;
    for (int i = 0; i < count; i++) {
        char key[32];
        snprintf(key, sizeof(key), "attr_%d", i % 100);
        ogc_feature_set_attribute_int(feature, key, i);
    }
    double set_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int total = 0;
    for (int i = 0; i < count; i++) {
        char key[32];
        snprintf(key, sizeof(key), "attr_%d", i % 100);
        total += ogc_feature_get_attribute_int(feature, key);
    }
    double get_time = timer.ElapsedMs();
    
    printf("Set attribute: %.2f ms (%.2f ops/sec)\n", set_time, count / set_time * 1000);
    printf("Get attribute: %.2f ms (%.2f ops/sec)\n", get_time, count / get_time * 1000);
    printf("Attribute count: %d\n", ogc_feature_get_attribute_count(feature));
    
    ogc_feature_destroy(feature);
    printf("\n");
}

void benchmark_feature_collection(int count) {
    printf("=== Feature Collection Benchmark (%d features) ===\n", count);
    
    ogc_feature_collection_t* collection = ogc_feature_collection_create();
    
    BenchmarkTimer timer;
    for (int i = 0; i < count; i++) {
        ogc_geometry_t* point = ogc_point_create(i * 0.001, i * 0.001);
        ogc_feature_t* feature = ogc_feature_create(point);
        ogc_feature_collection_add_feature(collection, feature);
    }
    double add_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int size = ogc_feature_collection_get_size(collection);
    double size_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (int i = 0; i < size; i++) {
        ogc_feature_t* f = ogc_feature_collection_get_feature(collection, i);
        (void)f;
    }
    double get_time = timer.ElapsedMs();
    
    printf("Add features: %.2f ms (%.2f ops/sec)\n", add_time, count / add_time * 1000);
    printf("Get size: %.2f ms\n", size_time);
    printf("Get features: %.2f ms (%.2f ops/sec)\n", get_time, count / get_time * 1000);
    
    ogc_feature_collection_destroy(collection);
    printf("\n");
}

void benchmark_layer_operations(int count) {
    printf("=== Layer Operations Benchmark (%d features) ===\n", count);
    
    ogc_layer_t* layer = ogc_layer_create("benchmark_layer");
    
    BenchmarkTimer timer;
    for (int i = 0; i < count; i++) {
        ogc_geometry_t* point = ogc_point_create(i * 0.001, i * 0.001);
        ogc_feature_t* feature = ogc_feature_create(point);
        ogc_layer_add_feature(layer, feature);
    }
    double add_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int feature_count = ogc_layer_get_feature_count(layer);
    double count_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (int i = 0; i < feature_count; i++) {
        ogc_feature_t* f = ogc_layer_get_feature(layer, i);
        (void)f;
    }
    double get_time = timer.ElapsedMs();
    
    printf("Add features: %.2f ms (%.2f ops/sec)\n", add_time, count / add_time * 1000);
    printf("Get count: %.2f ms\n", count_time);
    printf("Get features: %.2f ms (%.2f ops/sec)\n", get_time, count / get_time * 1000);
    
    ogc_layer_destroy(layer);
    printf("\n");
}

void benchmark_layer_manager(int num_layers, int features_per_layer) {
    printf("=== Layer Manager Benchmark (%d layers, %d features each) ===\n", 
           num_layers, features_per_layer);
    
    ogc_layer_manager_t* manager = ogc_layer_manager_create();
    
    BenchmarkTimer timer;
    for (int i = 0; i < num_layers; i++) {
        char name[32];
        snprintf(name, sizeof(name), "layer_%d", i);
        ogc_layer_t* layer = ogc_layer_create(name);
        
        for (int j = 0; j < features_per_layer; j++) {
            ogc_geometry_t* point = ogc_point_create(j * 0.001, j * 0.001);
            ogc_feature_t* feature = ogc_feature_create(point);
            ogc_layer_add_feature(layer, feature);
        }
        
        ogc_layer_manager_add_layer(manager, layer);
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int layer_count = ogc_layer_manager_get_layer_count(manager);
    double count_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (int i = 0; i < num_layers; i++) {
        char name[32];
        snprintf(name, sizeof(name), "layer_%d", i);
        ogc_layer_t* layer = ogc_layer_manager_get_layer(manager, name);
        (void)layer;
    }
    double lookup_time = timer.ElapsedMs();
    
    printf("Create layers: %.2f ms\n", create_time);
    printf("Get count: %.2f ms\n", count_time);
    printf("Lookup layers: %.2f ms (%.2f ops/sec)\n", lookup_time, num_layers / lookup_time * 1000);
    
    ogc_layer_manager_destroy(manager);
    printf("\n");
}

void benchmark_tile_cache(int count) {
    printf("=== Tile Cache Benchmark (%d tiles) ===\n", count);
    
    ogc_tile_cache_t* cache = ogc_tile_cache_create(count * 2);
    
    BenchmarkTimer timer;
    for (int i = 0; i < count; i++) {
        ogc_tile_key_t key = {10, i % 100, i / 100};
        ogc_tile_data_t data;
        data.data = reinterpret_cast<unsigned char*>(const_cast<char*>("tile_data"));
        data.size = 9;
        ogc_tile_cache_put(cache, &key, &data);
    }
    double put_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    int hit_count = 0;
    for (int i = 0; i < count; i++) {
        ogc_tile_key_t key = {10, i % 100, i / 100};
        if (ogc_tile_cache_get(cache, &key) != nullptr) {
            hit_count++;
        }
    }
    double get_time = timer.ElapsedMs();
    
    printf("Put tiles: %.2f ms (%.2f ops/sec)\n", put_time, count / put_time * 1000);
    printf("Get tiles: %.2f ms (%.2f ops/sec)\n", get_time, count / get_time * 1000);
    printf("Cache hits: %d\n", hit_count);
    
    ogc_tile_cache_destroy(cache);
    printf("\n");
}

void benchmark_route_operations(int num_routes, int waypoints_per_route) {
    printf("=== Route Operations Benchmark (%d routes, %d waypoints each) ===\n",
           num_routes, waypoints_per_route);
    
    BenchmarkTimer timer;
    std::vector<ogc_route_t*> routes;
    
    for (int i = 0; i < num_routes; i++) {
        ogc_route_t* route = ogc_route_create();
        for (int j = 0; j < waypoints_per_route; j++) {
            ogc_waypoint_t* wp = ogc_waypoint_create(j * 0.01, j * 0.01);
            ogc_route_add_waypoint(route, wp);
            ogc_waypoint_destroy(wp);
        }
        routes.push_back(route);
    }
    double create_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    double total_distance = 0;
    for (auto route : routes) {
        total_distance += ogc_route_get_total_distance(route);
    }
    double distance_time = timer.ElapsedMs();
    
    timer = BenchmarkTimer();
    for (auto route : routes) {
        ogc_route_destroy(route);
    }
    double destroy_time = timer.ElapsedMs();
    
    printf("Create routes: %.2f ms\n", create_time);
    printf("Calculate distances: %.2f ms\n", distance_time);
    printf("Destroy routes: %.2f ms\n", destroy_time);
    printf("Total distance: %.2f\n", total_distance);
    printf("\n");
}

int main(int argc, char* argv[]) {
    printf("OGC Chart SDK C API Performance Benchmark\n");
    printf("Version: %s\n\n", ogc_sdk_get_version());
    
    benchmark_point_creation(10000);
    benchmark_linestring_operations(1000, 100);
    benchmark_polygon_area(100, 100);
    benchmark_spatial_operations(1000);
    benchmark_wkt_conversion(1000);
    benchmark_batch_destroy(10000);
    benchmark_envelope_operations(10000);
    benchmark_error_message_lookup(100000);
    benchmark_feature_creation(5000);
    benchmark_feature_attributes(10000);
    benchmark_feature_collection(5000);
    benchmark_layer_operations(5000);
    benchmark_layer_manager(10, 100);
    benchmark_tile_cache(1000);
    benchmark_route_operations(100, 20);
    
    printf("Benchmark completed.\n");
    return 0;
}
