---
name: "include-index-generator"
description: "Generates INDEX.md for include directories with header files, classes, and relationships. Invoke when user asks to create documentation for C++/C header files or needs to understand module structure."
---

# Include Index Generator

## Overview

This skill generates a comprehensive index file for C/C++ include directories, helping AI models and developers quickly understand the module structure, header files, classes, and their relationships.

## Purpose

索引文件的核心目的是**辅助大模型快速查找所需模块、类、接口、文件等**，提供结构化的模块导航入口，减少大模型在代码库中搜索的时间，提高上下文理解效率。

## When to Invoke

Invoke this skill when:
- User asks to generate documentation for an include directory
- User needs to understand the structure of a C++/C module
- User wants to create an index for header files to help AI models
- User mentions "生成索引md", "头文件索引", "include index", or similar terms
- User needs module-level documentation for code navigation

## Required Parameters

The user must provide:
- **include_directory**: Absolute path to the include directory (e.g., `e:\program\trae\chart\code\geom\include`)

Optional parameters (ask if needed):
- **output_filename**: Name of the output file (default: `index_[modulename].md`, e.g., `index_geom.md`)
- **module_name**: Name of the module for documentation title (default: derived from directory name)
- **language**: Documentation language - "en" or "zh" (default: "zh" if user uses Chinese)

## Workflow

### Step 1: Discover Header Files

Use `Glob` tool to find all header files in the specified directory:
```bash
Glob(path="<include_directory>", pattern="**/*.h")
Glob(path="<include_directory>", pattern="**/*.hpp")
Glob(path="<include_directory>", pattern="**/*.hxx")
```

### Step 2: Read and Analyze Header Files

For each header file found:
1. Use `Read` tool to read the file content
2. Extract the following information:
   - File description (from file comments or `@brief` tags)
   - Class/struct definitions with their access modifiers
   - Inheritance relationships (base classes)
   - Template parameters
   - Key methods and member variables
   - Type aliases and typedefs
   - Enums and constants

### Step 3: Build Class Relationship Graph

Analyze the extracted information to identify:
- Inheritance hierarchy (extends/derives from)
- Composition relationships (has-a, member variables of other classes)
- Dependencies (includes other headers)
- Template specializations

### Step 4: Generate Index File

Create a comprehensive markdown file with the following **required sections**:

#### 1. 模块描述 (Module Description)
- 模块功能概述
- 模块在系统中的定位
- 主要职责和范围

#### 2. 核心特性 (Core Features)
- 列出模块的主要功能特性（5-10条）
- 使用简洁的要点形式

#### 3. 头文件清单 (Header File List)
Table format with:
- File name (with relative link)
- Description
- Core classes/structs

#### 4. 类继承关系图 (Class Inheritance Diagram)
ASCII art showing class hierarchy:
```
BaseClass (abstract)
    ├── DerivedClass1
    │       └── SubDerivedClass1
    └── DerivedClass2
```

#### 5. 依赖关系图 (Dependency Graph)
Show include dependencies between files:
```
types.h
    │
    ├──► core.h ──► interface.h
    │
    └──► utils.h ──► helper.h
```

#### 6. 文件分类 (File Classification)
Group headers by functionality:
- **Core**: 核心类和接口
- **Utils**: 工具类和辅助函数
- **Algorithms**: 算法实现
- **Data**: 数据结构和模型
- **IO**: 输入输出相关
- **Other**: 其他分类

#### 7. 关键类 (Key Classes)
For each important class:
- Class name with link to header file
- Base classes (if any)
- Key methods with signatures
- Purpose and usage notes
- Code snippet showing class structure

#### 8. 接口 (Interfaces)
- List all abstract interfaces
- Show pure virtual methods
- Describe interface purpose

#### 9. 类型定义 (Type Definitions)
- Enums with values and descriptions
- Structs and their members
- Type aliases and typedefs
- Constants

#### 10. 类详细信息 (Class Details)
Detailed documentation for key classes:
- Constructor/destructor
- Public methods with parameters
- Protected/private members (if relevant)
- Usage notes

#### 11. 使用示例 (Usage Examples)
Code snippets showing common usage patterns:
```cpp
// Example 1: Basic usage
#include "ogc/module/class.h"
using namespace ogc;

auto obj = ClassName::Create();
obj->Method();

// Example 2: Advanced usage
// ...
```

#### 12. 修改历史 (Modification History)
Version history table:
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | YYYY-MM-DD | Team | Initial version |

### Step 5: Write the File

Use `Write` tool to save the index file in the include directory.

**File Naming Convention**:
- File name must follow pattern: `index_[modulename].md`
- Examples: `index_geom.md`, `index_database.md`, `index_navi.md`
- Module name is derived from the directory name (e.g., `geom` from `code/geom/include`)

## Analysis Guidelines

### Class Extraction Pattern

