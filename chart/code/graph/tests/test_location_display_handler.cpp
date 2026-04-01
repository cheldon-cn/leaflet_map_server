#include <gtest/gtest.h>
#include "ogc/draw/location_display_handler.h"
#include "ogc/draw/interaction_feedback.h"
#include "ogc/coordinate.h"

using namespace ogc::draw;
using ogc::Coordinate;

class LocationDisplayHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = LocationDisplayHandler::Create("test_location");
        handler->SetViewportSize(800, 600);
    }
    
    void TearDown() override {
        handler.reset();
    }
    
    std::unique_ptr<LocationDisplayHandler> handler;
};

TEST_F(LocationDisplayHandlerTest, CreateHandler) {
    EXPECT_TRUE(handler != nullptr);
    EXPECT_EQ(handler->GetName(), "test_location");
    EXPECT_TRUE(handler->IsEnabled());
}

TEST_F(LocationDisplayHandlerTest, SetPriority) {
    handler->SetPriority(50);
    EXPECT_EQ(handler->GetPriority(), 50);
}

TEST_F(LocationDisplayHandlerTest, EnableDisable) {
    handler->SetEnabled(true);
    EXPECT_TRUE(handler->IsEnabled());
    
    handler->SetEnabled(false);
    EXPECT_FALSE(handler->IsEnabled());
}

TEST_F(LocationDisplayHandlerTest, SetPosition) {
    handler->SetPosition(39.9, 116.4);
    
    EXPECT_TRUE(handler->HasValidPosition());
    const auto& pos = handler->GetPosition();
    EXPECT_DOUBLE_EQ(pos.latitude, 39.9);
    EXPECT_DOUBLE_EQ(pos.longitude, 116.4);
    EXPECT_EQ(pos.source, PositionSource::kManual);
    EXPECT_EQ(pos.status, PositionStatus::kManual);
}

TEST_F(LocationDisplayHandlerTest, SetPositionAndHeading) {
    handler->SetPositionAndHeading(39.9, 116.4, 45.0);
    
    EXPECT_TRUE(handler->HasValidPosition());
    EXPECT_TRUE(handler->HasValidHeading());
    
    const auto& pos = handler->GetPosition();
    EXPECT_DOUBLE_EQ(pos.latitude, 39.9);
    EXPECT_DOUBLE_EQ(pos.longitude, 116.4);
    
    const auto& heading = handler->GetHeading();
    EXPECT_DOUBLE_EQ(heading.degrees, 45.0);
    EXPECT_TRUE(heading.isTrue);
}

TEST_F(LocationDisplayHandlerTest, UpdatePosition) {
    Position pos;
    pos.latitude = 31.2;
    pos.longitude = 121.5;
    pos.altitude = 10.0;
    pos.accuracy = 5.0;
    pos.speed = 10.0;
    pos.bearing = 90.0;
    pos.source = PositionSource::kInternalGPS;
    pos.status = PositionStatus::k3DFix;
    pos.satelliteCount = 8;
    pos.hdop = 1.2;
    pos.vdop = 2.0;
    
    handler->UpdatePosition(pos);
    
    EXPECT_TRUE(handler->HasValidPosition());
    const auto& result = handler->GetPosition();
    EXPECT_DOUBLE_EQ(result.latitude, 31.2);
    EXPECT_DOUBLE_EQ(result.longitude, 121.5);
    EXPECT_DOUBLE_EQ(result.altitude, 10.0);
    EXPECT_DOUBLE_EQ(result.accuracy, 5.0);
    EXPECT_DOUBLE_EQ(result.speed, 10.0);
    EXPECT_DOUBLE_EQ(result.bearing, 90.0);
    EXPECT_EQ(result.source, PositionSource::kInternalGPS);
    EXPECT_EQ(result.status, PositionStatus::k3DFix);
    EXPECT_EQ(result.satelliteCount, 8);
}

TEST_F(LocationDisplayHandlerTest, UpdateHeading) {
    Heading heading;
    heading.degrees = 180.0;
    heading.magneticVariation = 5.0;
    heading.isTrue = true;
    heading.rateOfTurn = 2.0;
    
    handler->UpdateHeading(heading);
    
    EXPECT_TRUE(handler->HasValidHeading());
    const auto& result = handler->GetHeading();
    EXPECT_DOUBLE_EQ(result.degrees, 180.0);
    EXPECT_DOUBLE_EQ(result.magneticVariation, 5.0);
    EXPECT_TRUE(result.isTrue);
    EXPECT_DOUBLE_EQ(result.rateOfTurn, 2.0);
}

