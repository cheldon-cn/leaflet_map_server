# PIMPL 模式使用检查报告

> **检查日期**: 2026-04-15  
> **项目**: OGCGeometry v2.3.0  
> **检查范围**: code\modules.md 中各模块头文件

---

## 一、检查概述

### 1.1 检查目的

检查各模块头文件中类是否采用 PIMPL (Pointer to Implementation) 习惯用法，将实现细节移入 .cpp 文件，以解决以下问题：

1. **DLL 接口警告 (C4251)**: 避免标准库模板类成员导致的 DLL 导出警告
2. **ABI 兼容性**: 隐藏实现细节，保持二进制接口稳定
3. **编译依赖**: 减少头文件依赖，加快编译速度
4. **封装性**: 更好的信息隐藏和封装

### 1.2 检查结果统计

| 分类 | 模块数 | 类数量 |
|------|--------|--------|
| ✅ 已使用 PIMPL | 11 | 58 |
| ❌ 未使用 PIMPL | 3 | 15+ |
| ⚠️ 纯虚接口类 | 5 | 20+ |
| ℹ️ 静态工具类/简单值类 | 4 | 20+ |

---

## 二、已使用 PIMPL 模式的类

### 2.1 graph 模块 (17 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| RenderTask | render/render_task.h | 渲染任务类 |
| AsyncRenderer | render/async_renderer.h | 异步渲染器 |
| AsyncRenderBuilder | render/async_renderer.h | 异步渲染构建器 |
| RenderQueue | render/render_queue.h | 渲染队列 |
| LabelEngine | label/label_engine.h | 标签引擎 |
| LODManager | lod/lod_manager.h | LOD 管理器 |
| TransformManager | util/transform_manager.h | 坐标转换管理器 |
| InteractionHandler | interaction/interaction_handler.h | 交互处理器 |
| InteractionManager | interaction/interaction_handler.h | 交互管理器 |
| SelectionHandler | interaction/selection_handler.h | 选择处理器 |
| PanZoomHandler | interaction/pan_zoom_handler.h | 平移缩放处理器 |
| LocationDisplayHandler | interaction/location_display_handler.h | 位置显示处理器 |
| LayerConfig | layer/layer_manager.h | 图层配置 |
| LayerItem | layer/layer_manager.h | 图层项 |
| LayerManager | layer/layer_manager.h | 图层管理器 |
| LayerControlPanel | layer/layer_control_panel.h | 图层控制面板 |
| DayNightModeManager | util/day_night_mode_manager.h | 昼夜模式管理器 |

### 2.2 symbology 模块 (11 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| Symbolizer | symbolizer/symbolizer.h | 符号化器基类 |
| PointSymbolizer | symbolizer/point_symbolizer.h | 点符号化器 |
| LineSymbolizer | symbolizer/line_symbolizer.h | 线符号化器 |
| PolygonSymbolizer | symbolizer/polygon_symbolizer.h | 多边形符号化器 |
| RasterSymbolizer | symbolizer/raster_symbolizer.h | 栅格符号化器 |
| IconSymbolizer | symbolizer/icon_symbolizer.h | 图标符号化器 |
| SymbolizerRule | filter/symbolizer_rule.h | 符号化规则 |
| LogicalFilter | filter/logical_filter.h | 逻辑过滤器 |
| SpatialFilter | filter/spatial_filter.h | 空间过滤器 |
| S52SymbolRenderer | style/s52_symbol_renderer.h | S52 符号渲染器 |
| SldParser | style/sld_parser.h | SLD 解析器 |

### 2.3 feature 模块 (7 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| CNFeature | feature.h | 核心要素类 |
| CNFeatureDefn | feature_defn.h | 要素定义类 |
| CNFeatureCollection | feature_collection.h | 要素集合类 |
| SpatialQuery | spatial_query.h | 空间查询类 |
| WkbWktConverter | wkb_wkt_converter.h | WKB/WKT 转换器 |
| GeoJsonConverter | geojson_converter.h | GeoJSON 转换器 |
| BatchProcessor | batch_processor.h | 批量处理器 |

### 2.4 navi 模块 (5 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| TrackRecorder | track/track_recorder.h | 航迹记录器 |
| TrackManager | track/track_manager.h | 航迹管理器 |
| RouteManager | route/route_manager.h | 航线管理器 |
| NavigationEngine | navigation/navigation_engine.h | 导航引擎 |
| AISManager | ais/ais_manager.h | AIS 管理器 |

