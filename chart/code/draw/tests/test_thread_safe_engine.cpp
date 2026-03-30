#include <gtest/gtest.h>
#include "ogc/draw/thread_safe_engine.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <thread>
#include <vector>
#include <memory>

using namespace ogc::draw;

class ThreadSafeEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_rasterDevice = new RasterImageDevice(800, 600, PixelFormat::kRGBA8888);
        auto engine = std::unique_ptr<DrawEngine>(new Simple2DEngine(m_rasterDevice));
        m_safeEngine = std::unique_ptr<ThreadSafeEngine>(new ThreadSafeEngine(std::move(engine)));
    }
    
    void TearDown() override {
        m_safeEngine.reset();
        delete m_rasterDevice;
        m_rasterDevice = nullptr;
    }
    
    RasterImageDevice* m_rasterDevice = nullptr;
    std::unique_ptr<ThreadSafeEngine> m_safeEngine;
};

TEST_F(ThreadSafeEngineTest, Create) {
    EXPECT_NE(m_safeEngine.get(), nullptr);
}

TEST_F(ThreadSafeEngineTest, GetName) {
    auto name = m_safeEngine->GetName();
    EXPECT_FALSE(name.empty());
}

TEST_F(ThreadSafeEngineTest, GetType) {
    auto type = m_safeEngine->GetType();
    EXPECT_EQ(type, EngineType::kSimple2D);
}

TEST_F(ThreadSafeEngineTest, GetDevice) {
    auto device = m_safeEngine->GetDevice();
    EXPECT_NE(device, nullptr);
}

TEST_F(ThreadSafeEngineTest, BeginEnd) {
    auto result = m_safeEngine->Begin();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(m_safeEngine->IsActive());
    
    m_safeEngine->End();
    EXPECT_FALSE(m_safeEngine->IsActive());
}

TEST_F(ThreadSafeEngineTest, SetTransform) {
    m_safeEngine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_safeEngine->SetTransform(matrix);
    
    auto retrieved = m_safeEngine->GetTransform();
    EXPECT_DOUBLE_EQ(retrieved.m[0][2], 10);
    EXPECT_DOUBLE_EQ(retrieved.m[1][2], 20);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, ResetTransform) {
    m_safeEngine->Begin();
    
    TransformMatrix matrix = TransformMatrix::Translate(10, 20);
    m_safeEngine->SetTransform(matrix);
    m_safeEngine->ResetTransform();
    
    auto retrieved = m_safeEngine->GetTransform();
    EXPECT_TRUE(retrieved.IsIdentity());
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SetOpacity) {
    m_safeEngine->Begin();
    m_safeEngine->SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(m_safeEngine->GetOpacity(), 0.5);
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SetTolerance) {
    m_safeEngine->Begin();
    m_safeEngine->SetTolerance(0.01);
    EXPECT_DOUBLE_EQ(m_safeEngine->GetTolerance(), 0.01);
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SetAntialiasing) {
    m_safeEngine->Begin();
    m_safeEngine->SetAntialiasingEnabled(true);
    EXPECT_TRUE(m_safeEngine->IsAntialiasingEnabled());
    
    m_safeEngine->SetAntialiasingEnabled(false);
    EXPECT_FALSE(m_safeEngine->IsAntialiasingEnabled());
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SaveRestore) {
    m_safeEngine->Begin();
    
    m_safeEngine->SetOpacity(0.5);
    m_safeEngine->Save();
    
    m_safeEngine->SetOpacity(1.0);
    EXPECT_DOUBLE_EQ(m_safeEngine->GetOpacity(), 1.0);
    
    m_safeEngine->Restore();
    EXPECT_DOUBLE_EQ(m_safeEngine->GetOpacity(), 0.5);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SetClipRect) {
    m_safeEngine->Begin();
    m_safeEngine->SetClipRect(10, 20, 100, 50);
    
    auto clip = m_safeEngine->GetClipRegion();
    EXPECT_FALSE(clip.IsEmpty());
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, ResetClip) {
    m_safeEngine->Begin();
    m_safeEngine->SetClipRect(10, 20, 100, 50);
    m_safeEngine->ResetClip();
    
    auto clip = m_safeEngine->GetClipRegion();
    EXPECT_TRUE(clip.IsEmpty());
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, DrawPoints) {
    m_safeEngine->Begin();
    
    double x[] = {100, 200, 300};
    double y[] = {100, 200, 300};
    DrawStyle style;
    
    auto result = m_safeEngine->DrawPoints(x, y, 3, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, DrawLines) {
    m_safeEngine->Begin();
    
    double x1[] = {0, 100};
    double y1[] = {0, 100};
    double x2[] = {100, 200};
    double y2[] = {100, 200};
    DrawStyle style;
    
    auto result = m_safeEngine->DrawLines(x1, y1, x2, y2, 2, style);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, DrawRect) {
    m_safeEngine->Begin();
    
    DrawStyle style;
    auto result = m_safeEngine->DrawRect(50, 50, 100, 80, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, DrawCircle) {
    m_safeEngine->Begin();
    
    DrawStyle style;
    auto result = m_safeEngine->DrawCircle(400, 300, 50, style, true);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, Clear) {
    m_safeEngine->Begin();
    
    Color color(255, 255, 255);
    m_safeEngine->Clear(color);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, Flush) {
    m_safeEngine->Begin();
    m_safeEngine->Flush();
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, SupportsFeature) {
    bool supports = m_safeEngine->SupportsFeature("cpu_rendering");
    (void)supports;
}

TEST_F(ThreadSafeEngineTest, MeasureText) {
    m_safeEngine->Begin();
    
    Font font("Arial", 12);
    auto metrics = m_safeEngine->MeasureText("Hello World", font);
    
    EXPECT_GE(metrics.width, 0);
    EXPECT_GE(metrics.height, 0);
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, ManualLock) {
    m_safeEngine->Lock();
    m_safeEngine->Unlock();
}

TEST_F(ThreadSafeEngineTest, TryLock) {
    bool locked = m_safeEngine->TryLock();
    EXPECT_TRUE(locked);
    if (locked) {
        m_safeEngine->Unlock();
    }
}

TEST_F(ThreadSafeEngineTest, MultiThreadedAccess) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 100;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                double x = 100.0 + t * 10 + i;
                double y = 100.0 + t * 10 + i;
                DrawStyle style;
                
                m_safeEngine->DrawCircle(x, y, 5, style, true);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, MultiThreadedTransform) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t]() {
            TransformMatrix matrix = TransformMatrix::Translate(t * 10.0, t * 20.0);
            m_safeEngine->SetTransform(matrix);
            auto retrieved = m_safeEngine->GetTransform();
            (void)retrieved;
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    m_safeEngine->End();
}

TEST_F(ThreadSafeEngineTest, GetWrappedEngine) {
    auto wrapped = m_safeEngine->GetWrappedEngine();
    EXPECT_NE(wrapped, nullptr);
}
