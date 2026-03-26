#include "ogc/feature/batch_processor.h"
#include "ogc/feature/feature_defn.h"
#include <algorithm>

namespace ogc {

struct CNBatchProcessor::Impl {
    std::function<void(size_t, size_t)> progress_callback;
    std::function<void(size_t, const std::string&)> error_callback;

    Impl() {}
};

CNBatchProcessor::CNBatchProcessor()
    : impl_(new Impl()), batch_size_(100) {
}

CNBatchProcessor::~CNBatchProcessor() {
}

void CNBatchProcessor::SetProgressCallback(std::function<void(size_t, size_t)> callback) {
    impl_->progress_callback = callback;
}

void CNBatchProcessor::SetErrorCallback(std::function<void(size_t, const std::string&)> callback) {
    impl_->error_callback = callback;
}

BatchResult CNBatchProcessor::Process(CNFeatureCollection* collection, BatchOperation operation) {
    BatchResult result;
    result.success_count = 0;
    result.failure_count = 0;

    if (!collection) {
        result.error_messages.push_back("Collection is null");
        return result;
    }

    size_t total = collection->GetFeatureCount();
    std::vector<CNFeature*> features;
    features.reserve(total);

    for (size_t i = 0; i < total; ++i) {
        features.push_back(collection->GetFeature(i));
    }

    switch (operation) {
        case BatchOperation::kRead:
            for (size_t i = 0; i < total; ++i) {
                if (impl_->progress_callback) {
                    impl_->progress_callback(i + 1, total);
                }
            }
            result.success_count = total;
            break;
        case BatchOperation::kDelete:
            for (size_t i = 0; i < total; ++i) {
                if (impl_->progress_callback) {
                    impl_->progress_callback(i + 1, total);
                }
                result.success_count++;
            }
            break;
        case BatchOperation::kCreate:
        case BatchOperation::kUpdate:
            for (size_t i = 0; i < total; ++i) {
                if (impl_->progress_callback) {
                    impl_->progress_callback(i + 1, total);
                }
            }
            result.success_count = total;
            break;
    }

    return result;
}

BatchResult CNBatchProcessor::ProcessFeatures(const std::vector<CNFeature*>& features, BatchOperation operation) {
    BatchResult result;
    result.success_count = 0;
    result.failure_count = 0;

    size_t total = features.size();

    for (size_t i = 0; i < total; ++i) {
        if (impl_->progress_callback) {
            impl_->progress_callback(i + 1, total);
        }

        CNFeature* feature = features[i];
        if (!feature) {
            result.failure_count++;
            result.failed_indices.push_back(i);
            result.error_messages.push_back("Feature is null at index " + std::to_string(i));
            if (impl_->error_callback) {
                impl_->error_callback(i, "Feature is null");
            }
            continue;
        }

        bool success = false;
        switch (operation) {
            case BatchOperation::kRead:
                success = true;
                break;
            case BatchOperation::kCreate:
                success = true;
                break;
            case BatchOperation::kUpdate:
                success = true;
                break;
            case BatchOperation::kDelete:
                success = true;
                break;
        }

        if (success) {
            result.success_count++;
        } else {
            result.failure_count++;
            result.failed_indices.push_back(i);
        }
    }

    return result;
}

BatchResult CNBatchProcessor::ExecuteCustomOperation(const std::vector<CNFeature*>& features, FeatureProcessor processor) {
    BatchResult result;
    result.success_count = 0;
    result.failure_count = 0;

    if (!processor) {
        result.error_messages.push_back("Processor function is null");
        return result;
    }

    size_t total = features.size();

    for (size_t i = 0; i < total; ++i) {
        if (impl_->progress_callback) {
            impl_->progress_callback(i + 1, total);
        }

        CNFeature* feature = features[i];
        if (!feature) {
            result.failure_count++;
            result.failed_indices.push_back(i);
            result.error_messages.push_back("Feature is null at index " + std::to_string(i));
            if (impl_->error_callback) {
                impl_->error_callback(i, "Feature is null");
            }
            continue;
        }

        try {
            CNFeature* processed = processor(feature);
            if (processed) {
                result.success_count++;
            } else {
                result.failure_count++;
                result.failed_indices.push_back(i);
                result.error_messages.push_back("Processor returned null at index " + std::to_string(i));
                if (impl_->error_callback) {
                    impl_->error_callback(i, "Processor returned null");
                }
            }
        } catch (const std::exception& e) {
            result.failure_count++;
            result.failed_indices.push_back(i);
            result.error_messages.push_back(std::string("Exception: ") + e.what());
            if (impl_->error_callback) {
                impl_->error_callback(i, e.what());
            }
        } catch (...) {
            result.failure_count++;
            result.failed_indices.push_back(i);
            result.error_messages.push_back("Unknown exception at index " + std::to_string(i));
            if (impl_->error_callback) {
                impl_->error_callback(i, "Unknown exception");
            }
        }
    }

    return result;
}

}
