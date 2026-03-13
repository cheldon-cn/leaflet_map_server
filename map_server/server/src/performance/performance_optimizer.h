#ifndef CYCLE_PERFORMANCE_PERFORMANCE_OPTIMIZER_H
#define CYCLE_PERFORMANCE_PERFORMANCE_OPTIMIZER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <queue>
#include <condition_variable>
#include <unordered_map>
#include <future>

namespace cycle {
namespace performance {

class PerformanceMetrics {
public:
    PerformanceMetrics();
    
    void RecordRequest(const std::string& endpoint, double response_time, bool success);
    void RecordCacheHit(bool hit);
    void RecordConcurrentRequest(int count);
    
    double GetAverageResponseTime(const std::string& endpoint = "") const;
    double GetRequestRate() const;
    double GetCacheHitRate() const;
    int GetMaxConcurrentRequests() const;
    int GetCurrentConcurrentRequests() const;
    
    void UpdateSystemMetrics(double cpu_usage, size_t memory_usage);
    double GetCpuUsage() const;
    size_t GetMemoryUsage() const;
    
    void Reset();
    
    std::string GenerateReport() const;
    
private:
    mutable std::mutex mutex_;
    
    std::atomic<int64_t> total_requests_{0};
    std::atomic<int64_t> successful_requests_{0};
    std::atomic<int64_t> cache_hits_{0};
    std::atomic<int64_t> cache_misses_{0};
    std::atomic<int64_t> total_response_time_{0};
    
    std::atomic<int> current_concurrent_{0};
    std::atomic<int> max_concurrent_{0};
    
    std::atomic<double> cpu_usage_{0};
    std::atomic<size_t> memory_usage_{0};
    
    std::chrono::steady_clock::time_point start_time_;
    
    struct EndpointStats {
        int64_t count{0};
        int64_t total_time{0};
        int64_t errors{0};
    };
    
    std::unordered_map<std::string, EndpointStats> endpoint_stats_;
};

class ConnectionPool {
public:
    struct Connection {
        int id;
        bool in_use;
        std::chrono::steady_clock::time_point last_used;
    };
    
    ConnectionPool(size_t max_connections, size_t min_connections = 5);
    ~ConnectionPool();
    
    std::shared_ptr<Connection> AcquireConnection(int timeout_ms = 5000);
    void ReleaseConnection(std::shared_ptr<Connection> connection);
    
    size_t GetAvailableCount() const;
    size_t GetInUseCount() const;
    size_t GetTotalCount() const;
    
    void SetMaxConnections(size_t max_connections);
    void CleanupIdleConnections(int max_idle_time_ms = 300000);
    
private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    
    std::queue<std::shared_ptr<Connection>> available_connections_;
    std::vector<std::shared_ptr<Connection>> all_connections_;
    
    size_t max_connections_;
    size_t min_connections_;
    std::atomic<int> next_id_{0};
    
    std::thread cleanup_thread_;
    std::atomic<bool> running_{false};
    
    void StartCleanupThread();
    void StopCleanupThread();
    void CleanupWorker();
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads, size_t max_queue_size = 1000);
    ~ThreadPool();
    
    template<typename F, typename... Args>
    auto Submit(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    
    size_t GetQueueSize() const;
    size_t GetActiveThreads() const;
    size_t GetTotalThreads() const;
    
    void Resize(size_t num_threads);
    void Shutdown();
    
private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    size_t max_queue_size_;
    std::atomic<bool> stop_{false};
    std::atomic<size_t> active_threads_{0};
    
    void WorkerThread();
};

class RateLimiter {
public:
    RateLimiter(int max_requests, std::chrono::milliseconds time_window);
    
    bool AllowRequest();
    bool AllowRequest(const std::string& key);
    
    void SetLimit(int max_requests, std::chrono::milliseconds time_window);
    
    int GetRemainingRequests() const;
    int GetRemainingRequests(const std::string& key) const;
    
private:
    struct TokenBucket {
        int tokens;
        std::chrono::steady_clock::time_point last_refill;
    };
    
    mutable std::mutex mutex_;
    
    int max_requests_;
    std::chrono::milliseconds time_window_;
    
    TokenBucket global_bucket_;
    std::unordered_map<std::string, TokenBucket> key_buckets_;
    
    void RefillBucket(TokenBucket& bucket);
};

class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    bool Initialize(const std::string& config_path = "");
    
    void OptimizeThreadPool();
    void OptimizeConnectionPool();
    void OptimizeCacheStrategy();
    void OptimizeMemoryUsage();
    
    void AdjustBasedOnLoad();
    void AdjustBasedOnMetrics(const PerformanceMetrics& metrics);
    
    void RunPerformanceTest(const std::string& test_type, int duration_seconds = 60);
    std::string GetPerformanceReport() const;
    
    void StartMonitoring();
    void StopMonitoring();
    
private:
    mutable std::mutex mutex_;
    
    std::unique_ptr<PerformanceMetrics> metrics_;
    std::unique_ptr<ConnectionPool> connection_pool_;
    std::unique_ptr<ThreadPool> thread_pool_;
    std::unique_ptr<RateLimiter> rate_limiter_;
    
    std::thread monitor_thread_;
    std::atomic<bool> monitoring_{false};
    
    size_t optimal_thread_count_{0};
    size_t optimal_connection_count_{0};
    int rate_limit_{100};
    
    void MonitorWorker();
    void AnalyzePerformancePatterns();
    void ApplyOptimizations();
};

template<typename F, typename... Args>
auto ThreadPool::Submit(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> result = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (stop_) {
            throw std::runtime_error("ThreadPool is stopped");
        }
        
        if (tasks_.size() >= max_queue_size_) {
            throw std::runtime_error("ThreadPool queue is full");
        }
        
        tasks_.emplace([task](){ (*task)(); });
    }
    
    condition_.notify_one();
    return result;
}

} // namespace performance
} // namespace cycle

#endif // CYCLE_PERFORMANCE_PERFORMANCE_OPTIMIZER_H
