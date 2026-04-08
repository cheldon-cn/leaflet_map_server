# ogc_graph 模块 - 头文件索引

## 模块描述

ogc_graph 是 OGC 图表系统的**地图渲染核心模块**，提供地图渲染、图层管理、标签引擎、交互处理和 LOD 管理等核心功能。作为整个系统的渲染引擎，负责将地理数据转换为可视化图像，是用户交互的直接入口。

## 核心特性

- **渲染引擎**：高性能地图渲染，支持并发渲染和任务队列
- **图层管理**：多图层叠加、可见性控制、透明度管理
- **标签引擎**：智能标签避让、沿线标注、冲突检测
- **交互处理**：平移、缩放、选择、点击测试
- **LOD管理**：多级细节控制、动态切换
- **DrawFacade**：统一的绘图接口封装

---

## 头文件清单

### 渲染核心

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/graph/export.h) | DLL导出宏 | `OGC_GRAPH_API` |
| [render/draw_facade.h](ogc/graph/render/draw_facade.h) | 绘图门面 | `DrawFacade` |
| [render/draw_params.h](ogc/graph/render/draw_params.h) | 绘图参数 | `DrawParams` |
| [render/render_task.h](ogc/graph/render/render_task.h) | 渲染任务 | `RenderTask`, `RenderTaskState` |
| [render/render_queue.h](ogc/graph/render/render_queue.h) | 渲染队列 | `RenderQueue` |
| [render/basic_render_task.h](ogc/graph/render/basic_render_task.h) | 基础渲染任务 | `BasicRenderTask` |
| [render/async_renderer.h](ogc/graph/render/async_renderer.h) | 异步渲染器 | `AsyncRenderer` |

### 图层管理

| File | Description | Core Classes |
|------|-------------|--------------|
| [layer/layer_manager.h](ogc/graph/layer/layer_manager.h) | 图层管理器 | `LayerManager`, `LayerConfig` |
| [layer/layer_control_panel.h](ogc/graph/layer/layer_control_panel.h) | 图层控制面板 | `LayerControlPanel` |

### 标签引擎

| File | Description | Core Classes |
|------|-------------|--------------|
| [label/label_engine.h](ogc/graph/label/label_engine.h) | 标签引擎 | `LabelEngine` |
| [label/label_info.h](ogc/graph/label/label_info.h) | 标签信息 | `LabelInfo` |
| [label/label_placement.h](ogc/graph/label/label_placement.h) | 标签放置 | `LabelPlacement` |
| [label/label_conflict.h](ogc/graph/label/label_conflict.h) | 标签冲突 | `LabelConflictResolver` |

### 交互处理

| File | Description | Core Classes |
|------|-------------|--------------|
| [interaction/pan_zoom_handler.h](ogc/graph/interaction/pan_zoom_handler.h) | 平移缩放处理 | `PanZoomHandler` |
| [interaction/selection_handler.h](ogc/graph/interaction/selection_handler.h) | 选择处理 | `SelectionHandler` |
| [interaction/hit_test.h](ogc/graph/interaction/hit_test.h) | 点击测试 | `HitTest` |
| [interaction/location_display_handler.h](ogc/graph/interaction/location_display_handler.h) | 位置显示 | `LocationDisplayHandler` |
| [interaction/interaction_handler.h](ogc/graph/interaction/interaction_handler.h) | 交互处理器基类 | `InteractionHandler` |
| [interaction/interaction_feedback.h](ogc/graph/interaction/interaction_feedback.h) | 交互反馈 | `InteractionFeedback` |

### LOD管理

| File | Description | Core Classes |
|------|-------------|--------------|
| [lod/lod_manager.h](ogc/graph/lod/lod_manager.h) | LOD管理器 | `LODManager` |
| [lod/lod.h](ogc/graph/lod/lod.h) | LOD定义 | `LODLevel`, `LODStrategy` |

### 工具类

| File | Description | Core Classes |
|------|-------------|--------------|
| [util/transform_manager.h](ogc/graph/util/transform_manager.h) | 变换管理器 | `TransformManager` |
| [util/day_night_mode_manager.h](ogc/graph/util/day_night_mode_manager.h) | 日夜模式管理 | `DayNightModeManager` |
| [util/image_draw.h](ogc/graph/util/image_draw.h) | 图像绘制 | `ImageDraw` |

---

## 类继承关系图

```
DrawFacade (单例)
    └── 绘图接口封装

RenderTask
    └── BasicRenderTask

RenderQueue
    └── 任务队列管理

LayerManager
    └── 图层管理

LabelEngine
    └── 标签生成与放置

LODManager
    └── 多级细节管理

PanZoomHandler
SelectionHandler
HitTest
    └── 交互处理器
```

