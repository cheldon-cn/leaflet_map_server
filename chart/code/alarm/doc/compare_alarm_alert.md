# Alert 与 Alarm 模块对比分析报告

**日期**: 2026-04-06  
**分析范围**: code/alert vs code/alarm  
**版本**: v1.0

---

## 一、模块概述

### 1.1 alert 模块

| 属性 | 值 |
|------|-----|
| 路径 | `code/alert/` |
| 命名空间 | `ogc::alert` |
| 定位 | 航海预警系统核心引擎 |
| 特点 | 专业的航海预警计算（CPA/UKC）、多种预警类型、完整的预警引擎架构 |

### 1.2 alarm 模块

| 属性 | 值 |
|------|-----|
| 路径 | `code/alarm/` |
| 命名空间 | `alert` |
| 定位 | 警报服务与API层 |
| 特点 | REST/WebSocket API、数据适配器、通用警报引擎 |

---

## 二、功能对比

### 2.1 预警类型对比

| 预警类型 | alert 模块 | alarm 模块 |
|----------|------------|------------|
| 水深预警(UKC) | ✅ UKCCalculator | ✅ DepthAlertEngine |
| 碰撞预警(CPA) | ✅ CpaCalculator | ✅ CollisionAlertEngine |
| 气象预警 | ✅ WeatherAlertChecker | ✅ WeatherAlertEngine |
| 航道预警 | ✅ ChannelAlertChecker | ✅ ChannelAlertEngine |
| 偏航预警 | ✅ DeviationAlertChecker | - |
| 超速预警 | ✅ SpeedAlertChecker | - |
| 禁航区预警 | ✅ RestrictedAreaChecker | - |

### 2.2 核心类对比

| 功能 | alert 模块 | alarm 模块 |
|------|------------|------------|
| 预警引擎 | `IAlertEngine` | `IAlertEngine` |
| 预警检查器 | `IAlertChecker` | - |
| 预警计算器 | `CpaCalculator`, `UKCCalculator` | 内置于引擎 |
| 推送服务 | `IPushService` | `IAlertPushService` |
| 存储库 | `AlertRepository` | `AlertRepository` |
| REST接口 | `IRestController` | `RestController` |
| WebSocket | `WebSocketService` | `WebSocketService` |
| 配置管理 | `ConfigService` | `AlertConfigService` |

### 2.3 数据结构对比

| 类型 | alert 模块 | alarm 模块 |
|------|------------|------------|
| AlertType枚举 | `kDepth, kWeather, kCollision, kChannel, kDeviation, kSpeed, kRestrictedArea` | `kDepthAlert, kCollisionAlert, kWeatherAlert, kChannelAlert` |
| AlertLevel枚举 | `kNone, kLevel1-4` | `kLevel1_Critical - kLevel4_Minor` |
| AlertStatus枚举 | `kActive, kAcknowledged, kExpired, kDismissed` | `kPending, kActive, kPushed, kAcknowledged, kCleared, kExpired` |
| Alert类 | `AlertPtr` (智能指针) | `Alert` (值类型) |

---

## 三、重合部分分析

### 3.1 高度重合

| 功能 | 说明 |
|------|------|
| 预警核心概念 | 两模块都有Alert类、AlertType、AlertLevel、AlertStatus枚举 |
| 推送机制 | 两模块都有PushService，支持App/短信/邮件/声音推送 |
| REST API | 两模块都实现了REST控制器 |
| WebSocket | 两模块都支持WebSocket推送 |
| 数据持久化 | 两模块都有AlertRepository |
| 预警判定逻辑 | Depth/Collision/Weather/Channel四种类型两边都有实现 |

### 3.2 各自独有

#### alert 独有特性

| 特性 | 说明 |
|------|------|
| CPA/TCPA计算器 | 专业的最近会遇点计算 |
| UKC计算器 | 吃水深度安全余量计算 |
| 偏航预警 | 航线偏离检测 |
| 超速预警 | 限速区域检测 |
| 禁航区预警 | 空间区域检测 |
| 预警调度器 | `IScheduler` 定时调度 |
| 去重器 | `Deduplicator` 预警去重 |
| 缓存 | `Cache` 预警缓存 |
| 性能测试 | `PerformanceBenchmark`, `PerformanceTest` |
| 复杂的检查器架构 | `IAlertChecker` 抽象检查器模式 |

