#include <gtest/gtest.h>
#include "ogc/navi/ais/ais_target.h"

using namespace ogc::navi;

class AisTargetTest : public ::testing::Test {
protected:
    void SetUp() override {
        target = AisTarget::Create(123456789);
        target->SetLatitude(31.5);
        target->SetLongitude(121.5);
        target->SetSpeedOverGround(10.0);
        target->SetCourseOverGround(45.0);
        target->SetHeading(45.0);
        target->SetShipType(AisShipType::Cargo);
        target->SetShipName("TestShip");
        target->SetCallsign("TEST");
        target->SetDestination("Shanghai");
        target->SetShipLength(100);
        target->SetShipWidth(20);
        target->SetDraught(5.0);
        target->SetNavigationStatus(AisNavigationStatus::UnderWayUsingEngine);
        target->SetLastUpdate(1000.0);
    }
    
    void TearDown() override {
        if (target) {
            target->ReleaseReference();
        }
    }
    
    AisTarget* target = nullptr;
};

TEST_F(AisTargetTest, CreateWithMmsi) {
    EXPECT_EQ(target->GetMmsi(), 123456789);
}

TEST_F(AisTargetTest, SetAndGetLatitude) {
    target->SetLatitude(32.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), 32.0);
}

TEST_F(AisTargetTest, SetAndGetLongitude) {
    target->SetLongitude(122.0);
    EXPECT_DOUBLE_EQ(target->GetLongitude(), 122.0);
}

TEST_F(AisTargetTest, SetAndGetSpeedOverGround) {
    target->SetSpeedOverGround(15.0);
    EXPECT_DOUBLE_EQ(target->GetSpeedOverGround(), 15.0);
}

TEST_F(AisTargetTest, SetAndGetCourseOverGround) {
    target->SetCourseOverGround(90.0);
    EXPECT_DOUBLE_EQ(target->GetCourseOverGround(), 90.0);
}

TEST_F(AisTargetTest, SetAndGetHeading) {
    target->SetHeading(90.0);
    EXPECT_DOUBLE_EQ(target->GetHeading(), 90.0);
}

TEST_F(AisTargetTest, SetAndGetShipType) {
    target->SetShipType(AisShipType::Tanker);
    EXPECT_EQ(target->GetShipType(), AisShipType::Tanker);
}

TEST_F(AisTargetTest, SetAndGetShipName) {
    target->SetShipName("NewShip");
    EXPECT_EQ(target->GetShipName(), "NewShip");
}

TEST_F(AisTargetTest, SetAndGetCallsign) {
    target->SetCallsign("NEW");
    EXPECT_EQ(target->GetCallsign(), "NEW");
}

TEST_F(AisTargetTest, SetAndGetDestination) {
    target->SetDestination("Beijing");
    EXPECT_EQ(target->GetDestination(), "Beijing");
}

TEST_F(AisTargetTest, SetAndGetShipLength) {
    target->SetShipLength(200);
    EXPECT_EQ(target->GetShipLength(), 200);
}

TEST_F(AisTargetTest, SetAndGetShipWidth) {
    target->SetShipWidth(30);
    EXPECT_EQ(target->GetShipWidth(), 30);
}

TEST_F(AisTargetTest, SetAndGetDraught) {
    target->SetDraught(10.0);
    EXPECT_DOUBLE_EQ(target->GetDraught(), 10.0);
}

TEST_F(AisTargetTest, SetAndGetNavigationStatus) {
    target->SetNavigationStatus(AisNavigationStatus::AtAnchor);
    EXPECT_EQ(target->GetNavigationStatus(), AisNavigationStatus::AtAnchor);
}

TEST_F(AisTargetTest, SetAndGetLastUpdate) {
    target->SetLastUpdate(2000.0);
    EXPECT_DOUBLE_EQ(target->GetLastUpdate(), 2000.0);
}

TEST_F(AisTargetTest, SetAndGetCpa) {
    target->SetCpa(500.0);
    EXPECT_DOUBLE_EQ(target->GetCpa(), 500.0);
}

TEST_F(AisTargetTest, SetAndGetTcpa) {
    target->SetTcpa(300.0);
    EXPECT_DOUBLE_EQ(target->GetTcpa(), 300.0);
}

