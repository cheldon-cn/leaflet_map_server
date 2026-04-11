# C API 内核类映射表

> **版本**: v1.0  
> **创建日期**: 2026-04-11  
> **数据来源**: code\index_all.md

---

## 一、概述

本文档记录了SDK C API与内核C++类之间的映射关系，确保C API封装与内核实现保持一致。

---

## 二、映射规则

| 映射类型 | 说明 | 示例 |
|----------|------|------|
| 结构体 | 简单数据结构直接映射 | `TileKey` → `ogc_tile_key_t` |
| 不透明指针 | 复杂类使用void*封装 | `CNLayer` → `ogc_layer_t` |
| 别名 | typedef定义的类型别名 | `CNVectorLayer` → `ogc_vector_layer_t` |
| 工厂创建 | 通过Create函数创建 | `MemoryTileCache` → `ogc_memory_tile_cache_create` |

---

## 三、模块映射表

### 3.1 Base 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `Envelope` | `ogc_envelope_t` | 结构体 | ✅ |
| `Coordinate` | `ogc_coordinate_t` | 结构体 | ✅ |
| `Color` | `ogc_color_t` | 结构体 | ✅ |
| `Logger` | `ogc_logger_t` | 不透明指针 | ✅ |
| `PerformanceMonitor` | `ogc_performance_monitor_t` | 不透明指针 | ✅ |
| `PerformanceStats` | `ogc_performance_stats_t` | 结构体 | ✅ |

### 3.2 Geom 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `Geometry` | `ogc_geometry_t` | 不透明指针 | ✅ |
| `Point` | `ogc_point_t` | 不透明指针 | ✅ |
| `LineString` | `ogc_linestring_t` | 不透明指针 | ✅ |
| `Polygon` | `ogc_polygon_t` | 不透明指针 | ✅ |
| `MultiPoint` | `ogc_multipoint_t` | 不透明指针 | ✅ |
| `MultiLineString` | `ogc_multilinestring_t` | 不透明指针 | ✅ |
| `MultiPolygon` | `ogc_multipolygon_t` | 不透明指针 | ✅ |
| `GeometryCollection` | `ogc_geometrycollection_t` | 不透明指针 | ✅ |

### 3.3 Cache 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `TileKey` | `ogc_tile_key_t` | 结构体 | ✅ |
| `TileCache` | `ogc_tile_cache_t` | 不透明指针 | ✅ |
| `TileData` | `ogc_tile_data_t` | 不透明指针 | ✅ |
| `MemoryTileCache` | - | 工厂创建 `ogc_memory_tile_cache_create` | ✅ |
| `DiskTileCache` | - | 工厂创建 `ogc_disk_tile_cache_create` | ✅ |
| `MultiLevelTileCache` | `ogc_multi_level_tile_cache_t` | 不透明指针 | ✅ |
| `OfflineStorageManager` | `ogc_offline_storage_t` | 不透明指针 | ✅ |
| `OfflineRegion` | `ogc_offline_region_t` | 不透明指针 | ✅ |
| `DataEncryption` | `ogc_data_encryption_t` | 不透明指针 | ✅ |
| `CacheManager` | `ogc_cache_manager_t` | 不透明指针 | ✅ |

### 3.4 Proj 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `CoordinateTransformer` | `ogc_coord_transformer_t` | 不透明指针 | ✅ |
| `SpatialReference` | `ogc_spatial_ref_t` | 不透明指针 | ✅ |
| `TransformMatrix` | `ogc_transform_matrix_t` | 不透明指针 | ✅ |

### 3.5 Feature 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `CNFeature` | `ogc_feature_t` | 不透明指针 | ✅ |
| `CNFeatureDefn` | `ogc_feature_defn_t` | 不透明指针 | ✅ |
| `CNFieldValue` | `ogc_field_value_t` | 不透明指针 | ✅ |
| `CNFieldDefn` | `ogc_field_defn_t` | 不透明指针 | ✅ |

