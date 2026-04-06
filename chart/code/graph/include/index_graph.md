# Graph (Draw) Module - Header Index

## 模块描述

Graph模块（命名空间ogc::draw）提供完整的地图渲染功能，包括符号化引擎、标签引擎、瓦片缓存、坐标转换、交互处理等。支持SLD样式解析、Mapbox样式、S-52海图符号等。

## 核心特性

- 多种符号化器：点、线、面、文本、栅格、图标
- SLD样式解析和Mapbox样式支持
- 标签引擎和冲突检测
- 多级瓦片缓存（内存/磁盘）
- 坐标转换和投影支持
- 交互处理（平移、缩放、选择）
- LOD（细节层次）管理
- 异步渲染和性能监控
- S-52海图符号渲染
- 数据加密和离线存储
- 日夜模式切换
- 航迹记录和回放

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/draw/export.h) | DLL导出宏 | `OGC_GRAPH_API`, `OGC_DRAW_API` |
| [log.h](ogc/draw/log.h) | 日志工具 | `Log` |
| [draw_facade.h](ogc/draw/draw_facade.h) | 绘图门面 | `DrawFacade` |
| [draw_params.h](ogc/draw/draw_params.h) | 绘图参数 | `DrawParams` |
| [draw_style.h](ogc/draw/draw_style.h) | 绘图样式 | `DrawStyle` |
| [draw_context.h](ogc/draw/draw_context.h) | 绘图上下文 | `DrawContext` |
| [draw_device.h](ogc/draw/draw_device.h) | 绘图设备 | `DrawDevice` |
| [draw_engine.h](ogc/draw/draw_engine.h) | 绘图引擎 | `DrawEngine` |
| [draw_result.h](ogc/draw/draw_result.h) | 绘图结果 | `DrawResult` |
| [draw_types.h](ogc/draw/draw_types.h) | 绘图类型 | `DrawTypes` |
| [image_draw.h](ogc/draw/image_draw.h) | 图像绘图 | `ImageDraw` |
| [symbolizer.h](ogc/draw/symbolizer.h) | 符号化器基类 | `Symbolizer` |
| [point_symbolizer.h](ogc/draw/point_symbolizer.h) | 点符号化器 | `PointSymbolizer` |
| [line_symbolizer.h](ogc/draw/line_symbolizer.h) | 线符号化器 | `LineSymbolizer` |
| [polygon_symbolizer.h](ogc/draw/polygon_symbolizer.h) | 面符号化器 | `PolygonSymbolizer` |
| [text_symbolizer.h](ogc/draw/text_symbolizer.h) | 文本符号化器 | `TextSymbolizer` |
| [raster_symbolizer.h](ogc/draw/raster_symbolizer.h) | 栅格符号化器 | `RasterSymbolizer` |
| [icon_symbolizer.h](ogc/draw/icon_symbolizer.h) | 图标符号化器 | `IconSymbolizer` |
| [composite_symbolizer.h](ogc/draw/composite_symbolizer.h) | 组合符号化器 | `CompositeSymbolizer` |
| [symbolizer_rule.h](ogc/draw/symbolizer_rule.h) | 符号化规则 | `SymbolizerRule` |
| [rule_engine.h](ogc/draw/rule_engine.h) | 规则引擎 | `RuleEngine` |
| [sld_parser.h](ogc/draw/sld_parser.h) | SLD解析器 | `SldParser` |
| [mapbox_style_parser.h](ogc/draw/mapbox_style_parser.h) | Mapbox样式解析 | `MapboxStyleParser` |
| [s52_style_manager.h](ogc/draw/s52_style_manager.h) | S52样式管理 | `S52StyleManager` |
| [s52_symbol_renderer.h](ogc/draw/s52_symbol_renderer.h) | S52符号渲染 | `S52SymbolRenderer` |
| [label_engine.h](ogc/draw/label_engine.h) | 标签引擎 | `LabelEngine` |
| [label_placement.h](ogc/draw/label_placement.h) | 标签放置 | `LabelPlacement` |
| [label_conflict.h](ogc/draw/label_conflict.h) | 标签冲突 | `LabelConflict` |
| [tile_key.h](ogc/draw/tile_key.h) | 瓦片键 | `TileKey` |
| [tile_cache.h](ogc/draw/tile_cache.h) | 瓦片缓存 | `TileCache` |
| [tile_renderer.h](ogc/draw/tile_renderer.h) | 瓦片渲染器 | `TileRenderer` |
| [memory_tile_cache.h](ogc/draw/memory_tile_cache.h) | 内存瓦片缓存 | `MemoryTileCache` |
| [disk_tile_cache.h](ogc/draw/disk_tile_cache.h) | 磁盘瓦片缓存 | `DiskTileCache` |
| [multi_level_tile_cache.h](ogc/draw/multi_level_tile_cache.h) | 多级瓦片缓存 | `MultiLevelTileCache` |
| [coordinate_transform.h](ogc/draw/coordinate_transform.h) | 坐标转换 | `CoordinateTransform` |
| [coordinate_transformer.h](ogc/draw/coordinate_transformer.h) | 坐标转换器 | `CoordinateTransformer` |
| [proj_transformer.h](ogc/draw/proj_transformer.h) | 投影转换 | `ProjTransformer` |
| [transform_manager.h](ogc/draw/transform_manager.h) | 转换管理器 | `TransformManager` |
| [coord_system_preset.h](ogc/draw/coord_system_preset.h) | 坐标系预设 | `CoordSystemPreset` |
| [interaction_handler.h](ogc/draw/interaction_handler.h) | 交互处理器 | `IInteractionHandler`, `InteractionEvent` |
| [interaction_feedback.h](ogc/draw/interaction_feedback.h) | 交互反馈 | `FeedbackManager`, `FeedbackItem` |
| [pan_zoom_handler.h](ogc/draw/pan_zoom_handler.h) | 平移缩放处理 | `PanZoomHandler` |
| [selection_handler.h](ogc/draw/selection_handler.h) | 选择处理器 | `SelectionHandler` |
| [hit_test.h](ogc/draw/hit_test.h) | 点击测试 | `HitTest` |
| [location_display_handler.h](ogc/draw/location_display_handler.h) | 位置显示处理 | `LocationDisplayHandler`, `Position`, `Heading`, `Speed` |
| [lod.h](ogc/draw/lod.h) | LOD定义 | `Lod` |
| [lod_manager.h](ogc/draw/lod_manager.h) | LOD管理器 | `LodManager` |
| [render_queue.h](ogc/draw/render_queue.h) | 渲染队列 | `RenderQueue` |
| [render_task.h](ogc/draw/render_task.h) | 渲染任务 | `RenderTask` |
| [basic_render_task.h](ogc/draw/basic_render_task.h) | 基础渲染任务 | `BasicRenderTask` |
| [async_renderer.h](ogc/draw/async_renderer.h) | 异步渲染器 | `AsyncRenderer` |
| [layer_manager.h](ogc/draw/layer_manager.h) | 图层管理器 | `LayerManager` |
| [layer_control_panel.h](ogc/draw/layer_control_panel.h) | 图层控制面板 | `LayerControlPanel` |
| [filter.h](ogc/draw/filter.h) | 过滤器基类 | `Filter` |
| [comparison_filter.h](ogc/draw/comparison_filter.h) | 比较过滤器 | `ComparisonFilter` |
| [logical_filter.h](ogc/draw/logical_filter.h) | 逻辑过滤器 | `LogicalFilter` |
| [spatial_filter.h](ogc/draw/spatial_filter.h) | 空间过滤器 | `SpatialFilter` |
| [clipper.h](ogc/draw/clipper.h) | 裁剪器 | `Clipper` |
| [performance_monitor.h](ogc/draw/performance_monitor.h) | 性能监控 | `PerformanceMonitor` |
| [performance_metrics.h](ogc/draw/performance_metrics.h) | 性能指标 | `PerformanceMetrics` |
| [thread_safe.h](ogc/draw/thread_safe.h) | 线程安全工具 | `ThreadSafe` |
| [data_encryption.h](ogc/draw/data_encryption.h) | 数据加密 | `DataEncryption`, `EncryptionKey` |
| [offline_storage_manager.h](ogc/draw/offline_storage_manager.h) | 离线存储管理 | `OfflineStorageManager`, `OfflineRegion` |
| [offline_sync_manager.h](ogc/draw/offline_sync_manager.h) | 离线同步管理 | `OfflineSyncManager` |
| [day_night_mode_manager.h](ogc/draw/day_night_mode_manager.h) | 日夜模式管理 | `DayNightModeManager`, `ColorScheme` |
| [track_recorder.h](ogc/draw/track_recorder.h) | 航迹记录器 | `TrackRecorder`, `TrackPlayer`, `TrackData` |

