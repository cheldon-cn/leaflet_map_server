---
name: "test-report-generator"
description: "Generates detailed test reports and updates problem summary documentation. Invoke when user asks to generate test report, document test results, or update compile_test_problem_summary.md."
---

# Test Report Generator

## Overview

This skill automates the generation of detailed test reports for modules and maintains a comprehensive problem summary document. It is designed to:

1. **Generate Test Reports**: Create structured markdown reports from Google Test XML output
2. **Track Problems**: Document and categorize issues encountered during compilation and testing
3. **Maintain History**: Keep the problem summary document up-to-date with new findings

### Key Features
- Automated test result collection and parsing
- Structured report generation with statistics and details
- Problem classification and categorization
- Consistent documentation format
- Cross-platform support (Windows/Linux/macOS)
- Incremental problem numbering
- Automatic statistics update
- Quality metrics tracking

### Target Users
- Developers running module tests
- QA engineers documenting test results
- Project managers tracking quality metrics

### Quality Metrics
| 指标 | 目标值 | 测量方法 |
|------|--------|----------|
| 报告完整性 | 100%（所有必填字段） | 自动检查所有必填字段存在 |
| 问题分类准确率 | ≥95% | 人工审核分类正确性 |
| 统计数据准确率 | 100% | 自动验证数学计算 |
| 错误恢复成功率 | ≥90% | 记录恢复操作成功率 |
| 执行时间 | <30秒 | 计时测量 |

---

## When to Invoke

Invoke this skill when:
- User asks to generate a test report for a module
- User wants to document test results
- User needs to update compile_test_problem_summary.md with new issues

**Trigger Keywords**: "测试报告", "test report", "问题汇总", "compile_test_problem_summary"

### Do NOT Invoke

Do NOT invoke this skill when:
- User asks to evaluate skill quality → Use `skill-validator` instead
- User asks to review design documents → Use `technical-reviewer` or `multi-role-reviewer` instead
- User asks to create a new skill → Use `skill-creator` instead
- User asks to plan development tasks → Use `task-planner` instead

---

## Skill Selection Priority

When user request matches multiple skills, use this priority order:

| Priority | Skill | Trigger Keywords | When to Use |
|----------|-------|------------------|-------------|
| 1 | **User-specified** | User explicitly names a skill | Always honor user's explicit choice |
| 2 | **test-report-generator** | "测试报告", "test report", "问题汇总", "compile_test_problem_summary" | When generating test reports or updating problem summary |
| 3 | **skill-validator** | "评估skill质量", "skill quality", "验证skill" | When evaluating skill quality |
| 4 | **technical-reviewer** | "评审", "审计", "review", "audit" (single role) | When single expert review is needed |

### Decision Flow

```
User Request: "生成测试报告"
       │
       ▼
┌─────────────────────────────────────┐
│ Contains "测试报告" or "test report"?│
├─────────────────────────────────────┤
│         │                           │
│    YES  │   NO                      │
│         │                           │
│    ▼    │    ▼                      │
│ test-   │  Check other keywords     │
│ report- │  (skill quality, review)  │
│ generator│                          │
└─────────────────────────────────────┘
```

---

## Related Skills

| Skill | Relationship | When to Use Together |
|-------|--------------|---------------------|
| `skill-validator` | Evaluates skill quality | Use after creating/modifying this skill |
| `technical-reviewer` | Reviews design documents | Use for single-expert document review |
| `multi-role-reviewer` | Multi-perspective review | Use for comprehensive cross-review |
| `skill-creator` | Creates new skills | Use when creating similar skills |
| `problem-tracker` | Tracks development problems | Use for ongoing problem management |

---

## Input Requirements

| 参数 | 格式 | 示例 | 必填 | 说明 |
|------|------|------|------|------|
| 模块名称 | 小写字母，下划线分隔 | `feature`, `geom_layer` | ✅ | 模块目录名 |
| 测试可执行文件 | `ogc_<module>_tests.exe` | `ogc_feature_tests.exe` | 自动 | 自动推断 |
| 报告路径 | `code\<module>\tests\test_<module>_report.md` | `code\feature\tests\test_feature_report.md` | 自动 | 固定格式 |
| 报告格式 | `standard`/`summary`/`detailed` | `standard` | 否 | 默认standard |

**Valid Module Names**:
- `geometry` / `geom`
- `database` / `db`
- `feature`
- `layer`
- `graph`

---

## Workflow

