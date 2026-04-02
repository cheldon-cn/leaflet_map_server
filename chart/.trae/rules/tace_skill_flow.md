# Skill协作流程

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、code-implementer协作

```
用户请求 → 本规则生效 → 调用code-implementer
    ↓
检测完成状态 → 有待执行 → 继续调用
    ↓
无待执行 → 输出完成报告
```

---

## 二、task-planner协作

```
用户请求"规划并执行" → task-planner规划
    ↓
生成tasks.md → 自动触发本规则
    ↓
持续执行直到完成
```

---

## 三、职责分工

| 组件 | 职责 |
|------|------|
| 本规则 | 持续执行、优先级排序、完成检测 |
| code-implementer | 单任务TDD实施、测试验证 |
| task-planner | 任务规划、分解、生成清单 |

---

## 四、协作配置

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
