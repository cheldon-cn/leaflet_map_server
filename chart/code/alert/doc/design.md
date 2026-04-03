# 海图预警系统设计文档

**版本**: v1.1
**日期**: 2026-04-03
**状态**: 设计中
**需求来源**: requirement_spec.md v1.0
**变更记录**: v1.1 - 基于多角色交叉评审改进，添加核心调度逻辑、容错设计、缓存策略、并发处理、安全增强

---

## 1. 概述

### 1.1 文档目的

本文档描述海图预警系统的技术架构和详细设计，为后续开发提供技术指导。

### 1.2 范围

本设计覆盖以下功能：
- 水深预警（UKC计算）
- 气象预警（多源数据融合）
- 碰撞风险预警（CPA/TCPA计算）
- 航道状态预警（航行通告处理）
- 偏航预警（航线跟踪）
- 超速预警（限速区域监测）
- 禁航区预警（空间计算）
- 预警推送（多渠道推送）
- 预警配置管理
- 预警查询与确认

### 1.3 参考文档

- 海图预警系统需求规格说明书 v1.0
- 项目编码规则 (.trae/rules/project_rules.md)

### 1.4 需求追溯

| 需求ID | 需求描述 | 设计章节 |
|--------|----------|----------|
| F001 | 水深预警功能 | 4.1 |
| F002 | 气象预警功能 | 4.2 |
| F003 | 碰撞风险预警功能 | 4.3 |
| F004 | 航道状态预警功能 | 4.4 |
| F005 | 偏航预警功能 | 4.5 |
| F006 | 超速预警功能 | 4.6 |
| F007 | 禁航区预警功能 | 4.7 |
| F008 | 预警推送功能 | 4.8 |
| F009 | 预警配置管理功能 | 4.9 |
| F010 | 预警查询功能 | 4.10 |
| F011 | 预警确认功能 | 4.11 |

---

## 2. 设计目标

### 2.1 核心目标

1. **实时性**：预警判定<1s，预警生成<2s，推送发送<3s
2. **可靠性**：推送成功率>99%，预警遗漏率<0.1%
3. **可扩展性**：支持新增预警类型，支持多种推送渠道
4. **可配置性**：系统预设+用户自定义阈值配置

### 2.2 设计原则

- **单一职责原则**：每个模块只负责一种预警类型
- **开闭原则**：对扩展开放，对修改关闭
- **依赖倒置原则**：依赖抽象接口，不依赖具体实现
- **接口隔离原则**：接口小而专一

### 2.3 约束条件

| 约束项 | 要求 | 规则ID |
|--------|------|--------|
| 编程语言 | C++11 | STD-01~05 |
| 核心框架 | 无外部框架依赖，纯C++11标准库 | - |
| 代码规范 | Google C++ Style | - |
| 外部依赖 | libpq, sqlite3, libspatialite | - |

---

## 3. 系统架构

### 3.1 整体架构

采用分层架构模式，分为三层：

```
┌─────────────────────────────────────────────────────────────┐
│                      表现层 (API Layer)                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │AlertService │  │ConfigService│  │ QueryService│         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│                      业务层 (Business Layer)                 │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              AlertEngine (预警引擎核心)                │  │
│  ├──────────┬──────────┬──────────┬──────────┬─────────┤  │
│  │DepthAlert│WeatherAlert│CollisionAlert│ChannelAlert│...│  │
│  └──────────┴──────────┴──────────┴──────────┴─────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              PushService (推送服务)                    │  │
│  ├──────────┬──────────┬──────────┬──────────┐          │  │
│  │ AppPush  │ SoundPush│  SMSPush │ EmailPush│          │  │
│  └──────────┴──────────┴──────────┴──────────┘          │  │
├─────────────────────────────────────────────────────────────┤
│                      数据层 (Data Layer)                     │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │AlertRepository│ConfigRepository│DataSourceManager│      │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  SQLite     │  │ PostgreSQL  │  │  外部API    │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 架构决策

#### ADR-001: 选择分层架构

**背景**
系统功能需求11个，预计用户<1000，需要快速开发和维护。

**决策**
采用三层架构：表现层、业务层、数据层。

**理由**
1. 功能需求适中，分层架构足够应对
2. 团队熟悉分层架构，开发效率高
3. 模块间通过接口通信，便于测试和维护
4. 后期可演进为微服务架构

**影响**
- 模块间通过接口通信
- 数据层使用Repository模式
- 业务层使用策略模式实现不同预警类型

### 3.3 技术选型

| 层次 | 技术选型 | 版本 | 说明 |
|------|----------|------|------|
| 表现层 | 纯C++11 | C++11 | 无外部框架依赖 |
| 业务层 | 纯C++11 | C++11 | 使用策略模式、观察者模式 |
| 数据层 | SQLite/PostgreSQL | - | 使用libpq, sqlite3 |
| 空间数据 | libspatialite | - | 空间查询支持 |

---

### 3.4 模块间通信机制

#### 3.4.1 数据流架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         数据流向图                                        │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [外部数据源]                                                            │
│  ├── AIS数据 ──┐                                                        │
│  ├── 气象数据 ──┼──> [DataSourceManager] ──> [AlertEngine]              │
│  ├── 潮汐数据 ──┤         │                      │                      │
│  └── 航行通告 ──┘         │                      ▼                      │
│                           │              [预警判定]                      │
│                           │              [预警生成]                      │
│                           │                   │                         │
│                           ▼                   ▼                         │
│                    [数据缓存层]         [PushService]                    │
│                           │                   │                         │
│                           ▼                   ▼                         │
│                    [AlertRepository]    [推送渠道]                       │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 3.4.2 模块间接口调用时序

```
┌─────────┐     ┌──────────────┐     ┌─────────────┐     ┌─────────────┐
│ DataSource│     │ AlertEngine  │     │ PushService │     │ AlertRepo   │
│ Manager  │     │              │     │             │     │             │
└────┬────┘     └──────┬───────┘     └──────┬──────┘     └──────┬──────┘
     │                 │                    │                   │
     │ 1.获取数据      │                    │                   │
     │<────────────────│                    │                   │
     │                 │                    │                   │
     │ 2.返回数据      │                    │                   │
     │────────────────>│                    │                   │
     │                 │                    │                   │
     │                 │ 3.预警判定         │                   │
     │                 │───┐                │                   │
     │                 │   │                │                   │
     │                 │<──┘                │                   │
     │                 │                    │                   │
     │                 │ 4.生成预警         │                   │
     │                 │───┐                │                   │
     │                 │   │                │                   │
     │                 │<──┘                │                   │
     │                 │                    │                   │
     │                 │ 5.推送预警         │                   │
     │                 │───────────────────>│                   │
     │                 │                    │                   │
     │                 │                    │ 6.发送推送        │
     │                 │                    │───┐               │
     │                 │                    │   │               │
     │                 │                    │<──┘               │
     │                 │                    │                   │
     │                 │ 7.存储预警         │                   │
     │                 │───────────────────────────────────────>│
     │                 │                    │                   │
     │                 │                    │ 8.确认存储        │
     │                 │<───────────────────────────────────────│
     │                 │                    │                   │
