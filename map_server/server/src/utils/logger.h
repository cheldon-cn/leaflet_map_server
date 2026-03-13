#ifndef CYCLE_UTILS_LOGGER_H
#define CYCLE_UTILS_LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include "../config/config.h"

namespace cycle {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERR = 4
};

//struct LogConfig {
//    int level = 1;
//    std::string file = "./logs/server.log";
//    bool console_output = true;
//    bool rotate = true;
//    size_t max_size = 16 * 1024 * 1024;
//    int max_files = 5;
//};

class Logger {
public:
    static void Init(const LogConfig& config);
    static void Shutdown();
    
    static void Trace(const std::string& message);
    static void Debug(const std::string& message);
    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);
    
    static void SetLevel(LogLevel level);
    static LogLevel GetLevel();
    
    static void Flush();
    
private:
    static void Log(LogLevel level, const std::string& message);
    
    static std::string LevelToString(LogLevel level);
    static std::string GetCurrentTimestamp();
    static std::string FormatMessage(LogLevel level, const std::string& message);
    
    static std::ofstream logFile_;
    static LogLevel minLevel_;
    static bool consoleOutput_;
    static bool rotate_;
    static size_t maxSize_;
    static int maxFiles_;
    static std::string logFilePath_;
    static std::mutex mutex_;
    static std::atomic<bool> initialized_;
};

#define LOG_TRACE(msg) cycle::Logger::Trace(msg)
#define LOG_DEBUG(msg) cycle::Logger::Debug(msg)
#define LOG_INFO(msg) cycle::Logger::Info(msg)
#define LOG_WARN(msg) cycle::Logger::Warn(msg)
#define LOG_ERROR(msg) cycle::Logger::Error(msg)

} // namespace cycle

#endif // CYCLE_UTILS_LOGGER_H
