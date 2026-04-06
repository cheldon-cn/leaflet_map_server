# OGC Navigation Module

[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-proprietary-red.svg)](LICENSE)

海图导航系统模块，提供完整的海洋导航功能，包括定位、航线规划、导航引导、轨迹记录和AIS集成。

## 功能特性

### 定位模块 (Positioning)
- **NMEA解析**：支持GGA/RMC/VTG/GSA/GSV语句
- **坐标转换**：大圆距离、方位角、目标点计算
- **位置过滤**：异常检测、数据平滑
- **多源定位**：串口、文件、系统定位

### 航线模块 (Route)
- **航点管理**：创建、编辑、删除航点
- **航线规划**：多航点航线计算
- **UKC计算**：富余水深计算
- **航线存储**：数据库持久化

### 导航模块 (Navigation)
- **实时引导**：XTD、TTG、ETA、VMG计算
- **偏航检测**：可配置警告/报警阈值
- **航点提示**：到达提醒、航点动作

### 轨迹模块 (Track)
- **轨迹记录**：实时记录位置数据
- **轨迹简化**：Douglas-Peucker算法
- **轨迹分析**：距离、时长、速度统计
- **轨迹回放**：历史轨迹查询

### AIS模块
- **数据解析**：AIS消息解码
- **目标管理**：船舶信息维护
- **碰撞评估**：CPA/TCPA计算
- **风险预警**：碰撞风险提示

## 快速开始

### 环境要求

- C++11 或更高版本
- CMake 3.10+
- ogc_geometry, ogc_database

### 编译

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 基本使用

```cpp
#include <ogc/navi/positioning/coordinate_converter.h>
#include <ogc/navi/route/route.h>
#include <ogc/navi/route/waypoint.h>

using namespace ogc::navi;

// 计算距离
double distance = CoordinateConverter::Instance().CalculateGreatCircleDistance(
    31.2304, 121.4737,  // 上海
    39.9042, 116.4074   // 北京
);
// 结果: 约1067公里

// 创建航线
Route* route = Route::Create();
route->SetName("Shanghai-Beijing");

WaypointData wp_data;
wp_data.id = "WP001";
wp_data.name = "Shanghai";
wp_data.longitude = 121.4737;
wp_data.latitude = 31.2304;

Waypoint* wp = Waypoint::Create(wp_data);
route->AddWaypoint(wp);
wp->ReleaseReference();

route->ReleaseReference();
```

## 目录结构

```
navi/
├── include/ogc/navi/          # 公共头文件
│   ├── positioning/           # 定位模块
│   │   ├── nmea_parser.h      # NMEA解析器
│   │   ├── coordinate_converter.h  # 坐标转换
│   │   ├── position_filter.h  # 位置过滤器
│   │   ├── position_provider.h # 定位数据源
│   │   └── position_manager.h # 定位管理器
│   ├── route/                 # 航线模块
│   │   ├── waypoint.h         # 航点
│   │   ├── route.h            # 航线
│   │   ├── route_planner.h    # 航线规划
│   │   ├── route_manager.h    # 航线管理
│   │   └── ukc_calculator.h   # UKC计算
│   ├── navigation/            # 导航模块
│   │   ├── navigation_calculator.h  # 导航计算
│   │   ├── off_course_detector.h    # 偏航检测
│   │   ├── navigation_alerter.h     # 导航警报
│   │   └── navigation_engine.h      # 导航引擎
│   ├── track/                 # 轨迹模块
│   │   ├── track_point.h      # 轨迹点
│   │   ├── track.h            # 轨迹
│   │   ├── track_recorder.h   # 轨迹记录器
│   │   └── track_manager.h    # 轨迹管理
│   ├── ais/                   # AIS模块
│   │   ├── ais_message.h      # AIS消息
│   │   ├── ais_parser.h       # AIS解析
│   │   ├── ais_target.h       # AIS目标
│   │   ├── collision_assessor.h  # 碰撞评估
│   │   └── ais_manager.h      # AIS管理
│   ├── types.h                # 类型定义
│   ├── export.h               # 导出宏
│   └── index.md               # API文档
├── src/                       # 实现文件
│   ├── positioning/
│   ├── route/
│   ├── navigation/
│   ├── track/
│   └── ais/
├── tests/                     # 测试文件
│   ├── test_nmea_parser.cpp
│   ├── test_coordinate_converter.cpp
│   ├── test_position_filter.cpp
│   ├── test_waypoint.cpp
│   ├── test_route.cpp
│   ├── test_navigation_calculator.cpp
│   ├── test_off_course_detector.cpp
│   ├── test_track_point.cpp
│   ├── test_track.cpp
│   ├── test_ais_target.cpp
│   ├── test_integration.cpp
│   ├── test_performance.cpp
│   └── test_constants.h
└── doc/                       # 文档
    ├── user_manual.md         # 用户手册
    ├── chart_navi_system_design.md  # 设计文档
    └── tasks.md               # 任务清单
```

## 文档

- [用户手册](./doc/user_manual.md) - 完整的使用指南
- [API文档](./include/ogc/navi/index.md) - 详细的API参考
- [设计文档](./doc/chart_navi_system_design.md) - 系统设计说明
- [测试报告](./tests/navi_test_quality.md) - 测试质量评估

## 测试

```bash
# 运行所有测试
cd build
ctest -C Release

# 或直接运行测试程序
./test/ogc_navi_tests.exe
```

### 测试覆盖

| 类型 | 数量 | 说明 |
|------|------|------|
| 单元测试 | 259 | 各模块功能测试 |
| 集成测试 | 8 | 多模块协作测试 |
| 性能测试 | 14 | 关键算法基准 |
| **总计** | **281** | **100%通过** |

## 性能指标

| 操作 | 性能 |
|------|------|
| 坐标转换 | > 100,000 次/秒 |
| NMEA解析 | > 20,000 句/秒 |
| 轨迹简化(1000点) | < 100ms |
| 内存占用 | ~80字节/轨迹点 |

## 依赖

| 库 | 说明 |
|----|------|
| ogc_geometry | 几何类型和操作 |
| ogc_database | 数据库连接和存储 |
| libpq | PostgreSQL客户端 |
| sqlite3 | SQLite数据库 |
| libspatialite | 空间数据库扩展 |

## 许可证

Copyright © 2026. All rights reserved.
