# 编码规范避坑清单

> **主规则**: @.trae/rules/project_rules.md

---

## 一、规则引用

| 文件 | 内容 |
|------|------|
| @.trae/rules/pr_std_compat.md | C++标准兼容性 |
| @.trae/rules/pr_header.md | 头文件管理 |
| @.trae/rules/pr_api_naming.md | API命名一致性 |
| @.trae/rules/pr_const.md | const正确性 |
| @.trae/rules/pr_pointer.md | 智能指针转换 |
| @.trae/rules/pr_interface_design.md | 接口设计避坑清单 |
| @.trae/rules/pr_data_structure.md | 数据结构与算法避坑清单 |
| @.trae/rules/pr_build_config.md | 构建配置避坑清单 |
| @.trae/rules/pr_checklist.md | 编码实施检查清单 |

---

## 二、高频问题Top 5

| 排名 | 问题类型 | 关键规则 |
|------|----------|----------|
| 1 | 头文件管理 | HDR-01, HDR-04 |
| 2 | API命名 | API-01, API-02 |
| 3 | const正确性 | CONST-01, CONST-02 |
| 4 | 智能指针转换 | PTR-01, PTR-02 |
| 5 | C++标准不兼容 | STD-01~05 |

---

**版本**: v3.0