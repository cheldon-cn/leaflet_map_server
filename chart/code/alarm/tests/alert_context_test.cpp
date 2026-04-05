#include <gtest/gtest.h>
#include "alert/alert_context.h"

class AlertContextTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlertContextTest, DefaultConstructor) {
    alert::AlertContext context;
    EXPECT_DOUBLE_EQ(context.GetSpeed(), 0.0);
    EXPECT_DOUBLE_EQ(context.GetCourse(), 0.0);
    EXPECT_DOUBLE_EQ(context.GetHeading(), 0.0);
    EXPECT_DOUBLE_EQ(context.GetRateOfTurn(), 0.0);
}

TEST_F(AlertContextTest, TwoParamConstructor) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    
    EXPECT_EQ(context.GetShipInfo().GetShipId(), "SHIP001");
    EXPECT_DOUBLE_EQ(context.GetPosition().GetLongitude(), 120.5);
    EXPECT_DOUBLE_EQ(context.GetPosition().GetLatitude(), 31.2);
}

TEST_F(AlertContextTest, SettersAndGetters) {
    alert::AlertContext context;
    
    alert::ShipInfo ship("SHIP002", "Test Ship 2");
    ship.SetDraft(8.0);
    context.SetShipInfo(ship);
    EXPECT_EQ(context.GetShipInfo().GetShipId(), "SHIP002");
    
    alert::Coordinate position(121.0, 32.0);
    context.SetPosition(position);
    EXPECT_DOUBLE_EQ(context.GetPosition().GetLongitude(), 121.0);
    EXPECT_DOUBLE_EQ(context.GetPosition().GetLatitude(), 32.0);
    
    context.SetSpeed(15.5);
    EXPECT_DOUBLE_EQ(context.GetSpeed(), 15.5);
    
    context.SetCourse(180.0);
    EXPECT_DOUBLE_EQ(context.GetCourse(), 180.0);
    
    context.SetHeading(175.0);
    EXPECT_DOUBLE_EQ(context.GetHeading(), 175.0);
    
    context.SetRateOfTurn(2.5);
    EXPECT_DOUBLE_EQ(context.GetRateOfTurn(), 2.5);
    
    context.SetTimestamp("2026-01-01 10:00:00");
    EXPECT_EQ(context.GetTimestamp(), "2026-01-01 10:00:00");
}

TEST_F(AlertContextTest, EnvironmentData) {
    alert::AlertContext context;
    
    alert::EnvironmentData envData;
    envData.windSpeed = 10.0;
    envData.windDirection = 45.0;
    envData.waveHeight = 2.0;
    envData.wavePeriod = 6.0;
    envData.visibility = 5.0;
    envData.currentSpeed = 1.5;
    envData.currentDirection = 90.0;
    envData.airTemperature = 25.0;
    envData.waterTemperature = 20.0;
    envData.pressure = 1015.0;
    
    context.SetEnvironmentData(envData);
    
    const auto& data = context.GetEnvironmentData();
    EXPECT_DOUBLE_EQ(data.windSpeed, 10.0);
    EXPECT_DOUBLE_EQ(data.windDirection, 45.0);
    EXPECT_DOUBLE_EQ(data.waveHeight, 2.0);
    EXPECT_DOUBLE_EQ(data.wavePeriod, 6.0);
    EXPECT_DOUBLE_EQ(data.visibility, 5.0);
    EXPECT_DOUBLE_EQ(data.currentSpeed, 1.5);
    EXPECT_DOUBLE_EQ(data.currentDirection, 90.0);
    EXPECT_DOUBLE_EQ(data.airTemperature, 25.0);
    EXPECT_DOUBLE_EQ(data.waterTemperature, 20.0);
    EXPECT_DOUBLE_EQ(data.pressure, 1015.0);
}

TEST_F(AlertContextTest, Parameters) {
    alert::AlertContext context;
    
    context.SetParameter("custom1", 100.0);
    EXPECT_DOUBLE_EQ(context.GetParameter("custom1"), 100.0);
    
    EXPECT_DOUBLE_EQ(context.GetParameter("nonexistent", 0.0), 0.0);
    EXPECT_DOUBLE_EQ(context.GetParameter("nonexistent", 50.0), 50.0);
}

TEST_F(AlertContextTest, IsValid) {
    alert::AlertContext context1;
    EXPECT_FALSE(context1.IsValid());
    
    alert::ShipInfo ship("SHIP001", "Test");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context2(ship, position);
    EXPECT_TRUE(context2.IsValid());
    
    alert::AlertContext context3;
    alert::Coordinate invalidPosition(200.0, 100.0);
    context3.SetShipInfo(ship);
    context3.SetPosition(invalidPosition);
    EXPECT_FALSE(context3.IsValid());
}

TEST_F(AlertContextTest, EnvironmentDataDefaults) {
    alert::EnvironmentData envData;
    
    EXPECT_DOUBLE_EQ(envData.windSpeed, 0.0);
    EXPECT_DOUBLE_EQ(envData.windDirection, 0.0);
    EXPECT_DOUBLE_EQ(envData.waveHeight, 0.0);
    EXPECT_DOUBLE_EQ(envData.wavePeriod, 0.0);
    EXPECT_DOUBLE_EQ(envData.visibility, 10.0);
    EXPECT_DOUBLE_EQ(envData.currentSpeed, 0.0);
    EXPECT_DOUBLE_EQ(envData.currentDirection, 0.0);
    EXPECT_DOUBLE_EQ(envData.airTemperature, 20.0);
    EXPECT_DOUBLE_EQ(envData.waterTemperature, 15.0);
    EXPECT_DOUBLE_EQ(envData.pressure, 1013.25);
}