TEST_F(LocationDisplayHandlerTest, UpdateSpeed) {
    Speed speed;
    speed.knots = 15.0;
    speed.kmh = 27.78;
    speed.mph = 17.26;
    
    handler->UpdateSpeed(speed);
    
    EXPECT_TRUE(handler->HasValidSpeed());
    const auto& result = handler->GetSpeed();
    EXPECT_DOUBLE_EQ(result.knots, 15.0);
    EXPECT_DOUBLE_EQ(result.kmh, 27.78);
    EXPECT_DOUBLE_EQ(result.mph, 17.26);
}

TEST_F(LocationDisplayHandlerTest, UpdateLocation) {
    Position pos;
    pos.latitude = 30.0;
    pos.longitude = 120.0;
    
    Heading heading;
    heading.degrees = 90.0;
    
    Speed speed;
    speed.knots = 10.0;
    
    handler->UpdateLocation(pos, heading, speed);
    
    EXPECT_TRUE(handler->HasValidPosition());
    EXPECT_TRUE(handler->HasValidHeading());
    EXPECT_TRUE(handler->HasValidSpeed());
    
    LocationInfo info = handler->GetLocationInfo();
    EXPECT_TRUE(info.isValid);
    EXPECT_DOUBLE_EQ(info.position.latitude, 30.0);
    EXPECT_DOUBLE_EQ(info.heading.degrees, 90.0);
    EXPECT_DOUBLE_EQ(info.speed.knots, 10.0);
}

TEST_F(LocationDisplayHandlerTest, SetMapCoordinate) {
    Coordinate coord(116.4, 39.9);
    handler->SetMapCoordinate(coord);
    
    Coordinate result = handler->GetMapCoordinate();
    EXPECT_DOUBLE_EQ(result.x, 116.4);
    EXPECT_DOUBLE_EQ(result.y, 39.9);
}

TEST_F(LocationDisplayHandlerTest, SetLocationStyle) {
    LocationStyle style;
    style.vesselColor = 0xFFFF0000;
    style.headingColor = 0xFF0000FF;
    style.vesselSize = 30.0;
    style.headingLength = 60.0;
    style.showAccuracyCircle = false;
    
    handler->SetLocationStyle(style);
    
    auto result = handler->GetLocationStyle();
    EXPECT_EQ(result.vesselColor, 0xFFFF0000u);
    EXPECT_EQ(result.headingColor, 0xFF0000FFu);
    EXPECT_DOUBLE_EQ(result.vesselSize, 30.0);
    EXPECT_DOUBLE_EQ(result.headingLength, 60.0);
    EXPECT_FALSE(result.showAccuracyCircle);
}

TEST_F(LocationDisplayHandlerTest, SetLocationConfig) {
    LocationConfig config;
    config.autoCenter = true;
    config.autoRotate = true;
    config.smoothPosition = false;
    config.trackInterval = 2.0;
    
    handler->SetLocationConfig(config);
    
    auto result = handler->GetLocationConfig();
    EXPECT_TRUE(result.autoCenter);
    EXPECT_TRUE(result.autoRotate);
    EXPECT_FALSE(result.smoothPosition);
    EXPECT_DOUBLE_EQ(result.trackInterval, 2.0);
}

TEST_F(LocationDisplayHandlerTest, AutoCenter) {
    handler->SetAutoCenter(true);
    EXPECT_TRUE(handler->IsAutoCenter());
    
    handler->SetAutoCenter(false);
    EXPECT_FALSE(handler->IsAutoCenter());
}

TEST_F(LocationDisplayHandlerTest, AutoRotate) {
    handler->SetAutoRotate(true);
    EXPECT_TRUE(handler->IsAutoRotate());
    
    handler->SetAutoRotate(false);
    EXPECT_FALSE(handler->IsAutoRotate());
}

TEST_F(LocationDisplayHandlerTest, SetViewportSize) {
    handler->SetViewportSize(1024, 768);
}

TEST_F(LocationDisplayHandlerTest, SetFeedbackManager) {
    FeedbackManager* manager = nullptr;
    handler->SetFeedbackManager(manager);
    EXPECT_EQ(handler->GetFeedbackManager(), nullptr);
}

TEST_F(LocationDisplayHandlerTest, CustomTransforms) {
    handler->SetScreenToWorldTransform([](double x, double y) {
        return Coordinate(x * 2.0, y * 2.0);
    });
    
    handler->SetWorldToScreenTransform([](double x, double y) {
        return Coordinate(x / 2.0, y / 2.0);
    });
    
    Coordinate world = handler->ScreenToWorld(100, 200);
    EXPECT_DOUBLE_EQ(world.x, 200.0);
    EXPECT_DOUBLE_EQ(world.y, 400.0);
    
    Coordinate screen = handler->WorldToScreen(200, 400);
    EXPECT_DOUBLE_EQ(screen.x, 100.0);
    EXPECT_DOUBLE_EQ(screen.y, 200.0);
}

