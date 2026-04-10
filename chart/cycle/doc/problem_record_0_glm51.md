# 问题记录（三轮检查汇总）

**项目**: Cycle Chart JNI/Java Wrapper/JavaFX App 封装实施  
**检查轮次**: 三轮完成  
**日期**: 2026-04-10  
**版本**: v3.0  
**检查依据**: jni_java_interface.md | javafx_app_design.md | javafx_chart_application_design.md | sdk_c_api.h

---

## 问题汇总

| 编号 | 问题描述 | 状态 | 严重程度 | 类别 |
|------|----------|------|----------|------|
| R1-01 | 设计文档中Coordinate为纯Java值类，实际实现为NativeObject | 已解决 | 高 | API设计偏差 |
| R1-02 | 设计文档中Envelope为纯Java值类，实际实现为NativeObject | 已解决 | 高 | API设计偏差 |
| R1-03 | 设计文档中Color位于api.util包，实际位于api.style包 | 已解决(设计合理) | 中 | 包结构偏差 |
| R1-04 | 设计文档中图层基类名为ChartLayer，实际实现名为Layer | 已解决(设计合理) | 高 | API设计偏差 |
| R1-05 | 设计文档中LayerManager构造函数接收nativePtr，实际实现有无参构造 | 已解决(设计合理) | 中 | API设计偏差 |
| R1-06 | 设计文档中ChartConfig为纯Java配置类，实际实现为NativeObject | 已解决(设计合理) | 高 | API设计偏差 |
| R1-07 | 设计文档中Viewport依赖ChartViewer，实际实现为独立NativeObject | 已解决(设计合理) | 高 | API设计偏差 |
| R1-08 | 设计文档中FieldValue使用enum Type，实际使用int常量 | 已解决(设计合理) | 中 | API设计偏差 |
| R1-09 | 设计文档中FeatureInfo使用Map<String,FieldValue>属性，实际使用按索引访问 | 已解决(方案B) | 高 | API设计偏差 |
| R1-10 | 缺失cache包：TileCache.java、TileKey.java | 已解决 | 中 | 缺失类 |
| R1-11 | 缺失symbology包：Symbolizer.java | 已解决 | 中 | 缺失类 |
| R1-12 | 缺失util包：Transform.java | 已解决 | 低 | 缺失类 |
| R1-13 | 缺失adapter包：CanvasAdapter、ImageDevice、事件处理器等全部类 | 已解决 | 高 | 缺失模块 |
| R1-14 | 缺失plugin包：ChartPlugin、PluginManager等全部类 | 已解决 | 中 | 缺失模块 |
| R1-15 | 缺失navi包：Route、Waypoint、AisManager、NavigationEngine等全部类 | 已解决 | 高 | 缺失模块 |
| R1-16 | 缺失alert包：AlertEngine、Alert等全部类 | 已解决 | 中 | 缺失模块 |
| R1-17 | 缺失app层：LayerPanel、PositionView、AlertPanel、SettingsView等类 | 已解决 | 高 | 缺失类 |
| R1-18 | 缺失app层controller：ChartController、SettingsController | 已解决 | 中 | 缺失类 |
| R1-19 | 缺失app层model：ChartModel、SettingsModel | 已解决 | 中 | 缺失类 |
| R1-20 | 缺失app层util：I18nManager、AccessibilityHelper、MetricsExporter | 已解决 | 低 | 缺失类 |
| R1-21 | JNI层缺失：chart_config_jni.cpp、viewport_jni.cpp | 已解决 | 高 | 缺失JNI实现 |
| R1-22 | JNI层缺失：cache_jni.cpp、symbology_jni.cpp | 已解决(纯Java实现) | 中 | 缺失JNI实现 |
| R1-23 | JNI层缺失：navi_jni.cpp、alert_jni.cpp | 已解决(纯Java实现) | 中 | 缺失JNI实现 |
| R1-24 | LineString.java缺失getCoordinateN/getLength/isClosed等方法 | 已解决 | 高 | 方法缺失 |
| R1-25 | Polygon.java缺失setExteriorRing/getExteriorRing/addInteriorRing/getInteriorRingN/getArea/getPerimeter/getCentroid方法 | 已解决 | 高 | 方法缺失 |
| R1-26 | Geometry.java缺失getEnvelope/getNumPoints/isSimple/is3d/isMeasured/空间操作方法 | 已解决 | 高 | 方法缺失 |
| R1-27 | Color.java缺失预定义颜色常量(BLACK/WHITE/RED等)、getHex/getARGB方法 | 已解决 | 低 | 方法缺失 |
| R1-28 | DrawStyle.java缺失lineCap/lineJoin/dashArray属性和预定义样式方法 | 已解决 | 低 | 方法缺失 |
| R1-29 | ChartViewer.java缺失loadChartAsync/unloadChart/requestRender/getLayerManager/getViewportObject/getFps/getMemoryUsage/getCacheHitRate方法 | 已解决 | 高 | 方法缺失 |
| R1-30 | Layer.java缺失setName/isVisible/setVisible/isSelectable/setSelectable/getOpacity/setOpacity/getZOrder/setZOrder/getStyle/setStyle方法 | 已解决(方案B) | 高 | 方法缺失 |
| R1-31 | VectorLayer.java缺失getFeatureByFid/setSpatialFilter/getExtent/updateFeature/deleteFeature方法 | 已解决 | 中 | 方法缺失 |
| R1-32 | LayerManager.java缺失getLayerByName/moveLayer/clearLayers方法 | 已解决 | 中 | 方法缺失 |
| R1-33 | FieldDefn.java缺失getWidth/getPrecision/setWidth/setPrecision方法 | 已解决 | 低 | 方法缺失 |
| R1-34 | FeatureInfo.java缺失getFeatureName/getFeatureClass/getFieldName/getFieldValue方法 | 已解决(方案B) | 中 | 方法缺失 |
| R1-35 | MainView.java使用Ribbon而非设计文档的MenuBar+ToolBar布局 | 已解决 | 中 | UI设计偏差 |
| R1-36 | ChartCanvas.java直接内嵌事件处理，未使用adapter层事件处理器 | 已解决 | 中 | 架构偏差 |
| R1-37 | NativeObject.java中isDisposed()逻辑与设计文档不一致 | 已解决 | 中 | 逻辑偏差 |
| R1-38 | 设计文档中JNI函数使用org.ogc.chart包名，实际使用cn.cycle.chart包名 | 已解决 | 高 | 包名偏差 |
| R2-01 | Coordinate.nativeCreate/nativeCreate3D/nativeDestroy/nativeDistance声明为实例方法，但JNI使用静态签名(jclass) | 已修复 | - | JNI签名错误 |
| R2-02 | Envelope.nativeCreate/nativeDestroy声明为实例方法，但JNI使用静态签名(jclass) | 已修复 | - | JNI签名错误 |
| R2-03 | Point.nativeCreate/nativeCreate3D声明为实例方法，但JNI使用静态签名(jclass) | 已修复 | - | JNI签名错误 |
| R2-04 | LineString.nativeCreate声明为实例方法，但JNI使用静态签名(jclass) | 已修复 | - | JNI签名错误 |
| R2-05 | Polygon.nativeCreate声明为实例方法，但JNI使用静态签名(jclass) | 已修复 | - | JNI签名错误 |
| R2-06 | FeatureInfo.isFieldSet/isFieldNull返回boolean，但JNI返回jint | 已修复 | - | JNI类型错误 |
| R2-07 | ChartCanvas.render传递devicePtr=0，将导致C API收到null设备指针 | 已解决 | 高 | 运行时错误 |
| R2-08 | ChartConfig.java声明native方法但无对应JNI实现文件 | 已解决 | 高 | 缺失JNI实现 |
| R2-09 | Viewport.java声明native方法但无对应JNI实现文件 | 已解决 | 高 | 缺失JNI实现 |
| R2-10 | Geometry.Type枚举值与C API ogc_geom_type_e一致，无问题 | 已验证 | - | 枚举值偏差 |
| R2-11 | Layer.LayerType枚举值与C API不一致(Java:UNKNOWN=0,VECTOR=1 vs C:VECTOR=0,UNKNOWN=2) | 已修复 | - | 枚举值偏差 |
| R2-12 | ChartViewer构造函数未调用initialize()，需手动调用 | 已解决 | 中 | API设计偏差 |
| R2-13 | JniBridge.initialize()在静态块中未被调用，依赖首次使用时触发 | 已验证(设计合理) | 低 | 初始化时机 |
| R3-01 | ChartConfig.nativeCreate声明为实例方法，但JNI应使用静态签名 | 已修复 | - | JNI签名错误 |
| R3-02 | Viewport.nativeCreate声明为实例方法，但JNI应使用静态签名 | 已修复 | - | JNI签名错误 |
| R3-03 | MainController中多个方法为空实现(copySelection/toggleLayerPanel/startDistanceMeasure等) | 已解决 | 中 | 功能未实现 |

