# 海图显示预警应用 - JAR包分配清单

> **版本**: v1.0  
> **日期**: 2026-04-17  
> **依据**: echart_display_alarm_app_layout_new_tasks.md v2.0

---

## 一、JAR包概览

### 1.1 总体架构

| 层级 | JAR包数量 | 说明 |
|------|----------|------|
| 核心层 | 1个 | 基础框架和接口定义 |
| 基础服务层 | 2个 | 事件通信、国际化 |
| 核心功能层 | 2个 | 渲染引擎、数据管理 |
| 业务功能层 | 4个 | 预警、AIS、航线、工作区 |
| 扩展功能层 | 2个 | 主题、插件 |
| 应用层 | 1个 | 主程序入口 |
| **合计** | **12个** | - |

### 1.2 JAR包列表

| 序号 | JAR包名称 | 层级 | 优先级 | 预估工时 | 实现顺序 |
|------|-----------|------|--------|----------|----------|
| 1 | echart-core.jar | 核心层 | P0 | 68h | 第1个 |
| 2 | echart-event.jar | 基础服务层 | P0 | 18h | 第2个 |
| 3 | echart-i18n.jar | 基础服务层 | P1 | 24h | 第3个 |
| 4 | echart-render.jar | 核心功能层 | P0 | 74h | 第4个 |
| 5 | echart-data.jar | 核心功能层 | P0 | 34h | 第5个 |
| 6 | echart-alarm.jar | 业务功能层 | P0 | 96h | 第6个 |
| 7 | echart-ais.jar | 业务功能层 | P0 | 38h | 第7个 |
| 8 | echart-route.jar | 业务功能层 | P0 | 56h | 第8个 |
| 9 | echart-workspace.jar | 业务功能层 | P0 | 68h | 第9个 |
| 10 | echart-theme.jar | 扩展功能层 | P1 | 20h | 第10个 |
| 11 | echart-plugin.jar | 扩展功能层 | P2 | 24h | 第11个 |
| 12 | echart-app.jar | 应用层 | P0 | 50h | 第12个 |

---

## 二、JAR包详细说明

### 2.1 核心层

#### echart-core.jar

**描述**: 核心框架和基础组件，定义系统基础接口和数据结构

**功能模块**:
- 主布局框架
- Ribbon菜单栏
- 活动栏
- 侧边栏框架
- 海图显示区框架
- 右侧面板框架
- 状态栏
- 组件层次结构
- 基础样式系统
- 响应式布局框架

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T1 | 项目结构搭建 | 4h |
| T2 | 主布局框架实现 | 8h |
| T3 | Ribbon菜单栏实现 | 12h |
| T4 | 活动栏实现 | 6h |
| T5 | 侧边栏框架实现 | 8h |
| T6 | 海图显示区框架 | 6h |
| T7 | 右侧面板框架 | 8h |
| T8 | 状态栏实现 | 4h |
| T9 | 组件层次结构实现 | 6h |
| T10 | 基础样式系统 | 6h |
| **合计** | - | **68h** |

**主要类**:
- `MainLayout` - 主布局容器
- `RibbonMenuBar` - Ribbon菜单栏
- `ActivityBar` - 活动栏
- `SideBar` - 侧边栏
- `ChartDisplayArea` - 海图显示区
- `RightPanel` - 右侧面板
- `StatusBar` - 状态栏
- `ComponentHierarchy` - 组件层次结构
- `StyleManager` - 样式管理器
- `ResponsiveLayout` - 响应式布局

**依赖**:
- JavaFX 8
- ControlsFX
- fxribbon

**被依赖**:
- echart-event.jar
- echart-render.jar
- echart-data.jar
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-workspace.jar
- echart-theme.jar
- echart-plugin.jar
- echart-app.jar

---

### 2.2 基础服务层

#### echart-event.jar

**描述**: 事件总线和通信机制，实现模块间解耦通信

**功能模块**:
- 事件总线设计
- 事件类型定义
- 通信示例实现

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T22 | 事件总线设计 | 8h |
| T23 | 事件类型定义 | 6h |
| T24 | 通信示例实现 | 4h |
| **合计** | - | **18h** |

