#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace chart {
namespace parser {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Fatal = 5,
    Off = 6
};

class Logger {
public:
    static Logger& Instance();
    
    void SetLevel(LogLevel level) { m_level = level; }
    LogLevel GetLevel() const { return m_level; }
    
    void SetLogFile(const std::string& filePath);
    void SetConsoleOutput(bool enable) { m_consoleOutput = enable; }
    
    void Log(LogLevel level, const char* file, int line, const char* func, const std::string& message);
    
private:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string LevelToString(LogLevel level) const;
    std::string GetCurrentTime() const;
    
    LogLevel m_level;
    bool m_consoleOutput;
    std::ofstream m_logFile;
    std::mutex m_mutex;
};

#define LOG_TRACE(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Trace, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))
#define LOG_DEBUG(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Debug, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))
#define LOG_INFO(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Info, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))
#define LOG_WARN(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Warn, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))
#define LOG_ERROR(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Error, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))
#define LOG_FATAL(...) chart::parser::Logger::Instance().Log(chart::parser::LogLevel::Fatal, __FILE__, __LINE__, __func__, chart::parser::FormatString(__VA_ARGS__))

template<typename... Args>
std::string FormatString(const char* format, Args... args) {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), format, args...);
    return std::string(buffer);
}

#define LOG_TRACE_FMT(fmt, ...) LOG_TRACE(FormatString(fmt, __VA_ARGS__))
#define LOG_DEBUG_FMT(fmt, ...) LOG_DEBUG(FormatString(fmt, __VA_ARGS__))
#define LOG_INFO_FMT(fmt, ...) LOG_INFO(FormatString(fmt, __VA_ARGS__))
#define LOG_WARN_FMT(fmt, ...) LOG_WARN(FormatString(fmt, __VA_ARGS__))
#define LOG_ERROR_FMT(fmt, ...) LOG_ERROR(FormatString(fmt, __VA_ARGS__))
#define LOG_FATAL_FMT(fmt, ...) LOG_FATAL(FormatString(fmt, __VA_ARGS__))

} // namespace parser
} // namespace chart

#endif // ERROR_HANDLER_H
