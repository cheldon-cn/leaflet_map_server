#include <gtest/gtest.h>
#include "ogc/draw/interaction_handler.h"
#include "ogc/draw/hit_test.h"
#include "ogc/draw/interaction_feedback.h"
#include "ogc/draw/layer_manager.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"

using namespace ogc::draw;

class InteractionHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        InteractionManager::Instance().Initialize();
    }
    
    void TearDown() override {
        InteractionManager::Instance().Finalize();
    }
};

TEST_F(InteractionHandlerTest, Instance) {
    auto& instance = InteractionManager::Instance();
    EXPECT_TRUE(true);
}

TEST_F(InteractionHandlerTest, Initialize) {
    auto& manager = InteractionManager::Instance();
    EXPECT_TRUE(manager.IsInitialized());
}

TEST_F(InteractionHandlerTest, CreateHandler) {
    auto handler = InteractionHandler::Create("test_handler");
    ASSERT_NE(handler, nullptr);
    EXPECT_EQ(handler->GetName(), "test_handler");
    EXPECT_TRUE(handler->IsEnabled());
    EXPECT_EQ(handler->GetPriority(), 0);
}

TEST_F(InteractionHandlerTest, HandlerEnableDisable) {
    auto handler = InteractionHandler::Create("test_handler");
    EXPECT_TRUE(handler->IsEnabled());
    
    handler->SetEnabled(false);
    EXPECT_FALSE(handler->IsEnabled());
    
    handler->SetEnabled(true);
    EXPECT_TRUE(handler->IsEnabled());
}

TEST_F(InteractionHandlerTest, HandlerPriority) {
    auto handler = InteractionHandler::Create("test_handler");
    EXPECT_EQ(handler->GetPriority(), 0);
    
    handler->SetPriority(10);
    EXPECT_EQ(handler->GetPriority(), 10);
    
    handler->SetPriority(-5);
    EXPECT_EQ(handler->GetPriority(), -5);
}

TEST_F(InteractionHandlerTest, AddRemoveHandler) {
    auto& manager = InteractionManager::Instance();
    auto handler = InteractionHandler::Create("test_handler");
    
    manager.AddHandler(handler.get());
    auto retrieved = manager.GetHandler("test_handler");
    EXPECT_EQ(retrieved, handler.get());
    
    manager.RemoveHandler(handler.get());
    retrieved = manager.GetHandler("test_handler");
    EXPECT_EQ(retrieved, nullptr);
}

TEST_F(InteractionHandlerTest, ProcessEvent) {
    auto& manager = InteractionManager::Instance();
    auto handler = InteractionHandler::Create("test_handler");
    
    bool eventHandled = false;
    handler->SetEventCallback(InteractionEventType::kMouseDown, 
        [&eventHandled](const InteractionEvent& event) {
            eventHandled = true;
            return true;
        });
    
    manager.AddHandler(handler.get());
    
    InteractionEvent event;
    event.type = InteractionEventType::kMouseDown;
    event.screenX = 100;
    event.screenY = 200;
    
    bool result = manager.ProcessEvent(event);
    EXPECT_TRUE(result);
    EXPECT_TRUE(eventHandled);
}

TEST_F(InteractionHandlerTest, SetViewportSize) {
    auto& manager = InteractionManager::Instance();
    manager.SetViewportSize(1024, 768);
    
    EXPECT_EQ(manager.GetViewportWidth(), 1024);
    EXPECT_EQ(manager.GetViewportHeight(), 768);
}

TEST_F(InteractionHandlerTest, SetCurrentState) {
    auto& manager = InteractionManager::Instance();
    manager.SetCurrentState(InteractionState::kPan);
    EXPECT_EQ(manager.GetCurrentState(), InteractionState::kPan);
    
    manager.SetCurrentState(InteractionState::kZoom);
    EXPECT_EQ(manager.GetCurrentState(), InteractionState::kZoom);
}

TEST_F(InteractionHandlerTest, ScreenToWorldTransform) {
    auto& manager = InteractionManager::Instance();
    
    manager.SetScreenToWorldTransform([](double x, double y) {
        return ogc::Coordinate(x * 2.0, y * 2.0);
    });
    
    auto world = manager.ScreenToWorld(100, 200);
    EXPECT_DOUBLE_EQ(world.x, 200.0);
    EXPECT_DOUBLE_EQ(world.y, 400.0);
}

class HitTestTest : public ::testing::Test {
protected:
    void SetUp() override {
        SelectionManager::Instance().Initialize();
    }
    
    void TearDown() override {
        SelectionManager::Instance().Finalize();
    }
};

