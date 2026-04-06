#ifndef OGC_BASE_PERFORMANCE_METRICS_H
#define OGC_BASE_PERFORMANCE_METRICS_H

#include "export.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cstdint>
#include <memory>

namespace ogc {
namespace base {

struct FrameMetrics {
    uint64_t frameNumber = 0;
    double frameTimeMs = 0.0;
    double cpuTimeMs = 0.0;
    double gpuTimeMs = 0.0;
    double drawCalls = 0;
    uint64_t vertexCount = 0;
    uint64_t triangleCount = 0;
    uint64_t objectsRendered = 0;
    uint64_t objectsCulled = 0;
    std::map<std::string, double> customMetrics;
    
    void Reset() {
        frameNumber = 0;
        frameTimeMs = 0.0;
        cpuTimeMs = 0.0;
        gpuTimeMs = 0.0;
        drawCalls = 0;
        vertexCount = 0;
        triangleCount = 0;
        objectsRendered = 0;
        objectsCulled = 0;
        customMetrics.clear();
    }
};

struct RenderPassMetrics {
    std::string passName;
    double passTimeMs = 0.0;
    double drawCalls = 0;
    double vertexCount = 0;
    double triangleCount = 0;
    uint64_t objectsRendered = 0;
    uint64_t objectsCulled = 0;
    std::map<std::string, double> customMetrics;
    
    void Reset() {
        passName.clear();
        passTimeMs = 0.0;
        drawCalls = 0;
        vertexCount = 0;
        triangleCount = 0;
        objectsRendered = 0;
        objectsCulled = 0;
        customMetrics.clear();
    }
};

struct MemoryMetrics {
    double totalMemoryMB = 0.0;
    double usedMemoryMB = 0.0;
    double availableMemoryMB = 0.0;
    double textureMemoryMB = 0.0;
    double bufferMemoryMB = 0.0;
    double geometryMemoryMB = 0.0;
    double cacheMemoryMB = 0.0;
    uint64_t allocationCount = 0;
    uint64_t deallocationCount = 0;
    double peakMemoryMB = 0.0;
    std::map<std::string, double> memoryByCategory;
    
    double GetMemoryUsagePercent() const {
        if (totalMemoryMB > 0) {
            return (usedMemoryMB / totalMemoryMB) * 100.0;
        }
        return 0.0;
    }
    
    void Reset() {
        totalMemoryMB = 0.0;
        usedMemoryMB = 0.0;
        availableMemoryMB = 0.0;
        textureMemoryMB = 0.0;
        bufferMemoryMB = 0.0;
        geometryMemoryMB = 0.0;
        cacheMemoryMB = 0.0;
        allocationCount = 0;
        deallocationCount = 0;
        peakMemoryMB = 0.0;
        memoryByCategory.clear();
    }
};

struct OGC_BASE_API FpsMetrics {
    double currentFps = 0.0;
    double averageFps = 0.0;
    double minFps = 0.0;
    double maxFps = 0.0;
    double fpsVariance = 0.0;
    uint64_t frameCount = 0;
    uint64_t droppedFrames = 0;
    double frameTimeMs = 0.0;
    double averageFrameTimeMs = 0.0;
    std::vector<double> fpsHistory;
    size_t maxHistorySize = 60;
    
    void UpdateFps(double deltaTimeMs);
    
    void Reset() {
        currentFps = 0.0;
        averageFps = 0.0;
        minFps = 0.0;
        maxFps = 0.0;
        fpsVariance = 0.0;
        frameCount = 0;
        droppedFrames = 0;
        frameTimeMs = 0.0;
        averageFrameTimeMs = 0.0;
        fpsHistory.clear();
    }
};

struct GpuMetrics {
    double gpuUsagePercent = 0.0;
    double gpuMemoryMB = 0.0;
    double gpuTemperature = 0.0;
    double gpuCoreClockMHz = 0.0;
    double gpuMemoryClockMHz = 0.0;
    uint64_t drawCalls = 0;
    uint64_t stateChanges = 0;
    uint64_t shaderBinds = 0;
    uint64_t textureBinds = 0;
    uint64_t bufferBinds = 0;
    std::map<std::string, double> customGpuMetrics;
    
    void Reset() {
        gpuUsagePercent = 0.0;
        gpuMemoryMB = 0.0;
        gpuTemperature = 0.0;
        gpuCoreClockMHz = 0.0;
        gpuMemoryClockMHz = 0.0;
        drawCalls = 0;
        stateChanges = 0;
        shaderBinds = 0;
        textureBinds = 0;
        bufferBinds = 0;
        customGpuMetrics.clear();
    }
};

struct CpuMetrics {
    double cpuUsagePercent = 0.0;
    double userTimeMs = 0.0;
    double kernelTimeMs = 0.0;
    double idleTimeMs = 0.0;
    uint64_t contextSwitches = 0;
    uint64_t interrupts = 0;
    std::map<std::string, double> threadMetrics;
    
    void Reset() {
        cpuUsagePercent = 0.0;
        userTimeMs = 0.0;
        kernelTimeMs = 0.0;
        idleTimeMs = 0.0;
        contextSwitches = 0;
        interrupts = 0;
        threadMetrics.clear();
    }
};

struct OGC_BASE_API PerformanceMetrics {
    FrameMetrics frame;
    std::vector<RenderPassMetrics> renderPasses;
    MemoryMetrics memory;
    FpsMetrics fps;
    GpuMetrics gpu;
    CpuMetrics cpu;
    std::chrono::steady_clock::time_point collectionTime;
    std::map<std::string, double> customMetrics;
    
    void Reset();
    double GetTotalRenderTimeMs() const;
    double GetEfficiencyScore() const;
};

class OGC_BASE_API PerformanceMetricsCollector {
public:
    using Ptr = std::shared_ptr<PerformanceMetricsCollector>;
    
    PerformanceMetricsCollector();
    ~PerformanceMetricsCollector() = default;
    
    void BeginFrame();
    void EndFrame();
    
    void BeginRenderPass(const std::string& passName);
    void EndRenderPass();
    
    void RecordDrawCall(uint64_t vertexCount, uint64_t triangleCount);
    void RecordMemoryAllocation(double sizeMB, const std::string& category);
    void RecordMemoryDeallocation(double sizeMB, const std::string& category);
    void RecordCustomMetric(const std::string& name, double value);
    
    PerformanceMetrics GetCurrentMetrics() const;
    void Reset();
    
    void SetMaxHistorySize(size_t size);
    size_t GetMaxHistorySize() const;
    
    static PerformanceMetricsCollector::Ptr Create();

private:
    PerformanceMetrics m_metrics;
    std::chrono::steady_clock::time_point m_frameStartTime;
    std::chrono::steady_clock::time_point m_passStartTime;
    std::string m_currentPassName;
    size_t m_maxHistorySize;
};

} 
} 

#endif 