TEST_F(LocationDisplayHandlerTest, Callbacks) {
    bool positionChanged = false;
    bool headingChanged = false;
    bool speedChanged = false;
    bool locationUpdated = false;
    
    handler->SetPositionChangedCallback([&positionChanged](const Position&) {
        positionChanged = true;
    });
    
    handler->SetHeadingChangedCallback([&headingChanged](const Heading&) {
        headingChanged = true;
    });
    
    handler->SetSpeedChangedCallback([&speedChanged](const Speed&) {
        speedChanged = true;
    });
    
    handler->SetLocationUpdatedCallback([&locationUpdated](const LocationInfo&) {
        locationUpdated = true;
    });
    
    handler->SetPosition(39.9, 116.4);
    EXPECT_TRUE(positionChanged);
    EXPECT_TRUE(locationUpdated);
    
    Heading heading;
    heading.degrees = 90.0;
    handler->UpdateHeading(heading);
    EXPECT_TRUE(headingChanged);
    
    Speed speed;
    speed.knots = 10.0;
    handler->UpdateSpeed(speed);
    EXPECT_TRUE(speedChanged);
}

TEST_F(LocationDisplayHandlerTest, TrackRecording) {
    EXPECT_FALSE(handler->IsRecordingTrack());
    
    handler->StartTrackRecording();
    EXPECT_TRUE(handler->IsRecordingTrack());
    
    handler->StopTrackRecording();
    EXPECT_FALSE(handler->IsRecordingTrack());
}

TEST_F(LocationDisplayHandlerTest, ClearTrack) {
    handler->SetPosition(39.9, 116.4);
    handler->ClearTrack();
    EXPECT_EQ(handler->GetTrackPointCount(), 0u);
}

TEST_F(LocationDisplayHandlerTest, ShowLocation) {
    handler->ShowLocation(true);
    EXPECT_TRUE(handler->IsLocationVisible());
    
    handler->ShowLocation(false);
    EXPECT_FALSE(handler->IsLocationVisible());
}

TEST_F(LocationDisplayHandlerTest, DistanceToPositionWithoutPosition) {
    double dist = handler->DistanceToPosition(100, 200);
    EXPECT_DOUBLE_EQ(dist, -1.0);
}

TEST_F(LocationDisplayHandlerTest, DistanceToPositionWithPosition) {
    handler->SetMapCoordinate(Coordinate(500, 500));
    handler->SetPosition(39.9, 116.4);
    
    handler->SetScreenToWorldTransform([](double x, double y) {
        return Coordinate(x, y);
    });
    
    double dist = handler->DistanceToPosition(600, 600);
    double expected = std::sqrt(100.0 * 100.0 + 100.0 * 100.0);
    EXPECT_NEAR(dist, expected, 0.001);
}

TEST_F(LocationDisplayHandlerTest, IsNearPosition) {
    handler->SetMapCoordinate(Coordinate(500, 500));
    handler->SetPosition(39.9, 116.4);
    
    handler->SetScreenToWorldTransform([](double x, double y) {
        return Coordinate(x, y);
    });
    
    EXPECT_TRUE(handler->IsNearPosition(510, 510, 20.0));
    EXPECT_FALSE(handler->IsNearPosition(600, 600, 10.0));
}

TEST_F(LocationDisplayHandlerTest, DisabledHandlerIgnoresEvents) {
    handler->SetEnabled(false);
    
    InteractionEvent event;
    event.type = InteractionEventType::kKeyDown;
    event.key = 'C';
    event.isCtrl = true;
    
    bool result = handler->HandleEvent(event);
    EXPECT_FALSE(result);
}

TEST_F(LocationDisplayHandlerTest, GetState) {
    EXPECT_EQ(handler->GetState(), InteractionState::kNone);
}

TEST_F(LocationDisplayHandlerTest, LocationStyleDefaults) {
    LocationStyle style;
    EXPECT_EQ(style.vesselColor, 0xFFFF0000u);
    EXPECT_EQ(style.headingColor, 0xFF0000FFu);
    EXPECT_EQ(style.trackColor, 0xFF00FF00u);
    EXPECT_DOUBLE_EQ(style.vesselSize, 20.0);
    EXPECT_DOUBLE_EQ(style.headingLength, 50.0);
    EXPECT_TRUE(style.showAccuracyCircle);
    EXPECT_TRUE(style.showHeadingLine);
    EXPECT_TRUE(style.showVesselSymbol);
}

