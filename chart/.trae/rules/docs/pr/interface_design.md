# 接口设计避坑清单

> **详细文档**: @doc/experience_standard.md

---

## 一、纯虚接口类设计

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| INT-01 | 提供工厂方法 | 纯虚接口类需要静态Create方法 |
| INT-02 | 提供引用计数管理 | 添加ReleaseReference方法 |
| INT-03 | 派生类实现所有纯虚函数 | 使用override关键字确保完整性 |

**接口类模板**:
```cpp
class IExample {
public:
    virtual ~IExample() = default;
    static IExample* Create(const std::string& name);
    virtual void ReleaseReference() = 0;
    virtual const std::string& GetName() const = 0;
};
```

---

## 二、DLL导出配置

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| DLL-01 | 每个模块独立导出宏 | 使用`OGC_XXX_API`而非统一的`OGC_API` |
| DLL-02 | 纯虚接口类需要导出 | 即使是纯虚类也需要添加导出宏 |
| DLL-03 | Pimpl模式类需要导出 | 包含unique_ptr<Impl>的类需要导出 |
| DLL-04 | 静态库特殊处理 | 添加`OGC_XXX_STATIC`宏判断 |

**导出宏模板**:
```cpp
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
```

---

## 三、常见错误

| 错误类型 | 原因 | 解决方法 |
|----------|------|----------|
| 纯虚函数未实现 | 派生类未实现所有纯虚函数 | 使用override关键字确保完整性 |
| DLL导出缺失 | 纯虚接口类未添加导出宏 | 添加`OGC_XXX_API`宏 |
| 工厂方法缺失 | 纯虚接口类无法实例化 | 提供静态Create方法 |
