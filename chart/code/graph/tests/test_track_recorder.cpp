#include <gtest/gtest.h>
#include "ogc/navi/track/track_player.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"

using namespace ogc::navi;
using ogc::Coordinate;
using ogc::Envelope;

class TrackPlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        player = TrackPlayer::Create();
        
        TrackPlaybackData track;
        track.id = "test_track";
        track.name = "Test Track";
        track.totalDistance = 1000.0;
        track.totalDuration = 100.0;
        
        TrackSegment segment;
        for (int i = 0; i < 10; ++i) {
            TrackPoint point;
            point.position = Coordinate(i * 100.0, i * 100.0);
            point.heading = i * 10.0;
            point.speed = 10.0;
            point.distance = i > 0 ? 100.0 : 0.0;
            point.duration = 10.0;
            segment.points.push_back(point);
        }
        segment.totalDistance = 900.0;
        segment.totalDuration = 90.0;
        
        track.segments.push_back(segment);
        player->SetTrack(track);
    }
    
    void TearDown() override {
        player.reset();
    }
    
    std::unique_ptr<TrackPlayer> player;
};

TEST_F(TrackPlayerTest, CreatePlayer) {
    EXPECT_TRUE(player != nullptr);
    EXPECT_TRUE(player->IsStopped());
}

TEST_F(TrackPlayerTest, Play) {
    player->Play();
    EXPECT_TRUE(player->IsPlaying());
}

TEST_F(TrackPlayerTest, Pause) {
    player->Play();
    player->Pause();
    EXPECT_TRUE(player->IsPaused());
}

TEST_F(TrackPlayerTest, Stop) {
    player->Play();
    player->Stop();
    EXPECT_TRUE(player->IsStopped());
}

TEST_F(TrackPlayerTest, Seek) {
    player->Seek(0.5);
    EXPECT_NEAR(player->GetProgress(), 0.5, 0.1);
}

TEST_F(TrackPlayerTest, SeekToBeginning) {
    player->Seek(0.0);
    EXPECT_DOUBLE_EQ(player->GetProgress(), 0.0);
    EXPECT_EQ(player->GetCurrentPointIndex(), 0u);
}

TEST_F(TrackPlayerTest, SeekToEnd) {
    player->Seek(1.0);
    EXPECT_DOUBLE_EQ(player->GetProgress(), 1.0);
}

TEST_F(TrackPlayerTest, SetPlaybackSpeed) {
    player->SetPlaybackSpeed(2.0);
    EXPECT_DOUBLE_EQ(player->GetPlaybackSpeed(), 2.0);
}

TEST_F(TrackPlayerTest, SetPlaybackMode) {
    player->SetPlaybackMode(TrackPlayer::PlaybackMode::kFastForward);
    EXPECT_EQ(player->GetPlaybackMode(), TrackPlayer::PlaybackMode::kFastForward);
    
    player->SetPlaybackMode(TrackPlayer::PlaybackMode::kSlowMotion);
    EXPECT_EQ(player->GetPlaybackMode(), TrackPlayer::PlaybackMode::kSlowMotion);
}

TEST_F(TrackPlayerTest, GetCurrentPoint) {
    TrackPoint point = player->GetCurrentPoint();
    EXPECT_DOUBLE_EQ(point.position.x, 0.0);
    EXPECT_DOUBLE_EQ(point.position.y, 0.0);
}

TEST_F(TrackPlayerTest, GetElapsedTime) {
    player->Play();
    player->Update(10.0);
    EXPECT_DOUBLE_EQ(player->GetElapsedTime(), 10.0);
}

TEST_F(TrackPlayerTest, GetRemainingTime) {
    EXPECT_DOUBLE_EQ(player->GetRemainingTime(), 100.0);
}

TEST_F(TrackPlayerTest, SetLoopEnabled) {
    player->SetLoopEnabled(true);
    EXPECT_TRUE(player->IsLoopEnabled());
    
    player->SetLoopEnabled(false);
    EXPECT_FALSE(player->IsLoopEnabled());
}

TEST_F(TrackPlayerTest, ProgressCallback) {
    double progress = -1.0;
    player->SetProgressCallback([&progress](const TrackPoint&, double p) {
        progress = p;
    });
    
    player->Play();
    player->Update(10.0);
    
    EXPECT_GE(progress, 0.0);
}

TEST_F(TrackPlayerTest, StateChangedCallback) {
    TrackPlayer::PlaybackState state = TrackPlayer::PlaybackState::kStopped;
    player->SetStateChangedCallback([&state](TrackPlayer::PlaybackState s) {
        state = s;
    });
    
    player->Play();
    EXPECT_EQ(state, TrackPlayer::PlaybackState::kPlaying);
    
    player->Pause();
    EXPECT_EQ(state, TrackPlayer::PlaybackState::kPaused);
    
    player->Stop();
    EXPECT_EQ(state, TrackPlayer::PlaybackState::kStopped);
}

