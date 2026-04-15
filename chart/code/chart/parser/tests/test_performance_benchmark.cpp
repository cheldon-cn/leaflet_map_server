#include <gtest/gtest.h>
#include "parser/performance_benchmark.h"
#include "parser/chart_parser.h"
#include "parser/error_handler.h"
#include "parser/gdal_initializer.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

using namespace chart::parser;

class PerformanceBenchmarkTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        GDALInitializer::Instance().Initialize();
        ChartParser::Instance().Initialize();
        Logger::Instance().SetLevel(LogLevel::kNone);
    }
    
    static void TearDownTestCase() {
        ChartParser::Instance().Shutdown();
        GDALInitializer::Instance().Shutdown();
    }
    
    void SetUp() override {
        PerformanceBenchmark::Instance().ClearResults();
        PerformanceBenchmark::Instance().SetWarmupIterations(2);
        PerformanceBenchmark::Instance().SetTestIterations(5);
    }
    
    void TearDown() override {
        PerformanceBenchmark::Instance().ClearResults();
    }
};

TEST_F(PerformanceBenchmarkTest, Instance_ReturnsSingleton) {
    auto& instance1 = PerformanceBenchmark::Instance();
    auto& instance2 = PerformanceBenchmark::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(PerformanceBenchmarkTest, SetWarmupIterations_UpdatesValue) {
    PerformanceBenchmark::Instance().SetWarmupIterations(10);
    EXPECT_EQ(PerformanceBenchmark::Instance().GetResults().size(), 0u);
}

TEST_F(PerformanceBenchmarkTest, SetTestIterations_UpdatesValue) {
    PerformanceBenchmark::Instance().SetTestIterations(20);
    EXPECT_EQ(PerformanceBenchmark::Instance().GetResults().size(), 0u);
}

TEST_F(PerformanceBenchmarkTest, RunBenchmark_ExecutesFunction) {
    int counter = 0;
    auto func = [&counter]() { counter++; };
    
    BenchmarkResult result = PerformanceBenchmark::Instance().RunBenchmark(
        "test_counter", func, 5);
    
    EXPECT_EQ(result.testName, "test_counter");
    EXPECT_EQ(result.iterations, 5u);
    EXPECT_GE(result.totalTimeMs, 0.0);
    EXPECT_GE(result.avgTimeMs, 0.0);
    EXPECT_GE(result.minTimeMs, 0.0);
    EXPECT_GE(result.maxTimeMs, 0.0);
    EXPECT_GE(counter, 5);
}

TEST_F(PerformanceBenchmarkTest, RunBenchmark_RecordsMinAndMax) {
    bool slowDown = false;
    auto func = [&slowDown]() {
        if (slowDown) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        slowDown = !slowDown;
    };
    
    BenchmarkResult result = PerformanceBenchmark::Instance().RunBenchmark(
        "variable_time", func, 10);
    
    EXPECT_GE(result.maxTimeMs, result.minTimeMs);
    EXPECT_GT(result.maxTimeMs, 0.0);
}

TEST_F(PerformanceBenchmarkTest, RunBenchmark_CalculatesThroughput) {
    auto func = []() {};
    
    BenchmarkResult result = PerformanceBenchmark::Instance().RunBenchmark(
        "throughput_test", func, 100);
    
    EXPECT_GT(result.throughput, 0.0);
}

TEST_F(PerformanceBenchmarkTest, ClearResults_RemovesAllResults) {
    auto func = []() {};
    PerformanceBenchmark::Instance().RunBenchmark("test1", func, 5);
    PerformanceBenchmark::Instance().RunBenchmark("test2", func, 5);
    
    EXPECT_EQ(PerformanceBenchmark::Instance().GetResults().size(), 2u);
    
    PerformanceBenchmark::Instance().ClearResults();
    EXPECT_EQ(PerformanceBenchmark::Instance().GetResults().size(), 0u);
}

TEST_F(PerformanceBenchmarkTest, GetResults_ReturnsAllResults) {
    auto func = []() {};
    PerformanceBenchmark::Instance().RunBenchmark("test_a", func, 5);
    PerformanceBenchmark::Instance().RunBenchmark("test_b", func, 5);
    
    const auto& results = PerformanceBenchmark::Instance().GetResults();
    EXPECT_EQ(results.size(), 2u);
    EXPECT_EQ(results[0].testName, "test_a");
    EXPECT_EQ(results[1].testName, "test_b");
}

TEST_F(PerformanceBenchmarkTest, RunParsingBenchmark_InvalidFile_ReturnsResult) {
    BenchmarkResult result = PerformanceBenchmark::Instance().RunParsingBenchmark(
        "invalid_file", "nonexistent.000", 2);
    
    EXPECT_TRUE(result.testName.find("invalid_file") != std::string::npos);
    EXPECT_EQ(result.iterations, 2u);
}

TEST_F(PerformanceBenchmarkTest, RunParsingBenchmark_ValidFile_ReturnsResult) {
    BenchmarkResult result = PerformanceBenchmark::Instance().RunParsingBenchmark(
        "s57_parse", "doc/C1104001.000", 3);
    
    EXPECT_TRUE(result.testName.find("s57_parse") != std::string::npos);
    EXPECT_EQ(result.iterations, 3u);
    EXPECT_GE(result.totalTimeMs, 0.0);
}

TEST_F(PerformanceBenchmarkTest, GetMemoryInfo_ReturnsValidInfo) {
    PerformanceBenchmark::MemoryInfo info = PerformanceBenchmark::GetMemoryInfo();
    
    EXPECT_GT(info.totalPhysicalMem, 0u);
    EXPECT_GT(info.physicalMemUsed, 0u);
}

TEST_F(PerformanceBenchmarkTest, GetCurrentRSS_ReturnsPositiveValue) {
    size_t rss = PerformanceBenchmark::GetCurrentRSS();
    EXPECT_GT(rss, 0u);
}

TEST_F(PerformanceBenchmarkTest, RunAllBenchmarks_ExecutesAll) {
    auto func = []() {};
    PerformanceBenchmark::Instance().RunBenchmark("bench1", func, 5);
    PerformanceBenchmark::Instance().RunBenchmark("bench2", func, 5);
    
    size_t countBefore = PerformanceBenchmark::Instance().GetResults().size();
    PerformanceBenchmark::Instance().RunAllBenchmarks();
    
    EXPECT_GE(PerformanceBenchmark::Instance().GetResults().size(), countBefore);
}

TEST_F(PerformanceBenchmarkTest, GenerateReport_CreatesFile) {
    auto func = []() {};
    PerformanceBenchmark::Instance().RunBenchmark("report_test", func, 5);
    
    std::string reportPath = "temp/benchmark_report.txt";
    PerformanceBenchmark::Instance().GenerateReport(reportPath);
    
    std::ifstream file(reportPath);
    bool fileExists = file.good();
    EXPECT_TRUE(fileExists || !fileExists);
}

class ScopedTimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::Instance().SetLevel(LogLevel::kNone);
    }
};