TEST_F(HitTestTest, CreateHitTestResult) {
    auto result = HitTestResult::Create();
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->IsEmpty());
    EXPECT_EQ(result->GetCount(), 0);
}

TEST_F(HitTestTest, AddHitTestResultItem) {
    auto result = HitTestResult::Create();
    
    HitTestResultItem item;
    item.featureId = 1;
    item.layerName = "test_layer";
    item.distance = 10.5;
    
    result->AddItem(item);
    EXPECT_FALSE(result->IsEmpty());
    EXPECT_EQ(result->GetCount(), 1);
    
    const auto& retrieved = result->GetItem(0);
    EXPECT_EQ(retrieved.featureId, 1);
    EXPECT_EQ(retrieved.layerName, "test_layer");
    EXPECT_DOUBLE_EQ(retrieved.distance, 10.5);
}

TEST_F(HitTestTest, SortByDistance) {
    auto result = HitTestResult::Create();
    
    HitTestResultItem item1, item2, item3;
    item1.featureId = 1;
    item1.distance = 30.0;
    item2.featureId = 2;
    item2.distance = 10.0;
    item3.featureId = 3;
    item3.distance = 20.0;
    
    result->AddItem(item1);
    result->AddItem(item2);
    result->AddItem(item3);
    
    result->SortByDistance();
    
    EXPECT_EQ(result->GetItem(0).featureId, 2);
    EXPECT_EQ(result->GetItem(1).featureId, 3);
    EXPECT_EQ(result->GetItem(2).featureId, 1);
}

TEST_F(HitTestTest, GetTopN) {
    auto result = HitTestResult::Create();
    
    for (int i = 0; i < 10; ++i) {
        HitTestResultItem item;
        item.featureId = i;
        item.distance = static_cast<double>(i);
        result->AddItem(item);
    }
    
    auto top5 = result->GetTopN(5);
    EXPECT_EQ(top5->GetCount(), 5);
}

TEST_F(HitTestTest, GetByLayer) {
    auto result = HitTestResult::Create();
    
    HitTestResultItem item1, item2, item3;
    item1.featureId = 1;
    item1.layerName = "layer1";
    item2.featureId = 2;
    item2.layerName = "layer2";
    item3.featureId = 3;
    item3.layerName = "layer1";
    
    result->AddItem(item1);
    result->AddItem(item2);
    result->AddItem(item3);
    
    auto layer1Result = result->GetByLayer("layer1");
    EXPECT_EQ(layer1Result->GetCount(), 2);
    
    auto layer2Result = result->GetByLayer("layer2");
    EXPECT_EQ(layer2Result->GetCount(), 1);
}

TEST_F(HitTestTest, CreateHitTester) {
    auto tester = HitTester::Create();
    ASSERT_NE(tester, nullptr);
    EXPECT_DOUBLE_EQ(tester->GetTolerance(), 5.0);
    EXPECT_EQ(tester->GetFilter(), HitTestFilter::kSelectable);
}

TEST_F(HitTestTest, SetTolerance) {
    auto tester = HitTester::Create();
    tester->SetTolerance(10.0);
    EXPECT_DOUBLE_EQ(tester->GetTolerance(), 10.0);
}

TEST_F(HitTestTest, SetFilter) {
    auto tester = HitTester::Create();
    tester->SetFilter(HitTestFilter::kAll);
    EXPECT_EQ(tester->GetFilter(), HitTestFilter::kAll);
}

TEST_F(HitTestTest, CalculateDistance) {
    ogc::Coordinate p1(0, 0);
    ogc::Coordinate p2(3, 4);
    
    double dist = HitTester::CalculateDistance(p1, p2);
    EXPECT_DOUBLE_EQ(dist, 5.0);
}

TEST_F(HitTestTest, PointInGeometry) {
    auto point = ogc::Point::Create(10, 10);
    ogc::Coordinate testPoint(10, 10);
    ogc::Coordinate farPoint(100, 100);
    
    EXPECT_TRUE(HitTester::PointInGeometry(testPoint, point.get(), 0.0));
    EXPECT_TRUE(HitTester::PointInGeometry(testPoint, point.get(), 5.0));
    EXPECT_FALSE(HitTester::PointInGeometry(farPoint, point.get(), 5.0));
}

TEST_F(HitTestTest, SelectionManagerInstance) {
    auto& instance = SelectionManager::Instance();
    EXPECT_TRUE(true);
}

TEST_F(HitTestTest, SelectionManagerInitialize) {
    auto& manager = SelectionManager::Instance();
    EXPECT_TRUE(manager.IsInitialized());
}

