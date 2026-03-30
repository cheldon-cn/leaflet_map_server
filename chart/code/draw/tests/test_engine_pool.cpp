#include <gtest/gtest.h>
#include "ogc/draw/engine_pool.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"

using namespace ogc::draw;

class EnginePoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& pool = EnginePool::Instance();
        pool.Shutdown();
    }
    
    void TearDown() override {
        auto& pool = EnginePool::Instance();
        pool.Shutdown();
    }
};

TEST_F(EnginePoolTest, Singleton) {
    auto& pool1 = EnginePool::Instance();
    auto& pool2 = EnginePool::Instance();
    EXPECT_EQ(&pool1, &pool2);
}

TEST_F(EnginePoolTest, Initialize) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    EXPECT_EQ(pool.GetPoolSize(EngineType::kSimple2D), 0);
}

TEST_F(EnginePoolTest, SetEngineFactory) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    auto engine = pool.Acquire(EngineType::kSimple2D);
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(engine->GetType(), EngineType::kSimple2D);
}

TEST_F(EnginePoolTest, AcquireAndRelease) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    auto engine1 = pool.Acquire(EngineType::kSimple2D);
    ASSERT_NE(engine1, nullptr);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 0);
    
    pool.Release(engine1);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 1);
}

TEST_F(EnginePoolTest, Warmup) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 4);
}

TEST_F(EnginePoolTest, MaxPoolSize) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 2);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 2);
    
    pool.SetMaxPoolSize(EngineType::kSimple2D, 4);
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 4);
}

TEST_F(EnginePoolTest, Clear) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 4);
    
    pool.Clear(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 0);
}

TEST_F(EnginePoolTest, EngineGuard) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    {
        auto guard = pool.AcquireGuard(EngineType::kSimple2D);
        EXPECT_TRUE(guard);
        EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 0);
    }
    
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 1);
}

TEST_F(EnginePoolTest, EngineGuardMove) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    {
        auto guard1 = pool.AcquireGuard(EngineType::kSimple2D);
        EXPECT_TRUE(guard1);
        
        auto guard2 = std::move(guard1);
        EXPECT_TRUE(guard2);
        EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 0);
    }
    
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 1);
}

TEST_F(EnginePoolTest, Shutdown) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 4);
    
    pool.Shutdown();
    EXPECT_EQ(pool.GetPoolSize(EngineType::kSimple2D), 0);
}

TEST_F(EnginePoolTest, AcquireWithoutFactory) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 4);
    
    auto engine = pool.Acquire(EngineType::kSimple2D);
    EXPECT_EQ(engine, nullptr);
}

TEST_F(EnginePoolTest, MultipleEngineTypes) {
    auto& pool = EnginePool::Instance();
    pool.Initialize(EngineType::kSimple2D, 2);
    pool.Initialize(EngineType::kVector, 3);
    
    pool.SetEngineFactory(EngineType::kSimple2D, []() {
        auto device = new RasterImageDevice(100, 100);
        return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
    });
    
    pool.Warmup(EngineType::kSimple2D);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kSimple2D), 2);
    EXPECT_EQ(pool.GetAvailableCount(EngineType::kVector), 0);
}