#### alarm 独有特性

| 特性 | 说明 |
|------|------|
| AIS数据适配器 | `AisDataAdapter` AIS数据接入 |
| 海图数据适配器 | `ChartDataAdapter` 海图数据接入 |
| 气象数据适配器 | `WeatherDataAdapter` 气象数据接入 |
| 外部数据网关 | `ExternalDataGateway` 统一外部数据接入 |
| 航行通告解析 | `NoticeParser` 航道通告处理 |
| 警报判定服务 | `AlertJudgeService` 判定逻辑服务 |
| 警报工厂 | `AlertEngineFactory` 引擎工厂模式 |

---

## 四、关系分析

### 4.1 当前关系

```
┌─────────────────────────────────────────────────────────────┐
│                        应用层 (Application)                  │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│     navi      │     │    alarm      │     │    parser     │
│  (导航系统)   │     │  (警报服务)   │     │  (海图解析)   │
└───────────────┘     └───────┬───────┘     └───────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                       alert (航海预警)                       │
└─────────────────────────────────────────────────────────────┘
```

- **alarm 模块**: 提供外部接口层（REST/WebSocket），负责与外部系统（AIS、气象、海图）对接
- **alert 模块**: 提供航海预警核心计算和引擎，处理具体的预警判定逻辑
- **依赖关系**: alarm 可以调用 alert 的核心计算能力

### 4.2 功能层次

```
┌────────────────────────────────────────────┐
│         表现层 (API/Interface)             │
│  ┌──────────────┐  ┌──────────────┐       │
│  │   alarm      │  │   alert      │       │
│  │ REST/WebSocket│  │ IRestController│    │
│  └──────────────┘  └──────────────┘       │
├────────────────────────────────────────────┤
│         业务层 (Business Logic)            │
│  ┌──────────────┐  ┌──────────────┐       │
│  │   alarm      │  │   alert      │       │
│  │ AlertJudgeSvc │  │ AlertEngine  │       │
│  │ DataAdapter   │  │ AlertChecker │       │
│  └──────────────┘  └──────────────┘       │
├────────────────────────────────────────────┤
│         计算层 (Computing)                 │
│  ┌──────────────┐  ┌──────────────┐       │
│  │   alarm      │  │   alert      │       │
│  │ (无计算)     │  │ CpaCalculator│       │
│  │              │  │ UKCCalculator│       │
│  └──────────────┘  └──────────────┘       │
└────────────────────────────────────────────┘
```

---

## 五、融合建议

### 5.1 是否需要融合？

**结论：建议保持分离，但加强协同**

理由：
1. **关注点分离**: alert 关注航海专业计算，alarm 关注服务与集成
2. **复用价值**: alert 的 CPA/UKC 计算可被 alarm 调用
3. **避免重复**: 两边都有预警引擎实现，需要整合
4. **演进路径**: alarm 可作为 alert 的服务包装层

### 5.2 融合方案

#### 方案A：保持独立，强化调用关系（推荐）

```
┌─────────────────────────────────────────────────────────────┐
│                       统一API层                              │
│         (alarm 作为 alert 的服务包装层)                      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌───────────────┐     ┌───────────────┐
│     alarm     │────▶│     alert     │
│  (服务/适配)  │     │ (核心引擎)    │
└───────────────┘     └───────────────┘
```

**实施要点**：
1. alarm 调用 alert 的 CPA/UKC 计算器
2. alarm 的引擎工厂创建 alert 的 IAlertChecker
3. 统一 Alert 数据结构（选择 alert 的设计）
4. 保留各自的 Repository 实现（可通过适配器统一）

#### 方案B：完全合并

**优点**：
- 消除重复代码
- 统一架构

**缺点**：
- 改动较大
- 可能破坏现有依赖
- 需要大量测试

### 5.3 推荐的融合路径

