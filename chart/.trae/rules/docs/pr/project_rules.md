# 项目编码规则（核心）

> **完整规则**: @.trae/rules/coding_standards.md | @.trae/rules/interface_design.md | @.trae/rules/build_config.md

---

## 一、技术约束

| 约束项 | 要求 | 规则ID |
|--------|------|--------|
| 编程语言 | C++11 | STD-01~05 |

---

## 二、高频避坑Top 5

| 排名 | 问题类型 | 关键规则 |
|------|----------|----------|
| 1 | 接口实现缺失 | INT-01, INT-03 |
| 2 | DLL导出 | DLL-01, DLL-02 |
| 3 | 头文件管理 | HDR-01, HDR-04 |
| 4 | API命名 | API-01~03 |
| 5 | const正确性 | CONST-01~03 |

---

## 三、检查清单

**编码前**: C++11标准 [STD-01~05] | API命名 [API-01~03]
**编码时**: #include完整 [HDR-01] | const正确 [CONST-01~03]
**编译前**: CMakeLists更新 | DLL导出 [DLL-01~02]

---

## 四、引用规则文件

| 文件 | 内容 |
|------|------|
| @.trae/rules/coding_standards.md | 编码规范 |
| @.trae/rules/interface_design.md | 接口设计 |
| @.trae/rules/build_config.md | 构建配置 |
| @.trae/rules/checklists.md | 检查清单 |

---

**版本**: v3.0