### 2.5 database 模块 (7 个类) ✅ 新增

| 类名 | 头文件 | 说明 |
|------|--------|------|
| PostGISConnection | postgis_connection.h | PostgreSQL 连接类 |
| PostGISStatement | postgis_connection.h | PostgreSQL 语句类 |
| PostGISResultSet | postgis_connection.h | PostgreSQL 结果集类 |
| SpatiaLiteConnection | sqlite_connection.h | SQLite 连接类 |
| SpatiaLiteStatement | sqlite_connection.h | SQLite 语句类 |
| SpatiaLiteResultSet | sqlite_connection.h | SQLite 结果集类 |
| DbConnectionPool | connection_pool.h | 数据库连接池 |

### 2.6 cache 模块 (4 个类) ✅ 新增

| 类名 | 头文件 | 说明 |
|------|--------|------|
| DiskTileCache | tile/disk_tile_cache.h | 磁盘瓦片缓存 |
| MemoryTileCache | tile/memory_tile_cache.h | 内存瓦片缓存 |
| MultiLevelTileCache | tile/multi_level_tile_cache.h | 多级瓦片缓存 |
| CacheManager | cache_manager.h | 缓存管理器 |

### 2.7 draw 模块 (3 个类) ✅ 新增

| 类名 | 头文件 | 说明 |
|------|--------|------|
| Image | image.h | 图像类 |
| Font | font.h | 字体类 |
| Region | region.h | 区域类 |

### 2.8 alert 模块 (4 个类) ✅ 新增

| 类名 | 头文件 | 说明 |
|------|--------|------|
| AlertEngine | alert_engine.h | 预警引擎 |
| WeatherFusion | weather_fusion.h | 天气融合类 |
| NoticeProvider | notice_parser.h | 通知提供者 |
| SpeedZoneMatcher | speed_zone_matcher.h | 速度区域匹配器 |

### 2.9 geom 模块 (2 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| Geometry | geometry.h | 几何基类 |
| Polygon | polygon.h | 多边形类 |

### 2.10 proj 模块 (1 个类) ✅ 新增

| 类名 | 头文件 | 说明 |
|------|--------|------|
| ProjTransformer | proj_transformer.h | 投影转换器 |

### 2.11 layer 模块 (2 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| CNLayerGroup | layer_group.h | 图层组 |
| LayerGroupItem | layer_group.h | 图层组项 |

### 2.12 base 模块 (1 个类)

| 类名 | 头文件 | 说明 |
|------|--------|------|
| Logger | log.h | 日志记录器 |

---

## 三、未使用 PIMPL 模式的类

### 3.1 geom 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| Point | point.h | m_coord | 🟢 简单值类，无需 PIMPL |
| LineString | linestring.h | m_coords | 🟡 可选 |
| LinearRing | linearring.h | (继承自 LineString) | 🟡 可选 |
| MultiPoint | multipoint.h | m_points | 🟡 可选 |
| MultiLineString | multilinestring.h | m_lineStrings | 🟡 可选 |
| MultiPolygon | multipolygon.h | m_polygons | 🟡 可选 |
| GeometryCollection | geometrycollection.h | m_geometries | 🟡 可选 |
| GeometryFactory | factory.h | m_defaultSRID | 🟢 简单值类，无需 PIMPL |
| Envelope | envelope.h | m_min, m_max | 🟢 简单值类，无需 PIMPL |
| Coordinate | coordinate.h | x, y, z | 🟢 简单值类，无需 PIMPL |

### 3.2 draw 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| Clipper | clipper.h | m_clipEnvelope, m_hasClipEnvelope, m_tolerance | 🟡 可选 |
| Color | color.h | m_r, m_g, m_b, m_a | 🟢 简单值类，无需 PIMPL |
| DrawContext | draw_context.h | (纯虚接口类，无成员变量) | ⚪ 无需 PIMPL |
| DrawEngine | draw_engine.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| DrawDevice | draw_device.h | (纯虚接口类) | ⚪ 无需 PIMPL |

