#include <gtest/gtest.h>
#include "ogc/db/wkb_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/envelope.h"

using namespace ogc;
using namespace ogc::db;

class WkbConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(WkbConverterTest, PointToWkb) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, PointFromWkb) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toResult = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
    auto* outPoint = static_cast<Point*>(geometry.get());
    EXPECT_DOUBLE_EQ(outPoint->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(outPoint->GetY(), 2.0);
    EXPECT_EQ(geometry->GetSRID(), 4326);
}

TEST_F(WkbConverterTest, LineStringToWkb) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, LineStringFromWkb) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
    auto* outLine = static_cast<LineString*>(geometry.get());
    EXPECT_EQ(outLine->GetNumPoints(), 3);
}

TEST_F(WkbConverterTest, PolygonToWkb) {
    CoordinateList exteriorCoords = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(polygon.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, PolygonFromWkb) {
    CoordinateList exteriorCoords = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(polygon.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(WkbConverterTest, HexWkbConversion) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::string hexWkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toHexResult = WkbConverter::GeometryToHexWkb(point.get(), hexWkb, options);
    ASSERT_TRUE(toHexResult.IsSuccess());
    EXPECT_GT(hexWkb.size(), 0);
    
    std::unique_ptr<Geometry> geometry;
    Result fromHexResult = WkbConverter::HexWkbToGeometry(hexWkb, geometry, options);
    ASSERT_TRUE(fromHexResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbConverterTest, NullGeometry) {
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(nullptr, wkb, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbConverterTest, EmptyGeometry) {
    auto line = LineString::Create();
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(WkbConverterTest, InvalidWkbData) {
    std::vector<uint8_t> invalidWkb = {0x00, 0x01, 0xFF, 0xFF};
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbConverter::WkbToGeometry(invalidWkb, geometry, options);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with invalid WKB data";
}

TEST_F(WkbConverterTest, EmptyWkbData) {
    std::vector<uint8_t> emptyWkb;
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbConverter::WkbToGeometry(emptyWkb, geometry, options);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with empty WKB data";
}

TEST_F(WkbConverterTest, InvalidHexWkb) {
    std::string invalidHex = "NOT_VALID_HEX";
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbConverter::HexWkbToGeometry(invalidHex, geometry, options);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with invalid hex string";
}

TEST_F(WkbConverterTest, EmptyHexWkb) {
    std::string emptyHex;
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbConverter::HexWkbToGeometry(emptyHex, geometry, options);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with empty hex string";
}

TEST_F(WkbConverterTest, NullGeometryToHexWkb) {
    std::string hexWkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToHexWkb(nullptr, hexWkb, options);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with null geometry";
}

TEST_F(WkbConverterTest, LargeCoordinateBoundary) {
    auto point = Point::Create(1e10, -1e10);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle large coordinates";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
}

TEST_F(WkbConverterTest, ZeroCoordinateBoundary) {
    auto point = Point::Create(0.0, 0.0);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle zero coordinates";
}

TEST_F(WkbConverterTest, BigEndianOption) {
    auto point = Point::Create(1.0, 2.0);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.bigEndian = true;
    
    Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should support big endian";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
}

TEST_F(WkbConverterTest, MultiPointConversion) {
    auto mp = MultiPoint::Create();
    mp->AddPoint(Point::Create(1.0, 2.0));
    mp->AddPoint(Point::Create(3.0, 4.0));
    mp->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toResult = WkbConverter::GeometryToWkb(mp.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess()) << "Should convert MultiPoint";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kMultiPoint);
}

TEST_F(WkbConverterTest, MultiLineStringConversion) {
    auto ml = MultiLineString::Create();
    
    CoordinateList coords1 = {{0, 0}, {1, 1}};
    auto line1 = LineString::Create(coords1);
    ml->AddLineString(std::move(line1));
    
    CoordinateList coords2 = {{2, 2}, {3, 3}};
    auto line2 = LineString::Create(coords2);
    ml->AddLineString(std::move(line2));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(ml.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess()) << "Should convert MultiLineString";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kMultiLineString);
}

TEST_F(WkbConverterTest, MultiPolygonConversion) {
    auto mp = MultiPolygon::Create();
    
    CoordinateList coords1 = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}};
    auto ring1 = LinearRing::Create(coords1, true);
    auto poly1 = Polygon::Create(std::move(ring1));
    mp->AddPolygon(std::move(poly1));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(mp.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess()) << "Should convert MultiPolygon";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kMultiPolygon);
}

TEST_F(WkbConverterTest, GeometryCollectionConversion) {
    auto gc = GeometryCollection::Create();
    gc->AddGeometry(Point::Create(1.0, 2.0));
    gc->AddGeometry(LineString::Create({{0, 0}, {1, 1}}));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(gc.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess()) << "Should convert GeometryCollection";
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kGeometryCollection);
}
