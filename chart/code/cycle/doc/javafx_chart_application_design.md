# JavaFX海图显示应用设计文档

**文档编号**: CDS-JAVAFX-DESIGN-001  
**版本**: v1.1  
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
| v1.1 | 2026-04-08 | Team | 解决评审问题：线程安全、资源管理、错误恢复、帧率控制、触摸支持 |
| v1.2 | 2026-04-08 | Team | 多角色交叉评审改进：完整线程模型、JNI内存管理、性能指标定义、集成测试设计、插件扩展、安全性、国际化 |

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

### 3.7 JNI版本兼容性

| Java版本 | JNI版本 | 兼容性说明 |
|----------|---------|------------|
| Java 17 | JNI 1.8 | 当前目标版本 |
| Java 21 | JNI 1.8 | 完全兼容 |
| Java 11 | JNI 1.8 | 需移除Java 17特性 |

**注意事项**:
- 使用`JNI_VERSION_1_8`作为最低版本要求
- 避免使用特定版本的JNI特性
- 定期测试新Java版本兼容性
- 在JNI_OnLoad中检查版本

```cpp
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8) != JNI_OK) {
        return JNI_ERR; // 不支持的版本
    }
    
    JniEnvManager::GetInstance()->Initialize(vm);
    RegisterChartViewerNatives(env);
    
    return JNI_VERSION_1_8;
}
```

### 3.8 JNI局部引用管理

**问题**: 在循环或递归调用中，JNI局部引用可能累积导致局部引用表溢出。

**解决方案**: 使用`JniLocalRefScope`自动管理局部引用生命周期。

```cpp
namespace ogc::jni {

class JniLocalRefScope {
public:
    explicit JniLocalRefScope(JNIEnv* env) : m_env(env), m_startFrame(0) {
        m_startFrame = m_env->PushLocalFrame(LOCAL_FRAME_CAPACITY);
    }
    
    ~JniLocalRefScope() {
        if (m_startFrame >= 0) {
            m_env->PopLocalFrame(nullptr);
        }
    }
    
    static constexpr int LOCAL_FRAME_CAPACITY = 64;
    
private:
    JNIEnv* m_env;
    jint m_startFrame;
};

}

// 使用示例
JNIEXPORT void JNICALL Java_ogc_chart_ChartViewer_nativeProcessFeatures(
    JNIEnv* env, jobject obj, jlong ptr) {
    
    JniLocalRefScope scope(env);  // 自动管理局部引用
    
    for (int i = 0; i < 1000; i++) {
        jstring name = env->NewStringUTF(featureNames[i].c_str());
        jobject attr = env->NewObject(...);
        // 所有局部引用在scope析构时自动释放
    }
}
```

### 3.9 全局引用计数管理

**问题**: `JniGlobalRef`缺少引用计数机制，可能导致提前释放或内存泄漏。

**解决方案**: 实现`JniSharedGlobalRef`类，使用引用计数管理全局引用。

```cpp
namespace ogc::jni {

template<typename T>
class JniSharedGlobalRef {
public:
    JniSharedGlobalRef() : m_refCount(nullptr), m_ref(nullptr), m_env(nullptr) {}
    
    JniSharedGlobalRef(JNIEnv* env, T ref) : m_env(env) {
        if (ref) {
            m_ref = static_cast<T>(env->NewGlobalRef(ref));
            m_refCount = new std::atomic<int>(1);
        }
    }
    
    JniSharedGlobalRef(const JniSharedGlobalRef& other) 
        : m_env(other.m_env), m_ref(other.m_ref), m_refCount(other.m_refCount) {
        if (m_refCount) {
            m_refCount->fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    JniSharedGlobalRef(JniSharedGlobalRef&& other) noexcept
        : m_env(other.m_env), m_ref(other.m_ref), m_refCount(other.m_refCount) {
        other.m_ref = nullptr;
        other.m_refCount = nullptr;
    }
    
    ~JniSharedGlobalRef() {
        release();
    }
    
    JniSharedGlobalRef& operator=(const JniSharedGlobalRef& other) {
        if (this != &other) {
            release();
            m_env = other.m_env;
            m_ref = other.m_ref;
            m_refCount = other.m_refCount;
            if (m_refCount) {
                m_refCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    bool IsValid() const { return m_ref != nullptr; }
    int GetRefCount() const { return m_refCount ? m_refCount->load() : 0; }
    
private:
    void release() {
        if (m_refCount && m_refCount->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (m_ref && m_env) {
                m_env->DeleteGlobalRef(m_ref);
            }
            delete m_refCount;
            m_ref = nullptr;
            m_refCount = nullptr;
        }
    }
    
    JNIEnv* m_env;
    T m_ref;
    std::atomic<int>* m_refCount;
};

}
```

### 3.10 线程本地存储管理

**问题**: `JniEnvManager`的`AttachCurrentThread`缺少线程本地存储管理，可能导致重复Attach或Detach后访问失效。

**解决方案**: 使用线程本地存储缓存JNIEnv指针。

```cpp
namespace ogc::jni {

class JniEnvManager {
public:
    static JniEnvManager* GetInstance();
    
    JNIEnv* AttachCurrentThread() {
        if (m_tlsEnv) {
            return m_tlsEnv;
        }
        
        JNIEnv* env = nullptr;
        jint result = m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8);
        
        if (result == JNI_EDETACHED) {
            result = m_javaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
            if (result != JNI_OK) {
                return nullptr;
            }
            m_tlsEnv = env;
            m_tlsAttached = true;
        } else if (result == JNI_OK) {
            m_tlsEnv = env;
            m_tlsAttached = false;
        }
        
        return env;
    }
    
    void DetachCurrentThread() {
        if (m_tlsAttached && m_javaVM) {
            m_javaVM->DetachCurrentThread();
            m_tlsEnv = nullptr;
            m_tlsAttached = false;
        }
    }
    
    bool IsCurrentThreadAttached() const {
        return m_tlsEnv != nullptr;
    }
    
private:
    JavaVM* m_javaVM;
    std::mutex m_mutex;
    bool m_initialized;
    
    static thread_local JNIEnv* m_tlsEnv;
    static thread_local bool m_tlsAttached;
};

thread_local JNIEnv* JniEnvManager::m_tlsEnv = nullptr;
thread_local bool JniEnvManager::m_tlsAttached = false;

}
```

### 3.11 增强异常处理

**问题**: `JniException`缺少异常链和堆栈跟踪保留机制。

**解决方案**: 增强异常处理，支持异常链和完整堆栈跟踪。

```cpp
namespace ogc::jni {

class JniException {
public:
    static void ThrowWithCause(JNIEnv* env, const std::string& className,
                               const std::string& message, 
                               jthrowable cause = nullptr) {
        jclass exClass = env->FindClass(className.c_str());
        if (!exClass) {
            env->FindClass("java/lang/RuntimeException");
        }
        
        if (cause) {
            jmethodID ctor = env->GetMethodID(exClass, "<init>", 
                "(Ljava/lang/String;Ljava/lang/Throwable;)V");
            jstring jMsg = env->NewStringUTF(message.c_str());
            env->ThrowNew(exClass, message.c_str());
            jobject ex = env->NewObject(exClass, ctor, jMsg, cause);
            env->Throw(static_cast<jthrowable>(ex));
        } else {
            env->ThrowNew(exClass, message.c_str());
        }
    }
    
    static void TranslateAndThrowWithTrace(JNIEnv* env, const std::exception& e) {
        std::string trace = GetStackTrace();
        std::string message = std::string(e.what()) + "\nNative stack trace:\n" + trace;
        ThrowRuntimeException(env, message);
    }
    
    static std::string GetStackTrace() {
        std::ostringstream oss;
        void* buffer[64];
        int size = backtrace(buffer, 64);
        char** symbols = backtrace_symbols(buffer, size);
        for (int i = 0; i < size; i++) {
            oss << "  #" << i << " " << symbols[i] << "\n";
        }
        free(symbols);
        return oss.str();
    }
    
    static std::string GetJavaStackTrace(JNIEnv* env, jthrowable ex) {
        jclass stringWriterClass = env->FindClass("java/io/StringWriter");
        jclass printWriterClass = env->FindClass("java/io/PrintWriter");
        
        jobject stringWriter = env->NewObject(stringWriterClass,
            env->GetMethodID(stringWriterClass, "<init>", "()V"));
        jobject printWriter = env->NewObject(printWriterClass,
            env->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V"),
            stringWriter);
        
        jclass exClass = env->GetObjectClass(ex);
        jmethodID printStackTrace = env->GetMethodID(exClass, "printStackTrace",
            "(Ljava/io/PrintWriter;)V");
        env->CallVoidMethod(ex, printStackTrace, printWriter);
        
        jmethodID toString = env->GetMethodID(stringWriterClass, "toString",
            "()Ljava/lang/String;");
        jstring result = (jstring)env->CallObjectMethod(stringWriter, toString);
        
        return JniConverter::ToString(env, result);
    }
};

}
```

