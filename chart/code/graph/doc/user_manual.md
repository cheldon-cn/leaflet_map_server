# ogc_graph 模块用户手册

> **版本**: v1.0  
> **更新日期**: 2026-04-07  
> **适用范围**: OGC Chart 系统地图渲染核心模块

---

## 一、模块描述

ogc_graph 是 OGC 图表系统的**地图渲染核心模块**，位于系统架构的渲染层（Layer 6）。该模块提供地图渲染、图层管理、标签引擎、交互处理和 LOD 管理等核心功能。作为整个系统的渲染引擎，负责将地理数据转换为可视化图像，是用户交互的直接入口。

### 1.1 模块定位

在 OGC Chart 系统架构中，ogc_graph 位于渲染层：

```
┌─────────────────────────────────────────────────────────────┐
│                    业务层 (navi, alert, alarm)              │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    渲染层 (ogc_graph) ← 当前模块            │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    符号化层 (symbology)                     │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    缓存层 (cache)                           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    数据层 (layer, feature)                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、核心特性

| 特性 | 说明 |
|------|------|
| **渲染引擎** | 高性能地图渲染，支持并发渲染和任务队列 |
| **图层管理** | 多图层叠加、可见性控制、透明度管理 |
| **标签引擎** | 智能标签避让、沿线标注、冲突检测 |
| **交互处理** | 平移、缩放、选择、点击测试 |
| **LOD管理** | 多级细节控制、动态切换 |
| **DrawFacade** | 统一的绘图接口封装 |

---

## 三、依赖关系

### 3.1 依赖库

| 依赖类型 | 库名称 | 说明 |
|----------|--------|------|
| 内部依赖 | ogc_base | 基础工具（Logger, ThreadSafe） |
| 内部依赖 | ogc_proj | 坐标转换（CoordinateTransformer） |
| 内部依赖 | ogc_cache | 缓存（TileCache） |
| 内部依赖 | ogc_symbology | 符号化（Symbolizer, Filter） |
| 内部依赖 | ogc_geometry | 几何类型（Geometry, Envelope） |
| 内部依赖 | ogc_feature | 要素（CNFeature） |
| 内部依赖 | ogc_layer | 图层（CNLayer） |
| 内部依赖 | ogc_draw | 绘图引擎（DrawContext, DrawDevice, DrawEngine） |
| 外部依赖 | C++11 STL | 标准库 |

### 3.2 被依赖关系

ogc_graph 被以下模块依赖：
- ogc_navi（导航）
- ogc_alert（警报）
- ogc_alarm（警报服务）

---

## 四、目录结构

```
graph/
├── include/ogc/graph/
│   ├── export.h
│   ├── render/                     # 渲染核心
│   │   ├── draw_facade.h
│   │   ├── draw_params.h
│   │   ├── render_task.h
│   │   ├── render_queue.h
│   │   └── async_renderer.h
│   ├── layer/                      # 图层管理
│   │   ├── layer_manager.h
│   │   └── layer_control_panel.h
│   ├── label/                      # 标签引擎
│   │   ├── label_engine.h
│   │   ├── label_info.h
│   │   ├── label_placement.h
│   │   └── label_conflict.h
│   ├── interaction/                # 交互处理
│   │   ├── pan_zoom_handler.h
│   │   ├── selection_handler.h
│   │   ├── hit_test.h
│   │   └── location_display_handler.h
│   ├── lod/                        # LOD管理
│   │   ├── lod.h
│   │   └── lod_manager.h
│   └── util/                       # 工具类
│       ├── transform_manager.h
│       ├── day_night_mode_manager.h
│       └── image_draw.h
├── src/
│   ├── render/
│   ├── layer/
│   ├── label/
│   ├── interaction/
│   ├── lod/
│   └── util/
├── tests/
│   ├── CMakeLists.txt
│   ├── test_draw_facade.cpp
│   ├── test_render_task.cpp
│   ├── test_render_queue.cpp
│   ├── test_layer_manager.cpp
│   ├── test_label_engine.cpp
│   ├── test_lod_manager.cpp
│   └── ...
├── doc/
│   └── user_manual.md
└── CMakeLists.txt
```

---

## 五、关键类和主要方法

### 5.1 DrawFacade（绘图门面）

**头文件**: `<ogc/graph/render/draw_facade.h>`

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
    
    // 绘制几何对象
    DrawResult DrawGeometry(ogc::draw::DrawDevicePtr device, 
                            const Geometry* geometry, 
                            const ogc::draw::DrawStyle& style);
    
    // 渲染到图像
    DrawResult RenderToImage(const Geometry* geometry, const DrawParams& params, 
                              const ogc::draw::DrawStyle& style, 
                              uint8_t* imageData, int width, int height, int channels);
    
    // 默认设置
    void SetDefaultDrawStyle(const ogc::draw::DrawStyle& style);
    void SetDefaultFont(const ogc::draw::Font& font);
    void SetDefaultColor(const ogc::draw::Color& color);
    void SetDefaultDpi(double dpi);
    
    // 工厂方法
    ogc::draw::DrawStyle CreateStrokeStyle(uint32_t color, double width);
    ogc::draw::DrawStyle CreateFillStyle(uint32_t color);
    ogc::draw::Font CreateFont(const std::string& family, double size, 
                                bool bold = false, bool italic = false);
    ogc::draw::Color CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    
    // 版本信息
    static std::string GetVersion();
    static std::string GetBuildInfo();
};

} // namespace graph
} // namespace ogc
```

