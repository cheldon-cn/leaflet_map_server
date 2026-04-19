# JNI接口Java封装检查报告

> 生成日期: 2026-04-19
> JNI函数总数: 841
> Java类总数(JNI侧): 103
> Java wrapper已有类数: 116
> Java wrapper已有native方法类数: 90

---

## 一、总体统计

| 统计项 | 数量 |
|--------|------|
| JNI函数总数 | 841 |
| Java已封装函数(native声明) | 614 |
| Java未封装函数 | 227 |
| 封装覆盖率(基于native声明) | 73.0% |
| 完全封装的类 | 78 |
| 部分封装的类 | 11 |
| 完全未封装的类 | 14 |

> 注: Java wrapper中部分类虽未声明native方法，但通过高级封装方法间接调用JNI。
> 本报告仅统计直接声明native方法的覆盖率。

---

## 二、完全未封装的Java类（需新建）

| # | Java类 | JNI函数数 | 优先级 | 封装建议 |
|---|--------|-----------|--------|----------|
| 1 | `cn.cycle.chart.api.alert.Alert` | 17 | P2-低 | 需新建 |
| 2 | `cn.cycle.chart.api.navi.Route` | 17 | P1-中 | 需新建 |
| 3 | `cn.cycle.chart.api.navi.NavigationEngine` | 15 | P1-中 | 需新建 |
| 4 | `cn.cycle.chart.api.navi.Waypoint` | 15 | P1-中 | 需新建 |
| 5 | `cn.cycle.chart.api.alert.AlertEngine` | 14 | P2-低 | 需新建 |
| 6 | `cn.cycle.chart.api.cache.TileCache` | 13 | P1-中 | 需新建 |
| 7 | `cn.cycle.chart.api.feature.FieldValue` | 13 | P0-高 | 需新建 |
| 8 | `cn.cycle.chart.api.navi.AisManager` | 11 | P1-中 | 需新建 |
| 9 | `cn.cycle.chart.api.geometry.Coordinate` | 9 | P0-高 | 需新建 |
| 10 | `cn.cycle.chart.api.cache.TileKey` | 8 | P1-中 | 需新建 |
| 11 | `cn.cycle.chart.api.geometry.Envelope` | 8 | P0-高 | 需新建 |
| 12 | `cn.cycle.chart.api.plugin.ChartPlugin` | 6 | P2-低 | 需新建 |
| 13 | `cn.cycle.chart.api.plugin.PluginManager` | 6 | P2-低 | 需新建 |
| 14 | `cn.cycle.chart.api.symbology.Symbolizer` | 6 | P2-低 | 需新建 |

## 三、部分封装的Java类（需补充）

### cn.cycle.chart.api.geometry.Geometry
- 已封装: 14个, 未封装: 14个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jboolean | nativeEquals | `Java_cn_cycle_chart_api_geometry_Geometry_nativeEquals` | geometry_spatial_jni.cpp |
| 2 | jboolean | nativeIntersects | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersects` | geometry_spatial_jni.cpp |
| 3 | jboolean | nativeContains | `Java_cn_cycle_chart_api_geometry_Geometry_nativeContains` | geometry_spatial_jni.cpp |
| 4 | jboolean | nativeWithin | `Java_cn_cycle_chart_api_geometry_Geometry_nativeWithin` | geometry_spatial_jni.cpp |
| 5 | jboolean | nativeCrosses | `Java_cn_cycle_chart_api_geometry_Geometry_nativeCrosses` | geometry_spatial_jni.cpp |
| 6 | jboolean | nativeTouches | `Java_cn_cycle_chart_api_geometry_Geometry_nativeTouches` | geometry_spatial_jni.cpp |
| 7 | jboolean | nativeOverlaps | `Java_cn_cycle_chart_api_geometry_Geometry_nativeOverlaps` | geometry_spatial_jni.cpp |
| 8 | jdouble | nativeDistance | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDistance` | geometry_spatial_jni.cpp |
| 9 | jlong | nativeIntersection | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersection` | geometry_spatial_jni.cpp |
| 10 | jlong | nativeUnion | `Java_cn_cycle_chart_api_geometry_Geometry_nativeUnion` | geometry_spatial_jni.cpp |
| 11 | jlong | nativeDifference | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDifference` | geometry_spatial_jni.cpp |
| 12 | jlong | nativeBuffer | `Java_cn_cycle_chart_api_geometry_Geometry_nativeBuffer` | geometry_spatial_jni.cpp |
| 13 | jlong | nativeClone | `Java_cn_cycle_chart_api_geometry_Geometry_nativeClone` | geometry_spatial_jni.cpp |
| 14 | jstring | nativeGetTypeName | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetTypeName` | geometry_spatial_jni.cpp |

### cn.cycle.chart.api.layer.VectorLayer
- 已封装: 7个, 未封装: 13个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | jlong | nativeCreateFromDatasource | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromDatasource` | feature_layer_ext_jni.cpp |
| 3 | jlong | nativeGetFeatureById | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureById` | feature_layer_ext_jni.cpp |
| 4 | void | nativeSetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilter` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureDefn` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetSpatialFilter` | feature_layer_ext_jni.cpp |
| 7 | jstring | nativeGetAttributeFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetAttributeFilter` | feature_layer_ext_jni.cpp |
| 8 | jboolean | nativeUpdateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeUpdateFeature` | feature_layer_ext_jni.cpp |
| 9 | jboolean | nativeDeleteFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeDeleteFeature` | feature_layer_ext_jni.cpp |
| 10 | jboolean | nativeCreateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFeature` | feature_layer_ext_jni.cpp |
| 11 | jboolean | nativeSetStyle | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetStyle` | feature_layer_ext_jni.cpp |
| 12 | jlong | nativeCreateFromFeatures | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromFeatures` | layer_jni.cpp |
| 13 | jlong | nativeGetExtentPtr | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetExtentPtr` | layer_jni.cpp |

### cn.cycle.chart.api.layer.Layer
- 已封装: 3个, 未封装: 8个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureDefn` | feature_layer_ext_jni.cpp |
| 2 | jint | nativeGetGeomType | `Java_cn_cycle_chart_api_layer_Layer_nativeGetGeomType` | feature_layer_ext_jni.cpp |
| 3 | jlong | nativeGetFeatureCount | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureCount` | feature_layer_ext_jni.cpp |
| 4 | void | nativeResetReading | `Java_cn_cycle_chart_api_layer_Layer_nativeResetReading` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetNextFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetNextFeature` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeature` | feature_layer_ext_jni.cpp |
| 7 | void | nativeSetAttributeFilter | `Java_cn_cycle_chart_api_layer_Layer_nativeSetAttributeFilter` | feature_layer_ext_jni.cpp |
| 8 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_layer_Layer_nativeArrayDestroy` | layer_jni.cpp |

