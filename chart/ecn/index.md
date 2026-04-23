# ECN 模块索引

> **版本**: v1.0  
> **日期**: 2026-04-20  
> **用途**: 辅助大模型快速定位查找各模块及模块中方法

---

## ⚠️ 编译前置约束

> **重要**: 在编译各模块之前，必须先了解以下环境配置和依赖关系。

### 环境要求

| 环境 | 要求 | 说明 |
|------|------|------|
| JDK | 1.8+ | 推荐使用 `F:\enc\java` |
| JAVA_HOME | 必须设置 | 指向 JDK 1.8 安装目录 |
| Gradle | 4.5.1 | 位于 `gradle/gradle-4.5.1/` |

### Gradle 配置

**配置文件**: `ecn/config/common.gradle`

| 配置项 | 值 | 说明 |
|--------|-----|------|
| javaVersion | 1.8 | Java 版本 |
| encoding | UTF-8 | 编码格式 |
| junitVersion | 4.12 | JUnit 版本 |
| projectVersion | 1.2.1-alpha | 项目版本 |
| projectGroup | cn.cycle.echart | 项目组 |

### 编译与安装目录

> **基准目录**: 所有路径均相对于**项目根目录**（即 `e:\program\trae\chart/`）

| 目录类型 | 路径 | 说明 |
|----------|------|------|
| 编译根目录 | `build/ecn/` | Gradle 构建输出目录 |
| 各模块编译目录 | `build/ecn/[模块名]/` | 各模块独立编译目录 |
| 安装根目录 | `install/bin/` | JAR 安装目录 |
| 配置文件 | `ecn/config/` | Gradle 公共配置 |
| 安全策略文件 | `ecn/config/java.policy` | Java 安全策略 |

### 各模块编译产物

| 模块 | 编译产物 | 输出目录 |
|------|----------|----------|
| fxribbon | fxribbon-2.1.0.jar | build/ecn/fxribbon/libs/ |
| echart-core | echart-core-1.2.1-alpha.jar | build/ecn/echart-core/libs/ |
| echart-i18n | echart-i18n-1.2.1-alpha.jar | build/ecn/echart-i18n/libs/ |
| echart-render | echart-render-1.2.1-alpha.jar | build/ecn/echart-render/libs/ |
| echart-data | echart-data-1.2.1-alpha.jar | build/ecn/echart-data/libs/ |
| echart-alarm | echart-alarm-1.2.1-alpha.jar | build/ecn/echart-alarm/libs/ |
| echart-ais | echart-ais-1.2.1-alpha.jar | build/ecn/echart-ais/libs/ |
| echart-route | echart-route-1.2.1-alpha.jar | build/ecn/echart-route/libs/ |
| echart-workspace | echart-workspace-1.2.1-alpha.jar | build/ecn/echart-workspace/libs/ |
| echart-ui | echart-ui-1.2.1-alpha.jar | build/ecn/echart-ui/libs/ |
| echart-ui-render | echart-ui-render-1.2.1-alpha.jar | build/ecn/echart-ui-render/libs/ |
| echart-theme | echart-theme-1.2.1-alpha.jar | build/ecn/echart-theme/libs/ |
| echart-plugin | echart-plugin-1.2.1-alpha.jar | build/ecn/echart-plugin/libs/ |
| echart-facade | echart-facade-1.2.1-alpha.jar | build/ecn/echart-facade/libs/ |
| echart-app | echart-app-1.2.1-alpha.jar | build/ecn/echart-app/libs/ |

### 编译命令

```powershell
# 设置 JAVA_HOME
$env:JAVA_HOME='F:\enc\java'

# 编译单个模块（跳过测试）
gradle/gradle-4.5.1/bin/gradle.bat -p ecn/[模块名] build -x test

# 示例：编译 echart-core
gradle/gradle-4.5.1/bin/gradle.bat -p ecn/echart-core build -x test
```

### 模块依赖层级

```
Ribbon Layer: fxribbon (JavaFX Ribbon控件库，无依赖)
    ↓
Layer 0: echart-core (无依赖)
    ↓
Layer 1: echart-i18n (无依赖)
    ↓
Layer 2: echart-render (依赖 echart-core)
         echart-data (无依赖)
    ↓
Layer 3: echart-alarm (依赖 echart-core)
         echart-ais (依赖 echart-core)
         echart-route (无依赖)
         echart-workspace (依赖 echart-route, echart-ais)
    ↓
Layer 4: echart-ui (依赖 echart-core, echart-data, echart-alarm)
    ↓
Layer 5: echart-ui-render (依赖 echart-render)
         echart-theme (无依赖)
         echart-plugin (依赖 echart-core)
    ↓
Layer 6: echart-facade (依赖 echart-core, echart-data, echart-alarm, echart-ais, echart-route, echart-workspace)
    ↓
Layer 7: echart-app (依赖所有模块)
```

### 编译顺序

按依赖层级从低到高编译：

| 顺序 | 模块 | Layer |
|------|------|-------|
| 1 | fxribbon | Ribbon |
| 2 | echart-core | 0 |
| 3 | echart-i18n | 1 |
| 4 | echart-render, echart-data | 2 |
| 5 | echart-alarm, echart-ais, echart-route, echart-workspace | 3 |
| 6 | echart-ui | 4 |
| 7 | echart-ui-render, echart-theme, echart-plugin | 5 |
| 8 | echart-facade | 6 |
| 9 | echart-app | 7 |

---

## 模块总览

| 序号 | 模块名 | Layer | 包路径 | 依赖 | 说明 |
|------|--------|-------|--------|------|------|
| 1 | fxribbon | Ribbon | com.cycle.control | JavaFX | JavaFX Ribbon控件库 |
| 2 | echart-core | 0 | cn.cycle.echart.core | 无 | 核心框架，平台无关 |
| 3 | echart-i18n | 1 | cn.cycle.echart.i18n | 无 | 国际化支持 |
| 4 | echart-render | 2 | cn.cycle.echart.render | echart-core | 渲染引擎核心 |
| 5 | echart-data | 2 | cn.cycle.echart.data | 无 | 数据管理核心 |
| 6 | echart-alarm | 3 | cn.cycle.echart.alarm | echart-core | 预警系统核心 |
| 7 | echart-ais | 3 | cn.cycle.echart.ais | echart-core | AIS集成核心 |
| 8 | echart-route | 3 | cn.cycle.echart.route | 无 | 航线规划核心 |
| 9 | echart-workspace | 3 | cn.cycle.echart.workspace | echart-route, echart-ais | 工作区管理 |
| 10 | echart-ui | 4 | cn.cycle.echart.ui | echart-core, echart-data, echart-alarm, fxribbon | JavaFX UI控件 |
| 11 | echart-ui-render | 5 | cn.cycle.echart.ui.render | echart-render | JavaFX Canvas渲染 |
| 12 | echart-theme | 5 | cn.cycle.echart.theme | 无 | 主题管理 |
| 13 | echart-plugin | 5 | cn.cycle.echart.plugin | echart-core | 插件系统 |
| 14 | echart-facade | 6 | cn.cycle.echart.facade | echart-core, echart-data, echart-alarm, echart-ais, echart-route, echart-workspace | 服务门面层 |
| 15 | echart-app | 7 | cn.cycle.echart.app | 所有模块 | 应用入口 |