```
┌─────────────────────────────────────────────────────────────┐
│              Test Report Generator Flow                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 0: Input Validation                                   │
│  ├── Verify module name is valid                            │
│  ├── Check test executable exists                           │
│  ├── Verify output directories exist                        │
│  └── Check disk space availability                          │
│           │                                                 │
│           ↓ [Validation Failed → Exit with Error]           │
│           │                                                 │
│  Step 1: Collect Test Results                               │
│  ├── Run module tests with XML output                       │
│  ├── Parse XML test results file                            │
│  └── Extract test statistics                                │
│           │                                                 │
│           ↓ [Test Failed → Step 2.5]                        │
│           │                                                 │
│  Step 2: Generate Test Report                               │
│  ├── Create test_<module>_report.md                         │
│  ├── Fill in statistics and details                         │
│  └── Generate test suite details                            │
│           │                                                 │
│           ↓                                                 │
│  Step 2.5: Handle Test Failures (if any)                    │
│  ├── Analyze failure reasons                                │
│  ├── Categorize failure types                               │
│  └── Document issues                                        │
│           │                                                 │
│           ↓                                                 │
│  Step 3: Update Problem Summary                             │
│  ├── Read current problem count                             │
│  ├── Add new problems to summary table                      │
│  ├── Add detailed problem descriptions                      │
│  ├── Update classification statistics                       │
│  └── Update test results summary                            │
│           │                                                 │
│           ↓                                                 │
│  Step 4: Validation & Cleanup                               │
│  ├── Verify report completeness                             │
│  ├── Verify statistics accuracy                             │
│  ├── Clean up temporary files                               │
│  └── Generate summary output                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Step 0: Input Validation

Before starting the workflow, validate:

| 检查项 | 验证方法 | 失败处理 | 严重性 |
|--------|----------|----------|--------|
| 模块名称 | 检查`code\<module>`目录存在 | 提示有效模块名 | 🔴 阻塞 |
| 测试可执行文件 | 检查`ogc_<module>_tests.exe`存在 | 提示编译 | 🔴 阻塞 |
| 输出目录 | 检查`code\<module>\tests`存在 | 创建目录 | 🟡 警告 |
| 问题汇总文件 | 检查`doc\compile_test_problem_summary.md`存在 | 提示路径错误 | 🔴 阻塞 |
| 磁盘空间 | 检查可用空间 > 10MB | 提示清理磁盘 | 🟡 警告 |
| 文件权限 | 检查读写权限 | 提示权限问题 | 🔴 阻塞 |

**Validation Code Example**:
```bash
# Windows PowerShell
function Validate-Inputs {
    param([string]$module)
    
    $errors = @()
    $warnings = @()
    
    # Critical checks
    if (-not (Test-Path "code\$module")) {
        $errors += "❌ Module '$module' not found. Valid modules: feature, geom, database, layer, graph"
    }
    
    if (-not (Test-Path "code\build\test\ogc_${module}_tests.exe")) {
        $errors += "❌ Test executable not found. Run: cmake --build . --config Release"
    }
    
    if (-not (Test-Path "doc\compile_test_problem_summary.md")) {
        $errors += "❌ Problem summary file not found at doc\compile_test_problem_summary.md"
    }
    
    # Warning checks
    if (-not (Test-Path "code\$module\tests")) {
        $warnings += "⚠️ Output directory will be created: code\$module\tests"
    }
    
    $diskSpace = (Get-PSDrive C).Free / 1MB
    if ($diskSpace -lt 10) {
        $warnings += "⚠️ Low disk space: $([math]::Round($diskSpace, 2))MB available"
    }
    
    return @{ Errors = $errors; Warnings = $warnings }
}

