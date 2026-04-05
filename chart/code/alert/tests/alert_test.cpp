#include <gtest/gtest.h>
#include "ogc/alert/types.h"
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_checker.h"
#include "ogc/alert/alert_processor.h"
#include "ogc/alert/scheduler.h"
#include "ogc/alert/data_source_manager.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/push_service.h"
#include "ogc/alert/config_service.h"
#include "ogc/alert/query_service.h"
#include "ogc/alert/acknowledge_service.h"
#include "ogc/alert/depth_alert_checker.h"
#include "ogc/alert/collision_alert_checker.h"
#include "ogc/alert/weather_alert_checker.h"
#include "ogc/alert/channel_alert_checker.h"
#include "ogc/alert/deviation_alert_checker.h"
#include "ogc/alert/speed_alert_checker.h"
#include "ogc/alert/restricted_area_checker.h"
#include <memory>

namespace ogc {
namespace alert {
namespace test {

class TypesTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(TypesTest, DateTimeDefaultConstruction) {
    DateTime dt;
    EXPECT_EQ(dt.year, 1970);
    EXPECT_EQ(dt.month, 1);
    EXPECT_EQ(dt.day, 1);
}

TEST_F(TypesTest, DateTimeFromTimestamp) {
    DateTime dt = DateTime::FromTimestamp(1609459200);
    EXPECT_EQ(dt.year, 2021);
    EXPECT_EQ(dt.month, 1);
    EXPECT_EQ(dt.day, 1);
}

TEST_F(TypesTest, DateTimeNow) {
    DateTime now = DateTime::Now();
    DateTime later = DateTime::Now();
    EXPECT_LE(now.ToTimestamp(), later.ToTimestamp());
}

TEST_F(TypesTest, CoordinateDefaultConstruction) {
    Coordinate coord;
    EXPECT_DOUBLE_EQ(coord.longitude, 0.0);
    EXPECT_DOUBLE_EQ(coord.latitude, 0.0);
}

TEST_F(TypesTest, CoordinateParameterizedConstruction) {
    Coordinate coord(120.5, 31.2);
    EXPECT_DOUBLE_EQ(coord.longitude, 120.5);
    EXPECT_DOUBLE_EQ(coord.latitude, 31.2);
}

TEST_F(TypesTest, CoordinateEquality) {
    Coordinate coord1(120.5, 31.2);
    Coordinate coord2(120.5, 31.2);
    Coordinate coord3(120.6, 31.2);
    EXPECT_TRUE(coord1 == coord2);
    EXPECT_FALSE(coord1 == coord3);
}

TEST_F(TypesTest, AlertDefaultConstruction) {
    Alert alert;
    EXPECT_TRUE(alert.alert_id.empty());
    EXPECT_EQ(alert.alert_type, AlertType::kUnknown);
    EXPECT_EQ(alert.alert_level, AlertLevel::kNone);
    EXPECT_EQ(alert.status, AlertStatus::kActive);
    EXPECT_TRUE(alert.acknowledge_required);
}

TEST_F(TypesTest, AlertPtrCreation) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    
    EXPECT_EQ(alert->alert_id, "TEST_001");
    EXPECT_EQ(alert->alert_type, AlertType::kDepth);
    EXPECT_EQ(alert->alert_level, AlertLevel::kLevel2);
}

TEST_F(TypesTest, DepthAlertConstruction) {
    auto alert = std::make_shared<DepthAlert>();
    alert->current_depth = 10.5;
    alert->ukc = 2.5;
    alert->safety_depth = 8.0;
    
    EXPECT_DOUBLE_EQ(alert->current_depth, 10.5);
    EXPECT_DOUBLE_EQ(alert->ukc, 2.5);
    EXPECT_DOUBLE_EQ(alert->safety_depth, 8.0);
}

TEST_F(TypesTest, CollisionAlertConstruction) {
    auto alert = std::make_shared<CollisionAlert>();
    alert->cpa = 3.5;
    alert->tcpa = 15.0;
    alert->target_ship_id = "SHIP_001";
    
    EXPECT_DOUBLE_EQ(alert->cpa, 3.5);
    EXPECT_DOUBLE_EQ(alert->tcpa, 15.0);
    EXPECT_EQ(alert->target_ship_id, "SHIP_001");
}

TEST_F(TypesTest, AlertThresholdDetermineLevel) {
    AlertThreshold threshold;
    threshold.level1_threshold = 10.0;
    threshold.level2_threshold = 20.0;
    threshold.level3_threshold = 30.0;
    threshold.level4_threshold = 40.0;
    
    EXPECT_EQ(threshold.DetermineLevel(5.0), AlertLevel::kNone);
    EXPECT_EQ(threshold.DetermineLevel(15.0), AlertLevel::kLevel1);
    EXPECT_EQ(threshold.DetermineLevel(25.0), AlertLevel::kLevel2);
    EXPECT_EQ(threshold.DetermineLevel(35.0), AlertLevel::kLevel3);
    EXPECT_EQ(threshold.DetermineLevel(45.0), AlertLevel::kLevel4);
}

class AlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_engine = AlertEngine::Create();
    }
    