---

## 1. fxribbon (Ribbon Layer)
**路径**: `ecn/fxribbon/src/main/java/com/cycle/`
### 类列表
| 类名 | 类型 | 说明 |
|------|------|------|
| Ribbon | 类 | Ribbon控件主容器 |
| RibbonTab | 类 | Ribbon标签页 |
| RibbonGroup | 类 | Ribbon功能组 |
| RibbonMenuBar | 类 | Ribbon菜单栏 |
### 核心方法
#### Ribbon
- `ObservableList<RibbonTab> getTabs()` - 获取标签页列表
- `RibbonTab getSelectedRibbonTab()` - 获取选中标签页
- `void setSelectedRibbonTab(RibbonTab)` - 设置选中标签页
- `Node getQuickAccessBar()` - 获取快速访问栏
- `Node getWindowControls()` - 获取窗口控件
#### RibbonTab
- `ObservableList<RibbonGroup> getRibbonGroups()` - 获取功能组列表
- `void setContextualColor(String)` - 设置上下文颜色
- `String getContextualColor()` - 获取上下文颜色
#### RibbonGroup
- `String getTitle()` - 获取标题
- `void setTitle(String)` - 设置标题
- `ObservableList<Node> getChildren()` - 获取子节点
---
## 2. echart-core (Layer 0)

**路径**: `ecn/echart-core/src/main/java/cn/cycle/echart/core/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| LifecycleComponent | 接口 | 生命周期组件接口 |
| AbstractLifecycleComponent | 抽象类 | 生命周期组件基类 |
| EventBus | 接口 | 事件总线接口 |
| DefaultEventBus | 类 | 默认事件总线实现 |
| AppEvent | 类 | 应用事件 |
| AppEventType | 枚举 | 应用事件类型 |
| EventHandler | 接口 | 事件处理器 |
| PlatformAdapter | 接口 | 平台适配器接口 |
| ServiceLocator | 类 | 服务定位器 |
| PanelDescriptor | 接口 | 面板描述符接口 |
| TabDescriptor | 接口 | 标签页描述符接口 |
| ExtensionDescriptor | 接口 | 扩展描述符接口 |
| ErrorHandler | 接口 | 错误处理器接口 |
| DefaultErrorHandler | 类 | 默认错误处理器 |
| ErrorCode | 枚举 | 错误码 |
| LRUCache<K,V> | 类 | LRU缓存 |

### 核心方法

#### AbstractLifecycleComponent
- `String getName()` - 获取组件名称
- `State getState()` - 获取组件状态
- `void initialize()` - 初始化组件
- `void start()` - 启动组件
- `void stop()` - 停止组件
- `void dispose()` - 销毁组件
- `boolean isInitialized()` - 是否已初始化
- `boolean isRunning()` - 是否运行中
- `boolean isDisposed()` - 是否已销毁

#### DefaultEventBus
- `void publish(AppEvent event)` - 发布事件
- `void unsubscribe(AppEventType, EventHandler<?>)` - 取消订阅
- `void subscribeAll(EventHandler<Object>)` - 订阅所有事件
- `void unsubscribeAll(EventHandler<Object>)` - 取消所有订阅
- `void clear()` - 清除所有订阅
- `int getSubscriberCount(AppEventType)` - 获取订阅者数量
- `int getTotalSubscriberCount()` - 获取总订阅者数量
- `void shutdown()` - 关闭事件总线
- `boolean isAsyncMode()` - 是否异步模式

#### AppEvent
- `AppEventType getType()` - 获取事件类型
- `Object getData()` - 获取事件数据
- `long getTimestamp()` - 获取时间戳
- `String getSourceComponent()` - 获取来源组件

#### AppEventType
- `String getCode()` - 获取事件代码
- `String getDescription()` - 获取事件描述
- `static AppEventType fromCode(String)` - 根据代码获取类型

#### ServiceLocator
- `static boolean isRegistered(Class<?>)` - 服务是否已注册
- `static void unregister(Class<?>)` - 取消注册服务
- `static void clear()` - 清除所有注册

#### DefaultErrorHandler
- `void handleError(ErrorCode, String, Throwable)` - 处理错误
- `void handleError(ErrorCode, String)` - 处理错误
- `void handleWarning(ErrorCode, String)` - 处理警告
- `void handleInfo(ErrorCode, String)` - 处理信息
- `boolean shouldContinue(ErrorCode)` - 是否继续
- `String getLastErrorMessage()` - 获取最后错误消息
- `ErrorCode getLastErrorCode()` - 获取最后错误码
- `void clearLastError()` - 清除最后错误

#### ErrorCode
- `int getCode()` - 获取错误码
- `String getMessage()` - 获取错误消息
- `static ErrorCode fromCode(int)` - 根据代码获取枚举

#### LRUCache<K,V>
- `synchronized void put(K, V)` - 放入缓存
- `synchronized V get(K)` - 获取缓存
- `synchronized boolean containsKey(K)` - 是否包含键
- `synchronized V remove(K)` - 移除缓存
- `synchronized void clear()` - 清除缓存
- `synchronized int size()` - 缓存大小
- `int getMaxSize()` - 最大容量
- `synchronized boolean isEmpty()` - 是否为空
- `long getHitCount()` - 命中次数
- `long getMissCount()` - 未命中次数
- `synchronized double getHitRate()` - 命中率

---

## 3. echart-i18n (Layer 1)

**路径**: `ecn/echart-i18n/src/main/java/cn/cycle/echart/i18n/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| I18nManager | 类 | 国际化管理器(单例) |
| ResourceBundleLoader | 类 | 资源包加载器 |
| LocaleProvider | 接口 | 区域设置提供者 |
| MessageFormatter | 类 | 消息格式化器 |

### 核心方法

#### I18nManager
- `static I18nManager getInstance()` - 获取单例
- `String getMessage(String key)` - 获取消息
- `String getMessage(String key, Object... args)` - 获取格式化消息
- `boolean hasMessage(String key)` - 消息是否存在
- `Locale getLocale()` - 获取当前区域
- `void setLocale(Locale)` - 设置区域
- `Locale getDefaultLocale()` - 获取默认区域
- `void addLocaleChangeListener(LocaleChangeListener)` - 添加区域变更监听
- `void removeLocaleChangeListener(LocaleChangeListener)` - 移除区域变更监听
- `void clearCache()` - 清除缓存
- `void reset()` - 重置

#### ResourceBundleLoader
- `ResourceBundle loadBundle(Locale)` - 加载资源包
- `String getBaseName()` - 获取基础名称
- `boolean bundleExists(Locale)` - 资源包是否存在