# Linux/macOS Bash
validate_inputs() {
    local module=$1
    local errors=()
    local warnings=()
    
    # Critical checks
    if [ ! -d "code/$module" ]; then
        errors+=("❌ Module '$module' not found. Valid modules: feature, geom, database, layer, graph")
    fi
    
    if [ ! -f "code/build/test/ogc_${module}_tests" ]; then
        errors+=("❌ Test executable not found. Run: cmake --build .")
    fi
    
    if [ ! -f "doc/compile_test_problem_summary.md" ]; then
        errors+=("❌ Problem summary file not found at doc/compile_test_problem_summary.md")
    fi
    
    # Warning checks
    if [ ! -d "code/$module/tests" ]; then
        warnings+=("⚠️ Output directory will be created: code/$module/tests")
    fi
    
    local disk_space=$(df -m . | awk 'NR==2 {print $4}')
    if [ "$disk_space" -lt 10 ]; then
        warnings+=("⚠️ Low disk space: ${disk_space}MB available")
    fi
    
    echo "ERRORS: ${errors[@]}"
    echo "WARNINGS: ${warnings[@]}"
}
```

### Step 1: Collect Test Results

1. Run the module tests with XML output:

   **Windows (PowerShell)**:
   ```bash
   cd code/build
   .\test\ogc_<module>_tests.exe --gtest_output=xml:test_<module>_results.xml
   ```

   **Linux/macOS**:
   ```bash
   cd code/build
   ./test/ogc_<module>_tests --gtest_output=xml:test_<module>_results.xml
   ```

2. Parse the XML test results file to extract:
   - Total test count
   - Passed/Failed/Disabled/Error counts
   - Execution time
   - Individual test suite results
   - Failure details (if any)

**Error Handling**:
| 错误 | 处理方法 | 恢复策略 |
|------|----------|----------|
| 测试可执行文件不存在 | 验证模块名和编译配置 | 提示用户编译 |
| XML解析失败 | 检查测试是否成功运行 | 重新运行测试 |
| 无测试结果 | 确保模块有测试文件配置 | 检查CMakeLists.txt |
| 测试超时 | 设置合理超时时间 | 报告超时错误 |

### Step 2: Generate Test Report

Create the test report at `code\<module>\tests\test_<module>_report.md` with the following structure:

```markdown
# <Module>模块测试报告

## 概述
- 生成时间
- 测试框架
- 测试结果摘要

## 测试统计
- 测试套件总数
- 测试用例总数
- 通过/失败/禁用/错误数
- 总执行时间
- 通过率

## 测试套件详情
For each test suite:
- 测试用例数
- 通过数
- 执行时间
- 测试文件
- 测试用例列表（名称、状态、执行时间）
- 测试覆盖要点

## 测试文件清单
- 文件名
- 测试套件
- 用例数
- 说明

## 测试覆盖分析
### 功能覆盖
### 边界条件覆盖

## 测试环境

## 结论

## 附录：修复的问题
```

### Step 2.5: Handle Test Failures (if any)

If tests fail:
1. Analyze failure reasons from XML output
2. Categorize failures by type (compilation, runtime, assertion)
3. Document issues in problem summary
4. Optionally: Fix issues and re-run tests
5. Continue with report generation including failure details

**Failure Analysis Template**:
```markdown
## 失败用例分析

| 用例名称 | 失败原因 | 错误信息 | 建议修复 |
|----------|----------|----------|----------|
| <TestCase> | <Reason> | <Error> | <Fix> |
```

**Failure Categories**:
| 类别 | 说明 | 典型错误 | 优先级 |
|------|------|----------|--------|
| 断言失败 | EXPECT/ASSERT条件不满足 | Expected: 1, Actual: 0 | P2 |
| 异常抛出 | 未捕获的异常 | std::runtime_error | P1 |
| 超时 | 测试执行时间超过限制 | Test timeout exceeded | P2 |
| 段错误 | 内存访问错误 | SEH exception 0xc0000005 | P0 |

### Step 3: Update Problem Summary

Update `doc\compile_test_problem_summary.md`:

1. **Read Current State**:
   - Parse existing problem count
   - Identify last problem number
   - Extract current statistics

2. **Update Overview Section**:
   - Increment problem count
   - Update test results for the module

3. **Add New Problems to Problem Summary Table**:
   ```markdown
   | # | 问题 | 分类 | 状态 |
   |---|------|------|------|
   | N | <问题描述> | <分类> | ✅ |
   ```

4. **Add Detailed Problem Descriptions** (one section per problem):
   ```markdown
   ### N. <问题标题>

   | 项目 | 内容 |
   |------|------|
   | **问题描述** | <详细描述> |
   | **问题分类** | <分类> |
   | **错误位置** | <文件路径> |
   | **错误信息** | <编译器/运行时错误> |
   | **原因分析** | <根本原因> |
   | **解决方法** | <修复方法> |
   | **解决状态** | ✅ 已解决 |

   **代码变化:**

   修改前:
   ```cpp
   <问题代码>
   ```

   修改后:
   ```cpp
   <修复后代码>
   ```
   ```

5. **Update Compilation Test Flow Summary**:
   Add a new round for the module with steps taken.

6. **Update Problem Classification Statistics**:
   Adjust counts for each problem category.

### Step 4: Validation & Cleanup

**Report Completeness Check**:
| 检查项 | 必须存在 | 验证方法 | 失败处理 |
|--------|----------|----------|----------|
| 概述章节 | ✅ | 检查标题存在 | 添加缺失章节 |
| 测试统计 | ✅ | 检查表格完整 | 重新生成统计 |
| 测试套件详情 | ✅ | 检查至少1个套件 | 从XML提取 |
| 生成时间 | ✅ | 检查日期格式 | 添加当前时间 |
| 通过率计算 | ✅ | 验证数学正确性 | 重新计算 |

**Statistics Accuracy Check**:
```bash
# Verify statistics
total_tests = sum(suite.test_count for suite in suites)
passed_rate = (passed_count / total_count) * 100
assert abs(passed_rate - reported_rate) < 0.01
assert total_tests == sum_of_all_suite_tests
```

**Cleanup**:
- Remove temporary XML files (optional, keep for debugging)
- Remove checkpoint files
- Archive backup files
- Generate summary output

---

## Decision Points

### Decision 1: Test Results Status

```
Test Results
    │
    ├── All Passed (100%)
    │   └── Generate success report, update statistics only
    │
    ├── Some Failed (< 100%)
    │   ├── Analyze failure types
    │   ├── Document each failure
    │   └── Generate report with failure details
    │
    └── All Failed (0%)
        ├── Check build configuration
        ├── Document critical issues
        └── Generate failure analysis report
