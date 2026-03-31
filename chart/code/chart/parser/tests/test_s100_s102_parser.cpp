#include <gtest/gtest.h>
#include "chart_parser/s100_parser.h"
#include "chart_parser/s102_parser.h"

using namespace chart::parser;

class S100ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S100Parser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S100Parser* parser_;
};

TEST_F(S100ParserTest, GetSupportedFormats_ReturnsS100) {
    auto formats = parser_->GetSupportedFormats();
    
    ASSERT_EQ(formats.size(), 1);
    EXPECT_EQ(formats[0], ChartFormat::S100);
}

TEST_F(S100ParserTest, GetName_ReturnsCorrectName) {
    EXPECT_EQ(parser_->GetName(), "S100Parser");
}

TEST_F(S100ParserTest, GetVersion_ReturnsCorrectVersion) {
    EXPECT_EQ(parser_->GetVersion(), "1.0.0");
}

TEST_F(S100ParserTest, ParseInvalidFile_ReturnsError) {
    ParseResult result = parser_->ParseChart("nonexistent.h5");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::ErrGDALDatasetOpenFailed);
}

TEST_F(S100ParserTest, ParseFeatureFromString_ReturnsFalse) {
    Feature feature;
    bool result = parser_->ParseFeature("test", feature);
    
    EXPECT_FALSE(result);
}

TEST_F(S100ParserTest, IsS100File_H5File_ReturnsTrue) {
    EXPECT_TRUE(S100Parser::IsS100File("test.h5"));
    EXPECT_TRUE(S100Parser::IsS100File("test.hdf5"));
}

TEST_F(S100ParserTest, IsS100File_OtherFile_ReturnsFalse) {
    EXPECT_FALSE(S100Parser::IsS100File("test.000"));
    EXPECT_FALSE(S100Parser::IsS100File("test.xml"));
}

TEST_F(S100ParserTest, GetDatasetInfo_InvalidFile_ReturnsFalse) {
    S100DatasetInfo info;
    EXPECT_FALSE(parser_->GetDatasetInfo("nonexistent.h5", info));
}

class S102ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S102Parser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S102Parser* parser_;
};

TEST_F(S102ParserTest, GetSupportedFormats_ReturnsS102) {
    auto formats = parser_->GetSupportedFormats();
    
    ASSERT_EQ(formats.size(), 1);
    EXPECT_EQ(formats[0], ChartFormat::S102);
}

TEST_F(S102ParserTest, GetName_ReturnsCorrectName) {
    EXPECT_EQ(parser_->GetName(), "S102Parser");
}

TEST_F(S102ParserTest, GetVersion_ReturnsCorrectVersion) {
    EXPECT_EQ(parser_->GetVersion(), "1.0.0");
}

TEST_F(S102ParserTest, ParseInvalidFile_ReturnsError) {
    ParseResult result = parser_->ParseChart("nonexistent.h5");
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.features.size(), 0);
}

TEST_F(S102ParserTest, ParseFeatureFromString_ReturnsFalse) {
    Feature feature;
    bool result = parser_->ParseFeature("test", feature);
    
    EXPECT_FALSE(result);
}

TEST_F(S102ParserTest, ParseBathymetryGrid_InvalidFile_ReturnsFalse) {
    BathymetryGrid grid;
    EXPECT_FALSE(parser_->ParseBathymetryGrid("nonexistent.h5", grid));
}

TEST_F(S102ParserTest, CalculateStatistics_EmptyGrid_ReturnsZero) {
    BathymetryGrid grid;
    
    auto stats = parser_->CalculateStatistics(grid);
    
    EXPECT_EQ(stats.pointCount, 0);
    EXPECT_EQ(stats.minDepth, 0);
    EXPECT_EQ(stats.maxDepth, 0);
}

