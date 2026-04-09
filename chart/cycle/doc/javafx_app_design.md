# JNI与Java Wrapper层封装设计文档

**文档编号**: JNI-WRAPPER-DESIGN-001  
**版本**: v1.0  
**日期**: 2026-04-10  
**状态**: 设计中  
**依据文档**: [javafx_chart_application_design.md](javafx_chart_application_design.md) | [jni_java_interface.md](jni_java_interface.md) | [sdk_c_api.h](../../code/cycle/chart_c_api/sdk_c_api.h)

---

## 文档修订历史

| 版本 | 日期 | 作者 | 修订内容 |
|------|------|------|----------|
| v1.0 | 2026-04-10 | Team | 初始版本，整合JNI层和Java Wrapper层封装方案 |

---

## 目录

1. [概述](#1-概述)
2. [项目结构](#2-项目结构)
3. [JNI层封装设计](#3-jni层封装设计)
4. [Java Wrapper层封装设计](#4-java-wrapper层封装设计)
5. [FXRibbon集成方案](#5-fxribbon集成方案)
6. [性能优化设计](#6-性能优化设计)
7. [错误处理与资源管理](#7-错误处理与资源管理)
8. [线程安全设计](#8-线程安全设计)
9. [测试策略](#9-测试策略)
10. [附录](#10-附录)

---

## 1. 概述

### 1.1 设计目标

本文档详细描述基于现有C++核心模块和C API工程，封装JNI层和Java Wrapper层的完整方案，确保：

- **高质量**: 错误处理完整、资源不泄漏、线程安全、异常映射清晰
- **高效**: 最小化数据拷贝、缓存JNI引用、批量操作优化
- **易维护**: JNI层薄转换、业务逻辑在C API层、Java层友好封装

### 1.2 技术约束

| 约束项 | 要求 | 说明 |
|--------|------|------|
| Java版本 | Java 17 LTS | 长期支持版本 |
| JNI版本 | JNI 1.8 | 兼容Java 17+ |
| UI框架 | JavaFX 21 LTS | 长期支持版本 |
| 构建工具 | Gradle 8.x | 多模块项目支持 |
| C++标准 | C++11 | 与核心模块一致 |
| 线程模型 | 单线程操作 | ChartViewer等核心对象非线程安全 |

### 1.3 架构总览

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           JavaFX应用架构                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX应用层 (Application Layer)                  │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainView      │  │ ChartCanvas   │  │ RibbonBar     │           │   │
│  │  │ 主界面        │  │ 海图画布      │  │ Ribbon工具栏  │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java Wrapper层 (Wrapper Layer)                    │   │
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
│  │                    JNI桥接层 (JNI Bridge Layer)                      │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ JniEnvManager │  │ JniConverter  │  │ JniException  │           │   │
│  │  │ 环境管理      │  │ 数据转换      │  │ 异常处理      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐                              │   │
│  │  │ JniMemory     │  │ JniReference  │                              │   │
│  │  │ 内存管理      │  │ 引用缓存      │              │   │
│  │  └───────────────┘  └───────────────┘                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C API层 (C API Layer)                             │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ogc_*_create  │  │ ogc_*_destroy │  │ ogc_*_get/set │           │   │
│  │  │ 对象创建      │  │ 对象销毁      │  │ 属性访问      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (Core Layer)                            │   │
│  │  ogc_graph | ogc_draw | ogc_layer | ogc_symbology | ogc_cache      │   │
│  │  ogc_feature | ogc_geom | ogc_proj | ogc_database | ogc_base       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. 项目结构

### 2.1 多模块Gradle项目结构

```
项目根目录/
├── cpp/                           # C++核心工程（已有）
│   ├── core/                      # C++核心模块
│   └── c_api/                     # C API工程
│       ├── include/
│       │   └── sdk_c_api.h        # C API头文件
│       └── src/
│           └── sdk_c_api.cpp      # C API实现
│
├── jni/                           # JNI封装工程
│   ├── src/main/cpp/
│   │   ├── jni_env_manager.cpp    # JNI环境管理
│   │   ├── jni_converter.cpp      # 数据类型转换
│   │   ├── jni_exception.cpp      # 异常处理
│   │   ├── jni_memory.cpp         # 内存管理
│   │   ├── jni_reference_cache.cpp # 引用缓存
│   │   └── native/                # Native方法实现
│   │       ├── chart_viewer_jni.cpp
│   │       ├── geometry_jni.cpp
│   │       └── feature_jni.cpp
│   ├── include/
│   │   ├── jni_env_manager.h
│   │   ├── jni_converter.h
│   │   ├── jni_exception.h
│   │   ├── jni_memory.h
│   │   └── jni_reference_cache.h
│   └── CMakeLists.txt             # 构建JNI动态库
│
├── javawrapper/                   # Java Wrapper模块
│   ├── src/main/java/cn/cycle/chart/
│   │   ├── jni/                   # JNI桥接类
│   │   │   ├── JniBridge.java
│   │   │   ├── NativeObject.java
│   │   │   └── NativeHandle.java
│   │   ├── api/                   # Java API层
│   │   │   ├── core/
│   │   │   │   ├── ChartViewer.java
│   │   │   │   ├── ChartConfig.java
│   │   │   │   ├── Viewport.java
│   │   │   │   └── RenderContext.java
│   │   │   ├── geometry/
│   │   │   │   ├── Geometry.java
│   │   │   │   ├── Point.java
│   │   │   │   ├── LineString.java
│   │   │   │   ├── Polygon.java
│   │   │   │   ├── Envelope.java
│   │   │   │   └── Coordinate.java
│   │   │   ├── feature/
│   │   │   │   ├── FeatureInfo.java
│   │   │   │   ├── FieldValue.java
│   │   │   │   └── FieldDefn.java
│   │   │   ├── layer/
│   │   │   │   ├── ChartLayer.java
│   │   │   │   ├── VectorLayer.java
│   │   │   │   └── LayerManager.java
│   │   │   └── util/
│   │   │       ├── Color.java
│   │   │       └── Transform.java
│   │   └── internal/              # 内部实现
│   │       ├── NativeMethods.java
│   │       └── ResourceCleaner.java
│   └── build.gradle
│
├── javafx-app/                    # JavaFX界面应用
│   ├── src/main/java/cn/cycle/app/
│   │   ├── MainApp.java           # 应用入口
│   │   ├── view/
│   │   │   ├── MainView.java      # 主界面
│   │   │   ├── ChartCanvas.java   # 海图画布
│   │   │   ├── RibbonBar.java     # Ribbon工具栏
│   │   │   ├── LayerPanel.java    # 图层面板
│   │   │   └── SettingsView.java  # 设置界面
│   │   ├── controller/
│   │   │   ├── MainController.java
│   │   │   └── ChartController.java
│   │   └── util/
│   │       ├── I18nManager.java
│   │       └── ConfigManager.java
│   ├── src/main/resources/
│   │   ├── fxml/
│   │   │   └── main_view.fxml
│   │   └── css/
│   │       └── style.css
│   └── build.gradle
│
├── fxribbon/                      # FXRibbon库（第三方）
│   └── (FXRibbon-master源码)      # cycle\FXRibbon-master
│
├── install/                       # 安装目录（已有）
│   ├── include/                   # 头文件
│   ├── lib/                       # 库文件
│   └── bin/                       # 动态库
│
└── settings.gradle                # Gradle多模块配置
```

### 2.2 Gradle配置

#### settings.gradle

```gradle
rootProject.name = 'cycle-chart'

include 'javawrapper'
include 'javafx-app'

project(':javawrapper').projectDir = file('javawrapper')
project(':javafx-app').projectDir = file('javafx-app')
```

#### javawrapper/build.gradle

```gradle
plugins {
    id 'java-library'
}

group 'cn.cycle.chart'
version '1.0.0'

java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation 'org.junit.jupiter:junit-jupiter:5.9.2'
}

tasks.withType(JavaCompile) {
    options.encoding = 'UTF-8'
}

jar {
    manifest {
        attributes(
            'Implementation-Title': 'Cycle Chart Java Wrapper',
            'Implementation-Version': version
        )
    }
}
```

#### javafx-app/build.gradle

```gradle
plugins {
    id 'application'
    id 'org.openjfx.javafxplugin' version '0.1.0'
}

group 'cn.cycle.app'
version '1.0.0'

java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}

repositories {
    mavenCentral()
    flatDir {
        dirs '../fxribbon/build/libs'
    }
}

javafx {
    version = '21'
    modules = ['javafx.controls', 'javafx.fxml', 'javafx.graphics']
}

dependencies {
    implementation project(':javawrapper')
    implementation name: 'FXRibbon-1.2.0'
    testImplementation 'org.junit.jupiter:junit-jupiter:5.9.2'
}

application {
    mainClass = 'cn.cycle.app.MainApp'
}

tasks.withType(JavaCompile) {
    options.encoding = 'UTF-8'
}

run {
    jvmArgs = [
        '-Djava.library.path=../install/bin',
        '--add-exports=javafx.controls/com.sun.javafx.scene.control.behavior=ALL-UNNAMED',
        '--add-exports=javafx.controls/com.sun.javafx.scene.control=ALL-UNNAMED'
    ]
}
```

---

## 3. JNI层封装设计

### 3.1 设计原则

| 原则 | 说明 |
|------|------|
| **薄层转换** | JNI层只做类型转换，不包含业务逻辑 |
| **异常映射** | C++异常映射为Java异常，保持调用栈清晰 |
| **资源管理** | 使用RAII管理JNI局部引用和全局引用 |
| **性能优化** | 缓存jmethodID/jfieldID，减少查找开销 |
| **线程安全** | 明确线程模型，提供线程安全保证 |

### 3.2 JNI环境管理

#### jni_env_manager.h

```cpp
#ifndef JNI_ENV_MANAGER_H
#define JNI_ENV_MANAGER_H

#include <jni.h>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace ogc {
namespace jni {

class JniEnvManager {
public:
    static JniEnvManager* GetInstance();
    
    void Initialize(JavaVM* vm);
    void Shutdown();
    
    JNIEnv* GetEnv();
    JNIEnv* AttachCurrentThread();
    void DetachCurrentThread();
    
    bool IsInitialized() const { return m_initialized; }
    JavaVM* GetJavaVM() const { return m_javaVM; }
    
    void RegisterNatives(const std::string& className,
                        const JNINativeMethod* methods,
                        int numMethods);
    
    jclass FindClass(const std::string& className);
    jclass GetGlobalClassRef(const std::string& className);
    void ReleaseGlobalClassRef(jclass clazz);
    
    jmethodID GetMethodID(jclass clazz, const std::string& name, 
                          const std::string& signature);
    jmethodID GetStaticMethodID(jclass clazz, const std::string& name,
                                const std::string& signature);
    jfieldID GetFieldID(jclass clazz, const std::string& name,
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
    
    std::unordered_map<std::string, jclass> m_globalClassRefs;
    std::unordered_map<std::string, jmethodID> m_methodIDCache;
    std::unordered_map<std::string, jfieldID> m_fieldIDCache;
    std::unordered_map<std::thread::id, bool> m_attachedThreads;
};

}
}

#endif // JNI_ENV_MANAGER_H
```

#### jni_env_manager.cpp

```cpp
#include "jni_env_manager.h"
#include "jni_exception.h"
#include <sstream>

namespace ogc {
namespace jni {

JniEnvManager* JniEnvManager::GetInstance() {
    static JniEnvManager instance;
    return &instance;
}

JniEnvManager::JniEnvManager() 
    : m_javaVM(nullptr), m_initialized(false) {
}

JniEnvManager::~JniEnvManager() {
    Shutdown();
}

void JniEnvManager::Initialize(JavaVM* vm) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return;
    }
    
    m_javaVM = vm;
    m_initialized = true;
}

void JniEnvManager::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) {
        return;
    }
    
    for (auto& pair : m_globalClassRefs) {
        JNIEnv* env = GetEnv();
        if (env && pair.second) {
            env->DeleteGlobalRef(pair.second);
        }
    }
    m_globalClassRefs.clear();
    m_methodIDCache.clear();
    m_fieldIDCache.clear();
    
    for (auto& pair : m_attachedThreads) {
        if (pair.second && m_javaVM) {
            m_javaVM->DetachCurrentThread();
        }
    }
    m_attachedThreads.clear();
    
    m_javaVM = nullptr;
    m_initialized = false;
}

JNIEnv* JniEnvManager::GetEnv() {
    if (!m_javaVM) {
        return nullptr;
    }
    
    JNIEnv* env = nullptr;
    jint result = m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8);
    
    if (result == JNI_OK) {
        return env;
    }
    return nullptr;
}

JNIEnv* JniEnvManager::AttachCurrentThread() {
    if (!m_javaVM) {
        return nullptr;
    }
    
    JNIEnv* env = GetEnv();
    if (env) {
        return env;
    }
    
    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_attachedThreads.find(tid) == m_attachedThreads.end()) {
        JavaVMAttachArgs args = {JNI_VERSION_1_8, "JNI Thread", nullptr};
        m_javaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), &args);
        m_attachedThreads[tid] = true;
    } else {
        env = GetEnv();
    }
    
    return env;
}

void JniEnvManager::DetachCurrentThread() {
    if (!m_javaVM) {
        return;
    }
    
    std::thread::id tid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_attachedThreads.find(tid) != m_attachedThreads.end()) {
        m_javaVM->DetachCurrentThread();
        m_attachedThreads.erase(tid);
    }
}

jclass JniEnvManager::GetGlobalClassRef(const std::string& className) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_globalClassRefs.find(className);
    if (it != m_globalClassRefs.end()) {
        return it->second;
    }
    
    JNIEnv* env = AttachCurrentThread();
    if (!env) {
        return nullptr;
    }
    
    jclass localRef = env->FindClass(className.c_str());
    if (!localRef) {
        return nullptr;
    }
    
    jclass globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(localRef));
    env->DeleteLocalRef(localRef);
    
    if (globalRef) {
        m_globalClassRefs[className] = globalRef;
    }
    
    return globalRef;
}

jmethodID JniEnvManager::GetMethodID(jclass clazz, const std::string& name, 
                                     const std::string& signature) {
    JNIEnv* env = AttachCurrentThread();
    if (!env || !clazz) {
        return nullptr;
    }
    
    std::string key = name + signature;
    auto it = m_methodIDCache.find(key);
    if (it != m_methodIDCache.end()) {
        return it->second;
    }
    
    jmethodID methodID = env->GetMethodID(clazz, name.c_str(), signature.c_str());
    if (methodID) {
        m_methodIDCache[key] = methodID;
    }
    
    return methodID;
}

}
}
```

### 3.3 数据类型转换

#### jni_converter.h

```cpp
#ifndef JNI_CONVERTER_H
#define JNI_CONVERTER_H

#include <jni.h>
#include <string>
#include <vector>
#include <cstdint>

namespace ogc {
namespace jni {

class JniConverter {
public:
    static std::string ToString(JNIEnv* env, jstring str);
    static jstring ToJString(JNIEnv* env, const std::string& str);
    
    static std::vector<std::string> ToStringVector(JNIEnv* env, jobjectArray array);
    static jobjectArray ToJStringArray(JNIEnv* env, const std::vector<std::string>& vec);
    
    static std::vector<double> ToDoubleVector(JNIEnv* env, jdoubleArray array);
    static jdoubleArray ToJDoubleArray(JNIEnv* env, const std::vector<double>& vec);
    
    static std::vector<int32_t> ToIntVector(JNIEnv* env, jintArray array);
    static jintArray ToJIntArray(JNIEnv* env, const std::vector<int32_t>& vec);
    
    static std::vector<uint8_t> ToByteVector(JNIEnv* env, jbyteArray array);
    static jbyteArray ToJByteArray(JNIEnv* env, const std::vector<uint8_t>& vec);
    
    static jlong ToJLongPtr(void* ptr);
    static void* FromJLongPtr(jlong ptr);
    
    static jobject CreateArrayList(JNIEnv* env);
    static void ArrayListAdd(JNIEnv* env, jobject list, jobject element);
    static size_t ArrayListSize(JNIEnv* env, jobject list);
    
    static jobject CreateHashMap(JNIEnv* env);
    static void HashMapPut(JNIEnv* env, jobject map, jobject key, jobject value);
    
    static jobject ToJObject(JNIEnv* env, void* ptr, const std::string& className);
    static void* FromJObject(JNIEnv* env, jobject obj, const std::string& fieldName);
    
    static jobject CreateCoordinate(JNIEnv* env, double x, double y);
    static jobject CreateEnvelope(JNIEnv* env, double minX, double minY, 
                                  double maxX, double maxY);
    
    static jobject CreateColor(JNIEnv* env, uint8_t r, uint8_t g, 
                               uint8_t b, uint8_t a = 255);
    
    template<typename T>
    static T* GetNativePtr(JNIEnv* env, jobject obj, jlong ptrField) {
        return reinterpret_cast<T*>(ptrField);
    }
    
    template<typename T>
    static void SetNativePtr(JNIEnv* env, jobject obj, jlong ptrField, T* ptr) {
        return reinterpret_cast<jlong>(ptr);
    }
};

}
}

#endif // JNI_CONVERTER_H
```

#### jni_converter.cpp

```cpp
#include "jni_converter.h"
#include "jni_env_manager.h"
#include "jni_exception.h"

namespace ogc {
namespace jni {

std::string JniConverter::ToString(JNIEnv* env, jstring str) {
    if (!str) {
        return "";
    }
    
    const char* chars = env->GetStringUTFChars(str, nullptr);
    if (!chars) {
        return "";
    }
    
    std::string result(chars);
    env->ReleaseStringUTFChars(str, chars);
    return result;
}

jstring JniConverter::ToJString(JNIEnv* env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

std::vector<double> JniConverter::ToDoubleVector(JNIEnv* env, jdoubleArray array) {
    std::vector<double> result;
    
    if (!array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    if (length == 0) {
        return result;
    }
    
    jdouble* elements = env->GetDoubleArrayElements(array, nullptr);
    if (!elements) {
        return result;
    }
    
    result.assign(elements, elements + length);
    env->ReleaseDoubleArrayElements(array, elements, JNI_ABORT);
    
    return result;
}

jdoubleArray JniConverter::ToJDoubleArray(JNIEnv* env, const std::vector<double>& vec) {
    if (vec.empty()) {
        return env->NewDoubleArray(0);
    }
    
    jdoubleArray array = env->NewDoubleArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetDoubleArrayRegion(array, 0, static_cast<jsize>(vec.size()), 
                              vec.data());
    return array;
}

std::vector<uint8_t> JniConverter::ToByteVector(JNIEnv* env, jbyteArray array) {
    std::vector<uint8_t> result;
    
    if (!array) {
        return result;
    }
    
    jsize length = env->GetArrayLength(array);
    if (length == 0) {
        return result;
    }
    
    jbyte* elements = env->GetByteArrayElements(array, nullptr);
    if (!elements) {
        return result;
    }
    
    result.assign(reinterpret_cast<uint8_t*>(elements), 
                  reinterpret_cast<uint8_t*>(elements) + length);
    env->ReleaseByteArrayElements(array, elements, JNI_ABORT);
    
    return result;
}

jbyteArray JniConverter::ToJByteArray(JNIEnv* env, const std::vector<uint8_t>& vec) {
    if (vec.empty()) {
        return env->NewByteArray(0);
    }
    
    jbyteArray array = env->NewByteArray(static_cast<jsize>(vec.size()));
    if (!array) {
        return nullptr;
    }
    
    env->SetByteArrayRegion(array, 0, static_cast<jsize>(vec.size()),
                            reinterpret_cast<const jbyte*>(vec.data()));
    return array;
}

jlong JniConverter::ToJLongPtr(void* ptr) {
    return reinterpret_cast<jlong>(ptr);
}

void* JniConverter::FromJLongPtr(jlong ptr) {
    return reinterpret_cast<void*>(ptr);
}

jobject JniConverter::CreateArrayList(JNIEnv* env) {
    jclass listClass = env->FindClass("java/util/ArrayList");
    if (!listClass) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(listClass, "<init>", "()V");
    if (!constructor) {
        return nullptr;
    }
    
    return env->NewObject(listClass, constructor);
}

void JniConverter::ArrayListAdd(JNIEnv* env, jobject list, jobject element) {
    if (!list || !element) {
        return;
    }
    
    jclass listClass = env->FindClass("java/util/ArrayList");
    if (!listClass) {
        return;
    }
    
    jmethodID addMethod = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");
    if (!addMethod) {
        return;
    }
    
    env->CallBooleanMethod(list, addMethod, element);
}

size_t JniConverter::ArrayListSize(JNIEnv* env, jobject list) {
    if (!list) {
        return 0;
    }
    
    jclass listClass = env->FindClass("java/util/ArrayList");
    if (!listClass) {
        return 0;
    }
    
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    if (!sizeMethod) {
        return 0;
    }
    
    return static_cast<size_t>(env->CallIntMethod(list, sizeMethod));
}

jobject JniConverter::CreateCoordinate(JNIEnv* env, double x, double y) {
    jclass coordClass = env->FindClass("cn/cycle/chart/api/geometry/Coordinate");
    if (!coordClass) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(coordClass, "<init>", "(DD)V");
    if (!constructor) {
        return nullptr;
    }
    
    return env->NewObject(coordClass, constructor, x, y);
}

jobject JniConverter::CreateEnvelope(JNIEnv* env, double minX, double minY, 
                                     double maxX, double maxY) {
    jclass envClass = env->FindClass("cn/cycle/chart/api/geometry/Envelope");
    if (!envClass) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(envClass, "<init>", "(DDDD)V");
    if (!constructor) {
        return nullptr;
    }
    
    return env->NewObject(envClass, constructor, minX, minY, maxX, maxY);
}

jobject JniConverter::CreateColor(JNIEnv* env, uint8_t r, uint8_t g, 
                                  uint8_t b, uint8_t a) {
    jclass colorClass = env->FindClass("cn/cycle/chart/api/util/Color");
    if (!colorClass) {
        return nullptr;
    }
    
    jmethodID constructor = env->GetMethodID(colorClass, "<init>", "(IIII)V");
    if (!constructor) {
        return nullptr;
    }
    
    return env->NewObject(colorClass, constructor, 
                          static_cast<jint>(r), static_cast<jint>(g),
                          static_cast<jint>(b), static_cast<jint>(a));
}

}
}
```

### 3.4 异常处理

#### jni_exception.h

```cpp
#ifndef JNI_EXCEPTION_H
#define JNI_EXCEPTION_H

#include <jni.h>
#include <string>
#include <exception>

namespace ogc {
namespace jni {

class JniException {
public:
    static void CheckException(JNIEnv* env);
    static void ThrowException(JNIEnv* env, const std::string& className, 
                               const std::string& message);
    static void ThrowRuntimeException(JNIEnv* env, const std::string& message);
    static void ThrowIOException(JNIEnv* env, const std::string& message);
    static void ThrowIllegalArgumentException(JNIEnv* env, const std::string& message);
    static void ThrowNullPointerException(JNIEnv* env, const std::string& message);
    static void ThrowOutOfMemoryError(JNIEnv* env, const std::string& message);
    
    static bool HasException(JNIEnv* env);
    static void ClearException(JNIEnv* env);
    
    static void TranslateAndThrow(JNIEnv* env, const std::exception& e);
    static void TranslateCError(JNIEnv* env, int errorCode, const std::string& context);
    
    static std::string GetExceptionMessage(JNIEnv* env, jthrowable exception);
};

class JniExceptionGuard {
public:
    JniExceptionGuard(JNIEnv* env) : m_env(env), m_hasException(false) {
        m_hasException = env->ExceptionCheck() == JNI_TRUE;
        if (m_hasException) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
    
    ~JniExceptionGuard() {
        if (!m_hasException && m_env->ExceptionCheck() == JNI_TRUE) {
            m_env->ExceptionDescribe();
        }
    }
    
    bool HasException() const { return m_hasException; }
    
private:
    JNIEnv* m_env;
    bool m_hasException;
};

}
}

#endif // JNI_EXCEPTION_H
```

#### jni_exception.cpp

```cpp
#include "jni_exception.h"
#include "jni_env_manager.h"
#include <sstream>

namespace ogc {
namespace jni {

void JniException::CheckException(JNIEnv* env) {
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void JniException::ThrowException(JNIEnv* env, const std::string& className, 
                                  const std::string& message) {
    jclass exClass = env->FindClass(className.c_str());
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    } else {
        ThrowRuntimeException(env, "Failed to find exception class: " + className);
    }
}

void JniException::ThrowRuntimeException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/RuntimeException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowIOException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/io/IOException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowIllegalArgumentException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/IllegalArgumentException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowNullPointerException(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/NullPointerException");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

void JniException::ThrowOutOfMemoryError(JNIEnv* env, const std::string& message) {
    jclass exClass = env->FindClass("java/lang/OutOfMemoryError");
    if (exClass) {
        env->ThrowNew(exClass, message.c_str());
    }
}

bool JniException::HasException(JNIEnv* env) {
    return env->ExceptionCheck() == JNI_TRUE;
}

void JniException::ClearException(JNIEnv* env) {
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionClear();
    }
}

void JniException::TranslateAndThrow(JNIEnv* env, const std::exception& e) {
    std::string message = e.what();
    
    if (dynamic_cast<const std::bad_alloc*>(&e)) {
        ThrowOutOfMemoryError(env, message);
    } else if (dynamic_cast<const std::invalid_argument*>(&e)) {
        ThrowIllegalArgumentException(env, message);
    } else {
        ThrowRuntimeException(env, message);
    }
}

void JniException::TranslateCError(JNIEnv* env, int errorCode, const std::string& context) {
    std::ostringstream oss;
    oss << context << " failed with error code: " << errorCode;
    
    switch (errorCode) {
        case -1:
            ThrowIllegalArgumentException(env, oss.str());
            break;
        case -2:
            ThrowNullPointerException(env, oss.str());
            break;
        case -3:
            ThrowOutOfMemoryError(env, oss.str());
            break;
        case -7:
            ThrowIOException(env, oss.str());
            break;
        default:
            ThrowRuntimeException(env, oss.str());
            break;
    }
}

std::string JniException::GetExceptionMessage(JNIEnv* env, jthrowable exception) {
    if (!exception) {
        return "";
    }
    
    jclass throwableClass = env->FindClass("java/lang/Throwable");
    if (!throwableClass) {
        return "";
    }
    
    jmethodID getMessageMethod = env->GetMethodID(throwableClass, "getMessage", 
                                                   "()Ljava/lang/String;");
    if (!getMessageMethod) {
        return "";
    }
    
    jstring message = static_cast<jstring>(
        env->CallObjectMethod(exception, getMessageMethod));
    
    if (!message) {
        return "";
    }
    
    const char* chars = env->GetStringUTFChars(message, nullptr);
    std::string result(chars ? chars : "");
    env->ReleaseStringUTFChars(message, chars);
    
    return result;
}

}
}
```

### 3.5 内存管理

#### jni_memory.h

```cpp
#ifndef JNI_MEMORY_H
#define JNI_MEMORY_H

#include <jni.h>

namespace ogc {
namespace jni {

template<typename T>
class JniLocalRef {
public:
    JniLocalRef(JNIEnv* env, T ref) : m_env(env), m_ref(ref) {}
    ~JniLocalRef() { 
        if (m_ref) {
            m_env->DeleteLocalRef(m_ref); 
        }
    }
    
    JniLocalRef(const JniLocalRef&) = delete;
    JniLocalRef& operator=(const JniLocalRef&) = delete;
    
    JniLocalRef(JniLocalRef&& other) noexcept 
        : m_env(other.m_env), m_ref(other.m_ref) {
        other.m_ref = nullptr;
    }
    
    JniLocalRef& operator=(JniLocalRef&& other) noexcept {
        if (this != &other) {
            if (m_ref) {
                m_env->DeleteLocalRef(m_ref);
            }
            m_env = other.m_env;
            m_ref = other.m_ref;
            other.m_ref = nullptr;
        }
        return *this;
    }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    T operator->() const { return m_ref; }
    bool operator!() const { return m_ref == nullptr; }
    operator bool() const { return m_ref != nullptr; }
    
private:
    JNIEnv* m_env;
    T m_ref;
};

template<typename T>
class JniGlobalRef {
public:
    JniGlobalRef(JNIEnv* env, T ref) : m_env(env), m_ref(nullptr) {
        if (ref) {
            m_ref = static_cast<T>(env->NewGlobalRef(ref));
        }
    }
    
    ~JniGlobalRef() { 
        if (m_ref) {
            m_env->DeleteGlobalRef(m_ref); 
        }
    }
    
    JniGlobalRef(const JniGlobalRef&) = delete;
    JniGlobalRef& operator=(const JniGlobalRef&) = delete;
    
    JniGlobalRef(JniGlobalRef&& other) noexcept 
        : m_env(other.m_env), m_ref(other.m_ref) {
        other.m_ref = nullptr;
    }
    
    JniGlobalRef& operator=(JniGlobalRef&& other) noexcept {
        if (this != &other) {
            if (m_ref) {
                m_env->DeleteGlobalRef(m_ref);
            }
            m_env = other.m_env;
            m_ref = other.m_ref;
            other.m_ref = nullptr;
        }
        return *this;
    }
    
    T Get() const { return m_ref; }
    operator T() const { return m_ref; }
    bool operator!() const { return m_ref == nullptr; }
    operator bool() const { return m_ref != nullptr; }
    
private:
    JNIEnv* m_env;
    T m_ref;
};

class JniLocalRefScope {
public:
    JniLocalRefScope(JNIEnv* env, jint capacity = 16) 
        : m_env(env), m_success(false) {
        m_success = (env->PushLocalFrame(capacity) == 0);
    }
    
    ~JniLocalRefScope() {
        if (m_success) {
            m_env->PopLocalFrame(nullptr);
        }
    }
    
    bool Success() const { return m_success; }
    
private:
    JNIEnv* m_env;
    bool m_success;
};

}
}

#endif // JNI_MEMORY_H
```

### 3.6 Native方法实现示例

#### chart_viewer_jni.cpp

```cpp
#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeCreate
  (JNIEnv* env, jobject obj) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    
    try {
        ogc_chart_viewer_t* viewer = ogc_chart_viewer_create();
        return JniConverter::ToJLongPtr(viewer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeCreateWithConfig
  (JNIEnv* env, jobject obj, jobject config) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }
    
    try {
        jclass configClass = env->GetObjectClass(config);
        
        jmethodID getCachePathMethod = env->GetMethodID(configClass, 
            "getCachePath", "()Ljava/lang/String;");
        jmethodID getMaxCacheSizeMethod = env->GetMethodID(configClass, 
            "getMaxCacheSize", "()J");
        jmethodID getThreadCountMethod = env->GetMethodID(configClass, 
            "getThreadCount", "()I");
        
        jstring cachePath = static_cast<jstring>(
            env->CallObjectMethod(config, getCachePathMethod));
        jlong maxCacheSize = env->CallLongMethod(config, getMaxCacheSizeMethod);
        jint threadCount = env->CallIntMethod(config, getThreadCountMethod);
        
        ogc_chart_config_t cConfig;
        cConfig.cache_path = JniConverter::ToString(env, cachePath).c_str();
        cConfig.max_cache_size = static_cast<size_t>(maxCacheSize);
        cConfig.thread_count = static_cast<int>(threadCount);
        
        ogc_chart_viewer_t* viewer = ogc_chart_viewer_create_with_config(&cConfig);
        return JniConverter::ToJLongPtr(viewer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeDispose
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (viewer) {
            ogc_chart_viewer_destroy(viewer);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeLoadChart
  (JNIEnv* env, jobject obj, jlong ptr, jstring filePath) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return JNI_FALSE;
    }
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return JNI_FALSE;
        }
        
        std::string path = JniConverter::ToString(env, filePath);
        int result = ogc_chart_viewer_load_chart(viewer, path.c_str());
        
        if (result != OGC_SUCCESS) {
            JniException::TranslateCError(env, result, "Load chart");
            return JNI_FALSE;
        }
        
        return JNI_TRUE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeSetViewport
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, 
   jdouble maxX, jdouble maxY) {
    JniLocalRefScope scope(env);
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return;
        }
        
        int result = ogc_chart_viewer_set_viewport(viewer, minX, minY, maxX, maxY);
        if (result != OGC_SUCCESS) {
            JniException::TranslateCError(env, result, "Set viewport");
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jobject JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewport
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return nullptr;
        }
        
        double minX, minY, maxX, maxY;
        int result = ogc_chart_viewer_get_viewport(viewer, &minX, &minY, &maxX, &maxY);
        
        if (result != OGC_SUCCESS) {
            JniException::TranslateCError(env, result, "Get viewport");
            return nullptr;
        }
        
        return JniConverter::CreateEnvelope(env, minX, minY, maxX, maxY);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jobject JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeQueryFeature
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return nullptr;
        }
        
        ogc_feature_t* feature = ogc_chart_viewer_query_feature(viewer, x, y);
        if (!feature) {
            return nullptr;
        }
        
        jclass featureInfoClass = env->FindClass("cn/cycle/chart/api/feature/FeatureInfo");
        if (!featureInfoClass) {
            ogc_feature_destroy(feature);
            return nullptr;
        }
        
        jmethodID constructor = env->GetMethodID(featureInfoClass, "<init>", "(J)V");
        if (!constructor) {
            ogc_feature_destroy(feature);
            return nullptr;
        }
        
        jobject featureInfo = env->NewObject(featureInfoClass, constructor, 
                                             JniConverter::ToJLongPtr(feature));
        return featureInfo;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jobject JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeScreenToWorld
  (JNIEnv* env, jobject obj, jlong ptr, jdouble screenX, jdouble screenY) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            JniException::ThrowNullPointerException(env, "ChartViewer is null");
            return nullptr;
        }
        
        double worldX, worldY;
        int result = ogc_chart_viewer_screen_to_world(viewer, screenX, screenY, 
                                                       &worldX, &worldY);
        
        if (result != OGC_SUCCESS) {
            JniException::TranslateCError(env, result, "Screen to world");
            return nullptr;
        }
        
        return JniConverter::CreateCoordinate(env, worldX, worldY);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdouble JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetFps
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            return 0.0;
        }
        
        return ogc_chart_viewer_get_fps(viewer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jlong JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeGetMemoryUsage
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    
    try {
        ogc_chart_viewer_t* viewer = 
            static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!viewer) {
            return 0;
        }
        
        return static_cast<jlong>(ogc_chart_viewer_get_memory_usage(viewer));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

}
```

### 3.7 JNI_OnLoad实现

```cpp
#include "jni_env_manager.h"
#include <jni.h>

using namespace ogc::jni;

static JNINativeMethod g_chartViewerMethods[] = {
    {"nativeCreate", "()J", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeCreate},
    {"nativeCreateWithConfig", "(Lcn/cycle/chart/api/core/ChartConfig;)J", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeCreateWithConfig},
    {"nativeDispose", "(J)V", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeDispose},
    {"nativeLoadChart", "(JLjava/lang/String;)Z", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeLoadChart},
    {"nativeSetViewport", "(JDDDD)V", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeSetViewport},
    {"nativeGetViewport", "(J)Lcn/cycle/chart/api/geometry/Envelope;", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewport},
    {"nativeQueryFeature", "(JDD)Lcn/cycle/chart/api/feature/FeatureInfo;", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeQueryFeature},
    {"nativeScreenToWorld", "(JDD)Lcn/cycle/chart/api/geometry/Coordinate;", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeScreenToWorld},
    {"nativeGetFps", "(J)D", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeGetFps},
    {"nativeGetMemoryUsage", "(J)J", 
     (void*)Java_cn_cycle_chart_api_core_ChartViewer_nativeGetMemoryUsage},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8) != JNI_OK) {
        return JNI_ERR;
    }
    
    JniEnvManager::GetInstance()->Initialize(vm);
    
    jclass chartViewerClass = env->FindClass("cn/cycle/chart/api/core/ChartViewer");
    if (chartViewerClass) {
        env->RegisterNatives(chartViewerClass, g_chartViewerMethods, 
                             sizeof(g_chartViewerMethods) / sizeof(JNINativeMethod));
    }
    
    return JNI_VERSION_1_8;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    JniEnvManager::GetInstance()->Shutdown();
}
```

---

## 4. Java Wrapper层封装设计

### 4.1 设计原则

| 原则 | 说明 |
|------|------|
| **类型安全** | 隐藏long nativePtr，提供强类型接口 |
| **资源管理** | 实现AutoCloseable，支持try-with-resources |
| **异常转换** | 将JNI异常转为业务异常 |
| **易用性** | 提供流畅API，减少样板代码 |
| **可测试性** | 支持Mock和单元测试 |

### 4.2 NativeObject基类

#### NativeObject.java

```java
package cn.cycle.chart.jni;

public abstract class NativeObject implements AutoCloseable {
    
    protected long nativePtr;
    private boolean disposed = false;
    private final Object lock = new Object();
    
    protected NativeObject() {
        this.nativePtr = 0;
    }
    
    protected NativeObject(long nativePtr) {
        this.nativePtr = nativePtr;
    }
    
    public final long getNativePtr() {
        synchronized (lock) {
            return nativePtr;
        }
    }
    
    public final boolean isDisposed() {
        synchronized (lock) {
            return disposed || nativePtr == 0;
        }
    }
    
    protected final void setNativePtr(long ptr) {
        synchronized (lock) {
            this.nativePtr = ptr;
        }
    }
    
    @Override
    public final void close() {
        synchronized (lock) {
            if (!disposed && nativePtr != 0) {
                try {
                    nativeDispose(nativePtr);
                } finally {
                    nativePtr = 0;
                    disposed = true;
                }
            }
        }
    }
    
    protected abstract void nativeDispose(long ptr);
    
    @Override
    protected final void finalize() throws Throwable {
        try {
            close();
        } finally {
            super.finalize();
        }
    }
    
    protected final void checkNotDisposed() {
        if (isDisposed()) {
            throw new IllegalStateException(
                getClass().getSimpleName() + " has been disposed");
        }
    }
}
```

### 4.3 ChartViewer封装

#### ChartViewer.java

```java
package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.layer.LayerManager;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class ChartViewer extends NativeObject {
    
    static {
        JniBridge.initialize();
    }
    
    private final LayerManager layerManager;
    private final Viewport viewport;
    private final ExecutorService asyncExecutor;
    
    public ChartViewer() {
        setNativePtr(nativeCreate());
        this.layerManager = new LayerManager(nativeGetLayerManager(getNativePtr()));
        this.viewport = new Viewport(this);
        this.asyncExecutor = Executors.newSingleThreadExecutor();
    }
    
    public ChartViewer(ChartConfig config) {
        Objects.requireNonNull(config, "config must not be null");
        setNativePtr(nativeCreateWithConfig(config));
        this.layerManager = new LayerManager(nativeGetLayerManager(getNativePtr()));
        this.viewport = new Viewport(this);
        this.asyncExecutor = Executors.newSingleThreadExecutor();
    }
    
    public boolean loadChart(String filePath) {
        checkNotDisposed();
        Objects.requireNonNull(filePath, "filePath must not be null");
        return nativeLoadChart(getNativePtr(), filePath);
    }
    
    public CompletableFuture<Boolean> loadChartAsync(String filePath) {
        checkNotDisposed();
        Objects.requireNonNull(filePath, "filePath must not be null");
        
        return CompletableFuture.supplyAsync(() -> {
            synchronized (this) {
                return nativeLoadChart(getNativePtr(), filePath);
            }
        }, asyncExecutor);
    }
    
    public void unloadChart() {
        checkNotDisposed();
        nativeUnloadChart(getNativePtr());
    }
    
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
        if (scale <= 0) {
            throw new IllegalArgumentException("scale must be positive");
        }
        nativeSetScale(getNativePtr(), scale);
    }
    
    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }
    
    public FeatureInfo queryFeature(double x, double y) {
        checkNotDisposed();
        return nativeQueryFeature(getNativePtr(), x, y);
    }
    
    public List<FeatureInfo> queryFeatures(Envelope bounds) {
        checkNotDisposed();
        Objects.requireNonNull(bounds, "bounds must not be null");
        return nativeQueryFeatures(getNativePtr(), bounds);
    }
    
    public Coordinate screenToWorld(double screenX, double screenY) {
        checkNotDisposed();
        return nativeScreenToWorld(getNativePtr(), screenX, screenY);
    }
    
    public Coordinate worldToScreen(double worldX, double worldY) {
        checkNotDisposed();
        return nativeWorldToScreen(getNativePtr(), worldX, worldY);
    }
    
    public LayerManager getLayerManager() {
        checkNotDisposed();
        return layerManager;
    }
    
    public Viewport getViewportObject() {
        checkNotDisposed();
        return viewport;
    }
    
    public double getFps() {
        checkNotDisposed();
        return nativeGetFps(getNativePtr());
    }
    
    public long getMemoryUsage() {
        checkNotDisposed();
        return nativeGetMemoryUsage(getNativePtr());
    }
    
    public double getCacheHitRate() {
        checkNotDisposed();
        return nativeGetCacheHitRate(getNativePtr());
    }
    
    @Override
    protected void nativeDispose(long ptr) {
        asyncExecutor.shutdown();
        nativeDispose(ptr);
    }
    
    private native long nativeCreate();
    private native long nativeCreateWithConfig(ChartConfig config);
    private native void nativeDispose(long ptr);
    private native boolean nativeLoadChart(long ptr, String filePath);
    private native void nativeUnloadChart(long ptr);
    private native void nativeSetViewport(long ptr, double minX, double minY, 
                                          double maxX, double maxY);
    private native Envelope nativeGetViewport(long ptr);
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor, double centerX, 
                                   double centerY);
    private native void nativeSetScale(long ptr, double scale);
    private native double nativeGetScale(long ptr);
    private native FeatureInfo nativeQueryFeature(long ptr, double x, double y);
    private native List<FeatureInfo> nativeQueryFeatures(long ptr, Envelope bounds);
    private native Coordinate nativeScreenToWorld(long ptr, double screenX, 
                                                   double screenY);
    private native Coordinate nativeWorldToScreen(long ptr, double worldX, 
                                                   double worldY);
    private native long nativeGetLayerManager(long ptr);
    private native double nativeGetFps(long ptr);
    private native long nativeGetMemoryUsage(long ptr);
    private native double nativeGetCacheHitRate(long ptr);
}
```

### 4.4 Viewport封装

#### Viewport.java

```java
package cn.cycle.chart.api.core;

import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Coordinate;
import java.util.Objects;

public final class Viewport {
    
    private final ChartViewer viewer;
    
    Viewport(ChartViewer viewer) {
        this.viewer = Objects.requireNonNull(viewer, "viewer must not be null");
    }
    
    public Envelope getBounds() {
        return viewer.getViewport();
    }
    
    public void setBounds(Envelope bounds) {
        Objects.requireNonNull(bounds, "bounds must not be null");
        viewer.setViewport(bounds.getMinX(), bounds.getMinY(), 
                          bounds.getMaxX(), bounds.getMaxY());
    }
    
    public void setBounds(double minX, double minY, double maxX, double maxY) {
        viewer.setViewport(minX, minY, maxX, maxY);
    }
    
    public Coordinate getCenter() {
        Envelope bounds = getBounds();
        return new Coordinate(
            (bounds.getMinX() + bounds.getMaxX()) / 2.0,
            (bounds.getMinY() + bounds.getMaxY()) / 2.0
        );
    }
    
    public void setCenter(double x, double y) {
        Envelope bounds = getBounds();
        double width = bounds.getWidth();
        double height = bounds.getHeight();
        setBounds(x - width/2.0, y - height/2.0, x + width/2.0, y + height/2.0);
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
        Objects.requireNonNull(bounds, "bounds must not be null");
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

### 4.5 Geometry封装

#### Geometry.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.NativeObject;
import java.util.List;

public abstract class Geometry extends NativeObject {
    
    public enum Type {
        UNKNOWN(0),
        POINT(1),
        LINESTRING(2),
        POLYGON(3),
        MULTIPOINT(4),
        MULTILINESTRING(5),
        MULTIPOLYGON(6),
        GEOMETRYCOLLECTION(7);
        
        private final int value;
        
        Type(int value) {
            this.value = value;
        }
        
        public int getValue() {
            return value;
        }
        
        public static Type fromValue(int value) {
            for (Type type : values()) {
                if (type.value == value) {
                    return type;
                }
            }
            return UNKNOWN;
        }
    }
    
    public abstract Type getType();
    
    public abstract int getDimension();
    
    public abstract boolean isEmpty();
    
    public abstract boolean isValid();
    
    public abstract boolean isSimple();
    
    public abstract boolean is3D();
    
    public abstract boolean isMeasured();
    
    public abstract int getSRID();
    
    public abstract void setSRID(int srid);
    
    public abstract double getArea();
    
    public abstract double getLength();
    
    public abstract int getNumPoints();
    
    public abstract Envelope getEnvelope();
    
    public abstract Coordinate getCentroid();
    
    public abstract String asText(int precision);
    
    public abstract String asGeoJSON(int precision);
    
    public String asText() {
        return asText(6);
    }
    
    public String asGeoJSON() {
        return asGeoJSON(6);
    }
}
```

#### Point.java

```java
package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public final class Point extends Geometry {
    
    static {
        JniBridge.initialize();
    }
    
    public Point(double x, double y) {
        setNativePtr(nativeCreate(x, y));
    }
    
    public Point(double x, double y, double z) {
        setNativePtr(nativeCreate3D(x, y, z));
    }
    
    public Point(Coordinate coord) {
        this(coord.getX(), coord.getY(), coord.getZ());
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
    
    public Coordinate getCoordinate() {
        checkNotDisposed();
        return nativeGetCoordinate(getNativePtr());
    }
    
    public void setX(double x) {
        checkNotDisposed();
        nativeSetX(getNativePtr(), x);
    }
    
    public void setY(double y) {
        checkNotDisposed();
        nativeSetY(getNativePtr(), y);
    }
    
    public void setZ(double z) {
        checkNotDisposed();
        nativeSetZ(getNativePtr(), z);
    }
    
    @Override
    public Type getType() {
        return Type.POINT;
    }
    
    @Override
    public int getDimension() {
        return 0;
    }
    
    @Override
    public boolean isEmpty() {
        checkNotDisposed();
        return nativeIsEmpty(getNativePtr());
    }
    
    @Override
    public boolean isValid() {
        return true;
    }
    
    @Override
    public boolean isSimple() {
        return true;
    }
    
    @Override
    public boolean is3D() {
        checkNotDisposed();
        return nativeIs3D(getNativePtr());
    }
    
    @Override
    public boolean isMeasured() {
        checkNotDisposed();
        return nativeIsMeasured(getNativePtr());
    }
    
    @Override
    public int getSRID() {
        checkNotDisposed();
        return nativeGetSRID(getNativePtr());
    }
    
    @Override
    public void setSRID(int srid) {
        checkNotDisposed();
        nativeSetSRID(getNativePtr(), srid);
    }
    
    @Override
    public double getArea() {
        return 0.0;
    }
    
    @Override
    public double getLength() {
        return 0.0;
    }
    
    @Override
    public int getNumPoints() {
        return isEmpty() ? 0 : 1;
    }
    
    @Override
    public Envelope getEnvelope() {
        checkNotDisposed();
        return nativeGetEnvelope(getNativePtr());
    }
    
    @Override
    public Coordinate getCentroid() {
        return getCoordinate();
    }
    
    @Override
    public String asText(int precision) {
        checkNotDisposed();
        return nativeAsText(getNativePtr(), precision);
    }
    
    @Override
    public String asGeoJSON(int precision) {
        checkNotDisposed();
        return nativeAsGeoJSON(getNativePtr(), precision);
    }
    
    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }
    
    private native long nativeCreate(double x, double y);
    private native long nativeCreate3D(double x, double y, double z);
    private native void nativeDestroy(long ptr);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native double nativeGetZ(long ptr);
    private native Coordinate nativeGetCoordinate(long ptr);
    private native void nativeSetX(long ptr, double x);
    private native void nativeSetY(long ptr, double y);
    private native void nativeSetZ(long ptr, double z);
    private native boolean nativeIsEmpty(long ptr);
    private native boolean nativeIs3D(long ptr);
    private native boolean nativeIsMeasured(long ptr);
    private native int nativeGetSRID(long ptr);
    private native void nativeSetSRID(long ptr, int srid);
    private native Envelope nativeGetEnvelope(long ptr);
    private native String nativeAsText(long ptr, int precision);
    private native String nativeAsGeoJSON(long ptr, int precision);
}
```

### 4.6 FeatureInfo封装

#### FeatureInfo.java

```java
package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.geometry.Geometry;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

public final class FeatureInfo extends NativeObject {
    
    static {
        JniBridge.initialize();
    }
    
    private Map<String, FieldValue> fieldCache;
    
    FeatureInfo(long nativePtr) {
        setNativePtr(nativePtr);
        this.fieldCache = new HashMap<>();
    }
    
    public long getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }
    
    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }
    
    public Geometry getGeometry() {
        checkNotDisposed();
        return nativeGetGeometry(getNativePtr());
    }
    
    public int getFieldCount() {
        checkNotDisposed();
        return nativeGetFieldCount(getNativePtr());
    }
    
    public List<String> getFieldNames() {
        checkNotDisposed();
        return nativeGetFieldNames(getNativePtr());
    }
    
    public FieldValue getFieldValue(String fieldName) {
        checkNotDisposed();
        
        FieldValue cached = fieldCache.get(fieldName);
        if (cached != null) {
            return cached;
        }
        
        FieldValue value = nativeGetFieldValue(getNativePtr(), fieldName);
        if (value != null) {
            fieldCache.put(fieldName, value);
        }
        return value;
    }
    
    public Map<String, FieldValue> getAllFieldValues() {
        checkNotDisposed();
        
        List<String> names = getFieldNames();
        Map<String, FieldValue> values = new HashMap<>();
        
        for (String name : names) {
            FieldValue value = getFieldValue(name);
            if (value != null) {
                values.put(name, value);
            }
        }
        
        return values;
    }
    
    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }
    
    private native long nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native Geometry nativeGetGeometry(long ptr);
    private native int nativeGetFieldCount(long ptr);
    private native List<String> nativeGetFieldNames(long ptr);
    private native FieldValue nativeGetFieldValue(long ptr, String fieldName);
    private native void nativeDestroy(long ptr);
}
```

---

## 5. FXRibbon集成方案

### 5.1 FXRibbon简介

FXRibbon是一个开源的JavaFX Ribbon控件库，提供类似Microsoft Office的Ribbon界面风格。

**项目地址**: https://github.com/dukke/FXRibbon

**主要特性**:
- Ribbon、RibbonTab、RibbonGroup等控件
- 支持大图标、小图标按钮
- 支持下拉菜单、分割按钮
- 支持快速访问工具栏
- 支持键盘导航

### 5.2 集成步骤

#### 步骤1: 添加FXRibbon依赖

在`javafx-app/build.gradle`中添加:

```gradle
dependencies {
    implementation project(':javawrapper')
    implementation files('../fxribbon/build/libs/FXRibbon-1.2.0.jar')
    implementation 'org.openjfx:javafx-controls:21'
    implementation 'org.openjfx:javafx-fxml:21'
}
```

#### 步骤2: 构建FXRibbon库

```bash
cd fxribbon
./gradlew build
```

#### 步骤3: 创建Ribbon界面

### 5.3 Ribbon界面实现

#### MainView.java

```java
package cn.cycle.app.view;

import cn.cycle.app.controller.MainController;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.ChartConfig;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;
import org.controlsfx.control.Ribbon;
import org.controlsfx.control.RibbonTab;
import org.controlsfx.control.RibbonGroup;

public class MainView extends BorderPane {
    
    private final MainController controller;
    private final ChartCanvas chartCanvas;
    private final Ribbon ribbon;
    
    public MainView(Stage stage) {
        this.controller = new MainController();
        this.chartCanvas = new ChartCanvas();
        this.ribbon = createRibbon();
        
        setTop(ribbon);
        setCenter(chartCanvas);
        
        initializeChartViewer();
    }
    
    private Ribbon createRibbon() {
        Ribbon ribbon = new Ribbon();
        
        RibbonTab homeTab = createHomeTab();
        RibbonTab viewTab = createViewTab();
        RibbonTab toolsTab = createToolsTab();
        
        ribbon.getTabs().addAll(homeTab, viewTab, toolsTab);
        
        return ribbon;
    }
    
    private RibbonTab createHomeTab() {
        RibbonTab tab = new RibbonTab("主页");
        
        RibbonGroup fileGroup = new RibbonGroup("文件");
        Button openButton = new Button("打开海图");
        openButton.setOnAction(e -> controller.openChart());
        fileGroup.getNodes().add(openButton);
        
        RibbonGroup editGroup = new RibbonGroup("编辑");
        Button copyButton = new Button("复制");
        copyButton.setOnAction(e -> controller.copySelection());
        editGroup.getNodes().add(copyButton);
        
        tab.getRibbonGroups().addAll(fileGroup, editGroup);
        
        return tab;
    }
    
    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");
        
        RibbonGroup zoomGroup = new RibbonGroup("缩放");
        Button zoomInButton = new Button("放大");
        zoomInButton.setOnAction(e -> controller.zoomIn());
        
        Button zoomOutButton = new Button("缩小");
        zoomOutButton.setOnAction(e -> controller.zoomOut());
        
        Button fitButton = new Button("适应窗口");
        fitButton.setOnAction(e -> controller.fitToWindow());
        
        zoomGroup.getNodes().addAll(zoomInButton, zoomOutButton, fitButton);
        
        RibbonGroup layerGroup = new RibbonGroup("图层");
        Button layerPanelButton = new Button("图层面板");
        layerPanelButton.setOnAction(e -> controller.toggleLayerPanel());
        layerGroup.getNodes().add(layerPanelButton);
        
        tab.getRibbonGroups().addAll(zoomGroup, layerGroup);
        
        return tab;
    }
    
    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");
        
        RibbonGroup measureGroup = new RibbonGroup("测量");
        Button distanceButton = new Button("距离测量");
        distanceButton.setOnAction(e -> controller.startDistanceMeasure());
        
        Button areaButton = new Button("面积测量");
        areaButton.setOnAction(e -> controller.startAreaMeasure());
        
        measureGroup.getNodes().addAll(distanceButton, areaButton);
        
        RibbonGroup navGroup = new RibbonGroup("导航");
        Button routeButton = new Button("航线规划");
        routeButton.setOnAction(e -> controller.openRoutePlanner());
        navGroup.getNodes().add(routeButton);
        
        tab.getRibbonGroups().addAll(measureGroup, navGroup);
        
        return tab;
    }
    
    private void initializeChartViewer() {
        ChartConfig config = new ChartConfig();
        config.setCachePath("./cache");
        config.setMaxCacheSize(512 * 1024 * 1024);
        config.setThreadCount(4);
        
        ChartViewer viewer = new ChartViewer(config);
        chartCanvas.setChartViewer(viewer);
        controller.setChartViewer(viewer);
    }
    
    public ChartCanvas getChartCanvas() {
        return chartCanvas;
    }
    
    public MainController getController() {
        return controller;
    }
}
```

### 5.4 ChartCanvas实现

#### ChartCanvas.java

```java
package cn.cycle.app.view;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.paint.Color;

public class ChartCanvas extends Canvas {
    
    private ChartViewer chartViewer;
    private double lastX, lastY;
    
    public ChartCanvas() {
        setWidth(800);
        setHeight(600);
        
        setupEventHandlers();
    }
    
    private void setupEventHandlers() {
        setOnMousePressed(this::onMousePressed);
        setOnMouseDragged(this::onMouseDragged);
        setOnMouseReleased(this::onMouseReleased);
        setOnScroll(this::onScroll);
        widthProperty().addListener(obs -> render());
        heightProperty().addListener(obs -> render());
    }
    
    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
        render();
    }
    
    public void render() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        
        GraphicsContext gc = getGraphicsContext2D();
        gc.setFill(Color.WHITE);
        gc.fillRect(0, 0, getWidth(), getHeight());
        
        chartViewer.render(this, getWidth(), getHeight());
    }
    
    private void onMousePressed(MouseEvent e) {
        lastX = e.getX();
        lastY = e.getY();
    }
    
    private void onMouseDragged(MouseEvent e) {
        if (chartViewer == null) {
            return;
        }
        
        double dx = e.getX() - lastX;
        double dy = e.getY() - lastY;
        
        chartViewer.pan(-dx, -dy);
        
        lastX = e.getX();
        lastY = e.getY();
        
        render();
    }
    
    private void onMouseReleased(MouseEvent e) {
        if (chartViewer == null) {
            return;
        }
        
        Coordinate worldCoord = chartViewer.screenToWorld(e.getX(), e.getY());
        System.out.println("Clicked at: " + worldCoord.getX() + ", " + worldCoord.getY());
    }
    
    private void onScroll(ScrollEvent e) {
        if (chartViewer == null) {
            return;
        }
        
        double factor = e.getDeltaY() > 0 ? 1.1 : 0.9;
        chartViewer.zoom(factor, e.getX(), e.getY());
        
        render();
    }
}
```

---

## 6. 性能优化设计

### 6.1 JNI性能优化

| 优化项 | 方法 | 预期收益 |
|--------|------|----------|
| **缓存jmethodID/jfieldID** | 在JNI_OnLoad中一次性获取 | 减少90%查找开销 |
| **使用GetPrimitiveArrayCritical** | 对大块数据使用临界区访问 | 减少数据拷贝 |
| **批量操作** | 合并多个JNI调用为一次 | 减少50%跨边界开销 |
| **DirectByteBuffer** | 零拷贝传递大块数据 | 消除拷贝开销 |
| **局部引用管理** | 使用PushLocalFrame/PopLocalFrame | 避免引用泄漏 |

### 6.2 数据传输优化

#### 使用DirectByteBuffer

```java
// Java层
public class ChartViewer {
    public void renderWithBuffer(ByteBuffer buffer) {
        checkNotDisposed();
        nativeRenderWithBuffer(getNativePtr(), buffer);
    }
    
    private native void nativeRenderWithBuffer(long ptr, ByteBuffer buffer);
}
```

```cpp
// JNI层
JNIEXPORT void JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeRenderWithBuffer
  (JNIEnv* env, jobject obj, jlong ptr, jobject buffer) {
    void* data = env->GetDirectBufferAddress(buffer);
    if (!data) {
        JniException::ThrowIllegalArgumentException(env, "Not a direct buffer");
        return;
    }
    
    ogc_chart_viewer_t* viewer = 
        static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
    
    size_t size = env->GetDirectBufferCapacity(buffer);
    ogc_chart_viewer_render_to_buffer(viewer, data, size);
}
```

#### 批量查询优化

```java
// Java层
public List<FeatureInfo> queryFeaturesBatch(List<Coordinate> points) {
    checkNotDisposed();
    
    double[] coords = new double[points.size() * 2];
    for (int i = 0; i < points.size(); i++) {
        coords[i * 2] = points.get(i).getX();
        coords[i * 2 + 1] = points.get(i).getY();
    }
    
    return nativeQueryFeaturesBatch(getNativePtr(), coords);
}

private native List<FeatureInfo> nativeQueryFeaturesBatch(long ptr, double[] coords);
```

```cpp
// JNI层
JNIEXPORT jobject JNICALL 
Java_cn_cycle_chart_api_core_ChartViewer_nativeQueryFeaturesBatch
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray coords) {
    JniLocalRefScope scope(env);
    
    jdouble* elements = env->GetDoubleArrayElements(coords, nullptr);
    jsize length = env->GetArrayLength(coords);
    
    ogc_chart_viewer_t* viewer = 
        static_cast<ogc_chart_viewer_t*>(JniConverter::FromJLongPtr(ptr));
    
    jobject result = JniConverter::CreateArrayList(env);
    
    for (jsize i = 0; i < length; i += 2) {
        ogc_feature_t* feature = 
            ogc_chart_viewer_query_feature(viewer, elements[i], elements[i+1]);
        
        if (feature) {
            jclass featureInfoClass = env->FindClass("cn/cycle/chart/api/feature/FeatureInfo");
            jmethodID constructor = env->GetMethodID(featureInfoClass, "<init>", "(J)V");
            jobject featureInfo = env->NewObject(featureInfoClass, constructor, 
                                                 JniConverter::ToJLongPtr(feature));
            JniConverter::ArrayListAdd(env, result, featureInfo);
        }
    }
    
    env->ReleaseDoubleArrayElements(coords, elements, JNI_ABORT);
    return result;
}
```

### 6.3 缓存策略

| 缓存类型 | 缓存内容 | 缓存位置 | 失效策略 |
|----------|----------|----------|----------|
| **方法ID缓存** | jmethodID/jfieldID | JNI层全局Map | JNI_OnUnload |
| **类引用缓存** | jclass全局引用 | JNI层全局Map | JNI_OnUnload |
| **字段值缓存** | FeatureInfo字段值 | Java层Map | 对象销毁时 |
| **几何对象缓存** | Geometry对象 | Java层WeakReference | GC自动回收 |

---

## 7. 错误处理与资源管理

### 7.1 异常映射规则

| C++异常类型 | Java异常类型 | 错误码 |
|-------------|--------------|--------|
| `std::bad_alloc` | `OutOfMemoryError` | -3 |
| `std::invalid_argument` | `IllegalArgumentException` | -1 |
| `std::runtime_error` | `RuntimeException` | -5 |
| `ogc::IOException` | `IOException` | -7 |
| `ogc::ParseException` | `RuntimeException` | -8 |
| 其他异常 | `RuntimeException` | -5 |

### 7.2 资源管理策略

#### RAII模式

```cpp
// JNI层使用RAII管理资源
class JniLocalRefScope {
public:
    JniLocalRefScope(JNIEnv* env, jint capacity = 16) 
        : m_env(env), m_success(false) {
        m_success = (env->PushLocalFrame(capacity) == 0);
    }
    
    ~JniLocalRefScope() {
        if (m_success) {
            m_env->PopLocalFrame(nullptr);
        }
    }
    
private:
    JNIEnv* m_env;
    bool m_success;
};
```

#### AutoCloseable模式

```java
// Java层使用try-with-resources
try (ChartViewer viewer = new ChartViewer(config)) {
    viewer.loadChart("/path/to/chart.000");
    viewer.render(canvas, 800, 600);
} catch (Exception e) {
    logger.error("Failed to render chart", e);
}
```

### 7.3 资源泄漏检测

```java
// 使用PhantomReference检测资源泄漏
public class ResourceLeakDetector {
    private static final ReferenceQueue<NativeObject> queue = 
        new ReferenceQueue<>();
    
    private static final Set<PhantomReference<NativeObject>> refs = 
        ConcurrentHashMap.newKeySet();
    
    public static void register(NativeObject obj) {
        PhantomReference<NativeObject> ref = 
            new PhantomReference<>(obj, queue);
        refs.add(ref);
        
        new Thread(() -> {
            Reference<? extends NativeObject> clearedRef;
            while ((clearedRef = queue.poll()) != null) {
                refs.remove(clearedRef);
                System.err.println("Potential resource leak detected: " + 
                    clearedRef.getClass().getName());
            }
        }).start();
    }
}
```

---

## 8. 线程安全设计

### 8.1 线程模型

```
┌─────────────────────────────────────────────────────────────┐
│                      线程模型                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  JavaFX应用线程 (JavaFX Application Thread)                 │
│  ├─ UI事件处理                                              │
│  ├─ 渲染操作                                                │
│  └─ ChartViewer操作（必须在此线程）                         │
│                                                             │
│  JNI工作线程 (JNI Worker Threads)                           │
│  ├─ 异步加载海图                                            │
│  ├─ 后台数据处理                                            │
│  └─ 缓存管理                                                │
│                                                             │
│  C++内部线程 (C++ Internal Threads)                         │
│  ├─ 瓦片缓存线程                                            │
│  ├─ 数据库连接池线程                                        │
│  └─ 性能监控线程                                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 线程安全规则

| 组件 | 线程安全级别 | 使用规则 |
|------|--------------|----------|
| **ChartViewer** | 非线程安全 | 只在JavaFX应用线程操作 |
| **Viewport** | 非线程安全 | 只在JavaFX应用线程操作 |
| **LayerManager** | 读安全 | 读操作可多线程，写操作需同步 |
| **TileCache** | 线程安全 | 可多线程访问 |
| **Geometry** | 非线程安全 | 只在单线程操作 |
| **FeatureInfo** | 非线程安全 | 只在单线程操作 |

### 8.3 线程安全实现

```java
// Java层线程安全封装
public class ThreadSafeChartViewer {
    private final ChartViewer viewer;
    private final Object lock = new Object();
    
    public ThreadSafeChartViewer(ChartConfig config) {
        this.viewer = new ChartViewer(config);
    }
    
    public boolean loadChart(String filePath) {
        synchronized (lock) {
            return viewer.loadChart(filePath);
        }
    }
    
    public void setViewport(double minX, double minY, double maxX, double maxY) {
        synchronized (lock) {
            viewer.setViewport(minX, minY, maxX, maxY);
        }
    }
    
    public FeatureInfo queryFeature(double x, double y) {
        synchronized (lock) {
            return viewer.queryFeature(x, y);
        }
    }
}
```

### 8.4 JNI线程附加

```cpp
// JNI层线程附加管理
class JniThreadAttachment {
public:
    JniThreadAttachment() {
        m_env = JniEnvManager::GetInstance()->AttachCurrentThread();
    }
    
    ~JniThreadAttachment() {
        JniEnvManager::GetInstance()->DetachCurrentThread();
    }
    
    JNIEnv* GetEnv() { return m_env; }
    
private:
    JNIEnv* m_env;
};

// 使用示例
void backgroundWorker() {
    JniThreadAttachment attach;
    JNIEnv* env = attach.GetEnv();
    
    if (env) {
        // 执行JNI操作
    }
}
```

---

## 9. 测试策略

### 9.1 测试层次

| 测试层次 | 测试内容 | 工具 | 覆盖率目标 |
|----------|----------|------|------------|
| **单元测试** | Java API层逻辑 | JUnit 5 | >80% |
| **集成测试** | JNI调用正确性 | JUnit 5 + Native Test | >70% |
| **性能测试** | JNI调用性能 | JMH | 关键路径 |
| **内存测试** | 资源泄漏检测 | JUnit 5 + WeakReference | 所有Native对象 |

### 9.2 单元测试示例

```java
package cn.cycle.chart.api.test;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.ChartConfig;
import cn.cycle.chart.api.geometry.Envelope;
import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.*;

class ChartViewerTest {
    
    private ChartViewer viewer;
    
    @BeforeEach
    void setUp() {
        ChartConfig config = new ChartConfig();
        config.setCachePath("./test_cache");
        config.setMaxCacheSize(10 * 1024 * 1024);
        
        viewer = new ChartViewer(config);
    }
    
    @AfterEach
    void tearDown() {
        if (viewer != null) {
            viewer.close();
        }
    }
    
    @Test
    void testCreateAndDispose() {
        assertNotNull(viewer);
        assertFalse(viewer.isDisposed());
        
        viewer.close();
        assertTrue(viewer.isDisposed());
    }
    
    @Test
    void testSetViewport() {
        viewer.setViewport(0.0, 0.0, 100.0, 100.0);
        
        Envelope bounds = viewer.getViewport();
        assertNotNull(bounds);
        assertEquals(0.0, bounds.getMinX(), 0.001);
        assertEquals(0.0, bounds.getMinY(), 0.001);
        assertEquals(100.0, bounds.getMaxX(), 0.001);
        assertEquals(100.0, bounds.getMaxY(), 0.001);
    }
    
    @Test
    void testDisposedThrowsException() {
        viewer.close();
        
        assertThrows(IllegalStateException.class, () -> {
            viewer.setViewport(0, 0, 100, 100);
        });
    }
    
    @Test
    void testTryWithResources() {
        try (ChartViewer v = new ChartViewer()) {
            assertFalse(v.isDisposed());
        }
    }
}
```

### 9.3 性能测试示例

```java
package cn.cycle.chart.api.test;

import cn.cycle.chart.api.core.ChartViewer;
import org.openjdk.jmh.annotations.*;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MICROSECONDS)
@State(Scope.Benchmark)
public class ChartViewerPerformanceTest {
    
    private ChartViewer viewer;
    
    @Setup
    public void setUp() {
        viewer = new ChartViewer();
    }
    
    @TearDown
    public void tearDown() {
        viewer.close();
    }
    
    @Benchmark
    public void testSetViewport() {
        viewer.setViewport(0, 0, 100, 100);
    }
    
    @Benchmark
    public void testGetViewport() {
        viewer.getViewport();
    }
    
    @Benchmark
    public void testScreenToWorld() {
        viewer.screenToWorld(400, 300);
    }
}
```

---

## 10. 附录

### 10.1 JNI方法签名速查

| Java类型 | JNI签名 |
|----------|---------|
| `boolean` | `Z` |
| `byte` | `B` |
| `char` | `C` |
| `short` | `S` |
| `int` | `I` |
| `long` | `J` |
| `float` | `F` |
| `double` | `D` |
| `void` | `V` |
| `String` | `Ljava/lang/String;` |
| `Object` | `Ljava/lang/Object;` |
| `int[]` | `[I` |
| `String[]` | `[Ljava/lang/String;` |

### 10.2 常见JNI错误码

| 错误码 | 说明 | 解决方法 |
|--------|------|----------|
| `JNI_ERR` | 未知错误 | 检查JNI版本 |
| `JNI_EDETACHED` | 线程未附加 | 调用AttachCurrentThread |
| `JNI_EVERSION` | 版本不匹配 | 检查JNI_VERSION |
| `JNI_ENOMEM` | 内存不足 | 增加JVM内存 |

### 10.3 性能优化检查清单

```markdown
□ 已缓存所有jmethodID和jfieldID
□ 使用GetPrimitiveArrayCritical处理大数据
□ 批量操作减少JNI调用次数
□ 使用DirectByteBuffer零拷贝传输
□ 使用PushLocalFrame/PopLocalFrame管理局部引用
□ 避免在循环中创建局部引用
□ 异步操作使用独立线程
□ 关键路径进行性能测试
```

### 10.4 资源管理检查清单

```markdown
□ 所有NativeObject实现了AutoCloseable
□ 使用try-with-resources管理资源
□ JNI层使用RAII管理局部引用
□ 全局引用在JNI_OnUnload中释放
□ 实现了资源泄漏检测
□ 测试覆盖了资源释放场景
```

### 10.5 线程安全检查清单

```markdown
□ 明确标注了每个类的线程安全级别
□ 非线程安全类提供了同步封装
□ JNI线程正确附加和分离
□ 避免在多线程中共享非线程安全对象
□ 测试覆盖了多线程场景
```

---

## 版本历史

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.0 | 2026-04-10 | 初始版本，完整JNI层和Java Wrapper层封装方案 |

---

**文档结束**
