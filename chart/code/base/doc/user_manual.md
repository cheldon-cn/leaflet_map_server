# ogc_base 模块用户手册

> **版本**: v1.0  
> **更新日期**: 2026-04-07  
> **适用范围**: OGC Chart 系统基础工具库

---

## 一、模块描述

ogc_base 是 OGC 图表系统的**基础工具库**，位于系统架构的最底层（Layer 1）。该模块提供全局通用的基础能力，包括日志记录、线程安全工具和性能监控等功能，无任何业务依赖，可被所有其他模块复用。

### 1.1 模块定位

在 OGC Chart 系统架构中，ogc_base 作为最底层模块，为上层所有模块提供基础服务：

```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (navi, alert, alarm)              │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    渲染层 (graph, symbology)                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    数据层 (cache, layer, feature)           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    核心层 (geom, proj, draw)                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    基础层 (ogc_base) ← 当前模块             │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、核心特性

| 特性 | 说明 |
|------|------|
| **日志系统** | 支持多级别日志输出（Trace/Debug/Info/Warning/Error/Fatal），支持文件和控制台输出 |
| **线程安全** | 提供 ThreadSafe<T> 模板类和 ReadWriteLock 读写锁 |
| **性能监控** | 实时监控帧率、内存、渲染性能指标，支持阈值告警 |
| **单例模式** | Logger 和 PerformanceMonitor 采用单例设计 |
| **零依赖** | 仅依赖 C++11 标准库，无外部依赖 |

---

## 三、依赖关系

### 3.1 依赖库

| 依赖类型 | 库名称 | 说明 |
|----------|--------|------|
| 内部依赖 | 无 | 无其他模块依赖 |
| 外部依赖 | C++11 STL | 标准库（mutex, atomic, chrono, fstream, memory） |

### 3.2 被依赖关系

ogc_base 被以下模块依赖：
- ogc_proj（坐标转换）
- ogc_cache（缓存）
- ogc_symbology（符号化）
- ogc_graph（地图渲染）
- ogc_navi（导航）
- ogc_alert（警报）

---

## 四、目录结构

```
base/
├── include/ogc/base/
│   ├── export.h              # DLL导出宏定义
│   ├── log.h                 # 日志系统
│   ├── thread_safe.h         # 线程安全工具
│   ├── performance_metrics.h # 性能指标定义
│   └── performance_monitor.h # 性能监控管理
├── src/
│   ├── log.cpp
│   ├── thread_safe.cpp
│   ├── performance_metrics.cpp
│   └── performance_monitor.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test_log.cpp
│   ├── test_thread_safe.cpp
│   ├── test_performance_metrics.cpp
│   └── test_performance_monitor.cpp
├── doc/
│   └── user_manual.md        # 本文档
└── CMakeLists.txt
```

---

## 五、关键类和主要方法

### 5.1 Logger（日志管理器）

**头文件**: `<ogc/base/log.h>`

```cpp
namespace ogc {
namespace base {

enum class LogLevel {
    kTrace = 0,     // 跟踪级别
    kDebug = 1,     // 调试级别
    kInfo = 2,      // 信息级别
    kWarning = 3,   // 警告级别
    kError = 4,     // 错误级别
    kFatal = 5,     // 致命错误级别
    kNone = 6       // 无日志
};

class OGC_BASE_API Logger {
public:
    static Logger& Instance();
    
    void SetLevel(LogLevel level);
    LogLevel GetLevel() const;
    
    void SetLogFile(const std::string& filepath);
    void SetConsoleOutput(bool enable);
    
    void Trace(const std::string& message);
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    void Flush();
    void Close();
};

// 便捷宏定义
#define LOG_TRACE() LogHelper(LogLevel::kTrace)
#define LOG_DEBUG() LogHelper(LogLevel::kDebug)
#define LOG_INFO() LogHelper(LogLevel::kInfo)
#define LOG_WARNING() LogHelper(LogLevel::kWarning)
#define LOG_ERROR() LogHelper(LogLevel::kError)
#define LOG_FATAL() LogHelper(LogLevel::kFatal)

} // namespace base
} // namespace ogc
```

### 5.2 ThreadSafe<T>（线程安全封装）

**头文件**: `<ogc/base/thread_safe.h>`

```cpp
namespace ogc {
namespace base {

template<typename T>
class ThreadSafe {
public:
    ThreadSafe() = default;
    explicit ThreadSafe(const T& value);
    
    T Get() const;
    void Set(const T& value);
    T Exchange(const T& newValue);
    
    template<typename Func>
    auto WithLock(Func&& func) -> decltype(func(m_value));
    
    std::mutex& GetMutex() const;
};

class ReadWriteLock {
public:
    void ReadLock();
    void ReadUnlock();
    void WriteLock();
    void WriteUnlock();
};

} // namespace base
} // namespace ogc
```

### 5.3 PerformanceMonitor（性能监控）

**头文件**: `<ogc/base/performance_monitor.h>`

```cpp
namespace ogc {
namespace base {

struct PerformanceThreshold {
    double lowFpsThreshold = 15.0;
    double highFpsThreshold = 60.0;
    double memoryWarningThresholdMB = 1024.0;
    double memoryCriticalThresholdMB = 2048.0;
    double frameTimeWarningMs = 33.0;
    double frameTimeCriticalMs = 100.0;
    
