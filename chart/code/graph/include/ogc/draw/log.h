#ifndef OGC_DRAW_LOG_H
#define OGC_DRAW_LOG_H

#include "ogc/draw/export.h"
#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace ogc {
namespace draw {

enum class LogLevel {
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kWarning = 3,
    kError = 4,
    kFatal = 5,
    kNone = 6
};

class OGC_GRAPH_API Logger {
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
    
    void Flush();
    void Close();
    
    static std::string LevelToString(LogLevel level);
    static LogLevel StringToLevel(const std::string& str);

private:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void WriteLog(LogLevel level, const std::string& message);
    std::string GetTimestamp() const;
    
    LogLevel m_level;
    std::ofstream m_file;
    std::string m_filepath;
    bool m_consoleOutput;
    std::mutex m_mutex;
};

class OGC_GRAPH_API LogHelper {
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

#define LOG_TRACE() LogHelper(LogLevel::kTrace)
#define LOG_DEBUG() LogHelper(LogLevel::kDebug)
#define LOG_INFO() LogHelper(LogLevel::kInfo)
#define LOG_WARNING() LogHelper(LogLevel::kWarning)
#define LOG_ERROR() LogHelper(LogLevel::kError)
#define LOG_FATAL() LogHelper(LogLevel::kFatal)

}  
}  

#endif  
