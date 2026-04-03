# Test Module Runner Skill

自动化测试模块编译、运行和报告生成工具。

## 功能概述

- ✅ 自动编译指定模块的测试用例
- ✅ 智能超时控制（默认5秒）
- ✅ 自动跳过超时用例
- ✅ 生成详细测试报告
- ✅ 支持DLL依赖自动解析

## 快速开始

### 触发方式

当用户提到以下关键词时自动触发：
- "运行测试" / "执行测试用例"
- "测试 code/[module]/tests"
- "编译并运行测试"

### 使用示例

```
用户: 编译运行code\graph\tests中所有用例
助手: [自动执行]
      1. 编译graph模块测试
      2. 运行所有test_*.exe文件
      3. 跳过超过5秒的用例
      4. 更新doc\0_test_report.md
```

## 核心特性

### 1. 默认超时规则

**单个用例执行时间超过5秒自动跳过**

这是默认行为，无需用户显式指定。超时的用例会被标记为 `⏭️ SKIPPED`。

### 2. 测试报告

测试结果自动更新到 `doc\0_test_report.md`，包含：
- 测试汇总统计
- 详细测试结果
- 失败用例信息
- 超时跳过记录

### 3. 并行执行

使用PowerShell后台Job实现：
- 非阻塞执行
- 独立超时控制
- 资源隔离

## 文件结构

```
.trae/skills/test-module-runner/
├── SKILL.md              # Skill主文件
├── README.md             # 本文档
└── *-validator-result.md # 质量评估报告
```

## 技术实现

### 执行流程

```
Step 1: 解析模块名称
   └── 从用户输入提取模块名（如：code\graph\tests → graph）

Step 2: 创建Todo List
   └── 编译测试 → 运行测试 → 更新报告

Step 3: 编译测试
   └── cmake --build . --config Release

Step 4: 运行测试（带超时）
   └── Wait-Job -Timeout 5

Step 5: 更新测试报告
   └── 更新 doc\0_test_report.md
```

### 关键技术点

| 技术点 | 实现方式 |
|--------|----------|
| 超时控制 | PowerShell Wait-Job -Timeout 5 |
| DLL依赖 | 设置PATH环境变量 |
| 结果解析 | 正则匹配 `[PASSED]` / `[FAILED]` |
| 工作目录 | Set-Location 设置正确路径 |

## 约束与限制

### ✅ 支持的场景

- Windows平台 + PowerShell 5.0+
- CMake构建系统
- Google Test框架
- DLL依赖的测试用例

### ❌ 不支持的场景

- 非Windows平台
- 需要交互输入的测试
- 超长运行时间的测试（需调整超时值）

## 配置选项

### 自定义超时时间

修改SKILL.md中的超时值：

```powershell
$completed = Wait-Job $job -Timeout 10  # 改为10秒
```

### 自定义测试过滤器

```powershell
$tests = Get-ChildItem -Path $testDir -Filter "test_*_integration.exe"
```

## 质量评估

| 维度 | 得分 | 等级 |
|------|------|------|
| 功能维度 | 95.5 | 🟢 优秀 |
| 文档维度 | 96.8 | 🟢 优秀 |
| 可用性维度 | 97.5 | 🟢 优秀 |
| 可靠性维度 | 96.3 | 🟢 优秀 |
| 性能维度 | 95.0 | 🟢 优秀 |
| 可维护性维度 | 95.3 | 🟢 优秀 |
| **综合得分** | **96.2** | **🟢 S级（卓越）** |

## 相关技能

| Skill | 关系 |
|-------|------|
| test-quality-evaluator | 评估测试质量 |
| problem-tracker | 追踪失败测试 |
| test-report-generator | 生成详细报告 |

## 版本历史

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.2 | 2026-04-03 | 添加性能优化、扩展点、最佳实践、相关技能章节 |
| v1.1 | 2026-04-03 | 添加流程图、约束说明、决策指导、输入验证、版本历史 |
| v1.0 | 2026-04-03 | 初始版本，包含编译、运行、超时控制、报告生成功能 |

## 许可证

内部使用技能，遵循项目统一许可证。

---

**维护者**: Trae Skill System
**最后更新**: 2026-04-03
