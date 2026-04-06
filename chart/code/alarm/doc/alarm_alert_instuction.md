# Alert 与 Alarm 模块融合实施方案

> **版本**: v1.1  
> **日期**: 2026-04-06  
> **状态**: 部分完成（Phase 1 核心融合已完成）

---

## 一、文档概述

### 1.1 目的

本文档详细描述 `alert` 模块与 `alarm` 模块的融合实施方案，包括：
- 各组件的详细对比分析
- 融合建议与推荐方案
- 分阶段实施路线
- 风险控制与验收标准

### 1.2 融合原则

| 原则 | 说明 |
|------|------|
| 取长补短 | 从两模块中选取最优实现 |
| 向后兼容 | 保持现有 API 兼容性 |
| 渐进式融合 | 分阶段实施，降低风险 |
| 测试驱动 | 每个阶段都有完整的测试覆盖 |

### 1.3 目标架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           目标架构（修正版）                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                 【alarm 模块】服务层                              │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐    │   │
│  │  │ REST API     │ │ WebSocket    │ │ ExternalDataGateway  │    │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘    │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐    │   │
│  │  │AlertJudgeSvc │ │AlertConfigSvc│ │ AlertPushService     │    │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘    │   │
│  │  ┌──────────────────────────────────────────────────────────┐  │   │
│  │  │ DataAdapters: AISAdapter | WeatherAdapter | ChartAdapter │  │   │
│  │  └──────────────────────────────────────────────────────────┘  │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │ 依赖                                │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                 【alert 模块】核心层                              │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐    │   │
│  │  │ IAlertEngine │ │ IAlertChecker│ │ IScheduler           │    │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘    │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐    │   │
│  │  │AlertProcessor│ │ Deduplicator │ │ ConfigService        │    │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘    │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │ 使用                                │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │               【alert 模块】共享计算层（融合后）                   │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐    │   │
│  │  │ CpaCalculator│ │UKCCalculator │ │ SpatialDistance      │    │   │
│  │  │ (含地球曲率)  │ │ (含蹲底效应)  │ │                      │    │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘    │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │                                     │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │              【alert 模块】数据持久化层（融合后）                  │   │
│  │  ┌──────────────┐ ┌──────────────────┐ ┌────────────────────┐  │   │
│  │  │AlertRepository│ │ ConfigRepository │ │ AcknowledgeStore   │  │   │
│  │  └──────────────┘ └──────────────────┘ └────────────────────┘  │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 1.4 模块依赖关系

```
┌─────────────┐
│   alarm     │  服务层：对外接口、业务编排、数据适配
└──────┬──────┘
       │ 依赖
       ▼
┌─────────────┐
│   alert     │  核心层：预警引擎、检查器、调度器
│             │  计算层：CPA/UKC/空间距离计算
│             │  持久层：数据存储、配置管理
└──────┬──────┘
       │ 依赖
       ▼
┌─────────────┐
│  database   │  数据库连接、SQL执行
│  geom       │  几何计算、坐标转换
└─────────────┘
```

### 1.5 架构组件说明

| 层级 | 所属模块 | 组件 | 来源 | 职责 |
|------|----------|------|------|------|
| **服务层** | alarm | REST API | 融合 | 对外提供 RESTful 接口 |
| | alarm | WebSocket | 融合 | 实时推送预警消息 |
| | alarm | ExternalDataGateway | alarm | 接入外部数据源（AIS/气象/海图） |
| | alarm | AlertJudgeService | alarm | 预警判定服务，协调各引擎 |
| | alarm | AlertConfigService | alarm | 预警配置管理 |
| | alarm | AlertPushService | 融合 | 多渠道推送（App/短信/邮件） |
| | alarm | DataAdapters | alarm | 数据适配器（AIS/Weather/Chart） |
| **核心层** | alert | IAlertEngine | alert | 预警引擎接口 |
| | alert | IAlertChecker | alert | 预警检查器接口 |
| | alert | IScheduler | alert | 定时调度器 |
| | alert | AlertProcessor | alert | 预警处理器（去重/聚合） |
| | alert | Deduplicator | alert | 预警去重器 |
| | alert | ConfigService | alert | 配置服务 |
| **计算层** | alert | CpaCalculator | 融合 | CPA计算（含地球曲率修正） |
| | alert | UKCCalculator | 融合 | UKC计算（含蹲底效应） |
| | alert | SpatialDistance | alert | 空间距离计算 |
| **持久层** | alert | AlertRepository | 融合 | 预警数据存储 |
| | alert | ConfigRepository | alert | 配置数据存储 |
| | alert | AcknowledgeStore | alert | 确认记录存储 |

---

## 二、预警判定逻辑详细对比与融合方案

### 2.1 Depth（水深预警）

#### 2.1.1 实现差异对比

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 类名 | `DepthAlertChecker` | `DepthAlertEngine` |
| 设计模式 | Pimpl + IAlertChecker接口 | 简单继承 IAlertEngine |
| UKC计算 | `depth + tide - draft` | 考虑蹲底效应 squat |
| 返回类型 | `std::vector<AlertPtr>` 批量返回 | `Alert` 单个返回 |
| 数据源 | `SetDepthData/SetTideData` | `SetDepthProvider/SetTidalProvider` 回调 |

#### 2.1.2 核心计算对比

**alert 模块实现（简单）：**
```cpp
double CalculateUKC(double depth, double draft, double tide) const {
    return depth + tide - draft;  // 未考虑蹲底效应
}
```

**alarm 模块实现（专业）：**
```cpp
double CalculateSquat(double speed, double depth, double draft, double cb) {
    // 蹲底效应计算 - 基于Barrass公式
    double speedMs = speed * kKnotsToMs;
    double blockCoef = cb;  // 方形系数
    double squat = 2.4 * (speedMs * speedMs * blockCoef) / depth;
    return squat;
}

double CalculateUKC(double effectiveDepth, double draft, double squat) {
    return effectiveDepth - draft - squat;  // 考虑蹲底效应
}
```

#### 2.1.3 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 蹲底效应计算 | alarm | 航海专业要求，高速航行安全因素 |
| Pimpl 设计模式 | alert | 更好的封装性和二进制兼容性 |
| 批量返回 | alert | 支持一次检查生成多个预警 |
| 数据提供者回调 | alarm | 更灵活的数据源适配 |

#### 2.1.4 推荐融合方案

```cpp
class DepthAlertChecker : public IAlertChecker {
public:
    // 保留 alert 的接口设计
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    
    // 融合 alarm 的蹲底效应计算
    double CalculateSquat(double speed, double depth, double draft, double cb);
    double CalculateUKC(double effectiveDepth, double draft, double squat);
    
    // 保留 alarm 的数据提供者模式
    void SetDepthProvider(std::function<double(const Coordinate&)> provider);
    void SetTidalProvider(std::function<double(const Coordinate&)> provider);
};
```

---

### 2.2 Collision（碰撞预警）

