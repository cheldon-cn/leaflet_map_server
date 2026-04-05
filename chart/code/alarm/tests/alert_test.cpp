#include <gtest/gtest.h>
#include "alert/alert.h"

class AlertTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlertTest, DefaultConstructor) {
    alert::Alert alert;
    EXPECT_TRUE(alert.GetAlertId().empty());
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kDepthAlert);
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kNone);
    EXPECT_EQ(alert.GetStatus(), alert::AlertStatus::kPending);
    EXPECT_FALSE(alert.IsAcknowledged());
}

TEST_F(AlertTest, ThreeParamConstructor) {
    alert::Alert alert("ALERT001", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(alert.GetAlertId(), "ALERT001");
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kCollisionAlert);
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(alert.GetStatus(), alert::AlertStatus::kPending);
}

TEST_F(AlertTest, SettersAndGetters) {
    alert::Alert alert;
    
    alert.SetAlertId("ALERT002");
    EXPECT_EQ(alert.GetAlertId(), "ALERT002");
    
    alert.SetAlertType(alert::AlertType::kWeatherAlert);
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kWeatherAlert);
    
    alert.SetAlertLevel(alert::AlertLevel::kLevel1_Critical);
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kLevel1_Critical);
    
    alert.SetStatus(alert::AlertStatus::kActive);
    EXPECT_EQ(alert.GetStatus(), alert::AlertStatus::kActive);
    
    alert.SetTitle("Test Alert");
    EXPECT_EQ(alert.GetTitle(), "Test Alert");
    
    alert.SetMessage("Test message");
    EXPECT_EQ(alert.GetMessage(), "Test message");
    
    alert.SetRecommendation("Test recommendation");
    EXPECT_EQ(alert.GetRecommendation(), "Test recommendation");
    
    alert.SetPosition(120.5, 31.2);
    EXPECT_DOUBLE_EQ(alert.GetPosition().longitude, 120.5);
    EXPECT_DOUBLE_EQ(alert.GetPosition().latitude, 31.2);
    
    alert.SetIssueTime("2026-01-01 10:00:00");
    EXPECT_EQ(alert.GetIssueTime(), "2026-01-01 10:00:00");
    
    alert.SetExpireTime("2026-01-01 12:00:00");
    EXPECT_EQ(alert.GetExpireTime(), "2026-01-01 12:00:00");
    
    alert.SetClearTime("2026-01-01 11:00:00");
    EXPECT_EQ(alert.GetClearTime(), "2026-01-01 11:00:00");
    
    alert.SetClearReason("Resolved");
    EXPECT_EQ(alert.GetClearReason(), "Resolved");
    
    alert.SetAcknowledged(true);
    EXPECT_TRUE(alert.IsAcknowledged());
    
    alert.SetAcknowledgeTime("2026-01-01 10:30:00");
    EXPECT_EQ(alert.GetAcknowledgeTime(), "2026-01-01 10:30:00");
    
    alert.SetActionTaken("Changed course");
    EXPECT_EQ(alert.GetActionTaken(), "Changed course");
}

TEST_F(AlertTest, Details) {
    alert::Alert alert;
    
    alert.SetDetail("depth", 10.5);
    EXPECT_DOUBLE_EQ(alert.GetDetail("depth"), 10.5);
    
    EXPECT_DOUBLE_EQ(alert.GetDetail("nonexistent", 0.0), 0.0);
    
    alert.SetDetail("speed", 15.0);
    const auto& details = alert.GetDetails();
    EXPECT_EQ(details.size(), 2);
}

TEST_F(AlertTest, PushRecords) {
    alert::Alert alert;
    
    alert::PushRecord record1;
    record1.method = alert::PushMethod::kApp;
    record1.pushTime = "2026-01-01 10:00:00";
    record1.pushStatus = "success";
    
    alert.AddPushRecord(record1);
    
    const auto& records = alert.GetPushRecords();
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records[0].method, alert::PushMethod::kApp);
    
    alert::PushRecord record2;
    record2.method = alert::PushMethod::kSms;
    alert.AddPushRecord(record2);
    
    EXPECT_EQ(alert.GetPushRecords().size(), 2);
}

TEST_F(AlertTest, IsActive) {
    alert::Alert alert;
    
    alert.SetStatus(alert::AlertStatus::kPending);
    EXPECT_FALSE(alert.IsActive());
    
    alert.SetStatus(alert::AlertStatus::kActive);
    EXPECT_TRUE(alert.IsActive());
    
    alert.SetStatus(alert::AlertStatus::kPushed);
    EXPECT_TRUE(alert.IsActive());
    
    alert.SetStatus(alert::AlertStatus::kAcknowledged);
    EXPECT_FALSE(alert.IsActive());
    
    alert.SetStatus(alert::AlertStatus::kCleared);
    EXPECT_FALSE(alert.IsActive());
}

TEST_F(AlertTest, IsExpired) {
    alert::Alert alert;
    
    alert.SetStatus(alert::AlertStatus::kActive);
    EXPECT_FALSE(alert.IsExpired());
    
    alert.SetStatus(alert::AlertStatus::kExpired);
    EXPECT_TRUE(alert.IsExpired());
    
    alert.SetStatus(alert::AlertStatus::kCleared);
    EXPECT_TRUE(alert.IsExpired());
}

TEST_F(AlertTest, ToString) {
    alert::Alert alert("ALERT001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    std::string str = alert.ToString();
    
    EXPECT_TRUE(str.find("ALERT001") != std::string::npos);
    EXPECT_TRUE(str.find("depth") != std::string::npos);
    EXPECT_TRUE(str.find("critical") != std::string::npos);
}

TEST_F(AlertTest, GenerateAlertId) {
    std::string id1 = alert::Alert::GenerateAlertId(alert::AlertType::kDepthAlert);
    std::string id2 = alert::Alert::GenerateAlertId(alert::AlertType::kDepthAlert);
    
    EXPECT_TRUE(id1.find("ALERT_") == 0);
    EXPECT_NE(id1, id2);
}
