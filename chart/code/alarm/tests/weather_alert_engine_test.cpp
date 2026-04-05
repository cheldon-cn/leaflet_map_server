#include <gtest/gtest.h>
#include "alert/weather_alert_engine.h"
#include "alert/alert_context.h"
#include "alert/alert_exception.h"

class WeatherAlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = new alert::WeatherAlertEngine();
    }
    
    void TearDown() override {
        delete engine;
    }
    
    alert::WeatherAlertEngine* engine;
    
    static std::vector<alert::WeatherAlertData> MockWeatherProvider(const alert::Coordinate& pos) {
        std::vector<alert::WeatherAlertData> alerts;
        
        alert::WeatherAlertData alert1;
        alert1.alertId = "WX001";
        alert1.alertType = "STORM";
        alert1.level = alert::AlertLevel::kLevel2_Severe;
        alert1.issueTime = "2026-01-01 10:00:00";
        alert1.expireTime = "2026-01-01 18:00:00";
        alert1.content = "Storm warning in effect";
        alert1.affectedArea = "Area A";
        alerts.push_back(alert1);
        
        return alerts;
    }
};

TEST_F(WeatherAlertEngineTest, GetType) {
    EXPECT_EQ(engine->GetType(), alert::AlertType::kWeatherAlert);
}

TEST_F(WeatherAlertEngineTest, GetName) {
    EXPECT_EQ(engine->GetName(), "WeatherAlertEngine");
}

TEST_F(WeatherAlertEngineTest, SetAndGetThreshold) {
    alert::ThresholdConfig config("WeatherThreshold");
    config.SetLevel1Threshold(1.0);
    
    engine->SetThreshold(config);
    
    alert::ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_EQ(retrieved.GetName(), "WeatherThreshold");
}

TEST_F(WeatherAlertEngineTest, EvaluateWithInvalidContext) {
    alert::AlertContext context;
    
    EXPECT_THROW(engine->Evaluate(context), alert::InvalidParameterException);
}

TEST_F(WeatherAlertEngineTest, EvaluateNoAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kNone);
}

TEST_F(WeatherAlertEngineTest, EvaluateWithAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    
    engine->SetWeatherProvider(MockWeatherProvider);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_NE(alert.GetAlertLevel(), alert::AlertLevel::kNone);
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kWeatherAlert);
}

TEST_F(WeatherAlertEngineTest, ConvertToAlert) {
    alert::WeatherAlertData data;
    data.alertId = "WX001";
    data.alertType = "STORM";
    data.level = alert::AlertLevel::kLevel2_Severe;
    data.content = "Storm warning";
    
    alert::Alert alert = engine->ConvertToAlert(data);
    
    EXPECT_EQ(alert.GetAlertType(), alert::AlertType::kWeatherAlert);
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(alert.GetMessage(), "Storm warning");
}

TEST_F(WeatherAlertEngineTest, SetEnabledTypes) {
    std::vector<std::string> types;
    types.push_back("STORM");
    types.push_back("FOG");
    
    engine->SetEnabledTypes(types);
}

TEST_F(WeatherAlertEngineTest, SetWeatherProvider) {
    engine->SetWeatherProvider(MockWeatherProvider);
}
