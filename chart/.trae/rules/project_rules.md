# 项目编码规则（核心）

> **相关文档**: @doc/experience_standard.md | @doc/compile_test_problem_summary.md

---

## 一、技术约束（强制）

| 约束项 | 要求 | 规则ID |
|--------|------|--------|
| 编程语言 | C++11 | STD-01~05 |
| 核心框架 | 无外部框架依赖，纯C++11标准库 | - |
| 代码规范 | Google C++ Style | - |
| 外部依赖 | libpq, sqlite3, libspatialite | - |

> ⚠️ **违反以上任何一条，视为不合格代码。**

---

## 二、实施原则

- **以设计文档为准**：所有实现必须严格遵循设计文档
- **疑问处理**：如有歧义，必须查阅设计文档，不得自行猜测
- **文档优先**：当需求描述与设计文档冲突时，以设计文档为准并主动提示用户
- **规则索引**：遇到问题时，查阅 @doc/experience_standard.md

---

## 三、规则引用

| 文件 | 内容 |
|------|------|
| @.trae/rules/pr_coding_standards.md | 编码规范避坑清单 |
| @.trae/rules/pr_diagnosis.md | 快速诊断表 |
| @.trae/rules/pr_execution_flow.md | 执行流程约束 |
| @.trae/rules/pr_top_issues.md | 高频问题Top 10 |

**版本**: v3.0