### cn.cycle.chart.api.core.Viewport
- 已封装: 13个, 未封装: 7个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_Viewport_nativeCreate` | viewport_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_Viewport_nativeDestroy` | viewport_jni.cpp |
| 3 | jlong | nativeGetBounds | `Java_cn_cycle_chart_api_core_Viewport_nativeGetBounds` | viewport_jni.cpp |
| 4 | jdoubleArray | nativeScreenToWorld | `Java_cn_cycle_chart_api_core_Viewport_nativeScreenToWorld` | viewport_jni.cpp |
| 5 | jdoubleArray | nativeWorldToScreen | `Java_cn_cycle_chart_api_core_Viewport_nativeWorldToScreen` | viewport_jni.cpp |
| 6 | jint | nativeZoomToExtent | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToExtent` | viewport_jni.cpp |
| 7 | jint | nativeZoomToScale | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToScale` | viewport_jni.cpp |

### cn.cycle.chart.api.geometry.LineString
- 已封装: 3个, 未封装: 6个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeGetNumPoints | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetNumPoints` | geometry_jni.cpp |
| 3 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreateFromCoords` | geometry_spatial_jni.cpp |
| 4 | void | nativeAddPoint3D | `Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint3D` | geometry_spatial_jni.cpp |
| 5 | void | nativeSetPointN | `Java_cn_cycle_chart_api_geometry_LineString_nativeSetPointN` | geometry_spatial_jni.cpp |
| 6 | jlong | nativeGetPointGeometry | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointGeometry` | geometry_spatial_jni.cpp |

### cn.cycle.chart.api.geometry.Point
- 已封装: 6个, 未封装: 6个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeCreate3D | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate3D` | geometry_jni.cpp |
| 3 | jlong | nativeCreateFromCoord | `Java_cn_cycle_chart_api_geometry_Point_nativeCreateFromCoord` | geometry_spatial_jni.cpp |
| 4 | jdouble | nativeGetM | `Java_cn_cycle_chart_api_geometry_Point_nativeGetM` | geometry_spatial_jni.cpp |
| 5 | void | nativeSetM | `Java_cn_cycle_chart_api_geometry_Point_nativeSetM` | geometry_spatial_jni.cpp |
| 6 | jdoubleArray | nativeGetCoordinate | `Java_cn_cycle_chart_api_geometry_Point_nativeGetCoordinate` | geometry_spatial_jni.cpp |

### cn.cycle.chart.api.geometry.Polygon
- 已封装: 6个, 未封装: 4个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeCreateFromRing | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromRing` | geometry_spatial_jni.cpp |
| 3 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromCoords` | geometry_spatial_jni.cpp |
| 4 | jboolean | nativeIsValid | `Java_cn_cycle_chart_api_geometry_Polygon_nativeIsValid` | geometry_spatial_jni.cpp |

### cn.cycle.chart.api.layer.LayerManager
- 已封装: 12个, 未封装: 4个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetLayerByName | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerByName` | layer_jni.cpp |
| 2 | jint | nativeGetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerZOrder` | layer_jni.cpp |
| 3 | void | nativeSetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerZOrder` | layer_jni.cpp |
| 4 | void | nativeSortByZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSortByZOrder` | layer_jni.cpp |

