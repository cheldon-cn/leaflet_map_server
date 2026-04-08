/**
 * @file sdk_c_api.h
 * @brief OGC Chart SDK C API Header File
 * @version v1.0
 * @date 2026-04-08
 * 
 * This file provides the C API header mappings for all Java classes
 * defined in jni_java_interface.md. Each Java class is mapped to its
 * corresponding C++ class and header file.
 * 
 * @related jni_java_interface.md
 * @related index_all.md
 */

#ifndef OGC_CHART_SDK_C_API_H
#define OGC_CHART_SDK_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Module Overview
 * ============================================================================
 * 
 * | Module      | Description                    | Header Prefix           |
 * |-------------|--------------------------------|-------------------------|
 * | ogc_base    | Base utilities (log, monitor)  | ogc/base/               |
 * | ogc_proj    | Coordinate transformation      | ogc/proj/               |
 * | ogc_geom    | Geometry objects (OGC SFAS)    | ogc/                    |
 * | ogc_feature | Feature model                  | ogc/feature/            |
 * | ogc_layer   | Layer management               | ogc/layer/              |
 * | ogc_draw    | Drawing engine                 | ogc/draw/               |
 * | ogc_graph   | Map rendering core             | ogc/graph/              |
 * | ogc_cache   | Tile cache & offline storage   | ogc/cache/              |
 * | ogc_symbology| Symbolization & styles        | ogc/symbology/          |
 * | ogc_alert   | Maritime alerts                | ogc/alert/              |
 * | ogc_navi    | Navigation system              | ogc/navi/               |
 * | chart_parser| Chart parsing (S57/S101)       | chart_parser/           |
 */

/* ============================================================================
 * 1. Base Module (ogc_base)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.util
 * Description: Base utilities including logging and performance monitoring
 */

/* 1.1 Logger */
/* Java: cn.cycle.chart.api.util.Logger */
/* C++:   ogc::base::Logger */
/* Header: ogc/base/log.h */
typedef struct ogc_logger_t ogc_logger_t;

ogc_logger_t* ogc_logger_get_instance(void);
void ogc_logger_set_level(ogc_logger_t* logger, int level);
void ogc_logger_trace(ogc_logger_t* logger, const char* message);
void ogc_logger_debug(ogc_logger_t* logger, const char* message);
void ogc_logger_info(ogc_logger_t* logger, const char* message);
void ogc_logger_warning(ogc_logger_t* logger, const char* message);
void ogc_logger_error(ogc_logger_t* logger, const char* message);
void ogc_logger_fatal(ogc_logger_t* logger, const char* message);
void ogc_logger_set_log_file(ogc_logger_t* logger, const char* filepath);
void ogc_logger_set_console_output(ogc_logger_t* logger, int enable);

/* 1.2 PerformanceMonitor */
/* Java: cn.cycle.chart.api.util.PerformanceMonitor */
/* C++:   ogc::base::PerformanceMonitor */
/* Header: ogc/base/performance_monitor.h */
typedef struct ogc_performance_monitor_t ogc_performance_monitor_t;

ogc_performance_monitor_t* ogc_performance_monitor_get_instance(void);
void ogc_performance_monitor_start_frame(ogc_performance_monitor_t* monitor);
void ogc_performance_monitor_end_frame(ogc_performance_monitor_t* monitor);
double ogc_performance_monitor_get_fps(ogc_performance_monitor_t* monitor);
long ogc_performance_monitor_get_memory_used(ogc_performance_monitor_t* monitor);
void ogc_performance_monitor_reset(ogc_performance_monitor_t* monitor);

/* 1.3 PerformanceStats */
/* Java: cn.cycle.chart.api.util.PerformanceStats */
/* C++:   ogc::base::FrameMetrics, MemoryMetrics */
/* Header: ogc/base/performance_metrics.h */
typedef struct ogc_performance_stats_t {
    double fps;
    double frame_time_ms;
    long memory_used;
    long memory_total;
    int render_count;
    int cache_hit_count;
    int cache_miss_count;
} ogc_performance_stats_t;

void ogc_performance_stats_get(ogc_performance_stats_t* stats);

/* ============================================================================
 * 2. Geometry Module (ogc_geom)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.geometry
 * Description: OGC Simple Feature Access geometry implementation
 */

/* 2.1 Coordinate */
/* Java: cn.cycle.chart.api.geometry.Coordinate */
/* C++:   ogc::geom::Coordinate */
/* Header: ogc/coordinate.h */
typedef struct ogc_coordinate_t {
    double x;
    double y;
    double z;
    double m;
} ogc_coordinate_t;

ogc_coordinate_t* ogc_coordinate_create(double x, double y);
ogc_coordinate_t* ogc_coordinate_create_3d(double x, double y, double z);
ogc_coordinate_t* ogc_coordinate_create_m(double x, double y, double z, double m);
void ogc_coordinate_destroy(ogc_coordinate_t* coord);
double ogc_coordinate_distance(const ogc_coordinate_t* a, const ogc_coordinate_t* b);

/* 2.2 Envelope */
/* Java: cn.cycle.chart.api.geometry.Envelope */
/* C++:   ogc::geom::Envelope */
/* Header: ogc/envelope.h */
typedef struct ogc_envelope_t ogc_envelope_t;

ogc_envelope_t* ogc_envelope_create(void);
ogc_envelope_t* ogc_envelope_create_from_coords(double min_x, double min_y, double max_x, double max_y);
void ogc_envelope_destroy(ogc_envelope_t* env);
double ogc_envelope_get_min_x(const ogc_envelope_t* env);
double ogc_envelope_get_min_y(const ogc_envelope_t* env);
double ogc_envelope_get_max_x(const ogc_envelope_t* env);
double ogc_envelope_get_max_y(const ogc_envelope_t* env);
double ogc_envelope_get_width(const ogc_envelope_t* env);
double ogc_envelope_get_height(const ogc_envelope_t* env);
double ogc_envelope_get_area(const ogc_envelope_t* env);
int ogc_envelope_contains(const ogc_envelope_t* env, double x, double y);
int ogc_envelope_intersects(const ogc_envelope_t* env, const ogc_envelope_t* other);
void ogc_envelope_expand(ogc_envelope_t* env, double dx, double dy);
ogc_coordinate_t ogc_envelope_get_center(const ogc_envelope_t* env);

/* 2.3 Geometry (Base Class) */
/* Java: cn.cycle.chart.api.geometry.Geometry */
/* C++:   ogc::geom::Geometry */
/* Header: ogc/geometry.h */
typedef struct ogc_geometry_t ogc_geometry_t;

typedef enum ogc_geom_type_e {
    OGC_GEOM_TYPE_UNKNOWN = 0,
    OGC_GEOM_TYPE_POINT = 1,
    OGC_GEOM_TYPE_LINESTRING = 2,
    OGC_GEOM_TYPE_POLYGON = 3,
    OGC_GEOM_TYPE_MULTIPOINT = 4,
    OGC_GEOM_TYPE_MULTILINESTRING = 5,
    OGC_GEOM_TYPE_MULTIPOLYGON = 6,
    OGC_GEOM_TYPE_GEOMETRYCOLLECTION = 7
} ogc_geom_type_e;

void ogc_geometry_destroy(ogc_geometry_t* geom);
ogc_geom_type_e ogc_geometry_get_type(const ogc_geometry_t* geom);
const char* ogc_geometry_get_type_name(const ogc_geometry_t* geom);
int ogc_geometry_get_dimension(const ogc_geometry_t* geom);
int ogc_geometry_is_empty(const ogc_geometry_t* geom);
int ogc_geometry_is_valid(const ogc_geometry_t* geom);
int ogc_geometry_is_simple(const ogc_geometry_t* geom);
int ogc_geometry_is_3d(const ogc_geometry_t* geom);
int ogc_geometry_is_measured(const ogc_geometry_t* geom);
int ogc_geometry_get_srid(const ogc_geometry_t* geom);
void ogc_geometry_set_srid(ogc_geometry_t* geom, int srid);
double ogc_geometry_get_area(const ogc_geometry_t* geom);
double ogc_geometry_get_length(const ogc_geometry_t* geom);
size_t ogc_geometry_get_num_points(const ogc_geometry_t* geom);
ogc_envelope_t* ogc_geometry_get_envelope(const ogc_geometry_t* geom);
ogc_coordinate_t ogc_geometry_get_centroid(const ogc_geometry_t* geom);
char* ogc_geometry_as_text(const ogc_geometry_t* geom, int precision);
char* ogc_geometry_as_geojson(const ogc_geometry_t* geom, int precision);
void ogc_geometry_free_string(char* str);

/* Geometry spatial operations */
int ogc_geometry_equals(const ogc_geometry_t* a, const ogc_geometry_t* b, double tolerance);
int ogc_geometry_intersects(const ogc_geometry_t* a, const ogc_geometry_t* b);
int ogc_geometry_contains(const ogc_geometry_t* a, const ogc_geometry_t* b);
int ogc_geometry_within(const ogc_geometry_t* a, const ogc_geometry_t* b);
int ogc_geometry_crosses(const ogc_geometry_t* a, const ogc_geometry_t* b);
int ogc_geometry_touches(const ogc_geometry_t* a, const ogc_geometry_t* b);
int ogc_geometry_overlaps(const ogc_geometry_t* a, const ogc_geometry_t* b);
double ogc_geometry_distance(const ogc_geometry_t* a, const ogc_geometry_t* b);
ogc_geometry_t* ogc_geometry_intersection(const ogc_geometry_t* a, const ogc_geometry_t* b);
ogc_geometry_t* ogc_geometry_union(const ogc_geometry_t* a, const ogc_geometry_t* b);
ogc_geometry_t* ogc_geometry_difference(const ogc_geometry_t* a, const ogc_geometry_t* b);
ogc_geometry_t* ogc_geometry_buffer(const ogc_geometry_t* geom, double distance, int segments);
ogc_geometry_t* ogc_geometry_clone(const ogc_geometry_t* geom);

/* 2.4 Point */
/* Java: cn.cycle.chart.api.geometry.Point */
/* C++:   ogc::geom::Point */
/* Header: ogc/point.h */
ogc_geometry_t* ogc_point_create(double x, double y);
ogc_geometry_t* ogc_point_create_3d(double x, double y, double z);
ogc_geometry_t* ogc_point_create_from_coord(const ogc_coordinate_t* coord);
double ogc_point_get_x(const ogc_geometry_t* point);
double ogc_point_get_y(const ogc_geometry_t* point);
double ogc_point_get_z(const ogc_geometry_t* point);
double ogc_point_get_m(const ogc_geometry_t* point);
void ogc_point_set_x(ogc_geometry_t* point, double x);
void ogc_point_set_y(ogc_geometry_t* point, double y);
void ogc_point_set_z(ogc_geometry_t* point, double z);
void ogc_point_set_m(ogc_geometry_t* point, double m);
ogc_coordinate_t ogc_point_get_coordinate(const ogc_geometry_t* point);

/* 2.5 LineString */
/* Java: cn.cycle.chart.api.geometry.LineString */
/* C++:   ogc::geom::LineString */
/* Header: ogc/linestring.h */
ogc_geometry_t* ogc_linestring_create(void);
ogc_geometry_t* ogc_linestring_create_from_coords(const ogc_coordinate_t* coords, size_t count);
size_t ogc_linestring_get_num_points(const ogc_geometry_t* line);
ogc_coordinate_t ogc_linestring_get_point_n(const ogc_geometry_t* line, size_t index);
void ogc_linestring_add_point(ogc_geometry_t* line, double x, double y);
void ogc_linestring_add_point_3d(ogc_geometry_t* line, double x, double y, double z);
void ogc_linestring_set_point_n(ogc_geometry_t* line, size_t index, const ogc_coordinate_t* coord);
ogc_geometry_t* ogc_linestring_get_point_geometry(const ogc_geometry_t* line, size_t index);

/* 2.6 LinearRing */
/* Java: cn.cycle.chart.api.geometry.LinearRing */
/* C++:   ogc::geom::LinearRing */
/* Header: ogc/linearring.h */
ogc_geometry_t* ogc_linearring_create(void);
ogc_geometry_t* ogc_linearring_create_from_coords(const ogc_coordinate_t* coords, size_t count);
int ogc_linearring_is_closed(const ogc_geometry_t* ring);

/* 2.7 Polygon */
/* Java: cn.cycle.chart.api.geometry.Polygon */
/* C++:   ogc::geom::Polygon */
/* Header: ogc/polygon.h */
ogc_geometry_t* ogc_polygon_create(void);
ogc_geometry_t* ogc_polygon_create_from_ring(ogc_geometry_t* exterior_ring);
ogc_geometry_t* ogc_polygon_create_from_coords(const ogc_coordinate_t* coords, size_t count);
ogc_geometry_t* ogc_polygon_get_exterior_ring(const ogc_geometry_t* polygon);
size_t ogc_polygon_get_num_interior_rings(const ogc_geometry_t* polygon);
ogc_geometry_t* ogc_polygon_get_interior_ring_n(const ogc_geometry_t* polygon, size_t index);
void ogc_polygon_add_interior_ring(ogc_geometry_t* polygon, ogc_geometry_t* ring);
int ogc_polygon_is_valid(const ogc_geometry_t* polygon);

/* 2.8 MultiPoint */
/* Java: cn.cycle.chart.api.geometry.MultiPoint */
/* C++:   ogc::geom::MultiPoint */
/* Header: ogc/multipoint.h */
ogc_geometry_t* ogc_multipoint_create(void);
size_t ogc_multipoint_get_num_geometries(const ogc_geometry_t* mp);
ogc_geometry_t* ogc_multipoint_get_geometry_n(const ogc_geometry_t* mp, size_t index);
void ogc_multipoint_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* point);

