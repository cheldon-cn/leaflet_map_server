# 海图预警系统用户手册

**版本**: v1.0
**日期**: 2026-04-04
**适用版本**: ogc_alert v1.0.0

---

## 一、系统概述

### 1.1 简介

海图预警系统是一个专业的航海安全预警平台，提供多种预警类型的实时监测和推送服务。系统支持水深预警、碰撞预警、气象预警、航道预警、偏航预警、限速预警和禁航区预警等多种预警类型。

### 1.2 主要功能

- **多类型预警**: 支持7种预警类型，覆盖航海安全各个方面
- **实时监测**: 实时监测船舶状态和环境条件
- **智能推送**: 多渠道推送预警信息（App、声音、短信、邮件）
- **配置灵活**: 支持用户自定义预警阈值和推送策略
- **历史查询**: 提供预警历史查询和统计分析

### 1.3 系统架构

```
┌─────────────────────────────────────────┐
│           应用层 (Application)           │
│  REST API │ WebSocket │ 推送服务 │ 查询  │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│           业务层 (Business)              │
│  预警引擎 │ 处理器 │ 调度器 │ 配置服务   │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│           数据层 (Data)                  │
│  存储库 │ 数据源 │ 缓存 │ 索引           │
└─────────────────────────────────────────┘
```

---

## 二、快速开始

### 2.1 系统初始化

```cpp
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/push_service.h"

using namespace ogc::alert;

// 创建预警引擎
auto engine = AlertEngine::Create();

// 配置引擎
EngineConfig config;
config.check_interval_ms = 1000;  // 检查间隔1秒
config.max_concurrent_checks = 10; // 最大并发检查数

// 初始化引擎
engine->Initialize(config);
```

### 2.2 注册预警检查器

```cpp
// 创建水深预警检查器
auto depth_checker = std::shared_ptr<DepthAlertChecker>(
    DepthAlertChecker::Create().release()
);

// 注册到引擎
engine->RegisterChecker(depth_checker);

// 创建碰撞预警检查器
auto collision_checker = std::shared_ptr<CollisionAlertChecker>(
    CollisionAlertChecker::Create().release()
);
engine->RegisterChecker(collision_checker);
```

### 2.3 启动预警系统

```cpp
// 启动引擎
engine->Start();

// 提交预警事件
AlertEvent event;
event.event_id = "EVENT_001";
event.event_type = "depth_check";
event.ship_id = "SHIP_001";
event.user_id = "USER_001";
event.position = Coordinate(120.5, 31.2);

engine->SubmitEvent(event);

// 停止引擎
engine->Stop();
```

---

## 三、预警类型

### 3.1 水深预警 (Depth Alert)

监测船舶龙骨富余水深（UKC），当水深不足时触发预警。

**配置参数**:
- `ukc_safety_margin`: UKC安全余量（米）
- `use_dynamic_ukc`: 是否使用动态UKC计算

**使用示例**:
```cpp
UKCCalculator calculator;
UKCInput input;
input.chart_depth = 10.0;      // 海图水深
input.tide_height = 2.0;       // 潮高
input.ship_draft = 5.0;        // 船舶吃水
input.safety_margin = 0.5;     // 安全余量

UKCResult result = calculator.Calculate(input);
if (!result.is_safe) {
    // 触发水深预警
}
```

### 3.2 碰撞预警 (Collision Alert)

计算CPA（最近会遇距离）和TCPA（最近会遇时间），预测碰撞风险。

**配置参数**:
- `cpa_threshold_level1`: CPA一级阈值（海里）
- `cpa_threshold_level2`: CPA二级阈值（海里）
- `tcpa_threshold_level1`: TCPA一级阈值（分钟）

**使用示例**:
```cpp
CPACalculator calculator;

ShipMotion own_ship;
own_ship.position = Coordinate(120.5, 31.2);
own_ship.course = 45.0;  // 航向
own_ship.speed = 12.0;   // 航速

ShipMotion target_ship;
target_ship.position = Coordinate(120.6, 31.3);
target_ship.course = 225.0;
target_ship.speed = 15.0;

CPAResult result = calculator.Calculate(own_ship, target_ship);
if (result.cpa < 2.0 && result.tcpa < 30.0) {
    // 触发碰撞预警
}
```

