# 海图显示预警应用 - JAR包拆分实施任务计划

> **版本**: v1.3  
> **日期**: 2026-04-20  
> **依据**: echart_tasks_jar_lists_ui.md v2.3, echart_display_alarm_app_layout_new_tasks.md v2.0

---

## 概述

### 项目信息

| 项目 | 内容 |
|------|------|
| **项目名称** | 海图显示预警应用 - JAR包拆分实施 |
| **总任务数** | 15个JAR包 + 136个功能任务 |
| **预估总工时** | 1020h（JAR包拆分） |
| **目标周期** | 15周 |
| **团队规模** | 4名开发者 |

### 参考文档

| 文档 | 说明 |
|------|------|
| echart_tasks_jar_lists_ui.md | JAR包分配清单（UI拆分版）v2.3 |
| echart_display_alarm_app_layout_new_tasks.md | 海图显示预警应用布局任务计划 v2.0 |
| echart_tasks_jar_split_ui_javafx.md | JAR包UI拆分方案 v1.0 |

### 技术约束

| 约束项 | 要求 |
|--------|------|
| **Java版本** | JDK 8+ (1.8.0_60+) |
| **JavaFX版本** | JavaFX 8 (内置于JDK 8) |
| **构建工具** | Gradle 4.5.1 |
| **核心原则** | 核心JAR包无JavaFX依赖，UI类迁移至UI层JAR包 |

---

## 一、实施策略

### 1.1 实施原则

1. **自底向上**：按层级从Layer 0到Layer 7依次实施
2. **先拆后建**：先从现有代码中拆分出核心逻辑，再构建新的UI层
3. **接口先行**：每个JAR包先定义接口，再实现功能
4. **持续验证**：每完成一个JAR包即进行编译和单元测试验证

### 1.2 实施阶段

| 阶段 | 内容 | 周期 | 涉及JAR包 |
|------|------|------|----------|
| **阶段1** | 核心层 + 基础服务层 | 第1-3周 | core, event, i18n |
| **阶段2** | 核心功能层 | 第4-5周 | render, data |
| **阶段3** | 业务功能层 | 第6-9周 | alarm, ais, route, workspace |
| **阶段4** | UI层 + 扩展功能层 | 第10-12周 | ui, ui-render, theme, plugin |
| **阶段5** | 服务门面层 + 应用层 | 第13-15周 | facade, app |

---

## 二、JAR包实施任务清单

### 2.1 阶段1：核心层 + 基础服务层（第1-3周）

#### JAR-01: echart-core.jar（Layer 0，44h）

**优先级**: P0 | **负责人**: Dev B | **状态**: ✅ Done

**目标**: 建立平台无关的核心框架，定义系统基础接口和数据结构

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J01-01 | Gradle项目结构搭建 | 4h | - | T1 | ✅ Done |
| J01-02 | 组件层次结构实现 | 6h | J01-01 | T9 | ✅ Done |
| J01-03 | 事件总线设计（平台无关） | 8h | J01-01 | T22 | ✅ Done |
| J01-04 | 事件类型定义 | 6h | J01-03 | T23 | ✅ Done |
| J01-05 | PlatformAdapter接口定义 | 4h | J01-01 | 新增 | ✅ Done |
| J01-06 | ServiceLocator实现 | 4h | J01-01 | 新增 | ✅ Done |
| J01-07 | PanelDescriptor接口定义 | 4h | J01-01 | 新增 | ✅ Done |
| J01-08 | TabDescriptor接口定义 | 4h | J01-01 | 新增 | ✅ Done |
| J01-09 | ExtensionDescriptor接口定义 | 4h | J01-01 | 新增 | ✅ Done |

**产出类**:
- `LifecycleComponent`, `AbstractLifecycleComponent`
- `AppEventBus`, `AppEvent`, `AppEventType`
- `PlatformAdapter`, `ServiceLocator`
- `PanelDescriptor`, `TabDescriptor`, `ExtensionDescriptor`
- `LRUCache<K,V>`, `ErrorCode`, `ErrorHandler`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 单元测试覆盖率 > 80%
- [ ] 所有接口有Javadoc文档

---

#### JAR-02: echart-event.jar（Layer 1，18h）

**优先级**: P0 | **负责人**: Dev B | **状态**: ✅ Done

**目标**: 事件总线和通信机制，实现模块间解耦通信

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J02-01 | EventBus实现（基于PlatformAdapter） | 8h | J01-05 | T22 | ✅ Done |
| J02-02 | EventType枚举定义 | 6h | J02-01 | T23 | ✅ Done |
| J02-03 | 通信示例实现 | 4h | J02-01, J02-02 | T24 | ✅ Done |