/* 2.9 MultiLineString */
/* Java: cn.cycle.chart.api.geometry.MultiLineString */
/* C++:   ogc::geom::MultiLineString */
/* Header: ogc/multilinestring.h */
ogc_geometry_t* ogc_multilinestring_create(void);
size_t ogc_multilinestring_get_num_geometries(const ogc_geometry_t* mls);
ogc_geometry_t* ogc_multilinestring_get_geometry_n(const ogc_geometry_t* mls, size_t index);
void ogc_multilinestring_add_geometry(ogc_geometry_t* mls, ogc_geometry_t* linestring);

/* 2.10 MultiPolygon */
/* Java: cn.cycle.chart.api.geometry.MultiPolygon */
/* C++:   ogc::geom::MultiPolygon */
/* Header: ogc/multipolygon.h */
ogc_geometry_t* ogc_multipolygon_create(void);
size_t ogc_multipolygon_get_num_geometries(const ogc_geometry_t* mp);
ogc_geometry_t* ogc_multipolygon_get_geometry_n(const ogc_geometry_t* mp, size_t index);
void ogc_multipolygon_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* polygon);

/* 2.11 GeometryCollection */
/* Java: cn.cycle.chart.api.geometry.GeometryCollection */
/* C++:   ogc::geom::GeometryCollection */
/* Header: ogc/geometrycollection.h */
ogc_geometry_t* ogc_geometry_collection_create(void);
size_t ogc_geometry_collection_get_num_geometries(const ogc_geometry_t* gc);
ogc_geometry_t* ogc_geometry_collection_get_geometry_n(const ogc_geometry_t* gc, size_t index);
void ogc_geometry_collection_add_geometry(ogc_geometry_t* gc, ogc_geometry_t* geom);

/* 2.12 GeometryFactory */
/* Java: cn.cycle.chart.api.geometry.GeometryFactory */
/* C++:   ogc::geom::GeometryFactory */
/* Header: ogc/factory.h */
typedef struct ogc_geometry_factory_t ogc_geometry_factory_t;

ogc_geometry_factory_t* ogc_geometry_factory_create(void);
void ogc_geometry_factory_destroy(ogc_geometry_factory_t* factory);
ogc_geometry_t* ogc_geometry_factory_create_point(ogc_geometry_factory_t* factory, double x, double y);
ogc_geometry_t* ogc_geometry_factory_create_linestring(ogc_geometry_factory_t* factory);
ogc_geometry_t* ogc_geometry_factory_create_polygon(ogc_geometry_factory_t* factory);
ogc_geometry_t* ogc_geometry_factory_create_from_wkt(ogc_geometry_factory_t* factory, const char* wkt);
ogc_geometry_t* ogc_geometry_factory_create_from_wkb(ogc_geometry_factory_t* factory, const unsigned char* wkb, size_t size);
ogc_geometry_t* ogc_geometry_factory_create_from_geojson(ogc_geometry_factory_t* factory, const char* geojson);

/* ============================================================================
 * 3. Feature Module (ogc_feature)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.feature
 * Description: Feature model and field definitions
 */

/* 3.1 FieldType */
/* Java: cn.cycle.chart.api.feature.FieldType */
/* C++:   ogc::feature::CNFieldType */
/* Header: ogc/feature/field_type.h */
typedef enum ogc_field_type_e {
    OGC_FIELD_TYPE_INTEGER = 0,
    OGC_FIELD_TYPE_INTEGER64 = 1,
    OGC_FIELD_TYPE_REAL = 2,
    OGC_FIELD_TYPE_STRING = 3,
    OGC_FIELD_TYPE_BINARY = 4,
    OGC_FIELD_TYPE_DATE = 5,
    OGC_FIELD_TYPE_TIME = 6,
    OGC_FIELD_TYPE_DATETIME = 7,
    OGC_FIELD_TYPE_INTEGER_LIST = 8,
    OGC_FIELD_TYPE_REAL_LIST = 9,
    OGC_FIELD_TYPE_STRING_LIST = 10
} ogc_field_type_e;

/* 3.2 FieldDefn */
/* Java: cn.cycle.chart.api.feature.FieldDefn */
/* C++:   ogc::feature::CNFieldDefn */
/* Header: ogc/feature/field_defn.h */
typedef struct ogc_field_defn_t ogc_field_defn_t;

ogc_field_defn_t* ogc_field_defn_create(const char* name, ogc_field_type_e type);
void ogc_field_defn_destroy(ogc_field_defn_t* defn);
const char* ogc_field_defn_get_name(const ogc_field_defn_t* defn);
ogc_field_type_e ogc_field_defn_get_type(const ogc_field_defn_t* defn);
int ogc_field_defn_get_width(const ogc_field_defn_t* defn);
int ogc_field_defn_get_precision(const ogc_field_defn_t* defn);
void ogc_field_defn_set_width(ogc_field_defn_t* defn, int width);
void ogc_field_defn_set_precision(ogc_field_defn_t* defn, int precision);

/* 3.3 FieldValue */
/* Java: cn.cycle.chart.api.feature.FieldValue */
/* C++:   ogc::feature::CNFieldValue */
/* Header: ogc/feature/field_value.h */
typedef struct ogc_field_value_t ogc_field_value_t;

ogc_field_value_t* ogc_field_value_create(void);
void ogc_field_value_destroy(ogc_field_value_t* value);
int ogc_field_value_is_null(const ogc_field_value_t* value);
int ogc_field_value_is_set(const ogc_field_value_t* value);
int ogc_field_value_get_as_integer(const ogc_field_value_t* value);
long long ogc_field_value_get_as_integer64(const ogc_field_value_t* value);
double ogc_field_value_get_as_real(const ogc_field_value_t* value);
const char* ogc_field_value_get_as_string(const ogc_field_value_t* value);
void ogc_field_value_set_integer(ogc_field_value_t* value, int val);
void ogc_field_value_set_integer64(ogc_field_value_t* value, long long val);
void ogc_field_value_set_real(ogc_field_value_t* value, double val);
void ogc_field_value_set_string(ogc_field_value_t* value, const char* val);
void ogc_field_value_set_null(ogc_field_value_t* value);

/* 3.4 FeatureDefn */
/* Java: cn.cycle.chart.api.feature.FeatureDefn (via CNFeatureDefn) */
/* C++:   ogc::feature::CNFeatureDefn */
/* Header: ogc/feature/feature_defn.h */
typedef struct ogc_feature_defn_t ogc_feature_defn_t;

ogc_feature_defn_t* ogc_feature_defn_create(const char* name);
void ogc_feature_defn_destroy(ogc_feature_defn_t* defn);
const char* ogc_feature_defn_get_name(const ogc_feature_defn_t* defn);
size_t ogc_feature_defn_get_field_count(const ogc_feature_defn_t* defn);
ogc_field_defn_t* ogc_feature_defn_get_field_defn(const ogc_feature_defn_t* defn, size_t index);
int ogc_feature_defn_get_field_index(const ogc_feature_defn_t* defn, const char* name);
void ogc_feature_defn_add_field_defn(ogc_feature_defn_t* defn, ogc_field_defn_t* field);
size_t ogc_feature_defn_get_geom_field_count(const ogc_feature_defn_t* defn);

/* 3.5 FeatureInfo */
/* Java: cn.cycle.chart.api.feature.FeatureInfo */
/* C++:   ogc::feature::CNFeature */
/* Header: ogc/feature/feature.h */
typedef struct ogc_feature_t ogc_feature_t;

ogc_feature_t* ogc_feature_create(ogc_feature_defn_t* defn);
void ogc_feature_destroy(ogc_feature_t* feature);
long long ogc_feature_get_fid(const ogc_feature_t* feature);
void ogc_feature_set_fid(ogc_feature_t* feature, long long fid);
ogc_feature_defn_t* ogc_feature_get_defn(const ogc_feature_t* feature);
size_t ogc_feature_get_field_count(const ogc_feature_t* feature);
int ogc_feature_is_field_set(const ogc_feature_t* feature, size_t index);
int ogc_feature_is_field_null(const ogc_feature_t* feature, size_t index);
int ogc_feature_get_field_as_integer(const ogc_feature_t* feature, size_t index);
long long ogc_feature_get_field_as_integer64(const ogc_feature_t* feature, size_t index);
double ogc_feature_get_field_as_real(const ogc_feature_t* feature, size_t index);
const char* ogc_feature_get_field_as_string(const ogc_feature_t* feature, size_t index);
void ogc_feature_set_field_integer(ogc_feature_t* feature, size_t index, int value);
void ogc_feature_set_field_integer64(ogc_feature_t* feature, size_t index, long long value);
void ogc_feature_set_field_real(ogc_feature_t* feature, size_t index, double value);
void ogc_feature_set_field_string(ogc_feature_t* feature, size_t index, const char* value);
void ogc_feature_set_field_null(ogc_feature_t* feature, size_t index);
ogc_geometry_t* ogc_feature_get_geometry(const ogc_feature_t* feature);
void ogc_feature_set_geometry(ogc_feature_t* feature, ogc_geometry_t* geom);
ogc_geometry_t* ogc_feature_steal_geometry(ogc_feature_t* feature);
ogc_envelope_t* ogc_feature_get_envelope(const ogc_feature_t* feature);
ogc_feature_t* ogc_feature_clone(const ogc_feature_t* feature);

/* ============================================================================
 * 4. Layer Module (ogc_layer)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.layer
 * Description: Layer management and data sources
 */

/* 4.1 LayerType */
/* Java: cn.cycle.chart.api.layer.LayerType */
/* C++:   ogc::layer::CNLayerType */
/* Header: ogc/layer/layer_type.h */
typedef enum ogc_layer_type_e {
    OGC_LAYER_TYPE_VECTOR = 0,
    OGC_LAYER_TYPE_RASTER = 1,
    OGC_LAYER_TYPE_UNKNOWN = 2
} ogc_layer_type_e;

/* 4.2 ChartLayer */
/* Java: cn.cycle.chart.api.layer.ChartLayer */
/* C++:   ogc::layer::CNLayer */
/* Header: ogc/layer/layer.h */
typedef struct ogc_layer_t ogc_layer_t;

void ogc_layer_destroy(ogc_layer_t* layer);
const char* ogc_layer_get_name(const ogc_layer_t* layer);
ogc_layer_type_e ogc_layer_get_type(const ogc_layer_t* layer);

/* 4.3 VectorLayer */
/* Java: cn.cycle.chart.api.layer.VectorLayer */
/* C++:   ogc::layer::CNVectorLayer */
/* Header: ogc/layer/vector_layer.h */
ogc_layer_t* ogc_vector_layer_create(const char* name);
ogc_layer_t* ogc_vector_layer_create_from_datasource(const char* datasource_path, const char* layer_name);
size_t ogc_vector_layer_get_feature_count(const ogc_layer_t* layer);
ogc_feature_t* ogc_vector_layer_get_next_feature(ogc_layer_t* layer);
ogc_feature_t* ogc_vector_layer_get_feature_by_id(ogc_layer_t* layer, int64_t fid);
void ogc_vector_layer_reset_reading(ogc_layer_t* layer);
void ogc_vector_layer_set_spatial_filter(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y);
ogc_envelope_t* ogc_vector_layer_get_extent(ogc_layer_t* layer);
int ogc_vector_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature);
int ogc_vector_layer_update_feature(ogc_layer_t* layer, const ogc_feature_t* feature);
int ogc_vector_layer_delete_feature(ogc_layer_t* layer, int64_t fid);

/* 4.4 RasterLayer */
/* Java: cn.cycle.chart.api.layer.RasterLayer */
/* C++:   ogc::layer::CNRasterLayer */
/* Header: ogc/layer/raster_layer.h */
ogc_layer_t* ogc_raster_layer_create(const char* name, const char* filepath);
int ogc_raster_layer_get_width(const ogc_layer_t* layer);
int ogc_raster_layer_get_height(const ogc_layer_t* layer);
int ogc_raster_layer_get_band_count(const ogc_layer_t* layer);
double ogc_raster_layer_get_no_data_value(const ogc_layer_t* layer, int band_index);
ogc_envelope_t* ogc_raster_layer_get_extent(ogc_layer_t* layer);

/* 4.5 MemoryLayer */
/* Java: cn.cycle.chart.api.layer.MemoryLayer */
/* C++:   ogc::layer::CNMemoryLayer */
/* Header: ogc/layer/memory_layer.h */
ogc_layer_t* ogc_memory_layer_create(const char* name);
ogc_layer_t* ogc_memory_layer_create_from_features(const char* name, ogc_feature_t** features, size_t count);
int ogc_memory_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature);
int ogc_memory_layer_remove_feature(ogc_layer_t* layer, int64_t fid);
void ogc_memory_layer_clear(ogc_layer_t* layer);

/* 4.6 LayerGroup */
/* Java: cn.cycle.chart.api.layer.LayerGroup */
/* C++:   ogc::layer::CNLayerGroup */
/* Header: ogc/layer/layer_group.h */
typedef struct ogc_layer_group_t ogc_layer_group_t;

ogc_layer_group_t* ogc_layer_group_create(const char* name);
void ogc_layer_group_destroy(ogc_layer_group_t* group);
const char* ogc_layer_group_get_name(const ogc_layer_group_t* group);
size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group);
ogc_layer_t* ogc_layer_group_get_layer(const ogc_layer_group_t* group, size_t index);
void ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer);
void ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index);
void ogc_layer_group_move_layer(ogc_layer_group_t* group, size_t from, size_t to);
int ogc_layer_group_is_visible(const ogc_layer_group_t* group);
void ogc_layer_group_set_visible(ogc_layer_group_t* group, int visible);
double ogc_layer_group_get_opacity(const ogc_layer_group_t* group);
void ogc_layer_group_set_opacity(ogc_layer_group_t* group, double opacity);

