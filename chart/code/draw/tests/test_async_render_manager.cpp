#include <gtest/gtest.h>
#include <ogc/draw/async_render_manager.h>
#include <ogc/draw/tile_device.h>
#include <ogc/draw/tile_based_engine.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/geometry.h>
#include <ogc/point.h>
#include <thread>
#include <chrono>

class AsyncRenderManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = &ogc::draw::AsyncRenderManager::Instance();
        manager->SetMaxConcurrentTasks(2);
    }

    void TearDown() override {
        manager->CancelAllTasks();
        manager->WaitForAll(1000);
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

    ogc::draw::AsyncRenderManager* manager;
};

TEST_F(AsyncRenderManagerTest, Instance) {
    ogc::draw::AsyncRenderManager& instance1 = ogc::draw::AsyncRenderManager::Instance();
    ogc::draw::AsyncRenderManager& instance2 = ogc::draw::AsyncRenderManager::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(AsyncRenderManagerTest, SubmitTask) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    auto rect = ogc::draw::RectGeometry::Create(100, 100, 50, 50);
    geometries.push_back(rect.get());
    
    ogc::draw::DrawStyle style;
    
    auto task = manager->SubmitRenderTask(device, &engine, geometries, style);
    EXPECT_NE(task, nullptr);
    EXPECT_EQ(task->GetState(), ogc::draw::RenderState::kPending);
    
    task->Wait(5000);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderManagerTest, TaskPriority) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    auto rect = ogc::draw::RectGeometry::Create(100, 100, 50, 50);
    geometries.push_back(rect.get());
    
    ogc::draw::DrawStyle style;
    
    auto lowTask = manager->SubmitRenderTask(device, &engine, geometries, style, ogc::draw::TaskPriority::kLow);
    auto highTask = manager->SubmitRenderTask(device, &engine, geometries, style, ogc::draw::TaskPriority::kHigh);
    
    EXPECT_NE(lowTask, nullptr);
    EXPECT_NE(highTask, nullptr);
    
    lowTask->Wait(5000);
    highTask->Wait(5000);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderManagerTest, CancelAllTasks) {
    ogc::draw::TileDevice* device = CreateDevice();
    ogc::draw::TileBasedEngine engine(device);
    
    std::vector<ogc::Geometry*> geometries;
    auto rect = ogc::draw::RectGeometry::Create(100, 100, 50, 50);
    geometries.push_back(rect.get());
    
    ogc::draw::DrawStyle style;
    
    manager->SubmitRenderTask(device, &engine, geometries, style);
    manager->SubmitRenderTask(device, &engine, geometries, style);
    
    manager->CancelAllTasks();
    
    manager->WaitForAll(1000);
    
    DestroyDevice(device);
}

TEST_F(AsyncRenderManagerTest, MultipleGeometryTypes) {
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
    
    auto task = manager->SubmitRenderTask(device, &engine, geometries, style);
    EXPECT_NE(task, nullptr);
    
    task->Wait(5000);
    
    DestroyDevice(device);
}