### 3.6 Layer 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `CNLayer` | `ogc_layer_t` | 不透明指针 | ✅ |
| `CNVectorLayer` | `ogc_vector_layer_t` | 别名 (typedef) | ✅ |
| `CNMemoryLayer` | - | 工厂创建 `ogc_memory_layer_create` | ✅ |
| `CNLayerGroup` | `ogc_layer_group_t` | 不透明指针 | ✅ |
| `CNDataSource` | `ogc_datasource_t` | 不透明指针 | ✅ |
| `CNRasterLayer` | `ogc_raster_layer_t` | 不透明指针 | ✅ |
| `CNShapefileLayer` | - | 工厂创建 `ogc_shapefile_layer_open` | ✅ |
| `CNGeoJSONLayer` | - | 工厂创建 `ogc_geojson_layer_open` | ✅ |
| `CNPostGISLayer` | - | 工厂创建 | ✅ |
| `DriverManager` | `ogc_driver_manager_t` | 不透明指针 | ✅ |
| `Driver` | `ogc_driver_t` | 不透明指针 | ✅ |

### 3.7 Graph 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `TransformManager` | `ogc_transform_manager_t` | 不透明指针 | ✅ |
| `HitTestResult` | `ogc_hit_test_t` | 不透明指针 | ✅ |
| `LabelInfo` | `ogc_label_info_t` | 不透明指针 | ✅ |
| `RenderStats` | `ogc_render_stats_t` | 结构体 | ✅ |
| `ChartViewer` | `ogc_chart_viewer_t` | 不透明指针 | ✅ |
| `Viewport` | `ogc_viewport_t` | 不透明指针 | ✅ |
| `ChartConfig` | `ogc_chart_config_t` | 不透明指针 | ✅ |
| `LayerManager` | `ogc_layer_manager_t` | 不透明指针 | ✅ |
| `LabelEngine` | `ogc_label_engine_t` | 不透明指针 | ✅ |
| `LodManager` | `ogc_lod_manager_t` | 不透明指针 | ✅ |
| `RenderTask` | `ogc_render_task_t` | 不透明指针 | ✅ |
| `RenderQueue` | `ogc_render_queue_t` | 不透明指针 | ✅ |
| `RenderOptimizer` | `ogc_render_optimizer_t` | 不透明指针 | ✅ |

### 3.8 Symbology 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `Filter` | `ogc_filter_t` | 不透明指针 | ✅ |
| `ComparisonFilter` | `ogc_comparison_filter_t` | 不透明指针 | ✅ |
| `SpatialFilter` | `ogc_spatial_filter_t` | 不透明指针 | ✅ |
| `Symbolizer` | `ogc_symbolizer_t` | 不透明指针 | ✅ |
| `SymbolizerRule` | `ogc_symbolizer_rule_t` | 不透明指针 | ✅ |

### 3.9 Database 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `DbConnection` | `ogc_db_connection_t` | 不透明指针 | ✅ |
| `DbConnectionPool` | `ogc_db_pool_t` | 不透明指针 | ✅ |
| `DbTransaction` | `ogc_db_transaction_t` | 不透明指针 | ✅ |

### 3.10 Draw 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `DrawEngine` | `ogc_draw_engine_t` | 不透明指针 | ✅ |
| `DrawContext` | `ogc_draw_context_t` | 不透明指针 | ✅ |
| `DrawDevice` | `ogc_draw_device_t` | 不透明指针 | ✅ |
| `DrawStyle` | `ogc_draw_style_t` | 不透明指针 | ✅ |
| `Font` | `ogc_font_t` | 不透明指针 | ✅ |
| `Image` | `ogc_image_t` | 不透明指针 | ✅ |
| `Pen` | `ogc_pen_t` | 不透明指针 | ✅ |
| `Brush` | `ogc_brush_t` | 不透明指针 | ✅ |

