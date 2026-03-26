# Test Report Generator

测试报告生成器 - 自动化测试报告生成与问题汇总更新工具

## 简介

Test Report Generator 是一个自动化工具，用于生成模块测试报告并维护问题汇总文档。它能够：

1. **生成测试报告**: 从 Google Test XML 输出创建结构化的 Markdown 测试报告
2. **跟踪问题**: 文档化和分类编译测试过程中遇到的问题
3. **维护历史**: 保持问题汇总文档的更新

## 快速开始

### 基本用法

```
> 生成feature模块详细测试报告
```

```
> 生成geom模块测试报告，并更新问题汇总
```

### 指定模块

```
> 生成database模块测试报告code\database\tests\test_database_report.md
```

## 触发关键词

- "测试报告"
- "test report"
- "问题汇总"
- "compile_test_problem_summary"

## 支持的模块

| 模块名 | 别名 | 测试可执行文件 |
|--------|------|----------------|
| geometry | geom | ogc_geometry_tests.exe |
| database | db | ogc_database_tests.exe |
| feature | - | ogc_feature_tests.exe |
| layer | - | ogc_layer_tests.exe |
| graph | - | ogc_graph_tests.exe |

## 工作流程

```
Step 0: 输入验证
    ├── 验证模块名称
    ├── 检查测试可执行文件
    └── 检查输出目录
        │
        ↓
Step 1: 收集测试结果
    ├── 运行测试并生成XML输出
    └── 解析XML提取统计数据
        │
        ↓
Step 2: 生成测试报告
    ├── 创建 test_<module>_report.md
    └── 填充统计信息和详情
        │
        ↓
Step 3: 更新问题汇总
    ├── 读取当前问题数量
    ├── 添加新问题描述
    └── 更新分类统计
        │
        ↓
Step 4: 验证与清理
    ├── 验证报告完整性
    └── 清理临时文件
```

## 输出文件

| 文件 | 路径 | 说明 |
|------|------|------|
| 测试报告 | `code\<module>\tests\test_<module>_report.md` | 详细测试报告 |
| 问题汇总 | `doc\compile_test_problem_summary.md` | 问题汇总文档 |

## 报告格式

支持三种报告格式：

| 格式 | 说明 |
|------|------|
| standard | 标准格式，包含所有章节（默认） |
| summary | 简要格式，仅包含统计信息 |
| detailed | 详细格式，包含代码片段和堆栈跟踪 |

## 问题分类

| 分类 | 说明 | 示例 |
|------|------|------|
| 链接错误 | LNK2005, LNK2019等 | 重复定义、未解析符号 |
| API命名 | 方法名不一致 | CreateCNFieldDefn → CNFieldDefn::Create |
| 类型转换 | 重载函数调用不明确 | int/size_t歧义 |
| DLL导出 | 缺少导出宏 | 缺少OGC_FEATURE_API |
| 内存管理 | 双重释放、内存泄漏 | delete已添加的field |
| 数据序列化 | WKB/WKT序列化错误 | ring count计算错误 |
| 逻辑错误 | 业务逻辑实现错误 | GetEnvelope返回错误 |
| 接口实现缺失 | 纯虚函数未实现 | C2259错误 |
| 头文件管理 | 缺少include | 缺少statement.h |
| const正确性 | const方法调用非const | C2678错误 |

## 执行时间预估

| 阶段 | 预估时间 |
|------|----------|
| 输入验证 | 5-10秒 |
| 收集测试结果 | 10-60秒 |
| 生成测试报告 | 30-60秒 |
| 更新问题汇总 | 20-40秒 |
| 验证与清理 | 5-10秒 |
| **总计** | **1-3分钟** |

## 相关技能

| 技能 | 关系 | 使用场景 |
|------|------|----------|
| skill-validator | 评估技能质量 | 创建/修改此技能后使用 |
| technical-reviewer | 单角色评审 | 单专家文档评审 |
| multi-role-reviewer | 多角色交叉评审 | 综合性交叉评审 |
| skill-creator | 创建新技能 | 创建类似技能时使用 |
| problem-tracker | 跟踪开发问题 | 持续问题管理 |

## 不适用场景

以下情况不应使用此技能：

- 评估技能质量 → 使用 `skill-validator`
- 评审设计文档 → 使用 `technical-reviewer` 或 `multi-role-reviewer`
- 创建新技能 → 使用 `skill-creator`
- 规划开发任务 → 使用 `task-planner`

## 最佳实践

1. **先分类问题**: 在编写详细描述前先对问题分类
2. **完整信息**: 每个问题必须包含所有必填字段
3. **代码示例**: 展示修改前后的代码变化
4. **更新统计**: 添加问题后更新问题数量和分类统计
5. **备份原文件**: 修改前保存原始文件
6. **增量编号**: 始终使用下一个可用的问题编号

## 版本历史

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.6 | 2026-03-26 | 精简"When to Invoke"章节，添加Trigger Keywords |
| v1.5 | 2026-03-26 | 添加Skill Selection Priority、Related Skills、Do NOT Invoke章节 |
| v1.4 | 2026-03-26 | 添加更多异常类型、完善恢复矩阵 |
| v1.3 | 2026-03-26 | 添加质量指标、完善验证步骤 |
| v1.2 | 2026-03-26 | 添加性能优化建议、扩展错误类型 |
| v1.1 | 2026-03-26 | 添加输入验证、错误处理、流程图 |
| v1.0 | 2026-03-26 | 初始版本 |

## 质量评估

| 指标 | 结果 |
|------|------|
| 最终评分 | 95.83/100 |
| 评审状态 | ✅ 通过 |
| 质量等级 | A+级（优秀） |

---

**文档版本**: v1.6  
**最后更新**: 2026-03-26
