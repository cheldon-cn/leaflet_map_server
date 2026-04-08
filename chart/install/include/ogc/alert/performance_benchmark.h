#ifndef OGC_ALERT_PERFORMANCE_BENCHMARK_H
#define OGC_ALERT_PERFORMANCE_BENCHMARK_H

#include "export.h"
#include "types.h"
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <map>
#include <memory>

namespace ogc {
namespace alert {

struct PerformanceMetrics {
    std::string operation_name;
    int total_operations;
    double total_time_ms;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    int success_count;
    int failure_count;
    
    PerformanceMetrics();
};

struct OGC_ALERT_API PerformanceThreshold {
    std::string operation_name;
    double max_time_ms;
    double warning_threshold_ms;
    bool enabled;
    
    PerformanceThreshold();
    PerformanceThreshold(const std::string& name, double max_ms, double warn_ms);
};

struct OGC_ALERT_API PerformanceReport {
    std::string test_name;
    DateTime test_time;
    std::vector<PerformanceMetrics> metrics;
    std::map<std::string, PerformanceThreshold> thresholds;
    bool all_passed;
    std::string summary;
};

class OGC_ALERT_API PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void BeginSession(const std::string& session_name);
    void EndSession();
    
    void StartOperation(const std::string& operation_name);
    void EndOperation(const std::string& operation_name);
    
    void RecordMetric(const std::string& operation_name, double time_ms);
    
    PerformanceMetrics GetMetrics(const std::string& operation_name) const;
    std::vector<PerformanceMetrics> GetAllMetrics() const;
    
    void SetThreshold(const std::string& operation_name, const PerformanceThreshold& threshold);
    bool CheckThreshold(const std::string& operation_name) const;
    
    void Clear();
    
    static PerformanceProfiler* Instance();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

class OGC_ALERT_API PerformanceBenchmark {
public:
    struct BenchmarkConfig {
        int warmup_iterations;
        int test_iterations;
        int concurrent_threads;
        bool enable_profiling;
        double timeout_seconds;
    };
    
    PerformanceBenchmark();
    explicit PerformanceBenchmark(const BenchmarkConfig& config);
    ~PerformanceBenchmark();
    
    void SetConfig(const BenchmarkConfig& config);
    BenchmarkConfig GetConfig() const;
    
    PerformanceMetrics BenchmarkFunction(
        const std::string& name,
        std::function<void()> func);
    
    PerformanceMetrics BenchmarkFunctionWithSetup(
        const std::string& name,
        std::function<void()> setup,
        std::function<void()> func,
        std::function<void()> teardown);
    
    void RunAlertEngineBenchmark();
    void RunRepositoryBenchmark();
    void RunQueryBenchmark();
    void RunPushServiceBenchmark();
    void RunFullSystemBenchmark();
    
    PerformanceMetrics GetMetrics(const std::string& name) const;
    void SaveReport(const std::string& filepath);
    void PrintReport() const;
    void Clear();
    
    static PerformanceBenchmark* Instance();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

struct OptimizationSuggestion {
    std::string component;
    std::string issue;
    std::string suggestion;
    int priority;
    double estimated_improvement;
};

struct OptimizationResult {
    bool success;
    std::string message;
    double before_ms;
    double after_ms;
    double improvement_percent;
};

class OGC_ALERT_API PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    void AnalyzePerformance(const PerformanceProfiler& profiler);
    std::vector<OptimizationSuggestion> IdentifyBottlenecks(const PerformanceReport& report);
    
    OptimizationResult OptimizeCacheSize(const std::string& component, int new_size);
    OptimizationResult OptimizeThreadPoolSize(int new_size);
    OptimizationResult OptimizeBatchSize(const std::string& operation, int new_size);
    OptimizationResult OptimizeIndexing(const std::string& table, bool enable);
    
    void ApplyOptimizations(const std::vector<OptimizationSuggestion>& suggestions);
    
    std::string GenerateOptimizationReport() const;
    
    static PerformanceOptimizer* Instance();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
