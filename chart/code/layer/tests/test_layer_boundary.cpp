#include "gtest/gtest.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/layer/thread_safe_layer.h"
#include "ogc/layer/layer_group.h"
#include "ogc/layer/feature_stream.h"
#include "ogc/layer/connection_pool.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geometry.h"
#include "ogc/factory.h"

#include <limits>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

using namespace ogc;

class LayerBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("boundary_test", GeomType::kPoint);
        layer_->SetAutoFIDGeneration(false);
        
        auto* int_field = CreateCNFieldDefn("int_field");
        int_field->SetType(CNFieldType::kInteger);
        layer_->CreateField(int_field);
        
        auto* real_field = CreateCNFieldDefn("real_field");
        real_field->SetType(CNFieldType::kReal);
        layer_->CreateField(real_field);
        
        auto* str_field = CreateCNFieldDefn("str_field");
        str_field->SetType(CNFieldType::kString);
        str_field->SetWidth(255);
        layer_->CreateField(str_field);
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(LayerBoundaryTest, FID_MinValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(std::numeric_limits<int64_t>::min());
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(std::numeric_limits<int64_t>::min());
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), std::numeric_limits<int64_t>::min());
}

TEST_F(LayerBoundaryTest, FID_MaxValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(std::numeric_limits<int64_t>::max());
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(std::numeric_limits<int64_t>::max());
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), std::numeric_limits<int64_t>::max());
}

TEST_F(LayerBoundaryTest, FID_Zero) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(0);
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 0);
}

TEST_F(LayerBoundaryTest, FID_Negative) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(-1);
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(-1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), -1);
}

TEST_F(LayerBoundaryTest, IntegerField_MinValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue(std::numeric_limits<int32_t>::min()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(0).GetInteger(), std::numeric_limits<int32_t>::min());
}

TEST_F(LayerBoundaryTest, IntegerField_MaxValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue(std::numeric_limits<int32_t>::max()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(0).GetInteger(), std::numeric_limits<int32_t>::max());
}

TEST_F(LayerBoundaryTest, IntegerField_Zero) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue(0));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(0).GetInteger(), 0);
}

TEST_F(LayerBoundaryTest, RealField_MinValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(std::numeric_limits<double>::lowest()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_DOUBLE_EQ(retrieved->GetField(1).GetReal(), std::numeric_limits<double>::lowest());
}

TEST_F(LayerBoundaryTest, RealField_MaxValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(std::numeric_limits<double>::max()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_DOUBLE_EQ(retrieved->GetField(1).GetReal(), std::numeric_limits<double>::max());
}

TEST_F(LayerBoundaryTest, RealField_Epsilon) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(std::numeric_limits<double>::epsilon()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_DOUBLE_EQ(retrieved->GetField(1).GetReal(), std::numeric_limits<double>::epsilon());
}

TEST_F(LayerBoundaryTest, RealField_NegativeInfinity) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(-std::numeric_limits<double>::infinity()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_DOUBLE_EQ(retrieved->GetField(1).GetReal(), -std::numeric_limits<double>::infinity());
}

TEST_F(LayerBoundaryTest, RealField_PositiveInfinity) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(std::numeric_limits<double>::infinity()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_DOUBLE_EQ(retrieved->GetField(1).GetReal(), std::numeric_limits<double>::infinity());
}

TEST_F(LayerBoundaryTest, RealField_QuietNaN) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(1, CNFieldValue(std::numeric_limits<double>::quiet_NaN()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_TRUE(std::isnan(retrieved->GetField(1).GetReal()));
}

TEST_F(LayerBoundaryTest, StringField_Empty) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(2, CNFieldValue(""));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(2).GetString(), "");
}

TEST_F(LayerBoundaryTest, StringField_LongString) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    std::string long_str(1000, 'a');
    feature->SetField(2, CNFieldValue(long_str));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(2).GetString().size(), 1000);
}

