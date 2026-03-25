#include <gtest/gtest.h>
#include "ogc/db/geojson_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"

using namespace ogc;
using namespace ogc::db;

class GeoJsonConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(GeoJsonConverterTest, PointToJson) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::string json;
    GeoJsonOptions options;
    options.prettyPrint = true;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("Point"), std::string::npos);
    EXPECT_NE(json.find("1"), std::string::npos);
    EXPECT_NE(json.find("2"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, PointFromJson) {
    std::string json = "{\"type\":\"Point\",\"coordinates\":[1.0,2.0]}";
    
    std::unique_ptr<Geometry> geometry;
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
    auto* pt = static_cast<Point*>(geometry.get());
    EXPECT_DOUBLE_EQ(pt->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(pt->GetY(), 2.0);
}

TEST_F(GeoJsonConverterTest, LineStringToJson) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(line.get(), json);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("LineString"), std::string::npos);
    EXPECT_NE(json.find("0"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, LineStringFromJson) {
    std::string json = "{\"type\":\"LineString\",\"coordinates\":[[0,0],[1,1],[2,0]]}";
    
    std::unique_ptr<Geometry> geometry;
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
    auto* line = static_cast<LineString*>(geometry.get());
    EXPECT_EQ(line->GetNumPoints(), 3);
}

TEST_F(GeoJsonConverterTest, PolygonToJson) {
    CoordinateList exteriorCoords = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(polygon.get(), json);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("Polygon"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, PolygonFromJson) {
    std::string json = "{\"type\":\"Polygon\",\"coordinates\":[[[0,0],[4,0],[4,4],[0,4],[0,0]]]}";
    
    std::unique_ptr<Geometry> geometry;
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
    auto* poly = static_cast<Polygon*>(geometry.get());
    EXPECT_EQ(poly->GetExteriorRing()->GetNumPoints(), 5);
}

TEST_F(GeoJsonConverterTest, MultiPointToJson) {
    auto mp = MultiPoint::Create();
    mp->AddPoint(Point::Create(1.0, 2.0));
    mp->AddPoint(Point::Create(3.0, 4.0));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(mp.get(), json);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("MultiPoint"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, MultiPolygonToJson) {
    auto mp = MultiPolygon::Create();
    
    CoordinateList coords1 = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}};
    auto ring1 = LinearRing::Create(coords1, true);
    auto poly1 = Polygon::Create(std::move(ring1));
    mp->AddPolygon(std::move(poly1));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(mp.get(), json);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("MultiPolygon"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, PrettyPrint) {
    auto point = Point::Create(1.0, 2.0);
    
    std::string jsonPretty;
    std::string jsonCompact;
    
    GeoJsonOptions prettyOptions;
    prettyOptions.prettyPrint = true;
    
    GeoJsonConverter::GeometryToJson(point.get(), jsonPretty, prettyOptions);
    GeoJsonConverter::GeometryToJson(point.get(), jsonCompact, GeoJsonOptions());
    
    EXPECT_GT(jsonPretty.length(), jsonCompact.length());
}

TEST_F(GeoJsonConverterTest, IncludeBoundingBox) {
    auto point = Point::Create(1.0, 2.0);
    
    std::string json;
    GeoJsonOptions options;
    options.includeBoundingBox = true;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("bbox"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, Precision) {
    auto point = Point::Create(1.123456789, 2.987654321);
    
    std::string json6, json2;
    GeoJsonOptions options6, options2;
    options6.precision = 6;
    options2.precision = 2;
    
    GeoJsonConverter::GeometryToJson(point.get(), json6, options6);
    GeoJsonConverter::GeometryToJson(point.get(), json2, options2);
    
    EXPECT_NE(json6.find("1.123457"), std::string::npos);
    EXPECT_NE(json2.find("1.12"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, InvalidJson) {
    std::string json = "not valid json";
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, NullGeometryToJson) {
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(nullptr, json);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, EmptyJsonString) {
    std::string json;
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with empty JSON string";
}

TEST_F(GeoJsonConverterTest, MissingTypeField) {
    std::string json = "{\"coordinates\":[1.0,2.0]}";
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with missing type field";
}

TEST_F(GeoJsonConverterTest, InvalidGeometryType) {
    std::string json = "{\"type\":\"InvalidType\",\"coordinates\":[1.0,2.0]}";
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with invalid geometry type";
}

TEST_F(GeoJsonConverterTest, MissingCoordinatesField) {
    std::string json = "{\"type\":\"Point\"}";
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with missing coordinates";
}

TEST_F(GeoJsonConverterTest, MalformedCoordinates) {
    std::string json = "{\"type\":\"Point\",\"coordinates\":\"not_an_array\"}";
    std::unique_ptr<Geometry> geometry;
    
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_FALSE(result.IsSuccess()) << "Should fail with malformed coordinates";
}

TEST_F(GeoJsonConverterTest, MultiLineStringToJson) {
    auto ml = MultiLineString::Create();
    
    CoordinateList coords1 = {{0, 0}, {1, 1}};
    auto line1 = LineString::Create(coords1);
    ml->AddLineString(std::move(line1));
    
    CoordinateList coords2 = {{2, 2}, {3, 3}};
    auto line2 = LineString::Create(coords2);
    ml->AddLineString(std::move(line2));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(ml.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should convert MultiLineString";
    EXPECT_NE(json.find("MultiLineString"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, MultiLineStringFromJson) {
    std::string json = "{\"type\":\"MultiLineString\",\"coordinates\":[[[0,0],[1,1]],[[2,2],[3,3]]]}";
    
    std::unique_ptr<Geometry> geometry;
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_TRUE(result.IsSuccess()) << "Should parse MultiLineString";
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kMultiLineString);
}

TEST_F(GeoJsonConverterTest, GeometryCollectionToJson) {
    auto gc = GeometryCollection::Create();
    gc->AddGeometry(Point::Create(1.0, 2.0));
    gc->AddGeometry(LineString::Create({{0, 0}, {1, 1}}));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(gc.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should convert GeometryCollection";
    EXPECT_NE(json.find("GeometryCollection"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, GeometryCollectionFromJson) {
    std::string json = "{\"type\":\"GeometryCollection\",\"geometries\":[{\"type\":\"Point\",\"coordinates\":[1,2]},{\"type\":\"LineString\",\"coordinates\":[[0,0],[1,1]]}]}";
    
    std::unique_ptr<Geometry> geometry;
    Result result = GeoJsonConverter::GeometryFromJson(json, geometry);
    EXPECT_TRUE(result.IsSuccess()) << "Should parse GeometryCollection";
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kGeometryCollection);
}

TEST_F(GeoJsonConverterTest, EmptyGeometryToJson) {
    auto line = LineString::Create();
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(line.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle empty geometry";
}

TEST_F(GeoJsonConverterTest, LargeCoordinateValues) {
    auto point = Point::Create(1e10, -1e10);
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle large coordinate values";
}

TEST_F(GeoJsonConverterTest, ZeroCoordinateValues) {
    auto point = Point::Create(0.0, 0.0);
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle zero coordinate values";
}

TEST_F(GeoJsonConverterTest, NegativePrecision) {
    auto point = Point::Create(1.123456, 2.987654);
    
    std::string json;
    GeoJsonOptions options;
    options.precision = -1;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle negative precision";
}

TEST_F(GeoJsonConverterTest, VeryHighPrecision) {
    auto point = Point::Create(1.123456789012345, 2.987654321098765);
    
    std::string json;
    GeoJsonOptions options;
    options.precision = 15;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle high precision";
}

TEST_F(GeoJsonConverterTest, PolygonWithHoles) {
    CoordinateList exteriorCoords = {{0, 0}, {10, 0}, {10, 10}, {0, 10}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    CoordinateList holeCoords = {{2, 2}, {8, 2}, {8, 8}, {2, 8}, {2, 2}};
    auto hole = LinearRing::Create(holeCoords, true);
    polygon->AddInteriorRing(std::move(hole));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(polygon.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should convert polygon with holes";
}

TEST_F(GeoJsonConverterTest, NestedGeometryCollection) {
    auto outer = GeometryCollection::Create();
    auto inner = GeometryCollection::Create();
    inner->AddGeometry(Point::Create(1.0, 2.0));
    outer->AddGeometry(std::move(inner));
    outer->AddGeometry(Point::Create(3.0, 4.0));
    
    std::string json;
    Result result = GeoJsonConverter::GeometryToJson(outer.get(), json);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle nested GeometryCollection";
}