### 3.3 气象预警 (Weather Alert)

融合多种气象数据源，提供恶劣天气预警。

**支持数据源**:
- 气象站观测数据
- 卫星云图数据
- 雷达数据
- 数值预报数据

**使用示例**:
```cpp
WeatherFusion fusion;

WeatherObservation obs;
obs.location = Coordinate(120.5, 31.2);
obs.wind_speed = 25.0;     // 风速 m/s
obs.visibility = 500.0;    // 能见度 m
obs.wave_height = 3.0;     // 波高 m

fusion.AddObservation(obs);
WeatherCondition condition = fusion.Fuse();

if (condition.severity >= WeatherSeverity::kSevere) {
    // 触发气象预警
}
```

### 3.4 其他预警类型

- **航道预警**: 监测航道条件和交通状况
- **偏航预警**: 检测船舶偏离计划航线
- **限速预警**: 监测船舶是否超速
- **禁航区预警**: 检测船舶进入禁航区域

---

## 四、配置管理

### 4.1 用户配置

```cpp
auto config_service = std::make_shared<ConfigService>();

// 获取用户配置
AlertConfig config = config_service->GetConfig("USER_001");

// 设置阈值
config.depth_threshold.level1_threshold = 5.0;
config.collision_threshold.cpa_threshold_level1 = 2.0;

// 保存配置
config_service->SetConfig("USER_001", config);
```

### 4.2 推送策略

```cpp
auto push_service = std::make_shared<PushService>();

// 设置推送策略
std::vector<PushMethod> level1 = {PushMethod::kApp};
std::vector<PushMethod> level2 = {PushMethod::kApp, PushMethod::kSound};
std::vector<PushMethod> level3 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms};
std::vector<PushMethod> level4 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms, PushMethod::kEmail};

push_service->SetPushStrategy(level1, level2, level3, level4);
```

### 4.3 阈值管理

```cpp
auto threshold_manager = ThresholdManager::Create();

// 设置阈值
ThresholdConfig config;
config.alert_type = AlertType::kDepth;
config.value.level1_threshold = 5.0;
config.value.level2_threshold = 3.0;
config.value.level3_threshold = 1.0;

threshold_manager->SetThreshold(config);

// 获取阈值
auto threshold = threshold_manager->GetThreshold(AlertType::kDepth);
```

---

## 五、查询服务

### 5.1 查询预警列表

```cpp
auto repository = std::make_shared<AlertRepository>();
auto query_service = std::make_shared<QueryService>(repository);

// 构造查询参数
AlertQueryParams params;
params.user_id = "USER_001";
params.start_time = DateTime("2026-04-01 00:00:00");
params.end_time = DateTime("2026-04-04 23:59:59");
params.page = 1;
params.page_size = 20;

// 执行查询
AlertQueryResult result = query_service->GetAlertList(params);

// 遍历结果
for (const auto& alert : result.alerts) {
    std::cout << "Alert ID: " << alert->alert_id << std::endl;
    std::cout << "Type: " << static_cast<int>(alert->alert_type) << std::endl;
    std::cout << "Level: " << static_cast<int>(alert->alert_level) << std::endl;
}
```

### 5.2 查询活跃预警

```cpp
// 获取用户当前活跃预警
auto active_alerts = query_service->GetActiveAlerts("USER_001");

for (const auto& alert : active_alerts) {
    // 处理活跃预警
}
```

---

## 六、REST API

### 6.1 API端点

系统提供REST API接口，支持HTTP访问：

| 端点 | 方法 | 描述 |
|------|------|------|
| `/api/alerts` | GET | 查询预警列表 |
| `/api/alerts/{id}` | GET | 获取预警详情 |
| `/api/alerts` | POST | 创建预警 |
| `/api/config/{user_id}` | GET | 获取用户配置 |
| `/api/config/{user_id}` | PUT | 更新用户配置 |
| `/api/thresholds` | GET | 获取阈值配置 |
| `/api/thresholds` | PUT | 更新阈值配置 |