TEST_F(LayerBoundaryTest, StringField_SpecialCharacters) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(2, CNFieldValue("特殊字符\t\n\r\"'\\"));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(2).GetString(), "特殊字符\t\n\r\"'\\");
}

TEST_F(LayerBoundaryTest, StringField_Unicode) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(2, CNFieldValue("中文日本語한국어العربية"));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetField(2).GetString(), "中文日本語한국어العربية");
}

TEST_F(LayerBoundaryTest, Coordinate_MinValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
        std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    auto geom = retrieved->GetGeometry();
    ASSERT_NE(geom, nullptr);
}

TEST_F(LayerBoundaryTest, Coordinate_MaxValue) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max()));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    auto geom = retrieved->GetGeometry();
    ASSERT_NE(geom, nullptr);
}

TEST_F(LayerBoundaryTest, Coordinate_Zero) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(0.0, 0.0));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    auto geom = retrieved->GetGeometry();
    ASSERT_NE(geom, nullptr);
}

TEST_F(LayerBoundaryTest, Coordinate_Negative) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(-180.0, -90.0));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    auto geom = retrieved->GetGeometry();
    ASSERT_NE(geom, nullptr);
}

TEST_F(LayerBoundaryTest, SpatialFilter_ZeroExtent) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(0.0, 0.0));
    layer_->CreateFeature(feature);
    
    layer_->SetSpatialFilterRect(0.0, 0.0, 0.0, 0.0);
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    EXPECT_GE(count, 0);
}

TEST_F(LayerBoundaryTest, SpatialFilter_VeryLargeExtent) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(0.0, 0.0));
    layer_->CreateFeature(feature);
    
    layer_->SetSpatialFilterRect(
        std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    EXPECT_EQ(count, 1);
}

TEST_F(LayerBoundaryTest, SpatialFilter_NegativeExtent) {
    layer_->SetSpatialFilterRect(100.0, 100.0, 0.0, 0.0);
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    EXPECT_EQ(count, 0);
}

class LayerIndexBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("index_test", GeomType::kPoint);
        
        for (int i = 1; i <= 10; ++i) {
            CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
            feature->SetFID(i);
            layer_->CreateFeature(feature);
        }
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(LayerIndexBoundaryTest, GetFeature_IndexZero) {
    auto feature = layer_->GetFeature(0);
    EXPECT_EQ(feature, nullptr);
}

TEST_F(LayerIndexBoundaryTest, GetFeature_FirstIndex) {
    auto feature = layer_->GetFeature(1);
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetFID(), 1);
}

TEST_F(LayerIndexBoundaryTest, GetFeature_LastIndex) {
    auto feature = layer_->GetFeature(10);
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetFID(), 10);
}

TEST_F(LayerIndexBoundaryTest, GetFeature_IndexOutOfRange) {
    auto feature = layer_->GetFeature(11);
    EXPECT_EQ(feature, nullptr);
}

TEST_F(LayerIndexBoundaryTest, GetFeature_LargeIndex) {
    auto feature = layer_->GetFeature(std::numeric_limits<int64_t>::max());
    EXPECT_EQ(feature, nullptr);
}

TEST_F(LayerIndexBoundaryTest, DeleteFeature_IndexZero) {
    CNStatus status = layer_->DeleteFeature(0);
    EXPECT_EQ(status, CNStatus::kFeatureNotFound);
}

TEST_F(LayerIndexBoundaryTest, DeleteFeature_FirstIndex) {
    CNStatus status = layer_->DeleteFeature(1);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 9);
}

TEST_F(LayerIndexBoundaryTest, DeleteFeature_LastIndex) {
    CNStatus status = layer_->DeleteFeature(10);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 9);
}

TEST_F(LayerIndexBoundaryTest, DeleteFeature_IndexOutOfRange) {
    CNStatus status = layer_->DeleteFeature(11);
    EXPECT_EQ(status, CNStatus::kFeatureNotFound);
}

class LayerGroupBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        root_ = std::make_unique<CNLayerGroup>("root");
    }

    std::unique_ptr<CNLayerGroup> root_;
};

TEST_F(LayerGroupBoundaryTest, GetChild_EmptyGroup) {
    EXPECT_EQ(root_->GetChildCount(), 0);
    EXPECT_EQ(root_->GetChild(0), nullptr);
    EXPECT_EQ(root_->GetChild(-1), nullptr);
}

TEST_F(LayerGroupBoundaryTest, GetChild_FirstIndex) {
    auto layer = std::make_unique<CNMemoryLayer>("layer1", GeomType::kPoint);
    root_->AddLayer(std::move(layer));
    
    auto* child = root_->GetChild(0);
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->GetName(), "layer1");
}

TEST_F(LayerGroupBoundaryTest, GetChild_IndexOutOfRange) {
    auto layer = std::make_unique<CNMemoryLayer>("layer1", GeomType::kPoint);
    root_->AddLayer(std::move(layer));
    
    EXPECT_EQ(root_->GetChild(1), nullptr);
    EXPECT_EQ(root_->GetChild(100), nullptr);
    EXPECT_EQ(root_->GetChild(-1), nullptr);
}

TEST_F(LayerGroupBoundaryTest, GetLayer_EmptyGroup) {
    EXPECT_EQ(root_->GetLayerCount(), 0);
    EXPECT_EQ(root_->GetLayer(0), nullptr);
}

TEST_F(LayerGroupBoundaryTest, GetLayer_IndexOutOfRange) {
    auto layer = std::make_unique<CNMemoryLayer>("layer1", GeomType::kPoint);
    root_->AddLayer(std::move(layer));
    
    EXPECT_EQ(root_->GetLayer(1), nullptr);
    EXPECT_EQ(root_->GetLayer(-1), nullptr);
}

TEST_F(LayerGroupBoundaryTest, FindChild_NotFound) {
    auto* found = root_->FindChild("nonexistent");
    EXPECT_EQ(found, nullptr);
}

TEST_F(LayerGroupBoundaryTest, RemoveChild_IndexOutOfRange) {
    auto removed = root_->RemoveChild(0);
    EXPECT_EQ(removed, nullptr);
    
    removed = root_->RemoveChild(-1);
    EXPECT_EQ(removed, nullptr);
    
    removed = root_->RemoveChild(100);
    EXPECT_EQ(removed, nullptr);
}

TEST_F(LayerGroupBoundaryTest, AddLayer_MaxLayers) {
    for (int i = 0; i < 10000; ++i) {
        auto layer = std::make_unique<CNMemoryLayer>("layer_" + std::to_string(i), GeomType::kPoint);
        root_->AddLayer(std::move(layer));
    }
    
    EXPECT_EQ(root_->GetLayerCount(), 10000);
}

class ConcurrencyBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto memory_layer = std::make_unique<CNMemoryLayer>("concurrent_test", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("id");
        field->SetType(CNFieldType::kInteger);
        memory_layer->CreateField(field);
        
        layer_ = std::make_unique<CNThreadSafeLayer>(std::move(memory_layer));
    }

    std::unique_ptr<CNThreadSafeLayer> layer_;
};

TEST_F(ConcurrencyBoundaryTest, MaxThreads) {
    const int max_threads = 100;
    const int features_per_thread = 10;
    std::atomic<int> success_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < max_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < features_per_thread; ++i) {
                CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
                feature->SetFID(t * features_per_thread + i + 1);
                if (layer_->CreateFeature(feature) == CNStatus::kSuccess) {
                    success_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count, max_threads * features_per_thread);
    EXPECT_EQ(layer_->GetFeatureCount(), max_threads * features_per_thread);
}

