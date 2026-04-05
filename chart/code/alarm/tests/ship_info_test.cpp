#include <gtest/gtest.h>
#include "alert/ship_info.h"

class ShipInfoTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ShipInfoTest, DefaultConstructor) {
    alert::ShipInfo ship;
    EXPECT_TRUE(ship.GetShipId().empty());
    EXPECT_TRUE(ship.GetShipName().empty());
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kUnknown);
    EXPECT_DOUBLE_EQ(ship.GetDraft(), 0.0);
    EXPECT_DOUBLE_EQ(ship.GetLength(), 0.0);
    EXPECT_DOUBLE_EQ(ship.GetBeam(), 0.0);
    EXPECT_DOUBLE_EQ(ship.GetBlockCoefficient(), 0.7);
}

TEST_F(ShipInfoTest, TwoParamConstructor) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    EXPECT_EQ(ship.GetShipId(), "SHIP001");
    EXPECT_EQ(ship.GetShipName(), "Test Ship");
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kUnknown);
}

TEST_F(ShipInfoTest, SettersAndGetters) {
    alert::ShipInfo ship;
    
    ship.SetShipId("SHIP002");
    EXPECT_EQ(ship.GetShipId(), "SHIP002");
    
    ship.SetShipName("Test Ship 2");
    EXPECT_EQ(ship.GetShipName(), "Test Ship 2");
    
    ship.SetShipType(alert::ShipType::kCargo);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kCargo);
    
    ship.SetDraft(10.5);
    EXPECT_DOUBLE_EQ(ship.GetDraft(), 10.5);
    
    ship.SetLength(200.0);
    EXPECT_DOUBLE_EQ(ship.GetLength(), 200.0);
    
    ship.SetBeam(30.0);
    EXPECT_DOUBLE_EQ(ship.GetBeam(), 30.0);
    
    ship.SetBlockCoefficient(0.75);
    EXPECT_DOUBLE_EQ(ship.GetBlockCoefficient(), 0.75);
    
    ship.SetHeight(25.0);
    EXPECT_DOUBLE_EQ(ship.GetHeight(), 25.0);
    
    ship.SetGrossTonnage(15000.0);
    EXPECT_DOUBLE_EQ(ship.GetGrossTonnage(), 15000.0);
    
    ship.SetNetTonnage(10000.0);
    EXPECT_DOUBLE_EQ(ship.GetNetTonnage(), 10000.0);
    
    ship.SetMaxSpeed(20.0);
    EXPECT_DOUBLE_EQ(ship.GetMaxSpeed(), 20.0);
}

TEST_F(ShipInfoTest, IsValid) {
    alert::ShipInfo ship1;
    EXPECT_FALSE(ship1.IsValid());
    
    alert::ShipInfo ship2("SHIP001", "Test");
    EXPECT_FALSE(ship2.IsValid());
    
    alert::ShipInfo ship3("SHIP001", "Test");
    ship3.SetDraft(10.0);
    EXPECT_TRUE(ship3.IsValid());
    
    alert::ShipInfo ship4;
    ship4.SetDraft(10.0);
    EXPECT_FALSE(ship4.IsValid());
}

TEST_F(ShipInfoTest, AllShipTypes) {
    alert::ShipInfo ship;
    
    ship.SetShipType(alert::ShipType::kUnknown);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kUnknown);
    
    ship.SetShipType(alert::ShipType::kCargo);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kCargo);
    
    ship.SetShipType(alert::ShipType::kTanker);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kTanker);
    
    ship.SetShipType(alert::ShipType::kPassenger);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kPassenger);
    
    ship.SetShipType(alert::ShipType::kFishing);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kFishing);
    
    ship.SetShipType(alert::ShipType::kTug);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kTug);
    
    ship.SetShipType(alert::ShipType::kPilot);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kPilot);
    
    ship.SetShipType(alert::ShipType::kSearchRescue);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kSearchRescue);
    
    ship.SetShipType(alert::ShipType::kPleasure);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kPleasure);
    
    ship.SetShipType(alert::ShipType::kHighSpeed);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kHighSpeed);
    
    ship.SetShipType(alert::ShipType::kOther);
    EXPECT_EQ(ship.GetShipType(), alert::ShipType::kOther);
}