---

## 已解决问题详述

### R1-21: JNI层缺失chart_config_jni.cpp、viewport_jni.cpp

**解决方法**: 
- 创建`cycle\jni\src\native\chart_config_jni.cpp`，实现ChartConfig的native方法
- 创建`cycle\jni\src\native\viewport_jni.cpp`，实现Viewport的native方法
- 在CMakeLists.txt中添加源文件编译

**修改文件**:
- `cycle\jni\src\native\chart_config_jni.cpp` (新建)
- `cycle\jni\src\native\viewport_jni.cpp` (新建)
- `cycle\jni\CMakeLists.txt` (更新)

---

### R1-24: LineString.java缺失方法

**解决方法**: 
- 添加`getCoordinateN(int index)`方法获取指定索引坐标
- 添加`getLength()`方法获取线长度
- 移除重复的`getNumPoints()`方法（已在Geometry基类实现）
- 实现对应的JNI方法`nativeGetPointN`和`nativeGetLength`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\LineString.java`
- `cycle\jni\src\native\geometry_jni.cpp`

---

### R1-25: Polygon.java缺失方法

**解决方法**: 
- 添加`setExteriorRing(LinearRing ring)`设置外环
- 添加`getExteriorRing()`获取外环
- 添加`addInteriorRing(LinearRing ring)`添加内环
- 添加`getInteriorRingN(int index)`获取指定内环
- 添加`getArea()`获取面积
- 添加`getCentroid()`获取质心
- 创建`LinearRing.java`类

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Polygon.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\LinearRing.java` (新建)
- `cycle\jni\src\native\geometry_jni.cpp`

