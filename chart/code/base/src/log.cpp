#include "ogc/base/log.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace ogc {
namespace base {

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_level(LogLevel::kInfo)
    , m_consoleOutput(true) {
}

Logger::~Logger() {
    Close();
}

void Logger::SetLevel(LogLevel level) {
    m_level = level;
}

LogLevel Logger::GetLevel() const {
    return m_level;
}

void Logger::SetLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file.close();
    }
    m_filepath = filepath;
    m_file.open(filepath, std::ios::app);
}

void Logger::SetConsoleOutput(bool enable) {
    m_consoleOutput = enable;
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
    if (level < m_level) {
        return;
    }
    WriteLog(level, message);
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file.flush();
    }
    if (m_consoleOutput) {
        std::cout.flush();
    }
}

void Logger::Close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file.close();
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

void Logger::WriteLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] "
        << "[" << LevelToString(level) << "] "
        << message << std::endl;
    
    std::string logLine = oss.str();
    
    if (m_consoleOutput) {
        if (level >= LogLevel::kError) {
            std::cerr << logLine;
        } else {
            std::cout << logLine;
        }
    }
    
    if (m_file.is_open()) {
        m_file << logLine;
    }
}

std::string Logger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
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
