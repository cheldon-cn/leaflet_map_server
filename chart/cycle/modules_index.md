# 模块列表

> **更新日期**: 2026-04-13  
> **项目**: Cycle Chart Application v1.0.0

---
## ⚠️ 前置说明

> **重要**: 在获取各模块详细信息之前，必须先了解以下内容：
> 
> 1. **编译与安装目录** - 了解项目的构建输出结构和安装路径配置
> 2. **构建说明** - 确认所需构建环境和依赖
> 3. **模块依赖关系** - 理解模块间的依赖层级，确保编译顺序正确
>
> 请先阅读以下章节，再查阅各模块详情。

---

## 模块概览

| 模块 | 路径 | 语言 | 说明 |
|------|------|------|------|
| chart_c_api | code/cycle/chart_c_api | C++ | C API：跨语言接口，封装底层C++模块 |
| jni | cycle/jni | C++/Java | JNI本地接口：Java与C++桥接层 |
| javawrapper | cycle/javawrapper | Java | Java封装层：提供高层Java API |
| fxribbon | cycle/fxribbon | Java | JavaFX Ribbon组件：功能区UI控件 |
| javafx-app | cycle/javafx-app | Java | JavaFX应用程序：主应用程序 |
| doc | cycle/doc | - | 文档：设计文档、问题记录 |

---

## 模块依赖关系

```
chart_c_api (依赖code下所有C++模块)
  ↓
jni (依赖chart_c_api)
  ↓
javawrapper (依赖jni)
  ↓
fxribbon (独立UI组件)
  ↓
javafx-app (依赖javawrapper, fxribbon)
```

---

## 编译与安装目录

> **基准目录**: 本文档中所有路径均相对于**项目根目录**（即 `e:\program\trae\chart/`）

### 目录结构

| 目录类型 | 路径 | 说明 |
|----------|------|------|
| 编译根目录 | `build/` | CMake/Gradle构建输出目录 |
| 动态库/测试 | `build/test/` | `.dll`、`.exe`、`.jar` 文件 |
| 静态库/导入库 | `build/lib/` | `.lib` 文件 |
| 安装根目录 | `install/` | 安装输出目录 |
| 头文件 | `install/include/ogc/` | 各模块头文件 |
| 动态库/可执行 | `install/bin/` | 运行时文件 |
| Java构建目录 | `build/cycle/` | Gradle构建输出 |

### 各模块编译产物

| 模块 | 编译产物 | 输出目录 | 头文件目录 |
|------|----------|----------|------------|
| chart_c_api | ogc_chart_c_api.dll/lib | build/test, build/lib | install/include/ogc/capi/ |
| jni | ogc_chart_jni.dll | build/test | - |
| javawrapper | javawrapper-[VERSION].jar | build/test | - |
| fxribbon | fxribbon-[VERSION].jar | build/test | - |
| javafx-app | javafx-app-[VERSION].jar | build/test | - |

---

## 模块详情

### chart_c_api 模块

**路径**: `code/cycle/chart_c_api`

**说明**: C API跨语言接口，封装底层C++模块，提供统一的C语言接口供Java等其他语言调用。

**依赖模块**:
- ogc_base
- ogc_geometry
- ogc_feature
- ogc_layer
- ogc_draw
- ogc_graph
- ogc_cache
- ogc_symbology
- ogc_alert
- ogc_navi
- ogc_proj
- chart_parser

**主要文件**:
| 文件 | 说明 |
|------|------|
| sdk_c_api.h | 主头文件，定义所有C API |
| sdk_c_api_export.h | 导出宏定义 |
| sdk_c_api_common.cpp | 通用功能实现 |
| sdk_c_api_geom.cpp | 几何类型API |
| sdk_c_api_feature.cpp | 要素API |
| sdk_c_api_layer.cpp | 图层API |
| sdk_c_api_cache.cpp | 缓存API |
| sdk_c_api_alert.cpp | 预警API |
| sdk_c_api_navi.cpp | 导航API |
| sdk_c_api_draw.cpp | 绘制API |
| sdk_c_api_graph.cpp | 图形API |
| sdk_c_api_proj.cpp | 投影API |
| sdk_c_api_parser.cpp | 解析器API |
| sdk_c_api_plugin.cpp | 插件API |

**测试文件**:
| 测试文件 | 说明 |
|----------|------|
| test_sdk_c_api_common.cpp | 通用功能测试 |
| test_sdk_c_api_geom.cpp | 几何类型测试 |
| test_sdk_c_api_feature.cpp | 要素测试 |
| test_sdk_c_api_layer.cpp | 图层测试 |
| test_sdk_c_api_cache.cpp | 缓存测试 |
| test_sdk_c_api_alert.cpp | 预警测试 |
| test_sdk_c_api_navigation.cpp | 导航测试 |

