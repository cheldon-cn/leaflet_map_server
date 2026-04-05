#include <gtest/gtest.h>
#include "ogc/navi/track/track_point.h"

using namespace ogc::navi;

class TrackPointTest : public ::testing::Test {
protected:
    void SetUp() override {
        tp1 = TrackPoint::Create();
        tp1->SetPosition(121.4737, 31.2304);
        tp1->SetTimestamp(1000.0);
        tp1->SetSpeed(10.0);
        tp1->SetHeading(45.0);
        tp1->SetCourse(45.0);
        
        tp2 = TrackPoint::Create();
        tp2->SetPosition(121.4837, 31.2404);
        tp2->SetTimestamp(1010.0);
        tp2->SetSpeed(10.0);
        tp2->SetHeading(45.0);
        tp2->SetCourse(45.0);
    }
    
    void TearDown() override {
        if (tp1) tp1->ReleaseReference();
        if (tp2) tp2->ReleaseReference();
    }
    
    TrackPoint* tp1 = nullptr;
    TrackPoint* tp2 = nullptr;
};

TEST_F(TrackPointTest, Create) {
    TrackPoint* tp = TrackPoint::Create();
    EXPECT_DOUBLE_EQ(tp->GetLatitude(), 0.0);
    EXPECT_DOUBLE_EQ(tp->GetLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(tp->GetTimestamp(), 0.0);
    tp->ReleaseReference();
}

TEST_F(TrackPointTest, SetAndGetLatitude) {
    tp1->SetLatitude(32.0);
    EXPECT_DOUBLE_EQ(tp1->GetLatitude(), 32.0);
}

TEST_F(TrackPointTest, SetAndGetLongitude) {
    tp1->SetLongitude(122.0);
    EXPECT_DOUBLE_EQ(tp1->GetLongitude(), 122.0);
}

TEST_F(TrackPointTest, SetAndGetTimestamp) {
    tp1->SetTimestamp(2000.0);
    EXPECT_DOUBLE_EQ(tp1->GetTimestamp(), 2000.0);
}

TEST_F(TrackPointTest, SetAndGetSpeed) {
    tp1->SetSpeed(15.0);
    EXPECT_DOUBLE_EQ(tp1->GetSpeed(), 15.0);
}

TEST_F(TrackPointTest, SetAndGetHeading) {
    tp1->SetHeading(90.0);
    EXPECT_DOUBLE_EQ(tp1->GetHeading(), 90.0);
}

TEST_F(TrackPointTest, SetAndGetCourse) {
    tp1->SetCourse(90.0);
    EXPECT_DOUBLE_EQ(tp1->GetCourse(), 90.0);
}

TEST_F(TrackPointTest, SetAndGetAltitude) {
    tp1->SetAltitude(20.0);
    EXPECT_DOUBLE_EQ(tp1->GetAltitude(), 20.0);
}

TEST_F(TrackPointTest, SetAndGetHdop) {
    tp1->SetHdop(2.0);
    EXPECT_DOUBLE_EQ(tp1->GetHdop(), 2.0);
}

TEST_F(TrackPointTest, SetAndGetSatelliteCount) {
    tp1->SetSatelliteCount(10);
    EXPECT_EQ(tp1->GetSatelliteCount(), 10);
}

TEST_F(TrackPointTest, SetAndGetQuality) {
    tp1->SetQuality(PositionQuality::DGps);
    EXPECT_EQ(tp1->GetQuality(), PositionQuality::DGps);
}

TEST_F(TrackPointTest, GetPosition) {
    GeoPoint pos = tp1->GetPosition();
    EXPECT_DOUBLE_EQ(pos.longitude, 121.4737);
    EXPECT_DOUBLE_EQ(pos.latitude, 31.2304);
}

TEST_F(TrackPointTest, SetPosition) {
    tp1->SetPosition(122.0, 32.0);
    EXPECT_DOUBLE_EQ(tp1->GetLongitude(), 122.0);
    EXPECT_DOUBLE_EQ(tp1->GetLatitude(), 32.0);
}

TEST_F(TrackPointTest, ToDataFromData) {
    tp1->SetQuality(PositionQuality::Gps);
    tp1->SetAltitude(10.0);
    tp1->SetHdop(1.0);
    tp1->SetSatelliteCount(8);
    
    TrackPointData data = tp1->ToData();
    
    EXPECT_DOUBLE_EQ(data.latitude, 31.2304);
    EXPECT_DOUBLE_EQ(data.longitude, 121.4737);
    EXPECT_DOUBLE_EQ(data.timestamp, 1000.0);
    EXPECT_DOUBLE_EQ(data.speed, 10.0);
    EXPECT_DOUBLE_EQ(data.heading, 45.0);
    EXPECT_DOUBLE_EQ(data.course, 45.0);
    EXPECT_DOUBLE_EQ(data.altitude, 10.0);
    EXPECT_DOUBLE_EQ(data.hdop, 1.0);
    EXPECT_EQ(data.satellite_count, 8);
    EXPECT_EQ(data.quality, PositionQuality::Gps);
    
    TrackPoint* tp3 = TrackPoint::Create(data);
    EXPECT_DOUBLE_EQ(tp3->GetLatitude(), 31.2304);
    EXPECT_DOUBLE_EQ(tp3->GetLongitude(), 121.4737);
    EXPECT_DOUBLE_EQ(tp3->GetTimestamp(), 1000.0);
    
    tp3->ReleaseReference();
}

TEST_F(TrackPointTest, CreateWithData) {
    TrackPointData data;
    data.longitude = 120.0;
    data.latitude = 30.0;
    data.timestamp = 2000.0;
    data.speed = 15.0;
    data.heading = 90.0;
    data.course = 90.0;
    data.altitude = 5.0;
    data.hdop = 0.9;
    data.satellite_count = 12;
    data.quality = PositionQuality::Rtk;
    
    TrackPoint* tp = TrackPoint::Create(data);
    
    EXPECT_DOUBLE_EQ(tp->GetLongitude(), 120.0);
    EXPECT_DOUBLE_EQ(tp->GetLatitude(), 30.0);
    EXPECT_DOUBLE_EQ(tp->GetTimestamp(), 2000.0);
    EXPECT_DOUBLE_EQ(tp->GetSpeed(), 15.0);
    EXPECT_DOUBLE_EQ(tp->GetHeading(), 90.0);
    EXPECT_DOUBLE_EQ(tp->GetCourse(), 90.0);
    EXPECT_DOUBLE_EQ(tp->GetAltitude(), 5.0);
    EXPECT_DOUBLE_EQ(tp->GetHdop(), 0.9);
    EXPECT_EQ(tp->GetSatelliteCount(), 12);
    EXPECT_EQ(tp->GetQuality(), PositionQuality::Rtk);
    
    tp->ReleaseReference();
}
