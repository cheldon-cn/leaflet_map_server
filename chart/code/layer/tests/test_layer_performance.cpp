#include "gtest/gtest.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/layer/thread_safe_layer.h"
#include "ogc/layer/layer_group.h"
#include "ogc/layer/feature_stream.h"
#include "ogc/layer/read_write_lock.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geometry.h"
#include "ogc/factory.h"

#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

using namespace ogc;

class LayerPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("perf_test", GeomType::kPoint);
        
        auto* field1 = CreateCNFieldDefn("id");
        field1->SetType(CNFieldType::kInteger);
        layer_->CreateField(field1);
        
        auto* field2 = CreateCNFieldDefn("name");
        field2->SetType(CNFieldType::kString);
        field2->SetWidth(255);
        layer_->CreateField(field2);
        
        auto* field3 = CreateCNFieldDefn("value");
        field3->SetType(CNFieldType::kReal);
        layer_->CreateField(field3);
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(LayerPerformanceTest, CreateFeature_ResponseTime) {
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i + 1);
        feature->SetField(0, CNFieldValue(i));
        feature->SetField(1, CNFieldValue("Feature_" + std::to_string(i)));
        feature->SetField(2, CNFieldValue(i * 1.5));
        feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
            static_cast<double>(i % 100), static_cast<double>(i / 100)));
        layer_->CreateFeature(feature);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_ms = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_ms, 1.0) << "Average create feature time should be < 1ms";
    
    EXPECT_EQ(layer_->GetFeatureCount(), iterations);
}

TEST_F(LayerPerformanceTest, GetFeature_ResponseTime) {
    const int feature_count = 1000;
    for (int i = 1; i <= feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 1; i <= iterations; ++i) {
        auto retrieved = layer_->GetFeature(i);
        EXPECT_NE(retrieved, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double avg_time_ms = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time_ms, 0.5) << "Average get feature time should be < 0.5ms";
}

TEST_F(LayerPerformanceTest, IterateAllFeatures_ResponseTime) {
    const int feature_count = 5000;
    for (int i = 1; i <= feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(count, feature_count);
    double avg_time_ms = static_cast<double>(duration.count()) / feature_count;
    EXPECT_LT(avg_time_ms, 0.1) << "Average iteration time should be < 0.1ms";
}

TEST_F(LayerPerformanceTest, BatchCreate_Performance) {
    const int batch_size = 1000;
    std::vector<CNFeature*> features;
    
    for (int i = 0; i < batch_size; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i + 1);
        feature->SetField(0, CNFieldValue(i));
        feature->SetField(1, CNFieldValue("Batch_" + std::to_string(i)));
        feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
            static_cast<double>(i % 100), static_cast<double>(i / 100)));
        features.push_back(feature);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int64_t created = layer_->CreateFeatureBatch(features);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(created, batch_size);
    EXPECT_LT(duration.count(), 100) << "Batch create should complete in < 100ms";
}

TEST_F(LayerPerformanceTest, SpatialFilter_Performance) {
    const int feature_count = 10000;
    for (int i = 0; i < feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i + 1);
        double x = static_cast<double>((i % 100) * 10);
        double y = static_cast<double>((i / 100) * 10);
        feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(x, y));
        layer_->CreateFeature(feature);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    layer_->SetSpatialFilterRect(0.0, 0.0, 500.0, 500.0);
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_GT(count, 0);
    EXPECT_LT(duration.count(), 500) << "Spatial filter should complete in < 500ms";
}

TEST_F(LayerPerformanceTest, GetExtent_Performance) {
    const int feature_count = 5000;
    for (int i = 0; i < feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i + 1);
        feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
            static_cast<double>(i % 100), static_cast<double>(i / 100)));
        layer_->CreateFeature(feature);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    Envelope extent;
    CNStatus status = layer_->GetExtent(extent);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_LT(duration.count(), 100) << "GetExtent should complete in < 100ms";
}