#### MessageFormatter
- `String format(String pattern, Object...)` - 格式化消息
- `static String format(String, Locale, Object...)` - 静态格式化
- `Locale getLocale()` - 获取区域

---

## 4. echart-render (Layer 2)

**路径**: `ecn/echart-render/src/main/java/cn/cycle/echart/render/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| RenderContext | 接口 | 渲染上下文接口 |
| RenderEngine | 类 | 渲染引擎 |
| Layer | 类 | 渲染图层 |
| LayerManager | 类 | 图层管理器 |
| DirtyRegionManager | 类 | 脏区域管理器 |
| LODStrategy | 类 | LOD细节层次策略 |
| PerformanceMonitor | 类 | 性能监控器 |

### 核心方法

#### RenderEngine
- `LayerManager getLayerManager()` - 获取图层管理器
- `DirtyRegionManager getDirtyRegionManager()` - 获取脏区域管理器
- `PerformanceMonitor getPerformanceMonitor()` - 获取性能监控器
- `LODStrategy getLODStrategy()` - 获取LOD策略
- `void setContext(RenderContext)` - 设置渲染上下文
- `RenderContext getContext()` - 获取渲染上下文
- `void render()` - 执行渲染
- `void requestRepaint()` - 请求重绘
- `void markDirty(int, int, int, int)` - 标记脏区域
- `boolean isRendering()` - 是否正在渲染
- `void dispose()` - 销毁

#### Layer
- `String getName()` - 获取图层名称
- `boolean isVisible()` - 是否可见
- `void setVisible(boolean)` - 设置可见性
- `double getOpacity()` - 获取透明度
- `void setOpacity(double)` - 设置透明度
- `int getPriority()` - 获取优先级
- `void setPriority(int)` - 设置优先级
- `Rectangle2D getBounds()` - 获取边界
- `boolean needsRender(RenderContext)` - 是否需要渲染

#### LayerManager
- `void addLayer(Layer)` - 添加图层
- `void addLayer(Layer, int)` - 在指定位置添加图层
- `boolean removeLayer(Layer)` - 移除图层
- `List<Layer> getLayers()` - 获取所有图层
- `List<Layer> getVisibleLayers()` - 获取可见图层
- `int getLayerCount()` - 图层数量
- `Layer getLayer(int)` - 按索引获取图层
- `Layer getLayerByName(String)` - 按名称获取图层
- `void moveLayer(Layer, int)` - 移动图层
- `void clear()` - 清除所有图层

#### DirtyRegionManager
- `void addRegion(int, int, int, int)` - 添加脏区域
- `void addRegion(Rectangle2D)` - 添加脏区域
- `List<Rectangle2D> getRegions()` - 获取所有脏区域
- `Rectangle2D getMergedRegion()` - 获取合并区域
- `boolean hasDirtyRegions()` - 是否有脏区域
- `boolean intersects(Rectangle2D)` - 是否相交
- `int getRegionCount()` - 脏区域数量
- `void clear()` - 清除脏区域

#### LODStrategy
- `int calculateLOD(double, Layer)` - 计算LOD级别
- `boolean shouldRender(int, int)` - 是否应渲染
- `int getMaxLOD()` - 最大LOD
- `int getMinLOD()` - 最小LOD

#### PerformanceMonitor
- `void beginLayerRender(String)` - 开始图层渲染计时
- `void endLayerRender(String)` - 结束图层渲染计时
- `void recordFrameTime(long)` - 记录帧时间
- `double getLayerRenderTime(String)` - 获取图层渲染时间
- `long getLastFrameTime()` - 获取最后帧时间
- `double getAverageFrameTime()` - 平均帧时间
- `double getAverageFPS()` - 平均FPS
- `long getFrameCount()` - 帧计数
- `Map<String, LayerStats> getAllLayerStats()` - 所有图层统计
- `void reset()` - 重置

---

## 5. echart-data (Layer 2)

**路径**: `ecn/echart-data/src/main/java/cn/cycle/echart/data/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| ChartFile | 类 | 海图文件 |
| ChartFileManager | 类 | 海图文件管理器 |
| FeatureData | 类 | 要素数据 |
| LayerData | 类 | 图层数据 |
| DataImporter | 接口 | 数据导入接口 |
| DataExporter | 接口 | 数据导出接口 |
| DataImportException | 类 | 数据导入异常 |
| DataExportException | 类 | 数据导出异常 |

### 核心方法

#### ChartFileManager
- `ChartFile loadChart(String)` - 加载海图
- `boolean unloadChart(String)` - 卸载海图
- `ChartFile getChart(String)` - 获取海图
- `List<ChartFile> getLoadedCharts()` - 获取已加载海图
- `int getChartCount()` - 海图数量
- `boolean isChartLoaded(String)` - 海图是否已加载
- `List<ChartFile> findChartsByName(String)` - 按名称查找
- `List<ChartFile> findChartsByBounds(Rectangle2D)` - 按范围查找
- `void unloadAll()` - 卸载所有海图
- `void addChartFileListener(ChartFileListener)` - 添加监听器
- `void removeChartFileListener(ChartFileListener)` - 移除监听器

#### ChartFile
- `String getId()` - 获取ID
- `String getName()` - 获取名称
- `String getPath()` - 获取路径
- `Rectangle2D getBounds()` - 获取边界
- `String getFormat()` - 获取格式
- `long getFileSize()` - 获取文件大小
- `long getLastModified()` - 获取最后修改时间
- `boolean isLoaded()` - 是否已加载

#### FeatureData
- `String getId()` - 获取ID
- `String getGeometryType()` - 获取几何类型
- `Map<String, Object> getAttributes()` - 获取属性
- `void setAttribute(String, Object)` - 设置属性
- `Object getAttribute(String)` - 获取属性
- `String getAttributeAsString(String)` - 获取字符串属性
- `Integer getAttributeAsInt(String)` - 获取整型属性
- `Double getAttributeAsDouble(String)` - 获取双精度属性
- `Rectangle2D getBounds()` - 获取边界

#### LayerData
- `String getId()` - 获取ID
- `String getName()` - 获取名称
- `String getType()` - 获取类型
- `List<FeatureData> getFeatures()` - 获取要素列表
- `int getFeatureCount()` - 要素数量
- `void addFeature(FeatureData)` - 添加要素
- `void removeFeature(FeatureData)` - 移除要素
- `void clearFeatures()` - 清除要素
- `boolean isVisible()` - 是否可见
- `void setVisible(boolean)` - 设置可见性

---

## 6. echart-alarm (Layer 3)

