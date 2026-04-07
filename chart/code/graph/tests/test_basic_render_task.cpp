#include <gtest/gtest.h>
#include "ogc/graph/render/basic_render_task.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/raster_image_device.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::graph;
using ogc::Envelope;
using ogc::draw::RasterImageDevice;
using ogc::draw::DrawContext;
using ogc::draw::DrawContextPtr;
using ogc::draw::PixelFormat;
using ogc::draw::DrawResult;

class BasicRenderTaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_task = BasicRenderTask::Create();
        ASSERT_NE(m_task, nullptr);
        
        m_device = std::make_unique<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
        ASSERT_NE(m_device, nullptr);
        m_device->Initialize();
        
        m_context = DrawContext::Create(m_device.get());
        ASSERT_NE(m_context, nullptr);
    }
    
    void TearDown() override {
        m_context.reset();
        m_device.reset();
        m_task.reset();
    }
    
    BasicRenderTaskPtr m_task;
    std::unique_ptr<RasterImageDevice> m_device;
    DrawContextPtr m_context;
};

TEST_F(BasicRenderTaskTest, Create) {
    EXPECT_NE(m_task, nullptr);
}

TEST_F(BasicRenderTaskTest, CreateWithParams) {
    DrawParams params;
    auto task = BasicRenderTask::Create("test_id", params);
    ASSERT_NE(task, nullptr);
    EXPECT_EQ(task->GetId(), "test_id");
}

TEST_F(BasicRenderTaskTest, SetGetRenderFunction) {
    bool functionCalled = false;
    RenderFunction func = [&functionCalled](RenderTaskPtr, DrawContext&) {
        functionCalled = true;
        return true;
    };
    
    m_task->SetRenderFunction(func);
    auto retrieved = m_task->GetRenderFunction();
    EXPECT_TRUE(retrieved != nullptr);
}

TEST_F(BasicRenderTaskTest, SetGetDrawContext) {
    m_task->SetDrawContext(m_context.get());
    auto retrieved = m_task->GetDrawContext();
    EXPECT_EQ(retrieved, m_context.get());
}

TEST_F(BasicRenderTaskTest, ExecuteWithFunction) {
    bool functionCalled = false;
    m_task->SetRenderFunction([&functionCalled](RenderTaskPtr, DrawContext&) {
        functionCalled = true;
        return true;
    });
    m_task->SetDrawContext(m_context.get());
    
    bool result = m_task->Execute();
    EXPECT_TRUE(result);
    EXPECT_TRUE(functionCalled);
}

TEST_F(BasicRenderTaskTest, ExecuteWithoutFunction) {
    m_task->SetDrawContext(m_context.get());
    bool result = m_task->Execute();
    EXPECT_FALSE(result);
}

TEST_F(BasicRenderTaskTest, ExecuteWithoutContext) {
    bool functionCalled = false;
    m_task->SetRenderFunction([&functionCalled](RenderTaskPtr, DrawContext&) {
        functionCalled = true;
        return true;
    });
    
    bool result = m_task->Execute();
    EXPECT_FALSE(result);
    EXPECT_FALSE(functionCalled);
}

TEST_F(BasicRenderTaskTest, RenderFunctionReturnsFalse) {
    m_task->SetRenderFunction([](RenderTaskPtr, DrawContext&) {
        return false;
    });
    m_task->SetDrawContext(m_context.get());
    
    bool result = m_task->Execute();
    EXPECT_FALSE(result);
}

TEST_F(BasicRenderTaskTest, SetGetId) {
    m_task->SetId("my_task");
    EXPECT_EQ(m_task->GetId(), "my_task");
}

TEST_F(BasicRenderTaskTest, SetGetPriority) {
    m_task->SetPriority(RenderTaskPriority::kHigh);
    EXPECT_EQ(m_task->GetPriority(), RenderTaskPriority::kHigh);
}

TEST_F(BasicRenderTaskTest, SetGetExtent) {
    Envelope extent(0, 0, 100, 100);
    m_task->SetExtent(extent);
    auto retrieved = m_task->GetExtent();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 100);
}

TEST_F(BasicRenderTaskTest, SetGetProgress) {
    m_task->SetProgress(0.5);
    EXPECT_DOUBLE_EQ(m_task->GetProgress(), 0.5);
}

TEST_F(BasicRenderTaskTest, SetGetResult) {
    m_task->SetResult(DrawResult::kSuccess);
    EXPECT_EQ(m_task->GetResult(), DrawResult::kSuccess);
}

TEST_F(BasicRenderTaskTest, SetGetError) {
    m_task->SetError("Test error");
    EXPECT_EQ(m_task->GetError(), "Test error");
}

TEST_F(BasicRenderTaskTest, SetGetTag) {
    m_task->SetTag("my_tag");
    EXPECT_EQ(m_task->GetTag(), "my_tag");
}

TEST_F(BasicRenderTaskTest, SetGetTimeout) {
    m_task->SetTimeout(5000);
    EXPECT_EQ(m_task->GetTimeout(), 5000);
}

TEST_F(BasicRenderTaskTest, IsPending) {
    m_task->SetState(RenderTaskState::kPending);
    EXPECT_TRUE(m_task->IsPending());
}

TEST_F(BasicRenderTaskTest, IsRunning) {
    m_task->SetState(RenderTaskState::kRunning);
    EXPECT_TRUE(m_task->IsRunning());
}

TEST_F(BasicRenderTaskTest, IsCompleted) {
    m_task->SetState(RenderTaskState::kCompleted);
    EXPECT_TRUE(m_task->IsCompleted());
}

TEST_F(BasicRenderTaskTest, IsFailed) {
    m_task->SetState(RenderTaskState::kFailed);
    EXPECT_TRUE(m_task->IsFailed());
}

TEST_F(BasicRenderTaskTest, IsCancelled) {
    m_task->SetState(RenderTaskState::kCancelled);
    EXPECT_TRUE(m_task->IsCancelled());
}

TEST_F(BasicRenderTaskTest, Cancel) {
    m_task->Cancel();
    EXPECT_TRUE(m_task->IsCancelled());
}

TEST_F(BasicRenderTaskTest, ComplexRenderFunction) {
    int callCount = 0;
    m_task->SetRenderFunction([&callCount](RenderTaskPtr task, DrawContext& ctx) {
        callCount++;
        task->SetProgress(0.5);
        ctx.DrawPoint(128, 128);
        task->SetProgress(1.0);
        return true;
    });
    m_task->SetDrawContext(m_context.get());
    
    bool result = m_task->Execute();
    EXPECT_TRUE(result);
    EXPECT_EQ(callCount, 1);
    EXPECT_DOUBLE_EQ(m_task->GetProgress(), 1.0);
}