---

### jni 模块

**路径**: `cycle/jni`

**说明**: JNI本地接口，实现Java与C++的桥接，通过JNI调用chart_c_api模块。

**依赖模块**:
- chart_c_api
- Java JDK

**主要文件**:
| 文件 | 说明 |
|------|------|
| jni_env_manager.cpp | JNI环境管理 |
| jni_converter.cpp | 类型转换器 |
| jni_exception.cpp | 异常处理 |
| jni_reference_cache.cpp | 引用缓存 |
| jni_onload.cpp | JNI初始化 |
| native/chart_viewer_jni.cpp | ChartViewer JNI实现 |
| native/viewport_jni.cpp | Viewport JNI实现 |
| native/geometry_jni.cpp | Geometry JNI实现 |
| native/feature_jni.cpp | Feature JNI实现 |
| native/layer_jni.cpp | Layer JNI实现 |
| native/chart_config_jni.cpp | ChartConfig JNI实现 |
| native/adapter_jni.cpp | 适配器JNI实现 |

**编译要求**:
- 需要设置 `JAVA_HOME` 环境变量

---

### javawrapper 模块

**路径**: `cycle/javawrapper`

**说明**: Java封装层，提供高层Java API，封装JNI调用细节。

**主要包结构**:
```
cn.cycle.chart
├── jni/                    # JNI底层封装
│   ├── NativeObject.java
│   ├── NativeHandle.java
│   └── JniBridge.java
├── api/
│   ├── core/               # 核心类
│   │   ├── ChartViewer.java
│   │   ├── Viewport.java
│   │   └── ChartConfig.java
│   ├── geometry/           # 几何类
│   │   ├── Geometry.java
│   │   ├── Coordinate.java
│   │   ├── Envelope.java
│   │   ├── Point.java
│   │   ├── LineString.java
│   │   ├── LinearRing.java
│   │   └── Polygon.java
│   ├── feature/            # 要素类
│   │   ├── FeatureInfo.java
│   │   ├── FieldDefn.java
│   │   └── FieldValue.java
│   ├── layer/              # 图层类
│   │   ├── Layer.java
│   │   ├── VectorLayer.java
│   │   └── LayerManager.java
│   ├── cache/              # 缓存类
│   │   ├── TileCache.java
│   │   └── TileKey.java
│   ├── alert/              # 预警类
│   │   ├── Alert.java
│   │   └── AlertEngine.java
│   ├── navi/               # 导航类
│   │   ├── NavigationEngine.java
│   │   ├── AisManager.java
│   │   ├── Route.java
│   │   └── Waypoint.java
│   ├── symbology/          # 符号化类
│   │   └── Symbolizer.java
│   ├── style/              # 样式类
│   │   ├── Color.java
│   │   └── DrawStyle.java
│   ├── adapter/            # 适配器类
│   │   ├── CanvasAdapter.java
│   │   ├── ImageDevice.java
│   │   ├── PanHandler.java
│   │   ├── ZoomHandler.java
│   │   ├── ChartEventHandler.java
│   │   ├── ChartMouseEvent.java
│   │   ├── ChartKeyEvent.java
│   │   └── ChartScrollEvent.java
│   ├── plugin/             # 插件类
│   │   ├── PluginManager.java
│   │   └── ChartPlugin.java
│   └── util/               # 工具类
│       └── Transform.java
```

---

### fxribbon 模块

**路径**: `cycle/fxribbon`

**说明**: JavaFX Ribbon组件，提供功能区UI控件，类似于Office Ribbon界面。

**主要类**:
| 类名 | 说明 |
|------|------|
| Ribbon | 主功能区控件 |
| RibbonTab | 功能区标签页 |
| RibbonGroup | 功能区分组 |
| RibbonItem | 功能区项 |
| QuickAccessBar | 快速访问工具栏 |
| Column | 列布局 |

**依赖**:
- JavaFX 11

---

### javafx-app 模块

**路径**: `cycle/javafx-app`

**说明**: JavaFX主应用程序，整合所有模块，提供完整的海图应用界面。