**产出类**:
- `EventBus`, `EventType`, `Event`, `EventHandler`, `EventDispatcher`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 事件发布/订阅功能正常
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-03: echart-i18n.jar（Layer 1，24h）

**优先级**: P1 | **负责人**: Dev A | **状态**: ✅ Done

**目标**: 国际化支持，多语言资源管理

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J03-01 | 国际化架构设计 | 6h | J01-01 | T70 | ✅ Done |
| J03-02 | 国际化管理器实现 | 8h | J03-01 | T71 | ✅ Done |
| J03-03 | 多语言资源文件 | 10h | J03-02 | T72 | ✅ Done |

**产出类**:
- `I18nManager`, `ResourceBundleLoader`, `LocaleProvider`, `MessageFormatter`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 中英文切换正常
- [ ] 单元测试覆盖率 > 80%

---

### 2.2 阶段2：核心功能层（第4-5周）

#### JAR-04: echart-render.jar（Layer 2，112h）

**优先级**: P0 | **负责人**: Dev B | **状态**: ✅ Done

**目标**: 渲染引擎核心逻辑，负责海图要素渲染和性能优化（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J04-01 | RenderContext接口定义 | 4h | J01-01 | 新增 | ✅ Done |
| J04-02 | 分层渲染架构设计 | 8h | J04-01 | T11 | ✅ Done |
| J04-03 | 渲染数据流实现 | 10h | J04-02 | T12 | ✅ Done |
| J04-04 | 层间通信机制 | 6h | J04-02 | T13 | ✅ Done |
| J04-05 | 渲染优先级管理 | 6h | J04-02 | T14 | ✅ Done |
| J04-06 | JNI渲染桥接 | 16h | J04-02 | T16 | ✅ Done |
| J04-07 | 多图层叠加渲染 | 8h | J04-06 | T18 | ✅ Done |
| J04-08 | 脏区域重绘机制 | 10h | J04-02 | T26 | ✅ Done |
| J04-09 | LOD细节层次策略 | 8h | J04-08 | T28 | ✅ Done |
| J04-10 | 渲染性能监控 | 6h | J04-08 | T29 | ✅ Done |
| J04-11 | S-52符号库集成 | 10h | J04-06 | T107 | ✅ Done |
| J04-12 | 符号样式自定义 | 6h | J04-11 | T108 | ✅ Done |
| J04-13 | 性能监控机制完善 | 6h | J04-10 | T111 | ✅ Done |
| J04-14 | 内存泄漏检测集成 | 4h | J04-10 | T112 | ✅ Done |
| J04-15 | RenderEngine改造（使用RenderContext） | 4h | J04-01 | 新增 | ✅ Done |

**产出类**:
- `RenderEngine`, `RenderContext`, `LayerManager`, `JNIBridge`
- `DirtyRegionManager`, `LODStrategy`, `PerformanceMonitor`
- `S52SymbolLibrary`, `SymbolStyleCustomizer`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] RenderContext接口可被不同平台实现
- [ ] JNI桥接功能正常
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-05: echart-data.jar（Layer 2，20h）

**优先级**: P0 | **负责人**: Dev D | **状态**: ✅ Done

**目标**: 数据管理核心逻辑，海图文件和图层管理（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J05-01 | 数据模型类实现 | 4h | J01-01 | 新增 | ✅ Done |
| J05-02 | 海图文件管理功能 | 8h | J05-01 | T94 | ✅ Done |
| J05-03 | 数据导入导出功能 | 8h | J05-01 | T96 | ✅ Done |

**产出类**:
- `ChartFileManager`, `DataImporter`, `DataExporter`, `LayerManager`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 海图文件读写正常
- [ ] 单元测试覆盖率 > 80%

---

### 2.3 阶段3：业务功能层（第6-9周）

#### JAR-06: echart-alarm.jar（Layer 3，96h）

**优先级**: P0 | **负责人**: Dev D | **状态**: ⚠️ Partial

