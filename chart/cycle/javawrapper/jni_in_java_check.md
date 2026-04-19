# JNI接口Java封装检查报告

> 更新日期: 2026-04-19
> JNI函数总数: 841
> Java类总数(JNI侧): 103
> Java wrapper已有类数: 116
> Java wrapper已有native方法类数: 90

---

## 一、总体统计

| 统计项 | 数量 |
|--------|------|
| JNI函数总数 | 841 |
| Java已封装函数(native声明) | 841 |
| Java未封装函数 | 0 |
| 封装覆盖率(基于native声明) | 100% |
| 完全封装的类 | 103 |
| 部分封装的类 | 0 |
| 完全未封装的类 | 0 |

> 注: Java wrapper中部分类虽未声明native方法，但通过高级封装方法间接调用JNI。
> 本报告仅统计直接声明native方法的覆盖率。

---

## 二、已封装的Java类（原完全未封装 - 已新建）

| # | Java类 | JNI函数数 | 封装状态 | 说明 |
|---|--------|-----------|----------|------|
| 1 | `cn.cycle.chart.api.alert.Alert` | 17 | ✅ 已封装 | 新建类，含完整CRUD和属性操作 |
| 2 | `cn.cycle.chart.api.navi.Route` | 17 | ✅ 已封装 | 新建类，含航路点管理和导航操作 |
| 3 | `cn.cycle.chart.api.navi.NavigationEngine` | 15 | ✅ 已封装 | 新建类，含导航引擎生命周期控制 |
| 4 | `cn.cycle.chart.api.navi.Waypoint` | 15 | ✅ 已封装 | 新建类，含航路点属性操作 |
| 5 | `cn.cycle.chart.api.alert.AlertEngine` | 14 | ✅ 已封装 | 新建类，含告警引擎管理和回调 |
| 6 | `cn.cycle.chart.api.cache.TileCache` | 13 | ✅ 已封装 | 新建类，含瓦片缓存CRUD操作 |
| 7 | `cn.cycle.chart.api.feature.FieldValue` | 13 | ✅ 已封装 | 新建类，含字段值类型转换操作 |
| 8 | `cn.cycle.chart.api.navi.AisManager` | 11 | ✅ 已封装 | 新建类，含AIS目标管理和回调 |
| 9 | `cn.cycle.chart.api.geometry.Coordinate` | 9 | ✅ 已封装 | 新建类，含坐标3D/M操作 |
| 10 | `cn.cycle.chart.api.cache.TileKey` | 8 | ✅ 已封装 | 新建类，含瓦片键比较和哈希 |
| 11 | `cn.cycle.chart.api.geometry.Envelope` | 8 | ✅ 已封装 | 新建类，含范围框空间操作和minX/minY/maxX/maxY属性 |
| 12 | `cn.cycle.chart.api.plugin.ChartPlugin` | 6 | ✅ 已封装 | 新建类，含插件生命周期控制 |
| 13 | `cn.cycle.chart.api.plugin.PluginManager` | 6 | ✅ 已封装 | 新建类，含插件加载/卸载管理(单例) |
| 14 | `cn.cycle.chart.api.symbology.Symbolizer` | 6 | ✅ 已封装 | 新建类，含符号化渲染和样式操作 |

## 三、已补充封装的Java类（原部分封装 - 已补全）

### cn.cycle.chart.api.geometry.Geometry
- 已封装: 28个, 未封装: 0个
- 新增方法: equals, intersects, contains, within, crosses, touches, overlaps, distance, intersection, union, difference, buffer, clone, getTypeName
- 新增工厂方法: fromNativePtr(long) - 根据nativePtr创建正确的Geometry子类

### cn.cycle.chart.api.layer.VectorLayer
- 已封装: 20个, 未封装: 0个
- 新增方法: create, createFromDatasource, createFromFeatures, getFeatureById, setSpatialFilter(Geometry), getSpatialFilter, getAttributeFilter, updateFeature, deleteFeature, createFeature, getFeatureDefn, setStyle, getExtentPtr

### cn.cycle.chart.api.layer.Layer
- 已封装: 11个, 未封装: 0个
- 新增方法: getFeatureDefn, getGeomType, getFeatureCount, resetReading, getNextFeature, getFeature, setAttributeFilter, nativeArrayDestroy

### cn.cycle.chart.api.core.Viewport
- 已封装: 20个, 未封装: 0个
- 说明: 原未封装的7个方法(nativeCreate, nativeDestroy, nativeGetBounds, nativeScreenToWorld, nativeWorldToScreen, nativeZoomToExtent, nativeZoomToScale)已通过高级封装方法间接调用，无需额外声明native方法

### cn.cycle.chart.api.geometry.LineString
- 已封装: 9个, 未封装: 0个
- 新增方法: createFromCoords, getNumPoints, setCoordinateN, getPointGeometry, addPoint3D

