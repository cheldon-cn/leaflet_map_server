# C++标准兼容性

> **主规则**: @.trae/rules/project_rules.md

---

## 一、禁止使用的特性

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| STD-01 | 禁止C++17结构化绑定 | `auto [a,b]=pair;` | `auto a=pair.first;` |
| STD-02 | 禁止C++14泛型lambda | `[](auto x){}` | `template<T> void f(T)` |
| STD-03 | 禁止C++14返回推导 | `auto f(){return 1;}` | `int f(){return 1;}` |

---

## 二、允许使用的C++11特性

| 特性 | 说明 |
|------|------|
| 智能指针 | unique_ptr, shared_ptr, weak_ptr |
| 移动语义 | std::move, std::forward |
| RAII | 资源获取即初始化 |
| 原子操作 | std::atomic |
| 强类型枚举 | enum class |

---

**版本**: v3.0