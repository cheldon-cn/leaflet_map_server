# JNI与Java接口详细设计

**文档编号**: CDS-JNI-INTERFACE-001  
**版本**: v1.0  
**日期**: 2026-04-08  
**依据文档**: [javafx_chart_application_design.md](javafx_chart_application_design.md) | [index_all.md](../index_all.md)

---

## 目录

1. [概述](#1-概述)
2. [Java包结构](#2-java包结构)
3. [JNI桥接层接口](#3-jni桥接层接口)
4. [Java API层接口](#4-java-api层接口)
5. [JavaFX适配层接口](#5-javafx适配层接口)
6. [JavaFX应用层接口](#6-javafx应用层接口)
7. [C++核心模块映射](#7-c核心模块映射)
8. [接口实现清单](#8-接口实现清单)

---

## 1. 概述

### 1.1 设计目标

本文档详细列出实现JavaFX海图显示应用所需的所有Java类、接口及其对应的JNI接口，确保：

- Java层与C++核心模块的正确映射
- JNI接口的高效调用
- 资源的安全管理

### 1.2 依赖的C++模块

| 模块 | 核心类 | 用途 |
|------|--------|------|
| **ogc_graph** | `DrawFacade`, `RenderTask`, `LayerManager` | 渲染门面 |
| **ogc_draw** | `DrawEngine`, `DrawDevice`, `DrawContext`, `Color` | 绘图引擎 |
| **ogc_layer** | `CNLayer`, `CNVectorLayer`, `CNDataSource` | 图层管理 |
| **ogc_symbology** | `Symbolizer`, `Filter`, `S52StyleManager` | 符号化 |
| **ogc_cache** | `TileCache`, `TileKey` | 瓦片缓存 |
| **ogc_feature** | `CNFeature`, `CNFeatureDefn`, `CNFieldValue` | 要素模型 |
| **ogc_geom** | `Geometry`, `Point`, `LineString`, `Polygon`, `Envelope`, `Coordinate` | 几何对象 |
| **ogc_proj** | `CoordinateTransformer`, `ProjTransformer` | 坐标转换 |
| **ogc_base** | `Logger`, `PerformanceMonitor` | 基础工具 |
| **chart_parser** | `ChartParser`, `S57Parser` | 海图解析 |

---

## 2. Java包结构

```
cn.cycle.chart/
├── jni/                          # JNI桥接层
│   ├── JniBridge.java            # JNI初始化入口
│   ├── JniEnvManager.java        # 环境管理（内部使用）
│   ├── NativeObject.java         # Native对象基类
│   └── NativeHandle.java         # Native句柄管理
│
├── api/                          # Java API层
│   ├── core/
│   │   ├── ChartViewer.java      # 海图查看器
│   │   ├── ChartConfig.java      # 配置管理
│   │   ├── Viewport.java         # 视口管理
│   │   └── RenderContext.java    # 渲染上下文
│   ├── geometry/
│   │   ├── Geometry.java         # 几何对象基类
│   │   ├── Point.java            # 点几何
│   │   ├── LineString.java       # 线几何
│   │   ├── Polygon.java          # 面几何
│   │   ├── Envelope.java         # 包络矩形
│   │   └── Coordinate.java       # 坐标点
│   ├── feature/
│   │   ├── FeatureInfo.java      # 要素信息
│   │   ├── FieldValue.java       # 字段值
│   │   └── FieldDefn.java        # 字段定义
│   ├── layer/
│   │   ├── ChartLayer.java       # 图层基类
│   │   ├── VectorLayer.java      # 矢量图层
│   │   └── LayerManager.java     # 图层管理器
│   ├── cache/
│   │   ├── TileCache.java        # 瓦片缓存
│   │   └── TileKey.java          # 瓦片键
│   ├── symbology/
│   │   ├── Symbolizer.java       # 符号化器
│   │   └── DrawStyle.java        # 绘制样式
│   └── util/
│       ├── Color.java            # 颜色类
│       └── Transform.java        # 变换矩阵
│
├── adapter/                      # JavaFX适配层
│   ├── canvas/
│   │   ├── CanvasAdapter.java    # Canvas适配器
│   │   └── ImageDevice.java      # 图像设备
│   ├── event/
│   │   ├── ChartEventHandler.java # 事件处理器
│   │   ├── PanHandler.java       # 平移处理器
│   │   ├── ZoomHandler.java      # 缩放处理器
│   │   └── SelectHandler.java    # 选择处理器
│   ├── binding/
│   │   ├── PropertyBinder.java   # 属性绑定器
│   │   └── ThreadBridge.java     # 线程桥接
│   └── util/
│       ├── ImageConverter.java   # 图像转换
│       └── CoordinateConverter.java # 坐标转换
│
├── plugin/                       # 插件扩展层
│   ├── ChartPlugin.java          # 插件接口
│   ├── PluginManager.java        # 插件管理器
│   ├── LayerProvider.java        # 图层提供者
│   └── SymbolProvider.java       # 符号提供者
│
└── app/                          # JavaFX应用层
    ├── view/
    │   ├── MainView.java         # 主界面
    │   ├── ChartCanvas.java      # 海图画布
    │   ├── LayerPanel.java       # 图层面板
    │   ├── PositionView.java     # 船位显示
    │   ├── AlertPanel.java       # 警报面板
    │   └── SettingsView.java     # 设置界面
    ├── controller/
    │   ├── MainController.java   # 主控制器
    │   ├── ChartController.java  # 海图控制器
    │   └── SettingsController.java # 设置控制器
    ├── model/
    │   ├── ChartModel.java       # 海图模型
    │   └── SettingsModel.java    # 设置模型
    └── util/
        ├── I18nManager.java      # 国际化管理
        ├── AccessibilityHelper.java # 无障碍辅助
        └── MetricsExporter.java  # 指标导出
```

---

## 3. JNI桥接层接口

### 3.1 JniBridge.java

```java
package cn.cycle.chart.jni;

public final class JniBridge {
    
    private static volatile boolean initialized = false;
    private static final Object lock = new Object();
    
    static {
        System.loadLibrary("ogc_chart_jni");
    }
    
    public static void initialize() {
        if (!initialized) {
            synchronized (lock) {
                if (!initialized) {
                    nativeInitialize();
                    initialized = true;
                }
            }
        }
    }
    
    public static void shutdown() {
        if (initialized) {
            synchronized (lock) {
                if (initialized) {
                    nativeShutdown();
                    initialized = false;
                }
            }
        }
    }
    
    public static boolean isInitialized() {
        return initialized;
    }
    
    private static native void nativeInitialize();
    private static native void nativeShutdown();
}
```

**对应JNI实现**:

```cpp
// jni_bridge.cpp
JNIEXPORT void JNICALL Java_org_ogc_chart_jni_JniBridge_nativeInitialize
  (JNIEnv* env, jclass clazz) {
    ogc::jni::JniEnvManager::GetInstance()->Initialize(
        ogc::jni::JniEnvManager::GetJavaVMFromEnv(env));
    ogc::graph::DrawFacade::Instance().Initialize();
}

JNIEXPORT void JNICALL Java_org_ogc_chart_jni_JniBridge_nativeShutdown
  (JNIEnv* env, jclass clazz) {
    ogc::graph::DrawFacade::Instance().Shutdown();
    ogc::jni::JniEnvManager::GetInstance()->Shutdown();
}
```

### 3.2 NativeObject.java

```java
package cn.cycle.chart.jni;

public abstract class NativeObject implements AutoCloseable {
    
    protected long nativePtr;
    private boolean disposed = false;
    
    protected NativeObject() {
        this.nativePtr = 0;
    }
    
    protected NativeObject(long nativePtr) {
        this.nativePtr = nativePtr;
    }
    
    public long getNativePtr() {
        return nativePtr;
    }
    
    public boolean isDisposed() {
        return disposed || nativePtr == 0;
    }
    
    protected void setNativePtr(long ptr) {
        this.nativePtr = ptr;
    }
    
    @Override
    public void close() {
        if (!disposed && nativePtr != 0) {
            nativeDispose(nativePtr);
            nativePtr = 0;
            disposed = true;
        }
    }
    
    protected abstract void nativeDispose(long ptr);
    
    @Override
    protected void finalize() throws Throwable {
        try {
            close();
        } finally {
            super.finalize();
        }
    }
}
```

---

## 4. Java API层接口

### 4.1 ChartViewer.java

```java
package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.layer.LayerManager;
import java.util.List;
import java.util.function.Consumer;

public class ChartViewer extends NativeObject {
    
    private LayerManager layerManager;
    private Viewport viewport;
    
    public ChartViewer() {
        setNativePtr(nativeCreate());
        this.layerManager = new LayerManager(nativeGetLayerManager(getNativePtr()));
        this.viewport = new Viewport(this);
    }
    
    public ChartViewer(ChartConfig config) {
        setNativePtr(nativeCreateWithConfig(config));
        this.layerManager = new LayerManager(nativeGetLayerManager(getNativePtr()));
        this.viewport = new Viewport(this);
    }
    
    // 海图加载
    public boolean loadChart(String filePath) {
        checkNotDisposed();
        return nativeLoadChart(getNativePtr(), filePath);
    }
    
    public void loadChartAsync(String filePath, Consumer<Boolean> callback) {
        checkNotDisposed();
        new Thread(() -> {
            boolean result = nativeLoadChart(getNativePtr(), filePath);
            callback.accept(result);
        }).start();
    }
    
    public void unloadChart() {
        checkNotDisposed();
        nativeUnloadChart(getNativePtr());
    }
    
    // 渲染
    public void render(Object canvas, double width, double height) {
        checkNotDisposed();
        nativeRender(getNativePtr(), canvas, width, height);
    }
    
    public void requestRender() {
        checkNotDisposed();
        nativeRequestRender(getNativePtr());
    }
    
    // 视口操作
    public void setViewport(double minX, double minY, double maxX, double maxY) {
        checkNotDisposed();
        nativeSetViewport(getNativePtr(), minX, minY, maxX, maxY);
    }
    
    public Envelope getViewport() {
        checkNotDisposed();
        return nativeGetViewport(getNativePtr());
    }
    
    public void pan(double dx, double dy) {
        checkNotDisposed();
        nativePan(getNativePtr(), dx, dy);
    }
    
    public void zoom(double factor, double centerX, double centerY) {
        checkNotDisposed();
        nativeZoom(getNativePtr(), factor, centerX, centerY);
    }
    
    public void setScale(double scale) {
        checkNotDisposed();
        nativeSetScale(getNativePtr(), scale);
    }
    
    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }
    
    // 要素查询
    public FeatureInfo queryFeature(double x, double y) {
        checkNotDisposed();
        return nativeQueryFeature(getNativePtr(), x, y);
    }
    
    public List<FeatureInfo> queryFeatures(Envelope bounds) {
        checkNotDisposed();
        return nativeQueryFeatures(getNativePtr(), bounds);
    }
    
    // 坐标转换
    public Coordinate screenToWorld(double screenX, double screenY) {
        checkNotDisposed();
        return nativeScreenToWorld(getNativePtr(), screenX, screenY);
    }
    
    public Coordinate worldToScreen(double worldX, double worldY) {
        checkNotDisposed();
        return nativeWorldToScreen(getNativePtr(), worldX, worldY);
    }
    
    // 图层管理
    public LayerManager getLayerManager() {
        return layerManager;
    }
    
    public Viewport getViewportObject() {
        return viewport;
    }
    
    // 性能监控
    public double getFps() {
        return nativeGetFps(getNativePtr());
    }
    
    public long getMemoryUsage() {
        return nativeGetMemoryUsage(getNativePtr());
    }
    
    public double getCacheHitRate() {
        return nativeGetCacheHitRate(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native long nativeCreateWithConfig(ChartConfig config);
    @Override
    protected native void nativeDispose(long ptr);
    private native boolean nativeLoadChart(long ptr, String filePath);
    private native void nativeUnloadChart(long ptr);
    private native void nativeRender(long ptr, Object canvas, double width, double height);
    private native void nativeRequestRender(long ptr);
    private native void nativeSetViewport(long ptr, double minX, double minY, double maxX, double maxY);
    private native Envelope nativeGetViewport(long ptr);
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor, double centerX, double centerY);
    private native void nativeSetScale(long ptr, double scale);
    private native double nativeGetScale(long ptr);
    private native FeatureInfo nativeQueryFeature(long ptr, double x, double y);
    private native List<FeatureInfo> nativeQueryFeatures(long ptr, Envelope bounds);
    private native Coordinate nativeScreenToWorld(long ptr, double screenX, double screenY);
    private native Coordinate nativeWorldToScreen(long ptr, double worldX, double worldY);
    private native long nativeGetLayerManager(long ptr);
    private native double nativeGetFps(long ptr);
    private native long nativeGetMemoryUsage(long ptr);
    private native double nativeGetCacheHitRate(long ptr);
    
    private void checkNotDisposed() {
        if (isDisposed()) {
            throw new IllegalStateException("ChartViewer has been disposed");
        }
    }
}
```

**对应JNI实现**:

```cpp
// chart_viewer_jni.cpp
#include "ogc/draw/draw_facade.h"
#include "ogc/jni/jni_env.h"
#include "ogc/jni/jni_convert.h"
#include "ogc/jni/jni_exception.h"

using namespace ogc::jni;
using namespace ogc::graph;

JNIEXPORT jlong JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeCreate
  (JNIEnv* env, jobject obj) {
    JniLocalRefScope scope(env);
    try {
        DrawFacade* facade = &DrawFacade::Instance();
        return JniConverter::ToJLongPtr(facade);
    } catch (const std::exception& e) {
        JniException::ThrowRuntimeException(env, e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeDispose
  (JNIEnv* env, jobject obj, jlong ptr) {
    // DrawFacade是单例，不需要释放
}

JNIEXPORT jboolean JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeLoadChart
  (JNIEnv* env, jobject obj, jlong ptr, jstring filePath) {
    JniLocalRefScope scope(env);
    try {
        DrawFacade* facade = static_cast<DrawFacade*>(JniConverter::FromJLongPtr(ptr));
        std::string path = JniConverter::ToString(env, filePath);
        return facade->LoadChart(path);
    } catch (const std::exception& e) {
        JniException::ThrowRuntimeException(env, e.what());
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeRender
  (JNIEnv* env, jobject obj, jlong ptr, jobject canvas, jdouble width, jdouble height) {
    JniLocalRefScope scope(env);
    try {
        DrawFacade* facade = static_cast<DrawFacade*>(JniConverter::FromJLongPtr(ptr));
        facade->Render(canvas, width, height);
    } catch (const std::exception& e) {
        JniException::ThrowRuntimeException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeSetViewport
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    JniLocalRefScope scope(env);
    try {
        DrawFacade* facade = static_cast<DrawFacade*>(JniConverter::FromJLongPtr(ptr));
        facade->SetViewport(minX, minY, maxX, maxY);
    } catch (const std::exception& e) {
        JniException::ThrowRuntimeException(env, e.what());
    }
}

JNIEXPORT jobject JNICALL Java_org_ogc_chart_api_core_ChartViewer_nativeQueryFeature
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    JniLocalRefScope scope(env);
    try {
        DrawFacade* facade = static_cast<DrawFacade*>(JniConverter::FromJLongPtr(ptr));
        CNFeature* feature = facade->QueryFeature(x, y);
        if (feature) {
            return CreateFeatureInfo(env, feature);
        }
        return nullptr;
    } catch (const std::exception& e) {
        JniException::ThrowRuntimeException(env, e.what());
        return nullptr;
    }
}
```

### 4.2 ChartConfig.java

```java
package cn.cycle.chart.api.core;

public class ChartConfig {
    
    private String cachePath;
    private long maxCacheSize = 512 * 1024 * 1024; // 512MB
    private int threadCount = 4;
    private boolean enableAntialiasing = true;
    private boolean enableLabelCollision = true;
    private String s52StylePath;
    private double minZoomScale = 0.001;
    private double maxZoomScale = 100000.0;
    
    public ChartConfig() {}
    
    // Getters and Setters
    public String getCachePath() { return cachePath; }
    public void setCachePath(String cachePath) { this.cachePath = cachePath; }
    
    public long getMaxCacheSize() { return maxCacheSize; }
    public void setMaxCacheSize(long maxCacheSize) { this.maxCacheSize = maxCacheSize; }
    
    public int getThreadCount() { return threadCount; }
    public void setThreadCount(int threadCount) { this.threadCount = threadCount; }
    
    public boolean isEnableAntialiasing() { return enableAntialiasing; }
    public void setEnableAntialiasing(boolean enableAntialiasing) { this.enableAntialiasing = enableAntialiasing; }
    
    public boolean isEnableLabelCollision() { return enableLabelCollision; }
    public void setEnableLabelCollision(boolean enableLabelCollision) { this.enableLabelCollision = enableLabelCollision; }
    
    public String getS52StylePath() { return s52StylePath; }
    public void setS52StylePath(String s52StylePath) { this.s52StylePath = s52StylePath; }
    
    public double getMinZoomScale() { return minZoomScale; }
    public void setMinZoomScale(double minZoomScale) { this.minZoomScale = minZoomScale; }
    
    public double getMaxZoomScale() { return maxZoomScale; }
    public void setMaxZoomScale(double maxZoomScale) { this.maxZoomScale = maxZoomScale; }
}
```

### 4.3 Viewport.java

```java
package cn.cycle.chart.api.core;

import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Coordinate;

public class Viewport {
    
    private final ChartViewer viewer;
    
    Viewport(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    public Envelope getBounds() {
        return viewer.getViewport();
    }
    
    public void setBounds(Envelope bounds) {
        viewer.setViewport(bounds.getMinX(), bounds.getMinY(), 
                          bounds.getMaxX(), bounds.getMaxY());
    }
    
    public void setBounds(double minX, double minY, double maxX, double maxY) {
        viewer.setViewport(minX, minY, maxX, maxY);
    }
    
    public Coordinate getCenter() {
        Envelope bounds = getBounds();
        return new Coordinate(
            (bounds.getMinX() + bounds.getMaxX()) / 2,
            (bounds.getMinY() + bounds.getMaxY()) / 2
        );
    }
    
    public void setCenter(double x, double y) {
        Envelope bounds = getBounds();
        double width = bounds.getWidth();
        double height = bounds.getHeight();
        setBounds(x - width/2, y - height/2, x + width/2, y + height/2);
    }
    
    public double getScale() {
        return viewer.getScale();
    }
    
    public void setScale(double scale) {
        viewer.setScale(scale);
    }
    
    public void pan(double dx, double dy) {
        viewer.pan(dx, dy);
    }
    
    public void zoom(double factor) {
        Coordinate center = getCenter();
        viewer.zoom(factor, center.getX(), center.getY());
    }
    
    public void zoomTo(Envelope bounds) {
        setBounds(bounds);
    }
    
    public Coordinate screenToWorld(double screenX, double screenY) {
        return viewer.screenToWorld(screenX, screenY);
    }
    
    public Coordinate worldToScreen(double worldX, double worldY) {
        return viewer.worldToScreen(worldX, worldY);
    }
}
```

### 4.4 Geometry.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public abstract class Geometry extends NativeObject {
    
    public enum Type {
        POINT,
        LINESTRING,
        POLYGON,
        MULTIPOINT,
        MULTILINESTRING,
        MULTIPOLYGON,
        GEOMETRYCOLLECTION
    }
    
    public abstract Type getType();
    
    public abstract Envelope getEnvelope();
    
    public abstract String asText();
    
    public abstract Geometry clone();
    
    // Native方法
    public native int getDimension();
    public native int getCoordinateDimension();
    public native int getNumPoints();
    public native boolean isEmpty();
    public native boolean isSimple();
    public native boolean isValid();
    
    // 空间操作
    public native boolean contains(Geometry other);
    public native boolean intersects(Geometry other);
    public native boolean within(Geometry other);
    public native double distance(Geometry other);
    public native Geometry buffer(double distance);
    public native Geometry intersection(Geometry other);
    public native Geometry union(Geometry other);
    public native Geometry difference(Geometry other);
}
```

**对应JNI实现**:

```cpp
// geometry_jni.cpp
#include "ogc/geometry.h"
#include "ogc/jni/jni_env.h"
#include "ogc/jni/jni_convert.h"

JNIEXPORT jint JNICALL Java_org_ogc_chart_api_geometry_Geometry_getDimension
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Geometry* geom = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(ptr));
    return geom->GetDimension();
}

JNIEXPORT jint JNICALL Java_org_ogc_chart_api_geometry_Geometry_getNumPoints
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Geometry* geom = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(ptr));
    return geom->GetNumPoints();
}

JNIEXPORT jboolean JNICALL Java_org_ogc_chart_api_geometry_Geometry_isEmpty
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Geometry* geom = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(ptr));
    return geom->IsEmpty();
}

JNIEXPORT jboolean JNICALL Java_org_ogc_chart_api_geometry_Geometry_intersects
  (JNIEnv* env, jobject obj, jlong ptr, jlong otherPtr) {
    ogc::geom::Geometry* geom = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(ptr));
    ogc::geom::Geometry* other = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(otherPtr));
    return geom->Intersects(other);
}

JNIEXPORT jdouble JNICALL Java_org_ogc_chart_api_geometry_Geometry_distance
  (JNIEnv* env, jobject obj, jlong ptr, jlong otherPtr) {
    ogc::geom::Geometry* geom = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(ptr));
    ogc::geom::Geometry* other = static_cast<ogc::geom::Geometry*>(
        JniConverter::FromJLongPtr(otherPtr));
    return geom->Distance(other);
}
```

### 4.5 Point.java

```java
package cn.cycle.chart.api.geometry;

public class Point extends Geometry {
    
    public Point(double x, double y) {
        setNativePtr(nativeCreate(x, y));
    }
    
    public Point(double x, double y, double z) {
        setNativePtr(nativeCreate3D(x, y, z));
    }
    
    @Override
    public Type getType() {
        return Type.POINT;
    }
    
    public double getX() {
        checkNotDisposed();
        return nativeGetX(getNativePtr());
    }
    
    public double getY() {
        checkNotDisposed();
        return nativeGetY(getNativePtr());
    }
    
    public double getZ() {
        checkNotDisposed();
        return nativeGetZ(getNativePtr());
    }
    
    public void setX(double x) {
        checkNotDisposed();
        nativeSetX(getNativePtr(), x);
    }
    
    public void setY(double y) {
        checkNotDisposed();
        nativeSetY(getNativePtr(), y);
    }
    
    public Coordinate getCoordinate() {
        return new Coordinate(getX(), getY(), getZ());
    }
    
    @Override
    public Envelope getEnvelope() {
        return new Envelope(getX(), getY(), getX(), getY());
    }
    
    @Override
    public String asText() {
        return String.format("POINT (%.6f %.6f)", getX(), getY());
    }
    
    @Override
    public Geometry clone() {
        return new Point(getX(), getY(), getZ());
    }
    
    // Native方法
    private native long nativeCreate(double x, double y);
    private native long nativeCreate3D(double x, double y, double z);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native double nativeGetZ(long ptr);
    private native void nativeSetX(long ptr, double x);
    private native void nativeSetY(long ptr, double y);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

**对应JNI实现**:

```cpp
// point_jni.cpp
#include "ogc/point.h"

JNIEXPORT jlong JNICALL Java_org_ogc_chart_api_geometry_Point_nativeCreate
  (JNIEnv* env, jobject obj, jdouble x, jdouble y) {
    ogc::geom::Point* pt = ogc::geom::Point::Create(x, y);
    return JniConverter::ToJLongPtr(pt);
}

JNIEXPORT jdouble JNICALL Java_org_ogc_chart_api_geometry_Point_nativeGetX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Point* pt = static_cast<ogc::geom::Point*>(
        JniConverter::FromJLongPtr(ptr));
    return pt->GetX();
}

JNIEXPORT jdouble JNICALL Java_org_ogc_chart_api_geometry_Point_nativeGetY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Point* pt = static_cast<ogc::geom::Point*>(
        JniConverter::FromJLongPtr(ptr));
    return pt->GetY();
}

JNIEXPORT void JNICALL Java_org_ogc_chart_api_geometry_Point_nativeDispose
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::geom::Point* pt = static_cast<ogc::geom::Point*>(
        JniConverter::FromJLongPtr(ptr));
    delete pt;
}
```

### 4.6 LineString.java

```java
package cn.cycle.chart.api.geometry;

import java.util.List;
import java.util.ArrayList;

public class LineString extends Geometry {
    
    public LineString() {
        setNativePtr(nativeCreate());
    }
    
    public LineString(List<Coordinate> coordinates) {
        setNativePtr(nativeCreate());
        for (Coordinate coord : coordinates) {
            addPoint(coord.getX(), coord.getY());
        }
    }
    
    @Override
    public Type getType() {
        return Type.LINESTRING;
    }
    
    public void addPoint(double x, double y) {
        checkNotDisposed();
        nativeAddPoint(getNativePtr(), x, y);
    }
    
    public void addPoint(Coordinate coord) {
        addPoint(coord.getX(), coord.getY());
    }
    
    public int getNumPoints() {
        checkNotDisposed();
        return nativeGetNumPoints(getNativePtr());
    }
    
    public Coordinate getCoordinateN(int n) {
        checkNotDisposed();
        return nativeGetCoordinateN(getNativePtr(), n);
    }
    
    public List<Coordinate> getCoordinates() {
        List<Coordinate> coords = new ArrayList<>();
        int n = getNumPoints();
        for (int i = 0; i < n; i++) {
            coords.add(getCoordinateN(i));
        }
        return coords;
    }
    
    public double getLength() {
        checkNotDisposed();
        return nativeGetLength(getNativePtr());
    }
    
    public boolean isClosed() {
        checkNotDisposed();
        return nativeIsClosed(getNativePtr());
    }
    
    public Point getStartPoint() {
        checkNotDisposed();
        long ptr = nativeGetStartPoint(getNativePtr());
        return ptr != 0 ? new Point(ptr) : null;
    }
    
    public Point getEndPoint() {
        checkNotDisposed();
        long ptr = nativeGetEndPoint(getNativePtr());
        return ptr != 0 ? new Point(ptr) : null;
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeAddPoint(long ptr, double x, double y);
    private native int nativeGetNumPoints(long ptr);
    private native Coordinate nativeGetCoordinateN(long ptr, int n);
    private native double nativeGetLength(long ptr);
    private native boolean nativeIsClosed(long ptr);
    private native long nativeGetStartPoint(long ptr);
    private native long nativeGetEndPoint(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.7 Polygon.java

```java
package cn.cycle.chart.api.geometry;

import java.util.List;
import java.util.ArrayList;

public class Polygon extends Geometry {
    
    public Polygon() {
        setNativePtr(nativeCreate());
    }
    
    public Polygon(LineString exteriorRing) {
        setNativePtr(nativeCreate());
        setExteriorRing(exteriorRing);
    }
    
    @Override
    public Type getType() {
        return Type.POLYGON;
    }
    
    public void setExteriorRing(LineString ring) {
        checkNotDisposed();
        nativeSetExteriorRing(getNativePtr(), ring.getNativePtr());
    }
    
    public LineString getExteriorRing() {
        checkNotDisposed();
        long ptr = nativeGetExteriorRing(getNativePtr());
        return ptr != 0 ? new LineString(ptr) : null;
    }
    
    public void addInteriorRing(LineString ring) {
        checkNotDisposed();
        nativeAddInteriorRing(getNativePtr(), ring.getNativePtr());
    }
    
    public LineString getInteriorRingN(int n) {
        checkNotDisposed();
        long ptr = nativeGetInteriorRingN(getNativePtr(), n);
        return ptr != 0 ? new LineString(ptr) : null;
    }
    
    public int getNumInteriorRings() {
        checkNotDisposed();
        return nativeGetNumInteriorRings(getNativePtr());
    }
    
    public double getArea() {
        checkNotDisposed();
        return nativeGetArea(getNativePtr());
    }
    
    public double getPerimeter() {
        checkNotDisposed();
        return nativeGetPerimeter(getNativePtr());
    }
    
    public Point getCentroid() {
        checkNotDisposed();
        long ptr = nativeGetCentroid(getNativePtr());
        return ptr != 0 ? new Point(ptr) : null;
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetExteriorRing(long ptr, long ringPtr);
    private native long nativeGetExteriorRing(long ptr);
    private native void nativeAddInteriorRing(long ptr, long ringPtr);
    private native long nativeGetInteriorRingN(long ptr, int n);
    private native int nativeGetNumInteriorRings(long ptr);
    private native double nativeGetArea(long ptr);
    private native double nativeGetPerimeter(long ptr);
    private native long nativeGetCentroid(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.8 Envelope.java

```java
package cn.cycle.chart.api.geometry;

public class Envelope {
    
    private final double minX, minY, maxX, maxY;
    
    public Envelope(double minX, double minY, double maxX, double maxY) {
        this.minX = minX;
        this.minY = minY;
        this.maxX = maxX;
        this.maxY = maxY;
    }
    
    public double getMinX() { return minX; }
    public double getMinY() { return minY; }
    public double getMaxX() { return maxX; }
    public double getMaxY() { return maxY; }
    
    public double getWidth() { return maxX - minX; }
    public double getHeight() { return maxY - minY; }
    
    public double getCenterX() { return (minX + maxX) / 2; }
    public double getCenterY() { return (minY + maxY) / 2; }
    
    public boolean contains(double x, double y) {
        return x >= minX && x <= maxX && y >= minY && y <= maxY;
    }
    
    public boolean contains(Envelope other) {
        return other.minX >= minX && other.maxX <= maxX &&
               other.minY >= minY && other.maxY <= maxY;
    }
    
    public boolean intersects(Envelope other) {
        return !(other.maxX < minX || other.minX > maxX ||
                 other.maxY < minY || other.minY > maxY);
    }
    
    public Envelope intersection(Envelope other) {
        if (!intersects(other)) return null;
        return new Envelope(
            Math.max(minX, other.minX),
            Math.max(minY, other.minY),
            Math.min(maxX, other.maxX),
            Math.min(maxY, other.maxY)
        );
    }
    
    public Envelope expand(double distance) {
        return new Envelope(minX - distance, minY - distance,
                           maxX + distance, maxY + distance);
    }
    
    public Coordinate getCenter() {
        return new Coordinate(getCenterX(), getCenterY());
    }
    
    @Override
    public String toString() {
        return String.format("Envelope[%.6f, %.6f, %.6f, %.6f]", minX, minY, maxX, maxY);
    }
}
```

### 4.9 Coordinate.java

```java
package cn.cycle.chart.api.geometry;

public class Coordinate {
    
    private final double x, y, z;
    private final boolean hasZ;
    
    public Coordinate(double x, double y) {
        this.x = x;
        this.y = y;
        this.z = Double.NaN;
        this.hasZ = false;
    }
    
    public Coordinate(double x, double y, double z) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.hasZ = true;
    }
    
    public double getX() { return x; }
    public double getY() { return y; }
    public double getZ() { return z; }
    public boolean hasZ() { return hasZ; }
    
    public double distance(Coordinate other) {
        double dx = x - other.x;
        double dy = y - other.y;
        return Math.sqrt(dx * dx + dy * dy);
    }
    
    public double distance3D(Coordinate other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    @Override
    public String toString() {
        if (hasZ) {
            return String.format("(%.6f, %.6f, %.6f)", x, y, z);
        }
        return String.format("(%.6f, %.6f)", x, y);
    }
}
```

### 4.10 FeatureInfo.java

```java
package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Geometry;
import java.util.Map;
import java.util.HashMap;
import java.util.List;

public class FeatureInfo extends NativeObject {
    
    private Map<String, FieldValue> attributes;
    
    public FeatureInfo(long nativePtr) {
        setNativePtr(nativePtr);
        loadAttributes();
    }
    
    private void loadAttributes() {
        attributes = new HashMap<>();
        int fieldCount = nativeGetFieldCount(getNativePtr());
        for (int i = 0; i < fieldCount; i++) {
            String name = nativeGetFieldName(getNativePtr(), i);
            FieldValue value = nativeGetFieldValue(getNativePtr(), i);
            attributes.put(name, value);
        }
    }
    
    public long getFeatureId() {
        checkNotDisposed();
        return nativeGetFeatureId(getNativePtr());
    }
    
    public String getFeatureName() {
        checkNotDisposed();
        return nativeGetFeatureName(getNativePtr());
    }
    
    public String getFeatureClass() {
        checkNotDisposed();
        return nativeGetFeatureClass(getNativePtr());
    }
    
    public Geometry getGeometry() {
        checkNotDisposed();
        long ptr = nativeGetGeometry(getNativePtr());
        if (ptr == 0) return null;
        return GeometryFactory.create(ptr);
    }
    
    public int getFieldCount() {
        return attributes.size();
    }
    
    public Map<String, FieldValue> getAttributes() {
        return new HashMap<>(attributes);
    }
    
    public FieldValue getAttribute(String name) {
        return attributes.get(name);
    }
    
    public String getStringAttribute(String name) {
        FieldValue value = attributes.get(name);
        return value != null ? value.asString() : null;
    }
    
    public double getDoubleAttribute(String name) {
        FieldValue value = attributes.get(name);
        return value != null ? value.asDouble() : Double.NaN;
    }
    
    public int getIntAttribute(String name) {
        FieldValue value = attributes.get(name);
        return value != null ? value.asInt() : 0;
    }
    
    // Native方法
    private native long nativeGetFeatureId(long ptr);
    private native String nativeGetFeatureName(long ptr);
    private native String nativeGetFeatureClass(long ptr);
    private native long nativeGetGeometry(long ptr);
    private native int nativeGetFieldCount(long ptr);
    private native String nativeGetFieldName(long ptr, int index);
    private native FieldValue nativeGetFieldValue(long ptr, int index);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.11 FieldValue.java

```java
package cn.cycle.chart.api.feature;

public class FieldValue {
    
    public enum Type {
        INTEGER,
        DOUBLE,
        STRING,
        DATE,
        BINARY,
        NULL
    }
    
    private final Type type;
    private final Object value;
    
    public FieldValue(Type type, Object value) {
        this.type = type;
        this.value = value;
    }
    
    public Type getType() { return type; }
    public Object getValue() { return value; }
    
    public boolean isNull() { return type == Type.NULL; }
    
    public int asInt() {
        if (type == Type.INTEGER) return (Integer) value;
        if (type == Type.DOUBLE) return ((Double) value).intValue();
        return 0;
    }
    
    public double asDouble() {
        if (type == Type.DOUBLE) return (Double) value;
        if (type == Type.INTEGER) return ((Integer) value).doubleValue();
        return Double.NaN;
    }
    
    public String asString() {
        if (type == Type.STRING) return (String) value;
        return value != null ? value.toString() : null;
    }
    
    @Override
    public String toString() {
        return isNull() ? "NULL" : value.toString();
    }
}
```

### 4.12 ChartLayer.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.symbology.DrawStyle;

public class ChartLayer extends NativeObject {
    
    public enum LayerType {
        VECTOR,
        RASTER,
        GROUP
    }
    
    public ChartLayer(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public LayerType getLayerType() {
        checkNotDisposed();
        return LayerType.values()[nativeGetLayerType(getNativePtr())];
    }
    
    public boolean isVisible() {
        checkNotDisposed();
        return nativeIsVisible(getNativePtr());
    }
    
    public void setVisible(boolean visible) {
        checkNotDisposed();
        nativeSetVisible(getNativePtr(), visible);
    }
    
    public boolean isSelectable() {
        checkNotDisposed();
        return nativeIsSelectable(getNativePtr());
    }
    
    public void setSelectable(boolean selectable) {
        checkNotDisposed();
        nativeSetSelectable(getNativePtr(), selectable);
    }
    
    public double getOpacity() {
        checkNotDisposed();
        return nativeGetOpacity(getNativePtr());
    }
    
    public void setOpacity(double opacity) {
        checkNotDisposed();
        nativeSetOpacity(getNativePtr(), opacity);
    }
    
    public int getZOrder() {
        checkNotDisposed();
        return nativeGetZOrder(getNativePtr());
    }
    
    public void setZOrder(int order) {
        checkNotDisposed();
        nativeSetZOrder(getNativePtr(), order);
    }
    
    public DrawStyle getStyle() {
        checkNotDisposed();
        return nativeGetStyle(getNativePtr());
    }
    
    public void setStyle(DrawStyle style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style);
    }
    
    // Native方法
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetLayerType(long ptr);
    private native boolean nativeIsVisible(long ptr);
    private native void nativeSetVisible(long ptr, boolean visible);
    private native boolean nativeIsSelectable(long ptr);
    private native void nativeSetSelectable(long ptr, boolean selectable);
    private native double nativeGetOpacity(long ptr);
    private native void nativeSetOpacity(long ptr, double opacity);
    private native int nativeGetZOrder(long ptr);
    private native void nativeSetZOrder(long ptr, int order);
    private native DrawStyle nativeGetStyle(long ptr);
    private native void nativeSetStyle(long ptr, DrawStyle style);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.13 LayerManager.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class LayerManager extends NativeObject {
    
    public LayerManager(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public int getLayerCount() {
        checkNotDisposed();
        return nativeGetLayerCount(getNativePtr());
    }
    
    public ChartLayer getLayer(int index) {
        checkNotDisposed();
        long ptr = nativeGetLayer(getNativePtr(), index);
        return ptr != 0 ? new ChartLayer(ptr) : null;
    }
    
    public ChartLayer getLayerByName(String name) {
        checkNotDisposed();
        long ptr = nativeGetLayerByName(getNativePtr(), name);
        return ptr != 0 ? new ChartLayer(ptr) : null;
    }
    
    public List<ChartLayer> getLayers() {
        List<ChartLayer> layers = new ArrayList<>();
        int count = getLayerCount();
        for (int i = 0; i < count; i++) {
            layers.add(getLayer(i));
        }
        return layers;
    }
    
    public void addLayer(ChartLayer layer) {
        checkNotDisposed();
        nativeAddLayer(getNativePtr(), layer.getNativePtr());
    }
    
    public void removeLayer(ChartLayer layer) {
        checkNotDisposed();
        nativeRemoveLayer(getNativePtr(), layer.getNativePtr());
    }
    
    public void removeLayer(int index) {
        checkNotDisposed();
        nativeRemoveLayerAt(getNativePtr(), index);
    }
    
    public void moveLayer(int fromIndex, int toIndex) {
        checkNotDisposed();
        nativeMoveLayer(getNativePtr(), fromIndex, toIndex);
    }
    
    public void clearLayers() {
        checkNotDisposed();
        nativeClearLayers(getNativePtr());
    }
    
    // Native方法
    private native int nativeGetLayerCount(long ptr);
    private native long nativeGetLayer(long ptr, int index);
    private native long nativeGetLayerByName(long ptr, String name);
    private native void nativeAddLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayerAt(long ptr, int index);
    private native void nativeMoveLayer(long ptr, int from, int to);
    private native void nativeClearLayers(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.14 TileCache.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class TileCache extends NativeObject {
    
    public TileCache() {
        setNativePtr(nativeCreate());
    }
    
    public TileCache(long maxSize) {
        setNativePtr(nativeCreateWithSize(maxSize));
    }
    
    public void put(TileKey key, byte[] data) {
        checkNotDisposed();
        nativePut(getNativePtr(), key.getX(), key.getY(), key.getLevel(), data);
    }
    
    public byte[] get(TileKey key) {
        checkNotDisposed();
        return nativeGet(getNativePtr(), key.getX(), key.getY(), key.getLevel());
    }
    
    public boolean contains(TileKey key) {
        checkNotDisposed();
        return nativeContains(getNativePtr(), key.getX(), key.getY(), key.getLevel());
    }
    
    public void remove(TileKey key) {
        checkNotDisposed();
        nativeRemove(getNativePtr(), key.getX(), key.getY(), key.getLevel());
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public long getSize() {
        checkNotDisposed();
        return nativeGetSize(getNativePtr());
    }
    
    public long getMaxSize() {
        checkNotDisposed();
        return nativeGetMaxSize(getNativePtr());
    }
    
    public void setMaxSize(long maxSize) {
        checkNotDisposed();
        nativeSetMaxSize(getNativePtr(), maxSize);
    }
    
    public double getHitRate() {
        checkNotDisposed();
        return nativeGetHitRate(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native long nativeCreateWithSize(long maxSize);
    private native void nativePut(long ptr, int x, int y, int level, byte[] data);
    private native byte[] nativeGet(long ptr, int x, int y, int level);
    private native boolean nativeContains(long ptr, int x, int y, int level);
    private native void nativeRemove(long ptr, int x, int y, int level);
    private native void nativeClear(long ptr);
    private native long nativeGetSize(long ptr);
    private native long nativeGetMaxSize(long ptr);
    private native void nativeSetMaxSize(long ptr, long maxSize);
    private native double nativeGetHitRate(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 4.15 TileKey.java

```java
package cn.cycle.chart.api.cache;

public class TileKey {
    
    private final int x, y, level;
    
    public TileKey(int x, int y, int level) {
        this.x = x;
        this.y = y;
        this.level = level;
    }
    
    public int getX() { return x; }
    public int getY() { return y; }
    public int getLevel() { return level; }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (!(obj instanceof TileKey)) return false;
        TileKey other = (TileKey) obj;
        return x == other.x && y == other.y && level == other.level;
    }
    
    @Override
    public int hashCode() {
        return 31 * (31 * x + y) + level;
    }
    
    @Override
    public String toString() {
        return String.format("TileKey[%d, %d, %d]", x, y, level);
    }
}
```

### 4.16 DrawStyle.java

```java
package cn.cycle.chart.api.symbology;

import cn.cycle.chart.api.util.Color;

public class DrawStyle {
    
    private Color strokeColor;
    private Color fillColor;
    private double strokeWidth;
    private double opacity;
    private String lineCap;
    private String lineJoin;
    private double[] dashArray;
    
    public DrawStyle() {
        this.strokeColor = new Color(0, 0, 0);
        this.fillColor = new Color(255, 255, 255);
        this.strokeWidth = 1.0;
        this.opacity = 1.0;
        this.lineCap = "round";
        this.lineJoin = "round";
    }
    
    // Getters and Setters
    public Color getStrokeColor() { return strokeColor; }
    public void setStrokeColor(Color strokeColor) { this.strokeColor = strokeColor; }
    
    public Color getFillColor() { return fillColor; }
    public void setFillColor(Color fillColor) { this.fillColor = fillColor; }
    
    public double getStrokeWidth() { return strokeWidth; }
    public void setStrokeWidth(double strokeWidth) { this.strokeWidth = strokeWidth; }
    
    public double getOpacity() { return opacity; }
    public void setOpacity(double opacity) { this.opacity = opacity; }
    
    public String getLineCap() { return lineCap; }
    public void setLineCap(String lineCap) { this.lineCap = lineCap; }
    
    public String getLineJoin() { return lineJoin; }
    public void setLineJoin(String lineJoin) { this.lineJoin = lineJoin; }
    
    public double[] getDashArray() { return dashArray; }
    public void setDashArray(double[] dashArray) { this.dashArray = dashArray; }
    
    // 预定义样式
    public static DrawStyle defaultLine() {
        DrawStyle style = new DrawStyle();
        style.setFillOpacity(0);
        return style;
    }
    
    public static DrawStyle defaultPolygon() {
        DrawStyle style = new DrawStyle();
        style.setStrokeColor(new Color(0, 0, 0));
        style.setFillColor(new Color(200, 200, 200));
        return style;
    }
    
    public static DrawStyle defaultPoint() {
        DrawStyle style = new DrawStyle();
        style.setStrokeColor(new Color(0, 0, 0));
        style.setFillColor(new Color(255, 0, 0));
        return style;
    }
    
    public void setFillOpacity(double opacity) {
        this.fillColor = new Color(fillColor.getRed(), fillColor.getGreen(), 
                                   fillColor.getBlue(), (int)(opacity * 255));
    }
}
```

### 4.17 Color.java

```java
package cn.cycle.chart.api.util;

public class Color {
    
    private final int red, green, blue, alpha;
    
    public Color(int red, int green, int blue) {
        this(red, green, blue, 255);
    }
    
    public Color(int red, int green, int blue, int alpha) {
        this.red = clamp(red);
        this.green = clamp(green);
        this.blue = clamp(blue);
        this.alpha = clamp(alpha);
    }
    
    public Color(int rgb) {
        this.alpha = 255;
        this.red = (rgb >> 16) & 0xFF;
        this.green = (rgb >> 8) & 0xFF;
        this.blue = rgb & 0xFF;
    }
    
    public int getRed() { return red; }
    public int getGreen() { return green; }
    public int getBlue() { return blue; }
    public int getAlpha() { return alpha; }
    
    public int getRGB() {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
    
    public int getARGB() {
        return getRGB();
    }
    
    public String getHex() {
        return String.format("#%02X%02X%02X", red, green, blue);
    }
    
    public String getHexWithAlpha() {
        return String.format("#%02X%02X%02X%02X", alpha, red, green, blue);
    }
    
    public javafx.scene.paint.Color toJavaFXColor() {
        return new javafx.scene.paint.Color(
            red / 255.0, green / 255.0, blue / 255.0, alpha / 255.0);
    }
    
    public static Color fromJavaFXColor(javafx.scene.paint.Color fxColor) {
        return new Color(
            (int)(fxColor.getRed() * 255),
            (int)(fxColor.getGreen() * 255),
            (int)(fxColor.getBlue() * 255),
            (int)(fxColor.getOpacity() * 255)
        );
    }
    
    private static int clamp(int value) {
        return Math.max(0, Math.min(255, value));
    }
    
    // 预定义颜色
    public static final Color BLACK = new Color(0, 0, 0);
    public static final Color WHITE = new Color(255, 255, 255);
    public static final Color RED = new Color(255, 0, 0);
    public static final Color GREEN = new Color(0, 255, 0);
    public static final Color BLUE = new Color(0, 0, 255);
    public static final Color TRANSPARENT = new Color(0, 0, 0, 0);
}
```

---

## 5. JavaFX适配层接口

### 5.1 CanvasAdapter.java

```java
package cn.cycle.chart.adapter.canvas;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import cn.cycle.chart.api.core.ChartViewer;

public class CanvasAdapter {
    
    private final Canvas canvas;
    private final ChartViewer viewer;
    private final ImageDevice imageDevice;
    
    public CanvasAdapter(Canvas canvas, ChartViewer viewer) {
        this.canvas = canvas;
        this.viewer = viewer;
        this.imageDevice = new ImageDevice();
    }
    
    public void render() {
        GraphicsContext gc = canvas.getGraphicsContext2D();
        double width = canvas.getWidth();
        double height = canvas.getHeight();
        
        // 清空画布
        gc.clearRect(0, 0, width, height);
        
        // 调用Native渲染
        viewer.render(gc, width, height);
    }
    
    public void resize(double width, double height) {
        canvas.setWidth(width);
        canvas.setHeight(height);
        render();
    }
    
    public Canvas getCanvas() {
        return canvas;
    }
    
    public ChartViewer getViewer() {
        return viewer;
    }
    
    public Image captureImage() {
        return canvas.snapshot(null, null);
    }
    
    public void setOnRenderComplete(Runnable callback) {
        imageDevice.setOnRenderComplete(callback);
    }
}
```

### 5.2 ImageDevice.java

```java
package cn.cycle.chart.adapter.canvas;

import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.canvas.GraphicsContext;

public class ImageDevice {
    
    private Runnable onRenderComplete;
    
    public Image createImage(int width, int height) {
        return new WritableImage(width, height);
    }
    
    public void writePixels(WritableImage image, int[] pixels, int x, int y, int width, int height) {
        PixelWriter writer = image.getPixelWriter();
        writer.setPixels(x, y, width, height, 
                        javafx.scene.image.PixelFormat.getIntArgbInstance(),
                        pixels, 0, width);
    }
    
    public void drawImage(GraphicsContext gc, Image image, double x, double y) {
        gc.drawImage(image, x, y);
    }
    
    public void drawImage(GraphicsContext gc, Image image, 
                         double sx, double sy, double sw, double sh,
                         double dx, double dy, double dw, double dh) {
        gc.drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh);
    }
    
    public void setOnRenderComplete(Runnable callback) {
        this.onRenderComplete = callback;
    }
    
    public void notifyRenderComplete() {
        if (onRenderComplete != null) {
            onRenderComplete.run();
        }
    }
}
```

### 5.3 ChartEventHandler.java

```java
package cn.cycle.chart.adapter.event;

import javafx.scene.Node;
import javafx.scene.input.*;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.function.Consumer;

public class ChartEventHandler {
    
    private final ChartViewer viewer;
    private final Node node;
    
    private Consumer<FeatureInfo> onFeatureSelected;
    private Consumer<Coordinate> onCoordinateClicked;
    private Consumer<MouseEvent> onMouseMoved;
    
    public ChartEventHandler(Node node, ChartViewer viewer) {
        this.node = node;
        this.viewer = viewer;
        setupHandlers();
    }
    
    private void setupHandlers() {
        node.setOnMouseClicked(this::handleMouseClicked);
        node.setOnMousePressed(this::handleMousePressed);
        node.setOnMouseReleased(this::handleMouseReleased);
        node.setOnMouseDragged(this::handleMouseDragged);
        node.setOnMouseMoved(this::handleMouseMoved);
        node.setOnScroll(this::handleScroll);
        node.setOnZoom(this::handleZoom);
        node.setOnRotate(this::handleRotate);
    }
    
    private void handleMouseClicked(MouseEvent event) {
        if (event.getClickCount() == 1) {
            Coordinate worldCoord = viewer.screenToWorld(event.getX(), event.getY());
            if (onCoordinateClicked != null) {
                onCoordinateClicked.accept(worldCoord);
            }
            
            FeatureInfo feature = viewer.queryFeature(event.getX(), event.getY());
            if (feature != null && onFeatureSelected != null) {
                onFeatureSelected.accept(feature);
            }
        }
    }
    
    private void handleMousePressed(MouseEvent event) {
        node.requestFocus();
    }
    
    private void handleMouseReleased(MouseEvent event) {
        // 子类实现
    }
    
    private void handleMouseDragged(MouseEvent event) {
        // 由PanHandler处理
    }
    
    private void handleMouseMoved(MouseEvent event) {
        if (onMouseMoved != null) {
            onMouseMoved.accept(event);
        }
    }
    
    private void handleScroll(ScrollEvent event) {
        double factor = event.getDeltaY() > 0 ? 1.1 : 0.9;
        viewer.zoom(factor, event.getX(), event.getY());
    }
    
    private void handleZoom(ZoomEvent event) {
        viewer.zoom(event.getZoomFactor(), event.getX(), event.getY());
    }
    
    private void handleRotate(RotateEvent event) {
        // 可选：实现旋转
    }
    
    // 设置回调
    public void setOnFeatureSelected(Consumer<FeatureInfo> callback) {
        this.onFeatureSelected = callback;
    }
    
    public void setOnCoordinateClicked(Consumer<Coordinate> callback) {
        this.onCoordinateClicked = callback;
    }
    
    public void setOnMouseMoved(Consumer<MouseEvent> callback) {
        this.onMouseMoved = callback;
    }
}
```

### 5.4 PanHandler.java

```java
package cn.cycle.chart.adapter.event;

import javafx.scene.Node;
import javafx.scene.input.MouseEvent;
import cn.cycle.chart.api.core.ChartViewer;

public class PanHandler {
    
    private final ChartViewer viewer;
    private final Node node;
    
    private double lastX, lastY;
    private boolean panning = false;
    
    public PanHandler(Node node, ChartViewer viewer) {
        this.node = node;
        this.viewer = viewer;
        setupHandlers();
    }
    
    private void setupHandlers() {
        node.setOnMousePressed(this::onMousePressed);
        node.setOnMouseDragged(this::onMouseDragged);
        node.setOnMouseReleased(this::onMouseReleased);
    }
    
    private void onMousePressed(MouseEvent event) {
        if (event.isPrimaryButtonDown()) {
            lastX = event.getX();
            lastY = event.getY();
            panning = true;
        }
    }
    
    private void onMouseDragged(MouseEvent event) {
        if (panning && event.isPrimaryButtonDown()) {
            double dx = event.getX() - lastX;
            double dy = event.getY() - lastY;
            
            viewer.pan(-dx, -dy);
            
            lastX = event.getX();
            lastY = event.getY();
        }
    }
    
    private void onMouseReleased(MouseEvent event) {
        panning = false;
    }
}
```

### 5.5 ZoomHandler.java

```java
package cn.cycle.chart.adapter.event;

import javafx.scene.Node;
import javafx.scene.input.ScrollEvent;
import javafx.scene.input.ZoomEvent;
import cn.cycle.chart.api.core.ChartViewer;

public class ZoomHandler {
    
    private final ChartViewer viewer;
    private final Node node;
    
    private double minScale = 0.001;
    private double maxScale = 100000.0;
    
    public ZoomHandler(Node node, ChartViewer viewer) {
        this.node = node;
        this.viewer = viewer;
        setupHandlers();
    }
    
    private void setupHandlers() {
        node.setOnScroll(this::onScroll);
        node.setOnZoom(this::onZoom);
    }
    
    private void onScroll(ScrollEvent event) {
        double factor = event.getDeltaY() > 0 ? 1.1 : 0.9;
        double currentScale = viewer.getScale();
        double newScale = currentScale * factor;
        
        if (newScale >= minScale && newScale <= maxScale) {
            viewer.zoom(factor, event.getX(), event.getY());
        }
        
        event.consume();
    }
    
    private void onZoom(ZoomEvent event) {
        double factor = event.getZoomFactor();
        double currentScale = viewer.getScale();
        double newScale = currentScale * factor;
        
        if (newScale >= minScale && newScale <= maxScale) {
            viewer.zoom(factor, event.getX(), event.getY());
        }
        
        event.consume();
    }
    
    public void setScaleLimits(double minScale, double maxScale) {
        this.minScale = minScale;
        this.maxScale = maxScale;
    }
}
```

### 5.6 ThreadBridge.java

```java
package cn.cycle.chart.adapter.binding;

import javafx.application.Platform;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;
import java.util.function.Supplier;

public class ThreadBridge {
    
    private final ExecutorService backgroundExecutor;
    
    public ThreadBridge() {
        this.backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors());
    }
    
    public ThreadBridge(int threadCount) {
        this.backgroundExecutor = Executors.newFixedThreadPool(threadCount);
    }
    
    public <T> void runAsync(Supplier<T> backgroundTask, Consumer<T> uiCallback) {
        backgroundExecutor.submit(() -> {
            T result = backgroundTask.get();
            Platform.runLater(() -> uiCallback.accept(result));
        });
    }
    
    public void runAsync(Runnable backgroundTask, Runnable uiCallback) {
        backgroundExecutor.submit(() -> {
            backgroundTask.run();
            Platform.runLater(uiCallback);
        });
    }
    
    public void runInBackground(Runnable task) {
        backgroundExecutor.submit(task);
    }
    
    public void runOnUIThread(Runnable task) {
        if (Platform.isFxApplicationThread()) {
            task.run();
        } else {
            Platform.runLater(task);
        }
    }
    
    public void shutdown() {
        backgroundExecutor.shutdown();
    }
}
```

### 5.7 PropertyBinder.java

```java
package cn.cycle.chart.adapter.binding;

import javafx.beans.property.*;
import javafx.beans.value.ChangeListener;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.layer.ChartLayer;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class PropertyBinder {
    
    private final ChartViewer viewer;
    private final Map<String, ChangeListener<?>> listeners = new ConcurrentHashMap<>();
    
    public PropertyBinder(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    public void bindScale(DoubleProperty property) {
        property.set(viewer.getScale());
        ChangeListener<Number> listener = (obs, oldVal, newVal) -> {
            viewer.setScale(newVal.doubleValue());
        };
        property.addListener(listener);
        listeners.put("scale", listener);
    }
    
    public void bindLayerVisible(BooleanProperty property, ChartLayer layer) {
        property.set(layer.isVisible());
        ChangeListener<Boolean> listener = (obs, oldVal, newVal) -> {
            layer.setVisible(newVal);
        };
        property.addListener(listener);
        listeners.put("layer_" + layer.getName(), listener);
    }
    
    public void bindLayerOpacity(DoubleProperty property, ChartLayer layer) {
        property.set(layer.getOpacity());
        ChangeListener<Number> listener = (obs, oldVal, newVal) -> {
            layer.setOpacity(newVal.doubleValue());
        };
        property.addListener(listener);
        listeners.put("opacity_" + layer.getName(), listener);
    }
    
    public void unbindAll() {
        listeners.clear();
    }
}
```

---

## 6. JavaFX应用层接口

### 6.1 ChartCanvas.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.canvas.Canvas;
import javafx.scene.layout.StackPane;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.ChartConfig;
import cn.cycle.chart.adapter.canvas.CanvasAdapter;
import cn.cycle.chart.adapter.event.*;

public class ChartCanvas extends StackPane {
    
    private final Canvas canvas;
    private final ChartViewer viewer;
    private final CanvasAdapter adapter;
    private ChartEventHandler eventHandler;
    private PanHandler panHandler;
    private ZoomHandler zoomHandler;
    
    public ChartCanvas() {
        this.canvas = new Canvas();
        this.viewer = new ChartViewer();
        this.adapter = new CanvasAdapter(canvas, viewer);
        
        getChildren().add(canvas);
        setupCanvas();
        setupEventHandlers();
    }
    
    public ChartCanvas(ChartConfig config) {
        this.canvas = new Canvas();
        this.viewer = new ChartViewer(config);
        this.adapter = new CanvasAdapter(canvas, viewer);
        
        getChildren().add(canvas);
        setupCanvas();
        setupEventHandlers();
    }
    
    private void setupCanvas() {
        canvas.widthProperty().bind(widthProperty());
        canvas.heightProperty().bind(heightProperty());
        
        widthProperty().addListener((obs, oldVal, newVal) -> {
            adapter.resize(newVal.doubleValue(), canvas.getHeight());
        });
        
        heightProperty().addListener((obs, oldVal, newVal) -> {
            adapter.resize(canvas.getWidth(), newVal.doubleValue());
        });
    }
    
    private void setupEventHandlers() {
        eventHandler = new ChartEventHandler(this, viewer);
        panHandler = new PanHandler(this, viewer);
        zoomHandler = new ZoomHandler(this, viewer);
        
        setFocusTraversable(true);
    }
    
    public void loadChart(String path) {
        viewer.loadChart(path);
        adapter.render();
    }
    
    public void loadChartAsync(String path, Runnable callback) {
        viewer.loadChartAsync(path, success -> {
            if (success) {
                adapter.render();
                if (callback != null) callback.run();
            }
        });
    }
    
    public void render() {
        adapter.render();
    }
    
    public ChartViewer getViewer() {
        return viewer;
    }
    
    public ChartEventHandler getEventHandler() {
        return eventHandler;
    }
    
    public void dispose() {
        viewer.close();
    }
}
```

### 6.2 MainView.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.scene.input.*;
import cn.cycle.chart.api.core.ChartConfig;
import cn.cycle.chart.app.controller.MainController;

public class MainView extends BorderPane {
    
    private final MainController controller;
    private final ChartCanvas chartCanvas;
    private final LayerPanel layerPanel;
    private final PositionView positionView;
    private final AlertPanel alertPanel;
    private final MenuBar menuBar;
    private final ToolBar toolBar;
    private final StatusBar statusBar;
    
    public MainView() {
        this.controller = new MainController(this);
        this.chartCanvas = new ChartCanvas(createDefaultConfig());
        this.layerPanel = new LayerPanel(chartCanvas.getViewer());
        this.positionView = new PositionView(chartCanvas.getViewer());
        this.alertPanel = new AlertPanel();
        
        this.menuBar = createMenuBar();
        this.toolBar = createToolBar();
        this.statusBar = new StatusBar();
        
        setupLayout();
        setupEventHandlers();
    }
    
    private ChartConfig createDefaultConfig() {
        ChartConfig config = new ChartConfig();
        config.setCachePath(System.getProperty("user.home") + "/.ogc_chart/cache");
        config.setMaxCacheSize(512 * 1024 * 1024);
        config.setThreadCount(4);
        config.setEnableAntialiasing(true);
        return config;
    }
    
    private void setupLayout() {
        setTop(new VBox(menuBar, toolBar));
        setCenter(chartCanvas);
        
        VBox rightPanel = new VBox(layerPanel, alertPanel);
        rightPanel.setPrefWidth(250);
        setRight(rightPanel);
        
        setBottom(statusBar);
        setLeft(positionView);
    }
    
    private MenuBar createMenuBar() {
        MenuBar menuBar = new MenuBar();
        
        Menu fileMenu = new Menu("文件");
        fileMenu.getItems().addAll(
            createMenuItem("打开海图", controller::openChart, KeyCombination.CONTROL_DOWN, KeyCode.O),
            createMenuItem("关闭海图", controller::closeChart),
            new SeparatorMenuItem(),
            createMenuItem("导出图像", controller::exportImage, KeyCombination.CONTROL_DOWN, KeyCode.E),
            new SeparatorMenuItem(),
            createMenuItem("退出", controller::exit, KeyCombination.CONTROL_DOWN, KeyCode.Q)
        );
        
        Menu viewMenu = new Menu("视图");
        viewMenu.getItems().addAll(
            createMenuItem("放大", controller::zoomIn, KeyCombination.CONTROL_DOWN, KeyCode.PLUS),
            createMenuItem("缩小", controller::zoomOut, KeyCombination.CONTROL_DOWN, KeyCode.MINUS),
            createMenuItem("重置视图", controller::resetView, KeyCombination.CONTROL_DOWN, KeyCode.R)
        );
        
        Menu helpMenu = new Menu("帮助");
        helpMenu.getItems().addAll(
            createMenuItem("关于", controller::showAbout)
        );
        
        menuBar.getMenus().addAll(fileMenu, viewMenu, helpMenu);
        return menuBar;
    }
    
    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        
        Button openBtn = new Button("打开");
        openBtn.setOnAction(e -> controller.openChart());
        
        Button zoomInBtn = new Button("+");
        zoomInBtn.setOnAction(e -> controller.zoomIn());
        
        Button zoomOutBtn = new Button("-");
        zoomOutBtn.setOnAction(e -> controller.zoomOut());
        
        Button panBtn = new Button("平移");
        panBtn.setOnAction(e -> controller.enablePanMode());
        
        Button selectBtn = new Button("选择");
        selectBtn.setOnAction(e -> controller.enableSelectMode());
        
        toolBar.getItems().addAll(openBtn, new Separator(), 
            zoomInBtn, zoomOutBtn, panBtn, selectBtn);
        
        return toolBar;
    }
    
    private MenuItem createMenuItem(String text, Runnable action) {
        MenuItem item = new MenuItem(text);
        item.setOnAction(e -> action.run());
        return item;
    }
    
    private MenuItem createMenuItem(String text, Runnable action, 
                                   KeyCombination.Modifier modifier, KeyCode key) {
        MenuItem item = new MenuItem(text);
        item.setOnAction(e -> action.run());
        item.setAccelerator(new KeyCodeCombination(key, modifier));
        return item;
    }
    
    private void setupEventHandlers() {
        chartCanvas.getEventHandler().setOnFeatureSelected(feature -> {
            controller.onFeatureSelected(feature);
        });
        
        chartCanvas.getEventHandler().setOnCoordinateClicked(coord -> {
            controller.onCoordinateClicked(coord);
        });
    }
    
    public ChartCanvas getChartCanvas() { return chartCanvas; }
    public LayerPanel getLayerPanel() { return layerPanel; }
    public PositionView getPositionView() { return positionView; }
    public AlertPanel getAlertPanel() { return alertPanel; }
    public StatusBar getStatusBar() { return statusBar; }
}

### 6.3 LayerPanel.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.control.*;
import javafx.scene.layout.*;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.layer.ChartLayer;
import java.util.List;

public class LayerPanel extends VBox {
    
    private final ChartViewer viewer;
    private final ListView<LayerItem> layerList;
    
    public LayerPanel(ChartViewer viewer) {
        this.viewer = viewer;
        this.layerList = new ListView<>();
        
        setupUI();
        loadLayers();
    }
    
    private void setupUI() {
        Label title = new Label("图层管理");
        title.setStyle("-fx-font-weight: bold;");
        
        layerList.setCellFactory(list -> new LayerListCell());
        
        Button refreshBtn = new Button("刷新");
        refreshBtn.setOnAction(e -> loadLayers());
        
        HBox buttonBar = new HBox(5, refreshBtn);
        
        getChildren().addAll(title, layerList, buttonBar);
        setSpacing(10);
        setPadding(new Insets(10));
    }
    
    private void loadLayers() {
        layerList.getItems().clear();
        List<ChartLayer> layers = viewer.getLayerManager().getLayers();
        for (ChartLayer layer : layers) {
            layerList.getItems().add(new LayerItem(layer));
        }
    }
    
    private static class LayerItem {
        final ChartLayer layer;
        LayerItem(ChartLayer layer) { this.layer = layer; }
        @Override public String toString() { return layer.getName(); }
    }
    
    private class LayerListCell extends ListCell<LayerItem> {
        private final CheckBox checkBox = new CheckBox();
        private final Slider opacitySlider = new Slider(0, 1, 1);
        
        public LayerListCell() {
            checkBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
                if (getItem() != null) {
                    getItem().layer.setVisible(newVal);
                }
            });
            
            opacitySlider.valueProperty().addListener((obs, oldVal, newVal) -> {
                if (getItem() != null) {
                    getItem().layer.setOpacity(newVal.doubleValue());
                }
            });
        }
        
        @Override
        protected void updateItem(LayerItem item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setGraphic(null);
            } else {
                checkBox.setText(item.layer.getName());
                checkBox.setSelected(item.layer.isVisible());
                opacitySlider.setValue(item.layer.getOpacity());
                
                VBox content = new VBox(5, checkBox, 
                    new HBox(5, new Label("透明度:"), opacitySlider));
                setGraphic(content);
            }
        }
    }
}
```

### 6.4 PositionView.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.scene.paint.Color;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;

public class PositionView extends VBox {
    
    private final ChartViewer viewer;
    private final Label coordLabel;
    private final Label scaleLabel;
    private final Label centerLabel;
    
    public PositionView(ChartViewer viewer) {
        this.viewer = viewer;
        this.coordLabel = new Label("坐标: --");
        this.scaleLabel = new Label("比例尺: --");
        this.centerLabel = new Label("中心: --");
        
        setupUI();
        setupBindings();
    }
    
    private void setupUI() {
        setSpacing(5);
        setPadding(new Insets(10));
        setStyle("-fx-background-color: #f0f0f0;");
        
        Label title = new Label("位置信息");
        title.setStyle("-fx-font-weight: bold;");
        
        getChildren().addAll(title, coordLabel, scaleLabel, centerLabel);
    }
    
    private void setupBindings() {
        viewer.getEventHandler().setOnMouseMoved(event -> {
            Coordinate coord = viewer.screenToWorld(event.getX(), event.getY());
            if (coord != null) {
                coordLabel.setText(String.format("坐标: %.6f, %.6f", 
                    coord.getX(), coord.getY()));
            }
        });
    }
    
    public void updateScale(double scale) {
        scaleLabel.setText(String.format("比例尺: 1:%.0f", scale));
    }
    
    public void updateCenter(Coordinate center) {
        centerLabel.setText(String.format("中心: %.6f, %.6f", 
            center.getX(), center.getY()));
    }
}
```

### 6.5 AlertPanel.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.collections.*;
import java.util.List;

public class AlertPanel extends VBox {
    
    private final ListView<AlertItem> alertList;
    private final ObservableList<AlertItem> alerts;
    
    public AlertPanel() {
        this.alerts = FXCollections.observableArrayList();
        this.alertList = new ListView<>(alerts);
        
        setupUI();
    }
    
    private void setupUI() {
        Label title = new Label("警报信息");
        title.setStyle("-fx-font-weight: bold;");
        
        alertList.setCellFactory(list -> new AlertListCell());
        alertList.setPrefHeight(150);
        
        Button clearBtn = new Button("清除全部");
        clearBtn.setOnAction(e -> alerts.clear());
        
        HBox buttonBar = new HBox(5, clearBtn);
        
        getChildren().addAll(title, alertList, buttonBar);
        setSpacing(10);
        setPadding(new Insets(10));
    }
    
    public void addAlert(String type, String message, AlertLevel level) {
        alerts.add(0, new AlertItem(type, message, level));
    }
    
    public void clearAlerts() {
        alerts.clear();
    }
    
    public enum AlertLevel {
        INFO, WARNING, ERROR, CRITICAL
    }
    
    public static class AlertItem {
        final String type;
        final String message;
        final AlertLevel level;
        
        AlertItem(String type, String message, AlertLevel level) {
            this.type = type;
            this.message = message;
            this.level = level;
        }
    }
    
    private class AlertListCell extends ListCell<AlertItem> {
        @Override
        protected void updateItem(AlertItem item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setText(null);
                setGraphic(null);
            } else {
                Label icon = new Label(getIcon(item.level));
                Label text = new Label(item.type + ": " + item.message);
                text.setWrapText(true);
                text.setMaxWidth(200);
                
                HBox content = new HBox(5, icon, text);
                content.setStyle(getStyle(item.level));
                setGraphic(content);
            }
        }
        
        private String getIcon(AlertLevel level) {
            switch (level) {
                case INFO: return "ℹ";
                case WARNING: return "⚠";
                case ERROR: return "✗";
                case CRITICAL: return "‼";
                default: return "•";
            }
        }
        
        private String getStyle(AlertLevel level) {
            switch (level) {
                case WARNING: return "-fx-background-color: #fff3cd;";
                case ERROR: return "-fx-background-color: #f8d7da;";
                case CRITICAL: return "-fx-background-color: #dc3545; -fx-text-fill: white;";
                default: return "-fx-background-color: #d1ecf1;";
            }
        }
    }
}
```

---

## 6.4 导航模块接口 (navi)

### 6.4.1 Route.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;
import java.util.ArrayList;

public class Route extends NativeObject {
    
    public enum RouteStatus {
        PLANNING, ACTIVE, COMPLETED, CANCELLED
    }
    
    public Route() {
        setNativePtr(nativeCreate());
    }
    
    public String getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public RouteStatus getStatus() {
        checkNotDisposed();
        return RouteStatus.values()[nativeGetStatus(getNativePtr())];
    }
    
    public double getTotalDistance() {
        checkNotDisposed();
        return nativeGetTotalDistance(getNativePtr());
    }
    
    public int getWaypointCount() {
        checkNotDisposed();
        return nativeGetWaypointCount(getNativePtr());
    }
    
    public Waypoint getWaypoint(int index) {
        checkNotDisposed();
        long ptr = nativeGetWaypoint(getNativePtr(), index);
        return ptr != 0 ? new Waypoint(ptr) : null;
    }
    
    public List<Waypoint> getWaypoints() {
        List<Waypoint> waypoints = new ArrayList<>();
        int count = getWaypointCount();
        for (int i = 0; i < count; i++) {
            waypoints.add(getWaypoint(i));
        }
        return waypoints;
    }
    
    public void addWaypoint(Waypoint waypoint) {
        checkNotDisposed();
        nativeAddWaypoint(getNativePtr(), waypoint.getNativePtr());
    }
    
    public void addWaypoint(Coordinate coord, String name) {
        checkNotDisposed();
        nativeAddWaypointCoord(getNativePtr(), coord.getX(), coord.getY(), name);
    }
    
    public void removeWaypoint(int index) {
        checkNotDisposed();
        nativeRemoveWaypoint(getNativePtr(), index);
    }
    
    public void clearWaypoints() {
        checkNotDisposed();
        nativeClearWaypoints(getNativePtr());
    }
    
    public double getDistanceToNextWaypoint(Coordinate currentPosition) {
        checkNotDisposed();
        return nativeGetDistanceToNextWaypoint(getNativePtr(), 
            currentPosition.getX(), currentPosition.getY());
    }
    
    public Coordinate getNextWaypointPosition() {
        checkNotDisposed();
        return nativeGetNextWaypointPosition(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetStatus(long ptr);
    private native double nativeGetTotalDistance(long ptr);
    private native int nativeGetWaypointCount(long ptr);
    private native long nativeGetWaypoint(long ptr, int index);
    private native void nativeAddWaypoint(long ptr, long waypointPtr);
    private native void nativeAddWaypointCoord(long ptr, double x, double y, String name);
    private native void nativeRemoveWaypoint(long ptr, int index);
    private native void nativeClearWaypoints(long ptr);
    private native double nativeGetDistanceToNextWaypoint(long ptr, double x, double y);
    private native Coordinate nativeGetNextWaypointPosition(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

**对应JNI实现**:

```cpp
// route_jni.cpp
#include "ogc/navi/route/route.h"
#include "ogc/navi/route/waypoint.h"

JNIEXPORT jlong JNICALL Java_org_ogc_chart_api_navi_Route_nativeCreate
  (JNIEnv* env, jobject obj) {
    ogc::navi::Route* route = ogc::navi::Route::Create();
    return JniConverter::ToJLongPtr(route);
}

JNIEXPORT jstring JNICALL Java_org_ogc_chart_api_navi_Route_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::navi::Route* route = static_cast<ogc::navi::Route*>(
        JniConverter::FromJLongPtr(ptr));
    return JniConverter::ToJString(env, route->GetName());
}

JNIEXPORT jdouble JNICALL Java_org_ogc_chart_api_navi_Route_nativeGetTotalDistance
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc::navi::Route* route = static_cast<ogc::navi::Route*>(
        JniConverter::FromJLongPtr(ptr));
    return route->GetTotalDistance();
}
```

### 6.4.2 Waypoint.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public class Waypoint extends NativeObject {
    
    public enum WaypointType {
        NORMAL, ARRIVAL, DEPARTURE, TURN, SAFETY
    }
    
    public Waypoint() {
        setNativePtr(nativeCreate());
    }
    
    public Waypoint(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public Coordinate getPosition() {
        checkNotDisposed();
        return nativeGetPosition(getNativePtr());
    }
    
    public void setPosition(Coordinate coord) {
        checkNotDisposed();
        nativeSetPosition(getNativePtr(), coord.getX(), coord.getY());
    }
    
    public WaypointType getType() {
        checkNotDisposed();
        return WaypointType.values()[nativeGetType(getNativePtr())];
    }
    
    public void setType(WaypointType type) {
        checkNotDisposed();
        nativeSetType(getNativePtr(), type.ordinal());
    }
    
    public double getSpeed() {
        checkNotDisposed();
        return nativeGetSpeed(getNativePtr());
    }
    
    public void setSpeed(double speed) {
        checkNotDisposed();
        nativeSetSpeed(getNativePtr(), speed);
    }
    
    public double getXtdLimit() {
        checkNotDisposed();
        return nativeGetXtdLimit(getNativePtr());
    }
    
    public void setXtdLimit(double limit) {
        checkNotDisposed();
        nativeSetXtdLimit(getNativePtr(), limit);
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native Coordinate nativeGetPosition(long ptr);
    private native void nativeSetPosition(long ptr, double x, double y);
    private native int nativeGetType(long ptr);
    private native void nativeSetType(long ptr, int type);
    private native double nativeGetSpeed(long ptr);
    private native void nativeSetSpeed(long ptr, double speed);
    private native double nativeGetXtdLimit(long ptr);
    private native void nativeSetXtdLimit(long ptr, double limit);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.4.3 AisManager.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;
import java.util.ArrayList;
import java.util.function.Consumer;

public class AisManager extends NativeObject {
    
    public AisManager() {
        setNativePtr(nativeCreate());
    }
    
    public void initialize() {
        checkNotDisposed();
        nativeInitialize(getNativePtr());
    }
    
    public void shutdown() {
        checkNotDisposed();
        nativeShutdown(getNativePtr());
    }
    
    public void processMessage(byte[] data) {
        checkNotDisposed();
        nativeProcessMessage(getNativePtr(), data);
    }
    
    public AisTarget getTarget(int mmsi) {
        checkNotDisposed();
        long ptr = nativeGetTarget(getNativePtr(), mmsi);
        return ptr != 0 ? new AisTarget(ptr) : null;
    }
    
    public List<AisTarget> getAllTargets() {
        checkNotDisposed();
        long[] ptrs = nativeGetAllTargets(getNativePtr());
        List<AisTarget> targets = new ArrayList<>();
        for (long ptr : ptrs) {
            if (ptr != 0) {
                targets.add(new AisTarget(ptr));
            }
        }
        return targets;
    }
    
    public List<AisTarget> getTargetsInRange(Coordinate center, double rangeNm) {
        checkNotDisposed();
        long[] ptrs = nativeGetTargetsInRange(getNativePtr(), 
            center.getX(), center.getY(), rangeNm);
        List<AisTarget> targets = new ArrayList<>();
        for (long ptr : ptrs) {
            if (ptr != 0) {
                targets.add(new AisTarget(ptr));
            }
        }
        return targets;
    }
    
    public void setTargetCallback(Consumer<AisTarget> callback) {
        this.targetCallback = callback;
    }
    
    public int getTargetCount() {
        checkNotDisposed();
        return nativeGetTargetCount(getNativePtr());
    }
    
    // JNI回调
    private void onTargetUpdate(long targetPtr) {
        if (targetCallback != null) {
            targetCallback.accept(new AisTarget(targetPtr));
        }
    }
    
    private Consumer<AisTarget> targetCallback;
    
    // Native方法
    private native long nativeCreate();
    private native void nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native void nativeProcessMessage(long ptr, byte[] data);
    private native long nativeGetTarget(long ptr, int mmsi);
    private native long[] nativeGetAllTargets(long ptr);
    private native long[] nativeGetTargetsInRange(long ptr, double x, double y, double range);
    private native int nativeGetTargetCount(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.4.4 AisTarget.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public class AisTarget extends NativeObject {
    
    public enum ShipType {
        UNKNOWN, CARGO, TANKER, PASSENGER, FISHING, TOWING, DREDGING, 
        MILITARY, SAILING, PLEASURE, HSC, WIG, OTHER
    }
    
    public enum NavStatus {
        UNDER_WAY_ENGINE, AT_ANCHOR, NOT_UNDER_COMMAND, 
        RESTRICTED_MANEUVERABILITY, CONSTRAINED_DRAUGHT, MOORED,
        AGROUND, FISHING, UNDER_WAY_SAILING
    }
    
    public AisTarget(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public int getMmsi() {
        checkNotDisposed();
        return nativeGetMmsi(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public String getCallsign() {
        checkNotDisposed();
        return nativeGetCallsign(getNativePtr());
    }
    
    public ShipType getShipType() {
        checkNotDisposed();
        return ShipType.values()[nativeGetShipType(getNativePtr())];
    }
    
    public NavStatus getNavStatus() {
        checkNotDisposed();
        return NavStatus.values()[nativeGetNavStatus(getNativePtr())];
    }
    
    public Coordinate getPosition() {
        checkNotDisposed();
        return nativeGetPosition(getNativePtr());
    }
    
    public double getCog() {
        checkNotDisposed();
        return nativeGetCog(getNativePtr());
    }
    
    public double getSog() {
        checkNotDisposed();
        return nativeGetSog(getNativePtr());
    }
    
    public double getHeading() {
        checkNotDisposed();
        return nativeGetHeading(getNativePtr());
    }
    
    public double getLength() {
        checkNotDisposed();
        return nativeGetLength(getNativePtr());
    }
    
    public double getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }
    
    public double getDraft() {
        checkNotDisposed();
        return nativeGetDraft(getNativePtr());
    }
    
    public double getDestinationLatitude() {
        checkNotDisposed();
        return nativeGetDestinationLat(getNativePtr());
    }
    
    public double getDestinationLongitude() {
        checkNotDisposed();
        return nativeGetDestinationLon(getNativePtr());
    }
    
    public long getLastUpdate() {
        checkNotDisposed();
        return nativeGetLastUpdate(getNativePtr());
    }
    
    // Native方法
    private native int nativeGetMmsi(long ptr);
    private native String nativeGetName(long ptr);
    private native String nativeGetCallsign(long ptr);
    private native int nativeGetShipType(long ptr);
    private native int nativeGetNavStatus(long ptr);
    private native Coordinate nativeGetPosition(long ptr);
    private native double nativeGetCog(long ptr);
    private native double nativeGetSog(long ptr);
    private native double nativeGetHeading(long ptr);
    private native double nativeGetLength(long ptr);
    private native double nativeGetWidth(long ptr);
    private native double nativeGetDraft(long ptr);
    private native double nativeGetDestinationLat(long ptr);
    private native double nativeGetDestinationLon(long ptr);
    private native long nativeGetLastUpdate(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.4.5 NavigationEngine.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.function.Consumer;

public class NavigationEngine extends NativeObject {
    
    public enum NavigationStatus {
        IDLE, NAVIGATING, PAUSED, COMPLETED, ERROR
    }
    
    public NavigationEngine() {
        setNativePtr(nativeCreate());
    }
    
    public void setRoute(Route route) {
        checkNotDisposed();
        nativeSetRoute(getNativePtr(), route.getNativePtr());
    }
    
    public Route getRoute() {
        checkNotDisposed();
        long ptr = nativeGetRoute(getNativePtr());
        return ptr != 0 ? new Route(ptr) : null;
    }
    
    public void startNavigation() {
        checkNotDisposed();
        nativeStartNavigation(getNativePtr());
    }
    
    public void stopNavigation() {
        checkNotDisposed();
        nativeStopNavigation(getNativePtr());
    }
    
    public void pauseNavigation() {
        checkNotDisposed();
        nativePauseNavigation(getNativePtr());
    }
    
    public void resumeNavigation() {
        checkNotDisposed();
        nativeResumeNavigation(getNativePtr());
    }
    
    public NavigationStatus getStatus() {
        checkNotDisposed();
        return NavigationStatus.values()[nativeGetStatus(getNativePtr())];
    }
    
    public Waypoint getCurrentWaypoint() {
        checkNotDisposed();
        long ptr = nativeGetCurrentWaypoint(getNativePtr());
        return ptr != 0 ? new Waypoint(ptr) : null;
    }
    
    public int getCurrentWaypointIndex() {
        checkNotDisposed();
        return nativeGetCurrentWaypointIndex(getNativePtr());
    }
    
    public double getDistanceToDestination() {
        checkNotDisposed();
        return nativeGetDistanceToDestination(getNativePtr());
    }
    
    public double getEstimatedTimeOfArrival() {
        checkNotDisposed();
        return nativeGetETA(getNativePtr());
    }
    
    public double getCrossTrackError() {
        checkNotDisposed();
        return nativeGetCrossTrackError(getNativePtr());
    }
    
    public double getBearingToWaypoint() {
        checkNotDisposed();
        return nativeGetBearingToWaypoint(getNativePtr());
    }
    
    public void updatePosition(Coordinate position, double cog, double sog) {
        checkNotDisposed();
        nativeUpdatePosition(getNativePtr(), position.getX(), position.getY(), cog, sog);
    }
    
    public void setNavigationCallback(Consumer<NavigationEvent> callback) {
        this.navigationCallback = callback;
    }
    
    // JNI回调
    private void onNavigationEvent(int eventType, String data) {
        if (navigationCallback != null) {
            navigationCallback.accept(new NavigationEvent(eventType, data));
        }
    }
    
    private Consumer<NavigationEvent> navigationCallback;
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetRoute(long ptr, long routePtr);
    private native long nativeGetRoute(long ptr);
    private native void nativeStartNavigation(long ptr);
    private native void nativeStopNavigation(long ptr);
    private native void nativePauseNavigation(long ptr);
    private native void nativeResumeNavigation(long ptr);
    private native int nativeGetStatus(long ptr);
    private native long nativeGetCurrentWaypoint(long ptr);
    private native int nativeGetCurrentWaypointIndex(long ptr);
    private native double nativeGetDistanceToDestination(long ptr);
    private native double nativeGetETA(long ptr);
    private native double nativeGetCrossTrackError(long ptr);
    private native double nativeGetBearingToWaypoint(long ptr);
    private native void nativeUpdatePosition(long ptr, double x, double y, double cog, double sog);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.4.6 NavigationEvent.java

```java
package cn.cycle.chart.api.navi;

public class NavigationEvent {
    
    public enum EventType {
        WAYPOINT_ARRIVED, WAYPOINT_PASSED, OFF_COURSE, 
        ROUTE_COMPLETED, ROUTE_DEVIATION, ETA_UPDATE
    }
    
    private final EventType type;
    private final String data;
    private final long timestamp;
    
    public NavigationEvent(int eventType, String data) {
        this.type = EventType.values()[eventType];
        this.data = data;
        this.timestamp = System.currentTimeMillis();
    }
    
    public EventType getType() { return type; }
    public String getData() { return data; }
    public long getTimestamp() { return timestamp; }
}
```

### 6.4.7 PositionManager.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.function.Consumer;

public class PositionManager extends NativeObject {
    
    public enum PositionSource {
        GPS, DGPS, RTK, MANUAL, SIMULATED
    }
    
    public enum PositionQuality {
        INVALID, GPS, DGPS, PPS, RTK_FIXED, RTK_FLOAT, ESTIMATED, MANUAL
    }
    
    public PositionManager() {
        setNativePtr(nativeCreate());
    }
    
    public void initialize() {
        checkNotDisposed();
        nativeInitialize(getNativePtr());
    }
    
    public void shutdown() {
        checkNotDisposed();
        nativeShutdown(getNativePtr());
    }
    
    public Coordinate getPosition() {
        checkNotDisposed();
        return nativeGetPosition(getNativePtr());
    }
    
    public double getCourseOverGround() {
        checkNotDisposed();
        return nativeGetCog(getNativePtr());
    }
    
    public double getSpeedOverGround() {
        checkNotDisposed();
        return nativeGetSog(getNativePtr());
    }
    
    public double getHeading() {
        checkNotDisposed();
        return nativeGetHeading(getNativePtr());
    }
    
    public PositionQuality getQuality() {
        checkNotDisposed();
        return PositionQuality.values()[nativeGetQuality(getNativePtr())];
    }
    
    public int getSatelliteCount() {
        checkNotDisposed();
        return nativeGetSatelliteCount(getNativePtr());
    }
    
    public double getHdop() {
        checkNotDisposed();
        return nativeGetHdop(getNativePtr());
    }
    
    public long getLastUpdate() {
        checkNotDisposed();
        return nativeGetLastUpdate(getNativePtr());
    }
    
    public boolean isValid() {
        checkNotDisposed();
        return nativeIsValid(getNativePtr());
    }
    
    public void setPositionSource(PositionSource source) {
        checkNotDisposed();
        nativeSetPositionSource(getNativePtr(), source.ordinal());
    }
    
    public void setPositionCallback(Consumer<PositionUpdate> callback) {
        this.positionCallback = callback;
    }
    
    // JNI回调
    private void onPositionUpdate(double x, double y, double cog, double sog, int quality) {
        if (positionCallback != null) {
            PositionUpdate update = new PositionUpdate(
                new Coordinate(x, y), cog, sog, PositionQuality.values()[quality]);
            positionCallback.accept(update);
        }
    }
    
    private Consumer<PositionUpdate> positionCallback;
    
    // Native方法
    private native long nativeCreate();
    private native void nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native Coordinate nativeGetPosition(long ptr);
    private native double nativeGetCog(long ptr);
    private native double nativeGetSog(long ptr);
    private native double nativeGetHeading(long ptr);
    private native int nativeGetQuality(long ptr);
    private native int nativeGetSatelliteCount(long ptr);
    private native double nativeGetHdop(long ptr);
    private native long nativeGetLastUpdate(long ptr);
    private native boolean nativeIsValid(long ptr);
    private native void nativeSetPositionSource(long ptr, int source);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.4.8 PositionUpdate.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

public class PositionUpdate {
    
    private final Coordinate position;
    private final double cog;
    private final double sog;
    private final PositionManager.PositionQuality quality;
    private final long timestamp;
    
    public PositionUpdate(Coordinate position, double cog, double sog, 
                          PositionManager.PositionQuality quality) {
        this.position = position;
        this.cog = cog;
        this.sog = sog;
        this.quality = quality;
        this.timestamp = System.currentTimeMillis();
    }
    
    public Coordinate getPosition() { return position; }
    public double getCog() { return cog; }
    public double getSog() { return sog; }
    public PositionManager.PositionQuality getQuality() { return quality; }
    public long getTimestamp() { return timestamp; }
}
```

---

## 6.5 警报模块接口 (alert)

### 6.5.1 AlertEngine.java

```java
package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.function.Consumer;

public class AlertEngine extends NativeObject {
    
    public AlertEngine() {
        setNativePtr(nativeCreate());
    }
    
    public void initialize(EngineConfig config) {
        checkNotDisposed();
        nativeInitialize(getNativePtr(), config);
    }
    
    public void start() {
        checkNotDisposed();
        nativeStart(getNativePtr());
    }
    
    public void stop() {
        checkNotDisposed();
        nativeStop(getNativePtr());
    }
    
    public void addChecker(AlertChecker checker) {
        checkNotDisposed();
        nativeAddChecker(getNativePtr(), checker.getNativePtr());
    }
    
    public void removeChecker(String name) {
        checkNotDisposed();
        nativeRemoveChecker(getNativePtr(), name);
    }
    
    public void processEvent(AlertEvent event) {
        checkNotDisposed();
        nativeProcessEvent(getNativePtr(), event);
    }
    
    public List<Alert> getActiveAlerts() {
        checkNotDisposed();
        long[] ptrs = nativeGetActiveAlerts(getNativePtr());
        return Alert.fromNativePtrs(ptrs);
    }
    
    public void acknowledgeAlert(String alertId) {
        checkNotDisposed();
        nativeAcknowledgeAlert(getNativePtr(), alertId);
    }
    
    public AlertStatistics getStatistics() {
        checkNotDisposed();
        return nativeGetStatistics(getNativePtr());
    }
    
    public void setAlertCallback(Consumer<Alert> callback) {
        this.alertCallback = callback;
    }
    
    // JNI回调
    private void onAlert(long alertPtr) {
        if (alertCallback != null) {
            alertCallback.accept(new Alert(alertPtr));
        }
    }
    
    private Consumer<Alert> alertCallback;
    
    // Native方法
    private native long nativeCreate();
    private native void nativeInitialize(long ptr, EngineConfig config);
    private native void nativeStart(long ptr);
    private native void nativeStop(long ptr);
    private native void nativeAddChecker(long ptr, long checkerPtr);
    private native void nativeRemoveChecker(long ptr, String name);
    private native void nativeProcessEvent(long ptr, AlertEvent event);
    private native long[] nativeGetActiveAlerts(long ptr);
    private native void nativeAcknowledgeAlert(long ptr, String alertId);
    private native AlertStatistics nativeGetStatistics(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.5.2 Alert.java

```java
package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;
import java.util.ArrayList;

public class Alert extends NativeObject {
    
    public enum AlertType {
        DEPTH, WEATHER, COLLISION, CHANNEL, DEVIATION, SPEED, RESTRICTED_AREA
    }
    
    public enum AlertLevel {
        INFO(1), WARNING(2), ERROR(3), CRITICAL(4);
        
        private final int level;
        AlertLevel(int level) { this.level = level; }
        public int getLevel() { return level; }
    }
    
    public enum AlertStatus {
        ACTIVE, ACKNOWLEDGED, CLEARED, EXPIRED
    }
    
    public Alert(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public String getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }
    
    public AlertType getType() {
        checkNotDisposed();
        return AlertType.values()[nativeGetType(getNativePtr())];
    }
    
    public AlertLevel getLevel() {
        checkNotDisposed();
        return AlertLevel.values()[nativeGetLevel(getNativePtr()) - 1];
    }
    
    public AlertStatus getStatus() {
        checkNotDisposed();
        return AlertStatus.values()[nativeGetStatus(getNativePtr())];
    }
    
    public String getMessage() {
        checkNotDisposed();
        return nativeGetMessage(getNativePtr());
    }
    
    public String getSource() {
        checkNotDisposed();
        return nativeGetSource(getNativePtr());
    }
    
    public Coordinate getPosition() {
        checkNotDisposed();
        return nativeGetPosition(getNativePtr());
    }
    
    public long getTimestamp() {
        checkNotDisposed();
        return nativeGetTimestamp(getNativePtr());
    }
    
    public long getExpiryTime() {
        checkNotDisposed();
        return nativeGetExpiryTime(getNativePtr());
    }
    
    public boolean isExpired() {
        checkNotDisposed();
        return nativeIsExpired(getNativePtr());
    }
    
    public void acknowledge() {
        checkNotDisposed();
        nativeAcknowledge(getNativePtr());
    }
    
    public List<String> getDetails() {
        checkNotDisposed();
        return nativeGetDetails(getNativePtr());
    }
    
    public static List<Alert> fromNativePtrs(long[] ptrs) {
        List<Alert> alerts = new ArrayList<>();
        for (long ptr : ptrs) {
            if (ptr != 0) {
                alerts.add(new Alert(ptr));
            }
        }
        return alerts;
    }
    
    // Native方法
    private native String nativeGetId(long ptr);
    private native int nativeGetType(long ptr);
    private native int nativeGetLevel(long ptr);
    private native int nativeGetStatus(long ptr);
    private native String nativeGetMessage(long ptr);
    private native String nativeGetSource(long ptr);
    private native Coordinate nativeGetPosition(long ptr);
    private native long nativeGetTimestamp(long ptr);
    private native long nativeGetExpiryTime(long ptr);
    private native boolean nativeIsExpired(long ptr);
    private native void nativeAcknowledge(long ptr);
    private native List<String> nativeGetDetails(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.5.3 AlertChecker.java

```java
package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.NativeObject;

public abstract class AlertChecker extends NativeObject {
    
    public AlertChecker() {
        setNativePtr(nativeCreate());
    }
    
    public abstract String getName();
    public abstract Alert.AlertType getType();
    
    public void configure(CheckerConfig config) {
        checkNotDisposed();
        nativeConfigure(getNativePtr(), config);
    }
    
    public void setEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetEnabled(getNativePtr(), enabled);
    }
    
    public boolean isEnabled() {
        checkNotDisposed();
        return nativeIsEnabled(getNativePtr());
    }
    
    // Native方法
    protected native long nativeCreate();
    protected native void nativeConfigure(long ptr, CheckerConfig config);
    protected native void nativeSetEnabled(long ptr, boolean enabled);
    protected native boolean nativeIsEnabled(long ptr);
}
```

### 6.5.4 DepthAlertChecker.java

```java
package cn.cycle.chart.api.alert;

public class DepthAlertChecker extends AlertChecker {
    
    public DepthAlertChecker() {
        super();
    }
    
    @Override
    public String getName() {
        return "DepthAlert";
    }
    
    @Override
    public Alert.AlertType getType() {
        return Alert.AlertType.DEPTH;
    }
    
    public void setSafetyDepth(double depth) {
        checkNotDisposed();
        nativeSetSafetyDepth(getNativePtr(), depth);
    }
    
    public double getSafetyDepth() {
        checkNotDisposed();
        return nativeGetSafetyDepth(getNativePtr());
    }
    
    public void setShallowDepth(double depth) {
        checkNotDisposed();
        nativeSetShallowDepth(getNativePtr(), depth);
    }
    
    public double getShallowDepth() {
        checkNotDisposed();
        return nativeGetShallowDepth(getNativePtr());
    }
    
    public void setDeepDepth(double depth) {
        checkNotDisposed();
        nativeSetDeepDepth(getNativePtr(), depth);
    }
    
    public double getDeepDepth() {
        checkNotDisposed();
        return nativeGetDeepDepth(getNativePtr());
    }
    
    // Native方法
    private native void nativeSetSafetyDepth(long ptr, double depth);
    private native double nativeGetSafetyDepth(long ptr);
    private native void nativeSetShallowDepth(long ptr, double depth);
    private native double nativeGetShallowDepth(long ptr);
    private native void nativeSetDeepDepth(long ptr, double depth);
    private native double nativeGetDeepDepth(long ptr);
}
```

### 6.5.5 CollisionAlertChecker.java

```java
package cn.cycle.chart.api.alert;

public class CollisionAlertChecker extends AlertChecker {
    
    public CollisionAlertChecker() {
        super();
    }
    
    @Override
    public String getName() {
        return "CollisionAlert";
    }
    
    @Override
    public Alert.AlertType getType() {
        return Alert.AlertType.COLLISION;
    }
    
    public void setCpaThreshold(double cpaNm) {
        checkNotDisposed();
        nativeSetCpaThreshold(getNativePtr(), cpaNm);
    }
    
    public double getCpaThreshold() {
        checkNotDisposed();
        return nativeGetCpaThreshold(getNativePtr());
    }
    
    public void setTcpaThreshold(double tcpaMinutes) {
        checkNotDisposed();
        nativeSetTcpaThreshold(getNativePtr(), tcpaMinutes);
    }
    
    public double getTcpaThreshold() {
        checkNotDisposed();
        return nativeGetTcpaThreshold(getNativePtr());
    }
    
    public void setGuardZoneRadius(double radiusNm) {
        checkNotDisposed();
        nativeSetGuardZoneRadius(getNativePtr(), radiusNm);
    }
    
    public double getGuardZoneRadius() {
        checkNotDisposed();
        return nativeGetGuardZoneRadius(getNativePtr());
    }
    
    // Native方法
    private native void nativeSetCpaThreshold(long ptr, double cpa);
    private native double nativeGetCpaThreshold(long ptr);
    private native void nativeSetTcpaThreshold(long ptr, double tcpa);
    private native double nativeGetTcpaThreshold(long ptr);
    private native void nativeSetGuardZoneRadius(long ptr, double radius);
    private native double nativeGetGuardZoneRadius(long ptr);
}
```

### 6.5.6 AlertEvent.java

```java
package cn.cycle.chart.api.alert;

import cn.cycle.chart.api.geometry.Coordinate;

public class AlertEvent {
    
    private final String type;
    private final Coordinate position;
    private final double value;
    private final long timestamp;
    
    public AlertEvent(String type, Coordinate position, double value) {
        this.type = type;
        this.position = position;
        this.value = value;
        this.timestamp = System.currentTimeMillis();
    }
    
    public String getType() { return type; }
    public Coordinate getPosition() { return position; }
    public double getValue() { return value; }
    public long getTimestamp() { return timestamp; }
}
```

### 6.5.7 EngineConfig.java

```java
package cn.cycle.chart.api.alert;

public class EngineConfig {
    
    private int checkIntervalMs = 1000;
    private int maxConcurrentChecks = 10;
    private boolean enableDeduplication = true;
    private boolean enableAggregation = true;
    private int dedupWindowSeconds = 300;
    
    public int getCheckIntervalMs() { return checkIntervalMs; }
    public void setCheckIntervalMs(int checkIntervalMs) { 
        this.checkIntervalMs = checkIntervalMs; 
    }
    
    public int getMaxConcurrentChecks() { return maxConcurrentChecks; }
    public void setMaxConcurrentChecks(int maxConcurrentChecks) { 
        this.maxConcurrentChecks = maxConcurrentChecks; 
    }
    
    public boolean isEnableDeduplication() { return enableDeduplication; }
    public void setEnableDeduplication(boolean enableDeduplication) { 
        this.enableDeduplication = enableDeduplication; 
    }
    
    public boolean isEnableAggregation() { return enableAggregation; }
    public void setEnableAggregation(boolean enableAggregation) { 
        this.enableAggregation = enableAggregation; 
    }
    
    public int getDedupWindowSeconds() { return dedupWindowSeconds; }
    public void setDedupWindowSeconds(int dedupWindowSeconds) { 
        this.dedupWindowSeconds = dedupWindowSeconds; 
    }
}
```

### 6.5.8 AlertStatistics.java

```java
package cn.cycle.chart.api.alert;

public class AlertStatistics {
    
    private final int totalAlertsGenerated;
    private final int totalAlertsPushed;
    private final int totalAlertsAcknowledged;
    private final int activeAlerts;
    private final long lastCheckTime;
    
    public AlertStatistics(int generated, int pushed, int acknowledged, 
                          int active, long lastCheck) {
        this.totalAlertsGenerated = generated;
        this.totalAlertsPushed = pushed;
        this.totalAlertsAcknowledged = acknowledged;
        this.activeAlerts = active;
        this.lastCheckTime = lastCheck;
    }
    
    public int getTotalAlertsGenerated() { return totalAlertsGenerated; }
    public int getTotalAlertsPushed() { return totalAlertsPushed; }
    public int getTotalAlertsAcknowledged() { return totalAlertsAcknowledged; }
    public int getActiveAlerts() { return activeAlerts; }
    public long getLastCheckTime() { return lastCheckTime; }
}
```

---

## 6.6 坐标转换模块接口 (proj)

### 6.6.1 CoordinateTransformer.java

```java
package cn.cycle.chart.api.proj;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Geometry;

public class CoordinateTransformer extends NativeObject {
    
    public CoordinateTransformer(String sourceCRS, String targetCRS) {
        setNativePtr(nativeCreate(sourceCRS, targetCRS));
    }
    
    public static CoordinateTransformer createWGS84ToWebMercator() {
        return new CoordinateTransformer("EPSG:4326", "EPSG:3857");
    }
    
    public static CoordinateTransformer createWebMercatorToWGS84() {
        return new CoordinateTransformer("EPSG:3857", "EPSG:4326");
    }
    
    public boolean isValid() {
        return getNativePtr() != 0 && nativeIsValid(getNativePtr());
    }
    
    public Coordinate transform(Coordinate coord) {
        checkNotDisposed();
        return nativeTransform(getNativePtr(), coord.getX(), coord.getY());
    }
    
    public Coordinate transformInverse(Coordinate coord) {
        checkNotDisposed();
        return nativeTransformInverse(getNativePtr(), coord.getX(), coord.getY());
    }
    
    public void transformArray(double[] x, double[] y) {
        checkNotDisposed();
        nativeTransformArray(getNativePtr(), x, y, x.length);
    }
    
    public void transformArrayInverse(double[] x, double[] y) {
        checkNotDisposed();
        nativeTransformArrayInverse(getNativePtr(), x, y, x.length);
    }
    
    public Envelope transform(Envelope env) {
        checkNotDisposed();
        return nativeTransformEnvelope(getNativePtr(), 
            env.getMinX(), env.getMinY(), env.getMaxX(), env.getMaxY());
    }
    
    public Geometry transform(Geometry geometry) {
        checkNotDisposed();
        long ptr = nativeTransformGeometry(getNativePtr(), geometry.getNativePtr());
        return ptr != 0 ? GeometryFactory.create(ptr) : null;
    }
    
    public String getSourceCRS() {
        checkNotDisposed();
        return nativeGetSourceCRS(getNativePtr());
    }
    
    public String getTargetCRS() {
        checkNotDisposed();
        return nativeGetTargetCRS(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate(String sourceCRS, String targetCRS);
    private native boolean nativeIsValid(long ptr);
    private native Coordinate nativeTransform(long ptr, double x, double y);
    private native Coordinate nativeTransformInverse(long ptr, double x, double y);
    private native void nativeTransformArray(long ptr, double[] x, double[] y, int count);
    private native void nativeTransformArrayInverse(long ptr, double[] x, double[] y, int count);
    private native Envelope nativeTransformEnvelope(long ptr, double minX, double minY, 
                                                     double maxX, double maxY);
    private native long nativeTransformGeometry(long ptr, long geomPtr);
    private native String nativeGetSourceCRS(long ptr);
    private native String nativeGetTargetCRS(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.6.2 TransformMatrix.java

```java
package cn.cycle.chart.api.proj;

import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;

public class TransformMatrix {
    
    private final double[] matrix;
    
    public TransformMatrix() {
        this.matrix = new double[] {1, 0, 0, 0, 1, 0};
    }
    
    public TransformMatrix(double[] matrix) {
        this.matrix = matrix.clone();
    }
    
    public TransformMatrix(double offsetX, double offsetY, double scaleX, double scaleY) {
        this.matrix = new double[] {
            scaleX, 0, offsetX,
            0, scaleY, offsetY
        };
    }
    
    public static TransformMatrix createWorldToScreen(
            double worldMinX, double worldMinY, double worldMaxX, double worldMaxY,
            double screenMinX, double screenMinY, double screenMaxX, double screenMaxY) {
        double scaleX = (screenMaxX - screenMinX) / (worldMaxX - worldMinX);
        double scaleY = (screenMaxY - screenMinY) / (worldMaxY - worldMinY);
        double offsetX = screenMinX - worldMinX * scaleX;
        double offsetY = screenMaxY + worldMinY * scaleY; // Y轴反转
        return new TransformMatrix(offsetX, offsetY, scaleX, -scaleY);
    }
    
    public Coordinate transform(Coordinate coord) {
        double x = matrix[0] * coord.getX() + matrix[2];
        double y = matrix[4] * coord.getY() + matrix[5];
        return new Coordinate(x, y);
    }
    
    public Coordinate transformInverse(Coordinate coord) {
        double x = (coord.getX() - matrix[2]) / matrix[0];
        double y = (coord.getY() - matrix[5]) / matrix[4];
        return new Coordinate(x, y);
    }
    
    public Envelope transform(Envelope env) {
        Coordinate min = transform(new Coordinate(env.getMinX(), env.getMinY()));
        Coordinate max = transform(new Coordinate(env.getMaxX(), env.getMaxY()));
        return new Envelope(
            Math.min(min.getX(), max.getX()),
            Math.min(min.getY(), max.getY()),
            Math.max(min.getX(), max.getX()),
            Math.max(min.getY(), max.getY())
        );
    }
    
    public TransformMatrix multiply(TransformMatrix other) {
        double[] result = new double[6];
        result[0] = this.matrix[0] * other.matrix[0] + this.matrix[1] * other.matrix[3];
        result[1] = this.matrix[0] * other.matrix[1] + this.matrix[1] * other.matrix[4];
        result[2] = this.matrix[0] * other.matrix[2] + this.matrix[1] * other.matrix[5] + this.matrix[2];
        result[3] = this.matrix[3] * other.matrix[0] + this.matrix[4] * other.matrix[3];
        result[4] = this.matrix[3] * other.matrix[1] + this.matrix[4] * other.matrix[4];
        result[5] = this.matrix[3] * other.matrix[2] + this.matrix[4] * other.matrix[5] + this.matrix[5];
        return new TransformMatrix(result);
    }
    
    public double[] getMatrix() {
        return matrix.clone();
    }
    
    public double getScaleX() { return matrix[0]; }
    public double getScaleY() { return matrix[4]; }
    public double getOffsetX() { return matrix[2]; }
    public double getOffsetY() { return matrix[5]; }
}
```

---

## 6.7 符号化模块接口 (symbology)

### 6.7.1 Symbolizer.java

```java
package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;

public abstract class Symbolizer extends NativeObject {
    
    public enum SymbolizerType {
        POINT, LINE, POLYGON, TEXT, RASTER, ICON, COMPOSITE
    }
    
    public Symbolizer() {
        setNativePtr(nativeCreate());
    }
    
    public abstract SymbolizerType getType();
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public DrawStyle getStyle() {
        checkNotDisposed();
        return nativeGetStyle(getNativePtr());
    }
    
    public void setStyle(DrawStyle style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style);
    }
    
    public boolean canSymbolize(FeatureInfo feature) {
        checkNotDisposed();
        return nativeCanSymbolize(getNativePtr(), feature.getNativePtr());
    }
    
    // Native方法
    protected native long nativeCreate();
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native DrawStyle nativeGetStyle(long ptr);
    private native void nativeSetStyle(long ptr, DrawStyle style);
    private native boolean nativeCanSymbolize(long ptr, long featurePtr);
}
```

### 6.7.2 Filter.java

```java
package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.geometry.Geometry;

public abstract class Filter extends NativeObject {
    
    public enum FilterType {
        COMPARISON, LOGICAL, SPATIAL, FEATURE_ID, NONE
    }
    
    public Filter() {
        setNativePtr(nativeCreate());
    }
    
    public abstract FilterType getType();
    
    public abstract boolean evaluate(FeatureInfo feature);
    
    public abstract boolean evaluateGeometry(Geometry geometry);
    
    public abstract String toString();
    
    // Native方法
    protected native long nativeCreate();
}
```

### 6.7.3 ComparisonFilter.java

```java
package cn.cycle.chart.api.symbology;

public class ComparisonFilter extends Filter {
    
    public enum ComparisonOperator {
        EQUAL, NOT_EQUAL, LESS_THAN, GREATER_THAN, 
        LESS_THAN_OR_EQUAL, GREATER_THAN_OR_EQUAL, 
        LIKE, IS_NULL, BETWEEN
    }
    
    private final ComparisonOperator operator;
    private final String propertyName;
    private final String literal;
    
    public ComparisonFilter(ComparisonOperator operator, String propertyName, String literal) {
        super();
        this.operator = operator;
        this.propertyName = propertyName;
        this.literal = literal;
        nativeInit(getNativePtr(), operator.ordinal(), propertyName, literal);
    }
    
    @Override
    public FilterType getType() {
        return FilterType.COMPARISON;
    }
    
    @Override
    public boolean evaluate(FeatureInfo feature) {
        checkNotDisposed();
        return nativeEvaluate(getNativePtr(), feature.getNativePtr());
    }
    
    @Override
    public boolean evaluateGeometry(Geometry geometry) {
        return false;
    }
    
    public ComparisonOperator getOperator() { return operator; }
    public String getPropertyName() { return propertyName; }
    public String getLiteral() { return literal; }
    
    // Native方法
    private native void nativeInit(long ptr, int operator, String propertyName, String literal);
    private native boolean nativeEvaluate(long ptr, long featurePtr);
}
```

### 6.7.4 SymbolizerRule.java

```java
package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;
import java.util.List;
import java.util.ArrayList;

public class SymbolizerRule extends NativeObject {
    
    public SymbolizerRule() {
        setNativePtr(nativeCreate());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public Filter getFilter() {
        checkNotDisposed();
        long ptr = nativeGetFilter(getNativePtr());
        return ptr != 0 ? FilterFactory.create(ptr) : null;
    }
    
    public void setFilter(Filter filter) {
        checkNotDisposed();
        nativeSetFilter(getNativePtr(), filter.getNativePtr());
    }
    
    public List<Symbolizer> getSymbolizers() {
        checkNotDisposed();
        long[] ptrs = nativeGetSymbolizers(getNativePtr());
        List<Symbolizer> symbolizers = new ArrayList<>();
        for (long ptr : ptrs) {
            if (ptr != 0) {
                symbolizers.add(SymbolizerFactory.create(ptr));
            }
        }
        return symbolizers;
    }
    
    public void addSymbolizer(Symbolizer symbolizer) {
        checkNotDisposed();
        nativeAddSymbolizer(getNativePtr(), symbolizer.getNativePtr());
    }
    
    public void removeSymbolizer(Symbolizer symbolizer) {
        checkNotDisposed();
        nativeRemoveSymbolizer(getNativePtr(), symbolizer.getNativePtr());
    }
    
    public double getMinScale() {
        checkNotDisposed();
        return nativeGetMinScale(getNativePtr());
    }
    
    public void setMinScale(double scale) {
        checkNotDisposed();
        nativeSetMinScale(getNativePtr(), scale);
    }
    
    public double getMaxScale() {
        checkNotDisposed();
        return nativeGetMaxScale(getNativePtr());
    }
    
    public void setMaxScale(double scale) {
        checkNotDisposed();
        nativeSetMaxScale(getNativePtr(), scale);
    }
    
    public boolean isElseRule() {
        checkNotDisposed();
        return nativeIsElseRule(getNativePtr());
    }
    
    public void setElseRule(boolean elseRule) {
        checkNotDisposed();
        nativeSetElseRule(getNativePtr(), elseRule);
    }
    
    public boolean appliesTo(FeatureInfo feature, double scale) {
        checkNotDisposed();
        return nativeAppliesTo(getNativePtr(), feature.getNativePtr(), scale);
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native long nativeGetFilter(long ptr);
    private native void nativeSetFilter(long ptr, long filterPtr);
    private native long[] nativeGetSymbolizers(long ptr);
    private native void nativeAddSymbolizer(long ptr, long symbolizerPtr);
    private native void nativeRemoveSymbolizer(long ptr, long symbolizerPtr);
    private native double nativeGetMinScale(long ptr);
    private native void nativeSetMinScale(long ptr, double scale);
    private native double nativeGetMaxScale(long ptr);
    private native void nativeSetMaxScale(long ptr, double scale);
    private native boolean nativeIsElseRule(long ptr);
    private native void nativeSetElseRule(long ptr, boolean elseRule);
    private native boolean nativeAppliesTo(long ptr, long featurePtr, double scale);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.8 基础工具模块接口 (base)

### 6.8.1 Logger.java

```java
package cn.cycle.chart.api.util;

import cn.cycle.chart.jni.NativeObject;

public class Logger extends NativeObject {
    
    public enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    }
    
    private static Logger instance;
    
    private Logger() {
        setNativePtr(nativeGetInstance());
    }
    
    public static synchronized Logger getInstance() {
        if (instance == null) {
            instance = new Logger();
        }
        return instance;
    }
    
    public void setLevel(LogLevel level) {
        nativeSetLevel(level.ordinal());
    }
    
    public LogLevel getLevel() {
        return LogLevel.values()[nativeGetLevel()];
    }
    
    public void trace(String tag, String message) {
        nativeLog(0, tag, message);
    }
    
    public void debug(String tag, String message) {
        nativeLog(1, tag, message);
    }
    
    public void info(String tag, String message) {
        nativeLog(2, tag, message);
    }
    
    public void warn(String tag, String message) {
        nativeLog(3, tag, message);
    }
    
    public void error(String tag, String message) {
        nativeLog(4, tag, message);
    }
    
    public void fatal(String tag, String message) {
        nativeLog(5, tag, message);
    }
    
    public void setLogFile(String path) {
        nativeSetLogFile(path);
    }
    
    public void setMaxFileSize(long size) {
        nativeSetMaxFileSize(size);
    }
    
    public void setMaxFileCount(int count) {
        nativeSetMaxFileCount(count);
    }
    
    // Native方法
    private native long nativeGetInstance();
    private native void nativeSetLevel(int level);
    private native int nativeGetLevel();
    private native void nativeLog(int level, String tag, String message);
    private native void nativeSetLogFile(String path);
    private native void nativeSetMaxFileSize(long size);
    private native void nativeSetMaxFileCount(int count);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.8.2 PerformanceMonitor.java

```java
package cn.cycle.chart.api.util;

import cn.cycle.chart.jni.NativeObject;
import java.util.Map;
import java.util.HashMap;

public class PerformanceMonitor extends NativeObject {
    
    private static PerformanceMonitor instance;
    
    private PerformanceMonitor() {
        setNativePtr(nativeGetInstance());
    }
    
    public static synchronized PerformanceMonitor getInstance() {
        if (instance == null) {
            instance = new PerformanceMonitor();
        }
        return instance;
    }
    
    public void startMeasure(String name) {
        checkNotDisposed();
        nativeStartMeasure(getNativePtr(), name);
    }
    
    public long stopMeasure(String name) {
        checkNotDisposed();
        return nativeStopMeasure(getNativePtr(), name);
    }
    
    public long getAverageTime(String name) {
        checkNotDisposed();
        return nativeGetAverageTime(getNativePtr(), name);
    }
    
    public long getMaxTime(String name) {
        checkNotDisposed();
        return nativeGetMaxTime(getNativePtr(), name);
    }
    
    public long getMinTime(String name) {
        checkNotDisposed();
        return nativeGetMinTime(getNativePtr(), name);
    }
    
    public long getCallCount(String name) {
        checkNotDisposed();
        return nativeGetCallCount(getNativePtr(), name);
    }
    
    public Map<String, PerformanceStats> getAllStats() {
        checkNotDisposed();
        String[] names = nativeGetAllMeasureNames(getNativePtr());
        Map<String, PerformanceStats> stats = new HashMap<>();
        for (String name : names) {
            stats.put(name, getStats(name));
        }
        return stats;
    }
    
    public PerformanceStats getStats(String name) {
        checkNotDisposed();
        return nativeGetStats(getNativePtr(), name);
    }
    
    public void reset() {
        checkNotDisposed();
        nativeReset(getNativePtr());
    }
    
    public void setEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetEnabled(getNativePtr(), enabled);
    }
    
    public boolean isEnabled() {
        checkNotDisposed();
        return nativeIsEnabled(getNativePtr());
    }
    
    // Native方法
    private native long nativeGetInstance();
    private native void nativeStartMeasure(long ptr, String name);
    private native long nativeStopMeasure(long ptr, String name);
    private native long nativeGetAverageTime(long ptr, String name);
    private native long nativeGetMaxTime(long ptr, String name);
    private native long nativeGetMinTime(long ptr, String name);
    private native long nativeGetCallCount(long ptr, String name);
    private native String[] nativeGetAllMeasureNames(long ptr);
    private native PerformanceStats nativeGetStats(long ptr, String name);
    private native void nativeReset(long ptr);
    private native void nativeSetEnabled(long ptr, boolean enabled);
    private native boolean nativeIsEnabled(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.8.3 PerformanceStats.java

```java
package cn.cycle.chart.api.util;

public class PerformanceStats {
    
    private final String name;
    private final long totalTime;
    private final long averageTime;
    private final long maxTime;
    private final long minTime;
    private final long callCount;
    
    public PerformanceStats(String name, long totalTime, long averageTime, 
                           long maxTime, long minTime, long callCount) {
        this.name = name;
        this.totalTime = totalTime;
        this.averageTime = averageTime;
        this.maxTime = maxTime;
        this.minTime = minTime;
        this.callCount = callCount;
    }
    
    public String getName() { return name; }
    public long getTotalTime() { return totalTime; }
    public long getAverageTime() { return averageTime; }
    public long getMaxTime() { return maxTime; }
    public long getMinTime() { return minTime; }
    public long getCallCount() { return callCount; }
    
    @Override
    public String toString() {
        return String.format("PerformanceStats[%s: avg=%dms, max=%dms, min=%dms, calls=%d]",
            name, averageTime, maxTime, minTime, callCount);
    }
}
```

---

## 6.9 异常类和回调接口

### 6.9.1 ChartException.java

```java
package cn.cycle.chart.api.exception;

public class ChartException extends Exception {
    
    private final String errorCode;
    private final String details;
    
    public ChartException(String message) {
        super(message);
        this.errorCode = "UNKNOWN";
        this.details = null;
    }
    
    public ChartException(String message, Throwable cause) {
        super(message, cause);
        this.errorCode = "UNKNOWN";
        this.details = null;
    }
    
    public ChartException(String errorCode, String message) {
        super(message);
        this.errorCode = errorCode;
        this.details = null;
    }
    
    public ChartException(String errorCode, String message, String details) {
        super(message);
        this.errorCode = errorCode;
        this.details = details;
    }
    
    public ChartException(String errorCode, String message, Throwable cause) {
        super(message, cause);
        this.errorCode = errorCode;
        this.details = null;
    }
    
    public String getErrorCode() { return errorCode; }
    public String getDetails() { return details; }
    
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("ChartException[").append(errorCode).append("]: ").append(getMessage());
        if (details != null) {
            sb.append("\nDetails: ").append(details);
        }
        if (getCause() != null) {
            sb.append("\nCaused by: ").append(getCause());
        }
        return sb.toString();
    }
}
```

### 6.9.2 JniException.java

```java
package cn.cycle.chart.api.exception;

public class JniException extends RuntimeException {
    
    private final int nativeErrorCode;
    
    public JniException(String message) {
        super(message);
        this.nativeErrorCode = 0;
    }
    
    public JniException(String message, int nativeErrorCode) {
        super(message);
        this.nativeErrorCode = nativeErrorCode;
    }
    
    public JniException(String message, Throwable cause) {
        super(message, cause);
        this.nativeErrorCode = 0;
    }
    
    public int getNativeErrorCode() { return nativeErrorCode; }
}
```

### 6.9.3 RenderCallback.java

```java
package cn.cycle.chart.api.callback;

public interface RenderCallback {
    
    void onRenderComplete();
    
    void onRenderError(String error);
    
    void onRenderProgress(double progress);
}
```

### 6.9.4 LoadCallback.java

```java
package cn.cycle.chart.api.callback;

public interface LoadCallback {
    
    void onLoadSuccess(String chartId);
    
    void onLoadError(String error);
    
    void onLoadProgress(double progress);
}
```

### 6.9.5 TouchCallback.java

```java
package cn.cycle.chart.api.callback;

import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.feature.FeatureInfo;

public interface TouchCallback {
    
    void onTap(Coordinate worldCoord, double screenX, double screenY);
    
    void onDoubleTap(Coordinate worldCoord, double screenX, double screenY);
    
    void onLongPress(Coordinate worldCoord, double screenX, double screenY);
    
    void onFeatureTap(FeatureInfo feature, double screenX, double screenY);
}
```

---

## 6.10 枚举类型定义

### 6.10.1 DisplayMode.java

```java
package cn.cycle.chart.api.core;

public enum DisplayMode {
    
    DAY(0, "Day"),
    NIGHT(1, "Night"),
    DUSK(2, "Dusk"),
    CUSTOM(3, "Custom");
    
    private final int code;
    private final String displayName;
    
    DisplayMode(int code, String displayName) {
        this.code = code;
        this.displayName = displayName;
    }
    
    public int getCode() { return code; }
    public String getDisplayName() { return displayName; }
    
    public static DisplayMode fromCode(int code) {
        for (DisplayMode mode : values()) {
            if (mode.code == code) {
                return mode;
            }
        }
        return DAY;
    }
}
```

### 6.10.2 GeometryType.java

```java
package cn.cycle.chart.api.geometry;

public enum GeometryType {
    
    POINT(1, "Point"),
    LINESTRING(2, "LineString"),
    POLYGON(3, "Polygon"),
    MULTIPOINT(4, "MultiPoint"),
    MULTILINESTRING(5, "MultiLineString"),
    MULTIPOLYGON(6, "MultiPolygon"),
    GEOMETRYCOLLECTION(7, "GeometryCollection");
    
    private final int code;
    private final String name;
    
    GeometryType(int code, String name) {
        this.code = code;
        this.name = name;
    }
    
    public int getCode() { return code; }
    public String getName() { return name; }
    
    public static GeometryType fromCode(int code) {
        for (GeometryType type : values()) {
            if (type.code == code) {
                return type;
            }
        }
        return POINT;
    }
}
```

---

## 6.11 生命周期管理接口

### 6.11.1 AppLifecycleManager.java

```java
package cn.cycle.chart.api.lifecycle;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.ChartConfig;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

public class AppLifecycleManager {
    
    public enum AppState {
        CREATED, READY, LOADED, PAUSED, DISPOSED
    }
    
    private final AtomicReference<AppState> state = new AtomicReference<>(AppState.CREATED);
    private final ChartViewer viewer;
    private ExecutorService backgroundExecutor;
    private Consumer<AppState> stateChangeListener;
    
    public AppLifecycleManager() {
        this.viewer = new ChartViewer();
    }
    
    public AppLifecycleManager(ChartConfig config) {
        this.viewer = new ChartViewer(config);
    }
    
    public void initialize() {
        if (!state.compareAndSet(AppState.CREATED, AppState.READY)) {
            throw new IllegalStateException("Invalid state transition from " + state.get());
        }
        
        backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors());
        
        JniBridge.initialize();
        notifyStateChanged();
    }
    
    public void loadChart(String path, Consumer<Boolean> callback) {
        AppState current = state.get();
        if (current != AppState.READY && current != AppState.LOADED) {
            throw new IllegalStateException("Invalid state for loading: " + current);
        }
        
        backgroundExecutor.submit(() -> {
            try {
                boolean result = viewer.loadChart(path);
                if (result) {
                    state.set(AppState.LOADED);
                }
                if (callback != null) {
                    callback.accept(result);
                }
                notifyStateChanged();
            } catch (Exception e) {
                if (callback != null) {
                    callback.accept(false);
                }
            }
        });
    }
    
    public void pause() {
        if (state.compareAndSet(AppState.LOADED, AppState.PAUSED)) {
            viewer.flushCache();
            notifyStateChanged();
        }
    }
    
    public void resume() {
        if (state.compareAndSet(AppState.PAUSED, AppState.LOADED)) {
            notifyStateChanged();
        }
    }
    
    public void dispose() {
        AppState current = state.getAndSet(AppState.DISPOSED);
        if (current == AppState.DISPOSED) {
            return;
        }
        
        try {
            if (backgroundExecutor != null) {
                backgroundExecutor.shutdown();
                if (!backgroundExecutor.awaitTermination(5, TimeUnit.SECONDS)) {
                    backgroundExecutor.shutdownNow();
                }
            }
            
            viewer.close();
            JniBridge.shutdown();
            
            notifyStateChanged();
        } catch (Exception e) {
            // Log error
        }
    }
    
    public AppState getState() {
        return state.get();
    }
    
    public ChartViewer getViewer() {
        return viewer;
    }
    
    public void setStateChangeListener(Consumer<AppState> listener) {
        this.stateChangeListener = listener;
    }
    
    private void notifyStateChanged() {
        if (stateChangeListener != null) {
            stateChangeListener.accept(state.get());
        }
    }
}
```

---

## 6.12 插件扩展接口

### 6.12.1 ChartPlugin.java

```java
package cn.cycle.chart.api.plugin;

import java.util.List;

public interface ChartPlugin {
    
    String getName();
    
    String getVersion();
    
    String getDescription();
    
    void initialize(PluginContext context);
    
    void shutdown();
    
    List<LayerProvider> getLayerProviders();
    
    List<SymbolProvider> getSymbolProviders();
    
    List<ParserProvider> getParserProviders();
}
```

### 6.12.2 LayerProvider.java

```java
package cn.cycle.chart.api.plugin;

import cn.cycle.chart.api.layer.ChartLayer;

public interface LayerProvider {
    
    String getLayerType();
    
    ChartLayer createLayer(LayerConfig config);
    
    LayerConfig createDefaultConfig();
    
    boolean canHandle(String sourcePath);
}
```

### 6.12.3 SymbolProvider.java

```java
package cn.cycle.chart.api.plugin;

import cn.cycle.chart.api.symbology.Symbolizer;
import cn.cycle.chart.api.feature.FeatureInfo;

public interface SymbolProvider {
    
    String getSymbolType();
    
    Symbolizer createSymbol(SymbolConfig config);
    
    boolean canHandle(FeatureInfo feature);
}
```

### 6.12.4 ParserProvider.java

```java
package cn.cycle.chart.api.plugin;

public interface ParserProvider {
    
    String getParserType();
    
    String[] getSupportedExtensions();
    
    boolean canParse(String filePath);
    
    Parser createParser(ParserConfig config);
}
```

### 6.12.5 PluginManager.java

```java
package cn.cycle.chart.api.plugin;

import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.ServiceLoader;
import java.util.concurrent.ConcurrentHashMap;

public class PluginManager {
    
    private final Map<String, ChartPlugin> plugins = new ConcurrentHashMap<>();
    private final Map<String, LayerProvider> layerProviders = new ConcurrentHashMap<>();
    private final Map<String, SymbolProvider> symbolProviders = new ConcurrentHashMap<>();
    private final ServiceLoader<ChartPlugin> pluginLoader;
    
    public PluginManager() {
        this.pluginLoader = ServiceLoader.load(ChartPlugin.class);
    }
    
    public void loadPlugins() {
        for (ChartPlugin plugin : pluginLoader) {
            try {
                plugin.initialize(createPluginContext());
                plugins.put(plugin.getName(), plugin);
                
                for (LayerProvider provider : plugin.getLayerProviders()) {
                    layerProviders.put(provider.getLayerType(), provider);
                }
                
                for (SymbolProvider provider : plugin.getSymbolProviders()) {
                    symbolProviders.put(provider.getSymbolType(), provider);
                }
                
            } catch (Exception e) {
                System.err.println("Failed to load plugin: " + plugin.getName() + " - " + e.getMessage());
            }
        }
    }
    
    public void registerLayerProvider(LayerProvider provider) {
        layerProviders.put(provider.getLayerType(), provider);
    }
    
    public LayerProvider getLayerProvider(String type) {
        return layerProviders.get(type);
    }
    
    public void shutdown() {
        for (ChartPlugin plugin : plugins.values()) {
            try {
                plugin.shutdown();
            } catch (Exception e) {
                System.err.println("Error shutting down plugin: " + plugin.getName());
            }
        }
        plugins.clear();
        layerProviders.clear();
        symbolProviders.clear();
    }
    
    public List<ChartPlugin> getPlugins() {
        return List.copyOf(plugins.values());
    }
    
    private PluginContext createPluginContext() {
        return new PluginContext(this);
    }
}
```

### 6.12.6 PluginContext.java

```java
package cn.cycle.chart.api.plugin;

public class PluginContext {
    
    private final PluginManager pluginManager;
    
    public PluginContext(PluginManager pluginManager) {
        this.pluginManager = pluginManager;
    }
    
    public PluginManager getPluginManager() {
        return pluginManager;
    }
    
    public void registerLayerProvider(LayerProvider provider) {
        pluginManager.registerLayerProvider(provider);
    }
}
```

### 6.12.7 LayerConfig.java

```java
package cn.cycle.chart.api.plugin;

import java.util.HashMap;
import java.util.Map;

public class LayerConfig {
    
    private final Map<String, Object> properties = new HashMap<>();
    
    public LayerConfig set(String key, Object value) {
        properties.put(key, value);
        return this;
    }
    
    public Object get(String key) {
        return properties.get(key);
    }
    
    public String getString(String key) {
        Object value = properties.get(key);
        return value != null ? value.toString() : null;
    }
    
    public String getString(String key, String defaultValue) {
        String value = getString(key);
        return value != null ? value : defaultValue;
    }
    
    public int getInt(String key) {
        Object value = properties.get(key);
        if (value instanceof Number) {
            return ((Number) value).intValue();
        }
        return 0;
    }
    
    public int getInt(String key, int defaultValue) {
        Object value = properties.get(key);
        if (value instanceof Number) {
            return ((Number) value).intValue();
        }
        return defaultValue;
    }
    
    public double getDouble(String key) {
        Object value = properties.get(key);
        if (value instanceof Number) {
            return ((Number) value).doubleValue();
        }
        return 0.0;
    }
    
    public boolean getBoolean(String key) {
        Object value = properties.get(key);
        if (value instanceof Boolean) {
            return (Boolean) value;
        }
        return false;
    }
}
```

---

## 6.13 补充接口定义

### 6.13.1 MultiPoint.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class MultiPoint extends Geometry {
    
    public MultiPoint() {
        setNativePtr(nativeCreate());
    }
    
    public MultiPoint(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    @Override
    public GeometryType getGeometryType() {
        return GeometryType.MULTIPOINT;
    }
    
    public int getNumPoints() {
        checkNotDisposed();
        return nativeGetNumPoints(getNativePtr());
    }
    
    public Point getPointN(int index) {
        checkNotDisposed();
        long ptr = nativeGetPointN(getNativePtr(), index);
        return ptr != 0 ? new Point(ptr) : null;
    }
    
    public List<Point> getPoints() {
        List<Point> points = new ArrayList<>();
        int count = getNumPoints();
        for (int i = 0; i < count; i++) {
            points.add(getPointN(i));
        }
        return points;
    }
    
    public void addPoint(Point point) {
        checkNotDisposed();
        nativeAddPoint(getNativePtr(), point.getNativePtr());
    }
    
    public void addPoint(double x, double y) {
        checkNotDisposed();
        nativeAddPointCoord(getNativePtr(), x, y);
    }
    
    // Native方法
    private native long nativeCreate();
    private native int nativeGetNumPoints(long ptr);
    private native long nativeGetPointN(long ptr, int index);
    private native void nativeAddPoint(long ptr, long pointPtr);
    private native void nativeAddPointCoord(long ptr, double x, double y);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.2 MultiLineString.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class MultiLineString extends Geometry {
    
    public MultiLineString() {
        setNativePtr(nativeCreate());
    }
    
    public MultiLineString(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    @Override
    public GeometryType getGeometryType() {
        return GeometryType.MULTILINESTRING;
    }
    
    public int getNumLineStrings() {
        checkNotDisposed();
        return nativeGetNumLineStrings(getNativePtr());
    }
    
    public LineString getLineStringN(int index) {
        checkNotDisposed();
        long ptr = nativeGetLineStringN(getNativePtr(), index);
        return ptr != 0 ? new LineString(ptr) : null;
    }
    
    public List<LineString> getLineStrings() {
        List<LineString> lineStrings = new ArrayList<>();
        int count = getNumLineStrings();
        for (int i = 0; i < count; i++) {
            lineStrings.add(getLineStringN(i));
        }
        return lineStrings;
    }
    
    public void addLineString(LineString lineString) {
        checkNotDisposed();
        nativeAddLineString(getNativePtr(), lineString.getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native int nativeGetNumLineStrings(long ptr);
    private native long nativeGetLineStringN(long ptr, int index);
    private native void nativeAddLineString(long ptr, long lineStringPtr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.3 MultiPolygon.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class MultiPolygon extends Geometry {
    
    public MultiPolygon() {
        setNativePtr(nativeCreate());
    }
    
    public MultiPolygon(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    @Override
    public GeometryType getGeometryType() {
        return GeometryType.MULTIPOLYGON;
    }
    
    public int getNumPolygons() {
        checkNotDisposed();
        return nativeGetNumPolygons(getNativePtr());
    }
    
    public Polygon getPolygonN(int index) {
        checkNotDisposed();
        long ptr = nativeGetPolygonN(getNativePtr(), index);
        return ptr != 0 ? new Polygon(ptr) : null;
    }
    
    public List<Polygon> getPolygons() {
        List<Polygon> polygons = new ArrayList<>();
        int count = getNumPolygons();
        for (int i = 0; i < count; i++) {
            polygons.add(getPolygonN(i));
        }
        return polygons;
    }
    
    public void addPolygon(Polygon polygon) {
        checkNotDisposed();
        nativeAddPolygon(getNativePtr(), polygon.getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native int nativeGetNumPolygons(long ptr);
    private native long nativeGetPolygonN(long ptr, int index);
    private native void nativeAddPolygon(long ptr, long polygonPtr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.4 GeometryCollection.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class GeometryCollection extends Geometry {
    
    public GeometryCollection() {
        setNativePtr(nativeCreate());
    }
    
    public GeometryCollection(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    @Override
    public GeometryType getGeometryType() {
        return GeometryType.GEOMETRYCOLLECTION;
    }
    
    public int getNumGeometries() {
        checkNotDisposed();
        return nativeGetNumGeometries(getNativePtr());
    }
    
    public Geometry getGeometryN(int index) {
        checkNotDisposed();
        long ptr = nativeGetGeometryN(getNativePtr(), index);
        return ptr != 0 ? GeometryFactory.create(ptr) : null;
    }
    
    public List<Geometry> getGeometries() {
        List<Geometry> geometries = new ArrayList<>();
        int count = getNumGeometries();
        for (int i = 0; i < count; i++) {
            geometries.add(getGeometryN(i));
        }
        return geometries;
    }
    
    public void addGeometry(Geometry geometry) {
        checkNotDisposed();
        nativeAddGeometry(getNativePtr(), geometry.getNativePtr());
    }
    
    public void removeGeometry(int index) {
        checkNotDisposed();
        nativeRemoveGeometry(getNativePtr(), index);
    }
    
    // Native方法
    private native long nativeCreate();
    private native int nativeGetNumGeometries(long ptr);
    private native long nativeGetGeometryN(long ptr, int index);
    private native void nativeAddGeometry(long ptr, long geomPtr);
    private native void nativeRemoveGeometry(long ptr, int index);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.5 GeometryFactory.java

```java
package cn.cycle.chart.api.geometry;

public final class GeometryFactory {
    
    private GeometryFactory() {}
    
    public static Geometry create(long nativePtr) {
        int typeCode = nativeGetGeometryType(nativePtr);
        GeometryType type = GeometryType.fromCode(typeCode);
        
        switch (type) {
            case POINT:
                return new Point(nativePtr);
            case LINESTRING:
                return new LineString(nativePtr);
            case POLYGON:
                return new Polygon(nativePtr);
            case MULTIPOINT:
                return new MultiPoint(nativePtr);
            case MULTILINESTRING:
                return new MultiLineString(nativePtr);
            case MULTIPOLYGON:
                return new MultiPolygon(nativePtr);
            case GEOMETRYCOLLECTION:
                return new GeometryCollection(nativePtr);
            default:
                throw new IllegalArgumentException("Unknown geometry type: " + type);
        }
    }
    
    public static Point createPoint(double x, double y) {
        return new Point(x, y);
    }
    
    public static LineString createLineString(double[] x, double[] y) {
        LineString line = new LineString();
        for (int i = 0; i < x.length; i++) {
            line.addPoint(x[i], y[i]);
        }
        return line;
    }
    
    public static Polygon createPolygon(double[] shellX, double[] shellY) {
        Polygon polygon = new Polygon();
        LinearRing shell = new LinearRing();
        for (int i = 0; i < shellX.length; i++) {
            shell.addPoint(shellX[i], shellY[i]);
        }
        polygon.setExteriorRing(shell);
        return polygon;
    }
    
    public static Envelope createEnvelope(double minX, double minY, 
                                          double maxX, double maxY) {
        return new Envelope(minX, minY, maxX, maxY);
    }
    
    private static native int nativeGetGeometryType(long ptr);
}
```

### 6.13.6 FieldDefn.java

```java
package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.NativeObject;

public class FieldDefn extends NativeObject {
    
    public enum FieldType {
        INTEGER(0), 
        DOUBLE(1), 
        STRING(2), 
        DATE(3), 
        BINARY(4);
        
        private final int code;
        FieldType(int code) { this.code = code; }
        public int getCode() { return code; }
        
        public static FieldType fromCode(int code) {
            for (FieldType type : values()) {
                if (type.code == code) return type;
            }
            return STRING;
        }
    }
    
    public FieldDefn() {
        setNativePtr(nativeCreate());
    }
    
    public FieldDefn(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public FieldType getType() {
        checkNotDisposed();
        return FieldType.fromCode(nativeGetType(getNativePtr()));
    }
    
    public void setType(FieldType type) {
        checkNotDisposed();
        nativeSetType(getNativePtr(), type.getCode());
    }
    
    public int getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }
    
    public void setWidth(int width) {
        checkNotDisposed();
        nativeSetWidth(getNativePtr(), width);
    }
    
    public int getPrecision() {
        checkNotDisposed();
        return nativeGetPrecision(getNativePtr());
    }
    
    public void setPrecision(int precision) {
        checkNotDisposed();
        nativeSetPrecision(getNativePtr(), precision);
    }
    
    public boolean isNullable() {
        checkNotDisposed();
        return nativeIsNullable(getNativePtr());
    }
    
    public void setNullable(boolean nullable) {
        checkNotDisposed();
        nativeSetNullable(getNativePtr(), nullable);
    }
    
    public String getDefaultValue() {
        checkNotDisposed();
        return nativeGetDefaultValue(getNativePtr());
    }
    
    public void setDefaultValue(String value) {
        checkNotDisposed();
        nativeSetDefaultValue(getNativePtr(), value);
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetType(long ptr);
    private native void nativeSetType(long ptr, int type);
    private native int nativeGetWidth(long ptr);
    private native void nativeSetWidth(long ptr, int width);
    private native int nativeGetPrecision(long ptr);
    private native void nativeSetPrecision(long ptr, int precision);
    private native boolean nativeIsNullable(long ptr);
    private native void nativeSetNullable(long ptr, boolean nullable);
    private native String nativeGetDefaultValue(long ptr);
    private native void nativeSetDefaultValue(long ptr, String value);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.7 RenderContext.java

```java
package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Coordinate;

public class RenderContext extends NativeObject {
    
    public RenderContext() {
        setNativePtr(nativeCreate());
    }
    
    public RenderContext(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public Envelope getVisibleExtent() {
        checkNotDisposed();
        return nativeGetVisibleExtent(getNativePtr());
    }
    
    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }
    
    public double getDpi() {
        checkNotDisposed();
        return nativeGetDpi(getNativePtr());
    }
    
    public void setDpi(double dpi) {
        checkNotDisposed();
        nativeSetDpi(getNativePtr(), dpi);
    }
    
    public int getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }
    
    public int getHeight() {
        checkNotDisposed();
        return nativeGetHeight(getNativePtr());
    }
    
    public void setSize(int width, int height) {
        checkNotDisposed();
        nativeSetSize(getNativePtr(), width, height);
    }
    
    public Coordinate screenToWorld(double screenX, double screenY) {
        checkNotDisposed();
        return nativeScreenToWorld(getNativePtr(), screenX, screenY);
    }
    
    public Coordinate worldToScreen(double worldX, double worldY) {
        checkNotDisposed();
        return nativeWorldToScreen(getNativePtr(), worldX, worldY);
    }
    
    public boolean isCancelled() {
        checkNotDisposed();
        return nativeIsCancelled(getNativePtr());
    }
    
    public void cancel() {
        checkNotDisposed();
        nativeCancel(getNativePtr());
    }
    
    public double getProgress() {
        checkNotDisposed();
        return nativeGetProgress(getNativePtr());
    }
    
    public void setProgress(double progress) {
        checkNotDisposed();
        nativeSetProgress(getNativePtr(), progress);
    }
    
    public DisplayMode getDisplayMode() {
        checkNotDisposed();
        return DisplayMode.fromCode(nativeGetDisplayMode(getNativePtr()));
    }
    
    public void setDisplayMode(DisplayMode mode) {
        checkNotDisposed();
        nativeSetDisplayMode(getNativePtr(), mode.getCode());
    }
    
    // Native方法
    private native long nativeCreate();
    private native Envelope nativeGetVisibleExtent(long ptr);
    private native double nativeGetScale(long ptr);
    private native double nativeGetDpi(long ptr);
    private native void nativeSetDpi(long ptr, double dpi);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native void nativeSetSize(long ptr, int width, int height);
    private native Coordinate nativeScreenToWorld(long ptr, double x, double y);
    private native Coordinate nativeWorldToScreen(long ptr, double x, double y);
    private native boolean nativeIsCancelled(long ptr);
    private native void nativeCancel(long ptr);
    private native double nativeGetProgress(long ptr);
    private native void nativeSetProgress(long ptr, double progress);
    private native int nativeGetDisplayMode(long ptr);
    private native void nativeSetDisplayMode(long ptr, int mode);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.8 Track.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;
import java.util.ArrayList;

public class Track extends NativeObject {
    
    public enum TrackType {
        OWN_SHIP, OTHER_SHIP, HISTORICAL
    }
    
    public Track() {
        setNativePtr(nativeCreate());
    }
    
    public Track(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public String getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public TrackType getType() {
        checkNotDisposed();
        return TrackType.values()[nativeGetType(getNativePtr())];
    }
    
    public void setType(TrackType type) {
        checkNotDisposed();
        nativeSetType(getNativePtr(), type.ordinal());
    }
    
    public int getPointCount() {
        checkNotDisposed();
        return nativeGetPointCount(getNativePtr());
    }
    
    public TrackPoint getPoint(int index) {
        checkNotDisposed();
        long ptr = nativeGetPoint(getNativePtr(), index);
        return ptr != 0 ? new TrackPoint(ptr) : null;
    }
    
    public List<TrackPoint> getPoints() {
        List<TrackPoint> points = new ArrayList<>();
        int count = getPointCount();
        for (int i = 0; i < count; i++) {
            points.add(getPoint(i));
        }
        return points;
    }
    
    public void addPoint(Coordinate coord, long timestamp) {
        checkNotDisposed();
        nativeAddPoint(getNativePtr(), coord.getX(), coord.getY(), timestamp);
    }
    
    public void addPoint(TrackPoint point) {
        checkNotDisposed();
        nativeAddTrackPoint(getNativePtr(), point.getNativePtr());
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public void setMaxPoints(int maxPoints) {
        checkNotDisposed();
        nativeSetMaxPoints(getNativePtr(), maxPoints);
    }
    
    public int getMaxPoints() {
        checkNotDisposed();
        return nativeGetMaxPoints(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetType(long ptr);
    private native void nativeSetType(long ptr, int type);
    private native int nativeGetPointCount(long ptr);
    private native long nativeGetPoint(long ptr, int index);
    private native void nativeAddPoint(long ptr, double x, double y, long timestamp);
    private native void nativeAddTrackPoint(long ptr, long pointPtr);
    private native void nativeClear(long ptr);
    private native void nativeSetMaxPoints(long ptr, int maxPoints);
    private native int nativeGetMaxPoints(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.9 TrackPoint.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public class TrackPoint extends NativeObject {
    
    public TrackPoint() {
        setNativePtr(nativeCreate());
    }
    
    public TrackPoint(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public Coordinate getPosition() {
        checkNotDisposed();
        return nativeGetPosition(getNativePtr());
    }
    
    public void setPosition(Coordinate coord) {
        checkNotDisposed();
        nativeSetPosition(getNativePtr(), coord.getX(), coord.getY());
    }
    
    public long getTimestamp() {
        checkNotDisposed();
        return nativeGetTimestamp(getNativePtr());
    }
    
    public void setTimestamp(long timestamp) {
        checkNotDisposed();
        nativeSetTimestamp(getNativePtr(), timestamp);
    }
    
    public double getCog() {
        checkNotDisposed();
        return nativeGetCog(getNativePtr());
    }
    
    public void setCog(double cog) {
        checkNotDisposed();
        nativeSetCog(getNativePtr(), cog);
    }
    
    public double getSog() {
        checkNotDisposed();
        return nativeGetSog(getNativePtr());
    }
    
    public void setSog(double sog) {
        checkNotDisposed();
        nativeSetSog(getNativePtr(), sog);
    }
    
    public double getDepth() {
        checkNotDisposed();
        return nativeGetDepth(getNativePtr());
    }
    
    public void setDepth(double depth) {
        checkNotDisposed();
        nativeSetDepth(getNativePtr(), depth);
    }
    
    // Native方法
    private native long nativeCreate();
    private native Coordinate nativeGetPosition(long ptr);
    private native void nativeSetPosition(long ptr, double x, double y);
    private native long nativeGetTimestamp(long ptr);
    private native void nativeSetTimestamp(long ptr, long timestamp);
    private native double nativeGetCog(long ptr);
    private native void nativeSetCog(long ptr, double cog);
    private native double nativeGetSog(long ptr);
    private native void nativeSetSog(long ptr, double sog);
    private native double nativeGetDepth(long ptr);
    private native void nativeSetDepth(long ptr, double depth);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.13.10 I18nManager.java

```java
package cn.cycle.chart.app.util;

import java.util.Locale;
import java.util.ResourceBundle;
import java.util.MissingResourceException;
import java.util.concurrent.ConcurrentHashMap;

public class I18nManager {
    
    private static I18nManager instance;
    private static final String BUNDLE_BASE = "cn.cycle.chart.i18n.messages";
    
    private final ConcurrentHashMap<Locale, ResourceBundle> bundles = new ConcurrentHashMap<>();
    private volatile Locale currentLocale = Locale.getDefault();
    
    private I18nManager() {}
    
    public static synchronized I18nManager getInstance() {
        if (instance == null) {
            instance = new I18nManager();
        }
        return instance;
    }
    
    public void setLocale(Locale locale) {
        this.currentLocale = locale;
        Locale.setDefault(locale);
    }
    
    public Locale getLocale() {
        return currentLocale;
    }
    
    public String getString(String key) {
        try {
            return getBundle().getString(key);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    public String getString(String key, Object... args) {
        try {
            String pattern = getBundle().getString(key);
            return String.format(pattern, args);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    public String getString(Locale locale, String key) {
        try {
            return getBundle(locale).getString(key);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    private ResourceBundle getBundle() {
        return getBundle(currentLocale);
    }
    
    private ResourceBundle getBundle(Locale locale) {
        return bundles.computeIfAbsent(locale, 
            l -> ResourceBundle.getBundle(BUNDLE_BASE, l));
    }
    
    public void clearCache() {
        bundles.clear();
    }
    
    public static String tr(String key) {
        return getInstance().getString(key);
    }
    
    public static String tr(String key, Object... args) {
        return getInstance().getString(key, args);
    }
}
```

### 6.13.11 AccessibilityHelper.java

```java
package cn.cycle.chart.app.util;

import javafx.scene.Node;
import javafx.scene.control.Control;
import javafx.scene.control.Tooltip;
import javafx.scene.AccessibleRole;

public class AccessibilityHelper {
    
    public static void setupAccessible(Node node, String name, String description) {
        node.setAccessibleRole(AccessibleRole.NODE);
        node.setAccessibleText(name);
        node.setAccessibleHelp(description);
    }
    
    public static void setupButton(Control button, String text, String description) {
        button.setAccessibleRole(AccessibleRole.BUTTON);
        button.setAccessibleText(text);
        button.setAccessibleHelp(description);
        button.setTooltip(new Tooltip(description));
    }
    
    public static void setupTextField(Control textField, String name, String value) {
        textField.setAccessibleRole(AccessibleRole.TEXT_FIELD);
        textField.setAccessibleText(name + ": " + value);
    }
    
    public static void setupChartCanvas(Node canvas) {
        canvas.setAccessibleRole(AccessibleRole.IMAGE_VIEW);
        canvas.setAccessibleText("海图显示区域");
        canvas.setAccessibleHelp("使用鼠标拖动平移，滚轮缩放");
    }
    
    public static void announce(String message) {
        javafx.application.Platform.runLater(() -> {
            com.sun.javafx.application.PlatformImpl.tkGetToolkit()
                .notifyAccessibleContentChange(null);
        });
    }
    
    public static void focusAndAnnounce(Node node, String message) {
        node.requestFocus();
        announce(message);
    }
}
```

### 6.13.12 MetricsExporter.java

```java
package cn.cycle.chart.app.util;

import cn.cycle.chart.api.util.PerformanceMonitor;
import cn.cycle.chart.api.util.PerformanceStats;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Map;

public class MetricsExporter {
    
    public enum ExportFormat {
        CSV, JSON, TEXT
    }
    
    public static void exportToFile(String filePath, ExportFormat format) throws IOException {
        PerformanceMonitor monitor = PerformanceMonitor.getInstance();
        Map<String, PerformanceStats> stats = monitor.getAllStats();
        
        try (PrintWriter writer = new PrintWriter(new FileWriter(filePath))) {
            switch (format) {
                case CSV:
                    exportCsv(writer, stats);
                    break;
                case JSON:
                    exportJson(writer, stats);
                    break;
                case TEXT:
                    exportText(writer, stats);
                    break;
            }
        }
    }
    
    private static void exportCsv(PrintWriter writer, Map<String, PerformanceStats> stats) {
        writer.println("Name,TotalTime,AverageTime,MaxTime,MinTime,CallCount");
        for (Map.Entry<String, PerformanceStats> entry : stats.entrySet()) {
            PerformanceStats s = entry.getValue();
            writer.printf("%s,%d,%d,%d,%d,%d%n",
                s.getName(), s.getTotalTime(), s.getAverageTime(),
                s.getMaxTime(), s.getMinTime(), s.getCallCount());
        }
    }
    
    private static void exportJson(PrintWriter writer, Map<String, PerformanceStats> stats) {
        writer.println("{");
        writer.println("  \"timestamp\": \"" + 
            LocalDateTime.now().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME) + "\",");
        writer.println("  \"metrics\": {");
        
        boolean first = true;
        for (Map.Entry<String, PerformanceStats> entry : stats.entrySet()) {
            if (!first) writer.println(",");
            PerformanceStats s = entry.getValue();
            writer.printf("    \"%s\": {\"avg\": %d, \"max\": %d, \"min\": %d, \"calls\": %d}",
                s.getName(), s.getAverageTime(), s.getMaxTime(), 
                s.getMinTime(), s.getCallCount());
            first = false;
        }
        
        writer.println();
        writer.println("  }");
        writer.println("}");
    }
    
    private static void exportText(PrintWriter writer, Map<String, PerformanceStats> stats) {
        writer.println("Performance Metrics Report");
        writer.println("Generated: " + LocalDateTime.now());
        writer.println("=" .repeat(60));
        writer.println();
        
        for (Map.Entry<String, PerformanceStats> entry : stats.entrySet()) {
            writer.println(entry.getValue().toString());
        }
    }
    
    public static String exportToString(ExportFormat format) {
        StringBuilder sb = new StringBuilder();
        PerformanceMonitor monitor = PerformanceMonitor.getInstance();
        Map<String, PerformanceStats> stats = monitor.getAllStats();
        
        switch (format) {
            case CSV:
                sb.append("Name,TotalTime,AverageTime,MaxTime,MinTime,CallCount\n");
                for (PerformanceStats s : stats.values()) {
                    sb.append(s.getName()).append(",")
                      .append(s.getTotalTime()).append(",")
                      .append(s.getAverageTime()).append(",")
                      .append(s.getMaxTime()).append(",")
                      .append(s.getMinTime()).append(",")
                      .append(s.getCallCount()).append("\n");
                }
                break;
            default:
                for (PerformanceStats s : stats.values()) {
                    sb.append(s.toString()).append("\n");
                }
        }
        
        return sb.toString();
    }
}
```

---

## 6.14 渲染核心接口 (graph)

### 6.14.1 LabelEngine.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Geometry;
import java.util.List;

public class LabelEngine extends NativeObject {
    
    public enum LabelPlacement {
        POINT, LINE, POLYGON, AUTO
    }
    
    public enum ConflictResolution {
        NONE, HIDE, MOVE, ABBREVIATE
    }
    
    public LabelEngine() {
        setNativePtr(nativeCreate());
    }
    
    public void setEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetEnabled(getNativePtr(), enabled);
    }
    
    public boolean isEnabled() {
        checkNotDisposed();
        return nativeIsEnabled(getNativePtr());
    }
    
    public void setMinScale(double minScale) {
        checkNotDisposed();
        nativeSetMinScale(getNativePtr(), minScale);
    }
    
    public void setMaxScale(double maxScale) {
        checkNotDisposed();
        nativeSetMaxScale(getNativePtr(), maxScale);
    }
    
    public void setConflictResolution(ConflictResolution mode) {
        checkNotDisposed();
        nativeSetConflictResolution(getNativePtr(), mode.ordinal());
    }
    
    public void setLabelBuffer(double buffer) {
        checkNotDisposed();
        nativeSetLabelBuffer(getNativePtr(), buffer);
    }
    
    public List<LabelInfo> generateLabels(List<Geometry> geometries, 
                                           List<String> texts,
                                           double scale) {
        checkNotDisposed();
        long[] geomPtrs = new long[geometries.size()];
        for (int i = 0; i < geometries.size(); i++) {
            geomPtrs[i] = geometries.get(i).getNativePtr();
        }
        return nativeGenerateLabels(getNativePtr(), geomPtrs, 
                                    texts.toArray(new String[0]), scale);
    }
    
    public void clearCache() {
        checkNotDisposed();
        nativeClearCache(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetEnabled(long ptr, boolean enabled);
    private native boolean nativeIsEnabled(long ptr);
    private native void nativeSetMinScale(long ptr, double scale);
    private native void nativeSetMaxScale(long ptr, double scale);
    private native void nativeSetConflictResolution(long ptr, int mode);
    private native void nativeSetLabelBuffer(long ptr, double buffer);
    private native List<LabelInfo> nativeGenerateLabels(long ptr, long[] geomPtrs, 
                                                         String[] texts, double scale);
    private native void nativeClearCache(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.14.2 LabelInfo.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.api.geometry.Coordinate;

public class LabelInfo {
    
    private final String text;
    private final Coordinate position;
    private final double rotation;
    private final double width;
    private final double height;
    private final boolean visible;
    
    public LabelInfo(String text, Coordinate position, double rotation,
                     double width, double height, boolean visible) {
        this.text = text;
        this.position = position;
        this.rotation = rotation;
        this.width = width;
        this.height = height;
        this.visible = visible;
    }
    
    public String getText() { return text; }
    public Coordinate getPosition() { return position; }
    public double getRotation() { return rotation; }
    public double getWidth() { return width; }
    public double getHeight() { return height; }
    public boolean isVisible() { return visible; }
}
```

### 6.14.3 LODManager.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;

public class LODManager extends NativeObject {
    
    public enum LODStrategy {
        DISTANCE, SCALE, PIXEL_SIZE
    }
    
    public LODManager() {
        setNativePtr(nativeCreate());
    }
    
    public void setStrategy(LODStrategy strategy) {
        checkNotDisposed();
        nativeSetStrategy(getNativePtr(), strategy.ordinal());
    }
    
    public LODStrategy getStrategy() {
        checkNotDisposed();
        return LODStrategy.values()[nativeGetStrategy(getNativePtr())];
    }
    
    public void addLevel(LODLevel level) {
        checkNotDisposed();
        nativeAddLevel(getNativePtr(), level.getMinScale(), level.getMaxScale(), 
                       level.getDetailFactor());
    }
    
    public void removeLevel(int index) {
        checkNotDisposed();
        nativeRemoveLevel(getNativePtr(), index);
    }
    
    public int getLevelCount() {
        checkNotDisposed();
        return nativeGetLevelCount(getNativePtr());
    }
    
    public LODLevel getLevel(int index) {
        checkNotDisposed();
        return nativeGetLevel(getNativePtr(), index);
    }
    
    public List<LODLevel> getLevels() {
        List<LODLevel> levels = new ArrayList<>();
        int count = getLevelCount();
        for (int i = 0; i < count; i++) {
            levels.add(getLevel(i));
        }
        return levels;
    }
    
    public int getCurrentLevel(double scale) {
        checkNotDisposed();
        return nativeGetCurrentLevel(getNativePtr(), scale);
    }
    
    public void setAutoSwitch(boolean enabled) {
        checkNotDisposed();
        nativeSetAutoSwitch(getNativePtr(), enabled);
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetStrategy(long ptr, int strategy);
    private native int nativeGetStrategy(long ptr);
    private native void nativeAddLevel(long ptr, double minScale, double maxScale, 
                                        double detailFactor);
    private native void nativeRemoveLevel(long ptr, int index);
    private native int nativeGetLevelCount(long ptr);
    private native LODLevel nativeGetLevel(long ptr, int index);
    private native int nativeGetCurrentLevel(long ptr, double scale);
    private native void nativeSetAutoSwitch(long ptr, boolean enabled);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.14.4 LODLevel.java

```java
package cn.cycle.chart.api.graph;

public class LODLevel {
    
    private final double minScale;
    private final double maxScale;
    private final double detailFactor;
    private final String name;
    
    public LODLevel(double minScale, double maxScale, double detailFactor, String name) {
        this.minScale = minScale;
        this.maxScale = maxScale;
        this.detailFactor = detailFactor;
        this.name = name;
    }
    
    public double getMinScale() { return minScale; }
    public double getMaxScale() { return maxScale; }
    public double getDetailFactor() { return detailFactor; }
    public String getName() { return name; }
    
    public boolean isInRange(double scale) {
        return scale >= minScale && scale <= maxScale;
    }
}
```

### 6.14.5 HitTest.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.feature.FeatureInfo;
import java.util.List;

public class HitTest extends NativeObject {
    
    public enum HitTestMode {
        EXACT, BOUNDS, BUFFER
    }
    
    public HitTest() {
        setNativePtr(nativeCreate());
    }
    
    public void setMode(HitTestMode mode) {
        checkNotDisposed();
        nativeSetMode(getNativePtr(), mode.ordinal());
    }
    
    public HitTestMode getMode() {
        checkNotDisposed();
        return HitTestMode.values()[nativeGetMode(getNativePtr())];
    }
    
    public void setTolerance(double tolerance) {
        checkNotDisposed();
        nativeSetTolerance(getNativePtr(), tolerance);
    }
    
    public double getTolerance() {
        checkNotDisposed();
        return nativeGetTolerance(getNativePtr());
    }
    
    public FeatureInfo hitTest(double x, double y) {
        checkNotDisposed();
        long ptr = nativeHitTest(getNativePtr(), x, y);
        return ptr != 0 ? new FeatureInfo(ptr) : null;
    }
    
    public FeatureInfo hitTest(Coordinate coord) {
        return hitTest(coord.getX(), coord.getY());
    }
    
    public List<FeatureInfo> hitTestAll(double x, double y) {
        checkNotDisposed();
        long[] ptrs = nativeHitTestAll(getNativePtr(), x, y);
        return FeatureInfo.fromNativePtrs(ptrs);
    }
    
    public List<FeatureInfo> hitTestRect(Envelope bounds) {
        checkNotDisposed();
        long[] ptrs = nativeHitTestRect(getNativePtr(), bounds.getMinX(), 
                                         bounds.getMinY(), bounds.getMaxX(), 
                                         bounds.getMaxY());
        return FeatureInfo.fromNativePtrs(ptrs);
    }
    
    public void setLayerFilter(String[] layerNames) {
        checkNotDisposed();
        nativeSetLayerFilter(getNativePtr(), layerNames);
    }
    
    public void clearLayerFilter() {
        checkNotDisposed();
        nativeClearLayerFilter(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetMode(long ptr, int mode);
    private native int nativeGetMode(long ptr);
    private native void nativeSetTolerance(long ptr, double tolerance);
    private native double nativeGetTolerance(long ptr);
    private native long nativeHitTest(long ptr, double x, double y);
    private native long[] nativeHitTestAll(long ptr, double x, double y);
    private native long[] nativeHitTestRect(long ptr, double minX, double minY, 
                                             double maxX, double maxY);
    private native void nativeSetLayerFilter(long ptr, String[] layerNames);
    private native void nativeClearLayerFilter(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.14.6 TransformManager.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;

public class TransformManager extends NativeObject {
    
    public TransformManager() {
        setNativePtr(nativeCreate());
    }
    
    public void setViewport(double minX, double minY, double maxX, double maxY) {
        checkNotDisposed();
        nativeSetViewport(getNativePtr(), minX, minY, maxX, maxY);
    }
    
    public Envelope getViewport() {
        checkNotDisposed();
        return nativeGetViewport(getNativePtr());
    }
    
    public void setScreenSize(int width, int height) {
        checkNotDisposed();
        nativeSetScreenSize(getNativePtr(), width, height);
    }
    
    public int getScreenWidth() {
        checkNotDisposed();
        return nativeGetScreenWidth(getNativePtr());
    }
    
    public int getScreenHeight() {
        checkNotDisposed();
        return nativeGetScreenHeight(getNativePtr());
    }
    
    public Coordinate screenToWorld(double screenX, double screenY) {
        checkNotDisposed();
        return nativeScreenToWorld(getNativePtr(), screenX, screenY);
    }
    
    public Coordinate worldToScreen(double worldX, double worldY) {
        checkNotDisposed();
        return nativeWorldToScreen(getNativePtr(), worldX, worldY);
    }
    
    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }
    
    public void setScale(double scale) {
        checkNotDisposed();
        nativeSetScale(getNativePtr(), scale);
    }
    
    public void pan(double dx, double dy) {
        checkNotDisposed();
        nativePan(getNativePtr(), dx, dy);
    }
    
    public void zoom(double factor, double centerX, double centerY) {
        checkNotDisposed();
        nativeZoom(getNativePtr(), factor, centerX, centerY);
    }
    
    public void rotate(double angle) {
        checkNotDisposed();
        nativeRotate(getNativePtr(), angle);
    }
    
    public double getRotation() {
        checkNotDisposed();
        return nativeGetRotation(getNativePtr());
    }
    
    public void resetTransform() {
        checkNotDisposed();
        nativeResetTransform(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetViewport(long ptr, double minX, double minY, 
                                           double maxX, double maxY);
    private native Envelope nativeGetViewport(long ptr);
    private native void nativeSetScreenSize(long ptr, int width, int height);
    private native int nativeGetScreenWidth(long ptr);
    private native int nativeGetScreenHeight(long ptr);
    private native Coordinate nativeScreenToWorld(long ptr, double x, double y);
    private native Coordinate nativeWorldToScreen(long ptr, double x, double y);
    private native double nativeGetScale(long ptr);
    private native void nativeSetScale(long ptr, double scale);
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor, double cx, double cy);
    private native void nativeRotate(long ptr, double angle);
    private native double nativeGetRotation(long ptr);
    private native void nativeResetTransform(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.14.7 RenderTask.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class RenderTask extends NativeObject {
    
    public enum TaskState {
        PENDING, RUNNING, COMPLETED, FAILED, CANCELLED
    }
    
    public enum TaskPriority {
        LOW, NORMAL, HIGH, URGENT
    }
    
    public RenderTask() {
        setNativePtr(nativeCreate());
    }
    
    public String getTaskId() {
        checkNotDisposed();
        return nativeGetTaskId(getNativePtr());
    }
    
    public TaskState getState() {
        checkNotDisposed();
        return TaskState.values()[nativeGetState(getNativePtr())];
    }
    
    public void setPriority(TaskPriority priority) {
        checkNotDisposed();
        nativeSetPriority(getNativePtr(), priority.ordinal());
    }
    
    public TaskPriority getPriority() {
        checkNotDisposed();
        return TaskPriority.values()[nativeGetPriority(getNativePtr())];
    }
    
    public void setBounds(Envelope bounds) {
        checkNotDisposed();
        nativeSetBounds(getNativePtr(), bounds.getMinX(), bounds.getMinY(),
                        bounds.getMaxX(), bounds.getMaxY());
    }
    
    public Envelope getBounds() {
        checkNotDisposed();
        return nativeGetBounds(getNativePtr());
    }
    
    public void setScale(double scale) {
        checkNotDisposed();
        nativeSetScale(getNativePtr(), scale);
    }
    
    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }
    
    public double getProgress() {
        checkNotDisposed();
        return nativeGetProgress(getNativePtr());
    }
    
    public void cancel() {
        checkNotDisposed();
        nativeCancel(getNativePtr());
    }
    
    public boolean isCancelled() {
        checkNotDisposed();
        return nativeIsCancelled(getNativePtr());
    }
    
    public long getStartTime() {
        checkNotDisposed();
        return nativeGetStartTime(getNativePtr());
    }
    
    public long getEndTime() {
        checkNotDisposed();
        return nativeGetEndTime(getNativePtr());
    }
    
    public long getElapsedTime() {
        checkNotDisposed();
        return nativeGetElapsedTime(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native String nativeGetTaskId(long ptr);
    private native int nativeGetState(long ptr);
    private native void nativeSetPriority(long ptr, int priority);
    private native int nativeGetPriority(long ptr);
    private native void nativeSetBounds(long ptr, double minX, double minY, 
                                         double maxX, double maxY);
    private native Envelope nativeGetBounds(long ptr);
    private native void nativeSetScale(long ptr, double scale);
    private native double nativeGetScale(long ptr);
    private native double nativeGetProgress(long ptr);
    private native void nativeCancel(long ptr);
    private native boolean nativeIsCancelled(long ptr);
    private native long nativeGetStartTime(long ptr);
    private native long nativeGetEndTime(long ptr);
    private native long nativeGetElapsedTime(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.14.8 RenderQueue.java

```java
package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;
import java.util.ArrayList;
import java.util.function.Consumer;

public class RenderQueue extends NativeObject {
    
    public RenderQueue() {
        setNativePtr(nativeCreate());
    }
    
    public void submit(RenderTask task) {
        checkNotDisposed();
        nativeSubmit(getNativePtr(), task.getNativePtr());
    }
    
    public void submitWithCallback(RenderTask task, Consumer<RenderTask> callback) {
        checkNotDisposed();
        nativeSubmitWithCallback(getNativePtr(), task.getNativePtr(), callback);
    }
    
    public RenderTask getNext() {
        checkNotDisposed();
        long ptr = nativeGetNext(getNativePtr());
        return ptr != 0 ? new RenderTask(ptr) : null;
    }
    
    public int getQueueSize() {
        checkNotDisposed();
        return nativeGetQueueSize(getNativePtr());
    }
    
    public int getPendingCount() {
        checkNotDisposed();
        return nativeGetPendingCount(getNativePtr());
    }
    
    public int getRunningCount() {
        checkNotDisposed();
        return nativeGetRunningCount(getNativePtr());
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public void pause() {
        checkNotDisposed();
        nativePause(getNativePtr());
    }
    
    public void resume() {
        checkNotDisposed();
        nativeResume(getNativePtr());
    }
    
    public boolean isPaused() {
        checkNotDisposed();
        return nativeIsPaused(getNativePtr());
    }
    
    public void setMaxConcurrent(int max) {
        checkNotDisposed();
        nativeSetMaxConcurrent(getNativePtr(), max);
    }
    
    public int getMaxConcurrent() {
        checkNotDisposed();
        return nativeGetMaxConcurrent(getNativePtr());
    }
    
    public List<RenderTask> getAllTasks() {
        checkNotDisposed();
        long[] ptrs = nativeGetAllTasks(getNativePtr());
        List<RenderTask> tasks = new ArrayList<>();
        for (long ptr : ptrs) {
            if (ptr != 0) {
                tasks.add(new RenderTask(ptr));
            }
        }
        return tasks;
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSubmit(long ptr, long taskPtr);
    private native void nativeSubmitWithCallback(long ptr, long taskPtr, Object callback);
    private native long nativeGetNext(long ptr);
    private native int nativeGetQueueSize(long ptr);
    private native int nativeGetPendingCount(long ptr);
    private native int nativeGetRunningCount(long ptr);
    private native void nativeClear(long ptr);
    private native void nativePause(long ptr);
    private native void nativeResume(long ptr);
    private native boolean nativeIsPaused(long ptr);
    private native void nativeSetMaxConcurrent(long ptr, int max);
    private native int nativeGetMaxConcurrent(long ptr);
    private native long[] nativeGetAllTasks(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.15 导航扩展接口 (navi)

### 6.15.1 IPositionProvider.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

public interface IPositionProvider {
    
    String getName();
    String getType();
    boolean isConnected();
    void connect() throws Exception;
    void disconnect();
    Coordinate getPosition();
    double getSpeed();
    double getCourse();
    double getAltitude();
    PositionQuality getQuality();
    long getTimestamp();
    void setPositionListener(PositionListener listener);
    
    interface PositionListener {
        void onPositionUpdate(PositionUpdate update);
        void onConnectionLost();
        void onError(Exception e);
    }
}
```

### 6.15.2 PositionQuality.java

```java
package cn.cycle.chart.api.navi;

public enum PositionQuality {
    INVALID(0),
    GPS(1),
    DGPS(2),
    RTK_FIXED(3),
    RTK_FLOAT(4);
    
    private final int code;
    PositionQuality(int code) { this.code = code; }
    public int getCode() { return code; }
    
    public static PositionQuality fromCode(int code) {
        for (PositionQuality q : values()) {
            if (q.code == code) return q;
        }
        return INVALID;
    }
}
```

### 6.15.3 NmeaParser.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public class NmeaParser extends NativeObject {
    
    public NmeaParser() {
        setNativePtr(nativeCreate());
    }
    
    public PositionUpdate parse(String sentence) {
        checkNotDisposed();
        return nativeParse(getNativePtr(), sentence);
    }
    
    public boolean isValid(String sentence) {
        checkNotDisposed();
        return nativeIsValid(getNativePtr(), sentence);
    }
    
    public String getSentenceType(String sentence) {
        checkNotDisposed();
        return nativeGetSentenceType(getNativePtr(), sentence);
    }
    
    public String[] parseGga(String sentence) {
        checkNotDisposed();
        return nativeParseGga(getNativePtr(), sentence);
    }
    
    public String[] parseRmc(String sentence) {
        checkNotDisposed();
        return nativeParseRmc(getNativePtr(), sentence);
    }
    
    public String[] parseVtg(String sentence) {
        checkNotDisposed();
        return nativeParseVtg(getNativePtr(), sentence);
    }
    
    public String generateChecksum(String sentence) {
        checkNotDisposed();
        return nativeGenerateChecksum(getNativePtr(), sentence);
    }
    
    public boolean verifyChecksum(String sentence) {
        checkNotDisposed();
        return nativeVerifyChecksum(getNativePtr(), sentence);
    }
    
    // Native方法
    private native long nativeCreate();
    private native PositionUpdate nativeParse(long ptr, String sentence);
    private native boolean nativeIsValid(long ptr, String sentence);
    private native String nativeGetSentenceType(long ptr, String sentence);
    private native String[] nativeParseGga(long ptr, String sentence);
    private native String[] nativeParseRmc(long ptr, String sentence);
    private native String[] nativeParseVtg(long ptr, String sentence);
    private native String nativeGenerateChecksum(long ptr, String sentence);
    private native boolean nativeVerifyChecksum(long ptr, String sentence);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.15.4 CollisionAssessor.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;

public class CollisionAssessor extends NativeObject {
    
    public CollisionAssessor() {
        setNativePtr(nativeCreate());
    }
    
    public CpaResult calculateCpa(AisTarget ownShip, AisTarget target) {
        checkNotDisposed();
        return nativeCalculateCpa(getNativePtr(), ownShip.getNativePtr(), 
                                  target.getNativePtr());
    }
    
    public List<CpaResult> assessAll(AisTarget ownShip, List<AisTarget> targets) {
        checkNotDisposed();
        long[] targetPtrs = new long[targets.size()];
        for (int i = 0; i < targets.size(); i++) {
            targetPtrs[i] = targets.get(i).getNativePtr();
        }
        return nativeAssessAll(getNativePtr(), ownShip.getNativePtr(), targetPtrs);
    }
    
    public void setCpaThreshold(double cpaNm) {
        checkNotDisposed();
        nativeSetCpaThreshold(getNativePtr(), cpaNm);
    }
    
    public double getCpaThreshold() {
        checkNotDisposed();
        return nativeGetCpaThreshold(getNativePtr());
    }
    
    public void setTcpaThreshold(double tcpaMinutes) {
        checkNotDisposed();
        nativeSetTcpaThreshold(getNativePtr(), tcpaMinutes);
    }
    
    public double getTcpaThreshold() {
        checkNotDisposed();
        return nativeGetTcpaThreshold(getNativePtr());
    }
    
    public List<AisTarget> getDangerousTargets() {
        checkNotDisposed();
        long[] ptrs = nativeGetDangerousTargets(getNativePtr());
        return AisTarget.fromNativePtrs(ptrs);
    }
    
    // Native方法
    private native long nativeCreate();
    private native CpaResult nativeCalculateCpa(long ptr, long ownPtr, long targetPtr);
    private native List<CpaResult> nativeAssessAll(long ptr, long ownPtr, long[] targetPtrs);
    private native void nativeSetCpaThreshold(long ptr, double cpa);
    private native double nativeGetCpaThreshold(long ptr);
    private native void nativeSetTcpaThreshold(long ptr, double tcpa);
    private native double nativeGetTcpaThreshold(long ptr);
    private native long[] nativeGetDangerousTargets(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.15.5 CpaResult.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

public class CpaResult {
    
    private final long targetMmsi;
    private final double cpa;
    private final double tcpa;
    private final Coordinate cpaPosition;
    private final boolean isDangerous;
    private final double riskLevel;
    
    public CpaResult(long targetMmsi, double cpa, double tcpa, 
                     Coordinate cpaPosition, boolean isDangerous, double riskLevel) {
        this.targetMmsi = targetMmsi;
        this.cpa = cpa;
        this.tcpa = tcpa;
        this.cpaPosition = cpaPosition;
        this.isDangerous = isDangerous;
        this.riskLevel = riskLevel;
    }
    
    public long getTargetMmsi() { return targetMmsi; }
    public double getCpa() { return cpa; }
    public double getTcpa() { return tcpa; }
    public Coordinate getCpaPosition() { return cpaPosition; }
    public boolean isDangerous() { return isDangerous; }
    public double getRiskLevel() { return riskLevel; }
}
```

### 6.15.6 UkcCalculator.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public class UkcCalculator extends NativeObject {
    
    public UkcCalculator() {
        setNativePtr(nativeCreate());
    }
    
    public void setShipDraft(double draft) {
        checkNotDisposed();
        nativeSetShipDraft(getNativePtr(), draft);
    }
    
    public double getShipDraft() {
        checkNotDisposed();
        return nativeGetShipDraft(getNativePtr());
    }
    
    public void setSafetyMargin(double margin) {
        checkNotDisposed();
        nativeSetSafetyMargin(getNativePtr(), margin);
    }
    
    public double getSafetyMargin() {
        checkNotDisposed();
        return nativeGetSafetyMargin(getNativePtr());
    }
    
    public void setSquatFactor(double factor) {
        checkNotDisposed();
        nativeSetSquatFactor(getNativePtr(), factor);
    }
    
    public double calculateUkc(double chartedDepth, double tideHeight, 
                                double speed, double waveHeight) {
        checkNotDisposed();
        return nativeCalculateUkc(getNativePtr(), chartedDepth, tideHeight, 
                                   speed, waveHeight);
    }
    
    public double calculateRequiredDepth(double ukcRequired, double tideHeight) {
        checkNotDisposed();
        return nativeCalculateRequiredDepth(getNativePtr(), ukcRequired, tideHeight);
    }
    
    public boolean isSafe(double chartedDepth, double tideHeight, 
                          double speed, double waveHeight) {
        checkNotDisposed();
        return nativeIsSafe(getNativePtr(), chartedDepth, tideHeight, 
                            speed, waveHeight);
    }
    
    public double getMaxSafeSpeed(double chartedDepth, double tideHeight, 
                                   double minUkc) {
        checkNotDisposed();
        return nativeGetMaxSafeSpeed(getNativePtr(), chartedDepth, tideHeight, minUkc);
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetShipDraft(long ptr, double draft);
    private native double nativeGetShipDraft(long ptr);
    private native void nativeSetSafetyMargin(long ptr, double margin);
    private native double nativeGetSafetyMargin(long ptr);
    private native void nativeSetSquatFactor(long ptr, double factor);
    private native double nativeCalculateUkc(long ptr, double chartedDepth, 
                                              double tideHeight, double speed, 
                                              double waveHeight);
    private native double nativeCalculateRequiredDepth(long ptr, double ukc, double tide);
    private native boolean nativeIsSafe(long ptr, double chartedDepth, double tideHeight,
                                         double speed, double waveHeight);
    private native double nativeGetMaxSafeSpeed(long ptr, double chartedDepth, 
                                                 double tideHeight, double minUkc);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.15.7 OffCourseDetector.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.LineString;

public class OffCourseDetector extends NativeObject {
    
    public OffCourseDetector() {
        setNativePtr(nativeCreate());
    }
    
    public void setRoute(Route route) {
        checkNotDisposed();
        nativeSetRoute(getNativePtr(), route.getNativePtr());
    }
    
    public void clearRoute() {
        checkNotDisposed();
        nativeClearRoute(getNativePtr());
    }
    
    public void setMaxDeviation(double maxDeviationNm) {
        checkNotDisposed();
        nativeSetMaxDeviation(getNativePtr(), maxDeviationNm);
    }
    
    public double getMaxDeviation() {
        checkNotDisposed();
        return nativeGetMaxDeviation(getNativePtr());
    }
    
    public void setAlertThreshold(double thresholdNm) {
        checkNotDisposed();
        nativeSetAlertThreshold(getNativePtr(), thresholdNm);
    }
    
    public double getAlertThreshold() {
        checkNotDisposed();
        return nativeGetAlertThreshold(getNativePtr());
    }
    
    public DeviationResult checkDeviation(Coordinate currentPosition) {
        checkNotDisposed();
        return nativeCheckDeviation(getNativePtr(), currentPosition.getX(), 
                                    currentPosition.getY());
    }
    
    public double getCrossTrackError(Coordinate currentPosition) {
        checkNotDisposed();
        return nativeGetCrossTrackError(getNativePtr(), currentPosition.getX(), 
                                         currentPosition.getY());
    }
    
    public int getCurrentLegIndex(Coordinate currentPosition) {
        checkNotDisposed();
        return nativeGetCurrentLegIndex(getNativePtr(), currentPosition.getX(), 
                                         currentPosition.getY());
    }
    
    public Coordinate getClosestPointOnRoute(Coordinate currentPosition) {
        checkNotDisposed();
        return nativeGetClosestPointOnRoute(getNativePtr(), currentPosition.getX(), 
                                             currentPosition.getY());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetRoute(long ptr, long routePtr);
    private native void nativeClearRoute(long ptr);
    private native void nativeSetMaxDeviation(long ptr, double deviation);
    private native double nativeGetMaxDeviation(long ptr);
    private native void nativeSetAlertThreshold(long ptr, double threshold);
    private native double nativeGetAlertThreshold(long ptr);
    private native DeviationResult nativeCheckDeviation(long ptr, double x, double y);
    private native double nativeGetCrossTrackError(long ptr, double x, double y);
    private native int nativeGetCurrentLegIndex(long ptr, double x, double y);
    private native Coordinate nativeGetClosestPointOnRoute(long ptr, double x, double y);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.15.8 DeviationResult.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

public class DeviationResult {
    
    private final double crossTrackError;
    private final double distanceToRoute;
    private final int currentLegIndex;
    private final Coordinate closestPoint;
    private final boolean isOffCourse;
    private final DeviationSide deviationSide;
    
    public enum DeviationSide {
        PORT, STARBOARD, ON_ROUTE
    }
    
    public DeviationResult(double xte, double distance, int legIndex, 
                           Coordinate closest, boolean offCourse, int side) {
        this.crossTrackError = xte;
        this.distanceToRoute = distance;
        this.currentLegIndex = legIndex;
        this.closestPoint = closest;
        this.isOffCourse = offCourse;
        this.deviationSide = DeviationSide.values()[side];
    }
    
    public double getCrossTrackError() { return crossTrackError; }
    public double getDistanceToRoute() { return distanceToRoute; }
    public int getCurrentLegIndex() { return currentLegIndex; }
    public Coordinate getClosestPoint() { return closestPoint; }
    public boolean isOffCourse() { return isOffCourse; }
    public DeviationSide getDeviationSide() { return deviationSide; }
}
```

### 6.15.9 RoutePlanner.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import java.util.List;

public class RoutePlanner extends NativeObject {
    
    public enum PlanningMode {
        SHORTEST, SAFEST, FUEL_EFFICIENT, CUSTOM
    }
    
    public RoutePlanner() {
        setNativePtr(nativeCreate());
    }
    
    public void setPlanningMode(PlanningMode mode) {
        checkNotDisposed();
        nativeSetPlanningMode(getNativePtr(), mode.ordinal());
    }
    
    public PlanningMode getPlanningMode() {
        checkNotDisposed();
        return PlanningMode.values()[nativeGetPlanningMode(getNativePtr())];
    }
    
    public void setAvoidanceAreas(List<Envelope> areas) {
        checkNotDisposed();
        double[] coords = new double[areas.size() * 4];
        for (int i = 0; i < areas.size(); i++) {
            Envelope e = areas.get(i);
            coords[i * 4] = e.getMinX();
            coords[i * 4 + 1] = e.getMinY();
            coords[i * 4 + 2] = e.getMaxX();
            coords[i * 4 + 3] = e.getMaxY();
        }
        nativeSetAvoidanceAreas(getNativePtr(), coords);
    }
    
    public void clearAvoidanceAreas() {
        checkNotDisposed();
        nativeClearAvoidanceAreas(getNativePtr());
    }
    
    public void setMinDepth(double minDepth) {
        checkNotDisposed();
        nativeSetMinDepth(getNativePtr(), minDepth);
    }
    
    public double getMinDepth() {
        checkNotDisposed();
        return nativeGetMinDepth(getNativePtr());
    }
    
    public Route planRoute(Coordinate start, Coordinate end) {
        checkNotDisposed();
        long ptr = nativePlanRoute(getNativePtr(), start.getX(), start.getY(),
                                   end.getX(), end.getY());
        return ptr != 0 ? new Route(ptr) : null;
    }
    
    public Route planRouteWithWaypoints(List<Coordinate> waypoints) {
        checkNotDisposed();
        double[] coords = new double[waypoints.size() * 2];
        for (int i = 0; i < waypoints.size(); i++) {
            coords[i * 2] = waypoints.get(i).getX();
            coords[i * 2 + 1] = waypoints.get(i).getY();
        }
        long ptr = nativePlanRouteWithWaypoints(getNativePtr(), coords);
        return ptr != 0 ? new Route(ptr) : null;
    }
    
    public List<Route> getAlternativeRoutes() {
        checkNotDisposed();
        long[] ptrs = nativeGetAlternativeRoutes(getNativePtr());
        return Route.fromNativePtrs(ptrs);
    }
    
    public double estimateFuelConsumption(Route route) {
        checkNotDisposed();
        return nativeEstimateFuelConsumption(getNativePtr(), route.getNativePtr());
    }
    
    public double estimateTravelTime(Route route, double avgSpeed) {
        checkNotDisposed();
        return nativeEstimateTravelTime(getNativePtr(), route.getNativePtr(), avgSpeed);
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetPlanningMode(long ptr, int mode);
    private native int nativeGetPlanningMode(long ptr);
    private native void nativeSetAvoidanceAreas(long ptr, double[] coords);
    private native void nativeClearAvoidanceAreas(long ptr);
    private native void nativeSetMinDepth(long ptr, double depth);
    private native double nativeGetMinDepth(long ptr);
    private native long nativePlanRoute(long ptr, double startX, double startY,
                                         double endX, double endY);
    private native long nativePlanRouteWithWaypoints(long ptr, double[] coords);
    private native long[] nativeGetAlternativeRoutes(long ptr);
    private native double nativeEstimateFuelConsumption(long ptr, long routePtr);
    private native double nativeEstimateTravelTime(long ptr, long routePtr, double avgSpeed);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.15.10 TrackRecorder.java

```java
package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.List;

public class TrackRecorder extends NativeObject {
    
    public enum RecordingMode {
        CONTINUOUS, INTERVAL, DISTANCE
    }
    
    public TrackRecorder() {
        setNativePtr(nativeCreate());
    }
    
    public void setRecordingMode(RecordingMode mode) {
        checkNotDisposed();
        nativeSetRecordingMode(getNativePtr(), mode.ordinal());
    }
    
    public RecordingMode getRecordingMode() {
        checkNotDisposed();
        return RecordingMode.values()[nativeGetRecordingMode(getNativePtr())];
    }
    
    public void setInterval(long intervalMs) {
        checkNotDisposed();
        nativeSetInterval(getNativePtr(), intervalMs);
    }
    
    public long getInterval() {
        checkNotDisposed();
        return nativeGetInterval(getNativePtr());
    }
    
    public void setMinDistance(double distanceNm) {
        checkNotDisposed();
        nativeSetMinDistance(getNativePtr(), distanceNm);
    }
    
    public double getMinDistance() {
        checkNotDisposed();
        return nativeGetMinDistance(getNativePtr());
    }
    
    public void start() {
        checkNotDisposed();
        nativeStart(getNativePtr());
    }
    
    public void stop() {
        checkNotDisposed();
        nativeStop(getNativePtr());
    }
    
    public boolean isRecording() {
        checkNotDisposed();
        return nativeIsRecording(getNativePtr());
    }
    
    public void pause() {
        checkNotDisposed();
        nativePause(getNativePtr());
    }
    
    public void resume() {
        checkNotDisposed();
        nativeResume(getNativePtr());
    }
    
    public void recordPoint(Coordinate position, double cog, double sog, double depth) {
        checkNotDisposed();
        nativeRecordPoint(getNativePtr(), position.getX(), position.getY(), 
                          cog, sog, depth);
    }
    
    public Track getCurrentTrack() {
        checkNotDisposed();
        long ptr = nativeGetCurrentTrack(getNativePtr());
        return ptr != 0 ? new Track(ptr) : null;
    }
    
    public void saveTrack(String filePath) {
        checkNotDisposed();
        nativeSaveTrack(getNativePtr(), filePath);
    }
    
    public Track loadTrack(String filePath) {
        checkNotDisposed();
        long ptr = nativeLoadTrack(getNativePtr(), filePath);
        return ptr != 0 ? new Track(ptr) : null;
    }
    
    public void clearCurrentTrack() {
        checkNotDisposed();
        nativeClearCurrentTrack(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetRecordingMode(long ptr, int mode);
    private native int nativeGetRecordingMode(long ptr);
    private native void nativeSetInterval(long ptr, long interval);
    private native long nativeGetInterval(long ptr);
    private native void nativeSetMinDistance(long ptr, double distance);
    private native double nativeGetMinDistance(long ptr);
    private native void nativeStart(long ptr);
    private native void nativeStop(long ptr);
    private native boolean nativeIsRecording(long ptr);
    private native void nativePause(long ptr);
    private native void nativeResume(long ptr);
    private native void nativeRecordPoint(long ptr, double x, double y, 
                                           double cog, double sog, double depth);
    private native long nativeGetCurrentTrack(long ptr);
    private native void nativeSaveTrack(long ptr, String filePath);
    private native long nativeLoadTrack(long ptr, String filePath);
    private native void nativeClearCurrentTrack(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.16 海图解析接口 (parser)

### 6.16.1 ChartParser.java

```java
package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.geometry.Envelope;
import java.util.List;

public class ChartParser extends NativeObject {
    
    public enum ChartFormat {
        S57, S101, S63, AUTO
    }
    
    public ChartParser() {
        setNativePtr(nativeCreate());
    }
    
    public ChartParser(ChartFormat format) {
        setNativePtr(nativeCreateWithFormat(format.ordinal()));
    }
    
    public void setFormat(ChartFormat format) {
        checkNotDisposed();
        nativeSetFormat(getNativePtr(), format.ordinal());
    }
    
    public ChartFormat getFormat() {
        checkNotDisposed();
        return ChartFormat.values()[nativeGetFormat(getNativePtr())];
    }
    
    public boolean open(String filePath) {
        checkNotDisposed();
        return nativeOpen(getNativePtr(), filePath);
    }
    
    public void close() {
        checkNotDisposed();
        nativeClose(getNativePtr());
    }
    
    public boolean isOpen() {
        checkNotDisposed();
        return nativeIsOpen(getNativePtr());
    }
    
    public String getChartTitle() {
        checkNotDisposed();
        return nativeGetChartTitle(getNativePtr());
    }
    
    public Envelope getBounds() {
        checkNotDisposed();
        return nativeGetBounds(getNativePtr());
    }
    
    public int getFeatureCount() {
        checkNotDisposed();
        return nativeGetFeatureCount(getNativePtr());
    }
    
    public List<FeatureInfo> getAllFeatures() {
        checkNotDisposed();
        long[] ptrs = nativeGetAllFeatures(getNativePtr());
        return FeatureInfo.fromNativePtrs(ptrs);
    }
    
    public List<FeatureInfo> getFeaturesByClass(String featureClass) {
        checkNotDisposed();
        long[] ptrs = nativeGetFeaturesByClass(getNativePtr(), featureClass);
        return FeatureInfo.fromNativePtrs(ptrs);
    }
    
    public List<FeatureInfo> getFeaturesInBounds(Envelope bounds) {
        checkNotDisposed();
        long[] ptrs = nativeGetFeaturesInBounds(getNativePtr(), bounds.getMinX(),
                                                 bounds.getMinY(), bounds.getMaxX(),
                                                 bounds.getMaxY());
        return FeatureInfo.fromNativePtrs(ptrs);
    }
    
    public FeatureInfo getFeatureById(String featureId) {
        checkNotDisposed();
        long ptr = nativeGetFeatureById(getNativePtr(), featureId);
        return ptr != 0 ? new FeatureInfo(ptr) : null;
    }
    
    public List<String> getFeatureClasses() {
        checkNotDisposed();
        return nativeGetFeatureClasses(getNativePtr());
    }
    
    public String getMetadata(String key) {
        checkNotDisposed();
        return nativeGetMetadata(getNativePtr(), key);
    }
    
    public void setS52LookupTable(String lookupTablePath) {
        checkNotDisposed();
        nativeSetS52LookupTable(getNativePtr(), lookupTablePath);
    }
    
    // Native方法
    private native long nativeCreate();
    private native long nativeCreateWithFormat(int format);
    private native void nativeSetFormat(long ptr, int format);
    private native int nativeGetFormat(long ptr);
    private native boolean nativeOpen(long ptr, String filePath);
    private native void nativeClose(long ptr);
    private native boolean nativeIsOpen(long ptr);
    private native String nativeGetChartTitle(long ptr);
    private native Envelope nativeGetBounds(long ptr);
    private native int nativeGetFeatureCount(long ptr);
    private native long[] nativeGetAllFeatures(long ptr);
    private native long[] nativeGetFeaturesByClass(long ptr, String featureClass);
    private native long[] nativeGetFeaturesInBounds(long ptr, double minX, double minY,
                                                     double maxX, double maxY);
    private native long nativeGetFeatureById(long ptr, String featureId);
    private native List<String> nativeGetFeatureClasses(long ptr);
    private native String nativeGetMetadata(long ptr, String key);
    private native void nativeSetS52LookupTable(long ptr, String path);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.16.2 IncrementalParser.java

```java
package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;
import java.util.function.Consumer;

public class IncrementalParser extends NativeObject {
    
    public IncrementalParser() {
        setNativePtr(nativeCreate());
    }
    
    public void open(String filePath) {
        checkNotDisposed();
        nativeOpen(getNativePtr(), filePath);
    }
    
    public void close() {
        checkNotDisposed();
        nativeClose(getNativePtr());
    }
    
    public boolean isOpen() {
        checkNotDisposed();
        return nativeIsOpen(getNativePtr());
    }
    
    public void setFeatureCallback(Consumer<FeatureInfo> callback) {
        checkNotDisposed();
        nativeSetFeatureCallback(getNativePtr(), callback);
    }
    
    public void parseNext() {
        checkNotDisposed();
        nativeParseNext(getNativePtr());
    }
    
    public void parseAll() {
        checkNotDisposed();
        nativeParseAll(getNativePtr());
    }
    
    public void parseBatch(int batchSize) {
        checkNotDisposed();
        nativeParseBatch(getNativePtr(), batchSize);
    }
    
    public double getProgress() {
        checkNotDisposed();
        return nativeGetProgress(getNativePtr());
    }
    
    public void cancel() {
        checkNotDisposed();
        nativeCancel(getNativePtr());
    }
    
    public boolean isCancelled() {
        checkNotDisposed();
        return nativeIsCancelled(getNativePtr());
    }
    
    public int getParsedCount() {
        checkNotDisposed();
        return nativeGetParsedCount(getNativePtr());
    }
    
    public int getTotalCount() {
        checkNotDisposed();
        return nativeGetTotalCount(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeOpen(long ptr, String filePath);
    private native void nativeClose(long ptr);
    private native boolean nativeIsOpen(long ptr);
    private native void nativeSetFeatureCallback(long ptr, Object callback);
    private native void nativeParseNext(long ptr);
    private native void nativeParseAll(long ptr);
    private native void nativeParseBatch(long ptr, int batchSize);
    private native double nativeGetProgress(long ptr);
    private native void nativeCancel(long ptr);
    private native boolean nativeIsCancelled(long ptr);
    private native int nativeGetParsedCount(long ptr);
    private native int nativeGetTotalCount(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.17 安全与验证接口 (security)

### 6.17.1 InputValidator.java

```java
package cn.cycle.chart.api.security;

import java.util.Set;
import java.util.regex.Pattern;

public class InputValidator {
    
    private static final Pattern SAFE_PATH_PATTERN = 
        Pattern.compile("^[a-zA-Z0-9_\\-./]+$");
    private static final Set<String> ALLOWED_EXTENSIONS = 
        Set.of(".000", ".s57", ".s101", ".s63");
    
    public static void validateFilePath(String filePath) {
        if (filePath == null || filePath.isEmpty()) {
            throw new IllegalArgumentException("File path cannot be null or empty");
        }
        
        if (!SAFE_PATH_PATTERN.matcher(filePath).matches()) {
            throw new SecurityException("Invalid characters in file path");
        }
        
        if (filePath.contains("..")) {
            throw new SecurityException("Path traversal detected");
        }
        
        String extension = getFileExtension(filePath);
        if (!ALLOWED_EXTENSIONS.contains(extension.toLowerCase())) {
            throw new SecurityException("File type not allowed: " + extension);
        }
    }
    
    public static void validateCoordinate(double x, double y) {
        if (Double.isNaN(x) || Double.isNaN(y) || 
            Double.isInfinite(x) || Double.isInfinite(y)) {
            throw new IllegalArgumentException("Invalid coordinate values");
        }
        
        if (x < -180 || x > 180 || y < -90 || y > 90) {
            throw new IllegalArgumentException("Coordinate out of valid range");
        }
    }
    
    public static void validateConfig(ChartConfig config) {
        if (config.getCachePath() != null) {
            validateFilePath(config.getCachePath());
        }
        
        if (config.getMaxCacheSize() < 0 || config.getMaxCacheSize() > 1024 * 1024 * 1024) {
            throw new IllegalArgumentException("Invalid cache size");
        }
        
        if (config.getThreadCount() < 1 || config.getThreadCount() > 32) {
            throw new IllegalArgumentException("Invalid thread count");
        }
    }
    
    public static void validateScale(double scale) {
        if (Double.isNaN(scale) || Double.isInfinite(scale) || scale <= 0) {
            throw new IllegalArgumentException("Invalid scale value");
        }
    }
    
    public static void validateIndex(int index, int max) {
        if (index < 0 || index >= max) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + max);
        }
    }
    
    private static String getFileExtension(String path) {
        int lastDot = path.lastIndexOf('.');
        return lastDot >= 0 ? path.substring(lastDot) : "";
    }
}
```

### 6.17.2 SecureLibraryLoader.java

```java
package cn.cycle.chart.api.security;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class SecureLibraryLoader {
    
    private static final Set<String> ALLOWED_LIBRARIES = Set.of(
        "ogc_base", "ogc_geom", "ogc_proj", "ogc_draw", 
        "ogc_feature", "ogc_navi", "ogc_alert", "ogc_chart_jni"
    );
    
    private static final Map<String, String> LIBRARY_HASHES = new ConcurrentHashMap<>();
    private static final Set<String> loadedLibraries = ConcurrentHashMap.newKeySet();
    
    public static void loadLibrarySecurely(String libraryName) {
        if (!ALLOWED_LIBRARIES.contains(libraryName)) {
            throw new SecurityException("Library not in allowlist: " + libraryName);
        }
        
        if (loadedLibraries.contains(libraryName)) {
            return;
        }
        
        String libraryPath = findLibrary(libraryName);
        
        String expectedHash = LIBRARY_HASHES.get(libraryName);
        if (expectedHash != null && !verifyLibraryHash(libraryPath, expectedHash)) {
            throw new SecurityException("Library hash verification failed: " + libraryName);
        }
        
        System.load(libraryPath);
        loadedLibraries.add(libraryName);
    }
    
    public static void registerLibraryHash(String libraryName, String hash) {
        LIBRARY_HASHES.put(libraryName, hash);
    }
    
    public static boolean isLibraryLoaded(String libraryName) {
        return loadedLibraries.contains(libraryName);
    }
    
    private static String findLibrary(String libraryName) {
        String libraryFileName = System.mapLibraryName(libraryName);
        String[] searchPaths = getLibrarySearchPaths();
        
        for (String path : searchPaths) {
            java.nio.file.Path libPath = Paths.get(path, libraryFileName);
            if (Files.exists(libPath)) {
                return libPath.toString();
            }
        }
        
        throw new UnsatisfiedLinkError("Cannot find library: " + libraryName);
    }
    
    private static boolean verifyLibraryHash(String path, String expectedHash) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] fileBytes = Files.readAllBytes(Paths.get(path));
            byte[] hash = digest.digest(fileBytes);
            String actualHash = "sha256:" + bytesToHex(hash);
            return expectedHash.equals(actualHash);
        } catch (Exception e) {
            return false;
        }
    }
    
    private static String[] getLibrarySearchPaths() {
        String customPath = System.getProperty("ogc.library.path");
        if (customPath != null) {
            return new String[] { customPath };
        }
        return new String[] { 
            System.getProperty("java.library.path", "."),
            "./lib",
            "./native"
        };
    }
    
    private static String bytesToHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) {
            sb.append(String.format("%02x", b));
        }
        return sb.toString();
    }
}
```

### 6.17.3 SensitiveDataProtector.java

```java
package cn.cycle.chart.api.security;

import java.util.Arrays;
import java.util.regex.Pattern;

public class SensitiveDataProtector {
    
    private static final Pattern SENSITIVE_PATTERNS = Pattern.compile(
        "(password|token|key|secret|credential)\\s*=\\s*[^\\s]+",
        Pattern.CASE_INSENSITIVE
    );
    
    private static final Pattern COORDINATE_PATTERN = Pattern.compile(
        "\\b\\d{1,3}\\.\\d{4,}[NS]\\s*\\d{1,3}\\.\\d{4,}[EW]\\b"
    );
    
    public static String sanitizeLogMessage(String message) {
        if (message == null) return null;
        
        String sanitized = SENSITIVE_PATTERNS.matcher(message)
            .replaceAll("$1=***REDACTED***");
        
        return sanitized;
    }
    
    public static String sanitizeCoordinate(String message, boolean redactCoordinates) {
        if (message == null || !redactCoordinates) return message;
        
        return COORDINATE_PATTERN.matcher(message)
            .replaceAll("***COORDINATE REDACTED***");
    }
    
    public static void secureClear(byte[] data) {
        if (data != null) {
            Arrays.fill(data, (byte) 0);
        }
    }
    
    public static void secureClear(char[] data) {
        if (data != null) {
            Arrays.fill(data, '\0');
        }
    }
    
    public static void secureClear(int[] data) {
        if (data != null) {
            Arrays.fill(data, 0);
        }
    }
    
    public static void secureClear(long[] data) {
        if (data != null) {
            Arrays.fill(data, 0L);
        }
    }
    
    public static void secureClear(double[] data) {
        if (data != null) {
            Arrays.fill(data, 0.0);
        }
    }
    
    public static byte[] encryptSensitiveData(byte[] data, String key) {
        // 实际实现应使用加密库
        throw new UnsupportedOperationException("Use proper encryption library");
    }
    
    public static byte[] decryptSensitiveData(byte[] encryptedData, String key) {
        // 实际实现应使用加密库
        throw new UnsupportedOperationException("Use proper encryption library");
    }
}
```

---

## 6.18 渲染优化接口 (optimize)

### 6.18.1 RenderOptimizer.java

```java
package cn.cycle.chart.api.optimize;

import cn.cycle.chart.jni.NativeObject;

public class RenderOptimizer extends NativeObject {
    
    private static final int MAX_FPS = 60;
    private static final int MIN_FPS = 10;
    
    public RenderOptimizer() {
        setNativePtr(nativeCreate());
    }
    
    public void setMaxFps(int maxFps) {
        checkNotDisposed();
        nativeSetMaxFps(getNativePtr(), maxFps);
    }
    
    public int getMaxFps() {
        checkNotDisposed();
        return nativeGetMaxFps(getNativePtr());
    }
    
    public void setMinFps(int minFps) {
        checkNotDisposed();
        nativeSetMinFps(getNativePtr(), minFps);
    }
    
    public int getMinFps() {
        checkNotDisposed();
        return nativeGetMinFps(getNativePtr());
    }
    
    public void enableDirtyRectOptimization(boolean enabled) {
        checkNotDisposed();
        nativeEnableDirtyRectOptimization(getNativePtr(), enabled);
    }
    
    public boolean isDirtyRectOptimizationEnabled() {
        checkNotDisposed();
        return nativeIsDirtyRectOptimizationEnabled(getNativePtr());
    }
    
    public void markDirty(double minX, double minY, double maxX, double maxY) {
        checkNotDisposed();
        nativeMarkDirty(getNativePtr(), minX, minY, maxX, maxY);
    }
    
    public void markAllDirty() {
        checkNotDisposed();
        nativeMarkAllDirty(getNativePtr());
    }
    
    public void clearDirty() {
        checkNotDisposed();
        nativeClearDirty(getNativePtr());
    }
    
    public boolean isDirty() {
        checkNotDisposed();
        return nativeIsDirty(getNativePtr());
    }
    
    public void enableBatchRendering(boolean enabled) {
        checkNotDisposed();
        nativeEnableBatchRendering(getNativePtr(), enabled);
    }
    
    public boolean isBatchRenderingEnabled() {
        checkNotDisposed();
        return nativeIsBatchRenderingEnabled(getNativePtr());
    }
    
    public void setBatchSize(int batchSize) {
        checkNotDisposed();
        nativeSetBatchSize(getNativePtr(), batchSize);
    }
    
    public int getBatchSize() {
        checkNotDisposed();
        return nativeGetBatchSize(getNativePtr());
    }
    
    public void setFrameSkipEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetFrameSkipEnabled(getNativePtr(), enabled);
    }
    
    public boolean isFrameSkipEnabled() {
        checkNotDisposed();
        return nativeIsFrameSkipEnabled(getNativePtr());
    }
    
    public RenderStats getStats() {
        checkNotDisposed();
        return nativeGetStats(getNativePtr());
    }
    
    public void resetStats() {
        checkNotDisposed();
        nativeResetStats(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native void nativeSetMaxFps(long ptr, int fps);
    private native int nativeGetMaxFps(long ptr);
    private native void nativeSetMinFps(long ptr, int fps);
    private native int nativeGetMinFps(long ptr);
    private native void nativeEnableDirtyRectOptimization(long ptr, boolean enabled);
    private native boolean nativeIsDirtyRectOptimizationEnabled(long ptr);
    private native void nativeMarkDirty(long ptr, double minX, double minY, double maxX, double maxY);
    private native void nativeMarkAllDirty(long ptr);
    private native void nativeClearDirty(long ptr);
    private native boolean nativeIsDirty(long ptr);
    private native void nativeEnableBatchRendering(long ptr, boolean enabled);
    private native boolean nativeIsBatchRenderingEnabled(long ptr);
    private native void nativeSetBatchSize(long ptr, int size);
    private native int nativeGetBatchSize(long ptr);
    private native void nativeSetFrameSkipEnabled(long ptr, boolean enabled);
    private native boolean nativeIsFrameSkipEnabled(long ptr);
    private native RenderStats nativeGetStats(long ptr);
    private native void nativeResetStats(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.18.2 RenderStats.java

```java
package cn.cycle.chart.api.optimize;

public class RenderStats {
    
    private final long frameCount;
    private final long totalRenderTime;
    private final long averageRenderTime;
    private final long maxRenderTime;
    private final long minRenderTime;
    private final int currentFps;
    private final int averageFps;
    private final long skippedFrames;
    private final long batchCount;
    private final long drawCallCount;
    
    public RenderStats(long frameCount, long totalRenderTime, long averageRenderTime,
                       long maxRenderTime, long minRenderTime, int currentFps,
                       int averageFps, long skippedFrames, long batchCount, 
                       long drawCallCount) {
        this.frameCount = frameCount;
        this.totalRenderTime = totalRenderTime;
        this.averageRenderTime = averageRenderTime;
        this.maxRenderTime = maxRenderTime;
        this.minRenderTime = minRenderTime;
        this.currentFps = currentFps;
        this.averageFps = averageFps;
        this.skippedFrames = skippedFrames;
        this.batchCount = batchCount;
        this.drawCallCount = drawCallCount;
    }
    
    public long getFrameCount() { return frameCount; }
    public long getTotalRenderTime() { return totalRenderTime; }
    public long getAverageRenderTime() { return averageRenderTime; }
    public long getMaxRenderTime() { return maxRenderTime; }
    public long getMinRenderTime() { return minRenderTime; }
    public int getCurrentFps() { return currentFps; }
    public int getAverageFps() { return averageFps; }
    public long getSkippedFrames() { return skippedFrames; }
    public long getBatchCount() { return batchCount; }
    public long getDrawCallCount() { return drawCallCount; }
    
    @Override
    public String toString() {
        return String.format(
            "RenderStats{fps=%d, avgTime=%dms, frames=%d, batches=%d, drawCalls=%d}",
            currentFps, averageRenderTime, frameCount, batchCount, drawCallCount);
    }
}
```

### 6.18.3 TileRenderPriority.java

```java
package cn.cycle.chart.api.optimize;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.cache.TileKey;
import cn.cycle.chart.api.core.Viewport;

public class TileRenderPriority extends NativeObject {
    
    public enum Priority {
        CRITICAL, HIGH, MEDIUM, LOW
    }
    
    public TileRenderPriority() {
        setNativePtr(nativeCreate());
    }
    
    public Priority calculatePriority(TileKey tile, Viewport viewport) {
        checkNotDisposed();
        int priority = nativeCalculatePriority(getNativePtr(), tile.getX(), tile.getY(),
                                                tile.getLevel(), viewport.getCenterX(),
                                                viewport.getCenterY(), viewport.getScale());
        return Priority.values()[priority];
    }
    
    public void setCriticalDistance(double distance) {
        checkNotDisposed();
        nativeSetCriticalDistance(getNativePtr(), distance);
    }
    
    public double getCriticalDistance() {
        checkNotDisposed();
        return nativeGetCriticalDistance(getNativePtr());
    }
    
    public void setHighDistance(double distance) {
        checkNotDisposed();
        nativeSetHighDistance(getNativePtr(), distance);
    }
    
    public double getHighDistance() {
        checkNotDisposed();
        return nativeGetHighDistance(getNativePtr());
    }
    
    public void setMediumDistance(double distance) {
        checkNotDisposed();
        nativeSetMediumDistance(getNativePtr(), distance);
    }
    
    public double getMediumDistance() {
        checkNotDisposed();
        return nativeGetMediumDistance(getNativePtr());
    }
    
    public void setPreloadEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetPreloadEnabled(getNativePtr(), enabled);
    }
    
    public boolean isPreloadEnabled() {
        checkNotDisposed();
        return nativeIsPreloadEnabled(getNativePtr());
    }
    
    public void setPreloadDistance(double distance) {
        checkNotDisposed();
        nativeSetPreloadDistance(getNativePtr(), distance);
    }
    
    public double getPreloadDistance() {
        checkNotDisposed();
        return nativeGetPreloadDistance(getNativePtr());
    }
    
    // Native方法
    private native long nativeCreate();
    private native int nativeCalculatePriority(long ptr, int tileX, int tileY, int level,
                                                double centerX, double centerY, double scale);
    private native void nativeSetCriticalDistance(long ptr, double distance);
    private native double nativeGetCriticalDistance(long ptr);
    private native void nativeSetHighDistance(long ptr, double distance);
    private native double nativeGetHighDistance(long ptr);
    private native void nativeSetMediumDistance(long ptr, double distance);
    private native double nativeGetMediumDistance(long ptr);
    private native void nativeSetPreloadEnabled(long ptr, boolean enabled);
    private native boolean nativeIsPreloadEnabled(long ptr);
    private native void nativeSetPreloadDistance(long ptr, double distance);
    private native double nativeGetPreloadDistance(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.19 JavaFX适配层接口 (javafx)

### 6.19.1 ChartCanvas.java

```java
package cn.cycle.chart.adapter.canvas;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.animation.AnimationTimer;
import javafx.beans.property.*;
import javafx.scene.input.*;
import javafx.application.Platform;

public class ChartCanvas extends Canvas {
    
    private final ChartViewer viewer;
    private final AnimationTimer renderTimer;
    private final JavaFXEventHandler eventHandler;
    
    private final DoubleProperty centerX = new SimpleDoubleProperty();
    private final DoubleProperty centerY = new SimpleDoubleProperty();
    private final DoubleProperty zoomLevel = new SimpleDoubleProperty(1.0);
    private final DoubleProperty rotation = new SimpleDoubleProperty(0.0);
    private final BooleanProperty renderEnabled = new SimpleBooleanProperty(true);
    
    private volatile boolean dirty = true;
    private long lastRenderTime = 0;
    private static final long MIN_FRAME_INTERVAL_NS = 16_000_000;
    private static final long IDLE_FRAME_INTERVAL_NS = 100_000_000;
    
    public ChartCanvas(ChartViewer viewer) {
        this.viewer = viewer;
        this.eventHandler = new JavaFXEventHandler(viewer);
        
        renderTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (!renderEnabled.get()) return;
                
                long elapsed = now - lastRenderTime;
                long targetInterval = dirty ? MIN_FRAME_INTERVAL_NS : IDLE_FRAME_INTERVAL_NS;
                
                if (elapsed >= targetInterval) {
                    render();
                    lastRenderTime = now;
                }
            }
        };
        
        widthProperty().addListener(obs -> requestRender());
        heightProperty().addListener(obs -> requestRender());
        
        setupEventHandlers();
        setupTouchHandlers();
        
        renderTimer.start();
    }
    
    public ChartCanvas(ChartViewer viewer, double width, double height) {
        this(viewer);
        setWidth(width);
        setHeight(height);
    }
    
    private void render() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, getWidth(), getHeight());
        viewer.render(gc, getWidth(), getHeight());
        dirty = false;
    }
    
    public void requestRender() {
        dirty = true;
    }
    
    private void setupEventHandlers() {
        setOnMousePressed(eventHandler::onMousePressed);
        setOnMouseReleased(eventHandler::onMouseReleased);
        setOnMouseDragged(eventHandler::onMouseDragged);
        setOnMouseClicked(eventHandler::onMouseClicked);
        setOnScroll(eventHandler::onScroll);
        setOnKeyPressed(eventHandler::onKeyPressed);
        setOnKeyReleased(eventHandler::onKeyReleased);
    }
    
    private void setupTouchHandlers() {
        setOnTouchPressed(event -> {
            eventHandler.onTouchPressed(event.getTouchPoint());
        });
        setOnTouchMoved(event -> {
            eventHandler.onTouchMoved(event.getTouchPoint());
        });
        setOnTouchReleased(event -> {
            eventHandler.onTouchReleased(event.getTouchPoint());
        });
        setOnZoom(event -> {
            eventHandler.onZoom(event.getZoomFactor(), 
                               event.getCenterX(), event.getCenterY());
        });
        setOnRotate(event -> {
            eventHandler.onRotate(event.getAngle());
        });
    }
    
    public DoubleProperty centerXProperty() { return centerX; }
    public DoubleProperty centerYProperty() { return centerY; }
    public DoubleProperty zoomLevelProperty() { return zoomLevel; }
    public DoubleProperty rotationProperty() { return rotation; }
    public BooleanProperty renderEnabledProperty() { return renderEnabled; }
    
    public double getCenterX() { return centerX.get(); }
    public double getCenterY() { return centerY.get(); }
    public double getZoomLevel() { return zoomLevel.get(); }
    public double getRotation() { return rotation.get(); }
    public boolean isRenderEnabled() { return renderEnabled.get(); }
    
    public void setCenterX(double value) { centerX.set(value); }
    public void setCenterY(double value) { centerY.set(value); }
    public void setZoomLevel(double value) { zoomLevel.set(value); }
    public void setRotation(double value) { rotation.set(value); }
    public void setRenderEnabled(boolean value) { renderEnabled.set(value); }
    
    public void dispose() {
        renderTimer.stop();
    }
}
```

### 6.19.2 JavaFXEventHandler.java

```java
package cn.cycle.chart.adapter.event;

import javafx.scene.input.*;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;

public class JavaFXEventHandler {
    
    private final ChartViewer viewer;
    private double lastMouseX;
    private double lastMouseY;
    private boolean isDragging = false;
    
    public JavaFXEventHandler(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    public void onMousePressed(MouseEvent event) {
        lastMouseX = event.getX();
        lastMouseY = event.getY();
        isDragging = false;
    }
    
    public void onMouseReleased(MouseEvent event) {
        if (!isDragging) {
            handleClick(event.getX(), event.getY(), event.getClickCount());
        }
        isDragging = false;
    }
    
    public void onMouseDragged(MouseEvent event) {
        double dx = event.getX() - lastMouseX;
        double dy = event.getY() - lastMouseY;
        
        if (Math.abs(dx) > 2 || Math.abs(dy) > 2) {
            isDragging = true;
            viewer.pan(-dx, -dy);
        }
        
        lastMouseX = event.getX();
        lastMouseY = event.getY();
    }
    
    public void onMouseClicked(MouseEvent event) {
        // 点击处理在onMouseRelease中完成
    }
    
    public void onScroll(ScrollEvent event) {
        double zoomFactor = event.getDeltaY() > 0 ? 1.1 : 0.9;
        viewer.zoom(zoomFactor, event.getX(), event.getY());
    }
    
    public void onKeyPressed(KeyEvent event) {
        switch (event.getCode()) {
            case LEFT:
                viewer.pan(-50, 0);
                break;
            case RIGHT:
                viewer.pan(50, 0);
                break;
            case UP:
                viewer.pan(0, -50);
                break;
            case DOWN:
                viewer.pan(0, 50);
                break;
            case PLUS:
            case EQUALS:
                viewer.zoom(1.2, viewer.getViewport().getCenterX(), 
                           viewer.getViewport().getCenterY());
                break;
            case MINUS:
                viewer.zoom(0.8, viewer.getViewport().getCenterX(), 
                           viewer.getViewport().getCenterY());
                break;
        }
    }
    
    public void onKeyReleased(KeyEvent event) {
        // 按键释放处理
    }
    
    public void onTouchPressed(TouchPoint touchPoint) {
        lastMouseX = touchPoint.getX();
        lastMouseY = touchPoint.getY();
    }
    
    public void onTouchMoved(TouchPoint touchPoint) {
        double dx = touchPoint.getX() - lastMouseX;
        double dy = touchPoint.getY() - lastMouseY;
        viewer.pan(-dx, -dy);
        lastMouseX = touchPoint.getX();
        lastMouseY = touchPoint.getY();
    }
    
    public void onTouchReleased(TouchPoint touchPoint) {
        handleClick(touchPoint.getX(), touchPoint.getY(), 1);
    }
    
    public void onZoom(double zoomFactor, double centerX, double centerY) {
        viewer.zoom(zoomFactor, centerX, centerY);
    }
    
    public void onRotate(double angle) {
        viewer.rotate(angle);
    }
    
    private void handleClick(double x, double y, int clickCount) {
        if (clickCount == 1) {
            FeatureInfo feature = viewer.queryFeature(x, y);
            if (feature != null) {
                // 触发要素选择事件
                viewer.selectFeature(feature);
            }
        } else if (clickCount == 2) {
            // 双击放大
            viewer.zoom(2.0, x, y);
        }
    }
}
```

### 6.19.3 JavaFXImageDevice.java

```java
package cn.cycle.chart.adapter.canvas;

import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.image.Image;
import cn.cycle.chart.api.util.Color;

public class JavaFXImageDevice {
    
    private final GraphicsContext gc;
    private double currentX;
    private double currentY;
    
    public JavaFXImageDevice(GraphicsContext gc) {
        this.gc = gc;
    }
    
    public void drawLine(double x1, double y1, double x2, double y2) {
        gc.strokeLine(x1, y1, x2, y2);
    }
    
    public void drawPolyline(double[] xPoints, double[] yPoints, int nPoints) {
        gc.beginPath();
        gc.moveTo(xPoints[0], yPoints[0]);
        for (int i = 1; i < nPoints; i++) {
            gc.lineTo(xPoints[i], yPoints[i]);
        }
        gc.stroke();
    }
    
    public void drawPolygon(double[] xPoints, double[] yPoints, int nPoints) {
        gc.beginPath();
        gc.moveTo(xPoints[0], yPoints[0]);
        for (int i = 1; i < nPoints; i++) {
            gc.lineTo(xPoints[i], yPoints[i]);
        }
        gc.closePath();
        gc.stroke();
    }
    
    public void fillPolygon(double[] xPoints, double[] yPoints, int nPoints) {
        gc.beginPath();
        gc.moveTo(xPoints[0], yPoints[0]);
        for (int i = 1; i < nPoints; i++) {
            gc.lineTo(xPoints[i], yPoints[i]);
        }
        gc.closePath();
        gc.fill();
    }
    
    public void drawRect(double x, double y, double width, double height) {
        gc.strokeRect(x, y, width, height);
    }
    
    public void fillRect(double x, double y, double width, double height) {
        gc.fillRect(x, y, width, height);
    }
    
    public void drawOval(double x, double y, double width, double height) {
        gc.strokeOval(x, y, width, height);
    }
    
    public void fillOval(double x, double y, double width, double height) {
        gc.fillOval(x, y, width, height);
    }
    
    public void drawArc(double x, double y, double width, double height, 
                        double startAngle, double arcAngle) {
        gc.strokeArc(x, y, width, height, startAngle, arcAngle, 
                     javafx.scene.shape.ArcType.OPEN);
    }
    
    public void drawString(String text, double x, double y) {
        gc.fillText(text, x, y);
    }
    
    public void drawImage(Image image, double x, double y) {
        gc.drawImage(image, x, y);
    }
    
    public void drawImage(Image image, double x, double y, double w, double h) {
        gc.drawImage(image, x, y, w, h);
    }
    
    public void setColor(Color color) {
        javafx.scene.paint.Color fxColor = javafx.scene.paint.Color.rgb(
            color.getRed(), color.getGreen(), color.getBlue(), 
            color.getAlpha() / 255.0
        );
        gc.setStroke(fxColor);
        gc.setFill(fxColor);
    }
    
    public void setStrokeWidth(double width) {
        gc.setLineWidth(width);
    }
    
    public void setFont(String family, double size) {
        gc.setFont(Font.font(family, size));
    }
    
    public void setClip(double x, double y, double width, double height) {
        gc.beginPath();
        gc.rect(x, y, width, height);
        gc.clip();
    }
    
    public void clearClip() {
        gc.restore();
        gc.save();
    }
    
    public void translate(double tx, double ty) {
        gc.translate(tx, ty);
    }
    
    public void rotate(double angle) {
        gc.rotate(Math.toDegrees(angle));
    }
    
    public void scale(double sx, double sy) {
        gc.scale(sx, sy);
    }
    
    public void save() {
        gc.save();
    }
    
    public void restore() {
        gc.restore();
    }
    
    public void moveTo(double x, double y) {
        currentX = x;
        currentY = y;
        gc.beginPath();
        gc.moveTo(x, y);
    }
    
    public void lineTo(double x, double y) {
        currentX = x;
        currentY = y;
        gc.lineTo(x, y);
    }
    
    public void curveTo(double x1, double y1, double x2, double y2, double x3, double y3) {
        gc.bezierCurveTo(x1, y1, x2, y2, x3, y3);
    }
    
    public void closePath() {
        gc.closePath();
    }
    
    public void stroke() {
        gc.stroke();
    }
    
    public void fill() {
        gc.fill();
    }
}
```

### 6.19.4 JavaFXThreadBridge.java

```java
package cn.cycle.chart.adapter.binding;

import javafx.application.Platform;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.CompletableFuture;
import java.util.function.Supplier;

public class JavaFXThreadBridge {
    
    private final ExecutorService backgroundExecutor;
    
    public JavaFXThreadBridge() {
        this.backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors()
        );
    }
    
    public JavaFXThreadBridge(int threadCount) {
        this.backgroundExecutor = Executors.newFixedThreadPool(threadCount);
    }
    
    public void runOnFxThread(Runnable action) {
        if (Platform.isFxApplicationThread()) {
            action.run();
        } else {
            Platform.runLater(action);
        }
    }
    
    public <T> CompletableFuture<T> runOnBackgroundThread(Supplier<T> supplier) {
        return CompletableFuture.supplyAsync(supplier, backgroundExecutor);
    }
    
    public CompletableFuture<Void> runOnBackgroundThread(Runnable runnable) {
        return CompletableFuture.runAsync(runnable, backgroundExecutor);
    }
    
    public <T> CompletableFuture<T> runOnBackgroundThenFx(Supplier<T> backgroundTask) {
        return CompletableFuture.supplyAsync(backgroundTask, backgroundExecutor)
            .thenApplyAsync(result -> {
                return result;
            }, Platform::runLater);
    }
    
    public void executeWithProgress(Runnable backgroundTask, 
                                     Runnable onProgress,
                                     Runnable onComplete) {
        runOnBackgroundThread(() -> {
            backgroundTask.run();
            runOnFxThread(onComplete);
        });
    }
    
    public boolean isFxThread() {
        return Platform.isFxApplicationThread();
    }
    
    public void shutdown() {
        backgroundExecutor.shutdown();
    }
}
```

### 6.19.5 PropertyBinder.java

```java
package cn.cycle.chart.adapter.binding;

import javafx.beans.property.*;
import javafx.beans.value.ChangeListener;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import java.util.ArrayList;
import java.util.List;

public class PropertyBinder {
    
    private final ChartViewer viewer;
    private final List<ChangeListener<?>> listeners = new ArrayList<>();
    
    public PropertyBinder(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    public void bindViewport(DoubleProperty centerX, DoubleProperty centerY, 
                             DoubleProperty scale) {
        ChangeListener<Number> centerListener = (obs, oldVal, newVal) -> {
            Viewport viewport = viewer.getViewport();
            viewport.setCenter(centerX.get(), centerY.get());
            viewport.setScale(scale.get());
            viewer.setViewport(viewport);
        };
        
        centerX.addListener(centerListener);
        centerY.addListener(centerListener);
        scale.addListener(centerListener);
        
        listeners.add(centerListener);
    }
    
    public void bindZoom(DoubleProperty zoomLevel, double minZoom, double maxZoom) {
        ChangeListener<Number> zoomListener = (obs, oldVal, newVal) -> {
            double zoom = Math.max(minZoom, Math.min(maxZoom, newVal.doubleValue()));
            viewer.zoom(zoom / oldVal.doubleValue(), 
                       viewer.getViewport().getCenterX(),
                       viewer.getViewport().getCenterY());
        };
        
        zoomLevel.addListener(zoomListener);
        listeners.add(zoomListener);
    }
    
    public void bindRotation(DoubleProperty rotation) {
        ChangeListener<Number> rotationListener = (obs, oldVal, newVal) -> {
            viewer.rotate(newVal.doubleValue() - oldVal.doubleValue());
        };
        
        rotation.addListener(rotationListener);
        listeners.add(rotationListener);
    }
    
    public void bindLayerVisibility(BooleanProperty visible, String layerName) {
        ChangeListener<Boolean> visibilityListener = (obs, oldVal, newVal) -> {
            ChartLayer layer = viewer.getLayerManager().getLayerByName(layerName);
            if (layer != null) {
                layer.setVisible(newVal);
            }
        };
        
        visible.addListener(visibilityListener);
        listeners.add(visibilityListener);
    }
    
    public void bindEnabled(BooleanProperty enabled, Runnable onEnable, Runnable onDisable) {
        ChangeListener<Boolean> enabledListener = (obs, oldVal, newVal) -> {
            if (newVal) {
                onEnable.run();
            } else {
                onDisable.run();
            }
        };
        
        enabled.addListener(enabledListener);
        listeners.add(enabledListener);
    }
    
    public void unbindAll() {
        listeners.clear();
    }
    
    public void syncFromViewer(DoubleProperty centerX, DoubleProperty centerY, 
                               DoubleProperty scale) {
        Viewport viewport = viewer.getViewport();
        centerX.set(viewport.getCenterX());
        centerY.set(viewport.getCenterY());
        scale.set(viewport.getScale());
    }
}
```

---

## 6.20 生命周期管理接口 (lifecycle)

### 6.20.1 AppLifecycleManager.java

```java
package cn.cycle.chart.api.lifecycle;

import javafx.application.Platform;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

public class AppLifecycleManager {
    
    public enum AppState {
        CREATED,
        READY,
        LOADED,
        PAUSED,
        DISPOSED
    }
    
    private final AtomicReference<AppState> state = new AtomicReference<>(AppState.CREATED);
    private final ChartViewer viewer;
    private final ChartCanvas canvas;
    private ExecutorService backgroundExecutor;
    
    public AppLifecycleManager(ChartViewer viewer, ChartCanvas canvas) {
        this.viewer = viewer;
        this.canvas = canvas;
    }
    
    public void initialize() {
        if (!state.compareAndSet(AppState.CREATED, AppState.READY)) {
            throw new IllegalStateException("Invalid state transition");
        }
        
        backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors());
        
        viewer.setConfig(buildDefaultConfig());
        canvas.initialize();
    }
    
    public void loadChart(String path) {
        AppState current = state.get();
        if (current != AppState.READY && current != AppState.LOADED) {
            throw new IllegalStateException("Invalid state for loading: " + current);
        }
        
        backgroundExecutor.submit(() -> {
            try {
                viewer.loadChart(path);
                state.set(AppState.LOADED);
                Platform.runLater(() -> canvas.requestRender());
            } catch (Exception e) {
                Platform.runLater(() -> showError("Load failed", e));
            }
        });
    }
    
    public void pause() {
        if (state.compareAndSet(AppState.LOADED, AppState.PAUSED)) {
            canvas.pauseRendering();
            viewer.flushCache();
        }
    }
    
    public void resume() {
        if (state.compareAndSet(AppState.PAUSED, AppState.LOADED)) {
            canvas.resumeRendering();
        }
    }
    
    public void dispose() {
        AppState current = state.getAndSet(AppState.DISPOSED);
        if (current == AppState.DISPOSED) {
            return;
        }
        
        try {
            backgroundExecutor.shutdown();
            if (!backgroundExecutor.awaitTermination(5, TimeUnit.SECONDS)) {
                backgroundExecutor.shutdownNow();
            }
            
            canvas.dispose();
            viewer.close();
        } catch (Exception e) {
            LOG.error("Error during dispose", e);
        }
    }
    
    public AppState getState() {
        return state.get();
    }
}
```

### 6.20.2 ThreadSafeRenderState.java

```java
package cn.cycle.chart.api.lifecycle;

import javafx.application.Platform;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

public class ThreadSafeRenderState {
    
    private final AtomicReference<RenderState> state = new AtomicReference<>();
    private final CountDownLatch renderLatch = new CountDownLatch(1);
    
    public void updateState(RenderState newState) {
        state.set(newState);
        Platform.runLater(() -> {
            notifyStateChanged();
            renderLatch.countDown();
        });
    }
    
    public RenderState getState() {
        return state.get();
    }
    
    public boolean awaitRenderComplete(long timeout, TimeUnit unit) {
        try {
            return renderLatch.await(timeout, unit);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            return false;
        }
    }
    
    protected void notifyStateChanged() {
    }
}
```

---

## 6.21 错误恢复与容错接口 (recovery)

### 6.21.1 ErrorRecoveryManager.java

```java
package cn.cycle.chart.api.recovery;

import javafx.application.Platform;
import javafx.scene.control.Alert;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.atomic.AtomicInteger;

public class ErrorRecoveryManager {
    
    private final Map<Class<? extends Throwable>, RecoveryStrategy> strategies = new HashMap<>();
    private final CircuitBreaker circuitBreaker;
    
    public ErrorRecoveryManager() {
        this.circuitBreaker = new CircuitBreaker(5, Duration.ofMinutes(1));
        registerDefaultStrategies();
    }
    
    private void registerDefaultStrategies() {
        registerStrategy(JniException.class, this::recoverFromJniError);
        registerStrategy(OutOfMemoryError.class, this::recoverFromMemoryError);
        registerStrategy(IOException.class, this::recoverFromIOError);
        registerStrategy(RenderException.class, this::recoverFromRenderError);
    }
    
    public <T> T executeWithRecovery(Callable<T> operation, 
                                      Class<? extends Throwable>... recoverableErrors) {
        if (circuitBreaker.isOpen()) {
            throw new ServiceUnavailableException("Circuit breaker is open");
        }
        
        try {
            T result = operation.call();
            circuitBreaker.recordSuccess();
            return result;
        } catch (Throwable e) {
            circuitBreaker.recordFailure();
            
            for (Class<? extends Throwable> errorType : recoverableErrors) {
                if (errorType.isInstance(e)) {
                    RecoveryStrategy strategy = strategies.get(errorType);
                    if (strategy != null) {
                        return strategy.recover(e, operation);
                    }
                }
            }
            
            throw new RuntimeException("Unrecoverable error", e);
        }
    }
    
    private <T> T recoverFromJniError(Throwable error, Callable<T> operation) {
        LOG.warn("JNI error occurred, attempting recovery: {}", error.getMessage());
        
        if (getRetryCount() < MAX_RETRIES) {
            incrementRetryCount();
            try {
                Thread.sleep(RETRY_DELAY_MS);
                return operation.call();
            } catch (Exception e) {
                LOG.error("Retry failed", e);
            }
        }
        
        JniEnvManager.getInstance().resetEnvironment();
        return getFallbackValue();
    }
    
    private <T> T recoverFromMemoryError(Throwable error, Callable<T> operation) {
        LOG.error("Memory error occurred, performing emergency cleanup");
        
        CacheManager.getInstance().clearAll();
        System.gc();
        
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("内存不足");
            alert.setContentText("应用内存不足，已清理部分缓存。");
            alert.show();
        });
        
        throw new MemoryRecoveryException("Memory recovered, please retry operation");
    }
    
    private <T> T recoverFromRenderError(Throwable error, Callable<T> operation) {
        LOG.warn("Render error occurred, using fallback rendering");
        
        RenderContext context = getCurrentContext();
        context.markErrorArea(error);
        context.setSkipErrors(true);
        
        return null;
    }
    
    public void registerStrategy(Class<? extends Throwable> errorType, RecoveryStrategy strategy) {
        strategies.put(errorType, strategy);
    }
}
```

### 6.21.2 CircuitBreaker.java

```java
package cn.cycle.chart.api.recovery;

import java.time.Duration;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;

public class CircuitBreaker {
    
    public enum State {
        CLOSED,
        OPEN,
        HALF_OPEN
    }
    
    private final int failureThreshold;
    private final Duration resetTimeout;
    
    private final AtomicInteger failureCount = new AtomicInteger(0);
    private final AtomicLong lastFailureTime = new AtomicLong(0);
    private final AtomicReference<State> state = new AtomicReference<>(State.CLOSED);
    
    public CircuitBreaker(int failureThreshold, Duration resetTimeout) {
        this.failureThreshold = failureThreshold;
        this.resetTimeout = resetTimeout;
    }
    
    public boolean isOpen() {
        if (state.get() == State.OPEN) {
            if (System.currentTimeMillis() - lastFailureTime.get() > resetTimeout.toMillis()) {
                state.compareAndSet(State.OPEN, State.HALF_OPEN);
                return false;
            }
            return true;
        }
        return false;
    }
    
    public void recordSuccess() {
        failureCount.set(0);
        state.set(State.CLOSED);
    }
    
    public void recordFailure() {
        failureCount.incrementAndGet();
        lastFailureTime.set(System.currentTimeMillis());
        
        if (failureCount.get() >= failureThreshold) {
            state.set(State.OPEN);
        }
    }
    
    public State getState() {
        return state.get();
    }
}
```

### 6.21.3 GracefulDegradation.java

```java
package cn.cycle.chart.api.recovery;

import java.util.concurrent.atomic.AtomicReference;

public class GracefulDegradation {
    
    public enum DegradationLevel {
        FULL,
        REDUCED_CACHE,
        LOW_QUALITY,
        MINIMAL
    }
    
    private final AtomicReference<DegradationLevel> currentLevel = 
        new AtomicReference<>(DegradationLevel.FULL);
    
    private final CacheManager cacheManager;
    private final RenderConfig renderConfig;
    private final LayerManager layerManager;
    
    public GracefulDegradation(CacheManager cacheManager, 
                               RenderConfig renderConfig,
                               LayerManager layerManager) {
        this.cacheManager = cacheManager;
        this.renderConfig = renderConfig;
        this.layerManager = layerManager;
    }
    
    public void adjustBasedOnPerformance(PerformanceMetrics metrics) {
        if (metrics.getFps() < 15) {
            degradeTo(DegradationLevel.MINIMAL);
        } else if (metrics.getFps() < 20) {
            degradeTo(DegradationLevel.LOW_QUALITY);
        } else if (metrics.getMemoryUsage() > 0.8) {
            degradeTo(DegradationLevel.REDUCED_CACHE);
        } else {
            upgradeTo(DegradationLevel.FULL);
        }
    }
    
    private void degradeTo(DegradationLevel level) {
        if (level.ordinal() > currentLevel.get().ordinal()) {
            currentLevel.set(level);
            applyDegradation(level);
        }
    }
    
    private void upgradeTo(DegradationLevel level) {
        if (level.ordinal() < currentLevel.get().ordinal()) {
            currentLevel.set(level);
            applyUpgrade(level);
        }
    }
    
    private void applyDegradation(DegradationLevel level) {
        switch (level) {
            case REDUCED_CACHE:
                cacheManager.setMaxSize(cacheManager.getMaxSize() / 2);
                break;
            case LOW_QUALITY:
                renderConfig.setAntiAliasing(false);
                renderConfig.setDetailLevel(DetailLevel.LOW);
                break;
            case MINIMAL:
                cacheManager.setMaxSize(cacheManager.getMaxSize() / 4);
                renderConfig.setAntiAliasing(false);
                renderConfig.setDetailLevel(DetailLevel.MINIMAL);
                layerManager.setVisibleLayers(essentialLayersOnly);
                break;
        }
    }
    
    private void applyUpgrade(DegradationLevel level) {
        switch (level) {
            case FULL:
                cacheManager.restoreMaxSize();
                renderConfig.setAntiAliasing(true);
                renderConfig.setDetailLevel(DetailLevel.FULL);
                layerManager.restoreAllLayers();
                break;
        }
    }
    
    public DegradationLevel getCurrentLevel() {
        return currentLevel.get();
    }
}
```

---

## 6.22 性能监控接口 (monitor)

### 6.22.1 PerformanceMonitor.java

```java
package cn.cycle.chart.api.monitor;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class PerformanceMonitor {
    
    private final MetricsCollector collector = new MetricsCollector();
    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    
    public void start() {
        scheduler.scheduleAtFixedRate(this::collectMetrics, 0, 1, TimeUnit.SECONDS);
    }
    
    public void stop() {
        scheduler.shutdown();
    }
    
    private void collectMetrics() {
        collector.record("fps", calculateFPS());
        collector.record("java_heap", getJavaHeapUsed());
        collector.record("native_memory", getNativeMemoryUsed());
        collector.record("jni_calls", getJNICallCount());
        collector.record("cache_hits", getCacheHitRate());
        collector.record("render_time", getLastRenderTime());
    }
    
    public PerformanceReport generateReport() {
        return PerformanceReport.builder()
            .avgFps(collector.getAverage("fps"))
            .maxHeapUsed(collector.getMax("java_heap"))
            .maxNativeUsed(collector.getMax("native_memory"))
            .totalJNICalls(collector.getSum("jni_calls"))
            .avgCacheHitRate(collector.getAverage("cache_hits"))
            .avgRenderTime(collector.getAverage("render_time"))
            .build();
    }
    
    private double calculateFPS() {
        return fpsCalculator.getCurrentFPS();
    }
    
    private long getJavaHeapUsed() {
        Runtime runtime = Runtime.getRuntime();
        return runtime.totalMemory() - runtime.freeMemory();
    }
    
    private long getNativeMemoryUsed() {
        return NativeMemoryMonitor.getNativeMemoryUsed();
    }
}
```

### 6.22.2 MetricsCollector.java

```java
package cn.cycle.chart.api.monitor;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.DoubleAdder;

public class MetricsCollector {
    
    private final Map<String, Metric> metrics = new ConcurrentHashMap<>();
    
    public void record(String name, double value) {
        metrics.computeIfAbsent(name, k -> new Metric()).record(value);
    }
    
    public double getAverage(String name) {
        Metric metric = metrics.get(name);
        return metric != null ? metric.getAverage() : 0.0;
    }
    
    public double getMax(String name) {
        Metric metric = metrics.get(name);
        return metric != null ? metric.getMax() : 0.0;
    }
    
    public double getMin(String name) {
        Metric metric = metrics.get(name);
        return metric != null ? metric.getMin() : 0.0;
    }
    
    public long getSum(String name) {
        Metric metric = metrics.get(name);
        return metric != null ? metric.getSum() : 0;
    }
    
    public void reset() {
        metrics.clear();
    }
    
    private static class Metric {
        private final DoubleAdder sum = new DoubleAdder();
        private final AtomicLong count = new AtomicLong(0);
        private volatile double min = Double.MAX_VALUE;
        private volatile double max = Double.MIN_VALUE;
        
        void record(double value) {
            sum.add(value);
            count.incrementAndGet();
            
            synchronized (this) {
                if (value < min) min = value;
                if (value > max) max = value;
            }
        }
        
        double getAverage() {
            long c = count.get();
            return c > 0 ? sum.sum() / c : 0.0;
        }
        
        double getMax() { return max; }
        double getMin() { return min; }
        long getSum() { return (long) sum.sum(); }
    }
}
```

### 6.22.3 PerformanceReport.java

```java
package cn.cycle.chart.api.monitor;

public class PerformanceReport {
    
    private final double avgFps;
    private final long maxHeapUsed;
    private final long maxNativeUsed;
    private final long totalJniCalls;
    private final double avgCacheHitRate;
    private final double avgRenderTime;
    
    private PerformanceReport(Builder builder) {
        this.avgFps = builder.avgFps;
        this.maxHeapUsed = builder.maxHeapUsed;
        this.maxNativeUsed = builder.maxNativeUsed;
        this.totalJniCalls = builder.totalJniCalls;
        this.avgCacheHitRate = builder.avgCacheHitRate;
        this.avgRenderTime = builder.avgRenderTime;
    }
    
    public static Builder builder() {
        return new Builder();
    }
    
    public double getAvgFps() { return avgFps; }
    public long getMaxHeapUsed() { return maxHeapUsed; }
    public long getMaxNativeUsed() { return maxNativeUsed; }
    public long getTotalJniCalls() { return totalJniCalls; }
    public double getAvgCacheHitRate() { return avgCacheHitRate; }
    public double getAvgRenderTime() { return avgRenderTime; }
    
    public static class Builder {
        private double avgFps;
        private long maxHeapUsed;
        private long maxNativeUsed;
        private long totalJniCalls;
        private double avgCacheHitRate;
        private double avgRenderTime;
        
        public Builder avgFps(double avgFps) { this.avgFps = avgFps; return this; }
        public Builder maxHeapUsed(long maxHeapUsed) { this.maxHeapUsed = maxHeapUsed; return this; }
        public Builder maxNativeUsed(long maxNativeUsed) { this.maxNativeUsed = maxNativeUsed; return this; }
        public Builder totalJniCalls(long totalJniCalls) { this.totalJniCalls = totalJniCalls; return this; }
        public Builder avgCacheHitRate(double avgCacheHitRate) { this.avgCacheHitRate = avgCacheHitRate; return this; }
        public Builder avgRenderTime(double avgRenderTime) { this.avgRenderTime = avgRenderTime; return this; }
        
        public PerformanceReport build() {
            return new PerformanceReport(this);
        }
    }
}
```

---

## 6.23 健康检查与监控接口 (health)

### 6.23.1 HealthCheck.java

```java
package cn.cycle.chart.api.health;

public interface HealthCheck {
    
    HealthStatus check();
    
    String getName();
    
    default String getDetails() {
        return "";
    }
}
```

### 6.23.2 HealthStatus.java

```java
package cn.cycle.chart.api.health;

public enum HealthStatus {
    HEALTHY,
    DEGRADED,
    UNHEALTHY
}
```

### 6.23.3 ChartApplicationHealthCheck.java

```java
package cn.cycle.chart.api.health;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.cache.CacheManager;
import cn.cycle.chart.jni.JniEnvManager;

public class ChartApplicationHealthCheck implements HealthCheck {
    
    private final ChartViewer viewer;
    private final CacheManager cacheManager;
    
    public ChartApplicationHealthCheck(ChartViewer viewer, CacheManager cacheManager) {
        this.viewer = viewer;
        this.cacheManager = cacheManager;
    }
    
    @Override
    public HealthStatus check() {
        HealthStatus status = HealthStatus.HEALTHY;
        
        if (!checkNativeLibraries()) {
            return HealthStatus.UNHEALTHY;
        }
        
        if (!checkMemoryHealth()) {
            status = HealthStatus.DEGRADED;
        }
        
        if (!checkCacheHealth()) {
            status = HealthStatus.DEGRADED;
        }
        
        return status;
    }
    
    @Override
    public String getName() {
        return "ChartApplication";
    }
    
    private boolean checkNativeLibraries() {
        return JniEnvManager.getInstance().IsInitialized();
    }
    
    private boolean checkMemoryHealth() {
        long used = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
        long max = Runtime.getRuntime().maxMemory();
        return (double) used / max < 0.9;
    }
    
    private boolean checkCacheHealth() {
        return cacheManager.getHitRate() > 0.5;
    }
}
```

### 6.23.4 MetricsExporter.java

```java
package cn.cycle.chart.api.health;

public class MetricsExporter {
    
    private final PerformanceMonitor monitor;
    
    public MetricsExporter(PerformanceMonitor monitor) {
        this.monitor = monitor;
    }
    
    public String exportPrometheusFormat() {
        StringBuilder sb = new StringBuilder();
        
        sb.append("# HELP chart_fps Current frames per second\n");
        sb.append("# TYPE chart_fps gauge\n");
        sb.append("chart_fps ").append(getCurrentFPS()).append("\n\n");
        
        sb.append("# HELP chart_memory_heap_bytes Java heap memory usage\n");
        sb.append("# TYPE chart_memory_heap_bytes gauge\n");
        sb.append("chart_memory_heap_bytes ").append(getHeapUsage()).append("\n\n");
        
        sb.append("# HELP chart_memory_native_bytes Native memory usage\n");
        sb.append("# TYPE chart_memory_native_bytes gauge\n");
        sb.append("chart_memory_native_bytes ").append(getNativeUsage()).append("\n\n");
        
        sb.append("# HELP chart_cache_hit_rate Cache hit rate\n");
        sb.append("# TYPE chart_cache_hit_rate gauge\n");
        sb.append("chart_cache_hit_rate ").append(getCacheHitRate()).append("\n");
        
        return sb.toString();
    }
    
    private double getCurrentFPS() {
        return monitor.getCurrentFPS();
    }
    
    private long getHeapUsage() {
        return Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
    }
    
    private long getNativeUsage() {
        return NativeMemoryMonitor.getNativeMemoryUsed();
    }
    
    private double getCacheHitRate() {
        return cacheManager.getHitRate();
    }
}
```

---

## 6.24 库加载器接口 (loader)

### 6.24.1 LibraryLoader.java

```java
package cn.cycle.chart.api.loader;

public class LibraryLoader {
    
    private static final String[] NATIVE_LIBS = {
        "ogc_base", "ogc_geom", "ogc_proj", "ogc_chart_jni"
    };
    
    private static boolean loaded = false;
    
    public static synchronized void loadNativeLibraries() {
        if (loaded) {
            return;
        }
        
        String osName = System.getProperty("os.name").toLowerCase();
        String osArch = System.getProperty("os.arch");
        String libPath = getLibraryPath(osName, osArch);
        
        System.setProperty("java.library.path", libPath);
        
        for (String lib : NATIVE_LIBS) {
            System.loadLibrary(lib);
        }
        
        loaded = true;
    }
    
    private static String getLibraryPath(String osName, String osArch) {
        String basePath = getAppPath();
        if (osName.contains("win")) {
            return basePath + "/lib/windows-" + osArch;
        } else if (osName.contains("linux")) {
            return basePath + "/lib/linux-" + osArch;
        } else if (osName.contains("mac")) {
            return basePath + "/lib/macos-" + osArch;
        }
        throw new UnsupportedOperationException("Unsupported OS: " + osName);
    }
    
    private static String getAppPath() {
        return System.getProperty("app.path", ".");
    }
    
    public static boolean isLoaded() {
        return loaded;
    }
}
```

### 6.24.2 SecureLibraryLoader.java

```java
package cn.cycle.chart.api.loader;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.util.Map;
import java.util.Set;

public class SecureLibraryLoader {
    
    private static final Set<String> ALLOWED_LIBRARIES = Set.of(
        "ogc_base", "ogc_geom", "ogc_proj", "ogc_chart_jni"
    );
    
    private static final Map<String, String> LIBRARY_HASHES = Map.of(
        "ogc_base", "sha256:abc123...",
        "ogc_geom", "sha256:def456...",
        "ogc_chart_jni", "sha256:ghi789..."
    );
    
    public static void loadLibrarySecurely(String libraryName) {
        if (!ALLOWED_LIBRARIES.contains(libraryName)) {
            throw new SecurityException("Library not in allowlist: " + libraryName);
        }
        
        String libraryPath = findLibrary(libraryName);
        
        if (!verifyLibraryHash(libraryPath, LIBRARY_HASHES.get(libraryName))) {
            throw new SecurityException("Library hash verification failed: " + libraryName);
        }
        
        System.load(libraryPath);
    }
    
    private static String findLibrary(String libraryName) {
        String osName = System.getProperty("os.name").toLowerCase();
        String extension = osName.contains("win") ? ".dll" : 
                          osName.contains("mac") ? ".dylib" : ".so";
        
        String libPath = LibraryLoader.getLibraryPath(osName, System.getProperty("os.arch"));
        return libPath + "/" + libraryName + extension;
    }
    
    private static boolean verifyLibraryHash(String path, String expectedHash) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] fileBytes = Files.readAllBytes(Paths.get(path));
            byte[] hash = digest.digest(fileBytes);
            String actualHash = "sha256:" + bytesToHex(hash);
            return expectedHash.equals(actualHash);
        } catch (Exception e) {
            LOG.error("Hash verification failed", e);
            return false;
        }
    }
    
    private static String bytesToHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) {
            sb.append(String.format("%02x", b));
        }
        return sb.toString();
    }
}
```

---

## 6.25 绘图引擎接口 (draw)

### 6.25.1 DrawEngine.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public abstract class DrawEngine extends NativeObject {
    
    public enum EngineType {
        SIMPLE2D,
        GPU_ACCELERATED,
        VECTOR,
        TILE_BASED,
        PDF,
        SVG,
        WEBGL,
        CAIRO,
        DIRECT2D,
        GDIPLUS,
        QT,
        METAL,
        COREGRAPHICS
    }
    
    public enum StateFlag {
        CLIP_VALID(1),
        TRANSFORM_VALID(2),
        STYLE_VALID(4);
        
        private final int value;
        StateFlag(int value) { this.value = value; }
        public int getValue() { return value; }
    }
    
    protected DrawEngine(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public abstract EngineType getEngineType();
    
    public abstract DrawDevice createDevice(int width, int height);
    
    public abstract DrawContext createContext(DrawDevice device);
    
    public void beginDraw(DrawContext context) {
        checkNotDisposed();
        nativeBeginDraw(getNativePtr(), context.getNativePtr());
    }
    
    public void endDraw(DrawContext context) {
        checkNotDisposed();
        nativeEndDraw(getNativePtr(), context.getNativePtr());
    }
    
    public void flush() {
        checkNotDisposed();
        nativeFlush(getNativePtr());
    }
    
    public void setClip(DrawContext context, double x, double y, double width, double height) {
        checkNotDisposed();
        nativeSetClip(getNativePtr(), context.getNativePtr(), x, y, width, height);
    }
    
    public void setTransform(DrawContext context, double[] matrix) {
        checkNotDisposed();
        nativeSetTransform(getNativePtr(), context.getNativePtr(), matrix);
    }
    
    public void drawLine(DrawContext context, double x1, double y1, double x2, double y2) {
        checkNotDisposed();
        nativeDrawLine(getNativePtr(), context.getNativePtr(), x1, y1, x2, y2);
    }
    
    public void drawRect(DrawContext context, double x, double y, double width, double height) {
        checkNotDisposed();
        nativeDrawRect(getNativePtr(), context.getNativePtr(), x, y, width, height);
    }
    
    public void fillRect(DrawContext context, double x, double y, double width, double height) {
        checkNotDisposed();
        nativeFillRect(getNativePtr(), context.getNativePtr(), x, y, width, height);
    }
    
    public void drawCircle(DrawContext context, double cx, double cy, double radius) {
        checkNotDisposed();
        nativeDrawCircle(getNativePtr(), context.getNativePtr(), cx, cy, radius);
    }
    
    public void fillCircle(DrawContext context, double cx, double cy, double radius) {
        checkNotDisposed();
        nativeFillCircle(getNativePtr(), context.getNativePtr(), cx, cy, radius);
    }
    
    public void drawPolygon(DrawContext context, double[] xPoints, double[] yPoints, int count) {
        checkNotDisposed();
        nativeDrawPolygon(getNativePtr(), context.getNativePtr(), xPoints, yPoints, count);
    }
    
    public void fillPolygon(DrawContext context, double[] xPoints, double[] yPoints, int count) {
        checkNotDisposed();
        nativeFillPolygon(getNativePtr(), context.getNativePtr(), xPoints, yPoints, count);
    }
    
    public void drawText(DrawContext context, String text, double x, double y) {
        checkNotDisposed();
        nativeDrawText(getNativePtr(), context.getNativePtr(), text, x, y);
    }
    
    public void drawImage(DrawContext context, Image image, double x, double y) {
        checkNotDisposed();
        nativeDrawImage(getNativePtr(), context.getNativePtr(), image.getNativePtr(), x, y);
    }
    
    protected native void nativeBeginDraw(long ptr, long contextPtr);
    protected native void nativeEndDraw(long ptr, long contextPtr);
    protected native void nativeFlush(long ptr);
    protected native void nativeSetClip(long ptr, long contextPtr, double x, double y, double w, double h);
    protected native void nativeSetTransform(long ptr, long contextPtr, double[] matrix);
    protected native void nativeDrawLine(long ptr, long contextPtr, double x1, double y1, double x2, double y2);
    protected native void nativeDrawRect(long ptr, long contextPtr, double x, double y, double w, double h);
    protected native void nativeFillRect(long ptr, long contextPtr, double x, double y, double w, double h);
    protected native void nativeDrawCircle(long ptr, long contextPtr, double cx, double cy, double r);
    protected native void nativeFillCircle(long ptr, long contextPtr, double cx, double cy, double r);
    protected native void nativeDrawPolygon(long ptr, long contextPtr, double[] xPoints, double[] yPoints, int count);
    protected native void nativeFillPolygon(long ptr, long contextPtr, double[] xPoints, double[] yPoints, int count);
    protected native void nativeDrawText(long ptr, long contextPtr, String text, double x, double y);
    protected native void nativeDrawImage(long ptr, long contextPtr, long imagePtr, double x, double y);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.2 DrawDevice.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public abstract class DrawDevice extends NativeObject {
    
    public enum DeviceType {
        DISPLAY,
        RASTER_IMAGE,
        PDF,
        SVG,
        TILE,
        WEBGL
    }
    
    public enum DeviceState {
        UNINITIALIZED,
        READY,
        DRAWING,
        ERROR
    }
    
    protected DrawDevice(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public abstract DeviceType getDeviceType();
    
    public abstract int getWidth();
    
    public abstract int getHeight();
    
    public abstract double getDpi();
    
    public DeviceState getState() {
        checkNotDisposed();
        return DeviceState.values()[nativeGetState(getNativePtr())];
    }
    
    public boolean isReady() {
        return getState() == DeviceState.READY;
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public void resize(int width, int height) {
        checkNotDisposed();
        nativeResize(getNativePtr(), width, height);
    }
    
    public byte[] toByteArray(String format) {
        checkNotDisposed();
        return nativeToByteArray(getNativePtr(), format);
    }
    
    public void saveToFile(String path, String format) {
        checkNotDisposed();
        nativeSaveToFile(getNativePtr(), path, format);
    }
    
    protected native int nativeGetState(long ptr);
    protected native void nativeClear(long ptr);
    protected native void nativeResize(long ptr, int width, int height);
    protected native byte[] nativeToByteArray(long ptr, String format);
    protected native void nativeSaveToFile(long ptr, String path, String format);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.3 DrawContext.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public class DrawContext extends NativeObject {
    
    private final DrawDevice device;
    private final DrawEngine engine;
    
    public DrawContext(DrawDevice device, DrawEngine engine) {
        this.device = device;
        this.engine = engine;
        setNativePtr(nativeCreate(device.getNativePtr(), engine.getNativePtr()));
    }
    
    public DrawDevice getDevice() {
        return device;
    }
    
    public DrawEngine getEngine() {
        return engine;
    }
    
    public void setPen(Pen pen) {
        checkNotDisposed();
        nativeSetPen(getNativePtr(), pen.getNativePtr());
    }
    
    public void setBrush(Brush brush) {
        checkNotDisposed();
        nativeSetBrush(getNativePtr(), brush.getNativePtr());
    }
    
    public void setFont(Font font) {
        checkNotDisposed();
        nativeSetFont(getNativePtr(), font.getNativePtr());
    }
    
    public void pushState() {
        checkNotDisposed();
        nativePushState(getNativePtr());
    }
    
    public void popState() {
        checkNotDisposed();
        nativePopState(getNativePtr());
    }
    
    public void translate(double dx, double dy) {
        checkNotDisposed();
        nativeTranslate(getNativePtr(), dx, dy);
    }
    
    public void scale(double sx, double sy) {
        checkNotDisposed();
        nativeScale(getNativePtr(), sx, sy);
    }
    
    public void rotate(double angle) {
        checkNotDisposed();
        nativeRotate(getNativePtr(), angle);
    }
    
    private native long nativeCreate(long devicePtr, long enginePtr);
    private native void nativeSetPen(long ptr, long penPtr);
    private native void nativeSetBrush(long ptr, long brushPtr);
    private native void nativeSetFont(long ptr, long fontPtr);
    private native void nativePushState(long ptr);
    private native void nativePopState(long ptr);
    private native void nativeTranslate(long ptr, double dx, double dy);
    private native void nativeScale(long ptr, double sx, double sy);
    private native void nativeRotate(long ptr, double angle);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.4 Pen.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public class Pen extends NativeObject {
    
    public enum LineStyle {
        SOLID,
        DASH,
        DOT,
        DASH_DOT,
        DASH_DOT_DOT,
        NONE
    }
    
    public enum LineCap {
        FLAT,
        ROUND,
        SQUARE
    }
    
    public enum LineJoin {
        MITER,
        ROUND,
        BEVEL
    }
    
    public Pen() {
        setNativePtr(nativeCreate());
    }
    
    public Pen(Color color, double width) {
        setNativePtr(nativeCreateWithColor(color.getNativePtr(), width));
    }
    
    public void setColor(Color color) {
        checkNotDisposed();
        nativeSetColor(getNativePtr(), color.getNativePtr());
    }
    
    public Color getColor() {
        checkNotDisposed();
        long colorPtr = nativeGetColor(getNativePtr());
        return colorPtr != 0 ? new Color(colorPtr) : null;
    }
    
    public void setWidth(double width) {
        checkNotDisposed();
        nativeSetWidth(getNativePtr(), width);
    }
    
    public double getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }
    
    public void setStyle(LineStyle style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style.ordinal());
    }
    
    public LineStyle getStyle() {
        checkNotDisposed();
        return LineStyle.values()[nativeGetStyle(getNativePtr())];
    }
    
    public void setCap(LineCap cap) {
        checkNotDisposed();
        nativeSetCap(getNativePtr(), cap.ordinal());
    }
    
    public void setJoin(LineJoin join) {
        checkNotDisposed();
        nativeSetJoin(getNativePtr(), join.ordinal());
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithColor(long colorPtr, double width);
    private native void nativeSetColor(long ptr, long colorPtr);
    private native long nativeGetColor(long ptr);
    private native void nativeSetWidth(long ptr, double width);
    private native double nativeGetWidth(long ptr);
    private native void nativeSetStyle(long ptr, int style);
    private native int nativeGetStyle(long ptr);
    private native void nativeSetCap(long ptr, int cap);
    private native void nativeSetJoin(long ptr, int join);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.5 Brush.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public class Brush extends NativeObject {
    
    public enum BrushStyle {
        NONE,
        SOLID,
        LINEAR_GRADIENT,
        RADIAL_GRADIENT,
        PATTERN
    }
    
    public Brush() {
        setNativePtr(nativeCreate());
    }
    
    public Brush(Color color) {
        setNativePtr(nativeCreateWithColor(color.getNativePtr()));
    }
    
    public void setColor(Color color) {
        checkNotDisposed();
        nativeSetColor(getNativePtr(), color.getNativePtr());
    }
    
    public Color getColor() {
        checkNotDisposed();
        long colorPtr = nativeGetColor(getNativePtr());
        return colorPtr != 0 ? new Color(colorPtr) : null;
    }
    
    public void setStyle(BrushStyle style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style.ordinal());
    }
    
    public BrushStyle getStyle() {
        checkNotDisposed();
        return BrushStyle.values()[nativeGetStyle(getNativePtr())];
    }
    
    public void setLinearGradient(double x1, double y1, double x2, double y2, 
                                   Color startColor, Color endColor) {
        checkNotDisposed();
        nativeSetLinearGradient(getNativePtr(), x1, y1, x2, y2, 
                                startColor.getNativePtr(), endColor.getNativePtr());
    }
    
    public void setRadialGradient(double cx, double cy, double radius,
                                   Color centerColor, Color edgeColor) {
        checkNotDisposed();
        nativeSetRadialGradient(getNativePtr(), cx, cy, radius,
                                centerColor.getNativePtr(), edgeColor.getNativePtr());
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithColor(long colorPtr);
    private native void nativeSetColor(long ptr, long colorPtr);
    private native long nativeGetColor(long ptr);
    private native void nativeSetStyle(long ptr, int style);
    private native int nativeGetStyle(long ptr);
    private native void nativeSetLinearGradient(long ptr, double x1, double y1, double x2, double y2, 
                                                 long startColorPtr, long endColorPtr);
    private native void nativeSetRadialGradient(long ptr, double cx, double cy, double radius,
                                                 long centerColorPtr, long edgeColorPtr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.6 Font.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public class Font extends NativeObject {
    
    public enum FontWeight {
        THIN,
        LIGHT,
        NORMAL,
        MEDIUM,
        SEMIBOLD,
        BOLD,
        EXTRABOLD,
        BLACK
    }
    
    public enum FontStyle {
        NORMAL,
        ITALIC,
        OBLIQUE
    }
    
    public Font() {
        setNativePtr(nativeCreate());
    }
    
    public Font(String family, double size) {
        setNativePtr(nativeCreateWithFamily(family, size));
    }
    
    public void setFamily(String family) {
        checkNotDisposed();
        nativeSetFamily(getNativePtr(), family);
    }
    
    public String getFamily() {
        checkNotDisposed();
        return nativeGetFamily(getNativePtr());
    }
    
    public void setSize(double size) {
        checkNotDisposed();
        nativeSetSize(getNativePtr(), size);
    }
    
    public double getSize() {
        checkNotDisposed();
        return nativeGetSize(getNativePtr());
    }
    
    public void setWeight(FontWeight weight) {
        checkNotDisposed();
        nativeSetWeight(getNativePtr(), weight.ordinal());
    }
    
    public void setStyle(FontStyle style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style.ordinal());
    }
    
    public TextMetrics measureText(String text) {
        checkNotDisposed();
        long metricsPtr = nativeMeasureText(getNativePtr(), text);
        return metricsPtr != 0 ? new TextMetrics(metricsPtr) : null;
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithFamily(String family, double size);
    private native void nativeSetFamily(long ptr, String family);
    private native String nativeGetFamily(long ptr);
    private native void nativeSetSize(long ptr, double size);
    private native double nativeGetSize(long ptr);
    private native void nativeSetWeight(long ptr, int weight);
    private native void nativeSetStyle(long ptr, int style);
    private native long nativeMeasureText(long ptr, String text);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.25.7 Image.java

```java
package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.NativeObject;

public class Image extends NativeObject {
    
    public Image(int width, int height) {
        setNativePtr(nativeCreate(width, height));
    }
    
    public Image(String filePath) {
        setNativePtr(nativeLoad(filePath));
    }
    
    public static Image fromByteArray(byte[] data) {
        long ptr = nativeFromByteArray(data);
        return ptr != 0 ? new Image(ptr) : null;
    }
    
    private Image(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public int getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }
    
    public int getHeight() {
        checkNotDisposed();
        return nativeGetHeight(getNativePtr());
    }
    
    public int getStride() {
        checkNotDisposed();
        return nativeGetStride(getNativePtr());
    }
    
    public byte[] toByteArray(String format) {
        checkNotDisposed();
        return nativeToByteArray(getNativePtr(), format);
    }
    
    public void save(String path, String format) {
        checkNotDisposed();
        nativeSave(getNativePtr(), path, format);
    }
    
    public Color getPixel(int x, int y) {
        checkNotDisposed();
        long colorPtr = nativeGetPixel(getNativePtr(), x, y);
        return colorPtr != 0 ? new Color(colorPtr) : null;
    }
    
    public void setPixel(int x, int y, Color color) {
        checkNotDisposed();
        nativeSetPixel(getNativePtr(), x, y, color.getNativePtr());
    }
    
    private static native long nativeCreate(int width, int height);
    private static native long nativeLoad(String filePath);
    private static native long nativeFromByteArray(byte[] data);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native int nativeGetStride(long ptr);
    private native byte[] nativeToByteArray(long ptr, String format);
    private native void nativeSave(long ptr, String path, String format);
    private native long nativeGetPixel(long ptr, int x, int y);
    private native void nativeSetPixel(long ptr, int x, int y, long colorPtr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.26 图层管理接口 (layer)

### 6.26.1 RasterLayer.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class RasterLayer extends ChartLayer {
    
    public RasterLayer() {
        setNativePtr(nativeCreate());
    }
    
    public RasterLayer(String filePath) {
        setNativePtr(nativeCreateFromFile(filePath));
    }
    
    public int getRasterWidth() {
        checkNotDisposed();
        return nativeGetRasterWidth(getNativePtr());
    }
    
    public int getRasterHeight() {
        checkNotDisposed();
        return nativeGetRasterHeight(getNativePtr());
    }
    
    public int getBandCount() {
        checkNotDisposed();
        return nativeGetBandCount(getNativePtr());
    }
    
    public double getNoDataValue() {
        checkNotDisposed();
        return nativeGetNoDataValue(getNativePtr());
    }
    
    public void setNoDataValue(double value) {
        checkNotDisposed();
        nativeSetNoDataValue(getNativePtr(), value);
    }
    
    public double[] getPixelValue(int x, int y) {
        checkNotDisposed();
        return nativeGetPixelValue(getNativePtr(), x, y);
    }
    
    public void setResampleMethod(String method) {
        checkNotDisposed();
        nativeSetResampleMethod(getNativePtr(), method);
    }
    
    public void setContrastEnhancement(String enhancement) {
        checkNotDisposed();
        nativeSetContrastEnhancement(getNativePtr(), enhancement);
    }
    
    private native long nativeCreate();
    private native long nativeCreateFromFile(String filePath);
    private native int nativeGetRasterWidth(long ptr);
    private native int nativeGetRasterHeight(long ptr);
    private native int nativeGetBandCount(long ptr);
    private native double nativeGetNoDataValue(long ptr);
    private native void nativeSetNoDataValue(long ptr, double value);
    private native double[] nativeGetPixelValue(long ptr, int x, int y);
    private native void nativeSetResampleMethod(long ptr, String method);
    private native void nativeSetContrastEnhancement(long ptr, String enhancement);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.26.2 MemoryLayer.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.geometry.Geometry;

public class MemoryLayer extends VectorLayer {
    
    public MemoryLayer() {
        setNativePtr(nativeCreate());
    }
    
    public MemoryLayer(String name) {
        setNativePtr(nativeCreateWithName(name));
    }
    
    public void addFeature(FeatureInfo feature) {
        checkNotDisposed();
        nativeAddFeature(getNativePtr(), feature.getNativePtr());
    }
    
    public void addGeometry(Geometry geometry) {
        checkNotDisposed();
        nativeAddGeometry(getNativePtr(), geometry.getNativePtr());
    }
    
    public void addGeometryWithAttributes(Geometry geometry, Map<String, Object> attributes) {
        checkNotDisposed();
        nativeAddGeometryWithAttributes(getNativePtr(), geometry.getNativePtr(), attributes);
    }
    
    public void removeFeature(long featureId) {
        checkNotDisposed();
        nativeRemoveFeature(getNativePtr(), featureId);
    }
    
    public void updateFeature(long featureId, Geometry geometry) {
        checkNotDisposed();
        nativeUpdateFeature(getNativePtr(), featureId, geometry.getNativePtr());
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public int getFeatureCount() {
        checkNotDisposed();
        return nativeGetFeatureCount(getNativePtr());
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithName(String name);
    private native void nativeAddFeature(long ptr, long featurePtr);
    private native void nativeAddGeometry(long ptr, long geometryPtr);
    private native void nativeAddGeometryWithAttributes(long ptr, long geometryPtr, Map<String, Object> attributes);
    private native void nativeRemoveFeature(long ptr, long featureId);
    private native void nativeUpdateFeature(long ptr, long featureId, long geometryPtr);
    private native void nativeClear(long ptr);
    private native int nativeGetFeatureCount(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.26.3 LayerGroup.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public class LayerGroup extends ChartLayer {
    
    public LayerGroup() {
        setNativePtr(nativeCreate());
    }
    
    public LayerGroup(String name) {
        setNativePtr(nativeCreateWithName(name));
    }
    
    public void addLayer(ChartLayer layer) {
        checkNotDisposed();
        nativeAddLayer(getNativePtr(), layer.getNativePtr());
    }
    
    public void removeLayer(ChartLayer layer) {
        checkNotDisposed();
        nativeRemoveLayer(getNativePtr(), layer.getNativePtr());
    }
    
    public void removeLayerAt(int index) {
        checkNotDisposed();
        nativeRemoveLayerAt(getNativePtr(), index);
    }
    
    public ChartLayer getLayerAt(int index) {
        checkNotDisposed();
        long layerPtr = nativeGetLayerAt(getNativePtr(), index);
        return layerPtr != 0 ? new ChartLayer(layerPtr) : null;
    }
    
    public int getLayerCount() {
        checkNotDisposed();
        return nativeGetLayerCount(getNativePtr());
    }
    
    public List<ChartLayer> getLayers() {
        checkNotDisposed();
        return nativeGetLayers(getNativePtr());
    }
    
    public void moveLayer(int fromIndex, int toIndex) {
        checkNotDisposed();
        nativeMoveLayer(getNativePtr(), fromIndex, toIndex);
    }
    
    public void setExclusiveVisible(boolean exclusive) {
        checkNotDisposed();
        nativeSetExclusiveVisible(getNativePtr(), exclusive);
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithName(String name);
    private native void nativeAddLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayerAt(long ptr, int index);
    private native long nativeGetLayerAt(long ptr, int index);
    private native int nativeGetLayerCount(long ptr);
    private native List<ChartLayer> nativeGetLayers(long ptr);
    private native void nativeMoveLayer(long ptr, int fromIndex, int toIndex);
    private native void nativeSetExclusiveVisible(long ptr, boolean exclusive);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.26.4 DataSource.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public class DataSource extends NativeObject {
    
    public DataSource(String connectionString) {
        setNativePtr(nativeCreate(connectionString));
    }
    
    public boolean open() {
        checkNotDisposed();
        return nativeOpen(getNativePtr());
    }
    
    public void close() {
        checkNotDisposed();
        nativeClose(getNativePtr());
    }
    
    public boolean isOpen() {
        checkNotDisposed();
        return nativeIsOpen(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public String getConnectionString() {
        checkNotDisposed();
        return nativeGetConnectionString(getNativePtr());
    }
    
    public int getLayerCount() {
        checkNotDisposed();
        return nativeGetLayerCount(getNativePtr());
    }
    
    public List<String> getLayerNames() {
        checkNotDisposed();
        return nativeGetLayerNames(getNativePtr());
    }
    
    public VectorLayer getLayer(String name) {
        checkNotDisposed();
        long layerPtr = nativeGetLayer(getNativePtr(), name);
        return layerPtr != 0 ? new VectorLayer(layerPtr) : null;
    }
    
    public VectorLayer getLayer(int index) {
        checkNotDisposed();
        long layerPtr = nativeGetLayerByIndex(getNativePtr(), index);
        return layerPtr != 0 ? new VectorLayer(layerPtr) : null;
    }
    
    public boolean testCapability(String capability) {
        checkNotDisposed();
        return nativeTestCapability(getNativePtr(), capability);
    }
    
    private native long nativeCreate(String connectionString);
    private native boolean nativeOpen(long ptr);
    private native void nativeClose(long ptr);
    private native boolean nativeIsOpen(long ptr);
    private native String nativeGetName(long ptr);
    private native String nativeGetConnectionString(long ptr);
    private native int nativeGetLayerCount(long ptr);
    private native List<String> nativeGetLayerNames(long ptr);
    private native long nativeGetLayer(long ptr, String name);
    private native long nativeGetLayerByIndex(long ptr, int index);
    private native boolean nativeTestCapability(long ptr, String capability);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.26.5 DriverManager.java

```java
package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public class DriverManager extends NativeObject {
    
    private static DriverManager instance;
    
    public static synchronized DriverManager getInstance() {
        if (instance == null) {
            instance = new DriverManager();
        }
        return instance;
    }
    
    private DriverManager() {
        setNativePtr(nativeGetInstance());
    }
    
    public int getDriverCount() {
        checkNotDisposed();
        return nativeGetDriverCount(getNativePtr());
    }
    
    public List<String> getDriverNames() {
        checkNotDisposed();
        return nativeGetDriverNames(getNativePtr());
    }
    
    public Driver getDriver(String name) {
        checkNotDisposed();
        long driverPtr = nativeGetDriver(getNativePtr(), name);
        return driverPtr != 0 ? new Driver(driverPtr) : null;
    }
    
    public Driver getDriver(int index) {
        checkNotDisposed();
        long driverPtr = nativeGetDriverByIndex(getNativePtr(), index);
        return driverPtr != 0 ? new Driver(driverPtr) : null;
    }
    
    public DataSource open(String connectionString) {
        checkNotDisposed();
        long dsPtr = nativeOpen(getNativePtr(), connectionString);
        return dsPtr != 0 ? new DataSource(dsPtr) : null;
    }
    
    public boolean isDriverAvailable(String name) {
        checkNotDisposed();
        return nativeIsDriverAvailable(getNativePtr(), name);
    }
    
    private native long nativeGetInstance();
    private native int nativeGetDriverCount(long ptr);
    private native List<String> nativeGetDriverNames(long ptr);
    private native long nativeGetDriver(long ptr, String name);
    private native long nativeGetDriverByIndex(long ptr, int index);
    private native long nativeOpen(long ptr, String connectionString);
    private native boolean nativeIsDriverAvailable(long ptr, String name);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 6.27 缓存管理接口 (cache)

### 6.27.1 MemoryTileCache.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class MemoryTileCache extends TileCache {
    
    public MemoryTileCache() {
        setNativePtr(nativeCreate());
    }
    
    public MemoryTileCache(long maxMemoryBytes) {
        setNativePtr(nativeCreateWithLimit(maxMemoryBytes));
    }
    
    public void setMaxMemory(long maxBytes) {
        checkNotDisposed();
        nativeSetMaxMemory(getNativePtr(), maxBytes);
    }
    
    public long getMaxMemory() {
        checkNotDisposed();
        return nativeGetMaxMemory(getNativePtr());
    }
    
    public long getUsedMemory() {
        checkNotDisposed();
        return nativeGetUsedMemory(getNativePtr());
    }
    
    public double getMemoryUsageRatio() {
        checkNotDisposed();
        return nativeGetMemoryUsageRatio(getNativePtr());
    }
    
    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }
    
    public int getTileCount() {
        checkNotDisposed();
        return nativeGetTileCount(getNativePtr());
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithLimit(long maxBytes);
    private native void nativeSetMaxMemory(long ptr, long maxBytes);
    private native long nativeGetMaxMemory(long ptr);
    private native long nativeGetUsedMemory(long ptr);
    private native double nativeGetMemoryUsageRatio(long ptr);
    private native void nativeClear(long ptr);
    private native int nativeGetTileCount(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.27.2 DiskTileCache.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class DiskTileCache extends TileCache {
    
    public DiskTileCache(String cacheDirectory) {
        setNativePtr(nativeCreate(cacheDirectory));
    }
    
    public DiskTileCache(String cacheDirectory, long maxDiskBytes) {
        setNativePtr(nativeCreateWithLimit(cacheDirectory, maxDiskBytes));
    }
    
    public void setMaxDiskSpace(long maxBytes) {
        checkNotDisposed();
        nativeSetMaxDiskSpace(getNativePtr(), maxBytes);
    }
    
    public long getMaxDiskSpace() {
        checkNotDisposed();
        return nativeGetMaxDiskSpace(getNativePtr());
    }
    
    public long getUsedDiskSpace() {
        checkNotDisposed();
        return nativeGetUsedDiskSpace(getNativePtr());
    }
    
    public String getCacheDirectory() {
        checkNotDisposed();
        return nativeGetCacheDirectory(getNativePtr());
    }
    
    public void clearCache() {
        checkNotDisposed();
        nativeClearCache(getNativePtr());
    }
    
    public void setCompressionEnabled(boolean enabled) {
        checkNotDisposed();
        nativeSetCompressionEnabled(getNativePtr(), enabled);
    }
    
    public void setExpirationDays(int days) {
        checkNotDisposed();
        nativeSetExpirationDays(getNativePtr(), days);
    }
    
    public void cleanupExpired() {
        checkNotDisposed();
        nativeCleanupExpired(getNativePtr());
    }
    
    private native long nativeCreate(String cacheDirectory);
    private native long nativeCreateWithLimit(String cacheDirectory, long maxBytes);
    private native void nativeSetMaxDiskSpace(long ptr, long maxBytes);
    private native long nativeGetMaxDiskSpace(long ptr);
    private native long nativeGetUsedDiskSpace(long ptr);
    private native String nativeGetCacheDirectory(long ptr);
    private native void nativeClearCache(long ptr);
    private native void nativeSetCompressionEnabled(long ptr, boolean enabled);
    private native void nativeSetExpirationDays(long ptr, int days);
    private native void nativeCleanupExpired(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.27.3 MultiLevelTileCache.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public class MultiLevelTileCache extends TileCache {
    
    public MultiLevelTileCache() {
        setNativePtr(nativeCreate());
    }
    
    public void addCache(TileCache cache, int priority) {
        checkNotDisposed();
        nativeAddCache(getNativePtr(), cache.getNativePtr(), priority);
    }
    
    public void removeCache(TileCache cache) {
        checkNotDisposed();
        nativeRemoveCache(getNativePtr(), cache.getNativePtr());
    }
    
    public int getCacheCount() {
        checkNotDisposed();
        return nativeGetCacheCount(getNativePtr());
    }
    
    public void clearAll() {
        checkNotDisposed();
        nativeClearAll(getNativePtr());
    }
    
    public void setWriteThrough(boolean enabled) {
        checkNotDisposed();
        nativeSetWriteThrough(getNativePtr(), enabled);
    }
    
    public void setReadOrder(int[] priorities) {
        checkNotDisposed();
        nativeSetReadOrder(getNativePtr(), priorities);
    }
    
    private native long nativeCreate();
    private native void nativeAddCache(long ptr, long cachePtr, int priority);
    private native void nativeRemoveCache(long ptr, long cachePtr);
    private native int nativeGetCacheCount(long ptr);
    private native void nativeClearAll(long ptr);
    private native void nativeSetWriteThrough(long ptr, boolean enabled);
    private native void nativeSetReadOrder(long ptr, int[] priorities);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.27.4 OfflineStorageManager.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;
import java.util.List;

public class OfflineStorageManager extends NativeObject {
    
    public OfflineStorageManager(String storagePath) {
        setNativePtr(nativeCreate(storagePath));
    }
    
    public OfflineRegion downloadRegion(Envelope bounds, int minZoom, int maxZoom, 
                                         ProgressCallback callback) {
        checkNotDisposed();
        long regionPtr = nativeDownloadRegion(getNativePtr(), 
            bounds.getMinX(), bounds.getMinY(), 
            bounds.getMaxX(), bounds.getMaxY(),
            minZoom, maxZoom, callback);
        return regionPtr != 0 ? new OfflineRegion(regionPtr) : null;
    }
    
    public void cancelDownload(long regionId) {
        checkNotDisposed();
        nativeCancelDownload(getNativePtr(), regionId);
    }
    
    public List<OfflineRegion> getRegions() {
        checkNotDisposed();
        return nativeGetRegions(getNativePtr());
    }
    
    public OfflineRegion getRegion(long regionId) {
        checkNotDisposed();
        long regionPtr = nativeGetRegion(getNativePtr(), regionId);
        return regionPtr != 0 ? new OfflineRegion(regionPtr) : null;
    }
    
    public void deleteRegion(long regionId) {
        checkNotDisposed();
        nativeDeleteRegion(getNativePtr(), regionId);
    }
    
    public long getStorageUsed() {
        checkNotDisposed();
        return nativeGetStorageUsed(getNativePtr());
    }
    
    public long getStorageAvailable() {
        checkNotDisposed();
        return nativeGetStorageAvailable(getNativePtr());
    }
    
    public void setMaxStorage(long maxBytes) {
        checkNotDisposed();
        nativeSetMaxStorage(getNativePtr(), maxBytes);
    }
    
    public interface ProgressCallback {
        void onProgress(int percentage);
        void onComplete();
        void onError(String message);
    }
    
    private native long nativeCreate(String storagePath);
    private native long nativeDownloadRegion(long ptr, double minX, double minY, 
                                              double maxX, double maxY, 
                                              int minZoom, int maxZoom, 
                                              ProgressCallback callback);
    private native void nativeCancelDownload(long ptr, long regionId);
    private native List<OfflineRegion> nativeGetRegions(long ptr);
    private native long nativeGetRegion(long ptr, long regionId);
    private native void nativeDeleteRegion(long ptr, long regionId);
    private native long nativeGetStorageUsed(long ptr);
    private native long nativeGetStorageAvailable(long ptr);
    private native void nativeSetMaxStorage(long ptr, long maxBytes);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.27.5 OfflineRegion.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Envelope;

public class OfflineRegion extends NativeObject {
    
    protected OfflineRegion(long nativePtr) {
        setNativePtr(nativePtr);
    }
    
    public long getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }
    
    public Envelope getBounds() {
        checkNotDisposed();
        double[] bounds = nativeGetBounds(getNativePtr());
        return new Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
    }
    
    public int getMinZoom() {
        checkNotDisposed();
        return nativeGetMinZoom(getNativePtr());
    }
    
    public int getMaxZoom() {
        checkNotDisposed();
        return nativeGetMaxZoom(getNativePtr());
    }
    
    public long getSize() {
        checkNotDisposed();
        return nativeGetSize(getNativePtr());
    }
    
    public int getTileCount() {
        checkNotDisposed();
        return nativeGetTileCount(getNativePtr());
    }
    
    public long getCreatedTime() {
        checkNotDisposed();
        return nativeGetCreatedTime(getNativePtr());
    }
    
    public boolean isComplete() {
        checkNotDisposed();
        return nativeIsComplete(getNativePtr());
    }
    
    private native long nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native double[] nativeGetBounds(long ptr);
    private native int nativeGetMinZoom(long ptr);
    private native int nativeGetMaxZoom(long ptr);
    private native long nativeGetSize(long ptr);
    private native int nativeGetTileCount(long ptr);
    private native long nativeGetCreatedTime(long ptr);
    private native boolean nativeIsComplete(long ptr);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

### 6.27.6 DataEncryption.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.NativeObject;

public class DataEncryption extends NativeObject {
    
    public DataEncryption() {
        setNativePtr(nativeCreate());
    }
    
    public DataEncryption(String password) {
        setNativePtr(nativeCreateWithPassword(password));
    }
    
    public void setPassword(String password) {
        checkNotDisposed();
        nativeSetPassword(getNativePtr(), password);
    }
    
    public byte[] encrypt(byte[] data) {
        checkNotDisposed();
        return nativeEncrypt(getNativePtr(), data);
    }
    
    public byte[] decrypt(byte[] encryptedData) {
        checkNotDisposed();
        return nativeDecrypt(getNativePtr(), encryptedData);
    }
    
    public String encryptString(String text) {
        checkNotDisposed();
        return nativeEncryptString(getNativePtr(), text);
    }
    
    public String decryptString(String encryptedText) {
        checkNotDisposed();
        return nativeDecryptString(getNativePtr(), encryptedText);
    }
    
    public boolean encryptFile(String inputPath, String outputPath) {
        checkNotDisposed();
        return nativeEncryptFile(getNativePtr(), inputPath, outputPath);
    }
    
    public boolean decryptFile(String inputPath, String outputPath) {
        checkNotDisposed();
        return nativeDecryptFile(getNativePtr(), inputPath, outputPath);
    }
    
    public String generateKey() {
        checkNotDisposed();
        return nativeGenerateKey(getNativePtr());
    }
    
    public void setAlgorithm(String algorithm) {
        checkNotDisposed();
        nativeSetAlgorithm(getNativePtr(), algorithm);
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithPassword(String password);
    private native void nativeSetPassword(long ptr, String password);
    private native byte[] nativeEncrypt(long ptr, byte[] data);
    private native byte[] nativeDecrypt(long ptr, byte[] encryptedData);
    private native String nativeEncryptString(long ptr, String text);
    private native String nativeDecryptString(long ptr, String encryptedText);
    private native boolean nativeEncryptFile(long ptr, String inputPath, String outputPath);
    private native boolean nativeDecryptFile(long ptr, String inputPath, String outputPath);
    private native String nativeGenerateKey(long ptr);
    private native void nativeSetAlgorithm(long ptr, String algorithm);
    
    @Override
    protected native void nativeDispose(long ptr);
}
```

---

## 7. C++核心模块映射

### 7.1 模块映射表

| Java类 | C++类 | 模块 | 头文件 |
|--------|-------|------|--------|
| `ChartViewer` | `DrawFacade` | ogc_graph | draw_facade.h |
| `Geometry` | `ogc::geom::Geometry` | ogc_geom | geometry.h |
| `Point` | `ogc::geom::Point` | ogc_geom | point.h |
| `LineString` | `ogc::geom::LineString` | ogc_geom | linestring.h |
| `Polygon` | `ogc::geom::Polygon` | ogc_geom | polygon.h |
| `LinearRing` | `ogc::geom::LinearRing` | ogc_geom | linearring.h |
| `MultiPoint` | `ogc::geom::MultiPoint` | ogc_geom | multipoint.h |
| `MultiLineString` | `ogc::geom::MultiLineString` | ogc_geom | multilinestring.h |
| `MultiPolygon` | `ogc::geom::MultiPolygon` | ogc_geom | multipolygon.h |
| `GeometryCollection` | `ogc::geom::GeometryCollection` | ogc_geom | geometrycollection.h |
| `Envelope` | `ogc::geom::Envelope` | ogc_geom | envelope.h |
| `Coordinate` | `ogc::geom::Coordinate` | ogc_geom | coordinate.h |
| `FeatureInfo` | `CNFeature` | ogc_feature | feature.h |
| `FieldValue` | `CNFieldValue` | ogc_feature | field_value.h |
| `FieldDefn` | `CNFieldDefn` | ogc_feature | field_defn.h |
| `ChartLayer` | `CNLayer` | ogc_layer | layer.h |
| `VectorLayer` | `CNVectorLayer` | ogc_layer | vector_layer.h |
| `LayerManager` | `LayerManager` | ogc_graph | layer_manager.h |
| `TileCache` | `TileCache` | ogc_cache | tile_cache.h |
| `TileKey` | `TileKey` | ogc_cache | tile_key.h |
| `MemoryTileCache` | `MemoryTileCache` | ogc_cache | memory_tile_cache.h |
| `DiskTileCache` | `DiskTileCache` | ogc_cache | disk_tile_cache.h |
| `MultiLevelTileCache` | `MultiLevelTileCache` | ogc_cache | multi_level_tile_cache.h |
| `OfflineStorageManager` | `OfflineStorageManager` | ogc_cache | offline_storage_manager.h |
| `OfflineRegion` | `OfflineRegion` | ogc_cache | offline_storage_manager.h |
| `DataEncryption` | `DataEncryption` | ogc_cache | data_encryption.h |
| `DrawStyle` | `DrawStyle` | ogc_draw | draw_style.h |
| `Color` | `ogc::draw::Color` | ogc_draw | color.h |
| `RenderContext` | `RenderContext` | ogc_draw | render_context.h |
| `Route` | `ogc::navi::Route` | ogc_navi | route.h |
| `Waypoint` | `ogc::navi::Waypoint` | ogc_navi | waypoint.h |
| `AisManager` | `ogc::navi::AisManager` | ogc_navi | ais_manager.h |
| `AisTarget` | `ogc::navi::AisTarget` | ogc_navi | ais_target.h |
| `NavigationEngine` | `ogc::navi::NavigationEngine` | ogc_navi | navigation_engine.h |
| `PositionManager` | `ogc::navi::PositionManager` | ogc_navi | position_manager.h |
| `Track` | `ogc::navi::Track` | ogc_navi | track.h |
| `TrackPoint` | `ogc::navi::TrackPoint` | ogc_navi | track_point.h |
| `AlertEngine` | `ogc::alert::AlertEngine` | ogc_alert | alert_engine.h |
| `Alert` | `ogc::alert::Alert` | ogc_alert | alert.h |
| `AlertChecker` | `ogc::alert::AlertChecker` | ogc_alert | alert_checker.h |
| `CoordinateTransformer` | `CoordinateTransformer` | ogc_proj | coordinate_transformer.h |
| `TransformMatrix` | `TransformMatrix` | ogc_proj | transform_matrix.h |
| `Symbolizer` | `ogc::symbology::Symbolizer` | ogc_symbology | symbolizer.h |
| `Filter` | `ogc::symbology::Filter` | ogc_symbology | filter.h |
| `SymbolizerRule` | `ogc::symbology::SymbolizerRule` | ogc_symbology | symbolizer_rule.h |
| `Logger` | `ogc::base::Logger` | ogc_base | logger.h |
| `PerformanceMonitor` | `ogc::base::PerformanceMonitor` | ogc_base | performance_monitor.h |
| `I18nManager` | - | Java only | - |
| `AccessibilityHelper` | - | Java only | - |
| `MetricsExporter` | - | Java only | - |
| `GeometryFactory` | `ogc::geom::GeometryFactory` | ogc_geom | geometry_factory.h |
| `LabelEngine` | `ogc::graph::LabelEngine` | ogc_graph | label_engine.h |
| `LODManager` | `ogc::graph::LODManager` | ogc_graph | lod_manager.h |
| `HitTest` | `ogc::graph::HitTest` | ogc_graph | hit_test.h |
| `TransformManager` | `ogc::graph::TransformManager` | ogc_graph | transform_manager.h |
| `RenderTask` | `ogc::graph::RenderTask` | ogc_graph | render_task.h |
| `RenderQueue` | `ogc::graph::RenderQueue` | ogc_graph | render_queue.h |
| `IPositionProvider` | `IPositionProvider` | ogc_navi | position_provider.h |
| `NmeaParser` | `ogc::navi::NmeaParser` | ogc_navi | nmea_parser.h |
| `CollisionAssessor` | `ogc::navi::CollisionAssessor` | ogc_navi | collision_assessor.h |
| `UkcCalculator` | `ogc::navi::UkcCalculator` | ogc_navi | ukc_calculator.h |
| `OffCourseDetector` | `ogc::navi::OffCourseDetector` | ogc_navi | off_course_detector.h |
| `RoutePlanner` | `ogc::navi::RoutePlanner` | ogc_navi | route_planner.h |
| `TrackRecorder` | `ogc::navi::TrackRecorder` | ogc_navi | track_recorder.h |
| `ChartParser` | `ogc::parser::ChartParser` | ogc_parser | chart_parser.h |
| `IncrementalParser` | `ogc::parser::IncrementalParser` | ogc_parser | incremental_parser.h |
| `LabelInfo` | `ogc::graph::LabelInfo` | ogc_graph | label_info.h |
| `LODLevel` | `ogc::graph::LODLevel` | ogc_graph | lod_level.h |
| `CpaResult` | `ogc::navi::CpaResult` | ogc_navi | cpa_result.h |
| `DeviationResult` | `ogc::navi::DeviationResult` | ogc_navi | deviation_result.h |
| `PositionQuality` | `ogc::navi::PositionQuality` | ogc_navi | position_quality.h |
| `InputValidator` | - | Java only | - |
| `SecureLibraryLoader` | - | Java only | - |
| `SensitiveDataProtector` | - | Java only | - |
| `RenderOptimizer` | `ogc::graph::RenderOptimizer` | ogc_graph | render_optimizer.h |
| `RenderStats` | `ogc::graph::RenderStats` | ogc_graph | render_stats.h |
| `TileRenderPriority` | `ogc::graph::TileRenderPriority` | ogc_graph | tile_render_priority.h |
| `ChartCanvas` | - | JavaFX only | - |
| `JavaFXEventHandler` | - | JavaFX only | - |
| `JavaFXImageDevice` | - | JavaFX only | - |
| `JavaFXThreadBridge` | - | JavaFX only | - |
| `PropertyBinder` | - | JavaFX only | - |
| `AppLifecycleManager` | - | Java only | - |
| `ThreadSafeRenderState` | - | Java only | - |
| `ErrorRecoveryManager` | - | Java only | - |
| `CircuitBreaker` | - | Java only | - |
| `GracefulDegradation` | - | Java only | - |
| `MetricsCollector` | - | Java only | - |
| `PerformanceReport` | - | Java only | - |
| `HealthCheck` | - | Java only | - |
| `HealthStatus` | - | Java only | - |
| `ChartApplicationHealthCheck` | - | Java only | - |
| `LibraryLoader` | - | Java only | - |
| `DrawEngine` | `ogc::draw::DrawEngine` | ogc_draw | draw_engine.h |
| `DrawDevice` | `ogc::draw::DrawDevice` | ogc_draw | draw_device.h |
| `DrawContext` | `ogc::draw::DrawContext` | ogc_draw | draw_context.h |
| `Pen` | `ogc::draw::Pen` | ogc_draw | draw_style.h |
| `Brush` | `ogc::draw::Brush` | ogc_draw | draw_style.h |
| `Font` | `ogc::draw::Font` | ogc_draw | font.h |
| `Image` | `ogc::draw::Image` | ogc_draw | image.h |
| `RasterLayer` | `CNRasterLayer` | ogc_layer | raster_layer.h |
| `MemoryLayer` | `CNMemoryLayer` | ogc_layer | memory_layer.h |
| `LayerGroup` | `CNLayerGroup` | ogc_layer | layer_group.h |
| `DataSource` | `CNDataSource` | ogc_layer | datasource.h |
| `DriverManager` | `CNDriverManager` | ogc_layer | driver_manager.h |

### 7.2 方法映射表

#### ChartViewer方法映射

| Java方法 | C++方法 | 说明 |
|----------|---------|------|
| `loadChart(path)` | `DrawFacade::LoadChart(path)` | 加载海图 |
| `render(canvas, w, h)` | `DrawFacade::Render(device, w, h)` | 渲染 |
| `setViewport(...)` | `DrawFacade::SetViewport(...)` | 设置视口 |
| `pan(dx, dy)` | `DrawFacade::Pan(dx, dy)` | 平移 |
| `zoom(factor, cx, cy)` | `DrawFacade::Zoom(factor, cx, cy)` | 缩放 |
| `queryFeature(x, y)` | `DrawFacade::QueryFeature(x, y)` | 查询要素 |
| `screenToWorld(x, y)` | `DrawFacade::ScreenToWorld(x, y)` | 坐标转换 |
| `worldToScreen(x, y)` | `DrawFacade::WorldToScreen(x, y)` | 坐标转换 |

#### Geometry方法映射

| Java方法 | C++方法 | 说明 |
|----------|---------|------|
| `getDimension()` | `Geometry::GetDimension()` | 获取维度 |
| `getNumPoints()` | `Geometry::GetNumPoints()` | 获取点数 |
| `isEmpty()` | `Geometry::IsEmpty()` | 是否为空 |
| `intersects(other)` | `Geometry::Intersects(other)` | 相交判断 |
| `distance(other)` | `Geometry::Distance(other)` | 距离计算 |
| `buffer(dist)` | `Geometry::Buffer(dist)` | 缓冲区 |

---

## 8. 接口实现清单

### 8.1 JNI桥接层实现清单

| 文件 | 类/函数 | 状态 |
|------|---------|------|
| jni_env.cpp | `JniEnvManager` | 待实现 |
| jni_convert.cpp | `JniConverter` | 待实现 |
| jni_exception.cpp | `JniException` | 待实现 |
| jni_memory.cpp | `JniLocalRef`, `JniGlobalRef` | 待实现 |
| chart_viewer_jni.cpp | ChartViewer JNI方法 | 待实现 |
| geometry_jni.cpp | Geometry JNI方法 | 待实现 |
| point_jni.cpp | Point JNI方法 | 待实现 |
| linestring_jni.cpp | LineString JNI方法 | 待实现 |
| polygon_jni.cpp | Polygon JNI方法 | 待实现 |
| feature_jni.cpp | FeatureInfo JNI方法 | 待实现 |
| layer_jni.cpp | ChartLayer JNI方法 | 待实现 |

### 8.2 Java API层实现清单

| 包 | 类 | 状态 |
|----|----|----|
| cn.cycle.chart.jni | `JniBridge` | 待实现 |
| cn.cycle.chart.jni | `NativeObject` | 待实现 |
| cn.cycle.chart.api.core | `ChartViewer` | 待实现 |
| cn.cycle.chart.api.core | `ChartConfig` | 待实现 |
| cn.cycle.chart.api.core | `Viewport` | 待实现 |
| cn.cycle.chart.api.core | `DisplayMode` | 待实现 |
| cn.cycle.chart.api.core | `RenderContext` | 待实现 |
| cn.cycle.chart.api.geometry | `Geometry` | 待实现 |
| cn.cycle.chart.api.geometry | `Point` | 待实现 |
| cn.cycle.chart.api.geometry | `LineString` | 待实现 |
| cn.cycle.chart.api.geometry | `LinearRing` | 待实现 |
| cn.cycle.chart.api.geometry | `Polygon` | 待实现 |
| cn.cycle.chart.api.geometry | `MultiPoint` | 待实现 |
| cn.cycle.chart.api.geometry | `MultiLineString` | 待实现 |
| cn.cycle.chart.api.geometry | `MultiPolygon` | 待实现 |
| cn.cycle.chart.api.geometry | `GeometryCollection` | 待实现 |
| cn.cycle.chart.api.geometry | `Envelope` | 待实现 |
| cn.cycle.chart.api.geometry | `Coordinate` | 待实现 |
| cn.cycle.chart.api.geometry | `GeometryType` | 待实现 |
| cn.cycle.chart.api.geometry | `GeometryFactory` | 待实现 |
| cn.cycle.chart.api.feature | `FeatureInfo` | 待实现 |
| cn.cycle.chart.api.feature | `FieldValue` | 待实现 |
| cn.cycle.chart.api.feature | `FieldDefn` | 待实现 |
| cn.cycle.chart.api.layer | `ChartLayer` | 待实现 |
| cn.cycle.chart.api.layer | `LayerManager` | 待实现 |
| cn.cycle.chart.api.cache | `TileCache` | 待实现 |
| cn.cycle.chart.api.cache | `TileKey` | 待实现 |
| cn.cycle.chart.api.symbology | `DrawStyle` | 待实现 |
| cn.cycle.chart.api.symbology | `Symbolizer` | 待实现 |
| cn.cycle.chart.api.symbology | `Filter` | 待实现 |
| cn.cycle.chart.api.symbology | `ComparisonFilter` | 待实现 |
| cn.cycle.chart.api.symbology | `SymbolizerRule` | 待实现 |
| cn.cycle.chart.api.util | `Color` | 待实现 |
| cn.cycle.chart.api.util | `Logger` | 待实现 |
| cn.cycle.chart.api.util | `PerformanceMonitor` | 待实现 |
| cn.cycle.chart.api.util | `PerformanceStats` | 待实现 |
| cn.cycle.chart.api.navi | `Route` | 待实现 |
| cn.cycle.chart.api.navi | `Waypoint` | 待实现 |
| cn.cycle.chart.api.navi | `AisManager` | 待实现 |
| cn.cycle.chart.api.navi | `AisTarget` | 待实现 |
| cn.cycle.chart.api.navi | `NavigationEngine` | 待实现 |
| cn.cycle.chart.api.navi | `NavigationEvent` | 待实现 |
| cn.cycle.chart.api.navi | `PositionManager` | 待实现 |
| cn.cycle.chart.api.navi | `PositionUpdate` | 待实现 |
| cn.cycle.chart.api.navi | `Track` | 待实现 |
| cn.cycle.chart.api.navi | `TrackPoint` | 待实现 |
| cn.cycle.chart.api.navi | `IPositionProvider` | 待实现 |
| cn.cycle.chart.api.navi | `PositionQuality` | 待实现 |
| cn.cycle.chart.api.navi | `NmeaParser` | 待实现 |
| cn.cycle.chart.api.navi | `CollisionAssessor` | 待实现 |
| cn.cycle.chart.api.navi | `CpaResult` | 待实现 |
| cn.cycle.chart.api.navi | `UkcCalculator` | 待实现 |
| cn.cycle.chart.api.navi | `OffCourseDetector` | 待实现 |
| cn.cycle.chart.api.navi | `DeviationResult` | 待实现 |
| cn.cycle.chart.api.navi | `RoutePlanner` | 待实现 |
| cn.cycle.chart.api.navi | `TrackRecorder` | 待实现 |
| cn.cycle.chart.api.graph | `LabelEngine` | 待实现 |
| cn.cycle.chart.api.graph | `LabelInfo` | 待实现 |
| cn.cycle.chart.api.graph | `LODManager` | 待实现 |
| cn.cycle.chart.api.graph | `LODLevel` | 待实现 |
| cn.cycle.chart.api.graph | `HitTest` | 待实现 |
| cn.cycle.chart.api.graph | `TransformManager` | 待实现 |
| cn.cycle.chart.api.graph | `RenderTask` | 待实现 |
| cn.cycle.chart.api.graph | `RenderQueue` | 待实现 |
| cn.cycle.chart.api.parser | `ChartParser` | 待实现 |
| cn.cycle.chart.api.parser | `IncrementalParser` | 待实现 |
| cn.cycle.chart.api.security | `InputValidator` | 待实现 |
| cn.cycle.chart.api.security | `SecureLibraryLoader` | 待实现 |
| cn.cycle.chart.api.security | `SensitiveDataProtector` | 待实现 |
| cn.cycle.chart.api.optimize | `RenderOptimizer` | 待实现 |
| cn.cycle.chart.api.optimize | `RenderStats` | 待实现 |
| cn.cycle.chart.api.optimize | `TileRenderPriority` | 待实现 |
| cn.cycle.chart.adapter.canvas | `ChartCanvas` | 待实现 |
| cn.cycle.chart.adapter.canvas | `JavaFXImageDevice` | 待实现 |
| cn.cycle.chart.adapter.event | `JavaFXEventHandler` | 待实现 |
| cn.cycle.chart.adapter.binding | `JavaFXThreadBridge` | 待实现 |
| cn.cycle.chart.adapter.binding | `PropertyBinder` | 待实现 |
| cn.cycle.chart.api.alert | `AlertEngine` | 待实现 |
| cn.cycle.chart.api.alert | `Alert` | 待实现 |
| cn.cycle.chart.api.alert | `AlertChecker` | 待实现 |
| cn.cycle.chart.api.alert | `DepthAlertChecker` | 待实现 |
| cn.cycle.chart.api.alert | `CollisionAlertChecker` | 待实现 |
| cn.cycle.chart.api.alert | `AlertEvent` | 待实现 |
| cn.cycle.chart.api.alert | `EngineConfig` | 待实现 |
| cn.cycle.chart.api.alert | `AlertStatistics` | 待实现 |
| cn.cycle.chart.api.proj | `CoordinateTransformer` | 待实现 |
| cn.cycle.chart.api.proj | `TransformMatrix` | 待实现 |
| cn.cycle.chart.api.exception | `ChartException` | 待实现 |
| cn.cycle.chart.api.exception | `JniException` | 待实现 |
| cn.cycle.chart.api.callback | `RenderCallback` | 待实现 |
| cn.cycle.chart.api.callback | `LoadCallback` | 待实现 |
| cn.cycle.chart.api.callback | `TouchCallback` | 待实现 |
| cn.cycle.chart.api.lifecycle | `AppLifecycleManager` | 待实现 |
| cn.cycle.chart.api.lifecycle | `ThreadSafeRenderState` | 待实现 |
| cn.cycle.chart.api.recovery | `ErrorRecoveryManager` | 待实现 |
| cn.cycle.chart.api.recovery | `CircuitBreaker` | 待实现 |
| cn.cycle.chart.api.recovery | `GracefulDegradation` | 待实现 |
| cn.cycle.chart.api.monitor | `PerformanceMonitor` | 待实现 |
| cn.cycle.chart.api.monitor | `MetricsCollector` | 待实现 |
| cn.cycle.chart.api.monitor | `PerformanceReport` | 待实现 |
| cn.cycle.chart.api.health | `HealthCheck` | 待实现 |
| cn.cycle.chart.api.health | `HealthStatus` | 待实现 |
| cn.cycle.chart.api.health | `ChartApplicationHealthCheck` | 待实现 |
| cn.cycle.chart.api.health | `MetricsExporter` | 待实现 |
| cn.cycle.chart.api.loader | `LibraryLoader` | 待实现 |
| cn.cycle.chart.api.loader | `SecureLibraryLoader` | 待实现 |
| cn.cycle.chart.api.draw | `DrawEngine` | 待实现 |
| cn.cycle.chart.api.draw | `DrawDevice` | 待实现 |
| cn.cycle.chart.api.draw | `DrawContext` | 待实现 |
| cn.cycle.chart.api.draw | `Pen` | 待实现 |
| cn.cycle.chart.api.draw | `Brush` | 待实现 |
| cn.cycle.chart.api.draw | `Font` | 待实现 |
| cn.cycle.chart.api.draw | `Image` | 待实现 |
| cn.cycle.chart.api.layer | `RasterLayer` | 待实现 |
| cn.cycle.chart.api.layer | `MemoryLayer` | 待实现 |
| cn.cycle.chart.api.layer | `LayerGroup` | 待实现 |
| cn.cycle.chart.api.layer | `DataSource` | 待实现 |
| cn.cycle.chart.api.layer | `DriverManager` | 待实现 |
| cn.cycle.chart.api.cache | `MemoryTileCache` | 待实现 |
| cn.cycle.chart.api.cache | `DiskTileCache` | 待实现 |
| cn.cycle.chart.api.cache | `MultiLevelTileCache` | 待实现 |
| cn.cycle.chart.api.cache | `OfflineStorageManager` | 待实现 |
| cn.cycle.chart.api.cache | `OfflineRegion` | 待实现 |
| cn.cycle.chart.api.cache | `DataEncryption` | 待实现 |
| cn.cycle.chart.api.plugin | `ChartPlugin` | 待实现 |
| cn.cycle.chart.api.plugin | `LayerProvider` | 待实现 |
| cn.cycle.chart.api.plugin | `SymbolProvider` | 待实现 |
| cn.cycle.chart.api.plugin | `ParserProvider` | 待实现 |
| cn.cycle.chart.api.plugin | `PluginManager` | 待实现 |
| cn.cycle.chart.api.plugin | `PluginContext` | 待实现 |
| cn.cycle.chart.api.plugin | `LayerConfig` | 待实现 |
| cn.cycle.chart.app.util | `I18nManager` | 待实现 |
| cn.cycle.chart.app.util | `AccessibilityHelper` | 待实现 |
| cn.cycle.chart.app.util | `MetricsExporter` | 待实现 |

### 8.3 JavaFX适配层实现清单

| 包 | 类 | 状态 |
|----|----|----|
| cn.cycle.chart.adapter.canvas | `CanvasAdapter` | 待实现 |
| cn.cycle.chart.adapter.canvas | `ImageDevice` | 待实现 |
| cn.cycle.chart.adapter.event | `ChartEventHandler` | 待实现 |
| cn.cycle.chart.adapter.event | `PanHandler` | 待实现 |
| cn.cycle.chart.adapter.event | `ZoomHandler` | 待实现 |
| cn.cycle.chart.adapter.binding | `ThreadBridge` | 待实现 |
| cn.cycle.chart.adapter.binding | `PropertyBinder` | 待实现 |

### 8.4 JavaFX应用层实现清单

| 包 | 类 | 状态 |
|----|----|----|
| cn.cycle.chart.app.view | `MainView` | 待实现 |
| cn.cycle.chart.app.view | `ChartCanvas` | 待实现 |
| cn.cycle.chart.app.view | `LayerPanel` | 待实现 |
| cn.cycle.chart.app.view | `PositionView` | 待实现 |
| cn.cycle.chart.app.view | `AlertPanel` | 待实现 |
| cn.cycle.chart.app.view | `RoutePanel` | 待实现 |
| cn.cycle.chart.app.view | `AisPanel` | 待实现 |
| cn.cycle.chart.app.view | `TrackPanel` | 待实现 |
| cn.cycle.chart.app.controller | `MainController` | 待实现 |
| cn.cycle.chart.app.controller | `ChartController` | 待实现 |
| cn.cycle.chart.app.controller | `NavigationController` | 待实现 |
| cn.cycle.chart.app.util | `I18nManager` | 待实现 |
| cn.cycle.chart.app.util | `AccessibilityHelper` | 待实现 |
| cn.cycle.chart.app.util | `MetricsExporter` | 待实现 |

### 8.5 统计汇总

| 层次 | Java类数 | JNI方法数 | 预估工时 |
|------|----------|-----------|----------|
| JNI桥接层 | 2 | 200+ | 1周 |
| Java API层 | 165 | 1350+ | 5周 |
| JavaFX适配层 | 7 | 40+ | 1周 |
| JavaFX应用层 | 20 | 60+ | 2周 |
| **合计** | **194** | **1650+** | **9周** |

### 8.6 模块分类统计

| 模块 | Java类数 | 主要功能 |
|------|----------|----------|
| core | 5 | 核心配置、视口和渲染上下文管理 |
| geometry | 12 | 几何对象模型（含多几何类型和工厂类） |
| feature | 3 | 要素数据模型和字段定义 |
| layer | 7 | 图层管理（含栅格图层、内存图层、图层组、数据源、驱动管理） |
| cache | 11 | 缓存管理（内存缓存、磁盘缓存、多级缓存、离线存储、数据加密、缓存淘汰策略、缓存预热） |
| symbology | 5 | 符号化、样式和规则 |
| graph | 8 | 渲染核心（标签引擎、LOD管理、点击测试、变换管理、渲染队列） |
| navi | 13 | 导航、AIS、碰撞评估、UKC计算、偏航检测、航线规划、航迹记录 |
| alert | 8 | 警报引擎 |
| proj | 2 | 坐标转换 |
| parser | 2 | 海图解析（S57/S101/S63） |
| security | 3 | 输入验证、安全加载、敏感数据保护 |
| optimize | 6 | 渲染优化、统计、瓦片优先级、内存管理 |
| draw | 7 | 绘图引擎（引擎、设备、上下文、画笔、画刷、字体、图像） |
| javafx | 7 | JavaFX适配（Canvas、事件、图像设备、线程桥接、属性绑定、工具栏、状态栏） |
| lifecycle | 3 | 生命周期管理、线程安全渲染状态、应用状态枚举 |
| recovery | 5 | 错误恢复、熔断器、优雅降级、恢复策略、降级级别枚举 |
| monitor | 3 | 性能监控、指标收集、性能报告 |
| health | 4 | 健康检查、状态枚举、应用健康检查、指标导出 |
| loader | 2 | 库加载器、安全加载器 |
| util | 6 | 工具类（含指标导出、国际化绑定） |
| exception | 5 | 异常定义（含服务不可用、内存恢复、渲染异常） |
| callback | 3 | 回调接口 |
| plugin | 7 | 插件扩展机制 |
| i18n | 2 | 国际化管理、国际化绑定 |
| accessibility | 1 | 无障碍辅助 |
| model | 3 | 数据模型（海图模型、图层模型、设置模型） |

---

## 9. 补充接口定义

### 9.1 JavaFX应用层补充接口

#### 9.1.1 StatusBar.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import cn.cycle.chart.api.core.ChartViewer;

public class StatusBar extends HBox {
    
    private final Label fpsLabel;
    private final Label memoryLabel;
    private final Label coordLabel;
    private final Label scaleLabel;
    
    public StatusBar() {
        this.fpsLabel = new Label("FPS: --");
        this.memoryLabel = new Label("内存: --");
        this.coordLabel = new Label("坐标: --");
        this.scaleLabel = new Label("比例尺: --");
        
        setSpacing(20);
        setStyle("-fx-background-color: #f0f0f0; -fx-padding: 5;");
        
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        
        getChildren().addAll(coordLabel, scaleLabel, spacer, fpsLabel, memoryLabel);
    }
    
    public void updateFps(double fps) {
        fpsLabel.setText(String.format("FPS: %.1f", fps));
    }
    
    public void updateMemory(long used, long max) {
        memoryLabel.setText(String.format("内存: %dMB / %dMB", used / 1024 / 1024, max / 1024 / 1024));
    }
    
    public void updateCoordinate(double x, double y) {
        coordLabel.setText(String.format("坐标: %.6f, %.6f", x, y));
    }
    
    public void updateScale(double scale) {
        scaleLabel.setText(String.format("比例尺: 1:%.0f", scale));
    }
}
```

#### 9.1.2 ToolBar.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.control.*;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.app.controller.MainController;

public class ChartToolBar extends ToolBar {
    
    private final MainController controller;
    
    public ChartToolBar(MainController controller) {
        this.controller = controller;
        setupButtons();
    }
    
    private void setupButtons() {
        Button openBtn = createButton("打开", "open-chart", controller::openChart);
        Button saveBtn = createButton("保存", "save-chart", controller::saveChart);
        
        Separator separator1 = new Separator();
        
        Button zoomInBtn = createButton("+", "zoom-in", controller::zoomIn);
        Button zoomOutBtn = createButton("-", "zoom-out", controller::zoomOut);
        Button resetBtn = createButton("重置", "reset-view", controller::resetView);
        
        Separator separator2 = new Separator();
        
        ToggleButton panBtn = createToggleButton("平移", "pan-mode");
        ToggleButton selectBtn = createToggleButton("选择", "select-mode");
        
        getItems().addAll(openBtn, saveBtn, separator1, 
            zoomInBtn, zoomOutBtn, resetBtn, separator2,
            panBtn, selectBtn);
    }
    
    private Button createButton(String text, String id, Runnable action) {
        Button button = new Button(text);
        button.setId(id);
        button.setOnAction(e -> action.run());
        return button;
    }
    
    private ToggleButton createToggleButton(String text, String id) {
        ToggleButton button = new ToggleButton(text);
        button.setId(id);
        return button;
    }
}
```

#### 9.1.3 SettingsView.java

```java
package cn.cycle.chart.app.view;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import cn.cycle.chart.app.model.SettingsModel;

public class SettingsView extends VBox {
    
    private final SettingsModel model;
    
    public SettingsView(SettingsModel model) {
        this.model = model;
        setupUI();
    }
    
    private void setupUI() {
        setSpacing(10);
        setPadding(new Insets(10));
        
        TitledPane displayPane = createDisplaySettings();
        TitledPane cachePane = createCacheSettings();
        TitledPane performancePane = createPerformanceSettings();
        
        getChildren().addAll(displayPane, cachePane, performancePane);
    }
    
    private TitledPane createDisplaySettings() {
        VBox content = new VBox(10);
        
        CheckBox antialiasingCheck = new CheckBox("启用抗锯齿");
        antialiasingCheck.selectedProperty().bindBidirectional(model.antialiasingProperty());
        
        ComboBox<String> qualityCombo = new ComboBox<>();
        qualityCombo.getItems().addAll("低", "中", "高");
        qualityCombo.valueProperty().bindBidirectional(model.qualityProperty());
        
        content.getChildren().addAll(antialiasingCheck, new Label("渲染质量:"), qualityCombo);
        
        return new TitledPane("显示设置", content);
    }
    
    private TitledPane createCacheSettings() {
        VBox content = new VBox(10);
        
        Slider cacheSizeSlider = new Slider(128, 1024, 512);
        cacheSizeSlider.valueProperty().bindBidirectional(model.cacheSizeProperty());
        
        Label cacheSizeLabel = new Label();
        cacheSizeLabel.textProperty().bind(cacheSizeSlider.valueProperty().asString("%.0f MB"));
        
        Button clearCacheBtn = new Button("清除缓存");
        clearCacheBtn.setOnAction(e -> model.clearCache());
        
        content.getChildren().addAll(new Label("缓存大小:"), cacheSizeSlider, cacheSizeLabel, clearCacheBtn);
        
        return new TitledPane("缓存设置", content);
    }
    
    private TitledPane createPerformanceSettings() {
        VBox content = new VBox(10);
        
        Spinner<Integer> threadSpinner = new Spinner<>(1, 16, 4);
        threadSpinner.getValueFactory().valueProperty().bindBidirectional(model.threadCountProperty());
        
        CheckBox preloadCheck = new CheckBox("启用预加载");
        preloadCheck.selectedProperty().bindBidirectional(model.preloadEnabledProperty());
        
        content.getChildren().addAll(new Label("线程数:"), threadSpinner, preloadCheck);
        
        return new TitledPane("性能设置", content);
    }
}
```

#### 9.1.4 Model类

```java
package cn.cycle.chart.app.model;

import javafx.beans.property.*;

public class ChartModel {
    
    private final StringProperty chartPath = new SimpleStringProperty();
    private final BooleanProperty loaded = new SimpleBooleanProperty(false);
    private final DoubleProperty scale = new SimpleDoubleProperty(1.0);
    private final DoubleProperty centerX = new SimpleDoubleProperty(0);
    private final DoubleProperty centerY = new SimpleDoubleProperty(0);
    
    public StringProperty chartPathProperty() { return chartPath; }
    public BooleanProperty loadedProperty() { return loaded; }
    public DoubleProperty scaleProperty() { return scale; }
    public DoubleProperty centerXProperty() { return centerX; }
    public DoubleProperty centerYProperty() { return centerY; }
}

public class LayerModel {
    
    private final ObservableList<LayerItem> layers = FXCollections.observableArrayList();
    private final ObjectProperty<LayerItem> selectedLayer = new SimpleObjectProperty<>();
    
    public ObservableList<LayerItem> getLayers() { return layers; }
    public ObjectProperty<LayerItem> selectedLayerProperty() { return selectedLayer; }
}

public class SettingsModel {
    
    private final BooleanProperty antialiasing = new SimpleBooleanProperty(true);
    private final StringProperty quality = new SimpleStringProperty("中");
    private final DoubleProperty cacheSize = new SimpleDoubleProperty(512);
    private final IntegerProperty threadCount = new SimpleIntegerProperty(4);
    private final BooleanProperty preloadEnabled = new SimpleBooleanProperty(true);
    
    public BooleanProperty antialiasingProperty() { return antialiasing; }
    public StringProperty qualityProperty() { return quality; }
    public DoubleProperty cacheSizeProperty() { return cacheSize; }
    public IntegerProperty threadCountProperty() { return threadCount; }
    public BooleanProperty preloadEnabledProperty() { return preloadEnabled; }
    
    public void clearCache() {
        // 清除缓存逻辑
    }
}
```

### 9.2 异常与恢复补充接口

#### 9.2.1 RecoveryStrategy.java

```java
package cn.cycle.chart.api.recovery;

import java.util.concurrent.Callable;

public interface RecoveryStrategy {
    
    <T> T recover(Throwable error, Callable<T> originalOperation);
    
    int getMaxRetries();
    
    long getRetryDelayMs();
}
```

#### 9.2.2 异常类

```java
package cn.cycle.chart.api.exception;

public class ServiceUnavailableException extends RuntimeException {
    
    public ServiceUnavailableException(String message) {
        super(message);
    }
    
    public ServiceUnavailableException(String message, Throwable cause) {
        super(message, cause);
    }
}

public class MemoryRecoveryException extends RuntimeException {
    
    public MemoryRecoveryException(String message) {
        super(message);
    }
}

public class RenderException extends RuntimeException {
    
    private final String areaId;
    
    public RenderException(String message, String areaId) {
        super(message);
        this.areaId = areaId;
    }
    
    public String getAreaId() {
        return areaId;
    }
}
```

### 9.3 性能优化补充接口

#### 9.3.1 MemoryManager.java

```java
package cn.cycle.chart.api.optimize;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicLong;

public class MemoryManager {
    
    private static final long MAX_JAVA_HEAP = 256 * 1024 * 1024;
    private static final long MAX_NATIVE_MEMORY = 256 * 1024 * 1024;
    private static final double MEMORY_PRESSURE_THRESHOLD = 0.8;
    
    private final MemoryMonitor monitor = new MemoryMonitor();
    private final List<MemoryPressureListener> listeners = new CopyOnWriteArrayList<>();
    
    public enum MemoryType {
        JAVA_HEAP, NATIVE
    }
    
    public void checkMemoryPressure() {
        long javaUsed = monitor.getJavaHeapUsed();
        long nativeUsed = monitor.getNativeMemoryUsed();
        
        double javaPressure = (double) javaUsed / MAX_JAVA_HEAP;
        double nativePressure = (double) nativeUsed / MAX_NATIVE_MEMORY;
        
        if (javaPressure > MEMORY_PRESSURE_THRESHOLD) {
            notifyMemoryPressure(MemoryType.JAVA_HEAP, javaPressure);
        }
        
        if (nativePressure > MEMORY_PRESSURE_THRESHOLD) {
            notifyMemoryPressure(MemoryType.NATIVE, nativePressure);
        }
    }
    
    public void handleMemoryPressure(MemoryType type, double pressure) {
        if (pressure > 0.9) {
            emergencyRelease(type);
        } else if (pressure > 0.8) {
            aggressiveRelease(type);
        } else {
            normalRelease(type);
        }
    }
    
    private void emergencyRelease(MemoryType type) {
        if (type == MemoryType.JAVA_HEAP) {
            System.gc();
        }
    }
    
    private void aggressiveRelease(MemoryType type) {
        // 积极释放策略
    }
    
    private void normalRelease(MemoryType type) {
        // 正常释放策略
    }
    
    public void addMemoryPressureListener(MemoryPressureListener listener) {
        listeners.add(listener);
    }
    
    private void notifyMemoryPressure(MemoryType type, double pressure) {
        for (MemoryPressureListener listener : listeners) {
            listener.onMemoryPressure(type, pressure);
        }
    }
    
    public static class MemoryMonitor {
        
        public long getJavaHeapUsed() {
            Runtime runtime = Runtime.getRuntime();
            return runtime.totalMemory() - runtime.freeMemory();
        }
        
        public long getNativeMemoryUsed() {
            return nativeMemoryCounter.get();
        }
        
        private static final AtomicLong nativeMemoryCounter = new AtomicLong(0);
        
        public static void allocateNative(long size) {
            nativeMemoryCounter.addAndGet(size);
        }
        
        public static void releaseNative(long size) {
            nativeMemoryCounter.addAndGet(-size);
        }
    }
}

public interface MemoryPressureListener {
    
    void onMemoryPressure(MemoryManager.MemoryType type, double pressure);
}
```

#### 9.3.2 CacheEvictionPolicy.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.api.cache.TileKey;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class CacheEvictionPolicy {
    
    private static final int MAX_CACHE_SIZE = 128 * 1024 * 1024;
    private static final double EVICTION_THRESHOLD = 0.9;
    
    private final LinkedHashMap<TileKey, CacheEntry> cache;
    private final AtomicLong currentSize = new AtomicLong(0);
    
    public CacheEntry get(TileKey key) {
        CacheEntry entry = cache.get(key);
        if (entry != null) {
            entry.lastAccessTime = System.currentTimeMillis();
            entry.accessCount.incrementAndGet();
        }
        return entry;
    }
    
    public void put(TileKey key, CacheEntry entry) {
        if (currentSize.get() > MAX_CACHE_SIZE * EVICTION_THRESHOLD) {
            evict();
        }
        cache.put(key, entry);
        currentSize.addAndGet(entry.size);
    }
    
    private void evict() {
        List<CacheEntry> entries = new ArrayList<>(cache.values());
        entries.sort((a, b) -> Double.compare(
            calculateEvictionScore(a), calculateEvictionScore(b)));
        
        long targetSize = (long) (MAX_CACHE_SIZE * 0.7);
        Iterator<CacheEntry> it = entries.iterator();
        
        while (currentSize.get() > targetSize && it.hasNext()) {
            CacheEntry entry = it.next();
            cache.remove(entry.key);
            currentSize.addAndGet(-entry.size);
        }
    }
    
    private double calculateEvictionScore(CacheEntry entry) {
        long age = System.currentTimeMillis() - entry.lastAccessTime;
        int frequency = entry.accessCount.get();
        double distance = entry.distanceToViewport;
        
        return (age / 1000.0) * (1.0 / (frequency + 1)) * (distance / 1000.0);
    }
}

public class CacheEntry {
    
    final TileKey key;
    final long size;
    final double distanceToViewport;
    volatile long lastAccessTime;
    final AtomicInteger accessCount = new AtomicInteger(0);
    
    public CacheEntry(TileKey key, long size, double distanceToViewport) {
        this.key = key;
        this.size = size;
        this.distanceToViewport = distanceToViewport;
        this.lastAccessTime = System.currentTimeMillis();
    }
}
```

#### 9.3.3 CachePreheater.java

```java
package cn.cycle.chart.api.cache;

import cn.cycle.chart.api.core.Viewport;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;

public class CachePreheater {
    
    private final ExecutorService preloadExecutor;
    
    public CachePreheater(ExecutorService preloadExecutor) {
        this.preloadExecutor = preloadExecutor;
    }
    
    public void preheat(Viewport current, Viewport predicted) {
        List<TileKey> tiles = calculatePreheatTiles(current, predicted);
        
        CompletableFuture<?>[] futures = tiles.stream()
            .map(tile -> CompletableFuture.runAsync(() -> {
                loadTile(tile);
            }, preloadExecutor))
            .toArray(CompletableFuture[]::new);
        
        CompletableFuture.allOf(futures).join();
    }
    
    private List<TileKey> calculatePreheatTiles(Viewport current, Viewport predicted) {
        List<TileKey> tiles = new ArrayList<>();
        
        if (predicted.getZoomLevel() > current.getZoomLevel()) {
            tiles.addAll(getZoomInTiles(current));
        } else if (predicted.getZoomLevel() < current.getZoomLevel()) {
            tiles.addAll(getZoomOutTiles(current));
        }
        
        tiles.addAll(getPanTiles(current, predicted));
        
        return tiles;
    }
    
    private void loadTile(TileKey tile) {
        // 加载瓦片逻辑
    }
    
    private List<TileKey> getZoomInTiles(Viewport viewport) {
        return new ArrayList<>();
    }
    
    private List<TileKey> getZoomOutTiles(Viewport viewport) {
        return new ArrayList<>();
    }
    
    private List<TileKey> getPanTiles(Viewport current, Viewport predicted) {
        return new ArrayList<>();
    }
}
```

### 9.4 国际化补充接口

#### 9.4.1 I18nBindings.java

```java
package cn.cycle.chart.app.util;

import javafx.beans.binding.Bindings;
import javafx.beans.binding.StringBinding;
import javafx.scene.control.Labeled;
import javafx.stage.Stage;

public class I18nBindings {
    
    public static StringBinding createStringBinding(String key) {
        return Bindings.createStringBinding(
            () -> I18nManager.getString(key),
            I18nManager.localeProperty()
        );
    }
    
    public static StringBinding createStringBinding(String key, Object... args) {
        return Bindings.createStringBinding(
            () -> I18nManager.getString(key, args),
            I18nManager.localeProperty()
        );
    }
    
    public static void bindText(Labeled labeled, String key) {
        labeled.textProperty().bind(createStringBinding(key));
    }
    
    public static void bindTitle(Stage stage, String key) {
        stage.titleProperty().bind(createStringBinding(key));
    }
}
```

### 9.5 枚举补充定义

#### 9.5.1 DegradationLevel.java

```java
package cn.cycle.chart.api.recovery;

public enum DegradationLevel {
    FULL,           // 完整功能
    REDUCED_CACHE,  // 减少缓存
    LOW_QUALITY,    // 低质量渲染
    MINIMAL         // 最小功能
}
```

#### 9.5.2 AppState.java

```java
package cn.cycle.chart.api.lifecycle;

public enum AppState {
    CREATED,    // 应用创建，资源未初始化
    READY,      // 初始化完成，等待加载
    LOADED,     // 海图已加载，可渲染
    PAUSED,     // 应用暂停，释放部分资源
    DISPOSED    // 应用销毁，释放所有资源
}
```

---

## 版本信息

| 属性 | 值 |
|------|-----|
| 文档版本 | v1.5 |
| 生成日期 | 2026-04-08 |
| 更新日期 | 2026-04-08 |
| Java版本 | 17 LTS |
| JavaFX版本 | 21 LTS |
| JNI版本 | 1.8 |
| 迭代次数 | 8轮检查完善 |

### 更新记录

| 版本 | 日期 | 更新内容 |
|------|------|----------|
| v1.0 | 2026-04-08 | 初始版本，包含核心接口定义 |
| v1.1 | 2026-04-08 | 新增渲染核心接口（LabelEngine、LODManager、HitTest等）、导航扩展接口（IPositionProvider、NmeaParser、CollisionAssessor等）、海图解析接口、安全验证接口、渲染优化接口、JavaFX适配层接口 |
| v1.2 | 2026-04-08 | 新增生命周期管理接口（AppLifecycleManager、ThreadSafeRenderState）、错误恢复与容错接口（ErrorRecoveryManager、CircuitBreaker、GracefulDegradation）、性能监控接口（PerformanceMonitor、MetricsCollector、PerformanceReport）、健康检查与监控接口（HealthCheck、HealthStatus、ChartApplicationHealthCheck、MetricsExporter）、库加载器接口（LibraryLoader、SecureLibraryLoader） |
| v1.3 | 2026-04-08 | 新增绘图引擎接口（DrawEngine、DrawDevice、DrawContext、Pen、Brush、Font、Image）、图层管理扩展接口（RasterLayer、MemoryLayer、LayerGroup、DataSource、DriverManager） |
| v1.4 | 2026-04-08 | 新增缓存管理接口（MemoryTileCache、DiskTileCache、MultiLevelTileCache、OfflineStorageManager、OfflineRegion、DataEncryption） |
| v1.5 | 2026-04-08 | 按设计文档章节全面检查补充：新增JavaFX应用层接口（StatusBar、ChartToolBar、SettingsView、Model类）、异常与恢复接口（RecoveryStrategy、ServiceUnavailableException、MemoryRecoveryException、RenderException）、性能优化接口（MemoryManager、MemoryMonitor、MemoryPressureListener、CacheEvictionPolicy、CacheEntry、CachePreheater）、国际化接口（I18nBindings）、枚举定义（DegradationLevel、AppState） |

---

**相关文档**: [javafx_chart_application_design.md](javafx_chart_application_design.md) | [index_all.md](../index_all.md)