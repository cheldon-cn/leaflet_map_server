#ifndef PERFORMANCE_BENCHMARK_H
#define PERFORMANCE_BENCHMARK_H

#include "parse_result.h"
#include <string>
#include <vector>
#include <chrono>
#include <functional>

namespace chart {
namespace parser {

struct BenchmarkResult {
    std::string testName;
    size_t iterations;
    double totalTimeMs;
    double avgTimeMs;
    double minTimeMs;
    double maxTimeMs;
    double throughput;
    size_t memoryUsed;
    
    BenchmarkResult()
        : iterations(0)
        , totalTimeMs(0)
        , avgTimeMs(0)
        , minTimeMs(0)
        , maxTimeMs(0)
        , throughput(0)
        , memoryUsed(0)
    {}
};

class PerformanceBenchmark {
public:
    static PerformanceBenchmark& Instance();
    
    void SetWarmupIterations(size_t count) { m_warmupIterations = count; }
    void SetTestIterations(size_t count) { m_testIterations = count; }
    
    BenchmarkResult RunBenchmark(
        const std::string& name,
        std::function<void()> func,
        size_t iterations = 0
    );
    
    BenchmarkResult RunParsingBenchmark(
        const std::string& name,
        const std::string& filePath,
        size_t iterations = 10
    );
    
    void RunAllBenchmarks();
    void GenerateReport(const std::string& outputPath);
    
    void ClearResults() { m_results.clear(); }
    const std::vector<BenchmarkResult>& GetResults() const { return m_results; }
    
    struct MemoryInfo {
        size_t totalVirtualMem;
        size_t virtualMemUsed;
        size_t virtualMemUsedByMe;
        size_t totalPhysicalMem;
        size_t physicalMemUsed;
        size_t physicalMemUsedByMe;
    };
    
    static MemoryInfo GetMemoryInfo();
    static size_t GetCurrentRSS();
    
private:
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    PerformanceBenchmark(const PerformanceBenchmark&) = delete;
    PerformanceBenchmark& operator=(const PerformanceBenchmark&) = delete;
    
    void Warmup(std::function<void()> func);
    
    size_t m_warmupIterations;
    size_t m_testIterations;
    std::vector<BenchmarkResult> m_results;
};

class ScopedTimer {
public:
    explicit ScopedTimer(const std::string& name);
    ~ScopedTimer();
    
    double GetElapsedMs() const;
    
private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};

#define SCOPED_TIMER(name) ScopedTimer timer_##name(#name)
#define BENCHMARK(name, func) \
    chart::parser::PerformanceBenchmark::Instance().RunBenchmark(name, func)

} // namespace parser
} // namespace chart

#endif // PERFORMANCE_BENCHMARK_H
