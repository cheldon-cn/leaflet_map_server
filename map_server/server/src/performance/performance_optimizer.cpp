#include "performance_optimizer.h"
#include "../utils/logger.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace cycle {
namespace performance {

// PerformanceMetrics 实现
PerformanceMetrics::PerformanceMetrics() 
    : start_time_(std::chrono::steady_clock::now()) {
}

void PerformanceMetrics::RecordRequest(const std::string& endpoint, double response_time, bool success) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    total_requests_++;
    if (success) {
        successful_requests_++;
    }
    
    total_response_time_ += static_cast<int64_t>(response_time * 1000); // 转换为毫秒
    
    // 端点级统计
    auto& stats = endpoint_stats_[endpoint];
    stats.count++;
    stats.total_time += static_cast<int64_t>(response_time * 1000);
    if (!success) {
        stats.errors++;
    }
}

void PerformanceMetrics::RecordCacheHit(bool hit) {
    if (hit) {
        cache_hits_++;
    } else {
        cache_misses_++;
    }
}

void PerformanceMetrics::RecordConcurrentRequest(int count) {
    current_concurrent_ = count;
    int max_concurrent = max_concurrent_.load();
    while (count > max_concurrent && 
           !max_concurrent_.compare_exchange_weak(max_concurrent, count)) {
        // 循环直到成功更新
    }
}

double PerformanceMetrics::GetAverageResponseTime(const std::string& endpoint) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (endpoint.empty()) {
        if (successful_requests_ == 0) return 0.0;
        return static_cast<double>(total_response_time_) / successful_requests_ / 1000.0;
    }
    
    auto it = endpoint_stats_.find(endpoint);
    if (it == endpoint_stats_.end() || it->second.count == 0) {
        return 0.0;
    }
    
    return static_cast<double>(it->second.total_time) / it->second.count / 1000.0;
}

double PerformanceMetrics::GetRequestRate() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);
    
    if (duration.count() == 0) return 0.0;
    
    return static_cast<double>(total_requests_) / duration.count();
}

double PerformanceMetrics::GetCacheHitRate() const {
    int64_t total_cache_access = cache_hits_ + cache_misses_;
    if (total_cache_access == 0) return 0.0;
    
    return static_cast<double>(cache_hits_) / total_cache_access;
}

int PerformanceMetrics::GetMaxConcurrentRequests() const {
    return max_concurrent_.load();
}

int PerformanceMetrics::GetCurrentConcurrentRequests() const {
    return current_concurrent_.load();
}

void PerformanceMetrics::UpdateSystemMetrics(double cpu_usage, size_t memory_usage) {
    cpu_usage_ = cpu_usage;
    memory_usage_ = memory_usage;
}

double PerformanceMetrics::GetCpuUsage() const {
    return cpu_usage_.load();
}

size_t PerformanceMetrics::GetMemoryUsage() const {
    return memory_usage_.load();
}

void PerformanceMetrics::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    total_requests_ = 0;
    successful_requests_ = 0;
    cache_hits_ = 0;
    cache_misses_ = 0;
    total_response_time_ = 0;
    
    current_concurrent_ = 0;
    max_concurrent_ = 0;
    
    endpoint_stats_.clear();
    
    start_time_ = std::chrono::steady_clock::now();
}

std::string PerformanceMetrics::GenerateReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream report;
    report << std::fixed << std::setprecision(2);
    
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);
    
    report << "=== 性能指标报告 ===" << std::endl; 
    report << "运行时间: " << uptime.count() << " second" << std::endl;
    report << "总请求数: " << total_requests_ << std::endl;
    report << "成功请求: " << successful_requests_ << std::endl;
    report << "请求成功率: " << (total_requests_ > 0 ? 
        static_cast<double>(successful_requests_) / total_requests_ * 100 : 0) << "%" << std::endl;
    report << "平均响应时间: " << GetAverageResponseTime() << " second" << std::endl; 
    report << "请求速率: " << GetRequestRate() << " 请求/second" << std::endl;
    report << "缓存命中率: " << GetCacheHitRate() * 100 << "%" << std::endl;
    report << "最大并发数: " << max_concurrent_ << std::endl;
    report << "当前并发数: " << current_concurrent_ << std::endl;
    report << "CPU使用率: " << cpu_usage_ * 100 << "%" << std::endl;
    report << "内存使用: " << memory_usage_ / 1024 / 1024 << " MB" << std::endl;
    
    // 端点级统计
    if (!endpoint_stats_.empty()) {
        report << "\n端点统计:" << std::endl;
        for (const auto& [endpoint, stats] : endpoint_stats_) {
            double avg_time = stats.count > 0 ? 
                static_cast<double>(stats.total_time) / stats.count / 1000.0 : 0.0;
            double error_rate = stats.count > 0 ? 
                static_cast<double>(stats.errors) / stats.count * 100 : 0.0;
            
            report << "  " << endpoint << ": " << stats.count << " 请求, "
                   << "平均 " << avg_time << " 秒, "
                   << "错误率 " << error_rate << "%" << std::endl;
        }
    }
    
    return report.str();
}

