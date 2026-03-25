#include <gtest/gtest.h>
#include "ogc/draw/tile_renderer.h"
#include "ogc/draw/tile_device.h"
#include "ogc/draw/memory_tile_cache.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/async_renderer.h"
#include "ogc/draw/render_queue.h"
#include "ogc/draw/render_task.h"
#include "ogc/envelope.h"

#include <chrono>
#include <memory>
#include <vector>
#include <cstdint>

using namespace ogc::draw;
using ogc::Envelope;

class GraphPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = RasterImageDevice::Create(256, 256, 4);
        ASSERT_NE(device, nullptr);
        device->Initialize();
    }
    
    void TearDown() override {
        if (device) {
            device->Finalize();
        }
    }
    
    std::shared_ptr<RasterImageDevice> device;
};

TEST_F(GraphPerformanceTest, DrawPoint_ResponseTime) {
    const int iterations = 1000;
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.stroke.color = Color::Red().GetRGBA();
    style.stroke.width = 1.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        device->DrawPoint(128, 128, style);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    device->EndDraw();
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 100.0) << "Average draw point time should be < 100us";
}

TEST_F(GraphPerformanceTest, DrawLine_ResponseTime) {
    const int iterations = 1000;
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.stroke.color = Color::Blue().GetRGBA();
    style.stroke.width = 2.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        device->DrawLine(0, 0, 256, 256, style);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    device->EndDraw();
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 150.0) << "Average draw line time should be < 150us";
}

TEST_F(GraphPerformanceTest, DrawRect_ResponseTime) {
    const int iterations = 1000;
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.stroke.color = Color::Green().GetRGBA();
    style.stroke.width = 1.0;
    style.fill.color = Color::Yellow().GetRGBA();
    style.fill.visible = true;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        device->DrawRect(10, 10, 236, 236, style);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    device->EndDraw();
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 200.0) << "Average draw rect time should be < 200us";
}

TEST_F(GraphPerformanceTest, DrawPolygon_ResponseTime) {
    const int iterations = 500;
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    device->BeginDraw(params);
    
    DrawStyle style;
    style.stroke.color = Color::Black().GetRGBA();
    style.stroke.width = 1.0;
    style.fill.color = Color::Gray().GetRGBA();
    style.fill.visible = true;
    
    double x[] = {10, 50, 100, 150, 200, 150, 100, 50};
    double y[] = {128, 50, 50, 100, 128, 200, 200, 150};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        device->DrawPolygon(x, y, 8, style);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    device->EndDraw();
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 500.0) << "Average draw polygon time should be < 500us";
}

TEST_F(GraphPerformanceTest, Clear_ResponseTime) {
    const int iterations = 100;
    
    DrawParams params;
    params.pixel_width = 256;
    params.pixel_height = 256;
    params.extent = Envelope(0, 0, 256, 256);
    
    device->BeginDraw(params);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        device->Clear(Color::White());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    device->EndDraw();
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 1000.0) << "Average clear time should be < 1ms";
}

class TileCachePerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_shared<MemoryTileCache>(10 * 1024 * 1024);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::shared_ptr<MemoryTileCache> cache;
    
    std::vector<uint8_t> CreateTestData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(i % 256);
        }
        return data;
    }
};

TEST_F(TileCachePerformanceTest, PutTile_ResponseTime) {
    const int iterations = 1000;
    auto data = CreateTestData(10240);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        TileKey key(i / 100, i % 100, 10);
        cache->PutTile(key, data);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 100.0) << "Average put tile time should be < 100us";
}

TEST_F(TileCachePerformanceTest, GetTile_ResponseTime) {
    const int iterations = 1000;
    auto data = CreateTestData(10240);
    
    for (int i = 0; i < iterations; ++i) {
        TileKey key(i / 100, i % 100, 10);
        cache->PutTile(key, data);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        TileKey key(i / 100, i % 100, 10);
        cache->GetTile(key);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 50.0) << "Average get tile time should be < 50us";
}

TEST_F(TileCachePerformanceTest, HasTile_ResponseTime) {
    const int iterations = 1000;
    auto data = CreateTestData(10240);
    
    for (int i = 0; i < iterations; ++i) {
        TileKey key(i / 100, i % 100, 10);
        cache->PutTile(key, data);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        TileKey key(i / 100, i % 100, 10);
        cache->HasTile(key);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_us, 20.0) << "Average has tile time should be < 20us";
}

TEST_F(TileCachePerformanceTest, CacheEviction_Performance) {
    auto smallCache = std::make_shared<MemoryTileCache>(10240);
    auto data = CreateTestData(1024);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        TileKey key(0, i, 0);
        smallCache->PutTile(key, data);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 100) << "Cache eviction should complete in < 100ms";
    EXPECT_LE(smallCache->GetTileCount(), 10);
}

class TileDevicePerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        tileDevice = TileDevice::Create(256);
        ASSERT_NE(tileDevice, nullptr);
        tileDevice->Initialize();
    }
    
    void TearDown() override {
        if (tileDevice) {
            tileDevice->Finalize();
        }
    }
    
    TileDevicePtr tileDevice;
};

TEST_F(TileDevicePerformanceTest, RenderTile_ResponseTime) {
    const int iterations = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        tileDevice->BeginTile(i % 10, i / 10, 5);
        tileDevice->Clear(Color::White());
        
        DrawStyle style;
        style.stroke.color = Color::Red().GetRGBA();
        style.stroke.width = 2.0;
        
        tileDevice->DrawRect(10, 10, 236, 236, style);
        tileDevice->EndTile();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_ms = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_ms, 10.0) << "Average render tile time should be < 10ms";
}

TEST_F(TileDevicePerformanceTest, RenderMultipleZoomLevels_Performance) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int z = 0; z <= 5; ++z) {
        int maxTile = 1 << z;
        for (int x = 0; x < maxTile; ++x) {
            for (int y = 0; y < maxTile; ++y) {
                tileDevice->BeginTile(x, y, z);
                tileDevice->Clear(Color::White());
                
                DrawStyle style;
                style.stroke.color = Color::Blue().GetRGBA();
                style.stroke.width = 1.0;
                
                tileDevice->DrawRect(0, 0, 256, 256, style);
                tileDevice->EndTile();
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 5000) << "Render multiple zoom levels should complete in < 5s";
}

class RenderQueuePerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        queue = RenderQueue::Create();
    }
    
    void TearDown() override {
        queue.reset();
    }
    
    RenderQueuePtr queue;
};

class FastRenderTask : public RenderTask {
public:
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<FastRenderTask> Create() {
        return std::make_shared<FastRenderTask>();
    }
};

TEST_F(RenderQueuePerformanceTest, Enqueue_Performance) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto task = FastRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        queue->Enqueue(task);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count() * 1000) / iterations;
    EXPECT_LT(avg_time_us, 50.0) << "Average enqueue time should be < 50us";
}

TEST_F(RenderQueuePerformanceTest, Dequeue_Performance) {
    const int iterations = 10000;
    
    for (int i = 0; i < iterations; ++i) {
        auto task = FastRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        queue->Enqueue(task);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        queue->Dequeue();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count() * 1000) / iterations;
    EXPECT_LT(avg_time_us, 50.0) << "Average dequeue time should be < 50us";
}

TEST_F(RenderQueuePerformanceTest, PriorityOrdering_Performance) {
    const int iterations = 1000;
    queue->SetPriorityMode(true);
    
    for (int i = 0; i < iterations; ++i) {
        auto task = FastRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        task->SetPriority(static_cast<RenderTaskPriority>(i % 4));
        queue->Enqueue(task);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        queue->Dequeue();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 100) << "Priority dequeue should complete in < 100ms";
}

class SymbolizerPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        lineSymbolizer = LineSymbolizer::Create();
        polygonSymbolizer = PolygonSymbolizer::Create();
        pointSymbolizer = PointSymbolizer::Create();
    }
    
    void TearDown() override {
        lineSymbolizer.reset();
        polygonSymbolizer.reset();
        pointSymbolizer.reset();
    }
    
    LineSymbolizerPtr lineSymbolizer;
    PolygonSymbolizerPtr polygonSymbolizer;
    PointSymbolizerPtr pointSymbolizer;
};

TEST_F(SymbolizerPerformanceTest, LineSymbolizer_IsVisibleAtScale) {
    const int iterations = 10000;
    
    lineSymbolizer->SetMinScale(1000.0);
    lineSymbolizer->SetMaxScale(100000.0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        double scale = 1000.0 + (i % 100000);
        lineSymbolizer->IsVisibleAtScale(scale);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ns = static_cast<double>(duration.count() * 1000) / iterations;
    EXPECT_LT(avg_time_ns, 100.0) << "Average IsVisibleAtScale time should be < 100ns";
}

TEST_F(SymbolizerPerformanceTest, Symbolizer_Clone) {
    const int iterations = 1000;
    
    lineSymbolizer->SetWidth(2.0);
    lineSymbolizer->SetColor(0xFF0000);
    lineSymbolizer->SetDashStyle(DashStyle::kDash);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto cloned = lineSymbolizer->Clone();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_us = static_cast<double>(duration.count() * 1000) / iterations;
    EXPECT_LT(avg_time_us, 100.0) << "Average clone time should be < 100us";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
