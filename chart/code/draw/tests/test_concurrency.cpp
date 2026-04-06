#include <gtest/gtest.h>
#include "ogc/draw/thread_safe_engine.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/engine_pool.h"
#include "ogc/draw/draw_style.h"
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>

using namespace ogc::draw;

class ConcurrencyTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_device = new RasterImageDevice(800, 600, PixelFormat::kRGBA8888);
        auto engine = std::unique_ptr<DrawEngine>(new Simple2DEngine(m_device));
        m_safeEngine = std::unique_ptr<ThreadSafeEngine>(new ThreadSafeEngine(std::move(engine)));
    }

    void TearDown() override {
        m_safeEngine.reset();
        delete m_device;
    }

    RasterImageDevice* m_device;
    std::unique_ptr<ThreadSafeEngine> m_safeEngine;
};

TEST_F(ConcurrencyTest, ConcurrentBeginEnd) {
    const int numThreads = 8;
    const int iterations = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    std::atomic<int> failCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, &failCount, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                DrawResult result = m_safeEngine->Begin();
                if (result == DrawResult::kSuccess) {
                    successCount++;
                    m_safeEngine->End();
                } else {
                    failCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_GT(successCount.load(), 0);
}

TEST_F(ConcurrencyTest, ConcurrentDrawPoints) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int pointsPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, t, pointsPerThread]() {
            for (int i = 0; i < pointsPerThread; ++i) {
                double x = static_cast<double>(t * 200 + i);
                double y = static_cast<double>(100 + i);
                DrawStyle style;
                style.pen.color = Color::Red();
                
                double px[] = { x };
                double py[] = { y };
                DrawResult result = m_safeEngine->DrawPoints(px, py, 1, style);
                if (result == DrawResult::kSuccess) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), numThreads * pointsPerThread);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentDrawLines) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int linesPerThread = 50;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, t, linesPerThread]() {
            for (int i = 0; i < linesPerThread; ++i) {
                double x1 = static_cast<double>(t * 200);
                double y1 = static_cast<double>(i * 10);
                double x2 = x1 + 100;
                double y2 = y1 + 50;
                DrawStyle style;
                style.pen.color = Color::Blue();
                
                double px1[] = { x1 };
                double py1[] = { y1 };
                double px2[] = { x2 };
                double py2[] = { y2 };
                DrawResult result = m_safeEngine->DrawLines(px1, py1, px2, py2, 1, style);
                if (result == DrawResult::kSuccess) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), numThreads * linesPerThread);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentDrawRect) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int rectsPerThread = 50;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, t, rectsPerThread]() {
            for (int i = 0; i < rectsPerThread; ++i) {
                double x = static_cast<double>(t * 200 + i * 5);
                double y = static_cast<double>(i * 10);
                DrawStyle style;
                style.pen.color = Color::Green();
                style.brush.color = Color(0, 255, 0, 128);
                
                DrawResult result = m_safeEngine->DrawRect(x, y, 20, 15, style, true);
                if (result == DrawResult::kSuccess) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), numThreads * rectsPerThread);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentDrawCircle) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int circlesPerThread = 30;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, t, circlesPerThread]() {
            for (int i = 0; i < circlesPerThread; ++i) {
                double cx = static_cast<double>(t * 200 + 100);
                double cy = static_cast<double>(i * 20 + 50);
                DrawStyle style;
                style.pen.color = Color::Red();
                
                DrawResult result = m_safeEngine->DrawCircle(cx, cy, 10, style, false);
                if (result == DrawResult::kSuccess) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), numThreads * circlesPerThread);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentSetTransform) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 100;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                TransformMatrix matrix = TransformMatrix::Translate(t * 10.0, i * 1.0);
                m_safeEngine->SetTransform(matrix);
                
                TransformMatrix retrieved = m_safeEngine->GetTransform();
                (void)retrieved;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentSetOpacity) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 100;
    std::vector<std::thread> threads;
    std::atomic<double> lastOpacity(1.0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &lastOpacity, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                double opacity = 0.1 + (t * iterations + i) * 0.001;
                opacity = std::min(opacity, 1.0);
                m_safeEngine->SetOpacity(opacity);
                
                double retrieved = m_safeEngine->GetOpacity();
                lastOpacity.store(retrieved);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    double finalOpacity = lastOpacity.load();
    EXPECT_GE(finalOpacity, 0.0);
    EXPECT_LE(finalOpacity, 1.0);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentSaveRestore) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                m_safeEngine->Save();
                
                m_safeEngine->SetOpacity(0.5);
                TransformMatrix matrix = TransformMatrix::Rotate(t * 10.0);
                m_safeEngine->SetTransform(matrix);
                
                m_safeEngine->Restore();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentSetClipRect) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                double x = static_cast<double>(t * 200);
                double y = static_cast<double>(i * 10);
                m_safeEngine->SetClipRect(x, y, 100, 50);
                
                Region clip = m_safeEngine->GetClipRegion();
                (void)clip;
                
                m_safeEngine->ResetClip();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentClear) {
    const int numThreads = 4;
    const int iterations = 20;
    std::vector<std::thread> threads;
    std::atomic<int> clearCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &clearCount, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                m_safeEngine->Begin();
                
                Color color(t * 50, i * 10, 255 - t * 50);
                m_safeEngine->Clear(color);
                clearCount++;
                
                m_safeEngine->End();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(clearCount.load(), numThreads * iterations);
}

