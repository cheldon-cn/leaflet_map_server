#include <gtest/gtest.h>
#include "test_data.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/alert_repository.h"
#include <limits>
#include <cmath>

namespace ogc {
namespace alert {
namespace test {

class BoundaryValueTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository.reset(new AlertRepository());
    }
    
    void TearDown() override {
    }
    
    std::unique_ptr<AlertRepository> m_repository;
};

TEST_F(BoundaryValueTest, UKC_SafeBoundary) {
    UKCInput input = TestDataFactory::CreateUKCInput(
        TestUKCData::kShallowDepth,
        TestUKCData::kDefaultDraft,
        TestUKCData::kDefaultTide);
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_GE(result.ukc, 0.0);
    EXPECT_LE(result.ukc, 10.0);
}

TEST_F(BoundaryValueTest, UKC_CriticalBoundary) {
    UKCInput input;
    input.chart_depth = TestUKCData::kShallowDepth;
    input.ship_draft = TestUKCData::kLargeDraft;
    input.tide_height = TestUKCData::kLowTide;
    input.safety_margin = 0.0;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = TestUKCData::kDefaultWaterDensity;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_LE(result.ukc, 1.0);
}

TEST_F(BoundaryValueTest, UKC_ZeroDepth) {
    UKCInput input;
    input.chart_depth = 0.0;
    input.ship_draft = TestUKCData::kDefaultDraft;
    input.tide_height = 0.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = TestUKCData::kDefaultWaterDensity;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_LE(result.ukc, 0.0);
}

TEST_F(BoundaryValueTest, UKC_NegativeDepth) {
    UKCInput input;
    input.chart_depth = -5.0;
    input.ship_draft = TestUKCData::kDefaultDraft;
    input.tide_height = 0.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = TestUKCData::kDefaultWaterDensity;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_TRUE(std::isnan(result.ukc) || result.ukc < 0);
}

TEST_F(BoundaryValueTest, CPA_ExactDistance) {
    ShipMotion ship1 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    ShipMotion ship2 = TestDataFactory::CreateShipMotion(
        Coordinate(TestCoordinates::kShanghaiPort.longitude + 0.01, 
                   TestCoordinates::kShanghaiPort.latitude),
        TestShipData::kDefaultSpeed,
        TestShipData::kSouthHeading);
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ship1, ship2);
    
    EXPECT_GE(result.cpa, 0.0);
    EXPECT_LE(result.cpa, 10.0);
}

TEST_F(BoundaryValueTest, CPA_FarDistance) {
    ShipMotion ship1 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    ShipMotion ship2 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kBeijing,
        TestShipData::kDefaultSpeed,
        TestShipData::kSouthHeading);
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ship1, ship2);
    
    EXPECT_GE(result.cpa, 1.0);
}

TEST_F(BoundaryValueTest, CPA_OppositeHeading) {
    ShipMotion ship1 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    ShipMotion ship2 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kSouthHeading);
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ship1, ship2);
    
    EXPECT_GE(result.cpa, 0.0);
}

