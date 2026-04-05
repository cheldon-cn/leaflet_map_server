#include <gtest/gtest.h>
#include "alert/alert_engine_factory.h"
#include "alert/alert_exception.h"
#include "alert/depth_alert_engine.h"
#include "alert/collision_alert_engine.h"

class AlertEngineFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlertEngineFactoryTest, GetInstance) {
    alert::AlertEngineFactory& factory1 = alert::AlertEngineFactory::GetInstance();
    alert::AlertEngineFactory& factory2 = alert::AlertEngineFactory::GetInstance();
    
    EXPECT_EQ(&factory1, &factory2);
}

TEST_F(AlertEngineFactoryTest, RegisterAndCreateEngine) {
    alert::AlertEngineFactory& factory = alert::AlertEngineFactory::GetInstance();
    
    factory.RegisterEngine(alert::AlertType::kDepthAlert, []() -> alert::IAlertEngine* {
        return new alert::DepthAlertEngine();
    });
    
    EXPECT_TRUE(factory.IsEngineRegistered(alert::AlertType::kDepthAlert));
    
    alert::IAlertEngine* engine = factory.CreateEngine(alert::AlertType::kDepthAlert);
    EXPECT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), alert::AlertType::kDepthAlert);
    EXPECT_EQ(engine->GetName(), "DepthAlertEngine");
    
    delete engine;
}

TEST_F(AlertEngineFactoryTest, CreateUnregisteredEngine) {
    alert::AlertEngineFactory& factory = alert::AlertEngineFactory::GetInstance();
    
    EXPECT_THROW(factory.CreateEngine(alert::AlertType::kOtherAlert), alert::EngineException);
}

TEST_F(AlertEngineFactoryTest, GetRegisteredTypes) {
    alert::AlertEngineFactory& factory = alert::AlertEngineFactory::GetInstance();
    
    factory.RegisterEngine(alert::AlertType::kDepthAlert, []() -> alert::IAlertEngine* {
        return new alert::DepthAlertEngine();
    });
    factory.RegisterEngine(alert::AlertType::kCollisionAlert, []() -> alert::IAlertEngine* {
        return new alert::CollisionAlertEngine();
    });
    
    std::vector<alert::AlertType> types = factory.GetRegisteredTypes();
    
    EXPECT_GE(types.size(), 2);
    
    bool hasDepth = false;
    bool hasCollision = false;
    for (const auto& type : types) {
        if (type == alert::AlertType::kDepthAlert) hasDepth = true;
        if (type == alert::AlertType::kCollisionAlert) hasCollision = true;
    }
    EXPECT_TRUE(hasDepth);
    EXPECT_TRUE(hasCollision);
}

TEST_F(AlertEngineFactoryTest, EngineThreshold) {
    alert::AlertEngineFactory& factory = alert::AlertEngineFactory::GetInstance();
    
    factory.RegisterEngine(alert::AlertType::kDepthAlert, []() -> alert::IAlertEngine* {
        return new alert::DepthAlertEngine();
    });
    
    alert::IAlertEngine* engine = factory.CreateEngine(alert::AlertType::kDepthAlert);
    
    alert::ThresholdConfig config("TestThreshold");
    config.SetLevel1Threshold(0.1);
    engine->SetThreshold(config);
    
    alert::ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_EQ(retrieved.GetName(), "TestThreshold");
    EXPECT_DOUBLE_EQ(retrieved.GetLevel1Threshold(), 0.1);
    
    delete engine;
}

TEST_F(AlertEngineFactoryTest, EngineRegistrar) {
    alert::EngineRegistrar<alert::DepthAlertEngine> registrar(alert::AlertType::kDepthAlert);
    
    alert::AlertEngineFactory& factory = alert::AlertEngineFactory::GetInstance();
    EXPECT_TRUE(factory.IsEngineRegistered(alert::AlertType::kDepthAlert));
}
