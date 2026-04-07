---
name: "user-manual"
description: "Generates user manuals for modules with 9 sections. Invoke when user asks to create/update module documentation or generate user_manual.md files."
---

# User Manual Generator

This skill generates comprehensive user manuals for OGC Chart modules.

## When to Use

Invoke this skill when:
- User asks to generate user manual for a module
- User requests to create `user_manual.md` in module's doc directory
- User wants to update existing user manual to match current implementation
- User mentions "用户手册" or "user manual" for any module

## Manual Structure

The generated user manual contains 9 required sections:

### 1. 模块描述 (Module Description)
- Module positioning in system architecture
- Core responsibilities and purpose
- Layer classification (Layer 1-7)

### 2. 核心特性 (Core Features)
- Key functionality list
- Feature descriptions in table format

### 3. 依赖关系 (Dependencies)
- Internal dependencies (other modules)
- External dependencies (third-party libraries)
- Dependent modules (which modules depend on this)

### 4. 目录结构 (Directory Structure)
- File organization tree
- Header files, source files, tests, docs

### 5. 关键类和主要方法 (Key Classes and Methods)
- Core class definitions with code snippets
- Important method signatures
- Type definitions and enums

### 6. 使用示例 (Usage Examples)
- Practical code examples
- Common use cases
- Best practices

### 7. 编译和集成 (Build and Integration)
- CMake configuration
- Header file includes
- Link library instructions

### 8. 注意事项 (Notes)
- Important usage guidelines
- Common pitfalls
- Performance considerations

### 9. 版本历史 (Version History)
- Version changes table
- Date and author information

## Execution Process

### Step 1: Analyze Module
1. Read module's index file: `code/{module}/include/index_{module}.md`
2. Scan header files in `code/{module}/include/ogc/{module}/`
3. Review source files in `code/{module}/src/`
4. Check existing user manual if present

### Step 2: Extract Information
1. **From index file**: Module description, features, dependencies, key classes
2. **From header files**: Class definitions, method signatures, enums
3. **From CMakeLists.txt**: Build configuration, dependencies
4. **From split_graph.md**: Architecture positioning, layer classification

### Step 3: Generate/Update Manual
1. If manual exists: Compare with current implementation
2. Update inconsistent sections
3. If manual doesn't exist: Create new file
4. Save to `code/{module}/doc/user_manual.md`

### Step 4: Validate Content
1. Ensure all 9 sections are present
2. Verify code examples compile
3. Check dependency accuracy
4. Validate class/method signatures

## Template Format

```markdown
# {module_name} 模块用户手册

> **版本**: v1.0  
> **更新日期**: {date}  
> **适用范围**: OGC Chart 系统{module_description}

---

## 一、模块描述

{module_name} 是 OGC 图表系统的**{module_type}**，位于系统架构的{layer_name}（Layer {layer_num}）。

### 1.1 模块定位

在 OGC Chart 系统架构中，{module_name} 位于{layer_name}：

```
{architecture_diagram}
```

---

## 二、核心特性

| 特性 | 说明 |
|------|------|
| {feature_1} | {description_1} |
| {feature_2} | {description_2} |

---

## 三、依赖关系

### 3.1 依赖库

| 依赖类型 | 库名称 | 说明 |
|----------|--------|------|
| 内部依赖 | {module} | {description} |
| 外部依赖 | {library} | {description} |

### 3.2 被依赖关系

{module_name} 被以下模块依赖：
- {dependent_module_1}
- {dependent_module_2}

---

## 四、目录结构

```
{module}/
├── include/ogc/{module}/
│   ├── export.h
│   └── {subdirectories}
├── src/
├── tests/
├── doc/
│   └── user_manual.md
└── CMakeLists.txt
```

---

## 五、关键类和主要方法

### 5.1 {ClassName}（{Description}）

**头文件**: `<ogc/{module}/{header}.h>`

```cpp
{class_definition}
```

---

## 六、使用示例

### 6.1 {Example Title}

```cpp
{code_example}
```

---

## 七、编译和集成

### 7.1 CMake 配置

```cmake
{cmake_config}
```

### 7.2 头文件包含

```cpp
{header_includes}
```

---

## 八、注意事项

1. {note_1}
2. {note_2}

---

## 九、版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | {date} | {change_description} |

---

**文档维护**: OGC Chart 开发团队  
**技术支持**: 参见项目 README.md
```

## Quality Standards

A high-quality user manual should:

1. **Accuracy**: All class/method signatures match actual implementation
2. **Completeness**: All 9 sections present with meaningful content
3. **Clarity**: Clear explanations with proper formatting
4. **Examples**: Working code examples that demonstrate usage
5. **Consistency**: Consistent style with other module manuals
6. **Maintainability**: Easy to update when implementation changes

## Supported Modules

| Module | Type | Layer |
|--------|------|-------|
| ogc_base | 基础工具库 | Layer 1 |
| ogc_proj | 坐标转换库 | Layer 2 |
| ogc_cache | 缓存库 | Layer 4 |
| ogc_symbology | 符号化库 | Layer 5 |
| ogc_graph | 地图渲染核心 | Layer 6 |
| ogc_geom | 几何对象模型 | Layer 2 |
| ogc_draw | 绘图引擎 | Layer 2 |
| ogc_feature | 要素模型 | Layer 3 |
| ogc_layer | 图层抽象 | Layer 3 |
| ogc_navi | 航海导航 | Layer 7 |
| ogc_alert | 航海警报 | Layer 7 |

## Example Usage

```
User: 给模块ogc_base生成用户手册
Agent: [Invokes user-manual skill]
       [Reads base module files]
       [Generates user_manual.md]
       [Saves to code/base/doc/user_manual.md]
```

## Validation Checklist

Before completing, verify:

- [ ] All 9 sections present
- [ ] Module description accurate
- [ ] Dependencies correctly listed
- [ ] Directory structure matches actual files
- [ ] Class signatures match header files
- [ ] Code examples are syntactically correct
- [ ] CMake configuration is valid
- [ ] Notes are relevant and helpful
- [ ] Version history is updated

## Error Handling

### Common Error Scenarios

| Scenario | Cause | Handling |
|----------|-------|----------|
| Module directory not found | Invalid module name or module not created | Show available modules list, suggest correct name |
| Index file missing | Module not indexed yet | Suggest running include-index-generator skill first |
| Header files empty | No headers in include directory | Report warning, generate minimal manual with template |
| Permission denied | File write access issue | Show error with full path, suggest checking permissions |
| Invalid header format | Malformed header files (missing guards, etc.) | Skip problematic files, continue with warnings |
| Class signature mismatch | Manual outdated vs current implementation | Auto-update section, log changes made |
| CMakeLists.txt missing | Module build config not found | Use default template, add note about manual review |

### Error Recovery Strategy

```
┌─────────────────────────────────────────────────────────────┐
│                  Error Recovery Flow                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Error Detected                                             │
│       ↓                                                     │
│  ┌─────────────────┐                                        │
│  │ Is it blocking? │                                        │
│  └────────┬────────┘                                        │
│           │                                                 │
│     ┌─────┴─────┐                                           │
│     ↓           ↓                                           │
│   [Yes]       [No]                                          │
│     │           │                                           │
│     ↓           ↓                                           │
│  ┌─────────┐  ┌─────────────┐                               │
│  │ Stop &  │  │ Log warning │                               │
│  │ Report  │  │ Continue    │                               │
│  │ Error   │  │ with best   │                               │
│  │         │  │ effort      │                               │
│  └─────────┘  └─────────────┘                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Error Messages

**E001 - Module Not Found**
```
Error: Module '{module_name}' not found.
Available modules: base, cache, proj, symbology, graph, geom, draw, feature, layer, navi, alert
Suggestion: Check spelling or create the module first.
```

**E002 - Index File Missing**
```
Warning: Index file not found at code/{module}/include/index_{module}.md
Suggestion: Run include-index-generator skill to create index first.
Proceeding with direct header file analysis...
```

**E003 - Write Permission Denied**
```
Error: Cannot write to code/{module}/doc/user_manual.md
Path: {full_path}
Suggestion: Check file permissions or create doc directory manually.
```

**E004 - Header Parse Error**
```
Warning: Failed to parse header file: {header_path}
Reason: {specific_error}
Skipping this file and continuing...
```

### Partial Success Handling

When some sections cannot be fully generated:

1. **Mark incomplete sections**: Add `[TODO: Manual review needed]` placeholder
2. **Log warnings**: Document what was skipped and why
3. **Provide guidance**: Suggest manual steps to complete the section
4. **Continue generation**: Don't abort entire process for partial failures

Example output with warnings:
```markdown
## 五、关键类和主要方法

### 5.1 Logger

**头文件**: `<ogc/base/log.h>`

[TODO: Manual review needed - Header file could not be fully parsed]
<!-- Warning: Missing class definition, please add manually -->
```