**目标**: 预警系统核心逻辑，实现各类预警功能（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J06-01 | AudioPlayer接口定义 | 2h | J01-01 | 新增 | ✅ Done |
| J06-02 | AlarmNotifier改造 | 2h | J06-01 | 新增 | ✅ Done |
| J06-03 | 预警类型定义 | 6h | J02-01 | T34 | ✅ Done |
| J06-04 | 预警通知机制（平台无关） | 10h | J06-03 | T36 | ✅ Done |
| J06-05 | 预警响应流程 | 8h | J06-03 | T37 | ✅ Done |
| J06-06 | 碰撞预警实现 | 12h | J06-03 | T38 | ✅ Done |
| J06-07 | 偏航预警实现 | 8h | J06-03 | T39 | ✅ Done |
| J06-08 | 浅水预警实现 | 8h | J06-03 | T40 | ✅ Done |
| J06-09 | 禁航区预警实现 | 6h | J06-03 | T41 | ✅ Done |
| J06-10 | 气象预警实现 | 6h | J06-03 | T42 | ✅ Done |
| J06-11 | 值班报警实现 | 8h | J06-03 | T43 | ✅ Done |
| J06-12 | 预警历史记录 | 6h | J06-03 | T44 | ✅ Done |
| J06-13 | 预警统计功能 | 6h | J06-12 | T45 | ✅ Done |
| J06-14 | 预警抑制管理功能 | 6h | J06-03 | T115 | ✅ Done |
| J06-15 | 预警持久化功能 | 4h | J06-12 | T116 | ✅ Done |
| J06-16 | 预警规则引擎实现 | 6h | J06-03 | T126 | ✅ Done |
| J06-17 | 预警端到端测试 | 6h | J06-16 | T127 | 📋 Todo |
| J06-18 | 并发预警测试 | 4h | J06-06 | T117 | 📋 Todo |
| J06-19 | 预警抑制测试 | 4h | J06-14 | T118 | 📋 Todo |
| J06-20 | 预警无障碍通知 | 4h | J06-04 | T130 | 📋 Todo |
| J06-21 | 预警通知队列管理 | 4h | J06-04 | T132 | 📋 Todo |

**产出类**:
- `AlarmManager`, `AlarmType`, `Alarm`, `AlarmNotifier`, `AudioPlayer`
- `CollisionAlarm`, `DeviationAlarm`, `ShallowWaterAlarm`
- `RestrictedAreaAlarm`, `WeatherAlarm`, `WatchAlarm`
- `AlarmHistory`, `AlarmStatistics`, `AlarmSuppressionManager`
- `AlarmPersistence`, `AlarmRuleEngine`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] AudioPlayer为接口，无JavaFX实现
- [ ] 各类预警逻辑正确
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-07: echart-ais.jar（Layer 3，32h）

**优先级**: P0 | **负责人**: Dev D | **状态**: ✅ Done

**目标**: AIS集成核心逻辑，AIS目标数据模型和CPA/TCPA计算（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J07-01 | AIS目标交互逻辑 | 10h | J04-06 | T65 | ✅ Done |
| J07-02 | AIS预警关联机制 | 8h | J07-01, J06-03 | T66 | ✅ Done |
| J07-03 | AIS目标数据模型 | 6h | J07-01 | T68 | ✅ Done |
| J07-04 | CPA/TCPA计算 | 8h | J07-03 | T69 | ✅ Done |

**产出类**:
- `AISTargetManager`, `AISTarget`, `AISAlarmAssociation`, `CPATCPACalculator`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] CPA/TCPA计算结果正确
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-08: echart-route.jar（Layer 3，34h）

**优先级**: P0 | **负责人**: Dev D | **状态**: ✅ Done

**目标**: 航线规划核心逻辑，航线数据模型和管理（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J08-01 | 航线数据模型设计 | 6h | J02-01 | T86 | ✅ Done |
| J08-02 | 航点管理功能实现 | 8h | J08-01 | T89 | ✅ Done |
| J08-03 | 航线导入导出功能 | 6h | J08-01 | T90 | ✅ Done |
| J08-04 | 航线检查功能实现 | 8h | J08-01 | T91 | ✅ Done |
| J08-05 | RouteCalculator实现 | 6h | J08-01 | 新增 | ✅ Done |

**产出类**:
- `RouteManager`, `Route`, `Waypoint`, `WaypointManager`
- `RouteImporter`, `RouteExporter`, `RouteChecker`, `RouteCalculator`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 航线数据模型完整
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-09: echart-workspace.jar（Layer 3，94h）

**优先级**: P0 | **负责人**: Dev D | **状态**: ✅ Done