Look for these patterns in header files:
```cpp
class ClassName : public BaseClass {
public:
    // methods
protected:
    // members
};

template<typename T>
class TemplateClass {
    // ...
};

struct StructName {
    // ...
};
```

### Relationship Detection

- **Inheritance**: `class Derived : public Base`
- **Composition**: Member variables of class type
- **Dependency**: `#include` directives
- **Association**: Pointer/reference member variables

### Documentation Tags

Extract information from:
- `@file` comments
- `@brief` tags
- `@class` tags
- Regular comment blocks before class definitions

## Output Format

The generated INDEX.md should:
- Use markdown formatting
- Include clickable links to header files
- Use ASCII art for diagrams
- Provide code examples in appropriate language blocks
- Be in the same language as user's request (Chinese or English)

## Example Output Structure

```markdown
# [Module Name] Module - Header Index

## 模块描述

[模块功能概述，说明模块在系统中的定位和主要职责]

## 核心特性

- 特性1：[描述]
- 特性2：[描述]
- 特性3：[描述]
- ...

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [common.h](ogc/common.h) | Common definitions | `GeomType`, `Dimension` |
| [geometry.h](ogc/geometry.h) | Geometry base class | `Geometry` |

---

## 类继承关系图

```
Geometry (abstract)
    ├── Point
    ├── LineString
    └── Polygon
```

---

## 依赖关系图

```
types.h
    │
    ├──► geometry.h ──► point.h
    │                   └──► linestring.h
    │
    └──► envelope.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | geometry.h, point.h, linestring.h |
| **Utils** | envelope.h, coordinate.h |

---

## 关键类

### Geometry (抽象基类)
**File**: [geometry.h](ogc/geometry.h)  
**Description**: 几何对象基类

```cpp
class Geometry {
public:
    virtual GeomType GetGeomType() const = 0;
    virtual Geometry* Clone() const = 0;
    virtual Envelope GetEnvelope() const = 0;
};
```

---

## 接口

### IGeometryVisitor
```cpp
virtual void Visit(const Point* point) = 0;
virtual void Visit(const LineString* line) = 0;
```

---

## 类型定义

### GeomType (几何类型枚举)
```cpp
enum class GeomType {
    kUnknown = 0,
    kPoint = 1,
    kLineString = 2,
    kPolygon = 3
};
```

---

## 使用示例

### 创建几何对象

```cpp
#include "ogc/geometry.h"
#include "ogc/point.h"

using namespace ogc;

Point* pt = Point::Create(121.5, 31.2);
Envelope env = pt->GetEnvelope();
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | Initial version |

---

**Generated**: YYYY-MM-DD  
**Module Version**: vX.X  
**C++ Standard**: C++11
```

## Error Handling

If the directory doesn't exist or contains no header files:
- Inform the user clearly
- Suggest checking the path
- Ask if they want to search in a different location

## Best Practices

1. **Read files in batches**: Use multiple `Read` calls in a single response for efficiency
2. **Prioritize important classes**: Focus on public API classes over internal helpers
3. **Keep diagrams simple**: Don't overcomplicate ASCII art
4. **Use relative links**: Make file links work within the repository
5. **Follow user's language**: Match the language of the user's request
6. **Consistent naming**: Always use `index_[modulename].md` format
7. **Complete sections**: Ensure all 12 required sections are present
8. **Code examples**: Provide practical, runnable code snippets
9. **Clear navigation**: Use consistent formatting for quick scanning by AI models

## AI Model Navigation Tips

索引文件设计要点，帮助大模型快速定位：

1. **模块描述** - 让模型快速理解模块职责
2. **核心特性** - 关键功能一目了然
3. **头文件清单** - 快速定位目标文件
4. **类继承关系图** - 理解类层次结构（头文件清单后立即展示）
5. **依赖关系图** - 理解模块内部依赖（头文件清单后立即展示）
6. **关键类** - 提供类签名和方法签名，减少搜索
7. **使用示例** - 提供可直接使用的代码模板

## Multiple Module Index Generation

当需要为多个模块生成索引时，还应生成总索引文件：

### 总索引文件命名
- 文件名: `index_all.md`
- 位置: 项目code目录下

### 总索引文件内容
1. **项目概述** - 项目整体介绍
2. **模块索引表** - 所有模块的快速导航
3. **核心类快速查找** - 跨模块类查找表
4. **接口快速查找** - 跨模块接口查找表
5. **枚举类型快速查找** - 跨模块枚举查找表
6. **模块依赖关系图** - 模块间依赖关系
7. **命名空间结构** - 命名空间层次
8. **常用操作示例** - 跨模块使用示例
9. **文件路径约定** - 项目文件组织规则

## Validation

After generating INDEX.md:
- Verify all file links are correct
- Check that class relationships are accurate
- Ensure the file is saved in the correct location
- Confirm the markdown is properly formatted
