# 构建配置避坑清单

> **详细文档**: @doc/experience_standard.md

---

## 一、CMake配置

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| CMAKE-01 | VS多配置生成器需配置特定变量 | 设置`CMAKE_*_OUTPUT_DIRECTORY_RELEASE`等 |
| CMAKE-02 | 动态库输出目录同时设置RUNTIME、LIBRARY、ARCHIVE | 确保所有输出类型路径一致 |
| CMAKE-03 | 测试链接库确保路径一致 | 库输出路径与链接路径一致 |
| CMAKE-04 | 使用target_link_libraries链接依赖 | 避免手动设置链接路径 |

**CMake模板**:
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
```

---

## 二、测试配置

| 规则ID | 规则内容 | 说明 |
|--------|----------|------|
| TEST-01 | 使用gtest_main | 移除自定义main函数，使用gtest_main库 |
| UT-01 | 使用正确的API方法名 | 参考API命名对照表 |
| UT-02 | 抽象基类创建派生类 | 无法直接实例化抽象类 |
| UT-03 | 测试边界条件 | 空几何、空集合、边界值 |
| UT-04 | 测试错误处理 | 异常、错误返回值 |
| UT-05 | 并发测试禁用或特殊处理 | 避免竞态条件 |

---

## 三、常见错误

| 错误类型 | 原因 | 解决方法 |
|----------|------|----------|
| 无法解析的外部符号 | 库未链接或路径错误 | 检查CMAKE-02, CMAKE-04 |
| 重复定义 | main函数重复或符号冲突 | 使用gtest_main (TEST-01) |
| 找不到库文件 | 输出目录配置错误 | 检查CMAKE-01, CMAKE-02 |
| DLL导出问题 | 导出宏配置错误 | 检查DLL-01~04 |
