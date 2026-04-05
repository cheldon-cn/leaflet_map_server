#include <gtest/gtest.h>
#include "alert/alert_config_service.h"
#include "alert/alert_exception.h"
#include <fstream>

class AlertConfigServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = new alert::AlertConfigService();
    }
    
    void TearDown() override {
        delete service;
    }
    
    alert::AlertConfigService* service;
};

TEST_F(AlertConfigServiceTest, GetDefaultThreshold) {
    alert::ThresholdConfig config = service->GetThreshold(alert::AlertType::kDepthAlert);
    EXPECT_TRUE(config.IsValid());
}

TEST_F(AlertConfigServiceTest, SetThreshold) {
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.5);
    config.SetLevel2Threshold(1.0);
    config.SetLevel3Threshold(1.5);
    config.SetLevel4Threshold(2.0);
    
    service->SetThreshold(alert::AlertType::kDepthAlert, config);
    
    alert::ThresholdConfig retrieved = service->GetThreshold(alert::AlertType::kDepthAlert);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.5);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel2Threshold(), 1.0);
}

TEST_F(AlertConfigServiceTest, SetAndGetRule) {
    alert::AlertRule rule;
    rule.ruleId = "RULE001";
    rule.name = "Test Rule";
    rule.alertType = alert::AlertType::kDepthAlert;
    rule.enabled = true;
    rule.priority = 1;
    
    service->SetRule("RULE001", rule);
    
    alert::AlertRule retrieved = service->GetRule("RULE001");
    EXPECT_EQ(retrieved.ruleId, "RULE001");
    EXPECT_EQ(retrieved.name, "Test Rule");
    EXPECT_EQ(retrieved.alertType, alert::AlertType::kDepthAlert);
}

TEST_F(AlertConfigServiceTest, GetRulesByType) {
    alert::AlertRule rule1;
    rule1.ruleId = "RULE001";
    rule1.alertType = alert::AlertType::kDepthAlert;
    
    alert::AlertRule rule2;
    rule2.ruleId = "RULE002";
    rule2.alertType = alert::AlertType::kDepthAlert;
    
    alert::AlertRule rule3;
    rule3.ruleId = "RULE003";
    rule3.alertType = alert::AlertType::kCollisionAlert;
    
    service->SetRule("RULE001", rule1);
    service->SetRule("RULE002", rule2);
    service->SetRule("RULE003", rule3);
    
    std::vector<alert::AlertRule> depthRules = service->GetRulesByType(alert::AlertType::kDepthAlert);
    EXPECT_EQ(depthRules.size(), 2u);
    
    std::vector<alert::AlertRule> collisionRules = service->GetRulesByType(alert::AlertType::kCollisionAlert);
    EXPECT_EQ(collisionRules.size(), 1u);
}

TEST_F(AlertConfigServiceTest, DeleteRule) {
    alert::AlertRule rule;
    rule.ruleId = "RULE001";
    
    service->SetRule("RULE001", rule);
    service->DeleteRule("RULE001");
    
    alert::AlertRule retrieved = service->GetRule("RULE001");
    EXPECT_TRUE(retrieved.ruleId.empty());
}

TEST_F(AlertConfigServiceTest, NotificationConfig) {
    alert::NotificationConfig config;
    config.enableSound = false;
    config.enablePopup = true;
    config.enableEmail = true;
    config.emailRecipients.push_back("test@example.com");
    
    service->SetNotificationConfig(config);
    
    alert::NotificationConfig retrieved = service->GetNotificationConfig();
    EXPECT_FALSE(retrieved.enableSound);
    EXPECT_TRUE(retrieved.enablePopup);
    EXPECT_TRUE(retrieved.enableEmail);
    EXPECT_EQ(retrieved.emailRecipients.size(), 1u);
}

TEST_F(AlertConfigServiceTest, AlertTypeEnabled) {
    EXPECT_TRUE(service->IsAlertTypeEnabled(alert::AlertType::kDepthAlert));
    
    service->SetAlertTypeEnabled(alert::AlertType::kDepthAlert, false);
    EXPECT_FALSE(service->IsAlertTypeEnabled(alert::AlertType::kDepthAlert));
    
    service->SetAlertTypeEnabled(alert::AlertType::kDepthAlert, true);
    EXPECT_TRUE(service->IsAlertTypeEnabled(alert::AlertType::kDepthAlert));
}

TEST_F(AlertConfigServiceTest, GetEnabledAlertTypes) {
    std::vector<alert::AlertType> enabled = service->GetEnabledAlertTypes();
    EXPECT_GE(enabled.size(), 3u);
}

TEST_F(AlertConfigServiceTest, ConfigChangeCallback) {
    std::string lastChange;
    service->SetConfigChangeCallback([&lastChange](const std::string& change) {
        lastChange = change;
    });
    
    service->SetAlertTypeEnabled(alert::AlertType::kDepthAlert, false);
    EXPECT_FALSE(lastChange.empty());
}

TEST_F(AlertConfigServiceTest, ResetToDefaults) {
    service->SetAlertTypeEnabled(alert::AlertType::kDepthAlert, false);
    
    service->ResetToDefaults();
    
    EXPECT_TRUE(service->IsAlertTypeEnabled(alert::AlertType::kDepthAlert));
}

TEST_F(AlertConfigServiceTest, SaveAndLoadFile) {
    std::string testFile = "test_config.ini";
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.8);
    service->SetThreshold(alert::AlertType::kDepthAlert, config);
    
    service->SaveToFile(testFile);
    
    service->SetThreshold(alert::AlertType::kDepthAlert, alert::ThresholdConfig());
    
    service->LoadFromFile(testFile);
    
    std::remove(testFile.c_str());
}
