#include "ogc/draw/performance_metrics.h"
#include <algorithm>

namespace ogc {
namespace draw {

void FpsMetrics::UpdateFps(double deltaTimeMs) {
    if (deltaTimeMs > 0) {
        currentFps = 1000.0 / deltaTimeMs;
        frameTimeMs = deltaTimeMs;
    }
    
    frameCount++;
    
    if (fpsHistory.size() >= maxHistorySize) {
        fpsHistory.erase(fpsHistory.begin());
    }
    fpsHistory.push_back(currentFps);
    
    if (!fpsHistory.empty()) {
        double sum = 0.0;
        minFps = fpsHistory[0];
        maxFps = fpsHistory[0];
        
        for (double fps : fpsHistory) {
            sum += fps;
            if (fps < minFps) minFps = fps;
            if (fps > maxFps) maxFps = fps;
        }
        
        averageFps = sum / fpsHistory.size();
        
        double varianceSum = 0.0;
        for (double fps : fpsHistory) {
            double diff = fps - averageFps;
            varianceSum += diff * diff;
        }
        fpsVariance = varianceSum / fpsHistory.size();
        
        averageFrameTimeMs = fpsHistory.size() > 0 ? 1000.0 / averageFps : 0.0;
    }
}

void PerformanceMetrics::Reset() {
    frame.Reset();
    renderPasses.clear();
    memory.Reset();
    fps.Reset();
    gpu.Reset();
    cpu.Reset();
    customMetrics.clear();
}

double PerformanceMetrics::GetTotalRenderTimeMs() const {
    double total = frame.frameTimeMs;
    for (const auto& pass : renderPasses) {
        total += pass.passTimeMs;
    }
    return total;
}

double PerformanceMetrics::GetEfficiencyScore() const {
    if (fps.averageFps <= 0 || memory.usedMemoryMB <= 0) {
        return 0.0;
    }
    
    double fpsScore = std::min(fps.averageFps / 60.0, 1.0) * 40.0;
    double memoryScore = std::max(0.0, 1.0 - memory.GetMemoryUsagePercent() / 100.0) * 30.0;
    double drawCallScore = std::max(0.0, 1.0 - static_cast<double>(gpu.drawCalls) / 10000.0) * 30.0;
    
    return fpsScore + memoryScore + drawCallScore;
}

PerformanceMetricsCollector::PerformanceMetricsCollector()
    : m_maxHistorySize(60) {
    m_metrics.collectionTime = std::chrono::steady_clock::now();
}

void PerformanceMetricsCollector::BeginFrame() {
    m_frameStartTime = std::chrono::steady_clock::now();
    m_metrics.frame.frameNumber++;
}

void PerformanceMetricsCollector::EndFrame() {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_frameStartTime);
    double frameTimeMs = duration.count() / 1000.0;
    
    m_metrics.frame.frameTimeMs = frameTimeMs;
    m_metrics.fps.UpdateFps(frameTimeMs);
    m_metrics.collectionTime = endTime;
    
    if (frameTimeMs > 16.67) {
        m_metrics.fps.droppedFrames++;
    }
}

void PerformanceMetricsCollector::BeginRenderPass(const std::string& passName) {
    m_passStartTime = std::chrono::steady_clock::now();
    m_currentPassName = passName;
}

void PerformanceMetricsCollector::EndRenderPass() {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_passStartTime);
    double passTimeMs = duration.count() / 1000.0;
    
    RenderPassMetrics pass;
    pass.passName = m_currentPassName;
    pass.passTimeMs = passTimeMs;
    
    m_metrics.renderPasses.push_back(pass);
    m_currentPassName.clear();
}

void PerformanceMetricsCollector::RecordDrawCall(uint64_t vertexCount, uint64_t triangleCount) {
    m_metrics.frame.drawCalls++;
    m_metrics.frame.vertexCount += vertexCount;
    m_metrics.frame.triangleCount += triangleCount;
    m_metrics.gpu.drawCalls++;
    
    if (!m_metrics.renderPasses.empty()) {
        auto& lastPass = m_metrics.renderPasses.back();
        lastPass.drawCalls++;
        lastPass.vertexCount += vertexCount;
        lastPass.triangleCount += triangleCount;
    }
}

void PerformanceMetricsCollector::RecordMemoryAllocation(double sizeMB, const std::string& category) {
    m_metrics.memory.usedMemoryMB += sizeMB;
    m_metrics.memory.allocationCount++;
    
    if (m_metrics.memory.usedMemoryMB > m_metrics.memory.peakMemoryMB) {
        m_metrics.memory.peakMemoryMB = m_metrics.memory.usedMemoryMB;
    }
    
    auto it = m_metrics.memory.memoryByCategory.find(category);
    if (it != m_metrics.memory.memoryByCategory.end()) {
        it->second += sizeMB;
    } else {
        m_metrics.memory.memoryByCategory[category] = sizeMB;
    }
}

void PerformanceMetricsCollector::RecordMemoryDeallocation(double sizeMB, const std::string& category) {
    m_metrics.memory.usedMemoryMB -= sizeMB;
    if (m_metrics.memory.usedMemoryMB < 0) {
        m_metrics.memory.usedMemoryMB = 0;
    }
    m_metrics.memory.deallocationCount++;
    
    auto it = m_metrics.memory.memoryByCategory.find(category);
    if (it != m_metrics.memory.memoryByCategory.end()) {
        it->second -= sizeMB;
        if (it->second < 0) {
            it->second = 0;
        }
    }
}

void PerformanceMetricsCollector::RecordCustomMetric(const std::string& name, double value) {
    m_metrics.customMetrics[name] = value;
}

PerformanceMetrics PerformanceMetricsCollector::GetCurrentMetrics() const {
    return m_metrics;
}

void PerformanceMetricsCollector::Reset() {
    m_metrics.Reset();
    m_metrics.collectionTime = std::chrono::steady_clock::now();
}

void PerformanceMetricsCollector::SetMaxHistorySize(size_t size) {
    m_maxHistorySize = size;
    m_metrics.fps.maxHistorySize = size;
}

size_t PerformanceMetricsCollector::GetMaxHistorySize() const {
    return m_maxHistorySize;
}

PerformanceMetricsCollector::Ptr PerformanceMetricsCollector::Create() {
    return std::make_shared<PerformanceMetricsCollector>();
}

} 
} 