```

#### 3.4.3 消息格式定义

```cpp
struct AlertMessage {
    std::string message_id;
    std::string source_module;
    std::string target_module;
    DateTime timestamp;
    MessageType type;
    std::string payload;
    int priority;
    bool require_ack;
};

enum class MessageType {
    DATA_REQUEST,
    DATA_RESPONSE,
    ALERT_GENERATED,
    ALERT_PUSHED,
    ALERT_ACKNOWLEDGED,
    ERROR_REPORT
};
```

---

### 3.5 AlertEngine核心调度逻辑

#### 3.5.1 预警引擎架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         AlertEngine 核心架构                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    调度器 (Scheduler)                             │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 定时任务队列 │  │ 事件队列   │  │ 优先级队列  │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    预警检查器 (AlertChecker)                      │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐   │   │
│  │  │Depth    │ │Weather  │ │Collision│ │Channel  │ │...      │   │   │
│  │  │Checker  │ │Checker  │ │Checker  │ │Checker  │ │         │   │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘ └─────────┘   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    预警处理器 (AlertProcessor)                    │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 预警生成器  │  │ 预警去重器  │  │ 预警聚合器  │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    推送分发器 (PushDispatcher)                    │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 推送策略    │  │ 渠道选择    │  │ 重试管理    │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 3.5.2 调度算法流程

```
调度流程:

1. 初始化阶段
   ├── 加载配置（阈值、推送策略）
   ├── 注册预警检查器
   └── 启动调度线程

2. 运行阶段
   ├── 循环执行:
   │   ├── 检查定时任务队列
   │   │   └── 每隔N秒执行一次预警检查
   │   ├── 检查事件队列
   │   │   └── 处理外部触发事件（如AIS更新）
   │   ├── 执行预警检查
   │   │   ├── 并行执行各类型预警检查
   │   │   └── 收集检查结果
   │   ├── 预警处理
   │   │   ├── 去重（相同预警合并）
   │   │   ├── 聚合（相关预警合并）
   │   │   └── 排序（按优先级）
   │   └── 推送分发
   │       ├── 选择推送渠道
   │       ├── 执行推送
   │       └── 记录推送结果

3. 关闭阶段
   ├── 停止接收新任务
   ├── 等待当前任务完成
   └── 释放资源
```

#### 3.5.3 核心接口定义

```cpp
class IAlertEngine {
public:
    virtual ~IAlertEngine() = default;
    
    virtual void Initialize(const EngineConfig& config) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    
    virtual void RegisterChecker(IAlertCheckerPtr checker) = 0;
    virtual void UnregisterChecker(const std::string& checker_id) = 0;
    
    virtual void SubmitEvent(const AlertEvent& event) = 0;
    virtual AlertStatistics GetStatistics() const = 0;
};

class IAlertChecker {
public:
    virtual ~IAlertChecker() = default;
    
    virtual std::string GetCheckerId() const = 0;
    virtual std::vector<AlertPtr> Check(const CheckContext& context) = 0;
    virtual int GetPriority() const = 0;
    virtual bool IsEnabled() const = 0;
};

class IAlertProcessor {
public:
    virtual ~IAlertProcessor() = default;
    