**主要类**:
- `EventBus` - 事件总线
- `EventType` - 事件类型枚举
- `Event` - 事件基类
- `EventHandler` - 事件处理器接口
- `EventDispatcher` - 事件分发器

**依赖**:
- echart-core.jar

**被依赖**:
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-workspace.jar
- echart-app.jar

---

#### echart-i18n.jar

**描述**: 国际化支持，多语言资源管理

**功能模块**:
- 国际化架构设计
- 国际化管理器实现
- 多语言资源文件

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T70 | 国际化架构设计 | 6h |
| T71 | 国际化管理器实现 | 8h |
| T72 | 多语言资源文件 | 10h |
| **合计** | - | **24h** |

**主要类**:
- `I18nManager` - 国际化管理器
- `ResourceBundleLoader` - 资源包加载器
- `LocaleProvider` - 语言环境提供者
- `MessageFormatter` - 消息格式化器

**依赖**:
- echart-core.jar

**被依赖**:
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-app.jar

---

### 2.3 核心功能层

#### echart-render.jar

**描述**: 渲染引擎，负责海图要素渲染和性能优化

**功能模块**:
- 分层渲染架构
- 渲染数据流
- 层间通信机制
- 渲染优先级管理
- Canvas渲染集成
- JNI渲染桥接
- 海图要素渲染
- 多图层叠加渲染
- 脏区域重绘机制
- 瓦片缓存策略
- LOD细节层次策略
- 渲染性能监控
- 海图要素交互
- 高亮渲染配置
- 视图操作功能
- S-52符号库集成
- 符号样式自定义

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T11 | 分层渲染架构设计 | 8h |
| T12 | 渲染数据流实现 | 10h |
| T13 | 层间通信机制 | 6h |
| T14 | 渲染优先级管理 | 6h |
| T15 | Canvas渲染集成 | 12h |
| T16 | JNI渲染桥接 | 16h |
| T17 | 海图要素渲染 | 12h |
| T18 | 多图层叠加渲染 | 8h |
| T26 | 脏区域重绘机制 | 10h |
| T27 | 瓦片缓存策略 | 12h |
| T28 | LOD细节层次策略 | 8h |
| T29 | 渲染性能监控 | 6h |
| T97 | 测量工具基础框架 | 6h |
| T98 | 距离测量功能 | 4h |
| T99 | 面积测量功能 | 4h |
| T100 | 方位测量功能 | 4h |
| T101 | 海图要素交互实现 | 8h |
| T102 | 高亮渲染配置实现 | 6h |
| T103 | 视图操作功能实现 | 6h |
| T107 | S-52符号库集成 | 10h |
| T108 | 符号样式自定义 | 6h |
| T111 | 性能监控机制完善 | 6h |
| T112 | 内存泄漏检测集成 | 4h |
| **合计** | - | **182h** |

**主要类**:
- `RenderEngine` - 渲染引擎
- `LayerManager` - 图层管理器
- `CanvasRenderer` - Canvas渲染器
- `JNIBridge` - JNI桥接
- `ChartElementRenderer` - 海图要素渲染器
- `DirtyRegionManager` - 脏区域管理器
- `TileCache` - 瓦片缓存
- `LODStrategy` - LOD策略
- `PerformanceMonitor` - 性能监控器
- `MeasurementTool` - 测量工具
- `HighlightRenderer` - 高亮渲染器
- `ViewOperator` - 视图操作器
- `S52SymbolLibrary` - S-52符号库
- `SymbolStyleCustomizer` - 符号样式定制器

**依赖**:
- echart-core.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)

**被依赖**:
- echart-ais.jar
- echart-app.jar

---

#### echart-data.jar

**描述**: 数据管理，海图文件和图层管理

**功能模块**:
- 数据目录面板
- 海图文件管理
- 图层管理面板
- 数据导入导出

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T93 | 数据目录面板实现 | 10h |
| T94 | 海图文件管理功能 | 8h |
| T95 | 图层管理面板实现 | 8h |
| T96 | 数据导入导出功能 | 8h |
| **合计** | - | **34h** |

