# 多JAR包JavaFX UI拆分方案分析

> **版本**: v1.0  
> **日期**: 2026-04-17  
> **目标**: 将UI控件相关代码从多个JAR包中抽离，确保核心业务逻辑不依赖JavaFX/fxribbon，以支持Android/Web复用

---

## 一、背景与目标

### 1.1 当前问题

当前系统中多个核心JAR包存在JavaFX依赖，导致：

| 问题 | 影响 | 严重程度 |
|------|------|---------|
| **核心层绑定JavaFX** | 核心业务逻辑无法在Android/Web使用 | 🔴 严重 |
| **依赖传递污染** | 所有依赖核心层的模块间接依赖JavaFX | 🔴 严重 |
| **职责混合** | 业务逻辑与UI控件耦合，违反单一职责原则 | 🟡 中等 |
| **测试困难** | 核心逻辑测试需要JavaFX运行环境 | 🟡 中等 |
| **部署臃肿** | Android/Web仅需核心逻辑，却被迫引入JavaFX | 🔴 严重 |

### 1.2 涉及JAR包范围

| 序号 | JAR包名称 | 层级 | 当前JavaFX依赖程度 |
|------|-----------|------|-------------------|
| 1 | echart-core.jar | 核心层 | 🔴 重度 |
| 2 | echart-event.jar | 基础服务层 | 🟡 轻度 |
| 3 | echart-render.jar | 核心功能层 | 🔴 重度 |
| 4 | echart-data.jar | 核心功能层 | 🟡 中度 |
| 5 | echart-alarm.jar | 业务功能层 | 🟡 中度 |
| 6 | echart-ais.jar | 业务功能层 | 🟢 无/轻度 |
| 7 | echart-route.jar | 业务功能层 | 🟡 中度 |
| 8 | echart-plugin.jar | 扩展功能层 | 🔴 重度 |

### 1.3 拆分目标

| 目标 | 说明 |
|------|------|
| **核心业务逻辑无JavaFX依赖** | 业务逻辑可在任何Java 8+平台运行 |
| **UI层独立可替换** | JavaFX UI作为独立JAR包，可替换为Android/Web UI实现 |
| **依赖方向正确** | UI层依赖核心层，核心层不依赖UI层 |
| **对现有模块影响最小** | 其他JAR包的依赖关系仅需微调 |
| **保持接口兼容** | 通过抽象接口保持向后兼容 |

### 1.4 目标平台复用场景

| 平台 | UI技术 | 可复用模块 |
|------|--------|-----------|
| **Android** | Android View / Jetpack Compose | echart-core, echart-event, echart-alarm(业务逻辑), echart-data(数据层), echart-ais(数据模型), echart-route(数据模型) |
| **Web** | HTML5 Canvas / WebGL | echart-core, echart-event, echart-alarm(业务逻辑) |
| **桌面(JavaFX)** | JavaFX 8 | 所有模块 |

---

## 二、各JAR包现状分析

### 2.1 echart-core.jar 分析

#### 2.1.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `MainView` | 主布局容器 | extends BorderPane | 🔴 重度 | → echart-ui.jar |
| `RibbonMenuBar` | Ribbon菜单栏 | extends JavaFX Control | 🔴 重度 | → echart-ui.jar |
| `ActivityBar` | 活动栏 | extends VBox | 🔴 重度 | → echart-ui.jar |
| `SideBarManager` | 侧边栏管理 | VBox, StackPane, Tooltip, Button | 🔴 重度 | → echart-ui.jar |
| `ChartDisplayArea` | 海图显示区 | Canvas, StackPane | 🔴 重度 | → echart-ui.jar |
| `RightTabManager` | 右侧面板 | TabPane, Tab | 🔴 重度 | → echart-ui.jar |
| `StatusBar` | 状态栏 | extends HBox | 🔴 重度 | → echart-ui.jar |
| `ComponentHierarchy` | 组件层次结构 | 描述JavaFX组件树 | 🟡 中度 | → echart-ui.jar |
| `StyleManager` | 样式管理器 | JavaFX CSS | 🔴 重度 | → echart-ui.jar |
| `ResponsiveLayoutManager` | 响应式布局 | Region, ChangeListener | 🔴 重度 | → echart-ui.jar |
| `AppContext` | 全局上下文 | Stage引用 | 🔴 重度 | → echart-ui.jar |
| `LifecycleComponent` | 生命周期接口 | 无 | 🟢 无 | 保留 |
| `AbstractLifecycleComponent` | 生命周期基类 | 无 | 🟢 无 | 保留 |
| `LRUCache<K,V>` | LRU缓存 | 无 | 🟢 无 | 保留 |
| `AppEvent` | 事件数据 | 无 | 🟢 无 | 保留 |
| `AppEventType` | 事件类型枚举 | 无 | 🟢 无 | 保留 |
| `AppEventBus` | 事件总线 | Platform.runLater() | 🟡 轻度 | 改造后保留 |
| `SideBarPanel` | 侧边栏面板接口 | Node返回值 | 🟡 中度 | 改造后保留 |
| `RightTabPanel` | 右侧面板接口 | Node返回值 | 🟡 中度 | 改造后保留 |

#### 2.1.2 JavaFX依赖分类统计

```
echart-core.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 25%
│   ├── LifecycleComponent 接口
│   ├── AbstractLifecycleComponent 类
│   ├── LRUCache<K,V> 类
│   ├── AppEvent 类
│   ├── AppEventType 枚举
│   └── ErrorCode 枚举（部分）
│
├── 轻度JavaFX依赖（可抽象解耦）──── 约 15%
│   ├── AppEventBus（Platform.runLater可抽象）
│   ├── SideBarPanel 接口（Node可泛化）
│   ├── RightTabPanel 接口（Node可泛化）
│   └── Extension 接口族（Node可泛化）
│
└── 重度JavaFX依赖（必须移至UI层）── 约 60%
    ├── MainView (BorderPane)
    ├── RibbonMenuBar
    ├── ActivityBar (VBox)
    ├── SideBarManager (VBox, StackPane)
    ├── ChartDisplayArea (Canvas)
    ├── RightTabManager (TabPane)
    ├── StatusBar (HBox)
    ├── StyleManager (CSS)
    ├── ResponsiveLayoutManager (Region)
    └── AppContext (Stage)
```

---

### 2.2 echart-event.jar 分析

#### 2.2.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `EventBus` | 事件总线 | Platform.runLater() | 🟡 轻度 | 改造后保留 |
| `EventType` | 事件类型枚举 | 无 | 🟢 无 | 保留 |
| `Event` | 事件基类 | 无 | 🟢 无 | 保留 |
| `EventHandler` | 事件处理器接口 | 无 | 🟢 无 | 保留 |
| `EventDispatcher` | 事件分发器 | Platform.runLater() | 🟡 轻度 | 改造后保留 |

#### 2.2.2 JavaFX依赖分析