---

## 类继承关系图

```
Symbolizer (abstract)
    ├── PointSymbolizer
    ├── LineSymbolizer
    ├── PolygonSymbolizer
    ├── TextSymbolizer
    ├── RasterSymbolizer
    ├── IconSymbolizer
    └── CompositeSymbolizer

TileCache (abstract)
    ├── MemoryTileCache
    ├── DiskTileCache
    └── MultiLevelTileCache

IInteractionHandler (abstract)
    ├── PanZoomHandler
    ├── SelectionHandler
    └── LocationDisplayHandler

Filter (abstract)
    ├── ComparisonFilter
    ├── LogicalFilter
    └── SpatialFilter

RenderTask (abstract)
    └── BasicRenderTask
```

---

## 依赖关系图

```
export.h
    │
    ├──► log.h
    │
    ├──► draw_result.h
    │       │
    │       ├──► draw_types.h ──► draw_style.h
    │       │                          │
    │       │                          └──► draw_params.h
    │       │
    │       ├──► draw_device.h ──► draw_engine.h
    │       │         │
    │       │         └──► draw_context.h
    │       │
    │       └──► symbolizer.h
    │                │
    │                ├──► point_symbolizer.h
    │                ├──► line_symbolizer.h
    │                ├──► polygon_symbolizer.h
    │                ├──► text_symbolizer.h
    │                ├──► raster_symbolizer.h
    │                └──► composite_symbolizer.h

sld_parser.h ──► symbolizer_rule.h ──► rule_engine.h
mapbox_style_parser.h
s52_style_manager.h ──► s52_symbol_renderer.h

label_engine.h ──► label_placement.h ──► label_conflict.h

tile_cache.h
    ├──► memory_tile_cache.h
    ├──► disk_tile_cache.h
    └──► multi_level_tile_cache.h

interaction_handler.h
    ├──► interaction_feedback.h
    ├──► pan_zoom_handler.h
    ├──► selection_handler.h ──► hit_test.h
    └──► location_display_handler.h ──► track_recorder.h

coordinate_transform.h
    ├──► coordinate_transformer.h
    ├──► proj_transformer.h
    └──► transform_manager.h ──► coord_system_preset.h

data_encryption.h
offline_storage_manager.h ──► offline_sync_manager.h
day_night_mode_manager.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | export.h, log.h, draw_facade.h, draw_params.h, draw_style.h, draw_context.h, draw_result.h, image_draw.h |
| **Device/Engine** | draw_device.h, draw_engine.h |
| **Symbolizer** | symbolizer.h, point_symbolizer.h, line_symbolizer.h, polygon_symbolizer.h, text_symbolizer.h, raster_symbolizer.h, icon_symbolizer.h, composite_symbolizer.h |
| **Style** | sld_parser.h, mapbox_style_parser.h, s52_style_manager.h, s52_symbol_renderer.h, symbolizer_rule.h, rule_engine.h |
| **Label** | label_engine.h, label_placement.h, label_conflict.h |
| **Tile** | tile_key.h, tile_cache.h, tile_renderer.h, memory_tile_cache.h, disk_tile_cache.h, multi_level_tile_cache.h |
| **Transform** | coordinate_transform.h, coordinate_transformer.h, proj_transformer.h, transform_manager.h, coord_system_preset.h |
| **Interaction** | interaction_handler.h, interaction_feedback.h, pan_zoom_handler.h, selection_handler.h, hit_test.h, location_display_handler.h |
| **LOD** | lod.h, lod_manager.h |
| **Render** | render_queue.h, render_task.h, basic_render_task.h, async_renderer.h, layer_manager.h, layer_control_panel.h |
| **Filter** | filter.h, comparison_filter.h, logical_filter.h, spatial_filter.h |
| **Utility** | clipper.h, performance_monitor.h, performance_metrics.h, thread_safe.h |
| **Security** | data_encryption.h, offline_storage_manager.h, offline_sync_manager.h |
| **UI** | day_night_mode_manager.h, track_recorder.h |

---

## 关键类

### DrawFacade
**File**: [draw_facade.h](ogc/draw/draw_facade.h)  
**Description**: 绘图门面类，提供统一的绘图接口

```cpp
class DrawFacade {
public:
    static DrawFacade& Instance();
    