---

## 依赖关系图

```
ogc_graph
    │
    ├── ogc_base (基础工具)
    │       ├── Logger
    │       └── ThreadSafe
    │
    ├── ogc_proj (坐标转换)
    │       └── CoordinateTransformer
    │
    ├── ogc_cache (缓存)
    │       └── TileCache
    │
    ├── ogc_symbology (符号化)
    │       ├── Symbolizer
    │       └── Filter
    │
    ├── ogc_geometry (几何)
    │       ├── Geometry
    │       └── Envelope
    │
    ├── ogc_feature (要素)
    │       └── CNFeature
    │
    ├── ogc_layer (图层)
    │       └── CNLayer
    │
    └── ogc_draw (绘图引擎)
            ├── DrawContext
            ├── DrawDevice
            └── DrawEngine
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Render** | draw_facade.h, draw_params.h, render_task.h, render_queue.h, render_thread_pool.h |
| **Layer** | layer_manager.h, layer_control_panel.h |
| **Label** | label_engine.h, label_info.h, label_placement.h |
| **Interaction** | pan_zoom_handler.h, selection_handler.h, hit_test.h, location_display_handler.h |
| **LOD** | lod_manager.h, lod.h |
| **Transform** | transform_manager.h |
| **Build** | export.h |

---

## 关键类

### DrawFacade (绘图门面)

**File**: [draw_facade.h](ogc/graph/render/draw_facade.h)  
**Description**: 统一的绘图接口封装，单例模式

```cpp
namespace ogc {
namespace graph {

class OGC_GRAPH_API DrawFacade {
public:
    static DrawFacade& Instance();
    
    DrawResult Initialize();
    DrawResult Finalize();
    bool IsInitialized() const;
    
    // 创建上下文
    ogc::draw::DrawContextPtr CreateContext();
    ogc::draw::DrawContextPtr CreateContext(ogc::draw::DrawDevicePtr device);
    ogc::draw::DrawContextPtr CreateContext(ogc::draw::DrawDevicePtr device, 
                                             ogc::draw::DrawEnginePtr engine);
    
    // 创建设备和引擎
    ogc::draw::DrawDevicePtr CreateDevice(ogc::draw::DeviceType type, 
                                           int width = 800, int height = 600);
    ogc::draw::DrawEnginePtr CreateEngine(ogc::draw::EngineType type);
    
    // 注册设备和引擎
    DrawResult RegisterDevice(const std::string& name, ogc::draw::DrawDevicePtr device);
    DrawResult RegisterEngine(const std::string& name, ogc::draw::DrawEnginePtr engine);
    
    // 获取设备和引擎
    ogc::draw::DrawDevicePtr GetDevice(const std::string& name);
    ogc::draw::DrawEnginePtr GetEngine(const std::string& name);
    
    // 绘制几何对象
    DrawResult DrawGeometry(ogc::draw::DrawDevicePtr device, 
                            const Geometry* geometry, 
                            const ogc::draw::DrawStyle& style);
    DrawResult DrawGeometry(ogc::draw::DrawContextPtr context, 
                            const Geometry* geometry, 
                            const ogc::draw::DrawStyle& style);
    
    // 渲染到图像
    DrawResult RenderToImage(const Geometry* geometry, const DrawParams& params, 
                              const ogc::draw::DrawStyle& style, 
                              uint8_t* imageData, int width, int height, int channels);
    DrawResult RenderToImage(const std::vector<Geometry*>& geometries, const DrawParams& params,
                              const std::vector<ogc::draw::DrawStyle>& styles, uint8_t* imageData, 
                              int width, int height, int channels);
    
    // 默认设置
    void SetDefaultDrawStyle(const ogc::draw::DrawStyle& style);
    ogc::draw::DrawStyle GetDefaultDrawStyle() const;
    void SetDefaultFont(const ogc::draw::Font& font);
    ogc::draw::Font GetDefaultFont() const;
    void SetDefaultColor(const ogc::draw::Color& color);
    ogc::draw::Color GetDefaultColor() const;
    void SetDefaultDpi(double dpi);
    double GetDefaultDpi() const;
    void SetDefaultBackground(const ogc::draw::Color& color);
    ogc::draw::Color GetDefaultBackground() const;
    
    // 工厂方法
    ogc::draw::DrawStyle CreateStrokeStyle(uint32_t color, double width);
    ogc::draw::DrawStyle CreateFillStyle(uint32_t color);
    ogc::draw::DrawStyle CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor);
    ogc::draw::Font CreateFont(const std::string& family, double size, 
                                bool bold = false, bool italic = false);
    ogc::draw::Color CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    ogc::draw::Color CreateColorFromHex(const std::string& hex);
    DrawParams CreateDrawParams(const Envelope& extent, int width, int height, double dpi = 96.0);
    