```
echart-event.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 70%
│   ├── EventType 枚举
│   ├── Event 类
│   ├── EventHandler 接口
│   └── EventDispatcher 核心逻辑
│
└── 轻度JavaFX依赖（可抽象解耦）──── 约 30%
    ├── EventBus（Platform.runLater可抽象）
    └── EventDispatcher（Platform.runLater可抽象）
```

**关键改造点**：`Platform.runLater()` 调用需要抽象为平台无关接口。

---

### 2.3 echart-render.jar 分析

#### 2.3.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `RenderEngine` | 渲染引擎 | GraphicsContext | 🔴 重度 | → echart-ui-render.jar |
| `LayerManager` | 图层管理器 | 无 | 🟢 无 | 保留 |
| `CanvasRenderer` | Canvas渲染器 | Canvas, GraphicsContext | 🔴 重度 | → echart-ui-render.jar |
| `JNIBridge` | JNI桥接 | 无 | 🟢 无 | 保留 |
| `ChartElementRenderer` | 海图要素渲染器 | GraphicsContext | 🔴 重度 | → echart-ui-render.jar |
| `DirtyRegionManager` | 脏区域管理器 | Rectangle2D | 🟡 轻度 | 改造后保留 |
| `TileCache` | 瓦片缓存 | Image | 🔴 重度 | → echart-ui-render.jar |
| `LODStrategy` | LOD策略 | 无 | 🟢 无 | 保留 |
| `PerformanceMonitor` | 性能监控器 | 无 | 🟢 无 | 保留 |
| `MeasurementTool` | 测量工具 | Canvas, MouseEvent | 🔴 重度 | → echart-ui-render.jar |
| `HighlightRenderer` | 高亮渲染器 | GraphicsContext | 🔴 重度 | → echart-ui-render.jar |
| `ViewOperator` | 视图操作器 | Canvas | 🔴 重度 | → echart-ui-render.jar |
| `S52SymbolLibrary` | S-52符号库 | 无 | 🟢 无 | 保留 |
| `SymbolStyleCustomizer` | 符号样式定制器 | 无 | 🟢 无 | 保留 |

#### 2.3.2 JavaFX依赖分类统计

```
echart-render.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 40%
│   ├── LayerManager（图层管理逻辑）
│   ├── JNIBridge（JNI桥接）
│   ├── LODStrategy（LOD策略）
│   ├── PerformanceMonitor（性能监控）
│   ├── S52SymbolLibrary（符号库数据）
│   └── SymbolStyleCustomizer（样式配置）
│
├── 轻度JavaFX依赖（可抽象解耦）──── 约 10%
│   └── DirtyRegionManager（Rectangle2D可替换）
│
└── 重度JavaFX依赖（必须移至UI层）── 约 50%
    ├── RenderEngine（GraphicsContext）
    ├── CanvasRenderer（Canvas）
    ├── ChartElementRenderer（GraphicsContext）
    ├── TileCache（Image）
    ├── MeasurementTool（Canvas, MouseEvent）
    ├── HighlightRenderer（GraphicsContext）
    └── ViewOperator（Canvas）
```

**关键改造点**：渲染层需要抽象为平台无关的渲染接口，JavaFX Canvas实现作为其中一个实现。

---

### 2.4 echart-data.jar 分析

#### 2.4.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `DataCatalogPanel` | 数据目录面板 | TreeView, TreeItem | 🔴 重度 | → echart-ui.jar |
| `ChartFileManager` | 海图文件管理器 | 无 | 🟢 无 | 保留 |
| `LayerManagerPanel` | 图层管理面板 | ListView, CheckBox | 🔴 重度 | → echart-ui.jar |
| `DataImporter` | 数据导入器 | 无 | 🟢 无 | 保留 |
| `DataExporter` | 数据导出器 | 无 | 🟢 无 | 保留 |

#### 2.4.2 JavaFX依赖分类统计

```
echart-data.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 60%
│   ├── ChartFileManager（文件管理逻辑）
│   ├── DataImporter（数据导入逻辑）
│   ├── DataExporter（数据导出逻辑）
│   └── 数据模型类
│
└── 重度JavaFX依赖（必须移至UI层）── 约 40%
    ├── DataCatalogPanel（TreeView）
    └── LayerManagerPanel（ListView）
```

---

### 2.5 echart-alarm.jar 分析

#### 2.5.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `AlarmManager` | 预警管理器 | 无 | 🟢 无 | 保留 |
| `AlarmType` | 预警类型枚举 | 无 | 🟢 无 | 保留 |
| `AlarmPanel` | 预警面板 | VBox, ListView | 🔴 重度 | → echart-ui.jar |
| `AlarmListView` | 预警列表视图 | VBox, ListView | 🔴 重度 | → echart-ui.jar |
| `AlarmDetailPanel` | 预警详情面板 | VBox, Label, TextArea | 🔴 重度 | → echart-ui.jar |
| `AlarmNotifier` | 预警通知器 | Platform.runLater() | 🟡 轻度 | 改造后保留 |
| `AlarmResponseHandler` | 预警响应处理器 | 无 | 🟢 无 | 保留 |
| `CollisionAlarm` | 碰撞预警 | 无 | 🟢 无 | 保留 |
| `DeviationAlarm` | 偏航预警 | 无 | 🟢 无 | 保留 |
| `ShallowWaterAlarm` | 浅水预警 | 无 | 🟢 无 | 保留 |
| `RestrictedAreaAlarm` | 禁航区预警 | 无 | 🟢 无 | 保留 |
| `WeatherAlarm` | 气象预警 | 无 | 🟢 无 | 保留 |
| `WatchAlarm` | 值班报警 | 无 | 🟢 无 | 保留 |
| `AlarmHistory` | 预警历史 | 无 | 🟢 无 | 保留 |
| `AlarmStatistics` | 预警统计 | 无 | 🟢 无 | 保留 |
| `SoundPlayer` | 声音播放器 | AudioClip | 🔴 重度 | → echart-ui.jar |
| `AlarmSoundSettings` | 预警声音设置 | VBox, Slider, ComboBox | 🔴 重度 | → echart-ui.jar |
| `SoundSettingsPanel` | 声音设置面板 | VBox, CheckBox, Slider | 🔴 重度 | → echart-ui.jar |
| `AlarmSuppressionManager` | 预警抑制管理器 | 无 | 🟢 无 | 保留 |
| `AlarmPersistence` | 预警持久化 | 无 | 🟢 无 | 保留 |
| `AlarmRuleEngine` | 预警规则引擎 | 无 | 🟢 无 | 保留 |

#### 2.5.2 JavaFX依赖分类统计