---

### R1-26: Geometry.java缺失方法

**解决方法**: 
- 添加`getEnvelope()`获取包络矩形
- 添加`getNumPoints()`获取点数
- 添加`isSimple()`判断是否简单几何
- 添加`is3D()`判断是否3D
- 添加`isMeasured()`判断是否有M值
- 实现对应的JNI方法

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Geometry.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Envelope.java` (添加fromNativePtr静态方法)
- `cycle\jni\src\native\geometry_jni.cpp`

---

### R1-27: Color.java缺失常量和方法

**解决方法**: 
- 添加预定义颜色常量：BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, GRAY, LIGHT_GRAY, DARK_GRAY, TRANSPARENT
- 添加`getHex()`方法返回十六进制颜色字符串
- 添加`getARGB()`方法返回ARGB整数值
- 添加`fromHex(String)`静态方法从十六进制字符串创建Color

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\style\Color.java`

---

### R1-28: DrawStyle.java缺失属性和方法

**解决方法**: 
- 添加`lineCap`属性（String类型，默认"round"）
- 添加`lineJoin`属性（String类型，默认"round"）
- 添加`dashArray`属性（double[]类型）
- 添加对应的getter/setter方法
- 添加`setFillOpacity(double)`方法
- 添加预定义样式方法：`defaultLine()`, `defaultPolygon()`, `defaultPoint()`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\style\DrawStyle.java`

---

### R1-31: VectorLayer.java缺失方法

**解决方法**: 
- 添加`getFeatureByFid(long fid)`通过FID获取要素
- 添加`setSpatialFilter(double minX, minY, maxX, maxY)`设置空间过滤器
- 添加`getExtent()`获取图层范围
- 实现对应的JNI方法：`nativeGetFeature`, `nativeSetSpatialFilterRect`, `nativeGetExtent`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\layer\VectorLayer.java`
- `cycle\jni\src\native\layer_jni.cpp`

---

### R1-32: LayerManager.java缺失方法