**目标**: 工作区管理，工作区持久化和恢复（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J09-01 | 侧边栏面板接口 | 8h | J01-07 | T30 | ✅ Done |
| J09-02 | 面板管理器实现 | 10h | J09-01 | T31 | ✅ Done |
| J09-03 | 右侧标签页扩展 | 6h | J01-08 | T32 | ✅ Done |
| J09-04 | 状态持久化机制 | 8h | J09-02 | T33 | ✅ Done |
| J09-05 | 工作区数据结构 | 6h | J09-04 | T46 | ✅ Done |
| J09-06 | 工作区管理器实现 | 10h | J09-05 | T47 | ✅ Done |
| J09-07 | 工作区持久化 | 8h | J09-06 | T48 | ✅ Done |
| J09-08 | 工作区恢复机制 | 6h | J09-07 | T49 | ✅ Done |
| J09-09 | 工作区导出导入 | 8h | J09-06 | T50 | ✅ Done |
| J09-10 | 错误分类与处理 | 6h | J02-01 | T51 | ✅ Done |
| J09-11 | 错误码定义 | 4h | J09-10 | T52 | ✅ Done |
| J09-12 | 错误边界架构 | 8h | J09-10 | T53 | ✅ Done |
| J09-13 | 错误恢复机制 | 6h | J09-12 | T54 | ✅ Done |

**产出类**:
- `WorkspaceManager`, `Workspace`, `WorkspacePersistence`, `WorkspaceRecovery`
- `WorkspaceExporter`, `WorkspaceImporter`
- `ErrorHandler`, `ErrorCode`, `ErrorBoundary`, `ErrorRecovery`
- `PanelManager`, `TabExtension`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 工作区保存/恢复正常
- [ ] 单元测试覆盖率 > 80%

---

### 2.4 阶段4：UI层 + 扩展功能层（第10-12周）

#### JAR-10: echart-ui.jar（Layer 4，148h）

**优先级**: P0 | **负责人**: Dev A | **状态**: ✅ Done

**目标**: JavaFX UI控件和布局实现，从各JAR包迁移的UI类

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J10-01 | FxPlatformAdapter实现 | 2h | J01-05 | 新增 | ✅ Done |
| J10-02 | FxAppContext实现 | 4h | J01-06 | 新增 | ✅ Done |
| J10-03 | FxSideBarPanel接口 | 2h | J01-07 | 新增 | ✅ Done |
| J10-04 | FxRightTabPanel接口 | 2h | J01-08 | 新增 | ✅ Done |
| J10-05 | 主布局框架实现 | 8h | J10-01, J10-02 | T2 | ✅ Done |
| J10-06 | Ribbon菜单栏实现 | 12h | J10-05 | T3 | ✅ Done |
| J10-07 | 活动栏实现 | 6h | J10-05 | T4 | ✅ Done |
| J10-08 | 侧边栏框架实现 | 8h | J10-05 | T5 | ✅ Done |
| J10-09 | 海图显示区框架 | 6h | J10-05 | T6 | ✅ Done |
| J10-10 | 右侧面板框架 | 8h | J10-05 | T7 | ✅ Done |
| J10-11 | 状态栏实现 | 4h | J10-05 | T8 | ✅ Done |
| J10-12 | 基础样式系统 | 6h | J10-05 | T10 | ✅ Done |
| J10-13 | 数据目录面板实现 | 10h | J10-08, J05-02 | T93 | ✅ Done |
| J10-14 | 图层管理面板实现 | 8h | J10-08, J05-02 | T95 | ✅ Done |
| J10-15 | 预警面板设计实现 | 12h | J10-10, J06-03 | T35 | ✅ Done |
| J10-16 | 声音播放器实现（JavaFX） | 6h | J06-01 | T113 | ✅ Done |
| J10-17 | 预警声音设置界面 | 4h | J10-16 | T114 | ✅ Done |
| J10-18 | 预警详情显示 | 6h | J07-02 | T67 | ✅ Done |
| J10-19 | 航线创建功能实现 | 10h | J08-01 | T87 | ✅ Done |
| J10-20 | 航线编辑功能实现 | 8h | J10-19 | T88 | ✅ Done |
| J10-21 | 航线面板实现 | 10h | J10-08, J08-01 | T92 | ✅ Done |
| J10-22 | 错误提示UI | 6h | J09-12 | T55 | ✅ Done |

**产出类**:
- `FxPlatformAdapter`, `FxAppContext`, `FxSideBarPanel`, `FxRightTabPanel`
- `MainView`, `RibbonMenuBar`, `ActivityBar`, `SideBarManager`
- `RightTabManager`, `ChartDisplayArea`, `StatusBar`, `StyleManager`
- `DataCatalogPanel`, `LayerManagerPanel`
- `AlarmPanel`, `AlarmListView`, `AlarmDetailPanel`
- `SoundPlayer`（JavaFX实现）, `AlarmSoundSettings`
- `AlarmDetailDisplay`, `RouteCreator`, `RouteEditor`, `RoutePanel`
- `ErrorPromptUI`

