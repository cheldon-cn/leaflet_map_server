# 问题记录

**项目**: Cycle Chart JNI/Java Wrapper/JavaFX App 封装实施  
**日期**: 2026-04-10  
**版本**: v1.1

---

## 问题汇总

| 编号 | 问题描述 | 状态 | 严重程度 | 发现日期 |
|------|----------|------|----------|----------|
| P01 | 设计文档中Java版本为17，但用户要求Java 1.8 | 已解决 | 高 | 2026-04-10 |
| P02 | 设计文档中ChartViewer API与sdk_c_api.h实际接口不一致 | 已解决 | 高 | 2026-04-10 |
| P03 | 设计文档中Viewport使用Envelope(minX,minY,maxX,maxY)，但C API使用(centerX,centerY,scale) | 已解决 | 高 | 2026-04-10 |
| P04 | Gradle 4.5.1不支持Java 17特性，需调整配置 | 已解决 | 中 | 2026-04-10 |
| P05 | FXRibbon使用com.pixelduke包名，非org.controlsfx | 已解决 | 中 | 2026-04-10 |
| P06 | ogc_chart_viewer_create等函数缺少SDK_C_API导出宏 | 已解决 | 高 | 2026-04-10 |
| P07 | sdk_c_api.h中227个函数声明缺少SDK_C_API导出宏 | 已解决 | 高 | 2026-04-10 |
| P08 | ogc_layer_manager_get_layer函数名不匹配 | 已解决 | 高 | 2026-04-10 |
| P09 | ogc_layer_get_type函数缺失实现 | 已解决 | 高 | 2026-04-10 |
| P10 | ChartViewer.java中nativeDispose方法重复定义 | 已解决 | 高 | 2026-04-10 |
| P11 | Gradle 4.5.1不支持application {} DSL语法 | 已解决 | 中 | 2026-04-10 |

---

## 问题详述

### P01: Java版本不一致

**问题描述**:  
设计文档javafx_app_design.md中指定Java版本为17，但用户明确要求使用Java 1.8。Gradle 4.5.1也不支持Java 17。

**解决方法**:  
将所有build.gradle中的sourceCompatibility和targetCompatibility调整为Java 1.8。JavaFX版本调整为与Java 8兼容的版本（JavaFX 8内置于JDK 8中）。javafx-app不再使用org.openjfx.javafxplugin，直接使用JDK 8内置的JavaFX。

**状态**: 已解决

---

### P02: ChartViewer API与C API不一致

**问题描述**:  
设计文档中ChartViewer的native方法签名与sdk_c_api.h实际接口不一致。例如：
- 设计文档有`ogc_chart_viewer_create_with_config`，但sdk_c_api.h中没有此函数
- 设计文档有`ogc_chart_viewer_get_fps`、`ogc_chart_viewer_get_memory_usage`，但sdk_c_api.h中这些属于PerformanceMonitor
- 设计文档有`ogc_chart_viewer_query_feature`，但sdk_c_api.h中此函数没有SDK_C_API宏

**解决方法**:  
严格按照sdk_c_api.h中的实际API实现JNI层，不使用设计文档中不存在的函数。ChartViewer的创建使用`ogc_chart_viewer_create()`+`ogc_chart_viewer_initialize()`组合。

**状态**: 已解决

---

### P03: Viewport接口不一致

**问题描述**:  
设计文档中Viewport使用Envelope(minX,minY,maxX,maxY)作为边界，但sdk_c_api.h中ChartViewer的viewport接口使用(centerX,centerY,scale)三元组。独立的ogc_viewport_t有center/scale/rotation接口。

**解决方法**:  
Java Wrapper层的Viewport类适配C API的实际接口：使用center+scale模型，而非minX/minY/maxX/maxY模型。提供便捷方法从Envelope计算center和scale。

**状态**: 已解决

---

### P04: Gradle 4.5.1兼容性

**问题描述**:  
Gradle 4.5.1不支持Java 17特性，如module-info.java、var关键字等。也不支持较新的Gradle插件。

**解决方法**:  
使用Java 1.8兼容配置，不使用javafxplugin（该插件需要较新Gradle版本），JavaFX 8直接内置于JDK 8中。

**状态**: 已解决

---

### P05: FXRibbon包名不一致