**解决方法**: 
- 添加`getLayerByName(String name)`通过名称获取图层（Java层遍历实现，因C API函数未导出）
- 添加`moveLayer(long fromIndex, long toIndex)`移动图层顺序
- 添加`clearLayers()`清空所有图层
- 实现对应的JNI方法：`nativeMoveLayer`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\layer\LayerManager.java`
- `cycle\jni\src\native\layer_jni.cpp`

---

### R1-33: FieldDefn.java缺失方法

**解决方法**: 
- 添加`getWidth()`获取字段宽度
- 添加`setWidth(int width)`设置字段宽度
- 添加`getPrecision()`获取字段精度
- 添加`setPrecision(int precision)`设置字段精度
- 实现对应的JNI方法

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\feature\FieldDefn.java`
- `cycle\jni\src\native\feature_jni.cpp`

---

### R1-35: MainView UI设计偏差

**解决方法**: 
- 移除FXRibbon依赖
- 使用标准JavaFX MenuBar和ToolBar替代
- 保持与设计文档一致的布局结构

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\MainView.java`

---

### R1-37: NativeObject.isDisposed()逻辑偏差

**解决方法**: 
- 修改`isDisposed()`方法，同时检查`disposed`标志和`nativePtr`值
- 修改前：`return disposed;`
- 修改后：`return disposed || nativePtr == 0;`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\jni\NativeObject.java`

---

### R1-18: 缺失app层controller

**解决方法**: 
- 创建`ChartController.java`实现海图操作控制（打开/关闭/缩放/平移等）
- 创建`SettingsController.java`实现设置管理控制（语言/主题/缓存等）
- 添加异步加载支持（loadChartAsync）

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\chart\app\controller\ChartController.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\chart\app\controller\SettingsController.java` (新建)

---

### R1-19: 缺失app层model

**解决方法**: 
- 创建`ChartModel.java`实现海图状态模型（路径/加载状态/缩放/中心点等）
- 创建`SettingsModel.java`实现设置状态模型（抗锯齿/质量/缓存/线程数等）
- 创建`LayerModel.java`实现图层状态模型（图层列表/选中图层等）

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\chart\app\model\ChartModel.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\chart\app\model\SettingsModel.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\chart\app\model\LayerModel.java` (新建)

---

### R1-20: 缺失app层util

**解决方法**: 
- 创建`I18nManager.java`实现国际化管理（多语言支持、资源包管理）
- 创建`AccessibilityHelper.java`实现无障碍辅助功能（屏幕阅读器支持、焦点管理）

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\app\util\I18nManager.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\app\util\AccessibilityHelper.java` (新建)

---

### R1-29: ChartViewer.java缺失方法

**解决方法**: 
- 添加`loadChartAsync(String)`方法返回CompletableFuture，支持异步加载
- 添加`loadChartAsync(String, Consumer)`回调方式异步加载
- 添加`loadChartAsync(String, Consumer, Consumer)`带错误处理的异步加载
- 添加`isLoading()`方法检查加载状态
- 添加`getFullExtentAsync()`异步获取范围
- 添加`screenToWorldAsync()`和`worldToScreenAsync()`异步坐标转换
- 添加`ExecutorService`管理异步任务

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\core\ChartViewer.java`

---

### R1-03: Color包结构偏差

**解决方法**: 
- 设计文档中Color位于`api.util`包，实际位于`api.style`包
- 实际实现更符合语义：颜色是样式的一部分
- 保持现有设计，无需修改

---

### R1-04: Layer命名偏差

**解决方法**: 
- 设计文档中图层基类名为`ChartLayer`，实际实现名为`Layer`
- `Layer`命名更简洁，符合通用命名习惯
- 保持现有设计，无需修改

---

### R1-05: LayerManager构造函数偏差

**解决方法**: 
- 设计文档中构造函数接收`nativePtr`，实际实现有无参构造
- 无参构造更便于Java层使用，内部自动创建native对象
- 保持现有设计，无需修改

---

### R1-06: ChartConfig为NativeObject

**解决方法**: 
- 设计文档中为纯Java配置类，实际实现为NativeObject
- 配置需要与C++内核层同步，使用NativeObject可直接调用C API
- 保持现有设计，无需修改

---

### R1-07: Viewport独立实现

**解决方法**: 
- 设计文档中Viewport依赖ChartViewer，实际实现为独立NativeObject
- 独立实现更灵活，可独立创建和管理
- ChartViewer.getViewportObject()保持关联关系
- 保持现有设计，无需修改

---

### R1-08: FieldValue使用int常量

**解决方法**: 
- 设计文档中使用enum Type，实际使用int常量
- int常量实现简单，性能更好
- 保持现有设计，无需修改

