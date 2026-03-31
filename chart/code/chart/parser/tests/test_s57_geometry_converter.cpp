#include <gtest/gtest.h>
#include "chart_parser/s57_geometry_converter.h"
#include <ogr_geometry.h>

using namespace chart::parser;

class S57GeometryConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        converter_ = new S57GeometryConverter();
    }
    
    void TearDown() override {
        delete converter_;
    }
    
    S57GeometryConverter* converter_;
};

TEST_F(S57GeometryConverterTest, ConvertNullGeometry) {
    Geometry geometry;
    bool result = converter_->ConvertGeometry(nullptr, geometry);
    
    EXPECT_FALSE(result);
}

TEST_F(S57GeometryConverterTest, ConvertPoint) {
    OGRPoint point(120.0, 31.0, 10.0);
    
    Geometry geometry;
    bool result = converter_->ConvertGeometry(&point, geometry);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(geometry.type, GeometryType::Point);
    EXPECT_EQ(geometry.points.size(), 1);
    EXPECT_DOUBLE_EQ(geometry.points[0].x, 120.0);
    EXPECT_DOUBLE_EQ(geometry.points[0].y, 31.0);
    EXPECT_DOUBLE_EQ(geometry.points[0].z, 10.0);
}

TEST_F(S57GeometryConverterTest, ConvertLineString) {
    OGRLineString lineString;
    lineString.addPoint(0.0, 0.0, 0.0);
    lineString.addPoint(100.0, 0.0, 0.0);
    lineString.addPoint(100.0, 100.0, 0.0);
    
    Geometry geometry;
    bool result = converter_->ConvertGeometry(&lineString, geometry);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(geometry.type, GeometryType::Line);
    EXPECT_EQ(geometry.points.size(), 3);
    EXPECT_DOUBLE_EQ(geometry.points[0].x, 0.0);
    EXPECT_DOUBLE_EQ(geometry.points[1].x, 100.0);
    EXPECT_DOUBLE_EQ(geometry.points[2].y, 100.0);
}

TEST_F(S57GeometryConverterTest, ConvertPolygon) {
    OGRPolygon polygon;
    OGRLinearRing* ring = new OGRLinearRing();
    ring->addPoint(0.0, 0.0);
    ring->addPoint(100.0, 0.0);
    ring->addPoint(100.0, 100.0);
    ring->addPoint(0.0, 100.0);
    ring->addPoint(0.0, 0.0);
    polygon.addRing(ring);
    
    Geometry geometry;
    bool result = converter_->ConvertGeometry(&polygon, geometry);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(geometry.type, GeometryType::Area);
    EXPECT_EQ(geometry.rings.size(), 1);
    EXPECT_EQ(geometry.rings[0].size(), 5);
}

TEST_F(S57GeometryConverterTest, ConvertMultiPoint) {
    OGRMultiPoint multiPoint;
    OGRPoint* p1 = new OGRPoint(0.0, 0.0);
    OGRPoint* p2 = new OGRPoint(100.0, 100.0);
    multiPoint.addGeometry(p1);
    multiPoint.addGeometry(p2);
    
    Geometry geometry;
    bool result = converter_->ConvertGeometry(&multiPoint, geometry);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(geometry.type, GeometryType::MultiPoint);
    EXPECT_EQ(geometry.points.size(), 2);
}

TEST_F(S57GeometryConverterTest, ConvertMultiLineString) {
    OGRMultiLineString multiLine;
    OGRLineString* line1 = new OGRLineString();
    line1->addPoint(0.0, 0.0);
    line1->addPoint(100.0, 0.0);
    
    OGRLineString* line2 = new OGRLineString();
    line2->addPoint(0.0, 100.0);
    line2->addPoint(100.0, 100.0);
    
    multiLine.addGeometry(line1);
    multiLine.addGeometry(line2);
    
    Geometry geometry;
    bool result = converter_->ConvertGeometry(&multiLine, geometry);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(geometry.type, GeometryType::MultiLine);
    EXPECT_EQ(geometry.rings.size(), 2);
}
