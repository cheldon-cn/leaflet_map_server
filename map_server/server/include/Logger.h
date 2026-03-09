#pragma once
#include <string>
#include <sstream>
#include <mutex>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <thread>

namespace cycle {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

class Logger {
public:
    static Logger& GetInstance();

    void SetLogLevel(LogLevel level);
    void SetLogFile(const std::string& filePath);
    void EnableConsoleOutput(bool enable);

    void Log(LogLevel level, const std::string& message,
             const char* file = nullptr, int line = 0);

    // 便捷宏定义在 cpp 文件中提供
    void LogTrace(const std::string& msg, const char* file = nullptr, int line = 0);
    void LogDebug(const std::string& msg, const char* file = nullptr, int line = 0);
    void LogInfo(const std::string& msg, const char* file = nullptr, int line = 0);
    void LogWarn(const std::string& msg, const char* file = nullptr, int line = 0);
    void LogError(const std::string& msg, const char* file = nullptr, int line = 0);
    void LogFatal(const std::string& msg, const char* file = nullptr, int line = 0);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string FormatMessage(LogLevel level, const std::string& message,
                             const char* file, int line);
    std::string GetTimestamp();
    std::string GetThreadId();
    std::string GetLevelString(LogLevel level);

private:
    LogLevel m_level = LogLevel::INFO;
    bool m_consoleOutput = true;
    std::unique_ptr<std::ofstream> m_fileStream;
    std::mutex m_mutex;
};

// 格式化日志（支持流式操作）
class LogStream {
public:
    LogStream(LogLevel level, const char* file, int line);
    ~LogStream();

    template<typename T>
    LogStream& operator<<(const T& value) {
        m_stream << value;
        return *this;
    }

private:
    LogLevel m_level;
    const char* m_file;
    int m_line;
    std::ostringstream m_stream;
};

// 日志宏定义
#define LOG_TRACE(msg) \
    cycle::LogStream(cycle::LogLevel::TRACE, __FILE__, __LINE__) << msg

#define LOG_DEBUG(msg) \
    cycle::LogStream(cycle::LogLevel::DEBUG, __FILE__, __LINE__) << msg

#define LOG_INFO(msg) \
    cycle::LogStream(cycle::LogLevel::INFO, __FILE__, __LINE__) << msg

#define LOG_WARN(msg) \
    cycle::LogStream(cycle::LogLevel::WARN, __FILE__, __LINE__) << msg

#define LOG_ERROR(msg) \
    cycle::LogStream(cycle::LogLevel::ERROR, __FILE__, __LINE__) << msg

#define LOG_FATAL(msg) \
    cycle::LogStream(cycle::LogLevel::FATAL, __FILE__, __LINE__) << msg

} // namespace cycle