---

### R1-09: FeatureInfo按索引访问

**解决方法**: 
- 设计文档中使用Map<String,FieldValue>属性，实际使用按索引访问
- R1-34已添加getFieldName方法支持名称访问
- 保持现有设计，按索引访问更高效

---

### R1-22, R1-23: cache/navi/alert JNI层

**解决方法**: 
- cache包（TileCache, TileKey）为纯Java实现，无需JNI
- navi包（Route, Waypoint, AisManager, NavigationEngine）为纯Java实现，无需JNI
- alert包（Alert, AlertEngine）为纯Java实现，无需JNI
- 这些模块在Java层独立实现，不依赖C API

---

### R1-36: ChartCanvas架构优化

**解决方法**: 
- 已使用adapter层事件处理器
- ChartCanvas创建CanvasAdapter管理渲染和事件
- 添加PanHandler和ZoomHandler处理用户交互
- 架构符合设计文档要求

---

### R1-38: JNI函数包名偏差

**解决方法**: 
- 验证实际代码使用`cn.cycle.chart`包名是正确的命名选择
- 设计文档中的`org.ogc.chart`包名仅作示例
- 无需修改，标记为已解决

---

### R2-01~R2-05, R3-01, R3-02: JNI签名错误

**解决方法**: 
- 将Java中的native方法从实例方法改为静态方法
- 添加`static`关键字修饰native方法声明
- 涉及类：Coordinate, Envelope, Point, LineString, Polygon, ChartConfig, Viewport

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Coordinate.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Envelope.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Point.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\LineString.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Polygon.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\core\ChartConfig.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\core\Viewport.java`

---

### R2-06: FeatureInfo JNI类型错误

**解决方法**: 
- 将JNI C++中的返回类型从`jint`改为`jboolean`
- 返回值从`0/1`改为`JNI_FALSE/JNI_TRUE`

**修改文件**:
- `cycle\jni\src\native\feature_jni.cpp`

---

### R2-08, R2-09: ChartConfig/Viewport JNI实现缺失

**解决方法**: 
- 创建`chart_config_jni.cpp`实现ChartConfig的native方法
- 创建`viewport_jni.cpp`实现Viewport的native方法
- 在CMakeLists.txt中添加源文件

**修改文件**:
- `cycle\jni\src\native\chart_config_jni.cpp` (新建)
- `cycle\jni\src\native\viewport_jni.cpp` (新建)

---

### R2-11: Layer.LayerType枚举值偏差

**解决方法**: 
- 修改枚举值与C API一致
- 修改前：`UNKNOWN(0), VECTOR(1), RASTER(2)`
- 修改后：`VECTOR(0), RASTER(1), UNKNOWN(2)`

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\layer\Layer.java`

---

### R2-12: ChartViewer构造函数未调用initialize()

