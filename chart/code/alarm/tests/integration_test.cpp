#include <gtest/gtest.h>
#include "alert/alert_type_adapter.h"
#include "alert/checker_adapter.h"
#include "alert/push_channel_adapter.h"
#include "alert/alert_engine_factory.h"
#include "ogc/alert/alert_checker.h"
#include "ogc/alert/push_service.h"

using namespace alarm;
using namespace alert;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        AlertEngineFactory::GetInstance().RegisterAlertCheckers();
    }
    
    void TearDown() override {
    }
};

TEST_F(IntegrationTest, AlertTypeAdapter_ConvertAlertType) {
    EXPECT_EQ(AlertTypeAdapter::ToAlertType(AlertType::kDepthAlert), 
              ogc::alert::AlertType::kDepth);
    EXPECT_EQ(AlertTypeAdapter::ToAlertType(AlertType::kCollisionAlert), 
              ogc::alert::AlertType::kCollision);
    EXPECT_EQ(AlertTypeAdapter::ToAlertType(AlertType::kWeatherAlert), 
              ogc::alert::AlertType::kWeather);
    EXPECT_EQ(AlertTypeAdapter::ToAlertType(AlertType::kChannelAlert), 
              ogc::alert::AlertType::kChannel);
    EXPECT_EQ(AlertTypeAdapter::ToAlertType(AlertType::kOtherAlert), 
              ogc::alert::AlertType::kUnknown);
}

TEST_F(IntegrationTest, AlertTypeAdapter_ConvertAlertLevel) {
    EXPECT_EQ(AlertTypeAdapter::ToAlertLevel(AlertLevel::kLevel1_Critical), 
              ogc::alert::AlertLevel::kLevel1);
    EXPECT_EQ(AlertTypeAdapter::ToAlertLevel(AlertLevel::kLevel2_Severe), 
              ogc::alert::AlertLevel::kLevel2);
    EXPECT_EQ(AlertTypeAdapter::ToAlertLevel(AlertLevel::kLevel3_Moderate), 
              ogc::alert::AlertLevel::kLevel3);
    EXPECT_EQ(AlertTypeAdapter::ToAlertLevel(AlertLevel::kLevel4_Minor), 
              ogc::alert::AlertLevel::kLevel4);
    EXPECT_EQ(AlertTypeAdapter::ToAlertLevel(AlertLevel::kNone), 
              ogc::alert::AlertLevel::kNone);
}

TEST_F(IntegrationTest, AlertTypeAdapter_ConvertAlertStatus) {
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kPending), 
              ogc::alert::AlertStatus::kPending);
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kActive), 
              ogc::alert::AlertStatus::kActive);
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kPushed), 
              ogc::alert::AlertStatus::kPushed);
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kAcknowledged), 
              ogc::alert::AlertStatus::kAcknowledged);
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kCleared), 
              ogc::alert::AlertStatus::kCleared);
    EXPECT_EQ(AlertTypeAdapter::ToAlertStatus(AlertStatus::kExpired), 
              ogc::alert::AlertStatus::kExpired);
}

TEST_F(IntegrationTest, AlertTypeAdapter_ConvertAlert) {
    Alert alarm_alert;
    alarm_alert.SetAlertId("test-001");
    alarm_alert.SetAlertType(AlertType::kDepthAlert);
    alarm_alert.SetAlertLevel(AlertLevel::kLevel1_Critical);
    alarm_alert.SetStatus(AlertStatus::kActive);
    alarm_alert.SetTitle("Depth Alert");
    alarm_alert.SetMessage("Water depth too shallow");
    alarm_alert.SetPosition(120.5, 31.2);
    
    ogc::alert::AlertPtr alert_ptr = AlertTypeAdapter::ToAlertPtr(alarm_alert);
    
    ASSERT_NE(alert_ptr, nullptr);
    EXPECT_EQ(alert_ptr->alert_id, "test-001");
    EXPECT_EQ(alert_ptr->alert_type, ogc::alert::AlertType::kDepth);
    EXPECT_EQ(alert_ptr->alert_level, ogc::alert::AlertLevel::kLevel1);
    EXPECT_EQ(alert_ptr->status, ogc::alert::AlertStatus::kActive);
    EXPECT_EQ(alert_ptr->content.title, "Depth Alert");
    EXPECT_EQ(alert_ptr->content.message, "Water depth too shallow");
    EXPECT_DOUBLE_EQ(alert_ptr->position.longitude, 120.5);
    EXPECT_DOUBLE_EQ(alert_ptr->position.latitude, 31.2);
}