### 3.12 批量数据转换优化

**问题**: `JniConverter`的字符串和数组转换缺少批量优化，导致多次JNI调用开销。

**解决方案**: 使用DirectBuffer和批量转换方法。

```cpp
namespace ogc::jni {

class JniConverter {
public:
    static std::vector<Coordinate> ToCoordinateVectorBatch(JNIEnv* env, 
                                                            jobject obj,
                                                            jlong ptr,
                                                            int count) {
        std::vector<Coordinate> coords;
        coords.reserve(count);
        
        jobject directBuffer = env->NewDirectByteBuffer(
            reinterpret_cast<void*>(ptr), count * sizeof(double) * 2);
        
        jmethodID method = env->GetMethodID(
            env->GetObjectClass(obj), "fillCoordinateBuffer", "(Ljava/nio/ByteBuffer;)V");
        env->CallVoidMethod(obj, method, directBuffer);
        
        double* data = reinterpret_cast<double*>(
            env->GetDirectBufferAddress(directBuffer));
        
        for (int i = 0; i < count; i++) {
            coords.emplace_back(data[i * 2], data[i * 2 + 1]);
        }
        
        return coords;
    }
    
    static jobject CreateDirectBuffer(JNIEnv* env, void* data, size_t size) {
        return env->NewDirectByteBuffer(data, size);
    }
    
    static void* GetDirectBufferAddress(JNIEnv* env, jobject buffer) {
        return env->GetDirectBufferAddress(buffer);
    }
    
    static std::vector<std::string> ToStringVectorBatch(JNIEnv* env, 
                                                         jobjectArray array) {
        jsize count = env->GetArrayLength(array);
        std::vector<std::string> result;
        result.reserve(count);
        
        JniLocalRefScope scope(env);
        
        for (jsize i = 0; i < count; i++) {
            jstring str = (jstring)env->GetObjectArrayElement(array, i);
            const char* chars = env->GetStringUTFChars(str, nullptr);
            result.emplace_back(chars);
            env->ReleaseStringUTFChars(str, chars);
        }
        
        return result;
    }
};

}
```

**解决方案**:

```cpp
// 方案1：显式删除局部引用
void ProcessArray(JNIEnv* env, jobjectArray array) {
    jsize count = env->GetArrayLength(array);
    for (jsize i = 0; i < count; i++) {
        jobject item = env->GetObjectArrayElement(array, i);
        
        // 处理item...
        
        env->DeleteLocalRef(item); // 显式释放
    }
}

// 方案2：使用JniLocalRef自动管理（推荐）
void ProcessArraySafe(JNIEnv* env, jobjectArray array) {
    jsize count = env->GetArrayLength(array);
    for (jsize i = 0; i < count; i++) {
        JniLocalRef<jobject> item(env, env->GetObjectArrayElement(array, i));
        
        // 处理item...
        // 自动释放，无需手动DeleteLocalRef
    }
}

// 方案3：使用Push/PopLocalFrame管理作用域
void ProcessWithFrame(JNIEnv* env, jobjectArray array) {
    const int FRAME_CAPACITY = 16;
    env->PushLocalFrame(FRAME_CAPACITY);
    
    // 在此作用域内创建的局部引用会在Pop时自动释放
    jobject item = env->GetObjectArrayElement(array, 0);
    // 处理...
    
    env->PopLocalFrame(nullptr);
}
```