```
echart-alarm.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 65%
│   ├── AlarmManager（预警管理逻辑）
│   ├── AlarmType（预警类型定义）
│   ├── AlarmNotifier（通知逻辑，需改造）
│   ├── AlarmResponseHandler（响应处理）
│   ├── CollisionAlarm（碰撞预警逻辑）
│   ├── DeviationAlarm（偏航预警逻辑）
│   ├── ShallowWaterAlarm（浅水预警逻辑）
│   ├── RestrictedAreaAlarm（禁航区预警逻辑）
│   ├── WeatherAlarm（气象预警逻辑）
│   ├── WatchAlarm（值班报警逻辑）
│   ├── AlarmHistory（历史记录）
│   ├── AlarmStatistics（统计逻辑）
│   ├── AlarmSuppressionManager（抑制管理）
│   ├── AlarmPersistence（持久化）
│   ├── AlarmRuleEngine（规则引擎）
│   └── Alarm数据模型类
│
├── 轻度JavaFX依赖（可抽象解耦）──── 约 5%
│   └── AlarmNotifier（Platform.runLater可抽象）
│
└── 重度JavaFX依赖（必须移至UI层）── 约 30%
    ├── AlarmPanel（VBox）
    ├── AlarmListView（VBox, ListView）
    ├── AlarmDetailPanel（VBox, Label）
    ├── SoundPlayer（AudioClip）
    ├── AlarmSoundSettings（VBox）
    └── SoundSettingsPanel（VBox）
```

---

### 2.6 echart-ais.jar 分析

#### 2.6.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `AISTargetManager` | AIS目标管理器 | 无 | 🟢 无 | 保留 |
| `AISTarget` | AIS目标数据模型 | 无 | 🟢 无 | 保留 |
| `AISAlarmAssociation` | AIS预警关联 | 无 | 🟢 无 | 保留 |
| `AlarmDetailDisplay` | 预警详情显示 | VBox, Label | 🔴 重度 | → echart-ui.jar |
| `CPATCPACalculator` | CPA/TCPA计算器 | 无 | 🟢 无 | 保留 |

#### 2.6.2 JavaFX依赖分类统计

```
echart-ais.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 85%
│   ├── AISTargetManager（目标管理）
│   ├── AISTarget（数据模型）
│   ├── AISAlarmAssociation（关联逻辑）
│   └── CPATCPACalculator（计算逻辑）
│
└── 重度JavaFX依赖（必须移至UI层）── 约 15%
    └── AlarmDetailDisplay（VBox, Label）
```

---

### 2.7 echart-route.jar 分析

#### 2.7.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `RouteManager` | 航线管理器 | 无 | 🟢 无 | 保留 |
| `Route` | 航线数据模型 | 无 | 🟢 无 | 保留 |
| `Waypoint` | 航点数据模型 | 无 | 🟢 无 | 保留 |
| `RouteCreator` | 航线创建器 | Canvas, MouseEvent | 🔴 重度 | → echart-ui.jar |
| `RouteEditor` | 航线编辑器 | Canvas, MouseEvent | 🔴 重度 | → echart-ui.jar |
| `WaypointManager` | 航点管理器 | 无 | 🟢 无 | 保留 |
| `RouteImporter` | 航线导入器 | 无 | 🟢 无 | 保留 |
| `RouteExporter` | 航线导出器 | 无 | 🟢 无 | 保留 |
| `RouteChecker` | 航线检查器 | 无 | 🟢 无 | 保留 |
| `RoutePanel` | 航线面板 | VBox, ListView | 🔴 重度 | → echart-ui.jar |

#### 2.7.2 JavaFX依赖分类统计

```
echart-route.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 60%
│   ├── RouteManager（航线管理逻辑）
│   ├── Route（航线数据模型）
│   ├── Waypoint（航点数据模型）
│   ├── WaypointManager（航点管理）
│   ├── RouteImporter（导入逻辑）
│   ├── RouteExporter（导出逻辑）
│   ├── RouteChecker（检查逻辑）
│   └── 航线计算相关类
│
└── 重度JavaFX依赖（必须移至UI层）── 约 40%
    ├── RouteCreator（Canvas交互）
    ├── RouteEditor（Canvas交互）
    └── RoutePanel（VBox, ListView）
```

---

### 2.8 echart-plugin.jar 分析

#### 2.8.1 当前包含的主要类

| 类名 | 职责 | JavaFX依赖 | 依赖程度 | 拆分去向 |
|------|------|-----------|---------|---------|
| `PluginLoader` | 插件加载器 | 无 | 🟢 无 | 保留 |
| `PluginSecurityManager` | 插件安全管理器 | 无 | 🟢 无 | 保留 |
| `PluginManager` | 插件管理器 | 无 | 🟢 无 | 保留 |
| `Extension` | 扩展点基接口 | 无 | 🟢 无 | 保留 |
| `ExtensionContext` | 扩展上下文 | Stage | 🔴 重度 | 改造后保留 |
| `RibbonExtension` | Ribbon扩展接口 | Node, Control | 🔴 重度 | → echart-ui.jar |
| `SideBarExtension` | 侧边栏扩展接口 | Node | 🔴 重度 | → echart-ui.jar |
| `RightPanelExtension` | 右侧面板扩展接口 | Node | 🔴 重度 | → echart-ui.jar |
| `ToolExtension` | 工具扩展接口 | Cursor, MouseEvent | 🔴 重度 | → echart-ui.jar |
| `DataSourceExtension` | 数据源扩展接口 | 无 | 🟢 无 | 保留 |
| `AlarmRuleExtension` | 预警规则扩展接口 | 无 | 🟢 无 | 保留 |

#### 2.8.2 JavaFX依赖分类统计

```
echart-plugin.jar 当前内容
├── 纯Java（无JavaFX依赖）────────── 约 40%
│   ├── PluginLoader（加载逻辑）
│   ├── PluginSecurityManager（安全逻辑）
│   ├── PluginManager（管理逻辑）
│   ├── Extension（基础接口）
│   ├── DataSourceExtension（数据源接口）
│   └── AlarmRuleExtension（预警规则接口）
│
├── 轻度JavaFX依赖（可抽象解耦）──── 约 10%
│   └── ExtensionContext（Stage可抽象）
│
└── 重度JavaFX依赖（必须移至UI层）── 约 50%
    ├── RibbonExtension（Node, Control）
    ├── SideBarExtension（Node）
    ├── RightPanelExtension（Node）
    └── ToolExtension（Cursor, MouseEvent）
```

---

## 三、必要性分析

### 3.1 当前架构问题汇总

| JAR包 | 问题 | 影响 | 严重程度 |
|-------|------|------|---------|
| echart-core.jar | 核心框架绑定JavaFX | 无法在Android/Web使用 | 🔴 严重 |
| echart-event.jar | 事件总线绑定Platform.runLater | 事件分发依赖JavaFX线程模型 | 🟡 中等 |
| echart-render.jar | 渲染引擎绑定Canvas | 渲染逻辑无法复用 | 🔴 严重 |
| echart-data.jar | 数据面板绑定TreeView | 数据管理逻辑被污染 | 🟡 中等 |
| echart-alarm.jar | 预警UI与业务逻辑混合 | 预警逻辑无法独立测试 | 🔴 严重 |
| echart-ais.jar | AIS显示组件耦合 | AIS数据模型被污染 | 🟢 轻微 |
| echart-route.jar | 航线编辑器绑定Canvas | 航线逻辑无法复用 | 🟡 中等 |
| echart-plugin.jar | UI扩展接口绑定Node | 插件系统无法跨平台 | 🔴 严重 |

### 3.2 不拆分的后果