TEST_F(LayerPerformanceTest, Clone_Performance) {
    const int feature_count = 1000;
    for (int i = 1; i <= feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        feature->SetField(0, CNFieldValue(i));
        feature->SetField(1, CNFieldValue("Clone_" + std::to_string(i)));
        feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
            static_cast<double>(i), static_cast<double>(i * 2)));
        layer_->CreateFeature(feature);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto cloned = layer_->Clone();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFeatureCount(), feature_count);
    EXPECT_LT(duration.count(), 200) << "Clone should complete in < 200ms";
}

class ThreadSafeLayerPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto memory_layer = std::make_unique<CNMemoryLayer>("thread_perf", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("id");
        field->SetType(CNFieldType::kInteger);
        memory_layer->CreateField(field);
        
        layer_ = std::make_unique<CNThreadSafeLayer>(std::move(memory_layer));
    }

    std::unique_ptr<CNThreadSafeLayer> layer_;
};

TEST_F(ThreadSafeLayerPerformanceTest, DISABLED_ConcurrentCreate_Performance) {
    const int num_threads = 8;
    const int features_per_thread = 500;
    std::atomic<int> success_count{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < features_per_thread; ++i) {
                CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
                feature->SetFID(t * features_per_thread + i + 1);
                feature->SetField(0, CNFieldValue(t * features_per_thread + i));
                if (layer_->CreateFeature(feature) == CNStatus::kSuccess) {
                    success_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(success_count, num_threads * features_per_thread);
    EXPECT_LT(duration.count(), 2000) << "Concurrent create should complete in < 2s";
}

TEST_F(ThreadSafeLayerPerformanceTest, DISABLED_ConcurrentRead_Performance) {
    const int feature_count = 1000;
    for (int i = 1; i <= feature_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    const int num_threads = 8;
    const int reads_per_thread = 1000;
    std::atomic<int> total_reads{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < reads_per_thread; ++i) {
                int64_t fid = (t * reads_per_thread + i) % feature_count + 1;
                auto feature = layer_->GetFeature(fid);
                if (feature) {
                    total_reads++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(total_reads, num_threads * reads_per_thread);
    EXPECT_LT(duration.count(), 1000) << "Concurrent read should complete in < 1s";
}

TEST_F(ThreadSafeLayerPerformanceTest, DISABLED_ConcurrentReadWrite_Performance) {
    const int initial_features = 500;
    for (int i = 1; i <= initial_features; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    const int num_readers = 4;
    const int num_writers = 4;
    const int operations_per_thread = 200;
    std::atomic<int> read_count{0};
    std::atomic<int> write_count{0};
    std::atomic<int64_t> next_fid{initial_features + 1};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_readers; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                int64_t fid = (i % initial_features) + 1;
                auto feature = layer_->GetFeature(fid);
                if (feature) {
                    read_count++;
                }
            }
        });
    }
    
    for (int t = 0; t < num_writers; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
                feature->SetFID(next_fid++);
                if (layer_->CreateFeature(feature) == CNStatus::kSuccess) {
                    write_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(read_count, num_readers * operations_per_thread);
    EXPECT_EQ(write_count, num_writers * operations_per_thread);
    EXPECT_LT(duration.count(), 3000) << "Concurrent read/write should complete in < 3s";
}

class FeatureStreamPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("stream_perf", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("id");
        field->SetType(CNFieldType::kInteger);
        layer_->CreateField(field);
        
        const int feature_count = 10000;
        for (int i = 1; i <= feature_count; ++i) {
            CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
            feature->SetFID(i);
            feature->SetField(0, CNFieldValue(i));
            layer_->CreateFeature(feature);
        }
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(FeatureStreamPerformanceTest, BatchRead_Performance) {
    CNLayerFeatureStream stream(layer_.get(), 100);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int total = 0;
    while (!stream.IsEndOfStream()) {
        auto batch = stream.ReadNextBatch(500);
        total += static_cast<int>(batch.size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(total, 10000);
    EXPECT_LT(duration.count(), 500) << "Stream batch read should complete in < 500ms";
}

TEST_F(FeatureStreamPerformanceTest, SmallBatchRead_Performance) {
    CNLayerFeatureStream stream(layer_.get());
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int total = 0;
    while (!stream.IsEndOfStream()) {
        auto batch = stream.ReadNextBatch(10);
        total += static_cast<int>(batch.size());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(total, 10000);
    EXPECT_LT(duration.count(), 1000) << "Small batch read should complete in < 1s";
}

class LayerGroupPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        root_ = std::make_unique<CNLayerGroup>("root");
    }

    std::unique_ptr<CNLayerGroup> root_;
};

TEST_F(LayerGroupPerformanceTest, AddLayers_Performance) {
    const int layer_count = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < layer_count; ++i) {
        auto layer = std::make_unique<CNMemoryLayer>("layer_" + std::to_string(i), GeomType::kPoint);
        root_->AddLayer(std::move(layer));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(root_->GetLayerCount(), layer_count);
    EXPECT_LT(duration.count(), 200) << "Add layers should complete in < 200ms";
}

TEST_F(LayerGroupPerformanceTest, NestedGroups_Performance) {
    const int depth = 10;
    const int layers_per_level = 10;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    CNLayerGroup* current = root_.get();
    for (int d = 0; d < depth; ++d) {
        for (int i = 0; i < layers_per_level; ++i) {
            auto layer = std::make_unique<CNMemoryLayer>(
                "layer_d" + std::to_string(d) + "_" + std::to_string(i), GeomType::kPoint);
            current->AddLayer(std::move(layer));
        }
        
        if (d < depth - 1) {
            auto subgroup = std::make_unique<CNLayerGroup>("group_" + std::to_string(d + 1));
            auto* subgroup_ptr = subgroup.get();
            current->AddGroup(std::move(subgroup));
            current = subgroup_ptr;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(root_->GetLayerCount(), depth * layers_per_level);
    EXPECT_LT(duration.count(), 100) << "Create nested groups should complete in < 100ms";
}

TEST_F(LayerGroupPerformanceTest, FindLayer_Performance) {
    const int layer_count = 500;
    for (int i = 0; i < layer_count; ++i) {
        auto layer = std::make_unique<CNMemoryLayer>("find_layer_" + std::to_string(i), GeomType::kPoint);
        root_->AddLayer(std::move(layer));
    }
    
    const int search_count = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < search_count; ++i) {
        std::string name = "find_layer_" + std::to_string(i % layer_count);
        auto* found = root_->FindChild(name);
        EXPECT_NE(found, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 500) << "Find layer should complete in < 500ms";
}

class ReadWriteLockPerformanceTest : public ::testing::Test {
protected:
    CNReadWriteLock lock_;
};

TEST_F(ReadWriteLockPerformanceTest, DISABLED_ConcurrentRead_Performance) {
    const int num_threads = 16;
    const int reads_per_thread = 10000;
    std::atomic<int> counter{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < reads_per_thread; ++i) {
                CNReadWriteLock::ReadGuard guard(lock_);
                counter++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(counter, num_threads * reads_per_thread);
    EXPECT_LT(duration.count(), 500) << "Concurrent read should complete in < 500ms";
}

TEST_F(ReadWriteLockPerformanceTest, DISABLED_ConcurrentWrite_Performance) {
    const int num_threads = 8;
    const int writes_per_thread = 1000;
    std::atomic<int> counter{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < writes_per_thread; ++i) {
                CNReadWriteLock::WriteGuard guard(lock_);
                counter++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(counter, num_threads * writes_per_thread);
    EXPECT_LT(duration.count(), 2000) << "Concurrent write should complete in < 2s";
}

TEST_F(ReadWriteLockPerformanceTest, MixedReadWrite_Performance) {
    const int num_readers = 8;
    const int num_writers = 4;
    const int operations_per_thread = 1000;
    std::atomic<int> read_counter{0};
    std::atomic<int> write_counter{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_readers; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                CNReadWriteLock::ReadGuard guard(lock_);
                read_counter++;
            }
        });
    }
    
    for (int t = 0; t < num_writers; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                CNReadWriteLock::WriteGuard guard(lock_);
                write_counter++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(read_counter, num_readers * operations_per_thread);
    EXPECT_EQ(write_counter, num_writers * operations_per_thread);
    EXPECT_LT(duration.count(), 3000) << "Mixed read/write should complete in < 3s";
}