// ConnectionPool 实现
ConnectionPool::ConnectionPool(size_t max_connections, size_t min_connections) 
    : max_connections_(max_connections), min_connections_(min_connections) {
    
    // 创建初始连接
    for (size_t i = 0; i < min_connections; ++i) {
        auto conn = std::make_shared<Connection>();
        conn->id = next_id_++;
        conn->in_use = false;
        conn->last_used = std::chrono::steady_clock::now();
        
        available_connections_.push(conn);
        all_connections_.push_back(conn);
    }
    
    StartCleanupThread();
}

ConnectionPool::~ConnectionPool() {
    StopCleanupThread();
}

std::shared_ptr<ConnectionPool::Connection> ConnectionPool::AcquireConnection(int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    // 如果有可用连接，直接返回
    if (!available_connections_.empty()) {
        auto conn = available_connections_.front();
        available_connections_.pop();
        conn->in_use = true;
        conn->last_used = std::chrono::steady_clock::now();
        return conn;
    }
    
    // 如果没有达到最大连接数，创建新连接
    if (all_connections_.size() < max_connections_) {
        auto conn = std::make_shared<Connection>();
        conn->id = next_id_++;
        conn->in_use = true;
        conn->last_used = std::chrono::steady_clock::now();
        
        all_connections_.push_back(conn);
        return conn;
    }
    
    // 等待连接释放
    if (timeout_ms > 0) {
        auto status = condition_.wait_for(lock, std::chrono::milliseconds(timeout_ms));
        if (status == std::cv_status::timeout) {
            return nullptr; // 超时
        }
        
        if (!available_connections_.empty()) {
            auto conn = available_connections_.front();
            available_connections_.pop();
            conn->in_use = true;
            conn->last_used = std::chrono::steady_clock::now();
            return conn;
        }
    }
    
    return nullptr; // 没有可用连接
}

void ConnectionPool::ReleaseConnection(std::shared_ptr<Connection> connection) {
    if (!connection) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    connection->in_use = false;
    connection->last_used = std::chrono::steady_clock::now();
    available_connections_.push(connection);
    
    condition_.notify_one();
}

size_t ConnectionPool::GetAvailableCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return available_connections_.size();
}

size_t ConnectionPool::GetInUseCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return all_connections_.size() - available_connections_.size();
}

size_t ConnectionPool::GetTotalCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return all_connections_.size();
}

void ConnectionPool::SetMaxConnections(size_t max_connections) {
    std::lock_guard<std::mutex> lock(mutex_);
    max_connections_ = max_connections;
}

void ConnectionPool::CleanupIdleConnections(int max_idle_time_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto max_idle_time = std::chrono::milliseconds(max_idle_time_ms);
    
    // 清理空闲连接，但保持最小连接数
    std::queue<std::shared_ptr<Connection>> new_available;
    std::vector<std::shared_ptr<Connection>> new_all;
    
    size_t kept_count = 0;
    
    while (!available_connections_.empty()) {
        auto conn = available_connections_.front();
        available_connections_.pop();
        
        auto idle_time = now - conn->last_used;
        
        if (idle_time < max_idle_time || kept_count < min_connections_) {
            new_available.push(conn);
            new_all.push_back(conn);
            kept_count++;
        }
        // 否则连接被丢弃
    }
    
    // 保留正在使用的连接
    for (const auto& conn : all_connections_) {
        if (conn->in_use) {
            new_all.push_back(conn);
        }
    }
    
    available_connections_ = std::move(new_available);
    all_connections_ = std::move(new_all);
}

void ConnectionPool::StartCleanupThread() {
    running_ = true;
    cleanup_thread_ = std::thread(&ConnectionPool::CleanupWorker, this);
}

void ConnectionPool::StopCleanupThread() {
    running_ = false;
    if (cleanup_thread_.joinable()) {
        cleanup_thread_.join();
    }
}

void ConnectionPool::CleanupWorker() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::minutes(1)); // 每分钟清理一次
        
        if (running_) {
            CleanupIdleConnections();
        }
    }
}

// ThreadPool 实现
ThreadPool::ThreadPool(size_t num_threads, size_t max_queue_size) 
    : max_queue_size_(max_queue_size) {
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::WorkerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    Shutdown();
}