TEST_F(ConcurrencyTest, ConcurrentMixedOperations) {
    m_safeEngine->Begin();
    
    const int numThreads = 8;
    const int iterations = 50;
    std::vector<std::thread> threads;
    std::atomic<int> totalOps(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &totalOps, t, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                int opType = (t + i) % 5;
                
                switch (opType) {
                    case 0: {
                        double x[] = { static_cast<double>(t * 100 + i) };
                        double y[] = { static_cast<double>(i * 5) };
                        DrawStyle style;
                        m_safeEngine->DrawPoints(x, y, 1, style);
                        break;
                    }
                    case 1: {
                        DrawStyle style;
                        m_safeEngine->DrawRect(t * 50, i * 10, 20, 15, style, true);
                        break;
                    }
                    case 2: {
                        DrawStyle style;
                        m_safeEngine->DrawCircle(t * 100 + 50, i * 10 + 20, 10, style, false);
                        break;
                    }
                    case 3: {
                        TransformMatrix matrix = TransformMatrix::Translate(i, t);
                        m_safeEngine->SetTransform(matrix);
                        break;
                    }
                    case 4: {
                        m_safeEngine->SetOpacity(0.5 + (i % 5) * 0.1);
                        break;
                    }
                }
                totalOps++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(totalOps.load(), numThreads * iterations);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentDrawText) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int textsPerThread = 20;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &successCount, t, textsPerThread]() {
            Font font("Arial", 12);
            for (int i = 0; i < textsPerThread; ++i) {
                double x = static_cast<double>(t * 200 + i * 10);
                double y = static_cast<double>(i * 15 + 20);
                
                DrawResult result = m_safeEngine->DrawText(x, y, "Test", font, Color::Black());
                if (result == DrawResult::kSuccess) {
                    successCount++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), numThreads * textsPerThread);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, StressTest) {
    m_safeEngine->Begin();
    
    const int numThreads = 16;
    const int opsPerThread = 200;
    std::vector<std::thread> threads;
    std::atomic<int> totalOps(0);
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &totalOps, t, opsPerThread]() {
            for (int i = 0; i < opsPerThread; ++i) {
                DrawStyle style;
                style.pen.color = Color(t * 15, i * 2, 255 - t * 15);
                
                double x = static_cast<double>((t * 50 + i) % 800);
                double y = static_cast<double>((i * 3) % 600);
                
                double px[] = { x };
                double py[] = { y };
                m_safeEngine->DrawPoints(px, py, 1, style);
                totalOps++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    EXPECT_EQ(totalOps.load(), numThreads * opsPerThread);
    
    double opsPerMs = static_cast<double>(totalOps.load()) / duration.count();
    EXPECT_GT(opsPerMs, 0.0);
    
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, LockUnlockContention) {
    const int numThreads = 8;
    const int iterations = 100;
    std::vector<std::thread> threads;
    std::atomic<int> lockSuccess(0);
    std::atomic<int> lockFail(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &lockSuccess, &lockFail, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                if (m_safeEngine->TryLock()) {
                    lockSuccess++;
                    
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    
                    m_safeEngine->Unlock();
                } else {
                    lockFail++;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_GT(lockSuccess.load(), 0);
}

TEST_F(ConcurrencyTest, ConcurrentMeasureText) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 50;
    std::vector<std::thread> threads;
    std::atomic<int> measureCount(0);

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &measureCount, t, iterations]() {
            Font font("Arial", 12 + t * 2);
            for (int i = 0; i < iterations; ++i) {
                TextMetrics metrics = m_safeEngine->MeasureText("Test String", font);
                EXPECT_GE(metrics.width, 0);
                EXPECT_GE(metrics.height, 0);
                measureCount++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(measureCount.load(), numThreads * iterations);
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ConcurrentFlush) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    const int iterations = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, iterations]() {
            for (int i = 0; i < iterations; ++i) {
                m_safeEngine->Flush();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, NoDataRace) {
    m_safeEngine->Begin();
    
    const int numThreads = 4;
    std::vector<int> sharedData(100, 0);
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, &sharedData, t]() {
            for (int i = 0; i < 100; ++i) {
                m_safeEngine->Lock();
                sharedData[i]++;
                m_safeEngine->Unlock();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(sharedData[i], numThreads);
    }
    
    m_safeEngine->End();
}

TEST_F(ConcurrencyTest, ThreadLocalEngine) {
    const int numThreads = 4;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<RasterImageDevice>> devices;
    std::vector<std::unique_ptr<ThreadSafeEngine>> engines;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t]() {
            RasterImageDevice device(800, 600, PixelFormat::kRGBA8888);
            auto engine = std::unique_ptr<DrawEngine>(new Simple2DEngine(&device));
            auto safeEngine = std::unique_ptr<ThreadSafeEngine>(new ThreadSafeEngine(std::move(engine)));
            
            safeEngine->Begin();
            
            DrawStyle style;
            double x[] = { 100.0 };
            double y[] = { 100.0 };
            DrawResult result = safeEngine->DrawPoints(x, y, 1, style);
            EXPECT_EQ(result, DrawResult::kSuccess);
            
            safeEngine->End();
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}