    virtual std::vector<AlertPtr> Process(std::vector<AlertPtr> alerts) = 0;
    virtual void SetDeduplicationStrategy(IDeduplicationStrategyPtr strategy) = 0;
    virtual void SetAggregationStrategy(IAggregationStrategyPtr strategy) = 0;
};
```

---

## 4. 核心模块设计

### 4.1 水深预警模块 (depth_alert)

**功能描述**: 监测水深变化，根据UKC计算触发预警

**需求覆盖**: F001

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IDepthAlertChecker | CheckDepth() | 检查水深并生成预警 |
| IDepthCalculator | CalculateUKC() | 计算富余水深 |
| ITideProvider | GetTideHeight() | 获取潮汐高度 |

**依赖关系**: 
- 依赖数据层获取海图水深、潮汐数据
- 依赖配置模块获取阈值配置

**数据结构**:

```cpp
struct DepthAlertData {
    double chart_depth;
    double tide_height;
    double measurement_error;
    double ship_draft;
    double squat;
    double ukc;
};

struct DepthAlertThreshold {
    double level1_threshold;
    double level2_threshold;
    double level3_threshold;
    double level4_threshold;
};

class DepthAlertResult {
public:
    int GetAlertLevel() const;
    double GetUKC() const;
    const Coordinate& GetPosition() const;
    const std::string& GetRecommendation() const;
};
```

**算法说明**:

```
有效水深 = 海图水深 + 潮高 - 测量误差
UKC = 有效水深 - 船舶吃水 - 航行下沉量
Squat = Cb × V² / 100 (Barrass公式)
```

### 4.2 气象预警模块 (weather_alert)

**功能描述**: 接收气象数据，匹配预警区域，生成气象预警

**需求覆盖**: F002

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IWeatherAlertChecker | CheckWeather() | 检查气象并生成预警 |
| IWeatherDataProvider | GetWeatherData() | 获取气象数据 |
| IAreaMatcher | MatchArea() | 匹配预警区域 |

**依赖关系**:
- 依赖外部气象API获取数据
- 依赖空间计算模块匹配区域

**数据结构**:

```cpp
enum class WeatherType {
    WIND,
    RAIN,
    THUNDER,
    TYPHOON,
    FOG
};

struct WeatherAlertData {
    WeatherType type;
    int alert_level;
    DateTime issue_time;
    DateTime expire_time;
    Geometry affected_area;
    std::string content;
    double wind_speed_min;
    double wind_speed_max;
    std::string recommendation;
};
```

### 4.3 碰撞风险预警模块 (collision_alert)

**功能描述**: 处理AIS数据，计算CPA/TCPA，评估碰撞风险

**需求覆盖**: F003

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| ICollisionAlertChecker | CheckCollision() | 检查碰撞风险 |
| ICPACalculator | CalculateCPA() | 计算最近会遇距离 |
| ITCPACalculator | CalculateTCPA() | 计算到达最近会遇点时间 |
| IAISDataProcessor | ProcessAISData() | 处理AIS数据 |

**依赖关系**:
- 依赖AIS数据源
- 依赖位置预测模块

**数据结构**:

```cpp
struct AISTarget {
    uint32_t mmsi;
    Coordinate position;
    double course;
    double speed;
    DateTime update_time;
};

struct CollisionRisk {
    uint32_t target_mmsi;
    double cpa;
    double tcpa;
    int alert_level;
    Coordinate cpa_position;
};

struct CollisionAlertThreshold {
    double cpa_threshold_level1;
    double cpa_threshold_level2;
    double cpa_threshold_level3;
    double tcpa_threshold_level1;
    double tcpa_threshold_level2;
    double tcpa_threshold_level3;
};
```

**算法说明**:

```
CPA = |ΔR × sin(ΔC - ΔB)|
TCPA = ΔR × cos(ΔC - ΔB) / 相对速度
```

### 4.4 航道状态预警模块 (channel_alert)

**功能描述**: 监测航道状态，包括航道关闭、航标异常、桥梁净高

**需求覆盖**: F004

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IChannelAlertChecker | CheckChannelStatus() | 检查航道状态 |
| INoticeProvider | GetNavigationNotice() | 获取航行通告 |
| IBeaconMonitor | MonitorBeaconStatus() | 监测航标状态 |
| IBridgeClearanceChecker | CheckBridgeClearance() | 检查桥梁净高 |

**依赖关系**:
- 依赖航行通告数据源
- 依赖航标监测系统

**数据结构**:

```cpp
enum class ChannelAlertType {
    CHANNEL_CLOSED,
    TEMPORARY_BAN,
    CONSTRUCTION_AREA,
    MILITARY_EXERCISE
};

enum class BeaconAnomalyType {
    POSITION_SHIFT,
    LIGHT_OFF,
    MISSING,
    TYPE_CHANGED
};

struct ChannelAlertData {
    ChannelAlertType type;
    int alert_level;
    Geometry affected_area;
    DateTime start_time;
    DateTime end_time;
    std::string description;
};

struct BridgeClearanceData {
    std::string bridge_id;
    double design_clearance;
    double current_clearance;
    double current_water_level;
    double design_water_level;
};
```

### 4.5 偏航预警模块 (deviation_alert)

**功能描述**: 监测船舶偏航距离，触发偏航预警

**需求覆盖**: F005

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IDeviationAlertChecker | CheckDeviation() | 检查偏航 |
| IRouteTracker | TrackRoute() | 跟踪航线 |
| IDistanceCalculator | CalculateDeviation() | 计算偏航距离 |

**依赖关系**:
- 依赖航线数据
- 依赖位置服务

**数据结构**:

```cpp
struct DeviationAlertData {
    double deviation_distance;
    double max_deviation;
    Coordinate current_position;
    Coordinate nearest_waypoint;
    int alert_level;
};

