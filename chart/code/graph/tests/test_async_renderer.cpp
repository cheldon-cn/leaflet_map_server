#include <gtest/gtest.h>
#include "ogc/draw/async_renderer.h"
#include "ogc/draw/render_task.h"
#include "ogc/draw/render_queue.h"
#include <memory>

using namespace ogc::draw;

class TestRenderTaskForAsync : public RenderTask {
public:
    TestRenderTaskForAsync() : RenderTask() {}
    
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetProgress(0.5);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<TestRenderTaskForAsync> CreateTest() {
        return std::make_shared<TestRenderTaskForAsync>();
    }
};

class AsyncRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_renderer = AsyncRenderer::Create();
        ASSERT_NE(m_renderer, nullptr);
    }
    
    void TearDown() override {
        m_renderer.reset();
    }
    
    AsyncRendererPtr m_renderer;
};

TEST_F(AsyncRendererTest, Create) {
    EXPECT_NE(m_renderer, nullptr);
}

TEST_F(AsyncRendererTest, CreateWithConfig) {
    auto config = AsyncRenderConfig::HighPerformance();
    auto renderer = AsyncRenderer::Create(config);
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRendererTest, SetGetConfig) {
    AsyncRenderConfig config;
    config.maxConcurrentTasks = 8;
    m_renderer->SetConfig(config);
    
    auto retrieved = m_renderer->GetConfig();
    EXPECT_EQ(retrieved.maxConcurrentTasks, 8);
}

TEST_F(AsyncRendererTest, DefaultConfig) {
    auto config = AsyncRenderConfig::Default();
    EXPECT_EQ(config.maxConcurrentTasks, 4);
    EXPECT_EQ(config.taskTimeoutMs, 30000);
}

TEST_F(AsyncRendererTest, HighPerformanceConfig) {
    auto config = AsyncRenderConfig::HighPerformance();
    EXPECT_EQ(config.maxConcurrentTasks, 8);
    EXPECT_EQ(config.taskTimeoutMs, 60000);
}

TEST_F(AsyncRendererTest, SafeModeConfig) {
    auto config = AsyncRenderConfig::SafeMode();
    EXPECT_EQ(config.maxConcurrentTasks, 2);
    EXPECT_EQ(config.taskTimeoutMs, 10000);
}

TEST_F(AsyncRendererTest, SetMaxConcurrentTasks) {
    m_renderer->SetMaxConcurrentTasks(16);
    EXPECT_EQ(m_renderer->GetMaxConcurrentTasks(), 16);
}

TEST_F(AsyncRendererTest, SetProgressCallback) {
    bool callbackSet = false;
    m_renderer->SetProgressCallback([&callbackSet](double, const std::string&) {
        callbackSet = true;
    });
}

TEST_F(AsyncRendererTest, SetCompletionCallback) {
    bool callbackSet = false;
    m_renderer->SetCompletionCallback([&callbackSet](bool, const std::string&) {
        callbackSet = true;
    });
}

TEST_F(AsyncRendererTest, SetErrorCallback) {
    bool callbackSet = false;
    m_renderer->SetErrorCallback([&callbackSet](const std::string&) {
        callbackSet = true;
    });
}

TEST_F(AsyncRendererTest, StartAsyncWithTask) {
    auto task = TestRenderTaskForAsync::CreateTest();
    task->SetId("test_task");
    
    std::string renderId = m_renderer->StartAsync(task);
    EXPECT_FALSE(renderId.empty());
    
    m_renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRendererTest, StartAsyncWithQueue) {
    auto queue = RenderQueue::Create();
    auto task = TestRenderTaskForAsync::CreateTest();
    task->SetId("queue_task");
    queue->Enqueue(task);
    
    std::string renderId = m_renderer->StartAsync(queue);
    EXPECT_FALSE(renderId.empty());
    
    m_renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRendererTest, Cancel) {
    auto task = TestRenderTaskForAsync::CreateTest();
    std::string renderId = m_renderer->StartAsync(task);
    
    m_renderer->Cancel(renderId);
    EXPECT_TRUE(m_renderer->IsCancelled(renderId));
}

TEST_F(AsyncRendererTest, CancelAll) {
    auto task1 = TestRenderTaskForAsync::CreateTest();
    auto task2 = TestRenderTaskForAsync::CreateTest();
    
    m_renderer->StartAsync(task1);
    m_renderer->StartAsync(task2);
    
    m_renderer->CancelAll();
}

TEST_F(AsyncRendererTest, GetProgress) {
    auto task = TestRenderTaskForAsync::CreateTest();
    std::string renderId = m_renderer->StartAsync(task);
    
    double progress = m_renderer->GetProgress(renderId);
    EXPECT_GE(progress, 0.0);
    EXPECT_LE(progress, 1.0);
    
    m_renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRendererTest, GetStats) {
    auto task = TestRenderTaskForAsync::CreateTest();
    std::string renderId = m_renderer->StartAsync(task);
    
    auto stats = m_renderer->GetStats(renderId);
    EXPECT_GE(stats.totalTasks, 0);
    
    m_renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRendererTest, PauseResume) {
    auto task = TestRenderTaskForAsync::CreateTest();
    std::string renderId = m_renderer->StartAsync(task);
    
    m_renderer->Pause(renderId);
    EXPECT_TRUE(m_renderer->IsPaused(renderId));
    
    m_renderer->Resume(renderId);
    EXPECT_FALSE(m_renderer->IsPaused(renderId));
    
    m_renderer->Cancel(renderId);
}

TEST_F(AsyncRendererTest, GetActiveRenderCount) {
    size_t count = m_renderer->GetActiveRenderCount();
    EXPECT_GE(count, 0);
}

TEST_F(AsyncRendererTest, GetActiveRenderIds) {
    auto ids = m_renderer->GetActiveRenderIds();
}

TEST_F(AsyncRendererTest, Shutdown) {
    m_renderer->Shutdown();
    EXPECT_TRUE(m_renderer->IsShutdown());
}

class AsyncRenderBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_builder = std::make_unique<AsyncRenderBuilder>();
    }
    
    void TearDown() override {
        m_builder.reset();
    }
    
    std::unique_ptr<AsyncRenderBuilder> m_builder;
};

TEST_F(AsyncRenderBuilderTest, SetMaxConcurrentTasks) {
    m_builder->SetMaxConcurrentTasks(8);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
    EXPECT_EQ(renderer->GetMaxConcurrentTasks(), 8);
}

TEST_F(AsyncRenderBuilderTest, SetTaskTimeout) {
    m_builder->SetTaskTimeout(60000);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, EnableProgressReporting) {
    m_builder->EnableProgressReporting(true);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, EnableCancellation) {
    m_builder->EnableCancellation(true);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, AddTask) {
    auto task = TestRenderTaskForAsync::CreateTest();
    m_builder->AddTask(task);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, AddTasks) {
    std::vector<RenderTaskPtr> tasks;
    tasks.push_back(TestRenderTaskForAsync::CreateTest());
    tasks.push_back(TestRenderTaskForAsync::CreateTest());
    
    m_builder->AddTasks(tasks);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, SetQueue) {
    auto queue = RenderQueue::Create();
    m_builder->SetQueue(queue);
    auto renderer = m_builder->Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderBuilderTest, BuildAndStart) {
    auto task = TestRenderTaskForAsync::CreateTest();
    m_builder->AddTask(task);
    
    std::string renderId = m_builder->BuildAndStart();
    EXPECT_FALSE(renderId.empty());
}
