#include "ogc/base/log.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace ogc {
namespace base {

struct Logger::Impl {
    LogLevel level = LogLevel::kInfo;
    std::ofstream file;
    std::string filepath;
    bool consoleOutput = true;
    std::mutex mutex;
    
    void WriteLog(LogLevel level, const std::string& message);
    void WriteLogWithLocation(LogLevel level, const char* file, int line,
                              const char* func, const std::string& message);
    std::string GetTimestamp() const;
};

void Logger::Impl::WriteLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] "
        << "[" << Logger::LevelToString(level) << "] "
        << message << std::endl;
    
    std::string logLine = oss.str();
    
    if (consoleOutput) {
        if (level >= LogLevel::kError) {
            std::cerr << logLine;
        } else {
            std::cout << logLine;
        }
    }
    
    if (file.is_open()) {
        file << logLine;
    }
}

void Logger::Impl::WriteLogWithLocation(LogLevel level, const char* file, int line,
                                        const char* func, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] "
        << "[" << std::setw(7) << Logger::LevelToString(level) << "] "
        << "[" << file << ":" << line << "] "
        << message << std::endl;
    
    std::string logLine = oss.str();
    
    if (consoleOutput) {
        if (level >= LogLevel::kError) {
            std::cerr << logLine;
        } else {
            std::cout << logLine;
        }
    }
    
    if (this->file.is_open()) {
        this->file << logLine;
    }
}

std::string Logger::Impl::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : impl_(std::make_unique<Impl>()) {
}

Logger::~Logger() {
    Close();
}

void Logger::SetLevel(LogLevel level) {
    impl_->level = level;
}

LogLevel Logger::GetLevel() const {
    return impl_->level;
}

void Logger::SetLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->file.is_open()) {
        impl_->file.close();
    }
    impl_->filepath = filepath;
    impl_->file.open(filepath, std::ios::app);
}

void Logger::SetConsoleOutput(bool enable) {
    impl_->consoleOutput = enable;
}

void Logger::Trace(const std::string& message) {
    Log(LogLevel::kTrace, message);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::kDebug, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::kInfo, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::kWarning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::kError, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::kFatal, message);
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level < impl_->level) {
        return;
    }
    impl_->WriteLog(level, message);
}

void Logger::LogWithLocation(LogLevel level, const char* file, int line, 
                             const char* func, const std::string& message) {
    if (level < impl_->level) {
        return;
    }
    impl_->WriteLogWithLocation(level, file, line, func, message);
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->file.is_open()) {
        impl_->file.flush();
    }
    if (impl_->consoleOutput) {
        std::cout.flush();
    }
}

void Logger::Close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->file.is_open()) {
        impl_->file.close();
    }
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::kTrace: return "TRACE";
        case LogLevel::kDebug: return "DEBUG";
        case LogLevel::kInfo: return "INFO";
        case LogLevel::kWarning: return "WARNING";
        case LogLevel::kError: return "ERROR";
        case LogLevel::kFatal: return "FATAL";
        case LogLevel::kNone: return "NONE";
        default: return "UNKNOWN";
    }
}

LogLevel Logger::StringToLevel(const std::string& str) {
    if (str == "TRACE") return LogLevel::kTrace;
    if (str == "DEBUG") return LogLevel::kDebug;
    if (str == "INFO") return LogLevel::kInfo;
    if (str == "WARNING") return LogLevel::kWarning;
    if (str == "ERROR") return LogLevel::kError;
    if (str == "FATAL") return LogLevel::kFatal;
    return LogLevel::kNone;
}

LogHelper::LogHelper(LogLevel level) : m_level(level) {}

LogHelper::~LogHelper() {
    Logger::Instance().Log(m_level, m_buffer);
}

LogHelper& LogHelper::operator<<(const std::string& msg) {
    m_buffer += msg;
    return *this;
}

LogHelper& LogHelper::operator<<(const char* msg) {
    m_buffer += msg;
    return *this;
}

LogHelper& LogHelper::operator<<(int value) {
    m_buffer += std::to_string(value);
    return *this;
}

LogHelper& LogHelper::operator<<(double value) {
    m_buffer += std::to_string(value);
    return *this;
}

}  
}  
