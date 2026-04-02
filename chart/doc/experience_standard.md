# 编码实施经验标准 v1.0

> **文档目的**: 本文档总结了从实际项目中归纳的编码、编译、测试、运行经验，旨在帮助开发人员和大模型在实施过程中避免常见问题，提高开发效率和代码质量。
> 
> **适用范围**: C++11项目，特别是涉及几何库、数据库、空间索引等模块的项目。
> 
> **生成时间**: 2026-04-02  
> **数据来源**: compile_test_problem_summary.md (135个问题记录)

---

## 目录

- [一、编码规范标准](#一编码规范标准)
- [二、接口设计标准](#二接口设计标准)
- [三、构建配置标准](#三构建配置标准)
- [四、测试实施标准](#四测试实施标准)
- [五、数据结构与算法标准](#五数据结构与算法标准)
- [六、大模型编码实施流程](#六大模型编码实施流程)
- [七、问题分类索引](#七问题分类索引)
- [八、快速检查清单](#八快速检查清单)

---

## 一、编码规范标准

### 1.1 C++标准兼容性 ⚠️ 高频问题

**问题占比**: 9%  
**严重程度**: 高 - 导致编译失败

#### 核心规则

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| **STD-01** | 禁止使用C++17结构化绑定 | `auto [a, b] = pair;` | `auto a = pair.first; auto b = pair.second;` |
| **STD-02** | 禁止使用C++14泛型lambda | `auto f = [](auto x){};` | `template<typename T> void f(T x){}` |
| **STD-03** | 禁止使用C++14返回类型推导 | `auto f() { return 1; }` | `int f() { return 1; }` |
| **STD-04** | 禁止使用C++14二进制字面量 | `int x = 0b1010;` | `int x = 10;` |
| **STD-05** | 禁止使用C++14数字分隔符 | `int x = 1'000'000;` | `int x = 1000000;` |

#### 实施要点

```cpp
// ❌ 错误: C++17结构化绑定
auto [envelope, data] = entries[i];

// ✅ 正确: 传统pair访问
Envelope envelope = entries[i].first;
T data = entries[i].second;

// ❌ 错误: C++14泛型lambda
auto process = [](auto geom) { return geom->Area(); };

// ✅ 正确: 模板函数
template<typename T>
auto Process(T geom) -> decltype(geom->Area()) { 
    return geom->Area(); 
}
```

#### 检查方法

1. **编译器检查**: 使用 `-std=c++11` 编译选项
2. **静态分析**: 使用 clang-tidy 或 cppcheck
3. **代码审查**: 检查是否使用了 `auto [`, `[](auto`, `auto f()` 等语法

---

### 1.2 头文件管理 ⚠️ 高频问题

**问题占比**: 6%  
**严重程度**: 中 - 导致编译失败或运行时错误

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **HDR-01** | 显式包含所有使用的标准库头文件 | 不要依赖传递包含 |
| **HDR-02** | 前向声明仅用于指针/引用 | 继承关系需要完整定义 |
| **HDR-03** | cpp文件包含所有需要的头文件 | 避免依赖.h的传递包含 |
| **HDR-04** | 枚举类型需要完整定义 | 不能仅前向声明枚举类型 |

#### 常见遗漏头文件清单

| 使用的类型/函数 | 必须包含的头文件 |
|----------------|------------------|
| `std::map`, `std::multimap` | `<map>` |
| `std::set`, `std::multiset` | `<set>` |
| `std::unordered_map` | `<unordered_map>` |
| `std::sort`, `std::find`, `std::copy` | `<algorithm>` |
| `std::stringstream`, `std::istringstream` | `<sstream>` |
| `std::setw`, `std::setprecision` | `<iomanip>` |
| `std::function`, `std::bind` | `<functional>` |
| `std::unique_ptr`, `std::shared_ptr` | `<memory>` |
| `std::string` | `<string>` |
| `std::vector` | `<vector>` |
| `std::thread`, `std::mutex` | `<thread>`, `<mutex>` |
| `std::chrono` | `<chrono>` |

#### 实施要点

```cpp
// ❌ 错误: 依赖传递包含
// geometry.h 包含了 <string>，但不应依赖
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

```cpp
// ❌ 错误: 前向声明枚举类型
enum class GeomType;  // 不完整定义
void Process(GeomType type);  // 可能导致错误

// ✅ 正确: 包含完整定义
#include "geometry.h"  // 包含 GeomType 的完整定义
void Process(GeomType type);
```

#### 检查方法

1. **编译检查**: 逐个头文件独立编译
2. **IWYU工具**: 使用 include-what-you-use 工具
3. **代码审查**: 检查每个类型的使用是否包含对应头文件

---

### 1.3 API命名一致性 ⚠️ 高频问题

**问题占比**: 6%  
**严重程度**: 中 - 导致编译失败或API不一致

#### 核心规则

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| **API-01** | Getter方法使用Get前缀 | `Size()`, `Count()` | `GetSize()`, `GetCount()` |
| **API-02** | 索引访问使用N后缀 | `GetCoordinateAt(i)` | `GetCoordinateN(i)` |
| **API-03** | 遵循OGC标准方法名 | `AsWKT()` | `AsText()` |
| **API-04** | 简单结构体可用公开成员 | `TileKey::GetX()` | `TileKey::x` |
| **API-05** | 颜色转换使用Get前缀 | `Color::ToRGB()` | `Color::GetRGB()` |

#### API命名对照表

```
错误调用                           → 正确调用
──────────────────────────────────────────────────
RenderQueue::Size()                → RenderQueue::GetSize()
LineString::GetCoordinateAt(i)     → LineString::GetCoordinateN(i)
Polygon::GetInteriorRing(i)        → Polygon::GetInteriorRingN(i)
Geometry::AsWKT()                  → Geometry::AsText()
TileKey::GetX()                    → TileKey::x (公开成员)
Color::ToRGB()                     → Color::GetRGB()
Envelope::Contains(x, y)           → Envelope::Contains(Coordinate(x, y))
CNFieldDefn::Create(...)           → CNFieldDefn::CreateFieldDefn(...)
```

#### 实施要点

```cpp
// ❌ 错误: 不符合命名规范
class Container {
public:
    int Size() const { return m_data.size(); }  // 缺少Get前缀
    int GetItemAt(int index);  // 应使用N后缀
};

// ✅ 正确: 符合命名规范
class Container {
public:
    int GetSize() const { return m_data.size(); }  // Get前缀
    int GetItemN(int index);  // N后缀表示索引访问
};
```

#### 检查方法

1. **代码审查**: 检查所有public方法命名
2. **API文档**: 生成API文档并检查一致性
3. **单元测试**: 测试代码使用正确的API名称

---

### 1.4 const正确性 ⚠️ 高频问题

**问题占比**: 4%  
**严重程度**: 中 - 导致编译失败或逻辑错误

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **CONST-01** | const方法只能调用const方法 | const上下文调用非const方法会报错 |
| **CONST-02** | const方法中修改成员需声明mutable | 缓存、锁等成员需要mutable |
| **CONST-03** | 使用find替代operator[] | map的operator[]是非const的 |
| **CONST-04** | 迭代器使用const_iterator | 在const方法中使用const_iterator |

#### 实施要点

```cpp
class Example {
public:
    // ❌ 错误: const方法调用非const方法
    void DoSomething() const {
        // m_cache[key] = value;  // 错误: operator[]非const
    }
    
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
    
    // ✅ 正确: 使用const_iterator
    void PrintAll() const {
        for (auto it = m_cache.cbegin(); it != m_cache.cend(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    }
    
private:
    mutable std::map<std::string, int> m_cache;  // mutable允许const方法修改
    mutable std::mutex m_mutex;  // 锁通常需要mutable
};
```

#### 检查方法

1. **编译检查**: 编译器会报错const方法调用非const方法
2. **代码审查**: 检查所有const方法的实现
3. **静态分析**: 使用 clang-tidy 的 readability-make-member-function-const

---

### 1.5 智能指针与类型转换

**问题占比**: 4%  
**严重程度**: 高 - 导致内存泄漏或崩溃

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **PTR-01** | 派生类到基类转换使用release() | 转移所有权，避免双重删除 |
| **PTR-02** | Multi*几何类创建使用具体类型 | vector<GeometryPtr>不能直接创建MultiPoint |
| **PTR-03** | unique_ptr不可复制，使用std::move | 转移所有权 |
| **PTR-04** | shared_ptr使用make_shared | 避免内存碎片 |
| **PTR-05** | 所有权转移后不delete | release()后原指针不再负责删除 |

#### 实施要点

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

// ❌ 错误: release后继续使用原指针
std::unique_ptr<Geometry> p1 = CreateGeometry();
Geometry* raw = p1.release();
delete p1.get();  // 错误: 双重删除

// ✅ 正确: release后所有权转移
std::unique_ptr<Geometry> p1 = CreateGeometry();
Geometry* raw = p1.release();
// p1不再负责删除，由raw管理
delete raw;  // OK
```

#### 检查方法

1. **编译检查**: 编译器会报错unique_ptr复制
2. **静态分析**: 使用 clang-tidy 的 modernize-make-shared
3. **内存检查**: 使用 valgrind 或 AddressSanitizer

---

## 二、接口设计标准

### 2.1 纯虚接口类设计

**问题占比**: 9%  
**严重程度**: 高 - 导致编译失败或链接错误

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **INT-01** | 提供工厂方法 | 纯虚接口类需要静态Create方法 |
| **INT-02** | 提供引用计数管理 | 添加ReleaseReference方法 |
| **INT-03** | 派生类实现所有纯虚函数 | 使用override关键字确保完整性 |
| **INT-04** | 虚析构函数使用default | `virtual ~Interface() = default;` |

#### 接口类模板

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

#### 检查方法

1. **编译检查**: 确保所有纯虚函数都有实现
2. **代码审查**: 检查是否使用override关键字
3. **单元测试**: 创建派生类进行测试

---

### 2.2 DLL导出配置

**问题占比**: 8%  
**严重程度**: 高 - 导致链接失败

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **DLL-01** | 每个模块独立导出宏 | 使用`OGC_XXX_API`而非统一的`OGC_API` |
| **DLL-02** | 纯虚接口类需要导出 | 即使是纯虚类也需要添加导出宏 |
| **DLL-03** | Pimpl模式类需要导出 | 包含unique_ptr<Impl>的类需要导出 |
| **DLL-04** | 静态库特殊处理 | 添加`OGC_XXX_STATIC`宏判断 |

#### 导出宏模板

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

#### 检查方法

1. **编译检查**: 确保所有导出类都有导出宏
2. **链接检查**: 确保静态库和动态库配置正确
3. **符号检查**: 使用 `nm` 或 `dumpbin` 检查导出符号

---

## 三、构建配置标准

### 3.1 CMake配置

**问题占比**: 4%  
**严重程度**: 中 - 导致构建失败或运行时错误

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **CMAKE-01** | VS多配置生成器需要配置特定变量 | 设置`CMAKE_*_OUTPUT_DIRECTORY_RELEASE`等 |
| **CMAKE-02** | 动态库输出目录同时设置RUNTIME、LIBRARY、ARCHIVE | 确保所有输出类型路径一致 |
| **CMAKE-03** | 测试链接库确保路径一致 | 库输出路径与链接路径一致 |
| **CMAKE-04** | 使用target_link_libraries链接依赖 | 避免手动设置链接路径 |

#### CMake模板

```cmake
# 通用输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")

# VS多配置生成器需要额外设置
if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")
endif()

# 动态库
add_library(ogc_module SHARED ${SOURCES})

# 设置目标属性
set_target_properties(ogc_module PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release"
)

# 测试目标
add_executable(test_module ${TEST_SOURCES})
target_link_libraries(test_module 
    ogc_module
    gtest
    gtest_main
)
```

#### 检查方法

1. **构建检查**: 确保所有配置都能正确构建
2. **路径检查**: 检查输出文件是否在正确位置
3. **依赖检查**: 使用 `ldd` 或 `dumpbin` 检查依赖

---

### 3.2 测试配置

**问题占比**: 1%  
**严重程度**: 低 - 导致测试失败

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **TEST-01** | 使用gtest_main | 移除自定义main函数，使用gtest_main库 |
| **TEST-02** | 测试目标链接被测模块 | 确保链接正确 |
| **TEST-03** | 抽象基类创建派生类测试 | 无法直接实例化抽象类 |
| **TEST-04** | 并发测试需要特殊处理 | 禁用或使用线程安全配置 |

#### 检查方法

1. **编译检查**: 确保测试代码编译通过
2. **运行检查**: 确保测试可执行文件能运行
3. **结果检查**: 确保测试结果正确

---

## 四、测试实施标准

### 4.1 单元测试编写

**问题占比**: 7%  
**严重程度**: 中 - 导致测试失败或误报

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **UT-01** | 使用正确的API方法名 | 参考API命名对照表 |
| **UT-02** | 抽象基类创建派生类 | 无法直接实例化抽象类 |
| **UT-03** | 测试边界条件 | 空几何、空集合、边界值 |
| **UT-04** | 测试错误处理 | 异常、错误返回值 |
| **UT-05** | 并发测试禁用或特殊处理 | 避免竞态条件 |

#### 实施要点

```cpp
// ❌ 错误: 使用错误的API名称
TEST(GeometryTest, Size) {
    GeometryCollection col;
    EXPECT_EQ(col.Size(), 0);  // 错误: 应该是GetSize()
}

// ✅ 正确: 使用正确的API名称
TEST(GeometryTest, Size) {
    GeometryCollection col;
    EXPECT_EQ(col.GetSize(), 0);  // OK
}

// ❌ 错误: 直接实例化抽象类
TEST(InterfaceTest, Create) {
    IExample obj;  // 编译错误: 抽象类无法实例化
}

// ✅ 正确: 创建派生类
class MockExample : public IExample {
public:
    const std::string& GetName() const override { return m_name; }
    void DoSomething() override {}
private:
    std::string m_name = "test";
};

TEST(InterfaceTest, Create) {
    MockExample obj;  // OK
}
```

#### 检查方法

1. **编译检查**: 确保测试代码编译通过
2. **运行检查**: 确保测试运行无崩溃
3. **结果检查**: 确保测试结果符合预期

---

### 4.2 集成测试编写

**问题占比**: 3%  
**严重程度**: 中 - 导致集成失败

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **IT-01** | 测试模块间交互 | 确保接口正确对接 |
| **IT-02** | 测试数据流 | 确保数据正确传递 |
| **IT-03** | 测试错误传播 | 确保错误正确处理 |
| **IT-04** | 测试性能 | 确保性能符合要求 |

---

## 五、数据结构与算法标准

### 5.1 几何类使用

**问题占比**: 3%  
**严重程度**: 中 - 导致逻辑错误

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **GEO-01** | Polygon环使用LinearRing | 而非LineString |
| **GEO-02** | Envelope参数顺序 | `(min_x, min_y, max_x, max_y)` |
| **GEO-03** | 空几何检查 | 允许空几何序列化，不要直接拒绝 |
| **GEO-04** | 坐标默认值 | 使用`quiet_NaN()`表示空坐标 |

#### 实施要点

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

### 5.2 数据初始化

**问题占比**: 3%  
**严重程度**: 中 - 导致未定义行为

#### 核心规则

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| **INIT-01** | 成员变量在构造函数初始化列表中初始化 | 避免未初始化 |
| **INIT-02** | 布尔标志初始化 | 如`m_valid(false)` |
| **INIT-03** | 指针初始化为nullptr | 避免野指针 |
| **INIT-04** | 数值初始化为0或NaN | 根据语义选择 |

#### 实施要点

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

## 六、大模型编码实施流程

### 6.1 编码前检查清单

```markdown
□ 确认项目C++标准版本（本项目为C++11）
□ 查阅相关模块的index.md了解已有类和方法
□ 确认API命名规范（GetSize而非Size）
□ 确认设计文档中对应的模块接口定义
□ 确认所有需要的头文件
□ 确认DLL导出宏配置
```

### 6.2 编码时检查清单

```markdown
□ 所有使用的标准库类型都有对应的#include
□ const方法中调用的方法都是const的
□ 需要在const方法中修改的成员声明为mutable
□ 智能指针转换使用正确的方式
□ 纯虚接口类提供了工厂方法
□ 使用override关键字确保虚函数覆盖
□ 成员变量在构造函数初始化列表中初始化
```

### 6.3 编译前检查清单

```markdown
□ CMakeLists.txt中源文件列表已更新
□ 输出目录配置正确（包括配置特定变量）
□ DLL导出宏配置正确
□ 链接库配置正确
□ 测试目标链接了被测模块
```

### 6.4 测试前检查清单

```markdown
□ 测试用例使用正确的API方法名
□ 测试目标链接了被测模块
□ 抽象基类创建了派生类进行测试
□ 测试边界条件和错误处理
□ 并发测试已禁用或特殊处理
```

---

## 七、问题分类索引

### 7.1 问题分类统计

| 分类 | 数量 | 占比 | 严重程度 | 关键避坑点 |
|------|------|------|----------|------------|
| 接口实现缺失 | 10 | 9% | 高 | 纯虚函数全部实现，使用override |
| DLL导出 | 9 | 8% | 高 | 模块独立宏，接口类导出 |
| 头文件管理 | 7 | 6% | 中 | 显式包含标准库头文件 |
| API命名 | 7 | 6% | 中 | GetSize而非Size，GetCoordinateN |
| 测试用例 | 8 | 7% | 中 | 使用正确API，抽象类创建派生类 |
| 内存管理 | 3 | 3% | 高 | 所有权转移后不delete，使用引用计数 |
| const正确性 | 4 | 4% | 中 | mutable成员，const方法调用 |
| 智能指针转换 | 4 | 4% | 高 | release()转移，具体类型vector |
| 构建配置 | 4 | 4% | 中 | 配置特定输出目录变量 |
| 数据结构实现 | 3 | 3% | 中 | 区分叶子/非叶子节点，Envelope参数顺序 |
| 类型转换 | 3 | 3% | 中 | 显式类型转换，Polygon创建需先创建LinearRing |
| 链接错误 | 5 | 4% | 高 | 移除重复main函数，使用gtest_main |
| 数据序列化 | 3 | 3% | 中 | WKB ring数量计算，空几何处理 |
| 逻辑错误 | 3 | 3% | 中 | GetEnvelope无几何检查，FID验证逻辑 |
| 测试配置 | 1 | 1% | 低 | 自动FID生成配置 |
| 其他 | 41 | 36% | - | 参见详细问题描述 |

### 7.2 高频问题Top 10

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

## 八、快速检查清单

### 8.1 编译失败快速诊断

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

### 8.2 链接失败快速诊断

```
链接错误类型                    → 可能原因                    → 检查项
─────────────────────────────────────────────────────────────────────
无法解析的外部符号              → 库未链接或路径错误          → CMAKE-03, CMAKE-04
重复定义                        → main函数重复或符号冲突      → TEST-01
找不到库文件                    → 输出目录配置错误            → CMAKE-01, CMAKE-02
DLL导出问题                     → 导出宏配置错误              → DLL-01~04
```

### 8.3 测试失败快速诊断

```
测试失败类型                    → 可能原因                    → 检查项
─────────────────────────────────────────────────────────────────────
API调用错误                     → 方法名不正确                → API-01~05
空指针崩溃                      → 未初始化或所有权问题        → INIT-03, PTR-05
逻辑错误                        → 算法实现错误                → GEO-01~04
内存泄漏                        → 所有权管理错误              → PTR-01~05
并发问题                        → 线程安全问题                → UT-05
```

---

## 附录A: 规则ID索引

| 规则ID | 规则内容 | 章节 |
|--------|----------|------|
| STD-01 | 禁止使用C++17结构化绑定 | 1.1 |
| STD-02 | 禁止使用C++14泛型lambda | 1.1 |
| STD-03 | 禁止使用C++14返回类型推导 | 1.1 |
| HDR-01 | 显式包含所有使用的标准库头文件 | 1.2 |
| HDR-02 | 前向声明仅用于指针/引用 | 1.2 |
| HDR-03 | cpp文件包含所有需要的头文件 | 1.2 |
| HDR-04 | 枚举类型需要完整定义 | 1.2 |
| API-01 | Getter方法使用Get前缀 | 1.3 |
| API-02 | 索引访问使用N后缀 | 1.3 |
| API-03 | 遵循OGC标准方法名 | 1.3 |
| CONST-01 | const方法只能调用const方法 | 1.4 |
| CONST-02 | const方法中修改成员需声明mutable | 1.4 |
| CONST-03 | 使用find替代operator[] | 1.4 |
| PTR-01 | 派生类到基类转换使用release() | 1.5 |
| PTR-02 | Multi*几何类创建使用具体类型 | 1.5 |
| PTR-03 | unique_ptr不可复制，使用std::move | 1.5 |
| INT-01 | 提供工厂方法 | 2.1 |
| INT-02 | 提供引用计数管理 | 2.1 |
| INT-03 | 派生类实现所有纯虚函数 | 2.1 |
| DLL-01 | 每个模块独立导出宏 | 2.2 |
| DLL-02 | 纯虚接口类需要导出 | 2.2 |
| CMAKE-01 | VS多配置生成器需要配置特定变量 | 3.1 |
| CMAKE-02 | 动态库输出目录同时设置RUNTIME、LIBRARY、ARCHIVE | 3.1 |
| TEST-01 | 使用gtest_main | 3.2 |
| UT-01 | 使用正确的API方法名 | 4.1 |
| UT-02 | 抽象基类创建派生类 | 4.1 |
| GEO-01 | Polygon环使用LinearRing | 5.1 |
| GEO-02 | Envelope参数顺序 | 5.1 |

---

## 附录B: 参考文档

1. [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
2. [C++11 FAQ](https://stroustrup.com/C++11FAQ.html)
3. [OGC Simple Features Specification](https://www.ogc.org/standards/sfa)
4. [Google Test Primer](https://github.com/google/googletest/blob/main/docs/primer.md)
5. [CMake Documentation](https://cmake.org/documentation/)

---

**文档版本**: v1.0  
**最后更新**: 2026-04-02  
**维护者**: 开发团队