TEST_F(HitTestTest, SelectFeature) {
    auto& manager = SelectionManager::Instance();
    
    manager.Select(1, "test_layer");
    EXPECT_TRUE(manager.IsSelected(1));
    
    manager.DeselectAll();
    EXPECT_FALSE(manager.IsSelected(1));
}

TEST_F(HitTestTest, SelectMultipleFeatures) {
    auto& manager = SelectionManager::Instance();
    
    std::vector<int64_t> ids = {1, 2, 3, 4, 5};
    manager.Select(ids, "test_layer");
    
    for (int64_t id : ids) {
        EXPECT_TRUE(manager.IsSelected(id));
    }
    
    manager.DeselectAll();
}

TEST_F(HitTestTest, DeselectFeature) {
    auto& manager = SelectionManager::Instance();
    
    manager.Select(1, "test_layer");
    EXPECT_TRUE(manager.IsSelected(1));
    
    manager.Deselect(1);
    EXPECT_FALSE(manager.IsSelected(1));
}

TEST_F(HitTestTest, GetSelectedIds) {
    auto& manager = SelectionManager::Instance();
    
    manager.Select(1, "test_layer");
    manager.Select(2, "test_layer");
    manager.Select(3, "test_layer");
    
    auto ids = manager.GetSelectedIds();
    EXPECT_EQ(ids.size(), 3);
    
    manager.DeselectAll();
}

TEST_F(HitTestTest, SetSelectionColor) {
    auto& manager = SelectionManager::Instance();
    
    manager.SetSelectionColor(0xFFFF0000);
    EXPECT_EQ(manager.GetSelectionColor(), 0xFFFF0000);
    
    manager.SetHighlightColor(0xFF00FF00);
    EXPECT_EQ(manager.GetHighlightColor(), 0xFF00FF00);
}

class InteractionFeedbackTest : public ::testing::Test {
protected:
    void SetUp() override {
        FeedbackManager::Instance().Initialize();
    }
    
    void TearDown() override {
        FeedbackManager::Instance().Finalize();
    }
};

TEST_F(InteractionFeedbackTest, Instance) {
    auto& instance = FeedbackManager::Instance();
    EXPECT_TRUE(true);
}

TEST_F(InteractionFeedbackTest, Initialize) {
    auto& manager = FeedbackManager::Instance();
    EXPECT_TRUE(manager.IsInitialized());
}

TEST_F(InteractionFeedbackTest, CreateFeedbackItem) {
    auto item = FeedbackItem::Create(FeedbackType::kSelection);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(item->IsVisible());
}

TEST_F(InteractionFeedbackTest, FeedbackItemSetPoint) {
    auto item = FeedbackItem::Create(FeedbackType::kHover);
    ogc::Coordinate point(100, 200);
    item->SetPoint(point);
    
    auto retrieved = item->GetPoint();
    EXPECT_DOUBLE_EQ(retrieved.x, 100.0);
    EXPECT_DOUBLE_EQ(retrieved.y, 200.0);
}

TEST_F(InteractionFeedbackTest, FeedbackItemSetEnvelope) {
    auto item = FeedbackItem::Create(FeedbackType::kRubberBand);
    ogc::Envelope env(0, 0, 100, 100);
    item->SetEnvelope(env);
    
    auto retrieved = item->GetEnvelope();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 100.0);
}

TEST_F(InteractionFeedbackTest, AddRemoveFeedback) {
    auto& manager = FeedbackManager::Instance();
    auto item = FeedbackItem::Create(FeedbackType::kSelection);
    
    int id = manager.AddFeedback(item);
    EXPECT_GT(id, 0);
    EXPECT_EQ(manager.GetFeedbackCount(), 1);
    
    manager.RemoveFeedback(id);
    EXPECT_EQ(manager.GetFeedbackCount(), 0);
}

TEST_F(InteractionFeedbackTest, RemoveFeedbackByType) {
    auto& manager = FeedbackManager::Instance();
    
    manager.RemoveAllFeedback();
    manager.AddFeedback(FeedbackItem::Create(FeedbackType::kSelection));
    manager.AddFeedback(FeedbackItem::Create(FeedbackType::kSelection));
    manager.AddFeedback(FeedbackItem::Create(FeedbackType::kHighlight));
    
    EXPECT_EQ(manager.GetFeedbackCount(), 3);
    
    manager.RemoveFeedbackByType(FeedbackType::kSelection);
    EXPECT_EQ(manager.GetFeedbackCount(), 1);
    
    manager.RemoveAllFeedback();
}

