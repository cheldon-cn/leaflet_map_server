# JNI桥接层模块 (jni_bridge)

## 概述

本模块提供Java Native Interface (JNI)桥接功能，用于在C++和Java之间进行数据转换、异常处理和内存管理。

## 功能特性

- **JniEnvManager**: JNI环境管理，线程安全的JNIEnv获取
- **JniConverter**: Java-C++数据类型转换工具
- **JniExceptionHandler**: JNI异常检测和处理
- **JniMemoryManager**: JNI内存管理和对象生命周期管理

## 构建要求

- Android NDK r21+
- CMake 3.16+
- Android SDK API Level 21+

## 构建配置

```cmake
# 启用JNI桥接模块
option(JNI_BRIDGE_BUILD_TESTS "Build tests" ON)

# 添加子目录
add_subdirectory(jni_bridge)
```

## 使用示例

### 初始化

```cpp
#include <android/native_activity.h>
#include "jni_bridge/jni_env.h"

void InitializeJNI(JavaVM* vm) {
    auto* manager = ogc::jni::JniEnvManager::GetInstance();
    manager->Initialize(vm);
}
```

### 数据转换

```cpp
#include "jni_bridge/jni_convert.h"

void ConvertData(JNIEnv* env) {
    // 字符串转换
    std::string cppStr = ogc::jni::JniConverter::ToString(env, javaString);
    jstring javaStr = ogc::jni::JniConverter::ToJString(env, cppStr);
    
    // 数组转换
    std::vector<int> intVec = ogc::jni::JniConverter::ToIntVector(env, javaIntArray);
    jintArray javaArray = ogc::jni::JniConverter::ToJIntArray(env, intVec);
    
    // 指针转换
    jlong javaPtr = ogc::jni::JniConverter::ToJLongPtr(nativePtr);
    void* nativePtr = ogc::jni::JniConverter::FromJLongPtr(javaPtr);
}
```

### 异常处理

```cpp
#include "jni_bridge/jni_exception.h"

void HandleExceptions(JNIEnv* env) {
    // 检查并清除异常
    if (ogc::jni::JniExceptionHandler::CheckAndClear(env)) {
        // 异常已处理
    }
    
    // 抛出异常
    ogc::jni::JniExceptionHandler::ThrowRuntimeException(env, "Error message");
    
    // 安全调用
    ogc::jni::JniExceptionHandler::SafeCall(env, [&]() {
        // 可能抛出异常的代码
    });
}
```

### 内存管理

```cpp
#include "jni_bridge/jni_memory.h"

void ManageMemory() {
    auto* manager = ogc::jni::JniMemoryManager::GetInstance();
    
    // 分配内存
    void* ptr = manager->Allocate(1024);
    
    // 创建带析构函数的对象
    MyClass* obj = manager->New<MyClass>(arg1, arg2);
    
    // 释放内存
    manager->Delete(obj);
    
    // 设置内存限制
    manager->SetMemoryLimit(10 * 1024 * 1024); // 10MB
}
```

### 注册本地方法

```cpp
#include "jni_bridge/jni_env.h"

static JNINativeMethod g_methods[] = {
    {"nativeMethod", "(I)V", (void*)&nativeMethod},
    {"nativeMethod2", "()Ljava/lang/String;", (void*)&nativeMethod2}
};

void RegisterNatives() {
    auto* manager = ogc::jni::JniEnvManager::GetInstance();
    manager->RegisterNatives("com/example/MyClass", g_methods, 2);
}
```

## API参考

### JniEnvManager

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `Initialize(vm)` | 初始化JavaVM |
| `GetEnv()` | 获取当前线程的JNIEnv |
| `AttachCurrentThread()` | 附加当前线程并获取JNIEnv |
| `RegisterNatives(...)` | 注册本地方法 |

### JniConverter

| 方法 | 说明 |
|------|------|
| `ToString(env, jstr)` | jstring转std::string |
| `ToJString(env, str)` | std::string转jstring |
| `ToIntVector(env, arr)` | jintArray转vector<int> |
| `ToJIntArray(env, vec)` | vector<int>转jintArray |
| `ToJLongPtr(ptr)` | 指针转jlong |
| `FromJLongPtr(val)` | jlong转指针 |

### JniExceptionHandler

| 方法 | 说明 |
|------|------|
| `CheckAndClear(env)` | 检查并清除异常 |
| `ThrowIfPending(env)` | 如果有异常则抛出C++异常 |
| `ThrowRuntimeException(env, msg)` | 抛出Java RuntimeException |
| `GetStackTrace(env, ex)` | 获取异常堆栈跟踪 |

### JniMemoryManager

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `Allocate(size)` | 分配内存 |
| `Deallocate(ptr)` | 释放内存 |
| `New<T>(args...)` | 创建对象 |
| `Delete<T>(ptr)` | 删除对象 |
| `SetMemoryLimit(limit)` | 设置内存限制 |

## 注意事项

1. 本模块仅在Android平台编译
2. 使用前必须调用`Initialize(JavaVM*)`初始化
3. 非主线程需要使用`AttachCurrentThread()`
4. 使用`JniEnvScope`可以自动管理线程附加

## 版本历史

- v1.0.0: 初始版本，提供基础JNI桥接功能
