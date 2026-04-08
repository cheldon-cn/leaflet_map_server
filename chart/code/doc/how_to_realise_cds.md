# 海图显示系统实现分析文档

**版本**: v1.2  
**日期**: 2026-04-08  
**状态**: 评审通过版

---

## 1. 执行摘要

本文档基于现有模块资源和设计文档要求，分析海图显示系统(Chart Display System, CDS)的实现路径，重点评估JavaFX应用和Android应用的实现方案，并提出需要补充的信息和可扩展性建议。

### 1.1 核心结论

| 结论项 | 说明 |
|--------|------|
| **优先实现平台** | JavaFX桌面应用（开发效率高、调试便捷、API验证快） |
| **核心模块就绪度** | 约70%（geom/database/feature/layer/parser已实现） |
| **关键缺失模块** | S52样式库、符号库、JavaFX渲染适配 |
| **建议开发周期** | 4个月（JavaFX优先，再迁移Android） |

### 1.2 JavaFX优先的理由

| 维度 | JavaFX优势 | 说明 |
|------|------------|------|
| **开发效率** | 调试效率是Android的3-5倍 | 无需模拟器，直接运行调试 |
| **API验证** | 快速验证Java API设计 | 无JNI桥接复杂性 |
| **渲染调试** | Canvas直接绘制 | 无需OpenGL ES适配 |
| **问题隔离** | 核心逻辑与平台适配分离 | 降低Android开发风险 |
| **代码复用** | Java API可直接复用到Android | 迁移成本低 |

### 1.3 性能指标定义

| 指标类别 | 指标名称 | 目标值 | 测量方法 |
|----------|----------|--------|----------|
| **渲染性能** | 帧率 | ≥30fps | 帧计数器统计 |
| | 首帧渲染时间 | <500ms | 从加载到首帧完成 |
| | 瓦片渲染时间 | <50ms/瓦片 | 单瓦片渲染耗时 |
| **加载性能** | 单张海图加载 | <2s | 从文件到内存 |
| | 瓦片加载时间 | <100ms | 从缓存到显示 |
| | 离线数据初始化 | <5s | 应用启动到可用 |
| **内存性能** | 峰值内存(Android) | <512MB | 内存分析工具 |
| | 峰值内存(桌面) | <1GB | 内存分析工具 |
| | 缓存内存上限 | 256MB | 可配置 |
| **响应性能** | 平移响应延迟 | <100ms | 触摸到画面更新 |
| | 缩放响应延迟 | <150ms | 手势到画面更新 |
| | 要素查询响应 | <50ms | 点击到信息显示 |

---

## 2. 现有资源分析

### 2.1 已实现模块清单

根据 `code/index_all.md` 和实际代码结构，现有模块实现情况如下：