若不进行拆分，在Android/Web平台复用时将面临：

```
Android/Web 复用场景
├── 方案A：整体引入各JAR包
│   └── ❌ JavaFX在Android/Web不可用，运行时崩溃
│
├── 方案B：重写所有JAR包
│   └── ❌ 工作量巨大，且需维护多套代码
│
└── 方案C：拆分UI层
    └── ✅ 核心逻辑直接复用，仅替换UI层
```

### 3.3 必要性结论

**必要性等级：高**

拆分是支持多平台复用的前提条件。当前架构将平台无关的核心业务逻辑与平台特定的UI控件混合在多个JAR包中，直接阻碍了Android/Web平台的复用目标。

---

## 四、可行性分析

### 4.1 技术可行性

#### 4.1.1 JavaFX依赖可分离性

| 依赖类型 | 可分离性 | 解决方案 |
|---------|---------|---------|
| 继承JavaFX类（BorderPane等） | ✅ 可分离 | 移至UI层JAR包 |
| 方法参数/返回值使用Node | ✅ 可分离 | 引入泛型抽象或移至UI层 |
| Platform.runLater()调用 | ✅ 可分离 | 引入平台抽象接口 |
| Stage引用 | ✅ 可分离 | 移至UI层或抽象 |
| JavaFX CSS | ✅ 可分离 | 移至UI层 |
| Canvas/GraphicsContext | ✅ 可分离 | 引入渲染抽象接口 |
| AudioClip | ✅ 可分离 | 引入音频抽象接口 |
| MouseEvent | ✅ 可分离 | 引入事件抽象接口 |

#### 4.1.2 关键技术难点与解决方案

**难点1：Platform.runLater()依赖**

当前代码：
```java
private void dispatchEvent(AppEvent event) {
    if (Platform.isFxApplicationThread()) {
        invokeHandlers(handlers, event);
    } else {
        Platform.runLater(() -> invokeHandlers(handlers, event));
    }
}
```

解决方案：引入平台抽象接口
```java
public interface PlatformAdapter {
    boolean isUiThread();
    void runOnUi(Runnable action);
}

// JavaFX实现
public class FxPlatformAdapter implements PlatformAdapter {
    public boolean isUiThread() {
        return Platform.isFxApplicationThread();
    }
    public void runOnUi(Runnable action) {
        Platform.runLater(action);
    }
}

// Android实现
public class AndroidPlatformAdapter implements PlatformAdapter {
    private Handler uiHandler = new Handler(Looper.getMainLooper());
    public boolean isUiThread() {
        return Looper.myLooper() == Looper.getMainLooper();
    }
    public void runOnUi(Runnable action) {
        uiHandler.post(action);
    }
}
```

**难点2：Canvas渲染依赖**

当前代码：
```java
public class CanvasRenderer {
    private Canvas canvas;
    private GraphicsContext gc;
    
    public void render() {
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        gc.setFill(Color.BLUE);
        gc.fillRect(x, y, w, h);
    }
}
```

解决方案：引入渲染抽象接口
```java
// 核心层：平台无关的渲染接口
public interface RenderContext {
    void clear(double x, double y, double w, double h);
    void setFillColor(String color);
    void fillRect(double x, double y, double w, double h);
    void drawLine(double x1, double y1, double x2, double y2);
    void drawText(String text, double x, double y);
    // ... 更多渲染方法
}

// JavaFX实现
public class FxRenderContext implements RenderContext {
    private GraphicsContext gc;
    
    public void clear(double x, double y, double w, double h) {
        gc.clearRect(x, y, w, h);
    }
    
    public void setFillColor(String color) {
        gc.setFill(Color.web(color));
    }
    
    public void fillRect(double x, double y, double w, double h) {
        gc.fillRect(x, y, w, h);
    }
    // ...
}

// Android实现
public class AndroidRenderContext implements RenderContext {
    private Canvas canvas;
    private Paint paint;
    
    public void clear(double x, double y, double w, double h) {
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    }
    
    public void setFillColor(String color) {
        paint.setColor(Color.parseColor(color));
    }
    
    public void fillRect(double x, double y, double w, double h) {
        canvas.drawRect((float)x, (float)y, (float)(x+w), (float)(y+h), paint);
    }
    // ...
}
```

**难点3：音频播放依赖**

当前代码：
```java
public class SoundPlayer {
    private AudioClip clip;
    
    public void play() {
        clip.play();
    }
}
```

解决方案：引入音频抽象接口
```java
// 核心层：平台无关的音频接口
public interface AudioPlayer {
    void load(String resourcePath);
    void play();
    void stop();
    void setVolume(double volume);
    void setLoop(boolean loop);
}

// JavaFX实现
public class FxAudioPlayer implements AudioPlayer {
    private AudioClip clip;
    
    public void load(String resourcePath) {
        clip = new AudioClip(getClass().getResource(resourcePath).toExternalForm());
    }
    
    public void play() {
        clip.play();
    }
    // ...
}

// Android实现
public class AndroidAudioPlayer implements AudioPlayer {
    private MediaPlayer mediaPlayer;
    
    public void load(String resourcePath) {
        AssetFileDescriptor afd = context.getAssets().openFd(resourcePath);
        mediaPlayer = new MediaPlayer();
        mediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
        mediaPlayer.prepare();
    }
    
    public void play() {
        mediaPlayer.start();
    }
    // ...
}
```

**难点4：面板接口的Node依赖**

当前代码：
```java
public interface SideBarPanel extends LifecycleComponent {
    String getPanelId();
    String getPanelName();
    Node getIcon();      // JavaFX Node
    Node getContent();   // JavaFX Node
}
```

解决方案：泛化接口，将UI相关方法移至UI层扩展接口
```java
// 核心层：平台无关的面板描述
public interface PanelDescriptor extends LifecycleComponent {
    String getPanelId();
    String getPanelName();
    String getIconKey();  // 图标资源key，而非JavaFX Node
}

// UI层：JavaFX面板接口
public interface FxSideBarPanel extends PanelDescriptor {
    Node getIcon();
    Node getContent();
}

// Android层：Android面板接口
public interface AndroidSideBarPanel extends PanelDescriptor {
    Drawable getIcon();
    View getContent();
}
```

**难点5：插件扩展接口的Node依赖**

当前代码：
```java
public interface RibbonExtension extends Extension {
    String getTabId();
    String getTabName();
    Node getTabContent();  // JavaFX Node
    List<RibbonButton> getButtons();
}
```

解决方案：分离核心扩展接口与UI扩展接口
```java
// 核心层：平台无关的扩展描述
public interface ExtensionDescriptor extends Extension {
    String getExtensionId();
    String getExtensionName();
    String getIconKey();
}

// UI层：JavaFX扩展接口
public interface FxRibbonExtension extends ExtensionDescriptor {
    Node getTabContent();
    List<FxRibbonButton> getButtons();
}

// 核心层：数据源扩展（无UI依赖）
public interface DataSourceExtension extends Extension {
    String getDataSourceType();
    String getDataSourceName();
    boolean supportsUrl(String url);
    DataSource createDataSource(Map<String, Object> config);
}

// 核心层：预警规则扩展（无UI依赖）
public interface AlarmRuleExtension extends Extension {
    String getRuleId();
    String getRuleName();
    AlarmLevel getDefaultLevel();
    boolean evaluate(AlertContext context);
}
```

