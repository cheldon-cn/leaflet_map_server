# Alarm Module - Header Index

## 模块描述

Alarm模块是警报服务层，提供REST/WebSocket服务接口，适配ogc_alert模块的核心警报功能。该模块作为警报系统的对外服务层，负责HTTP请求处理、WebSocket推送、配置管理等服务端功能。

**模块定位**：
- 服务层：提供REST API和WebSocket服务
- 适配层：适配ogc_alert核心模块的接口
- 配置层：管理警报阈值、规则、通知配置

**命名空间**: `alert` (注意：与ogc_alert模块的`ogc::alert`命名空间不同)

## 核心特性

- REST API服务：警报查询、确认、统计接口
- WebSocket服务：实时警报推送
- 多通道推送：App、SMS、Email、Http
- 警报引擎适配：适配ogc_alert的IAlertChecker接口
- 阈值配置管理：动态配置警报阈值
- 规则配置管理：警报规则和通知配置
- 警报判断服务：多引擎协调评估
- 数据访问层：警报持久化存储

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [alert.h](alert/alert.h) | 警报实体 | `Alert`, `AlertPosition`, `PushRecord` |
| [alert_types.h](alert/alert_types.h) | 类型定义 | `AlertType`, `AlertLevel`, `AlertStatus`, `ShipType`, `PushMethod` |
| [coordinate.h](alert/coordinate.h) | 坐标类 | `Coordinate` |
| [ship_info.h](alert/ship_info.h) | 船舶信息 | `ShipInfo` |
| [i_alert_engine.h](alert/i_alert_engine.h) | 警报引擎接口 | `IAlertEngine` |
| [alert_context.h](alert/alert_context.h) | 警报上下文 | `AlertContext`, `EnvironmentData` |
| [threshold_config.h](alert/threshold_config.h) | 阈值配置 | `ThresholdConfig` |
| [alert_engine_factory.h](alert/alert_engine_factory.h) | 引擎工厂 | `AlertEngineFactory`, `EngineRegistrar` |
| [depth_alert_engine.h](alert/depth_alert_engine.h) | 深度警报引擎 | `DepthAlertEngine` |
| [collision_alert_engine.h](alert/collision_alert_engine.h) | 碰撞警报引擎 | `CollisionAlertEngine`, `AISTarget`, `CollisionTarget` |
| [weather_alert_engine.h](alert/weather_alert_engine.h) | 天气警报引擎 | `WeatherAlertEngine` |
| [channel_alert_engine.h](alert/channel_alert_engine.h) | 航道警报引擎 | `ChannelAlertEngine` |
| [alert_judge_service.h](alert/alert_judge_service.h) | 警报判断服务 | `IAlertJudgeService`, `AlertJudgeService` |
| [alert_push_service.h](alert/alert_push_service.h) | 警报推送服务 | `IAlertPushService`, `AlertPushService` |
| [alert_config_service.h](alert/alert_config_service.h) | 配置服务 | `IAlertConfigService`, `AlertConfigService`, `AlertRule`, `NotificationConfig` |
| [alert_repository.h](alert/alert_repository.h) | 警报存储 | `IAlertRepository`, `AlertRepository` |
| [alert_data_access.h](alert/alert_data_access.h) | 数据访问 | `IAlertDataAccess`, `AlertQueryCriteria`, `AlertStatistics` |
| [rest_controller.h](alert/rest_controller.h) | REST控制器 | `IRestController`, `RestController`, `HttpRequest`, `HttpResponse` |
| [websocket_service.h](alert/websocket_service.h) | WebSocket服务 | `IWebSocketService`, `WebSocketConfig`, `WebSocketClient` |
| [checker_adapter.h](alert/checker_adapter.h) | 检查器适配 | `CheckerAdapter`, `CheckerRegistry` |
| [rest_adapter.h](alert/rest_adapter.h) | REST适配器 | `RestAdapter`, `AlertControllerAdapter`, `ConfigControllerAdapter` |
| [repository_adapter.h](alert/repository_adapter.h) | 存储适配器 | `RepositoryAdapter` |
| [push_channel_adapter.h](alert/push_channel_adapter.h) | 推送通道适配 | `PushChannelAdapter` |
| [alert_type_adapter.h](alert/alert_type_adapter.h) | 类型适配器 | `AlertTypeAdapter` |
| [ais_data_adapter.h](alert/ais_data_adapter.h) | AIS数据适配 | `AisDataAdapter` |
| [chart_data_adapter.h](alert/chart_data_adapter.h) | 海图数据适配 | `ChartDataAdapter` |
| [weather_data_adapter.h](alert/weather_data_adapter.h) | 天气数据适配 | `WeatherDataAdapter` |
| [external_data_gateway.h](alert/external_data_gateway.h) | 外部数据网关 | `ExternalDataGateway` |