| 阶段 | 任务 | 优先级 |
|------|------|--------|
| 1 | 统一 Alert 数据结构（采用 alert 设计） | 高 |
| 2 | alarm 使用 alert 的 CPA/UKC 计算器 | 高 |
| 3 | alarm 使用 alert 的 IAlertChecker 体系 | 中 |
| 4 | 合并重复的推送服务（采用 alert 设计） | 中 |
| 5 | 统一 REST/WebSocket 接口规范 | 中 |
| 6 | 整合 Repository 实现 | 低 |

---

## 六、是否有必要独立的警报服务？

### 6.1 需要独立服务的原因

| 原因 | 说明 |
|------|------|
| 多系统集成 | AIS、气象、海图等多数据源需要统一接入层 |
| API抽象 | 为前端应用提供统一的HTTP/WebSocket接口 |
| 部署灵活 | 可独立部署、扩展的微服务架构 |
| 关注点分离 | 业务逻辑（alert）与服务集成（alarm）解耦 |

### 6.2 结论

**alarm 模块作为独立的警报服务是合理的**：
- 它是 alert 核心引擎的**服务包装层**
- 它处理外部系统集成和API暴露
- 它不替代 alert 的核心计算能力，而是调用它

---

## 七、总结

| 评估项 | 结论 |
|--------|------|
| 功能重合度 | 中等（4种预警类型重合，但实现方式不同） |
| 是否需要融合 | 建议保持分离，但加强协同 |
| 融合方式 | 采用方案A：alarm 作为 alert 的服务层 |
| 是否需要独立服务 | ✅ 是的，alarm 作为服务层有存在价值 |
| 下一步行动 | 统一Alert数据结构 → 打通计算器调用 → 整合引擎体系 |

---

## 八、预警判定逻辑详细对比

### 8.1 Depth（水深预警）对比

#### 8.1.1 实现差异

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 类名 | `DepthAlertChecker` | `DepthAlertEngine` |
| 设计模式 | Pimpl + IAlertChecker接口 | 简单继承 IAlertEngine |
| UKC计算 | `depth + tide - draft` | 考虑蹲底效应 squat |
| 返回类型 | `std::vector<AlertPtr>` 批量返回 | `Alert` 单个返回 |
| 数据源 | `SetDepthData/SetTideData` | `SetDepthProvider/SetTidalProvider` 回调 |

#### 8.1.2 核心计算对比

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

#### 8.1.3 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 蹲底效应计算 | alarm | 航海专业要求，高速航行安全因素 |
| Pimpl 设计模式 | alert | 更好的封装性和二进制兼容性 |
| 批量返回 | alert | 支持一次检查生成多个预警 |
| 数据提供者回调 | alarm | 更灵活的数据源适配 |

**推荐融合方案：**
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

### 8.2 Collision（碰撞预警）对比

#### 8.2.1 实现差异

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 类名 | `CollisionAlertChecker` + `CpaCalculator` | `CollisionAlertEngine` |
| 计算精度 | 平面坐标近似 | 地球曲率修正 |
| 结果信息 | 丰富（CPA位置、相对方位、警告消息） | 基础（CPA、TCPA） |
| 目标管理 | 单目标计算 | 多目标批量处理 |
| 风险排序 | 无 | 按风险级别排序 |

#### 8.2.2 核心计算对比

**alert 模块实现（平面坐标）：**
```cpp
CPAResult Calculate(const Coordinate& own_pos, double own_speed, double own_heading,
                    const Coordinate& target_pos, double target_speed, double target_heading) {
    // 平面坐标近似 - 未考虑地球曲率
    double dx = target_pos.longitude - own_pos.longitude;
    double dy = target_pos.latitude - own_pos.latitude;
    
    // 相对速度计算
    double rel_vx = target_vx - own_vx;
    double rel_vy = target_vy - own_vy;
    
    // CPA/TCPA 计算
    double tcpa = -(dx * rel_vx + dy * rel_vy) / rel_speed_squared;
    double cpa = std::sqrt(cpa_x * cpa_x + cpa_y * cpa_y);
    
    // 返回丰富的结果信息
    result.cpa_position_own = ...;    // CPA时刻本船位置
    result.cpa_position_target = ...;  // CPA时刻目标船位置
    result.relative_bearing = ...;     // 相对方位
    result.warning_message = ...;      // 详细警告消息
}
```