#### 4.1.3 技术可行性结论

| 评估维度 | 结论 | 说明 |
|---------|------|------|
| 依赖可分离性 | ✅ 可行 | 所有JavaFX依赖均可通过抽象或迁移解耦 |
| 接口兼容性 | ✅ 可行 | 通过泛化接口保持向后兼容 |
| 代码改动量 | 🟡 中等 | 主要是接口抽象和类迁移，非重写 |
| 编译隔离验证 | ✅ 可行 | 可通过Gradle依赖配置验证无JavaFX引用 |

### 4.2 依赖影响可行性

#### 4.2.1 当前依赖链

```
echart-core.jar (含JavaFX)
    ↑ 被依赖
    ├── echart-event.jar
    ├── echart-render.jar
    ├── echart-data.jar
    ├── echart-alarm.jar
    ├── echart-ais.jar
    ├── echart-route.jar
    ├── echart-workspace.jar
    ├── echart-theme.jar
    ├── echart-plugin.jar
    └── echart-app.jar
```

#### 4.2.2 拆分后依赖链

```
echart-core.jar (纯Java，无JavaFX)
    ↑ 被依赖
    ├── echart-ui.jar (JavaFX UI层)
    ├── echart-event.jar (改造后无JavaFX)
    ├── echart-render.jar (核心渲染逻辑)
    ├── echart-ui-render.jar (JavaFX渲染实现)
    ├── echart-data.jar (数据管理逻辑)
    ├── echart-alarm.jar (预警业务逻辑)
    ├── echart-ais.jar (AIS数据模型)
    ├── echart-route.jar (航线数据模型)
    ├── echart-workspace.jar
    ├── echart-theme.jar
    ├── echart-plugin.jar (核心插件接口)
    └── echart-app.jar

echart-ui.jar (JavaFX)
    ↑ 被依赖
    ├── echart-ui-render.jar
    ├── echart-theme.jar
    ├── echart-plugin.jar (UI扩展接口)
    └── echart-app.jar
```

### 4.3 可行性结论

**可行性等级：高**

拆分在技术上完全可行，主要工作是：
1. 将JavaFX UI类迁移至新JAR包
2. 对少量接口进行泛化抽象
3. 引入平台抽象接口（PlatformAdapter、RenderContext、AudioPlayer）
4. 调整Gradle依赖配置

---

## 五、拆分方案

### 5.1 拆分策略

采用**"核心下沉、UI上浮"**策略：
- 将平台无关的核心框架和业务逻辑保留在原JAR包
- 将JavaFX UI控件上浮至新的UI层JAR包
- 对耦合接口进行泛化抽象

### 5.2 新增JAR包定义

#### 5.2.1 echart-ui.jar（JavaFX UI层）

**描述**: JavaFX UI控件和布局实现

**包含内容**（从各JAR包迁移）:

| 来源JAR | 迁移类 | 职责 |
|---------|--------|------|
| echart-core.jar | `MainView` | 主布局容器 |
| echart-core.jar | `RibbonMenuBar` | Ribbon菜单栏 |
| echart-core.jar | `ActivityBar` | 活动栏 |
| echart-core.jar | `SideBarManager` | 侧边栏管理器 |
| echart-core.jar | `RightTabManager` | 右侧面板管理器 |
| echart-core.jar | `ChartDisplayArea` | 海图显示区 |
| echart-core.jar | `StatusBar` | 状态栏 |
| echart-core.jar | `ComponentHierarchy` | 组件层次结构 |
| echart-core.jar | `StyleManager` | 样式管理器 |
| echart-core.jar | `ResponsiveLayoutManager` | 响应式布局管理器 |
| echart-core.jar | `FxAppContext` | JavaFX应用上下文 |
| echart-data.jar | `DataCatalogPanel` | 数据目录面板 |
| echart-data.jar | `LayerManagerPanel` | 图层管理面板 |
| echart-alarm.jar | `AlarmPanel` | 预警面板 |
| echart-alarm.jar | `AlarmListView` | 预警列表视图 |
| echart-alarm.jar | `AlarmDetailPanel` | 预警详情面板 |
| echart-alarm.jar | `SoundPlayer` | 声音播放器 |
| echart-alarm.jar | `AlarmSoundSettings` | 预警声音设置 |
| echart-alarm.jar | `SoundSettingsPanel` | 声音设置面板 |
| echart-ais.jar | `AlarmDetailDisplay` | 预警详情显示 |
| echart-route.jar | `RouteCreator` | 航线创建器 |
| echart-route.jar | `RouteEditor` | 航线编辑器 |
| echart-route.jar | `RoutePanel` | 航线面板 |

**依赖**:
- echart-core.jar
- JavaFX 8
- ControlsFX
- fxribbon

**被依赖**:
- echart-ui-render.jar
- echart-theme.jar
- echart-app.jar

#### 5.2.2 echart-ui-render.jar（JavaFX渲染层）

**描述**: JavaFX Canvas渲染实现

**包含内容**（从echart-render.jar迁移）:

| 类名 | 职责 |
|------|------|
| `FxRenderContext` | JavaFX渲染上下文实现 |
| `FxCanvasRenderer` | JavaFX Canvas渲染器 |
| `FxChartElementRenderer` | JavaFX海图要素渲染器 |
| `FxTileCache` | JavaFX瓦片缓存（Image） |
| `FxMeasurementTool` | JavaFX测量工具 |
| `FxHighlightRenderer` | JavaFX高亮渲染器 |
| `FxViewOperator` | JavaFX视图操作器 |

**依赖**:
- echart-core.jar
- echart-render.jar
- echart-ui.jar
- JavaFX 8

**被依赖**:
- echart-app.jar

### 5.3 原JAR包改造详情

#### 5.3.1 echart-core.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `LifecycleComponent` | 生命周期接口 | 无变化 |
| `AbstractLifecycleComponent` | 生命周期基类 | 无变化 |
| `LRUCache<K,V>` | LRU缓存工具 | 无变化 |
| `AppEvent` | 事件数据类 | 无变化 |
| `AppEventType` | 事件类型枚举 | 无变化 |
| `AppEventBus` | 事件总线 | **改造**：移除Platform.runLater，使用PlatformAdapter |
| `PlatformAdapter` | 平台抽象接口 | **新增** |
| `ServiceLocator` | 服务定位器 | **新增** |
| `PanelDescriptor` | 面板描述接口 | **新增**（泛化SideBarPanel） |
| `TabDescriptor` | 标签页描述接口 | **新增**（泛化RightTabPanel） |
| `ErrorCode` | 错误码枚举 | 无变化 |
| `ErrorHandler` | 错误处理器接口 | 无变化 |
| `Extension` | 扩展点基接口 | **改造**：移除UI依赖 |
| `DataSourceExtension` | 数据源扩展接口 | 无变化 |
| `AlarmRuleExtension` | 预警规则扩展接口 | 无变化 |