struct DeviationThreshold {
    double level1_threshold;
    double level2_threshold;
    double level3_threshold;
};
```

### 4.6 超速预警模块 (speed_alert)

**功能描述**: 监测船舶航速，在限速区域触发超速预警

**需求覆盖**: F006

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| ISpeedAlertChecker | CheckSpeed() | 检查航速 |
| ISpeedLimitProvider | GetSpeedLimit() | 获取限速信息 |
| IZoneMatcher | MatchSpeedZone() | 匹配限速区域 |

**依赖关系**:
- 依赖限速区域数据
- 依赖位置服务

**数据结构**:

```cpp
struct SpeedAlertData {
    double current_speed;
    double speed_limit;
    double over_speed_ratio;
    Coordinate position;
    int alert_level;
};

struct SpeedThreshold {
    double level1_ratio;
    double level2_ratio;
    double level3_ratio;
};
```

### 4.7 禁航区预警模块 (restricted_area_alert)

**功能描述**: 监测船舶与禁航区距离，触发禁航区预警

**需求覆盖**: F007

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IRestrictedAreaChecker | CheckRestrictedArea() | 检查禁航区 |
| IRestrictedAreaProvider | GetRestrictedAreas() | 获取禁航区数据 |
| IDistanceCalculator | CalculateDistance() | 计算距离 |

**依赖关系**:
- 依赖禁航区数据
- 依赖空间计算模块

**数据结构**:

```cpp
struct RestrictedArea {
    std::string area_id;
    std::string area_name;
    Geometry boundary;
    std::string restriction_type;
};

struct RestrictedAreaAlertData {
    std::string area_id;
    double distance_to_boundary;
    bool is_inside;
    int alert_level;
    Coordinate position;
};
```

### 4.8 预警推送模块 (push_service)

**功能描述**: 根据预警级别和用户配置，推送预警信息

**需求覆盖**: F008

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IPushService | PushAlert() | 推送预警 |
| IPushChannel | Send() | 发送推送 |
| IPushStrategy | DeterminePushMethod() | 确定推送方式 |

**依赖关系**:
- 依赖配置模块获取推送配置
- 依赖各推送渠道实现

**数据结构**:

```cpp
enum class PushMethod {
    APP,
    SOUND,
    SMS,
    EMAIL
};

struct PushData {
    std::string push_id;
    std::string alert_id;
    DateTime push_time;
    std::vector<PushMethod> methods;
    std::vector<std::string> target_users;
    AlertContent content;
    bool read_status;
    bool acknowledge_required;
};

struct AlertContent {
    std::string type;
    int level;
    std::string title;
    std::string message;
    Coordinate position;
    std::string action_required;
};

struct PushConfig {
    std::vector<PushMethod> level1_methods;
    std::vector<PushMethod> level2_methods;
    std::vector<PushMethod> level3_methods;
    std::vector<PushMethod> level4_methods;
    int dedup_interval_seconds;
};
```

### 4.9 预警配置模块 (config_service)

**功能描述**: 管理系统预设阈值和用户自定义配置

**需求覆盖**: F009

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IConfigService | GetAlertConfig() | 获取预警配置 |
| IConfigService | SetAlertConfig() | 设置预警配置 |
| IThresholdProvider | GetThreshold() | 获取阈值 |
| IUserConfigRepository | LoadUserConfig() | 加载用户配置 |

**依赖关系**:
- 依赖数据层存储配置

**数据结构**:

```cpp
struct AlertConfig {
    std::string user_id;
    DepthAlertConfig depth_config;
    CollisionAlertConfig collision_config;
    WeatherAlertConfig weather_config;
    QuietHoursConfig quiet_hours;
};

struct QuietHoursConfig {
    bool enabled;
    std::string start_time;
    std::string end_time;
};

struct SystemDefaultConfig {
    DepthAlertThreshold depth_threshold;
    CollisionAlertThreshold collision_threshold;
    DeviationThreshold deviation_threshold;
    SpeedThreshold speed_threshold;
};
```

### 4.10 预警查询模块 (query_service)

**功能描述**: 提供预警列表、详情、历史查询功能

**需求覆盖**: F010

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IQueryService | GetAlertList() | 获取预警列表 |
| IQueryService | GetAlertDetail() | 获取预警详情 |
| IQueryService | GetAlertHistory() | 获取历史预警 |
| IAlertRepository | QueryAlerts() | 查询预警数据 |

**依赖关系**:
- 依赖数据层查询数据

**数据结构**:

```cpp
struct AlertQueryParams {
    std::string user_id;
    std::vector<std::string> alert_types;
    std::vector<int> alert_levels;
    DateTime start_time;
    DateTime end_time;
    int page;
    int page_size;
};

struct AlertListResult {
    std::vector<AlertRecord> alerts;
    int total_count;
    int page;
    int page_size;
};
```

### 4.11 预警确认模块 (acknowledge_service)

**功能描述**: 处理用户预警确认和反馈

**需求覆盖**: F011

**接口定义**:

| 接口名 | 方法 | 说明 |
|--------|------|------|
| IAcknowledgeService | AcknowledgeAlert() | 确认预警 |
| IAcknowledgeService | SubmitFeedback() | 提交反馈 |
| IAlertRepository | UpdateAlertStatus() | 更新预警状态 |

**依赖关系**:
- 依赖数据层更新数据

**数据结构**:

```cpp
struct AcknowledgeData {
    std::string alert_id;
    std::string user_id;
    DateTime acknowledge_time;
    std::string user_action;
};

