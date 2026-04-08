# Navi Module - Header Index

## 模块描述

Navi模块提供完整的航海导航功能，包括AIS目标管理、航线规划与监控、航迹记录、定位管理、导航计算等。支持NMEA解析、AIS消息解析、碰撞评估、偏航检测、UKC计算等功能。

## 核心特性

- AIS目标管理与消息解析
- 航线规划、监控与偏航检测
- 航迹记录与回放
- 多源定位（GPS、DGPS、RTK）
- NMEA协议解析
- 碰撞风险评估
- UKC（富余水深）计算
- 坐标转换
- 导航警报

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/navi/export.h) | DLL导出宏 | `OGC_NAVI_API` |
| [types.h](ogc/navi/types.h) | 类型定义 | `PositionQuality`, `NavigationStatus` |
| **AIS** | | |
| [ais_manager.h](ogc/navi/ais/ais_manager.h) | AIS管理器 | `AisManager` |
| [ais_message.h](ogc/navi/ais/ais_message.h) | AIS消息 | `AisMessage` |
| [ais_parser.h](ogc/navi/ais/ais_parser.h) | AIS解析器 | `AisParser` |
| [ais_target.h](ogc/navi/ais/ais_target.h) | AIS目标 | `AisTarget` |
| [collision_assessor.h](ogc/navi/ais/collision_assessor.h) | 碰撞评估 | `CollisionAssessor` |
| **Navigation** | | |
| [navigation_engine.h](ogc/navi/navigation/navigation_engine.h) | 导航引擎 | `NavigationEngine` |
| [navigation_calculator.h](ogc/navi/navigation/navigation_calculator.h) | 导航计算 | `NavigationCalculator` |
| [navigation_alerter.h](ogc/navi/navigation/navigation_alerter.h) | 导航警报 | `NavigationAlerter` |
| [off_course_detector.h](ogc/navi/navigation/off_course_detector.h) | 偏航检测 | `OffCourseDetector` |
| **Positioning** | | |
| [position_manager.h](ogc/navi/positioning/position_manager.h) | 定位管理 | `PositionManager` |
| [position_provider.h](ogc/navi/positioning/position_provider.h) | 定位源 | `IPositionProvider` |
| [position_filter.h](ogc/navi/positioning/position_filter.h) | 定位滤波 | `PositionFilter` |
| [nmea_parser.h](ogc/navi/positioning/nmea_parser.h) | NMEA解析 | `NmeaParser` |
| [coordinate_converter.h](ogc/navi/positioning/coordinate_converter.h) | 坐标转换 | `CoordinateConverter` |
| **Route** | | |
| [route.h](ogc/navi/route/route.h) | 航线 | `Route` |
| [route_manager.h](ogc/navi/route/route_manager.h) | 航线管理 | `RouteManager` |
| [route_planner.h](ogc/navi/route/route_planner.h) | 航线规划 | `RoutePlanner` |
| [waypoint.h](ogc/navi/route/waypoint.h) | 航路点 | `Waypoint` |
| [ukc_calculator.h](ogc/navi/route/ukc_calculator.h) | UKC计算 | `UkcCalculator` |
| **Track** | | |
| [track.h](ogc/navi/track/track.h) | 航迹 | `Track` |
| [track_manager.h](ogc/navi/track/track_manager.h) | 航迹管理 | `TrackManager` |
| [track_point.h](ogc/navi/track/track_point.h) | 航迹点 | `TrackPoint` |
| [track_recorder.h](ogc/navi/track/track_recorder.h) | 航迹记录 | `TrackRecorder` |
| [track_player.h](ogc/navi/track/track_player.h) | 航迹回放 | `TrackPlayer` |

---

## 类继承关系图

```
IPositionProvider (interface)
    ├── SerialPositionProvider
    ├── BluetoothPositionProvider
    ├── NetworkPositionProvider
    └── FilePositionProvider

ITrackRecorder (interface)
    └── TrackRecorder

Route
    └── RouteImpl

Track
    └── TrackImpl

AisTarget
    └── AisTargetImpl
```