**验证标准**:
- [ ] 所有UI类已从原JAR包迁移
- [ ] 原JAR包中无残留JavaFX依赖
- [ ] UI功能与迁移前一致
- [ ] 单元测试覆盖率 > 70%

---

#### JAR-11: echart-ui-render.jar（Layer 5，84h）

**优先级**: P0 | **负责人**: Dev A | **状态**: ✅ Done

**目标**: JavaFX Canvas渲染实现，从echart-render.jar迁移的渲染类

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J11-01 | FxRenderContext实现 | 6h | J04-01 | 新增 | ✅ Done |
| J11-02 | FxCanvasRenderer实现 | 4h | J11-01 | 新增 | ✅ Done |
| J11-03 | Canvas渲染集成 | 12h | J11-01, J10-09 | T15 | ✅ Done |
| J11-04 | 海图要素渲染 | 12h | J11-03 | T17 | ✅ Done |
| J11-05 | 瓦片缓存策略 | 12h | J11-03 | T27 | ✅ Done |
| J11-06 | 测量工具基础框架 | 6h | J11-04 | T97 | ✅ Done |
| J11-07 | 距离测量功能 | 4h | J11-06 | T98 | ✅ Done |
| J11-08 | 面积测量功能 | 4h | J11-06 | T99 | ✅ Done |
| J11-09 | 方位测量功能 | 4h | J11-06 | T100 | ✅ Done |
| J11-10 | 海图要素交互实现 | 8h | J11-04 | T101 | ✅ Done |
| J11-11 | 高亮渲染配置实现 | 6h | J11-10 | T102 | ✅ Done |
| J11-12 | 视图操作功能实现 | 6h | J11-03 | T103 | ✅ Done |

**产出类**:
- `FxRenderContext`, `FxCanvasRenderer`, `FxChartElementRenderer`
- `FxTileCache`, `FxMeasurementTool`, `FxHighlightRenderer`, `FxViewOperator`

**验证标准**:
- [ ] 渲染功能与迁移前一致
- [ ] echart-render.jar中无JavaFX依赖
- [ ] 单元测试覆盖率 > 70%

---

#### JAR-12: echart-theme.jar（Layer 5，46h）

**优先级**: P1 | **负责人**: Dev A | **状态**: ✅ Done

**目标**: 主题管理，主题切换和样式定制

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J12-01 | 主题管理器设计 | 6h | J10-12 | T73 | ✅ Done |
| J12-02 | 主题切换实现 | 8h | J12-01 | T74 | ✅ Done |
| J12-03 | 高DPI检测与缩放 | 8h | J10-05 | T75 | ✅ Done |
| J12-04 | 高DPI布局调整 | 6h | J12-03 | T76 | ✅ Done |
| J12-05 | 图标资源适配 | 6h | J12-03 | T77 | ✅ Done |
| J12-06 | 暗色主题预警颜色适配 | 4h | J12-02 | T125 | ✅ Done |
| J12-07 | 高对比度检测实现 | 4h | J12-01 | T128 | ✅ Done |
| J12-08 | 高对比度主题CSS | 4h | J12-07 | T129 | ✅ Done |

**产出类**:
- `ThemeManager`, `ThemeSwitcher`, `HighDPIScaler`, `HighDPILayoutAdjuster`
- `IconResourceAdapter`, `HighContrastDetector`, `DarkThemeAlarmColorAdapter`（通过AlarmStatusEvent监听，不直接依赖alarm）

**验证标准**:
- [ ] 主题切换正常
- [ ] 高DPI显示正常
- [ ] 暗色主题预警颜色正确

---

#### JAR-13: echart-plugin.jar（Layer 5，24h）

**优先级**: P2 | **负责人**: Dev B | **状态**: ✅ Done

**目标**: 插件系统核心接口，插件加载和扩展接口（无JavaFX依赖）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J13-01 | 插件加载机制 | 8h | J01-09 | T119 | ✅ Done |
| J13-02 | 插件安全机制 | 6h | J13-01 | T120 | ✅ Done |
| J13-03 | 数据源扩展接口 | 6h | J13-01 | T121 | ✅ Done |
| J13-04 | 预警规则扩展接口 | 4h | J13-01 | T122 | ✅ Done |

