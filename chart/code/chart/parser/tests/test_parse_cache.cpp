#include <gtest/gtest.h>
#include "parser/parse_cache.h"
#include "parser/parse_result.h"

using namespace chart::parser;

class ParseCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        ParseCache::Instance().Enable(true);
        ParseCache::Instance().Clear();
        ParseCache::Instance().ResetStatistics();
    }
    
    void TearDown() override {
        ParseCache::Instance().Clear();
    }
    
    ParseResult CreateTestResult(const std::string& filePath) {
        ParseResult result;
        result.success = true;
        result.filePath = filePath;
        
        Feature feature;
        feature.id = "1";
        feature.type = FeatureType::SOUNDG;
        feature.geometry.type = GeometryType::Point;
        feature.geometry.points.push_back(Point(120.0, 31.0, 10.0));
        result.features.push_back(feature);
        
        result.statistics.totalFeatureCount = 1;
        result.statistics.successCount = 1;
        result.statistics.parseTimeMs = 100.0;
        
        return result;
    }
};

TEST_F(ParseCacheTest, PutAndGet_CacheHit) {
    ParseResult result = CreateTestResult("test.000");
    
    ParseCache::Instance().Put("test.000", result);
    
    ParseResult cached;
    EXPECT_TRUE(ParseCache::Instance().Get("test.000", cached));
    EXPECT_TRUE(cached.success);
    EXPECT_EQ(cached.filePath, "test.000");
    EXPECT_EQ(cached.features.size(), 1);
}

TEST_F(ParseCacheTest, Get_CacheMiss) {
    ParseResult result;
    EXPECT_FALSE(ParseCache::Instance().Get("nonexistent.000", result));
}

TEST_F(ParseCacheTest, Clear_RemovesAllEntries) {
    ParseResult result = CreateTestResult("test1.000");
    ParseCache::Instance().Put("test1.000", result);
    
    ParseCache::Instance().Clear();
    
    ParseResult cached;
    EXPECT_FALSE(ParseCache::Instance().Get("test1.000", cached));
    EXPECT_EQ(ParseCache::Instance().GetSize(), 0);
}

TEST_F(ParseCacheTest, Remove_RemovesSpecificEntry) {
    ParseResult result1 = CreateTestResult("test1.000");
    ParseResult result2 = CreateTestResult("test2.000");
    
    ParseCache::Instance().Put("test1.000", result1);
    ParseCache::Instance().Put("test2.000", result2);
    
    ParseCache::Instance().Remove("test1.000");
    
    ParseResult cached;
    EXPECT_FALSE(ParseCache::Instance().Get("test1.000", cached));
    EXPECT_TRUE(ParseCache::Instance().Get("test2.000", cached));
}

TEST_F(ParseCacheTest, HasEntry_ReturnsCorrectStatus) {
    ParseResult result = CreateTestResult("test.000");
    
    EXPECT_FALSE(ParseCache::Instance().HasEntry("test.000"));
    
    ParseCache::Instance().Put("test.000", result);
    
    EXPECT_TRUE(ParseCache::Instance().HasEntry("test.000"));
    EXPECT_FALSE(ParseCache::Instance().HasEntry("nonexistent.000"));
}

TEST_F(ParseCacheTest, GetSize_ReturnsCorrectCount) {
    EXPECT_EQ(ParseCache::Instance().GetSize(), 0);
    
    ParseResult result1 = CreateTestResult("test1.000");
    ParseResult result2 = CreateTestResult("test2.000");
    
    ParseCache::Instance().Put("test1.000", result1);
    EXPECT_EQ(ParseCache::Instance().GetSize(), 1);
    
    ParseCache::Instance().Put("test2.000", result2);
    EXPECT_EQ(ParseCache::Instance().GetSize(), 2);
}

TEST_F(ParseCacheTest, Statistics_TracksHitsAndMisses) {
    ParseResult result = CreateTestResult("test.000");
    ParseCache::Instance().Put("test.000", result);
    
    ParseResult cached;
    bool hit = ParseCache::Instance().Get("test.000", cached);
    EXPECT_TRUE(hit);
    
    bool miss = ParseCache::Instance().Get("nonexistent.000", cached);
    EXPECT_FALSE(miss);
    
    auto stats = ParseCache::Instance().GetStatistics();
    EXPECT_GT(stats.hitCount, 0u);
    EXPECT_GT(stats.missCount, 0u);
}

TEST_F(ParseCacheTest, Enable_DisablesCache) {
    ParseCache::Instance().Enable(false);
    EXPECT_FALSE(ParseCache::Instance().IsEnabled());
    
    ParseResult result = CreateTestResult("test.000");
    ParseCache::Instance().Put("test.000", result);
    
    ParseResult cached;
    EXPECT_FALSE(ParseCache::Instance().Get("test.000", cached));
    
    ParseCache::Instance().Enable(true);
    EXPECT_TRUE(ParseCache::Instance().IsEnabled());
}

TEST_F(ParseCacheTest, SetMaxSize_EvictsOldEntries) {
    ParseCache::Instance().SetMaxSize(2);
    
    ParseResult result1 = CreateTestResult("test1.000");
    ParseResult result2 = CreateTestResult("test2.000");
    ParseResult result3 = CreateTestResult("test3.000");
    
    ParseCache::Instance().Put("test1.000", result1);
    ParseCache::Instance().Put("test2.000", result2);
    ParseCache::Instance().Put("test3.000", result3);
    
    EXPECT_EQ(ParseCache::Instance().GetSize(), 2);
}

TEST_F(ParseCacheTest, SaveAndLoadToFile_PersistsCache) {
    ParseResult result = CreateTestResult("test.000");
    ParseCache::Instance().Put("test.000", result);
    
    std::string cacheFile = "test_cache.bin";
    EXPECT_TRUE(ParseCache::Instance().SaveToFile(cacheFile));
    
    ParseCache::Instance().Clear();
    EXPECT_FALSE(ParseCache::Instance().HasEntry("test.000"));
    
    EXPECT_TRUE(ParseCache::Instance().LoadFromFile(cacheFile));
    EXPECT_TRUE(ParseCache::Instance().HasEntry("test.000"));
    
    std::remove(cacheFile.c_str());
}

TEST_F(ParseCacheTest, ResetStatistics_ClearsAllCounters) {
    ParseResult result = CreateTestResult("test.000");
    ParseCache::Instance().Put("test.000", result);
    
    ParseResult cached;
    ParseCache::Instance().Get("test.000", cached);
    
    auto stats = ParseCache::Instance().GetStatistics();
    EXPECT_EQ(stats.hitCount, 1);
    
    ParseCache::Instance().ResetStatistics();
    
    stats = ParseCache::Instance().GetStatistics();
    EXPECT_EQ(stats.hitCount, 0);
    EXPECT_EQ(stats.missCount, 0);
    EXPECT_DOUBLE_EQ(stats.hitRate, 0.0);
}