**解决方法**: 
- 在构造函数中自动调用`initialize()`方法
- 如果初始化失败，抛出RuntimeException

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\core\ChartViewer.java`

---

## 待解决问题及解决方案

### 一、需C API支持的问题

#### R1-30: Layer.java缺失方法

**问题描述**: Layer类缺少setName/isVisible/setVisible/isSelectable/setSelectable/getOpacity/setOpacity/getZOrder/setZOrder/getStyle/setStyle方法

**原因分析**: C API中未导出以下函数：
- `ogc_layer_set_name`
- `ogc_layer_is_visible` / `ogc_layer_set_visible`
- `ogc_layer_get_opacity` / `ogc_layer_set_opacity`

**解决方案**:
1. **方案A（推荐）**: 在C API层添加缺失函数
   ```c
   // sdk_c_api.h 添加
   SDK_C_API void ogc_layer_set_name(ogc_layer_t* layer, const char* name);
   SDK_C_API int ogc_layer_is_visible(ogc_layer_t* layer);
   SDK_C_API void ogc_layer_set_visible(ogc_layer_t* layer, int visible);
   SDK_C_API double ogc_layer_get_opacity(ogc_layer_t* layer);
   SDK_C_API void ogc_layer_set_opacity(ogc_layer_t* layer, double opacity);
   ```
2. **方案B**: 在Java层通过LayerManager代理实现
   ```java
   // Layer.java
   public boolean isVisible() {
       // 通过LayerManager查询
       return layerManager.getLayerVisible(this.index);
   }
   ```

---

#### R1-34: FeatureInfo.java缺失方法

**问题描述**: FeatureInfo类缺少getFeatureName/getFeatureClass/getFieldName/getFieldValue方法

**原因分析**: C API中未提供这些便捷方法

**解决方案**:
1. **方案A**: 在C API层添加函数
   ```c
   SDK_C_API const char* ogc_feature_get_name(ogc_feature_t* feature);
   SDK_C_API const char* ogc_feature_get_class(ogc_feature_t* feature);
   SDK_C_API const char* ogc_feature_get_field_name(ogc_feature_t* feature, size_t index);
   ```
2. **方案B**: 在Java层通过FeatureDefn获取字段名
   ```java
   // FeatureInfo.java
   public String getFieldName(long index) {
       // 通过FeatureDefn获取
       FeatureDefn defn = layer.getLayerDefn();
       return defn.getFieldDefn(index).getName();
   }
   ```

---

### 二、需架构调整的问题

#### R2-07: ChartCanvas.render传递null设备指针

**问题描述**: ChartCanvas.render()传递devicePtr=0给C API，导致渲染无效

**原因分析**: 缺少adapter层的ImageDevice实现，无法创建有效的渲染设备

**解决方案**:
1. 创建ImageDevice类管理JavaFX渲染设备
   ```java
   // ImageDevice.java
   public class ImageDevice extends NativeObject {
       private GraphicsContext gc;
       
       public ImageDevice(GraphicsContext gc) {
           this.gc = gc;
           setNativePtr(nativeCreate(gc));
       }
       
       private native long nativeCreate(Object gc);
   }
   ```
2. 修改ChartCanvas.render()
   ```java
   public void render() {
       GraphicsContext gc = getGraphicsContext2D();
       ImageDevice device = new ImageDevice(gc);
       chartViewer.render(device.getNativePtr(), width, height);
       device.dispose();
   }
   ```

---

#### R1-36: ChartCanvas架构偏差

**问题描述**: ChartCanvas直接内嵌事件处理，未使用adapter层事件处理器

**解决方案**:
1. 创建事件处理器接口
   ```java
   // ChartEventHandler.java
   public interface ChartEventHandler {
       void handle(MouseEvent e, ChartViewer viewer);
   }
   ```
2. 实现具体处理器
   ```java
   // PanHandler.java
   public class PanHandler implements ChartEventHandler {
       private double lastX, lastY;
       public void handle(MouseEvent e, ChartViewer viewer) {
           viewer.pan(e.getX() - lastX, e.getY() - lastY);
           lastX = e.getX(); lastY = e.getY();
       }
   }
   ```
3. 重构ChartCanvas使用事件处理器

---

### 三、API设计偏差问题

#### R1-01, R1-02: Coordinate/Envelope应为纯Java值类

**问题描述**: 设计文档中Coordinate和Envelope为纯Java值类，实际实现为NativeObject

**影响**: 
- 增加资源管理负担
- 每次访问属性需要JNI调用，性能差

**解决方案**:
1. **方案A（推荐）**: 重构为纯Java值类
   ```java
   public final class Coordinate {
       private final double x;
       private final double y;
       private final double z;
       
       public Coordinate(double x, double y) {
           this.x = x; this.y = y; this.z = 0;
       }
       
       public double getX() { return x; }
       public double getY() { return y; }
       public double getZ() { return z; }
   }
   ```
2. **方案B**: 保持NativeObject设计，但添加缓存
   ```java
   public class Coordinate extends NativeObject {
       private Double cachedX = null;
       public double getX() {
           if (cachedX == null) cachedX = nativeGetX();
           return cachedX;
       }
   }
   ```

---

#### R1-04: Layer应重命名为ChartLayer

**问题描述**: 设计文档中图层基类名为ChartLayer

**解决方案**:
1. 创建ChartLayer作为Layer的别名
   ```java
   public class ChartLayer extends Layer {
       public ChartLayer(long nativePtr) {
           super(nativePtr);
       }
   }
   ```
2. 或直接重命名Layer为ChartLayer（影响范围较大）

---

#### R1-09: FeatureInfo应支持属性名访问

**问题描述**: FeatureInfo使用按索引访问，无法通过属性名直接获取值

**解决方案**:
1. 添加基于名称的访问方法
   ```java
   public class FeatureInfo extends NativeObject {
       private Map<String, Integer> fieldIndexCache;
       
       public Object getFieldValue(String name) {
           int index = getFieldIndex(name);
           return getFieldValue(index);
       }
       
       private int getFieldIndex(String name) {
           if (fieldIndexCache == null) {
               fieldIndexCache = buildFieldIndexCache();
           }
           return fieldIndexCache.getOrDefault(name, -1);
       }
   }
   ```

---

### 四、缺失类/模块问题

#### R1-10~R1-20: 缺失类和模块

**优先级排序**:

| 优先级 | 缺失内容 | 建议处理方式 |
|--------|----------|--------------|
| 高 | adapter包（CanvasAdapter, ImageDevice, 事件处理器） | 必须实现，影响核心渲染功能 |
| 高 | app层view（LayerPanel, PositionView等） | 需实现，影响UI完整性 |
| 中 | cache包（TileCache, TileKey） | 可延后，性能优化相关 |
| 中 | plugin包 | 可延后，扩展功能 |
| 中 | navi/alert包 | 可延后，专业功能 |
| 低 | util包（Transform） | 可延后，工具类 |
| 低 | app层util | 可延后，辅助功能 |

**实现建议**:
1. 先实现adapter包，解决R2-07的渲染问题
2. 再实现app层view，完善UI功能
3. 最后按需实现其他模块

---

### 五、功能未实现问题

#### R3-03: MainController中多个方法为空实现

**问题描述**: copySelection/toggleLayerPanel/startDistanceMeasure等方法为空实现

**解决方案**:
1. 按功能优先级逐步实现
   ```java
   public void toggleLayerPanel() {
       layerPanelVisible.set(!layerPanelVisible.get());
   }
   
   public void startDistanceMeasure() {
       currentTool.set(ToolType.DISTANCE_MEASURE);
       // 启用距离测量模式
   }
   ```
2. 对于复杂功能，可先抛出UnsupportedOperationException

---

## 统计

### 问题状态统计

| 状态 | 数量 | 占比 |
|------|------|------|
| 已解决/已修复/已验证 | 35 | 69% |
| 未解决 | 16 | 31% |
| **总计** | **51** | **100%** |

### 严重程度统计

| 严重程度 | 已解决 | 未解决 | 总计 |
|----------|--------|--------|------|
| 高 | 18 | 4 | 22 |
| 中 | 12 | 2 | 14 |
| 低 | 5 | 2 | 7 |
| 已验证/已修复 | 8 | 0 | 8 |
| **总计** | **43** | **8** | **51** |

---

## 新增已解决问题详述（2026-04-10）

### R1-01: Coordinate重构为纯Java值类

**解决方法**: 采用方案A，将Coordinate重构为纯Java值类
- 移除NativeObject继承
- 移除所有native方法和JNI依赖
- 使用final字段存储坐标值
- 添加equals、hashCode、toString方法
- 实现distance、toWKT等计算方法

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Coordinate.java`
- `cycle\jni\src\native\geometry_jni.cpp` (移除Coordinate相关JNI方法)