TEST_F(AisTargetTest, SetAndGetCollisionRisk) {
    target->SetCollisionRisk(true);
    EXPECT_TRUE(target->HasCollisionRisk());
    
    target->SetCollisionRisk(false);
    EXPECT_FALSE(target->HasCollisionRisk());
}

TEST_F(AisTargetTest, GetPosition) {
    GeoPoint pos = target->GetPosition();
    EXPECT_DOUBLE_EQ(pos.longitude, 121.5);
    EXPECT_DOUBLE_EQ(pos.latitude, 31.5);
}

TEST_F(AisTargetTest, SetPosition) {
    target->SetPosition(122.0, 32.0);
    EXPECT_DOUBLE_EQ(target->GetLongitude(), 122.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), 32.0);
}

TEST_F(AisTargetTest, ToDataFromData) {
    AisTargetData data = target->ToData();
    
    EXPECT_EQ(data.mmsi, 123456789);
    EXPECT_DOUBLE_EQ(data.latitude, 31.5);
    EXPECT_DOUBLE_EQ(data.longitude, 121.5);
    EXPECT_DOUBLE_EQ(data.speed_over_ground, 10.0);
    EXPECT_DOUBLE_EQ(data.course_over_ground, 45.0);
    EXPECT_EQ(data.ship_name, "TestShip");
    
    AisTarget* target2 = AisTarget::Create();
    target2->FromData(data);
    
    EXPECT_EQ(target2->GetMmsi(), 123456789);
    EXPECT_DOUBLE_EQ(target2->GetLatitude(), 31.5);
    EXPECT_DOUBLE_EQ(target2->GetLongitude(), 121.5);
    
    target2->ReleaseReference();
}

TEST_F(AisTargetTest, UpdatePositionReport) {
    AisPositionReport report;
    report.mmsi = 123456789;
    report.longitude = 122.0;
    report.latitude = 32.0;
    report.speed_over_ground = 15.0;
    report.course_over_ground = 90.0;
    report.heading = 90.0;
    report.nav_status = AisNavigationStatus::AtAnchor;
    report.timestamp = 2000.0;
    
    target->UpdatePositionReport(report);
    
    EXPECT_DOUBLE_EQ(target->GetLongitude(), 122.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), 32.0);
    EXPECT_DOUBLE_EQ(target->GetSpeedOverGround(), 15.0);
    EXPECT_DOUBLE_EQ(target->GetCourseOverGround(), 90.0);
    EXPECT_EQ(target->GetNavigationStatus(), AisNavigationStatus::AtAnchor);
}

TEST_F(AisTargetTest, UpdateStaticData) {
    AisStaticData data;
    data.mmsi = 123456789;
    data.ship_name = "UpdatedShip";
    data.callsign = "UPD";
    data.ship_type = AisShipType::Tanker;
    data.dimension_to_bow = 50;
    data.dimension_to_stern = 50;
    data.dimension_to_port = 10;
    data.dimension_to_starboard = 10;
    data.draught = 8.0;
    data.destination = "Tokyo";
    
    target->UpdateStaticData(data);
    
    EXPECT_EQ(target->GetShipName(), "UpdatedShip");
    EXPECT_EQ(target->GetCallsign(), "UPD");
    EXPECT_EQ(target->GetShipType(), AisShipType::Tanker);
    EXPECT_EQ(target->GetShipLength(), 100);
    EXPECT_EQ(target->GetShipWidth(), 20);
    EXPECT_DOUBLE_EQ(target->GetDraught(), 8.0);
    EXPECT_EQ(target->GetDestination(), "Tokyo");
}

TEST_F(AisTargetTest, Create_WithZeroMmsi) {
    AisTarget* t = AisTarget::Create(0);
    EXPECT_EQ(t->GetMmsi(), 0);
    t->ReleaseReference();
}

TEST_F(AisTargetTest, SetLatitude_MaxBoundary) {
    target->SetLatitude(90.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), 90.0);
}

TEST_F(AisTargetTest, SetLatitude_MinBoundary) {
    target->SetLatitude(-90.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), -90.0);
}

TEST_F(AisTargetTest, SetLongitude_MaxBoundary) {
    target->SetLongitude(180.0);
    EXPECT_DOUBLE_EQ(target->GetLongitude(), 180.0);
}

TEST_F(AisTargetTest, SetLongitude_MinBoundary) {
    target->SetLongitude(-180.0);
    EXPECT_DOUBLE_EQ(target->GetLongitude(), -180.0);
}

