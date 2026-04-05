#include <gtest/gtest.h>
#include "ogc/alert/types.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/deduplicator.h"
#include "ogc/alert/threshold_manager.h"
#include "ogc/alert/weather_fusion.h"
#include "ogc/alert/notice_parser.h"
#include "ogc/alert/spatial_distance.h"
#include "ogc/alert/push_strategy.h"
#include "ogc/alert/user_config_store.h"
#include "ogc/alert/websocket_service.h"
#include "ogc/alert/feedback_service.h"
#include "ogc/alert/query_service.h"
#include "ogc/alert/alert_repository.h"
#include <memory>

namespace ogc {
namespace alert {
namespace test {

class CpaCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new CPACalculator());
    }
    
    std::unique_ptr<CPACalculator> m_calculator;
};

TEST_F(CpaCalculatorTest, CreateCalculator) {
    ASSERT_NE(m_calculator, nullptr);
}

TEST_F(CpaCalculatorTest, CalculateCPA) {
    ShipMotion ownShip;
    ownShip.position = Coordinate(120.0, 30.0);
    ownShip.heading = 0.0;
    ownShip.speed = 10.0;
    
    ShipMotion targetShip;
    targetShip.position = Coordinate(120.1, 30.1);
    targetShip.heading = 180.0;
    targetShip.speed = 10.0;
    
    CPAResult result = m_calculator->Calculate(ownShip, targetShip);
    EXPECT_GT(result.cpa, 0.0);
}

class DeduplicatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_deduplicator.reset(new AlertDeduplicator());
    }
    
    std::unique_ptr<AlertDeduplicator> m_deduplicator;
};

TEST_F(DeduplicatorTest, CreateDeduplicator) {
    ASSERT_NE(m_deduplicator, nullptr);
}

TEST_F(DeduplicatorTest, IsDuplicate) {
    auto alert1 = std::make_shared<Alert>();
    alert1->alert_id = "TEST_001";
    alert1->alert_type = AlertType::kDepth;
    alert1->alert_level = AlertLevel::kLevel2;
    alert1->user_id = "USER_001";
    alert1->issue_time = DateTime::Now();
    alert1->position = Coordinate(120.0, 30.0);
    
    auto alert2 = std::make_shared<Alert>();
    alert2->alert_id = "TEST_002";
    alert2->alert_type = AlertType::kDepth;
    alert2->alert_level = AlertLevel::kLevel2;
    alert2->user_id = "USER_001";
    alert2->issue_time = DateTime::Now();
    alert2->position = Coordinate(120.0, 30.0);
    
    m_deduplicator->Process(alert1);
    EXPECT_TRUE(m_deduplicator->IsDuplicate(alert2));
}

class ThresholdManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager = ThresholdManager::Create();
    }
    
    std::unique_ptr<IThresholdManager> m_manager;
};

TEST_F(ThresholdManagerTest, CreateManager) {
    ASSERT_NE(m_manager, nullptr);
}

TEST_F(ThresholdManagerTest, GetDefaultThreshold) {
    auto config = m_manager->GetThreshold(AlertType::kDepth);
    EXPECT_GT(config.value.level1, 0.0);
}

class WeatherFusionTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_fusion.reset(new WeatherFusion());
    }
    
    std::unique_ptr<WeatherFusion> m_fusion;
};

TEST_F(WeatherFusionTest, CreateFusion) {
    ASSERT_NE(m_fusion, nullptr);
}

class NoticeParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_parser.reset(new NoticeParser());
    }
    
    std::unique_ptr<NoticeParser> m_parser;
};

TEST_F(NoticeParserTest, CreateParser) {
    ASSERT_NE(m_parser, nullptr);
}

TEST_F(NoticeParserTest, ParseNotice) {
    std::string noticeContent = "NOTICE ID: NAV_001\n"
                                "TITLE: Military Exercise Warning\n"
                                "TYPE: WARNING\n"
                                "STATUS: ACTIVE\n"
                                "EFFECTIVE FROM: 2026-04-04T00:00:00\n"
                                "EFFECTIVE TO: 2026-04-10T00:00:00\n"
                                "DESCRIPTION: Military exercise in progress in East China Sea area.";
    
    auto result = m_parser->Parse(noticeContent);
    EXPECT_TRUE(result.valid);
}

class SpatialDistanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new SpatialDistance());
    }
    
    std::unique_ptr<SpatialDistance> m_calculator;
};

TEST_F(SpatialDistanceTest, CreateCalculator) {
    ASSERT_NE(m_calculator, nullptr);
}

TEST_F(SpatialDistanceTest, PointToPointDistance) {
    Coordinate from(120.0, 30.0);
    Coordinate to(121.0, 31.0);
    
    double distance = m_calculator->PointToPoint(from, to);
    EXPECT_GT(distance, 0.0);
    EXPECT_LT(distance, 200.0);
}

class PushStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager.reset(new PushStrategyManager());
    }
    
    std::unique_ptr<PushStrategyManager> m_manager;
};

TEST_F(PushStrategyTest, CreateManager) {
    ASSERT_NE(m_manager, nullptr);
}

TEST_F(PushStrategyTest, GetDefaultStrategy) {
    auto strategy = m_manager->GetDefaultStrategy(AlertLevel::kLevel2);
    EXPECT_FALSE(strategy.empty());
}

class UserConfigStoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_store.reset(new UserConfigStore());
    }
    
    std::unique_ptr<UserConfigStore> m_store;
};

TEST_F(UserConfigStoreTest, CreateStore) {
    ASSERT_NE(m_store, nullptr);
}

TEST_F(UserConfigStoreTest, SaveAndLoadConfig) {
    UserAlertConfig config;
    config.user_id = "USER_001";
    config.depth_alert_enabled = false;
    config.depth_threshold = 3.0;
    
    EXPECT_TRUE(m_store->SaveConfig(config));
    
    auto loaded = m_store->LoadConfig("USER_001");
    EXPECT_EQ(loaded.user_id, "USER_001");
    EXPECT_FALSE(loaded.depth_alert_enabled);
    EXPECT_DOUBLE_EQ(loaded.depth_threshold, 3.0);
}

class WebSocketServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_service.reset(new WebSocketService());
    }
    
    std::unique_ptr<WebSocketService> m_service;
};

TEST_F(WebSocketServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(WebSocketServiceTest, StartStopService) {
    WebSocketConfig config;
    config.port = 8080;
    
    EXPECT_TRUE(m_service->Start(config));
    EXPECT_TRUE(m_service->IsRunning());
    
    m_service->Stop();
    EXPECT_FALSE(m_service->IsRunning());
}

class FeedbackServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_service.reset(new FeedbackService());
    }
    
    std::unique_ptr<FeedbackService> m_service;
};

TEST_F(FeedbackServiceTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

TEST_F(FeedbackServiceTest, SubmitFeedback) {
    AlertFeedback feedback;
    feedback.alert_id = "ALERT_001";
    feedback.user_id = "USER_001";
    feedback.type = FeedbackType::POSITIVE;
    feedback.rating = 5;
    feedback.comment = "Very helpful alert";
    
    EXPECT_TRUE(m_service->SubmitFeedback(feedback));
}

class QueryServiceOptimizedTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_repository = std::make_shared<AlertRepository>();
        m_service.reset(new QueryService(m_repository));
    }
    
    std::shared_ptr<AlertRepository> m_repository;
    std::unique_ptr<QueryService> m_service;
};

TEST_F(QueryServiceOptimizedTest, CreateService) {
    ASSERT_NE(m_service, nullptr);
}

}
}
}
