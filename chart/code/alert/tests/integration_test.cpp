#include <gtest/gtest.h>
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_checker.h"
#include "ogc/alert/depth_alert_checker.h"
#include "ogc/alert/alert_processor.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/push_service.h"
#include "ogc/alert/query_service.h"
#include "ogc/alert/config_service.h"
#include "ogc/alert/threshold_manager.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/scheduler.h"
#include <memory>
#include <thread>
#include <chrono>

namespace ogc {
namespace alert {
namespace test {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository = std::make_shared<AlertRepository>();
        m_engine = AlertEngine::Create();
        m_processor = IAlertProcessor::Create();
        m_pushService = std::make_shared<PushService>();
        m_queryService = std::make_shared<QueryService>(m_repository);
        m_configService = std::make_shared<ConfigService>();
        m_thresholdManager = ThresholdManager::Create();
        m_scheduler = Scheduler::Create();
    }
    
    void TearDown() override {
        if (m_engine && m_engine->IsRunning()) {
            m_engine->Stop();
        }
        if (m_scheduler && m_scheduler->IsRunning()) {
            m_scheduler->Stop();
        }
    }
    
    std::shared_ptr<AlertRepository> m_repository;
    std::unique_ptr<IAlertEngine> m_engine;
    std::unique_ptr<IAlertProcessor> m_processor;
    std::shared_ptr<PushService> m_pushService;
    std::shared_ptr<QueryService> m_queryService;
    std::shared_ptr<ConfigService> m_configService;
    std::unique_ptr<IThresholdManager> m_thresholdManager;
    std::unique_ptr<IScheduler> m_scheduler;
};

TEST_F(IntegrationTest, EndToEndAlertFlow) {
    EngineConfig config;
    config.check_interval_ms = 100;
    config.max_concurrent_checks = 5;
    m_engine->Initialize(config);
    
    auto depthChecker = std::shared_ptr<DepthAlertChecker>(DepthAlertChecker::Create().release());
    m_engine->RegisterChecker(depthChecker);
    
    m_engine->Start();
    EXPECT_TRUE(m_engine->IsRunning());
    
    AlertEvent event;
    event.event_id = "TEST_001";
    event.event_type = "depth_check";
    event.ship_id = "SHIP_001";
    event.user_id = "USER_001";
    event.priority = 1;
    
    m_engine->SubmitEvent(event);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    m_engine->Stop();
    EXPECT_FALSE(m_engine->IsRunning());
}

TEST_F(IntegrationTest, AlertRepositoryWithQueryService) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "INTEGRATION_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = "USER_001";
    alert->status = AlertStatus::kActive;
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto saved = m_repository->FindById("INTEGRATION_001");
    ASSERT_NE(saved, nullptr);
    EXPECT_EQ(saved->alert_type, AlertType::kDepth);
    
    auto activeAlerts = m_queryService->GetActiveAlerts("USER_001");
    EXPECT_EQ(activeAlerts.size(), 1);
}

TEST_F(IntegrationTest, ConfigServiceWithThresholdManager) {
    AlertConfig config;
    config.user_id = "USER_001";
    config.depth_threshold.level1_threshold = 5.0;
    config.collision_threshold.level1_threshold = 2.0;
    
    EXPECT_TRUE(m_configService->SetConfig("USER_001", config));
    
    auto loaded = m_configService->GetConfig("USER_001");
    EXPECT_DOUBLE_EQ(loaded.depth_threshold.level1_threshold, 5.0);
    
    auto threshold = m_thresholdManager->GetThreshold(AlertType::kDepth);
    EXPECT_GT(threshold.value.level1, 0.0);
}

TEST_F(IntegrationTest, ProcessorWithDeduplicator) {
    auto deduplicator = std::make_shared<AlertDeduplicator>();
    
    auto alert1 = std::make_shared<Alert>();
    alert1->alert_id = "DUP_001";
    alert1->alert_type = AlertType::kDepth;
    alert1->alert_level = AlertLevel::kLevel2;
    alert1->user_id = "USER_001";
    alert1->issue_time = DateTime::Now();
    alert1->position = Coordinate(120.0, 30.0);
    
    auto alert2 = std::make_shared<Alert>();
    alert2->alert_id = "DUP_002";
    alert2->alert_type = AlertType::kDepth;
    alert2->alert_level = AlertLevel::kLevel2;
    alert2->user_id = "USER_001";
    alert2->issue_time = DateTime::Now();
    alert2->position = Coordinate(120.0, 30.0);
    
    deduplicator->Process(alert1);
    EXPECT_TRUE(deduplicator->IsDuplicate(alert2));
    
    std::vector<AlertPtr> alerts = {alert1, alert2};
    auto processed = deduplicator->ProcessBatch(alerts);
    EXPECT_LE(processed.size(), 2);
}