#### 2.2.1 实现差异对比

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 类名 | `CollisionAlertChecker` | `CollisionAlertEngine` |
| 计算方法 | `CalculateCPA_TCPA()` 成员方法 | `CalculateCPA_TCPA()` 成员方法 |
| 计算精度 | 平面坐标近似 | 地球曲率修正 |
| 结果信息 | 基础（CPA、TCPA）+ `DetermineCollisionLevel()` | 基础（CPA、TCPA）+ `AssessRisk()` + 多目标处理 |
| 目标管理 | 单目标计算 | 多目标批量处理（`AISTarget`/`CollisionTarget`） |
| 风险排序 | 无 | 按风险级别排序（`GetRiskTargets()`） |

#### 2.2.2 核心计算对比

**alert 模块实现（平面坐标）：**
```cpp
// CollisionAlertChecker 的方法
void CalculateCPA_TCPA(const Coordinate& own_pos, double own_speed, double own_heading,
                       const Coordinate& target_pos, double target_speed, double target_heading,
                       double& cpa, double& tcpa) const;
                       
AlertLevel DetermineCollisionLevel(double cpa, double tcpa) const;
```

**alarm 模块实现（地球曲率修正）：**
```cpp
// CollisionAlertEngine 的方法
void CalculateCPA_TCPA(const Coordinate& myPos, double mySpeed, double myCourse,
                       const Coordinate& targetPos, double targetSpeed, double targetCourse,
                       double& cpa, double& tcpa);

AlertLevel AssessRisk(double cpa, double tcpa);

std::vector<CollisionTarget> GetRiskTargets(const AlertContext& context);

// 地球曲率修正 - 更精确的距离计算
// 内部实现：
double latAvg = (myPos.GetLatitude() + targetPos.GetLatitude()) / 2.0 * M_PI / 180.0;
double metersPerDegLon = kEarthRadiusMeters * std::cos(latAvg) * M_PI / 180.0;
double metersPerDegLat = kEarthRadiusMeters * M_PI / 180.0;
```

#### 2.2.3 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 地球曲率修正 | alarm | 长距离计算更精确 |
| 结果结构体 | alert | 信息丰富，便于决策 |
| 多目标处理 | alarm | 实际场景需要 |
| 风险排序 | alarm | 优先处理高风险目标 |
| 警告消息生成 | alert | 用户友好的提示 |

#### 2.2.4 推荐融合方案

```cpp
// 扩展 CPA 结果结构体（建议新增）
struct CPAResult {
    // 基础信息（两边都有）
    double cpa;
    double tcpa;
    bool is_dangerous;
    AlertLevel alert_level;
    
    // 扩展信息（建议新增）
    Coordinate cpa_position_own;
    Coordinate cpa_position_target;
    double relative_bearing;
    std::string warning_message;
    
    // 扩展信息（来自 alarm）
    double distance_at_cpa;
    double relative_speed;
};

// 融合后的 CollisionAlertChecker
class CollisionAlertChecker : public IAlertChecker {
public:
    // 保留 alert 的接口设计
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    
    // 融合 alarm 的地球曲率修正算法
    void CalculateCPA_TCPA(const Coordinate& own_pos, double own_speed, double own_heading,
                           const Coordinate& target_pos, double target_speed, double target_heading,
                           double& cpa, double& tcpa) const;
    
    // 新增：返回扩展结果的方法
    CPAResult CalculateCPAWithDetails(const Coordinate& own_pos, double own_speed, double own_heading,
                                       const Coordinate& target_pos, double target_speed, double target_heading) const;
    
    // 融合 alarm 的多目标处理
    void SetAISProvider(std::vector<AISTarget> (*provider)(const Coordinate&, double));
    std::vector<CollisionTarget> GetRiskTargets(const AlertContext& context);
    
private:
    // 地球曲率修正（来自 alarm）
    double CorrectForEarthCurvature(const Coordinate& pos1, const Coordinate& pos2);
};
```

---

### 2.3 Weather（气象预警）

#### 2.3.1 设计理念差异

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 设计理念 | 主动计算型 | 被动集成型 |
| 数据来源 | 船载传感器 | 外部气象服务 |
| 触发方式 | 参数超阈值 | 接收预警信息 |
| 适用场景 | 实时监测 | 气象部门预警转发 |

#### 2.3.2 实现对比

**alert 模块（主动计算）：**
```cpp
class WeatherAlertChecker : public IAlertChecker {
public:
    void SetWeatherData(std::shared_ptr<void> weather_data);
    
    // 基于参数计算预警级别
    AlertLevel DetermineWeatherLevel(double wind_speed, double wave_height, double visibility) const {
        if (wind_speed > 25.0 || wave_height > 4.0 || visibility < 0.5) 
            return AlertLevel::kLevel4;
        if (wind_speed > 17.0 || wave_height > 2.5 || visibility < 1.0) 
            return AlertLevel::kLevel3;
        // ...
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) override;
};
```

**alarm 模块（被动集成）：**
```cpp
struct WeatherAlertData {
    std::string alertId;
    std::string alertType;
    AlertLevel level;
    std::string issueTime;
    std::string expireTime;
    std::string content;
    std::string affectedArea;
    std::map<std::string, double> parameters;
};

class WeatherAlertEngine : public IAlertEngine {
public:
    // 从外部服务获取预警数据
    void SetWeatherProvider(std::vector<WeatherAlertData> (*provider)(const Coordinate&));
    
    // 判断是否在受影响区域
    bool IsInAffectedArea(const Coordinate& position, const std::string& area);
    
    Alert Evaluate(const AlertContext& context) override {
        std::vector<WeatherAlertData> alerts = FetchWeatherAlerts(context.GetPosition());
        for (const auto& alertData : alerts) {
            if (IsInAffectedArea(context.GetPosition(), alertData.affectedArea)) {
                return ConvertToAlert(alertData);
            }
        }
        return Alert();
    }
};
```

#### 2.3.3 融合建议

**结论：两者都需要保留，分别用于不同场景**

| 场景 | 推荐使用 | 原因 |
|------|----------|------|
| 船载传感器实时监测 | alert | 需要主动计算预警级别 |
| 气象部门预警转发 | alarm | 需要接收和转换外部数据 |
| 综合气象预警 | 融合 | 结合两种数据源 |

#### 2.3.4 推荐融合方案

```cpp
class WeatherAlertService {
public:
    // 主动监测（来自 alert）
    void SetSensorData(double wind_speed, double wave_height, double visibility);
    AlertLevel CalculateFromSensors();
    
    // 被动接收（来自 alarm）
    void SetExternalProvider(std::vector<WeatherAlertData> (*provider)(const Coordinate&));
    std::vector<Alert> FetchExternalAlerts(const Coordinate& position);
    
    // 综合预警
    std::vector<Alert> GetAllWeatherAlerts(const Coordinate& position);
};
```

---

### 2.4 Channel（航道预警）

#### 2.4.1 数据结构对比

**alert 模块（简洁）：**
```cpp
class ChannelAlertChecker : public IAlertChecker {
public:
    void SetNoticeData(std::shared_ptr<void> notice_data);
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    // 接口简洁，实现细节隐藏在 Pimpl 中
};
```