TEST_F(IntegrationTest, AlertTypeAdapter_RoundTrip) {
    Alert original;
    original.SetAlertId("roundtrip-001");
    original.SetAlertType(AlertType::kCollisionAlert);
    original.SetAlertLevel(AlertLevel::kLevel2_Severe);
    original.SetStatus(AlertStatus::kActive);
    original.SetTitle("Collision Alert");
    original.SetMessage("CPA too small");
    original.SetPosition(121.0, 31.5);
    
    ogc::alert::AlertPtr converted = AlertTypeAdapter::ToAlertPtr(original);
    Alert restored = AlertTypeAdapter::FromAlertPtr(converted);
    
    EXPECT_EQ(restored.GetAlertId(), original.GetAlertId());
    EXPECT_EQ(restored.GetAlertType(), original.GetAlertType());
    EXPECT_EQ(restored.GetAlertLevel(), original.GetAlertLevel());
    EXPECT_EQ(restored.GetStatus(), original.GetStatus());
    EXPECT_EQ(restored.GetTitle(), original.GetTitle());
    EXPECT_EQ(restored.GetMessage(), original.GetMessage());
    EXPECT_DOUBLE_EQ(restored.GetPosition().longitude, original.GetPosition().longitude);
    EXPECT_DOUBLE_EQ(restored.GetPosition().latitude, original.GetPosition().latitude);
}

TEST_F(IntegrationTest, CheckerAdapter_CreateEngine) {
    auto checker = ogc::alert::IAlertChecker::Create(ogc::alert::AlertType::kDepth);
    ASSERT_NE(checker, nullptr);
    
    CheckerRegistry::Instance().RegisterChecker(
        AlertType::kDepthAlert, 
        std::shared_ptr<ogc::alert::IAlertChecker>(checker.release()));
    
    auto engine = CheckerRegistry::Instance().CreateEngine(AlertType::kDepthAlert);
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), AlertType::kDepthAlert);
}

TEST_F(IntegrationTest, AlertEngineFactory_CreateEngineFromChecker) {
    AlertEngineFactory::GetInstance().RegisterAlertCheckers();
    
    EXPECT_TRUE(AlertEngineFactory::GetInstance().IsEngineRegistered(AlertType::kDepthAlert));
    EXPECT_TRUE(AlertEngineFactory::GetInstance().IsEngineRegistered(AlertType::kCollisionAlert));
    EXPECT_TRUE(AlertEngineFactory::GetInstance().IsEngineRegistered(AlertType::kWeatherAlert));
    EXPECT_TRUE(AlertEngineFactory::GetInstance().IsEngineRegistered(AlertType::kChannelAlert));
}

TEST_F(IntegrationTest, AlertEngineFactory_CreateEngineEx) {
    AlertEngineFactory::GetInstance().RegisterAlertCheckers();
    
    auto engine = AlertEngineFactory::GetInstance().CreateEngineEx(AlertType::kDepthAlert);
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), AlertType::kDepthAlert);
}

TEST_F(IntegrationTest, PushChannelAdapter_CreateAdapter) {
    auto adapter = CreateChannelAdapter(PushChannel::kWebSocket);
    ASSERT_NE(adapter, nullptr);
    EXPECT_EQ(adapter->GetMethod(), ogc::alert::PushMethod::kApp);
    
    adapter = CreateChannelAdapter(PushChannel::kSms);
    ASSERT_NE(adapter, nullptr);
    EXPECT_EQ(adapter->GetMethod(), ogc::alert::PushMethod::kSms);
    
    adapter = CreateChannelAdapter(PushChannel::kEmail);
    ASSERT_NE(adapter, nullptr);
    EXPECT_EQ(adapter->GetMethod(), ogc::alert::PushMethod::kEmail);
    
    adapter = CreateChannelAdapter(PushChannel::kHttp);
    ASSERT_NE(adapter, nullptr);
    EXPECT_EQ(adapter->GetMethod(), ogc::alert::PushMethod::kApp);
}

TEST_F(IntegrationTest, PushChannelAdapter_WebSocketChannel) {
    auto ws_adapter = std::make_shared<WebSocketChannelAdapter>();
    
    EXPECT_FALSE(ws_adapter->IsAvailable());
    
    ws_adapter->SetSender([](const std::string& message) {
        return !message.empty();
    });
    
    EXPECT_TRUE(ws_adapter->IsAvailable());
    
    auto alert = std::make_shared<ogc::alert::Alert>();
    alert->alert_id = "test-push-001";
    alert->content.message = "Test message";
    
    ogc::alert::PushResult result = ws_adapter->Send(alert, "user1");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.error_message, "");
}

TEST_F(IntegrationTest, PushChannelAdapter_PushServiceIntegration) {
    auto push_service = ogc::alert::IPushService::Create();
    ASSERT_NE(push_service, nullptr);
    
    PushChannelAdapter adapter;
    adapter.SetPushService(std::shared_ptr<ogc::alert::IPushService>(push_service.release()));
    
    PushConfig config;
    config.enableWebSocket = true;
    config.maxRetries = 3;
    adapter.SetConfig(config);
    
    PushConfig retrieved = adapter.GetConfig();
    EXPECT_TRUE(retrieved.enableWebSocket);
    EXPECT_EQ(retrieved.maxRetries, 3);
}