---

## 类继承关系图

```
IAlertEngine (interface)
    ├── DepthAlertEngine
    ├── CollisionAlertEngine
    ├── WeatherAlertEngine
    └── ChannelAlertEngine

IAlertJudgeService (interface)
    └── AlertJudgeService

IAlertPushService (interface)
    └── AlertPushService

IAlertConfigService (interface)
    └── AlertConfigService

IAlertRepository (interface)
    └── AlertRepository

IAlertDataAccess (interface)
    └── (实现类)

IRestController (interface)
    └── RestController

IWebSocketService (interface)
    └── WebSocketService

ogc::alert::IRestController (ogc_alert模块接口)
    └── RestAdapter (适配器)

ogc::alert::IAlertChecker (ogc_alert模块接口)
    └── CheckerAdapter (适配器)
```

---

## 依赖关系图

```
                    ┌─────────────────────────────────────────┐
                    │           ogc_alert 模块                │
                    │  (ogc::alert 命名空间)                   │
                    │  - IAlertChecker                        │
                    │  - AlertEngine                          │
                    │  - IAlertRepository                     │
                    │  - IRestController                      │
                    │  - PushService                          │
                    └──────────────────┬──────────────────────┘
                                       │
                                       ▼ 依赖
┌─────────────────────────────────────────────────────────────────────────────┐
│                           alarm 模块 (alert 命名空间)                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  alert_types.h                                                              │
│       │                                                                     │
│       ├──► alert.h ──► alert_context.h                                      │
│       │         │                                                           │
│       │         └──► alert_repository.h ──► alert_data_access.h             │
│       │                                                                     │
│       ├──► i_alert_engine.h                                                 │
│       │         │                                                           │
│       │         ├──► depth_alert_engine.h                                   │
│       │         ├──► collision_alert_engine.h                               │
│       │         ├──► weather_alert_engine.h                                 │
│       │         └──► channel_alert_engine.h                                 │
│       │                                                                     │
│       ├──► alert_engine_factory.h                                           │
│       │                                                                     │
│       ├──► alert_judge_service.h                                            │
│       │                                                                     │
│       ├──► alert_push_service.h                                             │
│       │                                                                     │
│       └──► alert_config_service.h                                           │
│                                                                             │
│  适配器层 (Adapter Layer):                                                   │
│  checker_adapter.h ──► ogc/alert/alert_checker.h                            │
│  rest_adapter.h ──► ogc/alert/rest_controller.h                             │
│  repository_adapter.h ──► ogc/alert/alert_repository.h                      │
│  push_channel_adapter.h ──► ogc/alert/push_service.h                        │
│  alert_type_adapter.h ──► ogc/alert/types.h                                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | alert.h, alert_types.h, coordinate.h, ship_info.h |
| **Engine** | i_alert_engine.h, depth_alert_engine.h, collision_alert_engine.h, weather_alert_engine.h, channel_alert_engine.h |
| **Service** | alert_judge_service.h, alert_push_service.h, alert_config_service.h |
| **Data** | alert_repository.h, alert_data_access.h, threshold_config.h |
| **API** | rest_controller.h, websocket_service.h |
| **Adapter** | checker_adapter.h, rest_adapter.h, repository_adapter.h, push_channel_adapter.h, alert_type_adapter.h |
| **Provider** | ais_data_adapter.h, chart_data_adapter.h, weather_data_adapter.h, external_data_gateway.h |

---

## 关键类

### Alert (警报实体)
**File**: [alert.h](alert/alert.h)  
**Description**: 警报实体类，包含警报的所有属性

```cpp
class Alert {
public:
    Alert();
    Alert(const std::string& alertId, AlertType type, AlertLevel level);
    
