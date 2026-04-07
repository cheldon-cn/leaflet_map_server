#include <gtest/gtest.h>
#include "ogc/graph/interaction/pan_zoom_handler.h"
#include "ogc/envelope.h"

using namespace ogc::graph;
using ogc::Coordinate;
using ogc::Envelope;

class PanZoomHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = PanZoomHandler::Create("test_handler");
        handler->SetViewportSize(800, 600);
        handler->SetExtent(Envelope(0, 0, 1000, 1000));
    }
    
    void TearDown() override {
        handler.reset();
    }
    
    std::unique_ptr<PanZoomHandler> handler;
};

TEST_F(PanZoomHandlerTest, CreateHandler) {
    EXPECT_TRUE(handler != nullptr);
    EXPECT_EQ(handler->GetName(), "test_handler");
    EXPECT_TRUE(handler->IsEnabled());
}

TEST_F(PanZoomHandlerTest, SetPriority) {
    handler->SetPriority(50);
    EXPECT_EQ(handler->GetPriority(), 50);
}

TEST_F(PanZoomHandlerTest, EnableDisable) {
    handler->SetEnabled(true);
    EXPECT_TRUE(handler->IsEnabled());
    
    handler->SetEnabled(false);
    EXPECT_FALSE(handler->IsEnabled());
}

TEST_F(PanZoomHandlerTest, SetExtent) {
    Envelope extent(100, 100, 500, 500);
    handler->SetExtent(extent);
    
    Envelope result = handler->GetExtent();
    EXPECT_FALSE(result.IsNull());
}

TEST_F(PanZoomHandlerTest, SetCenter) {
    handler->SetCenter(500, 500);
    
    auto center = handler->GetCenter();
    EXPECT_DOUBLE_EQ(center.x, 500.0);
    EXPECT_DOUBLE_EQ(center.y, 500.0);
}

TEST_F(PanZoomHandlerTest, SetZoom) {
    handler->SetZoom(2.0);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), 2.0);
    
    handler->SetZoom(0.5);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), 0.5);
}

TEST_F(PanZoomHandlerTest, ZoomLimits) {
    handler->SetMinZoom(0.1);
    handler->SetMaxZoom(100.0);
    
    handler->SetZoom(0.01);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), 0.1);
    
    handler->SetZoom(200.0);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), 100.0);
}

TEST_F(PanZoomHandlerTest, SetRotation) {
    handler->SetRotation(45.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 45.0);
    
    handler->SetRotation(-90.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), -90.0);
}

TEST_F(PanZoomHandlerTest, RotationNormalization) {
    handler->SetRotation(450.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 90.0);
    
    handler->SetRotation(-450.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), -90.0);
}

TEST_F(PanZoomHandlerTest, ResetRotation) {
    handler->SetRotation(45.0);
    handler->ResetRotation();
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 0.0);
}

TEST_F(PanZoomHandlerTest, Pan) {
    double initialCenterX = handler->GetCenter().x;
    double initialCenterY = handler->GetCenter().y;
    
    handler->Pan(100, 50);
    
    auto newCenter = handler->GetCenter();
    EXPECT_NE(newCenter.x, initialCenterX);
    EXPECT_NE(newCenter.y, initialCenterY);
}

TEST_F(PanZoomHandlerTest, PanTo) {
    handler->PanTo(300, 400);
    
    auto center = handler->GetCenter();
    EXPECT_DOUBLE_EQ(center.x, 300.0);
    EXPECT_DOUBLE_EQ(center.y, 400.0);
}

TEST_F(PanZoomHandlerTest, ZoomIn) {
    double initialZoom = handler->GetZoom();
    handler->ZoomIn();
    EXPECT_GT(handler->GetZoom(), initialZoom);
}

TEST_F(PanZoomHandlerTest, ZoomOut) {
    double initialZoom = handler->GetZoom();
    handler->ZoomOut();
    EXPECT_LT(handler->GetZoom(), initialZoom);
}

TEST_F(PanZoomHandlerTest, ZoomTo) {
    handler->ZoomTo(5.0);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), 5.0);
}

TEST_F(PanZoomHandlerTest, ZoomToPoint) {
    double initialZoom = handler->GetZoom();
    handler->ZoomToPoint(400, 300, 2.0);
    EXPECT_DOUBLE_EQ(handler->GetZoom(), initialZoom * 2.0);
}

TEST_F(PanZoomHandlerTest, Rotate) {
    handler->SetRotation(0.0);
    handler->Rotate(30.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 30.0);
    
    handler->Rotate(30.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 60.0);
}

TEST_F(PanZoomHandlerTest, RotateTo) {
    handler->RotateTo(90.0);
    EXPECT_DOUBLE_EQ(handler->GetRotation(), 90.0);
}

