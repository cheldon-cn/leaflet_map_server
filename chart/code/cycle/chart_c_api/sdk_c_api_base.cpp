/**
 * @file sdk_c_api_base.cpp
 * @brief OGC Chart SDK C API - Base Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/base/log.h>
#include <ogc/base/performance_monitor.h>
#include <ogc/base/performance_metrics.h>

#include <cstring>
#include <cstdlib>
#include <string>

using namespace ogc::base;

#ifdef __cplusplus
extern "C" {
#endif

ogc_logger_t* ogc_logger_get_instance(void) {
    return reinterpret_cast<ogc_logger_t*>(&Logger::Instance());
}

void ogc_logger_set_level(ogc_logger_t* logger, int level) {
    if (logger) {
        reinterpret_cast<Logger*>(logger)->SetLevel(static_cast<LogLevel>(level));
    }
}

void ogc_logger_trace(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Trace(std::string(message));
    }
}

void ogc_logger_debug(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Debug(std::string(message));
    }
}

void ogc_logger_info(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Info(std::string(message));
    }
}

void ogc_logger_warning(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Warning(std::string(message));
    }
}

void ogc_logger_error(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Error(std::string(message));
    }
}

void ogc_logger_fatal(ogc_logger_t* logger, const char* message) {
    if (logger && message) {
        reinterpret_cast<Logger*>(logger)->Fatal(std::string(message));
    }
}

void ogc_logger_set_log_file(ogc_logger_t* logger, const char* filepath) {
    if (logger && filepath) {
        reinterpret_cast<Logger*>(logger)->SetLogFile(std::string(filepath));
    }
}

void ogc_logger_set_console_output(ogc_logger_t* logger, int enable) {
    if (logger) {
        reinterpret_cast<Logger*>(logger)->SetConsoleOutput(enable != 0);
    }
}

ogc_performance_monitor_t* ogc_performance_monitor_get_instance(void) {
    static PerformanceMonitor instance;
    return reinterpret_cast<ogc_performance_monitor_t*>(&instance);
}

void ogc_performance_monitor_start_frame(ogc_performance_monitor_t* monitor) {
    if (monitor) {
        reinterpret_cast<PerformanceMonitor*>(monitor)->BeginFrame();
    }
}

void ogc_performance_monitor_end_frame(ogc_performance_monitor_t* monitor) {
    if (monitor) {
        reinterpret_cast<PerformanceMonitor*>(monitor)->EndFrame();
    }
}

double ogc_performance_monitor_get_fps(ogc_performance_monitor_t* monitor) {
    if (monitor) {
        return reinterpret_cast<PerformanceMonitor*>(monitor)->GetCurrentFps();
    }
    return 0.0;
}

long ogc_performance_monitor_get_memory_used(ogc_performance_monitor_t* monitor) {
    if (monitor) {
        return static_cast<long>(reinterpret_cast<PerformanceMonitor*>(monitor)->GetMemoryUsageMB());
    }
    return 0;
}

void ogc_performance_monitor_reset(ogc_performance_monitor_t* monitor) {
    if (monitor) {
        reinterpret_cast<PerformanceMonitor*>(monitor)->ClearAlerts();
    }
}

void ogc_performance_stats_get(ogc_performance_stats_t* stats) {
    if (stats) {
        static PerformanceMonitor monitor;
        stats->fps = monitor.GetCurrentFps();
        stats->frame_time_ms = monitor.GetFrameTimeMs();
        stats->memory_used = static_cast<long>(monitor.GetMemoryUsageMB());
        stats->memory_total = 0;
        stats->render_count = static_cast<int>(monitor.GetDrawCallCount());
        stats->cache_hit_count = 0;
        stats->cache_miss_count = 0;
    }
}

#ifdef __cplusplus
}
#endif