---

## 依赖关系图

```
types.h
    │
    ├──► ais/
    │       ├── ais_manager.h ──► ais_target.h
    │       ├── ais_parser.h ──► ais_message.h
    │       └── collision_assessor.h
    │
    ├──► navigation/
    │       ├── navigation_engine.h
    │       ├── navigation_calculator.h
    │       ├── navigation_alerter.h
    │       └── off_course_detector.h
    │
    ├──► positioning/
    │       ├── position_manager.h ──► position_provider.h
    │       ├── position_filter.h
    │       ├── nmea_parser.h
    │       └── coordinate_converter.h
    │
    ├──► route/
    │       ├── route.h ──► waypoint.h
    │       ├── route_manager.h
    │       ├── route_planner.h
    │       └── ukc_calculator.h
    │
    └──► track/
            ├── track.h ──► track_point.h
            ├── track_manager.h
            └── track_recorder.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | types.h, export.h |
| **AIS** | ais_manager.h, ais_message.h, ais_parser.h, ais_target.h, collision_assessor.h |
| **Navigation** | navigation_engine.h, navigation_calculator.h, navigation_alerter.h, off_course_detector.h |
| **Positioning** | position_manager.h, position_provider.h, position_filter.h, nmea_parser.h, coordinate_converter.h |
| **Route** | route.h, route_manager.h, route_planner.h, waypoint.h, ukc_calculator.h |
| **Track** | track.h, track_manager.h, track_point.h, track_recorder.h |

---

## 关键类

### Route
**File**: [route.h](ogc/navi/route/route.h)  
**Description**: 航线类

```cpp
struct RouteData {
    std::string id;
    std::string name;
    std::string description;
    RouteStatus status;
    double total_distance;
    double estimated_duration;
    std::string departure;
    std::string destination;
    std::vector<WaypointData> waypoints;
};

class Route {
public:
    static Route* Create();
    static Route* Create(const RouteData& data);
    
    const std::string& GetId() const;
    const std::string& GetName() const;
    RouteStatus GetStatus() const;
    double GetTotalDistance() const;
    
    void AddWaypoint(const WaypointData& wp);
    void RemoveWaypoint(int index);
    Waypoint* GetWaypoint(int index);
    int GetWaypointCount() const;
};
```

### AisManager
**File**: [ais_manager.h](ogc/navi/ais/ais_manager.h)  
**Description**: AIS目标管理器

```cpp
class AisManager {
public:
    void Initialize();
    void Shutdown();
    
    void ProcessMessage(const AisMessage& msg);
    AisTarget* GetTarget(uint32_t mmsi);
    std::vector<AisTarget*> GetAllTargets();
    std::vector<AisTarget*> GetTargetsInRange(const Coordinate& center, double range);
    
    void SetTargetCallback(std::function<void(const AisTarget&)> callback);
};
```

### NavigationEngine
**File**: [navigation_engine.h](ogc/navi/navigation/navigation_engine.h)  
**Description**: 导航引擎

```cpp
class NavigationEngine {
public:
    void SetRoute(Route* route);
    void StartNavigation();
    void StopNavigation();
    void PauseNavigation();
    
    NavigationStatus GetStatus() const;
    Waypoint* GetCurrentWaypoint();
    Waypoint* GetNextWaypoint();
    
    double GetDistanceToNextWaypoint();
    double GetTimeToNextWaypoint();
    double GetCrossTrackError();
    double GetBearingToNextWaypoint();
};
```

### PositionManager
**File**: [position_manager.h](ogc/navi/positioning/position_manager.h)  
**Description**: 定位管理器

```cpp
class PositionManager {
public:
    void AddProvider(IPositionProvider* provider);
    void RemoveProvider(IPositionProvider* provider);
    void SetPrimaryProvider(IPositionProvider* provider);
    
