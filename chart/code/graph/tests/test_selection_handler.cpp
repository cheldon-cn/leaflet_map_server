#include <gtest/gtest.h>
#include "ogc/draw/selection_handler.h"
#include "ogc/draw/hit_test.h"
#include "ogc/draw/interaction_feedback.h"
#include "ogc/envelope.h"

using namespace ogc::draw;
using ogc::Coordinate;
using ogc::Envelope;

class SelectionHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = SelectionHandler::Create("test_handler");
        handler->SetViewportSize(800, 600);
    }
    
    void TearDown() override {
        handler.reset();
    }
    
    std::unique_ptr<SelectionHandler> handler;
};

TEST_F(SelectionHandlerTest, CreateHandler) {
    EXPECT_TRUE(handler != nullptr);
    EXPECT_EQ(handler->GetName(), "test_handler");
    EXPECT_TRUE(handler->IsEnabled());
}

TEST_F(SelectionHandlerTest, SetPriority) {
    handler->SetPriority(50);
    EXPECT_EQ(handler->GetPriority(), 50);
}

TEST_F(SelectionHandlerTest, EnableDisable) {
    handler->SetEnabled(true);
    EXPECT_TRUE(handler->IsEnabled());
    
    handler->SetEnabled(false);
    EXPECT_FALSE(handler->IsEnabled());
}

TEST_F(SelectionHandlerTest, SetSelectionMode) {
    handler->SetSelectionMode(SelectionMode::kSingle);
    EXPECT_EQ(handler->GetSelectionMode(), SelectionMode::kSingle);
    
    handler->SetSelectionMode(SelectionMode::kMultiple);
    EXPECT_EQ(handler->GetSelectionMode(), SelectionMode::kMultiple);
}

TEST_F(SelectionHandlerTest, SetTolerance) {
    handler->SetTolerance(10.0);
    EXPECT_DOUBLE_EQ(handler->GetTolerance(), 10.0);
}

TEST_F(SelectionHandlerTest, SelectionParams) {
    SelectionParams params;
    params.mode = SelectionMode::kMultiple;
    params.tolerance = 15.0;
    params.enableBoxSelection = false;
    
    handler->SetSelectionParams(params);
    
    auto result = handler->GetSelectionParams();
    EXPECT_EQ(result.mode, SelectionMode::kMultiple);
    EXPECT_DOUBLE_EQ(result.tolerance, 15.0);
    EXPECT_FALSE(result.enableBoxSelection);
}

TEST_F(SelectionHandlerTest, SelectionStyle) {
    SelectionStyle style;
    style.fillColor = 0x00FF0000;
    style.strokeColor = 0x0000FF00;
    style.strokeWidth = 3.0;
    
    handler->SetSelectionStyle(style);
    
    auto result = handler->GetSelectionStyle();
    EXPECT_EQ(result.fillColor, 0x00FF0000u);
    EXPECT_EQ(result.strokeColor, 0x0000FF00u);
    EXPECT_DOUBLE_EQ(result.strokeWidth, 3.0);
}

TEST_F(SelectionHandlerTest, SelectFeature) {
    handler->SelectFeature("feature1");
    EXPECT_TRUE(handler->IsFeatureSelected("feature1"));
    EXPECT_EQ(handler->GetSelectionCount(), 1u);
}

TEST_F(SelectionHandlerTest, DeselectFeature) {
    handler->SelectFeature("feature1");
    EXPECT_TRUE(handler->IsFeatureSelected("feature1"));
    
    handler->DeselectFeature("feature1");
    EXPECT_FALSE(handler->IsFeatureSelected("feature1"));
    EXPECT_EQ(handler->GetSelectionCount(), 0u);
}

TEST_F(SelectionHandlerTest, ToggleFeature) {
    handler->ToggleFeature("feature1");
    EXPECT_TRUE(handler->IsFeatureSelected("feature1"));
    
    handler->ToggleFeature("feature1");
    EXPECT_FALSE(handler->IsFeatureSelected("feature1"));
}

