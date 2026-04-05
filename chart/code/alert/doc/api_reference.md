# 海图预警系统API文档

**版本**: v1.0
**日期**: 2026-04-04
**基础URL**: http://localhost:8080/api

---

## 一、API概览

### 1.1 认证

所有API请求需要在Header中携带认证信息：

```
Authorization: Bearer <token>
```

### 1.2 响应格式

所有API响应均为JSON格式：

```json
{
    "code": 0,
    "message": "success",
    "data": { ... }
}
```

### 1.3 错误码

| 错误码 | 描述 |
|--------|------|
| 0 | 成功 |
| 1001 | 参数错误 |
| 1002 | 配置错误 |
| 2001 | 数据库错误 |
| 2002 | 查询超时 |
| 3001 | 推送失败 |
| 3002 | 连接失败 |

---

## 二、预警管理API

### 2.1 查询预警列表

**请求**:
```
GET /alerts
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |
| alert_type | string | 否 | 预警类型 |
| alert_level | int | 否 | 预警等级 |
| start_time | string | 否 | 开始时间 |
| end_time | string | 否 | 结束时间 |
| page | int | 否 | 页码（默认1） |
| page_size | int | 否 | 每页数量（默认20） |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "alerts": [
            {
                "alert_id": "ALERT_001",
                "alert_type": "depth",
                "alert_level": 2,
                "status": "active",
                "issue_time": "2026-04-04T10:30:00Z",
                "expire_time": "2026-04-04T11:30:00Z",
                "position": {
                    "longitude": 120.5,
                    "latitude": 31.2
                },
                "content": {
                    "type": "depth",
                    "level": 2,
                    "title": "水深预警",
                    "message": "UKC不足，当前水深10米，船舶吃水8米",
                    "action_required": "减速或改变航向"
                },
                "user_id": "USER_001",
                "acknowledge_required": true
            }
        ],
        "total_count": 100,
        "page": 1,
        "page_size": 20
    }
}
```

### 2.2 获取预警详情

**请求**:
```
GET /alerts/{alert_id}
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| alert_id | string | 是 | 预警ID |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "alert_id": "ALERT_001",
        "alert_type": "depth",
        "alert_level": 2,
        "status": "active",
        "issue_time": "2026-04-04T10:30:00Z",
        "expire_time": "2026-04-04T11:30:00Z",
        "position": {
            "longitude": 120.5,
            "latitude": 31.2
        },
        "content": {
            "type": "depth",
            "level": 2,
            "title": "水深预警",
            "message": "UKC不足，当前水深10米，船舶吃水8米",
            "action_required": "减速或改变航向"
        },
        "user_id": "USER_001",
        "acknowledge_required": true
    }
}
```

### 2.3 确认预警

**请求**:
```
POST /alerts/{alert_id}/acknowledge
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| alert_id | string | 是 | 预警ID |
| user_id | string | 是 | 用户ID |
| action | string | 否 | 采取的行动 |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "alert_id": "ALERT_001",
        "acknowledged": true,
        "acknowledge_time": "2026-04-04T10:35:00Z"
    }
}
```

---

## 三、配置管理API

### 3.1 获取用户配置

**请求**:
```
GET /config/{user_id}
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "user_id": "USER_001",
        "depth_threshold": {
            "level1_threshold": 5.0,
            "level2_threshold": 3.0,
            "level3_threshold": 1.0,
            "level4_threshold": 0.5,
            "ukc_safety_margin": 0.5,
            "use_dynamic_ukc": true
        },
        "collision_threshold": {
            "level1_threshold": 6.0,
            "level2_threshold": 4.0,
            "level3_threshold": 2.0,
            "level4_threshold": 1.0,
            "cpa_threshold_level1": 6.0,
            "cpa_threshold_level2": 4.0,
            "cpa_threshold_level3": 2.0,
            "tcpa_threshold_level1": 30.0,
            "tcpa_threshold_level2": 20.0,
            "tcpa_threshold_level3": 10.0
        },
        "push_methods": ["app", "sound", "sms"],
        "quiet_hours": {
            "enabled": true,
            "start_time": "22:00",
            "end_time": "06:00"
        }
    }
}
```

### 3.2 更新用户配置

