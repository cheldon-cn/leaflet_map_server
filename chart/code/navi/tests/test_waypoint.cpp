#include <gtest/gtest.h>
#include "ogc/navi/route/waypoint.h"

using namespace ogc::navi;

class WaypointTest : public ::testing::Test {
protected:
    void SetUp() override {
        wp1 = Waypoint::Create();
        wp1->SetPosition(121.4737, 31.2304);
        wp1->SetName("Shanghai");
        
        wp2 = Waypoint::Create();
        wp2->SetPosition(116.4074, 39.9042);
        wp2->SetName("Beijing");
    }
    
    void TearDown() override {
        if (wp1) wp1->ReleaseReference();
        if (wp2) wp2->ReleaseReference();
    }
    
    Waypoint* wp1 = nullptr;
    Waypoint* wp2 = nullptr;
};

TEST_F(WaypointTest, Create) {
    Waypoint* wp = Waypoint::Create();
    EXPECT_DOUBLE_EQ(wp->GetLatitude(), 0.0);
    EXPECT_DOUBLE_EQ(wp->GetLongitude(), 0.0);
    EXPECT_EQ(wp->GetName(), "");
    wp->ReleaseReference();
}

TEST_F(WaypointTest, SetAndGetLatitude) {
    wp1->SetLatitude(32.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), 32.0);
}

TEST_F(WaypointTest, SetAndGetLongitude) {
    wp1->SetLongitude(122.0);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), 122.0);
}

TEST_F(WaypointTest, SetAndGetName) {
    wp1->SetName("NewName");
    EXPECT_EQ(wp1->GetName(), "NewName");
}

TEST_F(WaypointTest, SetAndGetId) {
    wp1->SetId("wp-001");
    EXPECT_EQ(wp1->GetId(), "wp-001");
}

TEST_F(WaypointTest, SetAndGetArrivalRadius) {
    wp1->SetArrivalRadius(200.0);
    EXPECT_DOUBLE_EQ(wp1->GetArrivalRadius(), 200.0);
}

TEST_F(WaypointTest, SetAndGetSpeedLimit) {
    wp1->SetSpeedLimit(12.5);
    EXPECT_DOUBLE_EQ(wp1->GetSpeedLimit(), 12.5);
}

TEST_F(WaypointTest, SetAndGetAction) {
    wp1->SetAction(WaypointAction::Turn);
    EXPECT_EQ(wp1->GetAction(), WaypointAction::Turn);
    
    wp1->SetAction(WaypointAction::Stop);
    EXPECT_EQ(wp1->GetAction(), WaypointAction::Stop);
}

TEST_F(WaypointTest, SetAndGetSequence) {
    wp1->SetSequence(5);
    EXPECT_EQ(wp1->GetSequence(), 5);
}

TEST_F(WaypointTest, SetAndGetMandatory) {
    wp1->SetMandatory(true);
    EXPECT_TRUE(wp1->IsMandatory());
    
    wp1->SetMandatory(false);
    EXPECT_FALSE(wp1->IsMandatory());
}

TEST_F(WaypointTest, SetAndGetNotes) {
    wp1->SetNotes("Test waypoint");
    EXPECT_EQ(wp1->GetNotes(), "Test waypoint");
}

TEST_F(WaypointTest, GetPosition) {
    GeoPoint pos = wp1->GetPosition();
    EXPECT_DOUBLE_EQ(pos.longitude, 121.4737);
    EXPECT_DOUBLE_EQ(pos.latitude, 31.2304);
}

TEST_F(WaypointTest, SetPosition) {
    wp1->SetPosition(122.0, 32.0);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), 122.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), 32.0);
}

TEST_F(WaypointTest, ToDataFromData) {
    wp1->SetId("wp-test");
    wp1->SetName("Test WP");
    wp1->SetArrivalRadius(0.5);
    wp1->SetSpeedLimit(15.0);
    wp1->SetAction(WaypointAction::Turn);
    wp1->SetSequence(1);
    wp1->SetMandatory(true);
    wp1->SetNotes("Test notes");
    
    WaypointData data = wp1->ToData();
    
    EXPECT_EQ(data.id, "wp-test");
    EXPECT_EQ(data.name, "Test WP");
    EXPECT_DOUBLE_EQ(data.longitude, 121.4737);
    EXPECT_DOUBLE_EQ(data.latitude, 31.2304);
    EXPECT_DOUBLE_EQ(data.arrival_radius, 0.5);
    EXPECT_DOUBLE_EQ(data.speed_limit, 15.0);
    EXPECT_EQ(data.action, WaypointAction::Turn);
    EXPECT_EQ(data.sequence, 1);
    EXPECT_TRUE(data.is_mandatory);
    EXPECT_EQ(data.notes, "Test notes");
    
    Waypoint* wp3 = Waypoint::Create(data);
    EXPECT_EQ(wp3->GetId(), "wp-test");
    EXPECT_EQ(wp3->GetName(), "Test WP");
    EXPECT_DOUBLE_EQ(wp3->GetLongitude(), 121.4737);
    EXPECT_DOUBLE_EQ(wp3->GetLatitude(), 31.2304);
    
    wp3->ReleaseReference();
}