### cn.cycle.chart.api.feature.FeatureInfo
- 已封装: 15个, 未封装: 3个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetFieldAsInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsInteger64` | feature_jni.cpp |
| 2 | void | nativeSetFieldInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldInteger64` | feature_jni.cpp |
| 3 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeArrayDestroy` | feature_jni.cpp |

### cn.cycle.chart.api.core.ChartConfig
- 已封装: 6个, 未封装: 2个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_ChartConfig_nativeCreate` | chart_config_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_ChartConfig_nativeDestroy` | chart_config_jni.cpp |

### cn.cycle.chart.api.geometry.LinearRing
- 已封装: 1个, 未封装: 2个

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreateFromCoords` | geometry_spatial_jni.cpp |

## 四、完全封装的Java类

- `cn.cycle.chart.api.Error`
- `cn.cycle.chart.api.MemoryUtils`
- `cn.cycle.chart.api.SDKVersion`
- `cn.cycle.chart.api.adapter.ImageDevice`
- `cn.cycle.chart.api.alert.CpaCalculator`
- `cn.cycle.chart.api.alert.UkcCalculator`
- `cn.cycle.chart.api.base.Logger`
- `cn.cycle.chart.api.base.PerformanceMonitor`
- `cn.cycle.chart.api.cache.CacheManager`
- `cn.cycle.chart.api.cache.DataEncryption`
- `cn.cycle.chart.api.cache.DiskTileCache`
- `cn.cycle.chart.api.cache.MemoryTileCache`
- `cn.cycle.chart.api.cache.MultiLevelTileCache`
- `cn.cycle.chart.api.cache.OfflineRegion`
- `cn.cycle.chart.api.cache.OfflineStorage`
- `cn.cycle.chart.api.core.ChartViewer`
- `cn.cycle.chart.api.core.RenderContext`
- `cn.cycle.chart.api.draw.Brush`
- `cn.cycle.chart.api.draw.Color`
- `cn.cycle.chart.api.draw.DrawDevice`
- `cn.cycle.chart.api.draw.DrawEngine`
- `cn.cycle.chart.api.draw.DrawStyle`
- `cn.cycle.chart.api.draw.Font`
- `cn.cycle.chart.api.draw.Image`
- `cn.cycle.chart.api.draw.ImageDevice`
- `cn.cycle.chart.api.draw.Pen`
- `cn.cycle.chart.api.exception.ChartException`
- `cn.cycle.chart.api.exception.JniException`
- `cn.cycle.chart.api.exception.RenderException`
- `cn.cycle.chart.api.feature.Feature`
- `cn.cycle.chart.api.feature.FeatureDefn`
- `cn.cycle.chart.api.feature.FieldDefn`
- `cn.cycle.chart.api.geometry.GeometryCollection`
- `cn.cycle.chart.api.geometry.GeometryFactory`
- `cn.cycle.chart.api.geometry.MultiLineString`
- `cn.cycle.chart.api.geometry.MultiPoint`
- `cn.cycle.chart.api.geometry.MultiPolygon`
- `cn.cycle.chart.api.graph.HitTest`
- `cn.cycle.chart.api.graph.LODManager`
- `cn.cycle.chart.api.graph.LabelEngine`
- `cn.cycle.chart.api.graph.LabelInfo`
- `cn.cycle.chart.api.graph.RenderOptimizer`
- `cn.cycle.chart.api.graph.RenderQueue`
- `cn.cycle.chart.api.graph.RenderTask`
- `cn.cycle.chart.api.graph.TransformManager`
- `cn.cycle.chart.api.health.HealthCheck`
- `cn.cycle.chart.api.layer.DataSource`
- `cn.cycle.chart.api.layer.DriverManager`
- `cn.cycle.chart.api.layer.LayerGroup`
- `cn.cycle.chart.api.layer.MemoryLayer`
- `cn.cycle.chart.api.layer.RasterLayer`
- `cn.cycle.chart.api.loader.LibraryLoader`
- `cn.cycle.chart.api.loader.SecureLibraryLoader`
- `cn.cycle.chart.api.navi.AisTarget`
- `cn.cycle.chart.api.navi.CollisionAssessor`
- `cn.cycle.chart.api.navi.NmeaParser`
- `cn.cycle.chart.api.navi.OffCourseDetector`
- `cn.cycle.chart.api.navi.PositionManager`
- `cn.cycle.chart.api.navi.PositionProvider`
- `cn.cycle.chart.api.navi.RouteManager`
- `cn.cycle.chart.api.navi.RoutePlanner`
- `cn.cycle.chart.api.navi.Track`
- `cn.cycle.chart.api.navi.TrackRecorder`
- `cn.cycle.chart.api.parser.ChartParser`
- `cn.cycle.chart.api.parser.IParser`
- `cn.cycle.chart.api.parser.IncrementalParser`
- `cn.cycle.chart.api.parser.S57Parser`
- `cn.cycle.chart.api.perf.PerformanceMonitor`
- `cn.cycle.chart.api.perf.RenderStats`
- `cn.cycle.chart.api.proj.CoordTransformer`
- `cn.cycle.chart.api.proj.TransformMatrix`
- `cn.cycle.chart.api.recovery.CircuitBreaker`
- `cn.cycle.chart.api.recovery.ErrorRecoveryManager`
- `cn.cycle.chart.api.recovery.GracefulDegradation`
- `cn.cycle.chart.api.symbology.ComparisonFilter`
- `cn.cycle.chart.api.symbology.Filter`
- `cn.cycle.chart.api.symbology.LogicalFilter`
- `cn.cycle.chart.api.symbology.SymbolizerRule`

