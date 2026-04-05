#include <gtest/gtest.h>
#include "ogc/alert/performance_benchmark.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/alert_repository.h"
#include <chrono>
#include <thread>
#include <vector>
#include <random>

namespace ogc {
namespace alert {
namespace test {

class PerformanceBenchmarkTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_profiler = PerformanceProfiler::Instance();
        m_profiler->Clear();
    }
    
    void TearDown() override {
        m_profiler->Clear();
    }
    
    PerformanceProfiler* m_profiler;
};

TEST_F(PerformanceBenchmarkTest, ProfilerInstance) {
    ASSERT_NE(m_profiler, nullptr);
    EXPECT_EQ(m_profiler, PerformanceProfiler::Instance());
}

TEST_F(PerformanceBenchmarkTest, SessionManagement) {
    m_profiler->BeginSession("TestSession");
    m_profiler->EndSession();
}

TEST_F(PerformanceBenchmarkTest, RecordMetric) {
    m_profiler->RecordMetric("TestOp", 10.5);
    auto metrics = m_profiler->GetMetrics("TestOp");
    EXPECT_EQ(metrics.total_operations, 1);
    EXPECT_DOUBLE_EQ(metrics.avg_time_ms, 10.5);
}

TEST_F(PerformanceBenchmarkTest, MultipleMetrics) {
    for (int i = 0; i < 10; i++) {
        m_profiler->RecordMetric("MultiOp", static_cast<double>(i + 1));
    }
    
    auto metrics = m_profiler->GetMetrics("MultiOp");
    EXPECT_EQ(metrics.total_operations, 10);
    EXPECT_DOUBLE_EQ(metrics.min_time_ms, 1.0);
    EXPECT_DOUBLE_EQ(metrics.max_time_ms, 10.0);
}

TEST_F(PerformanceBenchmarkTest, ThresholdCheck) {
    PerformanceThreshold threshold;
    threshold.operation_name = "ThresholdOp";
    threshold.max_time_ms = 100.0;
    threshold.warning_threshold_ms = 50.0;
    threshold.enabled = true;
    
    m_profiler->SetThreshold("ThresholdOp", threshold);
    m_profiler->RecordMetric("ThresholdOp", 30.0);
    EXPECT_TRUE(m_profiler->CheckThreshold("ThresholdOp"));
    
    m_profiler->RecordMetric("ThresholdOp", 200.0);
    EXPECT_FALSE(m_profiler->CheckThreshold("ThresholdOp"));
}

TEST_F(PerformanceBenchmarkTest, ClearMetrics) {
    m_profiler->RecordMetric("ClearOp", 10.0);
    EXPECT_EQ(m_profiler->GetMetrics("ClearOp").total_operations, 1);
    
    m_profiler->Clear();
    EXPECT_EQ(m_profiler->GetMetrics("ClearOp").total_operations, 0);
}

class UKCCalculatorPerfTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new UKCCalculator());
    }
    
    std::unique_ptr<UKCCalculator> m_calculator;
};

TEST_F(UKCCalculatorPerfTest, SingleCalculation) {
    UKCInput input;
    input.chart_depth = 10.0;
    input.tide_height = 2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    UKCResult result = m_calculator->Calculate(input);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(UKCCalculatorPerfTest, BatchCalculation) {
    std::vector<UKCInput> inputs;
    for (int i = 0; i < 1000; i++) {
        UKCInput input;
        input.chart_depth = 10.0 + i * 0.01;
        input.tide_height = 2.0;
        input.ship_draft = 5.0;
        input.safety_margin = 0.5;
        input.squat = 0.0;
        input.heel_correction = 0.0;
        input.wave_allowance = 0.0;
        input.water_density = 1.025;
        input.speed_knots = 0.0;
        inputs.push_back(input);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& input : inputs) {
        m_calculator->Calculate(input);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100);
}

class CPACalculatorPerfTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new CPACalculator());
    }
    
    std::unique_ptr<CPACalculator> m_calculator;
};

