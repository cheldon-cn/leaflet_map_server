# Alert Module - Header Index

## 模块描述

Alert模块提供完整的航海警报系统，包括深度警报、天气警报、碰撞警报、航道警报、偏航警报、速度警报、限制区域警报等。支持多通道推送、警报去重、阈值管理、用户配置等功能。

## 核心特性

- 多种警报类型：深度、天气、碰撞、航道、偏航、速度、限制区域
- 多级警报等级（Level1-4）
- 多通道推送：App、声音、短信、邮件
- 警报去重和聚合
- 阈值动态管理
- CPA/TCPA计算
- UKC（富余水深）计算
- 偏航检测
- 天气融合
- 用户配置存储

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/alert/export.h) | DLL导出宏 | `OGC_ALERT_API` |
| [types.h](ogc/alert/types.h) | 类型定义 | `AlertType`, `AlertLevel`, `AlertStatus` |
| [exception.h](ogc/alert/exception.h) | 异常定义 | `AlertException` |
| [alert_engine.h](ogc/alert/alert_engine.h) | 警报引擎 | `AlertEngine` |
| [alert_checker.h](ogc/alert/alert_checker.h) | 警报检查器基类 | `IAlertChecker` |
| [alert_processor.h](ogc/alert/alert_processor.h) | 警报处理器 | `IAlertProcessor` |
| [alert_repository.h](ogc/alert/alert_repository.h) | 警报存储 | `AlertRepository` |
| [depth_alert_checker.h](ogc/alert/depth_alert_checker.h) | 深度警报检查 | `DepthAlertChecker` |
| [weather_alert_checker.h](ogc/alert/weather_alert_checker.h) | 天气警报检查 | `WeatherAlertChecker` |
| [collision_alert_checker.h](ogc/alert/collision_alert_checker.h) | 碰撞警报检查 | `CollisionAlertChecker` |
| [channel_alert_checker.h](ogc/alert/channel_alert_checker.h) | 航道警报检查 | `ChannelAlertChecker` |
| [deviation_alert_checker.h](ogc/alert/deviation_alert_checker.h) | 偏航警报检查 | `DeviationAlertChecker` |
| [speed_alert_checker.h](ogc/alert/speed_alert_checker.h) | 速度警报检查 | `SpeedAlertChecker` |
| [restricted_area_checker.h](ogc/alert/restricted_area_checker.h) | 限制区域检查 | `RestrictedAreaChecker` |
| [cpa_calculator.h](ogc/alert/cpa_calculator.h) | CPA计算 | `CpaCalculator` |
| [ukc_calculator.h](ogc/alert/ukc_calculator.h) | UKC计算 | `UkcCalculator` |
| [deviation_calculator.h](ogc/alert/deviation_calculator.h) | 偏航计算 | `DeviationCalculator` |
| [spatial_distance.h](ogc/alert/spatial_distance.h) | 空间距离 | `SpatialDistance` |
| [speed_zone_matcher.h](ogc/alert/speed_zone_matcher.h) | 速度区域匹配 | `SpeedZoneMatcher` |
| [weather_fusion.h](ogc/alert/weather_fusion.h) | 天气融合 | `WeatherFusion` |
| [threshold_manager.h](ogc/alert/threshold_manager.h) | 阈值管理 | `ThresholdManager` |
| [config_service.h](ogc/alert/config_service.h) | 配置服务 | `ConfigService` |
| [user_config_store.h](ogc/alert/user_config_store.h) | 用户配置存储 | `UserConfigStore` |
| [push_service.h](ogc/alert/push_service.h) | 推送服务 | `PushService` |
| [push_strategy.h](ogc/alert/push_strategy.h) | 推送策略 | `PushStrategy` |
| [acknowledge_service.h](ogc/alert/acknowledge_service.h) | 确认服务 | `AcknowledgeService` |
| [feedback_service.h](ogc/alert/feedback_service.h) | 反馈服务 | `FeedbackService` |
| [query_service.h](ogc/alert/query_service.h) | 查询服务 | `QueryService` |
| [deduplicator.h](ogc/alert/deduplicator.h) | 去重器 | `Deduplicator` |
| [scheduler.h](ogc/alert/scheduler.h) | 调度器 | `IScheduler` |
| [cache.h](ogc/alert/cache.h) | 缓存 | `AlertCache` |
| [data_source_manager.h](ogc/alert/data_source_manager.h) | 数据源管理 | `DataSourceManager` |
| [notice_parser.h](ogc/alert/notice_parser.h) | 通告解析 | `NoticeParser` |
| [rest_controller.h](ogc/alert/rest_controller.h) | REST控制器 | `RestController` |
| [websocket_service.h](ogc/alert/websocket_service.h) | WebSocket服务 | `WebSocketService` |
| [performance_benchmark.h](ogc/alert/performance_benchmark.h) | 性能基准 | `PerformanceBenchmark` |
| [performance_test.h](ogc/alert/performance_test.h) | 性能测试 | `PerformanceTimer`, `PerformanceProfiler`, `PerformanceOptimizer` |

