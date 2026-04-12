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

#include <stddef.h>   /* for size_t */
#include <stdint.h>   /* for int64_t, uint32_t */
#include "sdk_c_api_export.h"

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
 * | ogc_geom    | Geometry objects (OGC SFAS)    | ogc/geom/               |
 * | ogc_feature | Feature model                  | ogc/feature/            |
 * | ogc_layer   | Layer management               | ogc/layer/              |
 * | ogc_draw    | Drawing engine                 | ogc/draw/               |
 * | ogc_graph   | Map rendering core             | ogc/graph/              |
 * | ogc_cache   | Tile cache & offline storage   | ogc/cache/              |
 * | ogc_symbology| Symbolization & styles        | ogc/symbology/          |
 * | ogc_alert   | Maritime alerts                | ogc/alert/              |
 * | ogc_navi    | Navigation system              | ogc/navi/               |
 * | chart_parser| Chart parsing (S57/S101)       | parser/           |
 */

/* ============================================================================
 * Error Codes (Standardized)
 * ============================================================================
 * Standardized error codes for consistent error handling across all API functions.
 */
typedef enum ogc_error_code_e {
    OGC_SUCCESS = 0,
    OGC_ERROR_INVALID_PARAM = -1,
    OGC_ERROR_NULL_POINTER = -2,
    OGC_ERROR_OUT_OF_MEMORY = -3,
    OGC_ERROR_NOT_FOUND = -4,
    OGC_ERROR_OPERATION_FAILED = -5,
    OGC_ERROR_INVALID_STATE = -6,
    OGC_ERROR_IO_ERROR = -7,
    OGC_ERROR_PARSE_ERROR = -8,
    OGC_ERROR_UNSUPPORTED = -9,
    OGC_ERROR_TIMEOUT = -10,
    OGC_ERROR_PERMISSION_DENIED = -11
} ogc_error_code_e;

SDK_C_API const char* ogc_error_get_message(ogc_error_code_e code);

/* ============================================================================
 * Thread Safety Notes
 * ============================================================================
 * 
 * Thread Safety Classification:
 * 
 * | Class/Component        | Thread Safety | Notes                                    |
 * |------------------------|---------------|------------------------------------------|
 * | ChartViewer            | NOT SAFE      | Must be operated from a single thread    |
 * | Viewport               | NOT SAFE      | Requires external synchronization        |
 * | LayerManager           | READ SAFE     | Read operations are thread safe          |
 * | TileCache              | THREAD SAFE   | Internally synchronized                  |
 * | MemoryTileCache        | THREAD SAFE   | Internally synchronized                  |
 * | DiskTileCache          | THREAD SAFE   | Internally synchronized                  |
 * | AlertEngine            | THREAD SAFE   | Uses internal mutex                      |
 * | AisManager             | THREAD SAFE   | Uses internal mutex                      |
 * | NavigationEngine       | NOT SAFE      | Must be operated from a single thread    |
 * | PositionManager        | THREAD SAFE   | Uses internal mutex                      |
 * | RouteManager           | NOT SAFE      | Requires external synchronization        |
 * | Logger                 | THREAD SAFE   | Internally synchronized                  |
 * | PerformanceMonitor     | THREAD SAFE   | Uses atomic operations                   |
 * | GeometryFactory        | NOT SAFE      | Create one per thread                    |
 * | DataSource             | NOT SAFE      | Requires external synchronization        |
 * | VectorLayer            | NOT SAFE      | Requires external synchronization        |
 * | RasterLayer            | READ SAFE     | Read operations are thread safe          |
 * | RenderQueue            | THREAD SAFE   | Internally synchronized                  |
 * | OfflineStorage         | THREAD SAFE   | Internally synchronized                  |
 * 
 * General Guidelines:
 * - Unless explicitly marked as THREAD SAFE, assume components are NOT thread safe
 * - For NOT SAFE components, use external mutex/lock for multi-threaded access
 * - READ SAFE means concurrent read operations are safe, but writes need synchronization
 * - Always check documentation for specific methods before using in multi-threaded context
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

/**
 * @brief Get the singleton Logger instance.
 * @return Pointer to the Logger instance.
 */
SDK_C_API ogc_logger_t* ogc_logger_get_instance(void);

/**
 * @brief Set the logging level.
 * @param logger Pointer to the Logger instance.
 * @param level Log level (0=TRACE, 1=DEBUG, 2=INFO, 3=WARNING, 4=ERROR, 5=FATAL).
 */
SDK_C_API void ogc_logger_set_level(ogc_logger_t* logger, int level);

/**
 * @brief Log a trace message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_trace(ogc_logger_t* logger, const char* message);

/**
 * @brief Log a debug message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_debug(ogc_logger_t* logger, const char* message);

/**
 * @brief Log an info message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_info(ogc_logger_t* logger, const char* message);

/**
 * @brief Log a warning message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_warning(ogc_logger_t* logger, const char* message);

/**
 * @brief Log an error message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_error(ogc_logger_t* logger, const char* message);

/**
 * @brief Log a fatal message.
 * @param logger Pointer to the Logger instance.
 * @param message The message to log.
 */
SDK_C_API void ogc_logger_fatal(ogc_logger_t* logger, const char* message);

/**
 * @brief Set the log file path.
 * @param logger Pointer to the Logger instance.
 * @param filepath Path to the log file.
 */
SDK_C_API void ogc_logger_set_log_file(ogc_logger_t* logger, const char* filepath);

/**
 * @brief Enable or disable console output.
 * @param logger Pointer to the Logger instance.
 * @param enable Non-zero to enable, zero to disable.
 */
SDK_C_API void ogc_logger_set_console_output(ogc_logger_t* logger, int enable);

/* 1.2 PerformanceMonitor */
/* Java: cn.cycle.chart.api.util.PerformanceMonitor */
/* C++:   ogc::base::PerformanceMonitor */
/* Header: ogc/base/performance_monitor.h */
typedef struct ogc_performance_monitor_t ogc_performance_monitor_t;

/**
 * @brief Get the singleton PerformanceMonitor instance.
 * @return Pointer to the PerformanceMonitor instance.
 */
SDK_C_API ogc_performance_monitor_t* ogc_performance_monitor_get_instance(void);

/**
 * @brief Mark the start of a frame for FPS calculation.
 * @param monitor Pointer to the PerformanceMonitor instance.
 */
SDK_C_API void ogc_performance_monitor_start_frame(ogc_performance_monitor_t* monitor);

/**
 * @brief Mark the end of a frame for FPS calculation.
 * @param monitor Pointer to the PerformanceMonitor instance.
 */
SDK_C_API void ogc_performance_monitor_end_frame(ogc_performance_monitor_t* monitor);

/**
 * @brief Get the current frames per second.
 * @param monitor Pointer to the PerformanceMonitor instance.
 * @return Current FPS value.
 */
SDK_C_API double ogc_performance_monitor_get_fps(ogc_performance_monitor_t* monitor);

/**
 * @brief Get the memory used by the application.
 * @param monitor Pointer to the PerformanceMonitor instance.
 * @return Memory used in bytes.
 */
SDK_C_API long ogc_performance_monitor_get_memory_used(ogc_performance_monitor_t* monitor);

/**
 * @brief Reset all performance counters.
 * @param monitor Pointer to the PerformanceMonitor instance.
 */
SDK_C_API void ogc_performance_monitor_reset(ogc_performance_monitor_t* monitor);

/* 1.3 PerformanceStats */
/* Java: cn.cycle.chart.api.util.PerformanceStats */
/* C++:   ogc::base::FrameMetrics, MemoryMetrics */
/* Header: ogc/base/performance_metrics.h */

/**
 * @brief Performance statistics structure.
 */
typedef struct ogc_performance_stats_t {
    double fps;              /**< Frames per second */
    double frame_time_ms;    /**< Frame time in milliseconds */
    long memory_used;        /**< Memory used in bytes */
    long memory_total;       /**< Total memory in bytes */
    int render_count;        /**< Number of render operations */
    int cache_hit_count;     /**< Number of cache hits */
    int cache_miss_count;    /**< Number of cache misses */
} ogc_performance_stats_t;

/**
 * @brief Get current performance statistics.
 * @param stats Pointer to the structure to fill with statistics.
 */
SDK_C_API void ogc_performance_stats_get(ogc_performance_stats_t* stats);

/* ============================================================================
 * 2. Geometry Module (ogc_geom)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.geometry
 * Description: OGC Simple Feature Access geometry implementation
 */

/* 2.1 Coordinate */
/* Java: cn.cycle.chart.api.geometry.Coordinate */
/* C++:   ogc::geom::Coordinate */
/* Header: ogc/geom/coordinate.h */

/**
 * @brief Coordinate structure representing a 2D/3D point with optional measure.
 */
typedef struct ogc_coordinate_t {
    double x;  /**< X coordinate (longitude) */
    double y;  /**< Y coordinate (latitude) */
    double z;  /**< Z coordinate (elevation), 0 if not 3D */
    double m;  /**< Measure value, 0 if not measured */
} ogc_coordinate_t;

/**
 * @brief Create a 2D coordinate.
 * @param x X coordinate value.
 * @param y Y coordinate value.
 * @return Pointer to newly created coordinate, or NULL on failure.
 */
SDK_C_API ogc_coordinate_t* ogc_coordinate_create(double x, double y);

/**
 * @brief Create a 3D coordinate.
 * @param x X coordinate value.
 * @param y Y coordinate value.
 * @param z Z coordinate value (elevation).
 * @return Pointer to newly created coordinate, or NULL on failure.
 */
SDK_C_API ogc_coordinate_t* ogc_coordinate_create_3d(double x, double y, double z);

/**
 * @brief Create a coordinate with measure value.
 * @param x X coordinate value.
 * @param y Y coordinate value.
 * @param z Z coordinate value (elevation).
 * @param m Measure value.
 * @return Pointer to newly created coordinate, or NULL on failure.
 */
SDK_C_API ogc_coordinate_t* ogc_coordinate_create_m(double x, double y, double z, double m);

/**
 * @brief Destroy a coordinate and free its memory.
 * @param coord Pointer to the coordinate to destroy.
 */
SDK_C_API void ogc_coordinate_destroy(ogc_coordinate_t* coord);

/**
 * @brief Calculate the Euclidean distance between two coordinates.
 * @param a First coordinate.
 * @param b Second coordinate.
 * @return Distance between the coordinates.
 */
SDK_C_API double ogc_coordinate_distance(const ogc_coordinate_t* a, const ogc_coordinate_t* b);

/* 2.2 Envelope */
/* Java: cn.cycle.chart.api.geometry.Envelope */
/* C++:   ogc::geom::Envelope */
/* Header: ogc/geom/envelope.h */

/**
 * @brief Opaque type representing a bounding box (envelope).
 */
typedef struct ogc_envelope_t ogc_envelope_t;

/**
 * @brief Create an empty envelope.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_envelope_create(void);

/**
 * @brief Create an envelope from coordinate bounds.
 * @param min_x Minimum X coordinate.
 * @param min_y Minimum Y coordinate.
 * @param max_x Maximum X coordinate.
 * @param max_y Maximum Y coordinate.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_envelope_create_from_coords(double min_x, double min_y, double max_x, double max_y);

/**
 * @brief Destroy an envelope and free its memory.
 * @param env Pointer to the envelope to destroy.
 */
SDK_C_API void ogc_envelope_destroy(ogc_envelope_t* env);

/**
 * @brief Get the minimum X coordinate.
 * @param env Pointer to the envelope.
 * @return Minimum X coordinate.
 */
SDK_C_API double ogc_envelope_get_min_x(const ogc_envelope_t* env);

/**
 * @brief Get the minimum Y coordinate.
 * @param env Pointer to the envelope.
 * @return Minimum Y coordinate.
 */
SDK_C_API double ogc_envelope_get_min_y(const ogc_envelope_t* env);

/**
 * @brief Get the maximum X coordinate.
 * @param env Pointer to the envelope.
 * @return Maximum X coordinate.
 */
SDK_C_API double ogc_envelope_get_max_x(const ogc_envelope_t* env);

/**
 * @brief Get the maximum Y coordinate.
 * @param env Pointer to the envelope.
 * @return Maximum Y coordinate.
 */
SDK_C_API double ogc_envelope_get_max_y(const ogc_envelope_t* env);

/**
 * @brief Get the width of the envelope.
 * @param env Pointer to the envelope.
 * @return Width (max_x - min_x).
 */
SDK_C_API double ogc_envelope_get_width(const ogc_envelope_t* env);

/**
 * @brief Get the height of the envelope.
 * @param env Pointer to the envelope.
 * @return Height (max_y - min_y).
 */
SDK_C_API double ogc_envelope_get_height(const ogc_envelope_t* env);

/**
 * @brief Get the area of the envelope.
 * @param env Pointer to the envelope.
 * @return Area (width * height).
 */
SDK_C_API double ogc_envelope_get_area(const ogc_envelope_t* env);

/**
 * @brief Check if a point is inside the envelope.
 * @param env Pointer to the envelope.
 * @param x X coordinate of the point.
 * @param y Y coordinate of the point.
 * @return Non-zero if the point is inside, zero otherwise.
 */
SDK_C_API int ogc_envelope_contains(const ogc_envelope_t* env, double x, double y);

/**
 * @brief Check if two envelopes intersect.
 * @param env Pointer to the first envelope.
 * @param other Pointer to the second envelope.
 * @return Non-zero if they intersect, zero otherwise.
 */
SDK_C_API int ogc_envelope_intersects(const ogc_envelope_t* env, const ogc_envelope_t* other);

/**
 * @brief Expand the envelope by the given amounts.
 * @param env Pointer to the envelope.
 * @param dx Amount to expand in X direction.
 * @param dy Amount to expand in Y direction.
 */
SDK_C_API void ogc_envelope_expand(ogc_envelope_t* env, double dx, double dy);

/**
 * @brief Get the center point of the envelope.
 * @param env Pointer to the envelope.
 * @return Coordinate at the center of the envelope.
 */
SDK_C_API ogc_coordinate_t ogc_envelope_get_center(const ogc_envelope_t* env);

/* 2.3 Geometry (Base Class) */
/* Java: cn.cycle.chart.api.geometry.Geometry */
/* C++:   ogc::geom::Geometry */
/* Header: ogc/geom/geometry.h */

/**
 * @brief Opaque type representing a geometry object.
 */
typedef struct ogc_geometry_t ogc_geometry_t;

/**
 * @brief Geometry type enumeration.
 */
typedef enum ogc_geom_type_e {
    OGC_GEOM_TYPE_UNKNOWN = 0,           /**< Unknown geometry type */
    OGC_GEOM_TYPE_POINT = 1,             /**< Point geometry */
    OGC_GEOM_TYPE_LINESTRING = 2,        /**< LineString geometry */
    OGC_GEOM_TYPE_POLYGON = 3,           /**< Polygon geometry */
    OGC_GEOM_TYPE_MULTIPOINT = 4,        /**< MultiPoint geometry */
    OGC_GEOM_TYPE_MULTILINESTRING = 5,   /**< MultiLineString geometry */
    OGC_GEOM_TYPE_MULTIPOLYGON = 6,      /**< MultiPolygon geometry */
    OGC_GEOM_TYPE_GEOMETRYCOLLECTION = 7 /**< GeometryCollection */
} ogc_geom_type_e;

/**
 * @brief Destroy a geometry and free its memory.
 * @param geom Pointer to the geometry to destroy.
 */
SDK_C_API void ogc_geometry_destroy(ogc_geometry_t* geom);

/**
 * @brief Get the geometry type.
 * @param geom Pointer to the geometry.
 * @return Geometry type enumeration value.
 */
SDK_C_API ogc_geom_type_e ogc_geometry_get_type(const ogc_geometry_t* geom);

/**
 * @brief Get the geometry type name as a string.
 * @param geom Pointer to the geometry.
 * @return Type name string (do not free).
 */
SDK_C_API const char* ogc_geometry_get_type_name(const ogc_geometry_t* geom);

/**
 * @brief Get the dimension of the geometry.
 * @param geom Pointer to the geometry.
 * @return Dimension (0=Point, 1=LineString, 2=Polygon).
 */
SDK_C_API int ogc_geometry_get_dimension(const ogc_geometry_t* geom);

/**
 * @brief Check if the geometry is empty.
 * @param geom Pointer to the geometry.
 * @return Non-zero if empty, zero otherwise.
 */
SDK_C_API int ogc_geometry_is_empty(const ogc_geometry_t* geom);

/**
 * @brief Check if the geometry is valid.
 * @param geom Pointer to the geometry.
 * @return Non-zero if valid, zero otherwise.
 */
SDK_C_API int ogc_geometry_is_valid(const ogc_geometry_t* geom);

/**
 * @brief Check if the geometry is simple (no self-intersections).
 * @param geom Pointer to the geometry.
 * @return Non-zero if simple, zero otherwise.
 */
SDK_C_API int ogc_geometry_is_simple(const ogc_geometry_t* geom);

/**
 * @brief Check if the geometry has Z coordinates.
 * @param geom Pointer to the geometry.
 * @return Non-zero if 3D, zero otherwise.
 */
SDK_C_API int ogc_geometry_is_3d(const ogc_geometry_t* geom);

/**
 * @brief Check if the geometry has measure values.
 * @param geom Pointer to the geometry.
 * @return Non-zero if measured, zero otherwise.
 */
SDK_C_API int ogc_geometry_is_measured(const ogc_geometry_t* geom);

/**
 * @brief Get the Spatial Reference System Identifier.
 * @param geom Pointer to the geometry.
 * @return SRID value.
 */
SDK_C_API int ogc_geometry_get_srid(const ogc_geometry_t* geom);

/**
 * @brief Set the Spatial Reference System Identifier.
 * @param geom Pointer to the geometry.
 * @param srid SRID value to set.
 */
SDK_C_API void ogc_geometry_set_srid(ogc_geometry_t* geom, int srid);

/**
 * @brief Get the area of the geometry.
 * @param geom Pointer to the geometry.
 * @return Area value (0 for points and lines).
 */
SDK_C_API double ogc_geometry_get_area(const ogc_geometry_t* geom);

/**
 * @brief Get the length of the geometry.
 * @param geom Pointer to the geometry.
 * @return Length value (perimeter for polygons).
 */
SDK_C_API double ogc_geometry_get_length(const ogc_geometry_t* geom);

/**
 * @brief Get the number of points in the geometry.
 * @param geom Pointer to the geometry.
 * @return Number of points.
 */
SDK_C_API size_t ogc_geometry_get_num_points(const ogc_geometry_t* geom);

/**
 * @brief Get the bounding envelope of the geometry.
 * @param geom Pointer to the geometry.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_geometry_get_envelope(const ogc_geometry_t* geom);

/**
 * @brief Get the centroid of the geometry.
 * @param geom Pointer to the geometry.
 * @return Coordinate at the centroid.
 */
SDK_C_API ogc_coordinate_t ogc_geometry_get_centroid(const ogc_geometry_t* geom);

/**
 * @brief Convert geometry to WKT (Well-Known Text) format.
 * @param geom Pointer to the geometry.
 * @param precision Number of decimal places.
 * @return WKT string (caller must free with ogc_geometry_free_string).
 */
SDK_C_API char* ogc_geometry_as_text(const ogc_geometry_t* geom, int precision);

/**
 * @brief Convert geometry to GeoJSON format.
 * @param geom Pointer to the geometry.
 * @param precision Number of decimal places.
 * @return GeoJSON string (caller must free with ogc_geometry_free_string).
 */
SDK_C_API char* ogc_geometry_as_geojson(const ogc_geometry_t* geom, int precision);

/**
 * @brief Free a string returned by geometry functions.
 * @param str String to free.
 */
SDK_C_API void ogc_geometry_free_string(char* str);

/* Geometry spatial operations */

/**
 * @brief Check if two geometries are equal within tolerance.
 * @param a First geometry.
 * @param b Second geometry.
 * @param tolerance Distance tolerance.
 * @return Non-zero if equal, zero otherwise.
 */
SDK_C_API int ogc_geometry_equals(const ogc_geometry_t* a, const ogc_geometry_t* b, double tolerance);

/**
 * @brief Check if two geometries intersect.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if they intersect, zero otherwise.
 */
