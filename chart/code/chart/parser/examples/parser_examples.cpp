#include <chart_parser/chart_parser.h>
#include <chart_parser/s57_parser.h>
#include <chart_parser/s102_parser.h>
#include <chart_parser/parse_cache.h>
#include <chart_parser/incremental_parser.h>
#include <chart_parser/performance_benchmark.h>
#include <iostream>
#include <iomanip>

using namespace chart::parser;

void PrintFeature(const Feature& feature) {
    std::cout << "  Feature ID: " << feature.id << std::endl;
    std::cout << "    Class: " << feature.className << std::endl;
    std::cout << "    Type: " << FeatureTypeToString(feature.type) << std::endl;
    
    std::cout << "    Geometry: " << GeometryTypeToString(feature.geometry.type);
    if (!feature.geometry.points.empty()) {
        std::cout << " (" << feature.geometry.points.size() << " points)";
    }
    std::cout << std::endl;
    
    if (!feature.attributes.empty()) {
        std::cout << "    Attributes:" << std::endl;
        for (const auto& attr : feature.attributes) {
            std::cout << "      " << attr.first << ": ";
            switch (attr.second.type) {
                case AttributeValue::Type::Integer:
                    std::cout << attr.second.intValue;
                    break;
                case AttributeValue::Type::Double:
                    std::cout << std::fixed << std::setprecision(2) << attr.second.doubleValue;
                    break;
                case AttributeValue::Type::String:
                    std::cout << attr.second.stringValue;
                    break;
                default:
                    std::cout << "(unknown)";
            }
            std::cout << std::endl;
        }
    }
}

void ExampleBasicParsing() {
    std::cout << "\n=== Basic S57 Parsing Example ===" << std::endl;
    
    S57Parser parser;
    ParseConfig config;
    config.validateGeometry = true;
    config.includeMetadata = true;
    
    ParseResult result = parser.ParseChart("doc/C1104001.000", config);
    
    if (result.success) {
        std::cout << "Parse successful!" << std::endl;
        std::cout << "Total features: " << result.statistics.totalFeatureCount << std::endl;
        std::cout << "Parse time: " << result.statistics.parseTimeMs << " ms" << std::endl;
        
        int count = 0;
        for (const auto& feature : result.features) {
            if (count++ < 5) {
                PrintFeature(feature);
            }
        }
        if (result.features.size() > 5) {
            std::cout << "  ... and " << (result.features.size() - 5) << " more features" << std::endl;
        }
    } else {
        std::cout << "Parse failed: " << result.errorMessage << std::endl;
    }
}

void ExampleCaching() {
    std::cout << "\n=== Caching Example ===" << std::endl;
    
    ParseCache& cache = ParseCache::Instance();
    cache.Enable(true);
    cache.SetMaxSize(50);
    cache.SetMaxAge(std::chrono::seconds(300));
    
    S57Parser parser;
    
    std::cout << "First parse (will be cached)..." << std::endl;
    ParseResult result1 = parser.ParseChart("doc/C1104001.000");
    std::cout << "Parse time: " << result1.statistics.parseTimeMs << " ms" << std::endl;
    
    cache.Put("doc/C1104001.000", result1);
    
    std::cout << "Second parse (from cache)..." << std::endl;
    ParseResult result2;
    if (cache.Get("doc/C1104001.000", result2)) {
        std::cout << "Cache hit!" << std::endl;
    } else {
        std::cout << "Cache miss!" << std::endl;
    }
    
    auto stats = cache.GetStatistics();
    std::cout << "Cache statistics:" << std::endl;
    std::cout << "  Hits: " << stats.hitCount << std::endl;
    std::cout << "  Misses: " << stats.missCount << std::endl;
    std::cout << "  Hit rate: " << std::fixed << std::setprecision(1) 
              << (stats.hitRate * 100) << "%" << std::endl;
}

