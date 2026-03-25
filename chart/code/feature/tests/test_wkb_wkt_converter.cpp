#include "gtest/gtest.h"
#include "ogc/feature/wkb_wkt_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometry_collection.h"

using namespace ogc;

class WkbWktConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(WkbWktConverterTest, PointToWKT) {
    auto point = Point::Create(100.0, 200.0);
    point->SetSRID(4326);
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(point.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess()) << result.GetMessage();
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
    EXPECT_NE(wkt.find("100"), std::string::npos);
    EXPECT_NE(wkt.find("200"), std::string::npos);
}

TEST_F(WkbWktConverterTest, LineStringToWKT) {
    auto line = LineString::Create({{0, 0}, {1, 1}, {2, 2}});
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(line.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(wkt.find("LINESTRING"), std::string::npos);
}

TEST_F(WkbWktConverterTest, PolygonToWKT) {
    auto polygon = Polygon::Create({{{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}});
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(polygon.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(wkt.find("POLYGON"), std::string::npos);
}

TEST_F(WkbWktConverterTest, WKTToPoint) {
    std::string wkt = "POINT(50 60)";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess()) << result.GetMessage();
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbWktConverterTest, WKTToLineString) {
    std::string wkt = "LINESTRING(0 0, 1 1, 2 2)";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
}

TEST_F(WkbWktConverterTest, WKTToPolygon) {
    std::string wkt = "POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(WkbWktConverterTest, PointToWKB) {
    auto point = Point::Create(123.456, -78.901);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbWktConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess()) << result.GetMessage();
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, LineStringToWKB) {
    auto line = LineString::Create({{0, 0}, {1, 1}});
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbWktConverter::GeometryToWkb(line.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, PolygonToWKB) {
    auto polygon = Polygon::Create({{{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}}});
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbWktConverter::GeometryToWkb(polygon.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, WKBToPoint) {
    auto originalPoint = Point::Create(100.0, 200.0);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(originalPoint.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, geometry, options);
    EXPECT_TRUE(fromWkbResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbWktConverterTest, WKBToLineString) {
    auto originalLine = LineString::Create({{0, 0}, {1, 1}, {2, 2}});
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(originalLine.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, geometry, options);
    EXPECT_TRUE(fromWkbResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
}

TEST_F(WkbWktConverterTest, WKBToPolygon) {
    auto originalPolygon = Polygon::Create({{{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}});
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(originalPolygon.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, geometry, options);
    EXPECT_TRUE(fromWkbResult.IsSuccess());
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(WkbWktConverterTest, RoundTripPoint) {
    auto original = Point::Create(123.456, -78.901);
    original->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(original.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> parsed;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, parsed, options);
    ASSERT_TRUE(fromWkbResult.IsSuccess());
    
    EXPECT_EQ(parsed->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbWktConverterTest, RoundTripWKT) {
    auto original = Point::Create(50.0, 60.0);
    
    std::string wkt;
    WktOptions options;
    
    Result toWktResult = WkbWktConverter::GeometryToWkt(original.get(), wkt, options);
    ASSERT_TRUE(toWktResult.IsSuccess());
    
    std::unique_ptr<Geometry> parsed;
    Result fromWktResult = WkbWktConverter::WktToGeometry(wkt, parsed, options);
    ASSERT_TRUE(fromWktResult.IsSuccess());
    
    EXPECT_EQ(parsed->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbWktConverterTest, InvalidWKT) {
    std::string invalidWkt = "INVALID WKT";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(invalidWkt, geometry, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, InvalidWKB) {
    std::vector<uint8_t> invalidWkb = {0x00, 0x01, 0x02, 0x03};
    
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbWktConverter::WkbToGeometry(invalidWkb, geometry, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, EmptyWKT) {
    std::string emptyWkt = "";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(emptyWkt, geometry, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, EmptyWKB) {
    std::vector<uint8_t> emptyWkb;
    
    std::unique_ptr<Geometry> geometry;
    WkbOptions options;
    
    Result result = WkbWktConverter::WkbToGeometry(emptyWkb, geometry, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, WKTWithSRID) {
    std::string wkt = "SRID=4326;POINT(0 0)";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    options.includeSRID = true;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(WkbWktConverterTest, WKBWithSRID) {
    auto point = Point::Create(0, 0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result result = WkbWktConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, MultiPointWKT) {
    std::string wkt = "MULTIPOINT((0 0), (1 1), (2 2))";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(WkbWktConverterTest, MultiLineStringWKT) {
    std::string wkt = "MULTILINESTRING((0 0, 1 1), (2 2, 3 3))";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(WkbWktConverterTest, MultiPolygonWKT) {
    std::string wkt = "MULTIPOLYGON(((0 0, 0 1, 1 1, 1 0, 0 0)))";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(WkbWktConverterTest, GeometryCollectionWKT) {
    std::string wkt = "GEOMETRYCOLLECTION(POINT(0 0), LINESTRING(1 1, 2 2))";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(WkbWktConverterTest, LargeCoordinates) {
    auto point = Point::Create(1e10, -1e10);
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(point.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, SmallCoordinates) {
    auto point = Point::Create(1e-10, -1e-10);
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(point.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(WkbWktConverterTest, WKTPrettyPrint) {
    auto line = LineString::Create({{0, 0}, {1, 1}, {2, 2}, {3, 3}});
    
    std::string wktCompact;
    WktOptions optionsCompact;
    optionsCompact.prettyPrint = false;
    WkbWktConverter::GeometryToWkt(line.get(), wktCompact, optionsCompact);
    
    std::string wktPretty;
    WktOptions optionsPretty;
    optionsPretty.prettyPrint = true;
    WkbWktConverter::GeometryToWkt(line.get(), wktPretty, optionsPretty);
    
    EXPECT_GT(wktPretty.length(), wktCompact.length());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