    // 版本信息
    static std::string GetVersion();
    static std::string GetBuildInfo();
};

} // namespace graph
} // namespace ogc
```

---

### RenderTask (渲染任务)

**File**: [render_task.h](ogc/graph/render/render_task.h)  
**Description**: 渲染任务抽象

```cpp
enum class RenderTaskState {
    kPending = 0,
    kQueued = 1,
    kRunning = 2,
    kCompleted = 3,
    kFailed = 4,
    kCancelled = 5
};

enum class RenderTaskPriority {
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kUrgent = 3
};

struct RenderTaskStats {
    int64_t queueTime;
    int64_t startTime;
    int64_t endTime;
    int64_t renderTime;
    size_t featureCount;
    size_t geometryCount;
    size_t memoryUsage;
};

class OGC_GRAPH_API RenderTask : public std::enable_shared_from_this<RenderTask> {
public:
    RenderTask();
    RenderTask(const std::string& id, const DrawParams& params);
    
    void SetId(const std::string& id);
    std::string GetId() const;
    
    void SetParams(const DrawParams& params);
    DrawParams GetParams() const;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    void SetPriority(RenderTaskPriority priority);
    RenderTaskPriority GetPriority() const;
    
    void SetState(RenderTaskState state);
    RenderTaskState GetState() const;
    
    void SetProgress(double progress);
    double GetProgress() const;
    
    void SetResult(const ogc::draw::DrawResult& result);
    ogc::draw::DrawResult GetResult() const;
    
    void SetCallback(RenderTaskCallback callback);
    
    virtual void Execute() = 0;
    virtual void Cancel();
    
    static RenderTaskPtr Create(const std::string& id, const DrawParams& params);
};

typedef std::shared_ptr<RenderTask> RenderTaskPtr;
```

---

### RenderQueue (渲染队列)

**File**: [render_queue.h](ogc/graph/render/render_queue.h)  
**Description**: 渲染任务队列管理

```cpp
struct RenderQueueStats {
    size_t totalTasks;
    size_t pendingTasks;
    size_t runningTasks;
    size_t completedTasks;
    size_t failedTasks;
    size_t cancelledTasks;
    int64_t totalWaitTime;
    int64_t totalRenderTime;
};

class OGC_GRAPH_API RenderQueue {
public:
    RenderQueue();
    explicit RenderQueue(size_t maxQueueSize);
    
    bool Enqueue(RenderTaskPtr task);
    RenderTaskPtr Dequeue();
    RenderTaskPtr TryDequeue(int64_t timeoutMs = 0);
    
    RenderTaskPtr Peek() const;
    
    bool IsEmpty() const;
    size_t GetSize() const;
    size_t GetMaxSize() const;
    void SetMaxSize(size_t maxSize);
    
    void Clear();
    void CancelAll();
    
    void RemoveTask(const std::string& taskId);
    RenderTaskPtr GetTask(const std::string& taskId) const;
    bool HasTask(const std::string& taskId) const;
    
    RenderQueueStats GetStats() const;
    void ResetStats();
    
    void SetOnTaskEnqueued(TaskEventHandler handler);
    void SetOnTaskDequeued(TaskEventHandler handler);
    void SetOnTaskCompleted(TaskEventHandler handler);
    void SetOnTaskFailed(TaskEventHandler handler);
    
    void SetPriorityMode(bool enable);
    bool IsPriorityModeEnabled() const;
    
    void Pause();
    void Resume();
    bool IsPaused() const;
    
    void WaitForNotEmpty();
    bool WaitForNotEmpty(int64_t timeoutMs);
    void NotifyAll();
    
    static RenderQueuePtr Create();
    static RenderQueuePtr Create(size_t maxQueueSize);
};

typedef std::shared_ptr<RenderQueue> RenderQueuePtr;
```

---

### LayerManager (图层管理器)

**File**: [layer_manager.h](ogc/graph/layer/layer_manager.h)  
**Description**: 多图层管理

```cpp
enum class LayerVisibility {
    kVisible,
    kHidden,
    kConditional
};

struct LayerInfo {
    std::string name;
    int index;
    LayerVisibility visibility;
    double minScale;
    double maxScale;
    double opacity;
    bool selectable;
    bool editable;
    bool hasLabels;
};

class OGC_GRAPH_API LayerManager {
public:
    LayerManager();
    ~LayerManager();
    