**路径**: `ecn/echart-alarm/src/main/java/cn/cycle/echart/alarm/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| AlarmManager | 类 | 预警管理器 |
| CpaResult | 类 | CPA计算结果 |
| UkcResult | 类 | UKC计算结果 |

### 核心方法

#### AlarmManager
- `Alert triggerAlert(Type, Severity, String, String)` - 触发预警
- `Alert triggerSafetyAlert(Severity, String, String)` - 触发安全预警
- `Alert triggerNavigationAlert(Severity, String, String)` - 触发导航预警
- `Alert triggerSystemAlert(Severity, String, String)` - 触发系统预警
- `Alert triggerWeatherAlert(Severity, String, String)` - 触发气象预警
- `Alert triggerAisAlert(Severity, String, String)` - 触发AIS预警
- `boolean acknowledgeAlert(String)` - 确认预警
- `boolean clearAlert(String)` - 清除预警
- `Alert getAlert(String)` - 获取预警
- `List<Alert> getActiveAlerts()` - 获取活动预警
- `List<Alert> getUnacknowledgedAlerts()` - 获取未确认预警
- `int getAlertCount()` - 预警数量
- `void clearAllAlerts()` - 清除所有预警
- `void addAlarmListener(AlarmListener)` - 添加监听器
- `void removeAlarmListener(AlarmListener)` - 移除监听器

#### CpaResult
- `double getCpa()` - 获取CPA
- `double getTcpa()` - 获取TCPA
- `double getDistance()` - 获取距离
- `double getBearing()` - 获取方位
- `boolean isDangerous()` - 是否危险

#### UkcResult
- `double getUkc()` - 获取UKC
- `double getSquat()` - 获取蹲伏量
- `double getChartedDepth()` - 获取图载水深
- `double getTideHeight()` - 获取潮高
- `double getDraft()` - 获取吃水
- `double getSafetyMargin()` - 获取安全余量
- `boolean isSafe()` - 是否安全

---

## 7. echart-ais (Layer 3)

**路径**: `ecn/echart-ais/src/main/java/cn/cycle/echart/ais/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| AISTarget | 类 | AIS目标数据模型 |
| AISTargetManager | 类 | AIS目标管理器 |
| AISAlarmAssociation | 类 | AIS预警关联 |
| CPATCPACalculator | 类 | CPA/TCPA计算器 |

### 核心方法

#### AISTargetManager
- `AISTarget getTarget(int)` - 获取AIS目标
- `void addTarget(AISTarget)` - 添加AIS目标
- `List<Integer> getAllTargetMmsi()` - 获取所有MMSI
- `List<AISTarget> getAllTargets()` - 获取所有目标
- `int getTargetCount()` - 目标数量
- `boolean removeTarget(int)` - 移除目标
- `void clear()` - 清除所有目标
- `void addAISTargetListener(AISTargetListener)` - 添加监听器
- `void removeAISTargetListener(AISTargetListener)` - 移除监听器

#### AISTarget
- `int getMmsi()` - 获取MMSI
- `String getName()` - 获取名称
- `double getLatitude()` - 获取纬度
- `double getLongitude()` - 获取经度
- `double getSpeed()` - 获取速度
- `double getCourse()` - 获取航向
- `double getHeading()` - 获取船首向
- `int getNavStatus()` - 获取航行状态
- `LocalDateTime getLastUpdate()` - 获取最后更新时间
- `void updatePosition(double, double, double, double)` - 更新位置

#### AISAlarmAssociation
- `void setCpaThreshold(double)` - 设置CPA阈值
- `double getCpaThreshold()` - 获取CPA阈值
- `void setTcpaThreshold(double)` - 设置TCPA阈值
- `double getTcpaThreshold()` - 获取TCPA阈值
- `boolean checkAlarm(AISTarget, double, double)` - 检查预警
- `List<Alert> getTargetAlerts(int)` - 获取目标预警
- `void clearTargetAlerts(int)` - 清除目标预警
- `void clearAll()` - 清除所有

#### CPATCPACalculator
- `CpaResult calculate(double, double, double, ...)` - 计算CPA/TCPA
- `double calculateDistance(double, double, double, double)` - 计算距离
- `double calculateBearing(double, double, double, double)` - 计算方位

---

## 8. echart-route (Layer 3)

**路径**: `ecn/echart-route/src/main/java/cn/cycle/echart/route/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| Route | 类 | 航线数据模型 |
| Waypoint | 类 | 航点数据模型 |
| RouteManager | 类 | 航线管理器 |
| RouteCalculator | 类 | 航线计算器 |
| RouteChecker | 类 | 航线检查器 |
| RouteImporter | 类 | 航线导入器 |
| RouteExporter | 类 | 航线导出器 |

### 核心方法

#### RouteManager
- `Route createRoute(String)` - 创建航线
- `boolean deleteRoute(String)` - 删除航线
- `Route getRoute(String)` - 获取航线
- `List<Route> getAllRoutes()` - 获取所有航线
- `int getRouteCount()` - 航线数量
- `boolean setActiveRoute(String)` - 设置活动航线
- `Route getActiveRoute()` - 获取活动航线
- `void clearActiveRoute()` - 清除活动航线
- `void updateRoute(Route)` - 更新航线
- `void addRouteListener(RouteListener)` - 添加监听器
- `void removeRouteListener(RouteListener)` - 移除监听器

#### Route
- `String getId()` - 获取ID
- `String getName()` - 获取/设置名称
- `int getStatus()` - 获取/设置状态
- `List<Waypoint> getWaypoints()` - 获取航点列表
- `int getWaypointCount()` - 航点数量
- `Waypoint getWaypoint(int)` - 按索引获取航点
- `void addWaypoint(Waypoint)` - 添加航点
- `void insertWaypoint(int, Waypoint)` - 插入航点
- `void removeWaypoint(int)` - 移除航点
- `void clearWaypoints()` - 清除航点
- `void reverse()` - 反转航线
- `Waypoint getCurrentWaypoint()` - 获取当前航点
- `boolean advanceToNextWaypoint()` - 前进到下一航点
- `double getTotalDistance()` - 获取总距离
- `LocalDateTime getEta()` - 获取预计到达时间

#### Waypoint
- `String getId()` - 获取ID
- `String getName()` - 获取/设置名称
- `double getLatitude()` - 获取/设置纬度
- `double getLongitude()` - 获取/设置经度
- `double getArrivalRadius()` - 获取/设置到达半径
- `double getTurnRadius()` - 获取/设置转弯半径
- `int getType()` - 获取/设置类型
- `String getDescription()` - 获取/设置描述
- `boolean isArrival()` - 是否到达
- `LocalDateTime getEta()` - 获取预计到达时间
- `int getSequence()` - 获取序号

#### RouteCalculator
- `double calculateDistance(double, double, double, double)` - 计算距离
- `double calculateLegDistance(Waypoint, Waypoint)` - 计算航段距离
- `double calculateTotalDistance(Route)` - 计算总距离
- `double calculateBearing(double, double, double, double)` - 计算方位
- `double calculateLegBearing(Waypoint, Waypoint)` - 计算航段方位
- `LocalDateTime calculateEta(Route, double, LocalDateTime)` - 计算预计到达时间
- `List<LocalDateTime> calculateWaypointEtas(Route, double, LocalDateTime)` - 计算各航点ETA
- `double calculateRemainingDistance(Route, double, double)` - 计算剩余距离
- `double calculateCrossTrackDistance(...)` - 计算偏航距离

#### RouteChecker
- `void setMinWaypointDistance(double)` - 设置最小航点距离
- `void setMaxWaypointDistance(double)` - 设置最大航点距离
- `void setMinTurnRadius(double)` - 设置最小转弯半径
- `List<CheckResult> checkRoute(Route)` - 检查航线

#### RouteExporter
- `void exportToFile(Route, File)` - 导出到文件
- `void exportToCsv(Route, File)` - 导出为CSV
- `void exportToTxt(Route, File)` - 导出为TXT
- `void exportReport(Route, File)` - 导出报告

#### RouteImporter
- `Route importFromFile(File)` - 从文件导入
- `Route importFromCsv(File)` - 从CSV导入
- `Route importFromTxt(File)` - 从TXT导入
- `List<Route> importFromDirectory(File)` - 从目录导入

---

## 9. echart-workspace (Layer 3)

**路径**: `ecn/echart-workspace/src/main/java/cn/cycle/echart/workspace/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| Workspace | 类 | 工作区数据模型 |
| WorkspaceConfig | 类 | 工作区配置 |
| WorkspaceManager | 类 | 工作区管理器 |
| WorkspacePersister | 类 | 工作区持久化 |

