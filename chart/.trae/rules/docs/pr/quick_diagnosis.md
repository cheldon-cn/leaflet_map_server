# 快速诊断表

> **详细文档**: @doc/experience_standard.md

---

## 一、编译失败快速诊断

```
编译错误类型                    → 可能原因                    → 检查项
─────────────────────────────────────────────────────────────────────
未定义类型                      → 头文件缺失                  → HDR-01, HDR-04
无法访问private成员             → 继承访问权限问题            → DLL-06
纯虚函数未实现                  → 接口实现缺失                → INT-03
C++17语法错误                   → C++标准不兼容               → STD-01~05
找不到符号                      → 链接配置错误                → CMAKE-04
重复定义                        → 头文件重复包含或main重复    → HDR-03, TEST-01
```

---

## 二、链接失败快速诊断

```
链接错误类型                    → 可能原因                    → 检查项
─────────────────────────────────────────────────────────────────────
无法解析的外部符号              → 库未链接或路径错误          → CMAKE-02
重复定义                        → main函数重复或符号冲突      → TEST-01
找不到库文件                    → 输出目录配置错误            → CMAKE-01
DLL导出问题                     → 导出宏配置错误              → DLL-01~02
```

---

## 三、测试失败快速诊断

```
测试失败类型                    → 可能原因                    → 检查项
─────────────────────────────────────────────────────────────────────
API调用错误                     → 方法名不正确                → API-01~03
空指针崩溃                      → 未初始化或所有权问题        → INIT-03, PTR-05
逻辑错误                        → 算法实现错误                → GEO-01~03
内存泄漏                        → 所有权管理错误              → PTR-01~05
并发问题                        → 线程安全问题                → UT-05
```

---

## 四、高频问题Top 10

| 排名 | 问题类型 | 出现次数 | 关键规则 |
|------|----------|----------|----------|
| 1 | 接口实现缺失 | 10 | INT-01, INT-03 |
| 2 | DLL导出 | 9 | DLL-01, DLL-02 |
| 3 | 测试用例 | 8 | UT-01, UT-02 |
| 4 | 头文件管理 | 7 | HDR-01, HDR-04 |
| 5 | API命名 | 7 | API-01, API-02 |
| 6 | 链接错误 | 5 | CMAKE-04, TEST-01 |
| 7 | const正确性 | 4 | CONST-01, CONST-02 |
| 8 | 智能指针转换 | 4 | PTR-01, PTR-02 |
| 9 | 构建配置 | 4 | CMAKE-01, CMAKE-02 |
| 10 | 内存管理 | 3 | PTR-05 |

---

## 五、规则ID索引

| 规则ID | 规则内容 | 文件 |
|--------|----------|------|
| STD-01~05 | C++标准兼容性 | @.trae/rules/coding_standards.md |
| HDR-01~04 | 头文件管理 | @.trae/rules/coding_standards.md |
| API-01~03 | API命名一致性 | @.trae/rules/coding_standards.md |
| CONST-01~03 | const正确性 | @.trae/rules/coding_standards.md |
| PTR-01~05 | 智能指针与类型转换 | @.trae/rules/coding_standards.md |
| INT-01~03 | 纯虚接口类设计 | @.trae/rules/interface_design.md |
| DLL-01~04 | DLL导出配置 | @.trae/rules/interface_design.md |
| CMAKE-01~04 | CMake配置 | @.trae/rules/build_config.md |
| TEST-01, UT-01~05 | 测试配置 | @.trae/rules/build_config.md |
| GEO-01~03 | 几何类使用 | @.trae/rules/coding_standards.md |
| INIT-01~03 | 数据初始化 | @.trae/rules/coding_standards.md |
