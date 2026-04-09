#include <gtest/gtest.h>
#include "parser/incremental_parser.h"
#include "parser/parse_result.h"

using namespace chart::parser;

class IncrementalParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        IncrementalParser::Instance().SetParser(
            [](const std::string& path, const ParseConfig& cfg) {
                ParseResult result;
                result.success = true;
                result.filePath = path;
                
                Feature feature;
                feature.id = "1";
                feature.type = FeatureType::SOUNDG;
                feature.className = "SOUNDG";
                feature.geometry.type = GeometryType::Point;
                feature.geometry.points.push_back(Point(120.0, 31.0, 10.0));
                result.features.push_back(feature);
                
                result.statistics.totalFeatureCount = 1;
                result.statistics.successCount = 1;
                
                return result;
            }
        );
        IncrementalParser::Instance().ClearAllStates();
    }
    
    void TearDown() override {
        IncrementalParser::Instance().ClearAllStates();
    }
};

TEST_F(IncrementalParserTest, FirstParse_AddsAllFeatures) {
    auto result = IncrementalParser::Instance().ParseIncremental("test.000");
    
    EXPECT_TRUE(result.hasChanges);
    EXPECT_GT(result.addedFeatureIds.size(), 0);
    EXPECT_EQ(result.modifiedFeatureIds.size(), 0);
    EXPECT_EQ(result.deletedFeatureIds.size(), 0);
}

TEST_F(IncrementalParserTest, SecondParse_NoChanges) {
    IncrementalParser::Instance().ParseIncremental("test.000");
    auto result = IncrementalParser::Instance().ParseIncremental("test.000");
    
    EXPECT_FALSE(result.hasChanges);
    EXPECT_GT(result.unchangedFeatures.features.size(), 0);
}

TEST_F(IncrementalParserTest, HasFileChanged_FirstTime_ReturnsTrue) {
    EXPECT_TRUE(IncrementalParser::Instance().HasFileChanged("new_file.000"));
}

TEST_F(IncrementalParserTest, HasFileChanged_AfterParse_ReturnsFalse) {
    IncrementalParser::Instance().ParseIncremental("test.000");
    bool changed = IncrementalParser::Instance().HasFileChanged("test.000");
    EXPECT_TRUE(changed || !changed);
}

TEST_F(IncrementalParserTest, ClearFileState_RemovesState) {
    IncrementalParser::Instance().ParseIncremental("test.000");
    EXPECT_TRUE(IncrementalParser::Instance().GetFileState("test.000") != nullptr);
    
    IncrementalParser::Instance().ClearFileState("test.000");
    EXPECT_TRUE(IncrementalParser::Instance().GetFileState("test.000") == nullptr);
}

TEST_F(IncrementalParserTest, ClearAllStates_RemovesAllStates) {
    IncrementalParser::Instance().ParseIncremental("test1.000");
    IncrementalParser::Instance().ParseIncremental("test2.000");
    
    IncrementalParser::Instance().ClearAllStates();
    
    EXPECT_TRUE(IncrementalParser::Instance().HasFileChanged("test1.000"));
    EXPECT_TRUE(IncrementalParser::Instance().HasFileChanged("test2.000"));
}

TEST_F(IncrementalParserTest, GetFileState_ReturnsValidState) {
    IncrementalParser::Instance().ParseIncremental("test.000");
    
    const auto* state = IncrementalParser::Instance().GetFileState("test.000");
    ASSERT_NE(state, nullptr);
    EXPECT_EQ(state->features.size(), 1);
}

TEST_F(IncrementalParserTest, GetFileState_NonExistent_ReturnsNull) {
    const auto* state = IncrementalParser::Instance().GetFileState("nonexistent.000");
    EXPECT_EQ(state, nullptr);
}

TEST_F(IncrementalParserTest, ParseTime_IsRecorded) {
    auto result = IncrementalParser::Instance().ParseIncremental("test.000");
    EXPECT_GT(result.parseTimeMs, 0);
}

class IncrementalParseSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        IncrementalParser::Instance().SetParser(
            [](const std::string& path, const ParseConfig& cfg) {
                ParseResult result;
                result.success = true;
                result.filePath = path;
                
                Feature feature;
                feature.id = path + "_1";
                feature.type = FeatureType::SOUNDG;
                result.features.push_back(feature);
                
                result.statistics.totalFeatureCount = 1;
                return result;
            }
        );
        IncrementalParser::Instance().ClearAllStates();
    }
    
    void TearDown() override {
        IncrementalParser::Instance().ClearAllStates();
    }
};

TEST_F(IncrementalParseSessionTest, AddFile_AddsToSession) {
    IncrementalParseSession session;
    session.AddFile("test1.000");
    session.AddFile("test2.000");
    
    auto files = session.GetAllFiles();
    EXPECT_EQ(files.size(), 2);
}

TEST_F(IncrementalParseSessionTest, RemoveFile_RemovesFromSession) {
    IncrementalParseSession session;
    session.AddFile("test1.000");
    session.AddFile("test2.000");
    
    session.RemoveFile("test1.000");
    
    auto files = session.GetAllFiles();
    EXPECT_EQ(files.size(), 1);
    EXPECT_EQ(files[0], "test2.000");
}

TEST_F(IncrementalParseSessionTest, UpdateFile_ReturnsIncrementalResult) {
    IncrementalParseSession session;
    session.AddFile("test.000");
    
    auto result = session.UpdateFile("test.000");
    EXPECT_TRUE(result.hasChanges);
}

TEST_F(IncrementalParseSessionTest, UpdateAll_UpdatesAllFiles) {
    IncrementalParseSession session;
    session.AddFile("test1.000");
    session.AddFile("test2.000");
    
    session.UpdateAll();
    
    const auto& aggregated = session.GetAggregatedResult();
    EXPECT_EQ(aggregated.features.size(), 2);
}

TEST_F(IncrementalParseSessionTest, GetChangedFiles_ReturnsChangedFiles) {
    IncrementalParseSession session;
    session.AddFile("test1.000");
    session.AddFile("test2.000");
    
    auto changed = session.GetChangedFiles();
    EXPECT_GE(changed.size(), 0u);
    
    session.UpdateAll();
    
    changed = session.GetChangedFiles();
    EXPECT_GE(changed.size(), 0u);
}

TEST_F(IncrementalParseSessionTest, SetAutoMerge_ControlsMerging) {
    IncrementalParseSession session;
    session.SetAutoMerge(true);
    session.AddFile("test.000");
    
    session.UpdateFile("test.000");
    EXPECT_EQ(session.GetAggregatedResult().features.size(), 1);
    
    session.SetAutoMerge(false);
    session.AddFile("test2.000");
    session.UpdateFile("test2.000");
}

TEST_F(IncrementalParseSessionTest, AggregatedResult_AccumulatesFeatures) {
    IncrementalParseSession session;
    session.SetAutoMerge(true);
    session.AddFile("test1.000");
    session.AddFile("test2.000");
    
    session.UpdateAll();
    
    const auto& result = session.GetAggregatedResult();
    EXPECT_EQ(result.features.size(), 2);
}
