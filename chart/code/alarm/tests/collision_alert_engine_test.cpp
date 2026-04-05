#include <gtest/gtest.h>
#include "alert/collision_alert_engine.h"
#include "alert/alert_context.h"
#include "alert/alert_exception.h"

class CollisionAlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = new alert::CollisionAlertEngine();
    }
    
    void TearDown() override {
        delete engine;
    }
    
    alert::CollisionAlertEngine* engine;
    
    static std::vector<alert::AISTarget> MockAISProvider(const alert::Coordinate& pos, double range) {
        std::vector<alert::AISTarget> targets;
        
        alert::AISTarget target1;
        target1.targetId = "TARGET001";
        target1.targetName = "Test Target 1";
        target1.position = alert::Coordinate(pos.GetLongitude() + 0.01, pos.GetLatitude());
        target1.speed = 10.0;
        target1.course = 180.0;
        targets.push_back(target1);
        
        alert::AISTarget target2;
        target2.targetId = "TARGET002";
        target2.targetName = "Test Target 2";
        target2.position = alert::Coordinate(pos.GetLongitude() + 0.02, pos.GetLatitude());
        target2.speed = 12.0;
        target2.course = 270.0;
        targets.push_back(target2);
        
        return targets;
    }
};

TEST_F(CollisionAlertEngineTest, GetType) {
    EXPECT_EQ(engine->GetType(), alert::AlertType::kCollisionAlert);
}

TEST_F(CollisionAlertEngineTest, GetName) {
    EXPECT_EQ(engine->GetName(), "CollisionAlertEngine");
}

TEST_F(CollisionAlertEngineTest, SetAndGetThreshold) {
    alert::ThresholdConfig config("CollisionThreshold");
    config.SetLevel1Threshold(0.5);
    config.SetLevel2Threshold(1.0);
    config.SetLevel3Threshold(2.0);
    config.SetLevel4Threshold(3.0);
    config.SetParameter("tcpa_level1", 6.0);
    config.SetParameter("tcpa_level2", 15.0);
    config.SetParameter("tcpa_level3", 30.0);
    
    engine->SetThreshold(config);
    
    alert::ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_EQ(retrieved.GetName(), "CollisionThreshold");
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.5);
}

TEST_F(CollisionAlertEngineTest, EvaluateWithInvalidContext) {
    alert::AlertContext context;
    
    EXPECT_THROW(engine->Evaluate(context), alert::InvalidParameterException);
}

TEST_F(CollisionAlertEngineTest, EvaluateNoAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(15.0);
    context.SetCourse(0.0);
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.1);
    config.SetLevel2Threshold(0.2);
    config.SetLevel3Threshold(0.3);
    config.SetLevel4Threshold(0.5);
    engine->SetThreshold(config);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kNone);
}

TEST_F(CollisionAlertEngineTest, EvaluateWithAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(15.0);
    context.SetCourse(0.0);
    
    engine->SetAISProvider(MockAISProvider);
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(5.0);
    config.SetLevel2Threshold(10.0);
    config.SetLevel3Threshold(20.0);
    config.SetLevel4Threshold(30.0);
    config.SetParameter("tcpa_level1", 60.0);
    config.SetParameter("tcpa_level2", 120.0);
    config.SetParameter("tcpa_level3", 180.0);
    engine->SetThreshold(config);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_NE(alert.GetAlertLevel(), alert::AlertLevel::kNone);
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kCollisionAlert);
}

TEST_F(CollisionAlertEngineTest, CalculateCPA_TCPA) {
    alert::Coordinate myPos(120.5, 31.2);
    alert::Coordinate targetPos(120.51, 31.2);
    
    double cpa, tcpa;
    engine->CalculateCPA_TCPA(myPos, 10.0, 0.0, targetPos, 10.0, 180.0, cpa, tcpa);
    
    EXPECT_GE(cpa, 0.0);
    EXPECT_GE(tcpa, 0.0);
}

TEST_F(CollisionAlertEngineTest, CalculateCPA_TCPA_HeadOn) {
    alert::Coordinate myPos(120.5, 31.2);
    alert::Coordinate targetPos(120.51, 31.2);
    
    double cpa, tcpa;
    engine->CalculateCPA_TCPA(myPos, 10.0, 90.0, targetPos, 10.0, 270.0, cpa, tcpa);
    
    EXPECT_GE(cpa, 0.0);
}

TEST_F(CollisionAlertEngineTest, AssessRisk) {
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(0.5);
    config.SetLevel2Threshold(1.0);
    config.SetLevel3Threshold(2.0);
    config.SetLevel4Threshold(3.0);
    config.SetParameter("tcpa_level1", 6.0);
    config.SetParameter("tcpa_level2", 15.0);
    config.SetParameter("tcpa_level3", 30.0);
    engine->SetThreshold(config);
    
    EXPECT_EQ(engine->AssessRisk(0.3, 5.0), alert::AlertLevel::kLevel1_Critical);
    EXPECT_EQ(engine->AssessRisk(0.8, 10.0), alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(engine->AssessRisk(1.5, 20.0), alert::AlertLevel::kLevel3_Moderate);
    EXPECT_EQ(engine->AssessRisk(2.5, 40.0), alert::AlertLevel::kLevel4_Minor);
    EXPECT_EQ(engine->AssessRisk(5.0, 60.0), alert::AlertLevel::kNone);
}

TEST_F(CollisionAlertEngineTest, GetRiskTargets) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    context.SetSpeed(15.0);
    context.SetCourse(0.0);
    
    engine->SetAISProvider(MockAISProvider);
    
    alert::ThresholdConfig config;
    config.SetLevel1Threshold(5.0);
    config.SetLevel2Threshold(10.0);
    config.SetLevel3Threshold(20.0);
    config.SetLevel4Threshold(30.0);
    config.SetParameter("tcpa_level1", 60.0);
    config.SetParameter("tcpa_level2", 120.0);
    config.SetParameter("tcpa_level3", 180.0);
    engine->SetThreshold(config);
    
    std::vector<alert::CollisionTarget> targets = engine->GetRiskTargets(context);
    
    EXPECT_GE(targets.size(), 0);
}
