#include <gtest/gtest.h>
#include "ogc/base/performance_monitor.h"
#include <memory>

using namespace ogc::base;

class PerformanceMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_monitor = PerformanceMonitor::Create();
        ASSERT_NE(m_monitor, nullptr);
    }
    
    void TearDown() override {
        m_monitor.reset();
    }
    
    PerformanceMonitor::Ptr m_monitor;
};

TEST_F(PerformanceMonitorTest, Create) {
    EXPECT_NE(m_monitor, nullptr);
}

TEST_F(PerformanceMonitorTest, CreateWithThresholds) {
    auto thresholds = PerformanceThreshold::Strict();
    auto monitor = PerformanceMonitor::Create(thresholds);
    ASSERT_NE(monitor, nullptr);
}

TEST_F(PerformanceMonitorTest, SetGetThresholds) {
    auto thresholds = PerformanceThreshold::Strict();
    m_monitor->SetThresholds(thresholds);
    
    auto retrieved = m_monitor->GetThresholds();
    EXPECT_DOUBLE_EQ(retrieved.lowFpsThreshold, 30.0);
}

TEST_F(PerformanceMonitorTest, DefaultThreshold) {
    auto thresholds = PerformanceThreshold::Default();
    EXPECT_DOUBLE_EQ(thresholds.lowFpsThreshold, 15.0);
    EXPECT_DOUBLE_EQ(thresholds.highFpsThreshold, 60.0);
}

TEST_F(PerformanceMonitorTest, StrictThreshold) {
    auto thresholds = PerformanceThreshold::Strict();
    EXPECT_DOUBLE_EQ(thresholds.lowFpsThreshold, 30.0);
    EXPECT_DOUBLE_EQ(thresholds.memoryWarningThresholdMB, 512.0);
}

TEST_F(PerformanceMonitorTest, RelaxedThreshold) {
    auto thresholds = PerformanceThreshold::Relaxed();
    EXPECT_DOUBLE_EQ(thresholds.lowFpsThreshold, 10.0);
    EXPECT_DOUBLE_EQ(thresholds.memoryWarningThresholdMB, 2048.0);
}

TEST_F(PerformanceMonitorTest, BeginEndFrame) {
    m_monitor->BeginFrame();
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_GE(metrics.frame.frameTimeMs, 0.0);
}

TEST_F(PerformanceMonitorTest, BeginEndRenderPass) {
    m_monitor->BeginFrame();
    m_monitor->BeginRenderPass("TestPass");
    m_monitor->EndRenderPass();
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_EQ(metrics.renderPasses.size(), 1);
}

TEST_F(PerformanceMonitorTest, RecordDrawCall) {
    m_monitor->BeginFrame();
    m_monitor->RecordDrawCall(100, 50);
    m_monitor->EndFrame();
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.frame.drawCalls, 1);
}

TEST_F(PerformanceMonitorTest, RecordMemoryAllocation) {
    m_monitor->RecordMemoryAllocation(100.0, "textures");
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 100.0);
}

TEST_F(PerformanceMonitorTest, RecordMemoryDeallocation) {
    m_monitor->RecordMemoryAllocation(100.0, "textures");
    m_monitor->RecordMemoryDeallocation(50.0, "textures");
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 50.0);
}

TEST_F(PerformanceMonitorTest, RecordCustomMetric) {
    m_monitor->RecordCustomMetric("custom_value", 42.0);
    
    auto metrics = m_monitor->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.customMetrics["custom_value"], 42.0);
}

TEST_F(PerformanceMonitorTest, EnableProfiling) {
    m_monitor->EnableProfiling(true);
    EXPECT_TRUE(m_monitor->IsProfilingEnabled());
    
    m_monitor->EnableProfiling(false);
    EXPECT_FALSE(m_monitor->IsProfilingEnabled());
}

TEST_F(PerformanceMonitorTest, SetHistorySize) {
    m_monitor->SetHistorySize(120);
    EXPECT_EQ(m_monitor->GetHistorySize(), 120);
}

TEST_F(PerformanceMonitorTest, GetCurrentPerformanceLevel) {
    m_monitor->BeginFrame();
    m_monitor->EndFrame();
    
    auto level = m_monitor->GetCurrentPerformanceLevel();
    EXPECT_GE(static_cast<int>(level), 0);
    EXPECT_LE(static_cast<int>(level), 4);
}

TEST_F(PerformanceMonitorTest, GetActiveAlerts) {
    auto alerts = m_monitor->GetActiveAlerts();
    EXPECT_TRUE(alerts.empty());
}

TEST_F(PerformanceMonitorTest, ClearAlerts) {
    m_monitor->ClearAlerts();
    auto alerts = m_monitor->GetActiveAlerts();
    EXPECT_TRUE(alerts.empty());
}

TEST_F(PerformanceMonitorTest, GetPerformanceReport) {
    m_monitor->BeginFrame();
    m_monitor->EndFrame();
    
    auto report = m_monitor->GetPerformanceReport();
    EXPECT_FALSE(report.empty());
}

TEST_F(PerformanceMonitorTest, SetAlertCallback) {
    bool callbackCalled = false;
    m_monitor->SetAlertCallback([&callbackCalled](const std::string&, const std::string&, double) {
        callbackCalled = true;
    });
    
    m_monitor->RecordMemoryAllocation(3000.0, "textures");
    
    auto alerts = m_monitor->GetActiveAlerts();
    if (!alerts.empty()) {
        EXPECT_TRUE(callbackCalled);
    }
}
