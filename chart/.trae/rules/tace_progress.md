# 进度文件保存与恢复

> **主规则**: @.trae/rules/task_continuous_execution.md

---

## 一、进度文件位置

| 文件 | 路径 |
|------|------|
| 执行进度 | .trae/progress/task_execution_progress.json |
| 任务状态 | .trae/progress/task_states.json |

---

## 二、进度文件JSON格式

```json
{
  "execution": {
    "startTime": "2026-04-02T12:15:30Z",
    "totalTasks": 48,
    "completedTasks": 15,
    "currentTask": "T16",
    "checksum": "abc123"
  }
}
```

---

## 三、写入时机

| 时机 | 操作 | 校验 |
|------|------|------|
| 任务完成后 | 立即写入进度文件 | 计算checksum |
| 任务状态更新 | 写入task_states.json | 验证完整性 |

---

## 四、文件校验机制

| 校验项 | 方法 | 失败处理 |
|--------|------|----------|
| 完整性 | checksum校验 | 使用备份文件 |
| 格式正确性 | JSON解析验证 | 重新初始化 |

---

## 五、恢复执行流程

| 步骤 | 内容 |
|------|------|
| Step 1 | 读取进度文件，校验checksum |
| Step 2 | 验证任务清单是否变化 |
| Step 3 | 从中断任务或下一任务继续 |

---

**版本**: v1.5
