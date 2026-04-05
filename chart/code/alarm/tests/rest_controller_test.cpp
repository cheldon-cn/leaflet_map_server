#include <gtest/gtest.h>
#include "alert/rest_controller.h"
#include <memory>

class RestControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        controller = new alert::RestController();
        controller->Start(8080);
    }
    
    void TearDown() override {
        controller->Stop();
        delete controller;
    }
    
    alert::RestController* controller;
    
    alert::HttpRequest CreateRequest(const std::string& method, 
                                       const std::string& path,
                                       const std::string& body = "",
                                       const std::string& auth = "Bearer user_001") {
        alert::HttpRequest req;
        req.method = method;
        req.path = path;
        req.body = body;
        req.authorization = auth;
        req.valid = true;
        return req;
    }
};

TEST_F(RestControllerTest, StartStop) {
    EXPECT_TRUE(controller->IsRunning());
    controller->Stop();
    EXPECT_FALSE(controller->IsRunning());
    controller->Start(8080);
    EXPECT_TRUE(controller->IsRunning());
}

TEST_F(RestControllerTest, HandleSubscribeSuccess) {
    alert::HttpRequest request = CreateRequest("POST", "/api/v1/alert/subscribe",
        "{\"user_id\":\"user_001\",\"ship_id\":\"ship_001\",\"alert_types\":[\"depth\",\"collision\"]}");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_FALSE(response.body.empty());
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
    EXPECT_TRUE(response.body.find("subscribe_id") != std::string::npos);
}

TEST_F(RestControllerTest, HandleSubscribeUnauthorized) {
    alert::HttpRequest request = CreateRequest("POST", "/api/v1/alert/subscribe",
        "{\"user_id\":\"user_001\"}", "");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 401);
}

TEST_F(RestControllerTest, HandleAlertListSuccess) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/list?page=1&page_size=10");
    request.queryParams["page"] = "1";
    request.queryParams["page_size"] = "10";
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
    EXPECT_TRUE(response.body.find("alerts") != std::string::npos);
}

TEST_F(RestControllerTest, HandleAlertListUnauthorized) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/list", "", "");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 401);
}

TEST_F(RestControllerTest, HandleAlertDetailSuccess) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/detail/ALERT_20240520_001");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
    EXPECT_TRUE(response.body.find("ALERT_20240520_001") != std::string::npos);
}

TEST_F(RestControllerTest, HandleAlertDetailUnauthorized) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/detail/ALERT_001", "", "");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 401);
}

TEST_F(RestControllerTest, HandleAcknowledgeSuccess) {
    alert::HttpRequest request = CreateRequest("POST", "/api/v1/alert/acknowledge/ALERT_20240520_001",
        "{\"user_id\":\"user_001\",\"action_taken\":\"Reduced speed to 5 knots\"}");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
    EXPECT_TRUE(response.body.find("acknowledged") != std::string::npos);
}

TEST_F(RestControllerTest, HandleAcknowledgeUnauthorized) {
    alert::HttpRequest request = CreateRequest("POST", "/api/v1/alert/acknowledge/ALERT_001",
        "{}", "");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 401);
}

TEST_F(RestControllerTest, HandleConfigGetSuccess) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/config");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
    EXPECT_TRUE(response.body.find("depth_alert") != std::string::npos);
    EXPECT_TRUE(response.body.find("collision_alert") != std::string::npos);
}

TEST_F(RestControllerTest, HandleConfigSetSuccess) {
    alert::HttpRequest request = CreateRequest("POST", "/api/v1/alert/config",
        "{\"depth_alert\":{\"enabled\":true,\"ukc_threshold_critical\":0.3}}");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("\"code\":0") != std::string::npos);
}

TEST_F(RestControllerTest, HandleNotFound) {
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/unknown/endpoint");
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 404);
}

TEST_F(RestControllerTest, HandleInvalidRequest) {
    alert::HttpRequest request;
    request.valid = false;
    
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 400);
}

TEST_F(RestControllerTest, ValidateTokenSuccess) {
    std::string userId;
    bool result = controller->ValidateToken("Bearer user_001", userId);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(userId, "user_001");
}

TEST_F(RestControllerTest, ValidateTokenFailure) {
    std::string userId;
    bool result = controller->ValidateToken("InvalidToken", userId);
    
    EXPECT_FALSE(result);
}

TEST_F(RestControllerTest, SetEnabled) {
    controller->SetEnabled(false);
    EXPECT_FALSE(controller->IsEnabled());
    
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/alert/list");
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_EQ(response.statusCode, 503);
    
    controller->SetEnabled(true);
    EXPECT_TRUE(controller->IsEnabled());
}

TEST_F(RestControllerTest, RegisterCustomHandler) {
    controller->RegisterHandler("/api/v1/custom", "GET",
        [](const alert::HttpRequest& req) {
            alert::HttpResponse resp;
            resp.statusCode = 200;
            resp.body = "{\"custom\":true}";
            resp.valid = true;
            return resp;
        });
    
    alert::HttpRequest request = CreateRequest("GET", "/api/v1/custom");
    alert::HttpResponse response = controller->HandleRequest(request);
    
    EXPECT_TRUE(response.valid);
    EXPECT_EQ(response.statusCode, 200);
    EXPECT_TRUE(response.body.find("custom") != std::string::npos);
}

TEST_F(RestControllerTest, UnregisterHandler) {
    controller->RegisterHandler("/api/v1/temp", "GET",
        [](const alert::HttpRequest& req) {
            alert::HttpResponse resp;
            resp.statusCode = 200;
            resp.valid = true;
            return resp;
        });
    
    alert::HttpRequest request1 = CreateRequest("GET", "/api/v1/temp");
    alert::HttpResponse response1 = controller->HandleRequest(request1);
    EXPECT_EQ(response1.statusCode, 200);
    
    controller->UnregisterHandler("/api/v1/temp", "GET");
    
    alert::HttpRequest request2 = CreateRequest("GET", "/api/v1/temp");
    alert::HttpResponse response2 = controller->HandleRequest(request2);
    EXPECT_EQ(response2.statusCode, 404);
}

TEST_F(RestControllerTest, SetJwtSecret) {
    controller->SetJwtSecret("test_secret_key");
    
    EXPECT_TRUE(controller->IsRunning());
}