```

### Decision 2: Problem Classification

```
New Problem Found
    │
    ├── Compilation Error
    │   └── Classify as: 链接错误/DLL导出/头文件管理
    │
    ├── Runtime Error
    │   └── Classify as: 内存管理/逻辑错误/数据序列化
    │
    └── Test Failure
        └── Classify as: 接口实现缺失/API命名/类型转换
```

### Decision 3: Report Format Selection

```
Report Format
    │
    ├── Standard Report
    │   └── Full details with all sections
    │
    ├── Summary Report
    │   └── Statistics and key findings only
    │
    └── Detailed Report
        └── Include code snippets and stack traces
```

### Decision 4: Error Recovery Strategy

```
Error Occurred
    │
    ├── Recoverable (Step 1, 2)
    │   ├── Use cached data
    │   ├── Retry operation
    │   └── Continue with partial results
    │
    └── Non-Recoverable (Step 0, 3)
        ├── Log error details
        ├── Save partial progress
        └── Exit with instructions
```

### Decision 5: Problem Numbering Strategy

```
New Problem
    │
    ├── Read last problem number
    │
    ├── Increment by 1
    │
    ├── Verify number not in use
    │
    └── Assign to new problem
```

---

## Constraints

### ❌ Do NOT

1. **Do NOT skip test execution**: Always run tests before generating report
2. **Do NOT modify existing problem numbers**: Only add new problems with incremental numbers
3. **Do NOT delete problem descriptions**: Keep historical records intact
4. **Do NOT change template structure**: Maintain consistent report format
5. **Do NOT ignore test failures**: Document all failures in problem summary
6. **Do NOT overwrite existing reports without backup**: Save previous reports
7. **Do NOT skip validation steps**: Always validate inputs before processing
8. **Do NOT modify problem numbers**: Numbers must remain sequential and immutable
9. **Do NOT skip statistics update**: Always update counts after adding problems
10. **Do NOT use inconsistent formatting**: Follow templates exactly
11. **Do NOT skip backup creation**: Always backup before modifications
12. **Do NOT ignore warnings**: Address all warnings before proceeding

### ✅ Do

1. **Always verify module name**: Ensure correct module path before starting
2. **Always check XML output**: Verify test results are valid before parsing
3. **Always categorize problems**: Use the defined classification categories
4. **Always update statistics**: Keep problem counts accurate
5. **Always include code examples**: Show before/after changes for clarity
6. **Always backup before modifications**: Keep original files safe
7. **Always validate output**: Check generated reports for completeness
8. **Always use incremental numbering**: New problems get next available number
9. **Always preserve history**: Never delete or modify existing content
10. **Always follow templates**: Use provided templates for consistency
11. **Always log operations**: Keep track of all changes made
12. **Always verify disk space**: Ensure sufficient space before operations

---

## Exception Handling

### Exception Types

| 异常类型 | 触发条件 | 处理方法 | 严重性 | 恢复策略 |
|----------|----------|----------|--------|----------|
| `FileNotFoundException` | 测试可执行文件不存在 | 提示用户检查模块名称和编译状态 | 🔴 阻塞 | 手动恢复 |
| `XmlParseException` | XML格式错误 | 提示用户重新运行测试 | 🟡 可恢复 | 自动重试 |
| `IOException` | 文件读写失败 | 提示用户检查文件权限 | 🔴 阻塞 | 恢复备份 |
| `InvalidModuleException` | 模块名称无效 | 列出有效模块名称 | 🔴 阻塞 | 手动恢复 |
| `PermissionDeniedException` | 文件被占用 | 提示关闭编辑器后重试 | 🟡 可恢复 | 自动重试 |
| `DiskFullException` | 磁盘空间不足 | 提示清理磁盘空间 | 🔴 阻塞 | 恢复备份 |
| `StatisticsMismatchException` | 统计数据不一致 | 重新计算统计数据 | 🟡 可恢复 | 自动重算 |
| `TemplateNotFoundException` | 模板文件缺失 | 使用内置默认模板 | 🟢 忽略 | 使用默认 |
| `BackupFailedException` | 备份创建失败 | 警告用户，继续操作 | 🟡 警告 | 继续执行 |
| `EncodingException` | 文件编码错误 | 转换为UTF-8编码 | 🟡 可恢复 | 自动转换 |

### Exception Handling Flow

```
Exception Occurred
    │
    ├── Log exception details (timestamp, module, step, error)
    │
    ├── Classify severity (Blocking/Recoverable/Ignorable)
    │
    ├── Display user-friendly message
    │
    ├── Suggest recovery action
    │
    ├── Save partial results (if any)
    │
    └── Provide manual continuation instructions