/* 4.7 DataSource */
/* Java: cn.cycle.chart.api.layer.DataSource */
/* C++:   ogc::layer::CNDataSource */
/* Header: ogc/layer/datasource.h */
typedef struct ogc_datasource_t ogc_datasource_t;

ogc_datasource_t* ogc_datasource_open(const char* path);
void ogc_datasource_close(ogc_datasource_t* ds);
int ogc_datasource_is_open(const ogc_datasource_t* ds);
const char* ogc_datasource_get_path(const ogc_datasource_t* ds);
size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds);
ogc_layer_t* ogc_datasource_get_layer(const ogc_datasource_t* ds, size_t index);
ogc_layer_t* ogc_datasource_get_layer_by_name(const ogc_datasource_t* ds, const char* name);
ogc_layer_t* ogc_datasource_create_layer(ogc_datasource_t* ds, const char* name, int geom_type);
int ogc_datasource_delete_layer(ogc_datasource_t* ds, const char* name);

/* 4.8 DriverManager */
/* Java: cn.cycle.chart.api.layer.DriverManager */
/* C++:   ogc::layer::CNDriverManager */
/* Header: ogc/layer/driver_manager.h */
typedef struct ogc_driver_manager_t ogc_driver_manager_t;
typedef struct ogc_driver_t ogc_driver_t;

ogc_driver_manager_t* ogc_driver_manager_get_instance(void);
int ogc_driver_manager_register_driver(ogc_driver_manager_t* mgr, ogc_driver_t* driver);
int ogc_driver_manager_unregister_driver(ogc_driver_manager_t* mgr, const char* name);
ogc_driver_t* ogc_driver_manager_get_driver(const ogc_driver_manager_t* mgr, const char* name);
int ogc_driver_manager_get_driver_count(const ogc_driver_manager_t* mgr);
const char* ogc_driver_manager_get_driver_name(const ogc_driver_manager_t* mgr, int index);
ogc_feature_defn_t* ogc_layer_get_feature_defn(ogc_layer_t* layer);
ogc_geom_type_e ogc_layer_get_geom_type(const ogc_layer_t* layer);
int ogc_layer_get_extent(ogc_layer_t* layer, ogc_envelope_t* extent);
long long ogc_layer_get_feature_count(ogc_layer_t* layer);
void ogc_layer_reset_reading(ogc_layer_t* layer);
ogc_feature_t* ogc_layer_get_next_feature(ogc_layer_t* layer);
ogc_feature_t* ogc_layer_get_feature(ogc_layer_t* layer, long long fid);
void ogc_layer_set_spatial_filter_rect(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y);
void ogc_layer_set_attribute_filter(ogc_layer_t* layer, const char* filter);

/* 4.3 VectorLayer */
/* Java: cn.cycle.chart.api.layer.VectorLayer */
/* C++:   ogc::layer::CNVectorLayer */
/* Header: ogc/layer/vector_layer.h */
ogc_layer_t* ogc_vector_layer_create(const char* name);
int ogc_vector_layer_create_feature(ogc_layer_t* layer, ogc_feature_t* feature);
int ogc_vector_layer_set_feature(ogc_layer_t* layer, ogc_feature_t* feature);
int ogc_vector_layer_delete_feature(ogc_layer_t* layer, long long fid);

/* 4.4 RasterLayer */
/* Java: cn.cycle.chart.api.layer.RasterLayer */
/* C++:   ogc::layer::CNRasterLayer */
/* Header: ogc/layer/raster_layer.h */
ogc_layer_t* ogc_raster_layer_create(const char* path);
int ogc_raster_layer_get_band_count(const ogc_layer_t* layer);
int ogc_raster_layer_get_size(const ogc_layer_t* layer, int* width, int* height);
double ogc_raster_layer_get_no_data_value(const ogc_layer_t* layer, int band);

/* 4.5 MemoryLayer */
/* Java: cn.cycle.chart.api.layer.MemoryLayer */
/* C++:   ogc::layer::CNMemoryLayer */
/* Header: ogc/layer/memory_layer.h */
ogc_layer_t* ogc_memory_layer_create(const char* name, ogc_feature_defn_t* defn);

/* 4.6 LayerGroup */
/* Java: cn.cycle.chart.api.layer.LayerGroup */
/* C++:   ogc::layer::CNLayerGroup */
/* Header: ogc/layer/layer_group.h */
typedef struct ogc_layer_group_t ogc_layer_group_t;

ogc_layer_group_t* ogc_layer_group_create(const char* name);
void ogc_layer_group_destroy(ogc_layer_group_t* group);
size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group);
ogc_layer_t* ogc_layer_group_get_layer(const ogc_layer_group_t* group, size_t index);
void ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer);
void ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index);
int ogc_layer_group_get_visible(const ogc_layer_group_t* group);
void ogc_layer_group_set_visible(ogc_layer_group_t* group, int visible);

/* 4.7 DataSource */
/* Java: cn.cycle.chart.api.layer.DataSource */
/* C++:   ogc::layer::CNDataSource */
/* Header: ogc/layer/datasource.h */
typedef struct ogc_datasource_t ogc_datasource_t;

ogc_datasource_t* ogc_datasource_open(const char* path);
void ogc_datasource_destroy(ogc_datasource_t* ds);
const char* ogc_datasource_get_name(const ogc_datasource_t* ds);
size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds);
ogc_layer_t* ogc_datasource_get_layer(ogc_datasource_t* ds, size_t index);
ogc_layer_t* ogc_datasource_get_layer_by_name(ogc_datasource_t* ds, const char* name);

/* 4.8 DriverManager */
/* Java: cn.cycle.chart.api.layer.DriverManager */
/* C++:   ogc::layer::CNDriverManager */
/* Header: ogc/layer/driver_manager.h */
typedef struct ogc_driver_manager_t ogc_driver_manager_t;

ogc_driver_manager_t* ogc_driver_manager_get_instance(void);
int ogc_driver_manager_register_driver(ogc_driver_manager_t* mgr, const char* name, const char* path);
int ogc_driver_manager_get_driver_count(const ogc_driver_manager_t* mgr);
const char* ogc_driver_manager_get_driver_name(const ogc_driver_manager_t* mgr, int index);

/* ============================================================================
 * 5. Draw Module (ogc_draw)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.draw
 * Description: Drawing engine and device abstraction
 */