struct FeedbackData {
    std::string alert_id;
    std::string user_id;
    std::string feedback_type;
    std::string feedback_content;
    DateTime feedback_time;
};
```

---

## 5. 数据模型

### 5.1 实体关系

```
┌─────────────┐       ┌─────────────┐       ┌─────────────┐
│    User     │       │    Alert    │       │  AlertConfig│
├─────────────┤       ├─────────────┤       ├─────────────┤
│ user_id(PK) │──┐    │ alert_id(PK)│──┐    │ config_id(PK)│
│ username    │  │    │ alert_type  │  │    │ user_id(FK) │
│ email       │  │    │ alert_level │  │    │ alert_type  │
│ phone       │  │    │ issue_time  │  │    │ enabled     │
│ vip_status  │  │    │ expire_time │  │    │ thresholds  │
└─────────────┘  │    │ position    │  │    │ push_methods│
                 │    │ content     │  │    └─────────────┘
                 │    │ status      │  │
                 │    │ user_id(FK) │──┘
                 │    └─────────────┘
                 │           │
                 │           │
                 └───────────┘
```

### 5.2 数据库设计

#### 表: alerts (预警记录表)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| alert_id | VARCHAR(64) | PRIMARY KEY | 预警唯一标识 |
| alert_type | VARCHAR(32) | NOT NULL | 预警类型 |
| alert_level | INTEGER | NOT NULL | 预警级别(1-4) |
| issue_time | TIMESTAMP | NOT NULL | 发布时间 |
| expire_time | TIMESTAMP | | 过期时间 |
| position | GEOMETRY | | 预警位置(POINT) |
| content | TEXT | NOT NULL | 预警内容(JSON) |
| status | VARCHAR(16) | NOT NULL | 预警状态 |
| user_id | VARCHAR(64) | | 关联用户ID |
| created_at | TIMESTAMP | DEFAULT NOW() | 创建时间 |
| updated_at | TIMESTAMP | | 更新时间 |

**索引设计**:
- idx_alerts_type: (alert_type) - 按类型查询
- idx_alerts_level: (alert_level) - 按级别查询
- idx_alerts_status: (status) - 按状态查询
- idx_alerts_issue_time: (issue_time) - 按时间查询
- idx_alerts_position: GIST(position) - 空间查询

#### 表: alert_configs (预警配置表)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| config_id | VARCHAR(64) | PRIMARY KEY | 配置唯一标识 |
| user_id | VARCHAR(64) | UNIQUE, NOT NULL | 用户ID |
| alert_type | VARCHAR(32) | NOT NULL | 预警类型 |
| enabled | BOOLEAN | NOT NULL | 是否启用 |
| thresholds | TEXT | | 阈值配置(JSON) |
| push_methods | TEXT | | 推送方式(JSON) |
| created_at | TIMESTAMP | DEFAULT NOW() | 创建时间 |
| updated_at | TIMESTAMP | | 更新时间 |

**索引设计**:
- idx_alert_configs_user: (user_id) - 按用户查询
- idx_alert_configs_type: (alert_type) - 按类型查询

#### 表: alert_acknowledges (预警确认表)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| acknowledge_id | VARCHAR(64) | PRIMARY KEY | 确认唯一标识 |
| alert_id | VARCHAR(64) | NOT NULL | 预警ID |
| user_id | VARCHAR(64) | NOT NULL | 用户ID |
| acknowledge_time | TIMESTAMP | NOT NULL | 确认时间 |
| user_action | VARCHAR(256) | | 用户采取的行动 |
| feedback_type | VARCHAR(32) | | 反馈类型 |
| feedback_content | TEXT | | 反馈内容 |
| created_at | TIMESTAMP | DEFAULT NOW() | 创建时间 |

**索引设计**:
- idx_acknowledges_alert: (alert_id) - 按预警查询
- idx_acknowledges_user: (user_id) - 按用户查询

#### 表: push_records (推送记录表)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| push_id | VARCHAR(64) | PRIMARY KEY | 推送唯一标识 |
| alert_id | VARCHAR(64) | NOT NULL | 预警ID |
| push_time | TIMESTAMP | NOT NULL | 推送时间 |
| push_method | VARCHAR(16) | NOT NULL | 推送方式 |
| target_user | VARCHAR(64) | NOT NULL | 目标用户 |
| push_status | VARCHAR(16) | NOT NULL | 推送状态 |
| error_message | TEXT | | 错误信息 |
| created_at | TIMESTAMP | DEFAULT NOW() | 创建时间 |

**索引设计**:
- idx_push_records_alert: (alert_id) - 按预警查询
- idx_push_records_user: (target_user) - 按用户查询
- idx_push_records_time: (push_time) - 按时间查询

### 5.3 存储周期

| 数据类型 | 存储周期 | 存储位置 |
|---------|---------|---------|
| 实时预警 | 7天 | SQLite |
| 历史预警 | 1年 | PostgreSQL |
| 统计数据 | 3年 | PostgreSQL |
| 配置数据 | 永久 | SQLite/PostgreSQL |

---

## 6. 接口设计

### 6.1 API列表

| 模块 | 接口 | 方法 | 路径 |
|------|------|------|------|
| config | 订阅预警 | POST | /api/alert/subscribe |
| config | 取消订阅 | POST | /api/alert/unsubscribe |
| config | 获取配置 | GET | /api/alert/config |
| config | 设置配置 | PUT | /api/alert/config |
| query | 预警列表 | GET | /api/alert/list |
| query | 预警详情 | GET | /api/alert/detail |
| query | 历史预警 | GET | /api/alert/history |
| acknowledge | 确认预警 | POST | /api/alert/acknowledge |
| acknowledge | 预警反馈 | POST | /api/alert/feedback |

### 6.2 接口详细设计

#### POST /api/alert/subscribe

**请求参数**:
| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |
| alert_types | array | 是 | 预警类型列表 |
| push_methods | array | 是 | 推送方式列表 |

**返回值**:
```json
{
  "code": 0,
  "data": {
    "subscribe_id": "SUB_001",
    "status": "active"
  },
  "message": "success"
}
```

#### GET /api/alert/list

**请求参数**:
| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |
| alert_types | array | 否 | 预警类型过滤 |
| alert_levels | array | 否 | 预警级别过滤 |
| page | int | 否 | 页码(默认1) |
| page_size | int | 否 | 每页数量(默认20) |

**返回值**:
```json
{
  "code": 0,
  "data": {
    "alerts": [
      {
        "alert_id": "ALERT_001",
        "alert_type": "depth",
        "alert_level": 2,
        "title": "水深预警-橙色",
        "message": "前方2海里处水深不足",
        "issue_time": "2024-05-20T08:00:00Z",
        "status": "active"
      }
    ],
    "total_count": 10,
    "page": 1,
    "page_size": 20
  },
  "message": "success"
}
```

#### POST /api/alert/acknowledge

**请求参数**:
| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| alert_id | string | 是 | 预警ID |
| user_id | string | 是 | 用户ID |
| action | string | 否 | 采取的行动 |

**返回值**:
```json
{
  "code": 0,
  "data": {
    "acknowledge_id": "ACK_001",
    "acknowledge_time": "2024-05-20T08:30:00Z"
  },
  "message": "success"
}
```

---

## 7. 非功能性设计

### 7.1 性能设计

| 指标 | 要求 | 设计方案 |
|------|------|----------|
| 预警判定 | <1s | 内存缓存热点数据，预计算 |
| 预警生成 | <2s | 异步处理，消息队列 |
| 推送发送 | <3s | 并发推送，批量处理 |
| 预警查询 | <500ms | 索引优化，分页查询 |

#### 7.1.1 缓存策略设计

**缓存架构**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         缓存层架构                                        │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    L1: 本地内存缓存                               │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 阈值配置    │  │ 船舶位置    │  │ 预警状态    │              │   │
│  │  │ TTL: 5min   │  │ TTL: 30s    │  │ TTL: 1min   │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    L2: 数据库缓存                                 │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 海图数据    │  │ 气象数据    │  │ 航行通告    │              │   │
│  │  │ TTL: 1h     │  │ TTL: 10min  │  │ TTL: 30min  │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    数据源                                        │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ PostgreSQL  │  │ SQLite      │  │ 外部API     │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**缓存策略配置**:

| 缓存类型 | 数据内容 | TTL | 更新策略 | 容量限制 |
|----------|----------|-----|----------|----------|
| 阈值配置 | 用户预警配置 | 5分钟 | 配置变更时主动失效 | 10000条 |
| 船舶位置 | 当前船舶位置 | 30秒 | AIS更新时覆盖 | 1000条 |
| 预警状态 | 活跃预警状态 | 1分钟 | 预警变更时更新 | 5000条 |
| 海图数据 | 水深、禁航区 | 1小时 | 定时刷新 | 100MB |
| 气象数据 | 天气预报 | 10分钟 | 定时刷新 | 50MB |

**缓存接口定义**:

```cpp
template<typename K, typename V>
class ICache {
public:
    virtual ~ICache() = default;
    