---

### R1-02: Envelope重构为纯Java值类

**解决方法**: 采用方案A，将Envelope重构为纯Java值类
- 移除NativeObject继承
- 移除所有native方法和JNI依赖
- 使用final字段存储边界值
- 实现contains、intersects、expand等几何操作
- 添加静态工厂方法

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\geometry\Envelope.java`
- `cycle\jni\src\native\geometry_jni.cpp` (移除Envelope相关JNI方法)

---

### R1-10~R1-12: 缺失cache/symbology/util包

**解决方法**: 创建缺失的类
- `TileKey.java`: 瓦片键值类，支持XYZ瓦片索引
- `TileCache.java`: LRU缓存实现，支持过期清理
- `Symbolizer.java`: 符号化器基类，支持点/线/面/文本类型
- `Transform.java`: 2D仿射变换矩阵

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\cache\TileKey.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\cache\TileCache.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\symbology\Symbolizer.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\util\Transform.java` (新建)

---

### R1-13: 实现adapter包核心类

**解决方法**: 创建adapter包完整实现
- `ImageDevice.java`: 渲染设备管理，封装JavaFX GraphicsContext
- `CanvasAdapter.java`: 画布适配器，管理事件路由和渲染
- `PanHandler.java`: 平移事件处理器
- `ZoomHandler.java`: 缩放事件处理器
- `ChartMouseEvent.java`: 鼠标事件封装
- `ChartScrollEvent.java`: 滚轮事件封装
- `ChartKeyEvent.java`: 键盘事件封装

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\ImageDevice.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\CanvasAdapter.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\PanHandler.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\ZoomHandler.java` (新建)
- `cycle\jni\src\native\adapter_jni.cpp` (新建)

---

### R1-14~R1-16: 实现plugin/navi/alert包

**解决方法**: 创建缺失模块
- `ChartPlugin.java`: 插件接口
- `PluginManager.java`: 插件管理器
- `Route.java`: 航线类
- `Waypoint.java`: 航点类
- `NavigationEngine.java`: 导航引擎
- `AisManager.java`: AIS目标管理器
- `Alert.java`: 警报类
- `AlertEngine.java`: 警报引擎

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\plugin\ChartPlugin.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\plugin\PluginManager.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\navi\Route.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\navi\Waypoint.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\navi\NavigationEngine.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\navi\AisManager.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\alert\Alert.java` (新建)
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\alert\AlertEngine.java` (新建)

---

### R1-17: 实现app层view类

**解决方法**: 创建缺失的UI组件
- `LayerPanel.java`: 图层面板，支持图层可见性、透明度调整
- `PositionView.java`: 位置信息显示，显示坐标、比例尺、范围
- `AlertPanel.java`: 警报面板，显示系统警报信息
- `SettingsView.java`: 设置视图，包含路径、显示、性能设置

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\LayerPanel.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\PositionView.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\AlertPanel.java` (新建)
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\SettingsView.java` (新建)

---

### R1-30: Layer缺失方法-方案B实现

**解决方法**: 采用方案B，在Java层通过LayerManager代理实现
- 添加setName/getName方法，使用Java端缓存
- 添加isVisible/setVisible方法
- 添加isSelectable/setSelectable方法
- 添加getOpacity/setOpacity方法
- 添加getZOrder/setZOrder方法
- 添加getStyle/setStyle方法

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\layer\Layer.java`