**主要类**:
- `DataCatalogPanel` - 数据目录面板
- `ChartFileManager` - 海图文件管理器
- `LayerManagerPanel` - 图层管理面板
- `DataImporter` - 数据导入器
- `DataExporter` - 数据导出器

**依赖**:
- echart-core.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)

**被依赖**:
- echart-app.jar

---

### 2.4 业务功能层

#### echart-alarm.jar

**描述**: 预警系统，实现各类预警功能

**功能模块**:
- 预警类型定义
- 预警面板设计
- 预警通知机制
- 预警响应流程
- 碰撞预警
- 偏航预警
- 浅水预警
- 禁航区预警
- 气象预警
- 值班报警
- 预警历史记录
- 预警统计功能
- 声音播放器
- 预警声音设置
- 预警抑制管理
- 预警持久化
- 预警规则引擎
- 预警端到端测试
- 并发预警测试
- 预警抑制测试

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T34 | 预警类型定义 | 6h |
| T35 | 预警面板设计实现 | 12h |
| T36 | 预警通知机制 | 10h |
| T37 | 预警响应流程 | 8h |
| T38 | 碰撞预警实现 | 12h |
| T39 | 偏航预警实现 | 8h |
| T40 | 浅水预警实现 | 8h |
| T41 | 禁航区预警实现 | 6h |
| T42 | 气象预警实现 | 6h |
| T43 | 值班报警实现 | 8h |
| T44 | 预警历史记录 | 6h |
| T45 | 预警统计功能 | 6h |
| T113 | 声音播放器实现 | 6h |
| T114 | 预警声音设置界面 | 4h |
| T115 | 预警抑制管理功能 | 6h |
| T116 | 预警持久化功能 | 4h |
| T126 | 预警规则引擎实现 | 6h |
| T127 | 预警端到端测试 | 6h |
| T117 | 并发预警测试 | 4h |
| T118 | 预警抑制测试 | 4h |
| T130 | 预警无障碍通知 | 4h |
| T132 | 预警通知队列管理 | 4h |
| **合计** | - | **144h** |

**主要类**:
- `AlarmManager` - 预警管理器
- `AlarmType` - 预警类型枚举
- `AlarmPanel` - 预警面板
- `AlarmNotifier` - 预警通知器
- `AlarmResponseHandler` - 预警响应处理器
- `CollisionAlarm` - 碰撞预警
- `DeviationAlarm` - 偏航预警
- `ShallowWaterAlarm` - 浅水预警
- `RestrictedAreaAlarm` - 禁航区预警
- `WeatherAlarm` - 气象预警
- `WatchAlarm` - 值班报警
- `AlarmHistory` - 预警历史
- `AlarmStatistics` - 预警统计
- `SoundPlayer` - 声音播放器
- `AlarmSoundSettings` - 预警声音设置
- `AlarmSuppressionManager` - 预警抑制管理器
- `AlarmPersistence` - 预警持久化
- `AlarmRuleEngine` - 预警规则引擎

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)

**被依赖**:
- echart-ais.jar
- echart-app.jar

---

#### echart-ais.jar

**描述**: AIS集成，AIS目标交互和CPA/TCPA计算

**功能模块**:
- AIS目标交互
- AIS预警关联机制
- 预警详情显示
- AIS目标数据模型
- CPA/TCPA计算

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T65 | AIS目标交互 | 10h |
| T66 | AIS预警关联机制 | 8h |
| T67 | 预警详情显示 | 6h |
| T68 | AIS目标数据模型 | 6h |
| T69 | CPA/TCPA计算 | 8h |
| **合计** | - | **38h** |

**主要类**:
- `AISTargetManager` - AIS目标管理器
- `AISTarget` - AIS目标数据模型
- `AISAlarmAssociation` - AIS预警关联
- `AlarmDetailDisplay` - 预警详情显示
- `CPATCPACalculator` - CPA/TCPA计算器

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-render.jar
- echart-alarm.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)

**被依赖**:
- echart-app.jar

---

#### echart-route.jar

**描述**: 航线规划，航线创建、编辑和管理

