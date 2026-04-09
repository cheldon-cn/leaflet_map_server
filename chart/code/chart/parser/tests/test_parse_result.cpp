#include <gtest/gtest.h>
#include "parser/parse_result.h"
#include "parser/error_codes.h"
#include "parser/chart_format.h"

using namespace chart::parser;

TEST(ParseResultTest, DefaultConstructor) {
    ParseResult result;
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::Success);
    EXPECT_TRUE(result.errorMessage.empty());
    EXPECT_TRUE(result.filePath.empty());
    EXPECT_TRUE(result.features.empty());
}

TEST(ParseResultTest, SetError) {
    ParseResult result;
    
    result.SetError(ErrorCode::ErrFileNotFound, "File not found");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::ErrFileNotFound);
    EXPECT_EQ(result.errorMessage, "File not found");
    EXPECT_TRUE(result.HasError());
}

TEST(ParseResultTest, ClearError) {
    ParseResult result;
    result.SetError(ErrorCode::ErrFileNotFound, "File not found");
    
    result.ClearError();
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.errorCode, ErrorCode::Success);
    EXPECT_TRUE(result.errorMessage.empty());
    EXPECT_FALSE(result.HasError());
}

TEST(ParseResultTest, HasError) {
    ParseResult result;
    
    EXPECT_FALSE(result.HasError());
    
    result.SetError(ErrorCode::ErrFileNotFound, "Error");
    
    EXPECT_TRUE(result.HasError());
}

TEST(FeatureTest, DefaultConstructor) {
    Feature feature;
    
    EXPECT_TRUE(feature.id.empty());
    EXPECT_EQ(feature.type, FeatureType::Unknown);
    EXPECT_EQ(feature.geometry.type, GeometryType::Unknown);
    EXPECT_TRUE(feature.className.empty());
    EXPECT_EQ(feature.rcid, 0);
}

TEST(GeometryTest, DefaultConstructor) {
    Geometry geometry;
    
    EXPECT_EQ(geometry.type, GeometryType::Unknown);
    EXPECT_TRUE(geometry.points.empty());
    EXPECT_TRUE(geometry.rings.empty());
}

TEST(PointTest, DefaultConstructor) {
    Point point;
    
    EXPECT_DOUBLE_EQ(point.x, 0.0);
    EXPECT_DOUBLE_EQ(point.y, 0.0);
    EXPECT_DOUBLE_EQ(point.z, 0.0);
}

TEST(PointTest, ParameterizedConstructor) {
    Point point(120.5, 31.2, 10.0);
    
    EXPECT_DOUBLE_EQ(point.x, 120.5);
    EXPECT_DOUBLE_EQ(point.y, 31.2);
    EXPECT_DOUBLE_EQ(point.z, 10.0);
}

TEST(ParseStatisticsTest, DefaultConstructor) {
    ParseStatistics stats;
    
    EXPECT_EQ(stats.totalFeatureCount, 0);
    EXPECT_EQ(stats.successCount, 0);
    EXPECT_EQ(stats.failedCount, 0);
    EXPECT_EQ(stats.skippedCount, 0);
    EXPECT_DOUBLE_EQ(stats.parseTimeMs, 0.0);
    EXPECT_DOUBLE_EQ(stats.memoryUsedMB, 0.0);
}

TEST(AttributeValueTest, DefaultConstructor) {
    AttributeValue value;
    
    EXPECT_EQ(value.type, AttributeValue::Type::Unknown);
    EXPECT_EQ(value.intValue, 0);
    EXPECT_DOUBLE_EQ(value.doubleValue, 0.0);
    EXPECT_TRUE(value.stringValue.empty());
    EXPECT_TRUE(value.listValue.empty());
}

TEST(ErrorCodeTest, ErrorCodeToString) {
    EXPECT_EQ(ErrorCodeToString(ErrorCode::Success), "Success");
    EXPECT_EQ(ErrorCodeToString(ErrorCode::ErrFileNotFound), "File not found");
    EXPECT_EQ(ErrorCodeToString(ErrorCode::ErrGDALInitFailed), "GDAL initialization failed");
    EXPECT_EQ(ErrorCodeToString(ErrorCode::ErrUnknown), "Unknown error");
}

TEST(FeatureTypeTest, FeatureTypeToString) {
    EXPECT_EQ(FeatureTypeToString(FeatureType::SOUNDG), "SOUNDG");
    EXPECT_EQ(FeatureTypeToString(FeatureType::LIGHTS), "LIGHTS");
    EXPECT_EQ(FeatureTypeToString(FeatureType::DEPARE), "DEPARE");
    EXPECT_EQ(FeatureTypeToString(FeatureType::Unknown), "Unknown");
}

TEST(GeometryTypeTest, GeometryTypeToString) {
    EXPECT_EQ(GeometryTypeToString(GeometryType::Point), "Point");
    EXPECT_EQ(GeometryTypeToString(GeometryType::Line), "Line");
    EXPECT_EQ(GeometryTypeToString(GeometryType::Area), "Area");
    EXPECT_EQ(GeometryTypeToString(GeometryType::Unknown), "Unknown");
}

TEST(ChartFormatTest, ChartFormatToString) {
    EXPECT_EQ(ChartFormatToString(ChartFormat::S57), "S57");
    EXPECT_EQ(ChartFormatToString(ChartFormat::S101), "S101");
    EXPECT_EQ(ChartFormatToString(ChartFormat::Unknown), "Unknown");
}

TEST(ChartFormatTest, StringToChartFormat) {
    EXPECT_EQ(StringToChartFormat("S57"), ChartFormat::S57);
    EXPECT_EQ(StringToChartFormat("s57"), ChartFormat::S57);
    EXPECT_EQ(StringToChartFormat("S101"), ChartFormat::S101);
    EXPECT_EQ(StringToChartFormat("unknown"), ChartFormat::Unknown);
}
