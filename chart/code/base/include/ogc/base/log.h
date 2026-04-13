#ifndef OGC_BASE_LOG_H
#define OGC_BASE_LOG_H

#include "export.h"
#include <string>
#include <fstream>
#include <mutex>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cstdio>
#include <cstdarg>

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

template<typename... Args>
std::string FormatString(const char* format, Args... args) {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), format, args...);
    return std::string(buffer);
}

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
    
    void Log(LogLevel level, const std::string& message);
    void LogWithLocation(LogLevel level, const char* file, int line, 
                         const char* func, const std::string& message);
    
    void Flush();
    void Close();
    
    static std::string LevelToString(LogLevel level);
    static LogLevel StringToLevel(const std::string& str);

private:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class OGC_BASE_API LogHelper {
public:
    LogHelper(LogLevel level);
    ~LogHelper();
    
    LogHelper& operator<<(const std::string& msg);
    LogHelper& operator<<(const char* msg);
    LogHelper& operator<<(int value);
    LogHelper& operator<<(double value);
    
private:
    LogLevel m_level;
    std::string m_buffer;
};

}  
}  
#define LOG_TRACE() ogc::base::LogHelper(ogc::base::LogLevel::kTrace)
#define LOG_DEBUG() ogc::base::LogHelper(ogc::base::LogLevel::kDebug)
#define LOG_INFO() ogc::base::LogHelper(ogc::base::LogLevel::kInfo)
#define LOG_WARNING() ogc::base::LogHelper(ogc::base::LogLevel::kWarning)
#define LOG_ERROR() ogc::base::LogHelper(ogc::base::LogLevel::kError)
#define LOG_FATAL() ogc::base::LogHelper(ogc::base::LogLevel::kFatal)

#define LOG_TRACE_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kTrace, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_DEBUG_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kDebug, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_INFO_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kInfo, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_WARN_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kWarning, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_ERROR_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kError, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_FATAL_FMT(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kFatal, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))


#endif  
