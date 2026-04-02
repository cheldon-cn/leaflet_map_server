# 编码规范示例代码

> 本文档包含编码规范的示例代码，供开发人员参考。

---

## 一、C++标准兼容性示例

### STD-01: 禁止C++17结构化绑定

```cpp
// ❌ 错误: C++17结构化绑定
auto [envelope, data] = entries[i];

// ✅ 正确: 传统pair访问
Envelope envelope = entries[i].first;
T data = entries[i].second;
```

### STD-02: 禁止C++14泛型lambda

```cpp
// ❌ 错误: C++14泛型lambda
auto process = [](auto geom) { return geom->Area(); };

// ✅ 正确: 模板函数
template<typename T>
auto Process(T geom) -> decltype(geom->Area()) { 
    return geom->Area(); 
}
```

---

## 二、头文件管理示例

### HDR-01: 显式包含所有标准库头文件

```cpp
// ❌ 错误: 依赖传递包含
class MyClass {
    std::string m_name;  // 可能编译失败
    std::map<int, int> m_data;  // 缺少 <map>
};

// ✅ 正确: 显式包含所有需要的头文件
#include <string>
#include <map>
#include "geometry.h"

class MyClass {
    std::string m_name;  // OK
    std::map<int, int> m_data;  // OK
};
```

### HDR-04: 枚举类型需要完整定义

```cpp
// ❌ 错误: 前向声明枚举类型
enum class GeomType;  // 不完整定义
void Process(GeomType type);  // 可能导致错误

// ✅ 正确: 包含完整定义
#include "geometry.h"  // 包含 GeomType 的完整定义
void Process(GeomType type);
```

---

## 三、const正确性示例

### CONST-01~03: const方法正确性

```cpp
class Example {
public:
    // ✅ 正确: 使用mutable成员
    void DoSomething() const {
        m_cache[key] = value;  // OK: m_cache是mutable
    }
    
    // ✅ 正确: 使用find替代operator[]
    int GetValue(const std::string& key) const {
        auto it = m_cache.find(key);  // OK: find是const的
        if (it != m_cache.end()) {
            return it->second;
        }
        return -1;
    }
    
private:
    mutable std::map<std::string, int> m_cache;  // mutable允许const方法修改
    mutable std::mutex m_mutex;  // 锁通常需要mutable
};
```

---

## 四、智能指针示例

### PTR-01~05: 智能指针正确使用

```cpp
// ❌ 错误: vector<GeometryPtr>直接创建MultiPoint
std::vector<GeometryPtr> points;
auto multiPoint = MultiPoint::Create(points);  // 编译错误

// ✅ 正确: 使用CoordinateList
CoordinateList coords;
for (const auto& pt : points) {
    coords.push_back(pt->GetCoordinateN(0));
}
auto multiPoint = MultiPoint::Create(coords);

// ❌ 错误: unique_ptr复制
std::unique_ptr<Geometry> p1 = CreateGeometry();
auto p2 = p1;  // 编译错误

// ✅ 正确: unique_ptr移动
std::unique_ptr<Geometry> p1 = CreateGeometry();
auto p2 = std::move(p1);  // OK
```

---

## 五、接口设计示例

### INT-01~03: 纯虚接口类设计

```cpp
// 接口类定义
class IExample {
public:
    // 虚析构函数
    virtual ~IExample() = default;
    
    // 工厂方法
    static IExample* Create(const std::string& name);
    
    // 引用计数
    virtual void ReleaseReference() = 0;
    
    // 纯虚方法
    virtual const std::string& GetName() const = 0;
    virtual void DoSomething() = 0;
};

// 实现类定义
class ExampleImpl : public IExample {
public:
    // 工厂方法实现
    static IExample* Create(const std::string& name) {
        return new ExampleImpl(name);
    }
    
    // 引用计数实现
    void ReleaseReference() override {
        delete this;
    }
    
    // 纯虚方法实现
    const std::string& GetName() const override {
        return m_name;
    }
    
    void DoSomething() override {
        // 实现
    }
    
private:
    ExampleImpl(const std::string& name) : m_name(name) {}
    std::string m_name;
};
```

---

## 六、DLL导出示例

### DLL-01~04: DLL导出宏配置

```cpp
// export.h
#ifdef OGC_MODULE_STATIC
    #define OGC_MODULE_API
#else
    #ifdef _WIN32
        #ifdef OGC_MODULE_EXPORTS
            #define OGC_MODULE_API __declspec(dllexport)
        #else
            #define OGC_MODULE_API __declspec(dllimport)
        #endif
    #else
        #define OGC_MODULE_API
    #endif
#endif

// 类声明
class OGC_MODULE_API MyClass { 
    // ...
};
```

---

## 七、几何类使用示例

### GEO-01~03: 几何类正确使用

```cpp
// ❌ 错误: Polygon使用LineString
LineString* ring = new LineString(coords);
Polygon* poly = new Polygon(ring);  // 错误: 应该使用LinearRing

// ✅ 正确: Polygon使用LinearRing
LinearRing* ring = new LinearRing(coords);
Polygon* poly = new Polygon(ring);  // OK

// ❌ 错误: Envelope参数顺序错误
Envelope env(max_x, min_y, min_x, max_y);  // 错误: 顺序不对

// ✅ 正确: Envelope参数顺序正确
Envelope env(min_x, min_y, max_x, max_y);  // OK
```

---

## 八、数据初始化示例

### INIT-01~03: 数据正确初始化

```cpp
// ❌ 错误: 成员变量未初始化
class Example {
public:
    Example() {}  // m_valid未初始化
private:
    bool m_valid;  // 未初始化，可能是任意值
};

// ✅ 正确: 成员变量在初始化列表中初始化
class Example {
public:
    Example() : m_valid(false), m_count(0), m_ptr(nullptr) {}
private:
    bool m_valid;
    int m_count;
    void* m_ptr;
};
```

---

**文档版本**: v1.0  
**最后更新**: 2026-04-02