## 五、未封装接口按模块分类

### 5.1 Alert 模块（2个类，需封装31个方法）

#### cn.cycle.chart.api.alert.Alert

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_alert_Alert_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_alert_Alert_nativeDestroy` | ❌ 未封装 |
| 3 | jint | nativeGetType | `Java_cn_cycle_chart_api_alert_Alert_nativeGetType` | ❌ 未封装 |
| 4 | jint | nativeGetLevel | `Java_cn_cycle_chart_api_alert_Alert_nativeGetLevel` | ❌ 未封装 |
| 5 | jstring | nativeGetMessage | `Java_cn_cycle_chart_api_alert_Alert_nativeGetMessage` | ❌ 未封装 |
| 6 | jdouble | nativeGetTimestamp | `Java_cn_cycle_chart_api_alert_Alert_nativeGetTimestamp` | ❌ 未封装 |
| 7 | jlong | nativeGetId | `Java_cn_cycle_chart_api_alert_Alert_nativeGetId` | ❌ 未封装 |
| 8 | jint | nativeGetSeverity | `Java_cn_cycle_chart_api_alert_Alert_nativeGetSeverity` | ❌ 未封装 |
| 9 | void | nativeSetSeverity | `Java_cn_cycle_chart_api_alert_Alert_nativeSetSeverity` | ❌ 未封装 |
| 10 | jdoubleArray | nativeGetPosition | `Java_cn_cycle_chart_api_alert_Alert_nativeGetPosition` | ❌ 未封装 |
| 11 | void | nativeSetPosition | `Java_cn_cycle_chart_api_alert_Alert_nativeSetPosition` | ❌ 未封装 |
| 12 | void | nativeSetId | `Java_cn_cycle_chart_api_alert_Alert_nativeSetId` | ❌ 未封装 |
| 13 | void | nativeSetMessage | `Java_cn_cycle_chart_api_alert_Alert_nativeSetMessage` | ❌ 未封装 |
| 14 | void | nativeSetTimestamp | `Java_cn_cycle_chart_api_alert_Alert_nativeSetTimestamp` | ❌ 未封装 |
| 15 | void | nativeSetAcknowledged | `Java_cn_cycle_chart_api_alert_Alert_nativeSetAcknowledged` | ❌ 未封装 |
| 16 | jboolean | nativeIsAcknowledged | `Java_cn_cycle_chart_api_alert_Alert_nativeIsAcknowledged` | ❌ 未封装 |
| 17 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_alert_Alert_nativeArrayDestroy` | ❌ 未封装 |

#### cn.cycle.chart.api.alert.AlertEngine

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeDestroy` | ❌ 未封装 |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeInitialize` | ❌ 未封装 |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeShutdown` | ❌ 未封装 |
| 5 | void | nativeCheckAll | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeCheckAll` | ❌ 未封装 |
| 6 | jlongArray | nativeGetActiveAlertIds | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetActiveAlertIds` | ❌ 未封装 |
| 7 | void | nativeAcknowledgeAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeAcknowledgeAlert` | ❌ 未封装 |
| 8 | jint | nativeAddAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeAddAlert` | ❌ 未封装 |
| 9 | jint | nativeRemoveAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeRemoveAlert` | ❌ 未封装 |
| 10 | jlong | nativeGetAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlert` | ❌ 未封装 |
| 11 | jint | nativeGetAlertCount | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlertCount` | ❌ 未封装 |
| 12 | void | nativeClearAlerts | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeClearAlerts` | ❌ 未封装 |
| 13 | void | nativeSetCallback | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeSetCallback` | ❌ 未封装 |
| 14 | void | nativeFreeAlerts | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeFreeAlerts` | ❌ 未封装 |

### 5.2 Cache 模块（2个类，需封装21个方法）

#### cn.cycle.chart.api.cache.TileCache

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_TileCache_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_TileCache_nativeDestroy` | ❌ 未封装 |
| 3 | jboolean | nativeHasTile | `Java_cn_cycle_chart_api_cache_TileCache_nativeHasTile` | ❌ 未封装 |
| 4 | jlong | nativeGetTilePtr | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetTilePtr` | ❌ 未封装 |
| 5 | void | nativePutTile | `Java_cn_cycle_chart_api_cache_TileCache_nativePutTile` | ❌ 未封装 |
| 6 | void | nativeRemoveTile | `Java_cn_cycle_chart_api_cache_TileCache_nativeRemoveTile` | ❌ 未封装 |
| 7 | void | nativeClear | `Java_cn_cycle_chart_api_cache_TileCache_nativeClear` | ❌ 未封装 |
| 8 | jlong | nativeGetSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetSize` | ❌ 未封装 |
| 9 | jlong | nativeGetMaxSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetMaxSize` | ❌ 未封装 |
| 10 | void | nativeSetMaxSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeSetMaxSize` | ❌ 未封装 |
| 11 | jint | nativeGetCount | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetCount` | ❌ 未封装 |
| 12 | jlong | nativeGetCapacity | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetCapacity` | ❌ 未封装 |
| 13 | void | nativeSetCapacity | `Java_cn_cycle_chart_api_cache_TileCache_nativeSetCapacity` | ❌ 未封装 |

