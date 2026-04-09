#include <gtest/gtest.h>
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/feature/geojson_converter.h"
#include "ogc/feature/wkb_wkt_converter.h"
#include "ogc/feature/spatial_query.h"
#include "ogc/feature/batch_processor.h"
#include "ogc/feature/feature_iterator.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"

using namespace ogc;

class FeatureIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("integration_test");
        
        CNFieldDefn* idField = CreateCNFieldDefn("id");
        idField->SetType(CNFieldType::kInteger);
        defn_->AddFieldDefn(idField);
        
        CNFieldDefn* nameField = CreateCNFieldDefn("name");
        nameField->SetType(CNFieldType::kString);
        defn_->AddFieldDefn(nameField);
        
        CNGeomFieldDefn* geomField = CreateCNGeomFieldDefn("geometry");
        geomField->SetGeomType(GeomType::kPoint);
        defn_->AddGeomFieldDefn(geomField);
    }
    
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(FeatureIntegrationTest, FeatureToGeoJsonWorkflow) {
    CNFeature feature(defn_);
    feature.SetFID(1);
    feature.SetFieldInteger(static_cast<size_t>(0), 100);
    feature.SetFieldString(static_cast<size_t>(1), "test_feature");
    feature.SetGeometry(Point::Create(116.4074, 39.9042));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("Feature"), std::string::npos);
    EXPECT_NE(json.find("Point"), std::string::npos);
}

TEST_F(FeatureIntegrationTest, FeatureToWktWorkflow) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(100.0, 200.0));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_FALSE(wkt.empty());
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(FeatureIntegrationTest, FeatureToWkbWorkflow) {
    CNFeature feature(defn_);
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}, {2, 2}}));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&feature);
    
    EXPECT_FALSE(wkb.empty());
}

TEST_F(FeatureIntegrationTest, GeoJsonToFeatureWorkflow) {
    std::string json = R"({
        "type": "Feature",
        "id": 42,
        "geometry": {"type": "Point", "coordinates": [50.0, 60.0]},
        "properties": {"id": 1, "name": "test"}
    })";
    
    CNGeoJSONConverter converter;
    CNFeature* feature = converter.FromGeoJSON(json);
    
    ASSERT_NE(feature, nullptr);
    
    delete feature;
}

TEST_F(FeatureIntegrationTest, WktToFeatureWorkflow) {
    CNFeature original(defn_);
    original.SetGeometry(Point::Create(100.0, 200.0));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&original);
    
    EXPECT_FALSE(wkt.empty());
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(FeatureIntegrationTest, WkbToFeatureWorkflow) {
    CNFeature original(defn_);
    original.SetGeometry(Point::Create(123.456, -78.901));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    
    CNFeature* feature = converter.FromWKB(wkb);
    ASSERT_NE(feature, nullptr);
    
    EXPECT_NE(feature->GetGeometry(), nullptr);
    
    delete feature;
}

TEST_F(FeatureIntegrationTest, FeatureCollectionIteration) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 10; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetFieldInteger(static_cast<size_t>(0), i * 10);
        f->SetGeometry(Point::Create(i * 1.0, i * 2.0));
        collection.AddFeature(f);
    }
    
    CNFeatureIterator iterator(&collection);
    int count = 0;
    while (iterator.HasNext()) {
        CNFeature* f = iterator.Next();
        ASSERT_NE(f, nullptr);
        EXPECT_EQ(f->GetFID(), count);
        count++;
    }
    EXPECT_EQ(count, 10);
}

TEST_F(FeatureIntegrationTest, BatchProcessingWorkflow) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 20; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetFieldInteger(static_cast<size_t>(0), i);
        f->SetFieldString(static_cast<size_t>(1), "feature_" + std::to_string(i));
        f->SetGeometry(Point::Create(i * 0.1, i * 0.2));
        collection.AddFeature(f);
    }
    
    CNBatchProcessor processor;
    processor.SetBatchSize(5);
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, 20);
}