    virtual bool Get(const K& key, V& value) = 0;
    virtual void Set(const K& key, const V& value, int ttl_seconds) = 0;
    virtual void Delete(const K& key) = 0;
    virtual void Clear() = 0;
    virtual CacheStats GetStats() const = 0;
};
```

#### 7.1.2 并发处理设计

**线程模型**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         并发处理架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    线程池配置                                     │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 调度线程池  │  │ 检查线程池  │  │ 推送线程池  │              │   │
│  │  │ 4 threads   │  │ 8 threads   │  │ 4 threads   │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    异步任务队列                                   │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 高优先级队列│  │ 普通优先级  │  │ 低优先级队列│              │   │
│  │  │ 容量: 100   │  │ 容量: 500   │  │ 容量: 1000  │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**并发处理策略**:

| 场景 | 并发策略 | 说明 |
|------|----------|------|
| 预警检查 | 并行执行 | 多类型预警同时检查 |
| 数据获取 | 异步IO | 非阻塞数据读取 |
| 推送发送 | 并发推送 | 多渠道同时推送 |
| 数据库操作 | 连接池 | 复用数据库连接 |

**异步处理接口**:

```cpp
class IAsyncTaskExecutor {
public:
    virtual ~IAsyncTaskExecutor() = default;
    
    virtual std::future<TResult> Submit(TaskPriority priority, 
                                        std::function<TResult()> task) = 0;
    virtual void SubmitBatch(std::vector<Task> tasks) = 0;
    virtual void Shutdown() = 0;
};