### 6.2 使用示例

```bash
# 查询预警列表
curl -X GET "http://localhost:8080/api/alerts?user_id=USER_001&page=1&page_size=20"

# 获取用户配置
curl -X GET "http://localhost:8080/api/config/USER_001"

# 更新阈值配置
curl -X PUT "http://localhost:8080/api/thresholds" \
  -H "Content-Type: application/json" \
  -d '{"type":"depth","level1":5.0,"level2":3.0}'
```

---

## 七、WebSocket服务

### 7.1 连接WebSocket

```javascript
// JavaScript示例
const ws = new WebSocket('ws://localhost:8080/ws');

ws.onopen = function() {
    // 订阅预警
    ws.send(JSON.stringify({
        action: 'subscribe',
        user_id: 'USER_001',
        alert_types: ['depth', 'collision']
    }));
};

ws.onmessage = function(event) {
    const alert = JSON.parse(event.data);
    console.log('Received alert:', alert);
};
```

### 7.2 消息格式

**订阅请求**:
```json
{
    "action": "subscribe",
    "user_id": "USER_001",
    "alert_types": ["depth", "collision"]
}
```

**预警推送**:
```json
{
    "alert_id": "ALERT_001",
    "alert_type": "depth",
    "alert_level": 2,
    "message": "水深预警：UKC不足",
    "position": {
        "longitude": 120.5,
        "latitude": 31.2
    },
    "timestamp": "2026-04-04T10:30:00Z"
}
```

---

## 八、性能优化

### 8.1 性能指标

系统性能指标如下：

| 操作 | 平均响应时间 | 目标 |
|------|--------------|------|
| 预警判定 | < 100ms | < 1s |
| 预警生成 | < 500ms | < 2s |
| 推送发送 | < 1s | < 3s |
| 查询响应 | < 200ms | < 1s |

### 8.2 性能测试

```cpp
#include "ogc/alert/performance_benchmark.h"

// 运行性能基准测试
PerformanceBenchmark benchmark;
benchmark.RunFullSystemBenchmark();
benchmark.PrintReport();

// 保存报告
benchmark.SaveReport("performance_report.txt");
```

---

## 九、故障排除

### 9.1 常见问题

**Q: 预警引擎无法启动**
A: 检查配置参数是否正确，确保至少注册了一个预警检查器。

**Q: 推送消息未收到**
A: 检查推送服务配置，确保推送渠道正确配置且可用。

**Q: 查询性能慢**
A: 检查数据库索引，确保常用查询字段已建立索引。

### 9.2 日志查看

系统使用统一的日志系统，可通过日志级别过滤：

```cpp
#include "ogc/draw/log.h"

// 设置日志级别
ogc::draw::LogHelper::SetLevel(ogc::draw::LogLevel::kDebug);

// 查看日志输出
// 日志文件位置: logs/alert_YYYYMMDD.log
```

---

## 十、附录

### 10.1 预警等级定义

| 等级 | 名称 | 描述 | 推送方式 |
|------|------|------|----------|
| Level1 | 提示 | 轻微风险 | App推送 |
| Level2 | 注意 | 中等风险 | App+声音 |
| Level3 | 警告 | 较高风险 | App+声音+短信 |
| Level4 | 危险 | 极高风险 | App+声音+短信+邮件 |

### 10.2 错误码定义

| 错误码 | 描述 |
|--------|------|
| 0 | 成功 |
| 1001 | 参数错误 |
| 1002 | 配置错误 |
| 2001 | 数据库错误 |
| 2002 | 查询超时 |
| 3001 | 推送失败 |
| 3002 | 连接失败 |

### 10.3 联系支持

如有问题，请联系技术支持：
- Email: support@example.com
- 文档: https://docs.example.com/alert

---

**文档版本**: v1.0
**最后更新**: 2026-04-04