**功能模块**:
- 航线数据模型
- 航线创建功能
- 航线编辑功能
- 航点管理功能
- 航线导入导出
- 航线检查功能
- 航线面板

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T86 | 航线数据模型设计 | 6h |
| T87 | 航线创建功能实现 | 10h |
| T88 | 航线编辑功能实现 | 8h |
| T89 | 航点管理功能实现 | 8h |
| T90 | 航线导入导出功能 | 6h |
| T91 | 航线检查功能实现 | 8h |
| T92 | 航线面板实现 | 10h |
| **合计** | - | **56h** |

**主要类**:
- `RouteManager` - 航线管理器
- `Route` - 航线数据模型
- `Waypoint` - 航点数据模型
- `RouteCreator` - 航线创建器
- `RouteEditor` - 航线编辑器
- `WaypointManager` - 航点管理器
- `RouteImporter` - 航线导入器
- `RouteExporter` - 航线导出器
- `RouteChecker` - 航线检查器
- `RoutePanel` - 航线面板

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)

**被依赖**:
- echart-app.jar

---

#### echart-workspace.jar

**描述**: 工作区管理，工作区持久化和恢复

**功能模块**:
- 工作区数据结构
- 工作区管理器
- 工作区持久化
- 工作区恢复机制
- 工作区导出导入
- 错误分类与处理
- 错误码定义
- 错误边界架构
- 错误恢复机制
- 错误提示UI
- 侧边栏面板接口
- 面板管理器实现
- 右侧标签页扩展
- 状态持久化机制

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T30 | 侧边栏面板接口 | 8h |
| T31 | 面板管理器实现 | 10h |
| T32 | 右侧标签页扩展 | 6h |
| T33 | 状态持久化机制 | 8h |
| T46 | 工作区数据结构 | 6h |
| T47 | 工作区管理器实现 | 10h |
| T48 | 工作区持久化 | 8h |
| T49 | 工作区恢复机制 | 6h |
| T50 | 工作区导出导入 | 8h |
| T51 | 错误分类与处理 | 6h |
| T52 | 错误码定义 | 4h |
| T53 | 错误边界架构 | 8h |
| T54 | 错误恢复机制 | 6h |
| T55 | 错误提示UI | 6h |
| **合计** | - | **100h** |

**主要类**:
- `WorkspaceManager` - 工作区管理器
- `Workspace` - 工作区数据结构
- `WorkspacePersistence` - 工作区持久化
- `WorkspaceRecovery` - 工作区恢复
- `WorkspaceExporter` - 工作区导出器
- `WorkspaceImporter` - 工作区导入器
- `ErrorHandler` - 错误处理器
- `ErrorCode` - 错误码枚举
- `ErrorBoundary` - 错误边界
- `ErrorRecovery` - 错误恢复
- `ErrorUI` - 错误提示UI
- `PanelManager` - 面板管理器
- `TabExtension` - 标签页扩展

**依赖**:
- echart-core.jar
- echart-event.jar

**被依赖**:
- echart-app.jar

---

### 2.5 扩展功能层

#### echart-theme.jar

**描述**: 主题管理，主题切换和样式定制

**功能模块**:
- 主题管理器设计
- 主题切换实现
- 高DPI检测与缩放
- 高DPI布局调整
- 图标资源适配
- 高对比度检测实现
- 高对比度主题CSS
- 暗色主题预警颜色适配

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T73 | 主题管理器设计 | 6h |
| T74 | 主题切换实现 | 8h |
| T75 | 高DPI检测与缩放 | 8h |
| T76 | 高DPI布局调整 | 6h |
| T77 | 图标资源适配 | 6h |
| T125 | 暗色主题预警颜色适配 | 4h |
| T128 | 高对比度检测实现 | 4h |
| T129 | 高对比度主题CSS | 4h |
| **合计** | - | **46h** |

**主要类**:
- `ThemeManager` - 主题管理器
- `ThemeSwitcher` - 主题切换器
- `HighDPIScaler` - 高DPI缩放器
- `HighDPILayoutAdjuster` - 高DPI布局调整器
- `IconResourceAdapter` - 图标资源适配器
- `HighContrastDetector` - 高对比度检测器
- `DarkThemeAlarmColorAdapter` - 暗色主题预警颜色适配器

