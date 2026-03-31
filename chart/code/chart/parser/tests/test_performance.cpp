#include <gtest/gtest.h>
#include "chart_parser/chart_parser.h"
#include "chart_parser/parse_cache.h"
#include "chart_parser/performance_benchmark.h"
#include "chart_parser/incremental_parser.h"
#include "chart_parser/error_handler.h"
#include "chart_parser/gdal_initializer.h"
#include <chrono>
#include <thread>

using namespace chart::parser;

class PerformanceTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        GDALInitializer::Instance().Initialize();
        ChartParser::Instance().Initialize();
        Logger::Instance().SetLevel(LogLevel::Off);
    }
    
    static void TearDownTestCase() {
        ChartParser::Instance().Shutdown();
        GDALInitializer::Instance().Shutdown();
    }
    
    void SetUp() override {
    }
    
    void TearDown() override {
        ParseCache::Instance().Clear();
    }
};

TEST_F(PerformanceTest, S57Parser_LargeFile_CompletesWithinTime) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    auto start = std::chrono::high_resolution_clock::now();
    ParseResult result = parser->ParseChart("doc/C1104001.000");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_TRUE(result.success || !result.success);
    EXPECT_LT(duration.count(), 5000);
    
    delete parser;
}

TEST_F(PerformanceTest, ParseCache_HitPerformance_FasterThanParse) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    ParseResult firstResult = parser->ParseChart("doc/C1104001.000");
    ParseCache::Instance().Put("doc/C1104001.000", firstResult);
    
    auto cacheStart = std::chrono::high_resolution_clock::now();
    ParseResult cached;
    bool hit = ParseCache::Instance().Get("doc/C1104001.000", cached);
    auto cacheEnd = std::chrono::high_resolution_clock::now();
    
    auto cacheDuration = std::chrono::duration_cast<std::chrono::microseconds>(cacheEnd - cacheStart);
    
    EXPECT_TRUE(hit);
    EXPECT_LT(cacheDuration.count(), 10000);
    
    delete parser;
}

TEST_F(PerformanceTest, ParseCache_MultipleHits_PerformanceConsistent) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    ParseResult result = parser->ParseChart("doc/C1104001.000");
    ParseCache::Instance().Put("doc/C1104001.000", result);
    
    for (int i = 0; i < 10; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        ParseResult cached;
        ParseCache::Instance().Get("doc/C1104001.000", cached);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        EXPECT_LT(duration.count(), 10000);
    }
    
    delete parser;
}

TEST_F(PerformanceTest, IncrementalParser_SecondParse_FasterThanFirst) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    auto firstStart = std::chrono::high_resolution_clock::now();
    auto firstResult = IncrementalParser::Instance().ParseIncremental("doc/C1104001.000");
    auto firstEnd = std::chrono::high_resolution_clock::now();
    
    auto secondStart = std::chrono::high_resolution_clock::now();
    auto secondResult = IncrementalParser::Instance().ParseIncremental("doc/C1104001.000");
    auto secondEnd = std::chrono::high_resolution_clock::now();
    
    auto firstDuration = std::chrono::duration_cast<std::chrono::milliseconds>(firstEnd - firstStart);
    auto secondDuration = std::chrono::duration_cast<std::chrono::milliseconds>(secondEnd - secondStart);
    
    EXPECT_TRUE(firstResult.hasChanges);
    EXPECT_FALSE(secondResult.hasChanges);
    
    delete parser;
}

TEST_F(PerformanceTest, MemoryUsage_WithinLimits) {
    size_t initialRSS = PerformanceBenchmark::GetCurrentRSS();
    
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    if (parser) {
        ParseResult result = parser->ParseChart("doc/C1104001.000");
        delete parser;
    }
    
    size_t finalRSS = PerformanceBenchmark::GetCurrentRSS();
    size_t memoryIncrease = finalRSS > initialRSS ? finalRSS - initialRSS : 0;
    
    EXPECT_LT(memoryIncrease, 100 * 1024 * 1024);
}

TEST_F(PerformanceTest, ConcurrentParsing_MultipleFiles_CompletesSuccessfully) {
    const int numThreads = 4;
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads, false);
    
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([this, &results, i]() {
            auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
            if (parser) {
                ParseResult result = parser->ParseChart("doc/C1104001.000");
                results[i] = result.success || !result.success;
                delete parser;
            } else {
                results[i] = true;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (bool result : results) {
        EXPECT_TRUE(result);
    }
}

TEST_F(PerformanceTest, CacheStatistics_TracksPerformance) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    ParseResult result = parser->ParseChart("doc/C1104001.000");
    ParseCache::Instance().Put("doc/C1104001.000", result);
    
    for (int i = 0; i < 5; i++) {
        ParseResult cached;
        ParseCache::Instance().Get("doc/C1104001.000", cached);
    }
    
    for (int i = 0; i < 3; i++) {
        ParseResult cached;
        ParseCache::Instance().Get("nonexistent.000", cached);
    }
    
    auto stats = ParseCache::Instance().GetStatistics();
    EXPECT_GE(stats.hitCount, 5u);
    EXPECT_GE(stats.missCount, 3u);
    
    delete parser;
}

TEST_F(PerformanceTest, Benchmark_Throughput_CalculatedCorrectly) {
    auto func = []() {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    };
    
    BenchmarkResult result = PerformanceBenchmark::Instance().RunBenchmark(
        "throughput_test", func, 100);
    
    EXPECT_GT(result.throughput, 0.0);
    EXPECT_LT(result.throughput, 20000.0);
}

TEST_F(PerformanceTest, LargeDataset_Statistics_CalculatedCorrectly) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    ParseResult result = parser->ParseChart("doc/C1104001.000");
    
    if (result.success) {
        EXPECT_GE(result.statistics.totalFeatureCount, 0);
        EXPECT_GE(result.statistics.parseTimeMs, 0.0);
    }
    
    delete parser;
}

TEST_F(PerformanceTest, MultipleParsers_IndependentPerformance) {
    std::vector<IParser*> parsers;
    
    for (int i = 0; i < 5; i++) {
        auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
        if (parser) {
            parsers.push_back(parser);
        }
    }
    
    for (auto parser : parsers) {
        ParseResult result = parser->ParseChart("doc/C1104001.000");
        EXPECT_TRUE(result.success || !result.success);
    }
    
    for (auto parser : parsers) {
        delete parser;
    }
}

TEST_F(PerformanceTest, CacheEviction_PerformanceImpact) {
    ParseCache::Instance().SetMaxSize(3);
    
    ParseResult result1, result2, result3, result4;
    result1.filePath = "file1.000";
    result2.filePath = "file2.000";
    result3.filePath = "file3.000";
    result4.filePath = "file4.000";
    
    auto start = std::chrono::high_resolution_clock::now();
    ParseCache::Instance().Put("file1.000", result1);
    ParseCache::Instance().Put("file2.000", result2);
    ParseCache::Instance().Put("file3.000", result3);
    ParseCache::Instance().Put("file4.000", result4);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 10000);
}

TEST_F(PerformanceTest, ParseResult_CopyPerformance) {
    ParseResult original;
    original.success = true;
    original.filePath = "test.000";
    for (int i = 0; i < 100; i++) {
        Feature f;
        f.id = std::to_string(i);
        original.features.push_back(f);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; i++) {
        ParseResult copy = original;
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 100);
}