| 模块 | 实现状态 | 核心类 | DLL/LIB | 头文件 |
|------|----------|--------|---------|--------|
| **base** | ✅ 完成 | Logger, ThreadSafe, PerformanceMonitor | ogc_base.dll | ogc/base/*.h |
| **proj** | ✅ 完成 | CoordinateTransformer, ProjTransformer | ogc_proj.dll | ogc/proj/*.h |
| **cache** | ✅ 完成 | TileCache, TileKey, OfflineStorageManager | ogc_cache.dll | ogc/cache/*.h |
| **symbology** | ✅ 完成 | Symbolizer, Filter, SymbolizerRule | ogc_symbology.dll | ogc/symbology/*.h |
| **geom** | ✅ 完成 | Geometry, Point, LineString, Polygon | ogc_geom.dll | ogc/geom/*.h |
| **database** | ✅ 完成 | DbConnection, DbConnectionPool, PostGIS, SpatiaLite | ogc_database.dll | ogc/db/*.h |
| **feature** | ✅ 完成 | CNFeature, CNFeatureDefn, CNFieldValue | ogc_feature.dll | ogc/feature/*.h |
| **layer** | ✅ 完成 | CNLayer, CNVectorLayer, CNDataSource | ogc_layer.dll | ogc/layer/*.h |
| **draw** | ✅ 完成 | DrawEngine, DrawDevice, DrawContext | ogc_draw.dll | ogc/draw/*.h |
| **graph** | ✅ 完成 | DrawFacade, Symbolizer, LabelEngine | ogc_graph.dll | ogc/draw/*.h |
| **alert** | ✅ 完成 | AlertEngine, IAlertChecker, CpaCalculator | ogc_alert.dll | ogc/alert/*.h |
| **navi** | ✅ 完成 | Route, AisManager, NavigationEngine | ogc_navi.dll | ogc/navi/*.h |
| **parser** | ✅ 完成 | ChartParser, S57Parser, S101Parser | chart_parser.dll | chart_parser/*.h |
| **android_adapter** | ⚠️ 框架完成 | AndroidEngine, AndroidTouchHandler | - | android_adapter/*.h |
| **jni_bridge** | ⚠️ 框架完成 | JniEnvManager, JniConverter | - | jni_bridge/*.h |
| **java_api** | ⚠️ 框架完成 | ChartViewer, ChartView, ChartConfig | - | Java类 |

### 2.2 模块依赖关系图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           现有模块依赖关系                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    应用层 (Application Layer)                        │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ Android App   │  │  JavaFX App   │  │  Web App      │           │   │
│  │  │  (待开发)     │  │  (待开发)     │  │  (待开发)     │           │   │
│  │  └───────┬───────┘  └───────┬───────┘  └───────────────┘           │   │
│  └──────────┼──────────────────┼──────────────────────────────────────┘   │
│             │                  │                                           │
│             ▼                  ▼                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    桥接层 (Bridge Layer)                             │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ java_api      │  │  jni_bridge   │  │android_adapter│           │   │
│  │  │ (框架完成)    │  │  (框架完成)   │  │  (框架完成)   │           │   │
│  │  │ [chart/java_  │  │ [chart/jni_   │  │ [chart/android│           │   │
│  │  │  api]         │  │  bridge]      │  │  _adapter]    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│         ┌──────────────────────────┼──────────────────────────┐            │
│         ▼                          ▼                          ▼            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    业务功能层 (Business Layer)                       │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ parser        │  │  navi         │  │  alarm        │           │   │
│  │  │ ✅ 完成       │  │  ✅ 完成      │  │  ✅ 完成      │           │   │
│  │  │ S57/S101解析  │  │  导航/航线    │  │ 警报服务层    │           │   │
│  │  │ [chart/parser]│  │               │  │ (REST/WS)     │           │   │
│  │  └───────┬───────┘  └───────────────┘  └───────┬───────┘           │   │
│  │          │                                        │                  │   │
│  │          │                                        ▼ 依赖             │   │
│  │          │                              ┌─────────────────┐         │   │
│  │          │                              │ alert ✅ 完成   │         │   │
│  │          │                              │ 警报核心层      │         │   │
│  │          │                              │ (检查/计算)     │         │   │
│  │          │                              └────────┬────────┘         │   │
│  └──────────┼───────────────────────────────────────┼──────────────────┘   │
│             │                                        │                      │
│             │ parser独立路径                         │ 业务层路径           │
│             ▼                                        ▼                      │
│  ┌──────────────────────┐   ┌─────────────────────────────────────────────┐ │
│  │ 数据解析路径         │   │              核心基础层 (Core Layer)         │ │
│  │ ┌─────────────────┐  │   │  ┌───────────────┐  ┌───────────────┐      │ │
│  │ │ feature ✅      │◄─┼───┤  │ graph ✅      │  │ draw ✅       │      │ │
│  │ │ 要素模型        │  │   │  │ 渲染门面      │  │ 绘图引擎      │      │ │
│  │ └────────┬────────┘  │   │  └───────┬───────┘  └───────┬───────┘      │ │
│  │          │           │   │          │                  │              │ │
│  │          ▼           │   │  ┌───────┴───────┐  ┌───────┴───────┐      │ │
│  │ ┌─────────────────┐  │   │  │ symbology ✅  │  │ cache ✅      │      │ │
│  │ │ geom ✅         │  │   │  │ 符号化        │  │ 瓦片缓存      │      │ │
│  │ │ 几何操作        │  │   │  └───────────────┘  └───────────────┘      │ │
│  │ └─────────────────┘  │   │  ┌───────────────┐  ┌───────────────┐      │ │
│  └──────────────────────┘   │  │ layer ✅      │  │ feature ✅    │      │ │
│                              │  │ 图层管理      │  │ 要素模型      │      │ │
│                              │  └───────────────┘  └───────┬───────┘      │ │
│                              └─────────────────────────────┼─────────────┘ │
│                                                            │               │
│                                                            ▼               │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    数据存储层 (Data Layer)                           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ database ✅   │  │  geom ✅      │  │  proj ✅      │           │   │
│  │  │ 数据库访问    │  │  几何操作     │  │  坐标转换     │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    基础工具层 (Utility Layer)                        │   │
│  │  ┌───────────────────────────────────────────────────────────────┐ │   │
│  │  │ base ✅ 完成                                                   │ │   │
│  │  │ 日志、线程安全、性能监控、内存管理                             │ │   │
│  │  └───────────────────────────────────────────────────────────────┘ │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  说明:                                                                      │
│  - parser独立于graph，直接依赖feature/geom                                  │
│  - alarm依赖alert模块接口(IAlertChecker/IAlertRepository等)                 │
│  - alert是警报核心层，提供检查计算功能                                       │
│  - alarm是警报服务层，提供REST/WebSocket服务                                │
│  - [xxx] 表示模块所在子目录                                                 │
│  - ✅ 表示模块已完成                                                        │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.3 现有资源评估

| 评估维度 | 现状 | 就绪度 |
|----------|------|--------|
| 核心数据模型 | geom/feature/layer完整实现 | 95% |
| 数据存储 | SQLite/PostgreSQL双支持 | 90% |
| 数据解析 | S57/S101/S102解析器完成 | 90% |
| 渲染引擎 | draw/graph模块框架完成 | 70% |
| Android适配 | JNI桥接框架完成 | 50% |
| JavaFX适配 | 未开始 | 0% |
| S52样式库 | 未实现 | 0% |
| 符号库 | 未实现 | 0% |

---

## 3. JavaFX应用实现方案

### 3.1 推荐实现路径

基于现有资源和开发效率考虑，**JavaFX应用应作为优先实现平台**，原因如下：

1. **开发效率高**：无需JNI桥接复杂性，直接使用Java API
2. **调试便捷**：无需模拟器，直接运行调试
3. **API验证快**：快速验证Java API设计，为Android迁移做准备
4. **渲染简单**：Canvas直接绘制，无需OpenGL ES适配
5. **问题隔离**：核心逻辑与平台适配分离，降低开发风险

### 3.2 JavaFX应用架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           JavaFX应用架构                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX应用层 (JavaFX Application)                 │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainView      │  │ ChartView     │  │ SettingsView  │           │   │
│  │  │ 主界面        │  │ 海图视图      │  │ 设置界面      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ LayerControl  │  │ PositionView  │  │ AlertPanel    │           │   │
│  │  │ 图层控制      │  │ 船位显示      │  │ 警报面板      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ToolBar       │  │ JavaFXCanvas  │  │ JavaFXEvent    │           │   │
│  │  │ 工具栏        │  │ Canvas渲染      │  │ 事件处理      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX适配层 (JavaFX Adapters)                    │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │CanvasAdapter  │  │EventHandler   │  │ ImageDevice   │           │   │
│  │  │ - DrawDevice  │  │ - MouseHandler│  │ - RasterDevice│           │   │
│  │  │ - beginDraw() │  │ - KeyHandler  │  │ - toImage()   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java API层 (java_api模块)                         │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartViewer   │  │ ChartConfig   │  │ FeatureInfo   │           │   │
│  │  │ - loadChart() │  │ - setStyle()  │  │ - getName()   │           │   │
│  │  │ - render()    │  │ - setMode()   │  │ - getGeom()   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ Geometry      │  │ ChartLayer    │  │ Position      │           │   │
│  │  │ - Point       │  │ - setVisible()│  │ - getLat()    │           │   │
│  │  │ - LineString  │  │ - setOpacity()│  │ - getLon()    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ JNI                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JNI桥接层 (jni_bridge模块)                        │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ JniEnvManager │  │ JniConverter  │  │ JniException  │           │   │
│  │  │ - GetEnv()    │  │ - ToString()  │  │ - Check()     │           │   │
│  │  │ - Attach()    │  │ - ToJString() │  │ - Throw()     │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐                              │   │
│  │  │ JniMemory     │  │ JniReference  │                              │   │
│  │  │ - Alloc()     │  │ - LocalRef    │                              │   │
│  │  │ - Release()   │  │ - GlobalRef   │                              │   │
│  │  └───────────────┘  └───────────────┘                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (Native Libraries)                      │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ogc_graph     │  │ ogc_draw      │  │ ogc_layer     │           │   │
│  │  │ 渲染门面      │  │ 绘图引擎      │  │ 图层管理      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ogc_symbology │  │ ogc_cache     │  │ ogc_feature   │           │   │
│  │  │ 符号化        │  │ 瓦片缓存      │  │ 要素模型      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ogc_database  │  │ ogc_geom      │  │ ogc_proj      │           │   │
│  │  │ 数据库访问    │  │ 几何操作      │  │ 坐标转换      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ogc_navi      │  │ ogc_alert     │  │ ogc_base      │           │   │
│  │  │ 导航系统      │  │ 航海警报      │  │ 基础工具      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐                                                   │   │
│  │  │ chart_parser  │  (位于chart/parser目录)                          │   │
│  │  │ 海图解析      │                                                   │   │
│  │  └───────────────┘                                                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2.1 JavaFX适配器设计

```cpp
// JavaFX Canvas渲染适配器
class JavaFXCanvasAdapter : public ogc::draw::DrawDevice {
public:
    JavaFXCanvasAdapter(jobject canvas);
    ~JavaFXCanvasAdapter() override;
    
    // DrawDevice接口实现
    void BeginDraw() override;
    void EndDraw() override;
    void DrawGeometry(const Geometry* geom, const DrawStyle& style) override;
    void DrawText(const std::string& text, const Point& pos, const TextStyle& style) override;
    void DrawImage(const Image* img, const Envelope& bounds) override;
    
private:
    jobject m_canvas;
    GraphicsContext m_gc;
};

// JavaFX事件处理器
class JavaFXEventHandler {
public:
    void OnMouseEvent(jobject event, MouseAction action);
    void OnKeyEvent(jobject event, KeyAction action);
    void OnScrollEvent(jobject event);
    
    // 事件回调
    void SetPanHandler(std::function<void(double, double)> handler);
    void SetZoomHandler(std::function<void(double, double, double)> handler);
    void SetClickHandler(std::function<void(double, double)> handler);
    
private:
    std::function<void(double, double)> m_panHandler;
    std::function<void(double, double, double)> m_zoomHandler;
    std::function<void(double, double)> m_clickHandler;
};

// JavaFX图像设备
class JavaFXImageDevice : public ogc::draw::RasterImageDevice {
public:
    JavaFXImageDevice(int width, int height);
    
    // RasterImageDevice接口实现
    void* GetPixelData() override;
    int GetWidth() const override;
    int GetHeight() const override;
    
    // 转换为JavaFX Image
    jobject ToJavaFXImage();
    
private:
    std::vector<uint8_t> m_pixelData;
    int m_width, m_height;
};
```

### 3.3 JavaFX实现任务清单

| 阶段 | 任务 | 优先级 | 预估工时 | 依赖 | 详细分解 |
|------|------|--------|----------|------|----------|
| **阶段1: 基础框架** | | | **2周** | | |
| | JavaFX项目初始化 | P0 | 2天 | - | Maven配置、模块结构 |
| | Java API封装设计 | P0 | 3天 | java_api | 接口定义、异常设计 |
| | JNI桥接完善 | P0 | 3天 | jni_bridge | 数据转换、异常传递 |
| | 基础UI框架搭建 | P0 | 2天 | JavaFX | 主界面、菜单栏 |
| | 实现主界面布局 | P0 | 1周 | - |
| | 实现工具栏和菜单 | P0 | 0.5周 | - |
| | 实现图层面板 | P0 | 0.5周 | - |
| | 实现属性面板 | P1 | 0.5周 | - |
| | 实现设置界面 | P1 | 0.5周 | - |
| **阶段2: 核心渲染** | | | **3周** | | |
| | Canvas渲染适配器 | P0 | 5天 | draw模块 | DrawDevice实现 |
| | 瓦片渲染流程 | P0 | 4天 | graph模块 | 瓦片请求、渲染、缓存 |
| | 图层管理集成 | P0 | 3天 | layer模块 | 图层显示控制 |
| | 坐标转换集成 | P0 | 3天 | proj模块 | 坐标系转换 |
| **阶段3: 数据处理** | | | **2周** | | |
| | 海图加载流程 | P0 | 4天 | parser模块 | 文件选择、解析、显示 |
| | 离线数据管理 | P1 | 3天 | database模块 | 数据导入导出 |
| | 缓存策略实现 | P1 | 3天 | cache模块 | 内存/磁盘缓存 |
| **阶段4: 交互功能** | | | **2周** | | |
| | 平移缩放交互 | P0 | 3天 | JavaFX | 鼠标事件处理 |
| | 要素选择查询 | P0 | 3天 | graph模块 | 点击测试、信息显示 |
| | 图层控制面板 | P1 | 2天 | JavaFX | 图层树、可见性控制 |
| | 设置界面 | P2 | 2天 | JavaFX | 样式、显示模式配置 |
| **阶段5: 样式符号** | | | **3周** | | |
| | S52样式库实现 | P0 | 5天 | symbology模块 | 样式规则解析 |
| | 符号库实现 | P0 | 5天 | S52样式库 | 符号渲染 |
| | 显示模式切换 | P1 | 2天 | symbology模块 | 日/夜/黄昏模式 |
| | 符号化渲染集成 | P0 | 3天 | graph模块 | 样式应用 |
| | 集成定位显示功能 | P1 | 0.5周 | navi模块 |


### 3.4 JavaFX关键技术点

#### 3.4.1 Canvas渲染实现

```java
public class ChartCanvas extends Canvas {
    private final ChartViewer viewer;
    private final AnimationTimer renderTimer;
    
    public ChartCanvas(ChartViewer viewer) {
        this.viewer = viewer;
        
        // 设置渲染循环
        renderTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                render();
            }
        };
        
        // 绑定尺寸变化
        widthProperty().addListener(evt -> render());
        heightProperty().addListener(evt -> render());
        
        // 绑定事件处理
        setupEventHandlers();
    }
    
    private void render() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, getWidth(), getHeight());
        
        // 调用Native渲染
        viewer.render(gc, getWidth(), getHeight());
    }
}
```

#### 3.4.2 桌面端特有功能

| 功能 | 说明 | 实现方式 |
|------|------|----------|
| 多窗口 | 支持多海图窗口 | JavaFX Stage |
| 打印输出 | 海图打印功能 | JavaFX Print API |
| 快捷键 | 键盘快捷操作 | JavaFX KeyEvent |
| 拖放操作 | 文件拖放加载 | JavaFX DragEvent |
| 分屏显示 | 多屏显示支持 | JavaFX Screen API |

---

## 4. Android应用实现方案

### 4.1 实现路径

JavaFX应用验证完成后，将核心功能迁移到Android平台。迁移策略：

1. **Java API复用**：java_api模块可直接复用
2. **渲染适配**：从JavaFX Canvas迁移到OpenGL ES
3. **交互适配**：从鼠标事件迁移到触摸事件
4. **资源优化**：针对移动端优化内存和性能

### 4.2 Android应用架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Android应用架构                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Android应用层 (Kotlin/Java)                       │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainActivity  │  │ ChartFragment │  │ SettingsAct   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartView     │  │ PositionView  │  │ LayerControl  │           │   │
│  │  │ (OpenGL ES)   │  │ (船位显示)    │  │ (图层控制)    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java API层 (java_api模块)                         │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartViewer   │  │ ChartConfig   │  │ FeatureInfo   │           │   │
│  │  │ - loadChart() │  │ - setStyle()  │  │ - getName()   │           │   │
│  │  │ - render()    │  │ - setMode()   │  │ - getGeom()   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ Geometry      │  │ ChartLayer    │  │ Position      │           │   │
│  │  │ - Point       │  │ - setVisible()│  │ - getLat()    │           │   │
│  │  │ - LineString  │  │ - setOpacity()│  │ - getLon()    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ JNI                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JNI桥接层 (jni_bridge模块)                        │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ JniEnvManager │  │ JniConverter  │  │ JniException  │           │   │
│  │  │ - GetEnv()    │  │ - ToString()  │  │ - Check()     │           │   │
│  │  │ - Attach()    │  │ - ToJString() │  │ - Throw()     │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (Native Libraries)                      │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ libogc_graph  │  │ libogc_draw   │  │ libogc_parser │           │   │
│  │  │ 渲染引擎      │  │ 绘制引擎      │  │ 数据解析      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ libogc_layer  │  │ libogc_feature│  │ libogc_geom   │           │   │
│  │  │ 图层管理      │  │ 要素管理      │  │ 几何操作      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐                              │   │
│  │  │ libogc_database│  │ libchart_parser│                             │   │
│  │  │ 数据库访问    │  │ 海图解析      │                              │   │
│  │  └───────────────┘  └───────────────┘                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Android适配层 (android_adapter模块)               │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ AndroidEngine │  │ AndroidTouch  │  │ AndroidLife   │           │   │
│  │  │ OpenGL ES渲染 │  │ 触摸事件处理  │  │ 生命周期管理  │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.3 Android实现任务清单

| 阶段 | 任务 | 优先级 | 预估工时 | 依赖 | 详细分解 |
|------|------|--------|----------|------|----------|
| **阶段1: 核心渲染** | | | **4周** | | |
| | OpenGL ES渲染引擎完善 | P0 | 2周 | android_adapter | 着色器、纹理、VAO/VBO |
| | 瓦片缓存机制实现 | P0 | 1周 | cache模块 | LRU缓存、内存控制 |
| | LOD渲染策略实现 | P0 | 1周 | draw模块 | 多级细节、动态加载 |
| **阶段2: 数据处理** | | | **3周** | | |
| | JNI数据转换完善 | P0 | 1周 | jni_bridge | 批量转换、内存优化 |
| | 离线数据存储实现 | P0 | 1周 | database模块 | SQLite集成、数据导入 |
| | 海图增量更新 | P1 | 1周 | parser模块 | 增量解析、差异更新 |
| **阶段3: 交互功能** | | | **2周** | | |
| | 触摸手势处理 | P0 | 1周 | android_adapter | 平移、缩放、旋转 |
| | 要素选择查询 | P0 | 1周 | graph模块 | 点击测试、信息弹窗 |
| | 实现定位显示 | P1 | 0.5周 | navi模块 |
| **阶段4: 显示样式** | | | **3周** | |
| | 实现S52样式库 | P0 | 2周 | graph模块 |
| | 实现符号库加载 | P0 | 1周 | graph模块 |
| | 实现日/夜模式切换 | P1 | 0.5周 | graph模块 |
| **阶段5: 应用集成** | | | **2周** | |
| | 实现Android Activity | P0 | 1周 | java_api |
| | 实现设置界面 | P1 | 0.5周 | - |
| | 实现权限管理 | P1 | 0.5周 | android_adapter |
| **阶段6: 性能优化** | | | **2周** | | |
| | 内存优化 | P0 | 1周 | 全部模块 | 内存池、对象复用 |
| | 渲染优化 | P0 | 1周 | draw模块 | 批处理、实例化渲染 |


### 4.4 Android关键技术点

#### 4.4.1 OpenGL ES渲染优化

```cpp
// 推荐的渲染架构
class AndroidChartRenderer {
public:
    void Render(const Viewport& viewport) {
        // 1. 检查瓦片缓存
        auto tiles = m_tileCache->GetVisibleTiles(viewport);
        
        // 2. 异步加载缺失瓦片
        for (const auto& tileId : tiles.missing) {
            m_tileLoader->LoadAsync(tileId);
        }
        
        // 3. 渲染可见瓦片
        for (const auto& tile : tiles.visible) {
            RenderTile(tile);
        }
        
        // 4. 渲染动态要素（船位、航线等）
        RenderDynamicElements(viewport);
    }
    
private:
    std::unique_ptr<TileCache> m_tileCache;
    std::unique_ptr<AsyncTileLoader> m_tileLoader;
};
```

#### 4.4.2 内存管理策略

| 策略 | 说明 | 目标 |
|------|------|------|
| 瓦片缓存 | LRU策略，限制内存占用 | <200MB |
| 纹理池 | 复用OpenGL纹理对象 | 减少GC压力 |
| 对象池 | 复用几何对象 | 减少内存分配 |
| 分级加载 | 按比例尺分级加载数据 | 按需加载 |

---

## 5. 平台抽象层设计

### 5.1 设计目标

为支持多平台（JavaFX、Android、WebAssembly），设计平台抽象层(Platform Abstraction Layer, PAL)，隔离平台差异。

### 5.2 核心接口设计

```cpp
namespace ogc::pal {

class IRenderDevice {
public:
    virtual ~IRenderDevice() = default;
    virtual bool Initialize(int width, int height) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void DrawGeometry(const Geometry* geom, const DrawStyle& style) = 0;
    virtual void DrawText(const std::string& text, const Point& pos, const TextStyle& style) = 0;
    virtual void DrawImage(const Image* img, const Envelope& bounds) = 0;
    virtual void* GetNativeHandle() = 0;
};

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual bool FileExists(const std::string& path) = 0;
    virtual std::unique_ptr<std::istream> OpenRead(const std::string& path) = 0;
    virtual std::unique_ptr<std::ostream> OpenWrite(const std::string& path) = 0;
    virtual std::string GetAppDataDir() = 0;
    virtual std::string GetCacheDir() = 0;
};

class IThreadManager {
public:
    virtual ~IThreadManager() = default;
    virtual void RunOnMainThread(std::function<void()> task) = 0;
    virtual void RunOnBackground(std::function<void()> task) = 0;
    virtual bool IsMainThread() = 0;
};

class PlatformServices {
public:
    static IRenderDevice* GetRenderDevice();
    static IFileSystem* GetFileSystem();
    static IThreadManager* GetThreadManager();
    static void Initialize(PlatformType type);
};

}
```

### 5.3 平台适配器实现

| 平台 | RenderDevice | FileSystem | ThreadManager |
|------|--------------|------------|---------------|
| JavaFX | JavaFXCanvasDevice | DesktopFileSystem | JavaFXThreadManager |
| Android | OpenGLESDevice | AndroidFileSystem | AndroidThreadManager |
| WebAssembly | WebGLDevice | EmscriptenFileSystem | WasmThreadManager |

---

## 6. 插件架构设计

### 6.1 设计目标

#### 6.1.1 插件化架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           插件化架构设计                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    插件管理器 (PluginManager)                        │   │
│  │  - LoadPlugin()     加载插件                                        │   │
│  │  - UnloadPlugin()   卸载插件                                        │   │
│  │  - GetPlugin()      获取插件实例                                    │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│         ┌──────────────────────────┼──────────────────────────┐            │
│         ▼                          ▼                          ▼            │
│  ┌───────────────┐         ┌───────────────┐         ┌───────────────┐    │
│  │ 渲染引擎插件  │         │ 数据源插件    │         │ 样式插件      │    │
│  │ IRenderEngine │         │ IDataSource   │         │ IStyleProvider│    │
│  ├───────────────┤         ├───────────────┤         ├───────────────┤    │
│  │ - OpenGL      │         │ - S57         │         │ - S52         │    │
│  │ - DirectX     │         │ - S101        │         │ - Custom      │    │
│  │ - Vulkan      │         │ - GeoJSON     │         │ - Day/Night   │    │
│  │ - Metal       │         │ - Shapefile   │         │               │    │
│  └───────────────┘         └───────────────┘         └───────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**插件接口定义**：

```cpp
// 渲染引擎插件接口
class IRenderEnginePlugin {
public:
    virtual ~IRenderEnginePlugin() = default;
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::unique_ptr<IRenderEngine> CreateEngine() = 0;
    virtual bool IsSupported(PlatformType platform) const = 0;
};

// 数据源插件接口
class IDataSourcePlugin {
public:
    virtual ~IDataSourcePlugin() = default;
    virtual std::string GetName() const = 0;
    virtual std::vector<std::string> GetSupportedFormats() const = 0;
    virtual std::unique_ptr<IDataSource> CreateSource(const std::string& path) = 0;
};

// 样式插件接口
class IStylePlugin {
public:
    virtual ~IStylePlugin() = default;
    virtual std::string GetName() const = 0;
    virtual Style GetStyle(const std::string& featureType) const = 0;
    virtual std::map<std::string, Color> GetColorScheme(DayNightMode mode) const = 0;
};
```

#### 6.1.2 插件生命周期管理

```cpp
namespace ogc::plugin {

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<std::string> supportedFormats;
};

struct PluginDependency {
    std::string name;
    std::string minVersion;
    bool required;
};

class PluginContext;

class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual PluginInfo GetInfo() const = 0;
    virtual std::vector<PluginDependency> GetDependencies() const = 0;
    
    virtual bool Initialize(PluginContext* context) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
};

class PluginContext {
public:
    virtual ~PluginContext() = default;
    virtual std::string GetPluginDir() const = 0;
    virtual std::string GetConfigDir() const = 0;
    virtual ILogger* GetLogger() const = 0;
    virtual IPlugin* GetPlugin(const std::string& name) = 0;
};

}
```

#### 6.1.3 动态库加载机制

```cpp
namespace ogc::plugin {

class DynamicLibrary {
public:
    static std::unique_ptr<DynamicLibrary> Load(const std::string& path);
    ~DynamicLibrary();
    
    void* GetSymbol(const std::string& name);
    template<typename T>
    T GetFunction(const std::string& name) {
        return reinterpret_cast<T>(GetSymbol(name));
    }
    
    const std::string& GetPath() const { return m_path; }
    bool IsLoaded() const { return m_handle != nullptr; }
    
private:
    DynamicLibrary(const std::string& path, void* handle);
    std::string m_path;
    void* m_handle;
};

using CreatePluginFunc = IPlugin* (*)();
using DestroyPluginFunc = void (*)(IPlugin*);

class PluginLoader {
public:
    static std::unique_ptr<IPlugin> LoadPlugin(const std::string& path);
    static void UnloadPlugin(IPlugin* plugin);
    static std::vector<std::string> FindPlugins(const std::string& directory);
    
private:
    static bool ValidatePlugin(DynamicLibrary* lib);
};

}
```

#### 6.1.4 完整的PluginManager实现

```cpp
namespace ogc::plugin {

class PluginManager {
public:
    static PluginManager& Instance();
    
    bool LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& name);
    void UnloadAllPlugins();
    
    void RegisterPlugin(std::unique_ptr<IPlugin> plugin);
    void UnregisterPlugin(const std::string& name);
    
    IPlugin* GetPlugin(const std::string& name);
    template<typename T>
    T* GetPluginAs(const std::string& name) {
        return dynamic_cast<T*>(GetPlugin(name));
    }
    
    std::vector<PluginInfo> GetAllPlugins() const;
    std::vector<std::string> GetPluginNames() const;
    
    bool ResolveDependencies();
    std::vector<std::string> GetLoadOrder();
    
    bool InitializeAll();
    void ShutdownAll();
    
    void SetPluginDir(const std::string& dir);
    void SetPluginContext(std::unique_ptr<PluginContext> context);
    
private:
    PluginManager() = default;
    
    std::map<std::string, std::unique_ptr<IPlugin>> m_plugins;
    std::map<std::string, std::unique_ptr<DynamicLibrary>> m_libraries;
    std::unique_ptr<PluginContext> m_context;
    std::string m_pluginDir;
};

}
```

#### 6.1.5 模块解耦设计

| 设计原则 | 实现方式 | 好处 |
|----------|----------|------|
| 依赖注入 | 使用接口和工厂模式 | 便于替换实现 |
| 事件驱动 | 使用观察者模式 | 模块间松耦合 |
| 配置化 | 外部配置文件驱动 | 无需修改代码 |
| 热加载 | 动态库加载机制 | 运行时扩展 |
| 生命周期 | Initialize/Shutdown模式 | 资源管理可控 |
| 依赖解析 | 拓扑排序算法 | 加载顺序正确 |

### 6.2 插件接口设计

为支持未来海图格式扩展（S-102水深、S-111海流、S-104水位等），无需修改核心代码。

```cpp
namespace chart::plugin {

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::vector<std::string> supportedFormats;
};

class IChartParserPlugin {
public:
    virtual ~IChartParserPlugin() = default;
    virtual PluginInfo GetInfo() const = 0;
    virtual bool CanParse(const std::string& format) const = 0;
    virtual std::unique_ptr<IChartParser> CreateParser(const std::string& format) = 0;
    virtual std::vector<std::string> GetFileExtensions() const = 0;
};

class PluginManager {
public:
    static PluginManager& Instance();
    void RegisterPlugin(std::unique_ptr<IChartParserPlugin> plugin);
    void UnregisterPlugin(const std::string& name);
    IChartParserPlugin* GetPlugin(const std::string& format);
    std::vector<PluginInfo> GetAllPlugins() const;
    bool LoadPlugin(const std::string& path);
    void UnloadAllPlugins();
};

}
```

### 6.3 插件注册示例

```cpp
class S102ParserPlugin : public IChartParserPlugin {
public:
    PluginInfo GetInfo() const override {
        return {"S-102 Parser", "1.0.0", "OGC Team", {"S-102"}};
    }
    
    bool CanParse(const std::string& format) const override {
        return format == "S-102";
    }
    
    std::unique_ptr<IChartParser> CreateParser(const std::string& format) override {
        return std::make_unique<S102Parser>();
    }
    
    std::vector<std::string> GetFileExtensions() const override {
        return {".102", ".s102"};
    }
};

PluginManager::Instance().RegisterPlugin(std::make_unique<S102ParserPlugin>());
```

### 6.4 API可扩展性

#### 6.4.1 版本化API

```java
// Java API版本管理
public class ChartAPI {
    private static final int CURRENT_VERSION = 2;
    
    public static ChartViewer createViewer(int apiVersion) {
        switch (apiVersion) {
            case 1:
                return new ChartViewerV1();  // 兼容旧版本
            case 2:
            default:
                return new ChartViewerV2();  // 当前版本
        }
    }
}
```

#### 6.4.2 功能开关

```java
// 功能开关配置
public class FeatureFlags {
    private Map<String, Boolean> flags = new HashMap<>();
    
    public boolean isEnabled(String feature) {
        return flags.getOrDefault(feature, false);
    }
    
    public void setEnabled(String feature, boolean enabled) {
        flags.put(feature, enabled);
    }
}

// 使用示例
if (featureFlags.isEnabled("advanced_rendering")) {
    viewer.enableAdvancedRendering();
}
```

---

## 7. 缓存策略详细设计

### 7.1 多级缓存架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           多级缓存架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L1: 热数据缓存 (Hot Cache)                        │   │
│  │  - 容量: 64MB                                                        │   │
│  │  - 策略: LRU with Frequency                                         │   │
│  │  - 内容: 当前视口瓦片、最近访问要素                                  │   │
│  │  - 命中率目标: > 80%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ 未命中                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L2: 内存缓存 (Memory Cache)                       │   │
│  │  - 容量: 256MB (可配置)                                              │   │
│  │  - 策略: LRU with Size Awareness                                    │   │
│  │  - 内容: 渲染瓦片、符号化结果                                        │   │
│  │  - 命中率目标: > 60%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ 未命中                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L3: 磁盘缓存 (Disk Cache)                         │   │
│  │  - 容量: 2GB (可配置)                                                │   │
│  │  - 策略: Time-based LRU                                             │   │
│  │  - 内容: 原始瓦片数据、解析结果                                      │   │
│  │  - 命中率目标: > 40%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ 未命中                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    数据源 (Data Source)                              │   │
│  │  - 本地文件: S57/S101海图文件                                        │   │
│  │  - 远程服务: WMS/WMTS服务                                            │   │
│  │  - 数据库: SQLite/PostGIS                                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.2 缓存淘汰策略

| 缓存级别 | 淘汰算法 | 考虑因素 |
|----------|----------|----------|
| L1热缓存 | LRU-Frequency | 访问时间、访问频率、视口距离 |
| L2内存缓存 | Size-Aware LRU | 访问时间、数据大小、渲染成本 |
| L3磁盘缓存 | Time-based LRU | 访问时间、数据时效、磁盘空间 |

### 7.3 缓存失效机制

| 失效类型 | 触发条件 | 处理方式 |
|----------|----------|----------|
| 时间失效 | 数据超过有效期 | 标记过期，后台刷新 |
| 空间失效 | 视口移动超出范围 | 延迟清理，保留边界数据 |
| 数据失效 | 海图更新通知 | 清除相关缓存，重新加载 |
| 手动失效 | 用户强制刷新 | 清除所有缓存，重新加载 |

---

## 8. 错误处理架构

### 8.1 异常层次结构

```cpp
namespace ogc {

class OgcException : public std::exception {
public:
    explicit OgcException(const std::string& message, ErrorCode code = ErrorCode::Unknown);
    virtual ~OgcException() = default;
    
    ErrorCode GetCode() const { return code_; }
    const std::string& GetMessage() const { return message_; }
    const char* what() const noexcept override { return message_.c_str(); }
    
private:
    std::string message_;
    ErrorCode code_;
};

class SystemException : public OgcException {
public:
    SystemException(const std::string& message, ErrorCode code = ErrorCode::SystemError);
};

class DataException : public OgcException {
public:
    DataException(const std::string& message, ErrorCode code = ErrorCode::DataError);
};

class RenderException : public OgcException {
public:
    RenderException(const std::string& message, ErrorCode code = ErrorCode::RenderError);
};

class ParseException : public OgcException {
public:
    ParseException(const std::string& message, ErrorCode code = ErrorCode::ParseError);
};

class NetworkException : public OgcException {
public:
    NetworkException(const std::string& message, ErrorCode code = ErrorCode::NetworkError);
};

}
```

### 8.2 错误码定义

| 错误码范围 | 类别 | 示例 |
|------------|------|------|
| 1000-1999 | 系统错误 | 内存不足、文件访问失败 |
| 2000-2999 | 数据错误 | 数据格式错误、数据损坏 |
| 3000-3999 | 渲染错误 | 渲染设备失败、着色器错误 |
| 4000-4999 | 解析错误 | 解析失败、格式不支持 |
| 5000-5999 | 网络错误 | 连接超时、服务不可用 |

### 8.3 JNI层错误传递

```java
public class ChartException extends Exception {
    private final int errorCode;
    private final String nativeMessage;
    
    public ChartException(int errorCode, String message) {
        super(message);
        this.errorCode = errorCode;
        this.nativeMessage = message;
    }
    
    public int getErrorCode() { return errorCode; }
    public String getNativeMessage() { return nativeMessage; }
}

public class ChartSystemException extends ChartException { ... }
public class ChartDataException extends ChartException { ... }
public class ChartRenderException extends ChartException { ... }
```

### 8.4 日志记录规范

| 日志级别 | 使用场景 | 示例 |
|----------|----------|------|
| TRACE | 详细调试信息 | 进入/退出函数、变量值 |
| DEBUG | 调试信息 | 关键路径、性能数据 |
| INFO | 正常运行信息 | 加载完成、状态变更 |
| WARN | 警告信息 | 性能下降、资源紧张 |
| ERROR | 错误信息 | 操作失败、异常捕获 |
| FATAL | 致命错误 | 系统崩溃、无法恢复 |

---

## 9. 版本兼容性说明

### 9.1 开发环境要求

| 组件 | 最低版本 | 推荐版本 | 说明 |
|------|----------|----------|------|
| **编译器** | | | |
| MSVC | 2017 | 2022 | Windows平台 |
| GCC | 7.0 | 11.0 | Linux平台 |
| Clang | 6.0 | 14.0 | macOS/iOS平台 |
| **运行时** | | | |
| Java | 11 | 17 LTS | JavaFX/Android |
| C++标准 | C++11 | C++17 | 核心模块 |

### 9.2 依赖库版本

| 依赖库 | 最低版本 | 推荐版本 | 用途 |
|--------|----------|----------|------|
| PROJ | 8.0 | 9.2+ | 坐标转换 |
| GDAL | 3.6 | 3.8+ | 地理数据处理 |
| SQLite | 3.35 | 3.40+ | 本地存储 |
| libspatialite | 5.0 | 5.1+ | 空间扩展 |
| libpq | 14 | 16+ | PostgreSQL连接 |
| GoogleTest | 1.12 | 1.14+ | 单元测试 |

### 9.3 平台版本要求

| 平台 | 最低版本 | 目标版本 | 说明 |
|------|----------|----------|------|
| JavaFX | 17 | 21 LTS | 桌面应用 |
| Android | API 24 (7.0) | API 34 (14) | 移动应用 |
| Android NDK | r21 | r26 | 原生编译 |

### 9.4 兼容性风险

| 风险 | 影响范围 | 缓解措施 |
|------|----------|----------|
| PROJ版本差异 | 坐标转换精度 | 锁定PROJ版本，添加精度测试 |
| Android API碎片化 | 功能可用性 | 运行时检测，降级处理 |
| JavaFX模块化 | 打包部署 | 使用jlink定制运行时 |

---

## 10. 测试策略

### 10.1 测试层次

| 测试层次 | 覆盖范围 | 工具 | 目标覆盖率 |
|----------|----------|------|------------|
| 单元测试 | 单个类/函数 | GoogleTest | > 80% |
| 集成测试 | 模块间交互 | GoogleTest | > 70% |
| 功能测试 | 端到端功能 | 手动/自动化 | 100%用例 |
| 性能测试 | 性能指标 | 自定义工具 | 满足指标 |
| 兼容性测试 | 多平台/多版本 | CI/CD | 全部通过 |

### 10.2 测试用例分类

| 类别 | 测试内容 | 优先级 |
|------|----------|--------|
| **核心功能** | | |
| 海图加载 | S57/S101解析、显示 | P0 |
| 渲染正确性 | 几何、符号、标签渲染 | P0 |
| 交互功能 | 平移、缩放、选择 | P0 |
| **性能测试** | | |
| 渲染性能 | 帧率、响应时间 | P0 |
| 内存使用 | 峰值、泄漏检测 | P0 |
| 加载性能 | 海图加载时间 | P1 |
| **兼容性测试** | | |
| 平台兼容 | JavaFX/Android | P0 |
| 数据兼容 | 多种海图格式 | P1 |
| 版本兼容 | 多版本依赖库 | P2 |

### 10.3 性能测试指标

| 指标 | 测试方法 | 通过标准 |
|------|----------|----------|
| 帧率 | 连续渲染10分钟 | ≥30fps，无卡顿 |
| 内存峰值 | 加载大型海图 | <512MB(Android)，<1GB(桌面) |
| 加载时间 | 冷启动加载 | <5s到可用状态 |
| 响应延迟 | 平移/缩放操作 | <100ms响应 |

### 10.4 性能测试工具

| 工具 | 用途 | 平台 | 使用场景 |
|------|------|------|----------|
| **JMeter** | 接口性能测试 | 跨平台 | REST API压力测试、并发测试 |
| **Android Profiler** | 内存/CPU分析 | Android | 内存泄漏检测、CPU热点分析 |
| **VisualVM** | JVM性能分析 | 桌面 | Java堆分析、线程分析、GC监控 |
| **Valgrind** | 内存检测 | Linux | C++内存泄漏、性能分析 |
| **RenderDoc** | 渲染调试 | 跨平台 | OpenGL ES帧捕获、着色器调试 |
| **Chrome DevTools** | 性能分析 | Web | 帧率分析、内存快照 |
| **自定义帧计数器** | 帧率统计 | 全平台 | 内置帧率监控，实时显示 |

**测试工具集成示例**:
```java
public class PerformanceMonitor {
    private long frameCount = 0;
    private long startTime = 0;
    
    public void onFrameRendered() {
        frameCount++;
        if (startTime == 0) startTime = System.currentTimeMillis();
        
        long elapsed = System.currentTimeMillis() - startTime;
        if (elapsed >= 1000) {
            double fps = frameCount * 1000.0 / elapsed;
            Logger.info("FPS: %.2f", fps);
            frameCount = 0;
            startTime = 0;
        }
    }
}
```

---

## 11. 安全设计

### 11.1 数据安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 数据加密 | AES-256加密存储 | 敏感海图数据 |
| 访问控制 | 基于角色的权限管理 | 多用户环境 |
| 数据校验 | SHA-256完整性校验 | 数据传输/存储 |

### 11.2 传输安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 传输加密 | TLS 1.3 | 远程数据访问 |
| 证书验证 | 双向认证 | 安全通信 |
| 请求签名 | HMAC-SHA256 | API调用 |

### 11.3 内存安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 边界检查 | 安全容器、迭代器 | 数据处理 |
| 内存清理 | 敏感数据清零 | 密钥/密码处理 |
| 防泄漏 | 智能指针、RAII | 全部模块 |

---

## 12. 部署文档

### 12.1 JavaFX应用部署

| 部署方式 | 工具 | 输出 | 适用场景 |
|----------|------|------|----------|
| 独立包 | jlink + jpackage | 安装程序/可执行文件 | 最终用户 |
| JAR包 | Maven Shade | 单一JAR文件 | 开发测试 |
| AppImage | jpackage | AppImage文件 | Linux |

**打包命令示例**:
```bash
jlink --module-path target/modules --add-modules ogc.chart --output runtime
jpackage --type exe --name ChartDisplay --input target --main-jar chart.jar --runtime-image runtime
```

### 12.2 Android应用部署

| 部署方式 | 工具 | 输出 | 适用场景 |
|----------|------|------|----------|
| APK | Gradle | .apk文件 | 开发测试 |
| AAB | Gradle | .aab文件 | 应用商店 |
| 私有部署 | adb | 直接安装 | 企业内部 |

**打包命令示例**:
```bash
./gradlew assembleRelease
./gradlew bundleRelease
```

### 12.3 更新机制

| 更新类型 | 触发方式 | 实现方式 |
|----------|----------|----------|
| 应用更新 | 用户确认 | 应用商店/自更新 |
| 海图更新 | 自动检测 | 增量下载 |
| 配置更新 | 后台同步 | 云端配置 |

### 12.4 回滚策略

| 回滚类型 | 触发条件 | 回滚步骤 | 恢复时间 |
|----------|----------|----------|----------|
| **应用回滚** | | | |
| 版本回退 | 新版本崩溃率>1% | 1. 停止新版本分发<br>2. 发布回退版本<br>3. 用户重新下载 | <1小时 |
| 热修复回滚 | 补丁导致异常 | 1. 禁用问题补丁<br>2. 清除本地缓存<br>3. 重启应用 | <5分钟 |
| **数据回滚** | | | |
| 数据库回滚 | 迁移失败 | 1. 停止服务<br>2. 恢复备份<br>3. 验证数据完整性 | <30分钟 |
| 配置回滚 | 配置错误 | 1. 加载上一版本配置<br>2. 重启服务<br>3. 验证功能 | <5分钟 |
| 海图回滚 | 海图损坏 | 1. 删除损坏海图<br>2. 重新下载<br>3. 验证完整性 | <10分钟 |
| **系统回滚** | | | |
| 完整回滚 | 系统不可用 | 1. 恢复系统镜像<br>2. 恢复数据库<br>3. 恢复配置 | <2小时 |

**回滚检查清单**:
```markdown
□ 备份当前状态（数据库、配置、日志）
□ 记录回滚原因和影响范围
□ 通知相关用户和运维人员
□ 执行回滚操作
□ 验证回滚后系统功能
□ 分析回滚原因，制定预防措施
□ 更新回滚日志
```

---

## 13. 风险评估与缓解

### 13.1 技术风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| JNI内存泄漏 | 中 | 高 | 使用智能指针，定期内存分析，添加泄漏检测工具 |
| 渲染性能不达标 | 中 | 高 | 提前性能测试，准备降级方案，优化关键路径 |
| S52规范理解偏差 | 低 | 中 | 参考官方实现，早期验证，建立测试用例库 |
| 跨平台兼容问题 | 中 | 中 | 持续集成测试，多平台验证，使用抽象层 |
| 依赖库版本冲突 | 低 | 中 | 锁定版本，使用包管理器，隔离依赖 |

### 13.2 项目风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| 工期延误 | 中 | 高 | 预留20%缓冲时间，优先核心功能 |
| 资源不足 | 低 | 高 | 提前规划，建立备份方案 |
| 需求变更 | 中 | 中 | 敏捷开发，迭代交付 |

---

## 14. 实施路线图

### 14.1 总体时间线

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           实施路线图 (20周)                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  第0-2周    第3-5周    第6-8周    第9-12周   第13-16周  第17-20周           │
│  ├──基础───┼──渲染───┼──数据───┼──交互───┼──样式───┼──Android──┤           │
│  │ 框架    │ 核心    │ 处理    │ 功能    │ 符号    │ 迁移      │           │
│  │         │         │         │         │         │           │           │
│  │ 环境准备│ Canvas  │ 海图    │ 平移    │ S52     │ OpenGL ES │           │
│  │ JavaFX  │ 渲染    │ 加载    │ 缩放    │ 样式    │ 渲染      │           │
│  │ 初始化  │ 适配    │ 流程    │ 查询    │ 库      │ 适配      │           │
│  │         │         │         │         │         │           │           │
│  └─────────┴─────────┴─────────┴─────────┴─────────┴───────────┘           │
│                                                                             │
│  里程碑:                                                                    │
│  M0(第2周): 环境准备完成                                                    │
│  M1(第5周): JavaFX框架可运行                                                │
│  M2(第8周): 基本渲染功能完成                                                │
│  M3(第12周): 数据加载+交互功能完成                                          │
│  M4(第16周): S52样式完整                                                    │
│  M5(第20周): Android版本发布                                                │
│                                                                             │
│  风险缓冲: 每阶段预留1周缓冲时间                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 14.2 里程碑定义

| 里程碑 | 时间 | 交付物 | 验收标准 | 风险等级 |
|--------|------|--------|----------|----------|
| M0 | 第2周 | 环境准备 | 开发环境配置完成，依赖库编译通过，CI/CD流水线可用 | 低 |
| M1 | 第5周 | JavaFX框架 | 可运行，显示空白地图，启动时间<3s | 低 |
| M2 | 第8周 | 渲染功能 | 可渲染几何图形，帧率≥30fps，瓦片渲染<50ms | 中 |
| M3 | 第12周 | 数据+交互 | 可加载S57海图，要素显示正确率>95%，平移缩放帧率≥25fps | 中 |
| M4 | 第16周 | 样式完整 | S52样式覆盖率>90%，显示模式切换<500ms，符号渲染正确 | 高 |
| M5 | 第20周 | Android版 | 功能完整度100%，帧率≥30fps，内存峰值<512MB | 高 |

### 14.3 风险应对计划

| 风险 | 概率 | 影响 | 应对措施 |
|------|------|------|----------|
| S52样式库复杂度超预期 | 高 | 高 | 提前调研S52规范，预留2周缓冲，考虑分阶段实现 |
| Android OpenGL ES适配困难 | 中 | 高 | 优先完成JavaFX版本验证，积累渲染经验后再迁移 |
| JNI桥接性能问题 | 中 | 中 | 使用Native Buffer优化，减少跨层调用 |
| 内存占用过高 | 中 | 中 | 实施内存监控，优化缓存策略，及时释放资源 |
| 第三方库版本冲突 | 低 | 中 | 锁定依赖版本，使用vcpkg管理 |

### 14.4 并行开发规划

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           并行开发任务                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  阶段3-5周 (渲染核心):                                                      │
│  ├── 任务A: Canvas渲染适配 (开发人员1)                                      │
│  ├── 任务B: 绘图引擎集成 (开发人员2)                                        │
│  └── 任务C: 瓦片管理器 (开发人员3)                                          │
│                                                                             │
│  阶段6-8周 (数据处理):                                                      │
│  ├── 任务A: S57解析集成 (开发人员1)                                         │
│  ├── 任务B: 要素查询接口 (开发人员2)                                        │
│  └── 任务C: 缓存策略实现 (开发人员3)                                        │
│                                                                             │
│  阶段9-16周 (交互+样式):                                                    │
│  ├── 任务A: 交互功能 (开发人员1)                                            │
│  ├── 任务B: S52样式库 (开发人员2)                                           │
│  └── 任务C: 符号渲染 (开发人员3)                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 15. 附录

### 15.1 参考文档

| 文档 | 路径 | 说明 |
|------|------|------|
| 模块索引 | code/index_all.md | 所有模块的统一入口 |
| 几何模块 | code/geom/include/index_geom.md | 几何对象模型 |
| 数据库模块 | code/database/include/index_database.md | 数据库抽象层 |
| 渲染模块 | code/graph/include/index_graph.md | 地图渲染核心 |
| 解析模块 | code/chart/parser/include/index_parser.md | 海图解析器 |

### 15.2 术语表

| 术语 | 英文 | 说明 |
|------|------|------|
| CDS | Chart Display System | 海图显示系统 |
| S52 | IHO S-52 | 海图显示规范 |
| S57 | IHO S-57 | 电子海图数据标准 |
| S101 | IHO S-101 | 新一代电子海图标准 |
| JNI | Java Native Interface | Java本地接口 |
| LOD | Level of Detail | 多级细节 |
| PAL | Platform Abstraction Layer | 平台抽象层 |

### 15.2 技术栈清单

| 分类 | 技术 | 版本 | 用途 |
|------|------|------|------|
| 核心语言 | C++ | 11 | 核心业务逻辑 |
| 核心语言 | Java | 11+ | Android/JavaFX应用 |
| 核心语言 | Kotlin | 1.6+ | Android应用(可选) |
| 构建工具 | CMake | 3.16+ | C++构建 |
| 构建工具 | Gradle | 7.0+ | Android构建 |
| 渲染框架 | OpenGL ES | 3.0 | Android渲染 |
| 渲染框架 | JavaFX | 11+ | 桌面应用 |
| 数据解析 | GDAL | 3.4+ | S57/S101解析 |
| 坐标转换 | PROJ | 8.0+ | 坐标系转换 |
| 本地存储 | SQLite | 3.35+ | 离线数据存储 |
| 空间扩展 | SpatiaLite | 5.0+ | 空间索引 |

---

**变更说明**: 
- v1.4 (2026-04-08): 修正alarm依赖alert模块关系，明确警报核心层与服务层分离
- v1.3 (2026-04-08): 补充alarm模块到依赖关系图，修正parser依赖链，完善JavaFX架构图和适配器设计，补充插件生命周期管理和动态加载机制，调整实施路线图时间估算并增加风险应对计划
- v1.2 (2026-04-08): 量化里程碑验收标准，添加性能测试工具说明，添加回滚策略
- v1.1 (2026-04-08): 修复架构优先级矛盾，添加性能指标、错误处理架构、平台抽象层、插件架构、缓存策略、版本兼容性、测试策略、安全设计、部署文档等章节
- v1.0 (2026-04-06): 初始版本