### 核心方法

#### WorkspaceManager
- `Workspace createWorkspace(String)` - 创建工作区
- `Workspace loadWorkspace(File)` - 加载工作区
- `void saveWorkspace(Workspace, File)` - 保存工作区
- `void saveCurrentWorkspace()` - 保存当前工作区
- `void setCurrentWorkspace(Workspace)` - 设置当前工作区
- `Workspace getCurrentWorkspace()` - 获取当前工作区
- `void closeCurrentWorkspace()` - 关闭当前工作区
- `void resetCurrentWorkspace()` - 重置当前工作区
- `String getLastWorkspacePath()` - 获取最后工作区路径
- `void setAutoSave(boolean)` - 设置自动保存
- `boolean isAutoSave()` - 是否自动保存
- `void setAutoSaveInterval(int)` - 设置自动保存间隔
- `int getAutoSaveInterval()` - 获取自动保存间隔
- `void addListener(WorkspaceManagerListener)` - 添加监听器
- `void removeListener(WorkspaceManagerListener)` - 移除监听器

#### Workspace
- `String getId()` - 获取ID
- `String getName()` - 获取/设置名称
- `String getDescription()` - 获取/设置描述
- `LocalDateTime getCreatedTime()` - 获取创建时间
- `LocalDateTime getModifiedTime()` - 获取修改时间
- `String getFilePath()` - 获取/设置文件路径
- `WorkspaceConfig getConfig()` - 获取/设置配置
- `List<String> getLoadedCharts()` - 获取已加载海图
- `void addChart(String)` - 添加海图
- `void removeChart(String)` - 移除海图
- `List<Route> getRoutes()` - 获取航线列表
- `void addRoute(Route)` - 添加航线
- `List<String> getAlarmIds()` - 获取预警ID列表
- `List<AISTarget> getAisTargets()` - 获取AIS目标
- `boolean isDirty()` - 是否已修改
- `void markDirty()` - 标记已修改
- `void markClean()` - 标记未修改
- `void reset()` - 重置

#### WorkspaceConfig
- `double getCenterLatitude()` - 获取/设置中心纬度
- `double getCenterLongitude()` - 获取/设置中心经度
- `double getZoomLevel()` - 获取/设置缩放级别
- `double getRotation()` - 获取/设置旋转角度
- `boolean isShowAisLayer()` - 是否显示AIS图层
- `boolean isShowRouteLayer()` - 是否显示航线图层
- `boolean isShowAlarmLayer()` - 是否显示预警图层
- `boolean isShowChartLayer()` - 是否显示海图图层
- `String getDisplayMode()` - 获取/设置显示模式
- `String getColorScheme()` - 获取/设置颜色方案
- `Map<String, String> getLayerSettings()` - 获取图层设置
- `Map<String, Object> getCustomSettings()` - 获取自定义设置
- `void clear()` - 清除配置

#### WorkspacePersister
- `void save(Workspace, File)` - 保存工作区到文件
- `Workspace load(File)` - 从文件加载工作区

---

## 10. echart-ui (Layer 4)

**路径**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| MainView | 类 | 主布局视图 |
| RibbonMenuBar | 类 | Ribbon菜单栏 |
| ActivityBar | 类 | 活动栏 |
| SideBarManager | 类 | 侧边栏管理器 |
| RightTabManager | 类 | 右侧标签页管理器 |
| ChartDisplayArea | 类 | 海图显示区 |
| StatusBar | 类 | 状态栏 |
| StyleManager | 类 | 样式管理器 |
| FxPlatformAdapter | 类 | JavaFX平台适配器 |
| FxAppContext | 类 | JavaFX应用上下文 |
| FxSideBarPanel | 接口 | 侧边栏面板接口 |
| FxRightTabPanel | 接口 | 右侧标签页面板接口 |
| DataCatalogPanel | 类 | 数据目录面板 |
| LayerManagerPanel | 类 | 图层管理面板 |
| AlarmPanel | 类 | 预警面板 |
| RoutePanel | 类 | 航线面板 |
| LogPanel | 类 | 日志面板 |
| PropertyPanel | 类 | 属性面板 |
| TerminalPanel | 类 | 终端面板 |
| ErrorDialog | 类 | 错误对话框 |
| SoundPlayer | 类 | 声音播放器 |

### 核心方法

#### FxPlatformAdapter
- `String getPlatformName()` - 获取平台名称
- `void runOnUiThread(Runnable)` - 在UI线程运行
- `boolean isUiThread()` - 是否UI线程
- `void playSound(String)` - 播放声音
- `void stopSound()` - 停止声音
- `void showNotification(String, String, String)` - 显示通知
- `double getScreenDpi()` - 获取屏幕DPI
- `double getScreenScale()` - 获取屏幕缩放
- `String getResourcePath(String)` - 获取资源路径
- `String getConfigDirectory()` - 获取配置目录
- `String getLogDirectory()` - 获取日志目录
- `String getCacheDirectory()` - 获取缓存目录

#### ChartDisplayArea
- `void redraw()` - 重绘
- `double getZoomLevel()` - 获取缩放级别
- `void setZoomLevel(double)` - 设置缩放级别
- `void zoomIn()` - 放大
- `void zoomOut()` - 缩小
- `void fitToWindow()` - 适应窗口
- `double getCenterX()` - 获取中心X
- `double getCenterY()` - 获取中心Y
- `void pan(double, double)` - 平移
- `double getRotation()` - 获取旋转角度
- `void setRotation(double)` - 设置旋转角度
- `void rotate(double)` - 旋转
- `Canvas getCanvas()` - 获取画布

#### SideBarManager
- `void registerPanel(FxSideBarPanel)` - 注册面板
- `void unregisterPanel(String)` - 取消注册面板
- `FxSideBarPanel getPanel(String)` - 获取面板
- `void showPanel(String)` - 显示面板
- `void hidePanel(String)` - 隐藏面板
- `FxSideBarPanel getActivePanel()` - 获取活动面板
- `Map<String, FxSideBarPanel> getPanels()` - 获取所有面板