**alarm 模块（完善）：**
```cpp
struct ChannelStatus {
    std::string channelId;
    std::string channelName;
    bool isNavigable;
    double minDepth;
    double maxWidth;
    std::string status;
    std::string restriction;
};

struct NavAidStatus {
    std::string navAidId;
    std::string navAidName;
    std::string type;
    bool isOperational;
    std::string status;
    Coordinate position;
};

struct ChannelData {
    std::vector<ChannelStatus> channels;
    std::vector<NavAidStatus> navAids;
    std::vector<std::string> notices;
};

class ChannelAlertEngine : public IAlertEngine {
    // 完善的数据结构和方法
};
```

#### 2.4.2 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 数据结构 | alarm | 更完善，覆盖航道和助航设备 |
| 接口设计 | alert | Pimpl 模式更优雅 |
| 批量返回 | alert | 支持多个航道预警 |

#### 2.4.3 推荐融合方案

```cpp
// 统一数据结构（来自 alarm）
struct ChannelStatus { ... };
struct NavAidStatus { ... };
struct ChannelData { ... };

// 融合接口设计
class ChannelAlertChecker : public IAlertChecker {
public:
    // 保留 alert 的接口风格
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    
    // 使用 alarm 的数据结构
    void SetChannelData(std::shared_ptr<ChannelData> data);
    
    // 新增方法
    std::vector<AlertPtr> CheckChannelStatus(const ChannelStatus& channel);
    std::vector<AlertPtr> CheckNavAidStatus(const NavAidStatus& navAid);
};
```

---

### 2.5 预警判定逻辑融合总结

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    融合后的预警引擎（位于 alert 模块）                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │          【alert 模块】统一接口层                                 │   │
│  │  IAlertChecker + Pimpl + 批量返回                                │   │
│  │  ├── DepthAlertChecker                                           │   │
│  │  ├── CollisionAlertChecker                                       │   │
│  │  ├── WeatherAlertChecker                                         │   │
│  │  └── ChannelAlertChecker                                         │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │ 调用                                │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │          【alert 模块】核心计算层（融合两模块算法）                 │   │
│  │  ┌───────────────┐ ┌───────────────┐ ┌───────────────────┐      │   │
│  │  │ UKCCalculator │ │ CpaCalculator │ │ WeatherCalculator │      │   │
│  │  │ (alert 接口   │ │ (alert 接口   │ │ (alert 主动计算   │      │   │
│  │  │ + alarm 蹲底) │ │ + alarm 曲率) │ │ + alarm 外部集成) │      │   │
│  │  └───────────────┘ └───────────────┘ └───────────────────┘      │   │
│  │  ┌───────────────────────────────────────────────────────────┐  │   │
│  │  │ ChannelCalculator (alert 接口 + alarm 数据结构)            │  │   │
│  │  └───────────────────────────────────────────────────────────┘  │   │
│  └────────────────────────────────┬────────────────────────────────┘   │
│                                   │ 使用                                │
│                                   ▼                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │          【alert 模块】数据结构层（融合两模块）                     │   │
│  │  来自 alert: AlertPtr, AlertQueryParams, AlertThreshold         │   │
│  │  来自 alarm: WeatherAlertData, ChannelStatus, NavAidStatus      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 2.5.1 融合要点说明

| 组件 | 接口来源 | 算法来源 | 数据结构来源 |
|------|----------|----------|--------------|
| DepthAlertChecker | alert | alarm（蹲底效应） | alert |
| CollisionAlertChecker | alert | alarm（地球曲率） | alert |
| WeatherAlertChecker | alert | alert（主动计算）+ alarm（外部集成） | 两边融合 |
| ChannelAlertChecker | alert | alert | alarm（ChannelStatus等） |

---

## 三、推送服务详细对比与融合方案

### 3.1 接口设计对比

#### 3.1.1 alert 模块推送服务

```cpp
// 推送渠道接口
class IPushChannel {
public:
    virtual PushMethod GetMethod() const = 0;
    virtual bool IsAvailable() const = 0;
    virtual PushResult Send(const AlertPtr& alert, const std::string& user_id) = 0;
};

// 推送服务接口
class IPushService {
public:
    // 渠道管理
    virtual void RegisterChannel(IPushChannelPtr channel) = 0;
    virtual void UnregisterChannel(PushMethod method) = 0;
    virtual IPushChannelPtr GetChannel(PushMethod method) const = 0;
    
    // 推送策略（按预警级别配置推送方式）
    virtual void SetPushStrategy(
        const std::vector<PushMethod>& level1_methods,
        const std::vector<PushMethod>& level2_methods,
        const std::vector<PushMethod>& level3_methods,
        const std::vector<PushMethod>& level4_methods) = 0;
    
    // 推送操作
    virtual std::vector<PushResult> Push(const AlertPtr& alert, 
                                          const std::vector<std::string>& user_ids) = 0;
    virtual std::vector<PushResult> PushByMethod(const AlertPtr& alert, 
                                                  const std::vector<std::string>& user_ids,
                                                  PushMethod method) = 0;
    
    // 重试机制
    virtual void SetRetryCount(int count) = 0;
    virtual void SetRetryInterval(int interval_ms) = 0;
    
    // 历史记录
    virtual std::vector<PushRecord> GetPushHistory(const std::string& alert_id) = 0;
};
```

#### 3.1.2 alarm 模块推送服务

```cpp
// 推送渠道枚举
enum class PushChannel {
    kWebSocket,
    kSms,
    kEmail,
    kHttp
};

// 推送配置结构体
struct PushConfig {
    bool enableWebSocket;
    bool enableSms;
    bool enableEmail;
    bool enableHttp;
    
    std::string smsGateway;
    std::string smsApiKey;
    
    std::string smtpServer;
    int smtpPort;
    std::string smtpUser;
    std::string smtpPassword;
    
    std::string httpEndpoint;
    std::string httpApiKey;
    
    int maxRetries;
    int retryIntervalMs;
};

// 推送服务接口
class IAlertPushService {
public:
    virtual PushResult Push(const Alert& alert, PushChannel channel) = 0;
    virtual std::vector<PushResult> PushToAllChannels(const Alert& alert) = 0;
    virtual std::vector<PushResult> PushToChannels(const Alert& alert, 
                                                    const std::vector<PushChannel>& channels) = 0;
    
    virtual void SetConfig(const PushConfig& config) = 0;
    virtual PushConfig GetConfig() const = 0;
    
    virtual void EnableChannel(PushChannel channel, bool enable) = 0;
    virtual bool IsChannelEnabled(PushChannel channel) const = 0;
    
    virtual void SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) = 0;
};
```