**依赖**:
- echart-core.jar
- echart-alarm.jar

**被依赖**:
- echart-app.jar

---

#### echart-plugin.jar

**描述**: 插件系统，插件加载和扩展接口

**功能模块**:
- 插件加载机制
- 插件安全机制
- 数据源扩展接口
- 预警规则扩展接口

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T119 | 插件加载机制 | 8h |
| T120 | 插件安全机制 | 6h |
| T121 | 数据源扩展接口 | 6h |
| T122 | 预警规则扩展接口 | 4h |
| **合计** | - | **24h** |

**主要类**:
- `PluginLoader` - 插件加载器
- `PluginSecurityManager` - 插件安全管理器
- `DataSourceExtension` - 数据源扩展接口
- `AlarmRuleExtension` - 预警规则扩展接口

**依赖**:
- echart-core.jar
- echart-alarm.jar

**被依赖**:
- echart-app.jar

---

### 2.6 应用层

#### echart-app.jar

**描述**: 应用主程序，整合所有模块

**功能模块**:
- 新手引导流程设计
- 引导管理器实现
- 无障碍设计实现
- 键盘导航支持
- 屏幕阅读器支持
- 焦点管理器实现
- 焦点遍历顺序配置
- 面板拖拽调整功能
- 日志标签页
- 属性标签页
- 终端标签页
- 属性查询功能
- 空间查询功能
- 模块依赖关系梳理
- 数据流设计实现
- 集成接口定义
- 主布局代码实现
- 断点设计实现
- 面板折叠机制
- 单元测试框架搭建
- 核心模块单元测试
- 集成测试框架
- 模块集成测试
- UI自动化测试框架
- UI自动化测试用例
- 性能测试场景设计
- 性能测试执行
- 测试覆盖率分析

**包含任务**:
| 任务ID | 任务名称 | 工时 |
|--------|----------|------|
| T19 | 响应式布局框架 | 10h |
| T20 | 断点设计实现 | 6h |
| T21 | 面板折叠机制 | 8h |
| T25 | 主布局代码实现 | 12h |
| T56 | 单元测试框架搭建 | 6h |
| T57 | 核心模块单元测试 | 16h |
| T58 | 集成测试框架 | 8h |
| T59 | 模块集成测试 | 12h |
| T60 | UI自动化测试框架 | 10h |
| T61 | UI自动化测试用例 | 12h |
| T62 | 性能测试场景设计 | 8h |
| T63 | 性能测试执行 | 10h |
| T64 | 测试覆盖率分析 | 6h |
| T78 | 模块依赖关系梳理 | 4h |
| T79 | 数据流设计实现 | 8h |
| T80 | 集成接口定义 | 6h |
| T81 | 新手引导流程设计 | 6h |
| T82 | 引导管理器实现 | 8h |
| T83 | 无障碍设计实现 | 10h |
| T84 | 键盘导航支持 | 6h |
| T85 | 屏幕阅读器支持 | 6h |
| T104 | 日志标签页实现 | 6h |
| T105 | 属性标签页实现 | 6h |
| T106 | 终端标签页实现 | 8h |
| T109 | 属性查询功能 | 6h |
| T110 | 空间查询功能 | 8h |
| T123 | 焦点管理器实现 | 6h |
| T124 | 焦点遍历顺序配置 | 4h |
| T131 | 面板拖拽调整功能 | 6h |
| **合计** | - | **224h** |

**主要类**:
- `Application` - 应用主类
- `MainLayoutCode` - 主布局代码
- `BreakpointDesign` - 断点设计
- `PanelFoldingMechanism` - 面板折叠机制
- `ModuleDependencyAnalyzer` - 模块依赖关系分析器
- `DataFlowDesigner` - 数据流设计器
- `IntegrationInterface` - 集成接口
- `UserGuideManager` - 用户引导管理器
- `AccessibilityDesigner` - 无障碍设计器
- `KeyboardNavigation` - 键盘导航
- `ScreenReaderSupport` - 屏幕阅读器支持
- `FocusManager` - 焦点管理器
- `FocusTraversalConfig` - 焦点遍历配置
- `PanelDragAdjustment` - 面板拖拽调整
- `LogTab` - 日志标签页
- `PropertyTab` - 属性标签页
- `TerminalTab` - 终端标签页
- `PropertyQuery` - 属性查询
- `SpatialQuery` - 空间查询