TEST_F(FeatureIntegrationTest, SpatialQueryWorkflow) {
    auto point = Point::Create(116.4074, 39.9042);
    
    CNSpatialQuery query = CNSpatialQuery::Intersects(std::move(point));
    query.SetBufferDistance(100.0);
    query.SetSRID(4326);
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kIntersects);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), 100.0);
    EXPECT_EQ(query.GetSRID(), 4326);
    
    std::string wkt = query.ToWKT();
    EXPECT_FALSE(wkt.empty());
}

TEST_F(FeatureIntegrationTest, FeatureCloneAndCompare) {
    CNFeature original(defn_);
    original.SetFID(100);
    original.SetFieldInteger(static_cast<size_t>(0), 42);
    original.SetFieldString(static_cast<size_t>(1), "original");
    original.SetGeometry(Point::Create(10.0, 20.0));
    
    CNFeature* cloned = original.Clone();
    ASSERT_NE(cloned, nullptr);
    
    EXPECT_TRUE(original.Equal(*cloned));
    EXPECT_EQ(cloned->GetFID(), 100);
    EXPECT_EQ(cloned->GetFieldAsInteger(static_cast<size_t>(0)), 42);
    EXPECT_EQ(cloned->GetFieldAsString(static_cast<size_t>(1)), "original");
    
    delete cloned;
}

TEST_F(FeatureIntegrationTest, FeatureCollectionClone) {
    CNFeatureCollection original;
    original.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        original.AddFeature(f);
    }
    
    CNFeatureCollection* cloned = original.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFeatureCount(), 5);
    
    delete cloned;
}

TEST_F(FeatureIntegrationTest, RoundTripGeoJson) {
    CNFeature original(defn_);
    original.SetFID(999);
    original.SetFieldInteger(static_cast<size_t>(0), 123);
    original.SetFieldString(static_cast<size_t>(1), "roundtrip_test");
    original.SetGeometry(Point::Create(45.0, 90.0));
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&original);
    
    CNFeature* parsed = converter.FromGeoJSON(json);
    ASSERT_NE(parsed, nullptr);
    
    delete parsed;
}

TEST_F(FeatureIntegrationTest, RoundTripWkb) {
    auto ring = LinearRing::Create({{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    
    CNFeature original(defn_);
    original.SetGeometry(std::move(polygon));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    
    CNFeature* parsed = converter.FromWKB(wkb);
    ASSERT_NE(parsed, nullptr);
    
    GeometryPtr geom = parsed->GetGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPolygon);
    
    delete parsed;
}

TEST_F(FeatureIntegrationTest, MultipleGeometryTypes) {
    CNFeature feature(defn_);
    
    feature.SetGeometry(Point::Create(0, 0));
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
    
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}}));
    json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
    
    auto ring = LinearRing::Create({{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}}, true);
    feature.SetGeometry(Polygon::Create(std::move(ring)));
    json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
}

TEST_F(FeatureIntegrationTest, LargeFeatureCollection) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    const int featureCount = 1000;
    for (int i = 0; i < featureCount; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetFieldInteger(static_cast<size_t>(0), i);
        f->SetGeometry(Point::Create(i * 0.001, i * 0.002));
        collection.AddFeature(f);
    }
    
    EXPECT_EQ(collection.GetFeatureCount(), featureCount);
    
    CNBatchProcessor processor;
    processor.SetBatchSize(100);
    
    BatchResult result = processor.Process(&collection, BatchOperation::kRead);
    EXPECT_EQ(result.success_count, featureCount);
}

TEST_F(FeatureIntegrationTest, FeatureWithNullGeometry) {
    CNFeature feature(defn_);
    feature.SetFID(1);
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
}

TEST_F(FeatureIntegrationTest, FeatureWithAllFieldTypes) {
    CNFeature feature(defn_);
    
    feature.SetFieldInteger(static_cast<size_t>(0), 42);
    feature.SetFieldString(static_cast<size_t>(1), "test");
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(&feature);
    EXPECT_FALSE(json.empty());
}