### 3.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 渠道抽象 | IPushChannel 接口 | 枚举 + 配置 | alert（更灵活） |
| 渠道管理 | 动态注册/注销 | 配置开关 | alert（可扩展） |
| 推送策略 | 按预警级别配置 | 全渠道推送 | alert（更智能） |
| 配置方式 | 运行时配置 | 结构体配置 | alarm（更直观） |
| 回调机制 | 无 | 有 | alarm（便于监控） |
| 历史记录 | 有 | 无 | alert（便于追踪） |
| 重试机制 | 接口支持 | 配置支持 | 融合 |

### 3.3 推荐融合方案

```cpp
// 统一推送渠道接口（来自 alert）
class IPushChannel {
public:
    virtual PushMethod GetMethod() const = 0;
    virtual bool IsAvailable() const = 0;
    virtual PushResult Send(const AlertPtr& alert, const std::string& user_id) = 0;
};

// 统一推送配置（来自 alarm）
struct PushConfig {
    bool enableWebSocket;
    bool enableSms;
    bool enableEmail;
    bool enableHttp;
    
    // SMS 配置
    std::string smsGateway;
    std::string smsApiKey;
    
    // Email 配置
    std::string smtpServer;
    int smtpPort;
    std::string smtpUser;
    std::string smtpPassword;
    
    // HTTP 配置
    std::string httpEndpoint;
    std::string httpApiKey;
    
    // 重试配置
    int maxRetries;
    int retryIntervalMs;
};

// 融合推送服务接口
class IPushService {
public:
    // 渠道管理（来自 alert）
    virtual void RegisterChannel(IPushChannelPtr channel) = 0;
    virtual void UnregisterChannel(PushMethod method) = 0;
    
    // 配置管理（来自 alarm）
    virtual void SetConfig(const PushConfig& config) = 0;
    virtual PushConfig GetConfig() const = 0;
    
    // 推送策略（来自 alert）
    virtual void SetPushStrategy(
        const std::vector<PushMethod>& level1_methods,
        const std::vector<PushMethod>& level2_methods,
        const std::vector<PushMethod>& level3_methods,
        const std::vector<PushMethod>& level4_methods) = 0;
    
    // 推送操作（融合）
    virtual std::vector<PushResult> Push(const AlertPtr& alert, 
                                          const std::vector<std::string>& user_ids) = 0;
    virtual std::vector<PushResult> PushToChannels(const AlertPtr& alert,
                                                    const std::vector<PushMethod>& channels) = 0;
    
    // 回调机制（来自 alarm）
    virtual void SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) = 0;
    
    // 历史记录（来自 alert）
    virtual std::vector<PushRecord> GetPushHistory(const std::string& alert_id) = 0;
};
```

---

## 四、REST API 详细对比与融合方案

### 4.1 接口设计对比

#### 4.1.1 alert 模块 REST 控制器

```cpp
// HTTP 请求/响应结构
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
    std::string content_type;
};

struct HttpResponse {
    int status_code;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string content_type;
    
    static HttpResponse Ok(const std::string& body);
    static HttpResponse Created(const std::string& body);
    static HttpResponse BadRequest(const std::string& message);
    static HttpResponse NotFound(const std::string& message);
    static HttpResponse InternalError(const std::string& message);
};

// REST 控制器接口
class IRestController {
public:
    virtual void RegisterRoute(const std::string& method, const std::string& path, 
                               RouteHandler handler) = 0;
    virtual HttpResponse HandleRequest(const HttpRequest& request) = 0;
    virtual std::vector<Route> GetRoutes() const = 0;
};
```

#### 4.1.2 alarm 模块 REST 控制器

```cpp
// HTTP 请求/响应结构
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string contentType;
    std::string authorization;
    bool valid;
};

struct HttpResponse {
    int statusCode;
    std::string statusMessage;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string contentType;
    bool valid;
};

// API 错误结构
struct ApiError {
    int code;
    std::string message;
    std::string details;
};

// 业务请求结构
struct SubscribeRequest {
    std::string userId;
    std::string shipId;
    std::vector<std::string> alertTypes;
    std::vector<std::string> pushMethods;
    bool valid;
};

struct AlertListRequest {
    std::string userId;
    std::string status;
    std::string level;
    std::string type;
    std::string startTime;
    std::string endTime;
    int page;
    int pageSize;
    bool valid;
};
```

### 4.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 路由注册 | 动态注册 | 固定路由 | alert（更灵活） |
| 响应构造 | 静态工厂方法 | 手动构造 | alert（更简洁） |
| 错误处理 | 简单消息 | 结构化错误 | alarm（更详细） |
| 认证支持 | 无 | authorization 字段 | alarm |
| 分页支持 | 无 | AlertListRequest | alarm |
| 业务请求 | 无 | 结构化请求体 | alarm |

### 4.3 推荐融合方案

```cpp
// 统一 HTTP 结构（融合两模块）
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string contentType;
    std::string authorization;  // 来自 alarm
    bool valid;                 // 来自 alarm
};

struct HttpResponse {
    int statusCode;
    std::string statusMessage;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string contentType;
    
    // 来自 alert 的工厂方法
    static HttpResponse Ok(const std::string& body);
    static HttpResponse BadRequest(const std::string& message);
    static HttpResponse Json(int status, const std::string& json_body);
    
    // 来自 alarm 的错误结构
    static HttpResponse Error(const ApiError& error);
};

// 统一 REST 控制器接口
class IRestController {
public:
    // 来自 alert 的动态路由
    virtual void RegisterRoute(const std::string& method, const std::string& path, 
                               RouteHandler handler) = 0;
    virtual HttpResponse HandleRequest(const HttpRequest& request) = 0;
    
    // 来自 alarm 的认证中间件
    virtual void SetAuthMiddleware(std::function<bool(const HttpRequest&)> middleware) = 0;
};
```

---

## 五、WebSocket 服务详细对比与融合方案

### 5.1 接口设计对比

#### 5.1.1 alert 模块 WebSocket 服务

```cpp
struct WebSocketConfig {
    std::string host;
    int port;
    int max_connections;
    int ping_interval_ms;
    int ping_timeout_ms;
    bool enable_ssl;
    std::string cert_file;
    std::string key_file;
};

struct WebSocketSession {
    std::string session_id;
    std::string user_id;
    std::string remote_address;
    DateTime connected_at;
    bool authenticated;
};

class IWebSocketService {
public:
    // 生命周期管理
    virtual bool Start(const WebSocketConfig& config) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    // 回调设置
    virtual void SetMessageHandler(WebSocketMessageHandler handler) = 0;
    virtual void SetConnectHandler(WebSocketConnectHandler handler) = 0;
    virtual void SetDisconnectHandler(WebSocketDisconnectHandler handler) = 0;
    
    // 消息发送
    virtual bool SendToSession(const std::string& session_id, const std::string& message) = 0;
    virtual bool SendToUser(const std::string& user_id, const std::string& message) = 0;
    virtual void Broadcast(const std::string& message) = 0;
    virtual void BroadcastExcept(const std::string& message, 
                                  const std::vector<std::string>& exclude_sessions) = 0;
    
    // 会话管理
    virtual std::vector<WebSocketSession> GetActiveSessions() = 0;
    virtual WebSocketSession GetSession(const std::string& session_id) = 0;
    virtual int GetSessionCount() = 0;
    virtual bool CloseSession(const std::string& session_id) = 0;
    
    // 认证
    virtual bool AuthenticateSession(const std::string& session_id, 
                                      const std::string& user_id, 
                                      const std::string& token) = 0;
    virtual bool IsSessionAuthenticated(const std::string& session_id) = 0;
    
    // 预警订阅
    virtual void SubscribeToAlert(const std::string& session_id, AlertType alert_type) = 0;
    virtual void UnsubscribeFromAlert(const std::string& session_id, AlertType alert_type) = 0;
    virtual std::vector<AlertType> GetSubscriptions(const std::string& session_id) = 0;
};
```

