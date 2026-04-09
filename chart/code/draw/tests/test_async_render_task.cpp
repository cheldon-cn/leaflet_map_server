#include <gtest/gtest.h>
#include <ogc/draw/async_render_task.h>
#include <ogc/draw/tile_device.h>
#include <ogc/draw/tile_based_engine.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/geometry.h>
#include <ogc/geom/point.h>
#include <thread>
#include <chrono>

class AsyncRenderTaskTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    ogc::draw::TileDevice* CreateDevice() {
        ogc::draw::TileDevice* dev = new ogc::draw::TileDevice(1024, 1024, 256);
        dev->Initialize();
        return dev;
    }

    void DestroyDevice(ogc::draw::TileDevice* dev) {
        if (dev) {
            dev->Finalize();
            delete dev;
        }
    }
};

TEST_F(AsyncRenderTaskTest, Construction) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    
    EXPECT_EQ(task.GetState(), ogc::draw::RenderState::kPending);
    EXPECT_FLOAT_EQ(task.GetProgress(), 0.0f);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, ExecuteEmpty) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    task.Execute();
    
    EXPECT_EQ(task.GetState(), ogc::draw::RenderState::kCompleted);
    EXPECT_FLOAT_EQ(task.GetProgress(), 1.0f);
    
    ogc::draw::RenderResult result = task.GetResult();
    EXPECT_EQ(result.state, ogc::draw::RenderState::kCompleted);
    EXPECT_EQ(result.drawResult, ogc::draw::DrawResult::kSuccess);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, ExecuteWithGeometry) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    auto rect1 = ogc::draw::RectGeometry::Create(100, 100, 50, 50);
    auto rect2 = ogc::draw::RectGeometry::Create(200, 200, 50, 50);
    geometries.push_back(rect1.get());
    geometries.push_back(rect2.get());
    
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    task.Execute();
    
    EXPECT_EQ(task.GetState(), ogc::draw::RenderState::kCompleted);
    
    ogc::draw::RenderResult result = task.GetResult();
    EXPECT_EQ(result.state, ogc::draw::RenderState::kCompleted);
    EXPECT_EQ(result.geometryCount, 2);
    EXPECT_GT(result.elapsedMs, 0.0);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, Cancel) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    task.Cancel();
    
    EXPECT_EQ(task.GetState(), ogc::draw::RenderState::kPending);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, ProgressCallback) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    
    float lastProgress = 0.0f;
    task.SetProgressCallback([&lastProgress](float progress, const std::string& stage) {
        lastProgress = progress;
    });
    
    task.Execute();
    
    EXPECT_FLOAT_EQ(lastProgress, 1.0f);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, CompletionCallback) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    
    bool callbackCalled = false;
    ogc::draw::RenderResult callbackResult;
    task.SetCompletionCallback([&callbackCalled, &callbackResult](const ogc::draw::RenderResult& result) {
        callbackCalled = true;
        callbackResult = result;
    });
    
    task.Execute();
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(callbackResult.state, ogc::draw::RenderState::kCompleted);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderTaskTest, MultipleGeometryTypes) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    auto rect = ogc::draw::RectGeometry::Create(100, 100, 50, 50);
    auto circle = ogc::draw::CircleGeometry::Create(ogc::draw::Point(200, 200), 50);
    auto ellipse = ogc::draw::EllipseGeometry::Create(ogc::draw::Point(300, 300), 30, 50);
    
    geometries.push_back(rect.get());
    geometries.push_back(circle.get());
    geometries.push_back(ellipse.get());
    
    ogc::draw::DrawStyle style;
    
    ogc::draw::TileAsyncRenderTask task(device, &engine, geometries, style);
    task.Execute();
    
    EXPECT_EQ(task.GetState(), ogc::draw::RenderState::kCompleted);
    
    ogc::draw::RenderResult result = task.GetResult();
    EXPECT_EQ(result.geometryCount, 3);
    
    DestroyDevice(device);
}
