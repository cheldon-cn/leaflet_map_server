#include "gtest/gtest.h"
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
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"

using namespace ogc;

class FeatureIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("integration_test");
        
        CNFieldDefn* idField = CNFieldDefn::Create("id", CNFieldType::kInteger);
        defn_->AddFieldDefn(idField);
        idField->ReleaseReference();
        
        CNFieldDefn* nameField = CNFieldDefn::Create("name", CNFieldType::kString);
        defn_->AddFieldDefn(nameField);
        nameField->ReleaseReference();
        
        CNGeomFieldDefn* geomField = CNGeomFieldDefn::Create("geometry");
        geomField->SetGeometryType(GeomType::kPoint);
        geomField->SetSRID(4326);
        defn_->AddGeomFieldDefn(geomField);
        geomField->ReleaseReference();
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
    feature.SetFieldInteger(0, 100);
    feature.SetFieldString(1, "test_feature");
    feature.SetGeometry(Point::Create(116.4074, 39.9042));
    
    std::string json;
    GeoJsonOptions options;
    options.prettyPrint = false;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess()) << "Feature to GeoJSON conversion should succeed";
    EXPECT_NE(json.find("Feature"), std::string::npos);
    EXPECT_NE(json.find("Point"), std::string::npos);
}

TEST_F(FeatureIntegrationTest, FeatureToWktWorkflow) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(100.0, 200.0));
    
    GeometryPtr geom = feature.GetGeometry();
    ASSERT_NE(geom, nullptr);
    
    std::string wkt;
    WktOptions options;
    
    Result result = WkbWktConverter::GeometryToWkt(geom.get(), wkt, options);
    EXPECT_TRUE(result.IsSuccess()) << "Geometry to WKT conversion should succeed";
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(FeatureIntegrationTest, FeatureToWkbWorkflow) {
    CNFeature feature(defn_);
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}, {2, 2}}));
    
    GeometryPtr geom = feature.GetGeometry();
    ASSERT_NE(geom, nullptr);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbWktConverter::GeometryToWkb(geom.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess()) << "Geometry to WKB conversion should succeed";
    EXPECT_FALSE(wkb.empty());
}

TEST_F(FeatureIntegrationTest, GeoJsonToFeatureWorkflow) {
    std::string json = R"({
        "type": "Feature",
        "id": 42,
        "geometry": {"type": "Point", "coordinates": [50.0, 60.0]},
        "properties": {"id": 1, "name": "test"}
    })";
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    GeoJsonOptions options;
    Result result = GeoJsonConverter::JsonToFeature(json, &collection, options);
    EXPECT_TRUE(result.IsSuccess()) << "GeoJSON to Feature conversion should succeed";
    EXPECT_EQ(collection.GetFeatureCount(), 1);
}

TEST_F(FeatureIntegrationTest, WktToFeatureWorkflow) {
    std::string wkt = "POINT(100 200)";
    
    std::unique_ptr<Geometry> geometry;
    WktOptions options;
    
    Result result = WkbWktConverter::WktToGeometry(wkt, geometry, options);
    ASSERT_TRUE(result.IsSuccess()) << "WKT to Geometry conversion should succeed";
    
    CNFeature feature(defn_);
    feature.SetGeometry(std::move(geometry));
    
    GeometryPtr geom = feature.GetGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPoint);
}

TEST_F(FeatureIntegrationTest, WkbToFeatureWorkflow) {
    auto originalPoint = Point::Create(123.456, -78.901);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(originalPoint.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromWkbResult.IsSuccess());
    
    CNFeature feature(defn_);
    feature.SetGeometry(std::move(geometry));
    
    EXPECT_NE(feature.GetGeometry(), nullptr);
}