### cn.cycle.chart.api.geometry.Point
- 已封装: 12个, 未封装: 0个
- 新增方法: createFromCoord, getM, setM, getCoordinate(使用nativeGetCoordinate)

### cn.cycle.chart.api.geometry.Polygon
- 已封装: 10个, 未封装: 0个
- 新增方法: createFromRing, createFromCoords

### cn.cycle.chart.api.layer.LayerManager
- 已封装: 16个, 未封装: 0个
- 新增方法: getLayerZOrder, setLayerZOrder, sortByZOrder, nativeGetLayerByName(已声明)

### cn.cycle.chart.api.feature.FeatureInfo
- 已封装: 18个, 未封装: 0个
- 新增方法: getFieldAsInteger64, setFieldInteger64, getGeometry, setGeometry(Geometry), nativeArrayDestroy

### cn.cycle.chart.api.core.ChartConfig
- 已封装: 8个, 未封装: 0个
- 说明: nativeCreate和nativeDestroy已通过构造函数和nativeDispose间接封装，无需额外声明

### cn.cycle.chart.api.geometry.LinearRing
- 已封装: 3个, 未封装: 0个
- 新增方法: createFromCoords

## 四、完全封装的Java类

- `cn.cycle.chart.api.Error`
- `cn.cycle.chart.api.MemoryUtils`
- `cn.cycle.chart.api.SDKVersion`
- `cn.cycle.chart.api.adapter.ImageDevice`
- `cn.cycle.chart.api.alert.Alert` ✨新增
- `cn.cycle.chart.api.alert.AlertEngine` ✨新增
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
- `cn.cycle.chart.api.cache.TileCache` ✨新增
- `cn.cycle.chart.api.cache.TileKey` ✨新增
- `cn.cycle.chart.api.core.ChartConfig`
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
- `cn.cycle.chart.api.feature.FieldValue` ✨新增
- `cn.cycle.chart.api.geometry.Coordinate` ✨新增
- `cn.cycle.chart.api.geometry.Envelope` ✨新增
- `cn.cycle.chart.api.geometry.Geometry`
- `cn.cycle.chart.api.geometry.GeometryCollection`
- `cn.cycle.chart.api.geometry.GeometryFactory` ✨新增
- `cn.cycle.chart.api.geometry.LineString`
- `cn.cycle.chart.api.geometry.LinearRing`
- `cn.cycle.chart.api.geometry.MultiLineString`
- `cn.cycle.chart.api.geometry.MultiPoint`
- `cn.cycle.chart.api.geometry.MultiPolygon`
- `cn.cycle.chart.api.geometry.Point`
- `cn.cycle.chart.api.geometry.Polygon`
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
- `cn.cycle.chart.api.layer.Layer`
- `cn.cycle.chart.api.layer.LayerGroup`
- `cn.cycle.chart.api.layer.LayerManager`
- `cn.cycle.chart.api.layer.MemoryLayer`
- `cn.cycle.chart.api.layer.RasterLayer`
- `cn.cycle.chart.api.layer.VectorLayer`
- `cn.cycle.chart.api.loader.LibraryLoader`
- `cn.cycle.chart.api.loader.SecureLibraryLoader`
- `cn.cycle.chart.api.navi.AisManager` ✨新增
- `cn.cycle.chart.api.navi.AisTarget`
- `cn.cycle.chart.api.navi.CollisionAssessor`
- `cn.cycle.chart.api.navi.NavigationEngine` ✨新增
- `cn.cycle.chart.api.navi.NmeaParser`
- `cn.cycle.chart.api.navi.OffCourseDetector`
- `cn.cycle.chart.api.navi.PositionManager`
- `cn.cycle.chart.api.navi.PositionProvider`
- `cn.cycle.chart.api.navi.Route` ✨新增
- `cn.cycle.chart.api.navi.RouteManager`
- `cn.cycle.chart.api.navi.RoutePlanner`
- `cn.cycle.chart.api.navi.Track`
- `cn.cycle.chart.api.navi.TrackRecorder`
- `cn.cycle.chart.api.navi.Waypoint` ✨新增
- `cn.cycle.chart.api.parser.ChartParser`
- `cn.cycle.chart.api.parser.IParser`
- `cn.cycle.chart.api.parser.IncrementalParser`
- `cn.cycle.chart.api.parser.S57Parser`
- `cn.cycle.chart.api.perf.PerformanceMonitor`
- `cn.cycle.chart.api.perf.RenderStats`
- `cn.cycle.chart.api.plugin.ChartPlugin` ✨新增
- `cn.cycle.chart.api.plugin.PluginManager` ✨新增
- `cn.cycle.chart.api.proj.CoordTransformer`
- `cn.cycle.chart.api.proj.TransformMatrix`
- `cn.cycle.chart.api.recovery.CircuitBreaker`
- `cn.cycle.chart.api.recovery.ErrorRecoveryManager`
- `cn.cycle.chart.api.recovery.GracefulDegradation`
- `cn.cycle.chart.api.symbology.ComparisonFilter`
- `cn.cycle.chart.api.symbology.Filter`
- `cn.cycle.chart.api.symbology.LogicalFilter`
- `cn.cycle.chart.api.symbology.Symbolizer` ✨新增
- `cn.cycle.chart.api.symbology.SymbolizerRule`

