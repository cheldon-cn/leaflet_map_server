# 触发条件

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、默认触发词

| 触发词 | 说明 |
|--------|------|
| 按任务清单tasks.md执行 | 精确匹配 |
| 按照tasks.md清单实施 | 精确匹配 |
| 继续按任务清单执行 | 精确匹配 |
| 实施任务清单 | 精确匹配 |
| 执行任务清单 | 精确匹配 |

---

## 二、匹配模式

| 模式 | 说明 |
|------|------|
| exact | 精确匹配（默认） |
| fuzzy | 模糊匹配，包含关键词即可 |
| regex | 正则表达式匹配 |

---

## 三、自定义触发词

在 `.trae/config.json` 中配置：

```json
{
  "taskExecution": {
    "triggerMatchMode": "exact",
    "triggerKeywords": ["开始任务", "执行计划"]
  }
}
```

---

**版本**: v1.4
