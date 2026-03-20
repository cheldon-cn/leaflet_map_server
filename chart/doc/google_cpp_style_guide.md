# Google C++ Style Guide 完整指南

> 版本：C++20  
> 最后更新：2025年3月  
> 来源：https://google.github.io/styleguide/cppguide.html

## 目录

1. [概述](#概述)
2. [C++ 版本要求](#c-版本要求)
3. [命名规范](#命名规范)
   - [文件命名](#文件命名)
   - [类型命名](#类型命名)
   - [变量命名](#变量命名)
   - [常量命名](#常量命名)
   - [函数命名](#函数命名)
   - [命名空间命名](#命名空间命名)
   - [枚举命名](#枚举命名)
   - [宏命名](#宏命名)
4. [代码格式](#代码格式)
   - [行长度](#行长度)
   - [缩进与空格](#缩进与空格)
   - [函数声明与定义](#函数声明与定义)
   - [Lambda 表达式](#lambda-表达式)
   - [条件与循环语句](#条件与循环语句)
   - [指针和引用](#指针和引用)
5. [头文件规范](#头文件规范)
   - [#define 保护](#define-保护)
   - [包含顺序](#包含顺序)
   - [前向声明](#前向声明)
6. [类设计规范](#类设计规范)
   - [构造函数](#构造函数)
   - [隐式转换](#隐式转换)
   - [拷贝与移动](#拷贝与移动)
   - [继承](#继承)
   - [运算符重载](#运算符重载)
   - [声明顺序](#声明顺序)
7. [函数设计规范](#函数设计规范)
   - [参数与返回值](#参数与返回值)
   - [函数重载](#函数重载)
   - [默认参数](#默认参数)
8. [作用域管理](#作用域管理)
   - [命名空间](#命名空间)
   - [局部变量](#局部变量)
9. [最佳实践](#最佳实践)
   - [所有权与智能指针](#所有权与智能指针)
   - [常量与 constexpr](#常量与-constexpr)
   - [类型推导 auto](#类型推导-auto)
   - [注释规范](#注释规范)
10. [禁止使用的特性](#禁止使用的特性)
11. [代码检查工具](#代码检查工具)

---

## 概述

Google C++ Style Guide 旨在通过统一的编码规范提高代码的**可读性**、**可维护性**和**一致性**。本指南适用于所有 Google 开源项目和内部项目。

### 核心原则

1. **可读性优先**：代码是被阅读的次数远多于被编写的次数
2. **显式优于隐式**：明确表达意图，避免隐式行为
3. **一致性**：统一的风格降低认知负担
4. **现代 C++**：充分利用 C++20 的新特性

---

## C++ 版本要求

**目标版本：C++20**

```cpp
// ✓ 正确：使用 C++20 特性
#include <concepts>
#include <ranges>

template<std::integral T>
T add(T a, T b) {
    return a + b;
}

// ✗ 错误：不要使用 C++23 特性
// auto x = std::print("Hello"); // C++23 特性
```

**规则：**
- 代码应以 C++20 为目标
- 不要使用 C++23 特性
- 禁止使用非标准扩展（如 GCC 扩展）

---

## 命名规范

命名是代码可读性的基础。好的命名能让代码"自文档化"。

### 文件命名

**规则：** 全部小写，可包含下划线 (`_`) 或连字符 (`-`)

**示例：**

```text
✓ 正确示例：
my_useful_class.cpp
my-useful-class.cpp
myusefulclass.cpp
myusefulclass_test.cpp

✗ 错误示例：
MyUsefulClass.cpp          // 不要使用大写
myUsefulClass.cpp          // 不要使用驼峰命名
my_useful_class_test.cpp   // 可行但不推荐（下划线和连字符混用）
```

**建议：** 项目内保持统一，选择下划线或连字符其中一种风格。

---

### 类型命名

**规则：** 帕斯卡命名法（PascalCase），每个单词首字母大写，无下划线

适用范围：
- 类（class）
- 结构体（struct）
- 类型别名（using、typedef）
- 枚举（enum）
- 模板参数

**示例：**

```cpp
// ✓ 正确示例：类命名
class UrlTable {
    // ...
};

class UrlTableProperties {
    // ...
};

// ✓ 正确示例：结构体命名
struct Options {
    // ...
};

struct UrlTableProperties {
    // ...
};

// ✓ 正确示例：类型别名
using HashMap = std::unordered_map<std::string, std::string>;
typedef std::unordered_map<std::string, std::string> HashMap; // C++11 之前

// ✓ 正确示例：枚举命名
enum class UrlTableErrors {
    kOk = 0,
    kOutOfMemory,
    kMalformedInput,
};

// ✓ 正确示例：模板参数命名
template <typename TKey, typename TValue>
class HashTable {
    // ...
};

// ✗ 错误示例
class url_table { };        // 不要使用蛇形命名
class URLTable { };         // 不要使用全大写缩写
class urlTable { };         // 不要使用驼峰命名
```

---

### 变量命名

**规则：** 蛇形命名法（snake_case），全小写，单词间用下划线连接

**普通变量：**

```cpp
// ✓ 正确示例
std::string table_name;     // 蛇形命名
std::string name;           // 单词无下划线也可
int num_errors;             // 清晰描述用途
int num_completed_connections;

// ✗ 错误示例
std::string tableName;      // 不要使用驼峰命名
std::string Name;           // 不要以大写开头
int n;                      // 不要使用无意义的缩写
int nerr;                   // 不要使用模糊的缩写
```

**类数据成员：**

**规则：** 添加下划线后缀

```cpp
class TableInfo {
private:
    std::string name_;          // 下划线后缀
    int num_entries_;           // 下划线后缀
    static int instance_count_; // 静态成员也加下划线
    
public:
    const std::string& get_name() const { return name_; }
    void set_name(const std::string& name) { name_ = name; }
};

// ✗ 错误示例
class TableInfo {
    std::string m_name;         // 不要使用匈牙利命名法前缀
    std::string _name;          // 不要使用前导下划线
    std::string mName;          // 不要使用驼峰命名
};
```

**全局变量：**

**规则：** 尽量避免使用全局变量；如果必须使用，添加 `g_` 前缀

```cpp
// ✓ 正确示例（如果必须使用全局变量）
namespace {
    int g_instance_count = 0;  // g_ 前缀
}

// ✗ 错误示例
int instance_count = 0;        // 缺少前缀
```

**函数参数：**

```cpp
// ✓ 正确示例
void set_name(const std::string& new_name) {
    name_ = new_name;
}

void draw_rectangle(int width, int height);

// ✗ 错误示例
void set_name(const std::string& newName);  // 不要使用驼峰命名
```

---

### 常量命名

**规则：** 使用 `k` 前缀 + 帕斯卡命名法，或全大写 + 下划线

**推荐方式（k 前缀）：**

```cpp
// ✓ 正确示例：constexpr 常量
constexpr int kDaysInWeek = 7;
constexpr int kMaxRetryCount = 3;
constexpr double kPi = 3.14159265358979323846;

// ✓ 正确示例：const 常量
const int kBufferSize = 1024;
const std::string kDefaultName = "unnamed";

// ✓ 正确示例：类内静态常量
class MyConstants {
public:
    static constexpr int kMaxSize = 100;
    static const std::string kPrefix;
};

// ✗ 错误示例
const int max_size = 100;           // 不要使用蛇形命名
const int MAXSIZE = 100;            // 不要使用全大写无下划线
constexpr int MAX_RETRY_COUNT = 3;  // 全大写可接受，但 k 前缀更推荐
```

**传统方式（全大写）：**

```cpp
// 可接受但不太推荐
const int MAX_BUFFER_SIZE = 4096;
constexpr double PI = 3.14159265358979323846;
```

**建议：** 项目内保持统一，优先使用 `k` 前缀风格。

---

### 函数命名

**规则：** 普通函数使用帕斯卡命名法（PascalCase）

```cpp
// ✓ 正确示例：普通函数
void AddTableEntry();
bool DeleteUrl(const std::string& url);
int CalculateArea(int width, int height);
std::string GetUserName(int user_id);

// ✓ 正确示例：取值/设值函数（可使用蛇形命名）
class User {
private:
    std::string name_;
    
public:
    // 取值函数：蛇形命名，匹配变量名
    const std::string& name() const { return name_; }
    const std::string& get_name() const { return name_; }
    
    // 设值函数：蛇形命名，匹配变量名
    void set_name(const std::string& name) { name_ = name; }
};

// ✗ 错误示例
void addTableEntry();           // 不要使用驼峰命名
void add_table_entry();         // 不要使用蛇形命名（普通函数）
```

**注意：** 取值/设值函数可以使用蛇形命名法（`get_name()` / `set_name()`）以匹配变量名。

---

### 命名空间命名

**规则：** 全小写，基于项目名或路径

```cpp
// ✓ 正确示例
namespace geometry {
namespace rendering {
    // ...
}
}

namespace my_project::utils {
    // ...
}

// ✗ 错误示例
namespace Geometry { }          // 不要使用大写
namespace myProject { }         // 不要使用驼峰命名
namespace my_project_utils { }  // 嵌套命名空间更好
```

---

### 枚举命名

**规则：** 枚举类型使用帕斯卡命名法；枚举值使用 `k` 前缀或帕斯卡命名法

**推荐方式（枚举类 + k 前缀）：**

```cpp
// ✓ 正确示例：enum class
enum class HttpStatusCode {
    kOk = 200,
    kNotFound = 404,
    kInternalServerError = 500,
};

enum class Color {
    kRed,
    kGreen,
    kBlue,
};

// ✓ 正确示例：传统 enum
enum UrlTableErrors {
    kOk = 0,
    kOutOfMemory,
    kMalformedInput,
};

// ✗ 错误示例
enum class HTTP_STATUS_CODE { };    // 不要使用全大写
enum HttpStatusCode {
    OK = 200,                       // 缺少 k 前缀（虽然可接受但不统一）
    NOT_FOUND = 404,
};
```

**替代方式（帕斯卡命名法）：**

```cpp
// 可接受
enum class Color {
    Red,
    Green,
    Blue,
};
```

---

### 宏命名

**规则：** 极力避免使用宏；如果必须使用，全大写 + 下划线

```cpp
// ✓ 正确示例（如果必须使用宏）
#define PI_ROUNDED 3.14
#define MY_PROJECT_VERSION_MAJOR 1
#define MY_PROJECT_VERSION_MINOR 0

// 宏函数
#define ROUND(x) ((x) >= 0 ? (x) + 0.5 : (x) - 0.5)

// ✗ 错误示例
#define pi_rounded 3.14         // 不要使用小写
#define MyMacro(x) ((x) * 2)    // 不要使用帕斯卡命名
```

**建议：** 优先使用 `constexpr`、`inline` 函数或模板替代宏：

```cpp
// ✓ 优先使用 constexpr
constexpr double kPiRounded = 3.14;

// ✓ 优先使用 inline 函数
inline double Round(double x) {
    return x >= 0 ? x + 0.5 : x - 0.5;
}

// ✓ 优先使用模板函数
template <typename T>
inline T Square(T x) {
    return x * x;
}
```

---

## 代码格式

### 行长度

**规则：** 每行最多 80 个字符

```cpp
// ✓ 正确示例：长行应换行
bool result = VeryLongFunctionName(parameter1, parameter2, parameter3,
                                   parameter4, parameter5);

// ✗ 错误示例：超过 80 个字符的行
bool result = VeryLongFunctionName(parameter1, parameter2, parameter3, parameter4, parameter5);
```

**例外：**
- URL 或长路径可以超过 80 字符
- 字符串字面量可以超过 80 字符
- `#include` 语句可以超过 80 字符

---

### 缩进与空格

**规则：** 只使用空格缩进，每次缩进 2 个空格

```cpp
// ✓ 正确示例：2 空格缩进
namespace my_project {

class MyClass {
public:
    void DoSomething() {
        if (condition) {
            // 4 空格缩进
            int x = 10;
        }
    }
};

}  // namespace my_project

// ✗ 错误示例：使用制表符
void BadFunction() {
	if (condition) {  // 制表符（应该用空格）
	    // ...
	}
}
```

**配置建议：**
- 配置编辑器将 Tab 转换为空格
- 设置 Tab 宽度为 2 个空格

---

### 函数声明与定义

**规则：** 返回类型和函数名在同一行；参数列表可换行对齐

```cpp
// ✓ 正确示例：短函数
ReturnType ClassName::FunctionName(Type par_name1, Type par_name2) {
    DoSomething();
    DoSomethingElse();
}

// ✓ 正确示例：长函数名换行
ReturnType LongClassName::ReallyLongFunctionName(
    Type par_name1,
    Type par_name2,
    Type par_name3) {
    DoSomething();
}

// ✓ 正确示例：参数对齐
ReturnType ClassName::FunctionName(Type var1, Type var2,
                                   Type var3, Type var4) {
    DoSomething();
}

// ✗ 错误示例：返回类型单独一行
ReturnType
ClassName::FunctionName(Type par_name1, Type par_name2) {
    // ...
}

// ✗ 错误示例：参数未对齐
ReturnType ClassName::FunctionName(Type var1, Type var2,
    Type var3, Type var4) {
    // ...
}
```

---

### Lambda 表达式

**规则：** 格式与普通函数类似，参数列表和捕获列表格式统一

```cpp
// ✓ 正确示例：短 lambda
auto lambda = [](int x) { return x * 2; };

// ✓ 正确示例：长 lambda
auto long_lambda = [this, &variable](
    int param1,
    int param2) {
    int result = param1 + param2;
    DoSomething(result);
    return result;
};

// ✓ 正确示例：用于算法
std::transform(values.begin(), values.end(), results.begin(),
               [](int x) { return x * x; });

// ✗ 错误示例：格式混乱
auto lambda = [ this,&variable ] ( int x ) { return x * 2; };
```

---

### 条件与循环语句

**规则：** `if`、`for`、`while` 等语句后必须跟大括号，即使只有一条语句

```cpp
// ✓ 正确示例：始终使用大括号
if (condition) {
    DoSomething();
}

if (condition) {
    DoSomething();
} else {
    DoSomethingElse();
}

for (int i = 0; i < kMaxCount; ++i) {
    ProcessItem(i);
}

while (!done) {
    ProcessNext();
}

// ✗ 错误示例：省略大括号
if (condition)
    DoSomething();      // 危险：容易引入 bug

for (int i = 0; i < 10; ++i)
    Process(i);         // 危险：容易引入 bug
```

**注意：** 始终使用大括号可以避免 Apple SSL/TLS bug 类似的安全漏洞。

---

### 指针和引用

**规则：** 符号（`*` 或 `&`）与变量名相邻，不与类型相邻

```cpp
// ✓ 正确示例：符号与变量名相邻
char* c;
const std::string& name;
int* ptr;

// ✓ 正确示例：函数参数
void Function(const std::string& input, std::string* output);

// ✓ 正确示例：返回类型
const std::string& GetName() const;

// ✗ 错误示例：符号与类型相邻
char *c;              // 空格位置错误
const std::string &name;  // 空格位置错误
int * ptr;            // 两边都有空格，混乱

// ✗ 错误示例：函数参数
void Function(const std::string &input, std::string *output);
```

**原因：** 在声明多个变量时更清晰：

```cpp
// ✓ 清晰：每个变量独立声明
char* ptr1;
char* ptr2;

// ✗ 混乱：类型与符号相邻
char *ptr1, *ptr2;  // 看起来不一致
```

---

## 头文件规范

### #define 保护

**规则：** 所有头文件必须使用 `#define` 保护防止重复包含

**命名格式：** `<PROJECT>_<PATH>_<FILE>_H_`

```cpp
// 文件路径：foo/src/bar/baz.h
// ✓ 正确示例
#ifndef FOO_BAR_BAZ_H_
#define FOO_BAR_BAZ_H_

namespace foo {
namespace bar {

class Baz {
    // ...
};

}  // namespace bar
}  // namespace foo

#endif  // FOO_BAR_BAZ_H_

// ✗ 错误示例
#ifndef BAZ_H_          // 缺少项目名和路径
#define BAZ_H_
// ...
#endif

// ✗ 错误示例
#ifndef __BAZ_H__      // 不要使用双下划线
#define __BAZ_H__
// ...
#endif
```

**替代方案：** 现代 C++ 可使用 `#pragma once`（但 `#define` 更通用）

```cpp
// 可接受：#pragma once
#pragma once

namespace foo {
namespace bar {
class Baz {
    // ...
};
}
}
```

---

### 包含顺序

**规则：** 按以下顺序包含头文件，每组之间用空行隔开：

1. 相关头文件（如 `dir2/foo2.h`）
2. C 系统头文件（如 `<unistd.h>`、`<cstdio>`）
3. C++ 标准库头文件（如 `<vector>`、`<algorithm>`）
4. 其他库的 `.h` 文件
5. 本项目内的 `.h` 文件

每组内按字母顺序排列。

```cpp
// ✓ 正确示例：文件路径 foo/src/bar/baz.cc

#include "foo/bar/baz.h"           // 1. 相关头文件

#include <sys/types.h>             // 2. C 系统头文件
#include <unistd.h>

#include <algorithm>               // 3. C++ 标准库头文件
#include <map>
#include <string>
#include <vector>

#include "base/basictypes.h"       // 4. 其他库头文件
#include "base/commandlineflags.h"
#include "foo/bar/another_file.h"  // 5. 本项目头文件
#include "foo/bar/related_file.h"

// ✗ 错误示例：顺序混乱
#include <vector>
#include "foo/bar/baz.h"           // 相关头文件应该在第一位
#include <algorithm>
#include "base/basictypes.h"
#include <unistd.h>
```

**原因：** 将相关头文件放在第一位可以确保它是自包含的（不依赖其他头文件的间接包含）。

---

### 前向声明

**规则：** 尽可能避免使用前向声明，直接使用 `#include`

```cpp
// ✓ 正确示例：直接包含头文件
#include "foo/bar/baz.h"

void Function(const foo::bar::Baz& baz) {
    // 使用 Baz
}

// ✗ 不推荐：前向声明
namespace foo {
namespace bar {
class Baz;  // 前向声明
}
}

void Function(const foo::bar::Baz& baz);  // 如果 Baz 的大小或布局改变，可能出错

// ✗ 错误示例：前向声明导致隐藏依赖
// 如果 Baz 有模板参数或 typedef，前向声明会非常复杂
namespace foo {
namespace bar {
template <typename T>
class Baz;
}
}
```

**原因：**
- 前向声明会隐藏依赖关系
- 前向声明可能在库更新时失效
- 前向声明可能被内部实现细节破坏

**例外：** 当包含头文件导致循环依赖或编译速度严重下降时，可以考虑前向声明。

---

## 类设计规范

### 构造函数

**规则：** 避免在构造函数中调用虚函数或进行可能失败的操作

```cpp
// ✓ 正确示例：简单的构造函数
class MyClass {
public:
    MyClass(int value) : value_(value) {
        // 仅进行简单的初始化
    }
    
private:
    int value_;
};

// ✓ 正确示例：使用工厂函数处理可能失败的初始化
class DatabaseConnection {
public:
    // 静态工厂函数
    static std::unique_ptr<DatabaseConnection> Create(const std::string& db_path) {
        auto conn = std::make_unique<DatabaseConnection>();
        if (!conn->Connect(db_path)) {
            return nullptr;
        }
        return conn;
    }
    
private:
    DatabaseConnection() = default;
    bool Connect(const std::string& db_path);
};

// ✗ 错误示例：构造函数中调用虚函数
class Base {
public:
    Base() {
        Init();  // 错误：虚函数在基类构造函数中不会调用派生类实现
    }
    virtual void Init() {
        // 基类初始化
    }
};

class Derived : public Base {
public:
    void Init() override {
        // 这个不会被 Base() 调用！
    }
};

// ✗ 错误示例：构造函数中进行可能失败的操作
class FileReader {
public:
    FileReader(const std::string& path) {
        // 错误：文件打开可能失败，但构造函数无法返回错误
        file_.open(path);
        if (!file_.is_open()) {
            // 怎么处理错误？
        }
    }
private:
    std::ifstream file_;
};
```

**建议：** 如果初始化可能失败，使用工厂函数或 `Init()` 方法。

---

### 隐式转换

**规则：** 单参数构造函数和类型转换运算符必须声明为 `explicit`

```cpp
// ✓ 正确示例：explicit 构造函数
class String {
public:
    explicit String(const char* s);  // explicit 防止隐式转换
    
    // 允许隐式转换（显式说明意图）
    String(String&& other) noexcept;
};

// ✓ 正确示例：explicit 类型转换运算符
class BigInteger {
public:
    explicit operator int() const;  // explicit 防止意外转换
};

void ProcessString(const String& s);

// ✗ 错误示例：允许隐式转换
class String {
public:
    String(const char* s);  // 危险：允许隐式转换
};

void ProcessString(const String& s);
ProcessString("hello");  // 隐式转换，可能不是预期行为

// ✗ 错误示例：意外转换
class BigInteger {
public:
    operator int() const;  // 危险：允许隐式转换
};

BigInteger big = 12345;
int x = big;  // 隐式转换，可能丢失精度
```

**例外：** 当类型转换是类型设计的核心目的时，可以允许隐式转换（如 `string_view` 包装器）。

---

### 拷贝与移动

**规则：** 明确声明类的拷贝/移动语义

```cpp
// ✓ 正确示例：可拷贝类型
class CopyableClass {
public:
    CopyableClass(const CopyableClass& other) = default;
    CopyableClass& operator=(const CopyableClass& other) = default;
    
    CopyableClass(CopyableClass&& other) noexcept = default;
    CopyableClass& operator=(CopyableClass&& other) noexcept = default;
};

// ✓ 正确示例：仅可移动类型
class MoveOnlyClass {
public:
    MoveOnlyClass(const MoveOnlyClass&) = delete;
    MoveOnlyClass& operator=(const MoveOnlyClass&) = delete;
    
    MoveOnlyClass(MoveOnlyClass&& other) noexcept = default;
    MoveOnlyClass& operator=(MoveOnlyClass&& other) noexcept = default;
};

// ✓ 正确示例：不可拷贝/移动类型
class NonCopyableClass {
public:
    NonCopyableClass(const NonCopyableClass&) = delete;
    NonCopyableClass& operator=(const NonCopyableClass&) = delete;
    
    NonCopyableClass(NonCopyableClass&&) = delete;
    NonCopyableClass& operator=(NonCopyableClass&&) = delete;
};

// ✗ 错误示例：不明确声明拷贝/移动语义
class UnclearClass {
    // 编译器自动生成，但意图不明确
};
```

**建议：** 使用 `= default` 或 `= delete` 明确表达意图。

---

### 继承

**规则：** 优先使用组合而非继承；使用 `public` 继承；重写虚函数时使用 `override` 或 `final`

```cpp
// ✓ 正确示例：组合优于继承
class Engine {
public:
    virtual void Start() = 0;
};

class Car {
public:
    explicit Car(std::unique_ptr<Engine> engine)
        : engine_(std::move(engine)) {}
    
    void Start() { engine_->Start(); }
    
private:
    std::unique_ptr<Engine> engine_;  // 组合
};

// ✓ 正确示例：public 继承
class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void Move() = 0;
};

class Car : public Vehicle {  // public 继承
public:
    void Move() override {  // override 关键字
        // 实现
    }
};

// ✓ 正确示例：final 防止进一步继承
class FinalClass final : public Base {
    // ...
};

class FinalMethod : public Base {
public:
    void Move() final {  // final 防止派生类重写
        // ...
    }
};

// ✗ 错误示例：private 继承（应使用组合）
class Car : private Engine {  // 不推荐
    // ...
};

// ✗ 错误示例：缺少 override
class Car : public Vehicle {
public:
    void Move() {  // 错误：缺少 override
        // ...
    }
};

// ✗ 错误示例：非虚析构函数
class Base {
public:
    ~Base() {  // 错误：应该是虚析构函数
        // ...
    }
};
```

---

### 运算符重载

**规则：** 仅在运算符含义明确、符合直觉时才重载

```cpp
// ✓ 正确示例：有意义的运算符重载
class Vector2D {
public:
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }
    
    double& operator[](int index) {
        return index == 0 ? x : y;
    }
    
private:
    double x, y;
};

// ✓ 正确示例：输出流运算符
std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
    return os << "(" << vec.x << ", " << vec.y << ")";
}

// ✗ 错误示例：禁止重载的运算符
class MyClass {
public:
    void operator&&(const MyClass& other);  // 错误：禁止重载 &&
    void operator||(const MyClass& other);  // 错误：禁止重载 ||
    void operator,(const MyClass& other);   // 错误：禁止重载逗号
    void* operator&();                       // 错误：禁止重载一元 &
};

// ✗ 错误示例：含义不明确的运算符
class Temperature {
public:
    Temperature operator+(const Temperature& other) const {
        // 温度相加有意义吗？可能是错误的设计
        return Temperature(celsius + other.celsius);
    }
    
    Temperature operator/(const Temperature& other) const {
        // 温度相除更没有意义
        return Temperature(celsius / other.celsius);
    }
    
private:
    double celsius;
};

// ✗ 错误示例：用户定义字面量
Temperature operator"" _deg(long double value) {
    return Temperature(value);  // 错误：禁止使用用户定义字面量
}
```

---

### 声明顺序

**规则：** 类成员按以下顺序分组声明：

1. 类型别名（`using`、`typedef`）
2. 静态常量
3. 工厂函数
4. 构造函数和赋值运算符
5. 析构函数
6. 其他函数（静态函数、普通函数）
7. 数据成员（静态数据成员、普通数据成员）

```cpp
// ✓ 正确示例：正确的声明顺序
class MyClass {
public:
    // 1. 类型别名
    using ValueType = int;
    using iterator = ValueType*;
    
    // 2. 静态常量
    static constexpr int kMaxSize = 100;
    static const std::string kDefaultName;
    
    // 3. 工厂函数
    static std::unique_ptr<MyClass> Create();
    
    // 4. 构造函数和赋值运算符
    MyClass();
    explicit MyClass(int value);
    MyClass(const MyClass& other);
    MyClass(MyClass&& other) noexcept;
    MyClass& operator=(const MyClass& other);
    MyClass& operator=(MyClass&& other) noexcept;
    
    // 5. 析构函数
    ~MyClass();
    
    // 6. 其他函数
    void DoSomething();
    int GetValue() const;
    
private:
    // 7. 数据成员
    static int instance_count_;
    int value_;
    std::string name_;
};

// ✗ 错误示例：声明顺序混乱
class BadClass {
    int value_;              // 错误：数据成员应该在最后
public:
    void DoSomething();      // 错误：函数应该分组
    using ValueType = int;   // 错误：类型别名应该在最前面
    static constexpr int kMax = 100;
};
```

---

## 函数设计规范

### 参数与返回值

**规则：** 优先使用返回值而非输出参数；输入参数使用值或 `const` 引用，输出参数使用指针或引用

```cpp
// ✓ 正确示例：使用返回值
std::string GetName(int user_id);
std::vector<int> GetNumbers();

// ✓ 正确示例：输入参数使用 const 引用
void ProcessString(const std::string& input);

// ✓ 正确示例：输出参数使用指针（明确表示会修改）
void GetName(int user_id, std::string* output);

// ✓ 正确示例：可选参数使用 std::optional
std::optional<int> FindValue(const std::string& key);

// ✓ 正确示例：多个返回值使用结构体
struct SearchResult {
    bool found;
    int value;
    std::string message;
};
SearchResult Search(const std::string& query);

// ✓ 正确示例：C++17 结构化绑定
struct Point { double x, y; };
Point GetPoint();
auto [x, y] = GetPoint();  // C++17

// ✗ 错误示例：输出参数使用引用（不够明确）
void GetName(int user_id, std::string& output);  // 容易误认为是输入参数

// ✗ 错误示例：可选参数使用指针（不够安全）
int* FindValue(const std::string& key);  // 可能返回 nullptr

// ✗ 错误示例：过多的输出参数
void GetData(int id, std::string* name, int* age, 
             std::string* address, int* score);  // 难以理解
```

---

### 函数重载

**规则：** 仅在读者能直观理解调用意图时才使用重载

```cpp
// ✓ 正确示例：直观的重载
void Log(int value);
void Log(double value);
void Log(const std::string& value);

// ✓ 正确示例：参数类型完全不同
class String {
public:
    String(const char* s);
    String(const std::string& s);
};

// ✗ 错误示例：语义模糊的重载
class DataProcessor {
public:
    void Process(int value);      // 处理数值
    void Process(const char* s);  // 处理字符串？还是从字符串解析数值？
};

// ✗ 错误示例：容易混淆的重载
void AddItem(const std::string& name);
void AddItem(int id);
void AddItem(const char* name);  // 与 string 版本冲突？

// ✓ 更好的设计：使用不同名称
void AddItemByName(const std::string& name);
void AddItemById(int id);
```

---

### 默认参数

**规则：** 允许在非虚函数上使用默认参数，但需确保默认值始终相同

```cpp
// ✓ 正确示例：非虚函数使用默认参数
class HttpClient {
public:
    HttpResponse Get(const std::string& url,
                     int timeout_ms = 5000,
                     int retry_count = 3);
};

// ✓ 正确示例：构造函数使用默认参数
class Rectangle {
public:
    Rectangle(double width, double height, 
              const std::string& color = "white");
};

// ✗ 错误示例：虚函数使用默认参数
class Shape {
public:
    virtual void Draw(const std::string& color = "black") = 0;  // 危险
};

class Circle : public Shape {
public:
    void Draw(const std::string& color = "red") override;  // 不同的默认值
};

// 调用示例
Shape* shape = new Circle();
shape->Draw();  // 使用哪个默认值？"black" 还是 "red"？

// ✗ 错误示例：默认值在头文件和实现文件不一致
// 头文件
void Function(int value = 10);

// 实现文件
void Function(int value = 20) {  // 错误：不同的默认值
    // ...
}
```

---

## 作用域管理

### 命名空间

**规则：** 所有代码都应置于命名空间内；禁止使用 `using namespace` 指令

```cpp
// ✓ 正确示例：使用命名空间
namespace my_project {
namespace geometry {

class Point {
    // ...
};

class Vector {
    // ...
};

}  // namespace geometry
}  // namespace my_project

// ✓ 正确示例：使用完整限定名
void Function() {
    my_project::geometry::Point p;
    my_project::geometry::Vector v;
}

// ✓ 正确示例：.cc 文件中的命名空间别名
namespace geo = my_project::geometry;  // 仅在 .cc 文件中

void Function() {
    geo::Point p;
}

// ✗ 错误示例：在头文件中使用 using namespace
// my_header.h
using namespace std;  // 错误：污染所有包含此头文件的代码

class MyClass {
    string name_;  // 看起来像 std::string，但不明确
};

// ✗ 错误示例：在头文件中使用命名空间别名
// my_header.h
namespace geo = my_project::geometry;  // 错误：可能在其他代码中引起冲突

// ✗ 错误示例：内联命名空间
inline namespace v1 {  // 错误：禁止使用内联命名空间
    class MyClass { };
}
```

---

### 局部变量

**规则：** 将变量置于最窄的作用域内，并在声明时初始化

```cpp
// ✓ 正确示例：最窄作用域 + 初始化
int main() {
    for (int i = 0; i < 10; ++i) {
        std::string name = GetName(i);  // 循环内声明
        Process(name);
    }
}

// ✓ 正确示例：声明时初始化
int count = 0;
std::string name = "default";
std::vector<int> numbers = {1, 2, 3, 4, 5};

// ✗ 错误示例：作用域过宽
int main() {
    int i;  // 错误：声明在循环外
    std::string name;  // 错误：声明在循环外
    
    for (i = 0; i < 10; ++i) {
        name = GetName(i);
        Process(name);
    }
}

// ✗ 错误示例：未初始化
int count;  // 错误：未初始化
std::string name;  // 虽然会默认构造，但不够明确
```

---

## 最佳实践

### 所有权与智能指针

**规则：** 优先使用 `std::unique_ptr` 明确独占所有权；仅在共享不可变对象时使用 `std::shared_ptr`

```cpp
// ✓ 正确示例：unique_ptr 表示独占所有权
class Document {
public:
    explicit Document(std::unique_ptr<Page> first_page)
        : first_page_(std::move(first_page)) {}
    
private:
    std::unique_ptr<Page> first_page_;
};

// ✓ 正确示例：工厂函数返回 unique_ptr
class Widget {
public:
    static std::unique_ptr<Widget> Create(int type) {
        return std::make_unique<Widget>(type);
    }
};

// ✓ 正确示例：shared_ptr 表示共享所有权
class SharedCache {
public:
    std::shared_ptr<const Data> GetData(const std::string& key) {
        if (cache_.find(key) != cache_.end()) {
            return cache_[key];  // 共享数据
        }
        auto data = std::make_shared<Data>(LoadData(key));
        cache_[key] = data;
        return data;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<const Data>> cache_;
};

// ✓ 正确示例：使用原始指针表示非所有权
void ProcessWidget(const Widget* widget) {
    // 不拥有 widget，只是使用它
    widget->DoSomething();
}

// ✗ 错误示例：使用 auto_ptr
std::auto_ptr<Widget> widget(new Widget());  // 错误：已废弃

// ✗ 错误示例：过度使用 shared_ptr
class Document {
public:
    std::shared_ptr<Page> GetPage(int index);  // 错误：应该是 unique_ptr 或原始指针
};
```

---

### 常量与 constexpr

**规则：** 尽可能使用 `const`；编译期已知的常量使用 `constexpr`

```cpp
// ✓ 正确示例：使用 const
class Calculator {
public:
    int Multiply(int a, int b) const {  // const 成员函数
        return a * b;
    }
    
private:
    const std::string name_;  // const 数据成员
};

void Function() {
    const int kBufferSize = 1024;  // const 局部变量
    const std::string kMessage = "Hello";
}

// ✓ 正确示例：使用 constexpr（编译期常量）
constexpr int kDaysInWeek = 7;
constexpr double kPi = 3.14159265358979323846;

constexpr int Square(int x) {
    return x * x;
}

constexpr int result = Square(5);  // 编译期计算

// ✓ 正确示例：constexpr 构造函数
class Point {
public:
    constexpr Point(double x, double y) : x_(x), y_(y) {}
    constexpr double x() const { return x_; }
    constexpr double y() const { return y_; }
    
private:
    double x_, y_;
};

constexpr Point origin(0.0, 0.0);  // 编译期初始化

// ✗ 错误示例：可变值使用 constexpr
int runtime_value = GetValue();
constexpr int result = runtime_value;  // 错误：运行时值不能用于 constexpr

// ✗ 错误示例：忽略 const
class BadClass {
public:
    int GetValue() {  // 错误：应该是 const 成员函数
        return value_;
    }
};
```

---

### 类型推导 auto

**规则：** 可以使用 `auto`，但需确保类型清晰可读

```cpp
// ✓ 正确示例：类型明确或冗长
auto iter = my_map.find(key);  // 类型明确
auto result = std::make_unique<Widget>();  // 类型冗长
auto lambda = [](int x) { return x * 2; };  // lambda 类型复杂

// ✓ 正确示例：避免重复类型名
auto widget = std::make_unique<Widget>();  // 比 std::unique_ptr<Widget> 更清晰

// ✓ 正确示例：范围 for 循环
for (const auto& item : container) {
    Process(item);
}

// ✗ 错误示例：类型不清晰
auto x = GetWidget();  // x 是什么类型？
auto y = Compute();    // y 是什么类型？

// ✗ 错误示例：使用 auto 隐藏意图
auto value = 3.14;     // 应该明确写 double value = 3.14;
auto name = "hello";   // 应该明确写 const char* name = "hello";

// ✓ 更好的方式：仅在不必要时使用 auto
Widget* widget = GetWidget();      // 明确类型
double value = Compute();          // 明确类型
auto iter = my_map.find(key);      // 类型明确，使用 auto 简化
```

---

### 注释规范

**规则：** 使用 `//` 或 `/* */` 注释，保持统一

```cpp
// ✓ 正确示例：单行注释使用 //
// 计算两点之间的距离
double CalculateDistance(const Point& p1, const Point& p2);

// ✓ 正确示例：多行注释使用 //
// 这是一个复杂函数的注释
// 第一行说明
// 第二行说明
void ComplexFunction();

// ✓ 正确示例：文件注释
// Copyright 2025 Google LLC
// Author: John Doe
// Date: 2025-03-16
//
// This file contains the implementation of the geometry library.

// ✓ 正确示例：类注释
// Represents a 2D point in Cartesian coordinates.
// 
// Example:
//   Point p(1.0, 2.0);
//   double distance = p.DistanceTo(origin);
class Point {
    // ...
};

// ✓ 正确示例：函数注释
// Calculates the area of a rectangle.
//
// Args:
//   width: The width of the rectangle (must be positive)
//   height: The height of the rectangle (must be positive)
//
// Returns:
//   The area of the rectangle
//
// Throws:
//   std::invalid_argument if width or height is negative
double CalculateArea(double width, double height);

// ✓ 正确示例：TODO 注释
// TODO(johndoe): Optimize this algorithm for large datasets
// TODO(bugs_12345): Fix memory leak in edge case

// ✗ 错误示例：注释风格不一致
/* 这是一个注释 */
// 另一个注释  // 错误：风格不一致

// ✗ 错误示例：无意义的注释
int x = 10;  // 将 x 设置为 10（无意义的注释）

// ✗ 错误示例：过时的注释
// 这个函数返回 void（实际上返回 int）
int Function();
```

---

## 禁止使用的特性

以下 C++ 特性在 Google 代码中**禁止使用**：

### 1. 异常

**禁止原因：** 异常会增加代码复杂性和二进制大小，影响性能。

```cpp
// ✗ 错误示例：使用异常
void Function() {
    try {
        DoSomething();
    } catch (const std::exception& e) {
        // 错误：禁止使用异常
    }
}

// ✓ 正确示例：使用错误码或 Result 类型
absl::Status Function() {
    if (!DoSomething()) {
        return absl::InternalError("Operation failed");
    }
    return absl::OkStatus();
}

// ✓ 正确示例：使用 std::optional
std::optional<int> FindValue(const std::string& key) {
    if (key.empty()) {
        return std::nullopt;  // 表示未找到
    }
    return ComputeValue(key);
}
```

---

### 2. RTTI（运行时类型信息）

**禁止原因：** RTTI 会增加二进制大小，且通常表明设计问题。

```cpp
// ✗ 错误示例：使用 dynamic_cast
Base* base = new Derived();
Derived* derived = dynamic_cast<Derived*>(base);  // 错误：禁止使用 RTTI

// ✗ 错误示例：使用 typeid
if (typeid(*obj) == typeid(Derived)) {  // 错误：禁止使用 RTTI
    // ...
}

// ✓ 正确示例：使用虚函数或访问者模式
class Base {
public:
    virtual ~Base() = default;
    virtual void Accept(Visitor* visitor) = 0;
};

class Derived : public Base {
public:
    void Accept(Visitor* visitor) override {
        visitor->Visit(this);  // 编译期类型安全
    }
};
```

---

### 3. 用户定义字面量

**禁止原因：** 容易引起混淆，不符合 Google 代码风格。

```cpp
// ✗ 错误示例：用户定义字面量
long double operator"" _deg(long double value) {
    return value * 3.14159 / 180.0;
}

long double angle = 90.0_deg;  // 错误：禁止使用

// ✓ 正确示例：使用普通函数
double DegreesToRadians(double degrees) {
    return degrees * 3.14159 / 180.0;
}

double angle = DegreesToRadians(90.0);
```

---

### 4. 流水线运算符 `|>`

**禁止原因：** C++23 特性，当前目标版本为 C++20。

```cpp
// ✗ 错误示例：使用 |>
auto result = data
    |> Transform(%)
    |> Filter(%)
    |> Collect(%);  // 错误：C++23 特性

// ✓ 正确示例：链式调用
auto result = Collect(Filter(Transform(data)));
```

---

## 代码检查工具

### cpplint

Google 提供的 C++ 代码风格检查工具：

```bash
# 安装
pip install cpplint

# 检查单个文件
cpplint my_file.cpp

# 检查整个项目
cpplint --recursive src/

# 忽略某些检查
cpplint --filter=-legal/copyright my_file.cpp
```

### clang-format

自动格式化工具，支持 Google 风格：

```bash
# 安装
# macOS: brew install clang-format
# Linux: sudo apt install clang-format
# Windows: choco install llvm

# 格式化单个文件
clang-format -i -style=Google my_file.cpp

# 格式化整个项目
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=Google

# 创建配置文件
clang-format -style=Google -dump-config > .clang-format
```

### .clang-format 配置文件

```yaml
BasedOnStyle: Google
IndentWidth: 2
ColumnLimit: 80
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false
```

---

## 附录：命名规范速查表

| 类型 | 命名风格 | 示例 |
|------|---------|------|
| 文件 | 小写 + 下划线/连字符 | `my_class.cpp`, `my-class.cpp` |
| 类/结构体 | PascalCase | `MyClass`, `UrlTable` |
| 函数 | PascalCase | `CalculateArea()`, `GetUrl()` |
| 变量 | snake_case | `table_name`, `num_errors` |
| 类成员变量 | snake_case + 下划线后缀 | `name_`, `num_entries_` |
| 全局变量 | g_ + snake_case | `g_instance_count` |
| 常量 | k + PascalCase 或全大写 | `kDaysInWeek`, `MAX_BUFFER_SIZE` |
| 命名空间 | 小写 | `geometry`, `my_project::utils` |
| 枚举类型 | PascalCase | `HttpStatusCode` |
| 枚举值 | k + PascalCase | `kOk`, `kNotFound` |
| 宏 | 全大写 + 下划线 | `MY_MACRO`, `PI_ROUNDED` |

---

## 总结

Google C++ Style Guide 的核心目标是提高代码的**可读性**、**可维护性**和**一致性**。通过遵循本指南，您可以：

1. 编写清晰、易读的代码
2. 减少常见的编程错误
3. 提高团队协作效率
4. 利用现代 C++ 的优势（智能指针、`constexpr` 等）

**关键要点：**
- 使用现代 C++20 特性
- 遵循统一的命名规范（PascalCase、snake_case）
- 保持一致的代码格式（80 字符、2 空格缩进）
- 优先使用 `const` 和 `constexpr`
- 使用智能指针管理所有权
- 避免异常、RTTI 等复杂特性

**参考资源：**
- [Google C++ Style Guide 官方文档](https://google.github.io/styleguide/cppguide.html)
- [Google C++ Style Guide 中文翻译](https://cppguide.cn/)
- [cpplint 工具](https://github.com/google/styleguide/tree/master/cpplint)
- [clang-format 工具](https://clang.llvm.org/docs/ClangFormat.html)