### 5.2 RenderTask（渲染任务）

**头文件**: `<ogc/graph/render/render_task.h>`

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

class OGC_GRAPH_API RenderTask : public std::enable_shared_from_this<RenderTask> {
public:
    RenderTask();
    RenderTask(const std::string& id, const DrawParams& params);
    
    void SetId(const std::string& id);
    std::string GetId() const;
    
    void SetPriority(RenderTaskPriority priority);
    RenderTaskPriority GetPriority() const;
    
    void SetState(RenderTaskState state);
    RenderTaskState GetState() const;
    
    void SetProgress(double progress);
    double GetProgress() const;
    
    void SetCallback(RenderTaskCallback callback);
    
    virtual void Execute() = 0;
    virtual void Cancel();
    
    static RenderTaskPtr Create(const std::string& id, const DrawParams& params);
};
```

### 5.3 RenderQueue（渲染队列）

**头文件**: `<ogc/graph/render/render_queue.h>`

```cpp
struct RenderQueueStats {
    size_t totalTasks;
    size_t pendingTasks;
    size_t runningTasks;
    size_t completedTasks;
    size_t failedTasks;
    size_t cancelledTasks;
};

class OGC_GRAPH_API RenderQueue {
public:
    RenderQueue();
    explicit RenderQueue(size_t maxQueueSize);
    
    bool Enqueue(RenderTaskPtr task);
    RenderTaskPtr Dequeue();
    RenderTaskPtr TryDequeue(int64_t timeoutMs = 0);
    
    bool IsEmpty() const;
    size_t GetSize() const;
    void Clear();
    void CancelAll();
    
    RenderQueueStats GetStats() const;
    
    void SetPriorityMode(bool enable);
    void Pause();
    void Resume();
    
    static RenderQueuePtr Create();
};
```

### 5.4 LayerManager（图层管理器）

**头文件**: `<ogc/graph/layer/layer_manager.h>`

```cpp
enum class LayerVisibility {
    kVisible,
    kHidden,
    kConditional
};

class OGC_GRAPH_API LayerManager {
public:
    LayerManager();
    
    // 图层操作
    void AddLayer(CNLayer* layer, int index = -1);
    void RemoveLayer(const std::string& name);
    void ClearLayers();
    
    // 图层查询
    CNLayer* GetLayer(const std::string& name) const;
    std::vector<CNLayer*> GetLayers() const;
    size_t GetLayerCount() const;
    
    // 图层顺序
    void MoveLayer(int fromIndex, int toIndex);
    void MoveLayerUp(const std::string& name);
    void MoveLayerDown(const std::string& name);
    
    // 可见性
    void SetLayerVisibility(const std::string& name, LayerVisibility visibility);
    void SetLayerOpacity(const std::string& name, double opacity);
    
    // 渲染
    ogc::draw::DrawResult Render(DrawContext& context);
    
    // 查询
    std::vector<int64_t> QueryFeatures(const Envelope& extent) const;
    
    static LayerManagerPtr Create();
};
```

### 5.5 LabelEngine（标签引擎）

**头文件**: `<ogc/graph/label/label_engine.h>`

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
    void SetAllowOverlap(bool allow);
    void SetFollowLine(bool follow);
    
    // 标签生成
    std::vector<LabelInfo> GenerateLabels(const std::vector<const CNFeature*>& features, 
                                           ogc::draw::DrawContext& context);
    
    // 标签放置
    LabelPlacementResult PlaceLabel(const CNFeature* feature, ogc::draw::DrawContext& context);
    LabelPlacementResult PlacePointLabel(const Geometry* geometry, const std::string& text, 
                                          ogc::draw::DrawContext& context);
    LabelPlacementResult PlaceLineLabel(const Geometry* geometry, const std::string& text, 
                                         ogc::draw::DrawContext& context);
    
    static LabelEnginePtr Create();
};
```