**最佳实践**:
- 循环中必须管理局部引用
- 优先使用RAII包装类（JniLocalRef）
- 大量对象处理时使用Push/PopLocalFrame
- 定期使用`env->EnsureLocalCapacity()`预留空间

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
    
    // ==================== JNI回调方法（线程安全） ====================
    
    /**
     * JNI回调方法 - 渲染完成
     * 
     * <p>注意：此方法由Native线程调用，必须使用Platform.runLater
     * 确保在JavaFX应用线程执行回调。</p>
     */
    private void onRenderComplete() {
        Platform.runLater(() -> {
            if (renderCallback != null) {
                renderCallback.onRenderComplete();
            }
        });
    }
    
    /**
     * JNI回调方法 - 加载成功
     */
    private void onLoadSuccess(String chartId) {
        Platform.runLater(() -> {
            if (loadCallback != null) {
                loadCallback.onLoadSuccess(chartId);
            }
        });
    }
    
    /**
     * JNI回调方法 - 加载失败
     */
    private void onLoadError(String error) {
        Platform.runLater(() -> {
            if (loadCallback != null) {
                loadCallback.onLoadError(error);
            }
        });
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
 * <p><b>重要：</b>必须调用close()释放Native资源，建议使用try-with-resources。</p>
 */
public class Geometry implements AutoCloseable {
    
    public enum Type {
        POINT,
        LINESTRING,
        POLYGON,
        MULTIPOINT,
        MULTILINESTRING,
        MULTIPOLYGON,
        GEOMETRYCOLLECTION
    }
    
    private long nativePtr;
    private boolean disposed = false;
    private Type type;
    
    Geometry(long nativePtr) {
        this.nativePtr = nativePtr;
        this.type = Type.values()[nativeGetType(nativePtr)];
    }
    
    public Type getType() {
        checkDisposed();
        return type;
    }
    
    public double[] getCoordinates() {
        checkDisposed();
        return nativeGetCoordinates(nativePtr);
    }
    
    public Envelope getEnvelope() {
        checkDisposed();
        double[] bounds = nativeGetEnvelope(nativePtr);
        return new Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
    }
    
    public String asWKT() {
        checkDisposed();
        return nativeAsWKT(nativePtr);
    }
    
    // ==================== 资源管理 ====================
    
    @Override
    public void close() {
        if (!disposed && nativePtr != 0) {
            nativeDispose(nativePtr);
            nativePtr = 0;
            disposed = true;
        }
    }
    
    private void checkDisposed() {
        if (disposed || nativePtr == 0) {
            throw new IllegalStateException("Geometry has been disposed");
        }
    }
    
    // Native methods
    private static native int nativeGetType(long ptr);
    private static native double[] nativeGetCoordinates(long ptr);
    private static native double[] nativeGetEnvelope(long ptr);
    private static native String nativeAsWKT(long ptr);
    private static native void nativeDispose(long ptr);
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

### 5.0 线程模型设计

**架构总览**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           线程模型架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX Application Thread                         │   │
│  │  - UI事件处理                                                        │   │
│  │  - Canvas渲染触发                                                    │   │
│  │  - 属性更新                                                          │   │
│  │  - Platform.runLater()执行点                                         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                    ┌───────────────┼───────────────┐                       │
│                    │               │               │                        │
│                    ▼               ▼               ▼                        │
│  ┌─────────────────────┐ ┌─────────────────────┐ ┌─────────────────────┐   │
│  │  Render Thread      │ │  Load Thread        │ │  Background Thread  │   │
│  │  - C++渲染引擎      │ │  - 海图加载         │ │  - 数据预处理       │   │
│  │  - 瓦片渲染         │ │  - 缓存预热         │ │  - 异步查询         │   │
│  │  - 图元绘制         │ │  - 符号解析         │ │  - 后台计算         │   │
│  └─────────────────────┘ └─────────────────────┘ └─────────────────────┘   │
│                    │               │               │                        │
│                    └───────────────┼───────────────┘                       │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JNI Thread Bridge                                 │   │
│  │  - AttachCurrentThread() / DetachCurrentThread()                    │   │
│  │  - 线程本地JNIEnv缓存                                               │   │
│  │  - Platform.runLater()回调                                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**线程安全规则**:

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| THREAD-01 | UI操作必须在JavaFX Application Thread | 使用Platform.runLater() |
| THREAD-02 | JNI回调必须切换到JavaFX线程 | Native线程不能直接操作UI |
| THREAD-03 | C++渲染线程独立于JavaFX线程 | 避免阻塞UI |
| THREAD-04 | 后台加载使用独立线程池 | 避免相互干扰 |
| THREAD-05 | 共享数据使用线程安全容器 | ConcurrentHashMap等 |

**线程同步机制**:

```java
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
}
```

### 5.0.1 应用生命周期管理

**生命周期状态机**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           应用生命周期状态机                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│    ┌─────────┐                                                              │
│    │ CREATED │ ←──────────────────────────────────────────┐                │
│    └────┬────┘                                            │                │
│         │ initialize()                                    │                │
│         ▼                                                 │                │
│    ┌─────────┐                                            │                │
│    │  READY  │ ←──────────────────────┐                   │                │
│    └────┬────┘                        │                   │                │
│         │ loadChart()                 │                   │                │
│         ▼                             │ resume()          │                │
│    ┌─────────┐                   ┌────┴────┐              │                │
│    │ LOADED  │ ── pause() ─────→ │ PAUSED  │ ── stop() ───┘                │
│    └────┬────┘                   └─────────┘                               │
│         │ dispose()                                                         │
│         ▼                                                                   │
│    ┌───────────┐                                                            │
│    │ DISPOSED  │                                                            │
│    └───────────┘                                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**生命周期管理器**:

```java
public enum AppState {
    CREATED,    // 应用创建，资源未初始化
    READY,      // 初始化完成，等待加载
    LOADED,     // 海图已加载，可渲染
    PAUSED,     // 应用暂停，释放部分资源
    DISPOSED    // 应用销毁，释放所有资源
}

public class AppLifecycleManager {
    private final AtomicReference<AppState> state = new AtomicReference<>(AppState.CREATED);
    private final ChartViewer viewer;
    private final ChartCanvas canvas;
    private final ExecutorService backgroundExecutor;
    
    public void initialize() {
        if (!state.compareAndSet(AppState.CREATED, AppState.READY)) {
            throw new IllegalStateException("Invalid state transition");
        }
        
        backgroundExecutor = Executors.newFixedThreadPool(
            Runtime.getRuntime().availableProcessors());
        
        viewer.setConfig(buildDefaultConfig());
        canvas.initialize();
        
        LOG.info("Application initialized");
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
            LOG.info("Application paused");
        }
    }
    
    public void resume() {
        if (state.compareAndSet(AppState.PAUSED, AppState.LOADED)) {
            canvas.resumeRendering();
            LOG.info("Application resumed");
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
            
            LOG.info("Application disposed");
        } catch (Exception e) {
            LOG.error("Error during dispose", e);
        }
    }
    
    public AppState getState() {
        return state.get();
    }
}
```

**JavaFX Application生命周期集成**:

```java
public class ChartApplication extends Application {
    
    private AppLifecycleManager lifecycleManager;
    
    @Override
    public void start(Stage primaryStage) {
        lifecycleManager = new AppLifecycleManager();
        
        MainView view = new MainView(primaryStage, lifecycleManager);
        Scene scene = new Scene(view, 1200, 800);
        
        primaryStage.setTitle("海图显示系统");
        primaryStage.setScene(scene);
        primaryStage.setOnCloseRequest(e -> {
            e.consume();
            shutdown();
        });
        
        lifecycleManager.initialize();
        primaryStage.show();
    }
    
    @Override
    public void stop() {
        shutdown();
    }
    
    private void shutdown() {
        if (lifecycleManager != null) {
            lifecycleManager.dispose();
        }
        Platform.exit();
    }
}
```

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
import javafx.application.Platform;

/**
 * JavaFX Canvas海图渲染组件
 * 
 * <p>基于JavaFX Canvas的海图显示组件，封装渲染循环和事件处理。</p>
 * <p><b>帧率控制：</b>使用脏标记和最小帧间隔避免过度渲染。</p>
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
    
    // 帧率控制
    private volatile boolean dirty = true;
    private long lastRenderTime = 0;
    private static final long MIN_FRAME_INTERVAL_NS = 16_000_000; // ~60fps
    private static final long IDLE_FRAME_INTERVAL_NS = 100_000_000; // 10fps when idle
    
    public ChartCanvas(ChartViewer viewer) {
        this.viewer = viewer;
        this.eventHandler = new JavaFXEventHandler(viewer);
        
        // 渲染循环（带帧率控制）
        renderTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (!renderEnabled.get()) return;
                
                long elapsed = now - lastRenderTime;
                long targetInterval = dirty ? MIN_FRAME_INTERVAL_NS : IDLE_FRAME_INTERVAL_NS;
                
                if (elapsed >= targetInterval) {
                    render();
                    lastRenderTime = now;
                    if (!dirty) {
                        // 无变化时降低帧率
                    }
                }
            }
        };
        
        // 尺寸绑定
        widthProperty().addListener(obs -> requestRender());
        heightProperty().addListener(obs -> requestRender());
        
        // 事件绑定
        setupEventHandlers();
        setupTouchHandlers(); // 触摸屏支持
        
        renderTimer.start();
    }
    
    private void render() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.clearRect(0, 0, getWidth(), getHeight());
        
        // 调用Native渲染
        viewer.render(gc, getWidth(), getHeight());
    }
    
    /**
     * 请求重绘
     * 
     * <p>设置脏标记，触发下一帧渲染。</p>
     */
    public void requestRender() {
        dirty = true;
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
    
    /**
     * 设置触摸屏事件处理器
     */
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
    
    bool HasError() const { return m_hasError; }
    void ClearError() { m_hasError = false; }

private:
    void EnsureGraphicsContext();
    void ApplyStyle(const DrawStyle& style);
    void ApplyTextStyle(const TextStyle& style);
    bool TryRecover();
    
    jobject m_canvas;
    jobject m_gc;
    JNIEnv* m_env;
    int m_width, m_height;
    bool m_hasError = false;
    
    jclass m_gcClass;
    jmethodID m_setStrokeMethod;
    jmethodID m_setFillMethod;
    jmethodID m_setLineWidthMethod;
    jmethodID m_strokeLineMethod;
    jmethodID m_fillRectMethod;
    jmethodID m_fillTextMethod;
};

// 错误恢复实现示例
void JavaFXCanvasAdapter::DrawLine(const std::vector<Point>& points, 
                                    const DrawStyle& style) {
    try {
        EnsureGraphicsContext();
        if (m_hasError) return;
        
        ApplyStyle(style);
        // 绘制逻辑...
    } catch (const std::exception& e) {
        LOG_ERROR("DrawLine failed: " + std::string(e.what()));
        m_hasError = true;
        if (TryRecover()) {
            m_hasError = false;
        }
    }
}

bool JavaFXCanvasAdapter::TryRecover() {
    try {
        m_gc = nullptr;
        EnsureGraphicsContext();
        return m_gc != nullptr;
    } catch (...) {
        return false;
    }
}

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
 * <p>处理鼠标、键盘、滚轮、触摸屏等输入事件，转换为海图操作。</p>
 */
public class JavaFXEventHandler {
    
    private final ChartViewer viewer;
    private Point2D lastMousePos;
    private Point2D lastTouchPos;
    private boolean isDragging = false;
    
    public JavaFXEventHandler(ChartViewer viewer) {
        this.viewer = viewer;
    }
    
    // ==================== 鼠标事件 ====================
    
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
    
    // ==================== 键盘事件 ====================
    
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
    
    // ==================== 触摸屏事件 ====================
    
    public void onTouchPressed(TouchPoint tp) {
        lastTouchPos = new Point2D(tp.getX(), tp.getY());
        isDragging = false;
    }
    
    public void onTouchMoved(TouchPoint tp) {
        if (lastTouchPos != null) {
            double dx = tp.getX() - lastTouchPos.getX();
            double dy = tp.getY() - lastTouchPos.getY();
            
            if (Math.abs(dx) > 2 || Math.abs(dy) > 2) {
                isDragging = true;
                viewer.pan(-dx, -dy);
            }
            
            lastTouchPos = new Point2D(tp.getX(), tp.getY());
        }
    }
    
    public void onTouchReleased(TouchPoint tp) {
        if (!isDragging) {
            // 触摸查询
            FeatureInfo feature = viewer.queryFeature(tp.getX(), tp.getY());
            if (feature != null) {
                showFeatureInfo(feature);
            }
        }
        isDragging = false;
    }
    
    public void onZoom(double factor, double centerX, double centerY) {
        viewer.zoom(factor, centerX, centerY);
    }
    
    public void onRotate(double angle) {
        viewer.rotate(angle);
    }
    
    // ==================== 辅助方法 ====================
    
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

### 7.0 插件扩展机制设计

**插件架构**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           插件扩展架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Core Application                                  │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ PluginManager │  │ LayerRegistry │  │ SymbolRegistry│           │   │
│  │  │ 插件管理器    │  │ 图层注册表    │  │ 符号注册表    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    │ SPI接口                                │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Plugin Layer                                      │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ CustomLayer   │  │ CustomSymbol  │  │ CustomParser  │           │   │
│  │  │ 自定义图层    │  │ 自定义符号    │  │ 自定义解析器  │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**插件接口定义**:

```java
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

public interface LayerProvider {
    
    String getLayerType();
    Layer createLayer(LayerConfig config);
    LayerConfig createDefaultConfig();
}

public interface SymbolProvider {
    
    String getSymbolType();
    Symbol createSymbol(SymbolConfig config);
    boolean canHandle(FeatureInfo feature);
}

public class PluginManager {
    
    private final Map<String, ChartPlugin> plugins = new ConcurrentHashMap<>();
    private final ServiceLoader<ChartPlugin> pluginLoader;
    
    public PluginManager() {
        this.pluginLoader = ServiceLoader.load(ChartPlugin.class);
    }
    
    public void loadPlugins() {
        for (ChartPlugin plugin : pluginLoader) {
            try {
                plugin.initialize(createPluginContext());
                plugins.put(plugin.getName(), plugin);
                LOG.info("Plugin loaded: {} v{}", plugin.getName(), plugin.getVersion());
            } catch (Exception e) {
                LOG.error("Failed to load plugin: " + plugin.getName(), e);
            }
        }
    }
    
    public void registerLayerProvider(LayerProvider provider) {
        layerRegistry.register(provider.getLayerType(), provider);
    }
    
    public Layer createLayer(String type, LayerConfig config) {
        LayerProvider provider = layerRegistry.getProvider(type);
        if (provider != null) {
            return provider.createLayer(config);
        }
        throw new IllegalArgumentException("Unknown layer type: " + type);
    }
}
```

**自定义图层示例**:

```java
public class WeatherLayerPlugin implements ChartPlugin {
    
    @Override
    public String getName() {
        return "Weather Layer Plugin";
    }
    
    @Override
    public String getVersion() {
        return "1.0.0";
    }
    
    @Override
    public List<LayerProvider> getLayerProviders() {
        return Arrays.asList(
            new WeatherRadarLayerProvider(),
            new WeatherOverlayLayerProvider()
        );
    }
}

public class WeatherRadarLayerProvider implements LayerProvider {
    
    @Override
    public String getLayerType() {
        return "weather.radar";
    }
    
    @Override
    public Layer createLayer(LayerConfig config) {
        WeatherRadarLayer layer = new WeatherRadarLayer();
        layer.setRadarSource(config.getString("radarSource"));
        layer.setUpdateInterval(config.getInt("updateInterval", 300000));
        return layer;
    }
}
```

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

### 7.6 错误恢复和容错机制

**错误分类和处理策略**:

| 错误类型 | 严重级别 | 处理策略 | 恢复机制 |
|----------|----------|----------|----------|
| **JNI调用失败** | 高 | 记录日志，抛出异常 | 重试或降级 |
| **内存不足** | 高 | 清理缓存，通知用户 | 释放非必要资源 |
| **海图加载失败** | 中 | 显示错误提示，记录日志 | 允许重新加载 |
| **渲染错误** | 中 | 跳过错误图元，继续渲染 | 标记错误区域 |
| **网络错误** | 低 | 使用离线缓存 | 自动重试 |

**错误恢复管理器**:

```java
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
        
        // 策略1: 重试
        if (getRetryCount() < MAX_RETRIES) {
            incrementRetryCount();
            try {
                Thread.sleep(RETRY_DELAY_MS);
                return operation.call();
            } catch (Exception e) {
                LOG.error("Retry failed", e);
            }
        }
        
        // 策略2: 重新初始化JNI环境
        JniEnvManager.getInstance().resetEnvironment();
        
        // 策略3: 降级处理
        return getFallbackValue();
    }
    
    private <T> T recoverFromMemoryError(Throwable error, Callable<T> operation) {
        LOG.error("Memory error occurred, performing emergency cleanup");
        
        // 清理缓存
        CacheManager.getInstance().clearAll();
        
        // 请求GC
        System.gc();
        
        // 通知用户
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
        
        // 标记错误区域
        RenderContext context = getCurrentContext();
        context.markErrorArea(error);
        
        // 跳过错误图元，继续渲染
        context.setSkipErrors(true);
        
        return null;
    }
}
```

**熔断器实现**:

```java
public class CircuitBreaker {
    
    private final int failureThreshold;
    private final Duration resetTimeout;
    
    private final AtomicInteger failureCount = new AtomicInteger(0);
    private final AtomicLong lastFailureTime = new AtomicLong(0);
    private final AtomicReference<State> state = new AtomicReference<>(State.CLOSED);
    
    public enum State {
        CLOSED,     // 正常状态
        OPEN,       // 熔断状态
        HALF_OPEN   // 半开状态（试探性恢复）
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
}
```

**优雅降级策略**:

```java
public class GracefulDegradation {
    
    private final DegradationLevel currentLevel = new AtomicReference<>(DegradationLevel.FULL);
    
    public enum DegradationLevel {
        FULL,           // 完整功能
        REDUCED_CACHE,  // 减少缓存
        LOW_QUALITY,    // 低质量渲染
        MINIMAL         // 最小功能
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
}
```

### 7.7 API文档规范

**API文档模板**:

```java
/**
 * 海图查看器核心类
 * 
 * <p>ChartViewer是JavaFX海图显示应用的核心类，封装了海图加载、渲染、查询等功能。
 * 该类通过JNI桥接层与C++核心模块交互，提供高性能的海图显示能力。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 创建查看器
 * try (ChartViewer viewer = new ChartViewer()) {
 *     // 加载海图
 *     viewer.loadChart("/path/to/chart.000");
 *     
 *     // 渲染到Canvas
 *     viewer.render(canvas.getGraphicsContext2D(), 800, 600);
 *     
 *     // 查询要素
 *     FeatureInfo feature = viewer.queryFeature(400, 300);
 *     System.out.println("Feature: " + feature.getFeatureId());
 * }
 * }</pre>
 * 
 * <h2>线程安全</h2>
 * <ul>
 *   <li>所有方法都可以从任意线程调用</li>
 *   <li>回调方法会在JavaFX应用线程执行</li>
 *   <li>内部使用线程安全的数据结构</li>
 * </ul>
 * 
 * <h2>资源管理</h2>
 * <p>ChartViewer实现了{@link AutoCloseable}接口，建议使用try-with-resources语句
 * 确保资源正确释放。</p>
 * 
 * @author OGC Team
 * @version 1.2
 * @since 1.0
 * @see ChartCanvas
 * @see FeatureInfo
 * @see ChartConfig
 */
public class ChartViewer implements AutoCloseable {
    
    /**
     * 加载海图文件
     * 
     * <p>从指定路径加载海图文件。支持S57和S101格式。
     * 加载操作在后台线程执行，不会阻塞UI线程。</p>
     * 
     * <h3>支持的文件格式</h3>
     * <ul>
     *   <li>S57 (.000) - IHO S-57标准</li>
     *   <li>S101 (.000) - IHO S-101标准</li>
     * </ul>
     * 
     * @param filePath 海图文件路径，不能为null
     * @throws ChartException 如果加载失败
     * @throws IllegalArgumentException 如果filePath为null或空
     * @throws SecurityException 如果没有文件读取权限
     */
    public void loadChart(String filePath) throws ChartException {
        // ...
    }
    
    /**
     * 查询指定位置的要素
     * 
     * <p>查询海图上指定屏幕坐标位置的要素信息。
     * 返回最上层的可见要素。</p>
     * 
     * @param x 屏幕X坐标（像素）
     * @param y 屏幕Y坐标（像素）
     * @return 要素信息，如果该位置没有要素则返回null
     * @throws IllegalStateException 如果查看器已关闭
     */
    public FeatureInfo queryFeature(double x, double y) {
        // ...
    }
}
```

**API版本兼容性**:

```java
/**
 * @apiNote 
 * <p>版本兼容性说明：</p>
 * <ul>
 *   <li>v1.0: 初始版本，基础渲染功能</li>
 *   <li>v1.1: 添加触摸屏支持</li>
 *   <li>v1.2: 添加插件扩展机制</li>
 * </ul>
 * 
 * <p>废弃API：</p>
 * <ul>
 *   <li>{@link #render(GraphicsContext, int, int)} 自v1.1起废弃，
 *       请使用{@link #render(Object, double, double)}</li>
 * </ul>
 */
```

---

## 8. 性能优化设计

### 8.0 性能指标定义

**核心性能指标**:

| 指标类别 | 指标名称 | 目标值 | 测量方法 | 优先级 |
|----------|----------|--------|----------|--------|
| **渲染性能** | 帧率 | ≥30fps | AnimationTimer统计 | 高 |
| | 首帧时间 | <500ms | 从loadChart到首帧渲染 | 高 |
| | 渲染延迟 | <100ms | 事件触发到渲染完成 | 高 |
| **内存性能** | Java堆内存 | ≤256MB | VisualVM监控 | 高 |
| | Native内存 | ≤256MB | 系统内存监控 | 高 |
| | 内存峰值 | ≤512MB | 压力测试 | 中 |
| **加载性能** | 海图加载时间 | ≤3秒 | 100MB海图 | 高 |
| | 瓦片加载时间 | ≤100ms | 单瓦片加载 | 中 |
| | 缓存命中率 | ≥80% | 缓存统计 | 中 |
| **响应性能** | 交互响应 | ≤100ms | 鼠标/触摸事件 | 高 |
| | 查询响应 | ≤50ms | 要素查询 | 中 |
| | 缩放响应 | ≤100ms | 缩放动画 | 中 |

**性能基准测试配置**:

```java
public class PerformanceBenchmark {
    
    @BenchmarkMode(Mode.AverageTime)
    @OutputTimeUnit(TimeUnit.MILLISECONDS)
    @Warmup(iterations = 3, time = 1)
    @Measurement(iterations = 5, time = 1)
    @Fork(1)
    public void benchmarkRender(Blackhole bh) {
        viewer.render(canvas, 1920, 1080);
    }
    
    @Benchmark
    public void benchmarkQuery(Blackhole bh) {
        FeatureInfo feature = viewer.queryFeature(960, 540);
        bh.consume(feature);
    }
    
    @Benchmark
    public void benchmarkPan(Blackhole bh) {
        viewer.pan(100, 100);
        bh.consume(viewer);
    }
}
```

### 8.1 渲染优化

| 优化项 | 实现方式 | 预期效果 |
|--------|----------|----------|
| 帧率控制 | AnimationTimer + 脏区域检测 | 避免过度渲染 |
| 批量绘制 | 合并相同样式图元 | 减少JNI调用 |
| 异步加载 | 后台线程加载数据 | UI不阻塞 |
| 缓存策略 | 瓦片缓存 + 符号缓存 | 减少重复计算 |

**渲染优化策略详解**:

```java
public class RenderOptimizer {
    
    private static final int MAX_FPS = 60;
    private static final int MIN_FPS = 10;
    private static final long FRAME_TIME_NS = 1_000_000_000L / MAX_FPS;
    
    private volatile boolean dirty = true;
    private long lastRenderTime = 0;
    private int consecutiveIdleFrames = 0;
    
    public void optimizeRenderLoop(AnimationTimer timer) {
        timer.handle(now -> {
            if (!dirty && consecutiveIdleFrames > 10) {
                return;
            }
            
            long elapsed = now - lastRenderTime;
            if (elapsed >= FRAME_TIME_NS) {
                render();
                lastRenderTime = now;
                
                if (dirty) {
                    consecutiveIdleFrames = 0;
                    dirty = false;
                } else {
                    consecutiveIdleFrames++;
                }
            }
        });
    }
    
    public void markDirty() {
        dirty = true;
        consecutiveIdleFrames = 0;
    }
}
```

**瓦片渲染优先级**:

```java
public class TileRenderPriority {
    
    public enum Priority {
        CRITICAL,   // 当前视口中心
        HIGH,       // 当前视口边缘
        MEDIUM,     // 预加载区域
        LOW         // 后台预加载
    }
    
    public Priority calculatePriority(TileKey tile, Viewport viewport) {
        double distance = tile.distanceToViewportCenter(viewport);
        
        if (distance < 100) return Priority.CRITICAL;
        if (distance < 500) return Priority.HIGH;
        if (distance < 1000) return Priority.MEDIUM;
        return Priority.LOW;
    }
    
    public void scheduleRender(TileKey tile, Priority priority) {
        switch (priority) {
            case CRITICAL:
                renderQueue.addFirst(tile);
                break;
            case HIGH:
                renderQueue.add(tile);
                break;
            case MEDIUM:
                preloadQueue.add(tile);
                break;
            case LOW:
                backgroundQueue.add(tile);
                break;
        }
    }
}
```

### 8.2 内存优化

| 优化项 | 实现方式 | 预期效果 |
|--------|----------|----------|
| 延迟加载 | 按需获取属性数据 | 减少内存占用 |
| 对象池 | 复用Java对象 | 减少GC压力 |
| 弱引用 | 缓存使用弱引用 | 自动回收不常用数据 |
| 批量释放 | 批量释放Native资源 | 减少JNI调用 |

**内存管理策略详解**:

```java
public class MemoryManager {
    
    private static final long MAX_JAVA_HEAP = 256 * 1024 * 1024;  // 256MB
    private static final long MAX_NATIVE_MEMORY = 256 * 1024 * 1024;  // 256MB
    private static final double MEMORY_PRESSURE_THRESHOLD = 0.8;
    
    private final MemoryMonitor monitor = new MemoryMonitor();
    private final List<MemoryPressureListener> listeners = new CopyOnWriteArrayList<>();
    
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
            cache.clear();
        } else {
            nativeCache.clear();
            viewer.flushCache();
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
```

**Java堆与Native内存协调**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           内存管理架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java Heap (≤256MB)                                │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartViewer   │  │ FeatureInfo   │  │ JavaFX Nodes  │           │   │
│  │  │ (轻量包装)    │  │ (按需加载)    │  │ (UI组件)      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    │ JNI指针 (jlong)                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Native Memory (≤256MB)                            │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ C++ Geometry  │  │ C++ Features  │  │ Render Cache  │           │   │
│  │  │ (核心数据)    │  │ (属性存储)    │  │ (瓦片缓存)    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  内存协调机制：                                                             │
│  1. Java对象仅持有Native指针，不存储实际数据                               │
│  2. Native内存独立管理，通过JNI计数器追踪                                   │
│  3. 内存压力时，优先释放缓存，保留核心数据                                  │
│  4. 定期检查内存使用，主动释放不活跃数据                                    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 8.3 缓存策略

**缓存层次架构**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           缓存层次架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Level 1: 热数据缓存 (内存，≤128MB)                                         │
│  ├── 当前视口瓦片                                                          │
│  ├── 最近访问的要素                                                        │
│  └── 渲染结果缓存                                                          │
│                                                                             │
│  Level 2: 温数据缓存 (内存，≤128MB)                                         │
│  ├── 预加载区域瓦片                                                        │
│  ├── 符号化结果缓存                                                        │
│  └── 几何数据缓存                                                          │
│                                                                             │
│  Level 3: 冷数据缓存 (磁盘，≤1GB)                                           │
│  ├── 离线瓦片存储                                                          │
│  ├── 海图数据文件                                                          │
│  └── 符号库缓存                                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**缓存淘汰策略**:

```java
public class CacheEvictionPolicy {
    
    private static final int MAX_CACHE_SIZE = 128 * 1024 * 1024;  // 128MB
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
        
        entries.sort((a, b) -> {
            double scoreA = calculateEvictionScore(a);
            double scoreB = calculateEvictionScore(b);
            return Double.compare(scoreA, scoreB);
        });
        
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
```

**缓存预热策略**:

```java
public class CachePreheater {
    
    public void preheat(Viewport current, Viewport predicted) {
        List<TileKey> tiles = calculatePreheatTiles(current, predicted);
        
        CompletableFuture<?>[] futures = tiles.stream()
            .map(tile -> CompletableFuture.runAsync(() -> {
                if (!cache.contains(tile)) {
                    loadTile(tile);
                }
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
}
```

### 8.4 性能监控机制

**监控指标收集**:

```java
public class PerformanceMonitor {
    
    private final MetricsCollector collector = new MetricsCollector();
    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    
    public void start() {
        scheduler.scheduleAtFixedRate(this::collectMetrics, 0, 1, TimeUnit.SECONDS);
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
}
```

**性能日志输出**:

```
[PERF] 2026-04-08 10:30:45 | FPS: 45.2 | Heap: 180MB | Native: 120MB | JNI: 1523 | Cache: 85%
[PERF] 2026-04-08 10:30:46 | FPS: 42.8 | Heap: 185MB | Native: 125MB | JNI: 1687 | Cache: 87%
[PERF] 2026-04-08 10:30:47 | FPS: 48.1 | Heap: 182MB | Native: 118MB | JNI: 1402 | Cache: 88%
[PERF] 2026-04-08 10:30:48 | FPS: 50.0 | Heap: 178MB | Native: 115MB | JNI: 1256 | Cache: 90%
```

### 8.5 性能指标

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 帧率 | ≥30fps | AnimationTimer统计 |
| 首帧时间 | <500ms | 从加载到首帧 |
| 内存峰值 | <1GB | VisualVM监控 |
| 响应延迟 | <100ms | 事件到渲染 |

---

## 9. 测试设计

### 9.0 测试策略总览

**测试金字塔**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           测试金字塔                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│                         ┌─────────────────┐                                 │
│                         │   UI测试 (10%)   │                                │
│                         │   TestFX框架     │                                │
│                         └────────┬────────┘                                 │
│                    ┌─────────────┴─────────────┐                            │
│                    │     集成测试 (30%)         │                            │
│                    │  JNI集成 + 模块集成       │                            │
│                    └─────────────┬─────────────┘                            │
│               ┌──────────────────┴──────────────────┐                       │
│               │          单元测试 (60%)              │                       │
│               │   JUnit 5 + Mockito + GoogleTest    │                       │
│               └─────────────────────────────────────┘                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**测试覆盖率目标**:

| 测试层次 | 覆盖率目标 | 关键路径 | 工具 |
|----------|------------|----------|------|
| 单元测试 | ≥80% | 100% | JUnit 5, GoogleTest |
| 集成测试 | ≥60% | 100% | JUnit 5, TestFX |
| UI测试 | ≥40% | 100% | TestFX |
| 性能测试 | 基准测试 | 关键操作 | JMH |

### 9.1 测试层次

| 测试层次 | 覆盖范围 | 工具 |
|----------|----------|------|
| JNI单元测试 | 数据转换、异常处理 | GoogleTest |
| Java API单元测试 | Java层逻辑 | JUnit 5 |
| 集成测试 | JNI+Java联合 | JUnit 5 |
| UI测试 | JavaFX界面 | TestFX |
| 性能测试 | 渲染性能 | JMH |

### 9.2 集成测试设计

**JNI集成测试**:

```java
@Test
@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
class JniIntegrationTest {
    
    private static ChartViewer viewer;
    
    @BeforeAll
    static void setup() {
        LibraryLoader.loadNativeLibraries();
        viewer = new ChartViewer();
    }
    
    @AfterAll
    static void teardown() {
        if (viewer != null) {
            viewer.close();
        }
    }
    
    @Test
    @Order(1)
    void testJniInitialization() {
        assertNotNull(viewer);
        assertFalse(viewer.isDisposed());
    }
    
    @Test
    @Order(2)
    void testLoadChartIntegration() {
        String testChart = getResourcePath("/test_data/sample.000");
        assertDoesNotThrow(() -> viewer.loadChart(testChart));
    }
    
    @Test
    @Order(3)
    void testRenderIntegration() {
        GraphicsContext mockGc = createMockGraphicsContext();
        assertDoesNotThrow(() -> viewer.render(mockGc, 800, 600));
    }
    
    @Test
    @Order(4)
    void testQueryFeatureIntegration() {
        viewer.loadChart(getResourcePath("/test_data/sample.000"));
        
        FeatureInfo feature = viewer.queryFeature(400, 300);
        assertNotNull(feature);
        assertNotNull(feature.getFeatureId());
        assertNotNull(feature.getFeatureType());
    }
    
    @Test
    @Order(5)
    void testMultiThreadedAccess() throws InterruptedException {
        int threadCount = 10;
        CountDownLatch latch = new CountDownLatch(threadCount);
        AtomicInteger successCount = new AtomicInteger(0);
        
        for (int i = 0; i < threadCount; i++) {
            new Thread(() -> {
                try {
                    viewer.pan(10, 10);
                    viewer.zoom(1.1, 400, 300);
                    successCount.incrementAndGet();
                } finally {
                    latch.countDown();
                }
            }).start();
        }
        
        assertTrue(latch.await(10, TimeUnit.SECONDS));
        assertEquals(threadCount, successCount.get());
    }
    
    @Test
    @Order(6)
    void testMemoryLeakDetection() {
        long initialMemory = getNativeMemoryUsage();
        
        for (int i = 0; i < 100; i++) {
            FeatureInfo feature = viewer.queryFeature(400, 300);
            if (feature != null && feature.getGeometry() != null) {
                feature.getGeometry().close();
            }
        }
        
        System.gc();
        long finalMemory = getNativeMemoryUsage();
        
        assertTrue(finalMemory - initialMemory < 1024 * 1024, 
            "Potential memory leak detected");
    }
}
```

**JavaFX UI集成测试**:

```java
@Test
class ChartCanvasIntegrationTest extends TestFX {
    
    private ChartCanvas canvas;
    private ChartViewer viewer;
    
    @Override
    public void start(Stage stage) {
        LibraryLoader.loadNativeLibraries();
        viewer = new ChartViewer();
        canvas = new ChartCanvas(viewer);
        
        Scene scene = new Scene(new StackPane(canvas), 800, 600);
        stage.setScene(scene);
        stage.show();
    }
    
    @Test
    void testCanvasInitialization() {
        WaitForAsyncUtils.waitForFxEvents();
        assertEquals(800, canvas.getWidth(), 1.0);
        assertEquals(600, canvas.getHeight(), 1.0);
    }
    
    @Test
    void testMousePanInteraction() {
        viewer.loadChart(getTestChartPath());
        WaitForAsyncUtils.waitForFxEvents();
        
        double initialCenterX = canvas.centerXProperty().get();
        
        drag(canvas, MouseButton.PRIMARY)
            .moveBy(100, 0)
            .release();
        
        WaitForAsyncUtils.waitForFxEvents();
        
        assertNotEquals(initialCenterX, canvas.centerXProperty().get());
    }
    
    @Test
    void testScrollZoomInteraction() {
        viewer.loadChart(getTestChartPath());
        WaitForAsyncUtils.waitForFxEvents();
        
        double initialZoom = canvas.zoomLevelProperty().get();
        
        scroll(ScrollEvent.VerticalDirection.DOWN, 10)
            .on(canvas);
        
        WaitForAsyncUtils.waitForFxEvents();
        
        assertTrue(canvas.zoomLevelProperty().get() < initialZoom);
    }
    
    @Test
    void testTouchZoomGesture() {
        viewer.loadChart(getTestChartPath());
        WaitForAsyncUtils.waitForFxEvents();
        
        double initialZoom = canvas.zoomLevelProperty().get();
        
        Point2D center = new Point2D(400, 300);
        zoomBy(2.0, center);
        
        WaitForAsyncUtils.waitForFxEvents();
        
        assertTrue(canvas.zoomLevelProperty().get() > initialZoom);
    }
    
    @AfterEach
    void cleanup() {
        if (viewer != null) {
            viewer.close();
        }
    }
}
```

### 9.3 性能测试设计

**基准测试配置**:

```java
@State(Scope.Benchmark)
@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@Warmup(iterations = 3, time = 1)
@Measurement(iterations = 5, time = 1)
@Fork(1)
public class ChartPerformanceBenchmark {
    
    private ChartViewer viewer;
    private GraphicsContext mockGc;
    
    @Setup
    public void setup() {
        LibraryLoader.loadNativeLibraries();
        viewer = new ChartViewer();
        viewer.loadChart(getLargeChartPath());
        mockGc = createMockGraphicsContext();
    }
    
    @TearDown
    public void teardown() {
        if (viewer != null) {
            viewer.close();
        }
    }
    
    @Benchmark
    public void benchmarkRender() {
        viewer.render(mockGc, 1920, 1080);
    }
    
    @Benchmark
    public void benchmarkPan() {
        viewer.pan(100, 100);
    }
    
    @Benchmark
    public void benchmarkZoom() {
        viewer.zoom(1.1, 960, 540);
    }
    
    @Benchmark
    public void benchmarkQueryFeature() {
        viewer.queryFeature(960, 540);
    }
    
    @Benchmark
    public void benchmarkFullRenderCycle() {
        viewer.pan(50, 50);
        viewer.zoom(1.05, 960, 540);
        viewer.render(mockGc, 1920, 1080);
    }
}
```

**压力测试**:

```java
class StressTest {
    
    @Test
    void testLargeDatasetRendering() {
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart(getLargeChartPath(100 * 1024 * 1024));  // 100MB
            
            long startTime = System.currentTimeMillis();
            
            for (int i = 0; i < 100; i++) {
                viewer.render(mockGc, 1920, 1080);
                viewer.pan(10, 10);
            }
            
            long duration = System.currentTimeMillis() - startTime;
            assertTrue(duration < 10000, "Rendering too slow: " + duration + "ms");
        }
    }
    
    @Test
    void testMemoryPressure() {
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart(getLargeChartPath());
            
            long maxMemory = Runtime.getRuntime().maxMemory();
            long threshold = (long) (maxMemory * 0.8);
            
            for (int i = 0; i < 1000; i++) {
                FeatureInfo feature = viewer.queryFeature(
                    Math.random() * 1920, Math.random() * 1080);
                
                long usedMemory = Runtime.getRuntime().totalMemory() 
                    - Runtime.getRuntime().freeMemory();
                
                assertTrue(usedMemory < threshold, 
                    "Memory usage exceeded threshold: " + usedMemory);
            }
        }
    }
    
    @Test
    void testConcurrentAccess() throws Exception {
        int threadCount = 20;
        int operationsPerThread = 100;
        
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart(getTestChartPath());
            
            ExecutorService executor = Executors.newFixedThreadPool(threadCount);
            CountDownLatch latch = new CountDownLatch(threadCount);
            AtomicInteger errorCount = new AtomicInteger(0);
            
            for (int i = 0; i < threadCount; i++) {
                executor.submit(() -> {
                    try {
                        for (int j = 0; j < operationsPerThread; j++) {
                            viewer.pan(1, 1);
                            viewer.zoom(1.001, 960, 540);
                            viewer.queryFeature(960, 540);
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    } finally {
                        latch.countDown();
                    }
                });
            }
            
            assertTrue(latch.await(30, TimeUnit.SECONDS));
            assertEquals(0, errorCount.get(), "Errors during concurrent access");
            
            executor.shutdown();
        }
    }
}
```

**内存泄漏测试**:

```java
class MemoryLeakTest {
    
    @Test
    void testNoMemoryLeakInGeometry() {
        long initialMemory = getNativeMemoryUsage();
        
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart(getTestChartPath());
            
            for (int i = 0; i < 1000; i++) {
                FeatureInfo feature = viewer.queryFeature(400, 300);
                if (feature != null && feature.getGeometry() != null) {
                    feature.getGeometry().close();
                }
            }
        }
        
        System.gc();
        System.runFinalization();
        
        long finalMemory = getNativeMemoryUsage();
        long leaked = finalMemory - initialMemory;
        
        assertTrue(leaked < 1024 * 1024, 
            "Potential memory leak: " + (leaked / 1024) + "KB leaked");
    }
    
    @Test
    void testNoMemoryLeakInLongRunningSession() {
        long initialMemory = getNativeMemoryUsage();
        
        try (ChartViewer viewer = new ChartViewer()) {
            viewer.loadChart(getTestChartPath());
            
            for (int hour = 0; hour < 24; hour++) {
                for (int minute = 0; minute < 60; minute++) {
                    viewer.pan(1, 1);
                    viewer.zoom(1.0001, 960, 540);
                    
                    if (minute % 10 == 0) {
                        viewer.queryFeature(960, 540);
                    }
                }
                
                System.gc();
                long currentMemory = getNativeMemoryUsage();
                assertTrue(currentMemory - initialMemory < 50 * 1024 * 1024,
                    "Memory leak detected at hour " + hour);
            }
        }
    }
    
    private long getNativeMemoryUsage() {
        return NativeMemoryMonitor.getNativeMemoryUsed();
    }
}
```

### 9.4 测试用例

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

## 9. 日志与部署

### 9.1 日志系统集成

**架构设计**:

```
┌─────────────────────────────────────────────────────────────────┐
│                        日志系统架构                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐      ┌─────────────────┐                  │
│  │  Java层日志     │      │  C++层日志      │                  │
│  │  (SLF4J/Logback)│      │  (spdlog)       │                  │
│  └────────┬────────┘      └────────┬────────┘                  │
│           │                        │                            │
│           │                        ▼                            │
│           │              ┌─────────────────┐                    │
│           │              │ JNI日志桥接     │                    │
│           │              │ JniLogBridge    │                    │
│           │              └────────┬────────┘                    │
│           │                       │                             │
│           ▼                       ▼                             │
│  ┌─────────────────────────────────────────────┐                │
│  │           统一日志输出                       │                │
│  │  - 控制台输出                                │                │
│  │  - 文件输出（按日期滚动）                     │                │
│  │  - 异步输出（高性能）                        │                │
│  └─────────────────────────────────────────────┘                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**C++层日志桥接**:

```cpp
namespace ogc::jni {

class JniLogBridge {
public:
    static void Initialize(JNIEnv* env) {
        jclass logClass = env->FindClass("ogc/chart/internal/NativeLog");
        s_logClass = reinterpret_cast<jclass>(env->NewGlobalRef(logClass));
        s_logMethod = env->GetStaticMethodID(s_logClass, "log", 
            "(ILjava/lang/String;Ljava/lang/String;)V");
    }
    
    static void Log(int level, const std::string& tag, const std::string& message) {
        JNIEnv* env = JniEnvManager::GetInstance()->AttachCurrentThread();
        if (env && s_logClass) {
            jstring jTag = env->NewStringUTF(tag.c_str());
            jstring jMsg = env->NewStringUTF(message.c_str());
            env->CallStaticVoidMethod(s_logClass, s_logMethod, level, jTag, jMsg);
            env->DeleteLocalRef(jTag);
            env->DeleteLocalRef(jMsg);
        }
    }
    
private:
    static jclass s_logClass;
    static jmethodID s_logMethod;
};

// 日志宏
#define JNI_LOG_DEBUG(tag, msg) JniLogBridge::Log(0, tag, msg)
#define JNI_LOG_INFO(tag, msg)  JniLogBridge::Log(1, tag, msg)
#define JNI_LOG_WARN(tag, msg)  JniLogBridge::Log(2, tag, msg)
#define JNI_LOG_ERROR(tag, msg) JniLogBridge::Log(3, tag, msg)

}
```

**Java层日志接收**:

```java
package ogc.chart.internal;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class NativeLog {
    
    private static final Logger LOG = LoggerFactory.getLogger("Native");
    
    public static void log(int level, String tag, String message) {
        String fullMessage = "[" + tag + "] " + message;
        switch (level) {
            case 0: LOG.debug(fullMessage); break;
            case 1: LOG.info(fullMessage); break;
            case 2: LOG.warn(fullMessage); break;
            case 3: LOG.error(fullMessage); break;
        }
    }
}
```

### 9.2 部署与打包

**打包结构**:

```
chart-app/
├── lib/
│   ├── ogc_chart_jni.dll      # Windows
│   ├── libogc_chart_jni.so    # Linux
│   ├── libogc_chart_jni.dylib # macOS
│   ├── ogc_base.dll
│   ├── ogc_geom.dll
│   ├── ogc_proj.dll
│   └── ... (其他依赖库)
├── chart-app.jar              # 主应用JAR
├── lib/                       # Java依赖库
│   ├── javafx-controls.jar
│   ├── javafx-graphics.jar
│   └── ... (其他Java依赖)
└── config/
    ├── logback.xml            # 日志配置
    └── chart.properties       # 应用配置
```

**jpackage配置**:

```bash
# Windows打包
jpackage --name ChartApp \
    --input target/libs \
    --main-jar chart-app.jar \
    --main-class ogc.chart.app.Main \
    --java-options "--enable-preview" \
    --win-dir-chooser \
    --win-menu \
    --win-shortcut \
    --native-image-dir target/native

# Linux打包
jpackage --name ChartApp \
    --input target/libs \
    --main-jar chart-app.jar \
    --main-class ogc.chart.app.Main \
    --linux-deb-maintainer admin@example.com \
    --linux-menu-group "Graphics" \
    --native-image-dir target/native

# macOS打包
jpackage --name ChartApp \
    --input target/libs \
    --main-jar chart-app.jar \
    --main-class ogc.chart.app.Main \
    --mac-package-identifier com.example.chartapp \
    --native-image-dir target/native
```

**库路径配置**:

```java
public class LibraryLoader {
    
    private static final String[] NATIVE_LIBS = {
        "ogc_base", "ogc_geom", "ogc_proj", "ogc_chart_jni"
    };
    
    public static void loadNativeLibraries() {
        String osName = System.getProperty("os.name").toLowerCase();
        String osArch = System.getProperty("os.arch");
        String libPath = getLibraryPath(osName, osArch);
        
        // 设置库搜索路径
        System.setProperty("java.library.path", libPath);
        
        // 按依赖顺序加载库
        for (String lib : NATIVE_LIBS) {
            System.loadLibrary(lib);
        }
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
}
```

### 9.3 多平台构建配置

**Maven配置**:

```xml
<profiles>
    <profile>
        <id>windows-x64</id>
        <activation>
            <os><family>windows</family></os>
            <property><name>os.arch</name><value>amd64</value></property>
        </activation>
        <properties>
            <native.path>${project.basedir}/native/windows-x64</native.path>
        </properties>
    </profile>
    <profile>
        <id>linux-x64</id>
        <activation>
            <os><family>linux</family></os>
            <property><name>os.arch</name><value>amd64</value></property>
        </activation>
        <properties>
            <native.path>${project.basedir}/native/linux-x64</native.path>
        </properties>
    </profile>
    <profile>
        <id>macos-x64</id>
        <activation>
            <os><family>mac</family></os>
            <property><name>os.arch</name><value>amd64</value></property>
        </activation>
        <properties>
            <native.path>${project.basedir}/native/macos-x64</native.path>
        </properties>
    </profile>
</profiles>
```

---

## 10. 附录

### 10.0 安全性设计

**安全架构**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           安全架构                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    输入验证层                                        │   │
│  │  - 文件路径验证                                                      │   │
│  │  - 坐标范围验证                                                      │   │
│  │  - 配置参数验证                                                      │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    访问控制层                                        │   │
│  │  - 文件访问权限检查                                                  │   │
│  │  - 网络访问控制                                                      │   │
│  │  - Native库加载验证                                                  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    数据保护层                                        │   │
│  │  - 敏感数据加密                                                      │   │
│  │  - 日志脱敏                                                          │   │
│  │  - 内存安全清理                                                      │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**输入验证**:

```java
public class InputValidator {
    
    private static final Pattern SAFE_PATH_PATTERN = 
        Pattern.compile("^[a-zA-Z0-9_\\-./]+$");
    private static final Set<String> ALLOWED_EXTENSIONS = 
        Set.of(".000", ".s57", ".s101");
    
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
}
```

**Native库加载安全**:

```java
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
}
```

**敏感数据保护**:

```java
public class SensitiveDataProtector {
    
    private static final Pattern SENSITIVE_PATTERNS = Pattern.compile(
        "(password|token|key|secret)\\s*=\\s*[^\\s]+",
        Pattern.CASE_INSENSITIVE
    );
    
    public static String sanitizeLogMessage(String message) {
        return SENSITIVE_PATTERNS.matcher(message)
            .replaceAll("$1=***REDACTED***");
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
}
```

### 10.1 国际化设计

**国际化架构**:

```java
public class I18nManager {
    
    private static final String BUNDLE_BASE = "i18n.messages";
    private static final Map<Locale, ResourceBundle> bundles = new ConcurrentHashMap<>();
    
    private static final AtomicReference<Locale> currentLocale = 
        new AtomicReference<>(Locale.getDefault());
    
    public static void setLocale(Locale locale) {
        currentLocale.set(locale);
        Locale.setDefault(locale);
        notifyLocaleChange(locale);
    }
    
    public static String getString(String key) {
        return getBundle().getString(key);
    }
    
    public static String getString(String key, Object... args) {
        String pattern = getBundle().getString(key);
        return MessageFormat.format(pattern, args);
    }
    
    private static ResourceBundle getBundle() {
        return bundles.computeIfAbsent(currentLocale.get(), 
            locale -> ResourceBundle.getBundle(BUNDLE_BASE, locale));
    }
}
```

**多语言资源文件**:

```properties
# messages_zh_CN.properties
app.title=海图显示系统
menu.file=文件
menu.edit=编辑
menu.view=视图
menu.help=帮助
button.load=加载海图
button.export=导出
label.coordinates=坐标
label.scale=比例尺
error.load.failed=加载海图失败: {0}
error.memory.low=内存不足，已清理缓存
```

```properties
# messages_en_US.properties
app.title=Chart Display System
menu.file=File
menu.edit=Edit
menu.view=View
menu.help=Help
button.load=Load Chart
button.export=Export
label.coordinates=Coordinates
label.scale=Scale
error.load.failed=Failed to load chart: {0}
error.memory.low=Memory low, cache cleared
```

**JavaFX国际化绑定**:

```java
public class I18nBindings {
    
    public static StringBinding createStringBinding(String key) {
        return Bindings.createStringBinding(
            () -> I18nManager.getString(key),
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

// 使用示例
public class MainView extends BorderPane {
    
    public MainView() {
        Button loadButton = new Button();
        I18nBindings.bindText(loadButton, "button.load");
        
        Label coordLabel = new Label();
        I18nBindings.bindText(coordLabel, "label.coordinates");
    }
}
```

### 10.2 可访问性设计

**无障碍支持**:

```java
public class AccessibilityHelper {
    
    public static void setupAccessibility(Node node) {
        node.setFocusTraversable(true);
        
        node.addEventHandler(KeyEvent.KEY_PRESSED, event -> {
            if (event.getCode() == KeyCode.ENTER || 
                event.getCode() == KeyCode.SPACE) {
                node.fireEvent(new ActionEvent());
            }
        });
    }
    
    public static void setAccessibleInfo(Node node, String name, String description) {
        node.setAccessibleText(name);
        node.setAccessibleHelp(description);
    }
    
    public static void announceForAccessibility(String message) {
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText(message);
            alert.show();
        });
    }
}

// 使用示例
public class ChartCanvas extends Canvas {
    
    public ChartCanvas() {
        AccessibilityHelper.setupAccessibility(this);
        AccessibilityHelper.setAccessibleInfo(this,
            I18nManager.getString("canvas.name"),
            I18nManager.getString("canvas.description")
        );
    }
}
```

### 10.3 实施计划

| 阶段 | 任务 | 工时 | 交付物 |
|------|------|------|--------|
| 阶段1 | JNI桥接层完善 | 1周 | jni_bridge模块完整实现 |
| 阶段2 | Java API层开发 | 2周 | java_api模块完整实现 |
| 阶段3 | JavaFX适配层开发 | 2周 | javafx_adapter模块完整实现 |
| 阶段4 | JavaFX应用层开发 | 3周 | javafx_app可运行应用 |
| 阶段5 | 测试与优化 | 2周 | 测试报告、性能优化 |

### 10.4 运维监控设计

**健康检查接口**:

```java
public interface HealthCheck {
    
    HealthStatus check();
    
    enum HealthStatus {
        HEALTHY,    // 健康
        DEGRADED,   // 降级
        UNHEALTHY   // 不健康
    }
}

public class ChartApplicationHealthCheck implements HealthCheck {
    
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

**监控指标导出**:

```java
public class MetricsExporter {
    
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
}
```

### 10.5 参考文档

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