**alarm 模块实现（地球曲率修正）：**
```cpp
void CalculateCPA_TCPA(const Coordinate& myPos, double mySpeed, double myCourse,
                       const Coordinate& targetPos, double targetSpeed, double targetCourse,
                       double& cpa, double& tcpa) {
    // 地球曲率修正 - 更精确的距离计算
    double latAvg = (myPos.GetLatitude() + targetPos.GetLatitude()) / 2.0 * M_PI / 180.0;
    double metersPerDegLon = kEarthRadiusMeters * std::cos(latAvg) * M_PI / 180.0;
    double metersPerDegLat = kEarthRadiusMeters * M_PI / 180.0;
    
    // 转换为米制坐标
    double dx = (targetPos.GetLongitude() - myPos.GetLongitude()) * metersPerDegLon;
    double dy = (targetPos.GetLatitude() - myPos.GetLatitude()) * metersPerDegLat;
    
    // ... CPA/TCPA 计算
}
```

#### 8.2.3 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 地球曲率修正 | alarm | 长距离计算更精确 |
| 结果结构体 | alert | 信息丰富，便于决策 |
| 多目标处理 | alarm | 实际场景需要 |
| 风险排序 | alarm | 优先处理高风险目标 |
| 警告消息生成 | alert | 用户友好的提示 |

**推荐融合方案：**
```cpp
struct CPAResult {
    // 基础信息（两边都有）
    double cpa;
    double tcpa;
    bool is_dangerous;
    AlertLevel alert_level;
    
    // 扩展信息（来自 alert）
    Coordinate cpa_position_own;
    Coordinate cpa_position_target;
    double relative_bearing;
    std::string warning_message;
    
    // 扩展信息（来自 alarm）
    double distance_at_cpa;
    double relative_speed;
};

class CPACalculator {
public:
    // 使用 alarm 的地球曲率修正算法
    CPAResult Calculate(const Coordinate& own_pos, ...);
    
private:
    // 地球曲率修正（来自 alarm）
    double CorrectForEarthCurvature(const Coordinate& pos1, const Coordinate& pos2);
};
```

---

### 8.3 Weather（气象预警）对比

#### 8.3.1 设计理念差异

| 对比项 | alert 模块 | alarm 模块 |
|--------|------------|------------|
| 设计理念 | 主动计算型 | 被动集成型 |
| 数据来源 | 船载传感器 | 外部气象服务 |
| 触发方式 | 参数超阈值 | 接收预警信息 |
| 适用场景 | 实时监测 | 气象部门预警转发 |

#### 8.3.2 实现对比

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

#### 8.3.3 融合建议

**结论：两者都需要保留，分别用于不同场景**

| 场景 | 推荐使用 | 原因 |
|------|----------|------|
| 船载传感器实时监测 | alert | 需要主动计算预警级别 |
| 气象部门预警转发 | alarm | 需要接收和转换外部数据 |
| 综合气象预警 | 融合 | 结合两种数据源 |

**推荐融合方案：**
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

### 8.4 Channel（航道预警）对比

#### 8.4.1 数据结构对比

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

#### 8.4.2 融合建议

| 保留项 | 来源 | 原因 |
|--------|------|------|
| 数据结构 | alarm | 更完善，覆盖航道和助航设备 |
| 接口设计 | alert | Pimpl 模式更优雅 |
| 批量返回 | alert | 支持多个航道预警 |

**推荐融合方案：**
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

### 8.5 预警判定逻辑融合总结

