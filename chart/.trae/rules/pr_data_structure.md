# 数据结构与算法避坑清单

> **主规则**: @.trae/rules/project_rules.md

---

## 一、几何类使用

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| GEO-01 | Polygon环使用LinearRing | 而非LineString |
| GEO-02 | Envelope参数顺序 | `(min_x, min_y, max_x, max_y)` |
| GEO-03 | 空几何检查 | 允许空几何序列化，不要直接拒绝 |

---

## 二、数据初始化

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| INIT-01 | 成员变量在初始化列表中初始化 | 避免未初始化 |
| INIT-02 | 布尔标志初始化 | 如`m_valid(false)` |
| INIT-03 | 指针初始化为nullptr | 避免野指针 |

---

## 三、常见错误

| 错误类型 | 原因 | 解决方法 |
|----------|------|----------|
| 几何类型错误 | Polygon环使用了LineString | 改用LinearRing |
| 参数顺序错误 | Envelope参数顺序错误 | 使用`(min_x, min_y, max_x, max_y)` |
| 空指针崩溃 | 未初始化或所有权问题 | 检查INIT-03, PTR-05 |

---

**版本**: v3.0