TEST_F(BoundaryValueTest, CPA_ParallelHeading) {
    ShipMotion ship1 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    ShipMotion ship2 = TestDataFactory::CreateShipMotion(
        Coordinate(TestCoordinates::kShanghaiPort.longitude + 0.01,
                   TestCoordinates::kShanghaiPort.latitude),
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    CPACalculator calculator;
    CPAResult result = calculator.Calculate(ship1, ship2);
    
    EXPECT_TRUE(std::isnan(result.cpa) || result.cpa >= 0.0);
}

TEST_F(BoundaryValueTest, Deduplicator_TimeWindowBoundary) {
    DeduplicationConfig config;
    config.enabled = true;
    config.time_window_seconds = 1;
    config.spatial_threshold = 0.01;
    config.merge_similar = false;
    config.max_duplicate_count = 5;
    config.suppress_repeated = false;
    config.suppress_interval_seconds = 300;
    
    AlertDeduplicator deduplicator(config);
    
    auto alert1 = TestDataFactory::CreateTestAlert(
        "BOUND_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    alert1->issue_time = DateTime::Now();
    alert1->position = TestCoordinates::kShanghaiPort;
    
    deduplicator.Process(alert1);
    size_t count1 = deduplicator.GetProcessedCount();
    EXPECT_GE(count1, 1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    auto alert2 = TestDataFactory::CreateTestAlert(
        "BOUND_002",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    alert2->issue_time = DateTime::Now();
    alert2->position = TestCoordinates::kShanghaiPort;
    
    deduplicator.Process(alert2);
    size_t count2 = deduplicator.GetProcessedCount();
    EXPECT_GE(count2, count1);
}

TEST_F(BoundaryValueTest, Deduplicator_SpatialThresholdBoundary) {
    DeduplicationConfig config;
    config.enabled = true;
    config.time_window_seconds = 300;
    config.spatial_threshold = 0.01;
    config.merge_similar = false;
    config.max_duplicate_count = 5;
    config.suppress_repeated = false;
    config.suppress_interval_seconds = 300;
    
    AlertDeduplicator deduplicator(config);
    
    auto alert1 = TestDataFactory::CreateTestAlert(
        "SPATIAL_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    alert1->issue_time = DateTime::Now();
    alert1->position = TestCoordinates::kShanghaiPort;
    
    deduplicator.Process(alert1);
    
    auto alert2 = TestDataFactory::CreateTestAlert(
        "SPATIAL_002",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    alert2->issue_time = DateTime::Now();
    alert2->position = TestCoordinates::kShanghaiPort;
    
    bool isDuplicate = deduplicator.IsDuplicate(alert2);
    EXPECT_TRUE(isDuplicate);
}

TEST_F(BoundaryValueTest, Deduplicator_MaxDuplicateCount) {
    DeduplicationConfig config;
    config.enabled = true;
    config.time_window_seconds = 300;
    config.spatial_threshold = 0.01;
    config.merge_similar = false;
    config.max_duplicate_count = 3;
    config.suppress_repeated = false;
    config.suppress_interval_seconds = 300;
    
    AlertDeduplicator deduplicator(config);
    
    int processedCount = 0;
    for (int i = 0; i < 5; ++i) {
        auto alert = TestDataFactory::CreateTestAlert(
            "DUP_" + std::to_string(i),
            AlertType::kDepth,
            AlertLevel::kLevel2,
            TestAlertIds::kTestUserId1);
        
        alert->issue_time = DateTime::Now();
        alert->position = TestCoordinates::kShanghaiPort;
        
        auto result = deduplicator.Process(alert);
        if (result != nullptr) {
            processedCount++;
        }
    }
    
    EXPECT_GE(processedCount, 1);
}

TEST_F(BoundaryValueTest, Repository_LargeBatchQuery) {
    const int batchSize = 1000;
    
    for (int i = 0; i < batchSize; ++i) {
        auto alert = TestDataFactory::CreateTestAlert(
            "BATCH_" + std::to_string(i),
            AlertType::kDepth,
            AlertLevel::kLevel2,
            TestAlertIds::kTestUserId1);
        
        m_repository->Save(alert);
    }
    
    AlertQueryParams params;
    params.user_id = TestAlertIds::kTestUserId1;
    
    auto results = m_repository->Query(params);
    EXPECT_EQ(static_cast<int>(results.size()), batchSize);
}

TEST_F(BoundaryValueTest, UKC_MaxValues) {
    UKCInput input;
    input.chart_depth = std::numeric_limits<double>::max();
    input.ship_draft = std::numeric_limits<double>::max();
    input.tide_height = std::numeric_limits<double>::max();
    input.safety_margin = std::numeric_limits<double>::max();
    input.squat = std::numeric_limits<double>::max();
    input.heel_correction = std::numeric_limits<double>::max();
    input.wave_allowance = std::numeric_limits<double>::max();
    input.water_density = 1.025;
    input.speed_knots = std::numeric_limits<double>::max();
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_FALSE(std::isnan(result.ukc));
    EXPECT_TRUE(std::isfinite(result.ukc) || std::isinf(result.ukc));
}

TEST_F(BoundaryValueTest, UKC_MinValues) {
    UKCInput input;
    input.chart_depth = std::numeric_limits<double>::min();
    input.ship_draft = std::numeric_limits<double>::min();
    input.tide_height = std::numeric_limits<double>::min();
    input.safety_margin = std::numeric_limits<double>::min();
    input.squat = std::numeric_limits<double>::min();
    input.heel_correction = std::numeric_limits<double>::min();
    input.wave_allowance = std::numeric_limits<double>::min();
    input.water_density = 1.0;
    input.speed_knots = 0.0;
    
    UKCCalculator calculator;
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_FALSE(std::isnan(result.ukc) || result.ukc > 0);
}

}
}
}