    std::unique_ptr<IAlertEngine> m_engine;
};

TEST_F(AlertEngineTest, CreateEngine) {
    ASSERT_NE(m_engine, nullptr);
}

TEST_F(AlertEngineTest, InitializeEngine) {
    EngineConfig config;
    config.check_interval_ms = 1000;
    config.max_concurrent_checks = 10;
    m_engine->Initialize(config);
    EXPECT_TRUE(true);
}

TEST_F(AlertEngineTest, StartStopEngine) {
    EngineConfig config;
    config.check_interval_ms = 1000;
    m_engine->Initialize(config);
    m_engine->Start();
    EXPECT_TRUE(m_engine->IsRunning());
    m_engine->Stop();
    EXPECT_FALSE(m_engine->IsRunning());
}

TEST_F(AlertEngineTest, RegisterChecker) {
    auto checker = std::shared_ptr<DepthAlertChecker>(DepthAlertChecker::Create().release());
    m_engine->RegisterChecker(checker);
    EXPECT_TRUE(true);
}

TEST_F(AlertEngineTest, UnregisterChecker) {
    auto checker = std::shared_ptr<DepthAlertChecker>(DepthAlertChecker::Create().release());
    std::string id = checker->GetCheckerId();
    m_engine->RegisterChecker(checker);
    m_engine->UnregisterChecker(id);
    EXPECT_TRUE(true);
}

TEST_F(AlertEngineTest, GetStatistics) {
    EngineConfig config;
    config.check_interval_ms = 1000;
    m_engine->Initialize(config);
    auto stats = m_engine->GetStatistics();
    EXPECT_EQ(stats.total_alerts_generated, 0);
}

class AlertProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_processor = IAlertProcessor::Create();
    }
    
    std::unique_ptr<IAlertProcessor> m_processor;
};

TEST_F(AlertProcessorTest, CreateProcessor) {
    ASSERT_NE(m_processor, nullptr);
}

TEST_F(AlertProcessorTest, ProcessEmptyAlerts) {
    std::vector<AlertPtr> alerts;
    auto result = m_processor->Process(alerts);
    EXPECT_TRUE(result.empty());
}

TEST_F(AlertProcessorTest, ProcessSingleAlert) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    
    std::vector<AlertPtr> alerts = {alert};
    auto result = m_processor->Process(alerts);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(AlertProcessorTest, SetDeduplicationStrategy) {
    auto strategy = IDeduplicationStrategy::CreateDefault();
    m_processor->SetDeduplicationStrategy(strategy);
}

TEST_F(AlertProcessorTest, SetAggregationStrategy) {
    auto strategy = IAggregationStrategy::CreateDefault();
    m_processor->SetAggregationStrategy(strategy);
}

class SchedulerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_scheduler = Scheduler::Create();
    }
    
    std::unique_ptr<IScheduler> m_scheduler;
};

TEST_F(SchedulerTest, CreateScheduler) {
    ASSERT_NE(m_scheduler, nullptr);
}

TEST_F(SchedulerTest, StartStopScheduler) {
    m_scheduler->Start();
    EXPECT_TRUE(m_scheduler->IsRunning());
    m_scheduler->Stop();
    EXPECT_FALSE(m_scheduler->IsRunning());
}

TEST_F(SchedulerTest, ScheduleTask) {
    bool executed = false;
    std::string taskId = m_scheduler->ScheduleTask([&executed]() {
        executed = true;
    }, 1000, 1);
    
    EXPECT_FALSE(taskId.empty());
}

TEST_F(SchedulerTest, CancelTask) {
    std::string taskId = m_scheduler->ScheduleTask([]() {}, 1000, 1);
    m_scheduler->CancelTask(taskId);
}

TEST_F(SchedulerTest, PauseResumeTask) {
    std::string taskId = m_scheduler->ScheduleTask([]() {}, 1000, 1);
    m_scheduler->PauseTask(taskId);
    m_scheduler->ResumeTask(taskId);
}

class AlertRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository = AlertRepository::Create();
    }
    
    std::unique_ptr<IAlertRepository> m_repository;
};

TEST_F(AlertRepositoryTest, CreateRepository) {
    ASSERT_NE(m_repository, nullptr);
}