```

### Exception Recovery Matrix

| 异常类型 | 自动恢复 | 手动恢复 | 回滚操作 | 重试次数 |
|----------|----------|----------|----------|----------|
| FileNotFoundException | ❌ | ✅ | 无需回滚 | 0 |
| XmlParseException | ✅ 重试 | ✅ | 无需回滚 | 3 |
| IOException | ❌ | ✅ | 恢复备份 | 0 |
| InvalidModuleException | ❌ | ✅ | 无需回滚 | 0 |
| PermissionDeniedException | ✅ 重试 | ✅ | 无需回滚 | 3 |
| DiskFullException | ❌ | ✅ | 恢复备份 | 0 |
| StatisticsMismatchException | ✅ 重算 | ✅ | 无需回滚 | 1 |
| TemplateNotFoundException | ✅ 默认 | ❌ | 无需回滚 | 0 |
| BackupFailedException | ✅ 继续 | ❌ | 无需回滚 | 0 |
| EncodingException | ✅ 转换 | ✅ | 无需回滚 | 1 |

---

## Error Recovery

### Recovery Strategies

| 错误阶段 | 恢复策略 | 自动恢复 | 回滚策略 | 最大重试 |
|----------|----------|----------|----------|----------|
| Step 0 失败 | 提示正确输入格式 | 否 | 无需回滚 | 0 |
| Step 1 失败 | 跳过Step 1，使用已有的XML文件 | 是（如果XML存在） | 无需回滚 | 3 |
| Step 2 失败 | 保存部分报告，提示用户手动补充 | 部分 | 恢复备份 | 1 |
| Step 3 失败 | 保存测试报告，提示用户稍后更新问题汇总 | 否 | 恢复备份 | 0 |
| Step 4 失败 | 报告警告，不阻塞完成 | 是 | 无需回滚 | 1 |

### Partial Results Handling

- Always save progress before critical operations
- Support resuming from last successful step
- Provide manual continuation instructions
- Create checkpoint files for recovery

### Error Isolation

This skill operates in isolation and does not affect other skills:

| Isolation Aspect | Description |
|------------------|-------------|
| File System | Uses skill-specific file prefixes (`.test_report_generator_*`) |
| State Management | Independent progress tracking, no shared state |
| Error Propagation | Errors in this skill do not cascade to other skills |
| Recovery | Each skill maintains its own backup and recovery mechanism |

**Error Isolation Guarantee**:
- If this skill fails, other skills can continue to function normally
- No shared resources that could cause cascading failures
- Checkpoint files are isolated to prevent conflicts

**Checkpoint Files**:
| 文件 | 用途 | 生命周期 | 最大大小 |
|------|------|----------|----------|
| `.test_report_progress.json` | 跟踪当前步骤和部分数据 | 临时，完成后删除 | 10KB |
| `.test_report_backup.md` | 备份之前的报告（如存在） | 永久，用户决定保留 | 无限制 |
| `.test_report_error.log` | 记录错误详情 | 永久，用于调试 | 1MB |
| `.test_report_cache.xml` | 缓存XML测试结果 | 临时，报告生成后删除 | 5MB |

**Progress File Format**:
```json
{
  "version": "1.3",
  "module": "feature",
  "current_step": 2,
  "completed_steps": [0, 1],
  "timestamp": "2026-03-26T10:30:00Z",
  "test_results": {
    "total": 228,
    "passed": 228,
    "failed": 0,
    "disabled": 0,
    "errors": 0,
    "time_ms": 92
  },
  "problems_added": [],
  "errors": [],
  "warnings": []
}
```

---

## Error Message Format

### Standard Error Message

```
❌ Error: [Error Type]
   Module: [module_name]
   Step: [step_number]
   Reason: [detailed_reason]
   
   Suggested Action:
   1. [action_1]
   2. [action_2]
   
   For more help, see Troubleshooting section.
