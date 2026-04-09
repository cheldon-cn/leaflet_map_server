#include <gtest/gtest.h>
#include <ogc/graph/render/render_task.h>
#include <ogc/draw/draw_result.h>
#include <ogc/geom/envelope.h>
#include <memory>

using namespace ogc::graph;
using ogc::draw::DrawResult;
using ogc::Envelope;

class TestRenderTask : public RenderTask {
public:
    TestRenderTask() : RenderTask() {}
    TestRenderTask(const std::string& id, const DrawParams& params) : RenderTask(id, params) {}
    
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetProgress(0.5);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<TestRenderTask> CreateTest() {
        return std::make_shared<TestRenderTask>();
    }
};

class RenderTaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_task = TestRenderTask::CreateTest();
        ASSERT_NE(m_task, nullptr);
    }
    
    void TearDown() override {
        m_task.reset();
    }
    
    std::shared_ptr<TestRenderTask> m_task;
};

TEST_F(RenderTaskTest, Create) {
    EXPECT_NE(m_task, nullptr);
}

TEST_F(RenderTaskTest, CreateWithParams) {
    DrawParams params;
    auto task = RenderTask::Create("test_id", params);
    EXPECT_EQ(task, nullptr);
}

TEST_F(RenderTaskTest, SetGetId) {
    m_task->SetId("my_task");
    EXPECT_EQ(m_task->GetId(), "my_task");
}

TEST_F(RenderTaskTest, SetGetParams) {
    DrawParams params;
    m_task->SetParams(params);
    auto retrieved = m_task->GetParams();
}

TEST_F(RenderTaskTest, SetGetExtent) {
    Envelope extent(0, 0, 100, 100);
    m_task->SetExtent(extent);
    auto retrieved = m_task->GetExtent();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 100);
}

TEST_F(RenderTaskTest, SetGetPriority) {
    m_task->SetPriority(RenderTaskPriority::kHigh);
    EXPECT_EQ(m_task->GetPriority(), RenderTaskPriority::kHigh);
}

TEST_F(RenderTaskTest, SetGetState) {
    m_task->SetState(RenderTaskState::kQueued);
    EXPECT_EQ(m_task->GetState(), RenderTaskState::kQueued);
}

TEST_F(RenderTaskTest, SetGetProgress) {
    m_task->SetProgress(0.5);
    EXPECT_DOUBLE_EQ(m_task->GetProgress(), 0.5);
}

TEST_F(RenderTaskTest, SetGetResult) {
    m_task->SetResult(DrawResult::kSuccess);
    EXPECT_EQ(m_task->GetResult(), DrawResult::kSuccess);
}

TEST_F(RenderTaskTest, SetGetError) {
    m_task->SetError("Test error");
    EXPECT_EQ(m_task->GetError(), "Test error");
}

TEST_F(RenderTaskTest, SetGetStats) {
    RenderTaskStats stats;
    stats.featureCount = 100;
    stats.geometryCount = 50;
    m_task->SetStats(stats);
    auto retrieved = m_task->GetStats();
    EXPECT_EQ(retrieved.featureCount, 100);
    EXPECT_EQ(retrieved.geometryCount, 50);
}

TEST_F(RenderTaskTest, SetGetTag) {
    m_task->SetTag("my_tag");
    EXPECT_EQ(m_task->GetTag(), "my_tag");
}

TEST_F(RenderTaskTest, SetGetTimeout) {
    m_task->SetTimeout(5000);
    EXPECT_EQ(m_task->GetTimeout(), 5000);
}

TEST_F(RenderTaskTest, IsPending) {
    m_task->SetState(RenderTaskState::kPending);
    EXPECT_TRUE(m_task->IsPending());
}

TEST_F(RenderTaskTest, IsRunning) {
    m_task->SetState(RenderTaskState::kRunning);
    EXPECT_TRUE(m_task->IsRunning());
}

TEST_F(RenderTaskTest, IsCompleted) {
    m_task->SetState(RenderTaskState::kCompleted);
    EXPECT_TRUE(m_task->IsCompleted());
}

TEST_F(RenderTaskTest, IsFailed) {
    m_task->SetState(RenderTaskState::kFailed);
    EXPECT_TRUE(m_task->IsFailed());
}

TEST_F(RenderTaskTest, IsCancelled) {
    m_task->SetState(RenderTaskState::kCancelled);
    EXPECT_TRUE(m_task->IsCancelled());
}

TEST_F(RenderTaskTest, Cancel) {
    m_task->Cancel();
    EXPECT_TRUE(m_task->IsCancelled());
}

TEST_F(RenderTaskTest, Execute) {
    m_task->SetState(RenderTaskState::kQueued);
    bool result = m_task->Execute();
    EXPECT_TRUE(result);
    EXPECT_TRUE(m_task->IsCompleted());
}

TEST_F(RenderTaskTest, GetPriorityValue) {
    m_task->SetPriority(RenderTaskPriority::kLow);
    EXPECT_EQ(m_task->GetPriorityValue(), 0);
    
    m_task->SetPriority(RenderTaskPriority::kNormal);
    EXPECT_EQ(m_task->GetPriorityValue(), 1);
    
    m_task->SetPriority(RenderTaskPriority::kHigh);
    EXPECT_EQ(m_task->GetPriorityValue(), 2);
    
    m_task->SetPriority(RenderTaskPriority::kUrgent);
    EXPECT_EQ(m_task->GetPriorityValue(), 3);
}

TEST_F(RenderTaskTest, ComparisonOperator) {
    auto task1 = TestRenderTask::CreateTest();
    task1->SetPriority(RenderTaskPriority::kHigh);
    
    auto task2 = TestRenderTask::CreateTest();
    task2->SetPriority(RenderTaskPriority::kLow);
    
    EXPECT_TRUE(*task2 < *task1);
}

TEST_F(RenderTaskTest, SetCallback) {
    bool callbackCalled = false;
    m_task->SetCallback([&callbackCalled](const RenderTaskPtr&) {
        callbackCalled = true;
    });
    
    m_task->SetState(RenderTaskState::kQueued);
    m_task->Execute();
    m_task->NotifyCompletion();
    
    EXPECT_TRUE(callbackCalled);
}

TEST_F(RenderTaskTest, SetGetUserData) {
    int userData = 42;
    m_task->SetUserData(&userData);
    auto retrieved = m_task->GetUserData();
    EXPECT_EQ(*static_cast<int*>(retrieved), 42);
}