#### cn.cycle.chart.api.cache.TileKey

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jobject | nativeCreate | `Java_cn_cycle_chart_api_cache_TileKey_nativeCreate` | ❌ 未封装 |
| 2 | jboolean | nativeEquals | `Java_cn_cycle_chart_api_cache_TileKey_nativeEquals` | ❌ 未封装 |
| 3 | jstring | nativeToString | `Java_cn_cycle_chart_api_cache_TileKey_nativeToString` | ❌ 未封装 |
| 4 | jlong | nativeToIndex | `Java_cn_cycle_chart_api_cache_TileKey_nativeToIndex` | ❌ 未封装 |
| 5 | jobject | nativeGetParent | `Java_cn_cycle_chart_api_cache_TileKey_nativeGetParent` | ❌ 未封装 |
| 6 | jobjectArray | nativeGetChildren | `Java_cn_cycle_chart_api_cache_TileKey_nativeGetChildren` | ❌ 未封装 |
| 7 | jobject | nativeFromString | `Java_cn_cycle_chart_api_cache_TileKey_nativeFromString` | ❌ 未封装 |
| 8 | jlong | nativeToEnvelope | `Java_cn_cycle_chart_api_cache_TileKey_nativeToEnvelope` | ❌ 未封装 |

### 5.3 Core 模块（2个类，需封装9个方法）

#### cn.cycle.chart.api.core.ChartConfig

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_ChartConfig_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_ChartConfig_nativeDestroy` | ❌ 未封装 |

#### cn.cycle.chart.api.core.Viewport

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_Viewport_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_Viewport_nativeDestroy` | ❌ 未封装 |
| 3 | jlong | nativeGetBounds | `Java_cn_cycle_chart_api_core_Viewport_nativeGetBounds` | ❌ 未封装 |
| 4 | jdoubleArray | nativeScreenToWorld | `Java_cn_cycle_chart_api_core_Viewport_nativeScreenToWorld` | ❌ 未封装 |
| 5 | jdoubleArray | nativeWorldToScreen | `Java_cn_cycle_chart_api_core_Viewport_nativeWorldToScreen` | ❌ 未封装 |
| 6 | jint | nativeZoomToExtent | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToExtent` | ❌ 未封装 |
| 7 | jint | nativeZoomToScale | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToScale` | ❌ 未封装 |

### 5.4 Feature 模块（2个类，需封装16个方法）

#### cn.cycle.chart.api.feature.FieldValue

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_feature_FieldValue_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_feature_FieldValue_nativeDestroy` | ❌ 未封装 |
| 3 | jboolean | nativeIsNull | `Java_cn_cycle_chart_api_feature_FieldValue_nativeIsNull` | ❌ 未封装 |
| 4 | jboolean | nativeIsSet | `Java_cn_cycle_chart_api_feature_FieldValue_nativeIsSet` | ❌ 未封装 |
| 5 | jint | nativeGetAsInteger | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger` | ❌ 未封装 |
| 6 | jlong | nativeGetAsInteger64 | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger64` | ❌ 未封装 |
| 7 | jdouble | nativeGetAsReal | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsReal` | ❌ 未封装 |
| 8 | jstring | nativeGetAsString | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsString` | ❌ 未封装 |
| 9 | void | nativeSetInteger | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger` | ❌ 未封装 |
| 10 | void | nativeSetInteger64 | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger64` | ❌ 未封装 |
| 11 | void | nativeSetReal | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetReal` | ❌ 未封装 |
| 12 | void | nativeSetString | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetString` | ❌ 未封装 |
| 13 | void | nativeSetNull | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetNull` | ❌ 未封装 |