TEST_F(AisTargetTest, SetSpeedOverGround_Zero) {
    target->SetSpeedOverGround(0.0);
    EXPECT_DOUBLE_EQ(target->GetSpeedOverGround(), 0.0);
}

TEST_F(AisTargetTest, SetSpeedOverGround_Negative) {
    target->SetSpeedOverGround(-10.0);
    EXPECT_DOUBLE_EQ(target->GetSpeedOverGround(), -10.0);
}

TEST_F(AisTargetTest, SetCourseOverGround_Zero) {
    target->SetCourseOverGround(0.0);
    EXPECT_DOUBLE_EQ(target->GetCourseOverGround(), 0.0);
}

TEST_F(AisTargetTest, SetCourseOverGround_MaxBoundary) {
    target->SetCourseOverGround(360.0);
    EXPECT_DOUBLE_EQ(target->GetCourseOverGround(), 360.0);
}

TEST_F(AisTargetTest, SetHeading_Zero) {
    target->SetHeading(0.0);
    EXPECT_DOUBLE_EQ(target->GetHeading(), 0.0);
}

TEST_F(AisTargetTest, SetHeading_MaxBoundary) {
    target->SetHeading(360.0);
    EXPECT_DOUBLE_EQ(target->GetHeading(), 360.0);
}

TEST_F(AisTargetTest, SetShipLength_Zero) {
    target->SetShipLength(0);
    EXPECT_EQ(target->GetShipLength(), 0);
}

TEST_F(AisTargetTest, SetShipWidth_Zero) {
    target->SetShipWidth(0);
    EXPECT_EQ(target->GetShipWidth(), 0);
}

TEST_F(AisTargetTest, SetDraught_Zero) {
    target->SetDraught(0.0);
    EXPECT_DOUBLE_EQ(target->GetDraught(), 0.0);
}

TEST_F(AisTargetTest, SetDraught_Negative) {
    target->SetDraught(-5.0);
    EXPECT_DOUBLE_EQ(target->GetDraught(), -5.0);
}

TEST_F(AisTargetTest, SetCpa_Zero) {
    target->SetCpa(0.0);
    EXPECT_DOUBLE_EQ(target->GetCpa(), 0.0);
}

TEST_F(AisTargetTest, SetCpa_Negative) {
    target->SetCpa(-100.0);
    EXPECT_DOUBLE_EQ(target->GetCpa(), -100.0);
}

TEST_F(AisTargetTest, SetTcpa_Zero) {
    target->SetTcpa(0.0);
    EXPECT_DOUBLE_EQ(target->GetTcpa(), 0.0);
}

TEST_F(AisTargetTest, SetTcpa_Negative) {
    target->SetTcpa(-60.0);
    EXPECT_DOUBLE_EQ(target->GetTcpa(), -60.0);
}

TEST_F(AisTargetTest, SetShipName_Empty) {
    target->SetShipName("");
    EXPECT_EQ(target->GetShipName(), "");
}

TEST_F(AisTargetTest, SetCallsign_Empty) {
    target->SetCallsign("");
    EXPECT_EQ(target->GetCallsign(), "");
}

TEST_F(AisTargetTest, SetDestination_Empty) {
    target->SetDestination("");
    EXPECT_EQ(target->GetDestination(), "");
}

TEST_F(AisTargetTest, SetLastUpdate_Zero) {
    target->SetLastUpdate(0.0);
    EXPECT_DOUBLE_EQ(target->GetLastUpdate(), 0.0);
}

TEST_F(AisTargetTest, SetLastUpdate_Negative) {
    target->SetLastUpdate(-1.0);
    EXPECT_DOUBLE_EQ(target->GetLastUpdate(), -1.0);
}

TEST_F(AisTargetTest, SetPosition_Equator) {
    target->SetPosition(0.0, 0.0);
    EXPECT_DOUBLE_EQ(target->GetLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(target->GetLatitude(), 0.0);
}

TEST_F(AisTargetTest, CreateWithEmptyData) {
    AisTargetData data;
    AisTarget* t = AisTarget::Create();
    t->FromData(data);
    
    EXPECT_EQ(t->GetMmsi(), 0);
    EXPECT_DOUBLE_EQ(t->GetLatitude(), 0.0);
    EXPECT_DOUBLE_EQ(t->GetLongitude(), 0.0);
    
    t->ReleaseReference();
}