#### ErrorDialog
- `static void showError(String, String)` - 显示错误
- `static void showError(String, String, String)` - 显示错误(带标题)
- `static void showException(String, String, Throwable)` - 显示异常
- `static void showWarning(String, String)` - 显示警告
- `static void showWarning(String, String, String)` - 显示警告(带标题)
- `static void showInfo(String, String)` - 显示信息
- `static void showInfo(String, String, String)` - 显示信息(带标题)
- `static boolean showConfirmation(String, String)` - 显示确认
- `static boolean showConfirmation(String, String, String)` - 显示确认(带标题)
- `static ButtonType showChoices(String, String, String, ...)` - 显示选择

#### DataCatalogPanel
- `String getPanelId()` - 获取面板ID
- `String getTitle()` - 获取标题
- `Image getIconImage()` - 获取图标
- `void onActivate()` - 激活
- `void onDeactivate()` - 停用

#### LayerManagerPanel
- `void addLayer(String, boolean)` - 添加图层
- `String getPanelId()` - 获取面板ID
- `String getTitle()` - 获取标题
- `Image getIconImage()` - 获取图标
- `void onActivate()` - 激活
- `void onDeactivate()` - 停用

#### AlarmPanel
- `String getTabId()` - 获取标签页ID
- `String getIcon()` - 获取图标
- `Tab getTab()` - 获取Tab
- `void onSelected()` - 选中
- `void onDeselected()` - 取消选中
- `void refresh()` - 刷新

#### RoutePanel
- `String getTabId()` - 获取标签页ID
- `String getIcon()` - 获取图标
- `Tab getTab()` - 获取Tab
- `void onSelected()` - 选中
- `void onDeselected()` - 取消选中
- `void refresh()` - 刷新

#### LogPanel
- `void log(String)` - 记录日志
- `void logError(String)` - 记录错误
- `void logWarning(String)` - 记录警告
- `void logInfo(String)` - 记录信息
- `void logDebug(String)` - 记录调试
- `String getTabId()` - 获取标签页ID
- `void refresh()` - 刷新

#### PropertyPanel
- `void setProperties(Map<String, String>)` - 设置属性
- `String getPropertyValue(String)` - 获取属性值
- `Map<String, String> getAllProperties()` - 获取所有属性
- `void clear()` - 清除
- `void refresh()` - 刷新
- `void setTitle(String)` - 设置标题
- `String getTabId()` - 获取标签页ID

#### TerminalPanel
- `void appendOutput(String)` - 追加输出
- `String getTabId()` - 获取标签页ID
- `void refresh()` - 刷新

---

## 11. echart-ui-render (Layer 5)

**路径**: `ecn/echart-ui-render/src/main/java/cn/cycle/echart/ui/render/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| FxRenderContext | 类 | JavaFX渲染上下文 |
| FxCanvasRenderer | 类 | JavaFX Canvas渲染器 |
| ChartFeatureRenderer | 类 | 海图要素渲染器 |
| TileCache | 类 | 瓦片缓存 |
| MeasurementTool | 类 | 测量工具 |
| DistanceMeasurement | 类 | 距离测量 |
| AreaMeasurement | 类 | 面积测量 |
| BearingMeasurement | 类 | 方位测量 |

### 核心方法

#### FxRenderContext
- `int getWidth()` - 获取宽度
- `int getHeight()` - 获取高度
- `double getZoom()` - 获取缩放
- `void setZoom(double)` - 设置缩放
- `Rectangle2D getViewport()` - 获取视口
- `void setViewport(Rectangle2D)` - 设置视口
- `boolean needsRender()` - 是否需要渲染
- `void markDirty()` - 标记脏
- `void clearDirty()` - 清除脏标记
- `double getDevicePixelRatio()` - 获取设备像素比
- `void setDevicePixelRatio(double)` - 设置设备像素比
- `void dispose()` - 销毁
- `GraphicsContext getGraphicsContext()` - 获取图形上下文
- `Canvas getCanvas()` - 获取画布
- `void resize(double, double)` - 调整大小
- `void clear()` - 清除

#### FxCanvasRenderer
- `void start()` - 启动渲染
- `void stop()` - 停止渲染
- `void requestRepaint()` - 请求重绘
- `void setZoom(double)` - 设置缩放
- `double getZoom()` - 获取缩放
- `void zoomIn()` - 放大
- `void zoomOut()` - 缩小
- `void setBackgroundColor(Color)` - 设置背景色
- `Color getBackgroundColor()` - 获取背景色
- `LayerManager getLayerManager()` - 获取图层管理器
- `RenderEngine getEngine()` - 获取渲染引擎
- `FxRenderContext getContext()` - 获取渲染上下文
- `Canvas getCanvas()` - 获取画布
- `boolean isRunning()` - 是否运行中
- `void dispose()` - 销毁

#### ChartFeatureRenderer
- `void render(RenderContext)` - 渲染
- `boolean needsRender(RenderContext)` - 是否需要渲染
- `void setWaterColor(Color)` - 设置水色
- `void setLandColor(Color)` - 设置陆地色
- `void setDepthColor(Color)` - 设置深度色

#### TileCache
- `synchronized Tile get(String)` - 获取瓦片
- `synchronized void put(String, Tile)` - 放入瓦片
- `synchronized void remove(String)` - 移除瓦片
- `synchronized void clear()` - 清除缓存
- `synchronized int size()` - 缓存大小
- `synchronized boolean containsKey(String)` - 是否包含键
- `double getHitRate()` - 命中率
- `int getMaxSize()` - 最大容量
- `static String generateKey(int, int, int)` - 生成键

#### MeasurementTool
- `void setMode(MeasurementMode)` - 设置测量模式
- `MeasurementMode getMode()` - 获取测量模式
- `void addPoint(double, double)` - 添加测量点
- `void removeLastPoint()` - 移除最后一个点
- `void clearPoints()` - 清除所有点
- `void setActive(boolean)` - 设置激活状态
- `boolean isActive()` - 是否激活
- `void render(RenderContext)` - 渲染
- `double getTotalDistance()` - 获取总距离
- `double getTotalArea()` - 获取总面积
- `double getCurrentBearing()` - 获取当前方位
- `List<Point> getPoints()` - 获取测量点
- `void setLineColor(Color)` - 设置线颜色
- `void setPointColor(Color)` - 设置点颜色
- `void setTextColor(Color)` - 设置文字颜色

#### DistanceMeasurement
- `double getDistanceInNauticalMiles()` - 海里距离
- `double getDistanceInKilometers()` - 公里距离
- `double getDistanceInMeters()` - 米距离
- `String getFormattedDistance()` - 格式化距离
- `double getSegmentDistance(int)` - 航段距离
- `double getAverageSpeed(double)` - 平均速度
- `double getEstimatedTimeOfArrival(double)` - 预计到达时间

#### AreaMeasurement
- `double getAreaInSquareNauticalMiles()` - 平方海里面积
- `double getAreaInSquareKilometers()` - 平方公里面积
- `double getAreaInSquareMeters()` - 平方米面积
- `String getFormattedArea()` - 格式化面积
- `double getPerimeter()` - 周长
- `double getPerimeterInNauticalMiles()` - 海里周长
- `double getPerimeterInKilometers()` - 公里周长
- `Point getCentroid()` - 质心
- `boolean isPointInside(double, double)` - 点是否在区域内

#### BearingMeasurement
- `double getBearingInDegrees()` - 度数方位
- `double getBearingInRadians()` - 弧度方位
- `String getFormattedBearing()` - 格式化方位
- `String getCompassDirection(double)` - 罗盘方向
- `double getReciprocalBearing()` - 反方位
- `String getFormattedReciprocalBearing()` - 格式化反方位
- `double getRelativeBearing(double)` - 相对方位
- `boolean isAhead(double)` - 是否前方
- `boolean isAstern(double)` - 是否后方
- `boolean isPort(double)` - 是否左舷
- `boolean isStarboard(double)` - 是否右舷
- `double getQuadrant()` - 象限

---

## 12. echart-theme (Layer 5)

**路径**: `ecn/echart-theme/src/main/java/cn/cycle/echart/theme/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| Theme | 接口 | 主题接口 |
| AbstractTheme | 抽象类 | 主题基类 |
| LightTheme | 类 | 亮色主题 |
| DarkTheme | 类 | 暗色主题 |
| ThemeManager | 类 | 主题管理器 |

