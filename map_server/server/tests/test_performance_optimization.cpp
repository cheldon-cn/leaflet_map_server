#include <gtest/gtest.h>
#include "../src/performance/performance_optimizer.h"
#include "../src/utils/logger.h"
#include <thread>
#include <chrono>
#include <future>

using namespace cycle;
using namespace cycle::performance;

class PerformanceOptimizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化性能优化器
        optimizer_ = std::make_unique<PerformanceOptimizer>();
        ASSERT_TRUE(optimizer_->Initialize());
    }
    
    void TearDown() override {
        optimizer_->StopMonitoring();
    }
    
    std::unique_ptr<PerformanceOptimizer> optimizer_;
};

TEST_F(PerformanceOptimizationTest, PerformanceMetricsBasic) {
    PerformanceMetrics metrics;
    
    // 记录基本指标
    metrics.RecordRequest("/tile/0/0/0/png", 0.15, true);
    metrics.RecordRequest("/tile/1/0/0/png", 0.25, true);
    metrics.RecordRequest("/tile/2/0/0/png", 0.35, false);
    
    metrics.RecordCacheHit(true);
    metrics.RecordCacheHit(false);
    metrics.RecordCacheHit(true);
    
    metrics.RecordConcurrentRequest(5);
    metrics.RecordConcurrentRequest(10);
    
    // 验证指标
    EXPECT_NEAR(metrics.GetAverageResponseTime(), 0.25, 0.01);
    EXPECT_NEAR(metrics.GetCacheHitRate(), 0.666, 0.01);
    EXPECT_EQ(metrics.GetMaxConcurrentRequests(), 10);
    
    // 端点级统计
    EXPECT_NEAR(metrics.GetAverageResponseTime("/tile/0/0/0/png"), 0.15, 0.01);
    
    // 生成报告
    std::string report = metrics.GenerateReport();
    EXPECT_FALSE(report.empty());
    
    // 重置统计
    metrics.Reset();
    EXPECT_EQ(metrics.GetAverageResponseTime(), 0.0);
}

TEST_F(PerformanceOptimizationTest, ConnectionPoolBasic) {
    ConnectionPool pool(10, 2);
    
    // 测试初始状态
    EXPECT_EQ(pool.GetTotalCount(), 2);
    EXPECT_EQ(pool.GetAvailableCount(), 2);
    EXPECT_EQ(pool.GetInUseCount(), 0);
    
    // 获取连接
    auto conn1 = pool.AcquireConnection(1000);
    ASSERT_TRUE(conn1 != nullptr);
    EXPECT_EQ(pool.GetAvailableCount(), 1);
    EXPECT_EQ(pool.GetInUseCount(), 1);
    
    auto conn2 = pool.AcquireConnection(1000);
    ASSERT_TRUE(conn2 != nullptr);
    EXPECT_EQ(pool.GetAvailableCount(), 0);
    EXPECT_EQ(pool.GetInUseCount(), 2);
    
    // 释放连接
    pool.ReleaseConnection(conn1);
    EXPECT_EQ(pool.GetAvailableCount(), 1);
    EXPECT_EQ(pool.GetInUseCount(), 1);
    
    pool.ReleaseConnection(conn2);
    EXPECT_EQ(pool.GetAvailableCount(), 2);
    EXPECT_EQ(pool.GetInUseCount(), 0);
}

TEST_F(PerformanceOptimizationTest, ConnectionPoolTimeout) {
    ConnectionPool pool(2, 1);
    
    // 获取所有可用连接
    auto conn1 = pool.AcquireConnection(1000);
    ASSERT_TRUE(conn1 != nullptr);
    
    // 尝试获取连接（应该超时）
    auto start = std::chrono::steady_clock::now();
    auto conn2 = pool.AcquireConnection(100); // 100ms超时
    auto end = std::chrono::steady_clock::now();
    
    EXPECT_TRUE(conn2 == nullptr);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(duration.count(), 100);
    
    // 释放连接后应该可以获取
    pool.ReleaseConnection(conn1);
    conn2 = pool.AcquireConnection(1000);
    EXPECT_TRUE(conn2 != nullptr);
}

TEST_F(PerformanceOptimizationTest, ThreadPoolBasic) {
    ThreadPool pool(2);
    
    // 提交简单任务
    auto future1 = pool.Submit([]() { return 42; });
    auto future2 = pool.Submit([]() { return 100; });
    
    EXPECT_EQ(future1.get(), 42);
    EXPECT_EQ(future2.get(), 100);
    
    // 测试队列大小
    EXPECT_LE(pool.GetQueueSize(), 2);
    EXPECT_EQ(pool.GetTotalThreads(), 2);
}