**依赖**:
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- echart-render.jar
- echart-data.jar
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-workspace.jar
- echart-theme.jar
- echart-plugin.jar

**被依赖**:
- 无

---

## 三、依赖关系图

### 3.1 层级依赖关系

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Layer 5)                        │
│                     echart-app.jar                          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    扩展功能层 (Layer 4)                      │
│         echart-theme.jar    echart-plugin.jar               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    业务功能层 (Layer 3)                      │
│  echart-alarm.jar  echart-ais.jar  echart-route.jar         │
│              echart-workspace.jar                           │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    核心功能层 (Layer 2)                      │
│           echart-render.jar    echart-data.jar              │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   基础服务层 (Layer 1)                       │
│           echart-event.jar    echart-i18n.jar               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      核心层 (Layer 0)                        │
│                     echart-core.jar                         │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 JAR包依赖矩阵

| JAR包 | core | event | i18n | render | data | alarm | ais | route | workspace | theme | plugin | app | JNI | javawrapper |
|-------|------|-------|------|--------|------|-------|-----|-------|-----------|-------|--------|-----|-----|-------------|
| echart-core.jar | - | - | - | - | - | - | - | - | - | - | - | - | ❌ | ❌ |
| echart-event.jar | ✅ | - | - | - | - | - | - | - | - | - | - | - | ❌ | ❌ |
| echart-i18n.jar | ✅ | - | - | - | - | - | - | - | - | - | - | - | ❌ | ❌ |
| echart-render.jar | ✅ | - | - | - | - | - | - | - | - | - | - | - | ✅ | ✅ |
| echart-data.jar | ✅ | - | - | - | - | - | - | - | - | - | - | - | ✅ | ✅ |
| echart-alarm.jar | ✅ | ✅ | ✅ | - | - | - | - | - | - | - | - | - | ✅ | ✅ |
| echart-ais.jar | ✅ | ✅ | - | ✅ | - | ✅ | - | - | - | - | - | - | ✅ | ✅ |
| echart-route.jar | ✅ | ✅ | ✅ | - | - | - | - | - | - | - | - | - | ✅ | ✅ |
| echart-workspace.jar | ✅ | ✅ | - | - | - | - | - | - | - | - | - | - | ❌ | ❌ |
| echart-theme.jar | ✅ | - | - | - | - | ✅ | - | - | - | - | - | - | ❌ | ❌ |
| echart-plugin.jar | ✅ | - | - | - | - | ✅ | - | - | - | - | - | - | ❌ | ❌ |
| echart-app.jar | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | - | ✅ | ✅ |

### 3.3 详细依赖关系

#### echart-core.jar
**依赖**: 无
**被依赖**: 所有其他JAR包

#### echart-event.jar
**依赖**: 
- echart-core.jar
**被依赖**: 
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-workspace.jar
- echart-app.jar

#### echart-i18n.jar
**依赖**: 
- echart-core.jar
**被依赖**: 
- echart-alarm.jar
- echart-route.jar
- echart-app.jar

#### echart-render.jar
**依赖**: 
- echart-core.jar
- JNI Bridge (C++)
**被依赖**: 
- echart-ais.jar
- echart-app.jar

#### echart-data.jar
**依赖**: 
- echart-core.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)
**被依赖**: 
- echart-app.jar

#### echart-alarm.jar
**依赖**: 
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)
**被依赖**: 
- echart-ais.jar
- echart-theme.jar
- echart-plugin.jar
- echart-app.jar

#### echart-ais.jar
**依赖**: 
- echart-core.jar
- echart-event.jar
- echart-render.jar
- echart-alarm.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)
**被依赖**: 
- echart-app.jar

