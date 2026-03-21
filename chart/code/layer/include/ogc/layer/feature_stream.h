#pragma once

#include "ogc/layer/layer.h"

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace ogc {

enum class CNBackpressureStrategy {
    kNone,
    kBlock,
    kDrop,
    kBuffer,
    kThrottle
};

struct CNBackpressureConfig {
    CNBackpressureStrategy strategy = CNBackpressureStrategy::kBlock;
    size_t max_buffer_size = 10000;
    int block_timeout_ms = 5000;
    double throttle_factor = 0.8;
    bool enable_monitoring = true;
};

struct CNBackpressureStatus {
    size_t current_buffer_size = 0;
    size_t total_produced = 0;
    size_t total_consumed = 0;
    size_t total_dropped = 0;
    size_t total_blocked = 0;
    double production_rate = 0.0;
    double consumption_rate = 0.0;
    bool is_backpressured = false;
};

class OGC_LAYER_API CNFeatureStream {
public:
    virtual ~CNFeatureStream() = default;

    virtual std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(size_t batch_size = 1000) = 0;

    virtual bool IsEndOfStream() const = 0;

    virtual int64_t GetReadCount() const = 0;

    virtual void Reset() = 0;

    virtual void Close() = 0;

    virtual CNBackpressureStatus GetBackpressureStatus() const = 0;
};

class OGC_LAYER_API CNLayerFeatureStream : public CNFeatureStream {
public:
    CNLayerFeatureStream(CNLayer* layer, size_t batch_size = 1000);

    ~CNLayerFeatureStream() override;

    std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(size_t batch_size = 1000) override;

    bool IsEndOfStream() const override;

    int64_t GetReadCount() const override;

    void Reset() override;

    void Close() override;

    CNBackpressureStatus GetBackpressureStatus() const override;

    void SetBackpressureConfig(const CNBackpressureConfig& config);

private:
    CNLayer* layer_;
    size_t batch_size_;
    int64_t read_count_ = 0;
    bool closed_ = false;
    CNBackpressureConfig config_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::unique_ptr<CNFeature>> buffer_;
    CNBackpressureStatus status_;
};

} // namespace ogc