#### 5.1.2 alarm 模块 WebSocket 服务

```cpp
struct WebSocketConfig {
    int port;
    int maxConnections;
    int pingInterval;
    int pongTimeout;
    int maxMessageSize;
    bool enableSSL;
    std::string certPath;
    std::string keyPath;
};

struct WebSocketClient {
    std::string clientId;
    std::string userId;
    std::string connectionId;
    std::vector<std::string> subscribedTypes;
    std::string connectTime;
    std::string lastActiveTime;
    bool authenticated;
};

class IWebSocketService {
public:
    // 推送服务集成
    virtual void SetAlertPushService(std::shared_ptr<IAlertPushService> service) = 0;
    
    // 生命周期管理
    virtual bool Start(const WebSocketConfig& config) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    // 消息发送
    virtual bool SendToClient(const std::string& clientId, const WebSocketMessage& message) = 0;
    virtual bool SendToUser(const std::string& userId, const WebSocketMessage& message) = 0;
    virtual void Broadcast(const WebSocketMessage& message) = 0;
    virtual void BroadcastToType(const std::string& alertType, const WebSocketMessage& message) = 0;
    
    // 回调设置
    virtual void SetMessageCallback(MessageCallback callback) = 0;
    virtual void SetConnectionCallback(ConnectionCallback callback) = 0;
};
```

### 5.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 配置完整性 | 有 host 字段 | 无 host 字段 | alert |
| 会话信息 | 有 remote_address | 有 lastActiveTime | 融合 |
| 消息发送 | BroadcastExcept | BroadcastToType | 融合 |
| 认证机制 | 完整 | 基础 | alert |
| 预警订阅 | AlertType 枚举 | 字符串类型 | alert |
| 推送服务集成 | 无 | 有 | alarm |
| 消息大小限制 | 无 | 有 | alarm |

### 5.3 推荐融合方案

```cpp
// 统一配置结构
struct WebSocketConfig {
    std::string host;           // 来自 alert
    int port;
    int maxConnections;
    int maxMessageSize;         // 来自 alarm
    int pingInterval;
    int pongTimeout;            // 来自 alarm
    bool enableSSL;
    std::string certPath;
    std::string keyPath;
};

// 统一会话结构
struct WebSocketSession {
    std::string sessionId;
    std::string userId;
    std::string connectionId;
    std::string remoteAddress;  // 来自 alert
    std::string connectTime;
    std::string lastActiveTime; // 来自 alarm
    bool authenticated;
};

// 融合 WebSocket 服务接口
class IWebSocketService {
public:
    // 生命周期管理
    virtual bool Start(const WebSocketConfig& config) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    // 回调设置（融合）
    virtual void SetMessageHandler(WebSocketMessageHandler handler) = 0;
    virtual void SetConnectHandler(WebSocketConnectHandler handler) = 0;
    virtual void SetDisconnectHandler(WebSocketDisconnectHandler handler) = 0;
    
    // 消息发送（融合）
    virtual bool SendToSession(const std::string& sessionId, const WebSocketMessage& message) = 0;
    virtual bool SendToUser(const std::string& userId, const WebSocketMessage& message) = 0;
    virtual void Broadcast(const WebSocketMessage& message) = 0;
    virtual void BroadcastExcept(const WebSocketMessage& message, 
                                  const std::vector<std::string>& excludeSessions) = 0;
    virtual void BroadcastToType(AlertType alertType, const WebSocketMessage& message) = 0;
    
    // 会话管理（来自 alert）
    virtual std::vector<WebSocketSession> GetActiveSessions() = 0;
    virtual bool CloseSession(const std::string& sessionId) = 0;
    
    // 认证（来自 alert）
    virtual bool AuthenticateSession(const std::string& sessionId, 
                                      const std::string& userId, 
                                      const std::string& token) = 0;
    
    // 预警订阅（来自 alert）
    virtual void SubscribeToAlert(const std::string& sessionId, AlertType alertType) = 0;
    virtual void UnsubscribeFromAlert(const std::string& sessionId, AlertType alertType) = 0;
    
    // 推送服务集成（来自 alarm）
    virtual void SetAlertPushService(std::shared_ptr<IPushService> service) = 0;
};
```

---

## 六、Repository 详细对比与融合方案

### 6.1 接口设计对比

#### 6.1.1 alert 模块 Repository

```cpp
class IAlertRepository {
public:
    // CRUD 操作
    virtual bool Save(const AlertPtr& alert) = 0;
    virtual bool Update(const AlertPtr& alert) = 0;
    virtual bool Delete(const std::string& alert_id) = 0;
    virtual AlertPtr FindById(const std::string& alert_id) = 0;
    
    // 查询操作
    virtual std::vector<AlertPtr> Query(const AlertQueryParams& params) = 0;
    virtual int Count(const AlertQueryParams& params) = 0;
    
    // 便捷查询
    virtual std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) = 0;
    virtual std::vector<AlertPtr> GetAlertsByType(AlertType type, const std::string& user_id) = 0;
    virtual std::vector<AlertPtr> GetAlertsByLevel(AlertLevel level, const std::string& user_id) = 0;
    
    // 状态更新
    virtual bool UpdateStatus(const std::string& alert_id, AlertStatus status) = 0;
    virtual bool Acknowledge(const std::string& alert_id, const std::string& user_id, 
                             const std::string& action) = 0;
    
    // 扩展数据
    virtual bool SaveAcknowledge(const AcknowledgeData& data) = 0;
    virtual bool SaveFeedback(const FeedbackData& data) = 0;
};

// 配置仓储
class IConfigRepository {
public:
    virtual bool SaveConfig(const AlertConfig& config) = 0;
    virtual AlertConfig LoadConfig(const std::string& user_id) = 0;
    virtual bool DeleteConfig(const std::string& user_id) = 0;
    
    virtual bool UpdateThreshold(const std::string& user_id, AlertType type, 
                                  const AlertThreshold& threshold) = 0;
    virtual AlertThreshold GetThreshold(const std::string& user_id, AlertType type) = 0;
    
    virtual bool SetPushMethods(const std::string& user_id, 
                                 const std::vector<PushMethod>& methods) = 0;
    virtual std::vector<PushMethod> GetPushMethods(const std::string& user_id) = 0;
};
```

#### 6.1.2 alarm 模块 Repository

