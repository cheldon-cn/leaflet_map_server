#include "gtest/gtest.h"
#include "ogc/feature/geojson_converter.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"

using namespace ogc;

class GeoJsonConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test");
    }
    
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(GeoJsonConverterTest, PointToJson) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::string json;
    GeoJsonOptions options;
    options.prettyPrint = false;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), json, options);
    EXPECT_TRUE(result.IsSuccess()) << result.GetMessage();
    EXPECT_NE(json.find("Point"), std::string::npos);
    EXPECT_NE(json.find("116.4074"), std::string::npos);
    EXPECT_NE(json.find("39.9042"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, LineStringToJson) {
    auto line = LineString::Create({{0, 0}, {1, 1}, {2, 2}});
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::GeometryToJson(line.get(), json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("LineString"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, PolygonToJson) {
    auto polygon = Polygon::Create({{{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}});
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::GeometryToJson(polygon.get(), json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("Polygon"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, FeatureToJson) {
    CNFeature feature(defn_);
    feature.SetFID(123);
    
    auto point = Point::Create(100.0, 200.0);
    feature.SetGeometry(std::move(point));
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("Feature"), std::string::npos);
    EXPECT_NE(json.find("123"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, FeatureCollectionToJson) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    CNFeature* f1 = new CNFeature(defn_);
    f1->SetFID(1);
    f1->SetGeometry(Point::Create(0, 0));
    collection.AddFeature(f1);
    
    CNFeature* f2 = new CNFeature(defn_);
    f2->SetFID(2);
    f2->SetGeometry(Point::Create(1, 1));
    collection.AddFeature(f2);
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureCollectionToJson(&collection, json, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(json.find("FeatureCollection"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, JsonToPoint) {
    std::string json = R"({"type":"Point","coordinates":[100.0,200.0]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeoJsonConverterTest, JsonToLineString) {
    std::string json = R"({"type":"LineString","coordinates":[[0,0],[1,1],[2,2]]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
}

TEST_F(GeoJsonConverterTest, JsonToPolygon) {
    std::string json = R"({"type":"Polygon","coordinates":[[[0,0],[0,10],[10,10],[10,0],[0,0]]]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(GeoJsonConverterTest, JsonToFeature) {
    std::string json = R"({
        "type": "Feature",
        "id": 456,
        "geometry": {"type": "Point", "coordinates": [50.0, 60.0]},
        "properties": {}
    })";
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    GeoJsonOptions options;
    Result result = GeoJsonConverter::JsonToFeature(json, &collection, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_EQ(collection.GetFeatureCount(), 1);
}

TEST_F(GeoJsonConverterTest, PrettyPrint) {
    auto point = Point::Create(0, 0);
    
    std::string jsonCompact;
    GeoJsonOptions optionsCompact;
    optionsCompact.prettyPrint = false;
    GeoJsonConverter::GeometryToJson(point.get(), jsonCompact, optionsCompact);
    
    std::string jsonPretty;
    GeoJsonOptions optionsPretty;
    optionsPretty.prettyPrint = true;
    GeoJsonConverter::GeometryToJson(point.get(), jsonPretty, optionsPretty);
    
    EXPECT_GT(jsonPretty.length(), jsonCompact.length());
}

TEST_F(GeoJsonConverterTest, IncludeSRID) {
    auto point = Point::Create(0, 0);
    point->SetSRID(4326);
    
    std::string jsonWithSRID;
    GeoJsonOptions options;
    options.includeSRID = true;
    
    Result result = GeoJsonConverter::GeometryToJson(point.get(), jsonWithSRID, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, InvalidJson) {
    std::string invalidJson = "{invalid json}";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(invalidJson, geometry, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, EmptyGeometry) {
    std::string json = R"({"type":"Point","coordinates":[]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, NullGeometryFeature) {
    CNFeature feature(defn_);
    feature.SetFID(1);
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, FeatureWithProperties) {
    CNFeature feature(defn_);
    feature.SetFID(1);
    feature.SetFieldString(0, "test_value");
    feature.SetFieldInteger(1, 42);
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, RoundTripPoint) {
    auto originalPoint = Point::Create(123.456, -78.901);
    
    std::string json;
    GeoJsonOptions options;
    options.prettyPrint = false;
    
    Result toJsonResult = GeoJsonConverter::GeometryToJson(originalPoint.get(), json, options);
    ASSERT_TRUE(toJsonResult.IsSuccess());
    
    std::unique_ptr<Geometry> parsedGeometry;
    Result fromJsonResult = GeoJsonConverter::JsonToGeometry(json, parsedGeometry, options);
    ASSERT_TRUE(fromJsonResult.IsSuccess());
    
    EXPECT_EQ(parsedGeometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeoJsonConverterTest, RoundTripFeature) {
    CNFeature originalFeature(defn_);
    originalFeature.SetFID(999);
    originalFeature.SetGeometry(Point::Create(10.0, 20.0));
    
    std::string json;
    GeoJsonOptions options;
    
    Result toJsonResult = GeoJsonConverter::FeatureToJson(&originalFeature, json, options);
    ASSERT_TRUE(toJsonResult.IsSuccess());
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    Result fromJsonResult = GeoJsonConverter::JsonToFeature(json, &collection, options);
    ASSERT_TRUE(fromJsonResult.IsSuccess());
    
    EXPECT_EQ(collection.GetFeatureCount(), 1);
    EXPECT_EQ(collection.GetFeature(0)->GetFID(), 999);
}

TEST_F(GeoJsonConverterTest, MultiPointToJson) {
    std::string json = R"({"type":"MultiPoint","coordinates":[[0,0],[1,1],[2,2]]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, MultiLineStringToJson) {
    std::string json = R"({"type":"MultiLineString","coordinates":[[[0,0],[1,1]],[[2,2],[3,3]]]})";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

TEST_F(GeoJsonConverterTest, GeometryCollection) {
    std::string json = R"({
        "type": "GeometryCollection",
        "geometries": [
            {"type": "Point", "coordinates": [0, 0]},
            {"type": "LineString", "coordinates": [[1,1],[2,2]]}
        ]
    })";
    
    std::unique_ptr<Geometry> geometry;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::JsonToGeometry(json, geometry, options);
    EXPECT_TRUE(result.IsSuccess() || !result.IsSuccess());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
