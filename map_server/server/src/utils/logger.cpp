#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "../config/config.h"

namespace cycle {

std::ofstream Logger::logFile_;
LogLevel Logger::minLevel_ = LogLevel::DEBUG;
bool Logger::consoleOutput_ = true;
bool Logger::rotate_ = true;
size_t Logger::maxSize_ = 16 * 1024 * 1024;
int Logger::maxFiles_ = 5;
std::string Logger::logFilePath_ = "./logs/server.log";
std::mutex Logger::mutex_;
std::atomic<bool> Logger::initialized_(false);

void Logger::Init(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return;
    }
    
    minLevel_ = static_cast<LogLevel>(config.level);
    logFilePath_ = config.file;
    consoleOutput_ = config.console_output;
    rotate_ = config.rotate;
    maxSize_ = config.max_size;
    maxFiles_ = config.max_files;
    
    size_t pos = logFilePath_.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string dir = logFilePath_.substr(0, pos);
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }
    
    logFile_.open(logFilePath_, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFilePath_ << std::endl;
        return;
    }
    
    initialized_ = true;
    
    if (consoleOutput_) {
        std::cout << "[init] Logger initialized successfully" << std::endl;
    }
    if (logFile_.is_open()) {
        logFile_ << "[init] Logger initialized successfully" << std::endl;
    }
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        return;
    }
    
    if (logFile_.is_open()) {
        logFile_ << "[shutdown] Logger shutting down" << std::endl;
    }
    
    if (consoleOutput_) {
        std::cout << "[shutdown] Logger shutting down" << std::endl;
    }
    
    if (logFile_.is_open()) {
        logFile_.flush();
        logFile_.close();
    }
    
    initialized_ = false;
}

void Logger::Trace(const std::string& message) {
    Log(LogLevel::TRACE, message);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warn(const std::string& message) {
    Log(LogLevel::WARN, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERR, message);
}

void Logger::SetLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    minLevel_ = level;
}

LogLevel Logger::GetLevel() {
    return minLevel_;
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.flush();
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_ || level < minLevel_) {
        return;
    }
    
    std::string timestamp = GetCurrentTimestamp();
    std::string levelStr = LevelToString(level);
    std::string formatted = FormatMessage(level, message);
    
    if (consoleOutput_) {
        std::cout << "[" << timestamp << "] " << levelStr << ": " << message << std::endl;
    }
    
    if (logFile_.is_open()) {
        logFile_ << "[" << timestamp << "] " << levelStr << ": " << message << std::endl;
    }
}

std::string Logger::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm tm_buf{};
    std::tm* ptm = &tm_buf;
#ifdef _WIN32
    localtime_s(ptm, &time_t_now);
#else
    localtime_r(&time_t_now, ptm);
#endif
    
    std::stringstream ss;
    ss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::FormatMessage(LogLevel level, const std::string& message) {
    (void)level;
    return message;
}

} 