TEST_F(SelectionHandlerTest, SelectFeatures) {
    std::set<std::string> featureIds;
    featureIds.insert("feature1");
    featureIds.insert("feature2");
    featureIds.insert("feature3");
    
    handler->SelectFeatures(featureIds);
    
    EXPECT_EQ(handler->GetSelectionCount(), 3u);
    EXPECT_TRUE(handler->IsFeatureSelected("feature1"));
    EXPECT_TRUE(handler->IsFeatureSelected("feature2"));
    EXPECT_TRUE(handler->IsFeatureSelected("feature3"));
}

TEST_F(SelectionHandlerTest, ClearSelection) {
    handler->SelectFeature("feature1");
    handler->SelectFeature("feature2");
    EXPECT_EQ(handler->GetSelectionCount(), 2u);
    
    handler->ClearSelection();
    EXPECT_EQ(handler->GetSelectionCount(), 0u);
    EXPECT_FALSE(handler->HasSelection());
}

TEST_F(SelectionHandlerTest, HasSelection) {
    EXPECT_FALSE(handler->HasSelection());
    
    handler->SelectFeature("feature1");
    EXPECT_TRUE(handler->HasSelection());
    
    handler->ClearSelection();
    EXPECT_FALSE(handler->HasSelection());
}

TEST_F(SelectionHandlerTest, GetSelectedFeatures) {
    handler->SelectFeature("feature1");
    handler->SelectFeature("feature2");
    
    const auto& selected = handler->GetSelectedFeatures();
    EXPECT_EQ(selected.size(), 2u);
    EXPECT_NE(selected.find("feature1"), selected.end());
    EXPECT_NE(selected.find("feature2"), selected.end());
}

TEST_F(SelectionHandlerTest, SetViewportSize) {
    handler->SetViewportSize(1024, 768);
}

TEST_F(SelectionHandlerTest, SetLayerManager) {
    LayerManager* manager = nullptr;
    handler->SetLayerManager(manager);
    EXPECT_EQ(handler->GetLayerManager(), nullptr);
}

TEST_F(SelectionHandlerTest, SetHitTester) {
    HitTester* hitTester = nullptr;
    handler->SetHitTester(hitTester);
    EXPECT_EQ(handler->GetHitTester(), nullptr);
}

TEST_F(SelectionHandlerTest, SetFeedbackManager) {
    FeedbackManager* feedbackManager = nullptr;
    handler->SetFeedbackManager(feedbackManager);
    EXPECT_EQ(handler->GetFeedbackManager(), nullptr);
}

TEST_F(SelectionHandlerTest, HandleMouseDown) {
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
}