    DrawResult Initialize();
    DrawResult Finalize();
    
    DrawContextPtr CreateContext();
    DrawContextPtr CreateContext(DrawDevicePtr device);
    DrawContextPtr CreateContext(DrawDevicePtr device, DrawEnginePtr engine);
    
    DrawDevicePtr CreateDevice(DeviceType type, int width, int height);
    DrawEnginePtr CreateEngine(EngineType type);
    
    DrawResult RegisterDevice(const std::string& name, DrawDevicePtr device);
    DrawResult RegisterEngine(const std::string& name, DrawEnginePtr engine);
    
    DrawResult DrawGeometry(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style);
    DrawResult RenderToImage(const Geometry* geometry, const DrawParams& params, uint8_t* imageData, int width, int height);
};
```

### Symbolizer (抽象基类)
**File**: [symbolizer.h](ogc/draw/symbolizer.h)  
**Description**: 符号化器抽象基类

```cpp
class Symbolizer {
public:
    virtual ~Symbolizer() = default;
    
    virtual SymbolizerType GetType() const = 0;
    virtual std::string GetName() const = 0;
    
    virtual DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) = 0;
    virtual DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) = 0;
    
    virtual bool CanSymbolize(GeomType geomType) const = 0;
    virtual SymbolizerPtr Clone() const = 0;
};
```

### DataEncryption
**File**: [data_encryption.h](ogc/draw/data_encryption.h)  
**Description**: 数据加密类

```cpp
class DataEncryption {
public:
    static std::unique_ptr<DataEncryption> Create();
    