### 3.11 Navi 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `Waypoint` | `ogc_waypoint_t` | 不透明指针 | ✅ |
| `Route` | `ogc_route_t` | 不透明指针 | ✅ |
| `RouteManager` | `ogc_route_manager_t` | 不透明指针 | ✅ |
| `RoutePlanner` | `ogc_route_planner_t` | 不透明指针 | ✅ |
| `AisTarget` | `ogc_ais_target_t` | 不透明指针 | ✅ |
| `AisManager` | `ogc_ais_manager_t` | 不透明指针 | ✅ |
| `NavigationEngine` | `ogc_navigation_engine_t` | 不透明指针 | ✅ |
| `PositionManager` | `ogc_position_manager_t` | 不透明指针 | ✅ |
| `Track` | `ogc_track_t` | 不透明指针 | ✅ |
| `TrackPoint` | `ogc_track_point_t` | 不透明指针 | ✅ |
| `TrackRecorder` | `ogc_track_recorder_t` | 不透明指针 | ✅ |
| `PositionProvider` | `ogc_position_provider_t` | 不透明指针 | ✅ |
| `OffCourseDetector` | `ogc_off_course_detector_t` | 不透明指针 | ✅ |
| `NmeaParser` | `ogc_nmea_parser_t` | 不透明指针 | ✅ |
| `CollisionAssessor` | `ogc_collision_assessor_t` | 不透明指针 | ✅ |
| `CollisionRisk` | `ogc_collision_risk_t` | 结构体 | ✅ |
| `DeviationResult` | `ogc_deviation_result_t` | 结构体 | ✅ |

### 3.12 Alert 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `Alert` | `ogc_alert_t` | 不透明指针 | ✅ |
| `AlertEngine` | `ogc_alert_engine_t` | 不透明指针 | ✅ |
| `CpaCalculator` | - | 函数 `ogc_cpa_calculate` | ✅ |
| `UkcCalculator` | - | 函数 `ogc_ukc_calculate` | ✅ |
| `CpaResult` | `ogc_cpa_result_t` | 结构体 | ✅ |
| `UkcResult` | `ogc_ukc_result_t` | 结构体 | ✅ |

### 3.13 Parser 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `WktParser` | `ogc_wkt_parser_t` | 不透明指针 | ✅ |
| `WkbParser` | `ogc_wkb_parser_t` | 不透明指针 | ✅ |
| `GmlParser` | `ogc_gml_parser_t` | 不透明指针 | ✅ |
| `GeoJsonParser` | `ogc_geojson_parser_t` | 不透明指针 | ✅ |
| `ChartParser` | `ogc_chart_parser_t` | 不透明指针 | ✅ |
| `IParser` | `ogc_iparser_t` | 不透明指针 | ✅ |
| `S57Parser` | `ogc_s57_parser_t` | 不透明指针 | ✅ |
| `IncrementalParser` | `ogc_incremental_parser_t` | 不透明指针 | ✅ |
| `ParseResult` | `ogc_parse_result_t` | 不透明指针 | ✅ |
| `IncrementalParseResult` | `ogc_incremental_parse_result_t` | 结构体 | ✅ |
| `ParseConfig` | `ogc_parse_config_t` | 结构体 | ✅ |

### 3.14 Plugin 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `PluginManager` | `ogc_plugin_manager_t` | 不透明指针 | ✅ |
| `Plugin` | `ogc_plugin_t` | 不透明指针 | ✅ |
| `ChartPlugin` | `ogc_chart_plugin_t` | 不透明指针 | ✅ |
| `ErrorRecoveryManager` | `ogc_error_recovery_manager_t` | 不透明指针 | ✅ |
| `CircuitBreaker` | `ogc_circuit_breaker_t` | 不透明指针 | ✅ |
| `GracefulDegradation` | `ogc_graceful_degradation_t` | 不透明指针 | ✅ |
| `HealthCheck` | `ogc_health_check_t` | 不透明指针 | ✅ |
| `HealthCheckResult` | `ogc_health_check_result_t` | 结构体 | ✅ |
| `LibraryLoader` | `ogc_library_loader_t` | 不透明指针 | ✅ |
| `LibraryHandle` | `ogc_library_handle_t` | 句柄类型 | ✅ |
| `SecureLibraryLoader` | `ogc_secure_library_loader_t` | 不透明指针 | ✅ |

