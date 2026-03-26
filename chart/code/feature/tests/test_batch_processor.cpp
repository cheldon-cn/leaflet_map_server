#include "gtest/gtest.h"
#include "ogc/feature/batch_processor.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

class BatchProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test");
    }
    
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(BatchProcessorTest, DefaultConstructor) {
    CNBatchProcessor processor;
    EXPECT_EQ(processor.GetBatchSize(), 100);
}

TEST_F(BatchProcessorTest, SetBatchSize) {
    CNBatchProcessor processor;
    
    processor.SetBatchSize(50);
    EXPECT_EQ(processor.GetBatchSize(), 50);
    
    processor.SetBatchSize(1);
    EXPECT_EQ(processor.GetBatchSize(), 1);
    
    processor.SetBatchSize(1000);
    EXPECT_EQ(processor.GetBatchSize(), 1000);
}

TEST_F(BatchProcessorTest, ProcessEmptyCollection) {
    CNBatchProcessor processor;
    CNFeatureCollection collection;
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, 0);
    EXPECT_EQ(result.failure_count, 0);
}

TEST_F(BatchProcessorTest, ProcessFeaturesWithDefinition) {
    CNBatchProcessor processor;
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        collection.AddFeature(f);
    }
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, 5);
}

TEST_F(BatchProcessorTest, ProcessFeaturesVector) {
    CNBatchProcessor processor;
    std::vector<CNFeature*> features;
    
    for (int i = 0; i < 3; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        features.push_back(f);
    }
    
    BatchResult result = processor.ProcessFeatures(features, BatchOperation::kCreate);
    EXPECT_EQ(result.success_count, 3);
    
    for (auto* f : features) {
        delete f;
    }
}

TEST_F(BatchProcessorTest, CustomOperation) {
    CNBatchProcessor processor;
    std::vector<CNFeature*> features;
    
    for (int i = 0; i < 3; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        features.push_back(f);
    }
    
    int processedCount = 0;
    auto processor_func = [&processedCount](CNFeature* f) -> CNFeature* {
        processedCount++;
        return f;
    };
    
    BatchResult result = processor.ExecuteCustomOperation(features, processor_func);
    EXPECT_EQ(result.success_count, 3);
    EXPECT_EQ(processedCount, 3);
    
    for (auto* f : features) {
        delete f;
    }
}

TEST_F(BatchProcessorTest, ProgressCallback) {
    CNBatchProcessor processor;
    processor.SetBatchSize(2);
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        collection.AddFeature(f);
    }
    
    std::vector<std::pair<size_t, size_t>> progressCalls;
    processor.SetProgressCallback([&progressCalls](size_t current, size_t total) {
        progressCalls.push_back({current, total});
    });
    
    processor.Process(&collection, BatchOperation::kRead);
    
    EXPECT_FALSE(progressCalls.empty());
}

TEST_F(BatchProcessorTest, ErrorCallback) {
    CNBatchProcessor processor;
    
    std::vector<std::pair<size_t, std::string>> errorCalls;
    processor.SetErrorCallback([&errorCalls](size_t index, const std::string& msg) {
        errorCalls.push_back({index, msg});
    });
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    CNFeature* f = new CNFeature(defn_);
    f->SetFID(1);
    collection.AddFeature(f);
    
    processor.Process(&collection, BatchOperation::kRead);
}

TEST_F(BatchProcessorTest, BatchSizeOne) {
    CNBatchProcessor processor;
    processor.SetBatchSize(1);
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 3; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        collection.AddFeature(f);
    }
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, 3);
}

TEST_F(BatchProcessorTest, LargeBatchSize) {
    CNBatchProcessor processor;
    processor.SetBatchSize(10000);
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 10; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        collection.AddFeature(f);
    }
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, 10);
}

TEST_F(BatchProcessorTest, AllBatchOperations) {
    CNBatchProcessor processor;
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    CNFeature* f = new CNFeature(defn_);
    f->SetFID(1);
    collection.AddFeature(f);
    
    std::vector<BatchOperation> operations = {
        BatchOperation::kCreate,
        BatchOperation::kUpdate,
        BatchOperation::kDelete,
        BatchOperation::kRead
    };
    
    for (auto op : operations) {
        BatchResult result = processor.Process(&collection, op);
        EXPECT_EQ(result.success_count, 1);
    }
}

TEST_F(BatchProcessorTest, FailedIndicesTracking) {
    CNBatchProcessor processor;
    std::vector<CNFeature*> features;
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        features.push_back(f);
    }
    
    BatchResult result = processor.ProcessFeatures(features, BatchOperation::kCreate);
    
    for (auto* f : features) {
        delete f;
    }
}

TEST_F(BatchProcessorTest, ErrorMessageTracking) {
    CNBatchProcessor processor;
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    CNFeature* f = new CNFeature(defn_);
    f->SetFID(1);
    collection.AddFeature(f);
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.error_messages.size(), 0);
}