TEST_F(IntegrationTest, SchedulerWithAlertEngine) {
    m_scheduler->Start();
    EXPECT_TRUE(m_scheduler->IsRunning());
    
    int executionCount = 0;
    std::string taskId = m_scheduler->ScheduleTask([&executionCount]() {
        executionCount++;
    }, 100, 3);
    
    EXPECT_FALSE(taskId.empty());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    
    EXPECT_GE(executionCount, 2);
    
    m_scheduler->Stop();
    EXPECT_FALSE(m_scheduler->IsRunning());
}

TEST_F(IntegrationTest, CPACalculatorWithAlertGeneration) {
    CPACalculator calculator;
    
    ShipMotion ownShip;
    ownShip.position = Coordinate(120.0, 30.0);
    ownShip.heading = 0.0;
    ownShip.speed = 10.0;
    
    ShipMotion targetShip;
    targetShip.position = Coordinate(120.1, 30.1);
    targetShip.heading = 180.0;
    targetShip.speed = 10.0;
    
    CPAResult result = calculator.Calculate(ownShip, targetShip);
    
    EXPECT_GT(result.cpa, 0.0);
    EXPECT_GE(result.tcpa, 0.0);
    
    if (result.cpa < 2.0) {
        auto alert = std::make_shared<CollisionAlert>();
        alert->alert_id = "COLLISION_001";
        alert->alert_type = AlertType::kCollision;
        alert->alert_level = AlertLevel::kLevel3;
        alert->cpa = result.cpa;
        alert->tcpa = result.tcpa;
        alert->target_ship_id = "TARGET_001";
        
        EXPECT_TRUE(m_repository->Save(alert));
    }
}

TEST_F(IntegrationTest, UKCCalculatorWithDepthAlert) {
    UKCCalculator calculator;
    
    UKCInput input;
    input.ship_draft = 5.0;
    input.chart_depth = 10.0;
    input.tide_height = 2.0;
    input.safety_margin = 0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    UKCResult result = calculator.Calculate(input);
    
    EXPECT_GT(result.ukc, 0.0);
    
    if (result.ukc < 1.0) {
        auto alert = std::make_shared<DepthAlert>();
        alert->alert_id = "DEPTH_001";
        alert->alert_type = AlertType::kDepth;
        alert->alert_level = AlertLevel::kLevel2;
        alert->current_depth = input.chart_depth + input.tide_height;
        alert->ukc = result.ukc;
        alert->safety_depth = input.ship_draft + input.safety_margin;
        
        EXPECT_TRUE(m_repository->Save(alert));
    }
}

TEST_F(IntegrationTest, PushServiceWithAlertRepository) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "PUSH_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel3;
    alert->user_id = "USER_001";
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    std::vector<PushMethod> level1 = {PushMethod::kApp};
    std::vector<PushMethod> level2 = {PushMethod::kApp, PushMethod::kSound};
    std::vector<PushMethod> level3 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms};
    std::vector<PushMethod> level4 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms, PushMethod::kEmail};
    
    m_pushService->SetPushStrategy(level1, level2, level3, level4);
    
    auto activeAlerts = m_repository->GetActiveAlerts("USER_001");
    EXPECT_EQ(activeAlerts.size(), 1);
}

TEST_F(IntegrationTest, QueryServiceWithPagination) {
    for (int i = 0; i < 25; i++) {
        auto alert = std::make_shared<Alert>();
        alert->alert_id = "PAGE_" + std::to_string(i);
        alert->alert_type = AlertType::kDepth;
        alert->alert_level = AlertLevel::kLevel2;
        alert->user_id = "USER_001";
        m_repository->Save(alert);
    }
    
    AlertQueryParams params;
    params.user_id = "USER_001";
    params.page = 1;
    params.page_size = 10;
    
    auto result = m_queryService->GetAlertList(params);
    EXPECT_EQ(result.alerts.size(), 10);
    EXPECT_EQ(result.total_count, 25);
    EXPECT_EQ(result.page, 1);
}

TEST_F(IntegrationTest, MultipleModulesCoordination) {
    EngineConfig engineConfig;
    engineConfig.check_interval_ms = 100;
    m_engine->Initialize(engineConfig);
    
    AlertConfig alertConfig;
    alertConfig.user_id = "USER_001";
    alertConfig.depth_threshold.level1_threshold = 5.0;
    m_configService->SetConfig("USER_001", alertConfig);
    
    auto depthChecker = std::shared_ptr<DepthAlertChecker>(DepthAlertChecker::Create().release());
    m_engine->RegisterChecker(depthChecker);
    
    m_engine->Start();
    
    AlertEvent event;
    event.event_id = "MULTI_001";
    event.event_type = "depth_check";
    event.user_id = "USER_001";
    m_engine->SubmitEvent(event);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto stats = m_engine->GetStatistics();
    EXPECT_GE(stats.total_alerts_generated, 0);
    
    m_engine->Stop();
}

}
}
}