```cpp
class IAlertRepository {
public:
    // CRUD 操作
    virtual bool Create(const Alert& alert) = 0;
    virtual bool Update(const Alert& alert) = 0;
    virtual bool Remove(const std::string& alertId) = 0;
    virtual Alert GetById(const std::string& alertId) = 0;
    
    // 查询操作
    virtual std::vector<Alert> FindAll(int limit = 100, int offset = 0) = 0;
    virtual std::vector<Alert> FindByShipId(const std::string& shipId) = 0;
    virtual std::vector<Alert> FindByType(AlertType type) = 0;
    virtual std::vector<Alert> FindByLevel(AlertLevel level) = 0;
    virtual std::vector<Alert> FindByStatus(AlertStatus status) = 0;
    virtual std::vector<Alert> FindActive() = 0;
    
    // 状态更新
    virtual bool Acknowledge(const std::string& alertId, const std::string& operatorId) = 0;
    virtual bool Expire(const std::string& alertId) = 0;
    virtual bool Dismiss(const std::string& alertId, const std::string& reason) = 0;
    
    // 统计
    virtual AlertStatistics GetStatistics(const std::string& startTime, 
                                           const std::string& endTime) = 0;
    virtual int GetActiveCount() = 0;
};
```

### 6.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 数据类型 | AlertPtr 智能指针 | Alert 值类型 | alert（更安全） |
| 查询参数 | AlertQueryParams 结构 | 分散参数 | alert（更灵活） |
| 分页支持 | 在 QueryParams 中 | FindAll 参数 | alert |
| 船舶查询 | 无 | FindByShipId | alarm |
| 统计功能 | 无 | GetStatistics | alarm |
| 配置仓储 | 有 IConfigRepository | 无 | alert |
| 确认数据 | SaveAcknowledge | 仅 Acknowledge | alert |
| 反馈数据 | SaveFeedback | 无 | alert |

### 6.3 推荐融合方案

```cpp
// 统一查询参数
struct AlertQueryParams {
    std::string user_id;
    std::string ship_id;        // 来自 alarm
    AlertType type;
    AlertLevel level;
    AlertStatus status;
    std::string start_time;
    std::string end_time;
    int page;
    int page_size;
};

// 统一 Repository 接口
class IAlertRepository {
public:
    // CRUD 操作（使用 alert 的智能指针风格）
    virtual bool Save(const AlertPtr& alert) = 0;
    virtual bool Update(const AlertPtr& alert) = 0;
    virtual bool Delete(const std::string& alert_id) = 0;
    virtual AlertPtr FindById(const std::string& alert_id) = 0;
    
    // 查询操作（融合）
    virtual std::vector<AlertPtr> Query(const AlertQueryParams& params) = 0;
    virtual int Count(const AlertQueryParams& params) = 0;
    
    // 便捷查询（融合）
    virtual std::vector<AlertPtr> FindActive() = 0;
    virtual std::vector<AlertPtr> FindByShipId(const std::string& ship_id) = 0;
    virtual std::vector<AlertPtr> FindByType(AlertType type) = 0;
    virtual std::vector<AlertPtr> FindByLevel(AlertLevel level) = 0;
    virtual std::vector<AlertPtr> FindByStatus(AlertStatus status) = 0;
    
    // 状态更新（融合）
    virtual bool UpdateStatus(const std::string& alert_id, AlertStatus status) = 0;
    virtual bool Acknowledge(const std::string& alert_id, const std::string& user_id, 
                             const std::string& action) = 0;
    virtual bool Expire(const std::string& alert_id) = 0;
    virtual bool Dismiss(const std::string& alert_id, const std::string& reason) = 0;
    
    // 统计（来自 alarm）
    virtual AlertStatistics GetStatistics(const std::string& start_time, 
                                           const std::string& end_time) = 0;
    virtual int GetActiveCount() = 0;
    
    // 扩展数据（来自 alert）
    virtual bool SaveAcknowledge(const AcknowledgeData& data) = 0;
    virtual bool SaveFeedback(const FeedbackData& data) = 0;
};

// 配置仓储（保留 alert 的设计）
class IConfigRepository {
    virtual bool SaveConfig(const AlertConfig& config) = 0;
    virtual AlertConfig LoadConfig(const std::string& user_id) = 0;
    virtual bool UpdateThreshold(const std::string& user_id, AlertType type, 
                                  const AlertThreshold& threshold) = 0;
    virtual bool SetPushMethods(const std::string& user_id, 
                                 const std::vector<PushMethod>& methods) = 0;
};
```

---

## 七、融合实施优先级

### 7.1 优先级矩阵

| 组件 | 重合度 | 融合价值 | 实施难度 | 优先级 |
|------|--------|----------|----------|--------|
| 预警判定逻辑 | 高 | 高 | 中 | **P0** |
| 数据结构（枚举） | 高 | 高 | 低 | **P0** |
| 推送服务 | 中 | 高 | 中 | **P1** |
| REST API | 中 | 中 | 低 | **P2** |
| WebSocket | 中 | 中 | 中 | **P2** |
| Repository | 中 | 中 | 低 | **P3** |

### 7.2 实施顺序

```
Phase 1 (P0): 核心融合 - 数据结构与计算器
├── 1.1 统一 AlertType/AlertLevel/AlertStatus 枚举
├── 1.2 统一 Alert 数据类
├── 1.3 融合 UKCCalculator（添加蹲底效应）
└── 1.4 融合 CPACalculator（添加地球曲率修正）

Phase 2 (P1): 服务融合 - 检查器与推送
├── 2.1 整合检查器体系（IAlertChecker 适配器）
├── 2.2 融合推送服务接口
├── 2.3 融合推送配置结构
└── 2.4 统一推送策略

Phase 3 (P2): API融合 - 接口统一
├── 3.1 统一 REST 控制器接口
├── 3.2 统一 WebSocket 服务接口
└── 3.3 统一 HTTP 请求/响应结构

Phase 4 (P3): 数据融合 - 持久化
├── 4.1 统一 Repository 接口
├── 4.2 统一查询参数结构
└── 4.3 添加统计功能
```

---

## 八、详细实施路线

### 8.1 阶段一：核心融合（优先级：P0）

> 对应 Phase 1：数据结构与计算器融合

#### 8.1.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 | 状态 |
|--------|----------|----------|------------|------|
| P1-001 | 定义统一的 AlertType 枚举 | `alert/types.h`, `alarm/alert_types.h` | 2h | ✅ 完成 |
| P1-002 | 定义统一的 AlertLevel 枚举 | `alert/types.h`, `alarm/alert_types.h` | 1h | ✅ 完成 |
| P1-003 | 定义统一的 AlertStatus 枚举 | `alert/types.h`, `alarm/alert_types.h` | 1h | ✅ 完成 |
| P1-004 | 统一 Alert 数据类 | `alert/types.h`, `alarm/alert.h` | 4h | ✅ 完成 |
| P1-005 | 创建类型转换适配器 | 新建 `alarm/alert_type_adapter.h` | 2h | ✅ 完成 |
| P1-006 | 融合 UKCCalculator（添加蹲底效应） | `alert/ukc_calculator.cpp` | 4h | ✅ 完成 |
| P1-007 | 融合 CpaCalculator（添加地球曲率） | `alert/cpa_calculator.cpp` | 4h | ✅ 完成 |
| P1-008 | 更新单元测试 | `alert/tests/*`, `alarm/tests/*` | 4h | ✅ 完成 |