    virtual bool Initialize(const std::string& keyStoragePath) = 0;
    virtual void Shutdown() = 0;
    
    virtual EncryptionKey GenerateKey() = 0;
    virtual bool SetActiveKey(const EncryptionKey& key) = 0;
    
    virtual std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData) = 0;
    
    virtual Coordinate WGS84ToGCJ02(const Coordinate& coord) = 0;
    virtual Coordinate GCJ02ToWGS84(const Coordinate& coord) = 0;
};
```

### OfflineStorageManager
**File**: [offline_storage_manager.h](ogc/draw/offline_storage_manager.h)  
**Description**: 离线存储管理器

```cpp
class OfflineStorageManager {
public:
    static std::unique_ptr<OfflineStorageManager> Create(const std::string& storagePath);
    
    virtual std::string CreateRegion(const std::string& name,
                                      double minLon, double maxLon,
                                      double minLat, double maxLat,
                                      int minZoom, int maxZoom) = 0;
    
    virtual void StartDownload(const std::string& regionId,
                               ProgressCallback progressCallback,
                               CompletionCallback completionCallback) = 0;
    
    virtual bool StoreTile(const std::string& regionId,
                          const TileKey& key,
                          const std::vector<uint8_t>& data) = 0;
};
```

### DayNightModeManager
**File**: [day_night_mode_manager.h](ogc/draw/day_night_mode_manager.h)  
**Description**: 日夜模式管理器

```cpp
class DayNightModeManager {
public:
    static std::unique_ptr<DayNightModeManager> Create();
    
    void SetMode(DisplayMode mode);
    DisplayMode GetMode() const;
    
    void SetDayMode();
    void SetNightMode();
    
    const ColorScheme& GetCurrentScheme() const;
    void SetDayScheme(const ColorScheme& scheme);
    void SetNightScheme(const ColorScheme& scheme);
    
    void StartTransition(DisplayMode targetMode);
    bool IsTransitioning() const;
};
```

### TrackRecorder
**File**: [track_recorder.h](ogc/draw/track_recorder.h)  
**Description**: 航迹记录器

```cpp
class TrackRecorder {
public:
    static std::unique_ptr<TrackRecorder> Create();
    
    void StartRecording(const std::string& trackName = "");
    void StopRecording();
    void PauseRecording();
    void ResumeRecording();
    
    void AddPoint(const Coordinate& position, double heading, double speed);
    
    TrackData GetCurrentTrack() const;
    static TrackData LoadTrack(const std::string& filePath);
    static bool SaveTrack(const TrackData& track, const std::string& filePath);
};