/* 5.1 Color */
/* Java: cn.cycle.chart.api.util.Color */
/* C++:   ogc::draw::Color */
/* Header: ogc/draw/color.h */
typedef struct ogc_color_t {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ogc_color_t;

ogc_color_t ogc_color_from_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
ogc_color_t ogc_color_from_rgb(unsigned char r, unsigned char g, unsigned char b);
ogc_color_t ogc_color_from_hex(unsigned int hex);
unsigned int ogc_color_to_hex(const ogc_color_t* color);
void ogc_color_to_rgba(const ogc_color_t* color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

/* 5.2 Font */
/* Java: cn.cycle.chart.api.draw.Font */
/* C++:   ogc::draw::Font */
/* Header: ogc/draw/font.h */
typedef struct ogc_font_t ogc_font_t;

ogc_font_t* ogc_font_create(const char* family, double size);
void ogc_font_destroy(ogc_font_t* font);
const char* ogc_font_get_family(const ogc_font_t* font);
double ogc_font_get_size(const ogc_font_t* font);
int ogc_font_is_bold(const ogc_font_t* font);
int ogc_font_is_italic(const ogc_font_t* font);
void ogc_font_set_bold(ogc_font_t* font, int bold);
void ogc_font_set_italic(ogc_font_t* font, int italic);

/* 5.3 Pen */
/* Java: cn.cycle.chart.api.draw.Pen */
/* C++:   ogc::draw::Pen */
/* Header: ogc/draw/draw_style.h */
typedef struct ogc_pen_t ogc_pen_t;

ogc_pen_t* ogc_pen_create(void);
void ogc_pen_destroy(ogc_pen_t* pen);
ogc_color_t ogc_pen_get_color(const ogc_pen_t* pen);
double ogc_pen_get_width(const ogc_pen_t* pen);
void ogc_pen_set_color(ogc_pen_t* pen, ogc_color_t color);
void ogc_pen_set_width(ogc_pen_t* pen, double width);

/* 5.4 Brush */
/* Java: cn.cycle.chart.api.draw.Brush */
/* C++:   ogc::draw::Brush */
/* Header: ogc/draw/draw_style.h */
typedef struct ogc_brush_t ogc_brush_t;

ogc_brush_t* ogc_brush_create(void);
void ogc_brush_destroy(ogc_brush_t* brush);
ogc_color_t ogc_brush_get_color(const ogc_brush_t* brush);
void ogc_brush_set_color(ogc_brush_t* brush, ogc_color_t color);

/* 5.5 DrawStyle */
/* Java: cn.cycle.chart.api.symbology.DrawStyle */
/* C++:   ogc::draw::DrawStyle */
/* Header: ogc/draw/draw_style.h */
typedef struct ogc_draw_style_t ogc_draw_style_t;

ogc_draw_style_t* ogc_draw_style_create(void);
void ogc_draw_style_destroy(ogc_draw_style_t* style);
ogc_pen_t* ogc_draw_style_get_pen(ogc_draw_style_t* style);
ogc_brush_t* ogc_draw_style_get_brush(ogc_draw_style_t* style);
ogc_font_t* ogc_draw_style_get_font(ogc_draw_style_t* style);
void ogc_draw_style_set_pen(ogc_draw_style_t* style, ogc_pen_t* pen);
void ogc_draw_style_set_brush(ogc_draw_style_t* style, ogc_brush_t* brush);
void ogc_draw_style_set_font(ogc_draw_style_t* style, ogc_font_t* font);

/* 5.6 Image */
/* Java: cn.cycle.chart.api.draw.Image */
/* C++:   ogc::draw::Image */
/* Header: ogc/draw/image.h */
typedef struct ogc_image_t ogc_image_t;

ogc_image_t* ogc_image_create(int width, int height, int channels);
void ogc_image_destroy(ogc_image_t* image);
int ogc_image_get_width(const ogc_image_t* image);
int ogc_image_get_height(const ogc_image_t* image);
int ogc_image_get_channels(const ogc_image_t* image);
unsigned char* ogc_image_get_data(ogc_image_t* image);
const unsigned char* ogc_image_get_data_const(const ogc_image_t* image);
ogc_image_t* ogc_image_load_from_file(const char* path);
int ogc_image_save_to_file(const ogc_image_t* image, const char* path);

/* 5.7 DrawDevice */
/* Java: cn.cycle.chart.api.draw.DrawDevice */
/* C++:   ogc::draw::DrawDevice */
/* Header: ogc/draw/draw_device.h */
typedef struct ogc_draw_device_t ogc_draw_device_t;

typedef enum ogc_device_type_e {
    OGC_DEVICE_TYPE_DISPLAY = 0,
    OGC_DEVICE_TYPE_RASTER_IMAGE = 1,
    OGC_DEVICE_TYPE_PDF = 2,
    OGC_DEVICE_TYPE_SVG = 3,
    OGC_DEVICE_TYPE_TILE = 4,
    OGC_DEVICE_TYPE_WEBGL = 5
} ogc_device_type_e;

ogc_draw_device_t* ogc_draw_device_create(ogc_device_type_e type, int width, int height);
void ogc_draw_device_destroy(ogc_draw_device_t* device);
int ogc_draw_device_get_width(const ogc_draw_device_t* device);
int ogc_draw_device_get_height(const ogc_draw_device_t* device);
ogc_image_t* ogc_draw_device_get_image(ogc_draw_device_t* device);

/* 5.8 DrawEngine */
/* Java: cn.cycle.chart.api.draw.DrawEngine */
/* C++:   ogc::draw::DrawEngine */
/* Header: ogc/draw/draw_engine.h */
typedef struct ogc_draw_engine_t ogc_draw_engine_t;

typedef enum ogc_engine_type_e {
    OGC_ENGINE_TYPE_SIMPLE2D = 0,
    OGC_ENGINE_TYPE_GPU = 1,
    OGC_ENGINE_TYPE_VECTOR = 2,
    OGC_ENGINE_TYPE_TILE = 3
} ogc_engine_type_e;

ogc_draw_engine_t* ogc_draw_engine_create(ogc_engine_type_e type);
void ogc_draw_engine_destroy(ogc_draw_engine_t* engine);
int ogc_draw_engine_initialize(ogc_draw_engine_t* engine, ogc_draw_device_t* device);
void ogc_draw_engine_finalize(ogc_draw_engine_t* engine);

/* 5.9 DrawContext */
/* Java: cn.cycle.chart.api.core.RenderContext */
/* C++:   ogc::draw::DrawContext */
/* Header: ogc/draw/draw_context.h */
typedef struct ogc_draw_context_t ogc_draw_context_t;

ogc_draw_context_t* ogc_draw_context_create(ogc_draw_device_t* device, ogc_draw_engine_t* engine);
void ogc_draw_context_destroy(ogc_draw_context_t* ctx);
void ogc_draw_context_begin_draw(ogc_draw_context_t* ctx);
void ogc_draw_context_end_draw(ogc_draw_context_t* ctx);
void ogc_draw_context_clear(ogc_draw_context_t* ctx, ogc_color_t color);
void ogc_draw_context_draw_geometry(ogc_draw_context_t* ctx, const ogc_geometry_t* geom, const ogc_draw_style_t* style);
void ogc_draw_context_draw_point(ogc_draw_context_t* ctx, double x, double y, const ogc_draw_style_t* style);
void ogc_draw_context_draw_line(ogc_draw_context_t* ctx, double x1, double y1, double x2, double y2, const ogc_draw_style_t* style);
void ogc_draw_context_draw_rect(ogc_draw_context_t* ctx, double x, double y, double w, double h, const ogc_draw_style_t* style);
void ogc_draw_context_fill_rect(ogc_draw_context_t* ctx, double x, double y, double w, double h, const ogc_draw_style_t* style);
void ogc_draw_context_draw_text(ogc_draw_context_t* ctx, const char* text, double x, double y, const ogc_draw_style_t* style);
void ogc_draw_context_set_transform(ogc_draw_context_t* ctx, double* matrix);
void ogc_draw_context_reset_transform(ogc_draw_context_t* ctx);
void ogc_draw_context_clip(ogc_draw_context_t* ctx, const ogc_geometry_t* geom);
void ogc_draw_context_reset_clip(ogc_draw_context_t* ctx);

/* ============================================================================
 * 6. Graph Module (ogc_graph)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.core, cn.cycle.chart.api.graph
 * Description: Map rendering core, layer management, label engine
 */

/* 6.1 ChartViewer */
/* Java: cn.cycle.chart.api.core.ChartViewer */
/* C++:   ogc::graph::DrawFacade */
/* Header: ogc/graph/render/draw_facade.h */
typedef struct ogc_chart_viewer_t ogc_chart_viewer_t;

ogc_chart_viewer_t* ogc_chart_viewer_create(void);
void ogc_chart_viewer_destroy(ogc_chart_viewer_t* viewer);
int ogc_chart_viewer_initialize(ogc_chart_viewer_t* viewer);
void ogc_chart_viewer_shutdown(ogc_chart_viewer_t* viewer);
int ogc_chart_viewer_load_chart(ogc_chart_viewer_t* viewer, const char* path);
int ogc_chart_viewer_render(ogc_chart_viewer_t* viewer, ogc_draw_device_t* device, int width, int height);
void ogc_chart_viewer_set_viewport(ogc_chart_viewer_t* viewer, double center_x, double center_y, double scale);
void ogc_chart_viewer_get_viewport(ogc_chart_viewer_t* viewer, double* center_x, double* center_y, double* scale);
void ogc_chart_viewer_pan(ogc_chart_viewer_t* viewer, double dx, double dy);
void ogc_chart_viewer_zoom(ogc_chart_viewer_t* viewer, double factor, double center_x, double center_y);
ogc_feature_t* ogc_chart_viewer_query_feature(ogc_chart_viewer_t* viewer, double x, double y);
void ogc_chart_viewer_screen_to_world(ogc_chart_viewer_t* viewer, double screen_x, double screen_y, double* world_x, double* world_y);
void ogc_chart_viewer_world_to_screen(ogc_chart_viewer_t* viewer, double world_x, double world_y, double* screen_x, double* screen_y);

/* 6.2 Viewport */
/* Java: cn.cycle.chart.api.core.Viewport */
/* C++:   ogc::graph::Viewport (in DrawFacade) */
/* Header: ogc/graph/render/draw_facade.h */
typedef struct ogc_viewport_t ogc_viewport_t;

ogc_viewport_t* ogc_viewport_create(void);
void ogc_viewport_destroy(ogc_viewport_t* viewport);
double ogc_viewport_get_center_x(const ogc_viewport_t* viewport);
double ogc_viewport_get_center_y(const ogc_viewport_t* viewport);
double ogc_viewport_get_scale(const ogc_viewport_t* viewport);
double ogc_viewport_get_rotation(const ogc_viewport_t* viewport);
void ogc_viewport_set_center(ogc_viewport_t* viewport, double x, double y);
void ogc_viewport_set_scale(ogc_viewport_t* viewport, double scale);
void ogc_viewport_set_rotation(ogc_viewport_t* viewport, double rotation);
ogc_envelope_t* ogc_viewport_get_bounds(const ogc_viewport_t* viewport);
int ogc_viewport_zoom_to_extent(ogc_viewport_t* viewport, const ogc_envelope_t* extent);
int ogc_viewport_zoom_to_scale(ogc_viewport_t* viewport, double scale);

/* 6.3 ChartConfig */
/* Java: cn.cycle.chart.api.core.ChartConfig */
/* C++:   ogc::graph::DrawParams */
/* Header: ogc/graph/render/draw_params.h */
typedef struct ogc_chart_config_t ogc_chart_config_t;

ogc_chart_config_t* ogc_chart_config_create(void);
void ogc_chart_config_destroy(ogc_chart_config_t* config);
int ogc_chart_config_get_display_mode(const ogc_chart_config_t* config);
void ogc_chart_config_set_display_mode(ogc_chart_config_t* config, int mode);
int ogc_chart_config_get_show_grid(const ogc_chart_config_t* config);
void ogc_chart_config_set_show_grid(ogc_chart_config_t* config, int show);
double ogc_chart_config_get_dpi(const ogc_chart_config_t* config);
void ogc_chart_config_set_dpi(ogc_chart_config_t* config, double dpi);

/* 6.4 LayerManager */
/* Java: cn.cycle.chart.api.layer.LayerManager */
/* C++:   ogc::graph::LayerManager */
/* Header: ogc/graph/layer/layer_manager.h */
typedef struct ogc_layer_manager_t ogc_layer_manager_t;

ogc_layer_manager_t* ogc_layer_manager_create(void);
void ogc_layer_manager_destroy(ogc_layer_manager_t* mgr);
size_t ogc_layer_manager_get_layer_count(const ogc_layer_manager_t* mgr);
ogc_layer_t* ogc_layer_manager_get_layer(const ogc_layer_manager_t* mgr, size_t index);
ogc_layer_t* ogc_layer_manager_get_layer_by_name(const ogc_layer_manager_t* mgr, const char* name);
void ogc_layer_manager_add_layer(ogc_layer_manager_t* mgr, ogc_layer_t* layer);
void ogc_layer_manager_remove_layer(ogc_layer_manager_t* mgr, size_t index);
void ogc_layer_manager_move_layer(ogc_layer_manager_t* mgr, size_t from, size_t to);
int ogc_layer_manager_get_layer_visible(const ogc_layer_manager_t* mgr, size_t index);
void ogc_layer_manager_set_layer_visible(ogc_layer_manager_t* mgr, size_t index, int visible);
double ogc_layer_manager_get_layer_opacity(const ogc_layer_manager_t* mgr, size_t index);
void ogc_layer_manager_set_layer_opacity(ogc_layer_manager_t* mgr, size_t index, double opacity);

/* 6.5 LabelEngine */
/* Java: cn.cycle.chart.api.graph.LabelEngine */
/* C++:   ogc::graph::LabelEngine */
/* Header: ogc/graph/label/label_engine.h */
typedef struct ogc_label_engine_t ogc_label_engine_t;

ogc_label_engine_t* ogc_label_engine_create(void);
void ogc_label_engine_destroy(ogc_label_engine_t* engine);
void ogc_label_engine_set_max_labels(ogc_label_engine_t* engine, int max_labels);
int ogc_label_engine_get_max_labels(const ogc_label_engine_t* engine);
void ogc_label_engine_set_collision_detection(ogc_label_engine_t* engine, int enable);
void ogc_label_engine_clear(ogc_label_engine_t* engine);

/* 6.6 LabelInfo */
/* Java: cn.cycle.chart.api.graph.LabelInfo */
/* C++:   ogc::graph::LabelInfo */
/* Header: ogc/graph/label/label_info.h */
typedef struct ogc_label_info_t ogc_label_info_t;

ogc_label_info_t* ogc_label_info_create(const char* text, double x, double y);
void ogc_label_info_destroy(ogc_label_info_t* info);
const char* ogc_label_info_get_text(const ogc_label_info_t* info);
double ogc_label_info_get_x(const ogc_label_info_t* info);
double ogc_label_info_get_y(const ogc_label_info_t* info);
void ogc_label_info_set_text(ogc_label_info_t* info, const char* text);
void ogc_label_info_set_position(ogc_label_info_t* info, double x, double y);

/* 6.7 LODManager */
/* Java: cn.cycle.chart.api.graph.LODManager */
/* C++:   ogc::graph::LODManager */
/* Header: ogc/graph/lod/lod_manager.h */
typedef struct ogc_lod_manager_t ogc_lod_manager_t;

ogc_lod_manager_t* ogc_lod_manager_create(void);
void ogc_lod_manager_destroy(ogc_lod_manager_t* mgr);
int ogc_lod_manager_get_current_level(const ogc_lod_manager_t* mgr);
void ogc_lod_manager_set_current_level(ogc_lod_manager_t* mgr, int level);
int ogc_lod_manager_get_level_count(const ogc_lod_manager_t* mgr);
double ogc_lod_manager_get_scale_for_level(const ogc_lod_manager_t* mgr, int level);

/* 6.8 HitTest */
/* Java: cn.cycle.chart.api.graph.HitTest */
/* C++:   ogc::graph::HitTest */
/* Header: ogc/graph/interaction/hit_test.h */
typedef struct ogc_hit_test_t ogc_hit_test_t;

ogc_hit_test_t* ogc_hit_test_create(void);
void ogc_hit_test_destroy(ogc_hit_test_t* hit_test);
ogc_feature_t* ogc_hit_test_query_point(ogc_hit_test_t* hit_test, ogc_layer_t* layer, double x, double y, double tolerance);
ogc_feature_t** ogc_hit_test_query_rect(ogc_hit_test_t* hit_test, ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y, int* count);
void ogc_hit_test_free_features(ogc_feature_t** features);

/* 6.9 TransformManager */
/* Java: cn.cycle.chart.api.graph.TransformManager */
/* C++:   ogc::graph::TransformManager */
/* Header: ogc/graph/transform/transform_manager.h */
typedef struct ogc_transform_manager_t ogc_transform_manager_t;

ogc_transform_manager_t* ogc_transform_manager_create(void);
void ogc_transform_manager_destroy(ogc_transform_manager_t* mgr);
void ogc_transform_manager_set_viewport(ogc_transform_manager_t* mgr, double center_x, double center_y, double scale, double rotation);
void ogc_transform_manager_screen_to_world(const ogc_transform_manager_t* mgr, double sx, double sy, double* wx, double* wy);
void ogc_transform_manager_world_to_screen(const ogc_transform_manager_t* mgr, double wx, double wy, double* sx, double* sy);
void ogc_transform_manager_get_matrix(const ogc_transform_manager_t* mgr, double* matrix);

/* 6.10 RenderTask */
/* Java: cn.cycle.chart.api.graph.RenderTask */
/* C++:   ogc::graph::RenderTask */
/* Header: ogc/graph/render/render_task.h */
typedef struct ogc_render_task_t ogc_render_task_t;

ogc_render_task_t* ogc_render_task_create(void);
void ogc_render_task_destroy(ogc_render_task_t* task);
int ogc_render_task_execute(ogc_render_task_t* task);
int ogc_render_task_is_complete(const ogc_render_task_t* task);
void ogc_render_task_cancel(ogc_render_task_t* task);

/* 6.11 RenderQueue */
/* Java: cn.cycle.chart.api.graph.RenderQueue */
/* C++:   ogc::graph::RenderQueue */
/* Header: ogc/graph/render/render_queue.h */
typedef struct ogc_render_queue_t ogc_render_queue_t;

ogc_render_queue_t* ogc_render_queue_create(void);
void ogc_render_queue_destroy(ogc_render_queue_t* queue);
void ogc_render_queue_push(ogc_render_queue_t* queue, ogc_render_task_t* task);
ogc_render_task_t* ogc_render_queue_pop(ogc_render_queue_t* queue);
size_t ogc_render_queue_get_size(const ogc_render_queue_t* queue);
void ogc_render_queue_clear(ogc_render_queue_t* queue);

/* 6.12 RenderOptimizer */
/* Java: cn.cycle.chart.api.optimize.RenderOptimizer */
/* C++:   ogc::graph::RenderOptimizer */
/* Header: ogc/graph/render/render_optimizer.h */
typedef struct ogc_render_optimizer_t ogc_render_optimizer_t;

ogc_render_optimizer_t* ogc_render_optimizer_create(void);
void ogc_render_optimizer_destroy(ogc_render_optimizer_t* optimizer);
void ogc_render_optimizer_set_cache_enabled(ogc_render_optimizer_t* optimizer, int enable);
int ogc_render_optimizer_is_cache_enabled(const ogc_render_optimizer_t* optimizer);
void ogc_render_optimizer_clear_cache(ogc_render_optimizer_t* optimizer);

/* 6.13 RenderStats */
/* Java: cn.cycle.chart.api.optimize.RenderStats */
/* C++:   ogc::graph::RenderStats */
/* Header: ogc/graph/render/render_stats.h */
typedef struct ogc_render_stats_t {
    int total_frames;
    double total_render_time_ms;
    double avg_render_time_ms;
    int feature_count;
    int label_count;
    int cache_hits;
    int cache_misses;
} ogc_render_stats_t;

void ogc_render_stats_reset(ogc_render_stats_t* stats);

/* ============================================================================
 * 7. Cache Module (ogc_cache)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.cache
 * Description: Tile cache and offline storage
 */

/* 7.1 TileKey */
/* Java: cn.cycle.chart.api.cache.TileKey */
/* C++:   ogc::cache::TileKey */
/* Header: ogc/cache/tile/tile_key.h */
typedef struct ogc_tile_key_t {
    int x;
    int y;
    int z;
} ogc_tile_key_t;

ogc_tile_key_t ogc_tile_key_create(int x, int y, int z);
int ogc_tile_key_equals(const ogc_tile_key_t* a, const ogc_tile_key_t* b);
char* ogc_tile_key_to_string(const ogc_tile_key_t* key);
ogc_tile_key_t ogc_tile_key_from_string(const char* str);
uint64_t ogc_tile_key_to_index(const ogc_tile_key_t* key);
ogc_tile_key_t ogc_tile_key_get_parent(const ogc_tile_key_t* key);
void ogc_tile_key_get_children(const ogc_tile_key_t* key, ogc_tile_key_t children[4]);
ogc_envelope_t* ogc_tile_key_to_envelope(const ogc_tile_key_t* key);

/* 7.2 TileCache */
/* Java: cn.cycle.chart.api.cache.TileCache */
/* C++:   ogc::cache::TileCache */
/* Header: ogc/cache/tile/tile_cache.h */
typedef struct ogc_tile_cache_t ogc_tile_cache_t;
typedef struct ogc_tile_data_t ogc_tile_data_t;

ogc_tile_cache_t* ogc_tile_cache_create(void);
void ogc_tile_cache_destroy(ogc_tile_cache_t* cache);
int ogc_tile_cache_has_tile(const ogc_tile_cache_t* cache, const ogc_tile_key_t* key);
ogc_tile_data_t* ogc_tile_cache_get_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key);
void ogc_tile_cache_put_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key, const void* data, size_t size);
void ogc_tile_cache_remove_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key);
void ogc_tile_cache_clear(ogc_tile_cache_t* cache);
size_t ogc_tile_cache_get_size(const ogc_tile_cache_t* cache);
size_t ogc_tile_cache_get_max_size(const ogc_tile_cache_t* cache);
void ogc_tile_cache_set_max_size(ogc_tile_cache_t* cache, size_t max_size);