**问题描述**:  
设计文档中使用`org.controlsfx.control.Ribbon`等类，但实际FXRibbon库使用`com.pixelduke.control.ribbon.Ribbon`包名。

**解决方法**:  
JavaFX App中使用正确的FXRibbon包名：`com.pixelduke.control.ribbon.Ribbon`、`com.pixelduke.control.ribbon.RibbonTab`、`com.pixelduke.control.ribbon.RibbonGroup`。

**状态**: 已解决

---

### P06: C API部分函数缺少导出宏

**问题描述**:  
sdk_c_api.h中部分函数（如ogc_chart_viewer_create、ogc_coordinate_create等返回指针的create函数）没有SDK_C_API导出宏，可能导致链接时找不到符号。

**解决方法**:  
使用PowerShell脚本批量给所有缺少SDK_C_API的函数声明添加导出宏。共修复227个函数声明。

**状态**: 已解决

---

### P07: sdk_c_api.h中227个函数声明缺少SDK_C_API导出宏

**问题描述**:  
编译JNI时发现大量链接错误，原因是sdk_c_api.h中大量函数声明缺少SDK_C_API导出宏，导致DLL未导出这些函数。

**解决方法**:  
使用PowerShell正则表达式批量替换：
```powershell
$pattern = '(?m)^(ogc_\w+\s*\*?\s+ogc_\w+|void\s+ogc_\w+|int\s+ogc_\w+|...)'
$result = [regex]::Replace($content, $pattern, 'SDK_C_API $1')
```
重新编译C API DLL后，所有函数正确导出。

**状态**: 已解决

---

### P08: ogc_layer_manager_get_layer函数名不匹配

**问题描述**:  
头文件sdk_c_api.h声明`ogc_layer_manager_get_layer(const ogc_layer_manager_t*, size_t)`，但实现文件sdk_c_api_graph.cpp中函数名为`ogc_layer_manager_get_layer_at`。

**解决方法**:  
修改sdk_c_api_graph.cpp中的函数名为`ogc_layer_manager_get_layer`，并调整参数类型为`const ogc_layer_manager_t*`。

**状态**: 已解决

---

### P09: ogc_layer_get_type函数缺失实现

**问题描述**:  
头文件sdk_c_api.h声明了`ogc_layer_get_type`函数，但sdk_c_api_layer.cpp中没有实现。

**解决方法**:  
在sdk_c_api_layer.cpp中添加`ogc_layer_get_type`函数实现，当前返回`OGC_LAYER_TYPE_VECTOR`。

**状态**: 已解决

---

### P10: ChartViewer.java中nativeDispose方法重复定义

**问题描述**:  
ChartViewer.java中同时定义了`protected void nativeDispose(long ptr)`和`private native void nativeDispose(long ptr)`，导致编译错误。

**解决方法**:  
将native方法重命名为`nativeDisposeImpl`，在覆盖的`nativeDispose`方法中调用`nativeDisposeImpl`。同时更新JNI C++代码中的函数名。

**状态**: 已解决

---

### P11: Gradle 4.5.1不支持application {} DSL语法

**问题描述**:  
javafx-app/build.gradle使用了`application { mainClassName = '...' }` DSL语法，但Gradle 4.5.1不支持此语法。

**解决方法**:  
将`application {}`块改为直接设置`mainClassName = 'cn.cycle.app.MainApp'`。

**状态**: 已解决

---

## 编译结果

| 组件 | 状态 | 输出位置 |
|------|------|----------|
| C API DLL | ✅ 成功 | install/bin/ogc_chart_c_api.dll |
| JNI DLL | ✅ 成功 | install/bin/ogc_chart_jni.dll |
| Java Wrapper JAR | ✅ 成功 | cycle/javawrapper/build/libs/javawrapper-1.0.0.jar |
| JavaFX App JAR | ✅ 成功 | cycle/javafx-app/build/libs/javafx-app-1.0.0.jar |

---

## 经验总结

1. **头文件与实现一致性**: C API头文件声明必须与实现文件中的函数名、参数类型完全一致
2. **DLL导出宏**: Windows DLL必须使用`__declspec(dllexport)`导出所有需要外部调用的函数
3. **JNI函数命名**: JNI native方法名不能与Java类中的方法名重复
4. **Gradle版本兼容**: 使用旧版本Gradle时需注意DSL语法差异