```

### Example

```
❌ Error: Test Executable Not Found
   Module: feature
   Step: 1
   Reason: ogc_feature_tests.exe does not exist in code/build/test/
   
   Suggested Action:
   1. Verify module name is correct
   2. Run cmake --build . --config Release
   3. Check if tests are enabled in CMakeLists.txt
   
   For more help, see Troubleshooting section.
```

### Warning Message Format

```
⚠️ Warning: [Warning Type]
   Module: [module_name]
   Details: [warning_details]
   
   Continuing with: [fallback_action]
```

### Success Message Format

```
✅ Success: [Operation]
   Module: [module_name]
   Output: [output_file]
   Statistics: [key_stats]
   Duration: [execution_time]
```

---

## Troubleshooting

| 错误 | 原因 | 解决方法 | 预防措施 |
|------|------|----------|----------|
| Test executable not found | 模块名称错误或未编译 | 检查模块名称，运行cmake --build | 编译前检查 |
| XML file not created | 测试未运行或失败 | 手动运行测试，检查输出 | 检查测试配置 |
| XML parsing error | 输出格式异常 | 重新运行测试，检查gtest版本 | 使用稳定版本 |
| Problem summary not found | 文件路径错误 | 检查doc目录是否存在 | 固定路径约定 |
| Permission denied | 文件被占用 | 关闭编辑器，重试 | 使用文件锁 |
| Module name invalid | 不在有效模块列表中 | 使用有效模块名 | 输入验证 |
| Disk full | 磁盘空间不足 | 清理磁盘空间后重试 | 预检查空间 |
| Encoding error | 文件编码问题 | 确保使用UTF-8编码 | 统一编码设置 |
| Statistics mismatch | 计算错误或数据不一致 | 重新运行测试 | 验证计算逻辑 |
| Template missing | 模板文件被删除 | 使用内置默认模板 | 内置模板备份 |
| Backup failed | 权限或空间问题 | 检查权限和空间 | 预检查 |
| Timeout | 测试执行超时 | 增加超时时间或优化测试 | 设置合理超时 |

---

## Problem Classification Categories

| 分类 | 说明 | 常见错误代码 | 示例 | 优先级 |
|------|------|--------------|------|--------|
| 链接错误 | LNK2005, LNK2019等链接问题 | LNK2005, LNK2019, LNK2028 | 重复定义、未解析符号 | P1 |
| API命名 | 方法名不一致、调用错误的方法 | C2039 | CreateCNFieldDefn → CNFieldDefn::Create | P2 |
| 类型转换 | 重载函数调用不明确、参数类型不匹配 | C2668, C2665 | int/size_t歧义 | P2 |
| DLL导出 | 缺少导出宏、导出宏配置错误 | C4251, LNK2019 | 缺少OGC_FEATURE_API | P1 |
| 内存管理 | 双重释放、内存泄漏、所有权问题 | SEH exception | delete已添加的field | P0 |
| 数据序列化 | WKB/WKT序列化错误 | 运行时错误 | ring count计算错误 | P1 |
| 逻辑错误 | 业务逻辑实现错误 | 测试断言失败 | GetEnvelope返回错误 | P2 |
| 接口实现缺失 | 纯虚函数未实现 | C2259 | 未实现所有纯虚函数 | P1 |
| 头文件管理 | 缺少include、前向声明问题 | C2027, C2065 | 缺少statement.h | P2 |
| const正确性 | const方法调用非const方法 | C2678 | const上下文调用非const | P2 |

---

## Complete Example

**示例数量限制**: 最多使用2-3个完整示例，避免冗余

### Example: Generate Feature Module Test Report

**User Input**:
```
生成feature模块详细测试报告code\feature\tests\test_feature_report.md，并整理编译测试feature模块时遇到的问题更新到doc\compile_test_problem_summary.md
```

**Step 0: Input Validation**
```bash
# Check module exists
if (Test-Path "code\feature") { Write-Host "✓ Module 'feature' found" }

# Check test executable exists
if (Test-Path "code\build\test\ogc_feature_tests.exe") { Write-Host "✓ Test executable found" }