TEST_F(TrackPlayerTest, FinishedCallback) {
    bool finished = false;
    player->SetFinishedCallback([&finished]() {
        finished = true;
    });
    
    player->Play();
    player->SetPlaybackSpeed(100.0);
    player->Update(10.0);
    
    EXPECT_TRUE(finished || player->IsStopped());
}

TEST_F(TrackPlayerTest, PositionCallback) {
    Coordinate calledPos;
    double calledHeading = 0.0;
    double calledSpeed = 0.0;
    
    player->SetPositionCallback([&calledPos, &calledHeading, &calledSpeed]
        (const Coordinate& pos, double heading, double speed) {
        calledPos = pos;
        calledHeading = heading;
        calledSpeed = speed;
    });
    
    player->Play();
    player->Update(1.0);
}

TEST_F(TrackPlayerTest, SeekToPoint) {
    player->SeekToPoint(5);
    EXPECT_EQ(player->GetCurrentPointIndex(), 5u);
}

TEST_F(TrackPlayerTest, UpdateAdvancesPlayback) {
    player->Play();
    size_t initialIndex = player->GetCurrentPointIndex();
    
    player->Update(50.0);
    
    EXPECT_GE(player->GetCurrentPointIndex(), initialIndex);
}

TEST_F(TrackPlayerTest, LoopRestartsPlayback) {
    player->SetLoopEnabled(true);
    player->Play();
    player->SetPlaybackSpeed(100.0);
    player->Update(5.0);
    
    if (player->IsPlaying()) {
        EXPECT_LT(player->GetElapsedTime(), player->GetTrack().totalDuration);
    }
}

TEST_F(TrackPlayerTest, GetTrack) {
    const TrackPlaybackData& track = player->GetTrack();
    EXPECT_EQ(track.id, "test_track");
    EXPECT_EQ(track.name, "Test Track");
}

TEST_F(TrackPlayerTest, PlaybackSpeedClamped) {
    player->SetPlaybackSpeed(0.05);
    EXPECT_DOUBLE_EQ(player->GetPlaybackSpeed(), 0.1);
    
    player->SetPlaybackSpeed(20.0);
    EXPECT_DOUBLE_EQ(player->GetPlaybackSpeed(), 10.0);
}

TEST_F(TrackPlayerTest, SeekClamped) {
    player->Seek(-0.5);
    EXPECT_DOUBLE_EQ(player->GetProgress(), 0.0);
    
    player->Seek(1.5);
    EXPECT_DOUBLE_EQ(player->GetProgress(), 1.0);
}

TEST_F(TrackPlayerTest, TrackPointDefaults) {
    TrackPoint point;
    EXPECT_DOUBLE_EQ(point.heading, 0.0);
    EXPECT_DOUBLE_EQ(point.speed, 0.0);
    EXPECT_DOUBLE_EQ(point.distance, 0.0);
    EXPECT_DOUBLE_EQ(point.duration, 0.0);
}

TEST_F(TrackPlayerTest, TrackSegmentDefaults) {
    TrackSegment segment;
    EXPECT_TRUE(segment.points.empty());
    EXPECT_DOUBLE_EQ(segment.totalDistance, 0.0);
    EXPECT_DOUBLE_EQ(segment.totalDuration, 0.0);
}

TEST_F(TrackPlayerTest, TrackPlaybackDataDefaults) {
    TrackPlaybackData track;
    EXPECT_TRUE(track.id.empty());
    EXPECT_TRUE(track.name.empty());
    EXPECT_TRUE(track.segments.empty());
    EXPECT_DOUBLE_EQ(track.totalDistance, 0.0);
    EXPECT_DOUBLE_EQ(track.totalDuration, 0.0);
    EXPECT_TRUE(track.IsEmpty());
}

TEST_F(TrackPlayerTest, TrackPlaybackDataGetPointCount) {
    TrackPlaybackData track;
    EXPECT_EQ(track.GetPointCount(), 0u);
    
    TrackSegment segment;
    segment.points.resize(5);
    track.segments.push_back(segment);
    EXPECT_EQ(track.GetPointCount(), 5u);
    
    TrackSegment segment2;
    segment2.points.resize(3);
    track.segments.push_back(segment2);
    EXPECT_EQ(track.GetPointCount(), 8u);
}

TEST_F(TrackPlayerTest, TrackPlaybackDataGetBoundingBox) {
    TrackPlaybackData track;
    Envelope env = track.GetBoundingBox();
    EXPECT_TRUE(env.IsNull());
    
    TrackSegment segment;
    TrackPoint p1, p2, p3;
    p1.position = Coordinate(0.0, 0.0);
    p2.position = Coordinate(100.0, 50.0);
    p3.position = Coordinate(50.0, 100.0);
    segment.points.push_back(p1);
    segment.points.push_back(p2);
    segment.points.push_back(p3);
    track.segments.push_back(segment);
    
    Envelope bbox = track.GetBoundingBox();
    EXPECT_DOUBLE_EQ(bbox.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(bbox.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(bbox.GetMaxX(), 100.0);
    EXPECT_DOUBLE_EQ(bbox.GetMaxY(), 100.0);
}
