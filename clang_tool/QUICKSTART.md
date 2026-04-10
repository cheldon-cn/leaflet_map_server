# Quick Start Guide - SDK API Index and Validation Tool

## 快速开始（3步）

### Windows 用户

```cmd
# 1. 进入工具目录
cd install\bin\clang_tool

# 2. 安装依赖
pip install -r requirements.txt

# 3. 运行工具
run_tool.bat
```

### Linux/Mac 用户

```bash
# 1. 进入工具目录
cd install/bin/clang_tool

# 2. 安装依赖
pip3 install -r requirements.txt

# 3. 运行工具
chmod +x run_tool.sh
./run_tool.sh
```

## 输出文件说明

运行完成后，将在以下目录生成结果：

| 文件 | 位置 | 说明 |
|------|------|------|
| API 索引 | `index/api_index.json` | SDK 所有 API 的签名信息 |
| 使用示例 | `index/api_examples.json` | 从测试代码提取的真实使用示例 |
| 验证结果 | `reports/validation_results.json` | API 不匹配问题的详细数据 |
| 分析报告 | `reports/api_validation_report.md` | 适合大模型使用的分析报告 |

## 常用命令

```bash
# 运行完整工作流
python main.py --full

# 仅构建索引
python main.py --index-only

# 仅验证（使用现有索引）
python main.py --validate-only --skip-index

# 为特定 API 生成 Few-Shot 示例
python example_extractor.py --api ogc_coordinate_create

# 使用自定义配置
python main.py --config my_config.json --full
```

## 配置修改

编辑 `config.json` 文件可以：

- 修改 SDK 头文件目录
- 修改测试代码目录
- 调整验证规则
- 自定义报告格式

详细说明请参考 [README.md](README.md)

## 集成到开发流程

### 1. 代码审查前

```bash
# 在提交代码前运行验证
python main.py --full
# 检查报告，修复 Critical 和 Warning 问题
```

### 2. CI/CD 集成

```yaml
# 在 CI 中添加验证步骤
- name: API Validation
  run: |
    cd install/bin/clang_tool
    python main.py --full
    # 如果有 Critical 问题则失败
    python -c "import json; r=json.load(open('reports/validation_results.json')); exit(1 if r['statistics']['by_severity'].get('critical', 0) > 0 else 0)"
```

### 3. 大模型辅助修复

将生成的报告 `reports/api_validation_report.md` 提供给大模型：

```
请根据以下 API 验证报告修复代码中的问题：
[粘贴报告内容]
```

大模型会根据报告中的：
- 问题分类和优先级
- 修复建议
- 代码示例
- API 签名

进行精准修复，避免幻觉和错误调用。

## 故障排除

### 问题：找不到 libclang

```bash
# 设置 LLVM 安装路径
# Windows
set LLVM_INSTALL_DIR=C:\Program Files\LLVM

# Linux/Mac
export LLVM_INSTALL_DIR=/usr/lib/llvm-14
```

### 问题：解析失败

在 `config.json` 中添加必要的编译参数：

```json
{
  "clang": {
    "compilation_args": [
      "-std=c11",
      "-I/path/to/includes"
    ]
  }
}
```

## 获取帮助

```bash
# 查看帮助信息
python main.py --help

# 查看详细文档
# 打开 README.md
```