#### cn.cycle.chart.api.feature.FeatureInfo

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeGetFieldAsInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsInteger64` | ❌ 未封装 |
| 2 | void | nativeSetFieldInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldInteger64` | ❌ 未封装 |
| 3 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeArrayDestroy` | ❌ 未封装 |

### 5.5 Geometry 模块（7个类，需封装49个方法）

#### cn.cycle.chart.api.geometry.Coordinate

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeCreate3D | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate3D` | ❌ 未封装 |
| 3 | jlong | nativeCreateM | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreateM` | ❌ 未封装 |
| 4 | void | nativeDestroy | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeDestroy` | ❌ 未封装 |
| 5 | jdouble | nativeGetX | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetX` | ❌ 未封装 |
| 6 | jdouble | nativeGetY | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetY` | ❌ 未封装 |
| 7 | jdouble | nativeGetZ | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetZ` | ❌ 未封装 |
| 8 | jdouble | nativeGetM | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetM` | ❌ 未封装 |
| 9 | jdouble | nativeDistance | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeDistance` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.Envelope

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_Envelope_nativeCreateFromCoords` | ❌ 未封装 |
| 2 | jdouble | nativeGetWidth | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetWidth` | ❌ 未封装 |
| 3 | jdouble | nativeGetHeight | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetHeight` | ❌ 未封装 |
| 4 | jdouble | nativeGetArea | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetArea` | ❌ 未封装 |
| 5 | jboolean | nativeContains | `Java_cn_cycle_chart_api_geometry_Envelope_nativeContains` | ❌ 未封装 |
| 6 | jboolean | nativeIntersects | `Java_cn_cycle_chart_api_geometry_Envelope_nativeIntersects` | ❌ 未封装 |
| 7 | void | nativeExpand | `Java_cn_cycle_chart_api_geometry_Envelope_nativeExpand` | ❌ 未封装 |
| 8 | jdoubleArray | nativeGetCenter | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetCenter` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.Geometry

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jboolean | nativeEquals | `Java_cn_cycle_chart_api_geometry_Geometry_nativeEquals` | ❌ 未封装 |
| 2 | jboolean | nativeIntersects | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersects` | ❌ 未封装 |
| 3 | jboolean | nativeContains | `Java_cn_cycle_chart_api_geometry_Geometry_nativeContains` | ❌ 未封装 |
| 4 | jboolean | nativeWithin | `Java_cn_cycle_chart_api_geometry_Geometry_nativeWithin` | ❌ 未封装 |
| 5 | jboolean | nativeCrosses | `Java_cn_cycle_chart_api_geometry_Geometry_nativeCrosses` | ❌ 未封装 |
| 6 | jboolean | nativeTouches | `Java_cn_cycle_chart_api_geometry_Geometry_nativeTouches` | ❌ 未封装 |
| 7 | jboolean | nativeOverlaps | `Java_cn_cycle_chart_api_geometry_Geometry_nativeOverlaps` | ❌ 未封装 |
| 8 | jdouble | nativeDistance | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDistance` | ❌ 未封装 |
| 9 | jlong | nativeIntersection | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersection` | ❌ 未封装 |
| 10 | jlong | nativeUnion | `Java_cn_cycle_chart_api_geometry_Geometry_nativeUnion` | ❌ 未封装 |
| 11 | jlong | nativeDifference | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDifference` | ❌ 未封装 |
| 12 | jlong | nativeBuffer | `Java_cn_cycle_chart_api_geometry_Geometry_nativeBuffer` | ❌ 未封装 |
| 13 | jlong | nativeClone | `Java_cn_cycle_chart_api_geometry_Geometry_nativeClone` | ❌ 未封装 |
| 14 | jstring | nativeGetTypeName | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetTypeName` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.LineString

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeGetNumPoints | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetNumPoints` | ❌ 未封装 |
| 3 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreateFromCoords` | ❌ 未封装 |
| 4 | void | nativeAddPoint3D | `Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint3D` | ❌ 未封装 |
| 5 | void | nativeSetPointN | `Java_cn_cycle_chart_api_geometry_LineString_nativeSetPointN` | ❌ 未封装 |
| 6 | jlong | nativeGetPointGeometry | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointGeometry` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.LinearRing

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreateFromCoords` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.Point

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeCreate3D | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate3D` | ❌ 未封装 |
| 3 | jlong | nativeCreateFromCoord | `Java_cn_cycle_chart_api_geometry_Point_nativeCreateFromCoord` | ❌ 未封装 |
| 4 | jdouble | nativeGetM | `Java_cn_cycle_chart_api_geometry_Point_nativeGetM` | ❌ 未封装 |
| 5 | void | nativeSetM | `Java_cn_cycle_chart_api_geometry_Point_nativeSetM` | ❌ 未封装 |
| 6 | jdoubleArray | nativeGetCoordinate | `Java_cn_cycle_chart_api_geometry_Point_nativeGetCoordinate` | ❌ 未封装 |

#### cn.cycle.chart.api.geometry.Polygon

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeCreateFromRing | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromRing` | ❌ 未封装 |
| 3 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromCoords` | ❌ 未封装 |
| 4 | jboolean | nativeIsValid | `Java_cn_cycle_chart_api_geometry_Polygon_nativeIsValid` | ❌ 未封装 |

### 5.6 Layer 模块（3个类，需封装25个方法）