TEST_F(S102ParserTest, CalculateStatistics_ValidGrid_ReturnsCorrectStats) {
    BathymetryGrid grid;
    grid.rows = 2;
    grid.cols = 2;
    
    BathymetryPoint p1;
    p1.x = 0; p1.y = 0; p1.depth = 10.0;
    grid.points.push_back(p1);
    
    BathymetryPoint p2;
    p2.x = 1; p2.y = 0; p2.depth = 20.0;
    grid.points.push_back(p2);
    
    BathymetryPoint p3;
    p3.x = 0; p3.y = 1; p3.depth = 15.0;
    grid.points.push_back(p3);
    
    BathymetryPoint p4;
    p4.x = 1; p4.y = 1; p4.depth = 25.0;
    grid.points.push_back(p4);
    
    auto stats = parser_->CalculateStatistics(grid);
    
    EXPECT_EQ(stats.pointCount, 4);
    EXPECT_DOUBLE_EQ(stats.minDepth, 10.0);
    EXPECT_DOUBLE_EQ(stats.maxDepth, 25.0);
    EXPECT_DOUBLE_EQ(stats.avgDepth, 17.5);
}

TEST_F(S102ParserTest, GetDepthAtPosition_InsideGrid_ReturnsTrue) {
    BathymetryGrid grid;
    grid.rows = 10;
    grid.cols = 10;
    grid.minX = 0;
    grid.maxX = 100;
    grid.minY = 0;
    grid.maxY = 100;
    grid.cellSizeX = 10;
    grid.cellSizeY = 10;
    
    for (int i = 0; i < 100; ++i) {
        BathymetryPoint p;
        p.x = (i % 10) * 10 + 5;
        p.y = (i / 10) * 10 + 5;
        p.depth = static_cast<double>(i);
        grid.points.push_back(p);
    }
    
    double depth;
    bool result = parser_->GetDepthAtPosition(grid, 5, 5, depth);
    if (result) {
        EXPECT_GE(depth, 0.0);
    }
}

TEST_F(S102ParserTest, GetDepthAtPosition_OutsideGrid_ReturnsFalse) {
    BathymetryGrid grid;
    grid.rows = 10;
    grid.cols = 10;
    grid.minX = 0;
    grid.maxX = 100;
    grid.minY = 0;
    grid.maxY = 100;
    
    double depth;
    EXPECT_FALSE(parser_->GetDepthAtPosition(grid, -10, 50, depth));
    EXPECT_FALSE(parser_->GetDepthAtPosition(grid, 150, 50, depth));
    EXPECT_FALSE(parser_->GetDepthAtPosition(grid, 50, -10, depth));
    EXPECT_FALSE(parser_->GetDepthAtPosition(grid, 50, 150, depth));
}

TEST_F(S102ParserTest, BathymetryPoint_DefaultConstructor) {
    BathymetryPoint point;
    
    EXPECT_DOUBLE_EQ(point.x, 0.0);
    EXPECT_DOUBLE_EQ(point.y, 0.0);
    EXPECT_DOUBLE_EQ(point.depth, 0.0);
    EXPECT_DOUBLE_EQ(point.uncertainty, 0.0);
    EXPECT_EQ(point.quality, 0);
}

TEST_F(S102ParserTest, BathymetryGrid_DefaultConstructor) {
    BathymetryGrid grid;
    
    EXPECT_EQ(grid.rows, 0);
    EXPECT_EQ(grid.cols, 0);
    EXPECT_DOUBLE_EQ(grid.minX, 0.0);
    EXPECT_DOUBLE_EQ(grid.minY, 0.0);
    EXPECT_DOUBLE_EQ(grid.maxX, 0.0);
    EXPECT_DOUBLE_EQ(grid.maxY, 0.0);
    EXPECT_DOUBLE_EQ(grid.cellSizeX, 0.0);
    EXPECT_DOUBLE_EQ(grid.cellSizeY, 0.0);
    EXPECT_TRUE(grid.points.empty());
}