#### echart-route.jar
**依赖**: 
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- JNI Bridge (C++)
- javawrapper (基于sdk_c_api.h封装)
**被依赖**: 
- echart-app.jar

#### echart-workspace.jar
**依赖**: 
- echart-core.jar
- echart-event.jar
**被依赖**: 
- echart-app.jar

#### echart-theme.jar
**依赖**: 
- echart-core.jar
- echart-alarm.jar
**被依赖**: 
- echart-app.jar

#### echart-plugin.jar
**依赖**: 
- echart-core.jar
- echart-alarm.jar
**被依赖**: 
- echart-app.jar

#### echart-app.jar
**依赖**: 
- echart-core.jar
- echart-event.jar
- echart-i18n.jar
- echart-render.jar
- echart-data.jar
- echart-alarm.jar
- echart-ais.jar
- echart-route.jar
- echart-workspace.jar
- echart-theme.jar
- echart-plugin.jar
**被依赖**: 无

---

## 四、实现顺序

### 4.1 实现阶段

| 阶段 | JAR包 | 时间周期 | 说明 |
|------|-------|----------|------|
| **阶段1** | echart-core.jar | Week 1-2 | 核心框架，所有模块的基础 |
| **阶段2** | echart-event.jar, echart-i18n.jar | Week 3-4 | 基础服务，模块通信支持 |
| **阶段3** | echart-render.jar, echart-data.jar | Week 5-8 | 核心功能，渲染和数据管理 |
| **阶段4** | echart-alarm.jar, echart-ais.jar, echart-route.jar, echart-workspace.jar | Week 9-16 | 业务功能，核心业务实现 |
| **阶段5** | echart-theme.jar, echart-plugin.jar | Week 17-20 | 扩展功能，主题和插件 |
| **阶段6** | echart-app.jar | Week 21-23 | 应用整合，测试和完善 |

### 4.2 关键路径实现顺序

```
echart-core.jar (T1-T10)
    │
    ├─► echart-render.jar (T11-T18)
    │       │
    │       └─► echart-ais.jar (T65-T69)
    │               │
    │               └─► echart-app.jar
    │
    └─► echart-event.jar (T22-T24)
            │
            └─► echart-alarm.jar (T34-T45)
                    │
                    └─► echart-ais.jar (T66)
```

### 4.3 并行实现建议

| 时间段 | 可并行实现的JAR包 | 负责人建议 |
|--------|-------------------|------------|
| Week 1-2 | echart-core.jar | Dev A |
| Week 3-4 | echart-event.jar, echart-i18n.jar | Dev B, Dev A |
| Week 5-8 | echart-render.jar, echart-data.jar | Dev B, Dev A |
| Week 9-10 | echart-alarm.jar | Dev D |
| Week 11-12 | echart-workspace.jar | Dev D |
| Week 13-14 | 测试框架 (echart-app.jar部分) | Dev C |
| Week 15-16 | echart-ais.jar, echart-route.jar | Dev D, Dev B |
| Week 17-18 | echart-theme.jar | Dev A |
| Week 19-20 | echart-plugin.jar | Dev B |
| Week 21-23 | echart-app.jar | 全员 |

---

## 五、构建配置

### 5.1 Gradle配置示例

```groovy
// settings.gradle
rootProject.name = 'echart-display-alarm-app'
include 'echart-core'
include 'echart-event'
include 'echart-i18n'
include 'echart-render'
include 'echart-data'
include 'echart-alarm'
include 'echart-ais'
include 'echart-route'
include 'echart-workspace'
include 'echart-theme'
include 'echart-plugin'
include 'echart-app'
```

### 5.2 模块依赖配置

