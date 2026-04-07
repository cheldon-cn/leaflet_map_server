# ogc_base 模块 - 头文件索引

## 模块描述

ogc_base 是 OGC 图表系统的**基础工具库**，提供全局通用的基础能力，无业务依赖。作为整个系统的最底层模块，为其他所有模块提供日志记录、线程安全工具和性能监控等基础服务。

## 核心特性

- **日志系统**：支持多级别日志输出（Trace/Debug/Info/Warning/Error/Fatal）
- **线程安全**：提供线程安全封装模板和读写锁
- **性能监控**：实时监控帧率、内存、渲染性能指标
- **单例模式**：Logger 和 PerformanceMonitor 采用单例设计
- **零依赖**：仅依赖 C++11 标准库，无外部依赖

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/base/export.h) | DLL导出宏定义 | `OGC_BASE_API` |
| [log.h](ogc/base/log.h) | 日志系统 | `Logger`, `LogHelper`, `LogLevel` |
| [thread_safe.h](ogc/base/thread_safe.h) | 线程安全工具 | `ThreadSafe<T>`, `ReadWriteLock` |
| [performance_metrics.h](ogc/base/performance_metrics.h) | 性能指标定义 | `FrameMetrics`, `MemoryMetrics`, `FpsMetrics` |
| [performance_monitor.h](ogc/base/performance_monitor.h) | 性能监控管理 | `PerformanceMonitor`, `PerformanceThreshold` |

---

## 类继承关系图

```
PerformanceMonitor
    └── (单例模式)

Logger
    └── (单例模式)

ThreadSafe<T>
    └── (模板类)
```

---

## 依赖关系图

```
ogc_base (无外部依赖)
    │
    └── C++11 标准库
        ├── <mutex>
        ├── <atomic>
        ├── <chrono>
        ├── <fstream>
        └── <memory>
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | log.h, thread_safe.h |
| **Performance** | performance_metrics.h, performance_monitor.h |
| **Build** | export.h |

---

## 关键类

### Logger (日志管理器)

**File**: [log.h](ogc/base/log.h)  
**Description**: 单例模式日志管理器，支持多级别日志输出

```cpp
namespace ogc {
namespace base {

enum class LogLevel {
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kWarning = 3,
    kError = 4,
    kFatal = 5,
    kNone = 6
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

---

### ThreadSafe<T> (线程安全封装)

**File**: [thread_safe.h](ogc/base/thread_safe.h)  
**Description**: 模板类，为任意类型提供线程安全访问

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

---

### PerformanceMonitor (性能监控)

**File**: [performance_monitor.h](ogc/base/performance_monitor.h)  
**Description**: 实时监控渲染性能指标

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

## 类型定义

### LogLevel (日志级别枚举)

```cpp
enum class LogLevel {
    kTrace = 0,     // 跟踪级别
    kDebug = 1,     // 调试级别
    kInfo = 2,      // 信息级别
    kWarning = 3,   // 警告级别
    kError = 4,     // 错误级别
    kFatal = 5,     // 致命错误级别
    kNone = 6       // 无日志
};
```

### PerformanceLevel (性能等级枚举)

```cpp
enum class PerformanceLevel {
    kExcellent = 0,  // 优秀
    kGood = 1,       // 良好
    kFair = 2,       // 一般
    kPoor = 3,       // 较差
    kCritical = 4    // 危险
};
```

---

## 使用示例

### 日志系统使用

```cpp
#include <ogc/base/log.h>

using namespace ogc::base;

// 基本使用
Logger::Instance().SetLevel(LogLevel::kDebug);
Logger::Instance().SetConsoleOutput(true);
Logger::Instance().Info("Application started");

// 使用宏
LOG_INFO() << "Processing " << 100 << " items";
LOG_ERROR() << "Failed to load file: " << "config.json";
```

### 线程安全工具使用

```cpp
#include <ogc/base/thread_safe.h>

using namespace ogc::base;

// 线程安全计数器
ThreadSafe<int> counter(0);

// 多线程安全访问
counter.Set(100);
int value = counter.Get();

// 使用锁进行复杂操作
counter.WithLock([](int& v) {
    v++;
    v *= 2;
});
```

### 性能监控使用

```cpp
#include <ogc/base/performance_monitor.h>

using namespace ogc::base;

// 创建监控器
auto monitor = std::make_shared<PerformanceMonitor>(PerformanceThreshold::Strict());

// 设置告警回调
monitor->SetAlertCallback([](const std::string& type, const std::string& msg, double value) {
    LOG_WARNING() << "Performance Alert: " << type << " - " << msg;
});

// 帧监控
monitor->BeginFrame();
// ... 渲染代码 ...
monitor->EndFrame();

// 获取性能指标
auto metrics = monitor->GetCurrentMetrics();
std::cout << "FPS: " << metrics.fps.currentFps << std::endl;
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | Team | 从 ogc_graph 拆分创建 |

---

**Generated**: 2026-04-07  
**Module Version**: v1.0  
**C++ Standard**: C++11