TEST_F(WaypointTest, CreateWithData) {
    WaypointData data;
    data.id = "wp-123";
    data.name = "My Waypoint";
    data.longitude = 120.0;
    data.latitude = 30.0;
    data.arrival_radius = 1.0;
    data.speed_limit = 20.0;
    data.action = WaypointAction::Stop;
    data.sequence = 2;
    data.is_mandatory = false;
    data.notes = "Custom notes";
    
    Waypoint* wp = Waypoint::Create(data);
    
    EXPECT_EQ(wp->GetId(), "wp-123");
    EXPECT_EQ(wp->GetName(), "My Waypoint");
    EXPECT_DOUBLE_EQ(wp->GetLongitude(), 120.0);
    EXPECT_DOUBLE_EQ(wp->GetLatitude(), 30.0);
    EXPECT_DOUBLE_EQ(wp->GetArrivalRadius(), 1.0);
    EXPECT_DOUBLE_EQ(wp->GetSpeedLimit(), 20.0);
    EXPECT_EQ(wp->GetAction(), WaypointAction::Stop);
    EXPECT_EQ(wp->GetSequence(), 2);
    EXPECT_FALSE(wp->IsMandatory());
    EXPECT_EQ(wp->GetNotes(), "Custom notes");
    
    wp->ReleaseReference();
}

TEST_F(WaypointTest, SetLatitude_MaxBoundary) {
    wp1->SetLatitude(90.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), 90.0);
}

TEST_F(WaypointTest, SetLatitude_MinBoundary) {
    wp1->SetLatitude(-90.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), -90.0);
}

TEST_F(WaypointTest, SetLongitude_MaxBoundary) {
    wp1->SetLongitude(180.0);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), 180.0);
}

TEST_F(WaypointTest, SetLongitude_MinBoundary) {
    wp1->SetLongitude(-180.0);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), -180.0);
}

TEST_F(WaypointTest, SetArrivalRadius_Zero) {
    wp1->SetArrivalRadius(0.0);
    EXPECT_DOUBLE_EQ(wp1->GetArrivalRadius(), 0.0);
}

TEST_F(WaypointTest, SetArrivalRadius_Negative) {
    wp1->SetArrivalRadius(-1.0);
    EXPECT_DOUBLE_EQ(wp1->GetArrivalRadius(), -1.0);
}

TEST_F(WaypointTest, SetSpeedLimit_Zero) {
    wp1->SetSpeedLimit(0.0);
    EXPECT_DOUBLE_EQ(wp1->GetSpeedLimit(), 0.0);
}

TEST_F(WaypointTest, SetSpeedLimit_Negative) {
    wp1->SetSpeedLimit(-10.0);
    EXPECT_DOUBLE_EQ(wp1->GetSpeedLimit(), -10.0);
}

TEST_F(WaypointTest, SetName_Empty) {
    wp1->SetName("");
    EXPECT_EQ(wp1->GetName(), "");
}

TEST_F(WaypointTest, SetId_Empty) {
    wp1->SetId("");
    EXPECT_EQ(wp1->GetId(), "");
}

TEST_F(WaypointTest, SetNotes_Empty) {
    wp1->SetNotes("");
    EXPECT_EQ(wp1->GetNotes(), "");
}

TEST_F(WaypointTest, SetSequence_Zero) {
    wp1->SetSequence(0);
    EXPECT_EQ(wp1->GetSequence(), 0);
}

TEST_F(WaypointTest, SetSequence_Negative) {
    wp1->SetSequence(-1);
    EXPECT_EQ(wp1->GetSequence(), -1);
}

TEST_F(WaypointTest, CreateWithEmptyData) {
    WaypointData data;
    Waypoint* wp = Waypoint::Create(data);
    
    EXPECT_EQ(wp->GetId(), "");
    EXPECT_EQ(wp->GetName(), "");
    EXPECT_DOUBLE_EQ(wp->GetLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(wp->GetLatitude(), 0.0);
    
    wp->ReleaseReference();
}

TEST_F(WaypointTest, SetPosition_Equator) {
    wp1->SetPosition(0.0, 0.0);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), 0.0);
}

TEST_F(WaypointTest, SetPosition_PrimeMeridian) {
    wp1->SetPosition(0.0, 51.5);
    EXPECT_DOUBLE_EQ(wp1->GetLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(wp1->GetLatitude(), 51.5);
}
