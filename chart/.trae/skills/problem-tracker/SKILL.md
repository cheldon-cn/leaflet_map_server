---
name: "problem-tracker"
description: "Tracks and documents problems encountered during development processes (compile, test, etc.). Invoke when user wants to document issues/problems from any development process."
---

# Problem Tracker

This skill helps track and document problems encountered during development processes like compilation, testing, deployment, etc.

## Quick Start (1 Minute)

**Basic Usage:**
```
> 整理编译测试过程中遇到的问题
```

**That's it!** The system will:
1. Analyze conversation for problems
2. Categorize by type
3. Generate `{process}_problem_summary.md`

**Quick Output Example:**
```markdown
## 问题汇总
| # | 问题 | 分类 | 状态 |
|---|------|------|------|
| 1 | C++17语法错误 | 语言标准兼容性 | ✅ |
| 2 | 头文件缺失 | 头文件管理 | ✅ |
```

## When to Use

Invoke this skill when:
- User wants to document compile/test problems
- User asks to track issues from a development process
- User mentions "problem summary", "issue tracking", or similar
- User wants to create a problem documentation file

## Usage

### Input

| 模式 | 说明 | 使用场景 |
|------|------|----------|
| **自动模式** | Skill自动分析当前对话历史中的问题 | 默认模式，无需额外输入 |
| **手动模式** | 用户明确指定问题列表 | 需要自定义问题范围时 |

### Basic Usage (Default: Compile + Test)

```
User: "整理编译测试过程中遇到的问题"
User: "记录下编译和测试的问题"
```

This will:
1. Analyze the conversation history for compile/test problems
2. Create a summary document named `compile_test_problem_summary.md`

### Custom Process

```
User: "整理部署过程中遇到的问题到deploy_problem_summary.md"
User: "记录代码审查过程中的问题"
```

Specify the process name to customize the scope.

## Output File Naming

The output file follows this pattern:
- `{process_name}_problem_summary.md`
- Default: `compile_test_problem_summary.md`
- Examples:
  - `deploy_problem_summary.md`
  - `code_review_problem_summary.md`
  - `build_problem_summary.md`

## Document Structure

The generated document includes:

### 1. Overview
- Brief description of the process
- Total problem count
- Resolution status summary

### 2. Problem Summary Table
| # | Problem | Category | Status |
|---|---------|----------|--------|
| 1 | Description | Type | ✅/⏳ |

### 3. Detailed Problem Descriptions

Each problem includes:
- **问题描述**: What happened
- **问题分类**: Type of issue (compile error, runtime error, logic error, etc.)
- **错误位置**: File and line number
- **错误信息**: Actual error message
- **原因分析**: Root cause
- **解决方法**: How to fix it
- **解决状态**: ✅ Resolved / ⏳ Pending / ❌ Failed
- **代码变化**: Before/After code comparison

### 4. Statistics
- Problem count by status
- Problem count by category

### 5. Experience Summary
- Lessons learned
- Best practices discovered

### 6. TODO List
- Pending items
- Follow-up actions

## Problem Categories

Common categories include:
- **语言标准兼容性** (Language Standard Compatibility)
- **头文件管理** (Header Management)
- **类型定义** (Type Definition)
- **类继承/访问控制** (Inheritance/Access Control)
- **const正确性** (Const Correctness)
- **函数实现缺失** (Missing Implementation)
- **返回类型不匹配** (Return Type Mismatch)
- **智能指针转换** (Smart Pointer Conversion)
- **模板编程** (Template Programming)
- **跨平台兼容性** (Cross-platform Compatibility)

## Resolution Status

| Status | Symbol | Description |
|--------|--------|-------------|
| Resolved | ✅ | Problem fixed successfully |
| Pending | ⏳ | Solution identified, not yet applied |
| Failed | ❌ | Solution did not work |
| In Progress | 🔄 | Currently being worked on |

## Example Output

```markdown
# 编译与测试问题记录

## 概述
本文档记录了编译和测试过程中遇到的15个问题...

## 问题汇总

| # | 问题 | 分类 | 状态 |
|---|------|------|------|
| 1 | C++17结构化绑定语法错误 | 语言标准兼容性 | ✅ |
| 2 | 模板辅助函数实例化错误 | 模板编程 | ✅ |
...

## 详细问题描述

### 1. C++17结构化绑定语法错误

| 项目 | 内容 |
|------|------|
| **问题描述** | 使用了C++17的结构化绑定语法... |
...
```

## Workflow

1. **Analyze**: Scan conversation history for problems
   - If no problems found: Return message "未发现问题，无需生成问题记录文档"
2. **Categorize**: Group problems by type
3. **Document**: Create structured markdown file
4. **Summarize**: Add statistics and lessons learned
5. **Save**: Write to `{process}_problem_summary.md`
   - If directory doesn't exist: Create directory first
   - If write fails: Return error "无法写入文件，请检查目录权限"
   - If file exists: Append timestamp to filename

## Constraints

**❌ 不要做的事**:
1. 不要删除或修改已生成的问题记录文档
2. 不要忽略未解决的问题
3. 不要在问题记录中添加无关内容
4. 不要跳过问题分类步骤
5. 不要遗漏问题的代码变化对比

**✅ 要做的事**:
1. 及时更新问题状态
2. 记录详细的解决方法
3. 包含代码变化前后对比
4. 定期回顾未解决问题
5. 添加经验总结和最佳实践

## Notes

- The skill automatically detects the process type from context
- Problems are extracted from error messages, code changes, and discussions
- The document is written in the same language as the user's conversation
- File location defaults to `doc/` directory in the project root

## Version History

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.1 | 2026-03-20 | 添加Quick Start、Constraints、Input说明，完善错误处理 |
| v1.0 | 2026-03-20 | 初始版本，支持编译测试问题追踪 |
