#pragma once

#include "export.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include <functional>
#include <memory>
#include <vector>

namespace ogc {

enum class BatchOperation {
    kCreate,
    kUpdate,
    kDelete,
    kRead
};

struct BatchResult {
    size_t success_count;
    size_t failure_count;
    std::vector<size_t> failed_indices;
    std::vector<std::string> error_messages;
};

using FeatureProcessor = std::function<CNFeature*(CNFeature*)>;

class OGC_FEATURE_API CNBatchProcessor {
public:
    CNBatchProcessor();
    ~CNBatchProcessor();

    void SetBatchSize(size_t size) { batch_size_ = size; }
    size_t GetBatchSize() const { return batch_size_; }

    void SetProgressCallback(std::function<void(size_t, size_t)> callback);
    void SetErrorCallback(std::function<void(size_t, const std::string&)> callback);

    BatchResult Process(CNFeatureCollection* collection, BatchOperation operation);
    BatchResult ProcessFeatures(const std::vector<CNFeature*>& features, BatchOperation operation);
    BatchResult ExecuteCustomOperation(const std::vector<CNFeature*>& features, FeatureProcessor processor);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    size_t batch_size_;
};

using BatchProcessor = CNBatchProcessor;

}