/* 7.3 MemoryTileCache */
/* Java: cn.cycle.chart.api.cache.MemoryTileCache */
/* C++:   ogc::cache::MemoryTileCache */
/* Header: ogc/cache/tile/memory_tile_cache.h */
ogc_tile_cache_t* ogc_memory_tile_cache_create(size_t max_size_bytes);

/* 7.4 DiskTileCache */
/* Java: cn.cycle.chart.api.cache.DiskTileCache */
/* C++:   ogc::cache::DiskTileCache */
/* Header: ogc/cache/tile/disk_tile_cache.h */
ogc_tile_cache_t* ogc_disk_tile_cache_create(const char* cache_dir, size_t max_size_bytes);

/* 7.5 MultiLevelTileCache */
/* Java: cn.cycle.chart.api.cache.MultiLevelTileCache */
/* C++:   ogc::cache::MultiLevelTileCache */
/* Header: ogc/cache/tile/multi_level_tile_cache.h */
ogc_tile_cache_t* ogc_multi_level_tile_cache_create(ogc_tile_cache_t* memory_cache, ogc_tile_cache_t* disk_cache);

/* 7.6 OfflineStorageManager */
/* Java: cn.cycle.chart.api.cache.OfflineStorageManager */
/* C++:   ogc::cache::OfflineStorageManager */
/* Header: ogc/cache/offline/offline_storage_manager.h */
typedef struct ogc_offline_storage_t ogc_offline_storage_t;
typedef struct ogc_offline_region_t ogc_offline_region_t;

ogc_offline_storage_t* ogc_offline_storage_create(const char* storage_path);
void ogc_offline_storage_destroy(ogc_offline_storage_t* storage);
ogc_offline_region_t* ogc_offline_storage_create_region(ogc_offline_storage_t* storage, const ogc_envelope_t* bounds, int min_zoom, int max_zoom);
void ogc_offline_region_destroy(ogc_offline_region_t* region);
int ogc_offline_region_download(ogc_offline_region_t* region);
float ogc_offline_region_get_progress(const ogc_offline_region_t* region);
size_t ogc_offline_storage_get_region_count(const ogc_offline_storage_t* storage);
ogc_offline_region_t* ogc_offline_storage_get_region(const ogc_offline_storage_t* storage, size_t index);
void ogc_offline_storage_remove_region(ogc_offline_storage_t* storage, size_t index);

/* 7.7 DataEncryption */
/* Java: cn.cycle.chart.api.cache.DataEncryption */
/* C++:   ogc::cache::DataEncryption */
/* Header: ogc/cache/offline/data_encryption.h */
typedef struct ogc_data_encryption_t ogc_data_encryption_t;

ogc_data_encryption_t* ogc_data_encryption_create(const char* key);
void ogc_data_encryption_destroy(ogc_data_encryption_t* encryption);
int ogc_data_encryption_encrypt(ogc_data_encryption_t* encryption, const void* input, size_t input_size, void* output, size_t* output_size);
int ogc_data_encryption_decrypt(ogc_data_encryption_t* encryption, const void* input, size_t input_size, void* output, size_t* output_size);

/* ============================================================================
 * 8. Symbology Module (ogc_symbology)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.symbology
 * Description: Symbolization and style rules
 */

/* 8.1 Filter */
/* Java: cn.cycle.chart.api.symbology.Filter */
/* C++:   ogc::symbology::Filter */
/* Header: ogc/symbology/filter/filter.h */
typedef struct ogc_filter_t ogc_filter_t;

typedef enum ogc_filter_type_e {
    OGC_FILTER_TYPE_COMPARISON = 0,
    OGC_FILTER_TYPE_LOGICAL = 1,
    OGC_FILTER_TYPE_SPATIAL = 2,
    OGC_FILTER_TYPE_FEATURE_ID = 3,
    OGC_FILTER_TYPE_NONE = 4
} ogc_filter_type_e;

ogc_filter_t* ogc_filter_create(void);
void ogc_filter_destroy(ogc_filter_t* filter);
ogc_filter_type_e ogc_filter_get_type(const ogc_filter_t* filter);
int ogc_filter_evaluate(const ogc_filter_t* filter, const ogc_feature_t* feature);
char* ogc_filter_to_string(const ogc_filter_t* filter);

/* 8.2 ComparisonFilter */
/* Java: cn.cycle.chart.api.symbology.ComparisonFilter */
/* C++:   ogc::symbology::ComparisonFilter */
/* Header: ogc/symbology/filter/comparison_filter.h */
typedef enum ogc_comparison_operator_e {
    OGC_COMPARISON_EQUAL = 0,
    OGC_COMPARISON_NOT_EQUAL = 1,
    OGC_COMPARISON_LESS_THAN = 2,
    OGC_COMPARISON_GREATER_THAN = 3,
    OGC_COMPARISON_LESS_THAN_OR_EQUAL = 4,
    OGC_COMPARISON_GREATER_THAN_OR_EQUAL = 5,
    OGC_COMPARISON_LIKE = 6,
    OGC_COMPARISON_IS_NULL = 7
} ogc_comparison_operator_e;

ogc_filter_t* ogc_comparison_filter_create(const char* property, ogc_comparison_operator_e op, const char* value);

/* 8.3 LogicalFilter */
/* Java: cn.cycle.chart.api.symbology.LogicalFilter */
/* C++:   ogc::symbology::LogicalFilter */
/* Header: ogc/symbology/filter/logical_filter.h */
typedef enum ogc_logical_operator_e {
    OGC_LOGICAL_AND = 0,
    OGC_LOGICAL_OR = 1,
    OGC_LOGICAL_NOT = 2
} ogc_logical_operator_e;

ogc_filter_t* ogc_logical_filter_create(ogc_logical_operator_e op);
void ogc_logical_filter_add_filter(ogc_filter_t* logical, ogc_filter_t* filter);

/* 8.4 Symbolizer */
/* Java: cn.cycle.chart.api.symbology.Symbolizer */
/* C++:   ogc::symbology::Symbolizer */
/* Header: ogc/symbology/symbolizer/symbolizer.h */
typedef struct ogc_symbolizer_t ogc_symbolizer_t;

typedef enum ogc_symbolizer_type_e {
    OGC_SYMBOLIZER_POINT = 0,
    OGC_SYMBOLIZER_LINE = 1,
    OGC_SYMBOLIZER_POLYGON = 2,
    OGC_SYMBOLIZER_TEXT = 3,
    OGC_SYMBOLIZER_RASTER = 4,
    OGC_SYMBOLIZER_ICON = 5,
    OGC_SYMBOLIZER_COMPOSITE = 6
} ogc_symbolizer_type_e;

ogc_symbolizer_t* ogc_symbolizer_create(ogc_symbolizer_type_e type);
void ogc_symbolizer_destroy(ogc_symbolizer_t* symbolizer);
ogc_symbolizer_type_e ogc_symbolizer_get_type(const ogc_symbolizer_t* symbolizer);
ogc_draw_style_t* ogc_symbolizer_get_style(ogc_symbolizer_t* symbolizer);
void ogc_symbolizer_set_style(ogc_symbolizer_t* symbolizer, ogc_draw_style_t* style);

/* 8.5 ComparisonFilter */
/* Java: cn.cycle.chart.api.symbology.ComparisonFilter */
/* C++:   ogc::symbology::ComparisonFilter */
/* Header: ogc/symbology/filter/comparison_filter.h */
typedef struct ogc_comparison_filter_t ogc_comparison_filter_t;

typedef enum ogc_comparison_operator_e {
    OGC_COMPARISON_EQUAL = 0,
    OGC_COMPARISON_NOT_EQUAL = 1,
    OGC_COMPARISON_LESS_THAN = 2,
    OGC_COMPARISON_GREATER_THAN = 3,
    OGC_COMPARISON_LESS_EQUAL = 4,
    OGC_COMPARISON_GREATER_EQUAL = 5,
    OGC_COMPARISON_LIKE = 6,
    OGC_COMPARISON_IS_NULL = 7,
    OGC_COMPARISON_BETWEEN = 8
} ogc_comparison_operator_e;

ogc_comparison_filter_t* ogc_comparison_filter_create(const char* property_name, ogc_comparison_operator_e op, const char* value);
void ogc_comparison_filter_destroy(ogc_comparison_filter_t* filter);
int ogc_comparison_filter_evaluate(const ogc_comparison_filter_t* filter, const ogc_feature_t* feature);
const char* ogc_comparison_filter_get_property_name(const ogc_comparison_filter_t* filter);
ogc_comparison_operator_e ogc_comparison_filter_get_operator(const ogc_comparison_filter_t* filter);
const char* ogc_comparison_filter_get_value(const ogc_comparison_filter_t* filter);

/* 8.5 SymbolizerRule */
/* Java: cn.cycle.chart.api.symbology.SymbolizerRule */
/* C++:   ogc::symbology::SymbolizerRule */
/* Header: ogc/symbology/filter/symbolizer_rule.h */
typedef struct ogc_symbolizer_rule_t ogc_symbolizer_rule_t;

ogc_symbolizer_rule_t* ogc_symbolizer_rule_create(void);
void ogc_symbolizer_rule_destroy(ogc_symbolizer_rule_t* rule);
const char* ogc_symbolizer_rule_get_name(const ogc_symbolizer_rule_t* rule);
void ogc_symbolizer_rule_set_name(ogc_symbolizer_rule_t* rule, const char* name);
ogc_filter_t* ogc_symbolizer_rule_get_filter(const ogc_symbolizer_rule_t* rule);
void ogc_symbolizer_rule_set_filter(ogc_symbolizer_rule_t* rule, ogc_filter_t* filter);
ogc_symbolizer_t* ogc_symbolizer_rule_get_symbolizer(const ogc_symbolizer_rule_t* rule);
void ogc_symbolizer_rule_set_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer);
double ogc_symbolizer_rule_get_min_scale(const ogc_symbolizer_rule_t* rule);
double ogc_symbolizer_rule_get_max_scale(const ogc_symbolizer_rule_t* rule);
void ogc_symbolizer_rule_set_scale_range(ogc_symbolizer_rule_t* rule, double min_scale, double max_scale);

/* ============================================================================
 * 9. Alert Module (ogc_alert)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.alert
 * Description: Maritime alert system
 */

/* 9.1 AlertType */
/* Java: cn.cycle.chart.api.alert.AlertType */
/* C++:   ogc::alert::AlertType */
/* Header: ogc/alert/types.h */
typedef enum ogc_alert_type_e {
    OGC_ALERT_TYPE_DEPTH = 0,
    OGC_ALERT_TYPE_WEATHER = 1,
    OGC_ALERT_TYPE_COLLISION = 2,
    OGC_ALERT_TYPE_CHANNEL = 3,
    OGC_ALERT_TYPE_DEVIATION = 4,
    OGC_ALERT_TYPE_SPEED = 5,
    OGC_ALERT_TYPE_RESTRICTED_AREA = 6
} ogc_alert_type_e;

/* 9.2 AlertLevel */
/* Java: cn.cycle.chart.api.alert.AlertLevel */
/* C++:   ogc::alert::AlertLevel */
/* Header: ogc/alert/types.h */
typedef enum ogc_alert_level_e {
    OGC_ALERT_LEVEL_INFO = 0,
    OGC_ALERT_LEVEL_WARNING = 1,
    OGC_ALERT_LEVEL_ALARM = 2,
    OGC_ALERT_LEVEL_CRITICAL = 3
} ogc_alert_level_e;

/* 9.3 Alert */
/* Java: cn.cycle.chart.api.alert.Alert */
/* C++:   ogc::alert::Alert */
/* Header: ogc/alert/alert.h */
typedef struct ogc_alert_t ogc_alert_t;

ogc_alert_t* ogc_alert_create(ogc_alert_type_e type, ogc_alert_level_e level, const char* message);
void ogc_alert_destroy(ogc_alert_t* alert);
ogc_alert_type_e ogc_alert_get_type(const ogc_alert_t* alert);
ogc_alert_level_e ogc_alert_get_level(const ogc_alert_t* alert);
const char* ogc_alert_get_message(const ogc_alert_t* alert);
double ogc_alert_get_timestamp(const ogc_alert_t* alert);
ogc_coordinate_t ogc_alert_get_position(const ogc_alert_t* alert);
void ogc_alert_set_position(ogc_alert_t* alert, const ogc_coordinate_t* pos);