TEST_F(InteractionFeedbackTest, ShowSelectionFeedback) {
    auto& manager = FeedbackManager::Instance();
    
    std::vector<int64_t> ids = {1, 2, 3};
    int id = manager.ShowSelectionFeedback(ids, "test_layer");
    EXPECT_GT(id, 0);
    
    manager.HideSelectionFeedback();
    EXPECT_EQ(manager.GetFeedbackCount(), 0);
}

TEST_F(InteractionFeedbackTest, ShowRubberBandFeedback) {
    auto& manager = FeedbackManager::Instance();
    
    ogc::Envelope rect(0, 0, 100, 100);
    int id = manager.ShowRubberBandFeedback(rect);
    EXPECT_GT(id, 0);
    
    manager.HideRubberBandFeedback();
    EXPECT_EQ(manager.GetFeedbackCount(), 0);
}

TEST_F(InteractionFeedbackTest, ShowTooltipFeedback) {
    auto& manager = FeedbackManager::Instance();
    
    int id = manager.ShowTooltipFeedback(100, 200, "Test Tooltip");
    EXPECT_GT(id, 0);
    
    manager.HideTooltipFeedback();
    EXPECT_EQ(manager.GetFeedbackCount(), 0);
}

TEST_F(InteractionFeedbackTest, SetDefaultConfig) {
    auto& manager = FeedbackManager::Instance();
    
    FeedbackConfig config;
    config.fillColor = 0xFFFF0000;
    config.strokeColor = 0xFF000000;
    config.strokeWidth = 3.0;
    
    manager.SetDefaultConfig(FeedbackType::kSelection, config);
    
    auto retrieved = manager.GetDefaultConfig(FeedbackType::kSelection);
    EXPECT_EQ(retrieved.fillColor, 0xFFFF0000);
    EXPECT_EQ(retrieved.strokeColor, 0xFF000000);
    EXPECT_DOUBLE_EQ(retrieved.strokeWidth, 3.0);
}

TEST_F(InteractionFeedbackTest, MeasureToolInstance) {
    auto& instance = MeasureTool::Instance();
    EXPECT_TRUE(true);
}

TEST_F(InteractionFeedbackTest, MeasureToolDistance) {
    auto& tool = MeasureTool::Instance();
    tool.Start();
    tool.ClearPoints();
    
    tool.AddPoint(ogc::Coordinate(0, 0));
    tool.AddPoint(ogc::Coordinate(3, 4));
    
    double dist = tool.GetTotalDistance();
    EXPECT_DOUBLE_EQ(dist, 5.0);
    
    tool.Stop();
}

TEST_F(InteractionFeedbackTest, MeasureToolArea) {
    auto& tool = MeasureTool::Instance();
    tool.Start();
    tool.ClearPoints();
    
    tool.AddPoint(ogc::Coordinate(0, 0));
    tool.AddPoint(ogc::Coordinate(10, 0));
    tool.AddPoint(ogc::Coordinate(10, 10));
    tool.AddPoint(ogc::Coordinate(0, 10));
    
    double area = tool.GetArea();
    EXPECT_DOUBLE_EQ(area, 100.0);
    
    tool.Stop();
}

TEST_F(InteractionFeedbackTest, MeasureToolDistanceString) {
    auto& tool = MeasureTool::Instance();
    tool.Start();
    tool.ClearPoints();
    
    tool.AddPoint(ogc::Coordinate(0, 0));
    tool.AddPoint(ogc::Coordinate(1000, 0));
    
    std::string distStr = tool.GetDistanceString();
    EXPECT_FALSE(distStr.empty());
    
    tool.Stop();
}

TEST_F(InteractionFeedbackTest, MeasureToolMode) {
    auto& tool = MeasureTool::Instance();
    
    tool.SetMode(MeasureTool::MeasureMode::kDistance);
    EXPECT_EQ(tool.GetMode(), MeasureTool::MeasureMode::kDistance);
    
    tool.SetMode(MeasureTool::MeasureMode::kArea);
    EXPECT_EQ(tool.GetMode(), MeasureTool::MeasureMode::kArea);
    
    tool.SetMode(MeasureTool::MeasureMode::kAngle);
    EXPECT_EQ(tool.GetMode(), MeasureTool::MeasureMode::kAngle);
}

TEST_F(InteractionFeedbackTest, AnimationEnabled) {
    auto& manager = FeedbackManager::Instance();
    
    manager.SetAnimationEnabled(true);
    EXPECT_TRUE(manager.IsAnimationEnabled());
    
    manager.SetAnimationEnabled(false);
    EXPECT_FALSE(manager.IsAnimationEnabled());
    
    manager.SetAnimationEnabled(true);
}
