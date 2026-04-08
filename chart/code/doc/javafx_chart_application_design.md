# JavaFX海图显示应用设计文档

**文档编号**: CDS-JAVAFX-DESIGN-001  
**版本**: v1.0  
**日期**: 2026-04-08  
**状态**: 正式版  
**需求文档**: [chart_display_system.md](../doc/yangzt/chart_display_system.md)  
**实现分析**: [how_to_realise_cds.md](../chart/doc/how_to_realise_cds.md)  
**模块索引**: [index_all.md](../index_all.md)

---

## 文档修订历史

| 版本 | 日期 | 作者 | 修订内容 |
|------|------|------|----------|
| v1.0 | 2026-04-08 | Team | 初始版本 |

---

## 目录

1. [概述](#1-概述)
2. [已有模块资源](#2-已有模块资源)
3. [JNI桥接层设计](#3-jni桥接层设计)
4. [Java API层设计](#4-java-api层设计)
5. [JavaFX适配层设计](#5-javafx适配层设计)
6. [JavaFX应用层设计](#6-javafx应用层设计)
7. [接口封装最佳实践](#7-接口封装最佳实践)
8. [性能优化设计](#8-性能优化设计)
9. [测试设计](#9-测试设计)
10. [附录](#10-附录)

---

## 1. 概述

### 1.1 文档目的

本文档详细描述JavaFX海图显示应用的设计方案，重点阐述如何在已有C++核心模块的基础上，通过JNI桥接和JavaFX适配层，构建高质量的桌面端海图显示应用。

### 1.2 设计原则

| 原则 | 说明 |
|------|------|
| **复用优先** | 充分利用已有C++核心模块，不重复实现 |
| **分层清晰** | JNI层、Java API层、适配层、应用层职责明确 |
| **高效封装** | 最小化JNI调用开销，批量数据传输 |
| **异常安全** | 完善的异常传递和资源管理机制 |
| **可测试性** | 各层可独立测试，支持Mock |

### 1.3 技术选型

| 技术领域 | 选型 | 版本要求 |
|----------|------|----------|
| Java运行时 | Java LTS | 17+ |
| UI框架 | JavaFX | 21 LTS |
| 构建工具 | Maven | 3.8+ |
| JNI机制 | JNI | Java 17 |
| 渲染方式 | Canvas | JavaFX Canvas |
| 测试框架 | JUnit 5 | 5.9+ |

### 1.4 系统架构总览

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           JavaFX应用架构                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX应用层 (Application Layer)                  │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainView      │  │ ChartCanvas   │  │ SettingsView  │           │   │
│  │  │ 主界面        │  │ 海图画布      │  │ 设置界面      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ LayerPanel    │  │ PositionView  │  │ AlertPanel    │           │   │
│  │  │ 图层面板      │  │ 船位显示      │  │ 警报面板      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX适配层 (Adapter Layer)                      │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │CanvasAdapter  │  │EventHandler   │  │ ImageDevice   │           │   │
│  │  │ Canvas适配    │  │ 事件处理      │  │ 图像设备      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐                              │   │
│  │  │ PropertyBind  │  │ ThreadBridge  │                              │   │
│  │  │ 属性绑定      │  │ 线程桥接      │                              │   │
│  │  └───────────────┘  └───────────────┘                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java API层 (java_api模块)                         │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartViewer   │  │ ChartConfig   │  │ FeatureInfo   │           │   │
│  │  │ 海图查看器    │  │ 配置管理      │  │ 要素信息      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ Geometry      │  │ ChartLayer    │  │ Viewport      │           │   │
│  │  │ 几何对象      │  │ 图层管理      │  │ 视口管理      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ JNI                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JNI桥接层 (jni_bridge模块)                        │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ JniEnvManager │  │ JniConverter  │  │ JniException  │           │   │
│  │  │ 环境管理      │  │ 数据转换      │  │ 异常处理      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐                              │   │
│  │  │ JniMemory     │  │ JniReference  │                              │   │
│  │  │ 内存管理      │  │ 引用管理      │                              │   │
│  │  └───────────────┘  └───────────────┘                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (已有模块)                              │   │
│  │  ogc_graph | ogc_draw | ogc_layer | ogc_symbology | ogc_cache      │   │
│  │  ogc_feature | ogc_geom | ogc_proj | ogc_database | ogc_base       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. 已有模块资源

### 2.1 核心模块清单

基于 [index_all.md](../index_all.md)，已有核心模块如下：

| 模块 | 核心类 | 功能说明 | 索引文件 |
|------|--------|----------|----------|
| **ogc_graph** | `DrawFacade`, `RenderTask`, `LayerManager` | 地图渲染门面 | [index_graph.md](../graph/include/index_graph.md) |
| **ogc_draw** | `DrawEngine`, `DrawDevice`, `DrawContext` | 绘图引擎 | [index_draw.md](../draw/include/index_draw.md) |
| **ogc_layer** | `CNLayer`, `CNVectorLayer`, `CNDataSource` | 图层管理 | [index_layer.md](../layer/include/index_layer.md) |
| **ogc_symbology** | `Symbolizer`, `Filter`, `SymbolizerRule` | 符号化 | [index_symbology.md](../symbology/include/index_symbology.md) |
| **ogc_cache** | `TileCache`, `TileKey`, `OfflineStorageManager` | 瓦片缓存 | [index_cache.md](../cache/include/index_cache.md) |
| **ogc_feature** | `CNFeature`, `CNFeatureDefn`, `CNFieldValue` | 要素模型 | [index_feature.md](../feature/include/index_feature.md) |
| **ogc_geom** | `Geometry`, `Point`, `LineString`, `Polygon` | 几何对象 | [index_geom.md](../geom/include/index_geom.md) |
| **ogc_proj** | `CoordinateTransformer`, `ProjTransformer` | 坐标转换 | [index_proj.md](../proj/include/index_proj.md) |
| **ogc_database** | `DbConnection`, `DbConnectionPool` | 数据库访问 | [index_database.md](../database/include/index_database.md) |
| **ogc_base** | `Logger`, `ThreadSafe<T>`, `PerformanceMonitor` | 基础工具 | [index_base.md](../base/include/index_base.md) |
| **chart_parser** | `ChartParser`, `S57Parser`, `S101Parser` | 海图解析 | [index_parser.md](../chart/parser/include/index_parser.md) |

### 2.2 核心数据结构（不重复实现）

以下数据结构已在C++核心模块中实现，Java层仅提供轻量级包装：

| 数据结构 | 所属模块 | Java包装类 | 说明 |
|----------|----------|------------|------|
| `Geometry` | ogc_geom | `Geometry.java` | 几何对象，通过指针传递 |
| `CNFeature` | ogc_feature | `FeatureInfo.java` | 要素信息，按需获取属性 |
| `TileKey` | ogc_cache | `TileKey.java` | 瓦片键，简单值对象 |
| `Envelope` | ogc_geom | `Envelope.java` | 包络矩形，简单值对象 |
| `Coordinate` | ogc_geom | `Coordinate.java` | 坐标点，简单值对象 |

### 2.3 关键接口映射

| C++接口 | Java API | 调用方式 |
|---------|----------|----------|
| `DrawFacade::Initialize()` | `ChartViewer.initialize()` | JNI直接调用 |
| `DrawFacade::LoadChart()` | `ChartViewer.loadChart()` | JNI直接调用 |
| `DrawFacade::Render()` | `ChartViewer.render()` | JNI + Canvas回调 |
| `DrawFacade::QueryFeature()` | `ChartViewer.queryFeature()` | JNI返回对象 |
| `LayerManager::SetLayerVisible()` | `ChartLayer.setVisible()` | JNI直接调用 |

---

## 3. JNI桥接层设计

### 3.1 模块结构

```
jni_bridge/
├── include/jni_bridge/
│   ├── export.h           # 导出宏定义
│   ├── jni_env.h          # JNI环境管理
│   ├── jni_convert.h      # 数据类型转换
│   ├── jni_exception.h    # 异常处理
│   └── jni_memory.h       # 内存管理
└── src/
    ├── jni_env.cpp
    ├── jni_convert.cpp
    ├── jni_exception.cpp
    └── jni_memory.cpp
```

### 3.2 JniEnvManager设计

```cpp
namespace ogc::jni {

class OGC_JNI_BRIDGE_API JniEnvManager {
public:
    static JniEnvManager* GetInstance();
    
    void Initialize(JavaVM* vm);
    void Shutdown();
    
    JNIEnv* GetEnv();
    JNIEnv* AttachCurrentThread();
    void DetachCurrentThread();
    
    bool IsInitialized() const;
    JavaVM* GetJavaVM() const;
    
    void RegisterNatives(const std::string& className,
                        const JNINativeMethod* methods,
                        int numMethods);
    
    jclass FindClass(const std::string& className);
    jclass GetGlobalClassRef(const std::string& className);
    void ReleaseGlobalClassRef(jclass clazz);
    
    jmethodID GetMethodID(jclass clazz, const std::string& name, 
                          const std::string& signature);
    
    jobject NewGlobalRef(jobject obj);
    void DeleteGlobalRef(jobject obj);
    
    template<typename Func>
    void WithEnv(Func&& func) {
        JNIEnv* env = AttachCurrentThread();
        if (env) {
            func(env);
        }
    }

private:
    JniEnvManager();
    ~JniEnvManager();
    
    JavaVM* m_javaVM;
    std::mutex m_mutex;
    bool m_initialized;
};

}
```

### 3.3 JniConverter设计

```cpp
namespace ogc::jni {

class OGC_JNI_BRIDGE_API JniConverter {
public:
    // 基础类型转换
    static std::string ToString(JNIEnv* env, jstring str);
    static jstring ToJString(JNIEnv* env, const std::string& str);
    
    // 数组转换
    static std::vector<std::string> ToStringVector(JNIEnv* env, jobjectArray array);
    static jobjectArray ToJStringArray(JNIEnv* env, const std::vector<std::string>& vec);
    
    static std::vector<double> ToDoubleVector(JNIEnv* env, jdoubleArray array);
    static jdoubleArray ToJDoubleArray(JNIEnv* env, const std::vector<double>& vec);
    
    // 指针转换（关键：用于传递C++对象）
    static jlong ToJLongPtr(void* ptr);
    static void* FromJLongPtr(jlong ptr);
    
    // 对象转换
    static jobject ToJObject(JNIEnv* env, void* ptr);
    static void* FromJObject(JNIEnv* env, jobject obj);
    
    // 集合转换
    static jobject CreateArrayList(JNIEnv* env);
    static void ArrayListAdd(JNIEnv* env, jobject list, jobject element);
    
    static jobject CreateHashMap(JNIEnv* env);
    static void HashMapPut(JNIEnv* env, jobject map, jobject key, jobject value);
    
    // 方法调用
    static jobject CallObjectMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
    static void CallVoidMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...);
};

}
```

### 3.4 JniException设计

```cpp
namespace ogc::jni {

class OGC_JNI_BRIDGE_API JniException {
public:
    static void CheckException(JNIEnv* env);
    static void ThrowException(JNIEnv* env, const std::string& className, 
                               const std::string& message);
    static void ThrowRuntimeException(JNIEnv* env, const std::string& message);
    static void ThrowIOException(JNIEnv* env, const std::string& message);
    static void ThrowIllegalArgumentException(JNIEnv* env, const std::string& message);
    
    static bool HasException(JNIEnv* env);
    static void ClearException(JNIEnv* env);
    
    static void TranslateAndThrow(JNIEnv* env, const std::exception& e);
};

}
```

### 3.5 JniMemory设计

```cpp
namespace ogc::jni {

template<typename T>
class JniLocalRef {
public:
    JniLocalRef(JNIEnv* env, T ref) : m_env(env), m_ref(ref) {}
    ~JniLocalRef() { if (m_ref) m_env->DeleteLocalRef(m_ref); }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    T operator->() const { return m_ref; }
    
private:
    JNIEnv* m_env;
    T m_ref;
};

template<typename T>
class JniGlobalRef {
public:
    JniGlobalRef(JNIEnv* env, T ref) : m_env(env) {
        m_ref = m_env->NewGlobalRef(ref);
    }
    ~JniGlobalRef() { if (m_ref) m_env->DeleteGlobalRef(m_ref); }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    
private:
    JNIEnv* m_env;
    T m_ref;
};

}
```

### 3.6 JNI方法注册

```cpp
static JNINativeMethod g_chartViewerMethods[] = {
    {"nativeInitialize", "()J", (void*)Java_ogc_chart_ChartViewer_nativeInitialize},
    {"nativeDispose", "(J)V", (void*)Java_ogc_chart_ChartViewer_nativeDispose},
    {"nativeLoadChart", "(JLjava/lang/String;)Z", (void*)Java_ogc_chart_ChartViewer_nativeLoadChart},
    {"nativeRender", "(JLjava/lang/Object;DD)V", (void*)Java_ogc_chart_ChartViewer_nativeRender},
    {"nativeSetViewport", "(JDDDD)V", (void*)Java_ogc_chart_ChartViewer_nativeSetViewport},
    {"nativePan", "(JDD)V", (void*)Java_ogc_chart_ChartViewer_nativePan},
    {"nativeZoom", "(JDDD)V", (void*)Java_ogc_chart_ChartViewer_nativeZoom},
    {"nativeQueryFeature", "(JDD)Logc/chart/FeatureInfo;", (void*)Java_ogc_chart_ChartViewer_nativeQueryFeature},
};

void RegisterChartViewerNatives(JNIEnv* env) {
    jclass clazz = env->FindClass("ogc/chart/ChartViewer");
    env->RegisterNatives(clazz, g_chartViewerMethods, 
                         sizeof(g_chartViewerMethods) / sizeof(JNINativeMethod));
}
```

---

## 4. Java API层设计

### 4.1 模块结构

```
java_api/
├── src/main/java/ogc/chart/
│   ├── ChartViewer.java      # 主入口类
│   ├── ChartConfig.java      # 配置类
│   ├── ChartView.java        # 视图组件（Android）
│   ├── ChartLayer.java       # 图层管理
│   ├── Geometry.java         # 几何对象
│   ├── FeatureInfo.java      # 要素信息
│   ├── Viewport.java         # 视口管理
│   ├── DisplayMode.java      # 显示模式
│   ├── ChartException.java   # 异常类
│   └── callback/
│       ├── RenderCallback.java
│       ├── LoadCallback.java
│       └── TouchCallback.java
└── pom.xml
```

### 4.2 ChartViewer核心类设计

```java
package ogc.chart;

/**
 * 海图显示系统主入口类
 * 
 * <p>ChartViewer是海图显示系统的核心类，封装了所有海图操作功能。</p>
 * 
 * <h3>设计要点：</h3>
 * <ul>
 *   <li>使用native指针持有C++对象</li>
 *   <li>所有native方法通过指针调用C++实现</li>
 *   <li>提供回调接口处理异步事件</li>
 *   <li>支持try-with-resources自动释放资源</li>
 * </ul>
 */
public class ChartViewer implements AutoCloseable {
    
    static {
        System.loadLibrary("ogc_chart_jni");
    }
    
    private long nativePtr;
    private RenderCallback renderCallback;
    private LoadCallback loadCallback;
    private TouchCallback touchCallback;
    
    public ChartViewer() {
        this.nativePtr = nativeInitialize();
        if (nativePtr == 0) {
            throw new ChartException("Failed to initialize ChartViewer");
        }
    }
    
    // ==================== 生命周期管理 ====================
    
    private native long nativeInitialize();
    private native void nativeDispose(long ptr);
    
    @Override
    public void close() {
        if (nativePtr != 0) {
            nativeDispose(nativePtr);
            nativePtr = 0;
        }
    }
    
    // ==================== 海图操作 ====================
    
    public void loadChart(String filePath) throws ChartException {
        checkDisposed();
        if (!nativeLoadChart(nativePtr, filePath)) {
            throw new ChartException("Failed to load chart: " + filePath);
        }
    }
    
    private native boolean nativeLoadChart(long ptr, String filePath);
    
    // ==================== 渲染操作 ====================
    
    public void render(Object canvas, double width, double height) {
        checkDisposed();
        nativeRender(nativePtr, canvas, width, height);
    }
    
    private native void nativeRender(long ptr, Object canvas, double width, double height);
    
    // ==================== 视口操作 ====================
    
    public void setViewport(double minX, double minY, double maxX, double maxY) {
        checkDisposed();
        nativeSetViewport(nativePtr, minX, minY, maxX, maxY);
    }
    
    private native void nativeSetViewport(long ptr, double minX, double minY, 
                                          double maxX, double maxY);
    
    public void pan(double dx, double dy) {
        checkDisposed();
        nativePan(nativePtr, dx, dy);
    }
    
    private native void nativePan(long ptr, double dx, double dy);
    
    public void zoom(double factor, double centerX, double centerY) {
        checkDisposed();
        nativeZoom(nativePtr, factor, centerX, centerY);
    }
    
    private native void nativeZoom(long ptr, double factor, double centerX, double centerY);
    
    // ==================== 查询操作 ====================
    
    public FeatureInfo queryFeature(double x, double y) {
        checkDisposed();
        return nativeQueryFeature(nativePtr, x, y);
    }
    
    private native FeatureInfo nativeQueryFeature(long ptr, double x, double y);
    
    // ==================== 配置操作 ====================
    
    public void setConfig(ChartConfig config) {
        checkDisposed();
        nativeSetConfig(nativePtr, config);
    }
    
    private native void nativeSetConfig(long ptr, ChartConfig config);
    
    // ==================== 回调设置 ====================
    
    public void setRenderCallback(RenderCallback callback) {
        this.renderCallback = callback;
    }
    
    public void setLoadCallback(LoadCallback callback) {
        this.loadCallback = callback;
    }
    
    public void setTouchCallback(TouchCallback callback) {
        this.touchCallback = callback;
    }
    
    // ==================== 工具方法 ====================
    
    private void checkDisposed() {
        if (nativePtr == 0) {
            throw new IllegalStateException("ChartViewer has been disposed");
        }
    }
    
    // ==================== JNI回调方法 ====================
    
    private void onRenderComplete() {
        if (renderCallback != null) {
            renderCallback.onRenderComplete();
        }
    }
    
    private void onLoadSuccess(String chartId) {
        if (loadCallback != null) {
            loadCallback.onLoadSuccess(chartId);
        }
    }
    
    private void onLoadError(String error) {
        if (loadCallback != null) {
            loadCallback.onLoadError(error);
        }
    }
}
```

### 4.3 ChartConfig配置类设计

```java
package ogc.chart;

/**
 * 海图配置类
 * 
 * <p>使用Builder模式构建不可变配置对象。</p>
 */
public class ChartConfig {
    
    private final String cachePath;
    private final long maxCacheSize;
    private final int threadCount;
    private final boolean enableAntiAliasing;
    private final boolean enableHighQuality;
    private final DisplayMode displayMode;
    private final double pixelRatio;
    
    private ChartConfig(Builder builder) {
        this.cachePath = builder.cachePath;
        this.maxCacheSize = builder.maxCacheSize;
        this.threadCount = builder.threadCount;
        this.enableAntiAliasing = builder.enableAntiAliasing;
        this.enableHighQuality = builder.enableHighQuality;
        this.displayMode = builder.displayMode;
        this.pixelRatio = builder.pixelRatio;
    }
    
    // Getters...
    
    public static class Builder {
        private String cachePath = System.getProperty("java.io.tmpdir") + "/chart_cache";
        private long maxCacheSize = 256 * 1024 * 1024; // 256MB
        private int threadCount = Runtime.getRuntime().availableProcessors();
        private boolean enableAntiAliasing = true;
        private boolean enableHighQuality = true;
        private DisplayMode displayMode = DisplayMode.DAY;
        private double pixelRatio = 1.0;
        
        public Builder setCachePath(String cachePath) {
            this.cachePath = cachePath;
            return this;
        }
        
        public Builder setMaxCacheSize(long maxCacheSize) {
            this.maxCacheSize = maxCacheSize;
            return this;
        }
        
        public Builder setThreadCount(int threadCount) {
            this.threadCount = threadCount;
            return this;
        }
        
        public Builder setEnableAntiAliasing(boolean enableAntiAliasing) {
            this.enableAntiAliasing = enableAntiAliasing;
            return this;
        }
        
        public Builder setEnableHighQuality(boolean enableHighQuality) {
            this.enableHighQuality = enableHighQuality;
            return this;
        }
        
        public Builder setDisplayMode(DisplayMode displayMode) {
            this.displayMode = displayMode;
            return this;
        }
        
        public Builder setPixelRatio(double pixelRatio) {
            this.pixelRatio = pixelRatio;
            return this;
        }
        
        public ChartConfig build() {
            return new ChartConfig(this);
        }
    }
}
```

### 4.4 Geometry几何类设计

```java
package ogc.chart;

/**
 * 几何对象类
 * 
 * <p>轻量级包装类，通过native指针引用C++几何对象。</p>
 * <p>不存储实际坐标数据，仅在需要时从C++层获取。</p>
 */
public class Geometry {
    
    public enum Type {
        POINT,
        LINESTRING,
        POLYGON,
        MULTIPOINT,
        MULTILINESTRING,
        MULTIPOLYGON,
        GEOMETRYCOLLECTION
    }
    
    private final long nativePtr;
    private Type type;
    
    Geometry(long nativePtr) {
        this.nativePtr = nativePtr;
        this.type = Type.values()[nativeGetType(nativePtr)];
    }
    
    public Type getType() {
        return type;
    }
    
    public double[] getCoordinates() {
        return nativeGetCoordinates(nativePtr);
    }
    
    public Envelope getEnvelope() {
        double[] bounds = nativeGetEnvelope(nativePtr);
        return new Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
    }
    
    public String asWKT() {
        return nativeAsWKT(nativePtr);
    }
    
    // Native methods
    private static native int nativeGetType(long ptr);
    private static native double[] nativeGetCoordinates(long ptr);
    private static native double[] nativeGetEnvelope(long ptr);
    private static native String nativeAsWKT(long ptr);
}
```

### 4.5 FeatureInfo要素信息类设计

```java
package ogc.chart;

import java.util.Map;
import java.util.HashMap;

/**
 * 要素信息类
 * 
 * <p>封装要素的属性信息，按需从C++层获取。</p>
 */
public class FeatureInfo {
    
    private final long nativePtr;
    private final String featureId;
    private final String featureType;
    private Map<String, Object> attributes;
    private Geometry geometry;
    
    FeatureInfo(long nativePtr, String featureId, String featureType) {
        this.nativePtr = nativePtr;
        this.featureId = featureId;
        this.featureType = featureType;
    }
    
    public String getFeatureId() {
        return featureId;
    }
    
    public String getFeatureType() {
        return featureType;
    }
    
    public Map<String, Object> getAttributes() {
        if (attributes == null) {
            attributes = nativeGetAttributes(nativePtr);
        }
        return attributes;
    }
    
    public Object getAttribute(String name) {
        return getAttributes().get(name);
    }
    
    public Geometry getGeometry() {
        if (geometry == null) {
            long geomPtr = nativeGetGeometry(nativePtr);
            if (geomPtr != 0) {
                geometry = new Geometry(geomPtr);
            }
        }
        return geometry;
    }
    
    // Native methods
    private static native Map<String, Object> nativeGetAttributes(long ptr);
    private static native long nativeGetGeometry(long ptr);
}
```

---

## 5. JavaFX适配层设计

### 5.1 模块结构

```
javafx_adapter/
├── src/main/java/ogc/chart/javafx/
│   ├── ChartCanvas.java         # Canvas渲染组件
│   ├── CanvasAdapter.java       # Canvas适配器
│   ├── JavaFXEventHandler.java  # 事件处理器
│   ├── JavaFXImageDevice.java   # 图像设备
│   ├── JavaFXThreadBridge.java  # 线程桥接
│   └── PropertyBinder.java      # 属性绑定器
└── pom.xml
```

### 5.2 ChartCanvas设计

```java
package ogc.chart.javafx;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.animation.AnimationTimer;
import javafx.beans.property.*;
import javafx.scene.input.*;

/**
 * JavaFX Canvas海图渲染组件
 * 
 * <p>基于JavaFX Canvas的海图显示组件，封装渲染循环和事件处理。</p>
 */
public class ChartCanvas extends Canvas {
    
    private final ChartViewer viewer;
    private final AnimationTimer renderTimer;
    private final JavaFXEventHandler eventHandler;
    
    // 属性绑定
    private final DoubleProperty centerX = new SimpleDoubleProperty();
    private final DoubleProperty centerY = new SimpleDoubleProperty();
    private final DoubleProperty zoomLevel = new SimpleDoubleProperty(1.0);
    private final BooleanProperty renderEnabled = new SimpleBooleanProperty(true);
    
    public ChartCanvas(ChartViewer viewer) {
        this.viewer = viewer;
        this.eventHandler = new JavaFXEventHandler(viewer);
        
        // 渲染循环
        renderTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (renderEnabled.get()) {
                    render();
                }
            }
        };
        
        // 尺寸绑定
        widthProperty().addListener(obs -> requestRender());
        heightProperty().addListener(obs -> requestRender());
        
        // 事件绑定
        setupEventHandlers();
        
        renderTimer.start();
    }
    
    private void render() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, getWidth(), getHeight());
        
        // 调用Native渲染
        viewer.render(gc, getWidth(), getHeight());
    }
    
    public void requestRender() {
        // 标记需要重绘
    }
    
    private void setupEventHandlers() {
        // 鼠标事件
        setOnMousePressed(eventHandler::onMousePressed);
        setOnMouseReleased(eventHandler::onMouseReleased);
        setOnMouseDragged(eventHandler::onMouseDragged);
        setOnMouseClicked(eventHandler::onMouseClicked);
        setOnScroll(eventHandler::onScroll);
        
        // 键盘事件
        setOnKeyPressed(eventHandler::onKeyPressed);
        setOnKeyReleased(eventHandler::onKeyReleased);
    }
    
    // 属性访问器
    public DoubleProperty centerXProperty() { return centerX; }
    public DoubleProperty centerYProperty() { return centerY; }
    public DoubleProperty zoomLevelProperty() { return zoomLevel; }
    public BooleanProperty renderEnabledProperty() { return renderEnabled; }
    
    public void dispose() {
        renderTimer.stop();
    }
}
```

### 5.3 CanvasAdapter设计

```cpp
namespace ogc::javafx {

class JavaFXCanvasAdapter : public ogc::draw::DrawDevice {
public:
    JavaFXCanvasAdapter(jobject canvas, int width, int height);
    ~JavaFXCanvasAdapter() override;
    
    // DrawDevice接口实现
    void BeginDraw() override;
    void EndDraw() override;
    void Flush() override;
    
    void DrawLine(const std::vector<Point>& points, const DrawStyle& style) override;
    void DrawPolygon(const std::vector<Point>& points, const DrawStyle& style) override;
    void DrawText(const std::string& text, const Point& pos, const TextStyle& style) override;
    void DrawImage(const Image* img, const Envelope& bounds) override;
    
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    
private:
    void EnsureGraphicsContext();
    void ApplyStyle(const DrawStyle& style);
    void ApplyTextStyle(const TextStyle& style);
    
    jobject m_canvas;
    jobject m_gc;
    JNIEnv* m_env;
    int m_width, m_height;
    
    jclass m_gcClass;
    jmethodID m_setStrokeMethod;
    jmethodID m_setFillMethod;
    jmethodID m_setLineWidthMethod;
    jmethodID m_strokeLineMethod;
    jmethodID m_fillRectMethod;
    jmethodID m_fillTextMethod;
};

}
```

### 5.4 JavaFXEventHandler设计

```java
package ogc.chart.javafx;

import javafx.scene.input.*;
import javafx.geometry.Point2D;

/**
 * JavaFX事件处理器
 * 
 * <p>处理鼠标、键盘、滚轮等输入事件，转换为海图操作。</p>
 */
public class JavaFXEventHandler {
    
    private final ChartViewer viewer;
    private Point2D lastMousePos;
    private boolean isDragging = false;
    
    public JavaFXEventHandler(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    public void onMousePressed(MouseEvent event) {
        lastMousePos = new Point2D(event.getX(), event.getY());
        isDragging = false;
    }
    
    public void onMouseReleased(MouseEvent event) {
        if (!isDragging) {
            // 单击查询
            FeatureInfo feature = viewer.queryFeature(event.getX(), event.getY());
            if (feature != null) {
                showFeatureInfo(feature);
            }
        }
        isDragging = false;
    }
    
    public void onMouseDragged(MouseEvent event) {
        if (lastMousePos != null) {
            double dx = event.getX() - lastMousePos.getX();
            double dy = event.getY() - lastMousePos.getY();
            
            if (Math.abs(dx) > 2 || Math.abs(dy) > 2) {
                isDragging = true;
                viewer.pan(-dx, -dy);
            }
            
            lastMousePos = new Point2D(event.getX(), event.getY());
        }
    }
    
    public void onMouseClicked(MouseEvent event) {
        if (event.getClickCount() == 2) {
            // 双击放大
            viewer.zoom(2.0, event.getX(), event.getY());
        }
    }
    
    public void onScroll(ScrollEvent event) {
        double factor = event.getDeltaY() > 0 ? 1.1 : 0.9;
        viewer.zoom(factor, event.getX(), event.getY());
    }
    
    public void onKeyPressed(KeyEvent event) {
        switch (event.getCode()) {
            case PLUS:
            case EQUALS:
                viewer.zoom(1.2, 0, 0);
                break;
            case MINUS:
                viewer.zoom(0.8, 0, 0);
                break;
            case LEFT:
                viewer.pan(50, 0);
                break;
            case RIGHT:
                viewer.pan(-50, 0);
                break;
            case UP:
                viewer.pan(0, 50);
                break;
            case DOWN:
                viewer.pan(0, -50);
                break;
        }
    }
    
    public void onKeyReleased(KeyEvent event) {
        // 键盘释放处理
    }
    
    private void showFeatureInfo(FeatureInfo feature) {
        // 显示要素信息
    }
}
```

### 5.5 JavaFXThreadBridge设计

```java
package ogc.chart.javafx;

import javafx.application.Platform;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 线程桥接器
 * 
 * <p>处理JavaFX应用线程与后台线程之间的任务调度。</p>
 */
public class JavaFXThreadBridge {
    
    private final ExecutorService backgroundExecutor;
    
    public JavaFXThreadBridge() {
        this.backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors()
        );
    }
    
    /**
     * 在JavaFX应用线程执行
     */
    public void runOnFxThread(Runnable task) {
        if (Platform.isFxApplicationThread()) {
            task.run();
        } else {
            Platform.runLater(task);
        }
    }
    
    /**
     * 在后台线程执行
     */
    public void runInBackground(Runnable task) {
        backgroundExecutor.submit(task);
    }
    
    /**
     * 在后台执行，完成后回调JavaFX线程
     */
    public <T> void runAsync(java.util.concurrent.Callable<T> task, 
                             java.util.function.Consumer<T> callback) {
        backgroundExecutor.submit(() -> {
            try {
                T result = task.call();
                runOnFxThread(() -> callback.accept(result));
            } catch (Exception e) {
                runOnFxThread(() -> {
                    throw new RuntimeException("Background task failed", e);
                });
            }
        });
    }
    
    public void shutdown() {
        backgroundExecutor.shutdown();
    }
}
```

---

## 6. JavaFX应用层设计

### 6.1 模块结构

```
javafx_app/
├── src/main/java/ogc/chart/app/
│   ├── Main.java                # 应用入口
│   ├── MainView.java            # 主界面
│   ├── ChartCanvas.java         # 海图画布
│   ├── controller/
│   │   ├── MainController.java  # 主控制器
│   │   ├── LayerController.java # 图层控制器
│   │   └── SettingsController.java
│   ├── view/
│   │   ├── LayerPanel.java      # 图层面板
│   │   ├── PositionPanel.java   # 船位面板
│   │   ├── AlertPanel.java      # 警报面板
│   │   └── ToolBar.java         # 工具栏
│   ├── model/
│   │   ├── ChartModel.java      # 海图模型
│   │   └── LayerModel.java      # 图层模型
│   └── util/
│       ├── FxmlLoader.java      # FXML加载器
│       └── ResourceUtil.java    # 资源工具
├── src/main/resources/
│   ├── fxml/
│   │   ├── MainView.fxml
│   │   └── SettingsView.fxml
│   └── css/
│       └── chart.css
└── pom.xml
```

### 6.2 MainView主界面设计

```java
package ogc.chart.app;

import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.stage.Stage;

/**
 * 主界面视图
 */
public class MainView extends BorderPane {
    
    private final ChartCanvas chartCanvas;
    private final LayerPanel layerPanel;
    private final PositionPanel positionPanel;
    private final ToolBar toolBar;
    
    public MainView(Stage stage) {
        // 初始化组件
        ChartViewer viewer = new ChartViewer();
        chartCanvas = new ChartCanvas(viewer);
        layerPanel = new LayerPanel(viewer);
        positionPanel = new PositionPanel();
        toolBar = new ToolBar(viewer);
        
        // 布局
        setTop(toolBar);
        setCenter(chartCanvas);
        setRight(createRightPanel());
        setBottom(createStatusBar());
        
        // 绑定Canvas尺寸
        chartCanvas.widthProperty().bind(widthProperty().subtract(250));
        chartCanvas.heightProperty().bind(heightProperty().subtract(80));
    }
    
    private VBox createRightPanel() {
        VBox panel = new VBox(10);
        panel.setPrefWidth(250);
        panel.getChildren().addAll(
            new TitledPane("图层", layerPanel),
            new TitledPane("船位", positionPanel)
        );
        return panel;
    }
    
    private HBox createStatusBar() {
        HBox statusBar = new HBox(10);
        statusBar.getStyleClass().add("status-bar");
        // 添加状态信息
        return statusBar;
    }
}
```

### 6.3 LayerPanel图层面板设计

```java
package ogc.chart.app.view;

import javafx.scene.control.*;
import javafx.collections.*;
import java.util.List;

/**
 * 图层面板
 */
public class LayerPanel extends VBox {
    
    private final ListView<ChartLayer> layerList;
    private final ChartViewer viewer;
    
    public LayerPanel(ChartViewer viewer) {
        this.viewer = viewer;
        
        layerList = new ListView<>();
        layerList.setCellFactory(list -> new LayerCell());
        
        getChildren().addAll(
            createToolBar(),
            layerList
        );
    }
    
    private ToolBar createToolBar() {
        Button addBtn = new Button("添加");
        Button removeBtn = new Button("移除");
        Button upBtn = new Button("上移");
        Button downBtn = new Button("下移");
        
        addBtn.setOnAction(e -> addLayer());
        removeBtn.setOnAction(e -> removeLayer());
        upBtn.setOnAction(e -> moveLayerUp());
        downBtn.setOnAction(e -> moveLayerDown());
        
        return new ToolBar(addBtn, removeBtn, upBtn, downBtn);
    }
    
    private void addLayer() {
        FileChooser chooser = new FileChooser();
        chooser.setTitle("选择海图文件");
        chooser.getExtensionFilters().addAll(
            new FileChooser.ExtensionFilter("S57文件", "*.000"),
            new FileChooser.ExtensionFilter("所有文件", "*.*")
        );
        
        List<File> files = chooser.showOpenMultipleDialog(getScene().getWindow());
        if (files != null) {
            for (File file : files) {
                viewer.loadChart(file.getAbsolutePath());
            }
        }
    }
    
    private void removeLayer() {
        ChartLayer layer = layerList.getSelectionModel().getSelectedItem();
        if (layer != null) {
            layerList.getItems().remove(layer);
        }
    }
    
    private void moveLayerUp() {
        // 上移图层
    }
    
    private void moveLayerDown() {
        // 下移图层
    }
    
    private static class LayerCell extends ListCell<ChartLayer> {
        private final CheckBox visibleCheck = new CheckBox();
        
        @Override
        protected void updateItem(ChartLayer layer, boolean empty) {
            super.updateItem(layer, empty);
            
            if (empty || layer == null) {
                setGraphic(null);
            } else {
                visibleCheck.setSelected(layer.isVisible());
                visibleCheck.setOnAction(e -> layer.setVisible(visibleCheck.isSelected()));
                setGraphic(new HBox(10, visibleCheck, new Label(layer.getName())));
            }
        }
    }
}
```

### 6.4 MVVM架构设计

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           MVVM架构                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    View层 (FXML + JavaFX Controls)                   │   │
│  │  MainView.fxml, LayerPanel.java, PositionPanel.java                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    │ 数据绑定                               │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    ViewModel层 (Presentation Logic)                  │   │
│  │  MainViewModel, ChartViewModel, LayerViewModel                       │   │
│  │  - JavaFX Properties                                                 │   │
│  │  - 命令模式 (Commands)                                               │   │
│  │  - 状态管理                                                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    │ 调用                                   │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Model层 (Business Logic)                          │   │
│  │  ChartModel, LayerModel, PositionModel                               │   │
│  │  - 封装ChartViewer操作                                               │   │
│  │  - 数据转换                                                          │   │
│  │  - 业务规则                                                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    │ JNI调用                                │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Native层 (C++ Core)                               │   │
│  │  ChartViewer → JNI → ogc_graph, ogc_draw, ogc_layer                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 7. 接口封装最佳实践

### 7.1 JNI封装原则

| 原则 | 说明 | 示例 |
|------|------|------|
| **最小化调用** | 减少JNI调用次数，批量传输数据 | 一次获取所有坐标，而非逐个获取 |
| **指针传递** | 用jlong传递C++对象指针 | `private long nativePtr` |
| **延迟加载** | 按需从C++获取数据 | `getAttributes()`首次调用才获取 |
| **异常转换** | C++异常转换为Java异常 | `JniException::TranslateAndThrow()` |
| **资源管理** | 使用AutoCloseable模式 | `implements AutoCloseable` |

### 7.2 高效数据传输

```java
// 错误：多次JNI调用
public double[] getCoordinates() {
    int count = nativeGetCoordinateCount(nativePtr);
    double[] coords = new double[count * 2];
    for (int i = 0; i < count; i++) {
        coords[i * 2] = nativeGetCoordinateX(nativePtr, i);
        coords[i * 2 + 1] = nativeGetCoordinateY(nativePtr, i);
    }
    return coords;
}

// 正确：单次JNI调用
public double[] getCoordinates() {
    return nativeGetCoordinates(nativePtr); // 一次获取所有数据
}
private native double[] nativeGetCoordinates(long ptr);
```

### 7.3 异常处理模式

```cpp
// C++层
JNIEXPORT void JNICALL Java_ogc_chart_ChartViewer_nativeLoadChart(
    JNIEnv* env, jobject obj, jlong ptr, jstring filePath) {
    
    try {
        auto viewer = reinterpret_cast<ChartViewer*>(ptr);
        std::string path = JniConverter::ToString(env, filePath);
        
        if (!viewer->LoadChart(path)) {
            JniException::ThrowIOException(env, "Failed to load chart: " + path);
            return;
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}
```

```java
// Java层
public void loadChart(String filePath) throws ChartException {
    checkDisposed();
    try {
        if (!nativeLoadChart(nativePtr, filePath)) {
            throw new ChartException("Failed to load chart: " + filePath);
        }
    } catch (Exception e) {
        throw new ChartException("Load chart failed", e);
    }
}
```

### 7.4 内存管理模式

```java
// 使用try-with-resources自动释放
try (ChartViewer viewer = new ChartViewer()) {
    viewer.loadChart("/path/to/chart.000");
    viewer.render(canvas, width, height);
} // 自动调用close()

// 或手动管理
ChartViewer viewer = null;
try {
    viewer = new ChartViewer();
    // 使用viewer...
} finally {
    if (viewer != null) {
        viewer.close();
    }
}
```

### 7.5 线程安全模式

```java
// 所有UI操作必须在JavaFX应用线程
Platform.runLater(() -> {
    chartCanvas.requestRender();
});

// 后台任务完成后回调UI线程
threadBridge.runAsync(
    () -> viewer.loadChart(path),  // 后台执行
    (result) -> updateUI()          // UI线程回调
);
```

---

## 8. 性能优化设计

### 8.1 渲染优化

| 优化项 | 实现方式 | 预期效果 |
|--------|----------|----------|
| 帧率控制 | AnimationTimer + 脏区域检测 | 避免过度渲染 |
| 批量绘制 | 合并相同样式图元 | 减少JNI调用 |
| 异步加载 | 后台线程加载数据 | UI不阻塞 |
| 缓存策略 | 瓦片缓存 + 符号缓存 | 减少重复计算 |

### 8.2 内存优化

| 优化项 | 实现方式 | 预期效果 |
|--------|----------|----------|
| 延迟加载 | 按需获取属性数据 | 减少内存占用 |
| 对象池 | 复用Java对象 | 减少GC压力 |
| 弱引用 | 缓存使用弱引用 | 自动回收不常用数据 |
| 批量释放 | 批量释放Native资源 | 减少JNI调用 |

### 8.3 性能指标

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 帧率 | ≥30fps | AnimationTimer统计 |
| 首帧时间 | <500ms | 从加载到首帧 |
| 内存峰值 | <1GB | VisualVM监控 |
| 响应延迟 | <100ms | 事件到渲染 |

---

## 9. 测试设计

### 9.1 测试层次

| 测试层次 | 覆盖范围 | 工具 |
|----------|----------|------|
| JNI单元测试 | 数据转换、异常处理 | GoogleTest |
| Java API单元测试 | Java层逻辑 | JUnit 5 |
| 集成测试 | JNI+Java联合 | JUnit 5 |
| UI测试 | JavaFX界面 | TestFX |
| 性能测试 | 渲染性能 | JMH |

### 9.2 测试用例

```java
class ChartViewerTest {
    
    @Test
    void testLoadChart() {
        try (ChartViewer viewer = new ChartViewer()) {
            assertDoesNotThrow(() -> viewer.loadChart("test.000"));
        }
    }
    
    @Test
    void testQueryFeature() {
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart("test.000");
            FeatureInfo feature = viewer.queryFeature(100, 100);
            assertNotNull(feature);
            assertNotNull(feature.getFeatureId());
        }
    }
    
    @Test
    void testAutoClose() {
        ChartViewer viewer = new ChartViewer();
        viewer.close();
        assertThrows(IllegalStateException.class, () -> viewer.loadChart("test.000"));
    }
}
```

---

## 10. 附录

### 10.1 实施计划

| 阶段 | 任务 | 工时 | 交付物 |
|------|------|------|--------|
| 阶段1 | JNI桥接层完善 | 1周 | jni_bridge模块完整实现 |
| 阶段2 | Java API层开发 | 2周 | java_api模块完整实现 |
| 阶段3 | JavaFX适配层开发 | 2周 | javafx_adapter模块完整实现 |
| 阶段4 | JavaFX应用层开发 | 3周 | javafx_app可运行应用 |
| 阶段5 | 测试与优化 | 2周 | 测试报告、性能优化 |

### 10.2 参考文档

| 文档 | 路径 |
|------|------|
| 需求说明书 | [chart_display_system.md](../doc/yangzt/chart_display_system.md) |
| 实现分析文档 | [how_to_realise_cds.md](../chart/doc/how_to_realise_cds.md) |
| 模块索引 | [index_all.md](../index_all.md) |
| 设计文档 | [chart_display_system_design.md](../chart/doc/chart_display_system_design.md) |

### 10.3 JNI方法签名速查

| Java类型 | JNI签名 |
|----------|---------|
| void | V |
| boolean | Z |
| byte | B |
| char | C |
| short | S |
| int | I |
| long | J |
| float | F |
| double | D |
| String | Ljava/lang/String; |
| Object | Ljava/lang/Object; |
| int[] | [I |
| String[] | [Ljava/lang/String; |

---

**文档结束**
