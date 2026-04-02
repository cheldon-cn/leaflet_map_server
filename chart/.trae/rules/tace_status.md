# 任务状态

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、状态定义

| 状态 | 标识 | 说明 |
|------|------|------|
| 待执行 | 📋 Todo | 任务尚未开始 |
| 进行中 | 🔄 In Progress | 任务正在执行 |
| 已完成 | ✅ Done | 任务已完成并通过验收 |
| 部分完成 | ⚠️ Partial | 任务部分完成，需要继续 |
| 阻塞 | 🚫 Blocked | 任务被阻塞，需要外部支持 |
| 已取消 | ❌ Cancelled | 任务已取消 |

---

## 二、状态转换规则

```
📋 Todo → 🔄 In Progress → ✅ Done
                ↓
            ⚠️ Partial → 🔄 In Progress → ✅ Done
                ↓
            🚫 Blocked → 等待用户确认 → 🔄 In Progress
```

---

## 三、任务完成检测

```markdown
Step 1: 更新当前任务状态为 ✅ Done 或 ❌ Failed
Step 2: 扫描任务清单，识别下一个待执行任务
Step 3: 判断是否继续
  IF 存在 📋 Todo 或 ⚠️ Partial 的任务 THEN
    → 自动继续执行下一个任务
  ELSE
    → 所有任务已完成，输出完成报告
  END IF
```

---

**版本**: v1.4