TEST_F(CPACalculatorPerfTest, SingleCalculation) {
    ShipMotion ownShip;
    ownShip.position = Coordinate(120.0, 30.0);
    ownShip.heading = 0.0;
    ownShip.speed = 10.0;
    
    ShipMotion targetShip;
    targetShip.position = Coordinate(120.1, 30.1);
    targetShip.heading = 180.0;
    targetShip.speed = 10.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    CPAResult result = m_calculator->Calculate(ownShip, targetShip);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(CPACalculatorPerfTest, BatchCalculation) {
    std::vector<std::pair<ShipMotion, ShipMotion>> scenarios;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> latDist(20.0, 40.0);
    std::uniform_real_distribution<double> lonDist(110.0, 130.0);
    std::uniform_real_distribution<double> headingDist(0.0, 360.0);
    std::uniform_real_distribution<double> speedDist(5.0, 20.0);
    
    for (int i = 0; i < 1000; i++) {
        ShipMotion ownShip;
        ownShip.position = Coordinate(lonDist(rng), latDist(rng));
        ownShip.heading = headingDist(rng);
        ownShip.speed = speedDist(rng);
        
        ShipMotion targetShip;
        targetShip.position = Coordinate(lonDist(rng), latDist(rng));
        targetShip.heading = headingDist(rng);
        targetShip.speed = speedDist(rng);
        
        scenarios.push_back({ownShip, targetShip});
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& scenario : scenarios) {
        m_calculator->Calculate(scenario.first, scenario.second);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100);
}

class DeduplicatorPerfTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_deduplicator.reset(new AlertDeduplicator());
    }
    
    std::unique_ptr<AlertDeduplicator> m_deduplicator;
};

TEST_F(DeduplicatorPerfTest, SingleProcess) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "PERF_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = "USER_001";
    alert->issue_time = DateTime::Now();
    alert->position = Coordinate(120.0, 30.0);
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = m_deduplicator->Process(alert);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(DeduplicatorPerfTest, BatchProcess) {
    std::vector<AlertPtr> alerts;
    for (int i = 0; i < 1000; i++) {
        auto alert = std::make_shared<Alert>();
        alert->alert_id = "PERF_" + std::to_string(i);
        alert->alert_type = AlertType::kDepth;
        alert->alert_level = AlertLevel::kLevel2;
        alert->user_id = "USER_001";
        alert->issue_time = DateTime::Now();
        alert->position = Coordinate(120.0 + i * 0.001, 30.0 + i * 0.001);
        alerts.push_back(alert);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = m_deduplicator->ProcessBatch(alerts);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 500);
}

TEST_F(DeduplicatorPerfTest, DuplicateDetection) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "DUP_BASE";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = "USER_001";
    alert->issue_time = DateTime::Now();
    alert->position = Coordinate(120.0, 30.0);
    
    m_deduplicator->Process(alert);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++) {
        auto dupAlert = std::make_shared<Alert>();
        dupAlert->alert_id = "DUP_" + std::to_string(i);
        dupAlert->alert_type = AlertType::kDepth;
        dupAlert->alert_level = AlertLevel::kLevel2;
        dupAlert->user_id = "USER_001";
        dupAlert->issue_time = DateTime::Now();
        dupAlert->position = Coordinate(120.0, 30.0);
        m_deduplicator->IsDuplicate(dupAlert);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 50);
}

class RepositoryPerfTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository.reset(new AlertRepository());
    }
    
    std::unique_ptr<AlertRepository> m_repository;
};

TEST_F(RepositoryPerfTest, SingleSave) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "REPO_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = "USER_001";
    alert->status = AlertStatus::kActive;
    
    auto start = std::chrono::high_resolution_clock::now();
    bool result = m_repository->Save(alert);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(RepositoryPerfTest, BatchSave) {
    std::vector<AlertPtr> alerts;
    for (int i = 0; i < 1000; i++) {
        auto alert = std::make_shared<Alert>();
        alert->alert_id = "REPO_" + std::to_string(i);
        alert->alert_type = AlertType::kDepth;
        alert->alert_level = AlertLevel::kLevel2;
        alert->user_id = "USER_001";
        alert->status = AlertStatus::kActive;
        alerts.push_back(alert);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& alert : alerts) {
        m_repository->Save(alert);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100);
}

TEST_F(RepositoryPerfTest, QueryPerformance) {
    for (int i = 0; i < 500; i++) {
        auto alert = std::make_shared<Alert>();
        alert->alert_id = "QUERY_" + std::to_string(i);
        alert->alert_type = (i % 2 == 0) ? AlertType::kDepth : AlertType::kCollision;
        alert->alert_level = static_cast<AlertLevel>((i % 4) + 1);
        alert->user_id = "USER_" + std::to_string(i % 10);
        alert->status = AlertStatus::kActive;
        m_repository->Save(alert);
    }
    
    AlertQueryParams params;
    params.user_id = "USER_5";
    
    auto start = std::chrono::high_resolution_clock::now();
    auto results = m_repository->Query(params);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 10);
}

}
}
}
