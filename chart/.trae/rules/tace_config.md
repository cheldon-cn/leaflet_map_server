# 配置文件规范

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、配置文件位置

所有配置参数必须定义在 `.trae/config.json` 文件中。

---

## 二、配置优先级

```
环境变量 > .trae/config.json > 规则默认值
```

---

## 三、配置项说明

| 配置节 | 说明 | 必需 |
|--------|------|------|
| triggerKeywords | 触发关键词列表 | ✅ |
| limits | 执行限制参数 | ✅ |
| concurrency | 并发执行配置 | ✅ |
| performance | 性能监控配置 | ✅ |
| stopKeywords | 停止请求关键词 | ✅ |
| paths | 文件路径配置 | ✅ |
| validation | 格式验证配置 | ✅ |
| retry | 重试机制配置 | ✅ |

---

## 四、环境变量覆盖

| 配置路径 | 环境变量名 |
|----------|------------|
| limits.maxExecutionTime | TASK_EXEC_MAX_TIME |
| limits.maxTasks | TASK_EXEC_MAX_TASKS |
| concurrency.enabled | TASK_EXEC_CONCURRENCY |
| concurrency.maxConcurrent | TASK_EXEC_MAX_CONCURRENT |

---

**版本**: v1.4