SDK_C_API int ogc_geometry_intersects(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Check if geometry a contains geometry b.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if a contains b, zero otherwise.
 */
SDK_C_API int ogc_geometry_contains(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Check if geometry a is within geometry b.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if a is within b, zero otherwise.
 */
SDK_C_API int ogc_geometry_within(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Check if geometry a crosses geometry b.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if a crosses b, zero otherwise.
 */
SDK_C_API int ogc_geometry_crosses(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Check if geometry a touches geometry b.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if a touches b, zero otherwise.
 */
SDK_C_API int ogc_geometry_touches(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Check if geometry a overlaps geometry b.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Non-zero if a overlaps b, zero otherwise.
 */
SDK_C_API int ogc_geometry_overlaps(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Calculate the distance between two geometries.
 * @param a First geometry.
 * @param b Second geometry.
 * @return Distance between geometries.
 */
SDK_C_API double ogc_geometry_distance(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Calculate the intersection of two geometries.
 * @param a First geometry.
 * @param b Second geometry.
 * @return New geometry representing the intersection, or NULL.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_intersection(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Calculate the union of two geometries.
 * @param a First geometry.
 * @param b Second geometry.
 * @return New geometry representing the union, or NULL.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_union(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Calculate the difference of two geometries (a - b).
 * @param a First geometry.
 * @param b Second geometry.
 * @return New geometry representing the difference, or NULL.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_difference(const ogc_geometry_t* a, const ogc_geometry_t* b);

/**
 * @brief Create a buffer around the geometry.
 * @param geom Pointer to the geometry.
 * @param distance Buffer distance.
 * @param segments Number of segments per quadrant.
 * @return New geometry representing the buffer, or NULL.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_buffer(const ogc_geometry_t* geom, double distance, int segments);

/**
 * @brief Create a deep copy of the geometry.
 * @param geom Pointer to the geometry.
 * @return New geometry that is a copy of the original, or NULL.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_clone(const ogc_geometry_t* geom);

/* 2.4 Point */
/* Java: cn.cycle.chart.api.geometry.Point */
/* C++:   ogc::geom::Point */
/* Header: ogc/geom/point.h */

/**
 * @brief Create a 2D point.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to newly created point geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_point_create(double x, double y);

/**
 * @brief Create a 3D point.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param z Z coordinate (elevation).
 * @return Pointer to newly created point geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_point_create_3d(double x, double y, double z);

/**
 * @brief Create a point from a coordinate.
 * @param coord Pointer to the coordinate.
 * @return Pointer to newly created point geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_point_create_from_coord(const ogc_coordinate_t* coord);

/**
 * @brief Get the X coordinate of a point.
 * @param point Pointer to the point geometry.
 * @return X coordinate value.
 */
SDK_C_API double ogc_point_get_x(const ogc_geometry_t* point);

/**
 * @brief Get the Y coordinate of a point.
 * @param point Pointer to the point geometry.
 * @return Y coordinate value.
 */
SDK_C_API double ogc_point_get_y(const ogc_geometry_t* point);

/**
 * @brief Get the Z coordinate of a point.
 * @param point Pointer to the point geometry.
 * @return Z coordinate value (0 if not 3D).
 */
SDK_C_API double ogc_point_get_z(const ogc_geometry_t* point);

/**
 * @brief Get the M (measure) value of a point.
 * @param point Pointer to the point geometry.
 * @return M value (0 if not measured).
 */
SDK_C_API double ogc_point_get_m(const ogc_geometry_t* point);

/**
 * @brief Set the X coordinate of a point.
 * @param point Pointer to the point geometry.
 * @param x X coordinate value.
 */
SDK_C_API void ogc_point_set_x(ogc_geometry_t* point, double x);

/**
 * @brief Set the Y coordinate of a point.
 * @param point Pointer to the point geometry.
 * @param y Y coordinate value.
 */
SDK_C_API void ogc_point_set_y(ogc_geometry_t* point, double y);

/**
 * @brief Set the Z coordinate of a point.
 * @param point Pointer to the point geometry.
 * @param z Z coordinate value.
 */
SDK_C_API void ogc_point_set_z(ogc_geometry_t* point, double z);

/**
 * @brief Set the M (measure) value of a point.
 * @param point Pointer to the point geometry.
 * @param m M value.
 */
SDK_C_API void ogc_point_set_m(ogc_geometry_t* point, double m);

/**
 * @brief Get the coordinate of a point.
 * @param point Pointer to the point geometry.
 * @return Coordinate structure.
 */
SDK_C_API ogc_coordinate_t ogc_point_get_coordinate(const ogc_geometry_t* point);

/* 2.5 LineString */
/* Java: cn.cycle.chart.api.geometry.LineString */
/* C++:   ogc::geom::LineString */
/* Header: ogc/geom/linestring.h */

/**
 * @brief Create an empty LineString.
 * @return Pointer to newly created LineString geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_linestring_create(void);

/**
 * @brief Create a LineString from an array of coordinates.
 * @param coords Array of coordinates.
 * @param count Number of coordinates.
 * @return Pointer to newly created LineString geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_linestring_create_from_coords(const ogc_coordinate_t* coords, size_t count);

/**
 * @brief Get the number of points in a LineString.
 * @param line Pointer to the LineString geometry.
 * @return Number of points.
 */
SDK_C_API size_t ogc_linestring_get_num_points(const ogc_geometry_t* line);

/**
 * @brief Get the Nth point coordinate from a LineString.
 * @param line Pointer to the LineString geometry.
 * @param index Point index (0-based).
 * @return Coordinate at the specified index.
 */
SDK_C_API ogc_coordinate_t ogc_linestring_get_point_n(const ogc_geometry_t* line, size_t index);

/**
 * @brief Add a 2D point to a LineString.
 * @param line Pointer to the LineString geometry.
 * @param x X coordinate.
 * @param y Y coordinate.
 */
SDK_C_API void ogc_linestring_add_point(ogc_geometry_t* line, double x, double y);

/**
 * @brief Add a 3D point to a LineString.
 * @param line Pointer to the LineString geometry.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param z Z coordinate.
 */
SDK_C_API void ogc_linestring_add_point_3d(ogc_geometry_t* line, double x, double y, double z);

/**
 * @brief Set the Nth point coordinate in a LineString.
 * @param line Pointer to the LineString geometry.
 * @param index Point index (0-based).
 * @param coord Pointer to the new coordinate.
 */
SDK_C_API void ogc_linestring_set_point_n(ogc_geometry_t* line, size_t index, const ogc_coordinate_t* coord);

/**
 * @brief Get the Nth point as a Point geometry from a LineString.
 * @param line Pointer to the LineString geometry.
 * @param index Point index (0-based).
 * @return Pointer to newly created Point geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_linestring_get_point_geometry(const ogc_geometry_t* line, size_t index);

/* 2.6 LinearRing */
/* Java: cn.cycle.chart.api.geometry.LinearRing */
/* C++:   ogc::geom::LinearRing */
/* Header: ogc/geom/linearring.h */

/**
 * @brief Create an empty LinearRing.
 * @return Pointer to newly created LinearRing geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_linearring_create(void);

/**
 * @brief Create a LinearRing from an array of coordinates.
 * @param coords Array of coordinates (must form a closed ring).
 * @param count Number of coordinates.
 * @return Pointer to newly created LinearRing geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_linearring_create_from_coords(const ogc_coordinate_t* coords, size_t count);

/**
 * @brief Check if a LinearRing is closed.
 * @param ring Pointer to the LinearRing geometry.
 * @return Non-zero if closed, zero otherwise.
 */
SDK_C_API int ogc_linearring_is_closed(const ogc_geometry_t* ring);

/* 2.7 Polygon */
/* Java: cn.cycle.chart.api.geometry.Polygon */
/* C++:   ogc::geom::Polygon */
/* Header: ogc/geom/polygon.h */

/**
 * @brief Create an empty Polygon.
 * @return Pointer to newly created Polygon geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_polygon_create(void);

/**
 * @brief Create a Polygon from an exterior ring.
 * @param exterior_ring Pointer to the exterior LinearRing (ownership transferred).
 * @return Pointer to newly created Polygon geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_polygon_create_from_ring(ogc_geometry_t* exterior_ring);

/**
 * @brief Create a Polygon from an array of coordinates.
 * @param coords Array of coordinates forming the exterior ring.
 * @param count Number of coordinates.
 * @return Pointer to newly created Polygon geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_polygon_create_from_coords(const ogc_coordinate_t* coords, size_t count);

/**
 * @brief Get the exterior ring of a Polygon.
 * @param polygon Pointer to the Polygon geometry.
 * @return Pointer to the exterior LinearRing geometry.
 */
SDK_C_API ogc_geometry_t* ogc_polygon_get_exterior_ring(const ogc_geometry_t* polygon);

/**
 * @brief Get the number of interior rings (holes) in a Polygon.
 * @param polygon Pointer to the Polygon geometry.
 * @return Number of interior rings.
 */
SDK_C_API size_t ogc_polygon_get_num_interior_rings(const ogc_geometry_t* polygon);

/**
 * @brief Get the Nth interior ring of a Polygon.
 * @param polygon Pointer to the Polygon geometry.
 * @param index Ring index (0-based).
 * @return Pointer to the interior LinearRing geometry.
 */
SDK_C_API ogc_geometry_t* ogc_polygon_get_interior_ring_n(const ogc_geometry_t* polygon, size_t index);

/**
 * @brief Add an interior ring (hole) to a Polygon.
 * @param polygon Pointer to the Polygon geometry.
 * @param ring Pointer to the LinearRing to add (ownership transferred).
 */
SDK_C_API void ogc_polygon_add_interior_ring(ogc_geometry_t* polygon, ogc_geometry_t* ring);

/**
 * @brief Check if a Polygon is valid.
 * @param polygon Pointer to the Polygon geometry.
 * @return Non-zero if valid, zero otherwise.
 */
SDK_C_API int ogc_polygon_is_valid(const ogc_geometry_t* polygon);

/* 2.8 MultiPoint */
/* Java: cn.cycle.chart.api.geometry.MultiPoint */
/* C++:   ogc::geom::MultiPoint */
/* Header: ogc/geom/multipoint.h */

/**
 * @brief Create an empty MultiPoint.
 * @return Pointer to newly created MultiPoint geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_multipoint_create(void);

/**
 * @brief Get the number of points in a MultiPoint.
 * @param mp Pointer to the MultiPoint geometry.
 * @return Number of points.
 */
SDK_C_API size_t ogc_multipoint_get_num_geometries(const ogc_geometry_t* mp);

/**
 * @brief Get the Nth point from a MultiPoint.
 * @param mp Pointer to the MultiPoint geometry.
 * @param index Point index (0-based).
 * @return Pointer to the Point geometry.
 */
SDK_C_API ogc_geometry_t* ogc_multipoint_get_geometry_n(const ogc_geometry_t* mp, size_t index);

/**
 * @brief Add a point to a MultiPoint.
 * @param mp Pointer to the MultiPoint geometry.
 * @param point Pointer to the Point geometry to add (ownership transferred).
 */
SDK_C_API void ogc_multipoint_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* point);

/* 2.9 MultiLineString */
/* Java: cn.cycle.chart.api.geometry.MultiLineString */
/* C++:   ogc::geom::MultiLineString */
/* Header: ogc/geom/multilinestring.h */

/**
 * @brief Create an empty MultiLineString.
 * @return Pointer to newly created MultiLineString geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_multilinestring_create(void);

/**
 * @brief Get the number of LineStrings in a MultiLineString.
 * @param mls Pointer to the MultiLineString geometry.
 * @return Number of LineStrings.
 */
SDK_C_API size_t ogc_multilinestring_get_num_geometries(const ogc_geometry_t* mls);

/**
 * @brief Get the Nth LineString from a MultiLineString.
 * @param mls Pointer to the MultiLineString geometry.
 * @param index LineString index (0-based).
 * @return Pointer to the LineString geometry.
 */
SDK_C_API ogc_geometry_t* ogc_multilinestring_get_geometry_n(const ogc_geometry_t* mls, size_t index);

/**
 * @brief Add a LineString to a MultiLineString.
 * @param mls Pointer to the MultiLineString geometry.
 * @param linestring Pointer to the LineString geometry to add (ownership transferred).
 */
SDK_C_API void ogc_multilinestring_add_geometry(ogc_geometry_t* mls, ogc_geometry_t* linestring);

/* 2.10 MultiPolygon */
/* Java: cn.cycle.chart.api.geometry.MultiPolygon */
/* C++:   ogc::geom::MultiPolygon */
/* Header: ogc/geom/multipolygon.h */

/**
 * @brief Create an empty MultiPolygon.
 * @return Pointer to newly created MultiPolygon geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_multipolygon_create(void);

/**
 * @brief Get the number of Polygons in a MultiPolygon.
 * @param mp Pointer to the MultiPolygon geometry.
 * @return Number of Polygons.
 */
SDK_C_API size_t ogc_multipolygon_get_num_geometries(const ogc_geometry_t* mp);

/**
 * @brief Get the Nth Polygon from a MultiPolygon.
 * @param mp Pointer to the MultiPolygon geometry.
 * @param index Polygon index (0-based).
 * @return Pointer to the Polygon geometry.
 */
SDK_C_API ogc_geometry_t* ogc_multipolygon_get_geometry_n(const ogc_geometry_t* mp, size_t index);

/**
 * @brief Add a Polygon to a MultiPolygon.
 * @param mp Pointer to the MultiPolygon geometry.
 * @param polygon Pointer to the Polygon geometry to add (ownership transferred).
 */
SDK_C_API void ogc_multipolygon_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* polygon);

/* 2.11 GeometryCollection */
/* Java: cn.cycle.chart.api.geometry.GeometryCollection */
/* C++:   ogc::geom::GeometryCollection */
/* Header: ogc/geom/geometrycollection.h */

/**
 * @brief Create an empty GeometryCollection.
 * @return Pointer to newly created GeometryCollection, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_collection_create(void);

/**
 * @brief Get the number of geometries in a GeometryCollection.
 * @param gc Pointer to the GeometryCollection.
 * @return Number of geometries.
 */
SDK_C_API size_t ogc_geometry_collection_get_num_geometries(const ogc_geometry_t* gc);

/**
 * @brief Get the Nth geometry from a GeometryCollection.
 * @param gc Pointer to the GeometryCollection.
 * @param index Geometry index (0-based).
 * @return Pointer to the geometry.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_collection_get_geometry_n(const ogc_geometry_t* gc, size_t index);

/**
 * @brief Add a geometry to a GeometryCollection.
 * @param gc Pointer to the GeometryCollection.
 * @param geom Pointer to the geometry to add (ownership transferred).
 */
SDK_C_API void ogc_geometry_collection_add_geometry(ogc_geometry_t* gc, ogc_geometry_t* geom);

/* 2.12 GeometryFactory */
/* Java: cn.cycle.chart.api.geometry.GeometryFactory */
/* C++:   ogc::geom::GeometryFactory */
/* Header: ogc/geom/factory.h */

/**
 * @brief Opaque type representing a geometry factory.
 */
typedef struct ogc_geometry_factory_t ogc_geometry_factory_t;

/**
 * @brief Create a geometry factory.
 * @return Pointer to newly created factory, or NULL on failure.
 */
SDK_C_API ogc_geometry_factory_t* ogc_geometry_factory_create(void);

/**
 * @brief Destroy a geometry factory.
 * @param factory Pointer to the factory to destroy.
 */
SDK_C_API void ogc_geometry_factory_destroy(ogc_geometry_factory_t* factory);

/**
 * @brief Create a point using the factory.
 * @param factory Pointer to the factory.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to newly created point geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_point(ogc_geometry_factory_t* factory, double x, double y);

/**
 * @brief Create a LineString using the factory.
 * @param factory Pointer to the factory.
 * @return Pointer to newly created LineString geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_linestring(ogc_geometry_factory_t* factory);

/**
 * @brief Create a Polygon using the factory.
 * @param factory Pointer to the factory.
 * @return Pointer to newly created Polygon geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_polygon(ogc_geometry_factory_t* factory);

/**
 * @brief Create a geometry from WKT string.
 * @param factory Pointer to the factory.
 * @param wkt WKT string.
 * @return Pointer to newly created geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_from_wkt(ogc_geometry_factory_t* factory, const char* wkt);

/**
 * @brief Create a geometry from WKB (Well-Known Binary).
 * @param factory Pointer to the factory.
 * @param wkb WKB byte array.
 * @param size Size of the WKB array.
 * @return Pointer to newly created geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_from_wkb(ogc_geometry_factory_t* factory, const unsigned char* wkb, size_t size);

/**
 * @brief Create a geometry from GeoJSON string.
 * @param factory Pointer to the factory.
 * @param geojson GeoJSON string.
 * @return Pointer to newly created geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_geometry_factory_create_from_geojson(ogc_geometry_factory_t* factory, const char* geojson);

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

/**
 * @brief Field type enumeration.
 */
typedef enum ogc_field_type_e {
    OGC_FIELD_TYPE_INTEGER = 0,       /**< 32-bit integer */
    OGC_FIELD_TYPE_INTEGER64 = 1,     /**< 64-bit integer */
    OGC_FIELD_TYPE_REAL = 2,          /**< Double precision floating point */
    OGC_FIELD_TYPE_STRING = 3,        /**< String */
    OGC_FIELD_TYPE_BINARY = 4,        /**< Binary data */
    OGC_FIELD_TYPE_DATE = 5,          /**< Date */
    OGC_FIELD_TYPE_TIME = 6,          /**< Time */
    OGC_FIELD_TYPE_DATETIME = 7,      /**< Date and time */
    OGC_FIELD_TYPE_INTEGER_LIST = 8,  /**< List of integers */
    OGC_FIELD_TYPE_REAL_LIST = 9,     /**< List of doubles */
    OGC_FIELD_TYPE_STRING_LIST = 10   /**< List of strings */
} ogc_field_type_e;

/* 3.2 FieldDefn */
/* Java: cn.cycle.chart.api.feature.FieldDefn */
/* C++:   ogc::feature::CNFieldDefn */
/* Header: ogc/feature/field_defn.h */

/**
 * @brief Opaque type representing a field definition.
 */
typedef struct ogc_field_defn_t ogc_field_defn_t;

/**
 * @brief Create a field definition.
 * @param name Field name.
 * @param type Field type.
 * @return Pointer to newly created field definition, or NULL on failure.
 */
SDK_C_API ogc_field_defn_t* ogc_field_defn_create(const char* name, ogc_field_type_e type);

/**
 * @brief Destroy a field definition.
 * @param defn Pointer to the field definition to destroy.
 */
SDK_C_API void ogc_field_defn_destroy(ogc_field_defn_t* defn);

/**
 * @brief Get the name of a field definition.
 * @param defn Pointer to the field definition.
 * @return Field name string.
 */
SDK_C_API const char* ogc_field_defn_get_name(const ogc_field_defn_t* defn);

/**
 * @brief Get the type of a field definition.
 * @param defn Pointer to the field definition.
 * @return Field type enumeration value.
 */
SDK_C_API ogc_field_type_e ogc_field_defn_get_type(const ogc_field_defn_t* defn);

/**
 * @brief Get the width of a field definition.
 * @param defn Pointer to the field definition.
 * @return Field width.
 */
SDK_C_API int ogc_field_defn_get_width(const ogc_field_defn_t* defn);

/**
 * @brief Get the precision of a field definition.
 * @param defn Pointer to the field definition.
 * @return Field precision.
 */
SDK_C_API int ogc_field_defn_get_precision(const ogc_field_defn_t* defn);

/**
 * @brief Set the width of a field definition.
 * @param defn Pointer to the field definition.
 * @param width Field width.
 */
SDK_C_API void ogc_field_defn_set_width(ogc_field_defn_t* defn, int width);

/**
 * @brief Set the precision of a field definition.
 * @param defn Pointer to the field definition.
 * @param precision Field precision.
 */
SDK_C_API void ogc_field_defn_set_precision(ogc_field_defn_t* defn, int precision);

/* 3.3 FieldValue */
/* Java: cn.cycle.chart.api.feature.FieldValue */
/* C++:   ogc::feature::CNFieldValue */
/* Header: ogc/feature/field_value.h */

/**
 * @brief Opaque type representing a field value.
 */
typedef struct ogc_field_value_t ogc_field_value_t;

/**
 * @brief Create an empty field value.
 * @return Pointer to newly created field value, or NULL on failure.
 */
SDK_C_API ogc_field_value_t* ogc_field_value_create(void);

/**
 * @brief Destroy a field value.
 * @param value Pointer to the field value to destroy.
 */
SDK_C_API void ogc_field_value_destroy(ogc_field_value_t* value);

/**
 * @brief Check if a field value is null.
 * @param value Pointer to the field value.
 * @return Non-zero if null, zero otherwise.
 */
SDK_C_API int ogc_field_value_is_null(const ogc_field_value_t* value);

/**
 * @brief Check if a field value is set.
 * @param value Pointer to the field value.
 * @return Non-zero if set, zero otherwise.
 */
SDK_C_API int ogc_field_value_is_set(const ogc_field_value_t* value);

/**
 * @brief Get field value as integer.
 * @param value Pointer to the field value.
 * @return Integer value.
 */
SDK_C_API int ogc_field_value_get_as_integer(const ogc_field_value_t* value);

/**
 * @brief Get field value as 64-bit integer.
 * @param value Pointer to the field value.
 * @return 64-bit integer value.
 */
SDK_C_API long long ogc_field_value_get_as_integer64(const ogc_field_value_t* value);

/**
 * @brief Get field value as double.
 * @param value Pointer to the field value.
 * @return Double value.
 */
SDK_C_API double ogc_field_value_get_as_real(const ogc_field_value_t* value);

/**
 * @brief Get field value as string.
 * @param value Pointer to the field value.
 * @return String value.
 */
SDK_C_API const char* ogc_field_value_get_as_string(const ogc_field_value_t* value);

/**
 * @brief Set field value as integer.
 * @param value Pointer to the field value.
 * @param val Integer value to set.
 */
SDK_C_API void ogc_field_value_set_integer(ogc_field_value_t* value, int val);

/**
 * @brief Set field value as 64-bit integer.
 * @param value Pointer to the field value.
 * @param val 64-bit integer value to set.
 */
SDK_C_API void ogc_field_value_set_integer64(ogc_field_value_t* value, long long val);

/**
 * @brief Set field value as double.
 * @param value Pointer to the field value.
 * @param val Double value to set.
 */
SDK_C_API void ogc_field_value_set_real(ogc_field_value_t* value, double val);

/**
 * @brief Set field value as string.
 * @param value Pointer to the field value.
 * @param val String value to set.
 */
SDK_C_API void ogc_field_value_set_string(ogc_field_value_t* value, const char* val);

/**
 * @brief Set field value to null.
 * @param value Pointer to the field value.
 */
SDK_C_API void ogc_field_value_set_null(ogc_field_value_t* value);

/* 3.4 FeatureDefn */
/* Java: cn.cycle.chart.api.feature.FeatureDefn (via CNFeatureDefn) */
/* C++:   ogc::feature::CNFeatureDefn */
/* Header: ogc/feature/feature_defn.h */

/**
 * @brief Opaque type representing a feature definition.
 */
typedef struct ogc_feature_defn_t ogc_feature_defn_t;

/**
 * @brief Create a feature definition.
 * @param name Feature definition name.
 * @return Pointer to newly created feature definition, or NULL on failure.
 */
SDK_C_API ogc_feature_defn_t* ogc_feature_defn_create(const char* name);

/**
 * @brief Destroy a feature definition.
 * @param defn Pointer to the feature definition to destroy.
 */
SDK_C_API void ogc_feature_defn_destroy(ogc_feature_defn_t* defn);

/**
 * @brief Get the name of a feature definition.
 * @param defn Pointer to the feature definition.
 * @return Feature definition name.
 */
SDK_C_API const char* ogc_feature_defn_get_name(const ogc_feature_defn_t* defn);

/**
 * @brief Get the number of fields in a feature definition.
 * @param defn Pointer to the feature definition.
 * @return Number of fields.
 */
SDK_C_API size_t ogc_feature_defn_get_field_count(const ogc_feature_defn_t* defn);

/**
 * @brief Get a field definition by index.
 * @param defn Pointer to the feature definition.
 * @param index Field index (0-based).
 * @return Pointer to the field definition.
 */
SDK_C_API ogc_field_defn_t* ogc_feature_defn_get_field_defn(const ogc_feature_defn_t* defn, size_t index);

/**
 * @brief Get the index of a field by name.
 * @param defn Pointer to the feature definition.
 * @param name Field name.
 * @return Field index, or -1 if not found.
 */
SDK_C_API int ogc_feature_defn_get_field_index(const ogc_feature_defn_t* defn, const char* name);

/**
 * @brief Add a field definition to a feature definition.
 * @param defn Pointer to the feature definition.
 * @param field Pointer to the field definition to add.
 */
SDK_C_API void ogc_feature_defn_add_field_defn(ogc_feature_defn_t* defn, ogc_field_defn_t* field);

/**
 * @brief Get the number of geometry fields in a feature definition.
 * @param defn Pointer to the feature definition.
 * @return Number of geometry fields.
 */
SDK_C_API size_t ogc_feature_defn_get_geom_field_count(const ogc_feature_defn_t* defn);

/* 3.5 FeatureInfo */
/* Java: cn.cycle.chart.api.feature.FeatureInfo */
/* C++:   ogc::feature::CNFeature */
/* Header: ogc/feature/feature.h */

/**
 * @brief Opaque type representing a feature.
 */
typedef struct ogc_feature_t ogc_feature_t;

/**
 * @brief Create a feature.
 * @param defn Pointer to the feature definition.
 * @return Pointer to newly created feature, or NULL on failure.
 */
SDK_C_API ogc_feature_t* ogc_feature_create(ogc_feature_defn_t* defn);

/**
 * @brief Destroy a feature.
 * @param feature Pointer to the feature to destroy.
 */
SDK_C_API void ogc_feature_destroy(ogc_feature_t* feature);

/**
 * @brief Get the feature ID.
 * @param feature Pointer to the feature.
 * @return Feature ID.
 */
SDK_C_API long long ogc_feature_get_fid(const ogc_feature_t* feature);

/**
 * @brief Set the feature ID.
 * @param feature Pointer to the feature.
 * @param fid Feature ID to set.
 */
SDK_C_API void ogc_feature_set_fid(ogc_feature_t* feature, long long fid);

/**
 * @brief Get the feature definition.
 * @param feature Pointer to the feature.
 * @return Pointer to the feature definition.
 */
SDK_C_API ogc_feature_defn_t* ogc_feature_get_defn(const ogc_feature_t* feature);

/**
 * @brief Get the number of fields in a feature.
 * @param feature Pointer to the feature.
 * @return Number of fields.
 */
SDK_C_API size_t ogc_feature_get_field_count(const ogc_feature_t* feature);
SDK_C_API int ogc_feature_is_field_set(const ogc_feature_t* feature, size_t index);
SDK_C_API int ogc_feature_is_field_null(const ogc_feature_t* feature, size_t index);
/**
 * @brief Get field value as integer.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @return Integer value.
 */
SDK_C_API int ogc_feature_get_field_as_integer(const ogc_feature_t* feature, size_t index);

/**
 * @brief Get field value as 64-bit integer.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @return 64-bit integer value.
 */
SDK_C_API long long ogc_feature_get_field_as_integer64(const ogc_feature_t* feature, size_t index);

/**
 * @brief Get field value as double.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @return Double value.
 */
SDK_C_API double ogc_feature_get_field_as_real(const ogc_feature_t* feature, size_t index);

/**
 * @brief Get field value as string.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @return String value.
 */
SDK_C_API const char* ogc_feature_get_field_as_string(const ogc_feature_t* feature, size_t index);

/**
 * @brief Set field value as integer.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @param value Integer value to set.
 */
SDK_C_API void ogc_feature_set_field_integer(ogc_feature_t* feature, size_t index, int value);

/**
 * @brief Set field value as 64-bit integer.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @param value 64-bit integer value to set.
 */
SDK_C_API void ogc_feature_set_field_integer64(ogc_feature_t* feature, size_t index, long long value);

/**
 * @brief Set field value as double.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @param value Double value to set.
 */
SDK_C_API void ogc_feature_set_field_real(ogc_feature_t* feature, size_t index, double value);

/**
 * @brief Set field value as string.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 * @param value String value to set.
 */
SDK_C_API void ogc_feature_set_field_string(ogc_feature_t* feature, size_t index, const char* value);

/**
 * @brief Set field value to null.
 * @param feature Pointer to the feature.
 * @param index Field index (0-based).
 */
SDK_C_API void ogc_feature_set_field_null(ogc_feature_t* feature, size_t index);

/* Field access by name (convenience methods) */

/**
 * @brief Get field value as string by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @return String value, or empty string if field not found.
 */
SDK_C_API const char* ogc_feature_get_field_as_string_by_name(const ogc_feature_t* feature, const char* name);

/**
 * @brief Set field value as string by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @param value String value to set.
 * @return 0 on success, -1 if field not found.
 */
SDK_C_API int ogc_feature_set_field_string_by_name(ogc_feature_t* feature, const char* name, const char* value);

/**
 * @brief Get field value as integer by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @return Integer value, or 0 if field not found.
 */
SDK_C_API int ogc_feature_get_field_as_integer_by_name(const ogc_feature_t* feature, const char* name);

/**
 * @brief Set field value as integer by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @param value Integer value to set.
 * @return 0 on success, -1 if field not found.
 */
SDK_C_API int ogc_feature_set_field_integer_by_name(ogc_feature_t* feature, const char* name, int value);

/**
 * @brief Get field value as real by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @return Real value, or 0.0 if field not found.
 */
SDK_C_API double ogc_feature_get_field_as_real_by_name(const ogc_feature_t* feature, const char* name);

/**
 * @brief Set field value as real by field name.
 * @param feature Pointer to the feature.
 * @param name Field name.
 * @param value Real value to set.
 * @return 0 on success, -1 if field not found.
 */
SDK_C_API int ogc_feature_set_field_real_by_name(ogc_feature_t* feature, const char* name, double value);

/**
 * @brief Get the geometry of a feature.
 * @param feature Pointer to the feature.
 * @return Pointer to the geometry (owned by feature).
 */
SDK_C_API ogc_geometry_t* ogc_feature_get_geometry(const ogc_feature_t* feature);

/**
 * @brief Set the geometry of a feature.
 * @param feature Pointer to the feature.
 * @param geom Pointer to the geometry (ownership transferred).
 */
SDK_C_API void ogc_feature_set_geometry(ogc_feature_t* feature, ogc_geometry_t* geom);

/**
 * @brief Steal the geometry from a feature (ownership transferred to caller).
 * @param feature Pointer to the feature.
 * @return Pointer to the geometry (caller must destroy).
 */
SDK_C_API ogc_geometry_t* ogc_feature_steal_geometry(ogc_feature_t* feature);

/**
 * @brief Get the bounding envelope of a feature.
 * @param feature Pointer to the feature.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_feature_get_envelope(const ogc_feature_t* feature);

/**
 * @brief Create a deep copy of a feature.
 * @param feature Pointer to the feature.
 * @return Pointer to newly created feature, or NULL on failure.
 */
SDK_C_API ogc_feature_t* ogc_feature_clone(const ogc_feature_t* feature);

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

/**
 * @brief Layer type enumeration.
 */
typedef enum ogc_layer_type_e {
    OGC_LAYER_TYPE_VECTOR = 0,   /**< Vector layer */
    OGC_LAYER_TYPE_RASTER = 1,   /**< Raster layer */
    OGC_LAYER_TYPE_UNKNOWN = 2   /**< Unknown layer type */
} ogc_layer_type_e;

/* 4.2 ChartLayer */
/* Java: cn.cycle.chart.api.layer.ChartLayer */
/* C++:   ogc::layer::CNLayer */
/* Header: ogc/layer/layer.h */

/**
 * @brief Opaque type representing a layer.
 * 
 * @note Layer Visibility and Opacity:
 * CNLayer does not provide visibility/opacity methods directly.
 * Use LayerManager functions instead:
 *   - ogc_layer_manager_get_layer_visible()
 *   - ogc_layer_manager_set_layer_visible()
 *   - ogc_layer_manager_get_layer_opacity()
 *   - ogc_layer_manager_set_layer_opacity()
 */
typedef struct ogc_layer_t ogc_layer_t;

/**
 * @brief Destroy a layer.
 * @param layer Pointer to the layer to destroy.
 */
SDK_C_API void ogc_layer_destroy(ogc_layer_t* layer);

/**
 * @brief Get the name of a layer.
 * @param layer Pointer to the layer.
 * @return Layer name string.
 */
SDK_C_API const char* ogc_layer_get_name(const ogc_layer_t* layer);

/**
 * @brief Get the type of a layer.
 * @param layer Pointer to the layer.
 * @return Layer type enumeration value.
 */
SDK_C_API ogc_layer_type_e ogc_layer_get_type(const ogc_layer_t* layer);

/* 4.3 VectorLayer */
/* Java: cn.cycle.chart.api.layer.VectorLayer */
/* C++:   ogc::layer::CNVectorLayer */
/* Header: ogc/layer/vector_layer.h */

/**
 * @brief Opaque type representing a vector layer.
 * @note This is an alias for ogc_layer_t.
 */
typedef ogc_layer_t ogc_vector_layer_t;

/**
 * @brief Create a vector layer.
 * @param name Layer name.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_vector_layer_create(const char* name);

/**
 * @brief Create a vector layer from a data source.
 * @param datasource_path Path to the data source file.
 * @param layer_name Name of the layer in the data source.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_vector_layer_create_from_datasource(const char* datasource_path, const char* layer_name);

/**
 * @brief Get the number of features in a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Number of features.
 */
SDK_C_API size_t ogc_vector_layer_get_feature_count(const ogc_layer_t* layer);

/**
 * @brief Get the next feature from a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Pointer to the next feature, or NULL if no more features.
 */
SDK_C_API ogc_feature_t* ogc_vector_layer_get_next_feature(ogc_layer_t* layer);

/**
 * @brief Get a feature by ID from a vector layer.
 * @param layer Pointer to the vector layer.
 * @param fid Feature ID.
 * @return Pointer to the feature, or NULL if not found.
 */
SDK_C_API ogc_feature_t* ogc_vector_layer_get_feature_by_id(ogc_layer_t* layer, int64_t fid);

/**
 * @brief Reset the reading position of a vector layer.
 * @param layer Pointer to the vector layer.
 */
SDK_C_API void ogc_vector_layer_reset_reading(ogc_layer_t* layer);

/**
 * @brief Set a spatial filter on a vector layer.
 * @param layer Pointer to the vector layer.
 * @param min_x Minimum X coordinate.
 * @param min_y Minimum Y coordinate.
 * @param max_x Maximum X coordinate.
 * @param max_y Maximum Y coordinate.
 */
SDK_C_API void ogc_vector_layer_set_spatial_filter(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y);

/**
 * @brief Get the extent of a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_vector_layer_get_extent(ogc_layer_t* layer);

/**
 * @brief Add a feature to a vector layer.
 * @param layer Pointer to the vector layer.
 * @param feature Pointer to the feature to add (ownership transferred).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_vector_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature);

/**
 * @brief Update a feature in a vector layer.
 * @param layer Pointer to the vector layer.
 * @param feature Pointer to the feature with updated values.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_vector_layer_update_feature(ogc_layer_t* layer, const ogc_feature_t* feature);

/**
 * @brief Delete a feature from a vector layer.
 * @param layer Pointer to the vector layer.
 * @param fid Feature ID to delete.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_vector_layer_delete_feature(ogc_layer_t* layer, int64_t fid);

/**
 * @brief Create a new feature in a vector layer.
 * @param layer Pointer to the vector layer.
 * @param feature Pointer to the feature to create.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_vector_layer_create_feature(ogc_layer_t* layer, ogc_feature_t* feature);

/**
 * @brief Get the feature definition of a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Pointer to the feature definition, or NULL on failure.
 */
SDK_C_API ogc_feature_defn_t* ogc_vector_layer_get_feature_defn(ogc_layer_t* layer);

/**
 * @brief Get the spatial filter geometry of a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Pointer to the spatial filter geometry, or NULL if not set.
 */
SDK_C_API ogc_geometry_t* ogc_vector_layer_get_spatial_filter(ogc_layer_t* layer);

/**
 * @brief Get the attribute filter string of a vector layer.
 * @param layer Pointer to the vector layer.
 * @return Attribute filter string, or empty string if not set.
 */
SDK_C_API const char* ogc_vector_layer_get_attribute_filter(const ogc_layer_t* layer);

/**
 * @brief Create a vector layer from an array of features.
 * @param name Layer name.
 * @param features Array of feature pointers.
 * @param count Number of features.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_vector_layer_create_from_features(const char* name, ogc_feature_t** features, size_t count);

/**
 * @brief Set the style for a vector layer.
 * @param layer Pointer to the layer.
 * @param style_name Style name or JSON style string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_vector_layer_set_style(ogc_layer_t* layer, const char* style_name);

/* 4.4 RasterLayer */
/* Java: cn.cycle.chart.api.layer.RasterLayer */
/* C++:   ogc::layer::CNRasterLayer */
/* Header: ogc/layer/raster_layer.h */

/**
 * @brief Opaque type representing a raster layer.
 * @note This is an alias for ogc_layer_t.
 */
typedef ogc_layer_t ogc_raster_layer_t;

/**
 * @brief Create a raster layer from a file.
 * @param name Layer name.
 * @param filepath Path to the raster file.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_raster_layer_create(const char* name, const char* filepath);

/**
 * @brief Get the width of a raster layer.
 * @param layer Pointer to the raster layer.
 * @return Width in pixels.
 */
SDK_C_API int ogc_raster_layer_get_width(const ogc_layer_t* layer);

/**
 * @brief Get the height of a raster layer.
 * @param layer Pointer to the raster layer.
 * @return Height in pixels.
 */
SDK_C_API int ogc_raster_layer_get_height(const ogc_layer_t* layer);

/**
 * @brief Get the number of bands in a raster layer.
 * @param layer Pointer to the raster layer.
 * @return Number of bands.
 */
SDK_C_API int ogc_raster_layer_get_band_count(const ogc_layer_t* layer);

/**
 * @brief Get the no-data value for a band in a raster layer.
 * @param layer Pointer to the raster layer.
 * @param band_index Band index (0-based).
 * @return No-data value.
 */
SDK_C_API double ogc_raster_layer_get_no_data_value(const ogc_layer_t* layer, int band_index);

/**
 * @brief Get the extent of a raster layer.
 * @param layer Pointer to the raster layer.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_raster_layer_get_extent(ogc_layer_t* layer);

/* 4.5 MemoryLayer */
/* Java: cn.cycle.chart.api.layer.MemoryLayer */
/* C++:   ogc::layer::CNMemoryLayer */
/* Header: ogc/layer/memory_layer.h */

/**
 * @brief Create an in-memory layer.
 * @param name Layer name.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_memory_layer_create(const char* name);

/**
 * @brief Create an in-memory layer from an array of features.
 * @param name Layer name.
 * @param features Array of feature pointers.
 * @param count Number of features.
 * @return Pointer to newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_memory_layer_create_from_features(const char* name, ogc_feature_t** features, size_t count);

/**
 * @brief Add a feature to a memory layer.
 * @param layer Pointer to the memory layer.
 * @param feature Pointer to the feature to add (ownership transferred).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_memory_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature);

/**
 * @brief Remove a feature from a memory layer.
 * @param layer Pointer to the memory layer.
 * @param fid Feature ID to remove.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_memory_layer_remove_feature(ogc_layer_t* layer, int64_t fid);

/**
 * @brief Clear all features from a memory layer.
 * @param layer Pointer to the memory layer.
 */
SDK_C_API void ogc_memory_layer_clear(ogc_layer_t* layer);

/**
 * @brief Get the feature count of a memory layer.
 * @param layer Pointer to the memory layer.
 * @return Number of features in the layer.
 */
SDK_C_API int64_t ogc_memory_layer_get_feature_count(const ogc_layer_t* layer);

/* 4.6 LayerGroup */
/* Java: cn.cycle.chart.api.layer.LayerGroup */
/* C++:   ogc::layer::CNLayerGroup */
/* Header: ogc/layer/layer_group.h */

/**
 * @brief Opaque type representing a layer group.
 */
typedef struct ogc_layer_group_t ogc_layer_group_t;

/**
 * @brief Create a layer group.
 * @param name Group name.
 * @return Pointer to newly created layer group, or NULL on failure.
 */
SDK_C_API ogc_layer_group_t* ogc_layer_group_create(const char* name);

/**
 * @brief Destroy a layer group.
 * @param group Pointer to the layer group to destroy.
 */
SDK_C_API void ogc_layer_group_destroy(ogc_layer_group_t* group);

/**
 * @brief Get the name of a layer group.
 * @param group Pointer to the layer group.
 * @return Group name string.
 */
SDK_C_API const char* ogc_layer_group_get_name(const ogc_layer_group_t* group);

/**
 * @brief Get the number of layers in a layer group.
 * @param group Pointer to the layer group.
 * @return Number of layers.
 */
SDK_C_API size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group);

/**
 * @brief Get a layer from a layer group by index.
 * @param group Pointer to the layer group.
 * @param index Layer index (0-based).
 * @return Pointer to the layer.
 */
SDK_C_API ogc_layer_t* ogc_layer_group_get_layer(const ogc_layer_group_t* group, size_t index);

/**
 * @brief Add a layer to a layer group.
 * @param group Pointer to the layer group.
 * @param layer Pointer to the layer to add.
 */
SDK_C_API void ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer);

/**
 * @brief Remove a layer from a layer group by index.
 * @param group Pointer to the layer group.
 * @param index Layer index to remove.
 */
SDK_C_API void ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index);

/**
 * @brief Move a layer within a layer group.
 * @param group Pointer to the layer group.
 * @param from Source index.
 * @param to Destination index.
 */
SDK_C_API void ogc_layer_group_move_layer(ogc_layer_group_t* group, size_t from, size_t to);

/**
 * @brief Check if a layer group is visible.
 * @param group Pointer to the layer group.
 * @return Non-zero if visible, zero otherwise.
 */
SDK_C_API int ogc_layer_group_is_visible(const ogc_layer_group_t* group);

/**
 * @brief Set the visibility of a layer group.
 * @param group Pointer to the layer group.
 * @param visible Non-zero to show, zero to hide.
 */
SDK_C_API void ogc_layer_group_set_visible(ogc_layer_group_t* group, int visible);

/**
 * @brief Get the opacity of a layer group.
 * @param group Pointer to the layer group.
 * @return Opacity value (0.0 to 1.0).
 */
SDK_C_API double ogc_layer_group_get_opacity(const ogc_layer_group_t* group);

/**
 * @brief Set the opacity of a layer group.
 * @param group Pointer to the layer group.
 * @param opacity Opacity value (0.0 to 1.0).
 */
SDK_C_API void ogc_layer_group_set_opacity(ogc_layer_group_t* group, double opacity);

/**
 * @brief Get the Z-order of a layer group.
 * @param group Pointer to the layer group.
 * @return Z-order value.
 */
SDK_C_API int ogc_layer_group_get_z_order(const ogc_layer_group_t* group);

/**
 * @brief Set the Z-order of a layer group.
 * @param group Pointer to the layer group.
 * @param z_order Z-order value.
 */
SDK_C_API void ogc_layer_group_set_z_order(ogc_layer_group_t* group, int z_order);

/* 4.7 DataSource */
/* Java: cn.cycle.chart.api.layer.DataSource */
/* C++:   ogc::layer::CNDataSource */
/* Header: ogc/layer/datasource.h */

/**
 * @brief Opaque type representing a data source.
 */
typedef struct ogc_datasource_t ogc_datasource_t;

/**
 * @brief Open a data source.
 * @param path Path to the data source file.
 * @return Pointer to newly opened data source, or NULL on failure.
 */
SDK_C_API ogc_datasource_t* ogc_datasource_open(const char* path);

/**
 * @brief Close a data source.
 * @param ds Pointer to the data source to close.
 */
SDK_C_API void ogc_datasource_close(ogc_datasource_t* ds);

/**
 * @brief Check if a data source is open.
 * @param ds Pointer to the data source.
 * @return Non-zero if open, zero otherwise.
 */
SDK_C_API int ogc_datasource_is_open(const ogc_datasource_t* ds);

/**
 * @brief Get the path of a data source.
 * @param ds Pointer to the data source.
 * @return Path string.
 */
SDK_C_API const char* ogc_datasource_get_path(const ogc_datasource_t* ds);

/**
 * @brief Get the number of layers in a data source.
 * @param ds Pointer to the data source.
 * @return Number of layers.
 */
SDK_C_API size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds);

/**
 * @brief Get a layer from a data source by index.
 * @param ds Pointer to the data source.
 * @param index Layer index (0-based).
 * @return Pointer to the layer.
 */
SDK_C_API ogc_layer_t* ogc_datasource_get_layer(const ogc_datasource_t* ds, size_t index);

/**
 * @brief Get a layer from a data source by name.
 * @param ds Pointer to the data source.
 * @param name Layer name.
 * @return Pointer to the layer, or NULL if not found.
 */
SDK_C_API ogc_layer_t* ogc_datasource_get_layer_by_name(const ogc_datasource_t* ds, const char* name);

/**
 * @brief Create a new layer in a data source.
 * @param ds Pointer to the data source.
 * @param name Layer name.
 * @param geom_type Geometry type.
 * @return Pointer to the newly created layer, or NULL on failure.
 */
SDK_C_API ogc_layer_t* ogc_datasource_create_layer(ogc_datasource_t* ds, const char* name, int geom_type);

/**
 * @brief Delete a layer from a data source.
 * @param ds Pointer to the data source.
 * @param name Layer name to delete.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_datasource_delete_layer(ogc_datasource_t* ds, const char* name);

/* 4.8 DriverManager */
/* Java: cn.cycle.chart.api.layer.DriverManager */
/* C++:   ogc::layer::CNDriverManager */
/* Header: ogc/layer/driver_manager.h */

/**
 * @brief Opaque type representing a driver manager.
 */
typedef struct ogc_driver_manager_t ogc_driver_manager_t;

/**
 * @brief Opaque type representing a driver.
 */
typedef struct ogc_driver_t ogc_driver_t;

/**
 * @brief Get the singleton instance of the driver manager.
 * @return Pointer to the driver manager instance.
 */
SDK_C_API ogc_driver_manager_t* ogc_driver_manager_get_instance(void);

/**
 * @brief Register a driver with the driver manager.
 * @param mgr Pointer to the driver manager.
 * @param driver Pointer to the driver to register.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_driver_manager_register_driver(ogc_driver_manager_t* mgr, ogc_driver_t* driver);

/**
 * @brief Unregister a driver from the driver manager.
 * @param mgr Pointer to the driver manager.
 * @param name Driver name.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_driver_manager_unregister_driver(ogc_driver_manager_t* mgr, const char* name);

/**
 * @brief Get a driver by name.
 * @param mgr Pointer to the driver manager.
 * @param name Driver name.
 * @return Pointer to the driver, or NULL if not found.
 */
SDK_C_API ogc_driver_t* ogc_driver_manager_get_driver(const ogc_driver_manager_t* mgr, const char* name);

/**
 * @brief Get the number of registered drivers.
 * @param mgr Pointer to the driver manager.
 * @return Number of drivers.
 */
SDK_C_API int ogc_driver_manager_get_driver_count(const ogc_driver_manager_t* mgr);

/**
 * @brief Get the name of a driver by index.
 * @param mgr Pointer to the driver manager.
 * @param index Driver index (0-based).
 * @return Driver name string.
 */
SDK_C_API const char* ogc_driver_manager_get_driver_name(const ogc_driver_manager_t* mgr, int index);

/**
 * @brief Get the feature definition of a layer.
 * @param layer Pointer to the layer.
 * @return Pointer to the feature definition.
 */
SDK_C_API ogc_feature_defn_t* ogc_layer_get_feature_defn(ogc_layer_t* layer);

/**
 * @brief Get the geometry type of a layer.
 * @param layer Pointer to the layer.
 * @return Geometry type enumeration value.
 */
SDK_C_API ogc_geom_type_e ogc_layer_get_geom_type(const ogc_layer_t* layer);

/**
 * @brief Get the extent of a layer.
 * @param layer Pointer to the layer.
 * @param extent Pointer to store the extent.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_layer_get_extent(ogc_layer_t* layer, ogc_envelope_t* extent);

/**
 * @brief Get the number of features in a layer.
 * @param layer Pointer to the layer.
 * @return Number of features.
 */
SDK_C_API long long ogc_layer_get_feature_count(ogc_layer_t* layer);

/**
 * @brief Reset the reading position of a layer.
 * @param layer Pointer to the layer.
 */
SDK_C_API void ogc_layer_reset_reading(ogc_layer_t* layer);

/**
 * @brief Get the next feature from a layer.
 * @param layer Pointer to the layer.
 * @return Pointer to the next feature, or NULL if no more features.
 */
SDK_C_API ogc_feature_t* ogc_layer_get_next_feature(ogc_layer_t* layer);

/**
 * @brief Get a feature by ID from a layer.
 * @param layer Pointer to the layer.
 * @param fid Feature ID.
 * @return Pointer to the feature, or NULL if not found.
 */
SDK_C_API ogc_feature_t* ogc_layer_get_feature(ogc_layer_t* layer, long long fid);

/**
 * @brief Set a rectangular spatial filter on a layer.
 * @param layer Pointer to the layer.
 * @param min_x Minimum X coordinate.
 * @param min_y Minimum Y coordinate.
 * @param max_x Maximum X coordinate.
 * @param max_y Maximum Y coordinate.
 */
SDK_C_API void ogc_layer_set_spatial_filter_rect(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y);

/**
 * @brief Set an attribute filter on a layer.
 * @param layer Pointer to the layer.
 * @param filter SQL WHERE clause filter string.
 */
SDK_C_API void ogc_layer_set_attribute_filter(ogc_layer_t* layer, const char* filter);

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

/**
 * @brief RGBA color structure.
 */
typedef struct ogc_color_t {
    unsigned char r;   /**< Red component (0-255) */
    unsigned char g;   /**< Green component (0-255) */
    unsigned char b;   /**< Blue component (0-255) */
    unsigned char a;   /**< Alpha component (0-255) */
} ogc_color_t;

/**
 * @brief Create a color from RGBA components.
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @param a Alpha component (0-255).
 * @return Color structure.
 */
SDK_C_API ogc_color_t ogc_color_from_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/**
 * @brief Create a color from RGB components (alpha = 255).
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return Color structure.
 */
SDK_C_API ogc_color_t ogc_color_from_rgb(unsigned char r, unsigned char g, unsigned char b);

/**
 * @brief Create a color from a hexadecimal value.
 * @param hex Hexadecimal color value (0xRRGGBB or 0xRRGGBBAA).
 * @return Color structure.
 */
SDK_C_API ogc_color_t ogc_color_from_hex(unsigned int hex);

/**
 * @brief Convert a color to a hexadecimal value.
 * @param color Pointer to the color.
 * @return Hexadecimal color value (0xRRGGBB).
 */
SDK_C_API unsigned int ogc_color_to_hex(const ogc_color_t* color);

/**
 * @brief Extract RGBA components from a color.
 * @param color Pointer to the color.
 * @param r Pointer to store red component.
 * @param g Pointer to store green component.
 * @param b Pointer to store blue component.
 * @param a Pointer to store alpha component.
 */
SDK_C_API void ogc_color_to_rgba(const ogc_color_t* color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

/* 5.2 Font */
/* Java: cn.cycle.chart.api.draw.Font */
/* C++:   ogc::draw::Font */
/* Header: ogc/draw/font.h */

/**
 * @brief Opaque type representing a font.
 */
typedef struct ogc_font_t ogc_font_t;

/**
 * @brief Create a font.
 * @param family Font family name.
 * @param size Font size in points.
 * @return Pointer to newly created font, or NULL on failure.
 */
SDK_C_API ogc_font_t* ogc_font_create(const char* family, double size);

/**
 * @brief Destroy a font.
 * @param font Pointer to the font to destroy.
 */
SDK_C_API void ogc_font_destroy(ogc_font_t* font);

/**
 * @brief Get the font family name.
 * @param font Pointer to the font.
 * @return Font family name string.
 */
SDK_C_API const char* ogc_font_get_family(const ogc_font_t* font);

/**
 * @brief Get the font size.
 * @param font Pointer to the font.
 * @return Font size in points.
 */
SDK_C_API double ogc_font_get_size(const ogc_font_t* font);

/**
 * @brief Check if the font is bold.
 * @param font Pointer to the font.
 * @return Non-zero if bold, zero otherwise.
 */
SDK_C_API int ogc_font_is_bold(const ogc_font_t* font);

/**
 * @brief Check if the font is italic.
 * @param font Pointer to the font.
 * @return Non-zero if italic, zero otherwise.
 */
SDK_C_API int ogc_font_is_italic(const ogc_font_t* font);

/**
 * @brief Set the font bold style.
 * @param font Pointer to the font.
 * @param bold Non-zero for bold, zero for normal.
 */
SDK_C_API void ogc_font_set_bold(ogc_font_t* font, int bold);

/**
 * @brief Set the font italic style.
 * @param font Pointer to the font.
 * @param italic Non-zero for italic, zero for normal.
 */
SDK_C_API void ogc_font_set_italic(ogc_font_t* font, int italic);

/* 5.3 Pen */
/* Java: cn.cycle.chart.api.draw.Pen */
/* C++:   ogc::draw::Pen */
/* Header: ogc/draw/draw_style.h */

/**
 * @brief Pen style enumeration.
 */
typedef enum ogc_pen_style_e {
    OGC_PEN_STYLE_SOLID = 0,       /**< Solid line */
    OGC_PEN_STYLE_DASH = 1,        /**< Dashed line */
    OGC_PEN_STYLE_DOT = 2,         /**< Dotted line */
    OGC_PEN_STYLE_DASHDOT = 3,     /**< Dash-dot line */
    OGC_PEN_STYLE_DASHDOTDOT = 4,  /**< Dash-dot-dot line */
    OGC_PEN_STYLE_NONE = 5         /**< No line */
} ogc_pen_style_e;

/**
 * @brief Opaque type representing a pen (line style).
 */
typedef struct ogc_pen_t ogc_pen_t;

/**
 * @brief Create a pen.
 * @return Pointer to newly created pen, or NULL on failure.
 */
SDK_C_API ogc_pen_t* ogc_pen_create(void);

/**
 * @brief Destroy a pen.
 * @param pen Pointer to the pen to destroy.
 */
SDK_C_API void ogc_pen_destroy(ogc_pen_t* pen);

/**
 * @brief Get the pen color.
 * @param pen Pointer to the pen.
 * @return Color structure.
 */
SDK_C_API ogc_color_t ogc_pen_get_color(const ogc_pen_t* pen);

/**
 * @brief Get the pen width.
 * @param pen Pointer to the pen.
 * @return Width in pixels.
 */
SDK_C_API double ogc_pen_get_width(const ogc_pen_t* pen);

/**
 * @brief Set the pen color.
 * @param pen Pointer to the pen.
 * @param color Color to set.
 */
SDK_C_API void ogc_pen_set_color(ogc_pen_t* pen, ogc_color_t color);

/**
 * @brief Set the pen width.
 * @param pen Pointer to the pen.
 * @param width Width in pixels.
 */
SDK_C_API void ogc_pen_set_width(ogc_pen_t* pen, double width);

/**
 * @brief Get the pen style.
 * @param pen Pointer to the pen.
 * @return Pen style enumeration value.
 */
SDK_C_API ogc_pen_style_e ogc_pen_get_style(const ogc_pen_t* pen);

/**
 * @brief Set the pen style.
 * @param pen Pointer to the pen.
 * @param style Pen style enumeration value.
 */
SDK_C_API void ogc_pen_set_style(ogc_pen_t* pen, ogc_pen_style_e style);

/* 5.4 Brush */
/* Java: cn.cycle.chart.api.draw.Brush */
/* C++:   ogc::draw::Brush */
/* Header: ogc/draw/draw_style.h */

/**
 * @brief Brush style enumeration.
 */
typedef enum ogc_brush_style_e {
    OGC_BRUSH_STYLE_SOLID = 0,          /**< Solid fill */
    OGC_BRUSH_STYLE_NONE = 1,           /**< No fill */
    OGC_BRUSH_STYLE_HORIZONTAL = 2,     /**< Horizontal hatch */
    OGC_BRUSH_STYLE_VERTICAL = 3,       /**< Vertical hatch */
    OGC_BRUSH_STYLE_CROSS = 4,          /**< Cross hatch */
    OGC_BRUSH_STYLE_BDIAGONAL = 5,      /**< Backward diagonal hatch */
    OGC_BRUSH_STYLE_FDIAGONAL = 6,      /**< Forward diagonal hatch */
    OGC_BRUSH_STYLE_DIAGONAL_CROSS = 7, /**< Diagonal cross hatch */
    OGC_BRUSH_STYLE_TEXTURE = 8         /**< Texture fill */
} ogc_brush_style_e;

/**
 * @brief Opaque type representing a brush (fill style).
 */
typedef struct ogc_brush_t ogc_brush_t;

/**
 * @brief Create a brush.
 * @return Pointer to newly created brush, or NULL on failure.
 */
SDK_C_API ogc_brush_t* ogc_brush_create(void);

/**
 * @brief Destroy a brush.
 * @param brush Pointer to the brush to destroy.
 */
SDK_C_API void ogc_brush_destroy(ogc_brush_t* brush);

/**
 * @brief Get the brush color.
 * @param brush Pointer to the brush.
 * @return Color structure.
 */
SDK_C_API ogc_color_t ogc_brush_get_color(const ogc_brush_t* brush);

/**
 * @brief Set the brush color.
 * @param brush Pointer to the brush.
 * @param color Color to set.
 */
SDK_C_API void ogc_brush_set_color(ogc_brush_t* brush, ogc_color_t color);

/**
 * @brief Get the brush style.
 * @param brush Pointer to the brush.
 * @return Brush style enumeration value.
 */
SDK_C_API ogc_brush_style_e ogc_brush_get_style(const ogc_brush_t* brush);

/**
 * @brief Set the brush style.
 * @param brush Pointer to the brush.
 * @param style Brush style enumeration value.
 */
SDK_C_API void ogc_brush_set_style(ogc_brush_t* brush, ogc_brush_style_e style);

/* 5.5 DrawStyle */
/* Java: cn.cycle.chart.api.symbology.DrawStyle */
/* C++:   ogc::draw::DrawStyle */
/* Header: ogc/draw/draw_style.h */

/**
 * @brief Opaque type representing a drawing style.
 */
typedef struct ogc_draw_style_t ogc_draw_style_t;

/**
 * @brief Create a drawing style.
 * @return Pointer to newly created style, or NULL on failure.
 */
SDK_C_API ogc_draw_style_t* ogc_draw_style_create(void);

/**
 * @brief Destroy a drawing style.
 * @param style Pointer to the style to destroy.
 */
SDK_C_API void ogc_draw_style_destroy(ogc_draw_style_t* style);

/**
 * @brief Get the pen from a drawing style.
 * @param style Pointer to the style.
 * @return Pointer to the pen.
 */
SDK_C_API ogc_pen_t* ogc_draw_style_get_pen(ogc_draw_style_t* style);

/**
 * @brief Get the brush from a drawing style.
 * @param style Pointer to the style.
 * @return Pointer to the brush.
 */
SDK_C_API ogc_brush_t* ogc_draw_style_get_brush(ogc_draw_style_t* style);

/**
 * @brief Get the font from a drawing style.
 * @param style Pointer to the style.
 * @return Pointer to the font.
 */
SDK_C_API ogc_font_t* ogc_draw_style_get_font(ogc_draw_style_t* style);

/**
 * @brief Set the pen for a drawing style.
 * @param style Pointer to the style.
 * @param pen Pointer to the pen.
 */
SDK_C_API void ogc_draw_style_set_pen(ogc_draw_style_t* style, ogc_pen_t* pen);

/**
 * @brief Set the brush for a drawing style.
 * @param style Pointer to the style.
 * @param brush Pointer to the brush.
 */
SDK_C_API void ogc_draw_style_set_brush(ogc_draw_style_t* style, ogc_brush_t* brush);

/**
 * @brief Set the font for a drawing style.
 * @param style Pointer to the style.
 * @param font Pointer to the font.
 */
SDK_C_API void ogc_draw_style_set_font(ogc_draw_style_t* style, ogc_font_t* font);

/* 5.6 Image */
/* Java: cn.cycle.chart.api.draw.Image */
/* C++:   ogc::draw::Image */
/* Header: ogc/draw/image.h */

/**
 * @brief Opaque type representing an image.
 */
typedef struct ogc_image_t ogc_image_t;

/**
 * @brief Create an image.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @param channels Number of color channels (1, 3, or 4).
 * @return Pointer to newly created image, or NULL on failure.
 */
SDK_C_API ogc_image_t* ogc_image_create(int width, int height, int channels);

/**
 * @brief Destroy an image.
 * @param image Pointer to the image to destroy.
 */
SDK_C_API void ogc_image_destroy(ogc_image_t* image);

/**
 * @brief Get the width of an image.
 * @param image Pointer to the image.
 * @return Width in pixels.
 */
SDK_C_API int ogc_image_get_width(const ogc_image_t* image);

/**
 * @brief Get the height of an image.
 * @param image Pointer to the image.
 * @return Height in pixels.
 */
SDK_C_API int ogc_image_get_height(const ogc_image_t* image);

/**
 * @brief Get the number of channels in an image.
 * @param image Pointer to the image.
 * @return Number of channels.
 */
SDK_C_API int ogc_image_get_channels(const ogc_image_t* image);

/**
 * @brief Get the pixel data of an image (mutable).
 * @param image Pointer to the image.
 * @return Pointer to the pixel data.
 */
unsigned char* ogc_image_get_data(ogc_image_t* image);

/**
 * @brief Get the pixel data of an image (const).
 * @param image Pointer to the image.
 * @return Const pointer to the pixel data.
 */
const unsigned char* ogc_image_get_data_const(const ogc_image_t* image);

/**
 * @brief Load an image from a file.
 * @param path Path to the image file.
 * @return Pointer to newly loaded image, or NULL on failure.
 */
SDK_C_API ogc_image_t* ogc_image_load_from_file(const char* path);

/**
 * @brief Save an image to a file.
 * @param image Pointer to the image.
 * @param path Path to save the image.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_image_save_to_file(const ogc_image_t* image, const char* path);

/* 5.7 DrawDevice */
/* Java: cn.cycle.chart.api.draw.DrawDevice */
/* C++:   ogc::draw::DrawDevice */
/* Header: ogc/draw/draw_device.h */

/**
 * @brief Opaque type representing a drawing device.
 */
typedef struct ogc_draw_device_t ogc_draw_device_t;

/**
 * @brief Device type enumeration.
 */
typedef enum ogc_device_type_e {
    OGC_DEVICE_TYPE_DISPLAY = 0,     /**< Display device */
    OGC_DEVICE_TYPE_RASTER_IMAGE = 1, /**< Raster image device */
    OGC_DEVICE_TYPE_PDF = 2,         /**< PDF device */
    OGC_DEVICE_TYPE_SVG = 3,         /**< SVG device */
    OGC_DEVICE_TYPE_TILE = 4,        /**< Tile device */
    OGC_DEVICE_TYPE_WEBGL = 5        /**< WebGL device */
} ogc_device_type_e;

/**
 * @brief Create a drawing device.
 * @param type Device type.
 * @param width Device width in pixels.
 * @param height Device height in pixels.
 * @return Pointer to newly created device, or NULL on failure.
 */
SDK_C_API ogc_draw_device_t* ogc_draw_device_create(ogc_device_type_e type, int width, int height);

/**
 * @brief Destroy a drawing device.
 * @param device Pointer to the device to destroy.
 */
SDK_C_API void ogc_draw_device_destroy(ogc_draw_device_t* device);

/**
 * @brief Get the width of a drawing device.
 * @param device Pointer to the device.
 * @return Width in pixels.
 */
SDK_C_API int ogc_draw_device_get_width(const ogc_draw_device_t* device);

/**
 * @brief Get the height of a drawing device.
 * @param device Pointer to the device.
 * @return Height in pixels.
 */
SDK_C_API int ogc_draw_device_get_height(const ogc_draw_device_t* device);

/**
 * @brief Get the image from a drawing device.
 * @param device Pointer to the device.
 * @return Pointer to the image.
 */
SDK_C_API ogc_image_t* ogc_draw_device_get_image(ogc_draw_device_t* device);

/* 5.7.1 ImageDevice (for JavaFX integration) */
/**
 * @brief Opaque type representing an image device.
 */
typedef struct ogc_image_device_t ogc_image_device_t;

/**
 * @brief Create an image device.
 * @param width Pixel width.
 * @param height Pixel height.
 * @return Pointer to newly created image device, or NULL on failure.
 */
SDK_C_API ogc_image_device_t* ogc_image_device_create(size_t width, size_t height);

/**
 * @brief Destroy an image device.
 * @param device Pointer to the image device to destroy.
 */
SDK_C_API void ogc_image_device_destroy(ogc_image_device_t* device);

/**
 * @brief Resize an image device.
 * @param device Pointer to the image device.
 * @param width New pixel width.
 * @param height New pixel height.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_image_device_resize(ogc_image_device_t* device, size_t width, size_t height);

/**
 * @brief Clear an image device.
 * @param device Pointer to the image device.
 */
SDK_C_API void ogc_image_device_clear(ogc_image_device_t* device);

/**
 * @brief Get pixel data from an image device.
 * @param device Pointer to the image device.
 * @param size Pointer to store the size of the pixel data.
 * @return Pointer to pixel data (BGRA format), or NULL on failure.
 */
SDK_C_API const unsigned char* ogc_image_device_get_pixels(ogc_image_device_t* device, size_t* size);

/**
 * @brief Get the width of an image device.
 * @param device Pointer to the image device.
 * @return Width in pixels, or 0 on failure.
 */
SDK_C_API int ogc_image_device_get_width(const ogc_image_device_t* device);

/**
 * @brief Get the height of an image device.
 * @param device Pointer to the image device.
 * @return Height in pixels, or 0 on failure.
 */
SDK_C_API int ogc_image_device_get_height(const ogc_image_device_t* device);

/**
 * @brief Get the native pointer of an image device.
 * @param device Pointer to the image device.
 * @return Native pointer, or 0 on failure.
 */
SDK_C_API long long ogc_image_device_get_native_ptr(const ogc_image_device_t* device);

/* 5.8 DrawEngine */
/* Java: cn.cycle.chart.api.draw.DrawEngine */
/* C++:   ogc::draw::DrawEngine */
/* Header: ogc/draw/draw_engine.h */

/**
 * @brief Opaque type representing a drawing engine.
 */
typedef struct ogc_draw_engine_t ogc_draw_engine_t;

/**
 * @brief Engine type enumeration.
 */
typedef enum ogc_engine_type_e {
    OGC_ENGINE_TYPE_SIMPLE2D = 0,  /**< Simple 2D engine */
    OGC_ENGINE_TYPE_GPU = 1,       /**< GPU-accelerated engine */
    OGC_ENGINE_TYPE_VECTOR = 2,    /**< Vector rendering engine */
    OGC_ENGINE_TYPE_TILE = 3       /**< Tile rendering engine */
} ogc_engine_type_e;

/**
 * @brief Create a drawing engine.
 * @param type Engine type.
 * @return Pointer to newly created engine, or NULL on failure.
 */
SDK_C_API ogc_draw_engine_t* ogc_draw_engine_create(ogc_engine_type_e type);

/**
 * @brief Destroy a drawing engine.
 * @param engine Pointer to the engine to destroy.
 */
SDK_C_API void ogc_draw_engine_destroy(ogc_draw_engine_t* engine);

/**
 * @brief Initialize a drawing engine with a device.
 * @param engine Pointer to the engine.
 * @param device Pointer to the device.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_draw_engine_initialize(ogc_draw_engine_t* engine, ogc_draw_device_t* device);

/**
 * @brief Finalize a drawing engine.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_draw_engine_finalize(ogc_draw_engine_t* engine);

/* 5.9 DrawContext */
/* Java: cn.cycle.chart.api.core.RenderContext */
/* C++:   ogc::draw::DrawContext */
/* Header: ogc/draw/draw_context.h */

/**
 * @brief Opaque type representing a drawing context.
 */
typedef struct ogc_draw_context_t ogc_draw_context_t;

/**
 * @brief Create a drawing context.
 * @param device Pointer to the drawing device.
 * @param engine Pointer to the drawing engine.
 * @return Pointer to newly created context, or NULL on failure.
 */
SDK_C_API ogc_draw_context_t* ogc_draw_context_create(ogc_draw_device_t* device, ogc_draw_engine_t* engine);

/**
 * @brief Destroy a drawing context.
 * @param ctx Pointer to the context to destroy.
 */
SDK_C_API void ogc_draw_context_destroy(ogc_draw_context_t* ctx);

/**
 * @brief Begin a drawing session.
 * @param ctx Pointer to the context.
 */
SDK_C_API void ogc_draw_context_begin_draw(ogc_draw_context_t* ctx);

/**
 * @brief End a drawing session.
 * @param ctx Pointer to the context.
 */
SDK_C_API void ogc_draw_context_end_draw(ogc_draw_context_t* ctx);

/**
 * @brief Clear the drawing context with a color.
 * @param ctx Pointer to the context.
 * @param color Clear color.
 */
SDK_C_API void ogc_draw_context_clear(ogc_draw_context_t* ctx, ogc_color_t color);

/**
 * @brief Draw a geometry with a style.
 * @param ctx Pointer to the context.
 * @param geom Pointer to the geometry to draw.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_draw_geometry(ogc_draw_context_t* ctx, const ogc_geometry_t* geom, const ogc_draw_style_t* style);
/**
 * @brief Draw a point.
 * @param ctx Pointer to the context.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_draw_point(ogc_draw_context_t* ctx, double x, double y, const ogc_draw_style_t* style);

/**
 * @brief Draw a line.
 * @param ctx Pointer to the context.
 * @param x1 Start X coordinate.
 * @param y1 Start Y coordinate.
 * @param x2 End X coordinate.
 * @param y2 End Y coordinate.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_draw_line(ogc_draw_context_t* ctx, double x1, double y1, double x2, double y2, const ogc_draw_style_t* style);

/**
 * @brief Draw a rectangle outline.
 * @param ctx Pointer to the context.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param w Width.
 * @param h Height.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_draw_rect(ogc_draw_context_t* ctx, double x, double y, double w, double h, const ogc_draw_style_t* style);

/**
 * @brief Fill a rectangle.
 * @param ctx Pointer to the context.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param w Width.
 * @param h Height.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_fill_rect(ogc_draw_context_t* ctx, double x, double y, double w, double h, const ogc_draw_style_t* style);

/**
 * @brief Draw text.
 * @param ctx Pointer to the context.
 * @param text Text string to draw.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_draw_context_draw_text(ogc_draw_context_t* ctx, const char* text, double x, double y, const ogc_draw_style_t* style);

/**
 * @brief Set the transformation matrix.
 * @param ctx Pointer to the context.
 * @param matrix 3x3 transformation matrix (9 elements).
 */
SDK_C_API void ogc_draw_context_set_transform(ogc_draw_context_t* ctx, double* matrix);

/**
 * @brief Reset the transformation matrix to identity.
 * @param ctx Pointer to the context.
 */
SDK_C_API void ogc_draw_context_reset_transform(ogc_draw_context_t* ctx);

/**
 * @brief Set a clipping region using a geometry.
 * @param ctx Pointer to the context.
 * @param geom Pointer to the clipping geometry.
 */
SDK_C_API void ogc_draw_context_clip(ogc_draw_context_t* ctx, const ogc_geometry_t* geom);

/**
 * @brief Reset the clipping region.
 * @param ctx Pointer to the context.
 */
SDK_C_API void ogc_draw_context_reset_clip(ogc_draw_context_t* ctx);

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

/**
 * @brief Opaque type representing a chart viewer.
 */
typedef struct ogc_chart_viewer_t ogc_chart_viewer_t;

/**
 * @brief Create a chart viewer.
 * @return Pointer to newly created viewer, or NULL on failure.
 */
SDK_C_API ogc_chart_viewer_t* ogc_chart_viewer_create(void);

/**
 * @brief Destroy a chart viewer.
 * @param viewer Pointer to the viewer to destroy.
 */
SDK_C_API void ogc_chart_viewer_destroy(ogc_chart_viewer_t* viewer);

/**
 * @brief Initialize a chart viewer.
 * @param viewer Pointer to the viewer.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_viewer_initialize(ogc_chart_viewer_t* viewer);

/**
 * @brief Shutdown a chart viewer.
 * @param viewer Pointer to the viewer.
 */
SDK_C_API void ogc_chart_viewer_shutdown(ogc_chart_viewer_t* viewer);

/**
 * @brief Load a chart from a file.
 * @param viewer Pointer to the viewer.
 * @param path Path to the chart file.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_viewer_load_chart(ogc_chart_viewer_t* viewer, const char* path);

/**
 * @brief Render the chart to a device.
 * @param viewer Pointer to the viewer.
 * @param device Pointer to the drawing device.
 * @param width Render width in pixels.
 * @param height Render height in pixels.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_viewer_render(ogc_chart_viewer_t* viewer, ogc_draw_device_t* device, int width, int height);

/**
 * @brief Set the viewport parameters.
 * @param viewer Pointer to the viewer.
 * @param center_x Center X coordinate.
 * @param center_y Center Y coordinate.
 * @param scale Scale factor.
 */
SDK_C_API void ogc_chart_viewer_set_viewport(ogc_chart_viewer_t* viewer, double center_x, double center_y, double scale);

/**
 * @brief Get the viewport parameters.
 * @param viewer Pointer to the viewer.
 * @param center_x Pointer to store center X coordinate.
 * @param center_y Pointer to store center Y coordinate.
 * @param scale Pointer to store scale factor.
 */
SDK_C_API void ogc_chart_viewer_get_viewport(ogc_chart_viewer_t* viewer, double* center_x, double* center_y, double* scale);

/**
 * @brief Pan the view.
 * @param viewer Pointer to the viewer.
 * @param dx Pan distance in X direction.
 * @param dy Pan distance in Y direction.
 */
SDK_C_API void ogc_chart_viewer_pan(ogc_chart_viewer_t* viewer, double dx, double dy);

/**
 * @brief Zoom the view.
 * @param viewer Pointer to the viewer.
 * @param factor Zoom factor (>1 to zoom in, <1 to zoom out).
 * @param center_x Zoom center X coordinate.
 * @param center_y Zoom center Y coordinate.
 */
SDK_C_API void ogc_chart_viewer_zoom(ogc_chart_viewer_t* viewer, double factor, double center_x, double center_y);

/**
 * @brief Query a feature at a point.
 * @param viewer Pointer to the viewer.
 * @param x X coordinate in world space.
 * @param y Y coordinate in world space.
 * @return Pointer to the feature, or NULL if not found.
 */
SDK_C_API ogc_feature_t* ogc_chart_viewer_query_feature(ogc_chart_viewer_t* viewer, double x, double y);

/**
 * @brief Convert screen coordinates to world coordinates.
 * @param viewer Pointer to the viewer.
 * @param screen_x Screen X coordinate.
 * @param screen_y Screen Y coordinate.
 * @param world_x Pointer to store world X coordinate.
 * @param world_y Pointer to store world Y coordinate.
 */
SDK_C_API void ogc_chart_viewer_screen_to_world(ogc_chart_viewer_t* viewer, double screen_x, double screen_y, double* world_x, double* world_y);

/**
 * @brief Convert world coordinates to screen coordinates.
 * @param viewer Pointer to the viewer.
 * @param world_x World X coordinate.
 * @param world_y World Y coordinate.
 * @param screen_x Pointer to store screen X coordinate.
 * @param screen_y Pointer to store screen Y coordinate.
 */
SDK_C_API void ogc_chart_viewer_world_to_screen(ogc_chart_viewer_t* viewer, double world_x, double world_y, double* screen_x, double* screen_y);

/* Forward declaration for ogc_viewport_t */
typedef struct ogc_viewport_t ogc_viewport_t;

/**
 * @brief Get the viewport object pointer.
 * @param viewer Pointer to the viewer.
 * @return Pointer to the viewport object, or NULL on failure.
 */
SDK_C_API ogc_viewport_t* ogc_chart_viewer_get_viewport_ptr(ogc_chart_viewer_t* viewer);

/**
 * @brief Get the full extent of the loaded chart.
 * @param viewer Pointer to the viewer.
 * @param min_x Pointer to store minimum X coordinate.
 * @param min_y Pointer to store minimum Y coordinate.
 * @param max_x Pointer to store maximum X coordinate.
 * @param max_y Pointer to store maximum Y coordinate.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_viewer_get_full_extent(ogc_chart_viewer_t* viewer, double* min_x, double* min_y, double* max_x, double* max_y);

/* 6.2 Viewport */
/* Java: cn.cycle.chart.api.core.Viewport */
/* C++:   ogc::graph::Viewport (in DrawFacade) */
/* Header: ogc/graph/render/draw_facade.h */

/**
 * @brief Create a viewport.
 * @return Pointer to newly created viewport, or NULL on failure.
 */
SDK_C_API ogc_viewport_t* ogc_viewport_create(void);

/**
 * @brief Destroy a viewport.
 * @param viewport Pointer to the viewport to destroy.
 */
SDK_C_API void ogc_viewport_destroy(ogc_viewport_t* viewport);

/**
 * @brief Get the center X coordinate.
 * @param viewport Pointer to the viewport.
 * @return Center X coordinate.
 */
SDK_C_API double ogc_viewport_get_center_x(const ogc_viewport_t* viewport);

/**
 * @brief Get the center Y coordinate.
 * @param viewport Pointer to the viewport.
 * @return Center Y coordinate.
 */
SDK_C_API double ogc_viewport_get_center_y(const ogc_viewport_t* viewport);

/**
 * @brief Get the scale factor.
 * @param viewport Pointer to the viewport.
 * @return Scale factor.
 */
SDK_C_API double ogc_viewport_get_scale(const ogc_viewport_t* viewport);

/**
 * @brief Get the rotation angle.
 * @param viewport Pointer to the viewport.
 * @return Rotation angle in degrees.
 */
SDK_C_API double ogc_viewport_get_rotation(const ogc_viewport_t* viewport);

/**
 * @brief Set the center coordinates.
 * @param viewport Pointer to the viewport.
 * @param x Center X coordinate.
 * @param y Center Y coordinate.
 */
SDK_C_API void ogc_viewport_set_center(ogc_viewport_t* viewport, double x, double y);

/**
 * @brief Set the scale factor.
 * @param viewport Pointer to the viewport.
 * @param scale Scale factor.
 */
SDK_C_API void ogc_viewport_set_scale(ogc_viewport_t* viewport, double scale);

/**
 * @brief Set the rotation angle.
 * @param viewport Pointer to the viewport.
 * @param rotation Rotation angle in degrees.
 */
SDK_C_API void ogc_viewport_set_rotation(ogc_viewport_t* viewport, double rotation);

/**
 * @brief Set the pixel size of the viewport.
 * @param viewport Pointer to the viewport.
 * @param width Pixel width.
 * @param height Pixel height.
 */
SDK_C_API void ogc_viewport_set_size(ogc_viewport_t* viewport, int width, int height);

/**
 * @brief Get the bounding envelope.
 * @param viewport Pointer to the viewport.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_viewport_get_bounds(const ogc_viewport_t* viewport);

/**
 * @brief Zoom to an extent.
 * @param viewport Pointer to the viewport.
 * @param extent Pointer to the extent to zoom to.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_zoom_to_extent(ogc_viewport_t* viewport, const ogc_envelope_t* extent);

/**
 * @brief Zoom to a specific scale.
 * @param viewport Pointer to the viewport.
 * @param scale Target scale.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_zoom_to_scale(ogc_viewport_t* viewport, double scale);

/**
 * @brief Convert screen coordinates to world coordinates.
 * @param viewport Pointer to the viewport.
 * @param sx Screen X coordinate (pixels).
 * @param sy Screen Y coordinate (pixels).
 * @param wx Pointer to store world X coordinate.
 * @param wy Pointer to store world Y coordinate.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_screen_to_world(const ogc_viewport_t* viewport, double sx, double sy, double* wx, double* wy);

/**
 * @brief Convert world coordinates to screen coordinates.
 * @param viewport Pointer to the viewport.
 * @param wx World X coordinate.
 * @param wy World Y coordinate.
 * @param sx Pointer to store screen X coordinate (pixels).
 * @param sy Pointer to store screen Y coordinate (pixels).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_world_to_screen(const ogc_viewport_t* viewport, double wx, double wy, double* sx, double* sy);

/**
 * @brief Pan the viewport by a delta.
 * @param viewport Pointer to the viewport.
 * @param dx Delta X in world coordinates.
 * @param dy Delta Y in world coordinates.
 */
SDK_C_API void ogc_viewport_pan(ogc_viewport_t* viewport, double dx, double dy);

/**
 * @brief Zoom the viewport by a factor.
 * @param viewport Pointer to the viewport.
 * @param factor Zoom factor (>1 to zoom in, <1 to zoom out).
 */
SDK_C_API void ogc_viewport_zoom(ogc_viewport_t* viewport, double factor);

/**
 * @brief Zoom the viewport at a specific center point.
 * @param viewport Pointer to the viewport.
 * @param factor Zoom factor (>1 to zoom in, <1 to zoom out).
 * @param center_x Center X coordinate in world coordinates.
 * @param center_y Center Y coordinate in world coordinates.
 */
SDK_C_API void ogc_viewport_zoom_at(ogc_viewport_t* viewport, double factor, double center_x, double center_y);

/**
 * @brief Set the extent of the viewport.
 * @param viewport Pointer to the viewport.
 * @param min_x Minimum X coordinate.
 * @param min_y Minimum Y coordinate.
 * @param max_x Maximum X coordinate.
 * @param max_y Maximum Y coordinate.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_set_extent(ogc_viewport_t* viewport, double min_x, double min_y, double max_x, double max_y);

/**
 * @brief Get the extent of the viewport.
 * @param viewport Pointer to the viewport.
 * @param min_x Pointer to store minimum X coordinate.
 * @param min_y Pointer to store minimum Y coordinate.
 * @param max_x Pointer to store maximum X coordinate.
 * @param max_y Pointer to store maximum Y coordinate.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_viewport_get_extent(const ogc_viewport_t* viewport, double* min_x, double* min_y, double* max_x, double* max_y);

/* 6.3 ChartConfig */
/* Java: cn.cycle.chart.api.core.ChartConfig */
/* C++:   ogc::graph::DrawParams */
/* Header: ogc/graph/render/draw_params.h */

/**
 * @brief Opaque type representing chart configuration.
 */
typedef struct ogc_chart_config_t ogc_chart_config_t;

/**
 * @brief Create a chart configuration.
 * @return Pointer to newly created configuration, or NULL on failure.
 */
SDK_C_API ogc_chart_config_t* ogc_chart_config_create(void);

/**
 * @brief Destroy a chart configuration.
 * @param config Pointer to the configuration to destroy.
 */
SDK_C_API void ogc_chart_config_destroy(ogc_chart_config_t* config);

/**
 * @brief Get the display mode.
 * @param config Pointer to the configuration.
 * @return Display mode.
 */
SDK_C_API int ogc_chart_config_get_display_mode(const ogc_chart_config_t* config);

/**
 * @brief Set the display mode.
 * @param config Pointer to the configuration.
 * @param mode Display mode.
 */
SDK_C_API void ogc_chart_config_set_display_mode(ogc_chart_config_t* config, int mode);

/**
 * @brief Check if grid is shown.
 * @param config Pointer to the configuration.
 * @return Non-zero if shown, zero otherwise.
 */
SDK_C_API int ogc_chart_config_get_show_grid(const ogc_chart_config_t* config);

/**
 * @brief Set whether to show grid.
 * @param config Pointer to the configuration.
 * @param show Non-zero to show, zero to hide.
 */
SDK_C_API void ogc_chart_config_set_show_grid(ogc_chart_config_t* config, int show);

/**
 * @brief Get the DPI setting.
 * @param config Pointer to the configuration.
 * @return DPI value.
 */
SDK_C_API double ogc_chart_config_get_dpi(const ogc_chart_config_t* config);

/**
 * @brief Set the DPI setting.
 * @param config Pointer to the configuration.
 * @param dpi DPI value.
 */
SDK_C_API void ogc_chart_config_set_dpi(ogc_chart_config_t* config, double dpi);

/* 6.4 LayerManager */
/* Java: cn.cycle.chart.api.layer.LayerManager */
/* C++:   ogc::graph::LayerManager */
/* Header: ogc/graph/layer/layer_manager.h */

/**
 * @brief Opaque type representing a layer manager.
 */
typedef struct ogc_layer_manager_t ogc_layer_manager_t;

/**
 * @brief Create a layer manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_layer_manager_t* ogc_layer_manager_create(void);

/**
 * @brief Destroy a layer manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_layer_manager_destroy(ogc_layer_manager_t* mgr);

/**
 * @brief Get the number of layers.
 * @param mgr Pointer to the manager.
 * @return Number of layers.
 */
SDK_C_API size_t ogc_layer_manager_get_layer_count(const ogc_layer_manager_t* mgr);

/**
 * @brief Get a layer by index.
 * @param mgr Pointer to the manager.
 * @param index Layer index (0-based).
 * @return Pointer to the layer.
 */
SDK_C_API ogc_layer_t* ogc_layer_manager_get_layer(const ogc_layer_manager_t* mgr, size_t index);

/**
 * @brief Get a layer by name.
 * @param mgr Pointer to the manager.
 * @param name Layer name.
 * @return Pointer to the layer, or NULL if not found.
 */
SDK_C_API ogc_layer_t* ogc_layer_manager_get_layer_by_name(const ogc_layer_manager_t* mgr, const char* name);

/**
 * @brief Add a layer to the manager.
 * @param mgr Pointer to the manager.
 * @param layer Pointer to the layer to add.
 */
SDK_C_API void ogc_layer_manager_add_layer(ogc_layer_manager_t* mgr, ogc_layer_t* layer);

/**
 * @brief Remove a layer by index.
 * @param mgr Pointer to the manager.
 * @param index Layer index to remove.
 */
SDK_C_API void ogc_layer_manager_remove_layer(ogc_layer_manager_t* mgr, size_t index);

/**
 * @brief Move a layer within the manager.
 * @param mgr Pointer to the manager.
 * @param from Source index.
 * @param to Destination index.
 */
SDK_C_API void ogc_layer_manager_move_layer(ogc_layer_manager_t* mgr, size_t from, size_t to);

/**
 * @brief Get the visibility of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @return Non-zero if visible, zero otherwise.
 */
SDK_C_API int ogc_layer_manager_get_layer_visible(const ogc_layer_manager_t* mgr, size_t index);

/**
 * @brief Set the visibility of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @param visible Non-zero to show, zero to hide.
 */
SDK_C_API void ogc_layer_manager_set_layer_visible(ogc_layer_manager_t* mgr, size_t index, int visible);

/**
 * @brief Get the opacity of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @return Opacity value (0.0 to 1.0).
 */
SDK_C_API double ogc_layer_manager_get_layer_opacity(const ogc_layer_manager_t* mgr, size_t index);

/**
 * @brief Set the opacity of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @param opacity Opacity value (0.0 to 1.0).
 */
SDK_C_API void ogc_layer_manager_set_layer_opacity(ogc_layer_manager_t* mgr, size_t index, double opacity);

/**
 * @brief Get the Z-order of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @return Z-order value.
 */
SDK_C_API int ogc_layer_manager_get_layer_z_order(const ogc_layer_manager_t* mgr, size_t index);

/**
 * @brief Set the Z-order of a layer.
 * @param mgr Pointer to the manager.
 * @param index Layer index.
 * @param z_order Z-order value.
 */
SDK_C_API void ogc_layer_manager_set_layer_z_order(ogc_layer_manager_t* mgr, size_t index, int z_order);

/**
 * @brief Sort layers by Z-order.
 * @param mgr Pointer to the manager.
 */
SDK_C_API void ogc_layer_manager_sort_by_z_order(ogc_layer_manager_t* mgr);

/* 6.5 LabelEngine */
/* Java: cn.cycle.chart.api.graph.LabelEngine */
/* C++:   ogc::graph::LabelEngine */
/* Header: ogc/graph/label/label_engine.h */

/**
 * @brief Opaque type representing a label engine.
 */
typedef struct ogc_label_engine_t ogc_label_engine_t;

/**
 * @brief Create a label engine.
 * @return Pointer to newly created engine, or NULL on failure.
 */
SDK_C_API ogc_label_engine_t* ogc_label_engine_create(void);

/**
 * @brief Destroy a label engine.
 * @param engine Pointer to the engine to destroy.
 */
SDK_C_API void ogc_label_engine_destroy(ogc_label_engine_t* engine);

/**
 * @brief Set the maximum number of labels.
 * @param engine Pointer to the engine.
 * @param max_labels Maximum number of labels.
 */
SDK_C_API void ogc_label_engine_set_max_labels(ogc_label_engine_t* engine, int max_labels);

/**
 * @brief Get the maximum number of labels.
 * @param engine Pointer to the engine.
 * @return Maximum number of labels.
 */
SDK_C_API int ogc_label_engine_get_max_labels(const ogc_label_engine_t* engine);

/**
 * @brief Enable or disable collision detection.
 * @param engine Pointer to the engine.
 * @param enable Non-zero to enable, zero to disable.
 */
SDK_C_API void ogc_label_engine_set_collision_detection(ogc_label_engine_t* engine, int enable);

/**
 * @brief Clear all labels.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_label_engine_clear(ogc_label_engine_t* engine);

/* 6.6 LabelInfo */
/* Java: cn.cycle.chart.api.graph.LabelInfo */
/* C++:   ogc::graph::LabelInfo */
/* Header: ogc/graph/label/label_info.h */

/**
 * @brief Opaque type representing label information.
 */
typedef struct ogc_label_info_t ogc_label_info_t;

/**
 * @brief Create a label info.
 * @param text Label text.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to newly created label info, or NULL on failure.
 */
SDK_C_API ogc_label_info_t* ogc_label_info_create(const char* text, double x, double y);

/**
 * @brief Destroy a label info.
 * @param info Pointer to the label info to destroy.
 */
SDK_C_API void ogc_label_info_destroy(ogc_label_info_t* info);

/**
 * @brief Get the label text.
 * @param info Pointer to the label info.
 * @return Label text string.
 */
SDK_C_API const char* ogc_label_info_get_text(const ogc_label_info_t* info);

/**
 * @brief Get the X coordinate.
 * @param info Pointer to the label info.
 * @return X coordinate.
 */
SDK_C_API double ogc_label_info_get_x(const ogc_label_info_t* info);

/**
 * @brief Get the Y coordinate.
 * @param info Pointer to the label info.
 * @return Y coordinate.
 */
SDK_C_API double ogc_label_info_get_y(const ogc_label_info_t* info);

/**
 * @brief Set the label text.
 * @param info Pointer to the label info.
 * @param text Label text.
 */
SDK_C_API void ogc_label_info_set_text(ogc_label_info_t* info, const char* text);

/**
 * @brief Set the label position.
 * @param info Pointer to the label info.
 * @param x X coordinate.
 * @param y Y coordinate.
 */
SDK_C_API void ogc_label_info_set_position(ogc_label_info_t* info, double x, double y);

/* 6.7 LODManager */
/* Java: cn.cycle.chart.api.graph.LODManager */
/* C++:   ogc::graph::LODManager */
/* Header: ogc/graph/lod/lod_manager.h */

/**
 * @brief Opaque type representing a LOD (Level of Detail) manager.
 */
typedef struct ogc_lod_manager_t ogc_lod_manager_t;

/**
 * @brief Create a LOD manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_lod_manager_t* ogc_lod_manager_create(void);

/**
 * @brief Destroy a LOD manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_lod_manager_destroy(ogc_lod_manager_t* mgr);

/**
 * @brief Get the current LOD level.
 * @param mgr Pointer to the manager.
 * @return Current LOD level.
 */
SDK_C_API int ogc_lod_manager_get_current_level(const ogc_lod_manager_t* mgr);

/**
 * @brief Set the current LOD level.
 * @param mgr Pointer to the manager.
 * @param level LOD level.
 */
SDK_C_API void ogc_lod_manager_set_current_level(ogc_lod_manager_t* mgr, int level);

/**
 * @brief Get the number of LOD levels.
 * @param mgr Pointer to the manager.
 * @return Number of LOD levels.
 */
SDK_C_API int ogc_lod_manager_get_level_count(const ogc_lod_manager_t* mgr);

/**
 * @brief Get the scale for a LOD level.
 * @param mgr Pointer to the manager.
 * @param level LOD level.
 * @return Scale for the level.
 */
SDK_C_API double ogc_lod_manager_get_scale_for_level(const ogc_lod_manager_t* mgr, int level);

/* 6.8 HitTest */
/* Java: cn.cycle.chart.api.graph.HitTest */
/* C++:   ogc::graph::HitTest */
/* Header: ogc/graph/interaction/hit_test.h */

/**
 * @brief Opaque type representing a hit test utility.
 */
typedef struct ogc_hit_test_t ogc_hit_test_t;

/**
 * @brief Create a hit test utility.
 * @return Pointer to newly created hit test, or NULL on failure.
 */
SDK_C_API ogc_hit_test_t* ogc_hit_test_create(void);

/**
 * @brief Destroy a hit test utility.
 * @param hit_test Pointer to the hit test to destroy.
 */
SDK_C_API void ogc_hit_test_destroy(ogc_hit_test_t* hit_test);

/**
 * @brief Query a feature at a point.
 * @param hit_test Pointer to the hit test.
 * @param layer Pointer to the layer to query.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param tolerance Search tolerance.
 * @return Pointer to the feature, or NULL if not found.
 */
SDK_C_API ogc_feature_t* ogc_hit_test_query_point(ogc_hit_test_t* hit_test, ogc_layer_t* layer, double x, double y, double tolerance);

/**
 * @brief Query features within a rectangle.
 * @param hit_test Pointer to the hit test.
 * @param layer Pointer to the layer to query.
 * @param min_x Minimum X coordinate.
 * @param min_y Minimum Y coordinate.
 * @param max_x Maximum X coordinate.
 * @param max_y Maximum Y coordinate.
 * @param count Pointer to store the number of features found.
 * @return Array of feature pointers (caller must free with ogc_hit_test_free_features).
 */
ogc_feature_t** ogc_hit_test_query_rect(ogc_hit_test_t* hit_test, ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y, int* count);

/**
 * @brief Free an array of features returned by ogc_hit_test_query_rect.
 * @param features Array of feature pointers to free.
 */
SDK_C_API void ogc_hit_test_free_features(ogc_feature_t** features);

/* 6.9 TransformManager */
/* Java: cn.cycle.chart.api.graph.TransformManager */
/* C++:   ogc::graph::TransformManager */
/* Header: ogc/graph/transform/transform_manager.h */

/**
 * @brief Opaque type representing a transform manager.
 */
typedef struct ogc_transform_manager_t ogc_transform_manager_t;

/**
 * @brief Create a transform manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_transform_manager_t* ogc_transform_manager_create(void);

/**
 * @brief Destroy a transform manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_transform_manager_destroy(ogc_transform_manager_t* mgr);

/**
 * @brief Set the viewport parameters.
 * @param mgr Pointer to the manager.
 * @param center_x Center X coordinate.
 * @param center_y Center Y coordinate.
 * @param scale Scale factor.
 * @param rotation Rotation angle in degrees.
 */
SDK_C_API void ogc_transform_manager_set_viewport(ogc_transform_manager_t* mgr, double center_x, double center_y, double scale, double rotation);

/**
 * @brief Get the transformation matrix.
 * @param mgr Pointer to the manager.
 * @param matrix Pointer to store 3x3 transformation matrix (9 elements).
 */
SDK_C_API void ogc_transform_manager_get_matrix(const ogc_transform_manager_t* mgr, double* matrix);

/* 6.10 RenderTask */
/* Java: cn.cycle.chart.api.graph.RenderTask */
/* C++:   ogc::graph::RenderTask */
/* Header: ogc/graph/render/render_task.h */

/**
 * @brief Opaque type representing a render task.
 */
typedef struct ogc_render_task_t ogc_render_task_t;

/**
 * @brief Create a render task.
 * @return Pointer to newly created task, or NULL on failure.
 */
SDK_C_API ogc_render_task_t* ogc_render_task_create(void);

/**
 * @brief Destroy a render task.
 * @param task Pointer to the task to destroy.
 */
SDK_C_API void ogc_render_task_destroy(ogc_render_task_t* task);

/**
 * @brief Execute the render task.
 * @param task Pointer to the task.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_render_task_execute(ogc_render_task_t* task);

/**
 * @brief Check if the render task is complete.
 * @param task Pointer to the task.
 * @return Non-zero if complete, zero otherwise.
 */
SDK_C_API int ogc_render_task_is_complete(const ogc_render_task_t* task);

/**
 * @brief Cancel the render task.
 * @param task Pointer to the task.
 */
SDK_C_API void ogc_render_task_cancel(ogc_render_task_t* task);

/* 6.11 RenderQueue */
/* Java: cn.cycle.chart.api.graph.RenderQueue */
/* C++:   ogc::graph::RenderQueue */
/* Header: ogc/graph/render/render_queue.h */

/**
 * @brief Opaque type representing a render queue.
 */
typedef struct ogc_render_queue_t ogc_render_queue_t;

/**
 * @brief Create a render queue.
 * @return Pointer to newly created queue, or NULL on failure.
 */
SDK_C_API ogc_render_queue_t* ogc_render_queue_create(void);

/**
 * @brief Destroy a render queue.
 * @param queue Pointer to the queue to destroy.
 */
SDK_C_API void ogc_render_queue_destroy(ogc_render_queue_t* queue);

/**
 * @brief Push a task to the queue.
 * @param queue Pointer to the queue.
 * @param task Pointer to the task to push.
 */
SDK_C_API void ogc_render_queue_push(ogc_render_queue_t* queue, ogc_render_task_t* task);

/**
 * @brief Pop a task from the queue.
 * @param queue Pointer to the queue.
 * @return Pointer to the popped task, or NULL if queue is empty.
 */
SDK_C_API ogc_render_task_t* ogc_render_queue_pop(ogc_render_queue_t* queue);

/**
 * @brief Get the number of tasks in the queue.
 * @param queue Pointer to the queue.
 * @return Number of tasks.
 */
SDK_C_API size_t ogc_render_queue_get_size(const ogc_render_queue_t* queue);

/**
 * @brief Clear all tasks from the queue.
 * @param queue Pointer to the queue.
 */
SDK_C_API void ogc_render_queue_clear(ogc_render_queue_t* queue);

/* 6.12 RenderOptimizer */
/* Java: cn.cycle.chart.api.optimize.RenderOptimizer */
/* C++:   ogc::graph::RenderOptimizer */
/* Header: ogc/graph/render/render_optimizer.h */

/**
 * @brief Opaque type representing a render optimizer.
 */
typedef struct ogc_render_optimizer_t ogc_render_optimizer_t;

/**
 * @brief Create a render optimizer.
 * @return Pointer to newly created optimizer, or NULL on failure.
 */
SDK_C_API ogc_render_optimizer_t* ogc_render_optimizer_create(void);

/**
 * @brief Destroy a render optimizer.
 * @param optimizer Pointer to the optimizer to destroy.
 */
SDK_C_API void ogc_render_optimizer_destroy(ogc_render_optimizer_t* optimizer);

/**
 * @brief Enable or disable caching.
 * @param optimizer Pointer to the optimizer.
 * @param enable Non-zero to enable, zero to disable.
 */
SDK_C_API void ogc_render_optimizer_set_cache_enabled(ogc_render_optimizer_t* optimizer, int enable);

/**
 * @brief Check if caching is enabled.
 * @param optimizer Pointer to the optimizer.
 * @return Non-zero if enabled, zero otherwise.
 */
SDK_C_API int ogc_render_optimizer_is_cache_enabled(const ogc_render_optimizer_t* optimizer);

/**
 * @brief Clear the optimizer cache.
 * @param optimizer Pointer to the optimizer.
 */
SDK_C_API void ogc_render_optimizer_clear_cache(ogc_render_optimizer_t* optimizer);

/* 6.13 RenderStats */
/* Java: cn.cycle.chart.api.optimize.RenderStats */
/* C++:   ogc::graph::RenderStats */
/* Header: ogc/graph/render/render_stats.h */

/**
 * @brief Render statistics structure.
 */
typedef struct ogc_render_stats_t {
    int total_frames;           /**< Total number of frames rendered */
    double total_render_time_ms; /**< Total render time in milliseconds */
    double avg_render_time_ms;   /**< Average render time per frame */
    int feature_count;          /**< Number of features rendered */
    int label_count;            /**< Number of labels rendered */
    int cache_hits;             /**< Number of cache hits */
    int cache_misses;           /**< Number of cache misses */
} ogc_render_stats_t;

/**
 * @brief Reset render statistics.
 * @param stats Pointer to the statistics to reset.
 */
SDK_C_API void ogc_render_stats_reset(ogc_render_stats_t* stats);

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

/**
 * @brief Tile key structure for identifying tiles.
 */
typedef struct ogc_tile_key_t {
    int x;   /**< Tile X coordinate */
    int y;   /**< Tile Y coordinate */
    int z;   /**< Tile zoom level */
} ogc_tile_key_t;

/**
 * @brief Create a tile key.
 * @param x Tile X coordinate.
 * @param y Tile Y coordinate.
 * @param z Tile zoom level.
 * @return Tile key structure.
 */
SDK_C_API ogc_tile_key_t ogc_tile_key_create(int x, int y, int z);

/**
 * @brief Check if two tile keys are equal.
 * @param a First tile key.
 * @param b Second tile key.
 * @return Non-zero if equal, zero otherwise.
 */
SDK_C_API int ogc_tile_key_equals(const ogc_tile_key_t* a, const ogc_tile_key_t* b);

/**
 * @brief Convert a tile key to a string.
 * @param key Pointer to the tile key.
 * @return String representation (caller must free).
 */
SDK_C_API char* ogc_tile_key_to_string(const ogc_tile_key_t* key);

/**
 * @brief Create a tile key from a string.
 * @param str String representation (format: "x/y/z").
 * @return Tile key structure.
 */
SDK_C_API ogc_tile_key_t ogc_tile_key_from_string(const char* str);

/**
 * @brief Convert a tile key to a unique index.
 * @param key Pointer to the tile key.
 * @return Unique index value.
 */
SDK_C_API uint64_t ogc_tile_key_to_index(const ogc_tile_key_t* key);

/**
 * @brief Get the parent tile key (one zoom level up).
 * @param key Pointer to the tile key.
 * @return Parent tile key structure.
 */
SDK_C_API ogc_tile_key_t ogc_tile_key_get_parent(const ogc_tile_key_t* key);

/**
 * @brief Get the four child tile keys (one zoom level down).
 * @param key Pointer to the tile key.
 * @param children Array to store the 4 child tile keys.
 */
SDK_C_API void ogc_tile_key_get_children(const ogc_tile_key_t* key, ogc_tile_key_t children[4]);

/**
 * @brief Convert a tile key to a bounding envelope.
 * @param key Pointer to the tile key.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_tile_key_to_envelope(const ogc_tile_key_t* key);

/* 7.2 TileCache */
/* Java: cn.cycle.chart.api.cache.TileCache */
/* C++:   ogc::cache::TileCache */
/* Header: ogc/cache/tile/tile_cache.h */

/**
 * @brief Opaque type representing a tile cache.
 */
typedef struct ogc_tile_cache_t ogc_tile_cache_t;

/**
 * @brief Opaque type representing tile data.
 */
typedef struct ogc_tile_data_t ogc_tile_data_t;

/**
 * @brief Create a tile cache.
 * @return Pointer to newly created cache, or NULL on failure.
 */
SDK_C_API ogc_tile_cache_t* ogc_tile_cache_create(void);

/**
 * @brief Destroy a tile cache.
 * @param cache Pointer to the cache to destroy.
 */
SDK_C_API void ogc_tile_cache_destroy(ogc_tile_cache_t* cache);

/**
 * @brief Check if a tile exists in the cache.
 * @param cache Pointer to the cache.
 * @param key Pointer to the tile key.
 * @return Non-zero if tile exists, zero otherwise.
 */
SDK_C_API int ogc_tile_cache_has_tile(const ogc_tile_cache_t* cache, const ogc_tile_key_t* key);

/**
 * @brief Get a tile from the cache.
 * @param cache Pointer to the cache.
 * @param key Pointer to the tile key.
 * @return Pointer to the tile data, or NULL if not found.
 */
SDK_C_API ogc_tile_data_t* ogc_tile_cache_get_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key);

/**
 * @brief Put a tile into the cache.
 * @param cache Pointer to the cache.
 * @param key Pointer to the tile key.
 * @param data Pointer to the tile data.
 * @param size Size of the tile data.
 */
SDK_C_API void ogc_tile_cache_put_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key, const void* data, size_t size);

/**
 * @brief Remove a tile from the cache.
 * @param cache Pointer to the cache.
 * @param key Pointer to the tile key.
 */
SDK_C_API void ogc_tile_cache_remove_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key);

/**
 * @brief Clear all tiles from the cache.
 * @param cache Pointer to the cache.
 */
SDK_C_API void ogc_tile_cache_clear(ogc_tile_cache_t* cache);

/**
 * @brief Get the current size of the cache.
 * @param cache Pointer to the cache.
 * @return Current cache size in bytes.
 */
SDK_C_API size_t ogc_tile_cache_get_size(const ogc_tile_cache_t* cache);

/**
 * @brief Get the maximum size of the cache.
 * @param cache Pointer to the cache.
 * @return Maximum cache size in bytes.
 */
SDK_C_API size_t ogc_tile_cache_get_max_size(const ogc_tile_cache_t* cache);

/**
 * @brief Set the maximum size of the cache.
 * @param cache Pointer to the cache.
 * @param max_size Maximum cache size in bytes.
 */
SDK_C_API void ogc_tile_cache_set_max_size(ogc_tile_cache_t* cache, size_t max_size);

/**
 * @brief Get the number of tiles in the cache.
 * @param cache Pointer to the cache.
 * @return Number of cached tiles.
 */
SDK_C_API size_t ogc_tile_cache_get_count(const ogc_tile_cache_t* cache);

/**
 * @brief Get the cache capacity (alias for ogc_tile_cache_get_max_size).
 * @param cache Pointer to the cache.
 * @return Cache capacity in bytes.
 */
SDK_C_API size_t ogc_tile_cache_get_capacity(const ogc_tile_cache_t* cache);

/**
 * @brief Set the cache capacity (alias for ogc_tile_cache_set_max_size).
 * @param cache Pointer to the cache.
 * @param capacity Cache capacity in bytes.
 */
SDK_C_API void ogc_tile_cache_set_capacity(ogc_tile_cache_t* cache, size_t capacity);

/* 7.3 MemoryTileCache */
/* Java: cn.cycle.chart.api.cache.MemoryTileCache */
/* C++:   ogc::cache::MemoryTileCache */
/* Header: ogc/cache/tile/memory_tile_cache.h */

/**
 * @brief Create an in-memory tile cache.
 * @param max_size_bytes Maximum cache size in bytes.
 * @return Pointer to newly created cache, or NULL on failure.
 */
SDK_C_API ogc_tile_cache_t* ogc_memory_tile_cache_create(size_t max_size_bytes);

/**
 * @brief Get the current memory usage of a memory tile cache.
 * @param cache Pointer to the memory tile cache.
 * @return Current memory usage in bytes.
 */
SDK_C_API size_t ogc_memory_tile_cache_get_memory_usage(const ogc_tile_cache_t* cache);

/* 7.4 DiskTileCache */
/* Java: cn.cycle.chart.api.cache.DiskTileCache */
/* C++:   ogc::cache::DiskTileCache */
/* Header: ogc/cache/tile/disk_tile_cache.h */

/**
 * @brief Create a disk-based tile cache.
 * @param cache_dir Directory to store cache files.
 * @param max_size_bytes Maximum cache size in bytes.
 * @return Pointer to newly created cache, or NULL on failure.
 */
SDK_C_API ogc_tile_cache_t* ogc_disk_tile_cache_create(const char* cache_dir, size_t max_size_bytes);

/* 7.5 MultiLevelTileCache */
/* Java: cn.cycle.chart.api.cache.MultiLevelTileCache */
/* C++:   ogc::cache::MultiLevelTileCache */
/* Header: ogc/cache/tile/multi_level_tile_cache.h */

/**
 * @brief Create a multi-level tile cache (memory + disk).
 * @param memory_cache Pointer to the memory cache.
 * @param disk_cache Pointer to the disk cache.
 * @return Pointer to newly created cache, or NULL on failure.
 */
SDK_C_API ogc_tile_cache_t* ogc_multi_level_tile_cache_create(ogc_tile_cache_t* memory_cache, ogc_tile_cache_t* disk_cache);

/* 7.6 OfflineStorageManager */
/* Java: cn.cycle.chart.api.cache.OfflineStorageManager */
/* C++:   ogc::cache::OfflineStorageManager */
/* Header: ogc/cache/offline/offline_storage_manager.h */

/**
 * @brief Opaque type representing offline storage.
 */
typedef struct ogc_offline_storage_t ogc_offline_storage_t;

/**
 * @brief Opaque type representing an offline region.
 */
typedef struct ogc_offline_region_t ogc_offline_region_t;

/**
 * @brief Create an offline storage manager.
 * @param storage_path Path to the storage directory.
 * @return Pointer to newly created storage, or NULL on failure.
 */
SDK_C_API ogc_offline_storage_t* ogc_offline_storage_create(const char* storage_path);

/**
 * @brief Destroy an offline storage manager.
 * @param storage Pointer to the storage to destroy.
 */
SDK_C_API void ogc_offline_storage_destroy(ogc_offline_storage_t* storage);

/**
 * @brief Create an offline region for download.
 * @param storage Pointer to the storage.
 * @param bounds Bounding envelope for the region.
 * @param min_zoom Minimum zoom level.
 * @param max_zoom Maximum zoom level.
 * @return Pointer to newly created region, or NULL on failure.
 */
SDK_C_API ogc_offline_region_t* ogc_offline_storage_create_region(ogc_offline_storage_t* storage, const ogc_envelope_t* bounds, int min_zoom, int max_zoom);

/**
 * @brief Destroy an offline region.
 * @param region Pointer to the region to destroy.
 */
SDK_C_API void ogc_offline_region_destroy(ogc_offline_region_t* region);

/**
 * @brief Start downloading an offline region.
 * @param region Pointer to the region.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_region_download(ogc_offline_region_t* region);

/**
 * @brief Get the download progress of an offline region.
 * @param region Pointer to the region.
 * @return Progress value (0.0 to 1.0).
 */
SDK_C_API float ogc_offline_region_get_progress(const ogc_offline_region_t* region);

/**
 * @brief Get the number of offline regions.
 * @param storage Pointer to the storage.
 * @return Number of regions.
 */
SDK_C_API size_t ogc_offline_storage_get_region_count(const ogc_offline_storage_t* storage);

/**
 * @brief Get an offline region by index.
 * @param storage Pointer to the storage.
 * @param index Region index (0-based).
 * @return Pointer to the region.
 */
SDK_C_API ogc_offline_region_t* ogc_offline_storage_get_region(const ogc_offline_storage_t* storage, size_t index);

/**
 * @brief Get an offline region by ID.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return Pointer to the region, or NULL if not found.
 */
SDK_C_API ogc_offline_region_t* ogc_offline_storage_get_region_by_id(const ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Get the ID of an offline region.
 * @param region Pointer to the region.
 * @return Region ID string, or NULL on failure.
 */
SDK_C_API const char* ogc_offline_region_get_id(const ogc_offline_region_t* region);

/**
 * @brief Get the name of an offline region.
 * @param region Pointer to the region.
 * @return Region name string, or NULL on failure.
 */
SDK_C_API const char* ogc_offline_region_get_name(const ogc_offline_region_t* region);

/**
 * @brief Remove an offline region.
 * @param storage Pointer to the storage.
 * @param index Region index to remove.
 */
SDK_C_API void ogc_offline_storage_remove_region(ogc_offline_storage_t* storage, size_t index);

/**
 * @brief Remove an offline region by ID.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_remove_region_by_id(ogc_offline_storage_t* storage, const char* region_id);

/* Offline Region Download Control */

/**
 * @brief Start downloading an offline region by ID.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_start_download(ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Pause downloading an offline region.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_pause_download(ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Resume downloading an offline region.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_resume_download(ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Cancel downloading an offline region.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_cancel_download(ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Check if an offline region is downloading.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return 1 if downloading, 0 if not, -1 on error.
 */
SDK_C_API int ogc_offline_storage_is_downloading(const ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Get the download progress of an offline region by ID.
 * @param storage Pointer to the storage.
 * @param region_id Region ID string.
 * @return Progress value (0.0 to 1.0), or -1.0 on error.
 */
SDK_C_API double ogc_offline_storage_get_download_progress(const ogc_offline_storage_t* storage, const char* region_id);

/**
 * @brief Get the total storage size in bytes.
 * @param storage Pointer to the storage.
 * @return Total storage size in bytes.
 */
SDK_C_API size_t ogc_offline_storage_get_storage_size(const ogc_offline_storage_t* storage);

/**
 * @brief Get the used storage size in bytes.
 * @param storage Pointer to the storage.
 * @return Used storage size in bytes.
 */
SDK_C_API size_t ogc_offline_storage_get_used_size(const ogc_offline_storage_t* storage);

/**
 * @brief Store chart data in offline storage.
 * @param storage Pointer to the storage.
 * @param chart_id Chart identifier.
 * @param data Chart data bytes.
 * @param size Data size in bytes.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_store_chart(ogc_offline_storage_t* storage, const char* chart_id, const void* data, size_t size);

/**
 * @brief Get chart data path from offline storage.
 * @param storage Pointer to the storage.
 * @param chart_id Chart identifier.
 * @return Chart file path, or NULL on failure. Caller must NOT free.
 */
SDK_C_API const char* ogc_offline_storage_get_chart_path(ogc_offline_storage_t* storage, const char* chart_id);

/**
 * @brief Remove chart data from offline storage.
 * @param storage Pointer to the storage.
 * @param chart_id Chart identifier.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_offline_storage_remove_chart(ogc_offline_storage_t* storage, const char* chart_id);

/**
 * @brief Get the number of stored charts.
 * @param storage Pointer to the storage.
 * @return Number of stored charts.
 */
SDK_C_API size_t ogc_offline_storage_get_chart_count(const ogc_offline_storage_t* storage);

/* 7.7 DataEncryption */
/* Java: cn.cycle.chart.api.cache.DataEncryption */
/* C++:   ogc::cache::DataEncryption */
/* Header: ogc/cache/offline/data_encryption.h */

/**
 * @brief Opaque type representing data encryption.
 */
typedef struct ogc_data_encryption_t ogc_data_encryption_t;

/**
 * @brief Create a data encryption utility.
 * @param key Encryption key.
 * @return Pointer to newly created encryption, or NULL on failure.
 */
SDK_C_API ogc_data_encryption_t* ogc_data_encryption_create(const char* key);

/**
 * @brief Destroy a data encryption utility.
 * @param encryption Pointer to the encryption to destroy.
 */
SDK_C_API void ogc_data_encryption_destroy(ogc_data_encryption_t* encryption);

/**
 * @brief Encrypt data.
 * @param encryption Pointer to the encryption.
 * @param input Input data to encrypt.
 * @param input_size Size of input data.
 * @param output Buffer to store encrypted data.
 * @param output_size Pointer to store output size (in: buffer size, out: actual size).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_data_encryption_encrypt(ogc_data_encryption_t* encryption, const void* input, size_t input_size, void* output, size_t* output_size);

/**
 * @brief Decrypt data.
 * @param encryption Pointer to the encryption.
 * @param input Input data to decrypt.
 * @param input_size Size of input data.
 * @param output Buffer to store decrypted data.
 * @param output_size Pointer to store output size (in: buffer size, out: actual size).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_data_encryption_decrypt(ogc_data_encryption_t* encryption, const void* input, size_t input_size, void* output, size_t* output_size);

/* 7.8 CacheManager */
/* Java: cn.cycle.chart.api.cache.CacheManager */
/* C++:   ogc::cache::CacheManager */
/* Header: ogc/cache/cache_manager.h */

/**
 * @brief Opaque type representing a cache manager.
 */
typedef struct ogc_cache_manager_t ogc_cache_manager_t;

/**
 * @brief Create a cache manager.
 * @return Pointer to newly created cache manager, or NULL on failure.
 */
SDK_C_API ogc_cache_manager_t* ogc_cache_manager_create(void);

/**
 * @brief Destroy a cache manager.
 * @param mgr Pointer to the cache manager to destroy.
 */
SDK_C_API void ogc_cache_manager_destroy(ogc_cache_manager_t* mgr);

/**
 * @brief Get the number of managed caches.
 * @param mgr Pointer to the cache manager.
 * @return Number of caches.
 */
SDK_C_API size_t ogc_cache_manager_get_cache_count(const ogc_cache_manager_t* mgr);

/**
 * @brief Get a managed cache by name.
 * @param mgr Pointer to the cache manager.
 * @param name Cache name.
 * @return Pointer to the cache, or NULL if not found.
 */
SDK_C_API ogc_tile_cache_t* ogc_cache_manager_get_cache(ogc_cache_manager_t* mgr, const char* name);

/**
 * @brief Add a cache to the manager.
 * @param mgr Pointer to the cache manager.
 * @param name Cache name.
 * @param cache Pointer to the cache to add.
 */
SDK_C_API void ogc_cache_manager_add_cache(ogc_cache_manager_t* mgr, const char* name, ogc_tile_cache_t* cache);

/**
 * @brief Remove a cache from the manager.
 * @param mgr Pointer to the cache manager.
 * @param name Cache name to remove.
 */
SDK_C_API void ogc_cache_manager_remove_cache(ogc_cache_manager_t* mgr, const char* name);

/**
 * @brief Clear all managed caches.
 * @param mgr Pointer to the cache manager.
 */
SDK_C_API void ogc_cache_manager_clear_all(ogc_cache_manager_t* mgr);

/**
 * @brief Get a tile from a named cache.
 * @param mgr Pointer to the cache manager.
 * @param cache_name Name of the cache.
 * @param key Pointer to the tile key.
 * @param output Buffer to store tile data.
 * @param output_size Pointer to store output size (in: buffer size, out: actual size).
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_cache_manager_get_tile(ogc_cache_manager_t* mgr, const char* cache_name, const ogc_tile_key_t* key, void* output, size_t* output_size);

/**
 * @brief Put a tile into a named cache.
 * @param mgr Pointer to the cache manager.
 * @param cache_name Name of the cache.
 * @param key Pointer to the tile key.
 * @param data Tile data.
 * @param size Size of tile data.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_cache_manager_put_tile(ogc_cache_manager_t* mgr, const char* cache_name, const ogc_tile_key_t* key, const void* data, size_t size);

/**
 * @brief Get total size of all managed caches.
 * @param mgr Pointer to the cache manager.
 * @return Total size in bytes.
 */
SDK_C_API size_t ogc_cache_manager_get_total_size(const ogc_cache_manager_t* mgr);

/**
 * @brief Get total tile count across all managed caches.
 * @param mgr Pointer to the cache manager.
 * @return Total tile count.
 */
SDK_C_API size_t ogc_cache_manager_get_total_tile_count(const ogc_cache_manager_t* mgr);

/**
 * @brief Flush all managed caches.
 * @param mgr Pointer to the cache manager.
 */
SDK_C_API void ogc_cache_manager_flush_all(ogc_cache_manager_t* mgr);

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

/**
 * @brief Opaque type representing a filter.
 */
typedef struct ogc_filter_t ogc_filter_t;

/**
 * @brief Filter type enumeration.
 */
typedef enum ogc_filter_type_e {
    OGC_FILTER_TYPE_COMPARISON = 0,  /**< Comparison filter */
    OGC_FILTER_TYPE_LOGICAL = 1,     /**< Logical filter (AND/OR/NOT) */
    OGC_FILTER_TYPE_SPATIAL = 2,     /**< Spatial filter */
    OGC_FILTER_TYPE_FEATURE_ID = 3,  /**< Feature ID filter */
    OGC_FILTER_TYPE_NONE = 4         /**< No filter */
} ogc_filter_type_e;

/**
 * @brief Create a filter.
 * @return Pointer to newly created filter, or NULL on failure.
 */
SDK_C_API ogc_filter_t* ogc_filter_create(void);

/**
 * @brief Destroy a filter.
 * @param filter Pointer to the filter to destroy.
 */
SDK_C_API void ogc_filter_destroy(ogc_filter_t* filter);

/**
 * @brief Get the type of a filter.
 * @param filter Pointer to the filter.
 * @return Filter type enumeration value.
 */
SDK_C_API ogc_filter_type_e ogc_filter_get_type(const ogc_filter_t* filter);

/**
 * @brief Evaluate a filter against a feature.
 * @param filter Pointer to the filter.
 * @param feature Pointer to the feature to evaluate.
 * @return Non-zero if the feature matches, zero otherwise.
 */
SDK_C_API int ogc_filter_evaluate(const ogc_filter_t* filter, const ogc_feature_t* feature);

/**
 * @brief Convert a filter to a string representation.
 * @param filter Pointer to the filter.
 * @return String representation (caller must free).
 */
SDK_C_API char* ogc_filter_to_string(const ogc_filter_t* filter);

/* 8.2 ComparisonFilter */
/* Java: cn.cycle.chart.api.symbology.ComparisonFilter */
/* C++:   ogc::symbology::ComparisonFilter */
/* Header: ogc/symbology/filter/comparison_filter.h */

/**
 * @brief Comparison operator enumeration.
 */
typedef enum ogc_comparison_operator_e {
    OGC_COMPARISON_EQUAL = 0,                /**< Equal to */
    OGC_COMPARISON_NOT_EQUAL = 1,            /**< Not equal to */
    OGC_COMPARISON_LESS_THAN = 2,            /**< Less than */
    OGC_COMPARISON_GREATER_THAN = 3,         /**< Greater than */
    OGC_COMPARISON_LESS_THAN_OR_EQUAL = 4,   /**< Less than or equal to */
    OGC_COMPARISON_GREATER_THAN_OR_EQUAL = 5, /**< Greater than or equal to */
    OGC_COMPARISON_LIKE = 6,                 /**< Pattern match */
    OGC_COMPARISON_IS_NULL = 7,              /**< Is null check */
    OGC_COMPARISON_BETWEEN = 8               /**< Between two values */
} ogc_comparison_operator_e;

/**
 * @brief Create a comparison filter.
 * @param property Property name to compare.
 * @param op Comparison operator.
 * @param value Value to compare against.
 * @return Pointer to newly created filter, or NULL on failure.
 */
SDK_C_API ogc_filter_t* ogc_comparison_filter_create(const char* property, ogc_comparison_operator_e op, const char* value);

/* 8.3 LogicalFilter */
/* Java: cn.cycle.chart.api.symbology.LogicalFilter */
/* C++:   ogc::symbology::LogicalFilter */
/* Header: ogc/symbology/filter/logical_filter.h */

/**
 * @brief Logical operator enumeration.
 */
typedef enum ogc_logical_operator_e {
    OGC_LOGICAL_AND = 0,  /**< Logical AND */
    OGC_LOGICAL_OR = 1,   /**< Logical OR */
    OGC_LOGICAL_NOT = 2   /**< Logical NOT */
} ogc_logical_operator_e;

/**
 * @brief Create a logical filter.
 * @param op Logical operator.
 * @return Pointer to newly created filter, or NULL on failure.
 */
SDK_C_API ogc_filter_t* ogc_logical_filter_create(ogc_logical_operator_e op);

/**
 * @brief Add a filter to a logical filter.
 * @param logical Pointer to the logical filter.
 * @param filter Pointer to the filter to add.
 */
SDK_C_API void ogc_logical_filter_add_filter(ogc_filter_t* logical, ogc_filter_t* filter);

/* 8.4 Symbolizer */
/* Java: cn.cycle.chart.api.symbology.Symbolizer */
/* C++:   ogc::symbology::Symbolizer */
/* Header: ogc/symbology/symbolizer/symbolizer.h */

/**
 * @brief Opaque type representing a symbolizer.
 */
typedef struct ogc_symbolizer_t ogc_symbolizer_t;

/**
 * @brief Symbolizer type enumeration.
 */
typedef enum ogc_symbolizer_type_e {
    OGC_SYMBOLIZER_POINT = 0,      /**< Point symbolizer */
    OGC_SYMBOLIZER_LINE = 1,       /**< Line symbolizer */
    OGC_SYMBOLIZER_POLYGON = 2,    /**< Polygon symbolizer */
    OGC_SYMBOLIZER_TEXT = 3,       /**< Text symbolizer */
    OGC_SYMBOLIZER_RASTER = 4,     /**< Raster symbolizer */
    OGC_SYMBOLIZER_ICON = 5,       /**< Icon symbolizer */
    OGC_SYMBOLIZER_COMPOSITE = 6   /**< Composite symbolizer */
} ogc_symbolizer_type_e;

/**
 * @brief Create a symbolizer.
 * @param type Symbolizer type.
 * @return Pointer to newly created symbolizer, or NULL on failure.
 */
SDK_C_API ogc_symbolizer_t* ogc_symbolizer_create(ogc_symbolizer_type_e type);

/**
 * @brief Destroy a symbolizer.
 * @param symbolizer Pointer to the symbolizer to destroy.
 */
SDK_C_API void ogc_symbolizer_destroy(ogc_symbolizer_t* symbolizer);

/**
 * @brief Get the type of a symbolizer.
 * @param symbolizer Pointer to the symbolizer.
 * @return Symbolizer type enumeration value.
 */
SDK_C_API ogc_symbolizer_type_e ogc_symbolizer_get_type(const ogc_symbolizer_t* symbolizer);

/**
 * @brief Get the drawing style of a symbolizer.
 * @param symbolizer Pointer to the symbolizer.
 * @return Pointer to the drawing style.
 */
SDK_C_API ogc_draw_style_t* ogc_symbolizer_get_style(ogc_symbolizer_t* symbolizer);

/**
 * @brief Set the drawing style of a symbolizer.
 * @param symbolizer Pointer to the symbolizer.
 * @param style Pointer to the drawing style.
 */
SDK_C_API void ogc_symbolizer_set_style(ogc_symbolizer_t* symbolizer, ogc_draw_style_t* style);

/**
 * @brief Symbolize a feature to a drawing device.
 * @param symbolizer Pointer to the symbolizer.
 * @param feature Pointer to the feature to symbolize.
 * @param device Pointer to the drawing device.
 * @return Non-zero on success, zero on failure.
 */
SDK_C_API int ogc_symbolizer_symbolize(ogc_symbolizer_t* symbolizer, const ogc_feature_t* feature, ogc_draw_device_t* device);

/* 8.5 ComparisonFilter (Extended) */
/* Java: cn.cycle.chart.api.symbology.ComparisonFilter */
/* C++:   ogc::symbology::ComparisonFilter */
/* Header: ogc/symbology/filter/comparison_filter.h */

/**
 * @brief Opaque type representing a comparison filter.
 */
typedef struct ogc_comparison_filter_t ogc_comparison_filter_t;

/**
 * @brief Destroy a comparison filter.
 * @param filter Pointer to the filter to destroy.
 */
SDK_C_API void ogc_comparison_filter_destroy(ogc_comparison_filter_t* filter);

/**
 * @brief Evaluate a comparison filter against a feature.
 * @param filter Pointer to the filter.
 * @param feature Pointer to the feature to evaluate.
 * @return Non-zero if the feature matches, zero otherwise.
 */
SDK_C_API int ogc_comparison_filter_evaluate(const ogc_comparison_filter_t* filter, const ogc_feature_t* feature);

/**
 * @brief Get the property name of a comparison filter.
 * @param filter Pointer to the filter.
 * @return Property name string.
 */
SDK_C_API const char* ogc_comparison_filter_get_property_name(const ogc_comparison_filter_t* filter);

/**
 * @brief Get the operator of a comparison filter.
 * @param filter Pointer to the filter.
 * @return Comparison operator.
 */
SDK_C_API ogc_comparison_operator_e ogc_comparison_filter_get_operator(const ogc_comparison_filter_t* filter);

/**
 * @brief Get the value of a comparison filter.
 * @param filter Pointer to the filter.
 * @return Value string.
 */
SDK_C_API const char* ogc_comparison_filter_get_value(const ogc_comparison_filter_t* filter);

/* 8.6 SymbolizerRule */
/* Java: cn.cycle.chart.api.symbology.SymbolizerRule */
/* C++:   ogc::symbology::SymbolizerRule */
/* Header: ogc/symbology/filter/symbolizer_rule.h */

/**
 * @brief Opaque type representing a symbolizer rule.
 */
typedef struct ogc_symbolizer_rule_t ogc_symbolizer_rule_t;

/**
 * @brief Create a symbolizer rule.
 * @return Pointer to newly created rule, or NULL on failure.
 */
SDK_C_API ogc_symbolizer_rule_t* ogc_symbolizer_rule_create(void);

/**
 * @brief Destroy a symbolizer rule.
 * @param rule Pointer to the rule to destroy.
 */
SDK_C_API void ogc_symbolizer_rule_destroy(ogc_symbolizer_rule_t* rule);

/**
 * @brief Get the name of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @return Rule name string.
 */
SDK_C_API const char* ogc_symbolizer_rule_get_name(const ogc_symbolizer_rule_t* rule);

/**
 * @brief Set the name of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param name Rule name.
 */
SDK_C_API void ogc_symbolizer_rule_set_name(ogc_symbolizer_rule_t* rule, const char* name);

/**
 * @brief Get the filter of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @return Pointer to the filter.
 */
SDK_C_API ogc_filter_t* ogc_symbolizer_rule_get_filter(const ogc_symbolizer_rule_t* rule);

/**
 * @brief Set the filter of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param filter Pointer to the filter.
 */
SDK_C_API void ogc_symbolizer_rule_set_filter(ogc_symbolizer_rule_t* rule, ogc_filter_t* filter);

/**
 * @brief Get a symbolizer from a symbolizer rule by index.
 * @param rule Pointer to the rule.
 * @param index Index of the symbolizer.
 * @return Pointer to the symbolizer.
 */
SDK_C_API ogc_symbolizer_t* ogc_symbolizer_rule_get_symbolizer(const ogc_symbolizer_rule_t* rule, size_t index);

/**
 * @brief Set the symbolizer of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param symbolizer Pointer to the symbolizer.
 */
SDK_C_API void ogc_symbolizer_rule_set_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer);

/**
 * @brief Get the minimum scale denominator of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @return Minimum scale denominator.
 */
SDK_C_API double ogc_symbolizer_rule_get_min_scale(const ogc_symbolizer_rule_t* rule);

/**
 * @brief Get the maximum scale denominator of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @return Maximum scale denominator.
 */
SDK_C_API double ogc_symbolizer_rule_get_max_scale(const ogc_symbolizer_rule_t* rule);

/**
 * @brief Set the minimum scale denominator of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param scale Minimum scale denominator.
 */
SDK_C_API void ogc_symbolizer_rule_set_min_scale(ogc_symbolizer_rule_t* rule, double scale);

/**
 * @brief Set the maximum scale denominator of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param scale Maximum scale denominator.
 */
SDK_C_API void ogc_symbolizer_rule_set_max_scale(ogc_symbolizer_rule_t* rule, double scale);

/**
 * @brief Add a symbolizer to a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param symbolizer Pointer to the symbolizer to add.
 */
SDK_C_API void ogc_symbolizer_rule_add_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer);

/**
 * @brief Check if a symbolizer rule is applicable to a feature at a given scale.
 * @param rule Pointer to the rule.
 * @param feature Pointer to the feature.
 * @param scale Current scale denominator.
 * @return Non-zero if applicable, zero otherwise.
 */
SDK_C_API int ogc_symbolizer_rule_is_applicable(const ogc_symbolizer_rule_t* rule, const ogc_feature_t* feature, double scale);

/**
 * @brief Set the scale range of a symbolizer rule.
 * @param rule Pointer to the rule.
 * @param min_scale Minimum scale denominator.
 * @param max_scale Maximum scale denominator.
 */
SDK_C_API void ogc_symbolizer_rule_set_scale_range(ogc_symbolizer_rule_t* rule, double min_scale, double max_scale);

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

/**
 * @brief Alert type enumeration.
 */
typedef enum ogc_alert_type_e {
    OGC_ALERT_TYPE_DEPTH = 0,          /**< Depth alert */
    OGC_ALERT_TYPE_WEATHER = 1,        /**< Weather alert */
    OGC_ALERT_TYPE_COLLISION = 2,      /**< Collision alert */
    OGC_ALERT_TYPE_CHANNEL = 3,        /**< Channel alert */
    OGC_ALERT_TYPE_DEVIATION = 4,      /**< Route deviation alert */
    OGC_ALERT_TYPE_SPEED = 5,          /**< Speed alert */
    OGC_ALERT_TYPE_RESTRICTED_AREA = 6 /**< Restricted area alert */
} ogc_alert_type_e;

/* 9.2 AlertLevel */
/* Java: cn.cycle.chart.api.alert.AlertLevel */
/* C++:   ogc::alert::AlertLevel */
/* Header: ogc/alert/types.h */

/**
 * @brief Alert level enumeration.
 */
typedef enum ogc_alert_level_e {
    OGC_ALERT_LEVEL_INFO = 0,     /**< Informational */
    OGC_ALERT_LEVEL_WARNING = 1,  /**< Warning */
    OGC_ALERT_LEVEL_ALARM = 2,    /**< Alarm */
    OGC_ALERT_LEVEL_CRITICAL = 3  /**< Critical */
} ogc_alert_level_e;

/* 9.3 Alert */
/* Java: cn.cycle.chart.api.alert.Alert */
/* C++:   ogc::alert::Alert */
/* Header: ogc/alert/alert.h */

/**
 * @brief Opaque type representing an alert.
 */
typedef struct ogc_alert_t ogc_alert_t;

/**
 * @brief Create an alert.
 * @param type Alert type.
 * @param level Alert level.
 * @param message Alert message.
 * @return Pointer to newly created alert, or NULL on failure.
 */
SDK_C_API ogc_alert_t* ogc_alert_create(ogc_alert_type_e type, ogc_alert_level_e level, const char* message);

/**
 * @brief Destroy an alert.
 * @param alert Pointer to the alert to destroy.
 */
SDK_C_API void ogc_alert_destroy(ogc_alert_t* alert);

/**
 * @brief Get the type of an alert.
 * @param alert Pointer to the alert.
 * @return Alert type enumeration value.
 */
SDK_C_API ogc_alert_type_e ogc_alert_get_type(const ogc_alert_t* alert);

/**
 * @brief Get the level of an alert.
 * @param alert Pointer to the alert.
 * @return Alert level enumeration value.
 */
SDK_C_API ogc_alert_level_e ogc_alert_get_level(const ogc_alert_t* alert);

/**
 * @brief Get the message of an alert.
 * @param alert Pointer to the alert.
 * @return Alert message string.
 */
SDK_C_API const char* ogc_alert_get_message(const ogc_alert_t* alert);

/**
 * @brief Get the timestamp of an alert.
 * @param alert Pointer to the alert.
 * @return Timestamp in seconds since epoch.
 */
SDK_C_API double ogc_alert_get_timestamp(const ogc_alert_t* alert);

/**
 * @brief Get the position of an alert.
 * @param alert Pointer to the alert.
 * @return Coordinate structure with the position.
 */
SDK_C_API ogc_coordinate_t ogc_alert_get_position(const ogc_alert_t* alert);

/**
 * @brief Set the position of an alert.
 * @param alert Pointer to the alert.
 * @param pos Pointer to the coordinate.
 */
SDK_C_API void ogc_alert_set_position(ogc_alert_t* alert, const ogc_coordinate_t* pos);

/**
 * @brief Alert severity enumeration.
 */
typedef enum ogc_alert_severity_e {
    OGC_ALERT_SEVERITY_LOW = 0,
    OGC_ALERT_SEVERITY_MEDIUM = 1,
    OGC_ALERT_SEVERITY_HIGH = 2,
    OGC_ALERT_SEVERITY_CRITICAL = 3
} ogc_alert_severity_e;

/**
 * @brief Set the ID of an alert.
 * @param alert Pointer to the alert.
 * @param id Alert ID.
 */
SDK_C_API void ogc_alert_set_id(ogc_alert_t* alert, int64_t id);

/**
 * @brief Get the ID of an alert.
 * @param alert Pointer to the alert.
 * @return Alert ID.
 */
SDK_C_API int64_t ogc_alert_get_id(const ogc_alert_t* alert);

/**
 * @brief Set the severity of an alert.
 * @param alert Pointer to the alert.
 * @param severity Alert severity.
 */
SDK_C_API void ogc_alert_set_severity(ogc_alert_t* alert, ogc_alert_severity_e severity);

/**
 * @brief Get the severity of an alert.
 * @param alert Pointer to the alert.
 * @return Alert severity.
 */
SDK_C_API ogc_alert_severity_e ogc_alert_get_severity(const ogc_alert_t* alert);

/**
 * @brief Set the message of an alert.
 * @param alert Pointer to the alert.
 * @param message Alert message.
 */
SDK_C_API void ogc_alert_set_message(ogc_alert_t* alert, const char* message);

/**
 * @brief Set the timestamp of an alert.
 * @param alert Pointer to the alert.
 * @param timestamp Timestamp in seconds since epoch.
 */
SDK_C_API void ogc_alert_set_timestamp(ogc_alert_t* alert, int64_t timestamp);

/**
 * @brief Set whether an alert is acknowledged.
 * @param alert Pointer to the alert.
 * @param acknowledged Non-zero if acknowledged, zero otherwise.
 */
SDK_C_API void ogc_alert_set_acknowledged(ogc_alert_t* alert, int acknowledged);

/**
 * @brief Check if an alert is acknowledged.
 * @param alert Pointer to the alert.
 * @return Non-zero if acknowledged, zero otherwise.
 */
SDK_C_API int ogc_alert_is_acknowledged(const ogc_alert_t* alert);

/* 9.4 AlertEngine */
/* Java: cn.cycle.chart.api.alert.AlertEngine */
/* C++:   ogc::alert::AlertEngine */
/* Header: ogc/alert/alert_engine.h */

/**
 * @brief Opaque type representing an alert engine.
 */
typedef struct ogc_alert_engine_t ogc_alert_engine_t;

/**
 * @brief Create an alert engine.
 * @return Pointer to newly created engine, or NULL on failure.
 */
SDK_C_API ogc_alert_engine_t* ogc_alert_engine_create(void);

/**
 * @brief Destroy an alert engine.
 * @param engine Pointer to the engine to destroy.
 */
SDK_C_API void ogc_alert_engine_destroy(ogc_alert_engine_t* engine);

/**
 * @brief Initialize an alert engine.
 * @param engine Pointer to the engine.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_alert_engine_initialize(ogc_alert_engine_t* engine);

/**
 * @brief Shutdown an alert engine.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_alert_engine_shutdown(ogc_alert_engine_t* engine);

/**
 * @brief Check all alert conditions.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_alert_engine_check_all(ogc_alert_engine_t* engine);

/**
 * @brief Get all active alerts.
 * @param engine Pointer to the engine.
 * @param count Pointer to store the number of alerts.
 * @return Array of alert pointers (caller must free with ogc_alert_engine_free_alerts).
 */
ogc_alert_t** ogc_alert_engine_get_active_alerts(ogc_alert_engine_t* engine, int* count);

/**
 * @brief Free an array of alerts returned by ogc_alert_engine_get_active_alerts.
 * @param alerts Array of alert pointers to free.
 */
SDK_C_API void ogc_alert_engine_free_alerts(ogc_alert_t** alerts);

/**
 * @brief Acknowledge an alert.
 * @param engine Pointer to the engine.
 * @param alert Pointer to the alert to acknowledge.
 */
SDK_C_API void ogc_alert_engine_acknowledge_alert(ogc_alert_engine_t* engine, const ogc_alert_t* alert);

/**
 * @brief Add an alert to the engine.
 * @param engine Pointer to the engine.
 * @param alert Pointer to the alert to add.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_alert_engine_add_alert(ogc_alert_engine_t* engine, ogc_alert_t* alert);

/**
 * @brief Remove an alert from the engine.
 * @param engine Pointer to the engine.
 * @param id ID of the alert to remove.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_alert_engine_remove_alert(ogc_alert_engine_t* engine, int64_t id);

/**
 * @brief Get an alert from the engine by ID.
 * @param engine Pointer to the engine.
 * @param id Alert ID.
 * @return Pointer to the alert, or NULL if not found.
 */
SDK_C_API ogc_alert_t* ogc_alert_engine_get_alert(ogc_alert_engine_t* engine, int64_t id);

/**
 * @brief Get the number of active alerts.
 * @param engine Pointer to the engine.
 * @return Number of active alerts.
 */
SDK_C_API size_t ogc_alert_engine_get_alert_count(const ogc_alert_engine_t* engine);

/**
 * @brief Clear all active alerts from the engine.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_alert_engine_clear_alerts(ogc_alert_engine_t* engine);

/**
 * @brief Set a callback for alert events.
 * @param engine Pointer to the engine.
 * @param callback Function pointer called when an alert is triggered.
 * @param user_data User data passed to the callback.
 */
SDK_C_API void ogc_alert_engine_set_callback(ogc_alert_engine_t* engine,
    void (*callback)(int alert_type, int alert_level, const char* message, void* user_data),
    void* user_data);

/* 9.5 CpaCalculator */
/* Java: cn.cycle.chart.api.navi.CpaCalculator */
/* C++:   ogc::alert::CpaCalculator */
/* Header: ogc/alert/cpa_calculator.h */

/**
 * @brief CPA (Closest Point of Approach) calculation result.
 */
typedef struct ogc_cpa_result_t {
    double cpa;         /**< Closest Point of Approach distance (nautical miles) */
    double tcpa;        /**< Time to CPA (minutes) */
    double bearing;     /**< Bearing to target (degrees) */
    int danger_level;   /**< Danger level (0=safe, 1=caution, 2=danger) */
} ogc_cpa_result_t;

/**
 * @brief Calculate CPA between two vessels.
 * @param own_lat Own vessel latitude (degrees).
 * @param own_lon Own vessel longitude (degrees).
 * @param own_speed Own vessel speed (knots).
 * @param own_course Own vessel course (degrees).
 * @param target_lat Target vessel latitude (degrees).
 * @param target_lon Target vessel longitude (degrees).
 * @param target_speed Target vessel speed (knots).
 * @param target_course Target vessel course (degrees).
 * @param result Pointer to store the calculation result.
 */
SDK_C_API void ogc_cpa_calculate(double own_lat, double own_lon, double own_speed, double own_course,
                       double target_lat, double target_lon, double target_speed, double target_course,
                       ogc_cpa_result_t* result);

/* 9.6 UkcCalculator */
/* Java: cn.cycle.chart.api.navi.UkcCalculator */
/* C++:   ogc::alert::UkcCalculator */
/* Header: ogc/alert/ukc_calculator.h */

/**
 * @brief UKC (Under Keel Clearance) calculation result.
 */
typedef struct ogc_ukc_result_t {
    double water_depth;     /**< Actual water depth (meters) */
    double charted_depth;   /**< Charted depth (meters) */
    double tide_height;     /**< Tide height (meters) */
    double squat;           /**< Squat value (meters) */
    double ukc;             /**< Under Keel Clearance (meters) */
    int safe;               /**< Non-zero if safe, zero if unsafe */
} ogc_ukc_result_t;

/**
 * @brief Calculate Under Keel Clearance.
 * @param charted_depth Charted depth (meters).
 * @param tide_height Tide height (meters).
 * @param draft Vessel draft (meters).
 * @param speed Vessel speed (knots).
 * @param squat_coeff Squat coefficient.
 * @param result Pointer to store the calculation result.
 */
SDK_C_API void ogc_ukc_calculate(double charted_depth, double tide_height, double draft, 
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

/**
 * @brief Route status enumeration.
 */
typedef enum ogc_route_status_e {
    OGC_ROUTE_STATUS_PLANNING = 0,   /**< Route is being planned */
    OGC_ROUTE_STATUS_ACTIVE = 1,     /**< Route is active */
    OGC_ROUTE_STATUS_COMPLETED = 2,  /**< Route is completed */
    OGC_ROUTE_STATUS_CANCELLED = 3   /**< Route is cancelled */
} ogc_route_status_e;

/* 10.2 NavigationStatus */
/* Java: cn.cycle.chart.api.navi.NavigationStatus */
/* C++:   ogc::navi::NavigationStatus */
/* Header: ogc/navi/types.h */

/**
 * @brief Navigation status enumeration.
 */
typedef enum ogc_navigation_status_e {
    OGC_NAV_STATUS_STANDBY = 0,     /**< Navigation is on standby */
    OGC_NAV_STATUS_NAVIGATING = 1,  /**< Navigation is active */
    OGC_NAV_STATUS_PAUSED = 2,      /**< Navigation is paused */
    OGC_NAV_STATUS_ARRIVED = 3      /**< Arrived at destination */
} ogc_navigation_status_e;

/* 10.3 Waypoint */
/* Java: cn.cycle.chart.api.navi.Waypoint */
/* C++:   ogc::navi::Waypoint */
/* Header: ogc/navi/route/waypoint.h */

/**
 * @brief Opaque type representing a waypoint.
 */
typedef struct ogc_waypoint_t ogc_waypoint_t;

/**
 * @brief Create a waypoint.
 * @param lat Latitude in degrees.
 * @param lon Longitude in degrees.
 * @return Pointer to newly created waypoint, or NULL on failure.
 */
SDK_C_API ogc_waypoint_t* ogc_waypoint_create(double lat, double lon);

/**
 * @brief Destroy a waypoint.
 * @param wp Pointer to the waypoint to destroy.
 */
SDK_C_API void ogc_waypoint_destroy(ogc_waypoint_t* wp);

/**
 * @brief Get the latitude of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Latitude in degrees.
 */
SDK_C_API double ogc_waypoint_get_latitude(const ogc_waypoint_t* wp);

/**
 * @brief Get the longitude of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Longitude in degrees.
 */
SDK_C_API double ogc_waypoint_get_longitude(const ogc_waypoint_t* wp);

/**
 * @brief Get the name of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Waypoint name string.
 */
SDK_C_API const char* ogc_waypoint_get_name(const ogc_waypoint_t* wp);

/**
 * @brief Set the name of a waypoint.
 * @param wp Pointer to the waypoint.
 * @param name Waypoint name.
 */
SDK_C_API void ogc_waypoint_set_name(ogc_waypoint_t* wp, const char* name);

/**
 * @brief Check if the waypoint is an arrival waypoint.
 * @param wp Pointer to the waypoint.
 * @return Non-zero if arrival waypoint, zero otherwise.
 */
SDK_C_API int ogc_waypoint_is_arrival(const ogc_waypoint_t* wp);

/**
 * @brief Set the arrival radius of a waypoint.
 * @param wp Pointer to the waypoint.
 * @param radius Arrival radius in nautical miles.
 */
SDK_C_API void ogc_waypoint_set_arrival_radius(ogc_waypoint_t* wp, double radius);

/**
 * @brief Get the arrival radius of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Arrival radius in nautical miles.
 */
SDK_C_API double ogc_waypoint_get_arrival_radius(const ogc_waypoint_t* wp);

/**
 * @brief Set the turn radius of a waypoint.
 * @param wp Pointer to the waypoint.
 * @param radius Turn radius in nautical miles.
 */
SDK_C_API void ogc_waypoint_set_turn_radius(ogc_waypoint_t* wp, double radius);

/**
 * @brief Get the turn radius of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Turn radius in nautical miles.
 */
SDK_C_API double ogc_waypoint_get_turn_radius(const ogc_waypoint_t* wp);

/**
 * @brief Waypoint type enumeration.
 */
typedef enum ogc_waypoint_type_e {
    OGC_WAYPOINT_TYPE_NORMAL = 0,
    OGC_WAYPOINT_TYPE_TURN = 1,
    OGC_WAYPOINT_TYPE_ARRIVAL = 2,
    OGC_WAYPOINT_TYPE_DEPARTURE = 3
} ogc_waypoint_type_e;

/**
 * @brief Set the type of a waypoint.
 * @param wp Pointer to the waypoint.
 * @param type Waypoint type.
 */
SDK_C_API void ogc_waypoint_set_type(ogc_waypoint_t* wp, ogc_waypoint_type_e type);

/**
 * @brief Get the type of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Waypoint type.
 */
SDK_C_API ogc_waypoint_type_e ogc_waypoint_get_type(const ogc_waypoint_t* wp);

/**
 * @brief Set the description of a waypoint.
 * @param wp Pointer to the waypoint.
 * @param description Waypoint description.
 */
SDK_C_API void ogc_waypoint_set_description(ogc_waypoint_t* wp, const char* description);

/**
 * @brief Get the description of a waypoint.
 * @param wp Pointer to the waypoint.
 * @return Waypoint description string.
 */
SDK_C_API const char* ogc_waypoint_get_description(const ogc_waypoint_t* wp);

/* 10.4 Route */
/* Java: cn.cycle.chart.api.navi.Route */
/* C++:   ogc::navi::Route */
/* Header: ogc/navi/route/route.h */

/**
 * @brief Opaque type representing a route.
 */
typedef struct ogc_route_t ogc_route_t;

/**
 * @brief Create a route.
 * @return Pointer to newly created route, or NULL on failure.
 */
SDK_C_API ogc_route_t* ogc_route_create(void);

/**
 * @brief Destroy a route.
 * @param route Pointer to the route to destroy.
 */
SDK_C_API void ogc_route_destroy(ogc_route_t* route);

/**
 * @brief Get the ID of a route.
 * @param route Pointer to the route.
 * @return Route ID string.
 */
SDK_C_API const char* ogc_route_get_id(const ogc_route_t* route);

/**
 * @brief Get the name of a route.
 * @param route Pointer to the route.
 * @return Route name string.
 */
SDK_C_API const char* ogc_route_get_name(const ogc_route_t* route);

/**
 * @brief Set the name of a route.
 * @param route Pointer to the route.
 * @param name Route name.
 */
SDK_C_API void ogc_route_set_name(ogc_route_t* route, const char* name);

/**
 * @brief Get the status of a route.
 * @param route Pointer to the route.
 * @return Route status enumeration value.
 */
SDK_C_API ogc_route_status_e ogc_route_get_status(const ogc_route_t* route);

/**
 * @brief Get the total distance of a route.
 * @param route Pointer to the route.
 * @return Total distance in nautical miles.
 */
SDK_C_API double ogc_route_get_total_distance(const ogc_route_t* route);

/**
 * @brief Get the number of waypoints in a route.
 * @param route Pointer to the route.
 * @return Number of waypoints.
 */
SDK_C_API size_t ogc_route_get_waypoint_count(const ogc_route_t* route);

/**
 * @brief Get a waypoint from a route by index.
 * @param route Pointer to the route.
 * @param index Waypoint index (0-based).
 * @return Pointer to the waypoint.
 */
SDK_C_API ogc_waypoint_t* ogc_route_get_waypoint(const ogc_route_t* route, size_t index);

/**
 * @brief Add a waypoint to a route.
 * @param route Pointer to the route.
 * @param wp Pointer to the waypoint to add.
 */
SDK_C_API void ogc_route_add_waypoint(ogc_route_t* route, ogc_waypoint_t* wp);

/**
 * @brief Remove a waypoint from a route.
 * @param route Pointer to the route.
 * @param index Waypoint index to remove.
 */
SDK_C_API void ogc_route_remove_waypoint(ogc_route_t* route, size_t index);

/**
 * @brief Get the current waypoint of a route.
 * @param route Pointer to the route.
 * @return Pointer to the current waypoint.
 */
SDK_C_API ogc_waypoint_t* ogc_route_get_current_waypoint(const ogc_route_t* route);

/**
 * @brief Advance to the next waypoint in a route.
 * @param route Pointer to the route.
 * @return 0 on success, non-zero if no more waypoints.
 */
SDK_C_API int ogc_route_advance_to_next_waypoint(ogc_route_t* route);

/**
 * @brief Insert a waypoint at a specific index.
 * @param route Pointer to the route.
 * @param index Index to insert at.
 * @param wp Pointer to the waypoint to insert.
 */
SDK_C_API void ogc_route_insert_waypoint(ogc_route_t* route, size_t index, ogc_waypoint_t* wp);

/**
 * @brief Clear all waypoints from a route.
 * @param route Pointer to the route.
 */
SDK_C_API void ogc_route_clear(ogc_route_t* route);

/**
 * @brief Reverse the order of waypoints in a route.
 * @param route Pointer to the route.
 */
SDK_C_API void ogc_route_reverse(ogc_route_t* route);

/**
 * @brief Get the ETA (estimated time of arrival) for a route.
 * @param route Pointer to the route.
 * @return ETA in seconds from now, or -1 if not available.
 */
SDK_C_API int64_t ogc_route_get_eta(const ogc_route_t* route);

/* 10.5 RouteManager */
/* Java: cn.cycle.chart.api.navi.RouteManager */
/* C++:   ogc::navi::RouteManager */
/* Header: ogc/navi/route/route_manager.h */

/**
 * @brief Opaque type representing a route manager.
 */
typedef struct ogc_route_manager_t ogc_route_manager_t;

/**
 * @brief Create a route manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_route_manager_t* ogc_route_manager_create(void);

/**
 * @brief Destroy a route manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_route_manager_destroy(ogc_route_manager_t* mgr);

/**
 * @brief Get the number of routes in a manager.
 * @param mgr Pointer to the manager.
 * @return Number of routes.
 */
SDK_C_API size_t ogc_route_manager_get_route_count(const ogc_route_manager_t* mgr);

/**
 * @brief Get a route from a manager by index.
 * @param mgr Pointer to the manager.
 * @param index Route index (0-based).
 * @return Pointer to the route.
 */
SDK_C_API ogc_route_t* ogc_route_manager_get_route(const ogc_route_manager_t* mgr, size_t index);

/**
 * @brief Get a route from a manager by ID.
 * @param mgr Pointer to the manager.
 * @param id Route ID string.
 * @return Pointer to the route, or NULL if not found.
 */
SDK_C_API ogc_route_t* ogc_route_manager_get_route_by_id(const ogc_route_manager_t* mgr, const char* id);

/**
 * @brief Add a route to a manager.
 * @param mgr Pointer to the manager.
 * @param route Pointer to the route to add.
 */
SDK_C_API void ogc_route_manager_add_route(ogc_route_manager_t* mgr, ogc_route_t* route);

/**
 * @brief Remove a route from a manager.
 * @param mgr Pointer to the manager.
 * @param id Route ID string to remove.
 */
SDK_C_API void ogc_route_manager_remove_route(ogc_route_manager_t* mgr, const char* id);

/**
 * @brief Get the active route from a manager.
 * @param mgr Pointer to the manager.
 * @return Pointer to the active route, or NULL if none.
 */
SDK_C_API ogc_route_t* ogc_route_manager_get_active_route(const ogc_route_manager_t* mgr);

/**
 * @brief Set the active route in a manager.
 * @param mgr Pointer to the manager.
 * @param id Route ID string to set as active.
 */
SDK_C_API void ogc_route_manager_set_active_route(ogc_route_manager_t* mgr, const char* id);

/* 10.6 RoutePlanner */
/* Java: cn.cycle.chart.api.navi.RoutePlanner */
/* C++:   ogc::navi::RoutePlanner */
/* Header: ogc/navi/route/route_planner.h */

/**
 * @brief Opaque type representing a route planner.
 */
typedef struct ogc_route_planner_t ogc_route_planner_t;

/**
 * @brief Create a route planner.
 * @return Pointer to newly created planner, or NULL on failure.
 */
SDK_C_API ogc_route_planner_t* ogc_route_planner_create(void);

/**
 * @brief Destroy a route planner.
 * @param planner Pointer to the planner to destroy.
 */
SDK_C_API void ogc_route_planner_destroy(ogc_route_planner_t* planner);

/**
 * @brief Plan a route between two waypoints.
 * @param planner Pointer to the planner.
 * @param start Starting waypoint.
 * @param end Ending waypoint.
 * @param avoid_areas Array of envelopes to avoid.
 * @param avoid_count Number of avoid areas.
 * @return Pointer to newly created route, or NULL on failure.
 */
SDK_C_API ogc_route_t* ogc_route_planner_plan_route(ogc_route_planner_t* planner, 
                                           const ogc_waypoint_t* start, 
                                           const ogc_waypoint_t* end,
                                           const ogc_envelope_t* avoid_areas,
                                           int avoid_count);

/* 10.7 AisTarget */
/* Java: cn.cycle.chart.api.navi.AisTarget */
/* C++:   ogc::navi::AisTarget */
/* Header: ogc/navi/ais/ais_target.h */

/**
 * @brief Opaque type representing an AIS target.
 */
typedef struct ogc_ais_target_t ogc_ais_target_t;

/**
 * @brief Create an AIS target.
 * @param mmsi MMSI number of the target.
 * @return Pointer to newly created target, or NULL on failure.
 */
SDK_C_API ogc_ais_target_t* ogc_ais_target_create(uint32_t mmsi);

/**
 * @brief Destroy an AIS target.
 * @param target Pointer to the target to destroy.
 */
SDK_C_API void ogc_ais_target_destroy(ogc_ais_target_t* target);

/**
 * @brief Get the MMSI of an AIS target.
 * @param target Pointer to the target.
 * @return MMSI number.
 */
SDK_C_API uint32_t ogc_ais_target_get_mmsi(const ogc_ais_target_t* target);

/**
 * @brief Get the name of an AIS target.
 * @param target Pointer to the target.
 * @param buffer Buffer to store the name string.
 * @param buffer_size Size of the buffer in bytes.
 * @return Length of the name string, or -1 on error.
 */
SDK_C_API int ogc_ais_target_get_name(const ogc_ais_target_t* target, char* buffer, size_t buffer_size);

/**
 * @brief Get the latitude of an AIS target.
 * @param target Pointer to the target.
 * @return Latitude in degrees.
 */
SDK_C_API double ogc_ais_target_get_latitude(const ogc_ais_target_t* target);

/**
 * @brief Get the longitude of an AIS target.
 * @param target Pointer to the target.
 * @return Longitude in degrees.
 */
SDK_C_API double ogc_ais_target_get_longitude(const ogc_ais_target_t* target);

/**
 * @brief Get the speed of an AIS target.
 * @param target Pointer to the target.
 * @return Speed in knots.
 */
SDK_C_API double ogc_ais_target_get_speed(const ogc_ais_target_t* target);

/**
 * @brief Get the course of an AIS target.
 * @param target Pointer to the target.
 * @return Course in degrees.
 */
SDK_C_API double ogc_ais_target_get_course(const ogc_ais_target_t* target);

/**
 * @brief Get the heading of an AIS target.
 * @param target Pointer to the target.
 * @return Heading in degrees.
 */
SDK_C_API double ogc_ais_target_get_heading(const ogc_ais_target_t* target);

/**
 * @brief Get the navigation status of an AIS target.
 * @param target Pointer to the target.
 * @return Navigation status code.
 */
SDK_C_API int ogc_ais_target_get_nav_status(const ogc_ais_target_t* target);

/**
 * @brief Update the position of an AIS target.
 * @param target Pointer to the target.
 * @param lat Latitude in degrees.
 * @param lon Longitude in degrees.
 * @param speed Speed in knots.
 * @param course Course in degrees.
 */
SDK_C_API void ogc_ais_target_update_position(ogc_ais_target_t* target, double lat, double lon, double speed, double course);

/* 10.8 AisManager */
/* Java: cn.cycle.chart.api.navi.AisManager */
/* C++:   ogc::navi::AisManager */
/* Header: ogc/navi/ais/ais_manager.h */

/**
 * @brief Opaque type representing an AIS manager.
 */
typedef struct ogc_ais_manager_t ogc_ais_manager_t;

/**
 * @brief Create an AIS manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_ais_manager_t* ogc_ais_manager_create(void);

/**
 * @brief Destroy an AIS manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_ais_manager_destroy(ogc_ais_manager_t* mgr);

/**
 * @brief Initialize an AIS manager.
 * @param mgr Pointer to the manager.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_ais_manager_initialize(ogc_ais_manager_t* mgr);

/**
 * @brief Shutdown an AIS manager.
 * @param mgr Pointer to the manager.
 */
SDK_C_API void ogc_ais_manager_shutdown(ogc_ais_manager_t* mgr);

/**
 * @brief Get an AIS target by MMSI.
 * @param mgr Pointer to the manager.
 * @param mmsi MMSI number of the target.
 * @return Pointer to the target, or NULL if not found.
 */
SDK_C_API ogc_ais_target_t* ogc_ais_manager_get_target(ogc_ais_manager_t* mgr, uint32_t mmsi);

/**
 * @brief Get all AIS targets.
 * @param mgr Pointer to the manager.
 * @param count Pointer to store the number of targets.
 * @return Array of target pointers (caller must free with ogc_ais_manager_free_targets).
 */
ogc_ais_target_t** ogc_ais_manager_get_all_targets(ogc_ais_manager_t* mgr, int* count);

/**
 * @brief Free an array of targets returned by ogc_ais_manager_get_all_targets.
 * @param targets Array of target pointers to free.
 */
SDK_C_API void ogc_ais_manager_free_targets(ogc_ais_target_t** targets);

/**
 * @brief Get AIS targets within a range.
 * @param mgr Pointer to the manager.
 * @param lat Center latitude in degrees.
 * @param lon Center longitude in degrees.
 * @param range_nm Range in nautical miles.
 * @param count Pointer to store the number of targets.
 * @return Array of target pointers (caller must free with ogc_ais_manager_free_targets).
 */
ogc_ais_target_t** ogc_ais_manager_get_targets_in_range(ogc_ais_manager_t* mgr, double lat, double lon, double range_nm, int* count);

/**
 * @brief Get the number of AIS targets.
 * @param mgr Pointer to the manager.
 * @return Number of targets.
 */
SDK_C_API int ogc_ais_manager_get_target_count(ogc_ais_manager_t* mgr);

/**
 * @brief Add an AIS target to the manager.
 * @param mgr Pointer to the manager.
 * @param target Pointer to the target to add.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_ais_manager_add_target(ogc_ais_manager_t* mgr, ogc_ais_target_t* target);

/**
 * @brief Remove an AIS target from the manager.
 * @param mgr Pointer to the manager.
 * @param mmsi MMSI number of the target to remove.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_ais_manager_remove_target(ogc_ais_manager_t* mgr, uint32_t mmsi);

/**
 * @brief Set a callback for AIS target updates.
 * @param mgr Pointer to the manager.
 * @param callback Function pointer called when a target is updated.
 * @param user_data User data passed to the callback.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_ais_manager_set_callback(ogc_ais_manager_t* mgr,
    void (*callback)(uint32_t mmsi, int event_type, void* user_data),
    void* user_data);

/* 10.9 NavigationEngine */
/* Java: cn.cycle.chart.api.navi.NavigationEngine */
/* C++:   ogc::navi::NavigationEngine */
/* Header: ogc/navi/navigation/navigation_engine.h */

/**
 * @brief Opaque type representing a navigation engine.
 */
typedef struct ogc_navigation_engine_t ogc_navigation_engine_t;

/**
 * @brief Create a navigation engine.
 * @return Pointer to newly created engine, or NULL on failure.
 */
SDK_C_API ogc_navigation_engine_t* ogc_navigation_engine_create(void);

/**
 * @brief Destroy a navigation engine.
 * @param engine Pointer to the engine to destroy.
 */
SDK_C_API void ogc_navigation_engine_destroy(ogc_navigation_engine_t* engine);

/**
 * @brief Initialize a navigation engine.
 * @param engine Pointer to the engine.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_navigation_engine_initialize(ogc_navigation_engine_t* engine);

/**
 * @brief Shutdown a navigation engine.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_navigation_engine_shutdown(ogc_navigation_engine_t* engine);

/**
 * @brief Set the active route for navigation.
 * @param engine Pointer to the engine.
 * @param route Pointer to the route.
 */
SDK_C_API void ogc_navigation_engine_set_route(ogc_navigation_engine_t* engine, ogc_route_t* route);

/**
 * @brief Get the active route.
 * @param engine Pointer to the engine.
 * @return Pointer to the active route.
 */
SDK_C_API ogc_route_t* ogc_navigation_engine_get_route(const ogc_navigation_engine_t* engine);

/**
 * @brief Start navigation.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_navigation_engine_start(ogc_navigation_engine_t* engine);

/**
 * @brief Stop navigation.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_navigation_engine_stop(ogc_navigation_engine_t* engine);

/**
 * @brief Pause navigation.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_navigation_engine_pause(ogc_navigation_engine_t* engine);

/**
 * @brief Resume navigation.
 * @param engine Pointer to the engine.
 */
SDK_C_API void ogc_navigation_engine_resume(ogc_navigation_engine_t* engine);

/**
 * @brief Get the navigation status.
 * @param engine Pointer to the engine.
 * @return Navigation status enumeration value.
 */
SDK_C_API ogc_navigation_status_e ogc_navigation_engine_get_status(const ogc_navigation_engine_t* engine);

/**
 * @brief Get the current waypoint.
 * @param engine Pointer to the engine.
 * @return Pointer to the current waypoint.
 */
SDK_C_API ogc_waypoint_t* ogc_navigation_engine_get_current_waypoint(const ogc_navigation_engine_t* engine);

/**
 * @brief Get the distance to the current waypoint.
 * @param engine Pointer to the engine.
 * @return Distance in nautical miles.
 */
SDK_C_API double ogc_navigation_engine_get_distance_to_waypoint(const ogc_navigation_engine_t* engine);

/**
 * @brief Get the bearing to the current waypoint.
 * @param engine Pointer to the engine.
 * @return Bearing in degrees.
 */
SDK_C_API double ogc_navigation_engine_get_bearing_to_waypoint(const ogc_navigation_engine_t* engine);

/**
 * @brief Get the cross-track error.
 * @param engine Pointer to the engine.
 * @return Cross-track error in nautical miles.
 */
SDK_C_API double ogc_navigation_engine_get_cross_track_error(const ogc_navigation_engine_t* engine);

/* 10.10 PositionManager */
/* Java: cn.cycle.chart.api.navi.PositionManager */
/* C++:   ogc::navi::PositionManager */
/* Header: ogc/navi/positioning/position_manager.h */

/**
 * @brief Opaque type representing a position manager.
 */
typedef struct ogc_position_manager_t ogc_position_manager_t;

/**
 * @brief Create a position manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_position_manager_t* ogc_position_manager_create(void);

/**
 * @brief Destroy a position manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_position_manager_destroy(ogc_position_manager_t* mgr);

/**
 * @brief Initialize a position manager.
 * @param mgr Pointer to the manager.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_position_manager_initialize(ogc_position_manager_t* mgr);

/**
 * @brief Shutdown a position manager.
 * @param mgr Pointer to the manager.
 */
SDK_C_API void ogc_position_manager_shutdown(ogc_position_manager_t* mgr);

/**
 * @brief Get the current latitude.
 * @param mgr Pointer to the manager.
 * @return Latitude in degrees.
 */
SDK_C_API double ogc_position_manager_get_latitude(const ogc_position_manager_t* mgr);

/**
 * @brief Get the current longitude.
 * @param mgr Pointer to the manager.
 * @return Longitude in degrees.
 */
SDK_C_API double ogc_position_manager_get_longitude(const ogc_position_manager_t* mgr);

/**
 * @brief Get the current speed.
 * @param mgr Pointer to the manager.
 * @return Speed in knots.
 */
SDK_C_API double ogc_position_manager_get_speed(const ogc_position_manager_t* mgr);

/**
 * @brief Get the current course.
 * @param mgr Pointer to the manager.
 * @return Course in degrees.
 */
SDK_C_API double ogc_position_manager_get_course(const ogc_position_manager_t* mgr);

/**
 * @brief Get the current heading.
 * @param mgr Pointer to the manager.
 * @return Heading in degrees.
 */
SDK_C_API double ogc_position_manager_get_heading(const ogc_position_manager_t* mgr);

/**
 * @brief Get the GPS fix quality.
 * @param mgr Pointer to the manager.
 * @return Fix quality (0=no fix, 1=GPS, 2=DGPS, etc.).
 */
SDK_C_API int ogc_position_manager_get_fix_quality(const ogc_position_manager_t* mgr);

/**
 * @brief Get the number of satellites in use.
 * @param mgr Pointer to the manager.
 * @return Number of satellites.
 */
SDK_C_API int ogc_position_manager_get_satellite_count(const ogc_position_manager_t* mgr);

/**
 * @brief Set the current position manually.
 * @param mgr Pointer to the manager.
 * @param latitude Latitude in degrees.
 * @param longitude Longitude in degrees.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_position_manager_set_position(ogc_position_manager_t* mgr, double latitude, double longitude);

/**
 * @brief Get the current position.
 * @param mgr Pointer to the manager.
 * @param latitude Output latitude in degrees.
 * @param longitude Output longitude in degrees.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_position_manager_get_position(const ogc_position_manager_t* mgr, double* latitude, double* longitude);

/**
 * @brief Set a callback for position updates.
 * @param mgr Pointer to the manager.
 * @param callback Function pointer called on position updates.
 * @param user_data User data passed to the callback.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_position_manager_set_callback(ogc_position_manager_t* mgr,
    void (*callback)(double lat, double lon, double speed, double course, void* user_data),
    void* user_data);

/* 10.11 Track */
/* Java: cn.cycle.chart.api.navi.Track */
/* C++:   ogc::navi::Track */
/* Header: ogc/navi/track/track.h */

/**
 * @brief Opaque type representing a track.
 */
typedef struct ogc_track_t ogc_track_t;

/**
 * @brief Opaque type representing a track point.
 */
typedef struct ogc_track_point_t ogc_track_point_t;

/**
 * @brief Create a track.
 * @return Pointer to newly created track, or NULL on failure.
 */
SDK_C_API ogc_track_t* ogc_track_create(void);

/**
 * @brief Destroy a track.
 * @param track Pointer to the track to destroy.
 */
SDK_C_API void ogc_track_destroy(ogc_track_t* track);

/**
 * @brief Get the ID of a track.
 * @param track Pointer to the track.
 * @param buffer Buffer to store the ID string.
 * @param buffer_size Size of the buffer in bytes.
 * @return Length of the ID string, or -1 on error.
 */
SDK_C_API int ogc_track_get_id(const ogc_track_t* track, char* buffer, size_t buffer_size);

/**
 * @brief Get the name of a track.
 * @param track Pointer to the track.
 * @param buffer Buffer to store the name string.
 * @param buffer_size Size of the buffer in bytes.
 * @return Length of the name string, or -1 on error.
 */
SDK_C_API int ogc_track_get_name(const ogc_track_t* track, char* buffer, size_t buffer_size);

/**
 * @brief Set the name of a track.
 * @param track Pointer to the track.
 * @param name Track name.
 */
SDK_C_API void ogc_track_set_name(ogc_track_t* track, const char* name);

/**
 * @brief Get the number of points in a track.
 * @param track Pointer to the track.
 * @return Number of points.
 */
SDK_C_API size_t ogc_track_get_point_count(const ogc_track_t* track);

/**
 * @brief Get a point from a track by index.
 * @param track Pointer to the track.
 * @param index Point index (0-based).
 * @return Pointer to the track point.
 */
SDK_C_API ogc_track_point_t* ogc_track_get_point(const ogc_track_t* track, size_t index);

/**
 * @brief Add a point to a track.
 * @param track Pointer to the track.
 * @param lat Latitude in degrees.
 * @param lon Longitude in degrees.
 * @param speed Speed in knots.
 * @param course Course in degrees.
 * @param timestamp Timestamp in seconds since epoch.
 */
SDK_C_API void ogc_track_add_point(ogc_track_t* track, double lat, double lon, double speed, double course, double timestamp);

/**
 * @brief Clear all points from a track.
 * @param track Pointer to the track.
 */
SDK_C_API void ogc_track_clear(ogc_track_t* track);

/**
 * @brief Convert a track to a LineString geometry.
 * @param track Pointer to the track.
 * @return Pointer to newly created LineString, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_track_to_linestring(const ogc_track_t* track);

/* 10.12 TrackRecorder */
/* Java: cn.cycle.chart.api.navi.TrackRecorder */
/* C++:   ogc::navi::TrackRecorder */
/* Header: ogc/navi/track/track_recorder.h */

/**
 * @brief Opaque type representing a track recorder.
 */
typedef struct ogc_track_recorder_t ogc_track_recorder_t;

/**
 * @brief Create a track recorder.
 * @return Pointer to newly created recorder, or NULL on failure.
 */
SDK_C_API ogc_track_recorder_t* ogc_track_recorder_create(void);

/**
 * @brief Destroy a track recorder.
 * @param recorder Pointer to the recorder to destroy.
 */
SDK_C_API void ogc_track_recorder_destroy(ogc_track_recorder_t* recorder);

/**
 * @brief Start recording a track.
 * @param recorder Pointer to the recorder.
 * @param track_name Name for the new track.
 */
SDK_C_API void ogc_track_recorder_start(ogc_track_recorder_t* recorder, const char* track_name);

/**
 * @brief Stop recording a track.
 * @param recorder Pointer to the recorder.
 */
SDK_C_API void ogc_track_recorder_stop(ogc_track_recorder_t* recorder);

/**
 * @brief Pause recording a track.
 * @param recorder Pointer to the recorder.
 */
SDK_C_API void ogc_track_recorder_pause(ogc_track_recorder_t* recorder);

/**
 * @brief Resume recording a track.
 * @param recorder Pointer to the recorder.
 */
SDK_C_API void ogc_track_recorder_resume(ogc_track_recorder_t* recorder);

/**
 * @brief Check if recording is active.
 * @param recorder Pointer to the recorder.
 * @return Non-zero if recording, zero otherwise.
 */
SDK_C_API int ogc_track_recorder_is_recording(const ogc_track_recorder_t* recorder);

/**
 * @brief Get the current track being recorded.
 * @param recorder Pointer to the recorder.
 * @return Pointer to the current track.
 */
SDK_C_API ogc_track_t* ogc_track_recorder_get_current_track(const ogc_track_recorder_t* recorder);

/**
 * @brief Set the recording interval.
 * @param recorder Pointer to the recorder.
 * @param seconds Interval in seconds.
 */
SDK_C_API void ogc_track_recorder_set_interval(ogc_track_recorder_t* recorder, int seconds);

/**
 * @brief Set the minimum distance between recorded points.
 * @param recorder Pointer to the recorder.
 * @param meters Minimum distance in meters.
 */
SDK_C_API void ogc_track_recorder_set_min_distance(ogc_track_recorder_t* recorder, double meters);

/* 10.13 IPositionProvider */
/* Java: cn.cycle.chart.api.navi.IPositionProvider */
/* C++:   ogc::navi::IPositionProvider */
/* Header: ogc/navi/positioning/position_provider.h */

/**
 * @brief Opaque type representing a position provider.
 */
typedef struct ogc_position_provider_t ogc_position_provider_t;

/**
 * @brief Create a serial port position provider.
 * @param port Serial port name (e.g., "COM1" or "/dev/ttyUSB0").
 * @param baud_rate Baud rate (e.g., 4800, 9600).
 * @return Pointer to newly created provider, or NULL on failure.
 */
SDK_C_API ogc_position_provider_t* ogc_position_provider_create_serial(const char* port, int baud_rate);

/**
 * @brief Create a network position provider.
 * @param host Host name or IP address.
 * @param port Port number.
 * @return Pointer to newly created provider, or NULL on failure.
 */
SDK_C_API ogc_position_provider_t* ogc_position_provider_create_network(const char* host, int port);

/**
 * @brief Destroy a position provider.
 * @param provider Pointer to the provider to destroy.
 */
SDK_C_API void ogc_position_provider_destroy(ogc_position_provider_t* provider);

/**
 * @brief Connect a position provider.
 * @param provider Pointer to the provider.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_position_provider_connect(ogc_position_provider_t* provider);

/**
 * @brief Disconnect a position provider.
 * @param provider Pointer to the provider.
 */
SDK_C_API void ogc_position_provider_disconnect(ogc_position_provider_t* provider);

/**
 * @brief Check if a position provider is connected.
 * @param provider Pointer to the provider.
 * @return Non-zero if connected, zero otherwise.
 */
SDK_C_API int ogc_position_provider_is_connected(const ogc_position_provider_t* provider);

/* 10.14 NmeaParser */
/* Java: cn.cycle.chart.api.navi.NmeaParser */
/* C++:   ogc::navi::NmeaParser */
/* Header: ogc/navi/positioning/nmea_parser.h */

/**
 * @brief Opaque type representing an NMEA parser.
 */
typedef struct ogc_nmea_parser_t ogc_nmea_parser_t;

/**
 * @brief Create an NMEA parser.
 * @return Pointer to newly created parser, or NULL on failure.
 */
SDK_C_API ogc_nmea_parser_t* ogc_nmea_parser_create(void);

/**
 * @brief Destroy an NMEA parser.
 * @param parser Pointer to the parser to destroy.
 */
SDK_C_API void ogc_nmea_parser_destroy(ogc_nmea_parser_t* parser);

/**
 * @brief Parse an NMEA sentence.
 * @param parser Pointer to the parser.
 * @param sentence NMEA sentence string.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_nmea_parser_parse(ogc_nmea_parser_t* parser, const char* sentence);

/**
 * @brief Get the type of the last parsed sentence.
 * @param parser Pointer to the parser.
 * @return Sentence type string (e.g., "GGA", "RMC").
 */
SDK_C_API const char* ogc_nmea_parser_get_sentence_type(const ogc_nmea_parser_t* parser);

/* 10.15 CollisionAssessor */
/* Java: cn.cycle.chart.api.navi.CollisionAssessor */
/* C++:   ogc::navi::CollisionAssessor */
/* Header: ogc/navi/ais/collision_assessor.h */

/**
 * @brief Opaque type representing a collision assessor.
 */
typedef struct ogc_collision_assessor_t ogc_collision_assessor_t;

/**
 * @brief Collision risk assessment result.
 */
typedef struct ogc_collision_risk_t {
    uint32_t target_mmsi;    /**< Target vessel MMSI */
    double cpa;              /**< Closest Point of Approach (nautical miles) */
    double tcpa;             /**< Time to CPA (minutes) */
    int risk_level;          /**< Risk level (0=low, 1=medium, 2=high) */
    char* risk_description;  /**< Risk description string */
} ogc_collision_risk_t;

/**
 * @brief Create a collision assessor.
 * @return Pointer to newly created assessor, or NULL on failure.
 */
SDK_C_API ogc_collision_assessor_t* ogc_collision_assessor_create(void);

/**
 * @brief Destroy a collision assessor.
 * @param assessor Pointer to the assessor to destroy.
 */
SDK_C_API void ogc_collision_assessor_destroy(ogc_collision_assessor_t* assessor);

/**
 * @brief Assess collision risk with a target vessel.
 * @param assessor Pointer to the assessor.
 * @param own_lat Own vessel latitude (degrees).
 * @param own_lon Own vessel longitude (degrees).
 * @param own_speed Own vessel speed (knots).
 * @param own_course Own vessel course (degrees).
 * @param target Pointer to the AIS target.
 * @return Pointer to collision risk result (caller must destroy).
 */
SDK_C_API ogc_collision_risk_t* ogc_collision_assessor_assess(ogc_collision_assessor_t* assessor,
                                                      double own_lat, double own_lon, 
                                                      double own_speed, double own_course,
                                                      const ogc_ais_target_t* target);

/**
 * @brief Destroy a collision risk result.
 * @param risk Pointer to the risk result to destroy.
 */
SDK_C_API void ogc_collision_risk_destroy(ogc_collision_risk_t* risk);

/* 10.16 OffCourseDetector */
/* Java: cn.cycle.chart.api.navi.OffCourseDetector */
/* C++:   ogc::navi::OffCourseDetector */
/* Header: ogc/navi/navigation/off_course_detector.h */

/**
 * @brief Opaque type representing an off-course detector.
 */
typedef struct ogc_off_course_detector_t ogc_off_course_detector_t;

/**
 * @brief Route deviation result.
 */
typedef struct ogc_deviation_result_t {
    double cross_track_error;   /**< Cross-track error (nautical miles) */
    double distance_to_route;   /**< Distance to route (nautical miles) */
    int is_off_course;          /**< Non-zero if off course */
    int side;                   /**< Side of route (1=left, -1=right, 0=on course) */
} ogc_deviation_result_t;

/**
 * @brief Create an off-course detector.
 * @return Pointer to newly created detector, or NULL on failure.
 */
SDK_C_API ogc_off_course_detector_t* ogc_off_course_detector_create(void);

/**
 * @brief Destroy an off-course detector.
 * @param detector Pointer to the detector to destroy.
 */
SDK_C_API void ogc_off_course_detector_destroy(ogc_off_course_detector_t* detector);

/**
 * @brief Set the off-course threshold.
 * @param detector Pointer to the detector.
 * @param meters Threshold distance in meters.
 */
SDK_C_API void ogc_off_course_detector_set_threshold(ogc_off_course_detector_t* detector, double meters);

/**
 * @brief Check if the vessel is off course.
 * @param detector Pointer to the detector.
 * @param lat Current latitude (degrees).
 * @param lon Current longitude (degrees).
 * @param from Pointer to the from waypoint.
 * @param to Pointer to the to waypoint.
 * @return Deviation result structure.
 */
SDK_C_API ogc_deviation_result_t ogc_off_course_detector_check(ogc_off_course_detector_t* detector,
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

/**
 * @brief Opaque type representing a coordinate transformer.
 */
typedef struct ogc_coord_transformer_t ogc_coord_transformer_t;

/**
 * @brief Create a coordinate transformer.
 * @param source_crs Source coordinate reference system (e.g., "EPSG:4326").
 * @param target_crs Target coordinate reference system (e.g., "EPSG:3857").
 * @return Pointer to newly created transformer, or NULL on failure.
 */
SDK_C_API ogc_coord_transformer_t* ogc_coord_transformer_create(const char* source_crs, const char* target_crs);

/**
 * @brief Destroy a coordinate transformer.
 * @param transformer Pointer to the transformer to destroy.
 */
SDK_C_API void ogc_coord_transformer_destroy(ogc_coord_transformer_t* transformer);

/**
 * @brief Check if a coordinate transformer is valid.
 * @param transformer Pointer to the transformer.
 * @return Non-zero if valid, zero otherwise.
 */
SDK_C_API int ogc_coord_transformer_is_valid(const ogc_coord_transformer_t* transformer);

/**
 * @brief Transform a coordinate.
 * @param transformer Pointer to the transformer.
 * @param coord Pointer to the coordinate to transform.
 * @return Transformed coordinate.
 */
SDK_C_API ogc_coordinate_t ogc_coord_transformer_transform(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord);

/**
 * @brief Transform a coordinate in the inverse direction.
 * @param transformer Pointer to the transformer.
 * @param coord Pointer to the coordinate to transform.
 * @return Transformed coordinate.
 */
SDK_C_API ogc_coordinate_t ogc_coord_transformer_transform_inverse(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord);

/**
 * @brief Transform an array of coordinates.
 * @param transformer Pointer to the transformer.
 * @param x Array of X coordinates (modified in place).
 * @param y Array of Y coordinates (modified in place).
 * @param count Number of coordinates.
 */
SDK_C_API void ogc_coord_transformer_transform_array(const ogc_coord_transformer_t* transformer, double* x, double* y, size_t count);

/**
 * @brief Transform an envelope.
 * @param transformer Pointer to the transformer.
 * @param env Pointer to the envelope to transform.
 * @return Pointer to newly created envelope, or NULL on failure.
 */
SDK_C_API ogc_envelope_t* ogc_coord_transformer_transform_envelope(const ogc_coord_transformer_t* transformer, const ogc_envelope_t* env);

/**
 * @brief Transform a geometry.
 * @param transformer Pointer to the transformer.
 * @param geom Pointer to the geometry to transform.
 * @return Pointer to newly created geometry, or NULL on failure.
 */
SDK_C_API ogc_geometry_t* ogc_coord_transformer_transform_geometry(const ogc_coord_transformer_t* transformer, const ogc_geometry_t* geom);

/**
 * @brief Get the source CRS of a transformer.
 * @param transformer Pointer to the transformer.
 * @return Source CRS string.
 */
SDK_C_API const char* ogc_coord_transformer_get_source_crs(const ogc_coord_transformer_t* transformer);

/**
 * @brief Get the target CRS of a transformer.
 * @param transformer Pointer to the transformer.
 * @return Target CRS string.
 */
SDK_C_API const char* ogc_coord_transformer_get_target_crs(const ogc_coord_transformer_t* transformer);

/* 11.2 TransformMatrix */
/* Java: cn.cycle.chart.api.proj.TransformMatrix */
/* C++:   ogc::proj::TransformMatrix */
/* Header: ogc/proj/transform_matrix.h */

/**
 * @brief Opaque type representing a transformation matrix.
 */
typedef struct ogc_transform_matrix_t ogc_transform_matrix_t;

/**
 * @brief Create a transformation matrix.
 * @return Pointer to newly created matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create(void);

/**
 * @brief Create an identity transformation matrix.
 * @return Pointer to newly created matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void);

/**
 * @brief Destroy a transformation matrix.
 * @param matrix Pointer to the matrix to destroy.
 */
SDK_C_API void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix);

/**
 * @brief Apply a translation to a matrix.
 * @param matrix Pointer to the matrix.
 * @param dx Translation in X direction.
 * @param dy Translation in Y direction.
 */
SDK_C_API void ogc_transform_matrix_translate(ogc_transform_matrix_t* matrix, double dx, double dy);

/**
 * @brief Apply a scale to a matrix.
 * @param matrix Pointer to the matrix.
 * @param sx Scale factor in X direction.
 * @param sy Scale factor in Y direction.
 */
SDK_C_API void ogc_transform_matrix_scale(ogc_transform_matrix_t* matrix, double sx, double sy);

/**
 * @brief Apply a rotation to a matrix.
 * @param matrix Pointer to the matrix.
 * @param angle_degrees Rotation angle in degrees.
 */
SDK_C_API void ogc_transform_matrix_rotate(ogc_transform_matrix_t* matrix, double angle_degrees);

/**
 * @brief Multiply two matrices.
 * @param result Pointer to store the result matrix.
 * @param a First matrix.
 * @param b Second matrix.
 */
SDK_C_API void ogc_transform_matrix_multiply(ogc_transform_matrix_t* result, const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b);

/**
 * @brief Transform a coordinate using a matrix.
 * @param matrix Pointer to the matrix.
 * @param coord Pointer to the coordinate to transform.
 * @return Transformed coordinate.
 */
SDK_C_API ogc_coordinate_t ogc_transform_matrix_transform(const ogc_transform_matrix_t* matrix, const ogc_coordinate_t* coord);

/**
 * @brief Get the values of a transformation matrix.
 * @param matrix Pointer to the matrix.
 * @param values Array to store the 9 matrix values (3x3).
 */
SDK_C_API void ogc_transform_matrix_get_values(const ogc_transform_matrix_t* matrix, double* values);

/* ============================================================================
 * 12. Chart Parser Module (chart_parser)
 * ============================================================================
 * Java Package: cn.cycle.chart.api.parser
 * Description: Chart parsing (S57/S101)
 */

/* 12.1 ChartFormat */
/* Java: cn.cycle.chart.api.parser.ChartFormat */
/* C++:   chart::parser::ChartFormat */
/* Header: parser/chart_format.h */

/**
 * @brief Chart format enumeration.
 */
typedef enum ogc_chart_format_e {
    OGC_CHART_FORMAT_S57 = 0,      /**< S-57 electronic navigational chart */
    OGC_CHART_FORMAT_S100 = 1,     /**< S-100 universal hydrographic data model */
    OGC_CHART_FORMAT_S101 = 2,     /**< S-101 electronic navigational chart */
    OGC_CHART_FORMAT_S102 = 3,     /**< S-102 bathymetric surface */
    OGC_CHART_FORMAT_GML = 4,      /**< GML format */
    OGC_CHART_FORMAT_UNKNOWN = 5   /**< Unknown format */
} ogc_chart_format_e;

/**
 * @brief Parse configuration structure.
 */
typedef struct ogc_parse_config_t {
    int validate_geometry;       /**< Whether to validate geometry */
    int validate_attributes;     /**< Whether to validate attributes */
    int strict_mode;             /**< Enable strict parsing mode */
    int include_metadata;        /**< Include metadata in result */
    int max_feature_count;       /**< Maximum features to parse (0 = unlimited) */
    const char* coordinate_system; /**< Coordinate system (e.g., "EPSG:4326") */
    double tolerance;            /**< Geometry tolerance */
} ogc_parse_config_t;

/* 12.2 ChartParser */
/* Java: cn.cycle.chart.api.parser.ChartParser */
/* C++:   chart::parser::ChartParser */
/* Header: parser/chart_parser.h */

/**
 * @brief Opaque type representing a chart parser.
 */
typedef struct ogc_chart_parser_t ogc_chart_parser_t;

/**
 * @brief Get the singleton chart parser instance.
 * @return Pointer to the chart parser instance.
 */
SDK_C_API ogc_chart_parser_t* ogc_chart_parser_get_instance(void);

/**
 * @brief Initialize the chart parser.
 * @param parser Pointer to the parser.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_parser_initialize(ogc_chart_parser_t* parser);

/**
 * @brief Shutdown the chart parser.
 * @param parser Pointer to the parser.
 */
SDK_C_API void ogc_chart_parser_shutdown(ogc_chart_parser_t* parser);

/**
 * @brief Get the supported chart formats.
 * @param parser Pointer to the parser.
 * @param formats Array to store the format enumerations.
 * @param max_count Maximum number of formats to return.
 * @return Number of formats returned.
 */
SDK_C_API int ogc_chart_parser_get_supported_formats(const ogc_chart_parser_t* parser, ogc_chart_format_e* formats, int max_count);

/* 12.3 IParser */
/* Java: cn.cycle.chart.api.parser.IParser */
/* C++:   chart::parser::IParser */
/* Header: parser/iparser.h */

/**
 * @brief Opaque type representing a parser interface.
 */
typedef struct ogc_iparser_t ogc_iparser_t;

/**
 * @brief Opaque type representing a parse result.
 */
typedef struct ogc_parse_result_t ogc_parse_result_t;

/**
 * @brief Create a parser for a specific format.
 * @param format Chart format to parse.
 * @return Pointer to newly created parser, or NULL on failure.
 */
SDK_C_API ogc_iparser_t* ogc_iparser_create(ogc_chart_format_e format);

/**
 * @brief Destroy a parser.
 * @param parser Pointer to the parser to destroy.
 */
SDK_C_API void ogc_iparser_destroy(ogc_iparser_t* parser);

/**
 * @brief Open a chart file for parsing.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_iparser_open(ogc_iparser_t* parser, const char* path);

/**
 * @brief Close the chart file.
 * @param parser Pointer to the parser.
 */
SDK_C_API void ogc_iparser_close(ogc_iparser_t* parser);

/**
 * @brief Parse the entire chart file.
 * @param parser Pointer to the parser.
 * @return Pointer to the parse result, or NULL on failure.
 */
SDK_C_API ogc_parse_result_t* ogc_iparser_parse(ogc_iparser_t* parser);

/**
 * @brief Parse the chart file incrementally.
 * @param parser Pointer to the parser.
 * @param batch_size Number of features to parse per batch.
 * @return Pointer to the parse result, or NULL on failure.
 */
SDK_C_API ogc_parse_result_t* ogc_iparser_parse_incremental(ogc_iparser_t* parser, int batch_size);

/**
 * @brief Get the format of a parser.
 * @param parser Pointer to the parser.
 * @return Chart format enumeration value.
 */
SDK_C_API ogc_chart_format_e ogc_iparser_get_format(const ogc_iparser_t* parser);

/* 12.4 S57Parser */
/* Java: cn.cycle.chart.api.parser.S57Parser */
/* C++:   chart::parser::S57Parser */
/* Header: parser/s57_parser.h */

/**
 * @brief Opaque type representing an S-57 parser.
 */
typedef struct ogc_s57_parser_t ogc_s57_parser_t;

/**
 * @brief Create an S-57 parser.
 * @return Pointer to newly created parser, or NULL on failure.
 */
SDK_C_API ogc_s57_parser_t* ogc_s57_parser_create(void);

/**
 * @brief Destroy an S-57 parser.
 * @param parser Pointer to the parser to destroy.
 */
SDK_C_API void ogc_s57_parser_destroy(ogc_s57_parser_t* parser);

/**
 * @brief Open an S-57 chart file.
 * @param parser Pointer to the parser.
 * @param path Path to the S-57 file.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_s57_parser_open(ogc_s57_parser_t* parser, const char* path);

/**
 * @brief Parse an S-57 chart file.
 * @param parser Pointer to the parser.
 * @return Pointer to the parse result, or NULL on failure.
 */
SDK_C_API ogc_parse_result_t* ogc_s57_parser_parse(ogc_s57_parser_t* parser);

/**
 * @brief Set a feature filter for parsing.
 * @param parser Pointer to the parser.
 * @param features Array of feature names to include.
 * @param count Number of features in the array.
 */
SDK_C_API void ogc_s57_parser_set_feature_filter(ogc_s57_parser_t* parser, const char** features, int count);

/**
 * @brief Set a spatial filter for parsing.
 * @param parser Pointer to the parser.
 * @param bounds Bounding envelope for the spatial filter.
 */
SDK_C_API void ogc_s57_parser_set_spatial_filter(ogc_s57_parser_t* parser, const ogc_envelope_t* bounds);

/* 12.5 IncrementalParser */
/* Java: cn.cycle.chart.api.parser.IncrementalParser */
/* C++:   chart::parser::IncrementalParser */
/* Header: parser/incremental_parser.h */

/**
 * @brief Opaque type representing an incremental parser.
 */
typedef struct ogc_incremental_parser_t ogc_incremental_parser_t;

/**
 * @brief Create an incremental parser.
 * @return Pointer to newly created parser, or NULL on failure.
 */
SDK_C_API ogc_incremental_parser_t* ogc_incremental_parser_create(void);

/**
 * @brief Destroy an incremental parser.
 * @param parser Pointer to the parser to destroy.
 */
SDK_C_API void ogc_incremental_parser_destroy(ogc_incremental_parser_t* parser);

/**
 * @brief Start incremental parsing.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 * @param batch_size Number of features to parse per batch.
 */
SDK_C_API void ogc_incremental_parser_start(ogc_incremental_parser_t* parser, const char* path, int batch_size);

/**
 * @brief Pause incremental parsing.
 * @param parser Pointer to the parser.
 */
SDK_C_API void ogc_incremental_parser_pause(ogc_incremental_parser_t* parser);

/**
 * @brief Resume incremental parsing.
 * @param parser Pointer to the parser.
 */
SDK_C_API void ogc_incremental_parser_resume(ogc_incremental_parser_t* parser);

/**
 * @brief Cancel incremental parsing.
 * @param parser Pointer to the parser.
 */
SDK_C_API void ogc_incremental_parser_cancel(ogc_incremental_parser_t* parser);

/**
 * @brief Parse the next batch of features.
 * @param parser Pointer to the parser.
 * @return Pointer to the parse result, or NULL if complete.
 */
SDK_C_API ogc_parse_result_t* ogc_incremental_parser_parse_next(ogc_incremental_parser_t* parser);

/**
 * @brief Check if there are more features to parse.
 * @param parser Pointer to the parser.
 * @return Non-zero if more features, zero otherwise.
 */
SDK_C_API int ogc_incremental_parser_has_more(const ogc_incremental_parser_t* parser);

/**
 * @brief Get the parsing progress.
 * @param parser Pointer to the parser.
 * @return Progress value (0.0 to 1.0).
 */
SDK_C_API float ogc_incremental_parser_get_progress(const ogc_incremental_parser_t* parser);

/**
 * @brief Check if a file has changed since last parse.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 * @return Non-zero if file has changed, zero otherwise.
 */
SDK_C_API int ogc_incremental_parser_has_file_changed(const ogc_incremental_parser_t* parser, const char* path);

/**
 * @brief Mark a file as processed.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 */
SDK_C_API void ogc_incremental_parser_mark_processed(ogc_incremental_parser_t* parser, const char* path);

/**
 * @brief Clear the state for a file.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 */
SDK_C_API void ogc_incremental_parser_clear_state(ogc_incremental_parser_t* parser, const char* path);

/**
 * @brief Incremental parse result structure.
 */
typedef struct ogc_incremental_parse_result_t {
    int has_changes;            /**< Whether changes were detected */
    double parse_time_ms;       /**< Parse time in milliseconds */
} ogc_incremental_parse_result_t;

/**
 * @brief Parse a file incrementally and detect changes.
 * @param parser Pointer to the parser.
 * @param path Path to the chart file.
 * @param config Parse configuration, or NULL for defaults.
 * @return Pointer to the incremental parse result, or NULL on failure.
 */
SDK_C_API ogc_incremental_parse_result_t* ogc_incremental_parser_parse_incremental(ogc_incremental_parser_t* parser, const char* path, const ogc_parse_config_t* config);

/**
 * @brief Destroy an incremental parse result.
 * @param result Pointer to the result to destroy.
 */
SDK_C_API void ogc_incremental_parse_result_destroy(ogc_incremental_parse_result_t* result);

/* 12.6 S57Parser Extended Functions */

/**
 * @brief Parse an S-57 chart file with configuration.
 * @param parser Pointer to the parser.
 * @param path Path to the S-57 file.
 * @param config Parse configuration, or NULL for defaults.
 * @return Pointer to the parse result, or NULL on failure.
 */
SDK_C_API ogc_parse_result_t* ogc_s57_parser_parse_file(ogc_s57_parser_t* parser, const char* path, const ogc_parse_config_t* config);

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

/**
 * @brief Opaque type representing a chart plugin.
 */
typedef struct ogc_chart_plugin_t ogc_chart_plugin_t;

/**
 * @brief Create a chart plugin.
 * @param name Plugin name.
 * @param version Plugin version.
 * @return Pointer to newly created plugin, or NULL on failure.
 */
SDK_C_API ogc_chart_plugin_t* ogc_chart_plugin_create(const char* name, const char* version);

/**
 * @brief Destroy a chart plugin.
 * @param plugin Pointer to the plugin to destroy.
 */
SDK_C_API void ogc_chart_plugin_destroy(ogc_chart_plugin_t* plugin);

/**
 * @brief Get the name of a plugin.
 * @param plugin Pointer to the plugin.
 * @return Plugin name string.
 */
SDK_C_API const char* ogc_chart_plugin_get_name(const ogc_chart_plugin_t* plugin);

/**
 * @brief Get the version of a plugin.
 * @param plugin Pointer to the plugin.
 * @return Plugin version string.
 */
SDK_C_API const char* ogc_chart_plugin_get_version(const ogc_chart_plugin_t* plugin);

/**
 * @brief Initialize a plugin.
 * @param plugin Pointer to the plugin.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_chart_plugin_initialize(ogc_chart_plugin_t* plugin);

/**
 * @brief Shutdown a plugin.
 * @param plugin Pointer to the plugin.
 */
SDK_C_API void ogc_chart_plugin_shutdown(ogc_chart_plugin_t* plugin);

/* 13.2 PluginManager */
/* Java: cn.cycle.chart.plugin.PluginManager */
/* C++:   ogc::draw::PluginManager */
/* Header: ogc/draw/plugin_manager.h */

/**
 * @brief Opaque type representing a plugin manager.
 */
typedef struct ogc_plugin_manager_t ogc_plugin_manager_t;

/**
 * @brief Get the singleton plugin manager instance.
 * @return Pointer to the plugin manager instance.
 */
SDK_C_API ogc_plugin_manager_t* ogc_plugin_manager_get_instance(void);

/**
 * @brief Load a plugin from a file path.
 * @param mgr Pointer to the manager.
 * @param path Path to the plugin file.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_plugin_manager_load_plugin(ogc_plugin_manager_t* mgr, const char* path);

/**
 * @brief Unload a plugin by name.
 * @param mgr Pointer to the manager.
 * @param name Name of the plugin to unload.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_plugin_manager_unload_plugin(ogc_plugin_manager_t* mgr, const char* name);

/**
 * @brief Get a plugin by name.
 * @param mgr Pointer to the manager.
 * @param name Plugin name.
 * @return Pointer to the plugin, or NULL if not found.
 */
SDK_C_API ogc_chart_plugin_t* ogc_plugin_manager_get_plugin(const ogc_plugin_manager_t* mgr, const char* name);

/**
 * @brief Get the number of loaded plugins.
 * @param mgr Pointer to the manager.
 * @return Number of plugins.
 */
SDK_C_API int ogc_plugin_manager_get_plugin_count(const ogc_plugin_manager_t* mgr);

/**
 * @brief Get the name of a plugin by index.
 * @param mgr Pointer to the manager.
 * @param index Plugin index (0-based).
 * @return Plugin name string, or NULL if index is invalid.
 */
SDK_C_API const char* ogc_plugin_manager_get_plugin_name(const ogc_plugin_manager_t* mgr, int index);

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

/**
 * @brief Opaque type representing an error recovery manager.
 */
typedef struct ogc_error_recovery_manager_t ogc_error_recovery_manager_t;

/**
 * @brief Create an error recovery manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_error_recovery_manager_t* ogc_error_recovery_manager_create(void);

/**
 * @brief Destroy an error recovery manager.
 * @param mgr Pointer to the manager to destroy.
 */
SDK_C_API void ogc_error_recovery_manager_destroy(ogc_error_recovery_manager_t* mgr);

/**
 * @brief Initialize an error recovery manager.
 * @param mgr Pointer to the manager.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_error_recovery_manager_initialize(ogc_error_recovery_manager_t* mgr);

/**
 * @brief Shutdown an error recovery manager.
 * @param mgr Pointer to the manager.
 */
SDK_C_API void ogc_error_recovery_manager_shutdown(ogc_error_recovery_manager_t* mgr);

/**
 * @brief Handle an error with the recovery manager.
 * @param mgr Pointer to the manager.
 * @param error_code Error code to handle.
 * @param context Context string for the error.
 * @return 0 if handled successfully, non-zero otherwise.
 */
SDK_C_API int ogc_error_recovery_manager_handle_error(ogc_error_recovery_manager_t* mgr, int error_code, const char* context);

/**
 * @brief Register a recovery strategy for an error code.
 * @param mgr Pointer to the manager.
 * @param error_code Error code to register for.
 * @param strategy Pointer to the recovery strategy.
 */
SDK_C_API void ogc_error_recovery_manager_register_strategy(ogc_error_recovery_manager_t* mgr, int error_code, void* strategy);

/**
 * @brief Set the degradation level.
 * @param mgr Pointer to the manager.
 * @param level Degradation level to set.
 */
SDK_C_API void ogc_error_recovery_manager_set_degradation_level(ogc_error_recovery_manager_t* mgr, int level);

/* 14.2 CircuitBreaker */
/* Java: cn.cycle.chart.api.recovery.CircuitBreaker */
/* C++:   ogc::recovery::CircuitBreaker */
/* Header: ogc/recovery/circuit_breaker.h */

/**
 * @brief Opaque type representing a circuit breaker.
 */
typedef struct ogc_circuit_breaker_t ogc_circuit_breaker_t;

/**
 * @brief Circuit breaker state enumeration.
 */
typedef enum ogc_circuit_state_e {
    OGC_CIRCUIT_CLOSED = 0,    /**< Circuit is closed (normal operation) */
    OGC_CIRCUIT_OPEN = 1,      /**< Circuit is open (failing) */
    OGC_CIRCUIT_HALF_OPEN = 2  /**< Circuit is half-open (testing) */
} ogc_circuit_state_e;

/**
 * @brief Create a circuit breaker.
 * @param failure_threshold Number of failures before opening.
 * @param recovery_timeout_ms Time in milliseconds before attempting recovery.
 * @return Pointer to newly created circuit breaker, or NULL on failure.
 */
SDK_C_API ogc_circuit_breaker_t* ogc_circuit_breaker_create(int failure_threshold, int recovery_timeout_ms);

/**
 * @brief Destroy a circuit breaker.
 * @param breaker Pointer to the circuit breaker to destroy.
 */
SDK_C_API void ogc_circuit_breaker_destroy(ogc_circuit_breaker_t* breaker);

/**
 * @brief Get the state of a circuit breaker.
 * @param breaker Pointer to the circuit breaker.
 * @return Circuit breaker state enumeration value.
 */
SDK_C_API ogc_circuit_state_e ogc_circuit_breaker_get_state(const ogc_circuit_breaker_t* breaker);

/**
 * @brief Check if a request is allowed.
 * @param breaker Pointer to the circuit breaker.
 * @return Non-zero if allowed, zero if blocked.
 */
SDK_C_API int ogc_circuit_breaker_allow_request(ogc_circuit_breaker_t* breaker);

/**
 * @brief Record a successful request.
 * @param breaker Pointer to the circuit breaker.
 */
SDK_C_API void ogc_circuit_breaker_record_success(ogc_circuit_breaker_t* breaker);

/**
 * @brief Record a failed request.
 * @param breaker Pointer to the circuit breaker.
 */
SDK_C_API void ogc_circuit_breaker_record_failure(ogc_circuit_breaker_t* breaker);

/**
 * @brief Reset a circuit breaker.
 * @param breaker Pointer to the circuit breaker.
 */
SDK_C_API void ogc_circuit_breaker_reset(ogc_circuit_breaker_t* breaker);

/* 14.3 GracefulDegradation */
/* Java: cn.cycle.chart.api.recovery.GracefulDegradation */
/* C++:   ogc::recovery::GracefulDegradation */
/* Header: ogc/recovery/graceful_degradation.h */

/**
 * @brief Opaque type representing a graceful degradation manager.
 */
typedef struct ogc_graceful_degradation_t ogc_graceful_degradation_t;

/**
 * @brief Degradation level enumeration.
 */
typedef enum ogc_degradation_level_e {
    OGC_DEGRADATION_NONE = 0,      /**< No degradation */
    OGC_DEGRADATION_MINIMAL = 1,   /**< Minimal degradation */
    OGC_DEGRADATION_MODERATE = 2,  /**< Moderate degradation */
    OGC_DEGRADATION_SEVERE = 3,    /**< Severe degradation */
    OGC_DEGRADATION_CRITICAL = 4   /**< Critical degradation */
} ogc_degradation_level_e;

/**
 * @brief Create a graceful degradation manager.
 * @return Pointer to newly created manager, or NULL on failure.
 */
SDK_C_API ogc_graceful_degradation_t* ogc_graceful_degradation_create(void);

/**
 * @brief Destroy a graceful degradation manager.
 * @param degradation Pointer to the manager to destroy.
 */
SDK_C_API void ogc_graceful_degradation_destroy(ogc_graceful_degradation_t* degradation);

/**
 * @brief Get the current degradation level.
 * @param degradation Pointer to the manager.
 * @return Degradation level enumeration value.
 */
SDK_C_API ogc_degradation_level_e ogc_graceful_degradation_get_level(const ogc_graceful_degradation_t* degradation);

/**
 * @brief Set the degradation level.
 * @param degradation Pointer to the manager.
 * @param level Degradation level to set.
 */
SDK_C_API void ogc_graceful_degradation_set_level(ogc_graceful_degradation_t* degradation, ogc_degradation_level_e level);

/**
 * @brief Check if a feature is enabled.
 * @param degradation Pointer to the manager.
 * @param feature Feature name to check.
 * @return Non-zero if enabled, zero if disabled.
 */
SDK_C_API int ogc_graceful_degradation_is_feature_enabled(const ogc_graceful_degradation_t* degradation, const char* feature);

/**
 * @brief Disable a feature.
 * @param degradation Pointer to the manager.
 * @param feature Feature name to disable.
 */
SDK_C_API void ogc_graceful_degradation_disable_feature(ogc_graceful_degradation_t* degradation, const char* feature);

/**
 * @brief Enable a feature.
 * @param degradation Pointer to the manager.
 * @param feature Feature name to enable.
 */
SDK_C_API void ogc_graceful_degradation_enable_feature(ogc_graceful_degradation_t* degradation, const char* feature);

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

/**
 * @brief Health status enumeration.
 */
typedef enum ogc_health_status_e {
    OGC_HEALTH_UNKNOWN = 0,     /**< Health status unknown */
    OGC_HEALTH_HEALTHY = 1,     /**< Component is healthy */
    OGC_HEALTH_DEGRADED = 2,    /**< Component is degraded */
    OGC_HEALTH_UNHEALTHY = 3    /**< Component is unhealthy */
} ogc_health_status_e;

/* 15.2 HealthCheck */
/* Java: cn.cycle.chart.api.health.HealthCheck */
/* C++:   ogc::health::HealthCheck */
/* Header: ogc/health/health_check.h */

/**
 * @brief Opaque type representing a health check.
 */
typedef struct ogc_health_check_t ogc_health_check_t;

/**
 * @brief Health check result structure.
 */
typedef struct ogc_health_check_result_t {
    ogc_health_status_e status;      /**< Health status */
    char* component_name;            /**< Component name */
    char* message;                   /**< Status message */
    double check_duration_ms;        /**< Duration of check in milliseconds */
} ogc_health_check_result_t;

/**
 * @brief Create a health check.
 * @param name Name of the health check.
 * @return Pointer to newly created health check, or NULL on failure.
 */
SDK_C_API ogc_health_check_t* ogc_health_check_create(const char* name);

/**
 * @brief Destroy a health check.
 * @param check Pointer to the health check to destroy.
 */
SDK_C_API void ogc_health_check_destroy(ogc_health_check_t* check);

/**
 * @brief Execute a health check.
 * @param check Pointer to the health check.
 * @return Pointer to the health check result, or NULL on failure.
 */
SDK_C_API ogc_health_check_result_t* ogc_health_check_execute(ogc_health_check_t* check);

/**
 * @brief Destroy a health check result.
 * @param result Pointer to the result to destroy.
 */
SDK_C_API void ogc_health_check_result_destroy(ogc_health_check_result_t* result);

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

/**
 * @brief Opaque type representing a library loader.
 */
typedef struct ogc_library_loader_t ogc_library_loader_t;

/**
 * @brief Opaque type representing a library handle.
 */
typedef void* ogc_library_handle_t;

/**
 * @brief Create a library loader.
 * @return Pointer to newly created loader, or NULL on failure.
 */
SDK_C_API ogc_library_loader_t* ogc_library_loader_create(void);

/**
 * @brief Destroy a library loader.
 * @param loader Pointer to the loader to destroy.
 */
SDK_C_API void ogc_library_loader_destroy(ogc_library_loader_t* loader);

/**
 * @brief Load a library.
 * @param loader Pointer to the loader.
 * @param library_path Path to the library file.
 * @return Handle to the loaded library, or NULL on failure.
 */
SDK_C_API ogc_library_handle_t ogc_library_loader_load(ogc_library_loader_t* loader, const char* library_path);

/**
 * @brief Unload a library.
 * @param loader Pointer to the loader.
 * @param handle Handle to the library to unload.
 */
SDK_C_API void ogc_library_loader_unload(ogc_library_loader_t* loader, ogc_library_handle_t handle);

/**
 * @brief Get a symbol from a library.
 * @param loader Pointer to the loader.
 * @param handle Handle to the library.
 * @param symbol_name Name of the symbol to get.
 * @return Pointer to the symbol, or NULL if not found.
 */
void* ogc_library_loader_get_symbol(ogc_library_loader_t* loader, ogc_library_handle_t handle, const char* symbol_name);

/**
 * @brief Get the last error message.
 * @param loader Pointer to the loader.
 * @return Error message string, or NULL if no error.
 */
SDK_C_API const char* ogc_library_loader_get_error(ogc_library_loader_t* loader);

/* 16.2 SecureLibraryLoader */
/* Java: cn.cycle.chart.api.loader.SecureLibraryLoader */
/* C++:   ogc::loader::SecureLibraryLoader */
/* Header: ogc/loader/secure_library_loader.h */

/**
 * @brief Opaque type representing a secure library loader.
 */
typedef struct ogc_secure_library_loader_t ogc_secure_library_loader_t;

/**
 * @brief Create a secure library loader.
 * @param allowed_paths Semicolon-separated list of allowed paths.
 * @return Pointer to newly created loader, or NULL on failure.
 */
SDK_C_API ogc_secure_library_loader_t* ogc_secure_library_loader_create(const char* allowed_paths);

/**
 * @brief Destroy a secure library loader.
 * @param loader Pointer to the loader to destroy.
 */
SDK_C_API void ogc_secure_library_loader_destroy(ogc_secure_library_loader_t* loader);

/**
 * @brief Load a library with security checks.
 * @param loader Pointer to the loader.
 * @param library_path Path to the library file.
 * @return Handle to the loaded library, or NULL on failure.
 */
SDK_C_API ogc_library_handle_t ogc_secure_library_loader_load(ogc_secure_library_loader_t* loader, const char* library_path);

/**
 * @brief Verify the signature of a library.
 * @param loader Pointer to the loader.
 * @param handle Handle to the library.
 * @param public_key Public key for signature verification.
 * @return 0 on success, non-zero on failure.
 */
SDK_C_API int ogc_secure_library_loader_verify_signature(ogc_secure_library_loader_t* loader, ogc_library_handle_t handle, const char* public_key);

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

/**
 * @brief Chart exception structure.
 */
typedef struct ogc_chart_exception_t {
    int error_code;      /**< Error code */
    char* message;       /**< Error message */
    char* context;       /**< Error context */
} ogc_chart_exception_t;

/**
 * @brief Create a chart exception.
 * @param error_code Error code.
 * @param message Error message.
 * @return Pointer to newly created exception, or NULL on failure.
 */
SDK_C_API ogc_chart_exception_t* ogc_chart_exception_create(int error_code, const char* message);

/**
 * @brief Destroy a chart exception.
 * @param ex Pointer to the exception to destroy.
 */
SDK_C_API void ogc_chart_exception_destroy(ogc_chart_exception_t* ex);

/**
 * @brief Get the error code from an exception.
 * @param ex Pointer to the exception.
 * @return Error code.
 */
SDK_C_API int ogc_chart_exception_get_error_code(const ogc_chart_exception_t* ex);

/**
 * @brief Get the message from an exception.
 * @param ex Pointer to the exception.
 * @return Error message string.
 */
SDK_C_API const char* ogc_chart_exception_get_message(const ogc_chart_exception_t* ex);

/**
 * @brief Get the context from an exception.
 * @param ex Pointer to the exception.
 * @return Error context string.
 */
SDK_C_API const char* ogc_chart_exception_get_context(const ogc_chart_exception_t* ex);

/* 17.2 JniException */
/* Java: cn.cycle.chart.api.exception.JniException */
/* C++:   ogc::exception::JniException */
/* Header: ogc/exception/jni_exception.h */

/**
 * @brief JNI exception structure.
 */
typedef struct ogc_jni_exception_t {
    int error_code;              /**< Error code */
    char* message;               /**< Error message */
    char* java_exception_class;  /**< Java exception class name */
} ogc_jni_exception_t;

/**
 * @brief Create a JNI exception.
 * @param message Error message.
 * @param java_class Java exception class name.
 * @return Pointer to newly created exception, or NULL on failure.
 */
SDK_C_API ogc_jni_exception_t* ogc_jni_exception_create(const char* message, const char* java_class);

/**
 * @brief Destroy a JNI exception.
 * @param ex Pointer to the exception to destroy.
 */
SDK_C_API void ogc_jni_exception_destroy(ogc_jni_exception_t* ex);

/* 17.3 RenderException */
/* Java: cn.cycle.chart.api.exception.RenderException */
/* C++:   ogc::exception::RenderException */
/* Header: ogc/exception/render_exception.h */

/**
 * @brief Render exception structure.
 */
typedef struct ogc_render_exception_t {
    int error_code;      /**< Error code */
    char* message;       /**< Error message */
    char* layer_name;    /**< Layer name where error occurred */
    char* operation;     /**< Operation that failed */
} ogc_render_exception_t;

/**
 * @brief Create a render exception.
 * @param message Error message.
 * @param layer_name Name of the layer where error occurred.
 * @return Pointer to newly created exception, or NULL on failure.
 */
SDK_C_API ogc_render_exception_t* ogc_render_exception_create(const char* message, const char* layer_name);

/**
 * @brief Destroy a render exception.
 * @param ex Pointer to the exception to destroy.
 */
SDK_C_API void ogc_render_exception_destroy(ogc_render_exception_t* ex);

/* ============================================================================
 * 18. Callback Types
 * ============================================================================
 */

/**
 * @brief Progress callback function type.
 * @param progress Progress value (0.0 to 1.0).
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_progress_callback_t)(float progress, void* user_data);

/**
 * @brief Alert callback function type.
 * @param alert Pointer to the alert structure.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_alert_callback_t)(const ogc_alert_t* alert, void* user_data);

/**
 * @brief Position update callback function type.
 * @param lat Latitude in degrees.
 * @param lon Longitude in degrees.
 * @param speed Speed in knots.
 * @param course Course in degrees.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_position_callback_t)(double lat, double lon, double speed, double course, void* user_data);

/**
 * @brief AIS target callback function type.
 * @param target Pointer to the AIS target structure.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_ais_target_callback_t)(const ogc_ais_target_t* target, void* user_data);

/**
 * @brief Render complete callback function type.
 * @param success Non-zero if render succeeded, zero on failure.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_render_callback_t)(int success, void* user_data);

/**
 * @brief Load complete callback function type.
 * @param success Non-zero if load succeeded, zero on failure.
 * @param chart_id Chart identifier.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_load_callback_t)(int success, const char* chart_id, void* user_data);

/**
 * @brief Touch callback function type.
 * @param touch_type Touch event type.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param user_data User-provided data pointer.
 */
typedef void (*ogc_touch_callback_t)(int touch_type, double x, double y, void* user_data);

/**
 * @brief Recovery strategy callback function type.
 * @param error_code Error code to recover from.
 * @param context Error context string.
 * @param user_data User-provided data pointer.
 * @return 0 on successful recovery, non-zero otherwise.
 */
typedef int (*ogc_recovery_strategy_t)(int error_code, const char* context, void* user_data);

/* ============================================================================
 * 19. Enum Types
 * ============================================================================
 */

/* DisplayMode */
/* Java: cn.cycle.chart.api.core.DisplayMode */

/**
 * @brief Display mode enumeration.
 */
typedef enum ogc_display_mode_e {
    OGC_DISPLAY_MODE_DAY = 0,    /**< Day display mode */
    OGC_DISPLAY_MODE_DUSK = 1,   /**< Dusk display mode */
    OGC_DISPLAY_MODE_NIGHT = 2   /**< Night display mode */
} ogc_display_mode_e;

/* AppState */
/* Java: cn.cycle.chart.api.core.AppState */

/**
 * @brief Application state enumeration.
 */
typedef enum ogc_app_state_e {
    OGC_APP_STATE_UNINITIALIZED = 0,   /**< Application is uninitialized */
    OGC_APP_STATE_INITIALIZING = 1,    /**< Application is initializing */
    OGC_APP_STATE_READY = 2,           /**< Application is ready */
    OGC_APP_STATE_RUNNING = 3,         /**< Application is running */
    OGC_APP_STATE_PAUSED = 4,          /**< Application is paused */
    OGC_APP_STATE_ERROR = 5,           /**< Application is in error state */
    OGC_APP_STATE_SHUTTING_DOWN = 6    /**< Application is shutting down */
} ogc_app_state_e;

/* ============================================================================
 * 20. Utility Classes
 * ============================================================================
 */

/* 20.1 TransformMatrix */
/* Java: cn.cycle.chart.api.proj.TransformMatrix */
/* C++:   ogc::proj::TransformMatrix */
/* Header: ogc/proj/transform_matrix.h */

/**
 * @brief Opaque type representing a transform matrix.
 */
typedef struct ogc_transform_matrix_t ogc_transform_matrix_t;

/**
 * @brief Create a transform matrix.
 * @return Pointer to newly created matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create(void);

/**
 * @brief Create an identity transform matrix.
 * @return Pointer to newly created identity matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void);

/**
 * @brief Create a translation transform matrix.
 * @param tx Translation in X direction.
 * @param ty Translation in Y direction.
 * @return Pointer to newly created translation matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create_translation(double tx, double ty);

/**
 * @brief Create a scale transform matrix.
 * @param sx Scale factor in X direction.
 * @param sy Scale factor in Y direction.
 * @return Pointer to newly created scale matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create_scale(double sx, double sy);

/**
 * @brief Create a rotation transform matrix.
 * @param angle Rotation angle in radians.
 * @return Pointer to newly created rotation matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_create_rotation(double angle);

/**
 * @brief Destroy a transform matrix.
 * @param matrix Pointer to the matrix to destroy.
 */
SDK_C_API void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix);

/**
 * @brief Multiply two transform matrices and return a new matrix.
 * @param a First matrix.
 * @param b Second matrix.
 * @return Pointer to newly created result matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_multiply_new(const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b);

/**
 * @brief Compute the inverse of a transform matrix.
 * @param matrix Pointer to the matrix.
 * @return Pointer to newly created inverse matrix, or NULL on failure.
 */
SDK_C_API ogc_transform_matrix_t* ogc_transform_matrix_inverse(const ogc_transform_matrix_t* matrix);

/**
 * @brief Transform a point using the matrix.
 * @param matrix Pointer to the matrix.
 * @param x Pointer to X coordinate (modified in place).
 * @param y Pointer to Y coordinate (modified in place).
 */
SDK_C_API void ogc_transform_matrix_transform_point(const ogc_transform_matrix_t* matrix, double* x, double* y);

/**
 * @brief Get the matrix elements.
 * @param matrix Pointer to the matrix.
 * @param elements Array to store 9 matrix elements (3x3 matrix).
 */
SDK_C_API void ogc_transform_matrix_get_elements(const ogc_transform_matrix_t* matrix, double* elements);

/**
 * @brief Set the matrix elements.
 * @param matrix Pointer to the matrix.
 * @param elements Array of 9 matrix elements (3x3 matrix).
 */
SDK_C_API void ogc_transform_matrix_set_elements(ogc_transform_matrix_t* matrix, const double* elements);

/* ============================================================================
 * 22. Batch Memory Management Functions
 * ============================================================================
 * Utility functions for batch destruction of arrays of objects.
 * These functions help prevent memory leaks when dealing with multiple objects.
 */

/**
 * @brief Destroy an array of geometry objects.
 * @param array Pointer to array of geometry pointers.
 * @param count Number of geometries in the array.
 */
SDK_C_API void ogc_geometry_array_destroy(ogc_geometry_t** array, size_t count);

/**
 * @brief Destroy an array of feature objects.
 * @param array Pointer to array of feature pointers.
 * @param count Number of features in the array.
 */
SDK_C_API void ogc_feature_array_destroy(ogc_feature_t** array, size_t count);

/**
 * @brief Destroy an array of string pointers.
 * @param array Pointer to array of string pointers.
 * @param count Number of strings in the array.
 */
SDK_C_API void ogc_string_array_destroy(char** array, size_t count);

/**
 * @brief Destroy an array of layer objects.
 * @param array Pointer to array of layer pointers.
 * @param count Number of layers in the array.
 */
SDK_C_API void ogc_layer_array_destroy(ogc_layer_t** array, size_t count);

/**
 * @brief Destroy an array of alert objects.
 * @param array Pointer to array of alert pointers.
 * @param count Number of alerts in the array.
 */
SDK_C_API void ogc_alert_array_destroy(ogc_alert_t** array, size_t count);

/**
 * @brief Destroy an array of waypoint objects.
 * @param array Pointer to array of waypoint pointers.
 * @param count Number of waypoints in the array.
 */
SDK_C_API void ogc_waypoint_array_destroy(ogc_waypoint_t** array, size_t count);

/**
 * @brief Destroy an array of AIS target objects.
 * @param array Pointer to array of AIS target pointers.
 * @param count Number of AIS targets in the array.
 */
SDK_C_API void ogc_ais_target_array_destroy(ogc_ais_target_t** array, size_t count);

/* ============================================================================
 * 23. Version Information
 * ============================================================================
 */

#define OGC_SDK_VERSION_MAJOR 2
#define OGC_SDK_VERSION_MINOR 3
#define OGC_SDK_VERSION_PATCH 0

/**
 * @brief Free a string allocated by SDK functions.
 * @param str String to free.
 */
SDK_C_API void ogc_free_string(char* str);

/**
 * @brief Free memory allocated by SDK functions.
 * @param ptr Pointer to memory to free.
 */
SDK_C_API void ogc_free_memory(void* ptr);

/**
 * @brief Get the SDK version string.
 * @return Version string in format "major.minor.patch" (caller must free with ogc_free_string).
 */
SDK_C_API char* ogc_sdk_get_version(void);

/**
 * @brief Get the SDK build date.
 * @return Build date string (caller must free with ogc_free_string).
 */
SDK_C_API char* ogc_sdk_get_build_date(void);

/**
 * @brief Get the SDK major version number.
 * @return Major version number.
 */
SDK_C_API int ogc_sdk_get_version_major(void);

/**
 * @brief Get the SDK minor version number.
 * @return Minor version number.
 */
SDK_C_API int ogc_sdk_get_version_minor(void);

/**
 * @brief Get the SDK patch version number.
 * @return Patch version number.
 */
SDK_C_API int ogc_sdk_get_version_patch(void);

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
