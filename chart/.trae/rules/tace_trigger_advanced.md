# 触发词匹配模式

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、匹配模式

| 模式 | 说明 |
|------|------|
| exact | 精确匹配（默认） |
| fuzzy | 模糊匹配，包含关键词即可 |
| regex | 正则表达式匹配 |

---

## 二、配置方式

```json
{
  "taskExecution": {
    "triggerMatchMode": "exact",
    "triggerKeywords": ["开始任务", "执行计划"]
  }
}
```

---

## 三、正则匹配示例

```json
{
  "taskExecution": {
    "triggerMatchMode": "regex",
    "triggerPatterns": [
      "^(按|按照|继续).*tasks?\\.md.*执行$",
      "^(执行|实施).*任务.*清单$"
    ]
  }
}
```

---

## 四、匹配优先级

```markdown
IF triggerMatchMode == "exact" THEN
  → 用户输入必须完全匹配
ELSE IF triggerMatchMode == "fuzzy" THEN
  → 包含任一关键词即可
ELSE IF triggerMatchMode == "regex" THEN
  → 匹配任一正则表达式
END IF
```

---

**版本**: v1.4
