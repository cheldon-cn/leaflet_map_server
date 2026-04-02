# 执行失败自动重试机制

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、重试配置

```json
{
  "retry": {
    "enabled": true,
    "maxRetries": 3,
    "retryInterval": "5s",
    "exponentialBackoff": true
  }
}
```

---

## 二、可重试错误类型

| 错误类型 | 可重试 | 说明 |
|----------|--------|------|
| 网络超时 | ✅ | 临时性网络问题 |
| 资源锁定 | ✅ | 并发资源冲突 |
| 内存不足 | ❌ | 需要释放资源 |
| 语法错误 | ❌ | 需要修复代码 |
| 依赖缺失 | ❌ | 需要安装依赖 |

---

## 三、重试流程

```
任务失败 → 检查错误类型
  ↓
是否可重试？
  ├─ YES → 等待retryInterval
  │         ↓
  │       重试次数 < maxRetries？
  │         ├─ YES → 重新执行任务
  │         └─ NO → 标记为最终失败
  └─ NO → 标记为最终失败
```

---

## 四、指数退避

| 重试次数 | 等待时间 |
|----------|----------|
| 1 | 5s |
| 2 | 10s |
| 3 | 20s |

---

## 五、重试日志

```json
{
  "taskId": "T16",
  "retryCount": 2,
  "lastError": "网络超时",
  "nextRetryTime": "2026-04-02T14:35:00Z"
}
```

---

**版本**: v1.5