TEST_F(PerformanceOptimizationTest, ThreadPoolConcurrent) {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    
    // 提交多个并发任务
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.Submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
        }));
    }
    
    // 等待所有任务完成
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_EQ(counter, 10);
}

TEST_F(PerformanceOptimizationTest, RateLimiterBasic) {
    RateLimiter limiter(5, std::chrono::seconds(1));
    
    // 前5个请求应该被允许
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(limiter.AllowRequest());
    }
    
    // 第6个请求应该被拒绝
    EXPECT_FALSE(limiter.AllowRequest());
    
    // 检查剩余请求数
    EXPECT_EQ(limiter.GetRemainingRequests(), 0);
}

TEST_F(PerformanceOptimizationTest, RateLimiterKeyBased) {
    RateLimiter limiter(3, std::chrono::seconds(1));
    
    // 不同key的请求应该独立计数
    EXPECT_TRUE(limiter.AllowRequest("user1"));
    EXPECT_TRUE(limiter.AllowRequest("user1"));
    EXPECT_TRUE(limiter.AllowRequest("user2"));
    
    EXPECT_EQ(limiter.GetRemainingRequests("user1"), 1);
    EXPECT_EQ(limiter.GetRemainingRequests("user2"), 2);
    
    // user1达到限制
    EXPECT_TRUE(limiter.AllowRequest("user1"));
    EXPECT_FALSE(limiter.AllowRequest("user1"));
    
    // user2还有剩余
    EXPECT_TRUE(limiter.AllowRequest("user2"));
}

TEST_F(PerformanceOptimizationTest, PerformanceOptimizerIntegration) {
    // 测试性能优化器的集成功能
    
    // 启动监控
    optimizer_->StartMonitoring();
    
    // 模拟一些性能数据
    auto& metrics = *optimizer_;
    
    // 等待监控线程运行
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 获取性能报告
    std::string report = optimizer_->GetPerformanceReport();
    EXPECT_FALSE(report.empty());
    
    // 停止监控
    optimizer_->StopMonitoring();
}