# Check output directory
if (Test-Path "code\feature\tests") { Write-Host "✓ Output directory exists" }

# Check problem summary file
if (Test-Path "doc\compile_test_problem_summary.md") { Write-Host "✓ Problem summary file exists" }

# Check disk space
$diskSpace = (Get-PSDrive C).Free / 1MB
if ($diskSpace -gt 10) { Write-Host "✓ Disk space available: $([math]::Round($diskSpace, 2))MB" }
```

**Output**:
```
✓ Module 'feature' found
✓ Test executable found
✓ Output directory exists
✓ Problem summary file exists
✓ Disk space available: 50000MB
```

**Step 1: Collect Test Results**
```bash
cd code/build
.\test\ogc_feature_tests.exe --gtest_output=xml:test_feature_results.xml
```

**Output**:
```
[==========] Running 228 tests from 15 test suites.
[----------] Global test environment set-up.
[----------] 19 tests from CNFieldValue
[ RUN      ] CNFieldValue.DefaultConstructor
[       OK ] CNFieldValue.DefaultConstructor (0 ms)
...
[==========] 228 tests from 15 test suites ran. (92 ms total)
[  PASSED  ] 228 tests.
```

**Step 2: Generate Test Report**

Create `code\feature\tests\test_feature_report.md`:
```markdown
# Feature模块测试报告

## 概述

本报告记录了 `ogc_feature` 模块的单元测试执行结果和详细分析。

**生成时间**: 2026-03-26  
**测试框架**: Google Test  
**测试结果**: ✅ 228个测试全部通过（通过率100%）

---

## 测试统计

| 指标 | 数值 |
|------|------|
| 测试套件总数 | 15 |
| 测试用例总数 | 228 |
| 通过用例数 | 228 |
| 失败用例数 | 0 |
| 禁用用例数 | 0 |
| 错误用例数 | 0 |
| 总执行时间 | 92ms |
| 通过率 | 100% |
...
```

**Step 3: Update Problem Summary**

Update `doc\compile_test_problem_summary.md`:
- Read current problem count: 101
- Add new problems #102-#111
- Update problem count from 101 to 111
- Add detailed problem descriptions
- Update classification statistics

**Step 4: Validation & Cleanup**
```bash
# Verify report completeness
✓ Overview section present
✓ Test statistics complete
✓ All 15 test suites documented
✓ Statistics accuracy verified (228/228 = 100%)
✓ Problem count updated correctly (101 → 111)

# Cleanup
✓ Removed temporary XML file
✓ Removed progress checkpoint
✓ Backup saved to .test_report_backup.md
```

**Final Output**:
```
✅ Success: Test Report Generated
   Module: feature
   Output: code\feature\tests\test_feature_report.md
   Statistics: 228 tests, 100% passed
   Duration: 2.5 seconds

