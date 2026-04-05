#include <gtest/gtest.h>
#include "test_data.h"
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_processor.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/push_service.h"
#include "ogc/alert/query_service.h"
#include "ogc/alert/config_service.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/depth_alert_checker.h"
#include "ogc/alert/collision_alert_checker.h"
#include <memory>
#include <thread>
#include <chrono>

namespace ogc {
namespace alert {
namespace test {

class CrossModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository = std::make_shared<AlertRepository>();
        m_engine = AlertEngine::Create();
        m_processor = IAlertProcessor::Create();
        m_pushService = std::make_shared<PushService>();
        m_queryService = std::make_shared<QueryService>(m_repository);
        m_configService = std::make_shared<ConfigService>();
    }
    
    void TearDown() override {
        if (m_engine && m_engine->IsRunning()) {
            m_engine->Stop();
        }
    }
    
    std::shared_ptr<AlertRepository> m_repository;
    std::unique_ptr<IAlertEngine> m_engine;
    std::unique_ptr<IAlertProcessor> m_processor;
    std::shared_ptr<PushService> m_pushService;
    std::shared_ptr<QueryService> m_queryService;
    std::shared_ptr<ConfigService> m_configService;
};

TEST_F(CrossModuleTest, UKCCalculatorToAlertRepository) {
    UKCCalculator calculator;
    UKCInput input = TestDataFactory::CreateUKCInput(
        TestUKCData::kShallowDepth,
        TestUKCData::kDefaultDraft,
        TestUKCData::kDefaultTide);
    
    UKCResult result = calculator.Calculate(input);
    
    auto alert = std::make_shared<DepthAlert>();
    alert->alert_id = "UKC_CROSS_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = result.ukc < 1.0 ? AlertLevel::kLevel3 : AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    alert->current_depth = input.chart_depth + input.tide_height;
    alert->ukc = result.ukc;
    alert->safety_depth = input.ship_draft + input.safety_margin;
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("UKC_CROSS_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_type, AlertType::kDepth);
    
    DepthAlert* depthAlert = static_cast<DepthAlert*>(saved.get());
    EXPECT_DOUBLE_EQ(depthAlert->current_depth, input.chart_depth + input.tide_height);
}

TEST_F(CrossModuleTest, CPACalculatorToAlertRepository) {
    CPACalculator calculator;
    ShipMotion ship1 = TestDataFactory::CreateShipMotion(
        TestCoordinates::kShanghaiPort,
        TestShipData::kDefaultSpeed,
        TestShipData::kNorthHeading);
    
    ShipMotion ship2 = TestDataFactory::CreateShipMotion(
        Coordinate(TestCoordinates::kShanghaiPort.longitude + 0.01,
                   TestCoordinates::kShanghaiPort.latitude),
        TestShipData::kDefaultSpeed,
        TestShipData::kSouthHeading);
    
    CPAResult result = calculator.Calculate(ship1, ship2);
    
    auto alert = std::make_shared<CollisionAlert>();
    alert->alert_id = "CPA_CROSS_001";
    alert->alert_type = AlertType::kCollision;
    alert->alert_level = result.cpa < 2.0 ? AlertLevel::kLevel3 : AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->issue_time = DateTime::Now();
    alert->position = ship1.position;
    alert->cpa = result.cpa;
    alert->tcpa = result.tcpa;
    alert->target_ship_id = "TARGET_001";
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("CPA_CROSS_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_type, AlertType::kCollision);
    
    CollisionAlert* collisionAlert = static_cast<CollisionAlert*>(saved.get());
    EXPECT_DOUBLE_EQ(collisionAlert->cpa, result.cpa);
}

TEST_F(CrossModuleTest, AlertProcessorToRepository) {
    auto alert1 = TestDataFactory::CreateTestAlert(
        "PROC_CROSS_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    EXPECT_TRUE(m_repository->Save(alert1));
    
    auto saved1 = m_repository->FindById("PROC_CROSS_001");
    EXPECT_NE(saved1, nullptr);
}

TEST_F(CrossModuleTest, DeduplicatorToRepository) {
    DeduplicationConfig config;
    config.enabled = true;
    config.time_window_seconds = 300;
    config.spatial_threshold = 0.01;
    config.merge_similar = false;
    config.max_duplicate_count = 5;
    
    AlertDeduplicator deduplicator(config);
    
    auto alert1 = TestDataFactory::CreateTestAlert(
        "DEDUP_CROSS_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    alert1->issue_time = DateTime::Now();
    alert1->position = TestCoordinates::kShanghaiPort;
    
    auto result1 = deduplicator.Process(alert1);
    if (result1) {
        EXPECT_TRUE(m_repository->Save(result1));
    }
    
    auto alert2 = TestDataFactory::CreateTestAlert(
        "DEDUP_CROSS_002",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    alert2->issue_time = DateTime::Now();
    alert2->position = TestCoordinates::kShanghaiPort;
    
    auto result2 = deduplicator.Process(alert2);
    if (result2) {
        m_repository->Save(result2);
    }
    
    auto allAlerts = m_repository->GetActiveAlerts(TestAlertIds::kTestUserId1);
    EXPECT_GE(allAlerts.size(), 1);
}

TEST_F(CrossModuleTest, ConfigServiceToAlertEngine) {
    AlertConfig config;
    config.user_id = TestAlertIds::kTestUserId1;
    config.depth_threshold.level1_threshold = TestThresholds::kDepthWarningThreshold;
    config.depth_threshold.level2_threshold = TestThresholds::kDepthCriticalThreshold;
    config.depth_threshold.level3_threshold = TestThresholds::kDepthCriticalThreshold;
    config.collision_threshold.level1_threshold = TestThresholds::kCPAWarningThreshold;
    
    EXPECT_TRUE(m_configService->SetConfig(TestAlertIds::kTestUserId1, config));
    
    auto loaded = m_configService->GetConfig(TestAlertIds::kTestUserId1);
    EXPECT_DOUBLE_EQ(loaded.depth_threshold.level1_threshold, TestThresholds::kDepthWarningThreshold);
    
    EngineConfig engineConfig;
    engineConfig.check_interval_ms = 100;
    m_engine->Initialize(engineConfig);
    
    auto depthChecker = std::shared_ptr<DepthAlertChecker>(DepthAlertChecker::Create().release());
    m_engine->RegisterChecker(depthChecker);
    
    m_engine->Start();
    EXPECT_TRUE(m_engine->IsRunning());
    
    m_engine->Stop();
}

TEST_F(CrossModuleTest, RepositoryToQueryService) {
    for (int i = 0; i < 10; ++i) {
        auto alert = TestDataFactory::CreateTestAlert(
            "QUERY_CROSS_" + std::to_string(i),
            AlertType::kDepth,
            AlertLevel::kLevel2,
            TestAlertIds::kTestUserId1);
        
        m_repository->Save(alert);
    }
    
    AlertQueryParams params;
    params.user_id = TestAlertIds::kTestUserId1;
    params.page = 1;
    params.page_size = 5;
    
    auto result = m_queryService->GetAlertList(params);
    EXPECT_EQ(result.alerts.size(), 5);
    EXPECT_EQ(result.total_count, 10);
}

TEST_F(CrossModuleTest, RepositoryToPushService) {
    auto alert = TestDataFactory::CreateTestAlert(
        "PUSH_CROSS_001",
        AlertType::kDepth,
        AlertLevel::kLevel3,
        TestAlertIds::kTestUserId1);
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    std::vector<PushMethod> level1 = {PushMethod::kApp};
    std::vector<PushMethod> level2 = {PushMethod::kApp, PushMethod::kSound};
    std::vector<PushMethod> level3 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms};
    std::vector<PushMethod> level4 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms, PushMethod::kEmail};
    
    m_pushService->SetPushStrategy(level1, level2, level3, level4);
    
    auto activeAlerts = m_repository->GetActiveAlerts(TestAlertIds::kTestUserId1);
    EXPECT_EQ(activeAlerts.size(), 1);
}

TEST_F(CrossModuleTest, FullAlertPipeline) {
    UKCCalculator ukcCalc;
    UKCInput input = TestDataFactory::CreateUKCInput(
        TestUKCData::kShallowDepth,
        TestUKCData::kDefaultDraft,
        TestUKCData::kDefaultTide);
    
    UKCResult ukcResult = ukcCalc.Calculate(input);
    
    auto alert = std::make_shared<DepthAlert>();
    alert->alert_id = "PIPELINE_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = ukcResult.ukc < 1.0 ? AlertLevel::kLevel3 : AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    alert->current_depth = input.chart_depth + input.tide_height;
    alert->ukc = ukcResult.ukc;
    alert->safety_depth = input.ship_draft + input.safety_margin;
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("PIPELINE_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_type, AlertType::kDepth);
}

TEST_F(CrossModuleTest, AlertStatusTransition) {
    auto alert = TestDataFactory::CreateTestAlert(
        "STATUS_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    alert->status = AlertStatus::kActive;
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("STATUS_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->status, AlertStatus::kActive);
    
    saved->status = AlertStatus::kAcknowledged;
    EXPECT_TRUE(m_repository->Update(saved));
    
    auto updated = m_repository->FindById("STATUS_001");
    ASSERT_NE(updated, nullptr);
    EXPECT_EQ(updated->status, AlertStatus::kAcknowledged);
    
    updated->status = AlertStatus::kDismissed;
    EXPECT_TRUE(m_repository->Update(updated));
    
    auto dismissed = m_repository->FindById("STATUS_001");
    ASSERT_NE(dismissed, nullptr);
    EXPECT_EQ(dismissed->status, AlertStatus::kDismissed);
}

TEST_F(CrossModuleTest, BatchAlertProcessing) {
    int savedCount = 0;
    for (int i = 0; i < 50; ++i) {
        auto alert = TestDataFactory::CreateTestAlert(
            "BATCH_" + std::to_string(i),
            AlertType::kDepth,
            AlertLevel::kLevel2,
            TestAlertIds::kTestUserId1);
        
        if (m_repository->Save(alert)) {
            savedCount++;
        }
    }
    
    EXPECT_GE(savedCount, 1);
    
    AlertQueryParams params;
    params.user_id = TestAlertIds::kTestUserId1;
    auto result = m_queryService->GetAlertList(params);
    EXPECT_GE(result.total_count, 1);
}

TEST_F(CrossModuleTest, AlertExpiration) {
    auto alert = TestDataFactory::CreateTestAlert(
        "EXPIRE_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("EXPIRE_001");
    ASSERT_NE(saved, nullptr);
}

TEST_F(CrossModuleTest, AlertPriorityOrdering) {
    auto alert1 = TestDataFactory::CreateTestAlert(
        "PRIORITY_001",
        AlertType::kDepth,
        AlertLevel::kLevel1,
        TestAlertIds::kTestUserId1);
    
    auto alert2 = TestDataFactory::CreateTestAlert(
        "PRIORITY_002",
        AlertType::kDepth,
        AlertLevel::kLevel3,
        TestAlertIds::kTestUserId1);
    
    auto alert3 = TestDataFactory::CreateTestAlert(
        "PRIORITY_003",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    m_repository->Save(alert1);
    m_repository->Save(alert2);
    m_repository->Save(alert3);
    
    auto activeAlerts = m_repository->GetActiveAlerts(TestAlertIds::kTestUserId1);
    EXPECT_EQ(activeAlerts.size(), 3);
}

TEST_F(CrossModuleTest, MultiUserIsolation) {
    auto alert1 = TestDataFactory::CreateTestAlert(
        "USER1_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    auto alert2 = TestDataFactory::CreateTestAlert(
        "USER2_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId2);
    
    m_repository->Save(alert1);
    m_repository->Save(alert2);
    
    auto user1Alerts = m_repository->GetActiveAlerts(TestAlertIds::kTestUserId1);
    auto user2Alerts = m_repository->GetActiveAlerts(TestAlertIds::kTestUserId2);
    
    EXPECT_EQ(user1Alerts.size(), 1);
    EXPECT_EQ(user2Alerts.size(), 1);
    EXPECT_EQ(user1Alerts[0]->alert_id, "USER1_001");
    EXPECT_EQ(user2Alerts[0]->alert_id, "USER2_001");
}

TEST_F(CrossModuleTest, AlertTypeFiltering) {
    auto depthAlert = TestDataFactory::CreateTestAlert(
        "TYPE_DEPTH_001",
        AlertType::kDepth,
        AlertLevel::kLevel2,
        TestAlertIds::kTestUserId1);
    
    m_repository->Save(depthAlert);
    
    auto saved = m_repository->FindById("TYPE_DEPTH_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_type, AlertType::kDepth);
}

TEST_F(CrossModuleTest, AlertLevelFiltering) {
    auto alert1 = TestDataFactory::CreateTestAlert(
        "LEVEL1_001",
        AlertType::kDepth,
        AlertLevel::kLevel1,
        TestAlertIds::kTestUserId1);
    
    m_repository->Save(alert1);
    
    auto saved = m_repository->FindById("LEVEL1_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_level, AlertLevel::kLevel1);
}

}
}
}
