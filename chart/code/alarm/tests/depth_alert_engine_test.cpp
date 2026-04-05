#include <gtest/gtest.h>
#include "alert/depth_alert_engine.h"
#include "alert/alert_context.h"
#include "alert/alert_exception.h"

class DepthAlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = new alert::DepthAlertEngine();
    }
    
    void TearDown() override {
        delete engine;
    }
    
    alert::DepthAlertEngine* engine;
    
    static double MockDepthProvider(const alert::Coordinate& pos) {
        return 15.0;
    }
    
    static double MockTidalProvider(const alert::Coordinate& pos) {
        return 2.0;
    }
};

TEST_F(DepthAlertEngineTest, GetType) {
    EXPECT_EQ(engine->GetType(), alert::AlertType::kDepthAlert);
}

TEST_F(DepthAlertEngineTest, GetName) {
    EXPECT_EQ(engine->GetName(), "DepthAlertEngine");
}

TEST_F(DepthAlertEngineTest, SetAndGetThreshold) {
    alert::ThresholdConfig config("DepthThreshold");
    config.SetLevel1Threshold(0.2);
    config.SetLevel2Threshold(0.4);
    config.SetLevel3Threshold(0.8);
    config.SetLevel4Threshold(1.2);
    
    engine->SetThreshold(config);
    
    alert::ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_EQ(retrieved.GetName(), "DepthThreshold");
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.2);
}

TEST_F(DepthAlertEngineTest, EvaluateWithInvalidContext) {
    alert::AlertContext context;
    
    EXPECT_THROW(engine->Evaluate(context), alert::InvalidParameterException);
}

TEST_F(DepthAlertEngineTest, EvaluateNoAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(5.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    engine->SetDepthProvider(MockDepthProvider);
    engine->SetTidalProvider(MockTidalProvider);
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.1);
    config.SetLevel2Threshold(0.2);
    config.SetLevel3Threshold(0.5);
    config.SetLevel4Threshold(1.0);
    engine->SetThreshold(config);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kNone);
}

TEST_F(DepthAlertEngineTest, EvaluateWithAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(16.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(10.0);
    
    engine->SetDepthProvider(MockDepthProvider);
    engine->SetTidalProvider(MockTidalProvider);
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.3);
    config.SetLevel2Threshold(0.5);
    config.SetLevel3Threshold(1.0);
    config.SetLevel4Threshold(1.5);
    engine->SetThreshold(config);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_NE(alert.GetAlertLevel(), alert::AlertLevel::kNone);
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kDepthAlert);
}

TEST_F(DepthAlertEngineTest, CalculateSquat) {
    double squat = engine->CalculateSquat(10.0, 20.0, 5.0, 0.7);
    EXPECT_GT(squat, 0.0);
    
    double squatZero = engine->CalculateSquat(0.0, 20.0, 5.0, 0.7);
    EXPECT_DOUBLE_EQ(squatZero, 0.0);
}

TEST_F(DepthAlertEngineTest, CalculateUKC) {
    double ukc = engine->CalculateUKC(20.0, 10.0, 2.0);
    EXPECT_DOUBLE_EQ(ukc, 8.0);
    
    double ukcNegative = engine->CalculateUKC(10.0, 15.0, 2.0);
    EXPECT_DOUBLE_EQ(ukcNegative, -7.0);
}

TEST_F(DepthAlertEngineTest, DetermineAlertLevel) {
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.3);
    config.SetLevel2Threshold(0.5);
    config.SetLevel3Threshold(1.0);
    config.SetLevel4Threshold(1.5);
    engine->SetThreshold(config);
    
    EXPECT_EQ(engine->DetermineAlertLevel(0.1), alert::AlertLevel::kLevel1_Critical);
    EXPECT_EQ(engine->DetermineAlertLevel(0.4), alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(engine->DetermineAlertLevel(0.8), alert::AlertLevel::kLevel3_Moderate);
    EXPECT_EQ(engine->DetermineAlertLevel(1.2), alert::AlertLevel::kLevel4_Minor);
    EXPECT_EQ(engine->DetermineAlertLevel(2.0), alert::AlertLevel::kNone);
}

TEST_F(DepthAlertEngineTest, SetProviders) {
    engine->SetDepthProvider(MockDepthProvider);
    engine->SetTidalProvider(MockTidalProvider);
    
    double depth = engine->CalculateEffectiveDepth(alert::Coordinate(120.5, 31.2));
    EXPECT_DOUBLE_EQ(depth, 15.0);
}