    const std::string& GetAlertId() const;
    AlertType GetAlertType() const;
    AlertLevel GetAlertLevel() const;
    AlertStatus GetStatus() const;
    
    const AlertPosition& GetPosition() const;
    void SetPosition(double lon, double lat);
    
    bool IsActive() const;
    bool IsExpired() const;
    
    static std::string GenerateAlertId(AlertType type);
};
```

### IAlertEngine (警报引擎接口)
**File**: [i_alert_engine.h](alert/i_alert_engine.h)  
**Description**: 警报引擎抽象接口

```cpp
class IAlertEngine {
public:
    virtual ~IAlertEngine() {}
    
    virtual Alert Evaluate(const AlertContext& context) = 0;
    virtual AlertType GetType() const = 0;
    virtual void SetThreshold(const ThresholdConfig& config) = 0;
    virtual ThresholdConfig GetThreshold() const = 0;
    virtual std::string GetName() const = 0;
};
```

### AlertJudgeService (警报判断服务)
**File**: [alert_judge_service.h](alert/alert_judge_service.h)  
**Description**: 协调多个警报引擎进行评估

```cpp
class IAlertJudgeService {
public:
    virtual std::vector<Alert> Evaluate(const AlertContext& context) = 0;
    virtual std::vector<Alert> EvaluateByType(const AlertContext& context, AlertType type) = 0;
    virtual void EnableEngine(AlertType type, bool enable) = 0;
    virtual void SetEngineThreshold(AlertType type, const ThresholdConfig& config) = 0;
};
```

### CheckerAdapter (检查器适配器)
**File**: [checker_adapter.h](alert/checker_adapter.h)  
**Description**: 适配ogc_alert模块的IAlertChecker接口

```cpp
class CheckerAdapter : public alert::IAlertEngine {
public:
    explicit CheckerAdapter(ogc::alert::IAlertCheckerPtr checker);
    
    alert::Alert Evaluate(const alert::AlertContext& context) override;
    alert::AlertType GetType() const override;
    
private:
    ogc::alert::IAlertCheckerPtr m_checker;
};
```

---

## 接口

### IAlertEngine
```cpp
virtual Alert Evaluate(const AlertContext& context) = 0;
virtual AlertType GetType() const = 0;
virtual void SetThreshold(const ThresholdConfig& config) = 0;
virtual ThresholdConfig GetThreshold() const = 0;
virtual std::string GetName() const = 0;
```

### IAlertJudgeService
```cpp
virtual std::vector<Alert> Evaluate(const AlertContext& context) = 0;
virtual std::vector<Alert> EvaluateByType(const AlertContext& context, AlertType type) = 0;
virtual void EnableEngine(AlertType type, bool enable) = 0;
virtual void SetEngineThreshold(AlertType type, const ThresholdConfig& config) = 0;
```

### IAlertPushService
```cpp
virtual PushResult Push(const Alert& alert, PushChannel channel) = 0;
virtual std::vector<PushResult> PushToAllChannels(const Alert& alert) = 0;
virtual void SetConfig(const PushConfig& config) = 0;
```

### IAlertConfigService
```cpp
virtual ThresholdConfig GetThreshold(AlertType type) const = 0;
virtual void SetThreshold(AlertType type, const ThresholdConfig& config) = 0;
virtual AlertRule GetRule(const std::string& ruleId) const = 0;
virtual NotificationConfig GetNotificationConfig() const = 0;
```

### IWebSocketService
```cpp
virtual bool Start(const WebSocketConfig& config) = 0;
virtual void Stop() = 0;
virtual bool SendToClient(const std::string& clientId, const WebSocketMessage& message) = 0;
virtual void Broadcast(const WebSocketMessage& message) = 0;
```

---

## 类型定义

### AlertType (警报类型枚举)
```cpp
enum class AlertType {
    kDepthAlert = 0,
    kCollisionAlert = 1,
    kWeatherAlert = 2,
    kChannelAlert = 3,
    kOtherAlert = 4
};
```

### AlertLevel (警报等级枚举)
```cpp
enum class AlertLevel {
    kLevel1_Critical = 1,
    kLevel2_Severe = 2,
    kLevel3_Moderate = 3,
    kLevel4_Minor = 4,
    kNone = 0
};
```

### AlertStatus (警报状态枚举)
```cpp
enum class AlertStatus {
    kPending = 0,
    kActive = 1,
    kPushed = 2,
    kAcknowledged = 3,
    kCleared = 4,
    kExpired = 5
};
```

### PushMethod (推送方式枚举)
```cpp
enum class PushMethod {
    kApp = 0,
    kSms = 1,
    kEmail = 2,
    kSound = 3,
    kScreen = 4
};
```

---

## 使用示例

### 创建警报引擎并评估

```cpp
#include "alert/depth_alert_engine.h"
#include "alert/alert_context.h"
#include "alert/ship_info.h"

