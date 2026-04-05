#include <gtest/gtest.h>
#include "alert/alert_data_access.h"
#include "alert/alert.h"

class AlertDataAccessTest : public ::testing::Test {
protected:
    void SetUp() override {
        dataAccess = new alert::AlertDataAccess();
    }
    
    void TearDown() override {
        delete dataAccess;
    }
    
    alert::AlertDataAccess* dataAccess;
    
    alert::Alert CreateTestAlert(const std::string& id, alert::AlertType type, alert::AlertLevel level) {
        alert::Alert alert(id, type, level);
        alert.SetTitle("Test Alert");
        alert.SetMessage("Test message");
        alert.SetPosition(120.5, 31.2);
        return alert;
    }
};

TEST_F(AlertDataAccessTest, SaveAndGetAlert) {
    alert::Alert alert = CreateTestAlert("ALERT001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel2_Severe);
    
    EXPECT_TRUE(dataAccess->SaveAlert(alert));
    
    alert::Alert retrieved = dataAccess->GetAlertById("ALERT001");
    EXPECT_EQ(retrieved.GetAlertId(), "ALERT001");
    EXPECT_EQ(retrieved.GetAlertType(), alert::AlertType::kDepthAlert);
    EXPECT_EQ(retrieved.GetAlertLevel(), alert::AlertLevel::kLevel2_Severe);
}

TEST_F(AlertDataAccessTest, UpdateAlert) {
    alert::Alert alert = CreateTestAlert("ALERT002", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel1_Critical);
    EXPECT_TRUE(dataAccess->SaveAlert(alert));
    
    alert.SetAlertLevel(alert::AlertLevel::kLevel2_Severe);
    EXPECT_TRUE(dataAccess->UpdateAlert(alert));
    
    alert::Alert updated = dataAccess->GetAlertById("ALERT002");
    EXPECT_EQ(updated.GetAlertLevel(), alert::AlertLevel::kLevel2_Severe);
}

TEST_F(AlertDataAccessTest, DeleteAlert) {
    alert::Alert alert = CreateTestAlert("ALERT003", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    EXPECT_TRUE(dataAccess->SaveAlert(alert));
    
    EXPECT_TRUE(dataAccess->DeleteAlert("ALERT003"));
    
    alert::Alert deleted = dataAccess->GetAlertById("ALERT003");
    EXPECT_TRUE(deleted.GetAlertId().empty());
}

TEST_F(AlertDataAccessTest, QueryAlerts) {
    alert::Alert alert1 = CreateTestAlert("ALERT004", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert::Alert alert2 = CreateTestAlert("ALERT005", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert::Alert alert3 = CreateTestAlert("ALERT006", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel3_Moderate);
    
    dataAccess->SaveAlert(alert1);
    dataAccess->SaveAlert(alert2);
    dataAccess->SaveAlert(alert3);
    
    alert::AlertQueryCriteria criteria;
    criteria.alertType = alert::AlertType::kDepthAlert;
    std::vector<alert::Alert> results = dataAccess->QueryAlerts(criteria);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(AlertDataAccessTest, SaveAndGetPushRecord) {
    alert::DataAccessPushRecord record;
    record.pushId = "PUSH001";
    record.alertId = "ALERT007";
    record.userId = "USER001";
    record.pushMethod = "websocket";
    record.pushStatus = "success";
    
    EXPECT_TRUE(dataAccess->SavePushRecord(record));
    
    std::vector<alert::DataAccessPushRecord> records = dataAccess->GetPushRecords("ALERT007");
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records[0].pushId, "PUSH001");
}

TEST_F(AlertDataAccessTest, SaveAndGetHistoryRecord) {
    alert::AlertHistoryRecord record;
    record.historyId = "HIST001";
    record.alertId = "ALERT008";
    record.action = "acknowledge";
    record.operatorId = "USER001";
    
    EXPECT_TRUE(dataAccess->SaveHistoryRecord(record));
    
    std::vector<alert::AlertHistoryRecord> records = dataAccess->GetHistoryRecords("ALERT008");
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records[0].action, "acknowledge");
}

TEST_F(AlertDataAccessTest, GetStatistics) {
    alert::Alert alert1 = CreateTestAlert("ALERT009", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("ALERT010", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    alert::Alert alert3 = CreateTestAlert("ALERT011", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel3_Moderate);
    alert3.SetStatus(alert::AlertStatus::kExpired);
    
    dataAccess->SaveAlert(alert1);
    dataAccess->SaveAlert(alert2);
    dataAccess->SaveAlert(alert3);
    
    alert::AlertStatistics stats = dataAccess->GetStatistics("", "");
    EXPECT_EQ(stats.totalCount, 3);
    EXPECT_EQ(stats.activeCount, 1);
    EXPECT_EQ(stats.acknowledgedCount, 1);
    EXPECT_EQ(stats.expiredCount, 1);
}

TEST_F(AlertDataAccessTest, AcknowledgeAlert) {
    alert::Alert alert = CreateTestAlert("ALERT012", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert.SetStatus(alert::AlertStatus::kActive);
    dataAccess->SaveAlert(alert);
    
    EXPECT_TRUE(dataAccess->AcknowledgeAlert("ALERT012", "USER001"));
    
    alert::Alert acknowledged = dataAccess->GetAlertById("ALERT012");
    EXPECT_EQ(acknowledged.GetStatus(), alert::AlertStatus::kAcknowledged);
    
    std::vector<alert::AlertHistoryRecord> history = dataAccess->GetHistoryRecords("ALERT012");
    EXPECT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].action, "acknowledge");
}

TEST_F(AlertDataAccessTest, ExpireAlert) {
    alert::Alert alert = CreateTestAlert("ALERT013", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert.SetStatus(alert::AlertStatus::kActive);
    dataAccess->SaveAlert(alert);
    
    EXPECT_TRUE(dataAccess->ExpireAlert("ALERT013"));
    
    alert::Alert expired = dataAccess->GetAlertById("ALERT013");
    EXPECT_EQ(expired.GetStatus(), alert::AlertStatus::kExpired);
}

TEST_F(AlertDataAccessTest, GetActiveAlerts) {
    alert::Alert alert1 = CreateTestAlert("ALERT014", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("ALERT015", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    alert::Alert alert3 = CreateTestAlert("ALERT016", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    alert3.SetStatus(alert::AlertStatus::kActive);
    
    dataAccess->SaveAlert(alert1);
    dataAccess->SaveAlert(alert2);
    dataAccess->SaveAlert(alert3);
    
    std::vector<alert::Alert> activeAlerts = dataAccess->GetActiveAlerts();
    EXPECT_EQ(activeAlerts.size(), 2);
    
    int count = dataAccess->GetActiveAlertCount();
    EXPECT_EQ(count, 2);
}

TEST_F(AlertDataAccessTest, IsConnected) {
    EXPECT_TRUE(dataAccess->IsConnected());
    
    dataAccess->SetConnectionCallback([]() { return false; });
    EXPECT_FALSE(dataAccess->IsConnected());
}

TEST_F(AlertDataAccessTest, StoragePath) {
    dataAccess->SetStoragePath("/tmp/alerts");
    EXPECT_EQ(dataAccess->GetStoragePath(), "/tmp/alerts");
}
