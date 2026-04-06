# OGC Navigation Module - User Manual

海图导航系统模块用户手册

---

## 目录

1. [概述](#1-概述)
2. [系统要求](#2-系统要求)
3. [安装与配置](#3-安装与配置)
4. [快速开始](#4-快速开始)
5. [模块详解](#5-模块详解)
6. [API参考](#6-api参考)
7. [常见问题](#7-常见问题)
8. [版本历史](#8-版本历史)

---

## 1. 概述

OGC Navigation Module 是一个完整的海洋导航系统模块，提供定位、航线规划、导航引导、轨迹记录和AIS集成等功能。

### 1.1 核心功能

| 功能模块 | 描述 |
|----------|------|
| **定位模块** | NMEA解析、多源定位、数据过滤、坐标转换 |
| **航线模块** | 航线规划、编辑、管理、UKC计算 |
| **导航模块** | 实时导航引导、偏航检测、航点提示 |
| **轨迹模块** | 轨迹记录、回放、分析、简化 |
| **AIS模块** | AIS数据解析、碰撞评估、风险预警 |

### 1.2 设计特点

- **C++11标准**：完全兼容C++11，无外部框架依赖
- **模块化设计**：各模块独立，可单独使用
- **单例模式**：核心组件采用单例模式，全局访问
- **工厂方法**：对象创建使用工厂方法，统一管理
- **引用计数**：资源管理使用引用计数，自动释放

---

## 2. 系统要求

### 2.1 开发环境

| 项目 | 要求 |
|------|------|
| 编译器 | MSVC 2015+ / GCC 4.8+ / Clang 3.4+ |
| C++标准 | C++11 |
| CMake | 3.10+ |

### 2.2 依赖库

| 库 | 版本 | 说明 |
|----|------|------|
| ogc_geometry | 1.0+ | 几何类型和操作 |
| ogc_database | 1.0+ | 数据库连接和存储 |
| libpq | - | PostgreSQL客户端库 |
| sqlite3 | - | SQLite数据库 |
| libspatialite | - | 空间数据库扩展 |

### 2.3 测试框架

- Google Test 1.8+

---

## 3. 安装与配置

### 3.1 编译

```bash
# 创建构建目录
mkdir build && cd build

# 配置
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release

# 运行测试
ctest -C Release
```

### 3.2 CMake配置

```cmake
# 添加依赖
find_package(ogc_geometry REQUIRED)
find_package(ogc_database REQUIRED)

# 链接模块
target_link_libraries(your_target
    PRIVATE
        ogc_navi
        ogc_geometry
        ogc_database
)
```

### 3.3 输出文件

| 文件 | 说明 |
|------|------|
| ogc_navi.dll | Windows动态库 |
| ogc_navi.so | Linux共享库 |
| ogc_navi_tests.exe | 测试可执行文件 |

---

## 4. 快速开始

### 4.1 基本使用流程

```cpp
#include <ogc/navi/positioning/coordinate_converter.h>
#include <ogc/navi/route/route.h>
#include <ogc/navi/route/waypoint.h>
#include <ogc/navi/navigation/navigation_calculator.h>

using namespace ogc::navi;

// 1. 计算两点间距离
double distance = CoordinateConverter::Instance().CalculateGreatCircleDistance(
    31.2304, 121.4737,  // 上海
    39.9042, 116.4074   // 北京
);

// 2. 创建航线
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

// 3. 计算导航数据
PositionData current_pos;
current_pos.latitude = 31.5;
current_pos.longitude = 121.5;
current_pos.speed = 15.0;
current_pos.course = 0.0;

Waypoint* next_wp = route->GetWaypoint(0);
double bearing = CoordinateConverter::Instance().CalculateBearing(
    current_pos.latitude, current_pos.longitude,
    next_wp->GetLatitude(), next_wp->GetLongitude()
);

// 4. 清理资源
route->ReleaseReference();
```

### 4.2 NMEA解析示例

```cpp
#include <ogc/navi/positioning/nmea_parser.h>

using namespace ogc::navi;

NmeaParser& parser = NmeaParser::Instance();

// 解析GGA语句
std::string gga = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
GgaData gga_data;
if (parser.ParseGGA(gga, gga_data)) {
    printf("位置: %.6f, %.6f\n", gga_data.latitude, gga_data.longitude);
    printf("卫星数: %d\n", gga_data.satellite_count);
    printf("HDOP: %.2f\n", gga_data.hdop);
}

// 解析RMC语句
std::string rmc = "$GPRMC,092751.000,A,5321.6802,N,00630.3372,W,12.5,45.0,191124,,,A*5C";
RmcData rmc_data;
if (parser.ParseRMC(rmc, rmc_data)) {
    printf("位置: %.6f, %.6f\n", rmc_data.latitude, rmc_data.longitude);
    printf("速度: %.1f 节\n", rmc_data.speed_knots);
    printf("航向: %.1f°\n", rmc_data.track_angle);
}
```

### 4.3 航迹记录示例

```cpp
#include <ogc/navi/track/track.h>
#include <ogc/navi/track/track_point.h>

using namespace ogc::navi;

Track* track = Track::Create();
track->SetName("Morning Trip");

// 添加轨迹点
for (int i = 0; i < 100; ++i) {
    TrackPointData tp_data;
    tp_data.longitude = 121.4737 + i * 0.001;
    tp_data.latitude = 31.2304 + i * 0.001;
    tp_data.timestamp = 1000.0 + i * 60.0;
    tp_data.speed = 10.0;
    tp_data.course = 45.0;
    
    TrackPoint* tp = TrackPoint::Create(tp_data);
    track->AddPoint(tp);
    tp->ReleaseReference();
}

// 获取统计信息
printf("总距离: %.2f 米\n", track->GetTotalDistance());
printf("总时长: %.2f 秒\n", track->GetTotalDuration());
printf("点数: %d\n", track->GetPointCount());

// 简化轨迹
track->Simplify(10.0);  // 10米容差
printf("简化后点数: %d\n", track->GetPointCount());

track->ReleaseReference();
```

---

## 5. 模块详解

### 5.1 定位模块 (Positioning)

#### 5.1.1 NmeaParser - NMEA解析器

单例类，解析NMEA 0183标准语句。

**支持的语句类型：**

| 语句 | 说明 |
|------|------|
| GGA | GPS定位数据 |
| RMC | 推荐最小定位数据 |
| VTG | 航迹和航速 |
| GSA | GPS精度因子 |
| GSV | 可见卫星信息 |

**使用方法：**

```cpp
NmeaParser& parser = NmeaParser::Instance();

GgaData gga;
RmcData rmc;
VtgData vtg;
GsaData gsa;
GsvData gsv;

parser.ParseGGA(sentence, gga);
parser.ParseRMC(sentence, rmc);
parser.ParseVTG(sentence, vtg);
parser.ParseGSA(sentence, gsa);
parser.ParseGSV(sentence, gsv);
```

#### 5.1.2 CoordinateConverter - 坐标转换器

单例类，提供坐标转换和地理计算功能。

**主要方法：**

| 方法 | 说明 |
|------|------|
| `CalculateGreatCircleDistance` | 计算大圆距离 |
| `CalculateBearing` | 计算方位角 |
| `CalculateDestination` | 计算目标点坐标 |
| `LatLonToUtm` | 经纬度转UTM |
| `UtmToLatLon` | UTM转经纬度 |

**示例：**

```cpp
CoordinateConverter& converter = CoordinateConverter::Instance();

// 计算距离（米）
double distance = converter.CalculateGreatCircleDistance(
    lat1, lon1, lat2, lon2);

// 计算方位角（度，0-360）
double bearing = converter.CalculateBearing(
    lat1, lon1, lat2, lon2);

// 计算目标点
double dest_lat, dest_lon;
converter.CalculateDestination(lat1, lon1, bearing, distance, dest_lat, dest_lon);
```

#### 5.1.3 PositionFilter - 位置过滤器

过滤异常位置数据，平滑位置输出。

**配置参数：**

| 参数 | 默认值 | 说明 |
|------|--------|------|
| max_speed | 30.0 | 最大速度（节） |
| max_acceleration | 10.0 | 最大加速度（m/s²） |
| max_hdop | 5.0 | 最大HDOP |
| min_satellites | 4 | 最少卫星数 |

**使用方法：**

```cpp
PositionFilter filter;
filter.SetMaxSpeed(25.0);
filter.SetMaxHdop(3.0);

PositionData raw_pos;
raw_pos.latitude = 31.2304;
raw_pos.longitude = 121.4737;
raw_pos.hdop = 1.0;
raw_pos.satellite_count = 8;
raw_pos.quality = PositionQuality::Gps;

PositionData filtered = filter.Filter(raw_pos);

// 获取统计信息
FilterStatistics stats = filter.GetStatistics();
printf("有效: %d, 异常: %d\n", stats.valid_count, stats.outlier_count);
```

### 5.2 航线模块 (Route)

#### 5.2.1 Waypoint - 航点

表示航线中的一个航点。

**属性：**

| 属性 | 类型 | 说明 |
|------|------|------|
| id | string | 航点ID |
| name | string | 航点名称 |
| longitude | double | 经度 |
| latitude | double | 纬度 |
| arrival_radius | double | 到达半径（海里） |
| action | WaypointAction | 航点动作 |

**创建方法：**

```cpp
WaypointData data;
data.id = "WP001";
data.name = "Start Point";
data.longitude = 121.4737;
data.latitude = 31.2304;
data.arrival_radius = 0.5;
data.action = WaypointAction::None;

Waypoint* wp = Waypoint::Create(data);
// 使用...
wp->ReleaseReference();
```

#### 5.2.2 Route - 航线

管理航线的航点序列。

**主要方法：**

| 方法 | 说明 |
|------|------|
| `AddWaypoint` | 添加航点 |
| `RemoveWaypoint` | 移除航点 |
| `GetWaypoint` | 获取航点 |
| `GetWaypointCount` | 获取航点数量 |
| `CalculateTotalDistance` | 计算总距离 |
| `CalculateLegBearing` | 计算航段方位角 |

**示例：**

```cpp
Route* route = Route::Create();
route->SetName("Shanghai-Tokyo");
route->SetDeparture("Shanghai");
route->SetDestination("Tokyo");

// 添加航点
Waypoint* wp1 = Waypoint::Create(wp1_data);
Waypoint* wp2 = Waypoint::Create(wp2_data);
route->AddWaypoint(wp1);
route->AddWaypoint(wp2);
wp1->ReleaseReference();
wp2->ReleaseReference();

// 计算距离
double total = route->CalculateTotalDistance();
double bearing = route->CalculateLegBearing(0);

route->ReleaseReference();
```

### 5.3 导航模块 (Navigation)

#### 5.3.1 NavigationCalculator - 导航计算器

单例类，计算导航相关数据。

**计算项目：**

| 项目 | 方法 | 说明 |
|------|------|------|
| XTD | `CalculateCrossTrackError` | 偏航距离 |
| TTG | `CalculateTimeToWaypoint` | 到达时间 |
| ETA | `CalculateEstimatedTimeArrival` | 预计到达时间 |
| VMG | `CalculateVelocityMadeGood` | 有效航速 |

**示例：**

```cpp
NavigationCalculator& calc = NavigationCalculator::Instance();

// 计算偏航距离
GeoPoint position(121.5, 31.5);
GeoPoint start(121.0, 31.0);
GeoPoint end(122.0, 32.0);
double xtd = calc.CalculateCrossTrackError(position, start, end);

// 计算到达时间
double distance = 10000.0;  // 10公里
double speed = 10.0;        // 10节
double ttg = calc.CalculateTimeToWaypoint(distance, speed);
```

#### 5.3.2 OffCourseDetector - 偏航检测器

单例类，检测船舶是否偏离航线。

**配置参数：**

| 参数 | 说明 |
|------|------|
| xtd_warning | 偏航警告阈值（米） |
| xtd_alarm | 偏航报警阈值（米） |

**使用方法：**

```cpp
OffCourseDetector& detector = OffCourseDetector::Instance();
detector.SetXtdWarning(100.0);  // 100米警告
detector.SetXtdAlarm(200.0);    // 200米报警

OffCourseResult result = detector.Detect(position, start, end);
if (result.is_off_course) {
    printf("偏航距离: %.1f 米\n", result.xtd);
    printf("建议航向: %.1f°\n", result.recommended_bearing);
}
```

### 5.4 轨迹模块 (Track)

#### 5.4.1 TrackPoint - 轨迹点

表示轨迹中的一个点。

**属性：**

| 属性 | 类型 | 说明 |
|------|------|------|
| longitude | double | 经度 |
| latitude | double | 纬度 |
| timestamp | double | 时间戳 |
| speed | double | 速度（节） |
| course | double | 航向（度） |

#### 5.4.2 Track - 轨迹

管理轨迹点序列。

**主要方法：**

| 方法 | 说明 |
|------|------|
| `AddPoint` | 添加轨迹点 |
| `GetPoint` | 获取轨迹点 |
| `GetPointCount` | 获取点数量 |
| `GetTotalDistance` | 获取总距离 |
| `GetTotalDuration` | 获取总时长 |
| `Simplify` | 简化轨迹 |
| `GetPointsInTimeRange` | 按时间范围查询 |

**轨迹简化算法：**

使用Douglas-Peucker算法，根据容差参数简化轨迹。

```cpp
Track* track = Track::Create();
// 添加点...

// 简化轨迹（10米容差）
track->Simplify(10.0);

track->ReleaseReference();
```

### 5.5 AIS模块

#### 5.5.1 AisTarget - AIS目标

表示一个AIS目标船舶。

**属性：**

| 属性 | 类型 | 说明 |
|------|------|------|
| mmsi | uint32_t | MMSI号码 |
| ship_name | string | 船名 |
| callsign | string | 呼号 |
| longitude | double | 经度 |
| latitude | double | 纬度 |
| sog | double | 对地速度（节） |
| cog | double | 对地航向（度） |
| cpa | double | 最近会遇距离（米） |
| tcpa | double | 到达最近会遇点时间（秒） |

**碰撞风险评估：**

```cpp
AisTarget* target = AisTarget::Create(123456789);
target->SetCpa(500.0);   // 500米
target->SetTcpa(300.0);  // 5分钟

if (target->HasCollisionRisk()) {
    printf("碰撞风险！CPA=%.0f米, TCPA=%.0f秒\n", 
           target->GetCpa(), target->GetTcpa());
}

target->ReleaseReference();
```

---

## 6. API参考

详细API文档请参考：[API文档](./include/ogc/navi/index.md)

### 6.1 命名规范

| 规则 | 示例 |
|------|------|
| Getter方法使用Get前缀 | `GetSize()`, `GetName()` |
| 索引访问使用N后缀 | `GetWaypointN(index)` |
| 遵循OGC标准方法名 | `AsText()` 而非 `AsWKT()` |

### 6.2 资源管理

所有通过Create方法创建的对象需要调用ReleaseReference释放：

```cpp
Route* route = Route::Create();
// 使用...
route->ReleaseReference();  // 释放资源
route = nullptr;            // 置空指针
```

### 6.3 单例访问

单例类使用Instance方法获取实例：

```cpp
NmeaParser& parser = NmeaParser::Instance();
CoordinateConverter& converter = CoordinateConverter::Instance();
NavigationCalculator& calc = NavigationCalculator::Instance();
OffCourseDetector& detector = OffCourseDetector::Instance();
```

---

## 7. 常见问题

### Q1: 如何处理无效的NMEA语句？

```cpp
GgaData data;
if (!parser.ParseGGA(sentence, data)) {
    // 解析失败，检查语句格式
    printf("无效的GGA语句\n");
}
```

### Q2: 如何设置位置过滤器的参数？

```cpp
PositionFilter filter;
filter.SetMaxSpeed(25.0);        // 最大速度25节
filter.SetMaxAcceleration(5.0);  // 最大加速度5m/s²
filter.SetMaxHdop(3.0);          // 最大HDOP 3.0
filter.SetMinSatellites(6);      // 最少6颗卫星
```

### Q3: 如何判断航点是否到达？

```cpp
double distance = converter.CalculateGreatCircleDistance(
    current_lat, current_lon,
    wp->GetLatitude(), wp->GetLongitude());

if (distance < wp->GetArrivalRadius() * 1852.0) {  // 海里转米
    printf("到达航点: %s\n", wp->GetName().c_str());
}
```

### Q4: 如何简化轨迹以减少存储空间？

```cpp
// 原始轨迹有1000个点
printf("原始点数: %d\n", track->GetPointCount());

// 使用10米容差简化
track->Simplify(10.0);

printf("简化后点数: %d\n", track->GetPointCount());
```

---

## 8. 版本历史

### v1.0.0 (2026-04-05)

**新增功能：**
- 定位模块：NMEA解析、坐标转换、位置过滤
- 航线模块：航点管理、航线计算
- 导航模块：导航计算、偏航检测
- 轨迹模块：轨迹记录、简化算法
- AIS模块：目标管理、碰撞评估

**测试覆盖：**
- 单元测试：259个测试用例
- 集成测试：8个测试用例
- 性能测试：14个基准测试
- 总计：281个测试，100%通过

---

## 技术支持

如有问题，请联系开发团队或查阅详细设计文档。

**相关文档：**
- [设计文档](./doc/chart_navi_system_design.md)
- [API文档](./include/ogc/navi/index.md)
- [测试报告](./tests/navi_test_quality.md)