**请求**:
```
PUT /config/{user_id}
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |
| config | object | 是 | 配置对象 |

**请求体**:
```json
{
    "depth_threshold": {
        "level1_threshold": 5.0,
        "level2_threshold": 3.0,
        "level3_threshold": 1.0,
        "level4_threshold": 0.5
    },
    "push_methods": ["app", "sound"]
}
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "user_id": "USER_001",
        "updated": true
    }
}
```

### 3.3 重置用户配置

**请求**:
```
POST /config/{user_id}/reset
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 是 | 用户ID |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "user_id": "USER_001",
        "reset": true
    }
}
```

---

## 四、阈值管理API

### 4.1 获取阈值配置

**请求**:
```
GET /thresholds
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| alert_type | string | 否 | 预警类型 |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "thresholds": [
            {
                "alert_type": "depth",
                "level1_threshold": 5.0,
                "level2_threshold": 3.0,
                "level3_threshold": 1.0,
                "level4_threshold": 0.5
            },
            {
                "alert_type": "collision",
                "level1_threshold": 6.0,
                "level2_threshold": 4.0,
                "level3_threshold": 2.0,
                "level4_threshold": 1.0
            }
        ]
    }
}
```

### 4.2 更新阈值配置

**请求**:
```
PUT /thresholds
```

**请求体**:
```json
{
    "alert_type": "depth",
    "level1_threshold": 5.0,
    "level2_threshold": 3.0,
    "level3_threshold": 1.0,
    "level4_threshold": 0.5
}
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "alert_type": "depth",
        "updated": true
    }
}
```

---

## 五、推送服务API

### 5.1 获取推送策略

**请求**:
```
GET /push/strategy
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "level1": ["app"],
        "level2": ["app", "sound"],
        "level3": ["app", "sound", "sms"],
        "level4": ["app", "sound", "sms", "email"]
    }
}
```

### 5.2 设置推送策略

**请求**:
```
PUT /push/strategy
```

**请求体**:
```json
{
    "level1": ["app"],
    "level2": ["app", "sound"],
    "level3": ["app", "sound", "sms"],
    "level4": ["app", "sound", "sms", "email"]
}
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "updated": true
    }
}
```

### 5.3 测试推送

**请求**:
```
POST /push/test
```

**请求体**:
```json
{
    "user_id": "USER_001",
    "push_method": "app",
    "message": "测试推送消息"
}
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "sent": true,
        "send_time": "2026-04-04T10:30:00Z"
    }
}
```

---

## 六、统计查询API

### 6.1 获取预警统计

**请求**:
```
GET /statistics/alerts
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 否 | 用户ID |
| start_time | string | 否 | 开始时间 |
| end_time | string | 否 | 结束时间 |
| group_by | string | 否 | 分组方式（day/hour/type） |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "total_count": 1000,
        "by_type": {
            "depth": 300,
            "collision": 250,
            "weather": 200,
            "channel": 150,
            "deviation": 100
        },
        "by_level": {
            "level1": 500,
            "level2": 300,
            "level3": 150,
            "level4": 50
        },
        "by_day": [
            {
                "date": "2026-04-01",
                "count": 120
            },
            {
                "date": "2026-04-02",
                "count": 150
            }
        ]
    }
}
```

### 6.2 获取系统状态

**请求**:
```
GET /statistics/system
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "engine_status": "running",
        "active_alerts": 15,
        "total_checkers": 7,
        "uptime_seconds": 86400,
        "performance": {
            "avg_check_time_ms": 50,
            "avg_push_time_ms": 200,
            "avg_query_time_ms": 30
        }
    }
}
```

---

## 七、反馈API

### 7.1 提交反馈

**请求**:
```
POST /feedback
```

**请求体**:
```json
{
    "user_id": "USER_001",
    "alert_id": "ALERT_001",
    "feedback_type": "false_alarm",
    "rating": 3,
    "comment": "该预警为误报",
    "contact": "user@example.com"
}
```

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "feedback_id": "FB_001",
        "submitted": true,
        "submit_time": "2026-04-04T10:30:00Z"
    }
}
```

### 7.2 获取反馈列表

**请求**:
```
GET /feedback
```

**参数**:
| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| user_id | string | 否 | 用户ID |
| status | string | 否 | 状态（pending/reviewed/resolved） |
| page | int | 否 | 页码 |
| page_size | int | 否 | 每页数量 |

**响应**:
```json
{
    "code": 0,
    "message": "success",
    "data": {
        "feedbacks": [
            {
                "feedback_id": "FB_001",
                "user_id": "USER_001",
                "alert_id": "ALERT_001",
                "feedback_type": "false_alarm",
                "rating": 3,
                "comment": "该预警为误报",
                "status": "pending",
                "submit_time": "2026-04-04T10:30:00Z"
            }
        ],
        "total_count": 50,
        "page": 1,
        "page_size": 20
    }
}
```

---

## 八、WebSocket API

### 8.1 连接

```
ws://localhost:8080/ws
```

### 8.2 订阅预警

**请求**:
```json
{
    "action": "subscribe",
    "user_id": "USER_001",
    "alert_types": ["depth", "collision"]
}
```

**响应**:
```json
{
    "action": "subscribed",
    "user_id": "USER_001",
    "alert_types": ["depth", "collision"]
}
```

### 8.3 取消订阅

**请求**:
```json
{
    "action": "unsubscribe",
    "user_id": "USER_001",
    "alert_types": ["depth"]
}
```

### 8.4 接收预警

**推送消息**:
```json
{
    "alert_id": "ALERT_001",
    "alert_type": "depth",
    "alert_level": 2,
    "status": "active",
    "issue_time": "2026-04-04T10:30:00Z",
    "position": {
        "longitude": 120.5,
        "latitude": 31.2
    },
    "content": {
        "type": "depth",
        "level": 2,
        "title": "水深预警",
        "message": "UKC不足",
        "action_required": "减速或改变航向"
    }
}
```

---

## 九、数据类型定义

### 9.1 Alert

| 字段 | 类型 | 描述 |
|------|------|------|
| alert_id | string | 预警ID |
| alert_type | string | 预警类型 |
| alert_level | int | 预警等级（1-4） |
| status | string | 状态（active/acknowledged/expired） |
| issue_time | string | 发布时间 |
| expire_time | string | 过期时间 |
| position | object | 位置信息 |
| content | object | 预警内容 |
| user_id | string | 用户ID |
| acknowledge_required | bool | 是否需要确认 |

### 9.2 AlertType

| 值 | 描述 |
|----|------|
| depth | 水深预警 |
| collision | 碰撞预警 |
| weather | 气象预警 |
| channel | 航道预警 |
| deviation | 偏航预警 |
| speed | 限速预警 |
| restricted | 禁航区预警 |

### 9.3 PushMethod

| 值 | 描述 |
|----|------|
| app | App推送 |
| sound | 声音提醒 |
| sms | 短信通知 |
| email | 邮件通知 |

---

**文档版本**: v1.0
**最后更新**: 2026-04-04
