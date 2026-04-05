#include <gtest/gtest.h>
#include "alert/alert_push_service.h"
#include "alert/alert.h"

class AlertPushServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = new alert::AlertPushService();
    }
    
    void TearDown() override {
        delete service;
    }
    
    alert::AlertPushService* service;
};

TEST_F(AlertPushServiceTest, DefaultConfig) {
    alert::PushConfig config = service->GetConfig();
    EXPECT_TRUE(config.enableWebSocket);
    EXPECT_FALSE(config.enableSms);
    EXPECT_FALSE(config.enableEmail);
    EXPECT_FALSE(config.enableHttp);
}

TEST_F(AlertPushServiceTest, SetConfig) {
    alert::PushConfig config;
    config.enableWebSocket = true;
    config.enableSms = true;
    config.maxRetries = 5;
    
    service->SetConfig(config);
    
    alert::PushConfig retrieved = service->GetConfig();
    EXPECT_TRUE(retrieved.enableWebSocket);
    EXPECT_TRUE(retrieved.enableSms);
    EXPECT_EQ(retrieved.maxRetries, 5);
}

TEST_F(AlertPushServiceTest, EnableDisableChannel) {
    service->EnableChannel(alert::PushChannel::kSms, true);
    EXPECT_TRUE(service->IsChannelEnabled(alert::PushChannel::kSms));
    
    service->EnableChannel(alert::PushChannel::kSms, false);
    EXPECT_FALSE(service->IsChannelEnabled(alert::PushChannel::kSms));
}

TEST_F(AlertPushServiceTest, PushToDisabledChannel) {
    service->EnableChannel(alert::PushChannel::kSms, false);
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    alert::PushResult result = service->Push(alert, alert::PushChannel::kSms);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.channel, alert::PushChannel::kSms);
}

TEST_F(AlertPushServiceTest, PushViaWebSocket) {
    service->EnableChannel(alert::PushChannel::kWebSocket, true);
    
    int callCount = 0;
    service->SetWebSocketSender([&callCount](const std::string& json) {
        callCount++;
        return true;
    });
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    alert::PushResult result = service->Push(alert, alert::PushChannel::kWebSocket);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(callCount, 1);
}

TEST_F(AlertPushServiceTest, PushViaWebSocketWithFailure) {
    service->EnableChannel(alert::PushChannel::kWebSocket, true);
    
    service->SetWebSocketSender([](const std::string& json) {
        return false;
    });
    
    alert::PushConfig config;
    config.maxRetries = 2;
    service->SetConfig(config);
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    alert::PushResult result = service->Push(alert, alert::PushChannel::kWebSocket);
    EXPECT_FALSE(result.success);
}

TEST_F(AlertPushServiceTest, PushToAllChannels) {
    alert::PushConfig config;
    config.enableWebSocket = true;
    config.enableSms = true;
    config.enableEmail = false;
    config.enableHttp = false;
    service->SetConfig(config);
    
    service->SetWebSocketSender([](const std::string&) { return true; });
    service->SetSmsSender([](const std::string&, const std::string&) { return true; });
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    std::vector<alert::PushResult> results = service->PushToAllChannels(alert);
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(AlertPushServiceTest, PushToSpecificChannels) {
    service->EnableChannel(alert::PushChannel::kWebSocket, true);
    service->EnableChannel(alert::PushChannel::kHttp, true);
    
    int wsCount = 0;
    int httpCount = 0;
    
    service->SetWebSocketSender([&wsCount](const std::string&) {
        wsCount++;
        return true;
    });
    service->SetHttpSender([&httpCount](const std::string&) {
        httpCount++;
        return true;
    });
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    std::vector<alert::PushChannel> channels = {
        alert::PushChannel::kWebSocket,
        alert::PushChannel::kHttp
    };
    
    std::vector<alert::PushResult> results = service->PushToChannels(alert, channels);
    EXPECT_EQ(results.size(), 2u);
    EXPECT_EQ(wsCount, 1);
    EXPECT_EQ(httpCount, 1);
}

TEST_F(AlertPushServiceTest, PushCallback) {
    service->EnableChannel(alert::PushChannel::kWebSocket, true);
    service->SetWebSocketSender([](const std::string&) { return true; });
    
    alert::Alert receivedAlert("", alert::AlertType::kDepthAlert, alert::AlertLevel::kNone);
    alert::PushResult receivedResult;
    
    service->SetPushCallback([&receivedAlert, &receivedResult](const alert::Alert& alert, const alert::PushResult& result) {
        receivedAlert = alert;
        receivedResult = result;
    });
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    service->Push(alert, alert::PushChannel::kWebSocket);
    
    EXPECT_EQ(receivedAlert.GetAlertId(), "TEST001");
    EXPECT_TRUE(receivedResult.success);
}

TEST_F(AlertPushServiceTest, AlertToJson) {
    service->EnableChannel(alert::PushChannel::kWebSocket, true);
    
    std::string capturedJson;
    service->SetWebSocketSender([&capturedJson](const std::string& json) {
        capturedJson = json;
        return true;
    });
    
    alert::Alert alert("TEST001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    alert.SetTitle("Test Alert");
    alert.SetMessage("Test message");
    
    service->Push(alert, alert::PushChannel::kWebSocket);
    
    EXPECT_TRUE(capturedJson.find("TEST001") != std::string::npos);
    EXPECT_TRUE(capturedJson.find("depth") != std::string::npos);
    EXPECT_TRUE(capturedJson.find("severe") != std::string::npos);
}