### 5.6 LODManager（LOD管理器）

**头文件**: `<ogc/graph/lod/lod_manager.h>`

```cpp
class OGC_GRAPH_API LODManager {
public:
    LODManager();
    explicit LODManager(LODStrategyPtr strategy);
    
    // LOD级别管理
    void AddLODLevel(LODLevelPtr level);
    void ClearLODLevels();
    std::vector<LODLevelPtr> GetLODLevels() const;
    
    // LOD选择
    int SelectLOD(double scale) const;
    int SelectLODByExtent(const Envelope& extent, int pixelWidth, int pixelHeight) const;
    
    // 可见性
    std::vector<int> GetVisibleLODs(double scale) const;
    
    // 范围设置
    void SetMinLOD(int minLOD);
    void SetMaxLOD(int maxLOD);
    
    // 预设LOD
    void CreateWebMercatorLODs(int minLevel = 0, int maxLevel = 18);
    
    // 工具方法
    double GetResolutionForLevel(int level) const;
    double GetScaleForLevel(int level, double dpi = 96.0) const;
    
    static LODManagerPtr Create();
    static LODManagerPtr CreateWebMercator(int minLevel = 0, int maxLevel = 18);
};
```

---

## 六、使用示例

### 6.1 DrawFacade 基本使用

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

// 渲染到图像
std::vector<uint8_t> imageData(800 * 600 * 4);
DrawParams params;
facade.RenderToImage(&line, params, style, imageData.data(), 800, 600, 4);

// 清理
facade.Finalize();
```

### 6.2 渲染队列使用

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

### 6.3 图层管理使用

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

// 查询要素
auto features = manager->QueryFeatures(envelope);
```

### 6.4 标签引擎使用

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

### 6.5 LOD管理使用

```cpp
#include <ogc/graph/lod/lod_manager.h>

using namespace ogc::graph;

// 创建 WebMercator LOD 管理器
auto lodManager = LODManager::CreateWebMercator(0, 18);

// 根据比例尺选择 LOD
int lod = lodManager->SelectLOD(scale);

// 获取可见 LOD
auto visibleLODs = lodManager->GetVisibleLODs(scale);

// 获取分辨率
double resolution = lodManager->GetResolutionForLevel(lod);

// 获取比例尺
double scale = lodManager->GetScaleForLevel(lod, 96.0);
```

---

## 七、类型定义

### 7.1 RenderTaskState（渲染任务状态）

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

### 7.2 LayerVisibility（图层可见性）

```cpp
enum class LayerVisibility {
    kVisible,     // 可见
    kHidden,      // 隐藏
    kConditional  // 条件可见
};
```

---

## 八、编译和集成

### 8.1 CMake 配置

```cmake
# 添加 graph 模块
add_subdirectory(graph)

# 链接 graph 库
target_link_libraries(your_target PRIVATE ogc_graph)
```

### 8.2 头文件包含

```cpp
// 渲染核心
#include <ogc/graph/render/draw_facade.h>
#include <ogc/graph/render/draw_params.h>
#include <ogc/graph/render/render_task.h>
#include <ogc/graph/render/render_queue.h>

// 图层管理
#include <ogc/graph/layer/layer_manager.h>

// 标签引擎
#include <ogc/graph/label/label_engine.h>

// 交互处理
#include <ogc/graph/interaction/pan_zoom_handler.h>
#include <ogc/graph/interaction/selection_handler.h>
#include <ogc/graph/interaction/hit_test.h>

// LOD管理
#include <ogc/graph/lod/lod_manager.h>
```

---

## 九、注意事项

1. **初始化顺序**：使用 DrawFacade 前必须调用 Initialize()
2. **线程安全**：RenderQueue 和 LayerManager 操作是线程安全的
3. **内存管理**：使用智能指针管理 RenderTask 和其他对象
4. **性能优化**：使用 LOD 管理减少不必要的渲染开销
5. **标签冲突**：LabelEngine 会自动处理标签冲突，可通过 SetAllowOverlap 控制

---

## 十、版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-04-06 | 模块拆分后保留核心渲染功能，包含渲染引擎、图层管理、标签引擎、交互处理、LOD管理 |

---

**文档维护**: OGC Chart 开发团队  
**技术支持**: 参见项目 README.md