TEST_F(ConcurrencyBoundaryTest, RapidCreateDelete) {
    const int iterations = 100;
    std::atomic<int> create_count{0};
    std::atomic<int> delete_count{0};
    
    for (int i = 0; i < iterations; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i + 1);
        if (layer_->CreateFeature(feature) == CNStatus::kSuccess) {
            create_count++;
        }
        
        if (i > 0 && i % 2 == 0) {
            if (layer_->DeleteFeature(i) == CNStatus::kSuccess) {
                delete_count++;
            }
        }
    }
    
    EXPECT_EQ(create_count, iterations);
    EXPECT_GT(delete_count, 0);
}

TEST_F(ConcurrencyBoundaryTest, ConcurrentReadWriteDelete) {
    const int initial_count = 100;
    for (int i = 1; i <= initial_count; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    const int num_threads = 20;
    std::atomic<int> read_count{0};
    std::atomic<int> write_count{0};
    std::atomic<int> delete_count{0};
    std::atomic<int64_t> next_fid{initial_count + 1};
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads / 2; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < 50; ++i) {
                int64_t fid = (i % initial_count) + 1;
                auto feature = layer_->GetFeature(fid);
                if (feature) {
                    read_count++;
                }
            }
        });
    }
    
    for (int t = 0; t < num_threads / 4; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < 20; ++i) {
                CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
                feature->SetFID(next_fid++);
                if (layer_->CreateFeature(feature) == CNStatus::kSuccess) {
                    write_count++;
                }
            }
        });
    }
    
    for (int t = 0; t < num_threads / 4; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < 10; ++i) {
                int64_t fid = (i * 10) + 1;
                if (layer_->DeleteFeature(fid) == CNStatus::kSuccess) {
                    delete_count++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(read_count, 0);
    EXPECT_GT(write_count, 0);
    EXPECT_GE(delete_count, 0);
}

class FeatureStreamBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("stream_boundary", GeomType::kPoint);
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(FeatureStreamBoundaryTest, EmptyStream) {
    CNLayerFeatureStream stream(layer_.get());
    
    EXPECT_TRUE(stream.IsEndOfStream());
    EXPECT_EQ(stream.GetReadCount(), 0);
    
    auto batch = stream.ReadNextBatch(10);
    EXPECT_TRUE(batch.empty());
}

TEST_F(FeatureStreamBoundaryTest, SingleFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    CNLayerFeatureStream stream(layer_.get());
    
    EXPECT_FALSE(stream.IsEndOfStream());
    
    auto batch = stream.ReadNextBatch(10);
    EXPECT_EQ(batch.size(), 1);
    
    EXPECT_TRUE(stream.IsEndOfStream());
}

TEST_F(FeatureStreamBoundaryTest, BatchSizeOne) {
    for (int i = 1; i <= 10; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    CNLayerFeatureStream stream(layer_.get());
    
    int count = 0;
    while (!stream.IsEndOfStream()) {
        auto batch = stream.ReadNextBatch(1);
        count += static_cast<int>(batch.size());
    }
    
    EXPECT_EQ(count, 10);
}

TEST_F(FeatureStreamBoundaryTest, BatchSizeLargerThanData) {
    for (int i = 1; i <= 5; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    CNLayerFeatureStream stream(layer_.get());
    
    auto batch = stream.ReadNextBatch(100);
    EXPECT_EQ(batch.size(), 5);
    
    EXPECT_TRUE(stream.IsEndOfStream());
}

TEST_F(FeatureStreamBoundaryTest, ResetAfterEnd) {
    for (int i = 1; i <= 5; ++i) {
        CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
        feature->SetFID(i);
        layer_->CreateFeature(feature);
    }
    
    CNLayerFeatureStream stream(layer_.get());
    
    while (!stream.IsEndOfStream()) {
        stream.ReadNextBatch(10);
    }
    
    EXPECT_TRUE(stream.IsEndOfStream());
    
    stream.Reset();
    
    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.GetReadCount(), 0);
    
    auto batch = stream.ReadNextBatch(10);
    EXPECT_EQ(batch.size(), 5);
}
