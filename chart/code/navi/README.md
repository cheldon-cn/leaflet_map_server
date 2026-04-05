# OGC Navigation Module

海图导航系统模块，提供航线规划、航行引导、定位追踪等导航相关功能。

## 功能特性

- **定位模块**：支持NMEA解析、多源定位、数据过滤、坐标转换
- **航线模块**：支持航线规划、编辑、管理、存储
- **导航模块**：支持实时导航引导、偏航检测、航点提示
- **轨迹模块**：支持轨迹记录、回放、分析、分享
- **AIS模块**：支持AIS数据解析、碰撞评估、风险预警

## 目录结构

```
navi/
├── include/ogc/navi/          # 头文件
│   ├── positioning/           # 定位模块
│   ├── route/                 # 航线模块
│   ├── navigation/            # 导航模块
│   ├── track/                 # 轨迹模块
│   ├── ais/                   # AIS模块
│   ├── types.h                # 类型定义
│   └── export.h               # 导出宏
├── src/                       # 源文件
│   ├── positioning/
│   ├── route/
│   ├── navigation/
│   ├── track/
│   └── ais/
├── tests/                     # 测试文件
└── doc/                       # 文档
```

## 依赖

- C++11 或更高版本
- ogc_geometry
- ogc_database

## 编译

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 使用示例

```cpp
#include <ogc/navi/positioning/position_manager.h>
#include <ogc/navi/navigation/navigation_engine.h>

using namespace ogc::navi;

// 初始化定位
ProviderConfig config;
config.device_path = "/dev/ttyUSB0";
config.baud_rate = 4800;
PositionManager::Instance().Initialize(PositionSource::Serial, config);

// 启动导航
NavigationEngine::Instance().StartNavigation(route);
```

## 许可证

Copyright © 2026
