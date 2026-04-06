---
name: "index-validator"
description: "Validates index files against actual code implementation. Invoke when user asks to check index quality, validate index consistency, or mentions index validation."
---

# Index Validator

## Overview

This skill validates index files (index_[modulename].md) against actual code implementation, ensuring consistency, completeness, and accuracy.

## Purpose

索引文件验证器的核心目的是**确保索引文件与实际代码实现保持一致**，检查章节完整性、接口正确性、命名准确性，并自动修复发现的问题。

## When to Invoke

Invoke this skill when:
- User asks to check index file quality
- User wants to validate index consistency with implementation
- User mentions "索引验证", "index validation", "检查索引", or similar terms
- User needs to verify index file accuracy after code changes

## Required Parameters

The user must provide:
- **module_name**: Name of the module to validate
  - Type: string
  - Example: `geom`, `database`, `feature`
  - Usage: "检查 geom 模块的索引文件质量"

Optional parameters:
- **auto_fix**: Whether to automatically fix issues found
  - Type: boolean
  - Default: `true`
  - Example: `false` (只检查不修复)
- **max_rounds**: Maximum validation rounds
  - Type: integer
  - Default: `3`
  - Example: `5` (最多5轮验证)

## Validation Checklist

### 1. 头文件完整性检查

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 文件数量对比 | 对比实际文件与索引列表 | `Glob` + `Read` |
| 文件名正确性 | 逐个对比文件名 | `Read` |
| 文件路径正确性 | 验证链接路径 | `Glob` |

**Validation Steps**:
```markdown
1. Use Glob to find all header files: `code/[module]/include/**/*.h`
2. Compare with index file's header list
3. Report missing files and extra files
```

### 2. 章节完整性检查

| 序号 | 章节名称 | 必需 |
|------|----------|------|
| 1 | 模块描述 | ✅ |
| 2 | 核心特性 | ✅ |
| 3 | 头文件清单 | ✅ |
| 4 | 类继承关系图 | ✅ |
| 5 | 依赖关系图 | ✅ |
| 6 | 文件分类 | ✅ |
| 7 | 关键类 | ✅ |
| 8 | 接口 | ✅ |
| 9 | 类型定义 | ✅ |
| 10 | 类详细信息 | ✅ |
| 11 | 使用示例 | ✅ |
| 12 | 修改历史 | ✅ |

**Validation Steps**:
```markdown
1. Read index file content
2. Check each required section exists
3. Verify section order matches template
4. Report missing or misplaced sections
```

### 3. 类继承关系验证

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 基类正确性 | 读取头文件验证继承关系 | `Read` |
| 派生类完整性 | 检查所有派生类是否列出 | `Grep` |
| 继承图准确性 | 对比实际继承与图中显示 | `Read` |

**Validation Steps**:
```markdown
1. For each class in index, read its header file
2. Extract base class from `class Derived : public Base`
3. Compare with inheritance diagram
4. Report incorrect inheritance relationships
```

### 4. 接口完整性检查

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 接口遗漏 | 对比头文件中的接口与索引 | `Read` + `Grep` |
| 接口多余 | 检查索引中是否有不存在的接口 | `Grep` |
| 方法签名正确性 | 验证参数类型和返回类型 | `Read` |

**Validation Steps**:
```markdown
1. Find all abstract classes/interfaces in headers (classes with pure virtual methods)
2. Compare with index's interface section
3. For each interface, verify method signatures match header
4. Report missing/extra interfaces and incorrect signatures
```

### 5. 类型定义验证

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 枚举值完整性 | 读取common.h验证所有值 | `Read` |
| 枚举值正确性 | 对比枚举值名称和顺序 | `Read` |
| 类型别名正确性 | 验证typedef和using语句 | `Grep` |
| 常量定义完整性 | 检查常量是否存在 | `Grep` |

**Validation Steps**:
```markdown
1. Read common.h or equivalent type definition files
2. Extract all enums, their values, and type aliases
3. Compare with index's type definitions section
4. Report missing/extra/incorrect type definitions
```