/* 9.4 AlertEngine */
/* Java: cn.cycle.chart.api.alert.AlertEngine */
/* C++:   ogc::alert::AlertEngine */
/* Header: ogc/alert/alert_engine.h */
typedef struct ogc_alert_engine_t ogc_alert_engine_t;

ogc_alert_engine_t* ogc_alert_engine_create(void);
void ogc_alert_engine_destroy(ogc_alert_engine_t* engine);
int ogc_alert_engine_initialize(ogc_alert_engine_t* engine);
void ogc_alert_engine_shutdown(ogc_alert_engine_t* engine);
void ogc_alert_engine_check_all(ogc_alert_engine_t* engine);
ogc_alert_t** ogc_alert_engine_get_active_alerts(ogc_alert_engine_t* engine, int* count);
void ogc_alert_engine_free_alerts(ogc_alert_t** alerts);
void ogc_alert_engine_acknowledge_alert(ogc_alert_engine_t* engine, const ogc_alert_t* alert);

/* 9.5 CpaCalculator */
/* Java: cn.cycle.chart.api.navi.CpaCalculator */
/* C++:   ogc::alert::CpaCalculator */
/* Header: ogc/alert/cpa_calculator.h */
typedef struct ogc_cpa_result_t {
    double cpa;
    double tcpa;
    double bearing;
    int danger_level;
} ogc_cpa_result_t;

void ogc_cpa_calculate(double own_lat, double own_lon, double own_speed, double own_course,
                       double target_lat, double target_lon, double target_speed, double target_course,
                       ogc_cpa_result_t* result);

/* 9.6 UkcCalculator */
/* Java: cn.cycle.chart.api.navi.UkcCalculator */
/* C++:   ogc::alert::UkcCalculator */
/* Header: ogc/alert/ukc_calculator.h */
typedef struct ogc_ukc_result_t {
    double water_depth;
    double charted_depth;
    double tide_height;
    double squat;
    double ukc;
    int safe;
} ogc_ukc_result_t;

void ogc_ukc_calculate(double charted_depth, double tide_height, double draft, 
                       double speed, double squat_coeff, ogc_ukc_result_t* result);

/* ============================================================================
 * 10. Navigation Module (ogc_navi)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.navi
 * Description: Navigation system including AIS, routes, tracks
 */

/* 10.1 RouteStatus */
/* Java: cn.cycle.chart.api.navi.RouteStatus */
/* C++:   ogc::navi::RouteStatus */
/* Header: ogc/navi/types.h */
typedef enum ogc_route_status_e {
    OGC_ROUTE_STATUS_PLANNING = 0,
    OGC_ROUTE_STATUS_ACTIVE = 1,
    OGC_ROUTE_STATUS_COMPLETED = 2,
    OGC_ROUTE_STATUS_CANCELLED = 3
} ogc_route_status_e;

/* 10.2 NavigationStatus */
/* Java: cn.cycle.chart.api.navi.NavigationStatus */
/* C++:   ogc::navi::NavigationStatus */
/* Header: ogc/navi/types.h */
typedef enum ogc_navigation_status_e {
    OGC_NAV_STATUS_STANDBY = 0,
    OGC_NAV_STATUS_NAVIGATING = 1,
    OGC_NAV_STATUS_PAUSED = 2,
    OGC_NAV_STATUS_ARRIVED = 3
} ogc_navigation_status_e;

/* 10.3 Waypoint */
/* Java: cn.cycle.chart.api.navi.Waypoint */
/* C++:   ogc::navi::Waypoint */
/* Header: ogc/navi/route/waypoint.h */
typedef struct ogc_waypoint_t ogc_waypoint_t;

ogc_waypoint_t* ogc_waypoint_create(double lat, double lon);
void ogc_waypoint_destroy(ogc_waypoint_t* wp);
double ogc_waypoint_get_latitude(const ogc_waypoint_t* wp);
double ogc_waypoint_get_longitude(const ogc_waypoint_t* wp);
const char* ogc_waypoint_get_name(const ogc_waypoint_t* wp);
void ogc_waypoint_set_name(ogc_waypoint_t* wp, const char* name);
int ogc_waypoint_is_arrival(const ogc_waypoint_t* wp);
void ogc_waypoint_set_arrival_radius(ogc_waypoint_t* wp, double radius);

/* 10.4 Route */
/* Java: cn.cycle.chart.api.navi.Route */
/* C++:   ogc::navi::Route */
/* Header: ogc/navi/route/route.h */
typedef struct ogc_route_t ogc_route_t;

ogc_route_t* ogc_route_create(void);
void ogc_route_destroy(ogc_route_t* route);
const char* ogc_route_get_id(const ogc_route_t* route);
const char* ogc_route_get_name(const ogc_route_t* route);
void ogc_route_set_name(ogc_route_t* route, const char* name);
ogc_route_status_e ogc_route_get_status(const ogc_route_t* route);
double ogc_route_get_total_distance(const ogc_route_t* route);
size_t ogc_route_get_waypoint_count(const ogc_route_t* route);
ogc_waypoint_t* ogc_route_get_waypoint(const ogc_route_t* route, size_t index);
void ogc_route_add_waypoint(ogc_route_t* route, ogc_waypoint_t* wp);
void ogc_route_remove_waypoint(ogc_route_t* route, size_t index);
ogc_waypoint_t* ogc_route_get_current_waypoint(const ogc_route_t* route);
int ogc_route_advance_to_next_waypoint(ogc_route_t* route);

/* 10.5 RouteManager */
/* Java: cn.cycle.chart.api.navi.RouteManager */
/* C++:   ogc::navi::RouteManager */
/* Header: ogc/navi/route/route_manager.h */
typedef struct ogc_route_manager_t ogc_route_manager_t;

ogc_route_manager_t* ogc_route_manager_create(void);
void ogc_route_manager_destroy(ogc_route_manager_t* mgr);
size_t ogc_route_manager_get_route_count(const ogc_route_manager_t* mgr);
ogc_route_t* ogc_route_manager_get_route(const ogc_route_manager_t* mgr, size_t index);
ogc_route_t* ogc_route_manager_get_route_by_id(const ogc_route_manager_t* mgr, const char* id);
void ogc_route_manager_add_route(ogc_route_manager_t* mgr, ogc_route_t* route);
void ogc_route_manager_remove_route(ogc_route_manager_t* mgr, const char* id);
ogc_route_t* ogc_route_manager_get_active_route(const ogc_route_manager_t* mgr);
void ogc_route_manager_set_active_route(ogc_route_manager_t* mgr, const char* id);

/* 10.6 RoutePlanner */
/* Java: cn.cycle.chart.api.navi.RoutePlanner */
/* C++:   ogc::navi::RoutePlanner */
/* Header: ogc/navi/route/route_planner.h */
typedef struct ogc_route_planner_t ogc_route_planner_t;

ogc_route_planner_t* ogc_route_planner_create(void);
void ogc_route_planner_destroy(ogc_route_planner_t* planner);
ogc_route_t* ogc_route_planner_plan_route(ogc_route_planner_t* planner, 
                                           const ogc_waypoint_t* start, 
                                           const ogc_waypoint_t* end,
                                           const ogc_envelope_t* avoid_areas,
                                           int avoid_count);

/* 10.7 AisTarget */
/* Java: cn.cycle.chart.api.navi.AisTarget */
/* C++:   ogc::navi::AisTarget */
/* Header: ogc/navi/ais/ais_target.h */
typedef struct ogc_ais_target_t ogc_ais_target_t;

ogc_ais_target_t* ogc_ais_target_create(uint32_t mmsi);
void ogc_ais_target_destroy(ogc_ais_target_t* target);
uint32_t ogc_ais_target_get_mmsi(const ogc_ais_target_t* target);
const char* ogc_ais_target_get_name(const ogc_ais_target_t* target);
double ogc_ais_target_get_latitude(const ogc_ais_target_t* target);
double ogc_ais_target_get_longitude(const ogc_ais_target_t* target);
double ogc_ais_target_get_speed(const ogc_ais_target_t* target);
double ogc_ais_target_get_course(const ogc_ais_target_t* target);
double ogc_ais_target_get_heading(const ogc_ais_target_t* target);
int ogc_ais_target_get_nav_status(const ogc_ais_target_t* target);
void ogc_ais_target_update_position(ogc_ais_target_t* target, double lat, double lon, double speed, double course);

/* 10.8 AisManager */
/* Java: cn.cycle.chart.api.navi.AisManager */
/* C++:   ogc::navi::AisManager */
/* Header: ogc/navi/ais/ais_manager.h */
typedef struct ogc_ais_manager_t ogc_ais_manager_t;

ogc_ais_manager_t* ogc_ais_manager_create(void);
void ogc_ais_manager_destroy(ogc_ais_manager_t* mgr);
int ogc_ais_manager_initialize(ogc_ais_manager_t* mgr);
void ogc_ais_manager_shutdown(ogc_ais_manager_t* mgr);
ogc_ais_target_t* ogc_ais_manager_get_target(ogc_ais_manager_t* mgr, uint32_t mmsi);
ogc_ais_target_t** ogc_ais_manager_get_all_targets(ogc_ais_manager_t* mgr, int* count);
void ogc_ais_manager_free_targets(ogc_ais_target_t** targets);
ogc_ais_target_t** ogc_ais_manager_get_targets_in_range(ogc_ais_manager_t* mgr, double lat, double lon, double range_nm, int* count);

/* 10.9 NavigationEngine */
/* Java: cn.cycle.chart.api.navi.NavigationEngine */
/* C++:   ogc::navi::NavigationEngine */
/* Header: ogc/navi/navigation/navigation_engine.h */
typedef struct ogc_navigation_engine_t ogc_navigation_engine_t;

ogc_navigation_engine_t* ogc_navigation_engine_create(void);
void ogc_navigation_engine_destroy(ogc_navigation_engine_t* engine);
int ogc_navigation_engine_initialize(ogc_navigation_engine_t* engine);
void ogc_navigation_engine_shutdown(ogc_navigation_engine_t* engine);
void ogc_navigation_engine_set_route(ogc_navigation_engine_t* engine, ogc_route_t* route);
ogc_route_t* ogc_navigation_engine_get_route(const ogc_navigation_engine_t* engine);
void ogc_navigation_engine_start(ogc_navigation_engine_t* engine);
void ogc_navigation_engine_stop(ogc_navigation_engine_t* engine);
void ogc_navigation_engine_pause(ogc_navigation_engine_t* engine);
void ogc_navigation_engine_resume(ogc_navigation_engine_t* engine);
ogc_navigation_status_e ogc_navigation_engine_get_status(const ogc_navigation_engine_t* engine);
ogc_waypoint_t* ogc_navigation_engine_get_current_waypoint(const ogc_navigation_engine_t* engine);
double ogc_navigation_engine_get_distance_to_waypoint(const ogc_navigation_engine_t* engine);
double ogc_navigation_engine_get_bearing_to_waypoint(const ogc_navigation_engine_t* engine);
double ogc_navigation_engine_get_cross_track_error(const ogc_navigation_engine_t* engine);

/* 10.10 PositionManager */
/* Java: cn.cycle.chart.api.navi.PositionManager */
/* C++:   ogc::navi::PositionManager */
/* Header: ogc/navi/positioning/position_manager.h */
typedef struct ogc_position_manager_t ogc_position_manager_t;

ogc_position_manager_t* ogc_position_manager_create(void);
void ogc_position_manager_destroy(ogc_position_manager_t* mgr);
int ogc_position_manager_initialize(ogc_position_manager_t* mgr);
void ogc_position_manager_shutdown(ogc_position_manager_t* mgr);
double ogc_position_manager_get_latitude(const ogc_position_manager_t* mgr);
double ogc_position_manager_get_longitude(const ogc_position_manager_t* mgr);
double ogc_position_manager_get_speed(const ogc_position_manager_t* mgr);
double ogc_position_manager_get_course(const ogc_position_manager_t* mgr);
double ogc_position_manager_get_heading(const ogc_position_manager_t* mgr);
int ogc_position_manager_get_fix_quality(const ogc_position_manager_t* mgr);
int ogc_position_manager_get_satellite_count(const ogc_position_manager_t* mgr);

/* 10.11 Track */
/* Java: cn.cycle.chart.api.navi.Track */
/* C++:   ogc::navi::Track */
/* Header: ogc/navi/track/track.h */
typedef struct ogc_track_t ogc_track_t;
typedef struct ogc_track_point_t ogc_track_point_t;

ogc_track_t* ogc_track_create(void);
void ogc_track_destroy(ogc_track_t* track);
const char* ogc_track_get_id(const ogc_track_t* track);
const char* ogc_track_get_name(const ogc_track_t* track);
void ogc_track_set_name(ogc_track_t* track, const char* name);
size_t ogc_track_get_point_count(const ogc_track_t* track);
ogc_track_point_t* ogc_track_get_point(const ogc_track_t* track, size_t index);
void ogc_track_add_point(ogc_track_t* track, double lat, double lon, double speed, double course, double timestamp);
void ogc_track_clear(ogc_track_t* track);
ogc_geometry_t* ogc_track_to_linestring(const ogc_track_t* track);

/* 10.12 TrackRecorder */
/* Java: cn.cycle.chart.api.navi.TrackRecorder */
/* C++:   ogc::navi::TrackRecorder */
/* Header: ogc/navi/track/track_recorder.h */
typedef struct ogc_track_recorder_t ogc_track_recorder_t;

