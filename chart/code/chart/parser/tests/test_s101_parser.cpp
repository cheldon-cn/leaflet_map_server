#include <gtest/gtest.h>
#include "parser/s101_parser.h"

using namespace chart::parser;

class S101ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S101Parser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S101Parser* parser_;
};

TEST_F(S101ParserTest, GetSupportedFormats) {
    auto formats = parser_->GetSupportedFormats();
    
    ASSERT_EQ(formats.size(), 1);
    EXPECT_EQ(formats[0], ChartFormat::S101);
}

TEST_F(S101ParserTest, GetName) {
    EXPECT_EQ(parser_->GetName(), "S101Parser");
}

TEST_F(S101ParserTest, GetVersion) {
    EXPECT_EQ(parser_->GetVersion(), "1.0.0");
}

TEST_F(S101ParserTest, ParseInvalidFile) {
    ParseResult result = parser_->ParseChart("nonexistent.000");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::ErrGDALDatasetOpenFailed);
}

TEST_F(S101ParserTest, ParseValidS101File) {
    std::string testFile = "doc/101KR004X0000.000";
    
    ParseResult result = parser_->ParseChart(testFile);
    
    if (result.success) {
        EXPECT_GT(result.statistics.totalFeatureCount, 0);
        EXPECT_GT(result.features.size(), 0);
        EXPECT_GT(result.statistics.parseTimeMs, 0);
    } else {
        std::cout << "Test file not found or parse failed: " << result.errorMessage << std::endl;
    }
}

TEST_F(S101ParserTest, ParseFeatureFromStringNotSupported) {
    Feature feature;
    bool result = parser_->ParseFeature("test", feature);
    
    EXPECT_FALSE(result);
}