**产出类**:
- `PluginLoader`, `PluginSecurityManager`, `PluginManager`
- `Extension`, `ExtensionContext`, `ExtensionDescriptor`
- `DataSourceExtension`, `AlarmRuleExtension`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 插件加载/卸载正常
- [ ] 单元测试覆盖率 > 80%

---

### 2.5 阶段5：服务门面层 + 应用层（第13-15周）

#### JAR-14: echart-facade.jar（Layer 6，16h）

**优先级**: P0 | **负责人**: Dev B | **状态**: ✅ Done

**目标**: 统一业务服务接口层，封装业务功能层的复杂调用，解耦功能层与应用层（不依赖JavaFX模块，保持多平台复用能力）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J14-01 | FacadeService接口设计 | 2h | J01-01 | 新增 | ✅ Done |
| J14-02 | AlarmFacade服务封装 | 2h | J14-01, J06-03 | 新增 | ✅ Done |
| J14-03 | RouteFacade服务封装 | 2h | J14-01, J08-01 | 新增 | ✅ Done |
| J14-04 | ChartFacade服务封装 | 2h | J14-01, J04-02 | 新增 | ✅ Done |
| J14-05 | AISFacade服务封装 | 2h | J14-01, J07-01 | 新增 | ✅ Done |
| J14-06 | ApplicationInitializer编排 | 4h | J14-01 | 新增 | ✅ Done |
| J14-07 | ServiceRegistry注册中心 | 2h | J14-01 | 新增 | ✅ Done |

**产出类**:
- `FacadeService`, `AlarmFacade`, `RouteFacade`, `ChartFacade`, `AISFacade`
- `ApplicationInitializer`, `ServiceRegistry`, `ConfigurationFacade`

**验证标准**:
- [ ] 编译通过，无JavaFX依赖
- [ ] 门面服务可正确调用底层业务模块
- [ ] 应用层通过门面层访问业务功能
- [ ] 单元测试覆盖率 > 80%

---

#### JAR-15: echart-app.jar（Layer 7，228h）

**优先级**: P0 | **负责人**: Dev A | **状态**: ⚠️ Partial

**目标**: 应用主程序，整合所有模块（依赖 facade + i18n + ui + ui-render + theme，通过facade访问业务服务）

| 任务ID | 任务名称 | 工时 | 依赖 | 来源 | 状态 |
|--------|----------|------|------|------|------|
| J15-01 | 响应式布局框架 | 10h | J10-05 | T19 | ✅ Done |
| J15-02 | 断点设计实现 | 6h | J15-01 | T20 | ✅ Done |
| J15-03 | 面板折叠机制 | 8h | J15-01 | T21 | ✅ Done |
| J15-04 | 主布局代码实现 | 12h | J15-01 | T25 | ✅ Done |
| J15-05 | 模块依赖关系梳理 | 4h | J14-06 | T78 | ✅ Done |
| J15-06 | 数据流设计实现 | 8h | J15-05 | T79 | ✅ Done |
| J15-07 | 集成接口定义 | 6h | J15-06 | T80 | ✅ Done |
| J15-08 | 新手引导流程设计 | 6h | J15-04 | T81 | 📋 Todo |
| J15-09 | 引导管理器实现 | 8h | J15-08 | T82 | 📋 Todo |
| J15-10 | 无障碍设计实现 | 10h | J15-04 | T83 | 📋 Todo |
| J15-11 | 键盘导航支持 | 6h | J15-10 | T84 | 📋 Todo |
| J15-12 | 屏幕阅读器支持 | 6h | J15-10 | T85 | 📋 Todo |
| J15-13 | 日志标签页实现 | 6h | J10-10 | T104 | ✅ Done |
| J15-14 | 属性标签页实现 | 6h | J10-10 | T105 | ✅ Done |
| J15-15 | 终端标签页实现 | 8h | J10-10 | T106 | ✅ Done |
| J15-16 | 属性查询功能 | 6h | J15-14 | T109 | ✅ Done |
| J15-17 | 空间查询功能 | 8h | J15-16 | T110 | ✅ Done |
| J15-18 | 焦点管理器实现 | 6h | J15-10 | T123 | 📋 Todo |
| J15-19 | 焦点遍历顺序配置 | 4h | J15-18 | T124 | 📋 Todo |
| J15-20 | 面板拖拽调整功能 | 6h | J15-03 | T131 | ✅ Done |