**迁移内容**（移至echart-ui.jar）:
- MainView, RibbonMenuBar, ActivityBar, SideBarManager, RightTabManager
- ChartDisplayArea, StatusBar, ComponentHierarchy, StyleManager
- ResponsiveLayoutManager, AppContext

**依赖**:
- JDK 8 标准库（无JavaFX）

#### 5.3.2 echart-event.jar 改造

**保留内容**:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `EventBus` | 事件总线 | **改造**：使用PlatformAdapter |
| `EventType` | 事件类型枚举 | 无变化 |
| `Event` | 事件基类 | 无变化 |
| `EventHandler` | 事件处理器接口 | 无变化 |
| `EventDispatcher` | 事件分发器 | **改造**：使用PlatformAdapter |

**改造示例**:
```java
public class EventBus {
    private PlatformAdapter platformAdapter;
    
    public void setPlatformAdapter(PlatformAdapter adapter) {
        this.platformAdapter = adapter;
    }
    
    private void dispatchEvent(Event event) {
        if (platformAdapter != null && platformAdapter.isUiThread()) {
            invokeHandlers(handlers, event);
        } else if (platformAdapter != null) {
            platformAdapter.runOnUi(() -> invokeHandlers(handlers, event));
        } else {
            invokeHandlers(handlers, event);
        }
    }
}
```

**依赖**:
- echart-core.jar
- JDK 8 标准库（无JavaFX）

#### 5.3.3 echart-render.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `RenderEngine` | 渲染引擎核心逻辑 | **改造**：使用RenderContext接口 |
| `RenderContext` | 渲染上下文接口 | **新增** |
| `LayerManager` | 图层管理器 | 无变化 |
| `JNIBridge` | JNI桥接 | 无变化 |
| `DirtyRegionManager` | 脏区域管理器 | **改造**：使用平台无关的Rectangle类 |
| `LODStrategy` | LOD策略 | 无变化 |
| `PerformanceMonitor` | 性能监控器 | 无变化 |
| `S52SymbolLibrary` | S-52符号库 | 无变化 |
| `SymbolStyleCustomizer` | 符号样式定制器 | 无变化 |

**迁移内容**（移至echart-ui-render.jar）:
- CanvasRenderer, ChartElementRenderer, TileCache
- MeasurementTool, HighlightRenderer, ViewOperator

**依赖**:
- echart-core.jar
- JNI Bridge (C++)
- JDK 8 标准库（无JavaFX）

#### 5.3.4 echart-data.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `ChartFileManager` | 海图文件管理器 | 无变化 |
| `DataImporter` | 数据导入器 | 无变化 |
| `DataExporter` | 数据导出器 | 无变化 |
| `LayerManager` | 图层管理逻辑 | 无变化 |
| 数据模型类 | 数据结构定义 | 无变化 |

**迁移内容**（移至echart-ui.jar）:
- DataCatalogPanel, LayerManagerPanel

**依赖**:
- echart-core.jar
- JNI Bridge (C++)
- JDK 8 标准库（无JavaFX）

#### 5.3.5 echart-alarm.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `AlarmManager` | 预警管理器 | 无变化 |
| `AlarmType` | 预警类型枚举 | 无变化 |
| `Alarm` | 预警数据模型 | 无变化 |
| `AlarmNotifier` | 预警通知器 | **改造**：使用PlatformAdapter |
| `AlarmResponseHandler` | 预警响应处理器 | 无变化 |
| `CollisionAlarm` | 碰撞预警逻辑 | 无变化 |
| `DeviationAlarm` | 偏航预警逻辑 | 无变化 |
| `ShallowWaterAlarm` | 浅水预警逻辑 | 无变化 |
| `RestrictedAreaAlarm` | 禁航区预警逻辑 | 无变化 |
| `WeatherAlarm` | 气象预警逻辑 | 无变化 |
| `WatchAlarm` | 值班报警逻辑 | 无变化 |
| `AlarmHistory` | 预警历史 | 无变化 |
| `AlarmStatistics` | 预警统计 | 无变化 |
| `AudioPlayer` | 音频播放接口 | **新增** |
| `AlarmSuppressionManager` | 预警抑制管理器 | 无变化 |
| `AlarmPersistence` | 预警持久化 | 无变化 |
| `AlarmRuleEngine` | 预警规则引擎 | 无变化 |

**迁移内容**（移至echart-ui.jar）:
- AlarmPanel, AlarmListView, AlarmDetailPanel
- SoundPlayer, AlarmSoundSettings, SoundSettingsPanel

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- JDK 8 标准库（无JavaFX）

#### 5.3.6 echart-ais.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `AISTargetManager` | AIS目标管理器 | 无变化 |
| `AISTarget` | AIS目标数据模型 | 无变化 |
| `AISAlarmAssociation` | AIS预警关联 | 无变化 |
| `CPATCPACalculator` | CPA/TCPA计算器 | 无变化 |

**迁移内容**（移至echart-ui.jar）:
- AlarmDetailDisplay

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-render.jar
- echart-alarm.jar
- JNI Bridge (C++)
- JDK 8 标准库（无JavaFX）

#### 5.3.7 echart-route.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `RouteManager` | 航线管理器 | 无变化 |
| `Route` | 航线数据模型 | 无变化 |
| `Waypoint` | 航点数据模型 | 无变化 |
| `WaypointManager` | 航点管理器 | 无变化 |
| `RouteImporter` | 航线导入器 | 无变化 |
| `RouteExporter` | 航线导出器 | 无变化 |
| `RouteChecker` | 航线检查器 | 无变化 |
| `RouteCalculator` | 航线计算器 | **新增**（从RouteCreator提取逻辑） |

**迁移内容**（移至echart-ui.jar）:
- RouteCreator, RouteEditor, RoutePanel

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- JDK 8 标准库（无JavaFX）

#### 5.3.8 echart-plugin.jar 改造

**保留内容**（纯Java，无JavaFX依赖）:

| 类/接口 | 职责 | 改造说明 |
|--------|------|---------|
| `PluginLoader` | 插件加载器 | 无变化 |
| `PluginSecurityManager` | 插件安全管理器 | 无变化 |
| `PluginManager` | 插件管理器 | 无变化 |
| `Extension` | 扩展点基接口 | **改造**：移除UI依赖 |
| `ExtensionContext` | 扩展上下文 | **改造**：移除Stage依赖 |
| `ExtensionDescriptor` | 扩展描述接口 | **新增** |
| `DataSourceExtension` | 数据源扩展接口 | 无变化 |
| `AlarmRuleExtension` | 预警规则扩展接口 | 无变化 |

**迁移内容**（移至echart-ui.jar）:
- RibbonExtension, SideBarExtension, RightPanelExtension, ToolExtension

**依赖**:
- echart-core.jar
- echart-alarm.jar
- JDK 8 标准库（无JavaFX）

### 5.4 新增抽象接口汇总