#### cn.cycle.chart.api.layer.Layer

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureDefn` | ❌ 未封装 |
| 2 | jint | nativeGetGeomType | `Java_cn_cycle_chart_api_layer_Layer_nativeGetGeomType` | ❌ 未封装 |
| 3 | jlong | nativeGetFeatureCount | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureCount` | ❌ 未封装 |
| 4 | void | nativeResetReading | `Java_cn_cycle_chart_api_layer_Layer_nativeResetReading` | ❌ 未封装 |
| 5 | jlong | nativeGetNextFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetNextFeature` | ❌ 未封装 |
| 6 | jlong | nativeGetFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeature` | ❌ 未封装 |
| 7 | void | nativeSetAttributeFilter | `Java_cn_cycle_chart_api_layer_Layer_nativeSetAttributeFilter` | ❌ 未封装 |
| 8 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_layer_Layer_nativeArrayDestroy` | ❌ 未封装 |

#### cn.cycle.chart.api.layer.LayerManager

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeGetLayerByName | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerByName` | ❌ 未封装 |
| 2 | jint | nativeGetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerZOrder` | ❌ 未封装 |
| 3 | void | nativeSetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerZOrder` | ❌ 未封装 |
| 4 | void | nativeSortByZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSortByZOrder` | ❌ 未封装 |

#### cn.cycle.chart.api.layer.VectorLayer

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreate` | ❌ 未封装 |
| 2 | jlong | nativeCreateFromDatasource | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromDatasource` | ❌ 未封装 |
| 3 | jlong | nativeGetFeatureById | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureById` | ❌ 未封装 |
| 4 | void | nativeSetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilter` | ❌ 未封装 |
| 5 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureDefn` | ❌ 未封装 |
| 6 | jlong | nativeGetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetSpatialFilter` | ❌ 未封装 |
| 7 | jstring | nativeGetAttributeFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetAttributeFilter` | ❌ 未封装 |
| 8 | jboolean | nativeUpdateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeUpdateFeature` | ❌ 未封装 |
| 9 | jboolean | nativeDeleteFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeDeleteFeature` | ❌ 未封装 |
| 10 | jboolean | nativeCreateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFeature` | ❌ 未封装 |
| 11 | jboolean | nativeSetStyle | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetStyle` | ❌ 未封装 |
| 12 | jlong | nativeCreateFromFeatures | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromFeatures` | ❌ 未封装 |
| 13 | jlong | nativeGetExtentPtr | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetExtentPtr` | ❌ 未封装 |

### 5.7 Navi 模块（4个类，需封装58个方法）

#### cn.cycle.chart.api.navi.AisManager

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_AisManager_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_AisManager_nativeDestroy` | ❌ 未封装 |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_navi_AisManager_nativeInitialize` | ❌ 未封装 |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_navi_AisManager_nativeShutdown` | ❌ 未封装 |
| 5 | jlong | nativeGetTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetTarget` | ❌ 未封装 |
| 6 | jintArray | nativeGetAllTargetMmsi | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetAllTargetMmsi` | ❌ 未封装 |
| 7 | jint | nativeGetTargetCount | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetTargetCount` | ❌ 未封装 |
| 8 | jint | nativeAddTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeAddTarget` | ❌ 未封装 |
| 9 | jint | nativeRemoveTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeRemoveTarget` | ❌ 未封装 |
| 10 | void | nativeSetCallback | `Java_cn_cycle_chart_api_navi_AisManager_nativeSetCallback` | ❌ 未封装 |
| 11 | void | nativeFreeTargets | `Java_cn_cycle_chart_api_navi_AisManager_nativeFreeTargets` | ❌ 未封装 |

#### cn.cycle.chart.api.navi.NavigationEngine

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeDestroy` | ❌ 未封装 |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeInitialize` | ❌ 未封装 |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeShutdown` | ❌ 未封装 |
| 5 | void | nativeSetRoute | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeSetRoute` | ❌ 未封装 |
| 6 | jlong | nativeGetRoute | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetRoute` | ❌ 未封装 |
| 7 | void | nativeStart | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStart` | ❌ 未封装 |
| 8 | void | nativeStop | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStop` | ❌ 未封装 |
| 9 | void | nativePause | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativePause` | ❌ 未封装 |
| 10 | void | nativeResume | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeResume` | ❌ 未封装 |
| 11 | jint | nativeGetStatus | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetStatus` | ❌ 未封装 |
| 12 | jlong | nativeGetCurrentWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCurrentWaypoint` | ❌ 未封装 |
| 13 | jdouble | nativeGetDistanceToWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetDistanceToWaypoint` | ❌ 未封装 |
| 14 | jdouble | nativeGetBearingToWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetBearingToWaypoint` | ❌ 未封装 |
| 15 | jdouble | nativeGetCrossTrackError | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCrossTrackError` | ❌ 未封装 |