**产出类**:
- `Application`, `MainLayoutCode`, `BreakpointDesign`, `PanelFoldingMechanism`
- `ModuleDependencyAnalyzer`, `DataFlowDesigner`, `IntegrationInterface`
- `UserGuideManager`, `AccessibilityDesigner`, `KeyboardNavigation`
- `ScreenReaderSupport`, `FocusManager`, `FocusTraversalConfig`
- `PanelDragAdjustment`, `LogTab`, `PropertyTab`, `TerminalTab`
- `PropertyQuery`, `SpatialQuery`

**验证标准**:
- [ ] 应用启动正常
- [ ] 所有模块集成正常
- [ ] UI功能完整
- [ ] 端到端测试通过

---

## 三、测试任务清单

### 3.1 单元测试（随JAR包同步实施）

| JAR包 | 测试工时 | 负责人 | 依赖 |
|-------|---------|--------|------|
| echart-core.jar | 6h | Dev C | J01完成 |
| echart-event.jar | 4h | Dev C | J02完成 |
| echart-i18n.jar | 4h | Dev C | J03完成 |
| echart-render.jar | 8h | Dev C | J04完成 |
| echart-data.jar | 4h | Dev C | J05完成 |
| echart-alarm.jar | 8h | Dev C | J06完成 |
| echart-ais.jar | 4h | Dev C | J07完成 |
| echart-route.jar | 4h | Dev C | J08完成 |
| echart-workspace.jar | 6h | Dev C | J09完成 |
| echart-ui.jar | 6h | Dev C | J10完成 |
| echart-ui-render.jar | 6h | Dev C | J11完成 |
| echart-theme.jar | 4h | Dev C | J12完成 |
| echart-plugin.jar | 4h | Dev C | J13完成 |
| echart-facade.jar | 4h | Dev C | J14完成 |
| echart-app.jar | 6h | Dev C | J15完成 |

### 3.2 集成测试

| 测试ID | 测试名称 | 工时 | 依赖 | 状态 |
|--------|----------|------|------|------|
| IT-01 | 核心层集成测试 | 4h | J01, J02, J03 | 📋 Todo |
| IT-02 | 核心功能层集成测试 | 4h | J04, J05 | 📋 Todo |
| IT-03 | 业务功能层集成测试 | 8h | J06, J07, J08, J09 | 📋 Todo |
| IT-04 | UI层集成测试 | 6h | J10, J11, J12 | 📋 Todo |
| IT-05 | 服务门面层集成测试 | 4h | J14 | 📋 Todo |
| IT-06 | 全系统集成测试 | 8h | J15 | 📋 Todo |
| IT-07 | UI自动化测试 | 12h | IT-06 | 📋 Todo |
| IT-08 | 性能测试 | 10h | IT-06 | 📋 Todo |

---

## 四、里程碑计划

| 里程碑 | 完成内容 | 工时 | 截止周 | 验收标准 |
|--------|---------|------|--------|---------|
| **M1** | 核心层+基础服务层 | 86h | 第3周 | core/event/i18n编译通过，无JavaFX依赖 |
| **M2** | 核心功能层 | 132h | 第5周 | render/data编译通过，RenderContext接口可用 |
| **M3** | 业务功能层 | 256h | 第9周 | alarm/ais/route/workspace编译通过，无JavaFX依赖 |
| **M4** | UI层+扩展功能层 | 302h | 第12周 | ui/ui-render/theme/plugin功能完整 |
| **M5** | 服务门面层+应用层 | 244h | 第14周 | facade/app集成通过，应用可启动 |
| **M6** | 测试与验收 | 78h | 第15周 | 全部测试通过，覆盖率达标 |

---

## 五、资源分配

### 5.1 团队分工

| 开发者 | 阶段1-2 | 阶段3 | 阶段4 | 阶段5 |
|--------|---------|-------|-------|-------|
| **Dev A** | i18n(J03) | - | ui(J10), theme(J12), ui-render(J11) | app(J15) |
| **Dev B** | core(J01), event(J02) | - | plugin(J13) | facade(J14) |
| **Dev C** | 单元测试 | 单元测试 | 单元测试 | 集成测试 |
| **Dev D** | - | alarm(J06), ais(J07), route(J08), workspace(J09) | - | - |

### 5.2 关键路径

```
J01(core) → J02(event) → J06(alarm) → J10(ui) → J11(ui-render) → J15(app)
J01(core) → J02(event) → J06(alarm) → J13(plugin) → J14(facade) → J15(app)
```

**关键路径工时**: 44h + 18h + 96h + 148h + 84h + 228h = 618h

---

## 六、风险与应对