ogc_track_recorder_t* ogc_track_recorder_create(void);
void ogc_track_recorder_destroy(ogc_track_recorder_t* recorder);
void ogc_track_recorder_start(ogc_track_recorder_t* recorder, const char* track_name);
void ogc_track_recorder_stop(ogc_track_recorder_t* recorder);
void ogc_track_recorder_pause(ogc_track_recorder_t* recorder);
void ogc_track_recorder_resume(ogc_track_recorder_t* recorder);
int ogc_track_recorder_is_recording(const ogc_track_recorder_t* recorder);
ogc_track_t* ogc_track_recorder_get_current_track(const ogc_track_recorder_t* recorder);
void ogc_track_recorder_set_interval(ogc_track_recorder_t* recorder, int seconds);
void ogc_track_recorder_set_min_distance(ogc_track_recorder_t* recorder, double meters);

/* 10.13 IPositionProvider */
/* Java: cn.cycle.chart.api.navi.IPositionProvider */
/* C++:   ogc::navi::IPositionProvider */
/* Header: ogc/navi/positioning/position_provider.h */
typedef struct ogc_position_provider_t ogc_position_provider_t;

ogc_position_provider_t* ogc_position_provider_create_serial(const char* port, int baud_rate);
ogc_position_provider_t* ogc_position_provider_create_network(const char* host, int port);
void ogc_position_provider_destroy(ogc_position_provider_t* provider);
int ogc_position_provider_connect(ogc_position_provider_t* provider);
void ogc_position_provider_disconnect(ogc_position_provider_t* provider);
int ogc_position_provider_is_connected(const ogc_position_provider_t* provider);

/* 10.14 NmeaParser */
/* Java: cn.cycle.chart.api.navi.NmeaParser */
/* C++:   ogc::navi::NmeaParser */
/* Header: ogc/navi/positioning/nmea_parser.h */
typedef struct ogc_nmea_parser_t ogc_nmea_parser_t;

ogc_nmea_parser_t* ogc_nmea_parser_create(void);
void ogc_nmea_parser_destroy(ogc_nmea_parser_t* parser);
int ogc_nmea_parser_parse(ogc_nmea_parser_t* parser, const char* sentence);
const char* ogc_nmea_parser_get_sentence_type(const ogc_nmea_parser_t* parser);

/* 10.15 CollisionAssessor */
/* Java: cn.cycle.chart.api.navi.CollisionAssessor */
/* C++:   ogc::navi::CollisionAssessor */
/* Header: ogc/navi/ais/collision_assessor.h */
typedef struct ogc_collision_assessor_t ogc_collision_assessor_t;
typedef struct ogc_collision_risk_t {
    uint32_t target_mmsi;
    double cpa;
    double tcpa;
    int risk_level;
    char* risk_description;
} ogc_collision_risk_t;

ogc_collision_assessor_t* ogc_collision_assessor_create(void);
void ogc_collision_assessor_destroy(ogc_collision_assessor_t* assessor);
ogc_collision_risk_t* ogc_collision_assessor_assess(ogc_collision_assessor_t* assessor,
                                                      double own_lat, double own_lon, 
                                                      double own_speed, double own_course,
                                                      const ogc_ais_target_t* target);
void ogc_collision_risk_destroy(ogc_collision_risk_t* risk);

/* 10.16 OffCourseDetector */
/* Java: cn.cycle.chart.api.navi.OffCourseDetector */
/* C++:   ogc::navi::OffCourseDetector */
/* Header: ogc/navi/navigation/off_course_detector.h */
typedef struct ogc_off_course_detector_t ogc_off_course_detector_t;
typedef struct ogc_deviation_result_t {
    double cross_track_error;
    double distance_to_route;
    int is_off_course;
    int side;
} ogc_deviation_result_t;

ogc_off_course_detector_t* ogc_off_course_detector_create(void);
void ogc_off_course_detector_destroy(ogc_off_course_detector_t* detector);
void ogc_off_course_detector_set_threshold(ogc_off_course_detector_t* detector, double meters);
ogc_deviation_result_t ogc_off_course_detector_check(ogc_off_course_detector_t* detector,
                                                       double lat, double lon,
                                                       const ogc_waypoint_t* from,
                                                       const ogc_waypoint_t* to);

/* ============================================================================
 * 11. Projection Module (ogc_proj)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.proj
 * Description: Coordinate transformation
 */

/* 11.1 CoordinateTransformer */
/* Java: cn.cycle.chart.api.proj.CoordinateTransformer */
/* C++:   ogc::proj::CoordinateTransformer */
/* Header: ogc/proj/coordinate_transformer.h */
typedef struct ogc_coord_transformer_t ogc_coord_transformer_t;

ogc_coord_transformer_t* ogc_coord_transformer_create(const char* source_crs, const char* target_crs);
void ogc_coord_transformer_destroy(ogc_coord_transformer_t* transformer);
int ogc_coord_transformer_is_valid(const ogc_coord_transformer_t* transformer);
ogc_coordinate_t ogc_coord_transformer_transform(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord);
ogc_coordinate_t ogc_coord_transformer_transform_inverse(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord);
void ogc_coord_transformer_transform_array(const ogc_coord_transformer_t* transformer, double* x, double* y, size_t count);
ogc_envelope_t* ogc_coord_transformer_transform_envelope(const ogc_coord_transformer_t* transformer, const ogc_envelope_t* env);
ogc_geometry_t* ogc_coord_transformer_transform_geometry(const ogc_coord_transformer_t* transformer, const ogc_geometry_t* geom);
const char* ogc_coord_transformer_get_source_crs(const ogc_coord_transformer_t* transformer);
const char* ogc_coord_transformer_get_target_crs(const ogc_coord_transformer_t* transformer);

/* 11.2 TransformMatrix */
/* Java: cn.cycle.chart.api.proj.TransformMatrix */
/* C++:   ogc::proj::TransformMatrix */
/* Header: ogc/proj/transform_matrix.h */
typedef struct ogc_transform_matrix_t ogc_transform_matrix_t;

ogc_transform_matrix_t* ogc_transform_matrix_create(void);
ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void);
void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix);
void ogc_transform_matrix_translate(ogc_transform_matrix_t* matrix, double dx, double dy);
void ogc_transform_matrix_scale(ogc_transform_matrix_t* matrix, double sx, double sy);
void ogc_transform_matrix_rotate(ogc_transform_matrix_t* matrix, double angle_degrees);
void ogc_transform_matrix_multiply(ogc_transform_matrix_t* result, const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b);
ogc_coordinate_t ogc_transform_matrix_transform(const ogc_transform_matrix_t* matrix, const ogc_coordinate_t* coord);
void ogc_transform_matrix_get_values(const ogc_transform_matrix_t* matrix, double* values);

/* ============================================================================
 * 12. Chart Parser Module (chart_parser)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.parser
 * Description: Chart parsing (S57/S101)
 */

/* 12.1 ChartFormat */
/* Java: cn.cycle.chart.api.parser.ChartFormat */
/* C++:   chart::parser::ChartFormat */
/* Header: chart_parser/chart_format.h */
typedef enum ogc_chart_format_e {
    OGC_CHART_FORMAT_S57 = 0,
    OGC_CHART_FORMAT_S100 = 1,
    OGC_CHART_FORMAT_S101 = 2,
    OGC_CHART_FORMAT_S102 = 3,
    OGC_CHART_FORMAT_GML = 4,
    OGC_CHART_FORMAT_UNKNOWN = 5
} ogc_chart_format_e;

/* 12.2 ChartParser */
/* Java: cn.cycle.chart.api.parser.ChartParser */
/* C++:   chart::parser::ChartParser */
/* Header: chart_parser/chart_parser.h */
typedef struct ogc_chart_parser_t ogc_chart_parser_t;

ogc_chart_parser_t* ogc_chart_parser_get_instance(void);
int ogc_chart_parser_initialize(ogc_chart_parser_t* parser);
void ogc_chart_parser_shutdown(ogc_chart_parser_t* parser);
int ogc_chart_parser_get_supported_formats(const ogc_chart_parser_t* parser, ogc_chart_format_e* formats, int max_count);

/* 12.3 IParser */
/* Java: cn.cycle.chart.api.parser.IParser */
/* C++:   chart::parser::IParser */
/* Header: chart_parser/iparser.h */
typedef struct ogc_iparser_t ogc_iparser_t;
typedef struct ogc_parse_result_t ogc_parse_result_t;

ogc_iparser_t* ogc_iparser_create(ogc_chart_format_e format);
void ogc_iparser_destroy(ogc_iparser_t* parser);
int ogc_iparser_open(ogc_iparser_t* parser, const char* path);
void ogc_iparser_close(ogc_iparser_t* parser);
ogc_parse_result_t* ogc_iparser_parse(ogc_iparser_t* parser);
ogc_parse_result_t* ogc_iparser_parse_incremental(ogc_iparser_t* parser, int batch_size);
ogc_chart_format_e ogc_iparser_get_format(const ogc_iparser_t* parser);

/* 12.4 S57Parser */
/* Java: cn.cycle.chart.api.parser.S57Parser */
/* C++:   chart::parser::S57Parser */
/* Header: chart_parser/s57_parser.h */
typedef struct ogc_s57_parser_t ogc_s57_parser_t;

ogc_s57_parser_t* ogc_s57_parser_create(void);
void ogc_s57_parser_destroy(ogc_s57_parser_t* parser);
int ogc_s57_parser_open(ogc_s57_parser_t* parser, const char* path);
ogc_parse_result_t* ogc_s57_parser_parse(ogc_s57_parser_t* parser);
void ogc_s57_parser_set_feature_filter(ogc_s57_parser_t* parser, const char** features, int count);
void ogc_s57_parser_set_spatial_filter(ogc_s57_parser_t* parser, const ogc_envelope_t* bounds);

/* 12.5 IncrementalParser */
/* Java: cn.cycle.chart.api.parser.IncrementalParser */
/* C++:   chart::parser::IncrementalParser */
/* Header: chart_parser/incremental_parser.h */
typedef struct ogc_incremental_parser_t ogc_incremental_parser_t;

ogc_incremental_parser_t* ogc_incremental_parser_create(void);
void ogc_incremental_parser_destroy(ogc_incremental_parser_t* parser);
void ogc_incremental_parser_start(ogc_incremental_parser_t* parser, const char* path, int batch_size);
void ogc_incremental_parser_pause(ogc_incremental_parser_t* parser);
void ogc_incremental_parser_resume(ogc_incremental_parser_t* parser);
void ogc_incremental_parser_cancel(ogc_incremental_parser_t* parser);
ogc_parse_result_t* ogc_incremental_parser_parse_next(ogc_incremental_parser_t* parser);
int ogc_incremental_parser_has_more(const ogc_incremental_parser_t* parser);
float ogc_incremental_parser_get_progress(const ogc_incremental_parser_t* parser);

/* ============================================================================
 * 13. Plugin Module
 * ============================================================================
 * Java Package: cn.cycle.chart.plugin
 * Description: Plugin extension mechanism
 */

/* 13.1 ChartPlugin */
/* Java: cn.cycle.chart.plugin.ChartPlugin */
/* C++:   ogc::draw::DrawDevicePlugin */
/* Header: ogc/draw/draw_device_plugin.h */
typedef struct ogc_chart_plugin_t ogc_chart_plugin_t;

ogc_chart_plugin_t* ogc_chart_plugin_create(const char* name, const char* version);
void ogc_chart_plugin_destroy(ogc_chart_plugin_t* plugin);
const char* ogc_chart_plugin_get_name(const ogc_chart_plugin_t* plugin);
const char* ogc_chart_plugin_get_version(const ogc_chart_plugin_t* plugin);
int ogc_chart_plugin_initialize(ogc_chart_plugin_t* plugin);
void ogc_chart_plugin_shutdown(ogc_chart_plugin_t* plugin);

/* 13.2 PluginManager */
/* Java: cn.cycle.chart.plugin.PluginManager */
/* C++:   ogc::draw::PluginManager */
/* Header: ogc/draw/plugin_manager.h */
typedef struct ogc_plugin_manager_t ogc_plugin_manager_t;

ogc_plugin_manager_t* ogc_plugin_manager_get_instance(void);
int ogc_plugin_manager_load_plugin(ogc_plugin_manager_t* mgr, const char* path);
int ogc_plugin_manager_unload_plugin(ogc_plugin_manager_t* mgr, const char* name);
ogc_chart_plugin_t* ogc_plugin_manager_get_plugin(const ogc_plugin_manager_t* mgr, const char* name);
int ogc_plugin_manager_get_plugin_count(const ogc_plugin_manager_t* mgr);
const char* ogc_plugin_manager_get_plugin_name(const ogc_plugin_manager_t* mgr, int index);

/* ============================================================================
 * 14. Error Recovery Module (ogc_recovery)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.recovery
 * Description: Error recovery and graceful degradation
 */

/* 14.1 ErrorRecoveryManager */
/* Java: cn.cycle.chart.api.recovery.ErrorRecoveryManager */
/* C++:   ogc::recovery::ErrorRecoveryManager */
/* Header: ogc/recovery/error_recovery_manager.h */
typedef struct ogc_error_recovery_manager_t ogc_error_recovery_manager_t;

ogc_error_recovery_manager_t* ogc_error_recovery_manager_create(void);
void ogc_error_recovery_manager_destroy(ogc_error_recovery_manager_t* mgr);
int ogc_error_recovery_manager_initialize(ogc_error_recovery_manager_t* mgr);
void ogc_error_recovery_manager_shutdown(ogc_error_recovery_manager_t* mgr);
int ogc_error_recovery_manager_handle_error(ogc_error_recovery_manager_t* mgr, int error_code, const char* context);
void ogc_error_recovery_manager_register_strategy(ogc_error_recovery_manager_t* mgr, int error_code, void* strategy);
void ogc_error_recovery_manager_set_degradation_level(ogc_error_recovery_manager_t* mgr, int level);

