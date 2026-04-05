#include <gtest/gtest.h>
#include "ogc/navi/track/track.h"
#include "ogc/navi/track/track_point.h"

using namespace ogc::navi;

class TrackTest : public ::testing::Test {
protected:
    void SetUp() override {
        track = Track::Create();
        track->SetName("TestTrack");
        
        tp1 = TrackPoint::Create();
        tp1->SetPosition(121.4737, 31.2304);
        tp1->SetTimestamp(1000.0);
        tp1->SetSpeed(10.0);
        
        tp2 = TrackPoint::Create();
        tp2->SetPosition(121.4837, 31.2404);
        tp2->SetTimestamp(1010.0);
        tp2->SetSpeed(12.0);
        
        tp3 = TrackPoint::Create();
        tp3->SetPosition(121.4937, 31.2504);
        tp3->SetTimestamp(1020.0);
        tp3->SetSpeed(11.0);
    }
    
    void TearDown() override {
        if (track) {
            track->ReleaseReference();
        }
        if (tp1) tp1->ReleaseReference();
        if (tp2) tp2->ReleaseReference();
        if (tp3) tp3->ReleaseReference();
    }
    
    Track* track = nullptr;
    TrackPoint* tp1 = nullptr;
    TrackPoint* tp2 = nullptr;
    TrackPoint* tp3 = nullptr;
};

TEST_F(TrackTest, Create) {
    EXPECT_EQ(track->GetPointCount(), 0);
    EXPECT_DOUBLE_EQ(track->GetTotalDistance(), 0.0);
}

TEST_F(TrackTest, SetAndGetName) {
    track->SetName("NewName");
    EXPECT_EQ(track->GetName(), "NewName");
}

TEST_F(TrackTest, SetAndGetDescription) {
    track->SetDescription("Test description");
    EXPECT_EQ(track->GetDescription(), "Test description");
}

TEST_F(TrackTest, SetAndGetId) {
    track->SetId("track-001");
    EXPECT_EQ(track->GetId(), "track-001");
}

TEST_F(TrackTest, SetAndGetType) {
    track->SetType(TrackType::Historical);
    EXPECT_EQ(track->GetType(), TrackType::Historical);
}

TEST_F(TrackTest, AddPoint) {
    track->AddPoint(tp1);
    tp1 = nullptr;  // Ownership transferred
    EXPECT_EQ(track->GetPointCount(), 1);
}

TEST_F(TrackTest, AddMultiplePoints) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    EXPECT_EQ(track->GetPointCount(), 3);
}

TEST_F(TrackTest, InsertPoint) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    TrackPoint* newPoint = TrackPoint::Create();
    newPoint->SetPosition(121.4837, 31.2404);
    newPoint->SetTimestamp(1010.0);
    track->InsertPoint(1, newPoint);
    
    EXPECT_EQ(track->GetPointCount(), 3);
    EXPECT_DOUBLE_EQ(track->GetPoint(1)->GetTimestamp(), 1010.0);
}

TEST_F(TrackTest, RemovePoint) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    track->RemovePoint(1);
    
    EXPECT_EQ(track->GetPointCount(), 2);
    EXPECT_DOUBLE_EQ(track->GetPoint(1)->GetTimestamp(), 1020.0);
}

TEST_F(TrackTest, ClearPoints) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    
    track->ClearPoints();
    
    EXPECT_EQ(track->GetPointCount(), 0);
}

TEST_F(TrackTest, GetTotalDistance) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    double distance = track->GetTotalDistance();
    EXPECT_GT(distance, 0.0);
}

TEST_F(TrackTest, GetTotalDuration) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    double duration = track->GetTotalDuration();
    EXPECT_DOUBLE_EQ(duration, 20.0);
}

TEST_F(TrackTest, GetStartTime) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    
    EXPECT_DOUBLE_EQ(track->GetStartTime(), 1000.0);
}

TEST_F(TrackTest, GetEndTime) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    
    EXPECT_DOUBLE_EQ(track->GetEndTime(), 1010.0);
}

TEST_F(TrackTest, GetPointsInTimeRange) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    std::vector<TrackPoint*> points = track->GetPointsInTimeRange(1005.0, 1015.0);
    EXPECT_EQ(points.size(), 1u);
}

TEST_F(TrackTest, Simplify) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    track->AddPoint(tp2);
    tp2 = nullptr;
    track->AddPoint(tp3);
    tp3 = nullptr;
    
    int before = track->GetPointCount();
    track->Simplify(10.0);  // 10 meter tolerance
    int after = track->GetPointCount();
    
    EXPECT_LE(after, before);
}

