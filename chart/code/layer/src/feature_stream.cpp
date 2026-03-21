#include "ogc/layer/feature_stream.h"

#include "ogc/feature/feature.h"

#include <queue>

namespace ogc {

CNLayerFeatureStream::CNLayerFeatureStream(CNLayer* layer, size_t batch_size)
    : layer_(layer), batch_size_(batch_size) {
}

CNLayerFeatureStream::~CNLayerFeatureStream() {
    Close();
}

std::vector<std::unique_ptr<CNFeature>> CNLayerFeatureStream::ReadNextBatch(size_t batch_size) {
    if (closed_ || !layer_) {
        return {};
    }

    std::vector<std::unique_ptr<CNFeature>> batch;
    batch.reserve(batch_size);

    for (size_t i = 0; i < batch_size; ++i) {
        auto feature = layer_->GetNextFeature();
        if (!feature) {
            break;
        }
        batch.push_back(std::move(feature));
        ++read_count_;
    }

    if (config_.enable_monitoring) {
        status_.total_produced += batch.size();
        status_.current_buffer_size = buffer_.size();
    }

    return batch;
}

bool CNLayerFeatureStream::IsEndOfStream() const {
    return closed_ || read_count_ >= layer_->GetFeatureCount();
}

int64_t CNLayerFeatureStream::GetReadCount() const {
    return read_count_;
}

void CNLayerFeatureStream::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (layer_) {
        layer_->ResetReading();
    }
    read_count_ = 0;
    closed_ = false;
}

void CNLayerFeatureStream::Close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    while (!buffer_.empty()) {
        buffer_.pop();
    }
}

CNBackpressureStatus CNLayerFeatureStream::GetBackpressureStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

void CNLayerFeatureStream::SetBackpressureConfig(const CNBackpressureConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

} // namespace ogc