### 3.3 cache 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| TileCache | tile/tile_cache.h | (纯虚接口类，无成员变量) | ⚪ 无需 PIMPL |
| TileKey | tile/tile_key.h | x, y, z | 🟢 简单值类，无需 PIMPL |

### 3.4 database 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| DbConnection | connection.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| WkbConverter | wkb_converter.h | (静态工具类) | ⚪ 无需 PIMPL |

### 3.5 alert 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| IWeatherFusion | weather_fusion.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| INoticeParser | notice_parser.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| INoticeProvider | notice_parser.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| NoticeParser | notice_parser.h | (无私有成员变量) | ⚪ 无需 PIMPL |
| ISpeedZoneMatcher | speed_zone_matcher.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| IAlertEngine | alert_engine.h | (纯虚接口类) | ⚪ 无需 PIMPL |

### 3.6 proj 模块

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| CoordinateTransformer | coordinate_transformer.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| CoordinateTransform | coordinate_transform.h | m_matrix, m_inverseMatrix, m_inverseValid | 🟡 可选 |

### 3.7 graph 模块 (剩余未改造类)

| 类名 | 头文件 | 私有成员 | 建议 |
|------|--------|----------|------|
| DrawFacade | draw_facade.h | m_initialized, m_defaultStyle, m_defaultFont, m_defaultColor | 🟡 可选（简单类型成员） |
| LODLevel | lod.h | m_level, m_minScale, m_maxScale, m_resolution | 🟡 可选 |
| BasicRenderTask | basic_render_task.h | m_renderFunction, m_context | 🟡 可选 |
| LabelConflictResolver | label_conflict.h | m_minDistance, m_allowOverlap, m_priorityMode | 🟡 可选 |
| SpatialIndexConflictResolver | label_conflict.h | (继承自 LabelConflictResolver) | 🟡 可选 |
| ImageData | image_draw.h | m_width, m_height, m_channels, m_data | 🟡 可选 |
| ImageDraw | image_draw.h | m_imageData, m_clipX, m_clipY | 🟡 可选 |
| IImageReader | image_draw.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| IImageWriter | image_draw.h | (纯虚接口类) | ⚪ 无需 PIMPL |
| ILayerRenderer | layer_manager.h | (纯虚接口类) | ⚪ 无需 PIMPL |

---

## 四、建议优先级

### 4.1 高优先级 (🔴 建议使用 PIMPL) - 已完成 ✅

以下类已完成 PIMPL 改造：

| 模块 | 类名 | 状态 |
|------|------|------|
| cache | DiskTileCache | ✅ 已完成 |
| cache | MemoryTileCache | ✅ 已完成 |
| cache | MultiLevelTileCache | ✅ 已完成 |
| database | PostGISConnection | ✅ 已完成 |
| database | PostGISStatement | ✅ 已完成 |
| database | PostGISResultSet | ✅ 已完成 |
| database | SpatiaLiteConnection | ✅ 已完成 |
| database | SpatiaLiteStatement | ✅ 已完成 |
| database | SpatiaLiteResultSet | ✅ 已完成 |
| database | DbConnectionPool | ✅ 已完成 |
| proj | ProjTransformer | ✅ 已完成 |
| draw | Image | ✅ 已完成 |
| draw | Font | ✅ 已完成 |
| draw | Region | ✅ 已完成 |
| alert | WeatherFusion | ✅ 已完成 |
| alert | NoticeProvider | ✅ 已完成 |
| alert | SpeedZoneMatcher | ✅ 已完成 |

### 4.2 中优先级 (🟡 可选)

以下类可以考虑使用 PIMPL 模式，但不是必须：

| 模块 | 类名 | 说明 |
|------|------|------|
| geom | LineString | 简单容器类，PIMPL 收益有限 |
| geom | MultiPoint | 简单容器类，PIMPL 收益有限 |
| geom | MultiLineString | 简单容器类，PIMPL 收益有限 |
| geom | MultiPolygon | 简单容器类，PIMPL 收益有限 |
| geom | GeometryCollection | 简单容器类，PIMPL 收益有限 |
| draw | Clipper | 成员变量较少，PIMPL 收益有限 |
| proj | CoordinateTransform | 成员变量较少，PIMPL 收益有限 |
| graph | DrawFacade | 成员变量均为简单类型，PIMPL 收益有限 |
| graph | LODLevel | 简单值类，PIMPL 收益有限 |
| graph | BasicRenderTask | 成员变量较少，PIMPL 收益有限 |
| graph | LabelConflictResolver | 成员变量较少，PIMPL 收益有限 |
| graph | ImageData | 简单数据类，PIMPL 收益有限 |
| graph | ImageDraw | 成员变量较少，PIMPL 收益有限 |