### 核心方法

#### ThemeManager
- `void registerTheme(Theme)` - 注册主题
- `void unregisterTheme(String)` - 取消注册主题
- `Theme getTheme(String)` - 获取主题
- `List<Theme> getAvailableThemes()` - 获取可用主题
- `Theme getCurrentTheme()` - 获取当前主题
- `void setCurrentTheme(Theme)` - 设置当前主题
- `void setCurrentTheme(String)` - 按名称设置当前主题
- `Theme getDefaultTheme()` - 获取默认主题
- `void setDefaultTheme(Theme)` - 设置默认主题
- `void resetToDefault()` - 重置为默认
- `boolean hasTheme(String)` - 主题是否存在
- `int getThemeCount()` - 主题数量
- `void addThemeChangeListener(ThemeChangeListener)` - 添加主题变更监听
- `void removeThemeChangeListener(ThemeChangeListener)` - 移除主题变更监听
- `void loadThemeFromProperties(Properties)` - 从属性加载主题
- `void saveThemeToProperties(Properties)` - 保存主题到属性

#### AbstractTheme (实现Theme接口)
- `String getName()` - 获取主题名称
- `String getDisplayName()` - 获取显示名称
- `String getDescription()` - 获取描述
- `String getVersion()` - 获取版本
- `String getAuthor()` - 获取作者
- `boolean isDark()` - 是否暗色主题
- `Map<String, String> getColors()` - 获取颜色映射
- `String getColor(String)` - 获取颜色
- `Map<String, String> getFonts()` - 获取字体映射
- `String getFont(String)` - 获取字体
- `String getStyleSheet()` - 获取样式表
- `String getIconPath()` - 获取图标路径

---

## 13. echart-plugin (Layer 5)

**路径**: `ecn/echart-plugin/src/main/java/cn/cycle/echart/plugin/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| Plugin | 接口 | 插件接口 |
| AbstractPlugin | 抽象类 | 插件基类 |
| PluginContext | 接口 | 插件上下文 |
| PluginManager | 类 | 插件管理器(实现PluginContext) |
| PluginState | 枚举 | 插件状态 |
| PluginException | 类 | 插件异常 |

### 核心方法

#### PluginManager (同时实现PluginContext)
- `void registerPlugin(Plugin)` - 注册插件
- `void unregisterPlugin(String)` - 取消注册插件
- `void initializePlugin(String)` - 初始化插件
- `void startPlugin(String)` - 启动插件
- `void stopPlugin(String)` - 停止插件
- `Plugin getPlugin(String)` - 获取插件
- `List<Plugin> getPlugins()` - 获取所有插件
- `List<Plugin> getPluginsByState(PluginState)` - 按状态获取插件
- `boolean hasPlugin(String)` - 插件是否存在
- `int getPluginCount()` - 插件数量
- `void startAll()` - 启动所有插件
- `void stopAll()` - 停止所有插件
- `void addLifecycleListener(PluginLifecycleListener)` - 添加生命周期监听
- `void removeLifecycleListener(PluginLifecycleListener)` - 移除生命周期监听
- `ServiceLocator getServiceLocator()` - 获取服务定位器
- `Object getProperty(String)` - 获取属性
- `Object getProperty(String, Object)` - 获取属性(带默认值)
- `void setProperty(String, Object)` - 设置属性
- `Map<String, Object> getProperties()` - 获取所有属性
- `String getDataDirectory()` - 获取数据目录
- `String getConfigDirectory()` - 获取配置目录
- `PluginManager getPluginManager()` - 获取插件管理器
- `void log(LogLevel, String)` - 记录日志
- `void log(LogLevel, String, Throwable)` - 记录日志(带异常)

#### AbstractPlugin (实现Plugin接口)
- `String getId()` - 获取插件ID
- `String getName()` - 获取插件名称
- `String getVersion()` - 获取版本
- `String getDescription()` - 获取描述
- `String getAuthor()` - 获取作者
- `Map<String, Object> getConfiguration()` - 获取配置
- `PluginState getState()` - 获取状态
- `void initialize(PluginContext)` - 初始化
- `void start()` - 启动
- `void stop()` - 停止
- `void destroy()` - 销毁

#### PluginState
- `String getDisplayName()` - 获取显示名称
- `boolean isRunning()` - 是否运行中
- `boolean isStopped()` - 是否已停止
- `boolean canStart()` - 是否可启动
- `boolean canStop()` - 是否可停止

#### PluginException
- `String getPluginId()` - 获取插件ID

---

## 14. echart-facade (Layer 6)

**路径**: `ecn/echart-facade/src/main/java/cn/cycle/echart/facade/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| ApplicationFacade | 接口 | 应用门面接口 |
| DefaultApplicationFacade | 类 | 默认应用门面实现 |
| FacadeException | 类 | 门面异常 |

### 核心方法

#### ApplicationFacade (接口)
- `void initialize()` - 初始化
- `void start()` - 启动
- `void stop()` - 停止
- `void destroy()` - 销毁
- `boolean isInitialized()` - 是否已初始化
- `boolean isRunning()` - 是否运行中

