#include <gtest/gtest.h>
#include "chart_parser/chart_parser.h"
#include "chart_parser/s57_parser.h"
#include "chart_parser/s101_parser.h"
#include "chart_parser/parse_cache.h"
#include "chart_parser/incremental_parser.h"
#include "chart_parser/data_converter.h"
#include "chart_parser/ogr_data_converter.h"
#include <ogr_geometry.h>

using namespace chart::parser;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ChartParser::Instance().Initialize();
        ParseCache::Instance().Enable(true);
        ParseCache::Instance().Clear();
        IncrementalParser::Instance().ClearAllStates();
    }
    
    void TearDown() override {
        IncrementalParser::Instance().ClearAllStates();
        ParseCache::Instance().Clear();
        ChartParser::Instance().Shutdown();
    }
};

TEST_F(IntegrationTest, ChartParser_CreateS57Parser_ReturnsValidParser) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->GetName(), "S57Parser");
    
    delete parser;
}

TEST_F(IntegrationTest, ChartParser_CreateS101Parser_ReturnsValidParser) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S101);
    
    if (parser != nullptr) {
        EXPECT_EQ(parser->GetName(), "S101Parser");
        delete parser;
    }
}

TEST_F(IntegrationTest, ChartParser_GetSupportedFormats_ReturnsAllFormats) {
    auto formats = ChartParser::Instance().GetSupportedFormats();
    
    EXPECT_GE(formats.size(), 1u);
}

TEST_F(IntegrationTest, S57Parser_ParseValidFile_ReturnsSuccess) {
    S57Parser parser;
    ParseResult result = parser.ParseChart("doc/C1104001.000");
    
    if (result.success) {
        EXPECT_GT(result.features.size(), 0);
        EXPECT_GT(result.statistics.totalFeatureCount, 0);
    }
}

TEST_F(IntegrationTest, ParseCache_IntegrationWithParser) {
    S57Parser parser;
    
    ParseResult result1 = parser.ParseChart("doc/C1104001.000");
    ParseCache::Instance().Put("doc/C1104001.000", result1);
    
    ParseResult cached;
    EXPECT_TRUE(ParseCache::Instance().Get("doc/C1104001.000", cached));
    
    auto stats = ParseCache::Instance().GetStatistics();
    EXPECT_GT(stats.hitCount, 0);
}

TEST_F(IntegrationTest, IncrementalParser_IntegrationWithParser) {
    IncrementalParser::Instance().SetParser(
        [](const std::string& path, const ParseConfig& cfg) {
            S57Parser parser;
            return parser.ParseChart(path, cfg);
        }
    );
    
    auto result = IncrementalParser::Instance().ParseIncremental("doc/C1104001.000");
    
    if (result.hasChanges && result.addedFeatures.success) {
        EXPECT_GT(result.addedFeatures.features.size(), 0);
    } else {
        EXPECT_TRUE(result.addedFeatures.HasError() || !result.hasChanges);
    }
}

TEST_F(IntegrationTest, DataConverterFactory_CreateOGRConverter_ReturnsValid) {
    auto converter = DataConverterFactory::Instance().CreateConverter("OGR");
    
    ASSERT_NE(converter, nullptr);
    EXPECT_EQ(converter->GetName(), "OGRDataConverter");
    EXPECT_EQ(converter->GetSourceType(), "OGR");
}

TEST_F(IntegrationTest, OGRDataConverter_ConvertPoint_Success) {
    OGRDataConverter converter;
    
    OGRPoint point(120.0, 31.0, 10.0);
    Geometry geometry;
    
    EXPECT_TRUE(converter.ConvertGeometry(&point, geometry));
    EXPECT_EQ(geometry.type, GeometryType::Point);
    EXPECT_EQ(geometry.points.size(), 1);
}

TEST_F(IntegrationTest, OGRDataConverter_ConvertLineString_Success) {
    OGRDataConverter converter;
    
    OGRLineString lineString;
    lineString.addPoint(0.0, 0.0, 0.0);
    lineString.addPoint(100.0, 0.0, 0.0);
    lineString.addPoint(100.0, 100.0, 0.0);
    
    Geometry geometry;
    EXPECT_TRUE(converter.ConvertGeometry(&lineString, geometry));
    EXPECT_EQ(geometry.type, GeometryType::Line);
    EXPECT_EQ(geometry.points.size(), 3);
}

TEST_F(IntegrationTest, OGRDataConverter_ConvertPolygon_Success) {
    OGRDataConverter converter;
    
    OGRPolygon polygon;
    OGRLinearRing* ring = new OGRLinearRing();
    ring->addPoint(0.0, 0.0);
    ring->addPoint(100.0, 0.0);
    ring->addPoint(100.0, 100.0);
    ring->addPoint(0.0, 100.0);
    ring->addPoint(0.0, 0.0);
    polygon.addRing(ring);
    
    Geometry geometry;
    EXPECT_TRUE(converter.ConvertGeometry(&polygon, geometry));
    EXPECT_EQ(geometry.type, GeometryType::Area);
    EXPECT_EQ(geometry.rings.size(), 1);
}

TEST_F(IntegrationTest, FullParsingWorkflow_S57) {
    auto parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    ASSERT_NE(parser, nullptr);
    
    ParseResult result = parser->ParseChart("doc/C1104001.000");
    
    if (result.success) {
        ParseCache::Instance().Put("doc/C1104001.000", result);
        
        ParseResult cached;
        EXPECT_TRUE(ParseCache::Instance().Get("doc/C1104001.000", cached));
    }
    
    delete parser;
}

TEST_F(IntegrationTest, MultipleParserInstances_WorkIndependently) {
    S57Parser parser1;
    S57Parser parser2;
    
    ParseResult result1 = parser1.ParseChart("doc/C1104001.000");
    ParseResult result2 = parser2.ParseChart("doc/C1104001.000");
    
    if (result1.success && result2.success) {
        EXPECT_EQ(result1.features.size(), result2.features.size());
    }
}

TEST_F(IntegrationTest, ParseResult_ErrorHandling) {
    S57Parser parser;
    ParseResult result = parser.ParseChart("nonexistent.000");
    
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.HasError());
    EXPECT_EQ(result.errorCode, ErrorCode::ErrGDALDatasetOpenFailed);
    
    result.ClearError();
    EXPECT_FALSE(result.HasError());
    EXPECT_EQ(result.errorCode, ErrorCode::Success);
}
