# 编码规范避坑清单

> **详细文档**: @doc/experience_standard.md

---

## 一、C++标准兼容性 ⚠️ 高频

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| STD-01 | 禁止C++17结构化绑定 | `auto [a,b]=pair;` | `auto a=pair.first;` |
| STD-02 | 禁止C++14泛型lambda | `[](auto x){}` | `template<T> void f(T)` |
| STD-03 | 禁止C++14返回推导 | `auto f(){return 1;}` | `int f(){return 1;}` |

---

## 二、头文件管理 ⚠️ 高频

| 规则ID | 规则内容 | 常见遗漏 |
|--------|----------|----------|
| HDR-01 | 显式包含所有标准库头文件 | `<map>`, `<algorithm>`, `<sstream>`, `<functional>`, `<memory>` |
| HDR-02 | 前向声明仅用于指针/引用 | 继承关系需要完整定义 |
| HDR-04 | 枚举类型需要完整定义 | 不能仅前向声明枚举 |

---

## 三、API命名一致性 ⚠️ 高频

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| API-01 | Getter方法使用Get前缀 | `Size()` | `GetSize()` |
| API-02 | 索引访问使用N后缀 | `GetCoordinateAt(i)` | `GetCoordinateN(i)` |
| API-03 | 遵循OGC标准方法名 | `AsWKT()` | `AsText()` |

**API命名对照表**:
```
错误调用                    → 正确调用
─────────────────────────────────────────
RenderQueue::Size()         → RenderQueue::GetSize()
LineString::GetCoordinateAt → LineString::GetCoordinateN
Polygon::GetInteriorRing    → Polygon::GetInteriorRingN
Geometry::AsWKT()           → Geometry::AsText()
TileKey::GetX()             → TileKey::x (公开成员)
Color::ToRGB()              → Color::GetRGB()
```

---

## 四、const正确性 ⚠️ 高频

| 规则ID | 规则内容 | 解决方法 |
|--------|----------|----------|
| CONST-01 | const方法只能调用const方法 | 确保被调方法是const |
| CONST-02 | const方法中修改成员需mutable | `mutable std::map<...> m_cache;` |
| CONST-03 | 使用find替代operator[] | `auto it = map.find(key);` |

---

## 五、智能指针与类型转换

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| PTR-01 | 派生类到基类用release() | 转移所有权，避免双重删除 |
| PTR-02 | Multi*几何类用具体类型 | `vector<GeometryPtr>`不能创建MultiPoint |
| PTR-03 | unique_ptr不可复制 | 使用std::move转移 |
| PTR-05 | 所有权转移后不delete | release()后原指针不再负责删除 |

---

## 六、数据初始化

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| INIT-01 | 成员变量在初始化列表中初始化 | 避免未初始化 |
| INIT-02 | 布尔标志初始化 | 如`m_valid(false)` |
| INIT-03 | 指针初始化为nullptr | 避免野指针 |

---

## 七、几何类使用

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| GEO-01 | Polygon环使用LinearRing | 而非LineString |
| GEO-02 | Envelope参数顺序 | `(min_x, min_y, max_x, max_y)` |
| GEO-03 | 空几何检查 | 允许空几何序列化 |