### 4.3 低优先级 (🟢 无需 PIMPL)

以下类是简单值类，无需使用 PIMPL 模式：

| 模块 | 类名 | 说明 |
|------|------|------|
| geom | Point | 仅包含一个 Coordinate 成员 |
| geom | GeometryFactory | 仅包含一个 int 成员 |
| geom | Envelope | 仅包含边界值成员 |
| geom | Coordinate | 仅包含坐标值成员 |
| draw | Color | 仅包含 4 个 uint8_t 成员 |
| cache | TileKey | 仅包含 3 个 int 成员 |
| base | Version | 仅包含 3 个 int 成员 |

### 4.4 无需 PIMPL (⚪)

以下类无需使用 PIMPL 模式：

| 类型 | 类名 |
|------|------|
| 纯虚接口类 | DrawContext, DrawEngine, DrawDevice, TileCache, DbConnection, CoordinateTransformer, IWeatherFusion, INoticeParser, INoticeProvider, ISpeedZoneMatcher, IAlertEngine, IImageReader, IImageWriter, ILayerRenderer |
| 静态工具类 | WkbConverter, WeatherQualityAssessor, WeatherInterpolator, WeatherAlertMatcher, NoticeMatcher, NoticeFormatter, SpeedZoneIndex, SpeedViolationChecker |
| 无私有成员变量 | NoticeParser |

---

## 五、PIMPL 模式实现指南

### 5.1 标准实现

```cpp
// header.h
class OGC_XXX_API MyClass {
public:
    MyClass();
    ~MyClass();
    
    // 公共接口方法...
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// source.cpp
struct MyClass::Impl {
    // 所有私有成员变量
    std::string name;
    std::vector<int> data;
    std::mutex mutex;
};

MyClass::MyClass() : impl_(std::make_unique<Impl>()) {}
MyClass::~MyClass() = default;
```

### 5.2 注意事项

1. **析构函数**: 必须在 .cpp 文件中定义（或使用 `= default`），因为 `unique_ptr<Impl>` 需要完整的 Impl 类型定义
2. **拷贝/移动**: 需要显式定义拷贝构造函数和赋值运算符
3. **性能**: 每次访问成员变量需要一次指针解引用，可能有轻微性能损失
4. **调试**: 调试时需要额外跳转才能看到成员变量

---

## 六、结论

### 6.1 当前状态

| 模块 | 已使用 PIMPL | 未使用 PIMPL | 完成率 |
|------|--------------|--------------|--------|
| graph | 17 | 10 | 63% |
| feature | 7 | 0 | 100% |
| symbology | 11 | 0 | 100% |
| navi | 5 | 0 | 100% |
| database | 7 | 0 | **100%** ✅ |
| cache | 4 | 0 | **100%** ✅ |
| draw | 3 | 2 | **60%** |
| alert | 4 | 0 | **100%** ✅ |
| proj | 1 | 1 | **50%** |
| geom | 2 | 10 | 17% |
| base | 1 | 0 | 100% |
| layer | 2 | 0 | 100% |

**总计**: 58 个类已使用 PIMPL，约 15 个类未使用 PIMPL（含可选类）

### 6.2 主要成果

1. **高优先级类全部完成**: database、cache、alert、proj、draw 模块的高优先级类已完成 PIMPL 改造
2. **graph 模块核心类完成**: 17 个核心类已完成 PIMPL 改造
3. **feature/symbology/navi 模块**: 已全面采用 PIMPL 模式
4. **DLL 接口警告大幅减少**: 高优先级类的 C4251 警告已消除

### 6.3 后续建议

1. **中优先级类按需改造**: geom、graph 模块的可选类可根据实际需求决定是否改造
2. **保持 PIMPL 模式一致性**: 新增类应优先考虑使用 PIMPL 模式
3. **定期检查**: 每次版本发布前检查新增类是否需要 PIMPL 改造

---

**检查人**: PIMPL 模式检查工具  
**报告生成日期**: 2026-04-15
