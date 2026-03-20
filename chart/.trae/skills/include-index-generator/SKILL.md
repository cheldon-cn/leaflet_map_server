---
name: "include-index-generator"
description: "Generates INDEX.md for include directories with header files, classes, and relationships. Invoke when user asks to create documentation for C++/C header files or needs to understand module structure."
---

# Include Index Generator

## Overview

This skill generates a comprehensive `INDEX.md` file for C/C++ include directories, helping AI models and developers quickly understand the module structure, header files, classes, and their relationships.

## When to Invoke

Invoke this skill when:
- User asks to generate documentation for an include directory
- User needs to understand the structure of a C++/C module
- User wants to create an index for header files to help AI models
- User mentions "生成索引md", "头文件索引", "include index", or similar terms

## Required Parameters

The user must provide:
- **include_directory**: Absolute path to the include directory (e.g., `e:\program\trae\chart\code\geom\include`)

Optional parameters (ask if needed):
- **output_filename**: Name of the output file (default: `INDEX.md`)
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

### Step 4: Generate INDEX.md

Create a comprehensive markdown file with the following sections:

#### 1. Overview
- Module description
- Purpose and scope

#### 2. Header File List
Table format with:
- File name (with relative link)
- Description
- Core classes/structs

#### 3. Class Inheritance Diagram
ASCII art showing class hierarchy:
```
BaseClass
    ├── DerivedClass1
    └── DerivedClass2
```

#### 4. Core Classes Detail
For each important class:
- Class name with link to header file
- Base classes
- Key methods
- Purpose and usage

#### 5. Type Aliases
List of typedefs and using declarations

#### 6. Dependency Graph
Show include dependencies between files

#### 7. Quick Usage Examples
Code snippets showing common usage patterns

#### 8. File Classification
Group headers by functionality (Core, Utils, Algorithms, etc.)

### Step 5: Write the File

Use `Write` tool to save `INDEX.md` in the include directory.

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
# [Module Name] - Header Index

## Overview
[Brief description]

## Header File List
| File | Description | Core Classes |
|------|-------------|--------------|
| [common.h](common.h) | Common definitions | `GeomType`, `Dimension` |
| [geometry.h](geometry.h) | Geometry base class | `Geometry` |

## Class Inheritance Diagram
```
Geometry
    ├── Point
    ├── LineString
    └── Polygon
```

## Core Classes Detail
### Geometry
[Details...]

## Quick Usage Examples
```cpp
[Code examples...]
```
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

## Validation

After generating INDEX.md:
- Verify all file links are correct
- Check that class relationships are accurate
- Ensure the file is saved in the correct location
- Confirm the markdown is properly formatted
