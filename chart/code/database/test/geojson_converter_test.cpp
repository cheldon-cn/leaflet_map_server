#include <gtest/gtest.h>
#include "ogc/db/geojson_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multipolygon.h"

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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