void ExampleIncrementalParsing() {
    std::cout << "\n=== Incremental Parsing Example ===" << std::endl;
    
    IncrementalParser& incParser = IncrementalParser::Instance();
    incParser.SetParser([](const std::string& path, const ParseConfig& cfg) {
        S57Parser parser;
        return parser.ParseChart(path, cfg);
    });
    
    std::cout << "First incremental parse..." << std::endl;
    IncrementalParseResult result1 = incParser.ParseIncremental("doc/C1104001.000");
    std::cout << "  Added: " << result1.addedFeatureIds.size() << std::endl;
    std::cout << "  Modified: " << result1.modifiedFeatureIds.size() << std::endl;
    std::cout << "  Deleted: " << result1.deletedFeatureIds.size() << std::endl;
    
    std::cout << "Second incremental parse (no changes)..." << std::endl;
    IncrementalParseResult result2 = incParser.ParseIncremental("doc/C1104001.000");
    std::cout << "  Has changes: " << (result2.hasChanges ? "yes" : "no") << std::endl;
    std::cout << "  Unchanged: " << result2.unchangedFeatures.features.size() << std::endl;
}

void ExampleBenchmarking() {
    std::cout << "\n=== Performance Benchmark Example ===" << std::endl;
    
    PerformanceBenchmark& bench = PerformanceBenchmark::Instance();
    bench.SetWarmupIterations(2);
    bench.SetTestIterations(5);
    
    bench.RunBenchmark("FeatureCreation", []() {
        Feature feature;
        feature.id = "test";
        feature.type = FeatureType::SOUNDG;
        feature.geometry.type = GeometryType::Point;
        for (int i = 0; i < 100; ++i) {
            feature.geometry.points.push_back(Point(i * 0.1, i * 0.2, 0));
        }
    }, 10);
    
    bench.RunBenchmark("AttributeMapCreation", []() {
        AttributeMap attrs;
        for (int i = 0; i < 50; ++i) {
            AttributeValue val;
            val.type = AttributeValue::Type::String;
            val.stringValue = "value_" + std::to_string(i);
            attrs["key_" + std::to_string(i)] = val;
        }
    }, 10);
    
    std::cout << "\nBenchmark Results:" << std::endl;
    for (const auto& result : bench.GetResults()) {
        std::cout << "  " << result.testName << ":" << std::endl;
        std::cout << "    Avg: " << std::fixed << std::setprecision(3) 
                  << result.avgTimeMs << " ms" << std::endl;
        std::cout << "    Min: " << result.minTimeMs << " ms" << std::endl;
        std::cout << "    Max: " << result.maxTimeMs << " ms" << std::endl;
        std::cout << "    Throughput: " << std::setprecision(1) 
                  << result.throughput << " /s" << std::endl;
    }
}

void ExampleS102Parsing() {
    std::cout << "\n=== S102 Bathymetry Parsing Example ===" << std::endl;
    
    S102Parser parser;
    
    std::cout << "S102 Parser supports formats: ";
    for (const auto& format : parser.GetSupportedFormats()) {
        std::cout << ChartFormatToString(format) << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Parser name: " << parser.GetName() << std::endl;
    std::cout << "Parser version: " << parser.GetVersion() << std::endl;
    
    std::cout << "\nNote: S102 parsing requires HDF5 bathymetry data files." << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  BathymetryGrid grid;" << std::endl;
    std::cout << "  if (parser.ParseBathymetryGrid(\"data.h5\", grid)) {" << std::endl;
    std::cout << "      auto stats = parser.CalculateStatistics(grid);" << std::endl;
    std::cout << "      // Use depth data..." << std::endl;
    std::cout << "  }" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "Chart Parser Library Examples" << std::endl;
    std::cout << "==============================" << std::endl;
    
    if (!ChartParser::Instance().Initialize()) {
        std::cerr << "Failed to initialize chart parser" << std::endl;
        return 1;
    }
    
    ExampleBasicParsing();
    ExampleCaching();
    ExampleIncrementalParsing();
    ExampleBenchmarking();
    ExampleS102Parsing();
    
    ChartParser::Instance().Shutdown();
    
    std::cout << "\n=== Examples Complete ===" << std::endl;
    
    return 0;
}
