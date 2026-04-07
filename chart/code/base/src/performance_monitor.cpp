#include "ogc/base/performance_monitor.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {
namespace base {

PerformanceMonitor::PerformanceMonitor()
    : m_thresholds(PerformanceThreshold::Default())
    , m_profilingEnabled(true) {
}

PerformanceMonitor::PerformanceMonitor(const PerformanceThreshold& thresholds)
    : m_thresholds(thresholds)
    , m_profilingEnabled(true) {
}

void PerformanceMonitor::SetThresholds(const PerformanceThreshold& thresholds) {
    m_thresholds = thresholds;
}

PerformanceThreshold PerformanceMonitor::GetThresholds() const {
    return m_thresholds;
}

void PerformanceMonitor::SetAlertCallback(PerformanceAlertCallback callback) {
    m_alertCallback = callback;
}

void PerformanceMonitor::BeginFrame() {
    if (m_profilingEnabled) {
        m_collector.BeginFrame();
    }
}

void PerformanceMonitor::EndFrame() {
    if (m_profilingEnabled) {
        m_collector.EndFrame();
        CheckThresholds();
    }
}

void PerformanceMonitor::BeginRenderPass(const std::string& passName) {
    if (m_profilingEnabled) {
        m_collector.BeginRenderPass(passName);
    }
}

void PerformanceMonitor::EndRenderPass() {
    if (m_profilingEnabled) {
        m_collector.EndRenderPass();
    }
}

void PerformanceMonitor::RecordDrawCall(uint64_t vertexCount, uint64_t triangleCount) {
    if (m_profilingEnabled) {
        m_collector.RecordDrawCall(vertexCount, triangleCount);
    }
}

void PerformanceMonitor::RecordMemoryAllocation(double sizeMB, const std::string& category) {
    if (m_profilingEnabled) {
        m_collector.RecordMemoryAllocation(sizeMB, category);
    }
}

void PerformanceMonitor::RecordMemoryDeallocation(double sizeMB, const std::string& category) {
    if (m_profilingEnabled) {
        m_collector.RecordMemoryDeallocation(sizeMB, category);
    }
}

void PerformanceMonitor::RecordCustomMetric(const std::string& name, double value) {
    if (m_profilingEnabled) {
        m_collector.RecordCustomMetric(name, value);
    }
}

PerformanceMetrics PerformanceMonitor::GetCurrentMetrics() const {
    return m_collector.GetCurrentMetrics();
}

PerformanceLevel PerformanceMonitor::GetCurrentPerformanceLevel() const {
    return CalculatePerformanceLevel();
}

std::vector<PerformanceAlert> PerformanceMonitor::GetActiveAlerts() const {
    std::lock_guard<std::mutex> lock(m_alertsMutex);
    std::vector<PerformanceAlert> activeAlerts;
    for (const auto& alert : m_alerts) {
        if (!alert.acknowledged) {
            activeAlerts.push_back(alert);
        }
    }
    return activeAlerts;
}

void PerformanceMonitor::AcknowledgeAlert(const std::string& alertType) {
    std::lock_guard<std::mutex> lock(m_alertsMutex);
    for (auto& alert : m_alerts) {
        if (alert.type == alertType) {
            alert.acknowledged = true;
        }
    }
}

void PerformanceMonitor::ClearAlerts() {
    std::lock_guard<std::mutex> lock(m_alertsMutex);
    m_alerts.clear();
}

double PerformanceMonitor::GetAverageFps() const {
    return m_collector.GetCurrentMetrics().fps.averageFps;
}

double PerformanceMonitor::GetCurrentFps() const {
    return m_collector.GetCurrentMetrics().fps.currentFps;
}

double PerformanceMonitor::GetFrameTimeMs() const {
    return m_collector.GetCurrentMetrics().fps.frameTimeMs;
}

double PerformanceMonitor::GetMemoryUsageMB() const {
    return m_collector.GetCurrentMetrics().memory.usedMemoryMB;
}

uint64_t PerformanceMonitor::GetDrawCallCount() const {
    return m_collector.GetCurrentMetrics().gpu.drawCalls;
}

void PerformanceMonitor::EnableProfiling(bool enable) {
    m_profilingEnabled = enable;
}

bool PerformanceMonitor::IsProfilingEnabled() const {
    return m_profilingEnabled;
}

void PerformanceMonitor::SetHistorySize(size_t size) {
    m_collector.SetMaxHistorySize(size);
}

size_t PerformanceMonitor::GetHistorySize() const {
    return m_collector.GetMaxHistorySize();
}

std::string PerformanceMonitor::GetPerformanceReport() const {
    std::ostringstream oss;
    auto metrics = m_collector.GetCurrentMetrics();
    auto level = CalculatePerformanceLevel();
    
    oss << "=== Performance Report ===\n";
    
    oss << "\n[Performance Level]: ";
    switch (level) {
        case PerformanceLevel::kExcellent: oss << "Excellent"; break;
        case PerformanceLevel::kGood: oss << "Good"; break;
        case PerformanceLevel::kFair: oss << "Fair"; break;
        case PerformanceLevel::kPoor: oss << "Poor"; break;
        case PerformanceLevel::kCritical: oss << "Critical"; break;
    }
    oss << "\n";
    
    oss << "\n[FPS Metrics]\n";
    oss << "  Current FPS: " << std::fixed << std::setprecision(1) << metrics.fps.currentFps << "\n";
    oss << "  Average FPS: " << metrics.fps.averageFps << "\n";
    oss << "  Min FPS: " << metrics.fps.minFps << "\n";
    oss << "  Max FPS: " << metrics.fps.maxFps << "\n";
    oss << "  Frame Time: " << metrics.fps.frameTimeMs << " ms\n";
    oss << "  Dropped Frames: " << metrics.fps.droppedFrames << "\n";
    
    oss << "\n[Frame Metrics]\n";
    oss << "  Frame Number: " << metrics.frame.frameNumber << "\n";
    oss << "  Frame Time: " << metrics.frame.frameTimeMs << " ms\n";
    oss << "  Draw Calls: " << metrics.frame.drawCalls << "\n";
    oss << "  Vertex Count: " << metrics.frame.vertexCount << "\n";
    oss << "  Triangle Count: " << metrics.frame.triangleCount << "\n";
    
    oss << "\n[Memory Metrics]\n";
    oss << "  Used Memory: " << metrics.memory.usedMemoryMB << " MB\n";
    oss << "  Peak Memory: " << metrics.memory.peakMemoryMB << " MB\n";
    oss << "  Texture Memory: " << metrics.memory.textureMemoryMB << " MB\n";
    oss << "  Buffer Memory: " << metrics.memory.bufferMemoryMB << " MB\n";
    oss << "  Allocations: " << metrics.memory.allocationCount << "\n";
    oss << "  Deallocations: " << metrics.memory.deallocationCount << "\n";
    
    oss << "\n[GPU Metrics]\n";
    oss << "  Draw Calls: " << metrics.gpu.drawCalls << "\n";
    oss << "  State Changes: " << metrics.gpu.stateChanges << "\n";
    oss << "  Shader Binds: " << metrics.gpu.shaderBinds << "\n";
    oss << "  Texture Binds: " << metrics.gpu.textureBinds << "\n";
    
    oss << "\n[Efficiency Score]: " << std::setprecision(1) << metrics.GetEfficiencyScore() << "/100\n";
    
    return oss.str();
}

PerformanceMonitor::Ptr PerformanceMonitor::Create() {
    return std::make_shared<PerformanceMonitor>();
}

PerformanceMonitor::Ptr PerformanceMonitor::Create(const PerformanceThreshold& thresholds) {
    return std::make_shared<PerformanceMonitor>(thresholds);
}

void PerformanceMonitor::CheckThresholds() {
    auto metrics = m_collector.GetCurrentMetrics();
    
    if (metrics.fps.currentFps < m_thresholds.lowFpsThreshold) {
        RaiseAlert("LowFPS", "FPS below threshold", metrics.fps.currentFps);
    }
    
    if (metrics.frame.frameTimeMs > m_thresholds.frameTimeCriticalMs) {
        RaiseAlert("CriticalFrameTime", "Frame time critical", metrics.frame.frameTimeMs);
    } else if (metrics.frame.frameTimeMs > m_thresholds.frameTimeWarningMs) {
        RaiseAlert("HighFrameTime", "Frame time high", metrics.frame.frameTimeMs);
    }
    
    if (metrics.memory.usedMemoryMB > m_thresholds.memoryCriticalThresholdMB) {
        RaiseAlert("CriticalMemory", "Memory usage critical", metrics.memory.usedMemoryMB);
    } else if (metrics.memory.usedMemoryMB > m_thresholds.memoryWarningThresholdMB) {
        RaiseAlert("HighMemory", "Memory usage high", metrics.memory.usedMemoryMB);
    }
    
    if (metrics.gpu.drawCalls > m_thresholds.drawCallCriticalThreshold) {
        RaiseAlert("CriticalDrawCalls", "Draw call count critical", static_cast<double>(metrics.gpu.drawCalls));
    } else if (metrics.gpu.drawCalls > m_thresholds.drawCallWarningThreshold) {
        RaiseAlert("HighDrawCalls", "Draw call count high", static_cast<double>(metrics.gpu.drawCalls));
    }
}

void PerformanceMonitor::RaiseAlert(const std::string& type, const std::string& message, double value) {
    PerformanceAlert alert;
    alert.type = type;
    alert.message = message;
    alert.value = value;
    alert.timestamp = std::chrono::steady_clock::now();
    alert.acknowledged = false;
    
    {
        std::lock_guard<std::mutex> lock(m_alertsMutex);
        m_alerts.push_back(alert);
    }
    
    if (m_alertCallback) {
        m_alertCallback(type, message, value);
    }
}

PerformanceLevel PerformanceMonitor::CalculatePerformanceLevel() const {
    auto metrics = m_collector.GetCurrentMetrics();
    
    if (metrics.fps.currentFps < m_thresholds.lowFpsThreshold ||
        metrics.memory.usedMemoryMB > m_thresholds.memoryCriticalThresholdMB ||
        metrics.frame.frameTimeMs > m_thresholds.frameTimeCriticalMs) {
        return PerformanceLevel::kCritical;
    }
    
    if (metrics.fps.currentFps < m_thresholds.lowFpsThreshold * 1.5 ||
        metrics.memory.usedMemoryMB > m_thresholds.memoryWarningThresholdMB ||
        metrics.frame.frameTimeMs > m_thresholds.frameTimeWarningMs) {
        return PerformanceLevel::kPoor;
    }
    
    if (metrics.fps.currentFps < m_thresholds.highFpsThreshold * 0.8 ||
        metrics.memory.usedMemoryMB > m_thresholds.memoryWarningThresholdMB * 0.5) {
        return PerformanceLevel::kFair;
    }
    
    if (metrics.fps.currentFps < m_thresholds.highFpsThreshold) {
        return PerformanceLevel::kGood;
    }
    
    return PerformanceLevel::kExcellent;
}

} 
} 
