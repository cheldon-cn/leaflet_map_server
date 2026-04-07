#include <gtest/gtest.h>
#include <ogc/base/performance_metrics.h>
#include <memory>

using namespace ogc::base;

class PerformanceMetricsTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_collector = PerformanceMetricsCollector::Create();
        ASSERT_NE(m_collector, nullptr);
    }
    
    void TearDown() override {
        m_collector.reset();
    }
    
    PerformanceMetricsCollector::Ptr m_collector;
};

TEST_F(PerformanceMetricsTest, FrameMetricsReset) {
    FrameMetrics metrics;
    metrics.frameNumber = 100;
    metrics.frameTimeMs = 16.67;
    metrics.drawCalls = 50;
    metrics.Reset();
    
    EXPECT_EQ(metrics.frameNumber, 0);
    EXPECT_DOUBLE_EQ(metrics.frameTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(metrics.drawCalls, 0);
}

TEST_F(PerformanceMetricsTest, RenderPassMetricsReset) {
    RenderPassMetrics metrics;
    metrics.passName = "TestPass";
    metrics.passTimeMs = 10.0;
    metrics.Reset();
    
    EXPECT_TRUE(metrics.passName.empty());
    EXPECT_DOUBLE_EQ(metrics.passTimeMs, 0.0);
}

TEST_F(PerformanceMetricsTest, MemoryMetricsGetUsagePercent) {
    MemoryMetrics metrics;
    metrics.totalMemoryMB = 1000.0;
    metrics.usedMemoryMB = 500.0;
    
    EXPECT_DOUBLE_EQ(metrics.GetMemoryUsagePercent(), 50.0);
}

TEST_F(PerformanceMetricsTest, MemoryMetricsGetUsagePercentZeroTotal) {
    MemoryMetrics metrics;
    metrics.totalMemoryMB = 0.0;
    metrics.usedMemoryMB = 500.0;
    
    EXPECT_DOUBLE_EQ(metrics.GetMemoryUsagePercent(), 0.0);
}

TEST_F(PerformanceMetricsTest, MemoryMetricsReset) {
    MemoryMetrics metrics;
    metrics.totalMemoryMB = 1000.0;
    metrics.usedMemoryMB = 500.0;
    metrics.allocationCount = 100;
    metrics.Reset();
    
    EXPECT_DOUBLE_EQ(metrics.totalMemoryMB, 0.0);
    EXPECT_DOUBLE_EQ(metrics.usedMemoryMB, 0.0);
    EXPECT_EQ(metrics.allocationCount, 0);
}

TEST_F(PerformanceMetricsTest, GpuMetricsReset) {
    GpuMetrics metrics;
    metrics.gpuUsagePercent = 80.0;
    metrics.drawCalls = 1000;
    metrics.Reset();
    
    EXPECT_DOUBLE_EQ(metrics.gpuUsagePercent, 0.0);
    EXPECT_EQ(metrics.drawCalls, 0);
}

TEST_F(PerformanceMetricsTest, CpuMetricsReset) {
    CpuMetrics metrics;
    metrics.cpuUsagePercent = 50.0;
    metrics.contextSwitches = 100;
    metrics.Reset();
    
    EXPECT_DOUBLE_EQ(metrics.cpuUsagePercent, 0.0);
    EXPECT_EQ(metrics.contextSwitches, 0);
}

TEST_F(PerformanceMetricsTest, FpsMetricsReset) {
    FpsMetrics metrics;
    metrics.currentFps = 60.0;
    metrics.frameCount = 100;
    metrics.Reset();
    
    EXPECT_DOUBLE_EQ(metrics.currentFps, 0.0);
    EXPECT_EQ(metrics.frameCount, 0);
}

TEST_F(PerformanceMetricsTest, CollectorCreate) {
    EXPECT_NE(m_collector, nullptr);
}

TEST_F(PerformanceMetricsTest, BeginEndFrame) {
    m_collector->BeginFrame();
    m_collector->EndFrame();
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_GE(metrics.frame.frameTimeMs, 0.0);
}

TEST_F(PerformanceMetricsTest, BeginEndRenderPass) {
    m_collector->BeginFrame();
    m_collector->BeginRenderPass("TestPass");
    m_collector->EndRenderPass();
    m_collector->EndFrame();
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_EQ(metrics.renderPasses.size(), 1);
}

TEST_F(PerformanceMetricsTest, RecordDrawCall) {
    m_collector->BeginFrame();
    m_collector->RecordDrawCall(100, 50);
    m_collector->EndFrame();
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.frame.drawCalls, 1);
}

TEST_F(PerformanceMetricsTest, RecordMemoryAllocation) {
    m_collector->RecordMemoryAllocation(100.0, "textures");
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 100.0);
}

TEST_F(PerformanceMetricsTest, RecordMemoryDeallocation) {
    m_collector->RecordMemoryAllocation(100.0, "textures");
    m_collector->RecordMemoryDeallocation(50.0, "textures");
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 50.0);
}

TEST_F(PerformanceMetricsTest, RecordCustomMetric) {
    m_collector->RecordCustomMetric("custom_value", 42.0);
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.customMetrics["custom_value"], 42.0);
}

TEST_F(PerformanceMetricsTest, Reset) {
    m_collector->RecordMemoryAllocation(100.0, "textures");
    m_collector->Reset();
    
    auto metrics = m_collector->GetCurrentMetrics();
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 0.0);
}

TEST_F(PerformanceMetricsTest, SetGetMaxHistorySize) {
    m_collector->SetMaxHistorySize(120);
    EXPECT_EQ(m_collector->GetMaxHistorySize(), 120);
}

TEST_F(PerformanceMetricsTest, PerformanceMetricsReset) {
    PerformanceMetrics metrics;
    metrics.frame.frameNumber = 100;
    metrics.memory.usedMemoryMB = 500.0;
    metrics.Reset();
    
    EXPECT_EQ(metrics.frame.frameNumber, 0);
    EXPECT_DOUBLE_EQ(metrics.memory.usedMemoryMB, 0.0);
}

TEST_F(PerformanceMetricsTest, GetTotalRenderTimeMs) {
    PerformanceMetrics metrics;
    metrics.frame.frameTimeMs = 10.0;
    RenderPassMetrics pass;
    pass.passTimeMs = 5.0;
    metrics.renderPasses.push_back(pass);
    
    EXPECT_DOUBLE_EQ(metrics.GetTotalRenderTimeMs(), 15.0);
}

TEST_F(PerformanceMetricsTest, GetEfficiencyScore) {
    PerformanceMetrics metrics;
    metrics.frame.objectsRendered = 100;
    metrics.frame.objectsCulled = 50;
    
    double score = metrics.GetEfficiencyScore();
    EXPECT_GE(score, 0.0);
    EXPECT_LE(score, 100.0);
}
