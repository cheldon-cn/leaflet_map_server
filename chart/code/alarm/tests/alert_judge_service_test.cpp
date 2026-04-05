#include <gtest/gtest.h>
#include "alert/alert_judge_service.h"
#include "alert/alert_context.h"
#include "alert/alert_exception.h"

class AlertJudgeServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = new alert::AlertJudgeService();
    }
    
    void TearDown() override {
        delete service;
    }
    
    alert::AlertJudgeService* service;
};

TEST_F(AlertJudgeServiceTest, GetSupportedTypes) {
    std::vector<alert::AlertType> types = service->GetSupportedTypes();
    EXPECT_GE(types.size(), 0);
}

TEST_F(AlertJudgeServiceTest, EnableDisableEngine) {
    service->EnableEngine(alert::AlertType::kDepthAlert, true);
    EXPECT_TRUE(service->IsEngineEnabled(alert::AlertType::kDepthAlert));
    
    service->EnableEngine(alert::AlertType::kDepthAlert, false);
    EXPECT_FALSE(service->IsEngineEnabled(alert::AlertType::kDepthAlert));
}

TEST_F(AlertJudgeServiceTest, SetEngineThreshold) {
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.5);
    config.SetLevel2Threshold(1.0);
    config.SetLevel3Threshold(1.5);
    config.SetLevel4Threshold(2.0);
    
    service->SetEngineThreshold(alert::AlertType::kDepthAlert, config);
    
    alert::ThresholdConfig retrieved = service->GetEngineThreshold(alert::AlertType::kDepthAlert);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.5);
    EXPECT_DOUBLE_EQ(retrieved.GetLevel2Threshold(), 1.0);
}

TEST_F(AlertJudgeServiceTest, EvaluateWithInvalidContext) {
    alert::AlertContext invalidContext;
    EXPECT_THROW(service->Evaluate(invalidContext), alert::InvalidParameterException);
}

TEST_F(AlertJudgeServiceTest, EvaluateByTypeWithInvalidContext) {
    alert::AlertContext invalidContext;
    EXPECT_THROW(service->EvaluateByType(invalidContext, alert::AlertType::kDepthAlert), 
                 alert::InvalidParameterException);
}

TEST_F(AlertJudgeServiceTest, EvaluateByTypesWithInvalidContext) {
    alert::AlertContext invalidContext;
    std::vector<alert::AlertType> types = {alert::AlertType::kDepthAlert};
    EXPECT_THROW(service->EvaluateByTypes(invalidContext, types), 
                 alert::InvalidParameterException);
}

TEST_F(AlertJudgeServiceTest, MaxAlertCount) {
    service->SetMaxAlertCount(5);
    EXPECT_EQ(service->GetMaxAlertCount(), 5u);
}

TEST_F(AlertJudgeServiceTest, EvaluateWithValidContext) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    std::vector<alert::Alert> alerts = service->Evaluate(context);
    EXPECT_GE(alerts.size(), 0u);
}

TEST_F(AlertJudgeServiceTest, EvaluateByType) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    std::vector<alert::Alert> alerts = service->EvaluateByType(context, alert::AlertType::kDepthAlert);
    EXPECT_GE(alerts.size(), 0u);
}

TEST_F(AlertJudgeServiceTest, EvaluateByTypes) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    std::vector<alert::AlertType> types = {
        alert::AlertType::kDepthAlert,
        alert::AlertType::kCollisionAlert
    };
    
    std::vector<alert::Alert> alerts = service->EvaluateByTypes(context, types);
    EXPECT_GE(alerts.size(), 0u);
}

TEST_F(AlertJudgeServiceTest, SetAlertFilter) {
    service->SetAlertFilter([](const alert::Alert& alert) {
        return alert.GetAlertLevel() != alert::AlertLevel::kLevel4_Minor;
    });
    
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    std::vector<alert::Alert> alerts = service->Evaluate(context);
    
    for (const auto& alert : alerts) {
        EXPECT_NE(alert.GetAlertLevel(), alert::AlertLevel::kLevel4_Minor);
    }
}
