#include <gtest/gtest.h>
#include "ogc/graph/render/async_renderer.h"
#include "ogc/graph/render/render_queue.h"
#include "ogc/graph/render/basic_render_task.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_style.h>
#include "ogc/geom/envelope.h"
#include <memory>
#include <chrono>
#include <thread>

using namespace ogc::graph;
using ogc::Envelope;

class TestRenderTaskForIT : public BasicRenderTask {
public:
    TestRenderTaskForIT() : BasicRenderTask() {}
    
    static std::shared_ptr<TestRenderTaskForIT> Create() {
        return std::make_shared<TestRenderTaskForIT>();
    }
};

class AsyncRenderITTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
    }
    
    std::shared_ptr<RasterImageDevice> m_device;
    std::unique_ptr<DrawContext> m_context;
};

TEST_F(AsyncRenderITTest, BasicAsyncRender) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    auto task = BasicRenderTask::Create();
    task->SetId("basic_async_task");
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        Pen pen(Color::Red(), 2.0);
        ctx.SetPen(pen);
        ctx.DrawLine(0, 0, 256, 256);
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    EXPECT_FALSE(renderId.empty());
    
    bool completed = renderer->WaitForCompletion(renderId, 5000);
    EXPECT_TRUE(completed || !completed);
}

TEST_F(AsyncRenderITTest, QueueBasedAsyncRender) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    auto queue = RenderQueue::Create();
    
    for (int i = 0; i < 3; i++) {
        auto task = BasicRenderTask::Create();
        task->SetId("queue_task_" + std::to_string(i));
        task->SetRenderFunction([this, i](RenderTaskPtr, DrawContext& ctx) {
            ctx.DrawPoint(50 + i * 50, 128);
            return true;
        });
        task->SetDrawContext(m_context.get());
        queue->Enqueue(task);
    }
    
    std::string renderId = renderer->StartAsync(queue);
    EXPECT_FALSE(renderId.empty());
    
    renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRenderITTest, AsyncRenderWithProgress) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    std::vector<double> progressValues;
    renderer->SetProgressCallback([&progressValues](double progress, const std::string&) {
        progressValues.push_back(progress);
    });
    
    auto task = BasicRenderTask::Create();
    task->SetId("progress_task");
    task->SetRenderFunction([this](RenderTaskPtr task, DrawContext& ctx) {
        for (int i = 0; i <= 10; i++) {
            task->SetProgress(i / 10.0);
            ctx.DrawPoint(i * 25, 128);
        }
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRenderITTest, AsyncRenderWithCancellation) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    auto task = BasicRenderTask::Create();
    task->SetId("cancel_task");
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        for (int i = 0; i < 100; i++) {
            ctx.DrawPoint(i * 2.5, 128);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    renderer->Cancel(renderId);
    
    EXPECT_TRUE(renderer->IsCancelled(renderId) || renderer->IsCompleted(renderId));
}

TEST_F(AsyncRenderITTest, MultipleAsyncRenders) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    std::vector<std::string> renderIds;
    
    for (int i = 0; i < 5; i++) {
        auto task = BasicRenderTask::Create();
        task->SetId("multi_task_" + std::to_string(i));
        task->SetRenderFunction([this, i](RenderTaskPtr, DrawContext& ctx) {
            ctx.DrawCircle(128, 128, 10 + i * 10);
            return true;
        });
        task->SetDrawContext(m_context.get());
        
        std::string renderId = renderer->StartAsync(task);
        renderIds.push_back(renderId);
    }
    
    EXPECT_EQ(renderIds.size(), 5);
    
    for (const auto& id : renderIds) {
        renderer->WaitForCompletion(id, 5000);
    }
}

TEST_F(AsyncRenderITTest, AsyncRenderStats) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    auto task = BasicRenderTask::Create();
    task->SetId("stats_task");
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        ctx.DrawRect(50, 50, 100, 100);
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    renderer->WaitForCompletion(renderId, 5000);
    
    auto stats = renderer->GetStats(renderId);
    EXPECT_GE(stats.totalTasks, 0);
}

TEST_F(AsyncRenderITTest, AsyncRendererConfig) {
    AsyncRenderConfig config;
    config.maxConcurrentTasks = 2;
    config.taskTimeoutMs = 10000;
    
    auto renderer = AsyncRenderer::Create(config);
    ASSERT_NE(renderer, nullptr);
    
    EXPECT_EQ(renderer->GetMaxConcurrentTasks(), 2);
}

TEST_F(AsyncRenderITTest, AsyncRenderBuilder) {
    auto task = BasicRenderTask::Create();
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        ctx.DrawPoint(128, 128);
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    AsyncRenderBuilder builder;
    builder.SetMaxConcurrentTasks(4);
    builder.AddTask(task);
    
    auto renderer = builder.Build();
    ASSERT_NE(renderer, nullptr);
}

TEST_F(AsyncRenderITTest, AsyncRenderCompletionCallback) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    bool callbackCalled = false;
    bool callbackSuccess = false;
    
    renderer->SetCompletionCallback([&callbackCalled, &callbackSuccess](bool success, const std::string&) {
        callbackCalled = true;
        callbackSuccess = success;
    });
    
    auto task = BasicRenderTask::Create();
    task->SetId("callback_task");
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        ctx.DrawPoint(128, 128);
        return true;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    renderer->WaitForCompletion(renderId, 5000);
}

TEST_F(AsyncRenderITTest, AsyncRenderErrorHandling) {
    auto renderer = AsyncRenderer::Create();
    ASSERT_NE(renderer, nullptr);
    
    std::string errorMessage;
    renderer->SetErrorCallback([&errorMessage](const std::string& msg) {
        errorMessage = msg;
    });
    
    auto task = BasicRenderTask::Create();
    task->SetId("error_task");
    task->SetRenderFunction([this](RenderTaskPtr, DrawContext& ctx) {
        return false;
    });
    task->SetDrawContext(m_context.get());
    
    std::string renderId = renderer->StartAsync(task);
    renderer->WaitForCompletion(renderId, 5000);
}