TEST_F(PanZoomHandlerTest, SetViewportSize) {
    handler->SetViewportSize(1024, 768);
    EXPECT_EQ(handler->GetViewportWidth(), 1024);
    EXPECT_EQ(handler->GetViewportHeight(), 768);
}

TEST_F(PanZoomHandlerTest, InertiaEnabled) {
    EXPECT_TRUE(handler->IsInertiaEnabled());
    
    handler->SetInertiaEnabled(false);
    EXPECT_FALSE(handler->IsInertiaEnabled());
}

TEST_F(PanZoomHandlerTest, InertiaFriction) {
    handler->SetInertiaFriction(0.9);
    EXPECT_DOUBLE_EQ(handler->GetInertiaFriction(), 0.9);
}

TEST_F(PanZoomHandlerTest, AnimationEnabled) {
    EXPECT_TRUE(handler->IsAnimationEnabled());
    
    handler->SetAnimationEnabled(false);
    EXPECT_FALSE(handler->IsAnimationEnabled());
}

TEST_F(PanZoomHandlerTest, Constraints) {
    Envelope constraints(100, 100, 900, 900);
    handler->SetConstraints(constraints);
    EXPECT_TRUE(handler->HasConstraints());
    
    handler->ClearConstraints();
    EXPECT_FALSE(handler->HasConstraints());
}

TEST_F(PanZoomHandlerTest, FitToExtent) {
    Envelope extent(200, 200, 800, 800);
    handler->FitToExtent(extent);
    
    auto center = handler->GetCenter();
    EXPECT_DOUBLE_EQ(center.x, 500.0);
    EXPECT_DOUBLE_EQ(center.y, 500.0);
}

TEST_F(PanZoomHandlerTest, ScreenToWorld) {
    handler->SetCenter(500, 500);
    handler->SetZoom(1.0);
    
    Coordinate world = handler->ScreenToWorld(400, 300);
    EXPECT_NEAR(world.x, 500.0, 0.001);
    EXPECT_NEAR(world.y, 500.0, 0.001);
}

TEST_F(PanZoomHandlerTest, WorldToScreen) {
    handler->SetCenter(500, 500);
    handler->SetZoom(1.0);
    
    Coordinate screen = handler->WorldToScreen(500, 500);
    EXPECT_DOUBLE_EQ(screen.x, 400.0);
    EXPECT_DOUBLE_EQ(screen.y, 300.0);
}

TEST_F(PanZoomHandlerTest, HandleMouseDown) {
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandleMouseMove) {
    InteractionEvent downEvent;
    downEvent.type = InteractionEventType::kMouseDown;
    downEvent.screenX = 100;
    downEvent.screenY = 200;
    downEvent.button = MouseButton::kLeft;
    handler->HandleEvent(downEvent);
    
    InteractionEvent moveEvent;
    moveEvent.type = InteractionEventType::kMouseMove;
    moveEvent.screenX = 150;
    moveEvent.screenY = 250;
    
    bool result = handler->HandleEvent(moveEvent);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandleMouseUp) {
    InteractionEvent downEvent;
    downEvent.type = InteractionEventType::kMouseDown;
    downEvent.screenX = 100;
    downEvent.screenY = 200;
    downEvent.button = MouseButton::kLeft;
    handler->HandleEvent(downEvent);
    
    InteractionEvent upEvent;
    upEvent.type = InteractionEventType::kMouseUp;
    upEvent.screenX = 150;
    upEvent.screenY = 250;
    upEvent.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(upEvent);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandleWheel) {
    double initialZoom = handler->GetZoom();
    
    InteractionEvent event;
    event.type = InteractionEventType::kMouseWheel;
    event.screenX = 400;
    event.screenY = 300;
    event.delta = 1.0;
    
    handler->SetAnimationEnabled(false);
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
    EXPECT_GT(handler->GetZoom(), initialZoom);
}

TEST_F(PanZoomHandlerTest, HandleKeyDown) {
    double initialZoom = handler->GetZoom();
    
    InteractionEvent event;
    event.type = InteractionEventType::kKeyDown;
    event.key = '+';
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
    EXPECT_GT(handler->GetZoom(), initialZoom);
}

TEST_F(PanZoomHandlerTest, HandleKeyDownArrowKeys) {
    double initialX = handler->GetCenter().x;
    
    InteractionEvent event;
    event.type = InteractionEventType::kKeyDown;
    event.key = 39;  // Right arrow
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
    EXPECT_NE(handler->GetCenter().x, initialX);
}

