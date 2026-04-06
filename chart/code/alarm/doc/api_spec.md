# 预警服务 REST API 规范

> **版本**: v1.0  
> **日期**: 2026-04-06  
> **基础路径**: `/api/v1/alerts`

---

## 一、API 概述

### 1.1 设计原则

| 原则 | 说明 |
|------|------|
| RESTful | 遵循 REST 架构风格 |
| 统一响应 | 所有接口使用统一的响应结构 |
| 版本控制 | URL 路径包含版本号 |
| 错误处理 | 标准化的错误响应格式 |

### 1.2 认证方式

```
Authorization: Bearer <token>
```

### 1.3 通用响应结构

```json
{
  "code": 0,
  "message": "success",
  "data": {},
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 1.4 错误响应结构

```json
{
  "code": 1001,
  "message": "Invalid parameter",
  "details": "Alert type is required",
  "timestamp": "2026-04-06T12:00:00Z"
}
```

---

## 二、预警管理接口

### 2.1 获取预警列表

**GET** `/api/v1/alerts`

**请求参数 (Query)**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| status | string | 否 | 预警状态: pending, active, pushed, acknowledged, cleared, expired |
| type | string | 否 | 预警类型: depth, collision, weather, channel |
| level | integer | 否 | 预警级别: 1-4 |
| start_time | string | 否 | 开始时间 (ISO 8601) |
| end_time | string | 否 | 结束时间 (ISO 8601) |
| page | integer | 否 | 页码，默认 1 |
| size | integer | 否 | 每页数量，默认 20，最大 100 |

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "alerts": [
      {
        "alert_id": "alert-20260406-001",
        "alert_type": "depth",
        "alert_level": 1,
        "status": "active",
        "title": "水深预警",
        "message": "当前水深不足，UKC低于安全阈值",
        "position": {
          "longitude": 120.5,
          "latitude": 31.2
        },
        "issue_time": "2026-04-06T12:00:00Z",
        "expire_time": "2026-04-06T13:00:00Z",
        "acknowledge_required": true
      }
    ],
    "total": 1,
    "page": 1,
    "size": 20
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 2.2 获取单个预警

**GET** `/api/v1/alerts/{id}`

**路径参数**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 预警ID |

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "alert": {
      "alert_id": "alert-20260406-001",
      "alert_type": "depth",
      "alert_level": 1,
      "status": "active",
      "title": "水深预警",
      "message": "当前水深不足，UKC低于安全阈值",
      "position": {
        "longitude": 120.5,
        "latitude": 31.2
      },
      "issue_time": "2026-04-06T12:00:00Z",
      "expire_time": "2026-04-06T13:00:00Z",
      "acknowledge_required": true,
      "details": {
        "current_depth": 5.2,
        "ukc": 0.3,
        "safety_depth": 0.5
      }
    }
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 2.3 创建预警（内部接口）

**POST** `/api/v1/alerts`

**请求体**

```json
{
  "alert_type": "depth",
  "alert_level": 1,
  "position": {
    "longitude": 120.5,
    "latitude": 31.2
  },
  "title": "水深预警",
  "message": "当前水深不足",
  "expire_minutes": 60,
  "details": {
    "current_depth": 5.2,
    "ukc": 0.3
  }
}
```

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "alert_id": "alert-20260406-001"
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 2.4 确认预警

**PUT** `/api/v1/alerts/{id}/acknowledge`

**路径参数**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 预警ID |

**请求体**

```json
{
  "action_taken": "已调整航向避开浅水区",
  "user_id": "user-001"
}
```

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "alert_id": "alert-20260406-001",
    "status": "acknowledged",
    "acknowledge_time": "2026-04-06T12:05:00Z"
  },
  "timestamp": "2026-04-06T12:05:00Z"
}
```

### 2.5 清除预警

**PUT** `/api/v1/alerts/{id}/clear`

**路径参数**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 预警ID |

**请求体**