### 3.15 Geom Factory 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `GeometryFactory` | `ogc_geometry_factory_t` | 不透明指针 | ✅ |

### 3.16 Exception 模块

| 内核类 | C API类型 | 映射方式 | 状态 |
|--------|-----------|----------|------|
| `ChartException` | `ogc_chart_exception_t` | 结构体 | ✅ |
| `JniException` | `ogc_jni_exception_t` | 结构体 | ✅ |
| `RenderException` | `ogc_render_exception_t` | 结构体 | ✅ |

---

## 四、命名约定

### 4.1 类型命名

| 规则 | 示例 |
|------|------|
| 内核类使用驼峰命名 | `CNFeature`, `TileKey` |
| C API类型添加`ogc_`前缀和`_t`后缀 | `ogc_feature_t`, `ogc_tile_key_t` |
| 结构体使用`struct`定义 | `typedef struct ogc_tile_key_t { ... }` |
| 不透明指针使用`void*`或前向声明 | `typedef struct ogc_feature_t ogc_feature_t;` |

### 4.2 函数命名

| 规则 | 示例 |
|------|------|
| 创建函数使用`_create`后缀 | `ogc_feature_create()` |
| 销毁函数使用`_destroy`后缀 | `ogc_feature_destroy()` |
| Getter使用`_get_`前缀 | `ogc_feature_get_id()` |
| Setter使用`_set_`前缀 | `ogc_feature_set_geometry()` |
| 索引访问使用`_n`后缀 | `ogc_route_get_waypoint_n()` |

---

## 五、生命周期管理

### 5.1 所有权规则

| 创建方式 | 释放责任 | 示例 |
|----------|----------|------|
| `*_create()` | 调用者负责调用`*_destroy()` | `ogc_feature_create()` / `ogc_feature_destroy()` |
| `*_get_*()` | 返回内部引用，不转移所有权 | `ogc_feature_get_geometry()` |
| `*_clone()` | 调用者负责释放 | `ogc_geometry_clone()` |

### 5.2 内存管理最佳实践

```c
// 正确：创建后释放
ogc_feature_t* feature = ogc_feature_create();
// ... 使用feature
ogc_feature_destroy(feature);

// 正确：克隆后释放
ogc_geometry_t* geom = ogc_feature_get_geometry(feature);
ogc_geometry_t* clone = ogc_geometry_clone(geom);
// ... 使用clone
ogc_geometry_destroy(clone);

// 错误：释放不拥有的对象
ogc_geometry_t* geom = ogc_feature_get_geometry(feature);
ogc_geometry_destroy(geom);  // 错误：geom的所有权属于feature
```

---

## 六、版本历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| v1.0 | 2026-04-11 | 初始版本，整理各模块内核类映射 |
| v1.1 | 2026-04-11 | 补充遗漏的类型映射：Logger、PerformanceMonitor、TransformMatrix、TileData、DriverManager、Driver、ChartViewer、Viewport、ChartConfig、LayerManager、LabelEngine、LodManager、RenderTask、RenderQueue、RenderOptimizer、Pen、Brush、TrackRecorder、PositionProvider、CollisionAssessor、CollisionRisk、DeviationResult、CpaResult、UkcResult、ChartParser、IParser、S57Parser、IncrementalParser、ParseResult、IncrementalParseResult、ParseConfig、ChartPlugin、ErrorRecoveryManager、CircuitBreaker、GracefulDegradation、HealthCheck、HealthCheckResult、LibraryLoader、LibraryHandle、SecureLibraryLoader、GeometryFactory、ChartException、JniException、RenderException |

---

**维护者**: SDK C API Team