| 风险 | 影响 | 概率 | 应对措施 |
|------|------|------|---------|
| UI迁移导致功能回归 | 高 | 中 | 迁移前编写UI自动化测试，迁移后回归验证 |
| 接口设计不合理导致返工 | 高 | 低 | 接口先行，评审后再实现 |
| 业务逻辑与UI耦合过深 | 中 | 高 | 逐步解耦，先提取接口再迁移实现 |
| JNI桥接与渲染拆分困难 | 高 | 中 | RenderContext抽象层隔离平台差异 |
| 多平台复用验证不足 | 中 | 低 | 核心JAR包强制无JavaFX依赖检查 |

---

## 七、工时汇总

### 7.1 按JAR包汇总

| JAR包 | 层级 | 工时 | JavaFX | 多平台 |
|-------|------|------|--------|--------|
| echart-core.jar | Layer 0 | 44h | ❌ | ✅ |
| echart-event.jar | Layer 1 | 18h | ❌ | ✅ |
| echart-i18n.jar | Layer 1 | 24h | ❌ | ✅ |
| echart-render.jar | Layer 2 | 112h | ❌ | ✅ |
| echart-data.jar | Layer 2 | 20h | ❌ | ✅ |
| echart-alarm.jar | Layer 3 | 96h | ❌ | ✅ |
| echart-ais.jar | Layer 3 | 32h | ❌ | ✅ |
| echart-route.jar | Layer 3 | 34h | ❌ | ✅ |
| echart-workspace.jar | Layer 3 | 94h | ❌ | ✅ |
| echart-ui.jar | Layer 4 | 148h | ✅ | ❌ |
| echart-ui-render.jar | Layer 5 | 84h | ✅ | ❌ |
| echart-theme.jar | Layer 5 | 46h | ✅ | ❌ |
| echart-plugin.jar | Layer 5 | 24h | ❌ | ✅ |
| echart-facade.jar | Layer 6 | 16h | ❌ | ✅ |
| echart-app.jar | Layer 7 | 228h | ✅ | ❌ |
| **合计** | - | **1020h** | - | - |

### 7.2 按阶段汇总

| 阶段 | 工时 | 占比 |
|------|------|------|
| 阶段1：核心层+基础服务层 | 86h | 8.5% |
| 阶段2：核心功能层 | 132h | 13.0% |
| 阶段3：业务功能层 | 256h | 25.2% |
| 阶段4：UI层+扩展功能层 | 302h | 29.6% |
| 阶段5：服务门面层+应用层 | 244h | 24.1% |
| **合计** | **1020h** | **100%** |

### 7.3 多平台复用统计

| 平台 | 可复用JAR数 | 需替换JAR数 | 复用率 |
|------|------------|------------|--------|
| Android | 11个 | 4个 | 73.3% |
| Web | 11个 | 4个 | 73.3% |
| 桌面(JavaFX) | 15个 | 0个 | 100% |

**需替换的4个JAR包**: echart-ui.jar, echart-ui-render.jar, echart-theme.jar, echart-app.jar

---

## 八、v2.3架构修正说明

### 8.1 修正背景

基于用户反馈："业务功能层包含UI子包，不可避免会依赖javafx控件，进而依赖javafx，与要求不服，不利于后期web\android等端jar包的复用"

### 8.2 修正内容

| 修正项 | v1.0 | v1.2 |
|--------|------|------|
| **业务面板位置** | 业务JAR的UI子包 | **迁移至ui.jar** |
| **ui.jar工时** | 66h | **148h** |
| **业务JAR工时** | 含UI组件 | **移除UI工时** |
| **总工时** | 586h | **1020h** |

### 8.3 修正后架构优势

1. **业务JAR完全平台无关**：alarm/ais/route/data无JavaFX依赖
2. **UI层集中管理**：所有JavaFX依赖集中在ui.jar/ui-render.jar/theme.jar/app.jar
3. **依赖方向正确**：ui.jar依赖业务JAR，而非反向
4. **多平台复用**：业务逻辑层(490h)可在Android/Web端完全复用

### 8.4 v1.2修正说明

| 修正项 | v1.1 | v1.2 |
|--------|------|------|
| **遗漏任务** | T55未分配 | **添加J10-22（错误提示UI）** |
| **ui.jar工时** | 142h | **148h** |
| **总工时** | 1014h | **1020h** |

**修正原因**: 对比 `echart_display_alarm_app_layout_new_tasks.md` 发现 T55（错误提示UI）遗漏，该任务为UI组件，应分配至 ui.jar。

---

**版本**: v1.2  
**作者**: 系统架构师  
**日期**: 2026-04-19
