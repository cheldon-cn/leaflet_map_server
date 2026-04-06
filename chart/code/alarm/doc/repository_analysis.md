# Repository 差异分析报告

> **版本**: v1.0  
> **日期**: 2026-04-06

---

## 一、接口对比

### 1.1 IAlertRepository 接口对比

| 功能 | alert 模块 | alarm 模块 | 差异说明 |
|------|-----------|-----------|----------|
| 保存预警 | `Save(AlertPtr)` | `Create(Alert)` | 参数类型不同 |
| 更新预警 | `Update(AlertPtr)` | `Update(Alert)` | 参数类型不同 |
| 删除预警 | `Delete(string)` | `Remove(string)` | 方法名不同 |
| 查找单个 | `FindById(string)` → `AlertPtr` | `GetById(string)` → `Alert` | 方法名和返回类型不同 |
| 查找全部 | `Query(AlertQueryParams)` | `FindAll(limit, offset)` | 参数类型不同 |
| 按类型查找 | `GetAlertsByType(type, userId)` | `FindByType(type)` | alert 需要用户ID |
| 按级别查找 | `GetAlertsByLevel(level, userId)` | `FindByLevel(level)` | alert 需要用户ID |
| 按状态查找 | 通过 `Query` 实现 | `FindByStatus(status)` | alarm 有独立方法 |
| 活动预警 | `GetActiveAlerts(userId)` | `FindActive()` | alert 需要用户ID |
| 确认预警 | `Acknowledge(id, userId, action)` | `Acknowledge(id, operatorId)` | 参数不同 |
| 更新状态 | `UpdateStatus(id, status)` | 无 | alert 独有 |
| 过期处理 | 无 | `Expire(id)` | alarm 独有 |
| 解除预警 | 无 | `Dismiss(id, reason)` | alarm 独有 |
| 统计信息 | 无 | `GetStatistics(start, end)` | alarm 独有 |
| 活动数量 | 通过 `Count` 实现 | `GetActiveCount()` | alarm 有独立方法 |
| 保存确认数据 | `SaveAcknowledge(AcknowledgeData)` | 无 | alert 独有 |
| 保存反馈 | `SaveFeedback(FeedbackData)` | 无 | alert 独有 |

### 1.2 IConfigRepository 接口对比

| 功能 | alert 模块 | alarm 模块 | 差异说明 |
|------|-----------|-----------|----------|
| 保存配置 | `SaveConfig(AlertConfig)` | 无 | alert 独有 |
| 加载配置 | `LoadConfig(userId)` | 无 | alert 独有 |
| 删除配置 | `DeleteConfig(userId)` | 无 | alert 独有 |
| 更新阈值 | `UpdateThreshold(userId, type, threshold)` | 无 | alert 独有 |
| 获取阈值 | `GetThreshold(userId, type)` | 无 | alert 独有 |
| 设置推送方式 | `SetPushMethods(userId, methods)` | 无 | alert 独有 |
| 获取推送方式 | `GetPushMethods(userId)` | 无 | alert 独有 |

---

## 二、数据结构对比

### 2.1 Alert 类型

| 属性 | alert 模块 | alarm 模块 | 差异说明 |
|------|-----------|-----------|----------|
| ID | `std::string id` | `std::string m_alertId` | 命名不同 |
| 类型 | `AlertType type` | `AlertType m_type` | 枚举值映射 |
| 级别 | `AlertLevel level` | `AlertLevel m_level` | 枚举值映射 |
| 状态 | `AlertStatus status` | `AlertStatus m_status` | 枚举值映射 |
| 标题 | `std::string title` | `std::string m_title` | 命名不同 |
| 消息 | `std::string message` | `std::string m_message` | 命名不同 |
| 位置 | `Coordinate position` | `Coordinate m_position` | 结构不同 |
| 时间 | `std::string created_at` | `std::string m_createTime` | 命名不同 |
| 用户ID | `std::string user_id` | `std::string m_shipId` | 语义不同 |

### 2.2 查询参数

| 参数 | alert 模块 | alarm 模块 |
|------|-----------|-----------|
| 类型 | `AlertQueryParams` 结构体 | `limit`, `offset` 参数 |
| 状态过滤 | `AlertQueryParams.status` | `FindByStatus()` 方法 |
| 时间范围 | `AlertQueryParams.start_time/end_time` | `GetStatistics()` 参数 |
| 分页 | `AlertQueryParams.page/size` | `limit`, `offset` 参数 |

---

## 三、适配策略

### 3.1 适配器设计

```cpp
class RepositoryAdapter : public ogc::alert::IAlertRepository {
public:
    RepositoryAdapter(std::shared_ptr<alert::IAlertRepository> alarmRepo);
    
    // 实现 alert 模块接口，内部调用 alarm 模块实现
    bool Save(const AlertPtr& alert) override;
    bool Update(const AlertPtr& alert) override;
    // ...
    
private:
    std::shared_ptr<alert::IAlertRepository> m_alarmRepo;
    
    // 类型转换辅助方法
    alert::Alert ConvertAlert(const ogc::alert::AlertPtr& alert);
    ogc::alert::AlertPtr ConvertAlert(const alert::Alert& alert);
};
```

### 3.2 类型映射

| alert 类型 | alarm 类型 | 转换方法 |
|-----------|-----------|----------|
| `AlertType::kDepth` | `AlertType::kDepthAlert` | `AlertTypeAdapter::ToAlertType()` |
| `AlertLevel::kLevel1` | `AlertLevel::kLevel1_Critical` | `AlertTypeAdapter::ToAlertLevel()` |
| `AlertStatus::kActive` | `AlertStatus::kActive` | `AlertTypeAdapter::ToAlertStatus()` |

### 3.3 功能映射

| alert 功能 | alarm 实现 | 备注 |
|-----------|-----------|------|
| `Save(AlertPtr)` | `Create(Alert)` | 类型转换后调用 |
| `FindById(id)` | `GetById(id)` | 类型转换后返回 |
| `GetActiveAlerts(userId)` | `FindActive()` | 忽略 userId 参数 |
| `Acknowledge(id, userId, action)` | `Acknowledge(id, operatorId)` | action 参数丢失 |
| `UpdateStatus(id, status)` | 通过 `Update(Alert)` 实现 | 需要先获取再更新 |

---

## 四、缺失功能处理

### 4.1 alert 独有功能

| 功能 | 处理方案 |
|------|----------|
| `UpdateStatus` | 通过 `GetById` + `Update` 组合实现 |
| `SaveAcknowledge` | 返回 false 或记录日志 |
| `SaveFeedback` | 返回 false 或记录日志 |
| 用户级别配置 | 暂不支持，使用默认配置 |

### 4.2 alarm 独有功能

| 功能 | 处理方案 |
|------|----------|
| `Expire` | 通过 `UpdateStatus(kExpired)` 实现 |
| `Dismiss` | 通过 `UpdateStatus(kCleared)` 实现 |
| `GetStatistics` | 新增到 alert 接口或忽略 |
| `GetActiveCount` | 通过 `Count` 实现 |

---

## 五、实施建议

### 5.1 优先级

1. **高优先级**：基本 CRUD 操作适配
2. **中优先级**：查询功能适配
3. **低优先级**：统计和反馈功能

### 5.2 测试策略

1. 单元测试：验证类型转换正确性
2. 集成测试：验证适配器与实际 Repository 交互
3. 性能测试：验证适配层不影响性能

---

**版本**: v1.0