    static PerformanceThreshold Default();
    static PerformanceThreshold Strict();
    static PerformanceThreshold Relaxed();
};

enum class PerformanceLevel {
    kExcellent = 0,
    kGood = 1,
    kFair = 2,
    kPoor = 3,
    kCritical = 4
};

class OGC_BASE_API PerformanceMonitor {
public:
    using Ptr = std::shared_ptr<PerformanceMonitor>;
    
    PerformanceMonitor();
    explicit PerformanceMonitor(const PerformanceThreshold& thresholds);
    
    void SetThresholds(const PerformanceThreshold& thresholds);
    void SetAlertCallback(PerformanceAlertCallback callback);
    
    void BeginFrame();
    void EndFrame();
    
    void RecordDrawCall(uint64_t vertexCount, uint64_t triangleCount);
    void RecordMemoryAllocation(double sizeMB, const std::string& category);
    
    PerformanceMetrics GetCurrentMetrics() const;
    PerformanceLevel GetCurrentPerformanceLevel() const;
};

} // namespace base
} // namespace ogc
```

---

## 六、使用示例

### 6.1 日志系统使用

```cpp
#include <ogc/base/log.h>

using namespace ogc::base;

// 初始化日志系统
Logger::Instance().SetLevel(LogLevel::kDebug);
Logger::Instance().SetConsoleOutput(true);
Logger::Instance().SetLogFile("app.log");

// 基本日志输出
Logger::Instance().Info("Application started");
Logger::Instance().Warning("Low memory warning");
Logger::Instance().Error("Failed to load resource: " + filename);

// 使用宏进行流式输出
LOG_INFO() << "Processing " << count << " items";
LOG_ERROR() << "Failed to load file: " << filename;
LOG_DEBUG() << "Variable x = " << x << ", y = " << y;

// 关闭日志
Logger::Instance().Close();
```

### 6.2 线程安全工具使用

```cpp
#include <ogc/base/thread_safe.h>

using namespace ogc::base;

// 线程安全计数器
ThreadSafe<int> counter(0);

// 多线程安全访问
void IncrementCounter() {
    counter.WithLock([](int& v) {
        v++;
    });
}

// 读取值
int GetCounter() {
    return counter.Get();
}

// 读写锁使用
ReadWriteLock rwLock;

void ReadData() {
    rwLock.ReadLock();
    // 读取共享数据...
    rwLock.ReadUnlock();
}

void WriteData() {
    rwLock.WriteLock();
    // 写入共享数据...
    rwLock.WriteUnlock();
}
```

### 6.3 性能监控使用

```cpp
#include <ogc/base/performance_monitor.h>

using namespace ogc::base;

// 创建监控器
auto monitor = std::make_shared<PerformanceMonitor>(PerformanceThreshold::Strict());

// 设置告警回调
monitor->SetAlertCallback([](const std::string& type, const std::string& msg, double value) {
    LOG_WARNING() << "Performance Alert: " << type << " - " << msg << " (" << value << ")";
});

// 帧监控循环
void RenderLoop() {
    while (running) {
        monitor->BeginFrame();
        
        // 渲染代码...
        monitor->RecordDrawCall(vertexCount, triangleCount);
        
        monitor->EndFrame();
        
        // 检查性能
        auto level = monitor->GetCurrentPerformanceLevel();
        if (level == PerformanceLevel::kPoor) {
            LOG_WARNING() << "Performance degradation detected";
        }
    }
}

// 获取性能指标
auto metrics = monitor->GetCurrentMetrics();
std::cout << "FPS: " << metrics.fps.currentFps << std::endl;
std::cout << "Frame Time: " << metrics.fps.frameTimeMs << " ms" << std::endl;
std::cout << "Memory: " << metrics.memory.usedMB << " MB" << std::endl;
```

---

## 七、编译和集成

### 7.1 CMake 配置

```cmake
# 添加 base 模块
add_subdirectory(base)

# 链接 base 库
target_link_libraries(your_target PRIVATE ogc_base)
```

### 7.2 头文件包含

```cpp
#include <ogc/base/log.h>
#include <ogc/base/thread_safe.h>
#include <ogc/base/performance_monitor.h>
```

---

## 八、注意事项

1. **日志级别设置**：生产环境建议设置为 `LogLevel::kInfo` 或更高
2. **线程安全**：ThreadSafe<T> 适用于简单类型，复杂对象建议使用 ReadWriteLock
3. **性能开销**：日志输出有性能开销，高频循环中避免使用低级别日志
4. **单例生命周期**：Logger 和 PerformanceMonitor 在程序结束时自动清理

---

## 九、版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-04-06 | 从 ogc_graph 拆分创建，包含日志、线程安全、性能监控功能 |

---

**文档维护**: OGC Chart 开发团队  
**技术支持**: 参见项目 README.md