---

## 类继承关系图

```
IAlertChecker (interface)
    ├── DepthAlertChecker
    ├── WeatherAlertChecker
    ├── CollisionAlertChecker
    ├── ChannelAlertChecker
    ├── DeviationAlertChecker
    ├── SpeedAlertChecker
    └── RestrictedAreaChecker

IAlertProcessor (interface)
    └── AlertProcessor

IScheduler (interface)
    └── Scheduler

PushStrategy (interface)
    ├── ImmediatePushStrategy
    ├── BatchPushStrategy
    └── PriorityPushStrategy
```

---

## 依赖关系图

```
types.h
    │
    ├──► alert_engine.h ──► scheduler.h
    │         │
    │         └──► alert_checker.h
    │                   │
    │                   ├──► depth_alert_checker.h ──► ukc_calculator.h
    │                   ├──► weather_alert_checker.h ──► weather_fusion.h
    │                   ├──► collision_alert_checker.h ──► cpa_calculator.h
    │                   ├──► channel_alert_checker.h
    │                   ├──► deviation_alert_checker.h ──► deviation_calculator.h
    │                   ├──► speed_alert_checker.h ──► speed_zone_matcher.h
    │                   └──► restricted_area_checker.h
    │
    ├──► alert_processor.h ──► deduplicator.h
    │
    ├──► push_service.h ──► push_strategy.h
    │
    ├──► threshold_manager.h ──► user_config_store.h
    │
    └──► rest_controller.h
            │
            └──► websocket_service.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | export.h, types.h, exception.h, alert_engine.h |
| **Checkers** | alert_checker.h, depth_alert_checker.h, weather_alert_checker.h, collision_alert_checker.h, channel_alert_checker.h, deviation_alert_checker.h, speed_alert_checker.h, restricted_area_checker.h |
| **Calculators** | cpa_calculator.h, ukc_calculator.h, deviation_calculator.h, spatial_distance.h, speed_zone_matcher.h |
| **Services** | push_service.h, acknowledge_service.h, feedback_service.h, query_service.h, config_service.h |
| **Management** | threshold_manager.h, user_config_store.h, data_source_manager.h |
| **Processing** | alert_processor.h, alert_repository.h, deduplicator.h, scheduler.h, cache.h |
| **Weather** | weather_fusion.h |
| **API** | rest_controller.h, websocket_service.h |
| **Utility** | push_strategy.h, notice_parser.h, performance_benchmark.h, performance_test.h |

---

## 关键类

### AlertEngine
**File**: [alert_engine.h](ogc/alert/alert_engine.h)  
**Description**: 警报引擎核心类

```cpp
struct EngineConfig {
    int check_interval_ms;
    int max_concurrent_checks;
    bool enable_deduplication;
    bool enable_aggregation;
    int dedup_window_seconds;
};

struct AlertStatistics {
    int total_alerts_generated;
    int total_alerts_pushed;
    int total_alerts_acknowledged;
    int active_alerts;
    DateTime last_check_time;
};

class AlertEngine {
public:
    void Initialize(const EngineConfig& config);
    void Start();
    void Stop();
    
    void AddChecker(IAlertCheckerPtr checker);
    void RemoveChecker(const std::string& name);
    
    void ProcessEvent(const AlertEvent& event);
    AlertStatistics GetStatistics() const;
};
```

### IAlertChecker
**File**: [alert_checker.h](ogc/alert/alert_checker.h)  
**Description**: 警报检查器接口

```cpp
class IAlertChecker {
public:
    virtual ~IAlertChecker() = default;
    
    virtual std::string GetName() const = 0;
    virtual AlertType GetType() const = 0;
    
    virtual std::vector<Alert> Check(const CheckContext& context) = 0;
    virtual void Configure(const Config& config) = 0;
};
```

### DepthAlertChecker
**File**: [depth_alert_checker.h](ogc/alert/depth_alert_checker.h)  
**Description**: 深度警报检查器

```cpp
class DepthAlertChecker : public IAlertChecker {
public:
    std::string GetName() const override { return "DepthAlert"; }
    AlertType GetType() const override { return AlertType::kDepth; }
    
    std::vector<Alert> Check(const CheckContext& context) override;
    void SetDepthThreshold(double depth);
    void SetUkcThreshold(double ukc);
};
```

### CpaCalculator
**File**: [cpa_calculator.h](ogc/alert/cpa_calculator.h)  
**Description**: CPA/TCPA计算器

```cpp
struct CpaResult {
    double cpa;           // 最近会遇距离
    double tcpa;          // 最近会遇时间
    double dcpa;          // DCPA
    Coordinate cpa_point; // CPA点
    bool is_dangerous;    // 是否危险
};