```groovy
// echart-event/build.gradle
dependencies {
    compile project(':echart-core')
}

// echart-i18n/build.gradle
dependencies {
    compile project(':echart-core')
}

// echart-render/build.gradle
dependencies {
    compile project(':echart-core')
    // JNI依赖
    compile project(':javawrapper')
    // JNI本地库
    runtime files('../jni/build/libs/ogc_chart_jni.dll')
}

// echart-data/build.gradle
dependencies {
    compile project(':echart-core')
    // JNI依赖
    compile project(':javawrapper')
    // JNI本地库
    runtime files('../jni/build/libs/ogc_chart_jni.dll')
}

// echart-alarm/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-event')
    compile project(':echart-i18n')
    // JNI依赖
    compile project(':javawrapper')
    // JNI本地库
    runtime files('../jni/build/libs/ogc_chart_jni.dll')
}

// echart-ais/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-event')
    compile project(':echart-render')
    compile project(':echart-alarm')
    // JNI依赖（显式声明，虽然可通过render传递）
    compile project(':javawrapper')
    // JNI本地库
    runtime files('../jni/build/libs/ogc_chart_jni.dll')
}

// echart-route/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-event')
    compile project(':echart-i18n')
    // JNI依赖
    compile project(':javawrapper')
    // JNI本地库
    runtime files('../jni/build/libs/ogc_chart_jni.dll')
}

// echart-workspace/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-event')
}

// echart-theme/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-alarm')
}

// echart-plugin/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-alarm')
}

// echart-app/build.gradle
dependencies {
    compile project(':echart-core')
    compile project(':echart-event')
    compile project(':echart-i18n')
    compile project(':echart-render')
    compile project(':echart-data')
    compile project(':echart-alarm')
    compile project(':echart-ais')
    compile project(':echart-route')
    compile project(':echart-workspace')
    compile project(':echart-theme')
    compile project(':echart-plugin')
    // JNI依赖（通过子模块传递，显式声明确保可用）
    compile project(':javawrapper')
}
```

### 5.3 JNI本地库配置

```groovy
// 在根项目的build.gradle中配置JNI本地库路径
allprojects {
    tasks.withType(JavaExec) {
        // 设置JNI本地库路径
        systemProperty 'java.library.path', '../jni/build/libs'
    }
}

// 或者在运行时指定
// java -Djava.library.path=../jni/build/libs -jar echart-app.jar
```

---

## 六、总结

### 6.1 JAR包统计

| 统计项 | 数量 |
|--------|------|
| JAR包总数 | 12个 |
| 核心层JAR包 | 1个 |
| 基础服务层JAR包 | 2个 |
| 核心功能层JAR包 | 2个 |
| 业务功能层JAR包 | 4个 |
| 扩展功能层JAR包 | 2个 |
| 应用层JAR包 | 1个 |

### 6.2 工时统计

| 层级 | 工时 | 占比 |
|------|------|------|
| 核心层 | 68h | 7.0% |
| 基础服务层 | 42h | 4.3% |
| 核心功能层 | 216h | 22.4% |
| 业务功能层 | 338h | 35.0% |
| 扩展功能层 | 70h | 7.2% |
| 应用层 | 224h | 23.2% |
| **合计** | **958h** | **100%** |

### 6.3 关键要点

1. **echart-core.jar** 是所有模块的基础，必须最先实现
2. **echart-render.jar** 是渲染核心，包含JNI集成，是关键路径
3. **echart-alarm.jar** 是预警系统核心，被多个模块依赖，需要JNI支持
4. **echart-ais.jar** 依赖渲染和预警，实现AIS集成，需要JNI支持
5. **echart-data.jar** 数据管理模块，需要JNI支持调用C API
6. **echart-route.jar** 航线规划模块，需要JNI支持调用C API
7. **echart-app.jar** 是最终整合模块，依赖所有其他模块

### 6.4 JNI依赖说明

| JAR包 | JNI依赖 | 调用的C API模块 |
|-------|---------|-----------------|
| echart-render.jar | ✅ | ogc_draw, ogc_graph, ogc_cache, ogc_symbology |
| echart-data.jar | ✅ | ogc_layer, ogc_feature, ogc_geom |
| echart-alarm.jar | ✅ | ogc_alert (预警引擎、CPA计算) |
| echart-ais.jar | ✅ | ogc_navi (AIS目标管理、CPA计算) |
| echart-route.jar | ✅ | ogc_navi (航线管理、航点管理) |

> **注意**: JNI依赖通过 `javawrapper` 模块封装，基于 `sdk_c_api.h` 提供的C API接口。

---

**文档结束**
