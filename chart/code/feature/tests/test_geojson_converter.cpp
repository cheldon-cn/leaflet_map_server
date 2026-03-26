#include "gtest/gtest.h"
#include "ogc/feature/geojson_converter.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/linearring.h"

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
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(116.4074, 39.9042));
    feature.GetGeometry()->SetSRID(4326);
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_NE(json.find("Point"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, LineStringToJson) {
    CNFeature feature(defn_);
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}, {2, 2}}));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_NE(json.find("LineString"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, PolygonToJson) {
    auto ring = LinearRing::Create({{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    
    CNFeature feature(defn_);
    feature.SetGeometry(std::move(polygon));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_NE(json.find("Polygon"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, FeatureToJson) {
    CNFeature feature(defn_);
    feature.SetFID(123);
    feature.SetGeometry(Point::Create(100.0, 200.0));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_NE(json.find("Feature"), std::string::npos);
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
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&collection);
    
    EXPECT_NE(json.find("FeatureCollection"), std::string::npos);
}

TEST_F(GeoJsonConverterTest, JsonToFeature) {
    std::string json = R"({
        "type": "Feature",
        "id": 456,
        "geometry": {"type": "Point", "coordinates": [50.0, 60.0]},
        "properties": {}
    })";
    
    CNGeoJSONConverter converter;
    CNFeature* feature = converter.FromGeoJSON(json);
    
    ASSERT_NE(feature, nullptr);
    
    delete feature;
}

TEST_F(GeoJsonConverterTest, JsonToFeatureCollection) {
    std::string json = R"({
        "type": "FeatureCollection",
        "features": [
            {
                "type": "Feature",
                "id": 1,
                "geometry": {"type": "Point", "coordinates": [0, 0]},
                "properties": {}
            }
        ]
    })";
    
    CNGeoJSONConverter converter;
    CNFeatureCollection* collection = converter.FromGeoJSONToCollection(json);
    
    ASSERT_NE(collection, nullptr);
    
    delete collection;
}

TEST_F(GeoJsonConverterTest, SetIndent) {
    CNGeoJSONConverter converter;
    
    converter.SetIndent(true);
    EXPECT_TRUE(converter.GetIndent());
    
    converter.SetIndent(false);
    EXPECT_FALSE(converter.GetIndent());
}

TEST_F(GeoJsonConverterTest, SetPrecision) {
    CNGeoJSONConverter converter;
    
    converter.SetPrecision(6);
    EXPECT_EQ(converter.GetPrecision(), 6);
    
    converter.SetPrecision(10);
    EXPECT_EQ(converter.GetPrecision(), 10);
}

TEST_F(GeoJsonConverterTest, RoundTripFeature) {
    CNFeature originalFeature(defn_);
    originalFeature.SetFID(999);
    originalFeature.SetGeometry(Point::Create(10.0, 20.0));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&originalFeature);
    
    CNFeature* parsed = converter.FromGeoJSON(json);
    ASSERT_NE(parsed, nullptr);
    
    delete parsed;
}

TEST_F(GeoJsonConverterTest, RoundTripFeatureCollection) {
    CNFeatureCollection originalCollection;
    originalCollection.SetFeatureDefinition(defn_);
    
    CNFeature* f1 = new CNFeature(defn_);
    f1->SetFID(1);
    f1->SetGeometry(Point::Create(0, 0));
    originalCollection.AddFeature(f1);
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&originalCollection);
    
    CNFeatureCollection* parsed = converter.FromGeoJSONToCollection(json);
    ASSERT_NE(parsed, nullptr);
    
    delete parsed;
}

TEST_F(GeoJsonConverterTest, NullGeometryFeature) {
    CNFeature feature(defn_);
    feature.SetFID(1);
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_FALSE(json.empty());
}

TEST_F(GeoJsonConverterTest, PrettyPrint) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(0, 0));
    
    CNGeoJSONConverter converter;
    converter.SetIndent(true);
    EXPECT_TRUE(converter.GetIndent());
    
    converter.SetIndent(false);
    EXPECT_FALSE(converter.GetIndent());
    
    std::string json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
}