TEST_F(SelectionHandlerTest, HandleMouseMove) {
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

TEST_F(SelectionHandlerTest, HandleMouseUp) {
    InteractionEvent downEvent;
    downEvent.type = InteractionEventType::kMouseDown;
    downEvent.screenX = 100;
    downEvent.screenY = 200;
    downEvent.button = MouseButton::kLeft;
    handler->HandleEvent(downEvent);
    
    InteractionEvent upEvent;
    upEvent.type = InteractionEventType::kMouseUp;
    upEvent.screenX = 100;
    upEvent.screenY = 200;
    upEvent.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(upEvent);
    EXPECT_TRUE(result);
}

TEST_F(SelectionHandlerTest, HandleKeyDownEscape) {
    InteractionEvent event;
    event.type = InteractionEventType::kKeyDown;
    event.key = 27;  // Escape
    
    bool result = handler->HandleEvent(event);
    EXPECT_TRUE(result);
}

TEST_F(SelectionHandlerTest, DisabledHandlerIgnoresEvents) {
    handler->SetEnabled(false);
    
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kLeft;
    
    bool result = handler->HandleEvent(event);
    EXPECT_FALSE(result);
}

TEST_F(SelectionHandlerTest, GetState) {
    EXPECT_EQ(handler->GetState(), InteractionState::kNone);
}

TEST_F(SelectionHandlerTest, Callbacks) {
    bool selectionChanged = false;
    std::string selectedFeatureId;
    bool isSelected = false;
    
    handler->SetSelectionChangedCallback([&selectionChanged](const std::set<std::string>&) {
        selectionChanged = true;
    });
    
    handler->SetFeatureSelectedCallback([&selectedFeatureId, &isSelected](const std::string& id, bool selected) {
        selectedFeatureId = id;
        isSelected = selected;
    });
    
    handler->SelectFeature("feature1");
    EXPECT_TRUE(selectionChanged);
}

TEST_F(SelectionHandlerTest, CustomTransforms) {
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

TEST_F(SelectionHandlerTest, BoxSelectionParams) {
    SelectionParams params;
    params.enableBoxSelection = true;
    handler->SetSelectionParams(params);
    
    EXPECT_TRUE(handler->GetSelectionParams().enableBoxSelection);
}

TEST_F(SelectionHandlerTest, MultiSelectionParams) {
    SelectionParams params;
    params.enableMultiSelection = true;
    handler->SetSelectionParams(params);
    
    EXPECT_TRUE(handler->GetSelectionParams().enableMultiSelection);
}

TEST_F(SelectionHandlerTest, SelectionEnvelope) {
    const auto& envelope = handler->GetSelectionEnvelope();
    EXPECT_TRUE(envelope.IsNull());
}

TEST_F(SelectionHandlerTest, RightMouseDoesNotSelect) {
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kRight;
    
    bool result = handler->HandleEvent(event);
    EXPECT_FALSE(result);
}

TEST_F(SelectionHandlerTest, MiddleMouseDoesNotSelect) {
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    event.button = MouseButton::kMiddle;
    
    bool result = handler->HandleEvent(event);
    EXPECT_FALSE(result);
}

TEST_F(SelectionHandlerTest, SelectSameFeatureTwice) {
    handler->SelectFeature("feature1");
    EXPECT_EQ(handler->GetSelectionCount(), 1u);
    
    handler->SelectFeature("feature1");
    EXPECT_EQ(handler->GetSelectionCount(), 1u);
}

TEST_F(SelectionHandlerTest, DeselectNonExistentFeature) {
    handler->SelectFeature("feature1");
    handler->DeselectFeature("feature2");
    EXPECT_EQ(handler->GetSelectionCount(), 1u);
    EXPECT_TRUE(handler->IsFeatureSelected("feature1"));
}

TEST_F(SelectionHandlerTest, SelectByPointWithoutHitTester) {
    handler->SelectByPoint(100, 200);
    EXPECT_EQ(handler->GetSelectionCount(), 0u);
}

TEST_F(SelectionHandlerTest, SelectByEnvelopeWithoutHitTester) {
    Envelope envelope(0, 0, 100, 100);
    handler->SelectByEnvelope(envelope);
    EXPECT_EQ(handler->GetSelectionCount(), 0u);
}

TEST_F(SelectionHandlerTest, SelectByGeometryWithoutHitTester) {
    handler->SelectByGeometry(nullptr);
    EXPECT_EQ(handler->GetSelectionCount(), 0u);
}

TEST_F(SelectionHandlerTest, ShowFeedbackParam) {
    SelectionParams params;
    params.showFeedback = true;
    handler->SetSelectionParams(params);
    
    EXPECT_TRUE(handler->GetSelectionParams().showFeedback);
}

TEST_F(SelectionHandlerTest, SelectionStyleDefaults) {
    SelectionStyle style;
    EXPECT_EQ(style.fillColor, 0x0000FF00u);
    EXPECT_EQ(style.strokeColor, 0x0000FF00u);
    EXPECT_DOUBLE_EQ(style.strokeWidth, 2.0);
    EXPECT_DOUBLE_EQ(style.fillOpacity, 0.3);
    EXPECT_DOUBLE_EQ(style.strokeOpacity, 1.0);
    EXPECT_FALSE(style.showVertices);
}
