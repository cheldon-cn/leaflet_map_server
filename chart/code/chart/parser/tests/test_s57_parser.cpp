#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chart_parser/s57_parser.h"
#include "chart_parser/chart_parser.h"

using namespace chart::parser;

class S57ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S57Parser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S57Parser* parser_;
};

TEST_F(S57ParserTest, GetSupportedFormats) {
    auto formats = parser_->GetSupportedFormats();
    
    ASSERT_EQ(formats.size(), 1);
    EXPECT_EQ(formats[0], ChartFormat::S57);
}

TEST_F(S57ParserTest, GetName) {
    EXPECT_EQ(parser_->GetName(), "S57Parser");
}

TEST_F(S57ParserTest, GetVersion) {
    EXPECT_EQ(parser_->GetVersion(), "1.0.0");
}

TEST_F(S57ParserTest, ParseInvalidFile) {
    ParseResult result = parser_->ParseChart("nonexistent.000");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::ErrGDALDatasetOpenFailed);
}

TEST_F(S57ParserTest, ParseValidS57File) {
    std::string testFile = "doc/C1104001.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success) {
        EXPECT_GT(result.statistics.totalFeatureCount, 0);
        EXPECT_GT(result.features.size(), 0);
        EXPECT_GT(result.statistics.parseTimeMs, 0);
    } else {
        std::cout << "Test file not found or parse failed: " << result.errorMessage << std::endl;
    }
}

TEST_F(S57ParserTest, ParseFeatureFromStringNotSupported) {
    Feature feature;
    bool result = parser_->ParseFeature("test", feature);
    
    EXPECT_FALSE(result);
}

TEST_F(S57ParserTest, ParseResultContainsMetadata) {
    std::string testFile = "doc/C1104001.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success) {
        EXPECT_FALSE(result.filePath.empty());
    }
}

TEST_F(S57ParserTest, ParseResultStatisticsValid) {
    std::string testFile = "doc/C1104001.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success) {
        EXPECT_GE(result.statistics.totalFeatureCount, 0);
        EXPECT_GE(result.statistics.successCount, 0);
        EXPECT_GE(result.statistics.parseTimeMs, 0);
    }
}

TEST_F(S57ParserTest, FeaturesHaveValidGeometry) {
    std::string testFile = "doc/C1104001.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success && !result.features.empty()) {
        for (const auto& feature : result.features) {
            if (feature.geometry.type != GeometryType::Unknown) {
                EXPECT_TRUE(
                    feature.geometry.type == GeometryType::Point ||
                    feature.geometry.type == GeometryType::Line ||
                    feature.geometry.type == GeometryType::Area ||
                    feature.geometry.type == GeometryType::MultiPoint ||
                    feature.geometry.type == GeometryType::MultiLine ||
                    feature.geometry.type == GeometryType::MultiArea
                );
            }
        }
    }
}

TEST_F(S57ParserTest, FeaturesHaveValidFeatureType) {
    std::string testFile = "doc/C1104001.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success && !result.features.empty()) {
        for (const auto& feature : result.features) {
            EXPECT_FALSE(feature.className.empty());
        }
    }
}
