# Skill协作

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、与code-implementer协作

```
用户请求 → 本规则生效 → 调用code-implementer
    → 检测完成状态 → 有任务则继续 → 无任务则输出报告
```

| 组件 | 职责 |
|------|------|
| 本规则 | 持续执行、优先级排序、完成检测 |
| code-implementer | 单任务TDD实施、测试验证 |

---

## 二、与task-planner协作

```
用户请求"规划并执行" → task-planner生成tasks.md
    → 自动触发本规则 → 执行任务清单
```

| 组件 | 职责 |
|------|------|
| task-planner | 设计文档分析、任务分解 |
| 本规则 | 持续执行、进度报告 |

---

## 三、协作配置

```json
{
  "taskExecution": {
    "autoTriggerAfterPlanning": true,
    "planningKeywords": ["规划并执行", "分解并执行"]
  }
}
```

---

**版本**: v1.4