✅ Success: Problem Summary Updated
   Output: doc\compile_test_problem_summary.md
   Added: 10 new problems (#102-#111)
   Total: 111 problems documented
```

---

## Template: Test Report

**必填字段标记**: `*` 表示必填字段

```markdown
# <Module>模块测试报告

## 概述 *REQUIRED*

本报告记录了 `ogc_<module>` 模块的单元测试执行结果和详细分析。

**生成时间** *REQUIRED*: <date>  
**测试框架** *REQUIRED*: Google Test  
**测试结果** *REQUIRED*: ✅ <count>个测试全部通过（通过率100%）

---

## 测试统计 *REQUIRED*

| 指标 | 数值 |
|------|------|
| 测试套件总数 *REQUIRED* | <num> |
| 测试用例总数 *REQUIRED* | <num> |
| 通过用例数 *REQUIRED* | <num> |
| 失败用例数 *REQUIRED* | <num> |
| 禁用用例数 | <num> |
| 错误用例数 | <num> |
| 总执行时间 *REQUIRED* | <time>ms |
| 通过率 *REQUIRED* | <rate>% |

---

## 测试套件详情 *REQUIRED*

### 1. <TestSuiteName> 测试套件

| 项目 | 内容 |
|------|------|
| 测试用例数 *REQUIRED* | <num> |
| 通过数 *REQUIRED* | <num> |
| 执行时间 | <time>ms |
| 测试文件 | <filename> |

**测试用例列表:**

| 用例名称 | 状态 | 执行时间 |
|----------|------|----------|
| <TestCaseName> | ✅ 通过 | 0ms |

**测试覆盖要点:**
- <要点1>
- <要点2>

---

## 附录：修复的问题

在测试过程中发现并修复了以下问题：

| 问题 | 文件 | 修复方法 |
|------|------|----------|
| <问题描述> | <文件名> | <修复方法> |
```

---

## Template: Problem Description

```markdown
### <N>. <问题标题>

| 项目 | 内容 |
|------|------|
| **问题描述** | <详细描述问题现象> |
| **问题分类** | <分类名称> |
| **错误位置** | `<文件路径>` |
| **错误信息** | `<完整错误信息>` |
| **原因分析** | <分析问题根本原因> |
| **解决方法** | <描述解决方案> |
| **解决状态** | ✅ 已解决 |

**代码变化:**

修改前:
```cpp
// 问题代码
```

修改后:
```cpp
// 修复后代码
```
```

---

## Best Practices

1. **Problem Classification First**: Always categorize problems before writing detailed descriptions
2. **Complete Information**: Each problem must include all required fields
3. **Code Examples**: Show before/after code changes for clarity
4. **Consistent Format**: Use the same table format for all problem descriptions
5. **Update Statistics**: Always update the problem count and classification statistics
6. **Backup Before Changes**: Save original files before modifications
7. **Validate Output**: Check generated reports for completeness and accuracy
8. **Incremental Numbering**: Always use next available problem number
9. **Preserve History**: Never modify or delete existing problem descriptions
10. **Follow Templates**: Use provided templates for consistency
11. **Log Operations**: Keep track of all changes made for audit purposes
12. **Verify Disk Space**: Ensure sufficient space before operations
13. **Handle Encoding**: Always use UTF-8 encoding for all files
14. **Test Recovery**: Periodically test backup and recovery procedures

---

## Output Files

| 文件 | 路径 | 用途 | 保留策略 | 最大大小 |
|------|------|------|----------|----------|
| Test Report | `code\<module>\tests\test_<module>_report.md` | 测试报告 | 永久保留 | 无限制 |
| Problem Summary | `doc\compile_test_problem_summary.md` | 问题汇总 | 永久保留 | 无限制 |
| Progress Checkpoint | `.test_report_generator_progress.json` | 进度跟踪 | 完成后删除 | 10KB |
| Backup | `.test_report_generator_backup.md` | 报告备份 | 用户决定 | 无限制 |
| Error Log | `.test_report_generator_error.log` | 错误日志 | 用于调试 | 1MB |
| XML Cache | `.test_report_generator_cache.xml` | XML缓存 | 报告生成后删除 | 5MB |

---

## Time & Token Estimation

### Time Estimation

| Phase | Estimated Time | Notes |
|-------|----------------|-------|
| Step 0: Input Validation | 5-10 seconds | File system checks |
| Step 1: Collect Test Results | 10-60 seconds | Depends on test count |
| Step 2: Generate Test Report | 30-60 seconds | Report generation |
| Step 2.5: Handle Failures | 10-30 seconds | If failures exist |
| Step 3: Update Problem Summary | 20-40 seconds | Problem documentation |
| Step 4: Validation & Cleanup | 5-10 seconds | Final checks |
| **Total** | **1-3 minutes** | Typical execution |

### Token Estimation

| Component | Token Range | Notes |
|-----------|-------------|-------|
| Skill loading | 2,000-3,000 | One-time per invocation |
| Document reading | 100-500 per page | Depends on document length |
| Test execution | 500-2,000 | Test output parsing |
| Report generation | 1,000-3,000 | Depends on test count |
| Problem documentation | 500-1,500 per problem | Depends on problem count |
| **Total** | **5,000-15,000** | Typical execution |

**Token Optimization Tips**:
- Use `summary` format for quick reports
- Limit problem descriptions to essential information
- Archive old test reports to reduce file size

---

## Version History

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.6 | 2026-03-26 | 精简"When to Invoke"章节，添加Trigger Keywords，消除与description重复 |
| v1.5 | 2026-03-26 | 添加Skill Selection Priority、Related Skills、Do NOT Invoke章节；添加模板必填字段标记；添加Time & Token Estimation；添加Error Isolation说明；修复检查点文件命名冲突 |
| v1.4 | 2026-03-26 | 添加更多异常类型、完善恢复矩阵、增加性能指标、扩展最佳实践 |
| v1.3 | 2026-03-26 | 添加质量指标、完善验证步骤、扩展恢复策略、增加预防措施 |
| v1.2 | 2026-03-26 | 添加性能优化建议、扩展错误类型、完善恢复策略 |
| v1.1 | 2026-03-26 | 添加输入验证、错误处理、约束说明、流程图、完整示例 |
| v1.0 | 2026-03-26 | 初始版本，包含测试报告生成和问题汇总更新功能 |