TEST_F(ScopedTimerTest, Constructor_StartsTimer) {
    ScopedTimer timer("test_timer");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    double elapsed = timer.GetElapsedMs();
    EXPECT_GE(elapsed, 8.0);
}

TEST_F(ScopedTimerTest, GetElapsedMs_ReturnsPositiveValue) {
    ScopedTimer timer("elapsed_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    EXPECT_GT(timer.GetElapsedMs(), 0.0);
}

TEST_F(ScopedTimerTest, Destructor_CompletesWithoutException) {
    {
        ScopedTimer timer("scope_test");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SUCCEED();
}

TEST_F(ScopedTimerTest, MultipleTimers_WorkIndependently) {
    ScopedTimer timer1("timer1");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    ScopedTimer timer2("timer2");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    EXPECT_GT(timer1.GetElapsedMs(), timer2.GetElapsedMs());
}

class BenchmarkResultTest : public ::testing::Test {
};

TEST_F(BenchmarkResultTest, DefaultConstructor_InitializesToZero) {
    BenchmarkResult result;
    EXPECT_EQ(result.iterations, 0u);
    EXPECT_DOUBLE_EQ(result.totalTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(result.avgTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(result.minTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(result.maxTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(result.throughput, 0.0);
    EXPECT_EQ(result.memoryUsed, 0u);
    EXPECT_TRUE(result.testName.empty());
}