enum class TaskPriority {
    HIGH,
    NORMAL,
    LOW
};
```

#### 7.1.3 数据库查询优化

| 优化策略 | 实现方式 | 预期效果 |
|----------|----------|----------|
| 索引优化 | 为常用查询字段创建索引 | 查询时间减少80% |
| 分页查询 | 使用LIMIT/OFFSET | 减少数据传输量 |
| 预编译语句 | 使用PreparedStatement | 减少SQL解析开销 |
| 批量操作 | 使用批量INSERT/UPDATE | 减少数据库往返 |
| 连接池 | 维护数据库连接池 | 减少连接建立开销 |

### 7.2 可靠性设计

| 指标 | 要求 | 设计方案 |
|------|------|----------|
| 推送成功率 | >99% | 重试机制，备用通道 |
| 预警遗漏率 | <0.1% | 冗余设计，定时检查 |
| 系统可用性 | >99.9% | 故障转移，数据备份 |

#### 7.2.1 错误处理与容错机制

**错误分类与处理策略**:

| 错误类型 | 错误级别 | 处理策略 | 重试次数 | 重试间隔 |
|----------|----------|----------|----------|----------|
| 网络超时 | 可恢复 | 自动重试 | 3次 | 指数退避 |
| 数据库连接失败 | 可恢复 | 连接池重连 | 3次 | 1s, 2s, 4s |
| 外部API调用失败 | 可恢复 | 降级处理 | 2次 | 5s |
| 数据解析错误 | 不可恢复 | 记录日志，跳过 | 0 | - |
| 配置错误 | 不可恢复 | 使用默认配置 | 0 | - |
| 内存不足 | 系统级 | 优雅降级 | 0 | - |

**容错设计架构**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         容错机制架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    熔断器 (Circuit Breaker)                      │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ CLOSED      │  │ OPEN        │  │ HALF_OPEN   │              │   │
│  │  │ 正常状态    │  │ 熔断状态    │  │ 探测状态    │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    重试策略 (Retry Policy)                       │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 固定间隔    │  │ 指数退避    │  │ 随机抖动    │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    降级策略 (Fallback)                           │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 缓存降级    │  │ 默认值降级  │  │ 功能降级    │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**容错接口定义**:

```cpp
class ICircuitBreaker {
public:
    virtual ~ICircuitBreaker() = default;
    
    virtual bool AllowRequest() = 0;
    virtual void RecordSuccess() = 0;
    virtual void RecordFailure() = 0;
    virtual CircuitState GetState() const = 0;
};

class IRetryPolicy {
public:
    virtual ~IRetryPolicy() = default;
    
    virtual bool ShouldRetry(int attempt, const std::exception& e) = 0;
    virtual int GetNextDelay(int attempt) = 0;
};

enum class CircuitState {
    CLOSED,
    OPEN,
    HALF_OPEN
};
```

### 7.3 安全设计

- 用户认证：JWT Token认证
- 数据加密：敏感数据加密存储
- 访问控制：基于角色的权限控制
- 日志审计：记录关键操作日志

#### 7.3.1 认证授权设计

**JWT认证机制**:

| 项目 | 配置 | 说明 |
|------|------|------|
| Token有效期 | 2小时 | Access Token有效期 |
| 刷新Token有效期 | 7天 | Refresh Token有效期 |
| 签名算法 | HS256 | HMAC SHA-256 |
| 密钥长度 | 256位 | 安全密钥长度 |

**Token刷新流程**:

```
┌─────────┐                    ┌─────────────┐
│  Client │                    │   Server    │
└────┬────┘                    └──────┬──────┘
     │                                │
     │ 1.请求(带Access Token)         │
     │───────────────────────────────>│
     │                                │
     │ 2.Token过期(401)               │
     │<───────────────────────────────│
     │                                │
     │ 3.刷新请求(带Refresh Token)    │
     │───────────────────────────────>│
     │                                │
     │ 4.验证Refresh Token            │
     │                                │───┐
     │                                │   │
     │                                │<──┘
     │                                │
     │ 5.返回新Access Token           │
     │<───────────────────────────────│
     │                                │
```

#### 7.3.2 数据加密设计

**加密算法选择**:

| 数据类型 | 加密算法 | 密钥长度 | 说明 |
|----------|----------|----------|------|
| 敏感数据存储 | AES-256-GCM | 256位 | 对称加密，认证加密 |
| 密码存储 | bcrypt | - | 单向哈希，加盐 |
| Token签名 | HS256 | 256位 | HMAC SHA-256 |
| 传输加密 | TLS 1.3 | - | 传输层安全 |

**密钥管理方案**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         密钥管理架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    密钥存储                                      │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 环境变量    │  │ 密钥文件    │  │ 密钥管理服务│              │   │
│  │  │ (开发环境)  │  │ (测试环境)  │  │ (生产环境)  │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    密钥轮换                                      │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │   │
│  │  │ 轮换周期    │  │ 旧密钥保留  │  │ 无缝切换    │              │   │
│  │  │ 90天        │  │ 7天         │  │ 支持        │              │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 7.3.3 API访问频率限制

**Rate Limiting策略**:

| API类型 | 限制策略 | 时间窗口 | 说明 |
|---------|----------|----------|------|
| 预警查询 | 100次/分钟 | 滑动窗口 | 按用户ID限制 |
| 配置修改 | 10次/分钟 | 滑动窗口 | 按用户ID限制 |
| 预警确认 | 50次/分钟 | 滑动窗口 | 按用户ID限制 |
| 推送回调 | 1000次/分钟 | 滑动窗口 | 按IP限制 |

**实现方案**:

```cpp
class IRateLimiter {
public:
    virtual ~IRateLimiter() = default;
    
