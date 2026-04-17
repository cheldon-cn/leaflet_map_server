#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <ogc/base/log.h>
#include <string>

#ifdef LOG_TRACE
#undef LOG_TRACE
#endif
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_WARNING
#undef LOG_WARNING
#endif
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#ifdef LOG_FATAL
#undef LOG_FATAL
#endif
#ifdef LOG_TRACE_FMT
#undef LOG_TRACE_FMT
#endif
#ifdef LOG_DEBUG_FMT
#undef LOG_DEBUG_FMT
#endif
#ifdef LOG_INFO_FMT
#undef LOG_INFO_FMT
#endif
#ifdef LOG_WARN_FMT
#undef LOG_WARN_FMT
#endif
#ifdef LOG_ERROR_FMT
#undef LOG_ERROR_FMT
#endif
#ifdef LOG_FATAL_FMT
#undef LOG_FATAL_FMT
#endif

namespace chart {
namespace parser {

using LogLevel = ogc::base::LogLevel;

constexpr LogLevel Trace = LogLevel::kTrace;
constexpr LogLevel Debug = LogLevel::kDebug;
constexpr LogLevel Info = LogLevel::kInfo;
constexpr LogLevel Warn = LogLevel::kWarning;
constexpr LogLevel Error = LogLevel::kError;
constexpr LogLevel Fatal = LogLevel::kFatal;
constexpr LogLevel Off = LogLevel::kNone;

using Logger = ogc::base::Logger;

using ogc::base::FormatString;

#define LOG_TRACE(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kTrace, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_DEBUG(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kDebug, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_INFO(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kInfo, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_WARN(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kWarning, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_ERROR(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kError, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))
#define LOG_FATAL(...) ogc::base::Logger::Instance().LogWithLocation( \
    ogc::base::LogLevel::kFatal, __FILE__, __LINE__, __func__, \
    ogc::base::FormatString(__VA_ARGS__))

#define LOG_TRACE_FMT(...) LOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG_FMT(...) LOG_DEBUG(__VA_ARGS__)
#define LOG_INFO_FMT(...) LOG_INFO(__VA_ARGS__)
#define LOG_WARN_FMT(...) LOG_WARN(__VA_ARGS__)
#define LOG_ERROR_FMT(...) LOG_ERROR(__VA_ARGS__)
#define LOG_FATAL_FMT(...) LOG_FATAL(__VA_ARGS__)

} // namespace parser
} // namespace chart

#endif // ERROR_HANDLER_H
