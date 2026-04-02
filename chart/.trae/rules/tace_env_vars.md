# 环境变量命名规则

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、命名规则

环境变量使用`TASK_EXEC_`前缀。

---

## 二、变量列表

| 配置路径 | 环境变量名 | 示例值 |
|----------|------------|--------|
| limits.maxExecutionTime | TASK_EXEC_MAX_TIME | "8h" |
| limits.maxTasks | TASK_EXEC_MAX_TASKS | "100" |
| limits.maxMemory | TASK_EXEC_MAX_MEMORY | "2GB" |
| concurrency.enabled | TASK_EXEC_CONCURRENCY | "true" |
| concurrency.maxConcurrent | TASK_EXEC_MAX_CONCURRENT | "3" |
| performance.enableMonitoring | TASK_EXEC_MONITORING | "true" |

---

## 三、加载流程

| 步骤 | 内容 |
|------|------|
| Step 1 | 扫描环境变量 |
| Step 2 | 转换为配置项类型 |
| Step 3 | 覆盖配置文件中的值 |
| Step 4 | 验证格式正确性 |

---

## 四、使用示例

```bash
export TASK_EXEC_MAX_TIME="4h"
export TASK_EXEC_MAX_CONCURRENT="5"
```

---

**版本**: v1.4
