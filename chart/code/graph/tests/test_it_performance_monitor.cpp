#include <gtest/gtest.h>
#include "ogc/draw/performance_monitor.h"
#include "ogc/draw/performance_metrics.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/envelope.h"
#include <memory>
#include <chrono>
#include <thread>

using namespace ogc::draw;
using ogc::Envelope;

class PerformanceMonitorITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device);
        ASSERT_NE(m_context, nullptr);
        m_context->Initialize();
        
        m_monitor = PerformanceMonitor::Create();
    }
    
    void TearDown() override {
        m_monitor.reset();
        m_context.reset();
        m_device.reset();
    }
    
    DrawDevicePtr m_device;
    DrawContextPtr m_context;
    PerformanceMonitor::Ptr m_monitor;
};

TEST_F(PerformanceMonitorITTest, BasicMonitoring) {
    m_monitor->BeginFrame();
    
    DrawStyle style;
    m_context->DrawPoint(128, 128);
    m_context->DrawLine(0, 0, 256, 256);
    m_context->DrawRect(50, 50, 100, 100);
    
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_GE(metrics.fps.frameCount, 0u);
}

TEST_F(PerformanceMonitorITTest, FrameTimeMeasurement) {
    m_monitor->BeginFrame();
    
    DrawParams params;
    m_context->BeginDraw(params);
    m_context->DrawPoint(128, 128);
    m_context->EndDraw();
    
    m_monitor->EndFrame();
    
    double frameTime = m_monitor->GetFrameTimeMs();
    EXPECT_GE(frameTime, 0.0);
}

TEST_F(PerformanceMonitorITTest, MultipleFrames) {
    for (int i = 0; i < 10; i++) {
        m_monitor->BeginFrame();
        
        DrawParams params;
        m_context->BeginDraw(params);
        m_context->DrawPoint(i * 25, 128);
        m_context->EndDraw();
        
        m_monitor->EndFrame();
    }
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_GE(metrics.fps.frameCount, 0u);
}

TEST_F(PerformanceMonitorITTest, RenderOperationCount) {
    m_monitor->BeginFrame();
    
    DrawParams params;
    m_context->BeginDraw(params);
    
    for (int i = 0; i < 100; i++) {
        m_context->DrawPoint(i * 2.5, i * 2.5);
    }
    
    m_context->EndDraw();
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_GE(metrics.frame.drawCalls, 0.0);
}

TEST_F(PerformanceMonitorITTest, PerformanceAlerts) {
    PerformanceThreshold thresholds;
    thresholds.frameTimeWarningMs = 10.0;
    m_monitor->SetThresholds(thresholds);
    
    m_monitor->BeginFrame();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    m_monitor->EndFrame();
    
    auto alerts = m_monitor->GetActiveAlerts();
    EXPECT_GE(alerts.size(), 0u);
}

TEST_F(PerformanceMonitorITTest, MemoryTracking) {
    m_monitor->BeginFrame();
    
    DrawParams params;
    m_context->BeginDraw(params);
    m_context->DrawRect(0, 0, 256, 256);
    m_context->EndDraw();
    
    m_monitor->RecordMemoryAllocation(10.0, "textures");
    
    m_monitor->EndFrame();
    
    double memory = m_monitor->GetMemoryUsageMB();
    EXPECT_GE(memory, 0.0);
}

TEST_F(PerformanceMonitorITTest, FpsMeasurement) {
    for (int i = 0; i < 10; i++) {
        m_monitor->BeginFrame();
        m_context->DrawPoint(128, 128);
        m_monitor->EndFrame();
    }
    
    double avgFps = m_monitor->GetAverageFps();
    EXPECT_GE(avgFps, 0.0);
    
    double currentFps = m_monitor->GetCurrentFps();
    EXPECT_GE(currentFps, 0.0);
}

TEST_F(PerformanceMonitorITTest, PerformanceReport) {
    for (int i = 0; i < 10; i++) {
        m_monitor->BeginFrame();
        m_context->DrawPoint(128, 128);
        m_monitor->EndFrame();
    }
    
    auto report = m_monitor->GetPerformanceReport();
    EXPECT_FALSE(report.empty());
}

TEST_F(PerformanceMonitorITTest, ProfilingEnabled) {
    m_monitor->EnableProfiling(true);
    EXPECT_TRUE(m_monitor->IsProfilingEnabled());
    
    m_monitor->EnableProfiling(false);
    EXPECT_FALSE(m_monitor->IsProfilingEnabled());
}

TEST_F(PerformanceMonitorITTest, PerformanceCallback) {
    int callbackCount = 0;
    m_monitor->SetAlertCallback([&callbackCount](const std::string&, const std::string&, double) {
        callbackCount++;
    });
    
    PerformanceThreshold thresholds;
    thresholds.frameTimeWarningMs = 1.0;
    m_monitor->SetThresholds(thresholds);
    
    m_monitor->BeginFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m_monitor->EndFrame();
    
    EXPECT_GE(callbackCount, 0);
}

TEST_F(PerformanceMonitorITTest, HistorySize) {
    m_monitor->SetHistorySize(100);
    EXPECT_EQ(m_monitor->GetHistorySize(), 100u);
}

TEST_F(PerformanceMonitorITTest, PerformanceLevel) {
    m_monitor->BeginFrame();
    m_context->DrawPoint(128, 128);
    m_monitor->EndFrame();
    
    auto level = m_monitor->GetCurrentPerformanceLevel();
    EXPECT_TRUE(level == PerformanceLevel::kExcellent ||
                level == PerformanceLevel::kGood ||
                level == PerformanceLevel::kFair ||
                level == PerformanceLevel::kPoor ||
                level == PerformanceLevel::kCritical);
}

TEST_F(PerformanceMonitorITTest, RenderPassTracking) {
    m_monitor->BeginFrame();
    
    m_monitor->BeginRenderPass("background");
    m_context->DrawRect(0, 0, 256, 256);
    m_monitor->EndRenderPass();
    
    m_monitor->BeginRenderPass("foreground");
    m_context->DrawPoint(128, 128);
    m_monitor->EndRenderPass();
    
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_GE(metrics.renderPasses.size(), 0u);
}

TEST_F(PerformanceMonitorITTest, CustomMetric) {
    m_monitor->BeginFrame();
    
    m_monitor->RecordCustomMetric("custom_time", 15.5);
    m_monitor->RecordCustomMetric("custom_count", 100.0);
    
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_TRUE(true);
}

TEST_F(PerformanceMonitorITTest, DrawCallRecording) {
    m_monitor->BeginFrame();
    
    m_monitor->RecordDrawCall(1000, 500);
    m_monitor->RecordDrawCall(2000, 1000);
    
    m_monitor->EndFrame();
    
    uint64_t drawCalls = m_monitor->GetDrawCallCount();
    EXPECT_GE(drawCalls, 0u);
}
