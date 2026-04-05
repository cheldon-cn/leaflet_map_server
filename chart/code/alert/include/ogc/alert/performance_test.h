#ifndef OGC_ALERT_PERFORMANCE_TEST_H
#define OGC_ALERT_PERFORMANCE_TEST_H

#include "export.h"
#include "types.h"
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <map>

namespace ogc {
namespace alert {

struct PerformanceMetrics {
    std::string operation_name;
    int total_operations;
    double total_time_ms;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    double operations_per_second;
    int timeout_count;
    double p50_time_ms;
    double p95_time_ms;
    double p99_time_ms;
};

struct PerformanceThreshold {
    double max_avg_time_ms;
    double max_p95_time_ms;
    double max_p99_time_ms;
    int max_timeout_count;
};

struct PerformanceReport {
    std::string test_name;
    DateTime test_time;
    std::vector<PerformanceMetrics> metrics;
    std::map<std::string, PerformanceThreshold> thresholds;
    bool all_passed;
    std::string summary;
};

class OGC_ALERT_API PerformanceTimer {
public:
    PerformanceTimer();
    explicit PerformanceTimer(const std::string& name);
    ~PerformanceTimer();
    
    void Start();
    void Stop();
    void Reset();
    
    double GetElapsedMs() const;
    double GetElapsedSeconds() const;
    
    static double GetTimestampMs();

private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start;
    std::chrono::high_resolution_clock::time_point m_end;
    bool m_running;
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
    
    PerformanceReport GenerateReport() const;
    void Clear();
    
    static PerformanceProfiler& Instance();

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
    
    PerformanceReport RunAlertEngineBenchmark();
    PerformanceReport RunRepositoryBenchmark();
    PerformanceReport RunQueryBenchmark();
    PerformanceReport RunPushServiceBenchmark();
    PerformanceReport RunFullSystemBenchmark();
    
    void SaveReport(const std::string& filepath);
    void PrintReport() const;

private:
    BenchmarkConfig m_config;
    std::vector<PerformanceMetrics> m_results;
    
    void Warmup(std::function<void()> func);
    std::vector<double> MeasureTimes(std::function<void()> func);
    PerformanceMetrics CalculateMetrics(const std::string& name, const std::vector<double>& times);
    double CalculatePercentile(const std::vector<double>& sorted_times, double percentile);
};

class OGC_ALERT_API PerformanceOptimizer {
public:
    struct OptimizationResult {
        std::string component_name;
        std::string optimization_type;
        double before_time_ms;
        double after_time_ms;
        double improvement_percent;
        std::string description;
    };
    
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    std::vector<std::string> IdentifyBottlenecks(const PerformanceReport& report);
    
    OptimizationResult OptimizeCacheSize(const std::string& component, int new_size);
    OptimizationResult OptimizeThreadPoolSize(int new_size);
    OptimizationResult OptimizeBatchSize(const std::string& operation, int new_size);
    OptimizationResult OptimizeIndexing(const std::string& table, const std::vector<std::string>& columns);
    
    void ApplyOptimizations();
    void RollbackOptimizations();
    
    std::vector<OptimizationResult> GetOptimizationHistory() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