**主要包结构**:
```
cn.cycle.app
├── MainApp.java            # 应用入口
├── accessibility/          # 无障碍支持
├── context/                # 应用上下文
├── controller/             # 控制器
├── dialog/                 # 对话框
├── dpi/                    # DPI缩放
├── event/                  # 事件系统
├── exception/              # 异常处理
├── focus/                  # 焦点管理
├── fxml/                   # FXML加载
├── i18n/                   # 国际化
├── layout/                 # 布局管理
├── lifecycle/              # 生命周期
├── loader/                 # 数据加载
├── loading/                # 加载状态
├── memory/                 # 内存管理
├── model/                  # 数据模型
├── panel/                  # 面板组件
├── prefs/                  # 用户偏好
├── render/                 # 渲染模块
├── shortcut/               # 快捷键
├── sidebar/                # 侧边栏
├── splash/                 # 启动画面
├── state/                  # 状态管理
├── status/                 # 状态栏
├── theme/                  # 主题管理
├── util/                   # 工具类
└── view/                   # 视图组件
```

**主要视图**:
| 视图 | 说明 |
|------|------|
| MainView | 主视图 |
| ChartCanvas | 海图画布 |
| ImageCanvas | 图像画布 |
| LayerPanel | 图层面板 |
| AlertPanel | 预警面板 |
| PositionView | 位置视图 |
| SettingsView | 设置视图 |

**侧边栏面板**:
| 面板 | 说明 |
|------|------|
| DataCatalogPanel | 数据目录面板 |
| QueryPanel | 查询面板 |
| StylePanel | 样式面板 |
| DataConvertPanel | 数据转换面板 |

**测试文件**:
| 测试文件 | 说明 |
|----------|------|
| AppEventTest.java | 事件测试 |
| AppEventBusTest.java | 事件总线测试 |
| I18nManagerTest.java | 国际化测试 |
| EventSystemIntegrationTest.java | 事件系统集成测试 |
| SideBarIntegrationTest.java | 侧边栏集成测试 |
| ThemeI18nIntegrationTest.java | 主题国际化集成测试 |
| AbstractLifecycleComponentTest.java | 生命周期组件测试 |

---

### doc 模块

**路径**: `cycle/doc`

**说明**: 文档目录，包含设计文档、问题记录等。

**文档列表**:
| 文档 | 说明 |
|------|------|
| app_layout_design.md | 应用布局设计 |
| app_layout_run_tasks.md | 应用布局任务 |
| app_ribbon_design.md | Ribbon设计 |
| chart_view_pan_zoom_intout.md | 海图平移缩放 |
| dll_exports.txt | DLL导出列表 |
| dll_exports_full.txt | DLL完整导出列表 |
| javafx_app_design.md | JavaFX应用设计 |
| javafx_chart_application_design.md | JavaFX海图应用设计 |
| jni_java_interface.md | JNI Java接口 |
| problem_record_0_glm51.md | 问题记录 |
| problem_record_glm51.md | 问题记录 |

---

## 构建说明

### C++模块构建 (CMake)

```bash
# 配置 (在项目根目录)
cmake -B build -S code -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release

# 安装
cmake --install build
```

### Java模块构建 (Gradle)

**Gradle配置**: 项目使用Gradle Wrapper，配置文件位于 `cycle/gradle/wrapper/gradle-wrapper.properties`

```properties
distributionUrl=file:///f:/enc/chart/gradle/gradle-4.5.1-all.zip
```

```bash
# 进入cycle目录
cd cycle

# 方式1: 使用Gradle Wrapper (需要先生成gradlew脚本)
# gradlew build

# 方式2: 直接使用本地Gradle
../gradle/gradle-4.5.1/bin/gradle.bat build

# 运行应用
../gradle/gradle-4.5.1/bin/gradle.bat :javafx-app:run

# 安装JAR到install目录
../gradle/gradle-4.5.1/bin/gradle.bat install
```

### 环境要求

| 环境 | 要求 |
|------|------|
| JDK | 1.8+ (推荐使用 `F:\enc\java`) |
| JavaFX | 11+ |
| JAVA_HOME | 必须设置 (`F:\enc\java`) |
| CMake | 3.10+ |
| C++编译器 | VS2015+ / GCC 7+ |
| Gradle | 4.5.1 (位于 `gradle/gradle-4.5.1/`) |
| Gradle Wrapper | 配置使用本地 `gradle/gradle-4.5.1-all.zip` |

---

## 线程安全说明

| 组件 | 线程安全 | 说明 |
|------|----------|------|
| ChartViewer | 不安全 | 必须在单线程中操作 |
| Viewport | 不安全 | 需要外部同步 |
| LayerManager | 读安全 | 读操作线程安全 |
| TileCache | 安全 | 内部同步 |
| AlertEngine | 安全 | 内部互斥锁 |
| NavigationEngine | 不安全 | 必须在单线程中操作 |
| PositionManager | 安全 | 内部互斥锁 |
| RouteManager | 不安全 | 需要外部同步 |
| Logger | 安全 | 内部同步 |
| PerformanceMonitor | 安全 | 原子操作 |

---

**版本**: v1.0.0