```
┌─────────────────────────────────────────────────────────────┐
│                    融合后的预警引擎                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              统一接口层 (来自 alert)                  │   │
│  │  IAlertChecker + Pimpl + 批量返回                    │   │
│  └──────────────────────────┬──────────────────────────┘   │
│                             │                               │
│  ┌──────────────────────────▼──────────────────────────┐   │
│  │              核心计算层 (融合两模块)                  │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐   │   │
│  │  │ UKCCalculator│ │ CPACalculator│ │WeatherCalc │   │   │
│  │  │ (alarm squat│ │ (alarm 曲率 │ │ (alert 计算 │   │   │
│  │  │  + alert接口)│ │ + alert结果)│ │ + alarm集成)│   │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘   │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              数据结构层 (来自 alarm)                  │   │
│  │  WeatherAlertData, ChannelStatus, NavAidStatus      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 九、推送服务详细对比

### 9.1 接口设计对比

#### 9.1.1 alert 模块推送服务

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

#### 9.1.2 alarm 模块推送服务

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

### 9.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 渠道抽象 | IPushChannel 接口 | 枚举 + 配置 | alert（更灵活） |
| 渠道管理 | 动态注册/注销 | 配置开关 | alert（可扩展） |
| 推送策略 | 按预警级别配置 | 全渠道推送 | alert（更智能） |
| 配置方式 | 运行时配置 | 结构体配置 | alarm（更直观） |
| 回调机制 | 无 | 有 | alarm（便于监控） |
| 历史记录 | 有 | 无 | alert（便于追踪） |
| 重试机制 | 接口支持 | 配置支持 | 融合 |

### 9.3 融合建议

**推荐融合方案：**
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
    virtual void SetPushStrategy(...) = 0;
    
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

## 十、REST API 详细对比

### 10.1 接口设计对比

#### 10.1.1 alert 模块 REST 控制器

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

// 预警控制器
class AlertController {
public:
    HttpResponse GetAlerts(const HttpRequest& request);
    HttpResponse GetAlertById(const HttpRequest& request);
    HttpResponse GetActiveAlerts(const HttpRequest& request);
    HttpResponse AcknowledgeAlert(const HttpRequest& request);
    HttpResponse GetAlertStatistics(const HttpRequest& request);
};

// 配置控制器
class ConfigController {
public:
    HttpResponse GetConfig(const HttpRequest& request);
    HttpResponse UpdateConfig(const HttpRequest& request);
    HttpResponse GetThreshold(const HttpRequest& request);
    HttpResponse UpdateThreshold(const HttpRequest& request);
};
```

#### 10.1.2 alarm 模块 REST 控制器

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

### 10.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 路由注册 | 动态注册 | 固定路由 | alert（更灵活） |
| 响应构造 | 静态工厂方法 | 手动构造 | alert（更简洁） |
| 错误处理 | 简单消息 | 结构化错误 | alarm（更详细） |
| 认证支持 | 无 | authorization 字段 | alarm |
| 分页支持 | 无 | AlertListRequest | alarm |
| 业务请求 | 无 | 结构化请求体 | alarm |

### 10.3 融合建议

**推荐融合方案：**
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

## 十一、WebSocket 服务详细对比

### 11.1 接口设计对比

#### 11.1.1 alert 模块 WebSocket 服务

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

#### 11.1.2 alarm 模块 WebSocket 服务

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

### 11.2 功能对比

| 功能 | alert 模块 | alarm 模块 | 推荐 |
|------|------------|------------|------|
| 配置完整性 | 有 host 字段 | 无 host 字段 | alert |
| 会话信息 | 有 remote_address | 有 lastActiveTime | 融合 |
| 消息发送 | BroadcastExcept | BroadcastToType | 融合 |
| 认证机制 | 完整 | 基础 | alert |
| 预警订阅 | AlertType 枚举 | 字符串类型 | alert |
| 推送服务集成 | 无 | 有 | alarm |
| 消息大小限制 | 无 | 有 | alarm |

### 11.3 融合建议

**推荐融合方案：**
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

## 十二、Repository 详细对比

### 12.1 接口设计对比

#### 12.1.1 alert 模块 Repository

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

#### 12.1.2 alarm 模块 Repository

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

### 12.2 功能对比

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

### 12.3 融合建议

**推荐融合方案：**
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

## 十三、融合实施优先级

### 13.1 优先级矩阵

| 组件 | 重合度 | 融合价值 | 实施难度 | 优先级 |
|------|--------|----------|----------|--------|
| 预警判定逻辑 | 高 | 高 | 中 | **P0** |
| 数据结构（枚举） | 高 | 高 | 低 | **P0** |
| 推送服务 | 中 | 高 | 中 | **P1** |
| REST API | 中 | 中 | 低 | **P2** |
| WebSocket | 中 | 中 | 中 | **P2** |
| Repository | 中 | 中 | 低 | **P3** |

### 13.2 实施顺序

