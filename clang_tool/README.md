# SDK API Index and Validation Tool

基于 python-libclang 的 SDK API 索引构建与验证工具，用于防止大模型幻觉、调用不存在的接口、使用类型与预期不一致等问题。

## 目录

- [功能特性](#功能特性)
- [安装依赖](#安装依赖)
- [快速开始](#快速开始)
- [详细使用](#详细使用)
- [配置说明](#配置说明)
- [输出格式](#输出格式)
- [高级用法](#高级用法)
- [常见问题](#常见问题)

## 功能特性

### 1. API 索引构建

从 SDK 头文件中提取完整的 API 签名信息：

- ✅ 函数签名（返回类型、参数类型、参数名称）
- ✅ 枚举定义（枚举名称、枚举值）
- ✅ 结构体定义（字段名称、字段类型）
- ✅ 类型定义（typedef）
- ✅ 位置信息（文件路径、行号）
- ✅ 注释信息（brief comment）

### 2. API 使用示例提取

从测试代码中提取真实的 API 使用示例：

- ✅ 自动识别测试文件
- ✅ 提取函数调用上下文
- ✅ 关联测试用例名称
- ✅ 生成 Few-Shot Prompt

### 3. 静态分析验证

检测测试代码中的 API 不匹配问题：

- ✅ 函数名称不匹配
- ✅ 参数数量不匹配
- ✅ 参数类型不匹配
- ✅ 返回类型不匹配
- ✅ 缺失的 API
- ✅ 未声明的 API

### 4. 分析报告生成

生成适合大模型使用的分析报告：

- ✅ 问题分类（Critical/Warning/Info/Suggestion）
- ✅ 修复建议
- ✅ 代码示例
- ✅ 行动计划
- ✅ LLM 使用指南

## 安装依赖

### 前置要求

- Python 3.7+
- LLVM/Clang (需要 libclang)

### 安装步骤

```bash
# 1. 安装 python-libclang
pip install libclang

# 2. 验证安装
python -c "from clang.cindex import Index; print('libclang installed successfully')"

# 3. (可选) 如果系统找不到 libclang，设置环境变量
# Windows:
set LLVM_INSTALL_DIR=C:\Program Files\LLVM

# Linux/Mac:
export LLVM_INSTALL_DIR=/usr/lib/llvm-14
```

## 快速开始

### 运行完整工作流

```bash
# 进入工具目录
cd install/bin/clang_tool

# 运行完整流程（索引构建 -> 示例提取 -> 验证 -> 报告生成）
python main.py --full
```

### 分步执行

```bash
# 步骤 1: 构建 API 索引
python main.py --index-only

# 步骤 2: 提取使用示例
python example_extractor.py

# 步骤 3: 验证 API 使用
python main.py --validate-only

# 步骤 4: 生成报告
python main.py --report-only
```

## 详细使用

### 命令行参数

#### 主程序 (main.py)

```bash
python main.py [OPTIONS]

选项:
  --full, -f              运行完整工作流
  --index-only            仅构建 API 索引
  --validate-only         仅验证 API
  --report-only           仅生成报告
  
  --config, -c FILE       配置文件路径 (默认: config.json)
  --project-root, -p DIR  项目根目录 (默认: 自动检测)
  
  --skip-index            跳过索引构建 (使用现有索引)
  --skip-examples         跳过示例提取
  
  --output-index FILE     API 索引输出路径
  --output-examples FILE  示例输出路径
  --output-validation FILE 验证结果输出路径
  --output-report FILE    报告输出路径
  
  --verbose, -v           详细输出
```

#### 独立模块

```bash
# API 索引构建
python api_indexer.py --config config.json --output api_index.json

# 示例提取
python example_extractor.py --config config.json --output api_examples.json

# API 验证
python api_validator.py --config config.json --index api_index.json --output validation.json

# 报告生成
python report_generator.py --config config.json --validation validation.json
```

### 使用示例

#### 示例 1: 为特定 API 生成 Few-Shot Prompt

```bash
# 生成 ogc_coordinate_create 的使用示例
python example_extractor.py --api ogc_coordinate_create
```

输出:
```
## Few-Shot Examples for ogc_coordinate_create

### Example 1 (from test_sdk_c_api_geom)
```cpp
ogc_coordinate_t* coord = ogc_coordinate_create(1.0, 2.0);
```
Location: code/cycle/chart_c_api/tests/test_sdk_c_api_geom.cpp:23
```

#### 示例 2: 使用自定义配置

```bash
# 创建自定义配置
cp config.json my_config.json

# 编辑配置文件
# 修改 sdk.include_dirs 指向你的 SDK 头文件目录
# 修改 tests.test_dirs 指向你的测试代码目录

# 使用自定义配置运行
python main.py --config my_config.json --full
```

#### 示例 3: 增量验证

```bash
# 首次运行，构建索引
python main.py --index-only

# 后续修改测试代码后，只运行验证
python main.py --validate-only --skip-index
```

## 配置说明

### 配置文件结构 (config.json)

```json
{
  "version": "1.0.0",
  
  "sdk": {
    "include_dirs": ["install/include/ogc/capi"],
    "header_patterns": ["*.h"],
    "exclude_patterns": ["*_export.h"]
  },
  
  "tests": {
    "test_dirs": ["code/cycle/chart_c_api/tests"],
    "test_patterns": ["test_*.cpp"],
    "exclude_patterns": ["*/pending_tests/*"]
  },
  
  "index": {
    "output_dir": "install/bin/clang_tool/index",
    "output_filename": "api_index.json"
  },
  
  "validation": {
    "mismatch_criteria": {
      "function_name_mismatch": true,
      "parameter_count_mismatch": true,
      "parameter_type_mismatch": true
    },
    "type_equivalence": {
      "int32_t": ["int", "int32_t"]
    },
    "strict_mode": false
  },
  
  "report": {
    "output_dir": "install/bin/clang_tool/reports",
    "output_filename": "api_validation_report.md",
    "include_examples": true,
    "include_fix_suggestions": true
  }
}
```

### 关键配置项说明

#### SDK 配置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| `include_dirs` | SDK 头文件目录列表 | `["install/include/ogc/capi"]` |
| `header_patterns` | 头文件匹配模式 | `["*.h"]` |
| `exclude_patterns` | 排除的文件模式 | `["*_export.h"]` |

#### 测试配置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| `test_dirs` | 测试代码目录列表 | `["code/cycle/chart_c_api/tests"]` |
| `test_patterns` | 测试文件匹配模式 | `["test_*.cpp"]` |
| `exclude_patterns` | 排除的测试文件 | `["*/pending_tests/*"]` |

#### 验证配置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| `mismatch_criteria` | 不匹配判定标准 | 见配置文件 |
| `type_equivalence` | 类型等价规则 | 见配置文件 |
| `strict_mode` | 严格模式 | `false` |
| `allow_implicit_cast` | 允许隐式转换 | `true` |

## 输出格式

### API 索引格式 (api_index.json)

```json
{
  "metadata": {
    "version": "1.0.0",
    "total_functions": 150,
    "total_enums": 20,
    "total_structs": 15,
    "total_typedefs": 30
  },
  "functions": {
    "ogc_coordinate_create": {
      "name": "ogc_coordinate_create",
      "return_type": "ogc_coordinate_t*",
      "parameters": [
        {
          "name": "x",
          "type_name": "double",
          "is_pointer": false,
          "is_const": false
        },
        {
          "name": "y",
          "type_name": "double",
          "is_pointer": false,
          "is_const": false
        }
      ],
      "location": {
        "file": "install/include/ogc/capi/sdk_c_api.h",
        "line": 125,
        "column": 1
      }
    }
  }
}
```

### 验证结果格式 (validation_results.json)

```json
{
  "statistics": {
    "total_issues": 5,
    "by_severity": {
      "critical": 2,
      "warning": 3
    },
    "by_type": {
      "parameter_count_mismatch": 2,
      "parameter_type_mismatch": 3
    }
  },
  "issues": [
    {
      "mismatch_type": "parameter_count_mismatch",
      "severity": "critical",
      "api_name": "ogc_point_create",
      "location": {
        "file": "code/cycle/chart_c_api/tests/test_sdk_c_api_geom.cpp",
        "line": 45,
        "column": 5
      },
      "description": "Parameter count mismatch for 'ogc_point_create'",
      "expected": "2 parameters",
      "actual": "3 parameters",
      "suggestion": "Expected 2 parameters, got 3..."
    }
  ]
}
```

### 分析报告格式 (api_validation_report.md)

报告包含以下部分：

1. **执行摘要** - 问题统计概览
2. **Critical 问题** - 必须立即修复的问题
3. **Warning 问题** - 应在发布前修复的问题
4. **Info 问题** - 信息性发现
5. **Suggestion** - 最佳实践建议
6. **行动计划** - 分阶段修复建议
7. **LLM 使用指南** - 给大模型的使用说明

## 高级用法

### 集成到 CI/CD 流程

```yaml
# .github/workflows/api-validation.yml
name: API Validation

on: [push, pull_request]

jobs:
  validate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Setup Python
        uses: actions/setup-python@v2
        with:
          python-version: '3.9'
      
      - name: Install dependencies
        run: pip install libclang
      
      - name: Run API validation
        run: |
          cd install/bin/clang_tool
          python main.py --full
      
      - name: Upload report
        uses: actions/upload-artifact@v2
        with:
          name: api-validation-report
          path: install/bin/clang_tool/reports/api_validation_report.md
```

### 自定义验证规则

```python
# custom_validator.py
from api_validator import APIValidator, MismatchIssue, MismatchSeverity

class CustomValidator(APIValidator):
    def validate_function_call(self, func_name, call_args, file_path, line, column):
        # 调用基类验证
        issue = super().validate_function_call(func_name, call_args, file_path, line, column)
        
        # 添加自定义验证逻辑
        if func_name.startswith('ogc_internal_'):
            issue = MismatchIssue(
                mismatch_type=MismatchType.DEPRECATED_API,
                severity=MismatchSeverity.WARNING,
                api_name=func_name,
                file_path=file_path,
                line_number=line,
                column=column,
                description=f"Internal API '{func_name}' should not be used in tests",
                expected=None,
                actual=None,
                parameter_mismatches=[],
                suggestion="Use public API instead"
            )
        
        return issue
```

### 生成 Few-Shot 数据集

```python
# generate_few_shot_dataset.py
import json
from pathlib import Path

def generate_dataset(examples_file, output_file):
    with open(examples_file, 'r') as f:
        examples = json.load(f)
    
    dataset = []
    for api_name, api_examples in examples['examples'].items():
        for example in api_examples:
            dataset.append({
                'api_name': api_name,
                'code': example['code_snippet'],
                'context': example['context_before'] + '\n' + example['context_after'],
                'source': example['location']['file']
            })
    
    with open(output_file, 'w') as f:
        json.dump(dataset, f, indent=2)
    
    print(f"Generated {len(dataset)} Few-Shot examples")

if __name__ == '__main__':
    generate_dataset(
        'index/api_examples.json',
        'few_shot_dataset.json'
    )
```

## 常见问题

### Q1: 找不到 libclang

**问题**: `ImportError: libclang could not be found`

**解决方案**:
```bash
# 方法 1: 安装 LLVM
# Windows: 下载并安装 LLVM from https://releases.llvm.org/
# Linux: sudo apt-get install llvm-14
# Mac: brew install llvm

# 方法 2: 设置环境变量
export LLVM_INSTALL_DIR=/path/to/llvm

# 方法 3: 在 Python 中指定路径
import clang.cindex
clang.cindex.Config.set_library_path('/path/to/llvm/lib')
```

### Q2: 解析头文件失败

**问题**: `Parse error in header file`

**解决方案**:
```json
// 在 config.json 中添加编译参数
{
  "clang": {
    "compilation_args": [
      "-std=c11",
      "-I/path/to/additional/includes",
      "-DDEFINE_NEEDED"
    ]
  }
}
```

### Q3: 类型不匹配误报

**问题**: 类型实际上是兼容的，但被报告为不匹配

**解决方案**:
```json
// 在 config.json 中添加类型等价规则
{
  "validation": {
    "type_equivalence": {
      "int32_t": ["int", "int32_t", "__int32"],
      "const char*": ["const char*", "char*", "LPCSTR"]
    },
    "allow_implicit_cast": true
  }
}
```

### Q4: 如何排除特定文件

**解决方案**:
```json
{
  "tests": {
    "exclude_patterns": [
      "*/pending_tests/*",
      "*/legacy/*",
      "*_old.cpp"
    ]
  }
}
```

### Q5: 如何处理宏定义的 API

**问题**: 宏定义的函数无法被解析

**解决方案**:
```json
{
  "clang": {
    "parse_options": {
      "detailed_preprocessing_record": true
    }
  }
}
```

## 项目结构

```
install/bin/clang_tool/
├── config.json              # 配置文件
├── main.py                  # 主入口程序
├── api_indexer.py           # API 索引构建模块
├── example_extractor.py     # 示例提取模块
├── api_validator.py         # API 验证模块
├── report_generator.py      # 报告生成模块
├── README.md                # 本文档
├── index/                   # 索引输出目录
│   ├── api_index.json       # API 索引
│   └── api_examples.json    # API 使用示例
└── reports/                 # 报告输出目录
    ├── validation_results.json  # 验证结果
    └── api_validation_report.md # 分析报告
```

## 贡献指南

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License

## 更新日志

### v1.0.0 (2026-04-10)
- ✅ 初始版本发布
- ✅ API 索引构建功能
- ✅ 示例提取功能
- ✅ 静态分析验证功能
- ✅ 报告生成功能