TEST_F(LocationDisplayHandlerTest, LocationConfigDefaults) {
    LocationConfig config;
    EXPECT_FALSE(config.autoCenter);
    EXPECT_FALSE(config.autoRotate);
    EXPECT_TRUE(config.smoothPosition);
    EXPECT_DOUBLE_EQ(config.smoothFactor, 0.3);
    EXPECT_FALSE(config.recordTrack);
    EXPECT_DOUBLE_EQ(config.trackInterval, 1.0);
    EXPECT_EQ(config.maxTrackPoints, 10000);
}

TEST_F(LocationDisplayHandlerTest, PositionDefaults) {
    Position pos;
    EXPECT_DOUBLE_EQ(pos.latitude, 0.0);
    EXPECT_DOUBLE_EQ(pos.longitude, 0.0);
    EXPECT_DOUBLE_EQ(pos.altitude, 0.0);
    EXPECT_DOUBLE_EQ(pos.accuracy, 0.0);
    EXPECT_DOUBLE_EQ(pos.speed, 0.0);
    EXPECT_DOUBLE_EQ(pos.bearing, 0.0);
    EXPECT_EQ(pos.source, PositionSource::kNone);
    EXPECT_EQ(pos.status, PositionStatus::kNoFix);
    EXPECT_EQ(pos.satelliteCount, 0);
}

TEST_F(LocationDisplayHandlerTest, HeadingDefaults) {
    Heading heading;
    EXPECT_DOUBLE_EQ(heading.degrees, 0.0);
    EXPECT_DOUBLE_EQ(heading.magneticVariation, 0.0);
    EXPECT_TRUE(heading.isTrue);
    EXPECT_DOUBLE_EQ(heading.rateOfTurn, 0.0);
}

TEST_F(LocationDisplayHandlerTest, SpeedDefaults) {
    Speed speed;
    EXPECT_DOUBLE_EQ(speed.knots, 0.0);
    EXPECT_DOUBLE_EQ(speed.kmh, 0.0);
    EXPECT_DOUBLE_EQ(speed.mph, 0.0);
}

TEST_F(LocationDisplayHandlerTest, PositionSources) {
    Position pos;
    
    pos.source = PositionSource::kInternalGPS;
    EXPECT_EQ(pos.source, PositionSource::kInternalGPS);
    
    pos.source = PositionSource::kExternalGPS;
    EXPECT_EQ(pos.source, PositionSource::kExternalGPS);
    
    pos.source = PositionSource::kBeidou;
    EXPECT_EQ(pos.source, PositionSource::kBeidou);
    
    pos.source = PositionSource::kRTK;
    EXPECT_EQ(pos.source, PositionSource::kRTK);
    
    pos.source = PositionSource::kManual;
    EXPECT_EQ(pos.source, PositionSource::kManual);
}

TEST_F(LocationDisplayHandlerTest, PositionStatuses) {
    Position pos;
    
    pos.status = PositionStatus::kNoFix;
    EXPECT_EQ(pos.status, PositionStatus::kNoFix);
    
    pos.status = PositionStatus::k2DFix;
    EXPECT_EQ(pos.status, PositionStatus::k2DFix);
    
    pos.status = PositionStatus::k3DFix;
    EXPECT_EQ(pos.status, PositionStatus::k3DFix);
    
    pos.status = PositionStatus::kDifferential;
    EXPECT_EQ(pos.status, PositionStatus::kDifferential);
    
    pos.status = PositionStatus::kRTK;
    EXPECT_EQ(pos.status, PositionStatus::kRTK);
    
    pos.status = PositionStatus::kFloatRTK;
    EXPECT_EQ(pos.status, PositionStatus::kFloatRTK);
    
    pos.status = PositionStatus::kEstimated;
    EXPECT_EQ(pos.status, PositionStatus::kEstimated);
    
    pos.status = PositionStatus::kManual;
    EXPECT_EQ(pos.status, PositionStatus::kManual);
}

TEST_F(LocationDisplayHandlerTest, CenterCallback) {
    double centerX = 0.0, centerY = 0.0;
    
    handler->SetCenterCallback([&centerX, &centerY](double x, double y) {
        centerX = x;
        centerY = y;
    });
    
    handler->SetAutoCenter(false);
    handler->SetPosition(39.9, 116.4);
    handler->CenterOnPosition();
    
    EXPECT_DOUBLE_EQ(centerX, 116.4);
    EXPECT_DOUBLE_EQ(centerY, 39.9);
}

TEST_F(LocationDisplayHandlerTest, RotateCallback) {
    double rotation = 0.0;
    
    handler->SetRotateCallback([&rotation](double deg) {
        rotation = deg;
    });
    
    handler->SetAutoRotate(true);
    handler->SetPosition(39.9, 116.4);
    
    Heading heading;
    heading.degrees = 45.0;
    handler->UpdateHeading(heading);
    
    EXPECT_DOUBLE_EQ(rotation, 45.0);
}
