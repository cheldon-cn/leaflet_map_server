#include <gtest/gtest.h>
#include "alert/alert_repository.h"
#include "alert/alert.h"

class AlertRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        repository = new alert::AlertRepository();
    }
    
    void TearDown() override {
        delete repository;
    }
    
    alert::AlertRepository* repository;
    
    alert::Alert CreateTestAlert(const std::string& id, alert::AlertType type, alert::AlertLevel level) {
        alert::Alert alert(id, type, level);
        alert.SetTitle("Test Alert");
        alert.SetMessage("Test message");
        alert.SetPosition(120.5, 31.2);
        return alert;
    }
};

TEST_F(AlertRepositoryTest, CreateAndGetById) {
    alert::Alert alert = CreateTestAlert("REPO001", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    
    EXPECT_TRUE(repository->Create(alert));
    
    alert::Alert retrieved = repository->GetById("REPO001");
    EXPECT_EQ(retrieved.GetAlertId(), "REPO001");
    EXPECT_EQ(retrieved.GetAlertType(), alert::AlertType::kDepthAlert);
}

TEST_F(AlertRepositoryTest, Update) {
    alert::Alert alert = CreateTestAlert("REPO002", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel1_Critical);
    repository->Create(alert);
    
    alert.SetAlertLevel(alert::AlertLevel::kLevel2_Severe);
    EXPECT_TRUE(repository->Update(alert));
    
    alert::Alert updated = repository->GetById("REPO002");
    EXPECT_EQ(updated.GetAlertLevel(), alert::AlertLevel::kLevel2_Severe);
}

TEST_F(AlertRepositoryTest, Remove) {
    alert::Alert alert = CreateTestAlert("REPO003", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    repository->Create(alert);
    
    EXPECT_TRUE(repository->Remove("REPO003"));
    
    alert::Alert removed = repository->GetById("REPO003");
    EXPECT_TRUE(removed.GetAlertId().empty());
}

TEST_F(AlertRepositoryTest, FindAll) {
    alert::Alert alert1 = CreateTestAlert("REPO004", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert::Alert alert2 = CreateTestAlert("REPO005", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    
    repository->Create(alert1);
    repository->Create(alert2);
    
    std::vector<alert::Alert> alerts = repository->FindAll();
    EXPECT_GE(alerts.size(), 2);
}

TEST_F(AlertRepositoryTest, FindByType) {
    alert::Alert alert1 = CreateTestAlert("REPO006", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert::Alert alert2 = CreateTestAlert("REPO007", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert::Alert alert3 = CreateTestAlert("REPO008", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel3_Moderate);
    
    repository->Create(alert1);
    repository->Create(alert2);
    repository->Create(alert3);
    
    std::vector<alert::Alert> alerts = repository->FindByType(alert::AlertType::kDepthAlert);
    EXPECT_EQ(alerts.size(), 2);
}

TEST_F(AlertRepositoryTest, FindByStatus) {
    alert::Alert alert1 = CreateTestAlert("REPO009", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("REPO010", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    
    repository->Create(alert1);
    repository->Create(alert2);
    
    std::vector<alert::Alert> activeAlerts = repository->FindByStatus(alert::AlertStatus::kActive);
    EXPECT_EQ(activeAlerts.size(), 1);
}

TEST_F(AlertRepositoryTest, FindActive) {
    alert::Alert alert1 = CreateTestAlert("REPO011", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("REPO012", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    alert::Alert alert3 = CreateTestAlert("REPO013", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    alert3.SetStatus(alert::AlertStatus::kActive);
    
    repository->Create(alert1);
    repository->Create(alert2);
    repository->Create(alert3);
    
    std::vector<alert::Alert> activeAlerts = repository->FindActive();
    EXPECT_EQ(activeAlerts.size(), 2);
}

TEST_F(AlertRepositoryTest, Acknowledge) {
    alert::Alert alert = CreateTestAlert("REPO014", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert.SetStatus(alert::AlertStatus::kActive);
    repository->Create(alert);
    
    EXPECT_TRUE(repository->Acknowledge("REPO014", "USER001"));
    
    alert::Alert acknowledged = repository->GetById("REPO014");
    EXPECT_EQ(acknowledged.GetStatus(), alert::AlertStatus::kAcknowledged);
}

TEST_F(AlertRepositoryTest, Expire) {
    alert::Alert alert = CreateTestAlert("REPO015", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert.SetStatus(alert::AlertStatus::kActive);
    repository->Create(alert);
    
    EXPECT_TRUE(repository->Expire("REPO015"));
    
    alert::Alert expired = repository->GetById("REPO015");
    EXPECT_EQ(expired.GetStatus(), alert::AlertStatus::kExpired);
}

TEST_F(AlertRepositoryTest, Dismiss) {
    alert::Alert alert = CreateTestAlert("REPO016", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert.SetStatus(alert::AlertStatus::kActive);
    repository->Create(alert);
    
    EXPECT_TRUE(repository->Dismiss("REPO016", "False alarm"));
    
    alert::Alert dismissed = repository->GetById("REPO016");
    EXPECT_EQ(dismissed.GetStatus(), alert::AlertStatus::kCleared);
}

TEST_F(AlertRepositoryTest, GetStatistics) {
    alert::Alert alert1 = CreateTestAlert("REPO017", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("REPO018", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    alert::Alert alert3 = CreateTestAlert("REPO019", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    alert3.SetStatus(alert::AlertStatus::kExpired);
    
    repository->Create(alert1);
    repository->Create(alert2);
    repository->Create(alert3);
    
    alert::AlertStatistics stats = repository->GetStatistics("", "");
    EXPECT_EQ(stats.totalCount, 3);
    EXPECT_EQ(stats.activeCount, 1);
    EXPECT_EQ(stats.acknowledgedCount, 1);
    EXPECT_EQ(stats.expiredCount, 1);
}

TEST_F(AlertRepositoryTest, GetActiveCount) {
    alert::Alert alert1 = CreateTestAlert("REPO020", alert::AlertType::kDepthAlert, alert::AlertLevel::kLevel1_Critical);
    alert1.SetStatus(alert::AlertStatus::kActive);
    alert::Alert alert2 = CreateTestAlert("REPO021", alert::AlertType::kCollisionAlert, alert::AlertLevel::kLevel2_Severe);
    alert2.SetStatus(alert::AlertStatus::kAcknowledged);
    alert::Alert alert3 = CreateTestAlert("REPO022", alert::AlertType::kWeatherAlert, alert::AlertLevel::kLevel3_Moderate);
    alert3.SetStatus(alert::AlertStatus::kActive);
    
    repository->Create(alert1);
    repository->Create(alert2);
    repository->Create(alert3);
    
    int count = repository->GetActiveCount();
    EXPECT_EQ(count, 2);
}