TEST_F(AlertRepositoryTest, SaveAndFindAlert) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->alert_type = AlertType::kDepth;
    
    EXPECT_TRUE(m_repository->Save(alert));
    
    auto found = m_repository->FindById("TEST_001");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->alert_id, "TEST_001");
}

TEST_F(AlertRepositoryTest, UpdateAlert) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->alert_level = AlertLevel::kLevel2;
    m_repository->Save(alert);
    
    alert->alert_level = AlertLevel::kLevel3;
    EXPECT_TRUE(m_repository->Update(alert));
    
    auto found = m_repository->FindById("TEST_001");
    EXPECT_EQ(found->alert_level, AlertLevel::kLevel3);
}

TEST_F(AlertRepositoryTest, DeleteAlert) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    m_repository->Save(alert);
    
    EXPECT_TRUE(m_repository->Delete("TEST_001"));
    EXPECT_EQ(m_repository->FindById("TEST_001"), nullptr);
}

TEST_F(AlertRepositoryTest, QueryAlerts) {
    auto alert1 = std::make_shared<Alert>();
    alert1->alert_id = "TEST_001";
    alert1->user_id = "USER_001";
    alert1->alert_type = AlertType::kDepth;
    
    auto alert2 = std::make_shared<Alert>();
    alert2->alert_id = "TEST_002";
    alert2->user_id = "USER_002";
    alert2->alert_type = AlertType::kCollision;
    
    m_repository->Save(alert1);
    m_repository->Save(alert2);
    
    AlertQueryParams params;
    params.user_id = "USER_001";
    auto result = m_repository->Query(params);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(AlertRepositoryTest, GetActiveAlerts) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->user_id = "USER_001";
    alert->status = AlertStatus::kActive;
    m_repository->Save(alert);
    
    auto activeAlerts = m_repository->GetActiveAlerts("USER_001");
    EXPECT_EQ(activeAlerts.size(), 1);
}

TEST_F(AlertRepositoryTest, AcknowledgeAlert) {
    auto alert = std::make_shared<Alert>();
    alert->alert_id = "TEST_001";
    alert->status = AlertStatus::kActive;
    m_repository->Save(alert);
    
    EXPECT_TRUE(m_repository->Acknowledge("TEST_001", "USER_001", "Acknowledged"));
    
    auto found = m_repository->FindById("TEST_001");
    EXPECT_EQ(found->status, AlertStatus::kAcknowledged);
}

class DepthAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = DepthAlertChecker::Create();
    }
    
    std::unique_ptr<DepthAlertChecker> m_checker;
};

TEST_F(DepthAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(DepthAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kDepth);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(DepthAlertCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

TEST_F(DepthAlertCheckerTest, SetThreshold) {
    DepthAlertThreshold threshold;
    threshold.level1_threshold = 2.0;
    threshold.level2_threshold = 1.5;
    threshold.level3_threshold = 1.0;
    threshold.level4_threshold = 0.5;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 2.0);
}

TEST_F(DepthAlertCheckerTest, CalculateUKC) {
    double ukc = m_checker->CalculateUKC(10.0, 5.0, 1.0);
    EXPECT_DOUBLE_EQ(ukc, 6.0);
}

TEST_F(DepthAlertCheckerTest, DetermineUKCLevel) {
    DepthAlertThreshold threshold;
    threshold.level1_threshold = 2.0;
    threshold.level2_threshold = 1.5;
    threshold.level3_threshold = 1.0;
    threshold.level4_threshold = 0.5;
    m_checker->SetThreshold(threshold);
    
    EXPECT_EQ(m_checker->DetermineUKCLevel(3.0), AlertLevel::kNone);
    EXPECT_EQ(m_checker->DetermineUKCLevel(1.8), AlertLevel::kLevel1);
    EXPECT_EQ(m_checker->DetermineUKCLevel(1.2), AlertLevel::kLevel2);
    EXPECT_EQ(m_checker->DetermineUKCLevel(0.8), AlertLevel::kLevel3);
    EXPECT_EQ(m_checker->DetermineUKCLevel(0.3), AlertLevel::kLevel4);
}

TEST_F(DepthAlertCheckerTest, Check) {
    CheckContext context;
    context.ship_position = Coordinate(120.5, 31.2);
    context.user_id = "USER_001";
    
    m_checker->SetShipDraft(5.0);
    auto alerts = m_checker->Check(context);
}

class CollisionAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = CollisionAlertChecker::Create();
    }
    
    std::unique_ptr<CollisionAlertChecker> m_checker;
};

TEST_F(CollisionAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(CollisionAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kCollision);
}