void ThreadPool::WorkerThread() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            condition_.wait(lock, [this]() {
                return stop_ || !tasks_.empty();
            });
            
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            task = std::move(tasks_.front());
            tasks_.pop();
            active_threads_++;
        }
        
        try {
            task();
        } catch (const std::exception& e) {
            LOG_ERROR("ThreadPool task error: " + std::string(e.what()));
        }
        
        active_threads_--;
    }
}

size_t ThreadPool::GetQueueSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}

size_t ThreadPool::GetActiveThreads() const {
    return active_threads_.load();
}

size_t ThreadPool::GetTotalThreads() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return workers_.size();
}

void ThreadPool::Resize(size_t num_threads) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t current_size = workers_.size();
    
    if (num_threads > current_size) {
        // 增加线程
        for (size_t i = current_size; i < num_threads; ++i) {
            workers_.emplace_back(&ThreadPool::WorkerThread, this);
        }
    } else if (num_threads < current_size) {
        // 减少线程（通过自然退出）
        stop_ = true;
    }
    
    condition_.notify_all();
}

void ThreadPool::Shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    
    condition_.notify_all();
    
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    workers_.clear();
}

// RateLimiter 实现
RateLimiter::RateLimiter(int max_requests, std::chrono::milliseconds time_window)
    : max_requests_(max_requests), time_window_(time_window) {
    
    global_bucket_.tokens = max_requests;
    global_bucket_.last_refill = std::chrono::steady_clock::now();
}

bool RateLimiter::AllowRequest() {
    std::lock_guard<std::mutex> lock(mutex_);
    RefillBucket(global_bucket_);
    
    if (global_bucket_.tokens > 0) {
        global_bucket_.tokens--;
        return true;
    }
    
    return false;
}

bool RateLimiter::AllowRequest(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& bucket = key_buckets_[key];
    RefillBucket(bucket);
    
    if (bucket.tokens > 0) {
        bucket.tokens--;
        return true;
    }
    
    return false;
}

void RateLimiter::SetLimit(int max_requests, std::chrono::milliseconds time_window) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    max_requests_ = max_requests;
    time_window_ = time_window;
    
    // 重置所有桶
    global_bucket_.tokens = max_requests;
    global_bucket_.last_refill = std::chrono::steady_clock::now();
    
    for (auto& [key, bucket] : key_buckets_) {
        bucket.tokens = max_requests;
        bucket.last_refill = std::chrono::steady_clock::now();
    }
}

int RateLimiter::GetRemainingRequests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return global_bucket_.tokens;
}

int RateLimiter::GetRemainingRequests(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = key_buckets_.find(key);
    if (it == key_buckets_.end()) {
        return max_requests_;
    }
    
    return it->second.tokens;
}

void RateLimiter::RefillBucket(TokenBucket& bucket) {
    auto now = std::chrono::steady_clock::now();
    auto time_since_refill = now - bucket.last_refill;
    
    if (time_since_refill >= time_window_) {
        bucket.tokens = max_requests_;
        bucket.last_refill = now;
    }
}

// PerformanceOptimizer 实现
PerformanceOptimizer::PerformanceOptimizer() {
    metrics_ = std::make_unique<PerformanceMetrics>();
}

PerformanceOptimizer::~PerformanceOptimizer() {
    StopMonitoring();
}

bool PerformanceOptimizer::Initialize(const std::string& config_path) {
    // 这里可以加载性能优化配置
    optimal_thread_count_ = std::thread::hardware_concurrency();
    optimal_connection_count_ = optimal_thread_count_ * 2;
    rate_limit_ = 100; // 默认速率限制
    
    // 初始化连接池和线程池
    connection_pool_ = std::make_unique<ConnectionPool>(optimal_connection_count_);
    thread_pool_ = std::make_unique<ThreadPool>(optimal_thread_count_);
    rate_limiter_ = std::make_unique<RateLimiter>(rate_limit_, std::chrono::seconds(1));
    
    LOG_INFO("PerformanceOptimizer initialized");
    return true;
}

void PerformanceOptimizer::OptimizeThreadPool() {
    if (!thread_pool_) return;
    
    // 基于当前负载调整线程池大小
    size_t current_threads = thread_pool_->GetTotalThreads();
    size_t active_threads = thread_pool_->GetActiveThreads();
    size_t queue_size = thread_pool_->GetQueueSize();
    
    // 如果队列中有任务且所有线程都在忙，考虑增加线程
    if (queue_size > 0 && active_threads == current_threads) {
        size_t new_size = std::min(current_threads * 2, optimal_thread_count_ * 4);
        if (new_size > current_threads) {
            thread_pool_->Resize(new_size);
            LOG_INFO("ThreadPool resized to " + std::to_string(new_size) + " threads");
        }
    }
    // 如果线程空闲较多，考虑减少线程
    else if (active_threads < current_threads / 2 && current_threads > optimal_thread_count_) {
        size_t new_size = std::max(active_threads + 2, optimal_thread_count_);
        thread_pool_->Resize(new_size);
        LOG_INFO("ThreadPool resized to " + std::to_string(new_size) + " threads");
    }
}