/* 14.2 CircuitBreaker */
/* Java: cn.cycle.chart.api.recovery.CircuitBreaker */
/* C++:   ogc::recovery::CircuitBreaker */
/* Header: ogc/recovery/circuit_breaker.h */
typedef struct ogc_circuit_breaker_t ogc_circuit_breaker_t;

typedef enum ogc_circuit_state_e {
    OGC_CIRCUIT_CLOSED = 0,
    OGC_CIRCUIT_OPEN = 1,
    OGC_CIRCUIT_HALF_OPEN = 2
} ogc_circuit_state_e;

ogc_circuit_breaker_t* ogc_circuit_breaker_create(int failure_threshold, int recovery_timeout_ms);
void ogc_circuit_breaker_destroy(ogc_circuit_breaker_t* breaker);
ogc_circuit_state_e ogc_circuit_breaker_get_state(const ogc_circuit_breaker_t* breaker);
int ogc_circuit_breaker_allow_request(ogc_circuit_breaker_t* breaker);
void ogc_circuit_breaker_record_success(ogc_circuit_breaker_t* breaker);
void ogc_circuit_breaker_record_failure(ogc_circuit_breaker_t* breaker);
void ogc_circuit_breaker_reset(ogc_circuit_breaker_t* breaker);

/* 14.3 GracefulDegradation */
/* Java: cn.cycle.chart.api.recovery.GracefulDegradation */
/* C++:   ogc::recovery::GracefulDegradation */
/* Header: ogc/recovery/graceful_degradation.h */
typedef struct ogc_graceful_degradation_t ogc_graceful_degradation_t;

typedef enum ogc_degradation_level_e {
    OGC_DEGRADATION_NONE = 0,
    OGC_DEGRADATION_MINIMAL = 1,
    OGC_DEGRADATION_MODERATE = 2,
    OGC_DEGRADATION_SEVERE = 3,
    OGC_DEGRADATION_CRITICAL = 4
} ogc_degradation_level_e;

ogc_graceful_degradation_t* ogc_graceful_degradation_create(void);
void ogc_graceful_degradation_destroy(ogc_graceful_degradation_t* degradation);
ogc_degradation_level_e ogc_graceful_degradation_get_level(const ogc_graceful_degradation_t* degradation);
void ogc_graceful_degradation_set_level(ogc_graceful_degradation_t* degradation, ogc_degradation_level_e level);
int ogc_graceful_degradation_is_feature_enabled(const ogc_graceful_degradation_t* degradation, const char* feature);
void ogc_graceful_degradation_disable_feature(ogc_graceful_degradation_t* degradation, const char* feature);
void ogc_graceful_degradation_enable_feature(ogc_graceful_degradation_t* degradation, const char* feature);

/* ============================================================================
 * 15. Health Check Module (ogc_health)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.health
 * Description: Health check and monitoring
 */

/* 15.1 HealthStatus */
/* Java: cn.cycle.chart.api.health.HealthStatus */
/* C++:   ogc::health::HealthStatus */
/* Header: ogc/health/health_status.h */
typedef enum ogc_health_status_e {
    OGC_HEALTH_UNKNOWN = 0,
    OGC_HEALTH_HEALTHY = 1,
    OGC_HEALTH_DEGRADED = 2,
    OGC_HEALTH_UNHEALTHY = 3
} ogc_health_status_e;

/* 15.2 HealthCheck */
/* Java: cn.cycle.chart.api.health.HealthCheck */
/* C++:   ogc::health::HealthCheck */
/* Header: ogc/health/health_check.h */
typedef struct ogc_health_check_t ogc_health_check_t;
typedef struct ogc_health_check_result_t {
    ogc_health_status_e status;
    char* component_name;
    char* message;
    double check_duration_ms;
} ogc_health_check_result_t;

ogc_health_check_t* ogc_health_check_create(const char* name);
void ogc_health_check_destroy(ogc_health_check_t* check);
ogc_health_check_result_t* ogc_health_check_execute(ogc_health_check_t* check);
void ogc_health_check_result_destroy(ogc_health_check_result_t* result);

/* ============================================================================
 * 16. Library Loader Module (ogc_loader)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.loader
 * Description: Native library loading
 */

/* 16.1 LibraryLoader */
/* Java: cn.cycle.chart.api.loader.LibraryLoader */
/* C++:   ogc::loader::LibraryLoader */
/* Header: ogc/loader/library_loader.h */
typedef struct ogc_library_loader_t ogc_library_loader_t;
typedef void* ogc_library_handle_t;

ogc_library_loader_t* ogc_library_loader_create(void);
void ogc_library_loader_destroy(ogc_library_loader_t* loader);
ogc_library_handle_t ogc_library_loader_load(ogc_library_loader_t* loader, const char* library_path);
void ogc_library_loader_unload(ogc_library_loader_t* loader, ogc_library_handle_t handle);
void* ogc_library_loader_get_symbol(ogc_library_loader_t* loader, ogc_library_handle_t handle, const char* symbol_name);
const char* ogc_library_loader_get_error(ogc_library_loader_t* loader);

/* 16.2 SecureLibraryLoader */
/* Java: cn.cycle.chart.api.loader.SecureLibraryLoader */
/* C++:   ogc::loader::SecureLibraryLoader */
/* Header: ogc/loader/secure_library_loader.h */
typedef struct ogc_secure_library_loader_t ogc_secure_library_loader_t;

ogc_secure_library_loader_t* ogc_secure_library_loader_create(const char* allowed_paths);
void ogc_secure_library_loader_destroy(ogc_secure_library_loader_t* loader);
ogc_library_handle_t ogc_secure_library_loader_load(ogc_secure_library_loader_t* loader, const char* library_path);
int ogc_secure_library_loader_verify_signature(ogc_secure_library_loader_t* loader, ogc_library_handle_t handle, const char* public_key);

/* ============================================================================
 * 17. Exception Module (ogc_exception)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.exception
 * Description: Exception types for error handling
 */

/* 17.1 ChartException */
/* Java: cn.cycle.chart.api.exception.ChartException */
/* C++:   ogc::exception::ChartException */
/* Header: ogc/exception/chart_exception.h */
typedef struct ogc_chart_exception_t {
    int error_code;
    char* message;
    char* context;
} ogc_chart_exception_t;

ogc_chart_exception_t* ogc_chart_exception_create(int error_code, const char* message);
void ogc_chart_exception_destroy(ogc_chart_exception_t* ex);
int ogc_chart_exception_get_error_code(const ogc_chart_exception_t* ex);
const char* ogc_chart_exception_get_message(const ogc_chart_exception_t* ex);
const char* ogc_chart_exception_get_context(const ogc_chart_exception_t* ex);

/* 17.2 JniException */
/* Java: cn.cycle.chart.api.exception.JniException */
/* C++:   ogc::exception::JniException */
/* Header: ogc/exception/jni_exception.h */
typedef struct ogc_jni_exception_t {
    int error_code;
    char* message;
    char* java_exception_class;
} ogc_jni_exception_t;

ogc_jni_exception_t* ogc_jni_exception_create(const char* message, const char* java_class);
void ogc_jni_exception_destroy(ogc_jni_exception_t* ex);

/* 17.3 RenderException */
/* Java: cn.cycle.chart.api.exception.RenderException */
/* C++:   ogc::exception::RenderException */
/* Header: ogc/exception/render_exception.h */
typedef struct ogc_render_exception_t {
    int error_code;
    char* message;
    char* layer_name;
    char* operation;
} ogc_render_exception_t;

ogc_render_exception_t* ogc_render_exception_create(const char* message, const char* layer_name);
void ogc_render_exception_destroy(ogc_render_exception_t* ex);

/* ============================================================================
 * 18. Callback Types
 * ============================================================================
 */

/* Progress callback */
typedef void (*ogc_progress_callback_t)(float progress, void* user_data);

/* Alert callback */
typedef void (*ogc_alert_callback_t)(const ogc_alert_t* alert, void* user_data);

/* Position update callback */
typedef void (*ogc_position_callback_t)(double lat, double lon, double speed, double course, void* user_data);

/* AIS target callback */
typedef void (*ogc_ais_target_callback_t)(const ogc_ais_target_t* target, void* user_data);

/* Render complete callback */
typedef void (*ogc_render_callback_t)(int success, void* user_data);

/* Load complete callback */
typedef void (*ogc_load_callback_t)(int success, const char* chart_id, void* user_data);

/* Touch callback */
typedef void (*ogc_touch_callback_t)(int touch_type, double x, double y, void* user_data);

/* Recovery strategy callback */
typedef int (*ogc_recovery_strategy_t)(int error_code, const char* context, void* user_data);

/* ============================================================================
 * 19. Enum Types
 * ============================================================================
 */

/* DisplayMode */
/* Java: cn.cycle.chart.api.core.DisplayMode */
typedef enum ogc_display_mode_e {
    OGC_DISPLAY_MODE_DAY = 0,
    OGC_DISPLAY_MODE_DUSK = 1,
    OGC_DISPLAY_MODE_NIGHT = 2
} ogc_display_mode_e;

/* AppState */
/* Java: cn.cycle.chart.api.core.AppState */
typedef enum ogc_app_state_e {
    OGC_APP_STATE_UNINITIALIZED = 0,
    OGC_APP_STATE_INITIALIZING = 1,
    OGC_APP_STATE_READY = 2,
    OGC_APP_STATE_RUNNING = 3,
    OGC_APP_STATE_PAUSED = 4,
    OGC_APP_STATE_ERROR = 5,
    OGC_APP_STATE_SHUTTING_DOWN = 6
} ogc_app_state_e;

/* ============================================================================
 * 20. Utility Classes
 * ============================================================================
 */

/* 20.1 TransformMatrix */
/* Java: cn.cycle.chart.api.proj.TransformMatrix */
/* C++:   ogc::proj::TransformMatrix */
/* Header: ogc/proj/transform_matrix.h */
typedef struct ogc_transform_matrix_t ogc_transform_matrix_t;

ogc_transform_matrix_t* ogc_transform_matrix_create(void);
ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void);
ogc_transform_matrix_t* ogc_transform_matrix_create_translation(double tx, double ty);
ogc_transform_matrix_t* ogc_transform_matrix_create_scale(double sx, double sy);
ogc_transform_matrix_t* ogc_transform_matrix_create_rotation(double angle);
void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix);
ogc_transform_matrix_t* ogc_transform_matrix_multiply(const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b);
ogc_transform_matrix_t* ogc_transform_matrix_inverse(const ogc_transform_matrix_t* matrix);
void ogc_transform_matrix_transform_point(const ogc_transform_matrix_t* matrix, double* x, double* y);
void ogc_transform_matrix_get_elements(const ogc_transform_matrix_t* matrix, double* elements);
void ogc_transform_matrix_set_elements(ogc_transform_matrix_t* matrix, const double* elements);

/* ============================================================================
 * 22. Version Information
 * ============================================================================
 */

#define OGC_SDK_VERSION_MAJOR 1
#define OGC_SDK_VERSION_MINOR 1
#define OGC_SDK_VERSION_PATCH 0

const char* ogc_sdk_get_version(void);
const char* ogc_sdk_get_build_date(void);
int ogc_sdk_get_version_major(void);
int ogc_sdk_get_version_minor(void);
int ogc_sdk_get_version_patch(void);

/* ============================================================================
 * 21. Module Summary Table
 * ============================================================================
 * 
 * | Module         | Java Classes | C++ Headers                                    | API Count |
 * |----------------|--------------|------------------------------------------------|-----------|
 * | ogc_base       | 3            | log.h, performance_monitor.h                   | 15        |
 * | ogc_geom       | 12           | geometry.h, point.h, polygon.h, etc.           | 80+       |
 * | ogc_feature    | 5            | feature.h, field_defn.h, field_value.h         | 40+       |
 * | ogc_layer      | 8            | layer.h, vector_layer.h, datasource.h          | 50+       |
 * | ogc_draw       | 9            | draw_engine.h, draw_device.h, color.h          | 45+       |
 * | ogc_graph      | 13           | draw_facade.h, layer_manager.h, etc.           | 60+       |
 * | ogc_cache      | 7            | tile_cache.h, tile_key.h, etc.                 | 30+       |
 * | ogc_symbology  | 6            | filter.h, symbolizer.h, comparison_filter.h    | 30+       |
 * | ogc_alert      | 6            | alert_engine.h, alert.h, types.h               | 20+       |
 * | ogc_navi       | 16           | route.h, ais_manager.h, navigation.h           | 70+       |
 * | ogc_proj       | 3            | coordinate_transformer.h, transform_matrix.h   | 20+       |
 * | chart_parser   | 5            | chart_parser.h, s57_parser.h, etc.             | 25+       |
 * | ogc_recovery   | 3            | error_recovery_manager.h, circuit_breaker.h    | 15+       |
 * | ogc_health     | 2            | health_check.h, health_status.h                | 8+        |
 * | ogc_loader     | 2            | library_loader.h, secure_library_loader.h      | 10+       |
 * | ogc_exception  | 3            | chart_exception.h, jni_exception.h, etc.       | 10+       |
 * | plugin         | 2            | plugin_manager.h, draw_device_plugin.h         | 10+       |
 * |----------------|--------------|------------------------------------------------|-----------|
 * | **Total**      | **105**      | **60+ header files**                           | **550+**  |
 */

#ifdef __cplusplus
}
#endif

#endif /* OGC_CHART_SDK_C_API_H */