using namespace alert;

DepthAlertEngine engine;
ThresholdConfig config;
config.SetLevel1Threshold(0.3);
engine.SetThreshold(config);

ShipInfo shipInfo;
shipInfo.SetDraft(5.0);

AlertContext context(shipInfo, Coordinate(121.5, 31.2));
context.SetSpeed(12.0);

Alert alert = engine.Evaluate(context);
if (alert.IsActive()) {
    std::cout << "Alert: " << alert.GetTitle() << std::endl;
}
```

### 使用警报判断服务

```cpp
#include "alert/alert_judge_service.h"
#include "alert/alert_engine_factory.h"

using namespace alert;

AlertJudgeService judgeService;

judgeService.RegisterEngine(AlertType::kDepthAlert, new DepthAlertEngine());
judgeService.RegisterEngine(AlertType::kCollisionAlert, new CollisionAlertEngine());

AlertContext context = ...;
std::vector<Alert> alerts = judgeService.Evaluate(context);

for (const auto& alert : alerts) {
    if (alert.GetAlertLevel() == AlertLevel::kLevel1_Critical) {
        // 处理关键警报
    }
}
```

### 适配ogc_alert模块

```cpp
#include "alert/checker_adapter.h"
#include <ogc/alert/depth_alert_checker.h>

using namespace alarm;
using namespace ogc::alert;

auto checker = std::make_shared<DepthAlertChecker>();
CheckerAdapter adapter(checker);

alert::AlertContext context = ...;
alert::Alert alert = adapter.Evaluate(context);
```

---

## 与ogc_alert模块的关系

| 特性 | alarm模块 | ogc_alert模块 |
|------|-----------|---------------|
| 命名空间 | `alert` | `ogc::alert` |
| 定位 | 服务层、适配层 | 核心业务层 |
| 职责 | REST/WebSocket服务、配置管理 | 警报检查、计算、推送 |
| 依赖关系 | 依赖ogc_alert | 独立核心模块 |

**适配器模式**：alarm模块通过适配器模式适配ogc_alert模块的接口：
- `CheckerAdapter` 适配 `ogc::alert::IAlertChecker`
- `RestAdapter` 适配 `ogc::alert::IRestController`
- `RepositoryAdapter` 适配 `ogc::alert::IAlertRepository`
- `PushChannelAdapter` 适配 `ogc::alert::PushService`

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-08 | Team | Initial version |

---

**Generated**: 2026-04-08  
**Module Version**: v1.0  
**C++ Standard**: C++11
