#include "gtest/gtest.h"
#include "ogc/layer/thread_safe_layer.h"
#include "ogc/layer/layer_observer.h"
#include "ogc/layer/layer_snapshot.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/factory.h"

#include <thread>
#include <vector>
#include <atomic>

using namespace ogc;

class CNThreadSafeLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto memory_layer = std::make_unique<CNMemoryLayer>("test_layer", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        memory_layer->CreateField(field);
        
        layer_ = std::make_unique<CNThreadSafeLayer>(std::move(memory_layer));
    }

    std::unique_ptr<CNThreadSafeLayer> layer_;
};

TEST_F(CNThreadSafeLayerTest, GetName) {
    EXPECT_EQ(layer_->GetName(), "test_layer");
}

TEST_F(CNThreadSafeLayerTest, GetLayerType) {
    EXPECT_EQ(layer_->GetLayerType(), CNLayerType::kMemory);
}

TEST_F(CNThreadSafeLayerTest, GetGeomType) {
    EXPECT_EQ(layer_->GetGeomType(), GeomType::kPoint);
}

TEST_F(CNThreadSafeLayerTest, CreateFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue("Test Point"));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
}

TEST_F(CNThreadSafeLayerTest, GetFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 1);
}

TEST_F(CNThreadSafeLayerTest, DeleteFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    
    CNStatus status = layer_->DeleteFeature(1);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 0);
}

TEST_F(CNThreadSafeLayerTest, SetSpatialFilterRect) {
    layer_->SetSpatialFilterRect(0.0, 0.0, 100.0, 100.0);
    Envelope extent;
    CNStatus status = layer_->GetExtent(extent, false);
    EXPECT_EQ(status, CNStatus::kSuccess);
}

TEST_F(CNThreadSafeLayerTest, Transaction) {
    EXPECT_EQ(layer_->StartTransaction(), CNStatus::kSuccess);
    EXPECT_EQ(layer_->CommitTransaction(), CNStatus::kSuccess);
}

TEST_F(CNThreadSafeLayerTest, TestCapability) {
    EXPECT_TRUE(layer_->TestCapability(CNLayerCapability::kCreateFeature));
}

TEST_F(CNThreadSafeLayerTest, Clone) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    auto cloned = layer_->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), layer_->GetName());
}

TEST_F(CNThreadSafeLayerTest, ThreadSafety) {
    const int num_threads = 4;
    const int features_per_thread = 100;
    std::atomic<int> success_count{0};
    
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
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
    
    EXPECT_EQ(success_count, num_threads * features_per_thread);
    EXPECT_EQ(layer_->GetFeatureCount(), num_threads * features_per_thread);
}

class TestLayerObserver : public CNLayerObserver {
public:
    void OnLayerChanged(CNLayer* layer, const CNLayerEventArgs& args) override {
        (void)layer;
        last_type_ = args.type;
        last_fid_ = args.fid;
        event_count_++;
    }
    
    bool OnLayerChanging(CNLayer* layer, const CNLayerEventArgs& args) override {
        (void)layer;
        changing_count_++;
        return allow_change_;
    }
    
    CNLayerEventType last_type_ = CNLayerEventType::kCleared;
    int64_t last_fid_ = -1;
    int event_count_ = 0;
    int changing_count_ = 0;
    bool allow_change_ = true;
};

class CNLayerObserverTest : public ::testing::Test {
protected:
    void SetUp() override {
        observer_ = std::make_unique<TestLayerObserver>();
    }

    std::unique_ptr<TestLayerObserver> observer_;
};

TEST_F(CNLayerObserverTest, EventTypeValues) {
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kFeatureAdded), 0);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kFeatureDeleted), 1);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kFeatureModified), 2);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kSchemaChanged), 3);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kSpatialFilterChanged), 4);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kAttributeFilterChanged), 5);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kCleared), 6);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kTransactionStarted), 7);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kTransactionCommitted), 8);
    EXPECT_EQ(static_cast<int>(CNLayerEventType::kTransactionRolledBack), 9);
}

