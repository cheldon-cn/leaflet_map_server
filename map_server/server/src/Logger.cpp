#include "Logger.h"
#include <iostream>
#include <ctime>

namespace cycle {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
}

Logger::~Logger() {
    if (m_fileStream && m_fileStream->is_open()) {
        m_fileStream->close();
    }
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_level = level;
}

void Logger::SetLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fileStream && m_fileStream->is_open()) {
        m_fileStream->close();
    }
    m_fileStream = std::make_unique<std::ofstream>(filePath, std::ios::app);
    if (!m_fileStream->is_open()) {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
        m_fileStream.reset();
    }
}

void Logger::EnableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleOutput = enable;
}

void Logger::Log(LogLevel level, const std::string& message,
                 const char* file, int line) {
    if (level < m_level) {
        return;
    }

    std::string formatted = FormatMessage(level, message, file, line);

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_consoleOutput) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    }

    if (m_fileStream && m_fileStream->is_open()) {
        *m_fileStream << formatted << std::endl;
        m_fileStream->flush();
    }
}

void Logger::LogTrace(const std::string& msg, const char* file, int line) {
    Log(LogLevel::TRACE, msg, file, line);
}

void Logger::LogDebug(const std::string& msg, const char* file, int line) {
    Log(LogLevel::DEBUG, msg, file, line);
}

void Logger::LogInfo(const std::string& msg, const char* file, int line) {
    Log(LogLevel::INFO, msg, file, line);
}

void Logger::LogWarn(const std::string& msg, const char* file, int line) {
    Log(LogLevel::WARN, msg, file, line);
}

void Logger::LogError(const std::string& msg, const char* file, int line) {
    Log(LogLevel::ERROR, msg, file, line);
}

void Logger::LogFatal(const std::string& msg, const char* file, int line) {
    Log(LogLevel::FATAL, msg, file, line);
}

std::string Logger::FormatMessage(LogLevel level, const std::string& message,
                                 const char* file, int line) {
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "]"
        << " [" << GetThreadId() << "]"
        << " [" << GetLevelString(level) << "]";

    if (file && line > 0) {
        // 只显示文件名，不显示完整路径
        const char* filename = file;
        const char* lastSlash = file;
        while (*filename) {
            if (*filename == '/' || *filename == '\\') {
                lastSlash = filename + 1;
            }
            ++filename;
        }
        oss << " [" << lastSlash << ":" << line << "]";
    }

    oss << " " << message;
    return oss.str();
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::GetThreadId() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

std::string Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

// LogStream 实现
LogStream::LogStream(LogLevel level, const char* file, int line)
    : m_level(level), m_file(file), m_line(line) {
}

LogStream::~LogStream() {
    Logger::GetInstance().Log(m_level, m_stream.str(), m_file, m_line);
}

} // namespace cycle