TEST_F(IntegrationTest, FullIntegration_AlertWorkflow) {
    AlertEngineFactory::GetInstance().RegisterAlertCheckers();
    
    auto engine = AlertEngineFactory::GetInstance().CreateEngineEx(AlertType::kDepthAlert);
    ASSERT_NE(engine, nullptr);
    
    AlertContext context;
    ShipInfo ship_info;
    ship_info.SetShipId("SHIP001");
    ship_info.SetShipName("Test Ship");
    context.SetShipInfo(ship_info);
    context.SetPosition(Coordinate(120.5, 31.2));
    context.SetSpeed(10.0);
    context.SetHeading(45.0);
    
    ThresholdConfig threshold;
    threshold.SetLevel1Threshold(0.5);
    threshold.SetLevel2Threshold(1.0);
    threshold.SetLevel3Threshold(2.0);
    threshold.SetLevel4Threshold(3.0);
    engine->SetThreshold(threshold);
    
    ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.5);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel2Threshold(), 1.0);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel3Threshold(), 2.0);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel4Threshold(), 3.0);
}

TEST_F(IntegrationTest, CoordinateAdapter_Convert) {
    alert::Coordinate alarm_coord(120.5, 31.2);
    
    ogc::alert::Coordinate alert_coord = AlertTypeAdapter::ToCoordinate(alarm_coord);
    EXPECT_DOUBLE_EQ(alert_coord.longitude, 120.5);
    EXPECT_DOUBLE_EQ(alert_coord.latitude, 31.2);
    
    alert::Coordinate restored = AlertTypeAdapter::FromCoordinate(alert_coord);
    EXPECT_DOUBLE_EQ(restored.GetLongitude(), 120.5);
    EXPECT_DOUBLE_EQ(restored.GetLatitude(), 31.2);
}

#include "alert/rest_adapter.h"

class ApiIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_adapter = new RestAdapter();
        m_adapter->RegisterDefaultRoutes();
    }
    
    void TearDown() override {
        delete m_adapter;
    }
    
    RestAdapter* m_adapter;
};

TEST_F(ApiIntegrationTest, GetAlerts_ReturnsSuccess) {
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
    EXPECT_FALSE(response.body.empty());
}

TEST_F(ApiIntegrationTest, GetActiveAlerts_ReturnsSuccess) {
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/active";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, GetConfig_ReturnsSuccess) {
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/config";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, UpdateConfig_ReturnsSuccess) {
    ogc::alert::HttpRequest request;
    request.method = "PUT";
    request.path = "/api/v1/alerts/config";
    request.body = R"({"enabled_channels":{"app":true,"sms":false}})";
    request.content_type = "application/json";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
}

TEST_F(ApiIntegrationTest, GetPushStatus_ReturnsSuccess) {
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/push/status";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, NotFound_Returns404) {
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/nonexistent";
    
    ogc::alert::HttpResponse response = m_adapter->HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 404);
}

TEST_F(ApiIntegrationTest, AlertControllerAdapter_GetAlertById) {
    AlertControllerAdapter adapter;
    
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/ALERT001";
    
    ogc::alert::HttpResponse response = adapter.GetAlertById(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, AlertControllerAdapter_AcknowledgeAlert) {
    AlertControllerAdapter adapter;
    
    ogc::alert::HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/alerts/ALERT001/acknowledge";
    request.body = R"({"action_taken":"减速航行"})";
    request.content_type = "application/json";
    
    ogc::alert::HttpResponse response = adapter.AcknowledgeAlert(request);
    
    EXPECT_EQ(response.status_code, 200);
}

TEST_F(ApiIntegrationTest, ConfigControllerAdapter_GetThreshold) {
    ConfigControllerAdapter adapter;
    
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/config/thresholds";
    
    ogc::alert::HttpResponse response = adapter.GetThreshold(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, PushControllerAdapter_GetPushStatus) {
    PushControllerAdapter adapter;
    
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/alerts/push/status";
    
    ogc::alert::HttpResponse response = adapter.GetPushStatus(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.content_type, "application/json");
}

TEST_F(ApiIntegrationTest, RestAdapter_RegisterCustomRoute) {
    RestAdapter adapter;
    
    adapter.RegisterRoute("GET", "/api/v1/custom", 
        [](const ogc::alert::HttpRequest& req) {
            return ogc::alert::HttpResponse::Ok(R"({"custom":true})");
        });
    
    ogc::alert::HttpRequest request;
    request.method = "GET";
    request.path = "/api/v1/custom";
    
    ogc::alert::HttpResponse response = adapter.HandleRequest(request);
    
    EXPECT_EQ(response.status_code, 200);
    EXPECT_NE(response.body.find("custom"), std::string::npos);
}

TEST_F(ApiIntegrationTest, RestAdapter_GetRoutes) {
    RestAdapter adapter;
    adapter.RegisterDefaultRoutes();
    
    std::vector<ogc::alert::Route> routes = adapter.GetRoutes();
    
    EXPECT_FALSE(routes.empty());
}