| 接口名 | 所在JAR | 职责 | 实现位置 |
|--------|---------|------|---------|
| `PlatformAdapter` | echart-core.jar | 平台线程抽象 | echart-ui.jar (FxPlatformAdapter) |
| `RenderContext` | echart-render.jar | 渲染上下文抽象 | echart-ui-render.jar (FxRenderContext) |
| `AudioPlayer` | echart-alarm.jar | 音频播放抽象 | echart-ui.jar (FxAudioPlayer) |
| `PanelDescriptor` | echart-core.jar | 面板描述抽象 | echart-ui.jar (FxSideBarPanel) |
| `ExtensionDescriptor` | echart-plugin.jar | 扩展描述抽象 | echart-ui.jar (FxRibbonExtension等) |

### 5.5 拆分后JAR包总览

| 序号 | JAR包名称 | 层级 | JavaFX依赖 | 变化说明 |
|------|-----------|------|-----------|---------|
| 1 | echart-core.jar | 核心层 | ❌ 无 | 移除UI类，保留纯Java核心 |
| 2 | **echart-ui.jar** | **UI层(新增)** | ✅ 有 | **新增JAR包，承载JavaFX UI** |
| 3 | echart-event.jar | 基础服务层 | ❌ 无 | 改造后无JavaFX依赖 |
| 4 | echart-i18n.jar | 基础服务层 | ❌ 无 | 无变化 |
| 5 | echart-render.jar | 核心功能层 | ❌ 无 | 保留渲染逻辑，移除Canvas实现 |
| 6 | **echart-ui-render.jar** | **UI层(新增)** | ✅ 有 | **新增JAR包，承载JavaFX渲染** |
| 7 | echart-data.jar | 核心功能层 | ❌ 无 | 移除UI面板 |
| 8 | echart-alarm.jar | 业务功能层 | ❌ 无 | 保留业务逻辑，移除UI |
| 9 | echart-ais.jar | 业务功能层 | ❌ 无 | 移除UI组件 |
| 10 | echart-route.jar | 业务功能层 | ❌ 无 | 保留数据模型，移除UI |
| 11 | echart-workspace.jar | 业务功能层 | ❌ 无 | 无变化 |
| 12 | echart-theme.jar | 扩展功能层 | ✅ 有 | 新增依赖echart-ui.jar |
| 13 | echart-plugin.jar | 扩展功能层 | ❌ 无 | 保留核心接口，移除UI扩展 |
| 14 | echart-app.jar | 应用层 | ✅ 有 | 新增依赖echart-ui.jar |

### 5.6 拆分后依赖关系图

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Layer 5)                        │
│                     echart-app.jar                          │
└─────────────────────────────────────────────────────────────┘
          │                              │
          │ (依赖JavaFX UI)              │ (依赖业务模块)
          ▼                              ▼
┌──────────────────┐  ┌─────────────────────────────────────┐
│    UI层 (新增)    │  │          扩展功能层 (Layer 4)        │
│ echart-ui.jar    │  │  echart-theme.jar  echart-plugin.jar│
│ echart-ui-render │  └─────────────────────────────────────┘
└──────────────────┘                    │
          │                             │
          ▼                             ▼
┌──────────────────┐  ┌─────────────────────────────────────┐
│   核心层 (Layer 0)│  │          业务功能层 (Layer 3)        │
│ echart-core.jar  │  │ echart-alarm echart-ais echart-route│
│ (纯Java，无JavaFX)│  │        echart-workspace             │
└──────────────────┘  └─────────────────────────────────────┘
          │                             │
          ▼                             ▼
┌──────────────────┐  ┌─────────────────────────────────────┐
│ 基础服务层(Layer1)│  │          核心功能层 (Layer 2)        │
│echart-event.jar  │  │ echart-render.jar  echart-data.jar  │
│echart-i18n.jar   │  │ (核心逻辑，无JavaFX)                  │
└──────────────────┘  └─────────────────────────────────────┘
          │                             │
          └──────────────┬──────────────┘
                         ▼
              ┌─────────────────────┐
              │   JNI Bridge (C++)  │
              │  ogc_chart_jni.dll  │
              └─────────────────────┘
```

---

## 六、多平台复用映射

### 6.1 各平台可复用JAR包

| JAR包 | Android | Web | 桌面(JavaFX) | 说明 |
|-------|---------|-----|-------------|------|
| echart-core.jar | ✅ | ✅ | ✅ | 核心框架，完全复用 |
| echart-ui.jar | ❌ | ❌ | ✅ | 需实现Android/Web UI |
| echart-ui-render.jar | ❌ | ❌ | ✅ | 需实现Android/Web渲染 |
| echart-event.jar | ✅ | ✅ | ✅ | 事件总线，完全复用 |
| echart-i18n.jar | ✅ | ✅ | ✅ | 国际化，完全复用 |
| echart-render.jar | ✅ | ✅ | ✅ | 渲染逻辑，需实现RenderContext |
| echart-data.jar | ✅ | ✅ | ✅ | 数据管理，完全复用 |
| echart-alarm.jar | ✅ | ✅ | ✅ | 预警逻辑，完全复用 |
| echart-ais.jar | ✅ | ✅ | ✅ | AIS数据模型，完全复用 |
| echart-route.jar | ✅ | ✅ | ✅ | 航线数据模型，完全复用 |
| echart-workspace.jar | ✅ | ✅ | ✅ | 工作区管理，完全复用 |
| echart-theme.jar | ❌ | ❌ | ✅ | 需实现Android/Web主题 |
| echart-plugin.jar | ✅ | ✅ | ✅ | 核心插件接口，完全复用 |
| echart-app.jar | ❌ | ❌ | ✅ | 需实现Android/Web入口 |

### 6.2 Android平台实现示例

```java
// Android平台适配器
public class AndroidPlatformAdapter implements PlatformAdapter {
    private Handler uiHandler = new Handler(Looper.getMainLooper());
    
    @Override
    public boolean isUiThread() {
        return Looper.myLooper() == Looper.getMainLooper();
    }
    
    @Override
    public void runOnUi(Runnable action) {
        uiHandler.post(action);
    }
}

// Android渲染上下文
public class AndroidRenderContext implements RenderContext {
    private Canvas canvas;
    private Paint paint;
    
    @Override
    public void clear(double x, double y, double w, double h) {
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    }
    
    @Override
    public void fillRect(double x, double y, double w, double h) {
        paint.setStyle(Paint.Style.FILL);
        canvas.drawRect((float)x, (float)y, (float)(x+w), (float)(y+h), paint);
    }
    // ...
}

// Android音频播放器
public class AndroidAudioPlayer implements AudioPlayer {
    private MediaPlayer mediaPlayer;
    
    @Override
    public void load(String resourcePath) {
        AssetFileDescriptor afd = context.getAssets().openFd(resourcePath);
        mediaPlayer = new MediaPlayer();
        mediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
        mediaPlayer.prepare();
    }
    