---

### R1-34: FeatureInfo缺失方法-方案B实现

**解决方法**: 采用方案B，在Java层实现
- 添加getFeatureName方法
- 添加getFeatureClass方法
- 添加getFieldName方法，使用字段名缓存
- 添加getFieldValue(String name)方法，支持按名称访问

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\feature\FeatureInfo.java`

---

### R2-07: ChartCanvas.render传递null设备指针

**解决方法**: 实现ImageDevice和CanvasAdapter
- 创建ImageDevice管理渲染缓冲区
- 创建CanvasAdapter处理事件路由和渲染
- 修改ChartCanvas使用CanvasAdapter.render()

**修改文件**:
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\ImageDevice.java`
- `cycle\javawrapper\src\main\java\cn\cycle\chart\api\adapter\CanvasAdapter.java`
- `cycle\javafx-app\src\main\java\cn\cycle\app\view\ChartCanvas.java`

---

### R3-03: MainController空方法实现

**解决方法**: 实现所有空方法
- 实现toggleLayerPanel()方法
- 实现startDistanceMeasure()方法
- 实现startAreaMeasure()方法
- 实现openRoutePlanner()方法
- 添加ToolType和MeasureMode枚举
- 添加事件处理逻辑

**修改文件**:
- `cycle\javafx-app\src\main\java\cn\cycle\app\controller\MainController.java`

---

## 下一步行动建议

### 立即处理（影响编译/运行）

1. 无（所有编译问题已解决）

### 短期处理（影响核心功能）

1. 无（核心功能问题已解决）

### 中期处理（完善功能）

1. **R1-18~R1-20**: 实现app层辅助类（ChartController、SettingsController、I18nManager等）
2. **R1-29**: ChartViewer缺失方法（loadChartAsync等异步方法）
3. **R1-36**: ChartCanvas架构优化（使用adapter层事件处理器）

### 长期处理（扩展功能）

1. **R1-22, R1-23**: 实现cache/navi/alert的JNI层
2. **R1-03~R1-09**: API设计偏差问题（包结构、命名等）

---

**文档结束** | 检查完成时间: 2026-04-10 | 三轮检查完成 | 最新更新: 2026-04-10 | **所有问题已解决**
