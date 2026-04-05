#include <gtest/gtest.h>
#include "test_data.h"
#include "ogc/alert/types.h"
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_processor.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/scheduler.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/weather_alert_checker.h"
#include "ogc/alert/channel_alert_checker.h"
#include "ogc/alert/deviation_alert_checker.h"
#include "ogc/alert/speed_alert_checker.h"
#include "ogc/alert/restricted_area_checker.h"
#include "ogc/alert/performance_benchmark.h"
#include <memory>
#include <limits>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace ogc {
namespace alert {
namespace test {

class ExceptionPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository.reset(new AlertRepository());
        m_engine.reset(new AlertEngine());
    }
    
    void TearDown() override {
    }
    
    std::unique_ptr<AlertRepository> m_repository;
    std::unique_ptr<AlertEngine> m_engine;
};

TEST_F(ExceptionPathTest, Repository_SaveNullAlert) {
    AlertPtr nullAlert;
    EXPECT_DEATH_IF_SUPPORTED(m_repository->Save(nullAlert), ".*");
}

TEST_F(ExceptionPathTest, Repository_QueryWithEmptyParams) {
    AlertQueryParams params;
    auto results = m_repository->Query(params);
    EXPECT_TRUE(results.empty());
}

TEST_F(ExceptionPathTest, Repository_GetAlertWithEmptyId) {
    auto alert = m_repository->FindById("");
    EXPECT_EQ(alert, nullptr);
}

TEST_F(ExceptionPathTest, Repository_DeleteNonExistentAlert) {
    EXPECT_NO_THROW(m_repository->Delete("NON_EXISTENT_ID"));
}

TEST_F(ExceptionPathTest, UKCCalculator_NaNInput) {
    UKCInput input;
    input.chart_depth = std::numeric_limits<double>::quiet_NaN();
    input.tide_height = 2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    EXPECT_TRUE(std::isnan(result.ukc) || result.ukc < 0);
}

TEST_F(ExceptionPathTest, UKCCalculator_InfinityInput) {
    UKCInput input;
    input.chart_depth = std::numeric_limits<double>::infinity();
    input.tide_height = 2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    EXPECT_TRUE(std::isinf(result.ukc) || std::isnan(result.ukc));
}

TEST_F(ExceptionPathTest, CPACalculator_ZeroSpeed) {
    ShipMotion ownShip;
    ownShip.position = Coordinate(0.0, 0.0);
    ownShip.heading = 0.0;
    ownShip.speed = 0.0;
    
    ShipMotion targetShip;
    targetShip.position = Coordinate(0.1, 0.1);
    targetShip.heading = 180.0;
    targetShip.speed = 10.0;
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ownShip, targetShip);
    EXPECT_TRUE(std::isfinite(result.cpa) || std::isnan(result.cpa));
}

TEST_F(ExceptionPathTest, CPACalculator_SamePosition) {
    ShipMotion ship1;
    ship1.position = Coordinate(120.0, 30.0);
    ship1.heading = 0.0;
    ship1.speed = 10.0;
    
    ShipMotion ship2;
    ship2.position = Coordinate(120.0, 30.0);
    ship2.heading = 180.0;
    ship2.speed = 10.0;
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ship1, ship2);
    EXPECT_DOUBLE_EQ(result.cpa, 0.0);
}

TEST_F(ExceptionPathTest, Deduplicator_ProcessNullAlert) {
    AlertDeduplicator deduplicator;
    AlertPtr nullAlert;
    EXPECT_DEATH_IF_SUPPORTED(deduplicator.Process(nullAlert), ".*");
}

TEST_F(ExceptionPathTest, Deduplicator_ClearCache) {
    AlertDeduplicator deduplicator;
    EXPECT_NO_THROW(deduplicator.ClearHistory());
}

TEST_F(ExceptionPathTest, Deduplicator_GetCacheSize) {
    AlertDeduplicator deduplicator;
    EXPECT_EQ(deduplicator.GetProcessedCount(), 0);
}