    Coordinate GetCurrentPosition();
    PositionQuality GetPositionQuality();
    double GetHeading();
    double GetSpeed();
    
    void SetPositionCallback(std::function<void(const Coordinate&)> callback);
};
```

---

## 接口

### IPositionProvider
```cpp
virtual bool Initialize() = 0;
virtual void Shutdown() = 0;
virtual bool HasFix() const = 0;
virtual Coordinate GetPosition() const = 0;
virtual PositionQuality GetQuality() const = 0;
```

### ITrackRecorder
```cpp
virtual void Start() = 0;
virtual void Stop() = 0;
virtual void Record(const TrackPoint& point) = 0;
virtual bool IsRecording() const = 0;
```

---

## 类型定义

### PositionQuality (定位质量)
```cpp
enum class PositionQuality {
    Invalid = 0,
    Gps = 1,
    DGps = 2,
    Pps = 3,
    Rtk = 4,
    FloatRtk = 5,
    Simulation = 6,
    ManualInput = 7
};
```

### PositionSource (定位源)
```cpp
enum class PositionSource {
    Unknown = 0,
    System = 1,
    Serial = 2,
    Bluetooth = 3,
    Network = 4,
    File = 5
};
```

### NavigationStatus (导航状态)
```cpp
enum class NavigationStatus {
    Inactive = 0,
    Active = 1,
    Paused = 2,
    Completed = 3,
    OffRoute = 4,
    Arrived = 5
};
```

### RouteStatus (航线状态)
```cpp
enum class RouteStatus {
    Planned = 0,
    Active = 1,
    Completed = 2,
    Cancelled = 3
};
```

### WaypointAction (航路点动作)
```cpp
enum class WaypointAction {
    None = 0,
    Turn = 1,
    Stop = 2,
    SpeedChange = 3,
    CourseChange = 4
};
```

---

## 使用示例

### 航线规划

```cpp
#include "ogc/navi/route/route.h"
#include "ogc/navi/route/waypoint.h"

using namespace ogc::navi;

Route* route = Route::Create();
route->SetName("Shanghai to Tokyo");

WaypointData wp1;
wp1.name = "Shanghai";
wp1.position = Coordinate(121.5, 31.2);
route->AddWaypoint(wp1);

WaypointData wp2;
wp2.name = "Tokyo";
wp2.position = Coordinate(139.7, 35.7);
route->AddWaypoint(wp2);
```

### AIS目标处理

```cpp
#include "ogc/navi/ais/ais_manager.h"
#include "ogc/navi/ais/ais_parser.h"

AisManager aisMgr;
aisMgr.Initialize();

AisParser parser;
AisMessage msg = parser.Parse(raw_data);
aisMgr.ProcessMessage(msg);

std::vector<AisTarget*> targets = aisMgr.GetTargetsInRange(myPos, 10.0);
```

### 导航执行

```cpp
#include "ogc/navi/navigation/navigation_engine.h"

NavigationEngine navEngine;
navEngine.SetRoute(route);
navEngine.StartNavigation();

while (navEngine.GetStatus() == NavigationStatus::Active) {
    double xte = navEngine.GetCrossTrackError();
    double dist = navEngine.GetDistanceToNextWaypoint();
}
```

### 定位管理

```cpp
#include "ogc/navi/positioning/position_manager.h"
#include "ogc/navi/positioning/nmea_parser.h"

PositionManager posMgr;
posMgr.AddProvider(serialProvider);
posMgr.SetPrimaryProvider(serialProvider);

Coordinate pos = posMgr.GetCurrentPosition();
double heading = posMgr.GetHeading();
double speed = posMgr.GetSpeed();
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加AIS碰撞评估 |
| v1.2 | 2026-03-10 | Team | 添加UKC计算 |
| v1.3 | 2026-04-06 | index-validator | 修正章节顺序 |
| v1.4 | 2026-04-09 | index-validator | 移除index.md(非头文件)，补充track_player.h |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
