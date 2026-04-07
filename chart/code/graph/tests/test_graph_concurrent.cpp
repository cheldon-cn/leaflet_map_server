#include <gtest/gtest.h>
#include <ogc/cache/tile/memory_tile_cache.h>
#include <ogc/cache/tile/tile_key.h>
#include <ogc/graph/render/render_queue.h>
#include <ogc/graph/render/render_task.h>
#include <ogc/graph/render/async_renderer.h>
#include <ogc/draw/draw_result.h>

#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>

using namespace ogc::graph;
using ogc::cache::TileKey;
using ogc::cache::TileData;
using ogc::cache::MemoryTileCache;
using ogc::draw::DrawResult;

class ConcurrentTileCacheTest : public ::testing::Test {
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

TEST_F(ConcurrentTileCacheTest, ConcurrentPut) {
    const int num_threads = 8;
    const int tiles_per_thread = 100;
    std::atomic<int> success_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < tiles_per_thread; ++i) {
                TileKey key(t * tiles_per_thread + i, 0, 0);
                auto data = CreateTestData(1024);
                if (cache->PutTile(key, data)) {
                    success_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count, num_threads * tiles_per_thread);
}

TEST_F(ConcurrentTileCacheTest, ConcurrentGet) {
    const int num_tiles = 500;
    
    for (int i = 0; i < num_tiles; ++i) {
        TileKey key(i, 0, 0);
        auto data = CreateTestData(1024);
        cache->PutTile(key, data);
    }
    
    const int num_threads = 8;
    std::atomic<int> hit_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < num_tiles; ++i) {
                TileKey key(i, 0, 0);
                TileData tile = cache->GetTile(key);
                if (tile.IsValid()) {
                    hit_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(hit_count, num_threads * num_tiles);
}

TEST_F(ConcurrentTileCacheTest, ConcurrentPutAndGet) {
    const int num_threads = 8;
    const int operations_per_thread = 100;
    std::atomic<int> put_count{0};
    std::atomic<int> get_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                if (i % 2 == 0) {
                    TileKey key(t * operations_per_thread + i, 0, 0);
                    auto data = CreateTestData(512);
                    if (cache->PutTile(key, data)) {
                        put_count++;
                    }
                } else {
                    TileKey key(t * operations_per_thread + i - 1, 0, 0);
                    TileData tile = cache->GetTile(key);
                    if (tile.IsValid()) {
                        get_count++;
                    }
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(put_count, 0);
}

TEST_F(ConcurrentTileCacheTest, ConcurrentHasTile) {
    const int num_tiles = 200;
    
    for (int i = 0; i < num_tiles; ++i) {
        TileKey key(i, 0, 0);
        auto data = CreateTestData(512);
        cache->PutTile(key, data);
    }
    
    const int num_threads = 8;
    std::atomic<int> has_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < num_tiles; ++i) {
                TileKey key(i, 0, 0);
                if (cache->HasTile(key)) {
                    has_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(has_count, num_threads * num_tiles);
}

TEST_F(ConcurrentTileCacheTest, ConcurrentRemove) {
    const int num_tiles = 200;
    
    for (int i = 0; i < num_tiles; ++i) {
        TileKey key(i, 0, 0);
        auto data = CreateTestData(512);
        cache->PutTile(key, data);
    }
    
    const int num_threads = 4;
    std::atomic<int> remove_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < num_tiles / num_threads; ++i) {
                TileKey key(t * (num_tiles / num_threads) + i, 0, 0);
                if (cache->RemoveTile(key)) {
                    remove_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(remove_count, num_tiles);
}

class ConcurrentRenderQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        queue = RenderQueue::Create();
    }
    
    void TearDown() override {
        queue.reset();
    }
    
    RenderQueuePtr queue;
};

class ConcurrentRenderTask : public RenderTask {
public:
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<ConcurrentRenderTask> Create() {
        return std::make_shared<ConcurrentRenderTask>();
    }
};

TEST_F(ConcurrentRenderQueueTest, ConcurrentEnqueue) {
    const int num_threads = 8;
    const int tasks_per_thread = 100;
    std::atomic<int> success_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < tasks_per_thread; ++i) {
                auto task = ConcurrentRenderTask::Create();
                task->SetId("task_" + std::to_string(t) + "_" + std::to_string(i));
                if (queue->Enqueue(task)) {
                    success_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count, num_threads * tasks_per_thread);
    EXPECT_EQ(queue->GetSize(), num_threads * tasks_per_thread);
}

TEST_F(ConcurrentRenderQueueTest, ConcurrentDequeue) {
    const int num_tasks = 500;
    
    for (int i = 0; i < num_tasks; ++i) {
        auto task = ConcurrentRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        queue->Enqueue(task);
    }
    
    const int num_threads = 8;
    std::atomic<int> dequeue_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < num_tasks / num_threads; ++i) {
                auto task = queue->Dequeue();
                if (task != nullptr) {
                    dequeue_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(dequeue_count, num_tasks);
    EXPECT_TRUE(queue->IsEmpty());
}

TEST_F(ConcurrentRenderQueueTest, ConcurrentEnqueueDequeue) {
    const int num_threads = 8;
    const int operations_per_thread = 100;
    std::atomic<int> enqueue_count{0};
    std::atomic<int> dequeue_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                if (i % 2 == 0) {
                    auto task = ConcurrentRenderTask::Create();
                    task->SetId("task_" + std::to_string(t) + "_" + std::to_string(i));
                    if (queue->Enqueue(task)) {
                        enqueue_count++;
                    }
                } else {
                    auto task = queue->Dequeue();
                    if (task != nullptr) {
                        dequeue_count++;
                    }
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(enqueue_count, 0);
}

TEST_F(ConcurrentRenderQueueTest, ConcurrentHasTask) {
    const int num_tasks = 200;
    
    for (int i = 0; i < num_tasks; ++i) {
        auto task = ConcurrentRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        queue->Enqueue(task);
    }
    
    const int num_threads = 8;
    std::atomic<int> has_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < num_tasks; ++i) {
                if (queue->HasTask("task_" + std::to_string(i))) {
                    has_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(has_count, num_threads * num_tasks);
}

TEST_F(ConcurrentRenderQueueTest, ConcurrentClear) {
    const int num_threads = 4;
    
    for (int i = 0; i < 100; ++i) {
        auto task = ConcurrentRenderTask::Create();
        task->SetId("task_" + std::to_string(i));
        queue->Enqueue(task);
    }
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            queue->Clear();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_TRUE(queue->IsEmpty());
}

class ConcurrentAsyncRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        renderer = AsyncRenderer::Create();
    }
    
    void TearDown() override {
        renderer.reset();
    }
    
    AsyncRendererPtr renderer;
};

class AsyncTestTask : public RenderTask {
public:
    bool Execute() override {
        SetState(RenderTaskState::kRunning);
        SetState(RenderTaskState::kCompleted);
        SetResult(DrawResult::kSuccess);
        return true;
    }
    
    static std::shared_ptr<AsyncTestTask> Create() {
        return std::make_shared<AsyncTestTask>();
    }
};

TEST_F(ConcurrentAsyncRendererTest, ConcurrentStartAsync) {
    const int num_threads = 4;
    const int tasks_per_thread = 10;
    std::vector<std::string> renderIds;
    std::mutex ids_mutex;
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < tasks_per_thread; ++i) {
                auto task = AsyncTestTask::Create();
                task->SetId("task_" + std::to_string(t) + "_" + std::to_string(i));
                std::string id = renderer->StartAsync(task);
                
                std::lock_guard<std::mutex> lock(ids_mutex);
                renderIds.push_back(id);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(renderIds.size(), num_threads * tasks_per_thread);
    
    for (const auto& id : renderIds) {
        renderer->WaitForCompletion(id, 5000);
    }
}

TEST_F(ConcurrentAsyncRendererTest, ConcurrentCancel) {
    std::vector<std::string> renderIds;
    
    for (int i = 0; i < 10; ++i) {
        auto task = AsyncTestTask::Create();
        task->SetId("task_" + std::to_string(i));
        renderIds.push_back(renderer->StartAsync(task));
    }
    
    const int num_threads = 4;
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < 3; ++i) {
                renderer->Cancel(renderIds[t * 3 + i]);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

TEST_F(ConcurrentAsyncRendererTest, ConcurrentGetProgress) {
    std::vector<std::string> renderIds;
    
    for (int i = 0; i < 5; ++i) {
        auto task = AsyncTestTask::Create();
        task->SetId("task_" + std::to_string(i));
        renderIds.push_back(renderer->StartAsync(task));
    }
    
    const int num_threads = 8;
    std::atomic<int> query_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (const auto& id : renderIds) {
                double progress = renderer->GetProgress(id);
                if (progress >= 0.0 && progress <= 1.0) {
                    query_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(query_count, 0);
}

class MaxThreadsTest : public ::testing::Test {
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

TEST_F(MaxThreadsTest, MaxConcurrentThreads) {
    const int max_threads = 100;
    const int operations_per_thread = 10;
    std::atomic<int> success_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < max_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                TileKey key(t * operations_per_thread + i, 0, 0);
                auto data = CreateTestData(256);
                if (cache->PutTile(key, data)) {
                    success_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count, max_threads * operations_per_thread);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