    // 图层操作
    void AddLayer(CNLayer* layer, int index = -1);
    void RemoveLayer(const std::string& name);
    void RemoveLayer(int index);
    void ClearLayers();
    
    // 图层查询
    CNLayer* GetLayer(const std::string& name) const;
    CNLayer* GetLayer(int index) const;
    std::vector<CNLayer*> GetLayers() const;
    std::vector<std::string> GetLayerNames() const;
    int GetLayerIndex(const std::string& name) const;
    size_t GetLayerCount() const;
    bool HasLayer(const std::string& name) const;
    
    // 图层顺序
    void MoveLayer(int fromIndex, int toIndex);
    void MoveLayerUp(const std::string& name);
    void MoveLayerDown(const std::string& name);
    
    // 可见性
    void SetLayerVisibility(const std::string& name, LayerVisibility visibility);
    LayerVisibility GetLayerVisibility(const std::string& name) const;
    void SetLayerOpacity(const std::string& name, double opacity);
    double GetLayerOpacity(const std::string& name) const;
    
    // 渲染
    ogc::draw::DrawResult Render(DrawContext& context);
    ogc::draw::DrawResult RenderLayer(const std::string& name, DrawContext& context);
    
    // 选择
    std::vector<int64_t> QueryFeatures(const Envelope& extent) const;
    std::vector<int64_t> QueryFeatures(const Coordinate& point, double tolerance) const;
    
    static LayerManagerPtr Create();
};

typedef std::shared_ptr<LayerManager> LayerManagerPtr;
```

---

### LabelEngine (标签引擎)

**File**: [label_engine.h](ogc/graph/label/label_engine.h)  
**Description**: 智能标签放置引擎

```cpp
struct LabelPlacementResult {
    bool success;
    double x;
    double y;
    double rotation;
    std::string message;
};

class OGC_GRAPH_API LabelEngine {
public:
    LabelEngine();
    
    // 标签属性
    void SetLabelProperty(const std::string& propertyName);
    void SetFont(const ogc::draw::Font& font);
    void SetColor(uint32_t color);
    void SetHaloColor(uint32_t color);
    void SetHaloRadius(double radius);
    
    // 放置参数
    void SetMinDistance(double distance);
    void SetMaxDistance(double distance);
    void SetPriority(int priority);
    void SetAllowOverlap(bool allow);
    void SetFollowLine(bool follow);
    void SetMaxAngleDelta(double delta);
    
    // 冲突检测
    void SetConflictResolver(LabelConflictResolverPtr resolver);
    
    // 标签生成
    std::vector<LabelInfo> GenerateLabels(const std::vector<const CNFeature*>& features, 
                                           ogc::draw::DrawContext& context);
    std::vector<LabelInfo> GenerateLabels(const CNFeature* feature, 
                                           ogc::draw::DrawContext& context);
    
    // 标签放置
    LabelPlacementResult PlaceLabel(const CNFeature* feature, ogc::draw::DrawContext& context);
    LabelPlacementResult PlacePointLabel(const Geometry* geometry, const std::string& text, 
                                          ogc::draw::DrawContext& context);
    LabelPlacementResult PlaceLineLabel(const Geometry* geometry, const std::string& text, 
                                         ogc::draw::DrawContext& context);
    
    static LabelEnginePtr Create();
};

typedef std::shared_ptr<LabelEngine> LabelEnginePtr;
```

---

### LODManager (LOD管理器)

**File**: [lod_manager.h](ogc/graph/lod/lod_manager.h)  
**Description**: 多级细节管理

```cpp
class OGC_GRAPH_API LODManager {
public:
    LODManager();
    explicit LODManager(LODStrategyPtr strategy);
    
    // 策略设置
    void SetStrategy(LODStrategyPtr strategy);
    LODStrategyPtr GetStrategy() const;
    
    // LOD级别管理
    void AddLODLevel(LODLevelPtr level);
    void RemoveLODLevel(int level);
    void ClearLODLevels();
    
    LODLevelPtr GetLODLevel(int level) const;
    std::vector<LODLevelPtr> GetLODLevels() const;
    size_t GetLODCount() const;
    
