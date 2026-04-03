# Android平台适配模块 (android_adapter)

## 概述

本模块提供Android平台的海图显示系统适配功能，包括OpenGL ES渲染、触摸事件处理、生命周期管理和权限管理。

## 功能特性

- **AndroidEngine**: OpenGL ES 2.0/3.0渲染引擎封装
- **AndroidTouchHandler**: 多点触摸事件处理和手势识别
- **AndroidLifecycle**: Android Activity生命周期管理
- **AndroidPermission**: Android运行时权限管理
- **AndroidPlatform**: Android平台信息和工具函数

## 构建要求

- Android NDK r21+
- CMake 3.16+
- Android SDK API Level 21+

## 构建配置

```cmake
# 启用Android适配模块
option(ANDROID_ADAPTER_BUILD_TESTS "Build tests" ON)

# 添加子目录
add_subdirectory(android_adapter)
```

## 使用示例

### 初始化

```cpp
#include <android/native_activity.h>
#include "android_adapter/android_platform.h"
#include "android_adapter/android_engine.h"
#include "android_adapter/android_lifecycle.h"

void InitializeAndroid(ANativeActivity* activity) {
    // 初始化平台信息
    auto* platform = ogc::android::AndroidPlatform::GetInstance();
    platform->Initialize(activity);
    
    // 初始化生命周期
    auto* lifecycle = ogc::android::AndroidLifecycle::GetInstance();
    lifecycle->Initialize(activity);
    
    // 初始化渲染引擎
    auto* engine = ogc::android::AndroidEngine::GetInstance();
    AndroidEngineConfig config;
    config.width = platform->GetScreenInfo().width;
    config.height = platform->GetScreenInfo().height;
    engine->Initialize(config);
}
```

### 触摸事件处理

```cpp
#include "android_adapter/android_touch_handler.h"

void SetupTouchHandler() {
    auto* handler = ogc::android::AndroidTouchHandler::GetInstance();
    
    handler->SetGestureCallback([](const ogc::android::GestureState& state) {
        if (state.isPinching) {
            // 处理缩放手势
            float scale = state.scale;
        }
        if (state.isPanning) {
            // 处理平移手势
            float dx = state.panX;
            float dy = state.panY;
        }
    });
}
```

### 权限请求

```cpp
#include "android_adapter/android_permission.h"

void RequestLocationPermission() {
    auto* permission = ogc::android::AndroidPermission::GetInstance();
    
    if (!permission->HasLocationPermission()) {
        permission->RequestPermission(
            ogc::android::PermissionType::kLocation,
            [](const std::vector<ogc::android::PermissionResult>& results) {
                for (const auto& result : results) {
                    if (result.status == ogc::android::PermissionStatus::kGranted) {
                        // 权限已授予
                    } else {
                        // 权限被拒绝
                    }
                }
            });
    }
}
```

## API参考

### AndroidEngine

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `Initialize(config)` | 初始化渲染引擎 |
| `CreateSurface(window)` | 创建渲染表面 |
| `BeginFrame()` | 开始帧渲染 |
| `EndFrame()` | 结束帧渲染 |
| `SwapBuffers()` | 交换缓冲区 |

### AndroidTouchHandler

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `ProcessTouchEvent(...)` | 处理触摸事件 |
| `SetGestureCallback(callback)` | 设置手势回调 |
| `GetGestureState()` | 获取手势状态 |

### AndroidLifecycle

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `Initialize(activity)` | 初始化生命周期 |
| `SetCallbacks(callbacks)` | 设置生命周期回调 |
| `GetState()` | 获取当前状态 |

### AndroidPermission

| 方法 | 说明 |
|------|------|
| `GetInstance()` | 获取单例实例 |
| `CheckPermission(type)` | 检查权限状态 |
| `RequestPermission(type, callback)` | 请求权限 |
| `HasLocationPermission()` | 检查定位权限 |

## 注意事项

1. 本模块仅在Android平台编译，非Android平台会生成空接口库
2. 需要在AndroidManifest.xml中声明所需权限
3. 渲染线程需要在Activity生命周期回调中正确管理

## 版本历史

- v1.0.0: 初始版本，提供基础Android平台适配功能