TEST_F(FeatureIntegrationTest, FeatureCollectionIteration) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 10; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetFieldInteger(0, i * 10);
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
        f->SetFieldInteger(0, i);
        f->SetFieldString(1, "feature_" + std::to_string(i));
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
    
    CNSpatialQuery query = CNSpatialQuery::Intersects(point);
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
    original.SetFieldInteger(0, 42);
    original.SetFieldString(1, "original");
    original.SetGeometry(Point::Create(10.0, 20.0));
    
    CNFeature* cloned = original.Clone();
    ASSERT_NE(cloned, nullptr);
    
    EXPECT_TRUE(original.Equal(*cloned));
    EXPECT_EQ(cloned->GetFID(), 100);
    EXPECT_EQ(cloned->GetFieldAsInteger(0), 42);
    EXPECT_EQ(cloned->GetFieldAsString(1), "original");
    
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
    original.SetFieldInteger(0, 123);
    original.SetFieldString(1, "roundtrip_test");
    original.SetGeometry(Point::Create(45.0, 90.0));
    
    std::string json;
    GeoJsonOptions options;
    
    Result toJsonResult = GeoJsonConverter::FeatureToJson(&original, json, options);
    ASSERT_TRUE(toJsonResult.IsSuccess());
    
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    Result fromJsonResult = GeoJsonConverter::JsonToFeature(json, &collection, options);
    ASSERT_TRUE(fromJsonResult.IsSuccess());
    
    EXPECT_EQ(collection.GetFeatureCount(), 1);
    EXPECT_EQ(collection.GetFeature(0)->GetFID(), 999);
}

TEST_F(FeatureIntegrationTest, RoundTripWkb) {
    CNFeature original(defn_);
    original.SetGeometry(Polygon::Create({{{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}}));
    
    GeometryPtr geom = original.GetGeometry();
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toWkbResult = WkbWktConverter::GeometryToWkb(geom.get(), wkb, options);
    ASSERT_TRUE(toWkbResult.IsSuccess());
    
    std::unique_ptr<Geometry> parsed;
    Result fromWkbResult = WkbWktConverter::WkbToGeometry(wkb, parsed, options);
    ASSERT_TRUE(fromWkbResult.IsSuccess());
    
    EXPECT_EQ(parsed->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(FeatureIntegrationTest, MultipleGeometryTypes) {
    std::vector<std::pair<GeomType, std::function<GeometryPtr()>>> geometries = {
        {GeomType::kPoint, []() { return Point::Create(0, 0); }},
        {GeomType::kLineString, []() { return LineString::Create({{0, 0}, {1, 1}}); }},
        {GeomType::kPolygon, []() { return Polygon::Create({{{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}}}); }}
    };
    
    for (const auto& [type, createGeom] : geometries) {
        CNFeature feature(defn_);
        feature.SetGeometry(createGeom());
        
        std::string json;
        GeoJsonOptions options;
        
        Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
        EXPECT_TRUE(result.IsSuccess()) << "Should convert geometry type: " << static_cast<int>(type);
    }
}

TEST_F(FeatureIntegrationTest, LargeFeatureCollection) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    const int featureCount = 1000;
    for (int i = 0; i < featureCount; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetFieldInteger(0, i);
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
    feature.SetFieldInteger(0, 42);
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess()) << "Should handle null geometry";
}

TEST_F(FeatureIntegrationTest, FeatureWithAllFieldTypes) {
    CNFeature feature(defn_);
    
    feature.SetFieldInteger(0, 42);
    feature.SetFieldString(1, "test");
    
    std::string json;
    GeoJsonOptions options;
    
    Result result = GeoJsonConverter::FeatureToJson(&feature, json, options);
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(FeatureIntegrationTest, ConcurrentFeatureOperations) {
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn_);
    
    for (int i = 0; i < 100; ++i) {
        CNFeature* f = new CNFeature(defn_);
        f->SetFID(i);
        f->SetGeometry(Point::Create(i * 0.01, i * 0.02));
        collection.AddFeature(f);
    }
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&collection, &successCount, this]() {
            CNFeatureIterator iterator(&collection);
            while (iterator.HasNext()) {
                CNFeature* f = iterator.Next();
                if (f != nullptr) {
                    successCount++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount, 400);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
