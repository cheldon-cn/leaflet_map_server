#include "chart_parser/error_handler.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace chart {
namespace parser {

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_level(LogLevel::Info)
    , m_consoleOutput(true) {
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

void Logger::SetLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    
    m_logFile.open(filePath, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
    }
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO";
        case LogLevel::Warn:  return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
        default:              return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::Log(LogLevel level, const char* file, int line, const char* func, const std::string& message) {
    if (level < m_level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::stringstream ss;
    ss << "[" << GetCurrentTime() << "] ";
    ss << "[" << std::setw(5) << LevelToString(level) << "] ";
    ss << "[" << file << ":" << line << "] ";
    ss << message;
    
    if (m_consoleOutput) {
        if (level >= LogLevel::Error) {
            std::cerr << ss.str() << std::endl;
        } else {
            std::cout << ss.str() << std::endl;
        }
    }
    
    if (m_logFile.is_open()) {
        m_logFile << ss.str() << std::endl;
        m_logFile.flush();
    }
}

} // namespace parser
} // namespace chart
