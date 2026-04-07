#include <gtest/gtest.h>
#include "ogc/graph/render/render_queue.h"
#include "ogc/graph/render/render_task.h"
#include <memory>

using namespace ogc::graph;

class TestRenderTask : public RenderTask {
public:
    TestRenderTask() : RenderTask() {}
    TestRenderTask(const std::string& id, const DrawParams& params) : RenderTask(id, params) {}
    
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<TestRenderTask> CreateTest() {
        return std::make_shared<TestRenderTask>();
    }
};

class RenderQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_queue = RenderQueue::Create();
        ASSERT_NE(m_queue, nullptr);
    }
    
    void TearDown() override {
        m_queue.reset();
    }
    
    RenderQueuePtr m_queue;
};

TEST_F(RenderQueueTest, Create) {
    EXPECT_NE(m_queue, nullptr);
}

TEST_F(RenderQueueTest, CreateWithMaxSize) {
    auto queue = RenderQueue::Create(100);
    ASSERT_NE(queue, nullptr);
    EXPECT_EQ(queue->GetMaxSize(), 100);
}

TEST_F(RenderQueueTest, EnqueueDequeue) {
    auto task = TestRenderTask::CreateTest();
    task->SetId("task1");
    
    EXPECT_TRUE(m_queue->Enqueue(task));
    EXPECT_EQ(m_queue->GetSize(), 1);
    
    auto dequeued = m_queue->Dequeue();
    ASSERT_NE(dequeued, nullptr);
    EXPECT_EQ(dequeued->GetId(), "task1");
    EXPECT_EQ(m_queue->GetSize(), 0);
}

TEST_F(RenderQueueTest, EnqueueMultiple) {
    for (int i = 0; i < 5; i++) {
        auto task = TestRenderTask::CreateTest();
        task->SetId("task" + std::to_string(i));
        EXPECT_TRUE(m_queue->Enqueue(task));
    }
    EXPECT_EQ(m_queue->GetSize(), 5);
}

TEST_F(RenderQueueTest, IsEmpty) {
    EXPECT_TRUE(m_queue->IsEmpty());
    
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    EXPECT_FALSE(m_queue->IsEmpty());
}

TEST_F(RenderQueueTest, GetSize) {
    EXPECT_EQ(m_queue->GetSize(), 0);
    
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    EXPECT_EQ(m_queue->GetSize(), 1);
}

TEST_F(RenderQueueTest, SetGetMaxSize) {
    m_queue->SetMaxSize(50);
    EXPECT_EQ(m_queue->GetMaxSize(), 50);
}

TEST_F(RenderQueueTest, EnqueueBeyondMaxSize) {
    m_queue->SetMaxSize(2);
    
    auto task1 = TestRenderTask::CreateTest();
    auto task2 = TestRenderTask::CreateTest();
    auto task3 = TestRenderTask::CreateTest();
    
    EXPECT_TRUE(m_queue->Enqueue(task1));
    EXPECT_TRUE(m_queue->Enqueue(task2));
    EXPECT_FALSE(m_queue->Enqueue(task3));
}

TEST_F(RenderQueueTest, Clear) {
    for (int i = 0; i < 3; i++) {
        auto task = TestRenderTask::CreateTest();
        m_queue->Enqueue(task);
    }
    m_queue->Clear();
    EXPECT_EQ(m_queue->GetSize(), 0);
    EXPECT_TRUE(m_queue->IsEmpty());
}

TEST_F(RenderQueueTest, CancelAll) {
    for (int i = 0; i < 3; i++) {
        auto task = TestRenderTask::CreateTest();
        m_queue->Enqueue(task);
    }
    m_queue->CancelAll();
    EXPECT_TRUE(m_queue->IsEmpty());
}

TEST_F(RenderQueueTest, Peek) {
    auto task = TestRenderTask::CreateTest();
    task->SetId("peek_task");
    m_queue->Enqueue(task);
    
    auto peeked = m_queue->Peek();
    ASSERT_NE(peeked, nullptr);
    EXPECT_EQ(peeked->GetId(), "peek_task");
    EXPECT_EQ(m_queue->GetSize(), 1);
}

TEST_F(RenderQueueTest, RemoveTask) {
    auto task1 = TestRenderTask::CreateTest();
    task1->SetId("task1");
    auto task2 = TestRenderTask::CreateTest();
    task2->SetId("task2");
    
    m_queue->Enqueue(task1);
    m_queue->Enqueue(task2);
    
    m_queue->RemoveTask("task1");
    EXPECT_FALSE(m_queue->HasTask("task1"));
    EXPECT_TRUE(m_queue->HasTask("task2"));
}

TEST_F(RenderQueueTest, GetTask) {
    auto task = TestRenderTask::CreateTest();
    task->SetId("my_task");
    m_queue->Enqueue(task);
    
    auto retrieved = m_queue->GetTask("my_task");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetId(), "my_task");
}

TEST_F(RenderQueueTest, HasTask) {
    auto task = TestRenderTask::CreateTest();
    task->SetId("existing_task");
    m_queue->Enqueue(task);
    
    EXPECT_TRUE(m_queue->HasTask("existing_task"));
    EXPECT_FALSE(m_queue->HasTask("nonexistent_task"));
}

TEST_F(RenderQueueTest, GetStats) {
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    
    auto stats = m_queue->GetStats();
    EXPECT_EQ(stats.totalTasks, 1);
    EXPECT_EQ(stats.pendingTasks, 1);
}

TEST_F(RenderQueueTest, ResetStats) {
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    m_queue->Dequeue();
    
    m_queue->ResetStats();
    auto stats = m_queue->GetStats();
    EXPECT_EQ(stats.totalTasks, 0);
}

TEST_F(RenderQueueTest, PriorityMode) {
    m_queue->SetPriorityMode(true);
    EXPECT_TRUE(m_queue->IsPriorityModeEnabled());
    
    m_queue->SetPriorityMode(false);
    EXPECT_FALSE(m_queue->IsPriorityModeEnabled());
}

TEST_F(RenderQueueTest, PriorityOrdering) {
    m_queue->SetPriorityMode(true);
    
    auto lowTask = TestRenderTask::CreateTest();
    lowTask->SetId("low");
    lowTask->SetPriority(RenderTaskPriority::kLow);
    
    auto highTask = TestRenderTask::CreateTest();
    highTask->SetId("high");
    highTask->SetPriority(RenderTaskPriority::kHigh);
    
    m_queue->Enqueue(lowTask);
    m_queue->Enqueue(highTask);
    
    auto first = m_queue->Dequeue();
    EXPECT_EQ(first->GetId(), "high");
}

TEST_F(RenderQueueTest, PauseResume) {
    m_queue->Pause();
    EXPECT_TRUE(m_queue->IsPaused());
    
    m_queue->Resume();
    EXPECT_FALSE(m_queue->IsPaused());
}

TEST_F(RenderQueueTest, SetOnTaskEnqueued) {
    bool callbackCalled = false;
    m_queue->SetOnTaskEnqueued([&callbackCalled](const RenderTaskPtr&) {
        callbackCalled = true;
    });
    
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    
    EXPECT_TRUE(callbackCalled);
}

TEST_F(RenderQueueTest, SetOnTaskDequeued) {
    bool callbackCalled = false;
    m_queue->SetOnTaskDequeued([&callbackCalled](const RenderTaskPtr&) {
        callbackCalled = true;
    });
    
    auto task = TestRenderTask::CreateTest();
    m_queue->Enqueue(task);
    m_queue->Dequeue();
    
    EXPECT_TRUE(callbackCalled);
}
