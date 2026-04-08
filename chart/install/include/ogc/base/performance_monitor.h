#ifndef OGC_BASE_PERFORMANCE_MONITOR_H
#define OGC_BASE_PERFORMANCE_MONITOR_H

#include "export.h"
#include "performance_metrics.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace ogc {
namespace base {

using PerformanceAlertCallback = std::function<void(const std::string& alertType, const std::string& message, double value)>;

struct PerformanceThreshold {
    double lowFpsThreshold = 15.0;
    double highFpsThreshold = 60.0;
    double memoryWarningThresholdMB = 1024.0;
    double memoryCriticalThresholdMB = 2048.0;
    double frameTimeWarningMs = 33.0;
    double frameTimeCriticalMs = 100.0;
    uint64_t drawCallWarningThreshold = 5000;
    uint64_t drawCallCriticalThreshold = 10000;
    
    static PerformanceThreshold Default() {
        return PerformanceThreshold();
    }
    
    static PerformanceThreshold Strict() {
        PerformanceThreshold t;
        t.lowFpsThreshold = 30.0;
        t.highFpsThreshold = 120.0;
        t.memoryWarningThresholdMB = 512.0;
        t.memoryCriticalThresholdMB = 1024.0;
        t.frameTimeWarningMs = 20.0;
        t.frameTimeCriticalMs = 50.0;
        t.drawCallWarningThreshold = 2000;
        t.drawCallCriticalThreshold = 5000;
        return t;
    }
    
    static PerformanceThreshold Relaxed() {
        PerformanceThreshold t;
        t.lowFpsThreshold = 10.0;
        t.highFpsThreshold = 30.0;
        t.memoryWarningThresholdMB = 2048.0;
        t.memoryCriticalThresholdMB = 4096.0;
        t.frameTimeWarningMs = 50.0;
        t.frameTimeCriticalMs = 200.0;
        t.drawCallWarningThreshold = 10000;
        t.drawCallCriticalThreshold = 20000;
        return t;
    }
};

struct PerformanceAlert {
    std::string type;
    std::string message;
    double value;
    std::chrono::steady_clock::time_point timestamp;
    bool acknowledged;
};

enum class PerformanceLevel {
    kExcellent = 0,
    kGood = 1,
    kFair = 2,
    kPoor = 3,
    kCritical = 4
};

class OGC_BASE_API PerformanceMonitor {
public:
    using Ptr = std::shared_ptr<PerformanceMonitor>;
    
    PerformanceMonitor();
    explicit PerformanceMonitor(const PerformanceThreshold& thresholds);
    ~PerformanceMonitor() = default;
    
    void SetThresholds(const PerformanceThreshold& thresholds);
    PerformanceThreshold GetThresholds() const;
    
    void SetAlertCallback(PerformanceAlertCallback callback);
    
    void BeginFrame();
    void EndFrame();
    
    void BeginRenderPass(const std::string& passName);
    void EndRenderPass();
    
    void RecordDrawCall(uint64_t vertexCount, uint64_t triangleCount);
    void RecordMemoryAllocation(double sizeMB, const std::string& category);
    void RecordMemoryDeallocation(double sizeMB, const std::string& category);
    void RecordCustomMetric(const std::string& name, double value);
    
    PerformanceMetrics GetCurrentMetrics() const;
    PerformanceLevel GetCurrentPerformanceLevel() const;
    
    std::vector<PerformanceAlert> GetActiveAlerts() const;
    void AcknowledgeAlert(const std::string& alertType);
    void ClearAlerts();
    
    double GetAverageFps() const;
    double GetCurrentFps() const;
    double GetFrameTimeMs() const;
    double GetMemoryUsageMB() const;
    uint64_t GetDrawCallCount() const;
    
    void EnableProfiling(bool enable);
    bool IsProfilingEnabled() const;
    
    void SetHistorySize(size_t size);
    size_t GetHistorySize() const;
    
    std::string GetPerformanceReport() const;
    
    static PerformanceMonitor::Ptr Create();
    static PerformanceMonitor::Ptr Create(const PerformanceThreshold& thresholds);

private:
    void CheckThresholds();
    void RaiseAlert(const std::string& type, const std::string& message, double value);
    PerformanceLevel CalculatePerformanceLevel() const;
    
    PerformanceThreshold m_thresholds;
    PerformanceMetricsCollector m_collector;
    PerformanceAlertCallback m_alertCallback;
    std::vector<PerformanceAlert> m_alerts;
    mutable std::mutex m_alertsMutex;
    bool m_profilingEnabled;
};

}
}

#endif