#### cn.cycle.chart.api.navi.Route

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_Route_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_Route_nativeDestroy` | ❌ 未封装 |
| 3 | jstring | nativeGetId | `Java_cn_cycle_chart_api_navi_Route_nativeGetId` | ❌ 未封装 |
| 4 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_Route_nativeGetName` | ❌ 未封装 |
| 5 | void | nativeSetName | `Java_cn_cycle_chart_api_navi_Route_nativeSetName` | ❌ 未封装 |
| 6 | jint | nativeGetStatus | `Java_cn_cycle_chart_api_navi_Route_nativeGetStatus` | ❌ 未封装 |
| 7 | jdouble | nativeGetTotalDistance | `Java_cn_cycle_chart_api_navi_Route_nativeGetTotalDistance` | ❌ 未封装 |
| 8 | jint | nativeGetWaypointCount | `Java_cn_cycle_chart_api_navi_Route_nativeGetWaypointCount` | ❌ 未封装 |
| 9 | jlong | nativeGetWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeGetWaypoint` | ❌ 未封装 |
| 10 | void | nativeAddWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeAddWaypoint` | ❌ 未封装 |
| 11 | void | nativeRemoveWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeRemoveWaypoint` | ❌ 未封装 |
| 12 | jlong | nativeGetCurrentWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeGetCurrentWaypoint` | ❌ 未封装 |
| 13 | jint | nativeAdvanceToNextWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeAdvanceToNextWaypoint` | ❌ 未封装 |
| 14 | void | nativeInsertWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeInsertWaypoint` | ❌ 未封装 |
| 15 | void | nativeClear | `Java_cn_cycle_chart_api_navi_Route_nativeClear` | ❌ 未封装 |
| 16 | void | nativeReverse | `Java_cn_cycle_chart_api_navi_Route_nativeReverse` | ❌ 未封装 |
| 17 | jlong | nativeGetEta | `Java_cn_cycle_chart_api_navi_Route_nativeGetEta` | ❌ 未封装 |

#### cn.cycle.chart.api.navi.Waypoint

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_Waypoint_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_Waypoint_nativeDestroy` | ❌ 未封装 |
| 3 | jdouble | nativeGetLatitude | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLatitude` | ❌ 未封装 |
| 4 | jdouble | nativeGetLongitude | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLongitude` | ❌ 未封装 |
| 5 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetName` | ❌ 未封装 |
| 6 | void | nativeSetName | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetName` | ❌ 未封装 |
| 7 | jboolean | nativeIsArrival | `Java_cn_cycle_chart_api_navi_Waypoint_nativeIsArrival` | ❌ 未封装 |
| 8 | void | nativeSetArrivalRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetArrivalRadius` | ❌ 未封装 |
| 9 | jdouble | nativeGetArrivalRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetArrivalRadius` | ❌ 未封装 |
| 10 | void | nativeSetTurnRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetTurnRadius` | ❌ 未封装 |
| 11 | jdouble | nativeGetTurnRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetTurnRadius` | ❌ 未封装 |
| 12 | void | nativeSetType | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetType` | ❌ 未封装 |
| 13 | jint | nativeGetType | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetType` | ❌ 未封装 |
| 14 | void | nativeSetDescription | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetDescription` | ❌ 未封装 |
| 15 | jstring | nativeGetDescription | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetDescription` | ❌ 未封装 |

### 5.8 Plugin 模块（2个类，需封装12个方法）

#### cn.cycle.chart.api.plugin.ChartPlugin

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeCreate` | ❌ 未封装 |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeDestroy` | ❌ 未封装 |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetName` | ❌ 未封装 |
| 4 | jstring | nativeGetVersion | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetVersion` | ❌ 未封装 |
| 5 | jint | nativeInitialize | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeInitialize` | ❌ 未封装 |
| 6 | void | nativeShutdown | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeShutdown` | ❌ 未封装 |

#### cn.cycle.chart.api.plugin.PluginManager

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetInstance` | ❌ 未封装 |
| 2 | jint | nativeLoadPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeLoadPlugin` | ❌ 未封装 |
| 3 | jint | nativeUnloadPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeUnloadPlugin` | ❌ 未封装 |
| 4 | jint | nativeGetPluginCount | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginCount` | ❌ 未封装 |
| 5 | jstring | nativeGetPluginName | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginName` | ❌ 未封装 |
| 6 | jlong | nativeGetPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPlugin` | ❌ 未封装 |

### 5.9 Symbology 模块（1个类，需封装6个方法）

#### cn.cycle.chart.api.symbology.Symbolizer

| # | 返回类型 | 方法名 | JNI函数名 | 封装状态 |
|---|----------|--------|-----------|----------|
| 1 | jboolean | nativeSymbolize | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSymbolize` | ❌ 未封装 |
| 2 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeCreate` | ❌ 未封装 |
| 3 | void | nativeDestroy | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeDestroy` | ❌ 未封装 |
| 4 | jint | nativeGetType | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetType` | ❌ 未封装 |
| 5 | jlong | nativeGetStyle | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetStyle` | ❌ 未封装 |
| 6 | void | nativeSetStyle | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSetStyle` | ❌ 未封装 |