### 6. 方法签名验证

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 方法名正确性 | 对比头文件方法名 | `Read` |
| 参数类型正确性 | 验证参数列表 | `Read` |
| 返回类型正确性 | 验证返回值类型 | `Read` |
| 方法遗漏 | 检查关键方法是否列出 | `Grep` |

**Validation Steps**:
```markdown
1. For each key class in index, read its header file
2. Extract all public method signatures
3. Compare with index's class documentation
4. Report missing methods and incorrect signatures
```

### 7. 命名空间验证

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 命名空间正确性 | 检查namespace声明 | `Read` |
| 类名前缀正确性 | 验证类名是否在正确命名空间 | `Read` |

**Validation Steps**:
```markdown
1. Read header files and extract namespace declarations
2. Verify all classes in index are in correct namespace
3. Report namespace mismatches
```

### 8. 代码示例验证

| 检查项 | 方法 | 工具 |
|--------|------|------|
| 类型存在性 | 检查示例中使用的类型是否存在 | `Grep` |
| 方法存在性 | 验证调用的方法是否存在 | `Grep` |
| 语法正确性 | 检查代码语法 | Manual |

**Validation Steps**:
```markdown
1. Extract code examples from index
2. For each type used in examples, verify it exists in headers
3. For each method called, verify it exists in the class
4. Report references to non-existent types/methods
```

## Workflow

### Step 1: Initialize Validation

```markdown
1. Read the index file: `code/[module]/include/index_[module].md`
2. Read the SKILL.md template: `.trae/skills/include-index-generator/SKILL.md`
3. Initialize validation report
```

### Step 2: Header File Discovery

```markdown
1. Use Glob to find all header files in `code/[module]/include/`
2. Create a set of actual header files
3. Extract header files listed in index
4. Compare and report differences
```

### Step 3: Section Validation

```markdown
1. Parse index file for section headers (## ...)
2. Compare with required 12 sections
3. Check section order
4. Report missing/misplaced sections
```

### Step 4: Content Validation

```markdown
1. For each class in index:
   - Read corresponding header file
   - Verify base class
   - Verify method signatures
   - Check for missing methods

2. For each interface in index:
   - Verify it exists in headers
   - Verify method signatures

3. For each type definition:
   - Verify enum values
   - Verify type aliases
```

### Step 5: Generate Validation Report

```markdown
## 验证报告

### 检查摘要

| 检查项 | 状态 | 发现问题 | 修复状态 |
|--------|------|----------|----------|
| 头文件完整性 | ✅/❌ | X个问题 | ✅/❌ |
| 章节完整性 | ✅/❌ | X个问题 | ✅/❌ |
| 类继承关系 | ✅/❌ | X个问题 | ✅/❌ |
| 接口完整性 | ✅/❌ | X个问题 | ✅/❌ |
| 类型定义 | ✅/❌ | X个问题 | ✅/❌ |
| 方法签名 | ✅/❌ | X个问题 | ✅/❌ |
| 命名空间 | ✅/❌ | X个问题 | ✅/❌ |
| 代码示例 | ✅/❌ | X个问题 | ✅/❌ |

### 问题详情

#### 问题1: [问题描述]
- **位置**: [文件名:行号]
- **实际**: [实际内容]
- **期望**: [期望内容]
- **修复建议**: [建议]

### 修复操作

[列出已执行的修复操作]
```

### Step 6: Auto Fix (if enabled)

```markdown
1. For each issue found, apply appropriate fix:
   - Add missing sections
   - Correct inheritance diagrams
   - Add missing interfaces
   - Fix method signatures
   - Add missing type definitions
   - Correct code examples

2. Write updated index file

3. Re-validate to ensure fixes are correct
```

### Step 7: Multi-Round Validation