    // LOD选择
    int SelectLOD(double scale) const;
    int SelectLODByResolution(double resolution) const;
    int SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight) const;
    
    // 可见性
    std::vector<int> GetVisibleLODs(double scale) const;
    bool IsLODVisible(int lod) const;
    
    // 范围设置
    void SetMinLOD(int minLOD);
    void SetMaxLOD(int maxLOD);
    void SetLODRange(int minLOD, int maxLOD);
    
    // 过渡效果
    void SetTransitionFactor(double factor);
    void SetEnableTransitions(bool enable);
    
    // 预设LOD
    void CreateDefaultLODs(int minLevel, int maxLevel, double baseResolution);
    void CreateWebMercatorLODs(int minLevel = 0, int maxLevel = 18);
    
    // 工具方法
    double GetResolutionForLevel(int level) const;
    int GetLevelForResolution(double resolution) const;
    double GetScaleForLevel(int level, double dpi = 96.0) const;
    int GetLevelForScale(double scale, double dpi = 96.0) const;
    
    static LODManagerPtr Create();
    static LODManagerPtr CreateWebMercator(int minLevel = 0, int maxLevel = 18);
};

typedef std::shared_ptr<LODManager> LODManagerPtr;
```

---

## 类型定义

### RenderTaskState (渲染任务状态)

```cpp
enum class RenderTaskState {
    kPending = 0,    // 待处理
    kQueued = 1,     // 已入队
    kRunning = 2,    // 运行中
    kCompleted = 3,  // 已完成
    kFailed = 4,     // 失败
    kCancelled = 5   // 已取消
};
```

### LayerVisibility (图层可见性)

```cpp
enum class LayerVisibility {
    kVisible,     // 可见
    kHidden,      // 隐藏
    kConditional  // 条件可见
};
```

---

## 使用示例

### DrawFacade 基本使用

```cpp
#include <ogc/graph/render/draw_facade.h>

using namespace ogc::graph;

// 初始化
auto& facade = DrawFacade::Instance();
facade.Initialize();

// 创建设备
auto device = facade.CreateDevice(ogc::draw::DeviceType::kRaster, 800, 600);

// 创建样式
auto style = facade.CreateStrokeStyle(0xFF0000FF, 2.0);  // 蓝色，2像素宽

// 绘制几何对象
ogc::LineString line;
line.AddCoordinate(ogc::Coordinate(0, 0));
line.AddCoordinate(ogc::Coordinate(100, 100));

facade.DrawGeometry(device, &line, style);

// 清理
facade.Finalize();
```

### 渲染队列使用

```cpp
#include <ogc/graph/render/render_queue.h>
#include <ogc/graph/render/render_task.h>

using namespace ogc::graph;

// 创建渲染队列
auto queue = RenderQueue::Create(100);

// 创建渲染任务
auto task = RenderTask::Create("task1", DrawParams());
task->SetPriority(RenderTaskPriority::kHigh);

// 设置回调
task->SetCallback([](const RenderTaskPtr& t) {
    if (t->GetState() == RenderTaskState::kCompleted) {
        std::cout << "Task completed: " << t->GetId() << std::endl;
    }
});

// 入队
queue->Enqueue(task);

// 获取统计信息
auto stats = queue->GetStats();
std::cout << "Completed: " << stats.completedTasks << std::endl;
```

### 图层管理使用

```cpp
#include <ogc/graph/layer/layer_manager.h>

using namespace ogc::graph;

// 创建图层管理器
auto manager = LayerManager::Create();

// 添加图层
manager->AddLayer(roadLayer, 0);
manager->AddLayer(buildingLayer, 1);
manager->AddLayer(labelLayer, 2);

// 设置可见性
manager->SetLayerVisibility("roads", LayerVisibility::kVisible);
manager->SetLayerOpacity("buildings", 0.8);

// 移动图层顺序
manager->MoveLayerUp("labels");

// 渲染
ogc::draw::DrawContext context;
manager->Render(context);
```

### 标签引擎使用

```cpp
#include <ogc/graph/label/label_engine.h>

using namespace ogc::graph;

// 创建标签引擎
auto engine = LabelEngine::Create();

// 配置标签属性
engine->SetLabelProperty("name");
engine->SetFont(ogc::draw::Font("Arial", 12.0));
engine->SetColor(0xFF000000);
engine->SetHaloColor(0xFFFFFFFF);
engine->SetHaloRadius(2.0);
engine->SetMinDistance(50.0);
engine->SetAllowOverlap(false);

// 生成标签
std::vector<const CNFeature*> features = GetFeatures();
auto labels = engine->GenerateLabels(features, context);

for (const auto& label : labels) {
    // 绘制标签
    DrawLabel(label);
}
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | Team | 模块拆分后保留核心渲染功能 |
| v1.1 | 2026-04-09 | index-validator | 补充缺失头文件(export.h、async_renderer.h、basic_render_task.h、interaction_handler.h、interaction_feedback.h、label_conflict.h、util工具类)，修正transform_manager.h路径，补充DrawFacade和RenderQueue缺失方法 |

---

**Generated**: 2026-04-07  
**Module Version**: v1.0  
**C++ Standard**: C++11