```
Phase 1 (P0): 核心融合
├── 统一 AlertType/AlertLevel/AlertStatus 枚举
├── 融合 UKCCalculator（添加蹲底效应）
├── 融合 CPACalculator（添加地球曲率修正）
└── 统一 Alert 数据类

Phase 2 (P1): 服务融合
├── 融合推送服务接口
├── 融合推送配置结构
└── 统一推送策略

Phase 3 (P2): API融合
├── 统一 REST 控制器接口
├── 统一 WebSocket 服务接口
└── 统一 HTTP 请求/响应结构

Phase 4 (P3): 数据融合
├── 统一 Repository 接口
├── 统一查询参数结构
└── 添加统计功能
```

---

## 十四、实施路线（详细）

### 14.1 总体架构目标

```
┌─────────────────────────────────────────────────────────────┐
│                    目标架构                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                   alarm (服务层)                      │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐   │   │
│  │  │ REST API    │ │ WebSocket   │ │ 数据适配器  │   │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘   │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐   │   │
│  │  │ AlertFacade │ │ ConfigSvc   │ │ PushService │   │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘   │   │
│  └──────────────────────────┬──────────────────────────┘   │
│                             │ 调用                          │
│                             ▼                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                   alert (核心层)                      │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐   │   │
│  │  │ AlertEngine │ │ IAlertChecker│ │ Scheduler  │   │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘   │   │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐   │   │
│  │  │CpaCalculator│ │UKCCalculator│ │ Deduplicator│   │   │
│  │  └─────────────┘ └─────────────┘ └─────────────┘   │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 14.2 阶段一：统一数据结构（优先级：高）

#### 14.2.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P1-001 | 定义统一的 AlertType 枚举 | `alert/types.h`, `alarm/alert_types.h` | 2h |
| P1-002 | 定义统一的 AlertLevel 枚举 | `alert/types.h`, `alarm/alert_types.h` | 1h |
| P1-003 | 定义统一的 AlertStatus 枚举 | `alert/types.h`, `alarm/alert_types.h` | 1h |
| P1-004 | 统一 Alert 数据类 | `alert/types.h`, `alarm/alert.h` | 4h |
| P1-005 | 创建类型转换适配器 | 新建 `alarm/alert_type_adapter.h` | 2h |
| P1-006 | 更新单元测试 | `alert/tests/*`, `alarm/tests/*` | 4h |

#### 14.2.2 统一枚举设计

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
    kCritical = 1,    // 一级-危急
    kSevere = 2,      // 二级-严重
    kModerate = 3,    // 三级-中等
    kMinor = 4        // 四级-轻微
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

#### 14.2.3 验收标准

- [ ] 两模块使用相同的枚举定义
- [ ] 所有现有测试通过
- [ ] 无编译警告
- [ ] 类型转换适配器覆盖所有旧值

---

### 14.3 阶段二：打通计算器调用（优先级：高）

#### 14.3.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P2-001 | alarm 添加 alert 模块依赖 | `alarm/CMakeLists.txt` | 0.5h |
| P2-002 | 创建计算器调用包装类 | 新建 `alarm/calculator_wrapper.h` | 3h |
| P2-003 | 重构 DepthAlertEngine 使用 UKCCalculator | `alarm/depth_alert_engine.cpp` | 4h |
| P2-004 | 重构 CollisionAlertEngine 使用 CpaCalculator | `alarm/collision_alert_engine.cpp` | 4h |
| P2-005 | 添加集成测试 | 新建 `alarm/tests/calculator_integration_test.cpp` | 3h |
| P2-006 | 性能基准测试 | 新建 `alarm/tests/calculator_benchmark.cpp` | 2h |

#### 8.3.2 计算器调用包装类设计

```cpp
// alarm/calculator_wrapper.h
#pragma once

#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/ukc_calculator.h"
#include "alert/alert_types.h"
#include <memory>

namespace alert {

class CalculatorWrapper {
public:
    CalculatorWrapper();
    ~CalculatorWrapper();
    
    // UKC计算 - 调用alert模块的UKCCalculator
    struct UKCInput {
        double chart_depth;
        double tide_height;
        double ship_draft;
        double speed_knots;
        double safety_margin;
    };
    
    struct UKCResult {
        double ukc;
        bool is_safe;
        AlertLevel alert_level;
        std::string warning_message;
    };
    
    UKCResult CalculateUKC(const UKCInput& input);
    
    // CPA计算 - 调用alert模块的CpaCalculator
    struct CPAInput {
        double own_ship_lon;
        double own_ship_lat;
        double own_ship_speed;
        double own_ship_heading;
        double target_lon;
        double target_lat;
        double target_speed;
        double target_heading;
    };
    
    struct CPAResult {
        double cpa;           // 最近会遇距离（海里）
        double tcpa;          // 到达最近点时间（分钟）
        bool is_dangerous;
        AlertLevel alert_level;
    };
    
    CPAResult CalculateCPA(const CPAInput& input);
    
private:
    std::unique_ptr<ogc::alert::UKCCalculator> m_ukc_calc;
    std::unique_ptr<ogc::alert::CpaCalculator> m_cpa_calc;
};

}
```

#### 14.3.3 重构示例

```cpp
// 重构前: alarm/depth_alert_engine.cpp
Alert DepthAlertEngine::Evaluate(const AlertContext& context) {
    // 内置计算逻辑...
    double ukc = CalculateUKCInternally(context);
    // ...
}

// 重构后: alarm/depth_alert_engine.cpp
#include "calculator_wrapper.h"

Alert DepthAlertEngine::Evaluate(const AlertContext& context) {
    CalculatorWrapper::UKCInput input;
    input.chart_depth = context.chart_depth;
    input.tide_height = context.tide_height;
    input.ship_draft = context.ship_draft;
    input.speed_knots = context.speed;
    input.safety_margin = m_threshold.safety_margin;
    
    auto result = m_calculator->CalculateUKC(input);
    
    Alert alert;
    alert.SetAlertLevel(result.alert_level);
    alert.SetMessage(result.warning_message);
    // ...
}
```

#### 8.3.4 验收标准

- [ ] alarm 的 DepthAlertEngine 使用 alert 的 UKCCalculator
- [ ] alarm 的 CollisionAlertEngine 使用 alert 的 CpaCalculator
- [ ] 计算结果与原有实现一致
- [ ] 性能不低于原有实现
- [ ] 所有测试通过

---

### 14.4 阶段三：整合检查器体系（优先级：中）

#### 14.4.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P3-001 | 创建 IAlertChecker 适配器 | 新建 `alarm/checker_adapter.h` | 3h |
| P3-002 | 重构 AlertEngineFactory | `alarm/alert_engine_factory.cpp` | 4h |
| P3-003 | 迁移 WeatherAlertEngine | `alarm/weather_alert_engine.cpp` | 3h |
| P3-004 | 迁移 ChannelAlertEngine | `alarm/channel_alert_engine.cpp` | 3h |
| P3-005 | 添加检查器注册机制 | `alarm/checker_registry.h` | 2h |
| P3-006 | 更新集成测试 | `alarm/tests/` | 3h |

#### 14.4.2 检查器适配器设计

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

#### 14.4.3 验收标准

- [ ] alarm 可使用 alert 的 IAlertChecker
- [ ] AlertEngineFactory 支持创建适配后的引擎
- [ ] 所有预警类型正常工作
- [ ] 测试覆盖率 > 80%

---

### 14.5 阶段四：合并推送服务（优先级：中）

#### 14.5.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P4-001 | 分析两模块推送服务差异 | - | 2h |
| P4-002 | 设计统一推送接口 | `alert/push_service.h` | 3h |
| P4-003 | 实现推送渠道适配器 | 新建 `alarm/push_channel_adapter.h` | 4h |
| P4-004 | 迁移 alarm 的推送配置 | `alarm/alert_push_service.cpp` | 2h |
| P4-005 | 统一推送策略 | `alert/push_strategy.h` | 3h |
| P4-006 | 端到端推送测试 | 新建测试文件 | 3h |

#### 8.5.2 统一推送接口设计

```cpp
// 统一的推送接口（基于 alert 设计扩展）
class IPushService {
public:
    virtual ~IPushService() = default;
    
    // 渠道管理
    virtual void RegisterChannel(IPushChannelPtr channel) = 0;
    virtual void UnregisterChannel(PushMethod method) = 0;
    virtual IPushChannelPtr GetChannel(PushMethod method) const = 0;
    
    // 推送策略
    virtual void SetPushStrategy(const std::vector<PushMethod>& level1_methods,
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
    
    // 统计
    virtual PushStatistics GetStatistics() const = 0;
};
```

#### 14.5.3 验收标准

- [ ] 两模块使用统一的推送接口
- [ ] 支持所有推送渠道（App/短信/邮件/声音/WebSocket）
- [ ] 推送成功率 > 99%
- [ ] 推送延迟 < 3s

---

### 14.6 阶段五：统一API规范（优先级：中）

#### 14.6.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P5-001 | 设计统一的 REST API 规范 | 新建 `doc/api_spec.md` | 3h |
| P5-002 | 统一 HTTP 请求/响应结构 | `alert/rest_controller.h` | 2h |
| P5-003 | 统一 WebSocket 消息格式 | `alert/websocket_service.h` | 2h |
| P5-004 | 实现路由适配器 | `alarm/rest_adapter.cpp` | 4h |
| P5-005 | API 文档生成 | 使用 OpenAPI 规范 | 3h |
| P5-006 | 集成测试 | API 端到端测试 | 4h |

#### 8.6.2 统一 REST API 规范

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

#### 14.6.3 验收标准

- [ ] API 符合 RESTful 规范
- [ ] 所有端点有完整的文档
- [ ] 错误响应格式统一
- [ ] WebSocket 消息格式统一

---

### 8.7 阶段六：整合Repository（优先级：低）

#### 8.7.1 任务清单

| 任务ID | 任务描述 | 涉及文件 | 预估工作量 |
|--------|----------|----------|------------|
| P6-001 | 分析两模块 Repository 差异 | - | 2h |
| P6-002 | 设计统一的数据访问接口 | 新建 `common/alert_repository.h` | 3h |
| P6-003 | 实现数据访问适配器 | `alarm/repository_adapter.cpp` | 4h |
| P6-004 | 数据迁移脚本 | 新建脚本 | 2h |
| P6-005 | 性能测试 | - | 2h |

#### 14.7.2 验收标准

- [ ] 数据访问接口统一
- [ ] 数据迁移无丢失
- [ ] 查询性能不降低

---

### 14.8 实施时间线

```
Week 1-2: 阶段一（统一数据结构）
    ├── Day 1-3: 定义统一枚举和数据类
    ├── Day 4-5: 创建类型转换适配器
    └── Day 6-7: 更新测试

Week 3-4: 阶段二（打通计算器调用）
    ├── Day 1-2: 添加依赖和创建包装类
    ├── Day 3-5: 重构引擎
    └── Day 6-7: 集成测试和性能测试

Week 5-6: 阶段三（整合检查器体系）
    ├── Day 1-3: 创建适配器和注册机制
    ├── Day 4-5: 迁移引擎
    └── Day 6-7: 测试

Week 7-8: 阶段四（合并推送服务）
    ├── Day 1-2: 分析和设计
    ├── Day 3-5: 实现适配器
    └── Day 6-7: 端到端测试

Week 9-10: 阶段五（统一API规范）
    ├── Day 1-3: 设计和实现
    ├── Day 4-5: 文档生成
    └── Day 6-7: 集成测试

Week 11-12: 阶段六（整合Repository）
    ├── Day 1-3: 设计和实现
    ├── Day 4-5: 数据迁移
    └── Day 6-7: 性能测试
```

---

### 8.9 风险与缓解措施

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 破坏现有功能 | 高 | 完整的回归测试套件 |
| 性能下降 | 中 | 性能基准测试对比 |
| 数据丢失 | 高 | 数据备份和迁移验证 |
| API 不兼容 | 中 | 版本控制和向后兼容 |
| 依赖冲突 | 低 | 模块隔离和接口抽象 |

---

### 14.10 成功指标

| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 代码重复率 | < 5% | 静态分析工具 |
| 测试覆盖率 | > 80% | 代码覆盖率工具 |
| API 响应时间 | < 100ms | 性能监控 |
| 推送成功率 | > 99% | 日志统计 |
| 预警计算延迟 | < 1s | 性能测试 |

---

**报告完成**