```
┌─────────────────────────────────────────────────────────────┐
│              Multi-Round Validation Flow                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────┐                                        │
│  │ 修复完成?       │                                        │
│  └────────┬────────┘                                        │
│           │                                                 │
│     ┌─────┴─────┐                                           │
│     ↓           ↓                                           │
│   [是]        [否]                                          │
│     │           │                                           │
│     ↓           ↓                                           │
│  ┌─────────┐  ┌─────────┐                                   │
│  │重新验证 │  │输出报告 │                                   │
│  └────┬────┘  └─────────┘                                   │
│       │                                                     │
│       ↓                                                     │
│  ┌─────────────────┐                                        │
│  │ 问题数=0 或     │                                        │
│  │ 达到max_rounds? │                                        │
│  └────────┬────────┘                                        │
│           │                                                 │
│     ┌─────┴─────┐                                           │
│     ↓           ↓                                           │
│   [是]        [否]                                          │
│     │           │                                           │
│     ↓           │                                           │
│  ┌─────────┐    │                                           │
│  │输出报告 │    │                                           │
│  └─────────┘    │                                           │
│                 │                                           │
│                 └──→ 返回修复                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**决策条件**:
- **停止验证**: 问题数=0 或 已达到max_rounds
- **继续验证**: 问题数>0 且 未达到max_rounds

**执行步骤**:
1. If issues were fixed, run validation again
2. Repeat until no issues found or max_rounds reached
3. Report final status

## Validation Report Template

```markdown
# Index Validation Report

**Module**: [module_name]
**Index File**: code/[module]/include/index_[module].md
**Validation Date**: YYYY-MM-DD HH:MM:SS
**Validation Round**: X/Y

---

## 检查摘要

| 检查项 | 状态 | 发现问题 | 修复状态 |
|--------|------|----------|----------|
| 头文件完整性 | ✅ | 0 | - |
| 章节完整性 | ✅ | 0 | - |
| 类继承关系 | ✅ | 0 | - |
| 接口完整性 | ✅ | 0 | - |
| 类型定义 | ✅ | 0 | - |
| 方法签名 | ✅ | 0 | - |
| 命名空间 | ✅ | 0 | - |
| 代码示例 | ✅ | 0 | - |

---

## 问题详情

### 问题1: [问题描述]
- **类型**: [头文件缺失/章节缺失/继承错误/接口遗漏/类型错误/签名错误/命名空间错误/示例错误]
- **位置**: [文件名:行号或章节名]
- **实际**: [实际内容]
- **期望**: [期望内容]
- **修复建议**: [建议]
- **修复状态**: [待修复/已修复/无需修复]

---

## 修复操作

| 序号 | 操作 | 状态 |
|------|------|------|
| 1 | [修复描述] | ✅/❌ |

---

## 验证结论

- **总问题数**: X
- **已修复**: Y
- **未修复**: Z
- **验证通过**: ✅/❌
- **建议**: [后续建议]

---

**Validator**: index-validator v1.0
```

## Error Handling

- **Index file not found**: Inform user and suggest running include-index-generator first
- **Header directory not found**: Verify module name and path
- **Parse errors**: Report specific parsing issues with line numbers
- **Fix failures**: Report what couldn't be fixed and why

## Best Practices

1. **Read files in batches**: Use multiple `Read` calls in a single response for efficiency
2. **Validate incrementally**: Check one aspect at a time for clearer error reporting
3. **Preserve formatting**: When fixing, maintain original markdown formatting
4. **Report progress**: Update user on validation progress for large modules
5. **Multi-round validation**: Always re-validate after fixes to ensure correctness

## Integration with Other Skills

- **include-index-generator**: Use to regenerate index if validation fails critically
- **skill-validator**: Use to validate this skill's quality

## Example Usage

```markdown
User: 检查 geom 模块的索引文件质量

Assistant:
1. 读取 index_geom.md
2. 发现所有头文件
3. 执行8项检查
4. 生成验证报告
5. 自动修复问题
6. 多轮验证直到通过
```

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2026-04-06 | Initial version |