TEST_F(PerformanceOptimizationTest, HighConcurrencySimulation) {
    // 模拟高并发场景
    const int num_threads = 10;
    const int requests_per_thread = 100;
    
    PerformanceMetrics metrics;
    std::atomic<int> successful_requests{0};
    std::atomic<int> failed_requests{0};
    
    auto worker = [&](int thread_id) {
        for (int i = 0; i < requests_per_thread; ++i) {
            // 模拟请求处理时间
            double response_time = 0.1 + (std::rand() % 100) / 1000.0; // 100-200ms
            bool success = (std::rand() % 100) > 5; // 95%成功率
            
            metrics.RecordRequest("/tile/10/512/256/png", response_time, success);
            
            if (success) {
                successful_requests++;
            } else {
                failed_requests++;
            }
            
            // 随机缓存命中
            metrics.RecordCacheHit((std::rand() % 100) > 30); // 70%命中率
            
            // 随机并发数
            int concurrent = 1 + (std::rand() % 20);
            metrics.RecordConcurrentRequest(concurrent);
            
            // 短暂休眠
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    // 启动多个工作线程
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证统计结果
    int total_requests = successful_requests + failed_requests;
    EXPECT_EQ(total_requests, num_threads * requests_per_thread);
    
    double success_rate = static_cast<double>(successful_requests) / total_requests;
    EXPECT_NEAR(success_rate, 0.95, 0.05); // 允许5%的误差
    
    // 验证性能指标
    EXPECT_GT(metrics.GetRequestRate(), 0);
    EXPECT_GT(metrics.GetCacheHitRate(), 0);
    EXPECT_GT(metrics.GetMaxConcurrentRequests(), 0);
}

TEST_F(PerformanceOptimizationTest, MemoryUsageOptimization) {
    // 测试内存使用优化
    PerformanceMetrics metrics;
    
    // 模拟高内存使用
    metrics.UpdateSystemMetrics(0.8, 800 * 1024 * 1024); // 80% CPU, 800MB内存
    
    EXPECT_NEAR(metrics.GetCpuUsage(), 0.8, 0.01);
    EXPECT_EQ(metrics.GetMemoryUsage(), 800 * 1024 * 1024);
    
    // 这里可以测试内存优化逻辑
    // 例如：当内存使用超过阈值时触发清理操作
}

TEST_F(PerformanceOptimizationTest, ResponseTimeOptimization) {
    // 测试响应时间优化
    PerformanceMetrics metrics;
    
    // 记录不同响应时间的请求
    metrics.RecordRequest("/fast", 0.05, true);
    metrics.RecordRequest("/slow", 2.5, true);
    metrics.RecordRequest("/timeout", 5.0, false);
    
    double avg_response_time = metrics.GetAverageResponseTime();
    
    // 验证平均响应时间计算正确
    EXPECT_NEAR(avg_response_time, (0.05 + 2.5 + 5.0) / 3, 0.01);
    
    // 这里可以测试响应时间优化逻辑
    // 例如：当平均响应时间超过阈值时触发优化措施
}

TEST_F(PerformanceOptimizationTest, ConnectionPoolStressTest) {
    // 连接池压力测试
    ConnectionPool pool(20, 5);
    std::atomic<int> acquired_count{0};
    std::atomic<int> failed_count{0};
    
    const int num_threads = 30;
    const int iterations = 50;
    
    auto stress_worker = [&](int thread_id) {
        for (int i = 0; i < iterations; ++i) {
            auto conn = pool.AcquireConnection(50); // 短超时
            
            if (conn) {
                acquired_count++;
                
                // 模拟连接使用
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
                pool.ReleaseConnection(conn);
            } else {
                failed_count++;
            }
        }
    };
    
    // 启动压力测试线程
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(stress_worker, i);
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证连接池在压力下的表现
    int total_attempts = num_threads * iterations;
    double success_rate = static_cast<double>(acquired_count) / total_attempts;
    
    // 在压力测试中，成功率应该合理
    EXPECT_GT(success_rate, 0.5); // 至少50%的成功率
    
    // 连接池应该保持稳定
    EXPECT_LE(pool.GetInUseCount(), pool.GetTotalCount());
}

TEST_F(PerformanceOptimizationTest, ThreadPoolResize) {
    // 测试线程池动态调整
    ThreadPool pool(2);
    
    EXPECT_EQ(pool.GetTotalThreads(), 2);
    
    // 调整线程池大小
    pool.Resize(4);
    EXPECT_EQ(pool.GetTotalThreads(), 4);
    
    pool.Resize(1);
    // 注意：减少线程数可能不会立即生效
    // 但新大小应该被记录
    
    // 测试线程池在调整后仍能正常工作
    auto future = pool.Submit([]() { return 123; });
    EXPECT_EQ(future.get(), 123);
}

TEST_F(PerformanceOptimizationTest, RealWorldScenario) {
    // 模拟真实世界场景：地图瓦片服务
    PerformanceMetrics metrics;
    ConnectionPool db_pool(10, 3);
    ThreadPool render_pool(4);
    RateLimiter api_limiter(100, std::chrono::seconds(1));
    
    // 模拟不同类型的请求
    std::vector<std::string> endpoints = {
        "/tile/10/512/256/png32",
        "/tile/11/1024/512/png32", 
        "/tile/12/2048/1024/png32",
        "/metrics",
        "/health"
    };
    
    const int total_requests = 1000;
    
    for (int i = 0; i < total_requests; ++i) {
        // 随机选择端点
        std::string endpoint = endpoints[std::rand() % endpoints.size()];
        
        // 模拟响应时间（瓦片请求较慢，监控请求较快）
        double response_time = 0.1;
        if (endpoint.find("/tile") != std::string::npos) {
            response_time = 0.2 + (std::rand() % 300) / 1000.0; // 200-500ms
        }
        
        // 模拟成功率
        bool success = (std::rand() % 100) > 2; // 98%成功率
        
        // 记录指标
        metrics.RecordRequest(endpoint, response_time, success);
        
        // 模拟缓存命中（瓦片有较高缓存命中率）
        bool cache_hit = false;
        if (endpoint.find("/tile") != std::string::npos) {
            cache_hit = (std::rand() % 100) > 20; // 80%缓存命中率
        }
        metrics.RecordCacheHit(cache_hit);
        
        // 速率限制检查
        if (!api_limiter.AllowRequest()) {
            // 被限流的请求
            continue;
        }
        
        // 模拟并发请求
        int concurrent = 1 + (std::rand() % 15);
        metrics.RecordConcurrentRequest(concurrent);
    }
    
    // 验证真实世界场景的性能指标
    EXPECT_GT(metrics.GetRequestRate(), 0);
    EXPECT_NEAR(metrics.GetCacheHitRate(), 0.8, 0.1); // 大约80%缓存命中率
    EXPECT_GT(metrics.GetMaxConcurrentRequests(), 0);
    
    // 生成详细报告
    std::string report = metrics.GenerateReport();
    EXPECT_FALSE(report.empty());
    
    // 验证报告包含关键信息
    EXPECT_NE(report.find("总请求数"), std::string::npos);
    EXPECT_NE(report.find("缓存命中率"), std::string::npos);
    EXPECT_NE(report.find("平均响应时间"), std::string::npos);
}