TEST_F(TrackTest, ToDataFromData) {
    track->SetId("test-id");
    track->SetName("Test Track");
    track->SetDescription("Test Description");
    track->SetType(TrackType::RealTime);
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    TrackData data = track->ToData();
    
    EXPECT_EQ(data.id, "test-id");
    EXPECT_EQ(data.name, "Test Track");
    EXPECT_EQ(data.description, "Test Description");
    EXPECT_EQ(data.type, TrackType::RealTime);
    EXPECT_EQ(data.point_count, 1);
    
    Track* track2 = Track::Create(data);
    EXPECT_EQ(track2->GetId(), "test-id");
    EXPECT_EQ(track2->GetName(), "Test Track");
    EXPECT_EQ(track2->GetPointCount(), 1);
    
    track2->ReleaseReference();
}

TEST_F(TrackTest, EmptyTrack) {
    EXPECT_EQ(track->GetPointCount(), 0);
    EXPECT_DOUBLE_EQ(track->GetTotalDistance(), 0.0);
    EXPECT_DOUBLE_EQ(track->GetTotalDuration(), 0.0);
}

TEST_F(TrackTest, SinglePoint) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    EXPECT_EQ(track->GetPointCount(), 1);
    EXPECT_DOUBLE_EQ(track->GetTotalDistance(), 0.0);
    EXPECT_DOUBLE_EQ(track->GetTotalDuration(), 0.0);
}

TEST_F(TrackTest, GetPoint_EmptyTrack_ReturnsNull) {
    TrackPoint* tp = track->GetPoint(0);
    EXPECT_EQ(tp, nullptr);
}

TEST_F(TrackTest, GetPoint_IndexOutOfRange_ReturnsNull) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    TrackPoint* tp = track->GetPoint(10);
    EXPECT_EQ(tp, nullptr);
}

TEST_F(TrackTest, GetPoint_NegativeIndex_ReturnsNull) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    TrackPoint* tp = track->GetPoint(-1);
    EXPECT_EQ(tp, nullptr);
}

TEST_F(TrackTest, RemovePoint_EmptyTrack_HandlesGracefully) {
    EXPECT_NO_THROW(track->RemovePoint(0));
}

TEST_F(TrackTest, RemovePoint_IndexOutOfRange_HandlesGracefully) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    EXPECT_NO_THROW(track->RemovePoint(10));
}

TEST_F(TrackTest, InsertPoint_EmptyTrack_InsertsAtZero) {
    track->InsertPoint(0, tp1);
    tp1 = nullptr;
    
    EXPECT_EQ(track->GetPointCount(), 1);
}

TEST_F(TrackTest, InsertPoint_IndexOutOfRange_DoesNothing) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    track->InsertPoint(10, tp2);
    tp2 = nullptr;
    
    EXPECT_EQ(track->GetPointCount(), 1);
}

TEST_F(TrackTest, GetStartTime_EmptyTrack_ReturnsZero) {
    double time = track->GetStartTime();
    EXPECT_DOUBLE_EQ(time, 0.0);
}

TEST_F(TrackTest, GetEndTime_EmptyTrack_ReturnsZero) {
    double time = track->GetEndTime();
    EXPECT_DOUBLE_EQ(time, 0.0);
}

TEST_F(TrackTest, GetTotalDuration_EmptyTrack_ReturnsZero) {
    double duration = track->GetTotalDuration();
    EXPECT_DOUBLE_EQ(duration, 0.0);
}

TEST_F(TrackTest, GetTotalDuration_SinglePoint_ReturnsZero) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    double duration = track->GetTotalDuration();
    EXPECT_DOUBLE_EQ(duration, 0.0);
}

TEST_F(TrackTest, GetPointsInTimeRange_EmptyTrack_ReturnsEmpty) {
    std::vector<TrackPoint*> points = track->GetPointsInTimeRange(1000.0, 2000.0);
    EXPECT_EQ(points.size(), 0u);
}

TEST_F(TrackTest, GetPointsInTimeRange_NoMatch_ReturnsEmpty) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    std::vector<TrackPoint*> points = track->GetPointsInTimeRange(5000.0, 6000.0);
    EXPECT_EQ(points.size(), 0u);
}

TEST_F(TrackTest, Simplify_EmptyTrack_HandlesGracefully) {
    EXPECT_NO_THROW(track->Simplify(10.0));
}

TEST_F(TrackTest, Simplify_SinglePoint_HandlesGracefully) {
    track->AddPoint(tp1);
    tp1 = nullptr;
    
    EXPECT_NO_THROW(track->Simplify(10.0));
    EXPECT_EQ(track->GetPointCount(), 1);
}