TEST_F(PanZoomHandlerTest, HandleTouchStart) {
    InteractionEvent event;
    event.type = InteractionEventType::kTouchBegin;
    event.touchPositions.emplace_back(100, 200);
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandleTouchMove) {
    InteractionEvent startEvent;
    startEvent.type = InteractionEventType::kTouchBegin;
    startEvent.touchPositions.emplace_back(100, 200);
    handler->HandleEvent(startEvent);
    
    InteractionEvent moveEvent;
    moveEvent.type = InteractionEventType::kTouchMove;
    moveEvent.touchPositions.emplace_back(150, 250);
    
    bool result = handler->HandleEvent(moveEvent);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandleTouchEnd) {
    InteractionEvent startEvent;
    startEvent.type = InteractionEventType::kTouchBegin;
    startEvent.touchPositions.emplace_back(100, 200);
    handler->HandleEvent(startEvent);
    
    InteractionEvent endEvent;
    endEvent.type = InteractionEventType::kTouchEnd;
    
    bool result = handler->HandleEvent(endEvent);
    EXPECT_TRUE(result);
}

TEST_F(PanZoomHandlerTest, HandlePinchZoom) {
    InteractionEvent startEvent;
    startEvent.type = InteractionEventType::kTouchBegin;
    startEvent.touchPositions.emplace_back(100, 200);
    startEvent.touchPositions.emplace_back(300, 400);
    handler->HandleEvent(startEvent);
    
    double initialZoom = handler->GetZoom();
    
    InteractionEvent moveEvent;
    moveEvent.type = InteractionEventType::kTouchMove;
    moveEvent.touchPositions.emplace_back(50, 150);
    moveEvent.touchPositions.emplace_back(350, 450);
    
    handler->HandleEvent(moveEvent);
}

TEST_F(PanZoomHandlerTest, HandleDoubleClick) {
    double initialZoom = handler->GetZoom();
    
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDoubleClick;
    event.screenX = 400;
    event.screenY = 300;
    event.button = MouseButton::kLeft;
    
    handler->SetAnimationEnabled(false);
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
    EXPECT_GT(handler->GetZoom(), initialZoom);
}

TEST_F(PanZoomHandlerTest, DisabledHandlerIgnoresEvents) {
    handler->SetEnabled(false);
    
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(event);
    EXPECT_FALSE(result);
}

TEST_F(PanZoomHandlerTest, Callbacks) {
    bool extentChanged = false;
    bool zoomChanged = false;
    bool centerChanged = false;
    bool rotationChanged = false;
    
    handler->SetExtentChangedCallback([&extentChanged](const Envelope&) {
        extentChanged = true;
    });
    handler->SetZoomChangedCallback([&zoomChanged](double) {
        zoomChanged = true;
    });
    handler->SetCenterChangedCallback([&centerChanged](double, double) {
        centerChanged = true;
    });
    handler->SetRotationChangedCallback([&rotationChanged](double) {
        rotationChanged = true;
    });
    
    handler->SetZoom(2.0);
    EXPECT_TRUE(zoomChanged);
    
    handler->SetCenter(300, 400);
    EXPECT_TRUE(centerChanged);
    
    handler->SetRotation(45.0);
    EXPECT_TRUE(rotationChanged);
}

TEST_F(PanZoomHandlerTest, StateManagement) {
    PanZoomState state;
    state.centerX = 500;
    state.centerY = 600;
    state.zoom = 3.0;
    state.rotation = 30.0;
    
    handler->SetPanZoomState(state);
    
    const auto& result = handler->GetPanZoomState();
    EXPECT_DOUBLE_EQ(result.centerX, 500.0);
    EXPECT_DOUBLE_EQ(result.centerY, 600.0);
    EXPECT_DOUBLE_EQ(result.zoom, 3.0);
    EXPECT_DOUBLE_EQ(result.rotation, 30.0);
}

TEST_F(PanZoomHandlerTest, ZoomParams) {
    ZoomParams params;
    params.factor = 1.5;
    params.animate = false;
    params.animationDurationMs = 100;
    
    handler->SetZoomParams(params);
    
    auto result = handler->GetZoomParams();
    EXPECT_DOUBLE_EQ(result.factor, 1.5);
    EXPECT_FALSE(result.animate);
    EXPECT_EQ(result.animationDurationMs, 100);
}

TEST_F(PanZoomHandlerTest, CancelAnimation) {
    handler->SetAnimationEnabled(true);
    handler->SetInertiaEnabled(true);
    
    InteractionEvent downEvent;
    downEvent.type = InteractionEventType::kMouseDown;
    downEvent.screenX = 100;
    downEvent.screenY = 200;
    downEvent.button = MouseButton::kLeft;
    handler->HandleEvent(downEvent);
    
    handler->CancelAnimation();
    EXPECT_FALSE(handler->IsAnimating());
}

TEST_F(PanZoomHandlerTest, CustomTransforms) {
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
