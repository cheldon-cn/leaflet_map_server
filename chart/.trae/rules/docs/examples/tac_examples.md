# 任务清单持续执行示例代码

> 本文档包含任务清单持续执行规则的示例代码

---

## 一、配置文件示例

### 1.1 完整配置文件

```json
{
  "taskExecution": {
    "triggerKeywords": [
      "按任务清单tasks.md执行",
      "按照tasks.md清单实施",
      "继续按任务清单执行",
      "实施任务清单",
      "执行任务清单"
    ],
    "limits": {
      "maxExecutionTime": "8h",
      "maxTasks": 100,
      "maxLoops": 1000,
      "maxMemory": "2GB",
      "maxConsecutiveFailures": 5,
      "maxWaitTimeForCurrentTask": "5m",
      "taskTimeout": "10m"
    },
    "concurrency": {
      "enabled": true,
      "maxConcurrent": 3,
      "resourceThreshold": {
        "maxMemory": "1GB",
        "maxCPU": "80%"
      }
    },
    "performance": {
      "enableMonitoring": true,
      "cacheTaskList": true,
      "cacheRefreshInterval": "5m",
      "collectionInterval": "10s"
    },
    "stopKeywords": {
      "stop": "停止执行",
      "cancel": "取消任务",
      "terminate": "终止",
      "pause": "暂停",
      "interrupt": "中断",
      "forceStop": "强制停止"
    },
    "paths": {
      "progressFile": ".trae/progress/task_execution_progress.json",
      "stateFile": ".trae/progress/task_states.json",
      "performanceFile": ".trae/progress/performance.json",
      "defaultTaskList": "tasks.md"
    },
    "validation": {
      "requiredFields": ["taskId", "taskName", "taskStatus", "priority"],
      "validStatuses": ["📋 Todo", "🔄 In Progress", "✅ Done", "⚠️ Partial", "🚫 Blocked", "❌ Cancelled"],
      "validPriorities": ["P0", "P1", "P2", "P3"]
    },
    "retry": {
      "maxRetries": 3,
      "retryDelay": "1s",
      "networkRetryDelay": "5s"
    },
    "reporting": {
      "outputProgressReport": true,
      "outputMilestoneReport": true,
      "outputFinalReport": true,
      "includePerformanceStats": true
    }
  }
}
```

---

## 二、任务清单格式示例

### 2.1 Markdown格式

```markdown
# 任务清单

## Overview

- **总任务数**: 48
- **已完成**: 0
- **进行中**: 0
- **待执行**: 48
- **完成进度**: 0%

---

## Milestone 1: 基础框架 (M1)

| 任务ID | 任务名称 | 状态 | 优先级 | 依赖 | 预估工时 |
|--------|----------|------|--------|------|----------|
| T1 | 几何模块核心类实现 | 📋 Todo | P0 | - | 8h |
| T2 | 坐标系统实现 | 📋 Todo | P0 | T1 | 4h |
| T3 | 空间参考系统实现 | 📋 Todo | P1 | T2 | 6h |

---

## Milestone 2: 数据访问层 (M2)

| 任务ID | 任务名称 | 状态 | 优先级 | 依赖 | 预估工时 |
|--------|----------|------|--------|------|----------|
| T4 | 数据库连接池实现 | 📋 Todo | P0 | - | 6h |
| T5 | SQL查询构建器实现 | 📋 Todo | P0 | T4 | 8h |
```

---

## 三、进度文件示例

### 3.1 执行进度文件

```json
{
  "execution": {
    "startTime": "2026-04-02T12:15:30Z",
    "stopTime": "2026-04-02T14:30:25Z",
    "stopReason": "user_request",
    "stopType": "stop",
    "totalTasks": 48,
    "completedTasks": 15,
    "currentTask": "T16",
    "currentTaskStatus": "in_progress"
  },
  "tasks": {
    "completed": ["T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10", "T11", "T12", "T13", "T14", "T15"],
    "inProgress": ["T16"],
    "pending": ["T17", "T18", "T19", "T20", "T21", "T22", "T23", "T24", "T25", "T26", "T27", "T28", "T29", "T30", "T31", "T32", "T33", "T34", "T35", "T36", "T37", "T38", "T39", "T40", "T41", "T42", "T43", "T44", "T45", "T46", "T47", "T48"]
  },
  "performance": {
    "executionTime": "2h15m",
    "averageTaskTime": "9m",
    "memoryPeak": "385MB",
    "cpuAverage": "45%",
    "successRate": 1.0
  }
}
```

---

## 四、报告输出示例

### 4.1 任务执行输出

```markdown
## 任务执行完成

### ✅ 任务ID: T1 - 几何模块核心类实现

**执行结果**:
- 状态: ✅ Done
- 工时: 实际7.5h (预估8h)
- 测试覆盖率: 85%

**完成内容**:
- [x] Geometry基类实现
- [x] Point类实现
- [x] LineString类实现
- [x] Polygon类实现
- [x] 单元测试编写

**下一步**:
- 自动检测到下一个待执行任务: T2 - 坐标系统实现
- 继续执行...
```

### 4.2 里程碑完成报告

```markdown
## 里程碑完成报告

### 里程碑 M1: 基础框架

**进度统计**:
- 已完成任务: 10/10
- 完成百分比: 100%
- 已用工时: 45h/50h

**验收标准**:
- [x] 几何模块核心类实现完成
- [x] 坐标系统实现完成
- [x] 空间参考系统实现完成
- [x] 单元测试覆盖率>80%

**下一步**:
- 继续执行里程碑M2的任务
```

### 4.3 最终完成报告

```markdown
## 任务清单执行完成报告

### 总体统计

- **总任务数**: 48
- **已完成任务**: 48 ✅
- **总工时**: 180h
- **测试覆盖率**: 85%

### 里程碑完成情况

| 里程碑 | 任务数 | 完成数 | 状态 |
|--------|--------|--------|------|
| M1 | 10 | 10 | ✅ |
| M2 | 12 | 12 | ✅ |
| M3 | 10 | 10 | ✅ |
| M4 | 8 | 8 | ✅ |
| M5 | 8 | 8 | ✅ |

### 性能统计

- **总执行时间**: 6小时30分钟
- **平均任务时间**: 8分钟
- **最长任务**: T15 - 渲染引擎实现 (25分钟)
- **最短任务**: T8 - 工具类实现 (2分钟)
- **内存峰值**: 485MB
- **CPU平均使用率**: 42%
- **任务成功率**: 100%
- **性能评分**: 92/100

### 验收结果

- [x] 所有验收标准通过
- [x] 测试覆盖率达标
- [x] 文档完整

**任务清单执行完毕！**
```

---

## 五、环境变量示例

### 5.1 设置环境变量

```bash
# 设置环境变量覆盖配置
export TASK_EXEC_MAX_TIME="4h"
export TASK_EXEC_MAX_TASKS="50"
export TASK_EXEC_MAX_CONCURRENT="5"
export TASK_EXEC_CONCURRENCY="false"

# 执行任务清单
# 将使用：最大执行时间4小时，禁用并发执行
```

---

**文档版本**: v1.0  
**最后更新**: 2026-04-02