    virtual bool TryAcquire(const std::string& key, int permits) = 0;
    virtual int GetAvailablePermits(const std::string& key) = 0;
    virtual void Reset(const std::string& key) = 0;
};

struct RateLimitConfig {
    int max_requests;
    int window_seconds;
    std::string key_pattern;
};
```

**限流响应**:

```json
{
  "code": 429,
  "message": "Too Many Requests",
  "data": {
    "retry_after": 60,
    "limit": 100,
    "remaining": 0
  }
}
```

#### 7.3.4 日志脱敏策略

| 数据类型 | 脱敏规则 | 示例 |
|----------|----------|------|
| 手机号 | 保留前3后4 | 138****1234 |
| 邮箱 | 保留前3后@域名 | abc***@example.com |
| 身份证 | 保留前6后4 | 123456****1234 |
| 密码 | 完全隐藏 | ****** |
| Token | 保留前8后8 | eyJhbGci...J9.eyJ... |

### 7.4 可扩展性设计

- 模块化设计：支持新增预警类型
- 插件机制：支持新增推送渠道
- 配置驱动：阈值和规则可配置
- 接口抽象：便于替换实现

---

## 8. 测试策略

### 8.1 单元测试

- 覆盖率目标: >80%
- 测试框架: Google Test
- Mock框架: Google Mock

**测试重点**:
- 各预警算法正确性
- 阈值判断逻辑
- 数据转换正确性
- 异常处理

### 8.2 集成测试

- API测试：测试各接口功能
- 数据库测试：测试数据存取
- 推送测试：测试推送流程

### 8.3 性能测试

- 工具：自定义性能测试框架
- 场景：1000并发用户接收预警
- 指标：响应时间、吞吐量、资源占用

---

## 9. 需求追溯矩阵

| 需求ID | 需求描述 | 设计章节 | 实现模块 | 接口 | 状态 |
|--------|----------|----------|----------|------|------|
| F001 | 水深预警功能 | 4.1 | depth_alert | IDepthAlertChecker | 📋 待实现 |
| F002 | 气象预警功能 | 4.2 | weather_alert | IWeatherAlertChecker | 📋 待实现 |
| F003 | 碰撞风险预警功能 | 4.3 | collision_alert | ICollisionAlertChecker | 📋 待实现 |
| F004 | 航道状态预警功能 | 4.4 | channel_alert | IChannelAlertChecker | 📋 待实现 |
| F005 | 偏航预警功能 | 4.5 | deviation_alert | IDeviationAlertChecker | 📋 待实现 |
| F006 | 超速预警功能 | 4.6 | speed_alert | ISpeedAlertChecker | 📋 待实现 |
| F007 | 禁航区预警功能 | 4.7 | restricted_area_alert | IRestrictedAreaChecker | 📋 待实现 |
| F008 | 预警推送功能 | 4.8 | push_service | IPushService | 📋 待实现 |
| F009 | 预警配置管理功能 | 4.9 | config_service | IConfigService | 📋 待实现 |
| F010 | 预警查询功能 | 4.10 | query_service | IQueryService | 📋 待实现 |
| F011 | 预警确认功能 | 4.11 | acknowledge_service | IAcknowledgeService | 📋 待实现 |
| NF001 | 性能需求 | 7.1 | 全局 | - | 📋 待验证 |
| NF006 | 可靠性需求 | 7.2 | 全局 | - | 📋 待验证 |
| NF008 | 可用性需求 | 7.2 | 全局 | - | 📋 待验证 |

---

## 10. 附录

### 10.1 术语表

| 术语 | 说明 |
|------|------|
| UKC | Under Keel Clearance，富余水深 |
| CPA | Closest Point of Approach，最近会遇距离 |
| TCPA | Time to CPA，到达最近会遇点时间 |
| AIS | Automatic Identification System，自动识别系统 |
| VTS | Vessel Traffic Service，船舶交通服务 |
| ADR | Architecture Decision Record，架构决策记录 |

### 10.2 预警级别定义

| 级别 | 名称 | 颜色 | 说明 |
|------|------|------|------|
| 1 | 特别严重 | 红色 | 立即采取行动，生命安全威胁 |
| 2 | 严重 | 橙色 | 尽快采取行动，重大风险 |
| 3 | 较重 | 黄色 | 注意观察，潜在风险 |
| 4 | 一般 | 蓝色 | 提示信息，轻微风险 |

### 10.3 参考资源

- OGC Simple Features Specification
- IHO S-57 Standard
- IHO S-101 Standard
- 项目编码规则 (.trae/rules/project_rules.md)