class CpaCalculator {
public:
    CpaResult Calculate(
        const Coordinate& own_pos,
        double own_speed,
        double own_heading,
        const Coordinate& target_pos,
        double target_speed,
        double target_heading
    );
};
```

### PerformanceTimer
**File**: [performance_test.h](ogc/alert/performance_test.h)  
**Description**: 性能计时器

```cpp
class PerformanceTimer {
public:
    PerformanceTimer();
    explicit PerformanceTimer(const std::string& name);
    ~PerformanceTimer();
    
    void Start();
    void Stop();
    void Reset();
    
    double GetElapsedMs() const;
    double GetElapsedSeconds() const;
    
    static double GetTimestampMs();
};

class PerformanceProfiler {
public:
    void BeginSession(const std::string& session_name);
    void EndSession();
    
    void StartOperation(const std::string& operation_name);
    void EndOperation(const std::string& operation_name);
    
    void RecordMetric(const std::string& operation_name, double time_ms);
    
    PerformanceMetrics GetMetrics(const std::string& operation_name) const;
    std::vector<PerformanceMetrics> GetAllMetrics() const;
    
    PerformanceReport GenerateReport() const;
    
    static PerformanceProfiler& Instance();
};
```

---

## 接口

### IAlertChecker
```cpp
virtual std::string GetName() const = 0;
virtual AlertType GetType() const = 0;
virtual std::vector<Alert> Check(const CheckContext& context) = 0;
```

### IAlertProcessor
```cpp
virtual void Process(const Alert& alert) = 0;
virtual void ProcessBatch(const std::vector<Alert>& alerts) = 0;
```

### IScheduler
```cpp
virtual void Schedule(Task task, int interval_ms) = 0;
virtual void Cancel(const std::string& task_id) = 0;
```

---

## 类型定义

### AlertType (警报类型)
```cpp
enum class AlertType : uint8_t {
    kUnknown = 0,
    kDepth = 1,
    kWeather = 2,
    kCollision = 3,
    kChannel = 4,
    kDeviation = 5,
    kSpeed = 6,
    kRestrictedArea = 7
};
```

### AlertLevel (警报等级)
```cpp
enum class AlertLevel : uint8_t {
    kNone = 0,
    kLevel1 = 1,   // 提示
    kLevel2 = 2,   // 警告
    kLevel3 = 3,   // 严重
    kLevel4 = 4    // 紧急
};
```

### AlertStatus (警报状态)
```cpp
enum class AlertStatus : uint8_t {
    kActive = 0,       // 活动中
    kAcknowledged = 1, // 已确认
    kExpired = 2,      // 已过期
    kDismissed = 3     // 已忽略
};
```

### PushMethod (推送方式)
```cpp
enum class PushMethod : uint8_t {
    kApp = 0,
    kSound = 1,
    kSms = 2,
    kEmail = 3
};
```

### WeatherType (天气类型)
```cpp
enum class WeatherType : uint8_t {
    kClear = 0,
    kCloudy = 1,
    kRain = 2,
    kHeavyRain = 3,
    kThunderstorm = 4,
    kFog = 5,
    kSnow = 6,
    kIce = 7,
    kHighWind = 8,
    kStorm = 9,
    kHurricane = 10
};
```

---

## 使用示例

### 初始化警报引擎

```cpp
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/depth_alert_checker.h"

using namespace ogc::alert;

EngineConfig config;
config.check_interval_ms = 1000;
config.enable_deduplication = true;

AlertEngine engine;
engine.Initialize(config);

engine.AddChecker(std::make_shared<DepthAlertChecker>());
engine.Start();
```

### 处理事件

```cpp
AlertEvent event;
event.event_id = "evt_001";
event.event_type = "position_update";
event.ship_position = Coordinate(121.5, 31.2);
event.ship_speed = 12.5;
event.ship_heading = 180.0;

engine.ProcessEvent(event);
```

### 配置阈值

```cpp
#include "ogc/alert/threshold_manager.h"

ThresholdManager thresholdMgr;
thresholdMgr.SetDepthThreshold("ship_001", 10.0);
thresholdMgr.SetCpaThreshold("ship_001", 2.0);
```

### 性能测试

```cpp
#include "ogc/alert/performance_test.h"

auto& profiler = PerformanceProfiler::Instance();
profiler.BeginSession("alert_check");

profiler.StartOperation("depth_check");
DepthAlertChecker checker;
auto alerts = checker.Check(context);
profiler.EndOperation("depth_check");

auto report = profiler.GenerateReport();
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加WebSocket推送 |
| v1.2 | 2026-03-10 | Team | 添加天气融合功能 |
| v1.3 | 2026-04-06 | index-validator | 补充缺失头文件、修正章节顺序 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