    @Override
    public void play() {
        mediaPlayer.start();
    }
    // ...
}
```

### 6.3 Web平台实现示例

```java
// Web平台适配器（使用GWT/Vaadin）
public class WebPlatformAdapter implements PlatformAdapter {
    @Override
    public boolean isUiThread() {
        return true; // Web环境始终在UI线程
    }
    
    @Override
    public void runOnUi(Runnable action) {
        action.run(); // 直接执行
    }
}

// Web渲染上下文（使用HTML5 Canvas）
public class WebRenderContext implements RenderContext {
    private JavaScriptObject canvasContext;
    
    @Override
    public native void clear(double x, double y, double w, double h) /*-{
        this.canvasContext.clearRect(x, y, w, h);
    }-*/;
    
    @Override
    public native void fillRect(double x, double y, double w, double h) /*-{
        this.canvasContext.fillRect(x, y, w, h);
    }-*/;
    // ...
}
```

---

## 七、实施计划

### 7.1 阶段一：核心层改造（3天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 创建echart-ui.jar项目结构 | 2h | Gradle配置、目录结构 |
| 迁移echart-core.jar的UI类 | 4h | MainView、RibbonMenuBar等 |
| 创建PlatformAdapter接口 | 2h | 平台抽象接口 |
| 改造AppEventBus | 2h | 移除Platform.runLater依赖 |
| 创建ServiceLocator | 2h | 替代AppContext核心功能 |
| 改造面板接口 | 4h | PanelDescriptor、TabDescriptor |
| 编译验证 | 2h | 确保echart-core.jar无JavaFX依赖 |

### 7.2 阶段二：事件层改造（1天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 改造EventBus | 2h | 使用PlatformAdapter |
| 改造EventDispatcher | 2h | 使用PlatformAdapter |
| 编译验证 | 2h | 确保echart-event.jar无JavaFX依赖 |

### 7.3 阶段三：渲染层改造（3天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 创建echart-ui-render.jar项目结构 | 2h | Gradle配置、目录结构 |
| 创建RenderContext接口 | 4h | 渲染抽象接口 |
| 迁移Canvas渲染类 | 6h | CanvasRenderer等 |
| 创建FxRenderContext实现 | 4h | JavaFX渲染实现 |
| 改造DirtyRegionManager | 2h | 使用平台无关的Rectangle |
| 编译验证 | 2h | 确保echart-render.jar无JavaFX依赖 |

### 7.4 阶段四：数据层改造（1天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 迁移DataCatalogPanel | 2h | 移至echart-ui.jar |
| 迁移LayerManagerPanel | 2h | 移至echart-ui.jar |
| 编译验证 | 2h | 确保echart-data.jar无JavaFX依赖 |

### 7.5 阶段五：预警层改造（2天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 创建AudioPlayer接口 | 2h | 音频抽象接口 |
| 迁移预警UI类 | 4h | AlarmPanel、AlarmListView等 |
| 迁移声音相关类 | 2h | SoundPlayer、SoundSettingsPanel |
| 改造AlarmNotifier | 2h | 使用PlatformAdapter |
| 编译验证 | 2h | 确保echart-alarm.jar无JavaFX依赖 |

### 7.6 阶段六：AIS层改造（0.5天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 迁移AlarmDetailDisplay | 2h | 移至echart-ui.jar |
| 编译验证 | 1h | 确保echart-ais.jar无JavaFX依赖 |

### 7.7 阶段七：航线层改造（1天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 迁移RouteCreator | 2h | 移至echart-ui.jar |
| 迁移RouteEditor | 2h | 移至echart-ui.jar |
| 迁移RoutePanel | 2h | 移至echart-ui.jar |
| 编译验证 | 1h | 确保echart-route.jar无JavaFX依赖 |

### 7.8 阶段八：插件层改造（1天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 创建ExtensionDescriptor接口 | 2h | 扩展描述抽象 |
| 迁移UI扩展接口 | 3h | RibbonExtension等 |
| 改造ExtensionContext | 2h | 移除Stage依赖 |
| 编译验证 | 1h | 确保echart-plugin.jar无JavaFX依赖 |

### 7.9 阶段九：集成测试（2天）

| 任务 | 工时 | 说明 |
|------|------|------|
| 更新依赖配置 | 2h | 所有JAR包的Gradle配置 |
| 编译验证 | 2h | 确保所有核心JAR包无JavaFX依赖 |
| 功能测试 | 4h | 确保JavaFX桌面应用功能正常 |
| 多平台验证 | 4h | 验证核心JAR包可在无JavaFX环境编译 |

### 7.10 总体时间估计

| 阶段 | 工作量 | 优先级 |
|------|--------|--------|
| 核心层改造 | 3天 | P0 |
| 事件层改造 | 1天 | P0 |
| 渲染层改造 | 3天 | P0 |
| 数据层改造 | 1天 | P1 |
| 预警层改造 | 2天 | P1 |
| AIS层改造 | 0.5天 | P1 |
| 航线层改造 | 1天 | P1 |
| 插件层改造 | 1天 | P2 |
| 集成测试 | 2天 | P0 |
| **总计** | **14.5天** | - |

---

## 八、风险评估与缓解措施

### 8.1 技术风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|---------|
| 接口改造导致兼容性问题 | 中 | 高 | 保持向后兼容的默认方法 |
| 渲染抽象层性能下降 | 低 | 中 | 使用桥接模式，减少转换开销 |
| 插件系统扩展性受限 | 低 | 中 | 提供平台特定的扩展接口 |
| 编译隔离不彻底 | 中 | 高 | 使用Gradle依赖验证插件 |

### 8.2 项目风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|---------|
| 改造工作量超预期 | 中 | 中 | 分阶段实施，优先核心层 |
| 测试覆盖不足 | 中 | 高 | 增加单元测试和集成测试 |
| 文档更新滞后 | 低 | 低 | 同步更新设计文档 |

---

## 九、结论

### 9.1 必要性总结

将UI控件从多个核心JAR包中抽离是**高度必要**的，原因如下：

1. **多平台复用需求**：Android/Web平台无法使用JavaFX，必须拆分
2. **职责分离原则**：业务逻辑与UI控件混合违反单一职责原则
3. **测试便利性**：核心逻辑测试不应依赖JavaFX运行环境
4. **部署灵活性**：不同平台可选择不同的UI实现

### 9.2 可行性总结

拆分方案在技术上**完全可行**：

1. **依赖可分离**：所有JavaFX依赖均可通过抽象或迁移解耦
2. **接口可兼容**：通过泛化接口保持向后兼容
3. **工作量可控**：主要是接口抽象和类迁移，非重写
4. **可验证**：可通过Gradle配置验证无JavaFX引用

### 9.3 推荐方案

采用**"核心下沉、UI上浮"**策略，将8个JAR包中的UI控件迁移至2个新增的UI层JAR包：

- **echart-ui.jar**：承载所有JavaFX UI控件
- **echart-ui-render.jar**：承载JavaFX Canvas渲染实现

原JAR包保留核心业务逻辑，通过抽象接口实现平台无关性。

---

**版本**: v1.0  
**作者**: 系统架构师  
**日期**: 2026-04-17