#### 8.1.2 统一枚举设计

```cpp
// 统一的 AlertType 枚举（合并两模块）
enum class AlertType : uint8_t {
    kUnknown = 0,
    kDepth = 1,           // 水深预警
    kCollision = 2,       // 碰撞预警
    kWeather = 3,         // 气象预警
    kChannel = 4,         // 航道预警
    kDeviation = 5,       // 偏航预警 (alert独有)
    kSpeed = 6,           // 超速预警 (alert独有)
    kRestrictedArea = 7   // 禁航区预警 (alert独有)
};

// 统一的 AlertLevel 枚举
enum class AlertLevel : uint8_t {
    kNone = 0,
    kLevel1 = 1, // 一级-危急
    kLevel2 = 2, // 二级-严重
    kLevel3 = 3, // 三级-中等
    kLevel4 = 4  // 四级-轻微
};

// 统一的 AlertStatus 枚举（合并两模块状态）
enum class AlertStatus : uint8_t {
    kPending = 0,       // 待处理
    kActive = 1,        // 活跃
    kPushed = 2,        // 已推送
    kAcknowledged = 3,  // 已确认
    kCleared = 4,       // 已清除
    kExpired = 5,       // 已过期
    kDismissed = 6      // 已忽略
};
```

#### 8.1.3 验收标准

- [x] 两模块使用相同的枚举定义
- [x] 所有现有测试通过
- [x] 无编译警告
- [x] 类型转换适配器覆盖所有旧值

---

### 8.2 阶段二：服务融合（优先级：P1）

> 对应 Phase 2：检查器与推送服务融合

#### 8.2.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 | 状态 |
|--------|----------|----------|------------|------|
| P2-001 | alarm 添加 alert 模块依赖 | `alarm/CMakeLists.txt` | 0.5h | ✅ 完成 |
| P2-002 | 创建 IAlertChecker 适配器 | 新建 `alarm/checker_adapter.h` | 3h | ✅ 完成 |
| P2-003 | 重构 AlertEngineFactory | `alarm/alert_engine_factory.cpp` | 4h | ✅ 完成 |
| P2-004 | 融合推送服务接口 | `alert/push_service.h` | 3h | ✅ 完成 |
| P2-005 | 实现推送渠道适配器 | 新建 `alarm/push_channel_adapter.h` | 4h | ✅ 完成 |
| P2-006 | 统一推送策略 | `alert/push_strategy.h` | 3h | ✅ 完成 |
| P2-007 | 添加集成测试 | 新建 `alarm/tests/integration_test.cpp` | 3h | ✅ 完成 |

#### 8.2.2 检查器适配器设计

```cpp
// alarm/checker_adapter.h
#pragma once

#include "ogc/alert/alert_checker.h"
#include "i_alert_engine.h"
#include <memory>

namespace alert {

// 将 alert 的 IAlertChecker 适配为 alarm 的 IAlertEngine
class CheckerAdapter : public IAlertEngine {
public:
    explicit CheckerAdapter(ogc::alert::IAlertCheckerPtr checker);
    ~CheckerAdapter() override;
    
    Alert Evaluate(const AlertContext& context) override;
    AlertType GetType() const override;
    void SetThreshold(const ThresholdConfig& config) override;
    ThresholdConfig GetThreshold() const override;
    std::string GetName() const override;
    
private:
    ogc::alert::IAlertCheckerPtr m_checker;
};

// 检查器注册表
class CheckerRegistry {
public:
    static CheckerRegistry& Instance();
    
    void RegisterChecker(AlertType type, ogc::alert::IAlertCheckerPtr checker);
    void UnregisterChecker(AlertType type);
    ogc::alert::IAlertCheckerPtr GetChecker(AlertType type) const;
    std::unique_ptr<IAlertEngine> CreateEngine(AlertType type);
    
private:
    std::map<AlertType, ogc::alert::IAlertCheckerPtr> m_checkers;
};

}
```

#### 8.2.3 推送服务融合设计

```cpp
// 融合后的推送服务接口（位于 alert 模块）
class IPushService {
public:
    // 渠道管理（来自 alert）
    virtual void RegisterChannel(IPushChannelPtr channel) = 0;
    virtual void UnregisterChannel(PushMethod method) = 0;
    
    // 配置管理（来自 alarm）
    virtual void SetConfig(const PushConfig& config) = 0;
    virtual PushConfig GetConfig() const = 0;
    
    // 推送策略（来自 alert）
    virtual void SetPushStrategy(
        const std::vector<PushMethod>& level1_methods,
        const std::vector<PushMethod>& level2_methods,
        const std::vector<PushMethod>& level3_methods,
        const std::vector<PushMethod>& level4_methods) = 0;
    
    // 推送操作（融合）
    virtual std::vector<PushResult> Push(const AlertPtr& alert, 
                                          const std::vector<std::string>& user_ids) = 0;
    
    // 回调机制（来自 alarm）
    virtual void SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) = 0;
    
    // 历史记录（来自 alert）
    virtual std::vector<PushRecord> GetPushHistory(const std::string& alert_id) = 0;
};
```

#### 8.2.4 验收标准

- [x] alarm 可使用 alert 的 IAlertChecker（通过 CheckerAdapter）
- [x] AlertEngineFactory 支持创建适配后的引擎
- [x] 两模块使用统一的推送接口
- [x] 推送成功率 > 99%
- [x] 计算结果与原有实现一致
- [x] 支持所有推送渠道（App/短信/邮件/声音/WebSocket）
- [x] 推送延迟 < 3s
- [x] 性能不低于原有实现
- [x] 所有测试通过

---

### 8.3 阶段三：API融合（优先级：P2）

> 对应 Phase 3：接口统一

#### 8.3.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 | 状态 |
|--------|----------|----------|------------|------|
| P3-001 | 设计统一的 REST API 规范 | 新建 `doc/api_spec.md` | 3h | ✅ 完成 |
| P3-002 | 统一 HTTP 请求/响应结构 | `alert/rest_controller.h` | 2h | ✅ 完成 |
| P3-003 | 统一 WebSocket 消息格式 | `alert/websocket_service.h` | 2h | ✅ 完成 |
| P3-004 | 实现路由适配器 | `alarm/rest_adapter.cpp` | 4h | ✅ 完成 |
| P3-005 | API 文档生成 | 使用 OpenAPI 规范 | 3h | ✅ 完成 |
| P3-006 | 集成测试 | API 端到端测试 | 4h | ✅ 完成 |

#### 8.3.2 统一 REST API 规范