#### DefaultApplicationFacade
- `void initialize()` - 初始化
- `void start()` - 启动
- `void stop()` - 停止
- `void destroy()` - 销毁
- `boolean isInitialized()` - 是否已初始化
- `boolean isRunning()` - 是否运行中
- `ChartFileManager getChartFileManager()` - 获取海图文件管理器
- `AlarmManager getAlarmManager()` - 获取预警管理器
- `AISTargetManager getAisTargetManager()` - 获取AIS目标管理器
- `RouteManager getRouteManager()` - 获取航线管理器
- `WorkspaceManager getWorkspaceManager()` - 获取工作区管理器
- `void loadWorkspace(String)` - 加载工作区
- `void saveWorkspace()` - 保存工作区
- `void loadChart(String)` - 加载海图
- `void unloadChart(String)` - 卸载海图

#### FacadeException
- `String getOperation()` - 获取操作名称

---

## 15. echart-app (Layer 7)

**路径**: `ecn/echart-app/src/main/java/cn/cycle/echart/app/`

### 类列表

| 类名 | 类型 | 说明 |
|------|------|------|
| EChartApp | 类 | 应用主类(Application) |
| AppConfig | 类 | 应用配置 |

### 核心方法

#### EChartApp
- `static void main(String[])` - 主入口
- `void init()` - 初始化
- `void start(Stage)` - 启动
- `void stop()` - 停止
- `ApplicationFacade getFacade()` - 获取门面
- `ThemeManager getThemeManager()` - 获取主题管理器
- `I18nManager getI18nManager()` - 获取国际化管理器
- `MainView getMainView()` - 获取主视图
- `Stage getPrimaryStage()` - 获取主舞台

#### AppConfig
- `void save()` - 保存配置
- `String getString(String)` - 获取字符串配置
- `String getString(String, String)` - 获取字符串配置(带默认值)
- `int getInt(String)` - 获取整型配置
- `int getInt(String, int)` - 获取整型配置(带默认值)
- `boolean getBoolean(String)` - 获取布尔配置
- `boolean getBoolean(String, boolean)` - 获取布尔配置(带默认值)
- `void setString(String, String)` - 设置字符串配置
- `void setInt(String, int)` - 设置整型配置
- `void setBoolean(String, boolean)` - 设置布尔配置
- `String getTheme()` - 获取主题
- `void setTheme(String)` - 设置主题
- `String getLocale()` - 获取区域
- `void setLocale(String)` - 设置区域
- `boolean isAutoSaveEnabled()` - 是否自动保存
- `int getAutoSaveInterval()` - 自动保存间隔
- `int getChartCacheSize()` - 海图缓存大小
- `boolean isAlarmSoundEnabled()` - 预警声音是否启用
- `int getAisUpdateInterval()` - AIS更新间隔
- `int getWindowWidth()` - 窗口宽度
- `int getWindowHeight()` - 窗口高度
- `boolean isWindowMaximized()` - 窗口是否最大化
- `void setWindowSettings(int, int, boolean)` - 设置窗口参数

---

## 模块依赖关系图

```
Layer 0:  echart-core
            │
Layer 1:  ├── echart-i18n
            │
Layer 2:  ├── echart-render ── echart-core
            ├── echart-data
            │
Layer 3:  ├── echart-alarm ── echart-core
            ├── echart-ais ── echart-core
            ├── echart-route
            ├── echart-workspace ── echart-route, echart-ais
            │
Layer 4:  ├── echart-ui ── echart-core, echart-data, echart-alarm
            │
Layer 5:  ├── echart-ui-render ── echart-render
            ├── echart-theme
            ├── echart-plugin ── echart-core
            │
Layer 6:  ├── echart-facade ── echart-core, echart-data, echart-alarm, echart-ais, echart-route, echart-workspace
            │
Layer 7:  └── echart-app ── 所有模块
```

---

## 快速查找索引

### 按功能领域查找

| 功能领域 | 模块 | 关键类 |
|----------|------|--------|
| 生命周期管理 | echart-core | LifecycleComponent, AbstractLifecycleComponent |
| 事件通信 | echart-core | EventBus, DefaultEventBus, AppEvent, AppEventType, EventHandler |
| 服务定位 | echart-core | ServiceLocator |
| 错误处理 | echart-core | ErrorHandler, DefaultErrorHandler, ErrorCode |
| 缓存 | echart-core | LRUCache |
| 国际化 | echart-i18n | I18nManager, ResourceBundleLoader, MessageFormatter |
| 渲染引擎 | echart-render | RenderEngine, RenderContext, Layer, LayerManager |
| 脏区域管理 | echart-render | DirtyRegionManager |
| LOD策略 | echart-render | LODStrategy |
| 性能监控 | echart-render | PerformanceMonitor |
| 海图文件管理 | echart-data | ChartFileManager, ChartFile |
| 要素数据 | echart-data | FeatureData, LayerData |
| 预警管理 | echart-alarm | AlarmManager, CpaResult, UkcResult |
| AIS目标 | echart-ais | AISTarget, AISTargetManager, CPATCPACalculator |
| AIS预警关联 | echart-ais | AISAlarmAssociation |
| 航线管理 | echart-route | RouteManager, Route, Waypoint |
| 航线计算 | echart-route | RouteCalculator |
| 航线检查 | echart-route | RouteChecker |
| 航线导入导出 | echart-route | RouteImporter, RouteExporter |
| 工作区管理 | echart-workspace | WorkspaceManager, Workspace, WorkspaceConfig |
| 工作区持久化 | echart-workspace | WorkspacePersister |
| 主布局 | echart-ui | MainView, RibbonMenuBar, ActivityBar |
| 侧边栏 | echart-ui | SideBarManager, FxSideBarPanel |
| 右侧面板 | echart-ui | RightTabManager, FxRightTabPanel |
| 海图显示 | echart-ui | ChartDisplayArea |
| 状态栏 | echart-ui | StatusBar |
| 平台适配 | echart-ui | FxPlatformAdapter |
| 数据目录面板 | echart-ui | DataCatalogPanel |
| 图层管理面板 | echart-ui | LayerManagerPanel |
| 预警面板 | echart-ui | AlarmPanel |
| 航线面板 | echart-ui | RoutePanel |
| 日志面板 | echart-ui | LogPanel |
| 属性面板 | echart-ui | PropertyPanel |
| 终端面板 | echart-ui | TerminalPanel |
| 错误对话框 | echart-ui | ErrorDialog |
| 声音播放 | echart-ui | SoundPlayer |
| 样式管理 | echart-ui | StyleManager |
| JavaFX渲染 | echart-ui-render | FxRenderContext, FxCanvasRenderer |
| 海图要素渲染 | echart-ui-render | ChartFeatureRenderer |
| 瓦片缓存 | echart-ui-render | TileCache |
| 测量工具 | echart-ui-render | MeasurementTool, DistanceMeasurement, AreaMeasurement, BearingMeasurement |
| 主题管理 | echart-theme | ThemeManager, Theme, AbstractTheme, LightTheme, DarkTheme |
| 插件管理 | echart-plugin | PluginManager, Plugin, AbstractPlugin, PluginState |
| 应用门面 | echart-facade | ApplicationFacade, DefaultApplicationFacade |
| 应用入口 | echart-app | EChartApp, AppConfig |

---

**版本**: v1.0  
**日期**: 2026-04-20