```json
{
  "reason": "已安全通过浅水区",
  "user_id": "user-001"
}
```

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "alert_id": "alert-20260406-001",
    "status": "cleared",
    "clear_time": "2026-04-06T12:10:00Z"
  },
  "timestamp": "2026-04-06T12:10:00Z"
}
```

---

## 三、配置管理接口

### 3.1 获取预警配置

**GET** `/api/v1/alerts/config`

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "thresholds": {
      "depth": {
        "level1": 0.5,
        "level2": 1.0,
        "level3": 2.0,
        "level4": 3.0
      },
      "collision": {
        "level1": 0.5,
        "level2": 1.0,
        "level3": 2.0,
        "level4": 3.0
      }
    },
    "push_methods": ["app", "sound", "sms", "email"],
    "enabled_channels": {
      "app": true,
      "sound": true,
      "sms": false,
      "email": false
    }
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 3.2 更新预警配置

**PUT** `/api/v1/alerts/config`

**请求体**

```json
{
  "thresholds": {
    "depth": {
      "level1": 0.5,
      "level2": 1.0,
      "level3": 2.0,
      "level4": 3.0
    }
  },
  "enabled_channels": {
    "app": true,
    "sound": true,
    "sms": true,
    "email": false
  }
}
```

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "updated": true
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

---

## 四、推送服务接口

### 4.1 获取推送状态

**GET** `/api/v1/alerts/push/status`

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "channels": {
      "app": {
        "enabled": true,
        "available": true
      },
      "sound": {
        "enabled": true,
        "available": true
      },
      "sms": {
        "enabled": false,
        "available": true
      },
      "email": {
        "enabled": false,
        "available": false
      }
    }
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

### 4.2 手动推送预警

**POST** `/api/v1/alerts/{id}/push`

**路径参数**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | string | 是 | 预警ID |

**请求体**

```json
{
  "channels": ["app", "sound"],
  "user_ids": ["user-001", "user-002"]
}
```

**响应示例**

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "push_results": [
      {
        "channel": "app",
        "success": true,
        "message_id": "push-001"
      },
      {
        "channel": "sound",
        "success": true,
        "message_id": "push-002"
      }
    ]
  },
  "timestamp": "2026-04-06T12:00:00Z"
}
```

---

## 五、WebSocket 接口

### 5.1 连接地址

```
ws://host/api/v1/alerts/ws
```

### 5.2 消息格式

**订阅预警**

```json
{
  "action": "subscribe",
  "types": ["depth", "collision"],
  "levels": [1, 2]
}
```

**预警通知**

```json
{
  "event": "alert",
  "data": {
    "alert_id": "alert-20260406-001",
    "alert_type": "depth",
    "alert_level": 1,
    "title": "水深预警",
    "message": "当前水深不足",
    "position": {
      "longitude": 120.5,
      "latitude": 31.2
    },
    "issue_time": "2026-04-06T12:00:00Z"
  }
}
```

**状态更新通知**

```json
{
  "event": "status_update",
  "data": {
    "alert_id": "alert-20260406-001",
    "old_status": "active",
    "new_status": "acknowledged",
    "update_time": "2026-04-06T12:05:00Z"
  }
}
```

---

## 六、错误码定义

| 错误码 | 说明 |
|--------|------|
| 0 | 成功 |
| 1001 | 参数错误 |
| 1002 | 预警不存在 |
| 1003 | 预警已处理 |
| 1004 | 权限不足 |
| 1005 | 认证失败 |
| 2001 | 配置错误 |
| 2002 | 推送失败 |
| 3001 | 数据库错误 |
| 3002 | 内部服务错误 |

---

## 七、数据类型定义

### 7.1 AlertType 枚举

| 值 | 说明 |
|----|------|
| depth | 水深预警 |
| collision | 碰撞预警 |
| weather | 气象预警 |
| channel | 航道预警 |
| deviation | 偏航预警 |
| speed | 超速预警 |
| restricted_area | 禁航区预警 |

### 7.2 AlertLevel 枚举

| 值 | 说明 |
|----|------|
| 1 | 一级-危急 |
| 2 | 二级-严重 |
| 3 | 三级-中等 |
| 4 | 四级-轻微 |

### 7.3 AlertStatus 枚举

| 值 | 说明 |
|----|------|
| pending | 待处理 |
| active | 活跃 |
| pushed | 已推送 |
| acknowledged | 已确认 |
| cleared | 已清除 |
| expired | 已过期 |
| dismissed | 已忽略 |

---

**版本历史**

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-04-06 | 初始版本 |