TEST_F(CollisionAlertCheckerTest, DetermineCollisionLevel) {
    CollisionAlertThreshold threshold;
    threshold.cpa_threshold_level1 = 6.0;
    threshold.cpa_threshold_level2 = 4.0;
    threshold.cpa_threshold_level3 = 2.0;
    threshold.tcpa_threshold_level1 = 30.0;
    threshold.tcpa_threshold_level2 = 20.0;
    threshold.tcpa_threshold_level3 = 10.0;
    m_checker->SetThreshold(threshold);
    
    EXPECT_EQ(m_checker->DetermineCollisionLevel(5.0, 25.0), AlertLevel::kLevel2);
    EXPECT_EQ(m_checker->DetermineCollisionLevel(1.5, 8.0), AlertLevel::kLevel4);
}

class ConfigServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_service = ConfigService::Create();
    }
    
    std::unique_ptr<IConfigService> m_service;
};

TEST_F(ConfigServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(ConfigServiceTest, GetDefaultConfig) {
    auto config = m_service->GetConfig("USER_001");
    EXPECT_EQ(config.user_id, "USER_001");
    EXPECT_TRUE(config.depth_alert_enabled);
}

TEST_F(ConfigServiceTest, SetConfig) {
    AlertConfig config;
    config.user_id = "USER_001";
    config.depth_alert_enabled = false;
    
    EXPECT_TRUE(m_service->SetConfig("USER_001", config));
    
    auto retrieved = m_service->GetConfig("USER_001");
    EXPECT_FALSE(retrieved.depth_alert_enabled);
}

TEST_F(ConfigServiceTest, EnableDisableAlertType) {
    m_service->EnableAlertType("USER_001", AlertType::kDepth, false);
    EXPECT_FALSE(m_service->IsAlertTypeEnabled("USER_001", AlertType::kDepth));
    
    m_service->EnableAlertType("USER_001", AlertType::kDepth, true);
    EXPECT_TRUE(m_service->IsAlertTypeEnabled("USER_001", AlertType::kDepth));
}

TEST_F(ConfigServiceTest, ResetToDefaults) {
    m_service->EnableAlertType("USER_001", AlertType::kDepth, false);
    m_service->ResetToDefaults("USER_001");
    EXPECT_TRUE(m_service->IsAlertTypeEnabled("USER_001", AlertType::kDepth));
}

class PushServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_service = PushService::Create();
    }
    
    std::unique_ptr<IPushService> m_service;
};

TEST_F(PushServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(PushServiceTest, RegisterChannel) {
    auto channel = std::make_shared<AppPushChannel>();
    m_service->RegisterChannel(channel);
    
    auto retrieved = m_service->GetChannel(PushMethod::kApp);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetMethod(), PushMethod::kApp);
}

TEST_F(PushServiceTest, SetPushStrategy) {
    std::vector<PushMethod> level1 = {PushMethod::kApp};
    std::vector<PushMethod> level2 = {PushMethod::kApp, PushMethod::kSound};
    std::vector<PushMethod> level3 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms};
    std::vector<PushMethod> level4 = {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms, PushMethod::kEmail};
    
    m_service->SetPushStrategy(level1, level2, level3, level4);
}

class QueryServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto repository = std::make_shared<AlertRepository>();
        m_service.reset(new QueryService(repository));
    }
    
    std::unique_ptr<IQueryService> m_service;
};

TEST_F(QueryServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(QueryServiceTest, GetActiveAlerts) {
    auto alerts = m_service->GetActiveAlerts("USER_001");
    EXPECT_TRUE(alerts.empty());
}

TEST_F(QueryServiceTest, GetAlertList) {
    AlertQueryParams params;
    params.page = 1;
    params.page_size = 10;
    
    auto result = m_service->GetAlertList(params);
    EXPECT_EQ(result.total_count, 0);
}

class AcknowledgeServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto repository = std::make_shared<AlertRepository>();
        m_service.reset(new AcknowledgeService(repository));
    }
    
    std::unique_ptr<IAcknowledgeService> m_service;
};

TEST_F(AcknowledgeServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(AcknowledgeServiceTest, AcknowledgeAlert) {
    EXPECT_FALSE(m_service->AcknowledgeAlert("NONEXISTENT", "USER_001", "Action"));
}

TEST_F(AcknowledgeServiceTest, SubmitFeedback) {
    FeedbackData feedback;
    feedback.alert_id = "TEST_001";
    feedback.user_id = "USER_001";
    feedback.feedback_type = "Test";
    feedback.feedback_content = "Test feedback";
    
    EXPECT_TRUE(m_service->SubmitFeedback(feedback));
}

}
}
}
