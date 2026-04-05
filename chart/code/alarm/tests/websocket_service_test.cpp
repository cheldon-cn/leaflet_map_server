#include <gtest/gtest.h>
#include "alert/websocket_service.h"
#include <memory>

class WebSocketServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = new alert::WebSocketService();
        
        alert::WebSocketConfig config;
        config.port = 9002;
        config.maxConnections = 100;
        config.pingInterval = 30;
        
        service->Start(config);
    }
    
    void TearDown() override {
        service->Stop();
        delete service;
    }
    
    alert::WebSocketService* service;
};

TEST_F(WebSocketServiceTest, StartStop) {
    EXPECT_TRUE(service->IsRunning());
    service->Stop();
    EXPECT_FALSE(service->IsRunning());
    
    alert::WebSocketConfig config;
    config.port = 9003;
    service->Start(config);
    EXPECT_TRUE(service->IsRunning());
}

TEST_F(WebSocketServiceTest, SetEnabled) {
    EXPECT_TRUE(service->IsEnabled());
    
    service->SetEnabled(false);
    EXPECT_FALSE(service->IsEnabled());
    
    service->SetEnabled(true);
    EXPECT_TRUE(service->IsEnabled());
}

TEST_F(WebSocketServiceTest, ValidateTokenSuccess) {
    std::string userId;
    bool result = service->ValidateToken("Bearer user_001", userId);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(userId, "user_001");
}

TEST_F(WebSocketServiceTest, ValidateTokenFailure) {
    std::string userId;
    bool result = service->ValidateToken("InvalidToken", userId);
    
    EXPECT_FALSE(result);
}

TEST_F(WebSocketServiceTest, ValidateTokenEmpty) {
    std::string userId;
    bool result = service->ValidateToken("", userId);
    
    EXPECT_FALSE(result);
}

TEST_F(WebSocketServiceTest, SetJwtSecret) {
    service->SetJwtSecret("test_secret_key");
    
    EXPECT_TRUE(service->IsRunning());
}

TEST_F(WebSocketServiceTest, CreateAlertPushMessage) {
    alert::Alert alert;
    alert.SetAlertId("ALERT_001");
    alert.SetAlertType(alert::AlertType::kDepthAlert);
    alert.SetAlertLevel(alert::AlertLevel::kLevel2_Severe);
    alert.SetTitle("Depth Warning");
    alert.SetMessage("Depth insufficient ahead");
    alert.SetRecommendation("Reduce speed");
    alert.SetPosition(113.5, 30.5);
    
    alert::WebSocketMessage message = service->CreateAlertPushMessage(alert);
    
    EXPECT_TRUE(message.valid);
    EXPECT_EQ(message.type, "alert_push");
    EXPECT_FALSE(message.data.empty());
    EXPECT_FALSE(message.timestamp.empty());
    EXPECT_TRUE(message.data.find("ALERT_001") != std::string::npos);
    EXPECT_TRUE(message.data.find("depth") != std::string::npos);
}

TEST_F(WebSocketServiceTest, CreateAlertClearMessage) {
    std::string alertId = "ALERT_001";
    std::string reason = "Risk cleared";
    
    alert::WebSocketMessage message = service->CreateAlertClearMessage(alertId, reason);
    
    EXPECT_TRUE(message.valid);
    EXPECT_EQ(message.type, "alert_clear");
    EXPECT_FALSE(message.data.empty());
    EXPECT_TRUE(message.data.find("ALERT_001") != std::string::npos);
    EXPECT_TRUE(message.data.find("Risk cleared") != std::string::npos);
}

TEST_F(WebSocketServiceTest, GetClientCount) {
    EXPECT_EQ(service->GetClientCount(), 0);
}

TEST_F(WebSocketServiceTest, GetClientNotFound) {
    alert::WebSocketClient client = service->GetClient("nonexistent_client");
    EXPECT_FALSE(client.valid);
}

TEST_F(WebSocketServiceTest, GetAllClientsEmpty) {
    std::vector<alert::WebSocketClient> clients = service->GetAllClients();
    EXPECT_EQ(clients.size(), 0);
}

TEST_F(WebSocketServiceTest, SendToClientNotFound) {
    alert::WebSocketMessage message;
    message.type = "test";
    message.data = "{}";
    message.valid = true;
    
    bool result = service->SendToClient("nonexistent_client", message);
    EXPECT_FALSE(result);
}

TEST_F(WebSocketServiceTest, SendToUserNotFound) {
    alert::WebSocketMessage message;
    message.type = "test";
    message.data = "{}";
    message.valid = true;
    
    bool result = service->SendToUser("nonexistent_user", message);
    EXPECT_FALSE(result);
}

TEST_F(WebSocketServiceTest, Broadcast) {
    alert::WebSocketMessage message;
    message.type = "test";
    message.data = "{}";
    message.valid = true;
    
    EXPECT_NO_THROW(service->Broadcast(message));
}

TEST_F(WebSocketServiceTest, BroadcastToType) {
    alert::WebSocketMessage message;
    message.type = "test";
    message.data = "{}";
    message.valid = true;
    
    EXPECT_NO_THROW(service->BroadcastToType("depth", message));
}

TEST_F(WebSocketServiceTest, CloseClientNotFound) {
    bool result = service->CloseClient("nonexistent_client");
    EXPECT_FALSE(result);
}

TEST_F(WebSocketServiceTest, CloseAllClients) {
    EXPECT_NO_THROW(service->CloseAllClients());
}

TEST_F(WebSocketServiceTest, SetMessageCallback) {
    bool callbackCalled = false;
    std::string receivedClientId;
    
    service->SetMessageCallback([&](const std::string& clientId, const alert::WebSocketMessage& msg) {
        callbackCalled = true;
        receivedClientId = clientId;
    });
    
    EXPECT_TRUE(service->IsRunning());
}

TEST_F(WebSocketServiceTest, SetConnectionCallback) {
    bool connectCalled = false;
    bool disconnectCalled = false;
    
    service->SetConnectionCallback(
        [&](const std::string& clientId, const alert::WebSocketClient& client) {
            connectCalled = true;
        },
        [&](const std::string& clientId, const alert::WebSocketClient& client) {
            disconnectCalled = true;
        }
    );
    
    EXPECT_TRUE(service->IsRunning());
}

TEST_F(WebSocketServiceTest, GetLastError) {
    alert::WebSocketMessage message;
    message.valid = true;
    
    service->SendToClient("nonexistent", message);
    EXPECT_FALSE(service->GetLastError().empty());
}
