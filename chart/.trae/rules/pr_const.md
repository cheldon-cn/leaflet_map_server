# const正确性

> **主规则**: @.trae/rules/project_rules.md

---

## 一、const规则

| 规则ID | 规则内容 | 解决方法 |
|--------|----------|----------|
| CONST-01 | const方法只能调用const方法 | 确保被调方法是const |
| CONST-02 | const方法中修改成员需mutable | `mutable std::map<...> m_cache;` |
| CONST-03 | 使用find替代operator[] | `auto it = map.find(key);` |

---

## 二、常见错误

| 错误类型 | 原因 | 解决方法 |
|----------|------|----------|
| const方法调用非const方法 | 被调方法未声明const | 添加const修饰符 |
| 修改const对象成员 | 成员变量未声明mutable | 添加mutable关键字 |
| operator[]修改map | operator[]会插入元素 | 使用find方法 |

---

**版本**: v3.0