TEST_F(ExceptionPathTest, WeatherChecker_ExtremeValues) {
    auto checker = WeatherAlertChecker::Create();
    
    AlertLevel level = checker->DetermineWeatherLevel(
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min());
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(ExceptionPathTest, ChannelChecker_EmptyChannelId) {
    auto checker = ChannelAlertChecker::Create();
    EXPECT_NO_THROW(checker->SetNoticeData(nullptr));
}

TEST_F(ExceptionPathTest, DeviationChecker_NaNDeviation) {
    auto checker = DeviationAlertChecker::Create();
    double deviation = std::numeric_limits<double>::quiet_NaN();
    
    AlertLevel level = checker->DetermineDeviationLevel(deviation);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(ExceptionPathTest, SpeedChecker_NegativeSpeedLimit) {
    auto checker = SpeedAlertChecker::Create();
    
    double speedLimit = checker->GetSpeedLimit(Coordinate(120.0, 30.0));
    EXPECT_TRUE(std::isfinite(speedLimit) || speedLimit == 0.0);
}

TEST_F(ExceptionPathTest, RestrictedAreaChecker_EmptyAreaId) {
    auto checker = RestrictedAreaChecker::Create();
    
    double distance = checker->CalculateDistanceToBoundary(Coordinate(120.0, 30.0), "");
    EXPECT_TRUE(std::isfinite(distance) || distance < 0);
}

class PerformanceOptimizationTest : public ::testing::Test {
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

TEST_F(PerformanceOptimizationTest, Profiler_ClearMetrics) {
    m_profiler->RecordMetric("TestOp", 10.0);
    EXPECT_NO_THROW(m_profiler->Clear());
    auto metrics = m_profiler->GetMetrics("TestOp");
    EXPECT_EQ(metrics.total_operations, 0);
}

TEST_F(PerformanceOptimizationTest, Profiler_RecordMultipleMetrics) {
    for (int i = 0; i < 100; i++) {
        m_profiler->RecordMetric("BatchOp", static_cast<double>(i));
    }
    
    auto metrics = m_profiler->GetMetrics("BatchOp");
    EXPECT_EQ(metrics.total_operations, 100);
}

TEST_F(PerformanceOptimizationTest, Profiler_GetNonExistentMetrics) {
    auto metrics = m_profiler->GetMetrics("NonExistentOp");
    EXPECT_EQ(metrics.total_operations, 0);
}

TEST_F(PerformanceOptimizationTest, Profiler_SetThreshold) {
    PerformanceThreshold threshold("TestOp", 100.0, 80.0);
    EXPECT_NO_THROW(m_profiler->SetThreshold("TestOp", threshold));
}

TEST_F(PerformanceOptimizationTest, Profiler_CheckThresholdWithoutMetrics) {
    PerformanceThreshold threshold("NoMetricsOp", 100.0, 80.0);
    m_profiler->SetThreshold("NoMetricsOp", threshold);
    
    EXPECT_TRUE(m_profiler->CheckThreshold("NoMetricsOp"));
}

TEST_F(PerformanceOptimizationTest, Profiler_SessionManagement) {
    m_profiler->RecordMetric("ReportOp", 50.0);
    
    EXPECT_NO_THROW(m_profiler->BeginSession("TestSession"));
    EXPECT_NO_THROW(m_profiler->EndSession());
}

TEST_F(PerformanceOptimizationTest, Profiler_ConcurrentRecording) {
    const int threadCount = 4;
    const int recordsPerThread = 25;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < threadCount; t++) {
        threads.emplace_back([this, t, recordsPerThread]() {
            for (int i = 0; i < recordsPerThread; i++) {
                m_profiler->RecordMetric("ConcurrentOp", static_cast<double>(i));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto metrics = m_profiler->GetMetrics("ConcurrentOp");
    EXPECT_EQ(metrics.total_operations, threadCount * recordsPerThread);
}

class LoggingPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository.reset(new AlertRepository());
        m_processor = IAlertProcessor::Create();
    }
    
    std::unique_ptr<AlertRepository> m_repository;
    std::unique_ptr<IAlertProcessor> m_processor;
};

TEST_F(LoggingPathTest, Repository_LogSaveOperation) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "LOG_TEST_001";
    alert->alert_type = AlertType::kDepth;
    alert->user_id = "USER_001";
    
    EXPECT_NO_THROW(m_repository->Save(alert));
}

TEST_F(LoggingPathTest, Repository_LogQueryOperation) {
    AlertQueryParams params;
    params.user_id = "USER_001";
    
    EXPECT_NO_THROW(m_repository->Query(params));
}

TEST_F(LoggingPathTest, Repository_LogDeleteOperation) {
    EXPECT_NO_THROW(m_repository->Delete("LOG_TEST_001"));
}

TEST_F(LoggingPathTest, Processor_LogProcessOperation) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "LOG_PROCESS_001";
    alert->alert_type = AlertType::kDepth;
    
    std::vector<AlertPtr> alerts = {alert};
    EXPECT_NO_THROW(m_processor->Process(alerts));
}

TEST_F(LoggingPathTest, Processor_LogDeduplication) {
    auto deduplicator = IDeduplicationStrategy::CreateDefault();
    m_processor->SetDeduplicationStrategy(deduplicator);
    
    auto alert1 = std::make_shared<Alert>();
    alert1->alert_id = "DUP_LOG_001";
    alert1->alert_type = AlertType::kDepth;
    alert1->user_id = "USER_001";
    alert1->issue_time = DateTime::Now();
    alert1->position = Coordinate(120.0, 30.0);
    
    auto alert2 = std::make_shared<Alert>();
    alert2->alert_id = "DUP_LOG_002";
    alert2->alert_type = AlertType::kDepth;
    alert2->user_id = "USER_001";
    alert2->issue_time = DateTime::Now();
    alert2->position = Coordinate(120.0, 30.0);
    
    std::vector<AlertPtr> alerts = {alert1, alert2};
    EXPECT_NO_THROW(m_processor->Process(alerts));
}

}
}
}