TEST_F(CNLayerObserverTest, EventArgsDefaults) {
    CNLayerEventArgs args;
    args.type = CNLayerEventType::kFeatureAdded;
    args.fid = 1;
    
    EXPECT_EQ(args.type, CNLayerEventType::kFeatureAdded);
    EXPECT_EQ(args.fid, 1);
}

TEST_F(CNLayerObserverTest, OnLayerChangedCalled) {
    CNLayerEventArgs args;
    args.type = CNLayerEventType::kFeatureAdded;
    args.fid = 42;
    
    observer_->OnLayerChanged(nullptr, args);
    
    EXPECT_EQ(observer_->last_type_, CNLayerEventType::kFeatureAdded);
    EXPECT_EQ(observer_->last_fid_, 42);
    EXPECT_EQ(observer_->event_count_, 1);
}

TEST_F(CNLayerObserverTest, OnLayerChangingCalled) {
    CNLayerEventArgs args;
    args.type = CNLayerEventType::kFeatureDeleted;
    
    bool result = observer_->OnLayerChanging(nullptr, args);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(observer_->changing_count_, 1);
}

TEST_F(CNLayerObserverTest, OnLayerChangingCanVeto) {
    observer_->allow_change_ = false;
    
    CNLayerEventArgs args;
    bool result = observer_->OnLayerChanging(nullptr, args);
    
    EXPECT_FALSE(result);
}

class CNLayerSnapshotTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("test_layer", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        layer_->CreateField(field);
        
        for (int i = 1; i <= 5; ++i) {
            CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
            feature->SetFID(i);
            feature->SetField(0, CNFieldValue("Feature " + std::to_string(i)));
            layer_->CreateFeature(feature);
        }
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(CNLayerSnapshotTest, Size) {
    CNLayerSnapshot snapshot(layer_.get());
    EXPECT_EQ(snapshot.size(), 5);
}

TEST_F(CNLayerSnapshotTest, IteratorBeginEnd) {
    CNLayerSnapshot snapshot(layer_.get());
    
    auto it = snapshot.begin();
    auto end = snapshot.end();
    
    EXPECT_NE(it, end);
}

TEST_F(CNLayerSnapshotTest, IterateAllFeatures) {
    CNLayerSnapshot snapshot(layer_.get());
    
    int count = 0;
    for (auto it = snapshot.begin(); it != snapshot.end(); ++it) {
        count++;
    }
    
    EXPECT_EQ(count, 5);
}

TEST_F(CNLayerSnapshotTest, IteratorIncrement) {
    CNLayerSnapshot snapshot(layer_.get());
    
    auto it = snapshot.begin();
    EXPECT_NE(it, snapshot.end());
    
    ++it;
    EXPECT_NE(it, snapshot.end());
    
    it++;
    EXPECT_NE(it, snapshot.end());
}

TEST_F(CNLayerSnapshotTest, IteratorDereference) {
    CNLayerSnapshot snapshot(layer_.get());
    
    auto it = snapshot.begin();
    CNFeature& feature = *it;
    EXPECT_EQ(feature.GetFID(), 1);
}

TEST_F(CNLayerSnapshotTest, IteratorArrow) {
    CNLayerSnapshot snapshot(layer_.get());
    
    auto it = snapshot.begin();
    EXPECT_EQ(it->GetFID(), 1);
}

TEST_F(CNLayerSnapshotTest, IteratorEquality) {
    CNLayerSnapshot snapshot(layer_.get());
    
    auto it1 = snapshot.begin();
    auto it2 = snapshot.begin();
    
    EXPECT_EQ(it1, it2);
    
    ++it1;
    EXPECT_NE(it1, it2);
}

TEST_F(CNLayerSnapshotTest, SnapshotPreservesData) {
    CNLayerSnapshot snapshot(layer_.get());
    
    layer_->Clear();
    
    EXPECT_EQ(snapshot.size(), 5);
}

TEST_F(CNLayerSnapshotTest, EmptySnapshot) {
    CNMemoryLayer empty_layer("empty", GeomType::kPoint);
    CNLayerSnapshot snapshot(&empty_layer);
    
    EXPECT_EQ(snapshot.size(), 0);
    EXPECT_EQ(snapshot.begin(), snapshot.end());
}