void PerformanceOptimizer::OptimizeConnectionPool() {
    if (!connection_pool_) return;
    
    // 基于使用情况调整连接池
    size_t in_use = connection_pool_->GetInUseCount();
    size_t total = connection_pool_->GetTotalCount();
    
    // 如果使用率超过80%，考虑增加连接
    if (total > 0 && static_cast<double>(in_use) / total > 0.8) {
        size_t new_max = std::min(total * 2, optimal_connection_count_ * 4);
        connection_pool_->SetMaxConnections(new_max);
        LOG_INFO("ConnectionPool max connections increased to " + std::to_string(new_max));
    }
}

void PerformanceOptimizer::OptimizeCacheStrategy() {
    // 基于缓存命中率优化缓存策略
    double hit_rate = metrics_->GetCacheHitRate();
    
    if (hit_rate < 0.7) {
        // 缓存命中率低，可能需要调整缓存策略
        LOG_WARN("Cache hit rate is low: " + std::to_string(hit_rate * 100) + "%");
    }
}

void PerformanceOptimizer::OptimizeMemoryUsage() {
    size_t memory_usage = metrics_->GetMemoryUsage();
    
    // 如果内存使用过高，触发垃圾回收或缓存清理
    if (memory_usage > 1024 * 1024 * 1024) { // 1GB
        LOG_WARN("High memory usage detected: " + std::to_string(memory_usage / 1024 / 1024) + " MB");
        // 这里可以触发缓存清理或其他内存优化操作
    }
}

void PerformanceOptimizer::AdjustBasedOnLoad() {
    double request_rate = metrics_->GetRequestRate();
    int concurrent_requests = metrics_->GetCurrentConcurrentRequests();
    
    // 基于请求速率调整速率限制
    if (request_rate > rate_limit_ * 0.8) {
        int new_limit = static_cast<int>(request_rate * 1.2);
        rate_limiter_->SetLimit(new_limit, std::chrono::seconds(1));
        rate_limit_ = new_limit;
        LOG_INFO("Rate limit adjusted to " + std::to_string(new_limit) + " requests/second");
    }
}

void PerformanceOptimizer::AdjustBasedOnMetrics(const PerformanceMetrics& metrics) {
    // 基于详细指标进行优化
    double avg_response_time = metrics.GetAverageResponseTime();
    
    if (avg_response_time > 1.0) { // 响应时间超过1秒
        LOG_WARN("High response time detected: " + std::to_string(avg_response_time) + " seconds");
        // 触发性能优化措施
    }
}

void PerformanceOptimizer::RunPerformanceTest(const std::string& test_type, int duration_seconds) {
    LOG_INFO("Starting performance test: " + test_type + " for " + std::to_string(duration_seconds) + " seconds");
    
    // 这里可以实现具体的性能测试逻辑
    // 例如：并发请求测试、压力测试等
    
    LOG_INFO("Performance test completed");
}

std::string PerformanceOptimizer::GetPerformanceReport() const {
    if (!metrics_) return "Performance metrics not available";
    return metrics_->GenerateReport();
}

void PerformanceOptimizer::StartMonitoring() {
    if (monitoring_) return;
    
    monitoring_ = true;
    monitor_thread_ = std::thread(&PerformanceOptimizer::MonitorWorker, this);
    LOG_INFO("Performance monitoring started");
}

void PerformanceOptimizer::StopMonitoring() {
    if (!monitoring_) return;
    
    monitoring_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
    LOG_INFO("Performance monitoring stopped");
}

void PerformanceOptimizer::MonitorWorker() {
    while (monitoring_) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // 每10秒检查一次
        
        if (!monitoring_) break;
        
        try {
            AnalyzePerformancePatterns();
            ApplyOptimizations();
        } catch (const std::exception& e) {
            LOG_ERROR("Performance monitoring error: " + std::string(e.what()));
        }
    }
}

void PerformanceOptimizer::AnalyzePerformancePatterns() {
    // 分析性能模式，检测异常
    double response_time = metrics_->GetAverageResponseTime();
    double request_rate = metrics_->GetRequestRate();
    
    if (response_time > 2.0 && request_rate > 50) {
        LOG_WARN("Performance degradation detected: high response time under load");
    }
}

void PerformanceOptimizer::ApplyOptimizations() {
    // 应用性能优化
    OptimizeThreadPool();
    OptimizeConnectionPool();
    OptimizeCacheStrategy();
    OptimizeMemoryUsage();
    AdjustBasedOnLoad();
}

} // namespace performance
} // namespace cycle