class TrackPlayer {
public:
    void SetTrack(const TrackData& track);
    void Play();
    void Pause();
    void Stop();
    void Seek(double progress);
    void SetPlaybackSpeed(double speed);
};
```

### LocationDisplayHandler
**File**: [location_display_handler.h](ogc/draw/location_display_handler.h)  
**Description**: 位置显示处理器

```cpp
class LocationDisplayHandler : public IInteractionHandler {
public:
    void UpdatePosition(const Position& position);
    void UpdateHeading(const Heading& heading);
    void UpdateSpeed(const Speed& speed);
    
    void SetAutoCenter(bool enabled);
    void SetAutoRotate(bool enabled);
    
    void StartTrackRecording();
    void StopTrackRecording();
    
    const Position& GetPosition() const;
    const Heading& GetHeading() const;
    const Speed& GetSpeed() const;
};
```

---

## 类型定义

### SymbolizerType
```cpp
enum class SymbolizerType {
    kPoint,
    kLine,
    kPolygon,
    kText,
    kRaster,
    kComposite,
    kIcon
};
```

### DeviceType
```cpp
enum class DeviceType {
    kRaster,
    kVector,
    kGPU
};
```

### EngineType
```cpp
enum class EngineType {
    kSimple2D,
    kGPU,
    kCairo,
    kQt,
    kGDIPlus,
    kDirect2D
};
```

### DisplayMode
```cpp
enum class DisplayMode {
    kDay = 0,
    kNight = 1,
    kDusk = 2,
    kDawn = 3,
    kCustom = 4
};
```

### PositionSource
```cpp
enum class PositionSource {
    kNone = 0,
    kInternalGPS,
    kExternalGPS,
    kBeidou,
    kRTK,
    kManual
};
```

### EncryptionAlgorithm
```cpp
enum class EncryptionAlgorithm {
    kAES256_CBC = 0,
    kAES256_GCM = 1,
    kAES256_CTR = 2
};
```

---

## 使用示例

### 基本绘图

```cpp
#include "ogc/draw/draw_facade.h"

using namespace ogc::draw;

auto& facade = DrawFacade::Instance();
facade.Initialize();

// 创建设备
auto device = facade.CreateDevice(DeviceType::kRaster, 800, 600);
auto engine = facade.CreateEngine(EngineType::kSimple2D);
auto context = facade.CreateContext(device, engine);

// 绘制几何
DrawStyle style;
style.strokeColor = Color(255, 0, 0);
style.fillColor = Color(0, 255, 0, 128);

facade.DrawGeometry(context, polygon, style);
```

### 日夜模式切换

```cpp
#include "ogc/draw/day_night_mode_manager.h"

auto manager = DayNightModeManager::Create();

// 切换到夜间模式
manager->SetNightMode();

// 获取当前配色
const auto& scheme = manager->GetCurrentScheme();
Color bgColor = scheme.backgroundColor;
```

### 航迹记录

```cpp
#include "ogc/draw/track_recorder.h"

auto recorder = TrackRecorder::Create();

// 开始记录
recorder->StartRecording("My Track");

// 添加航点
recorder->AddPoint(Coordinate(116.4, 39.9), 45.0, 10.5);

// 停止记录
recorder->StopRecording();

// 保存航迹
auto track = recorder->GetCurrentTrack();
TrackRecorder::SaveTrack(track, "/path/to/track.gpx");
```

### 离线存储

```cpp
#include "ogc/draw/offline_storage_manager.h"

auto storage = OfflineStorageManager::Create("/path/to/storage");

// 创建离线区域
std::string regionId = storage->CreateRegion("Beijing", 
    116.0, 117.0, 39.5, 40.5, 10, 16);

// 开始下载
storage->StartDownload(regionId, 
    [](const DownloadProgress& p) { /* 进度回调 */ },
    [](StorageError err) { /* 完成回调 */ });
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加S52海图支持 |
| v1.2 | 2026-03-10 | Team | 添加Mapbox样式解析 |
| v1.3 | 2026-04-01 | Team | 添加多级瓦片缓存 |
| v1.4 | 2026-04-06 | index-validator | 补充缺失头文件、修正章节顺序、添加新功能类 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.4  
**C++ Standard**: C++11