```
# 预警服务 API 规范

## 基础路径
/api/v1/alerts

## 端点列表

### 1. 获取预警列表
GET /api/v1/alerts
Query: status, type, level, start_time, end_time, page, size
Response: { alerts: [], total: int, page: int, size: int }

### 2. 获取单个预警
GET /api/v1/alerts/{id}
Response: { alert: {} }

### 3. 创建预警（内部）
POST /api/v1/alerts
Body: { type, level, position, message, ... }
Response: { alert_id: string }

### 4. 确认预警
PUT /api/v1/alerts/{id}/acknowledge
Body: { action_taken: string }
Response: { success: bool }

### 5. 清除预警
PUT /api/v1/alerts/{id}/clear
Body: { reason: string }
Response: { success: bool }

### 6. 获取预警配置
GET /api/v1/alerts/config
Response: { thresholds: {}, push_methods: [] }

### 7. 更新预警配置
PUT /api/v1/alerts/config
Body: { thresholds: {}, push_methods: [] }
Response: { success: bool }

## WebSocket 消息格式
{
    "type": "alert_created|alert_updated|alert_cleared",
    "data": { alert object },
    "timestamp": "2026-04-06T12:00:00Z"
}
```

#### 8.3.3 验收标准

- [x] API 符合 RESTful 规范
- [x] 所有端点有完整的文档
- [x] 错误响应格式统一
- [x] WebSocket 消息格式统一

---

### 8.4 阶段四：数据融合（优先级：P3）

> 对应 Phase 4：持久化层统一

#### 8.4.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 | 状态 |
|--------|----------|----------|------------|------|
| P4-001 | 分析两模块 Repository 差异 | - | 2h | ✅ 完成 |
| P4-002 | 设计统一的数据访问接口 | `alert/alert_repository.h` | 3h | ✅ 完成 |
| P4-003 | 实现数据访问适配器 | `alarm/repository_adapter.cpp` | 4h | ✅ 完成 |
| P4-004 | 数据迁移脚本 | 新建脚本 | 2h | ✅ 完成 |
| P4-005 | 性能测试 | - | 2h | ✅ 完成 |

#### 8.4.2 验收标准

- [x] 数据访问接口统一
- [x] 数据迁移无丢失
- [x] 查询性能不降低

---

## 九、实施时间线

### 9.1 总体时间规划

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        实施时间线（共8周）                                │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Week 1-2: Phase 1 - 核心融合（P0）✅ 已完成                             │
│  ├── 里程碑 M1: 数据结构与计算器融合完成 ✅                               │
│  ├── Day 1-3: 统一枚举和数据类 ✅                                        │
│  ├── Day 4-5: 融合 UKCCalculator（蹲底效应）✅                            │
│  ├── Day 6-7: 融合 CpaCalculator（地球曲率）✅                            │
│  └── Day 8-10: 更新测试、验收 ✅                                         │
│                                                                         │
│  Week 3-4: Phase 2 - 服务融合（P1）✅ 已完成                             │
│  ├── 里程碑 M2: 检查器与推送服务融合完成 ✅                               │
│  ├── Day 1-3: 创建 CheckerAdapter 和 CheckerRegistry ✅                  │
│  ├── Day 4-6: 融合推送服务接口 ✅                                        │
│  └── Day 7-10: 集成测试、性能测试 ✅                                     │
│                                                                         │
│  Week 5-6: Phase 3 - API融合（P2）✅ 已完成                              │
│  ├── 里程碑 M3: API接口统一完成 ✅                                       │
│  ├── Day 1-3: 设计统一 REST API 规范 ✅                                  │
│  ├── Day 4-6: 实现路由适配器 ✅                                          │
│  └── Day 7-10: API 文档生成、集成测试 ✅                                 │
│                                                                         │
│  Week 7-8: Phase 4 - 数据融合（P3）✅ 已完成                             │
│  ├── 里程碑 M4: 持久化层统一完成 ✅                                      │
│  ├── Day 1-3: 设计统一 Repository 接口 ✅                                │
│  ├── Day 4-6: 数据迁移 ✅                                               │
│  └── Day 7-10: 性能测试、最终验收 ✅                                     │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 9.2 里程碑与验收点

| 里程碑 | 时间点 | 验收标准 | 状态 |
|--------|--------|----------|------|
| M1 | Week 2 结束 | 数据结构统一、计算器融合、测试通过 | ✅ 已完成 |
| M2 | Week 4 结束 | 检查器适配完成、推送服务统一、集成测试通过 | ✅ 已完成 |
| M3 | Week 6 结束 | API 规范统一、文档完整、端到端测试通过 | ✅ 已完成 |
| M4 | Week 8 结束 | Repository 统一、数据迁移完成、性能达标 | ✅ 已完成 |

### 9.3 回滚计划

| 阶段 | 回滚触发条件 | 回滚操作 |
|------|--------------|----------|
| Phase 1 | 测试失败率 > 5% | 恢复原有枚举定义 |
| Phase 2 | 推送成功率 < 95% | 切回原有推送服务 |
| Phase 3 | API 兼容性问题 | 启用版本路由 |
| Phase 4 | 数据迁移失败 | 恢复数据库备份 |

---

## 十、风险与缓解措施

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 破坏现有功能 | 高 | 完整的回归测试套件 |
| 性能下降 | 中 | 性能基准测试对比 |
| 数据丢失 | 高 | 数据备份和迁移验证 |
| API 不兼容 | 中 | 版本控制和向后兼容 |
| 依赖冲突 | 低 | 模块隔离和接口抽象 |

---

## 十一、成功指标

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 代码重复率 | < 5% | 静态分析工具 |
| 测试覆盖率 | > 80% | 代码覆盖率工具 |
| API 响应时间 | < 100ms | 性能监控 |
| 推送成功率 | > 99% | 日志统计 |
| 预警计算延迟 | < 1s | 性能测试 |

---

## 十二、附录

### 12.1 融合决策汇总

| 组件 | 推荐来源 | 融合要点 |
|------|----------|----------|
| **UKC 计算** | alarm | 添加蹲底效应计算 |
| **CPA 计算** | alarm | 添加地球曲率修正 |
| **CPA 结果结构** | alert | 保留丰富的结果信息 |
| **气象预警** | 两边都保留 | alert 主动计算 + alarm 被动集成 |
| **航道数据结构** | alarm | ChannelStatus/NavAidStatus 更完善 |
| **推送渠道抽象** | alert | IPushChannel 接口更灵活 |
| **推送配置** | alarm | PushConfig 结构体更直观 |
| **REST 路由** | alert | 动态注册更灵活 |
| **WebSocket 认证** | alert | 完整的认证机制 |
| **Repository 数据类型** | alert | AlertPtr 智能指针更安全 |

### 12.2 相关文档

| 文档 | 路径 |
|------|------|
| 对比分析报告 | `code/alarm/doc/compare_alarm_alert.md` |
| 项目编码规则 | `.trae/rules/project_rules.md` |
| 编码规范避坑清单 | `.trae/rules/pr_coding_standards.md` |

---

**文档完成**
