#include "parser/performance_benchmark.h"
#include "parser/s57_parser.h"
#include "parser/error_handler.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace chart {
namespace parser {

PerformanceBenchmark& PerformanceBenchmark::Instance() {
    static PerformanceBenchmark instance;
    return instance;
}

PerformanceBenchmark::PerformanceBenchmark()
    : m_warmupIterations(3)
    , m_testIterations(10) {
}

PerformanceBenchmark::~PerformanceBenchmark() {
}

PerformanceBenchmark::MemoryInfo PerformanceBenchmark::GetMemoryInfo() {
    MemoryInfo info = {0};
    
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    info.totalVirtualMem = memInfo.ullTotalPageFile;
    info.virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    info.totalPhysicalMem = memInfo.ullTotalPhys;
    info.physicalMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    info.virtualMemUsedByMe = pmc.PrivateUsage;
    info.physicalMemUsedByMe = pmc.WorkingSetSize;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    info.physicalMemUsedByMe = usage.ru_maxrss * 1024;
    info.virtualMemUsedByMe = info.physicalMemUsedByMe;
#endif
    
    return info;
}

size_t PerformanceBenchmark::GetCurrentRSS() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024;
#endif
}

void PerformanceBenchmark::Warmup(std::function<void()> func) {
    for (size_t i = 0; i < m_warmupIterations; ++i) {
        func();
    }
}

BenchmarkResult PerformanceBenchmark::RunBenchmark(
    const std::string& name,
    std::function<void()> func,
    size_t iterations) {
    
    if (iterations == 0) {
        iterations = m_testIterations;
    }
    
    BenchmarkResult result;
    result.testName = name;
    result.iterations = iterations;
    
    Warmup(func);
    
    std::vector<double> times;
    times.reserve(iterations);
    
    size_t memBefore = GetCurrentRSS();
    
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        times.push_back(elapsed);
    }
    
    size_t memAfter = GetCurrentRSS();
    result.memoryUsed = (memAfter > memBefore) ? (memAfter - memBefore) : 0;
    
    result.totalTimeMs = std::accumulate(times.begin(), times.end(), 0.0);
    result.avgTimeMs = result.totalTimeMs / iterations;
    result.minTimeMs = *std::min_element(times.begin(), times.end());
    result.maxTimeMs = *std::max_element(times.begin(), times.end());
    
    if (result.avgTimeMs > 0) {
        result.throughput = 1000.0 / result.avgTimeMs;
    }
    
    m_results.push_back(result);
    
    LOG_INFO("Benchmark '{}': avg={:.2f}ms, min={:.2f}ms, max={:.2f}ms, throughput={:.2f}/s",
             name, result.avgTimeMs, result.minTimeMs, result.maxTimeMs, result.throughput);
    
    return result;
}

BenchmarkResult PerformanceBenchmark::RunParsingBenchmark(
    const std::string& name,
    const std::string& filePath,
    size_t iterations) {
    
    S57Parser parser;
    ParseConfig config;
    
    auto parseFunc = [&parser, &filePath, &config]() {
        ParseResult result = parser.ParseChart(filePath, config);
    };
    
    BenchmarkResult result = RunBenchmark(name, parseFunc, iterations);
    result.testName = name + " (" + filePath + ")";
    
    return result;
}

void PerformanceBenchmark::RunAllBenchmarks() {
    LOG_INFO("Running all benchmarks...");
    
    RunBenchmark("ParseResult_Creation", []() {
        ParseResult result;
        result.features.resize(100);
        for (int i = 0; i < 100; ++i) {
            Feature f;
            f.id = std::to_string(i);
            f.geometry.type = GeometryType::Point;
            f.geometry.points.push_back(Point(i * 1.0, i * 2.0, 0));
            result.features.push_back(f);
        }
    }, 100);
    
    RunBenchmark("Geometry_Conversion", []() {
        Geometry geom;
        geom.type = GeometryType::Line;
        for (int i = 0; i < 1000; ++i) {
            geom.points.push_back(Point(i * 0.001, i * 0.002, 0));
        }
    }, 100);
    
    RunBenchmark("AttributeMap_Operations", []() {
        AttributeMap attrs;
        for (int i = 0; i < 50; ++i) {
            AttributeValue val;
            val.type = AttributeValue::Type::String;
            val.stringValue = "value_" + std::to_string(i);
            attrs["key_" + std::to_string(i)] = val;
        }
    }, 100);
    
    LOG_INFO("All benchmarks completed. {} results.", m_results.size());
}

void PerformanceBenchmark::GenerateReport(const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file) {
        LOG_ERROR("Failed to open report file: {}", outputPath);
        return;
    }
    
    file << "# Performance Benchmark Report\n\n";
    file << "Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
    
    file << "## Summary\n\n";
    file << "| Test Name | Iterations | Avg Time (ms) | Min Time (ms) | Max Time (ms) | Throughput (/s) | Memory (KB) |\n";
    file << "|-----------|------------|---------------|---------------|---------------|-----------------|-------------|\n";
    
    for (const auto& result : m_results) {
        file << "| " << result.testName 
             << " | " << result.iterations
             << " | " << std::fixed << std::setprecision(2) << result.avgTimeMs
             << " | " << result.minTimeMs
             << " | " << result.maxTimeMs
             << " | " << result.throughput
             << " | " << (result.memoryUsed / 1024)
             << " |\n";
    }
    
    file << "\n## Memory Usage\n\n";
    MemoryInfo memInfo = GetMemoryInfo();
    file << "- Total Physical Memory: " << (memInfo.totalPhysicalMem / 1024 / 1024) << " MB\n";
    file << "- Physical Memory Used: " << (memInfo.physicalMemUsed / 1024 / 1024) << " MB\n";
    file << "- Current Process RSS: " << (memInfo.physicalMemUsedByMe / 1024 / 1024) << " MB\n";
    
    LOG_INFO("Benchmark report generated: {}", outputPath);
}

ScopedTimer::ScopedTimer(const std::string& name)
    : m_name(name) {
    m_start = std::chrono::high_resolution_clock::now();
}

ScopedTimer::~ScopedTimer() {
    double elapsed = GetElapsedMs();
    LOG_DEBUG("Timer '{}': {:.2f} ms", m_name, elapsed);
}

double ScopedTimer::GetElapsedMs() const {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - m_start).count();
}

} // namespace parser
} // namespace chart