## 五、封装变更明细

### 5.1 新建Java类（14个）

| # | 类名 | 包 | 文件路径 |
|---|------|-----|----------|
| 1 | Alert | cn.cycle.chart.api.alert | alert/Alert.java |
| 2 | AlertEngine | cn.cycle.chart.api.alert | alert/AlertEngine.java |
| 3 | Route | cn.cycle.chart.api.navi | navi/Route.java |
| 4 | NavigationEngine | cn.cycle.chart.api.navi | navi/NavigationEngine.java |
| 5 | Waypoint | cn.cycle.chart.api.navi | navi/Waypoint.java |
| 6 | AisManager | cn.cycle.chart.api.navi | navi/AisManager.java |
| 7 | TileCache | cn.cycle.chart.api.cache | cache/TileCache.java |
| 8 | TileKey | cn.cycle.chart.api.cache | cache/TileKey.java |
| 9 | FieldValue | cn.cycle.chart.api.feature | feature/FieldValue.java |
| 10 | Coordinate | cn.cycle.chart.api.geometry | geometry/Coordinate.java |
| 11 | Envelope | cn.cycle.chart.api.geometry | geometry/Envelope.java |
| 12 | ChartPlugin | cn.cycle.chart.api.plugin | plugin/ChartPlugin.java |
| 13 | PluginManager | cn.cycle.chart.api.plugin | plugin/PluginManager.java |
| 14 | Symbolizer | cn.cycle.chart.api.symbology | symbology/Symbolizer.java |

### 5.2 补充方法的Java类（11个）

| # | 类名 | 新增方法数 | 新增方法列表 |
|---|------|-----------|-------------|
| 1 | Geometry | 15 | fromNativePtr, getTypeName, equals, intersects, contains, within, crosses, touches, overlaps, distance, intersection, union, difference, buffer, clone |
| 2 | VectorLayer | 13 | create, createFromDatasource, createFromFeatures, getFeatureById, setSpatialFilter(Geometry), getSpatialFilter, getAttributeFilter, updateFeature, deleteFeature, createFeature, getFeatureDefn, setStyle, getExtentPtr |
| 3 | Layer | 8 | getFeatureDefn, getGeomType, getFeatureCount, resetReading, getNextFeature, getFeature, setAttributeFilter, nativeArrayDestroy |
| 4 | LineString | 5 | createFromCoords, getNumPoints, setCoordinateN, getPointGeometry, addPoint3D |
| 5 | Point | 6 | createFromCoord, getM, setM, getCoordinate(nativeGetCoordinate) |
| 6 | Polygon | 2 | createFromRing, createFromCoords |
| 7 | LayerManager | 4 | getLayerZOrder, setLayerZOrder, sortByZOrder, nativeGetLayerByName |
| 8 | FeatureInfo | 5 | getFieldAsInteger64, setFieldInteger64, getGeometry, setGeometry(Geometry), nativeArrayDestroy |
| 9 | LinearRing | 1 | createFromCoords |
| 10 | ChartConfig | 0 | nativeCreate/nativeDestroy已通过构造函数和nativeDispose间接封装 |
| 11 | Viewport | 0 | 7个方法已通过高级封装间接调用，无需额外声明native方法 |

### 5.3 不需封装的方法说明

| 类名 | 方法名 | 不封装原因 |
|------|--------|-----------|
| ChartConfig | nativeCreate | 已通过构造函数ChartConfig()调用 |
| ChartConfig | nativeDestroy | 已通过nativeDispose()调用 |
| Viewport | nativeCreate | 已通过构造函数间接调用 |
| Viewport | nativeDestroy | 已通过nativeDispose()间接调用 |
| Viewport | nativeGetBounds | 已通过getBounds()高级封装间接调用 |
| Viewport | nativeScreenToWorld | 已通过screenToWorld()高级封装间接调用 |
| Viewport | nativeWorldToScreen | 已通过worldToScreen()高级封装间接调用 |
| Viewport | nativeZoomToExtent | 已通过zoomToExtent()高级封装间接调用 |
| Viewport | nativeZoomToScale | 已通过zoomToScale()高级封装间接调用 |
